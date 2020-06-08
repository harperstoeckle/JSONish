#include "jsh/lex.hpp"

#include <algorithm>
#include <cassert>
#include <charconv>
#include <locale>

namespace jsh
{
[[nodiscard]] static constexpr
bool is_space(char c) noexcept
{
	switch (c)
	{
	case ' ': case '\t': case '\n': case '\r':
		return true;
	default:
		return false;
	}
}

[[nodiscard]] static
bool is_hex_digit(char c) noexcept
{
	return std::isxdigit(c, std::locale::classic());
}

// This character can follow a backslash in a string.
[[nodiscard]] static constexpr
bool is_escapable(char c) noexcept
{
	switch (c)
	{
	case '"': case '\\': case '/':
	case 'b': case 'f': case 'n': case 'r': case 't': case 'u':
		return true;
	default:
		return false;
	}
}

// This character is not allowed at all in a string.
[[nodiscard]] static constexpr
bool is_disallowed_in_string(char c) noexcept
{
	return c < 0x20;
}

Token Lexer::extract_token(void) noexcept
{
	extract_leading_whitespace();
	auto tok_start = source_;

	if (at_end())
	{
		return Token::eof(tok_start);
	}

	switch (extract_char())
	{
	case '"': return extract_string(tok_start);
	case '{': return Token::lbrace(tok_start);
	case '}': return Token::rbrace(tok_start);
	case '[': return Token::lbracket(tok_start);
	case ']': return Token::rbracket(tok_start);
	case ',': return Token::comma(tok_start);
	case ':': return Token::colon(tok_start);
	default: return Token::invalid(tok_start, "unexpected character");
	}
}

bool Lexer::at_end(void) const noexcept
{
	return source_.chars.size() <= source_.offset;
}

void Lexer::extract_leading_whitespace(void) noexcept
{
	auto first = std::cbegin(source_.chars) + source_.offset;
	auto last = std::cend(source_.chars);
	auto after_whitespace = std::find_if_not(first, last, is_space);

	source_.offset += static_cast<std::size_t>(
		std::distance(first, after_whitespace));
}

[[nodiscard]]
char Lexer::peek_char(void) const noexcept
{
	assert(!at_end());
	return source_.chars[source_.offset];
}

char Lexer::extract_char(void) noexcept
{
	char c = peek_char();
	++source_.offset;
	return c;
}

/*
 * Map an escaped character to the character it represents. For example, 'n' is
 * mapped to a newline character and a backslash is mapped to itself. If the
 * character is not escapable by itself, it is mapped to a null character. It
 * is expected that this never be the case.
 */
[[nodiscard]] static constexpr
char map_escaped_char(char c) noexcept
{
	assert(is_escapable(c) && c != 'u');

	switch (c)
	{
	case '"': return '"';
	case '\\': return '\\';
	case '/': return '/';
	case 'b': return '\b';
	case 'f': return '\f';
	case 'n': return '\n';
	case 'r': return '\r';
	case 't': return '\t';
	default: return '\0';
	}
}

/*
 * Parse the hex digits of a four-digit hex unicode code point into a number.
 * No validation is done, so a valid value must be provided. If parsing fails,
 * 0 will just be returned.
 *
 * The input must be exactly four digits. This guarantees that it fits in 16
 * bits.
 */
[[nodiscard]] static
std::uint16_t parse_code_point(std::string_view str)
{
	assert(str.size() == 4);

	std::uint16_t result;
	auto [last, err] = std::from_chars(str.data(), str.data() + 4, result, 16);

	if (err != std::errc{} || last != str.data() + 4)
	{
		return 0;
	}

	return result;
}

/*
 * Get, from `value`, the range of bits [first, first + n). This starts from
 * the least significant bit, which is considered to have index 0.
 *
 * The result has the bottom `n` digits set to the range and the rest set to
 * zero. `n` absolutely must be less than or equal to 8.
 */
[[nodiscard]] static constexpr
char get_bit_range(
	std::uint16_t value, std::size_t first, std::size_t n) noexcept
{
	assert(n <= 8 && first < sizeof(value) * 8);
	return (value >> first) & ((1 << n) - 1);
}

/*
 * Push the four-character unicode code point `code_point` into `str` as utf-8.
 * Invalid code points are just treated as if they are valid.
 */
static
void push_unicode_as_utf8(std::string& str, std::uint16_t code_point)
{
	if (code_point < 0x0080)
	{
		str.push_back(static_cast<char>(code_point));
		return;
	}

	// Base of trailing byte for any multibyte character.
	constexpr unsigned char trailing_byte = 0b10000000;
	if (code_point < 0x0800)
	{
		str.push_back(0b11000000 | get_bit_range(code_point, 6, 5));
		str.push_back(trailing_byte | get_bit_range(code_point, 0, 6));
		return;
	}
	str.push_back(0b11100000 | get_bit_range(code_point, 12, 4));
	str.push_back(trailing_byte | get_bit_range(code_point, 6, 6));
	str.push_back(trailing_byte | get_bit_range(code_point, 0, 6));
}

Token Lexer::extract_string(SourcePosition tok_start)
{
	assert(source_.offset > 0 && source_.chars[source_.offset - 1] == '"');

	// TODO Can this be reserved upfront somewhere instead?
	std::string text;

	bool in_escape = false;
	// Number of digits yet to be processed in a unicode code point.
	int remaining_code_point_digits = 0;
	while (true)
	{
		if (at_end())
		{
			return Token::invalid(tok_start, "no closing quote");
		}

		char c = extract_char();
		if (remaining_code_point_digits > 0)
		{
			assert(remaining_code_point_digits <= 4);

			if (!is_hex_digit(c))
			{
				return Token::invalid(
					tok_start, "expected a hex digit");
			}
			if (remaining_code_point_digits == 1)
			{
				auto code_point_name =
					source_.chars.substr(source_.offset - 4, 4);
				std::uint16_t code_point_hex =
					parse_code_point(code_point_name);
				push_unicode_as_utf8(text, code_point_hex);
			}
			--remaining_code_point_digits;
		}
		else if (in_escape)
		{
			if (!is_escapable(c))
			{
				return Token::invalid(
					tok_start, "invalid escape sequence");
			}
			if (c == 'u')
			{
				remaining_code_point_digits = 4;
				in_escape = false;
				continue;
			}
			text.push_back(map_escaped_char(c));
			in_escape = false;
		}
		else if (c == '"')
		{
			break;
		}
		else if (c == '\\')
		{
			in_escape = true;
		}
		else if (is_disallowed_in_string(c))
		{
			return Token::invalid(
				tok_start, "invalid character in string");
		}
		else
		{
			text.push_back(c);
		}
	}

	return Token::string(tok_start, text);
}
} // namespace jsh
