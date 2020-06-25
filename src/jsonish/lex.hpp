#ifndef JSH_LEX_HPP_INCLUDED
#define JSH_LEX_HPP_INCLUDED

#include <cassert>
#include <optional>
#include <string>
#include <string_view>

namespace jsonish
{
enum struct TokenType
{
	eof, ///< No more characters can be read.
	invalid, ///< No token could be extracted.

	string, ///< A sequence of characters between quotes.

	lbrace, ///< An opening brace, '{'.
	rbrace, ///< A closing brace, '}'.

	lbracket, ///< An opening bracket, '['.
	rbracket, ///< A closing bracket, ']'.

	comma, ///< A comma, ','.
	colon ///< A colon, ':'.
};

struct SourcePosition
{
	/// The sequence of characters in a source unit.
	std::string_view chars;

	/// The index of the current character.
	std::size_t offset;
};

class Token
{
public:
	/// Create an eof token.
	[[nodiscard]] static
	Token eof(SourcePosition pos) noexcept
	{
		return Token(pos, TokenType::eof);
	}

	/// Create an invalid token with a reason.
	[[nodiscard]] static
	Token invalid(SourcePosition pos, std::string reason)
	{
		return Token(pos, TokenType::invalid, std::move(reason));
	}

	/// Create a string token.
	[[nodiscard]] static
	Token string(SourcePosition pos, std::string value)
	{
		return Token(pos, TokenType::string, std::move(value));
	}

	/// Create a left brace token.
	[[nodiscard]] static
	Token lbrace(SourcePosition pos) noexcept
	{
		return Token(pos, TokenType::lbrace);
	}

	/// Create a right brace token.
	[[nodiscard]] static
	Token rbrace(SourcePosition pos) noexcept
	{
		return Token(pos, TokenType::rbrace);
	}

	/// Create a left bracket token.
	[[nodiscard]] static
	Token lbracket(SourcePosition pos) noexcept
	{
		return Token(pos, TokenType::lbracket);
	}

	/// Create a right bracket token.
	[[nodiscard]] static
	Token rbracket(SourcePosition pos) noexcept
	{
		return Token(pos, TokenType::rbracket);
	}

	/// Create a comma token.
	[[nodiscard]] static
	Token comma(SourcePosition pos) noexcept
	{
		return Token(pos, TokenType::comma);
	}

	/// Create a colon token.
	[[nodiscard]] static
	Token colon(SourcePosition pos) noexcept
	{
		return Token(pos, TokenType::colon);
	}

	Token(Token const&) = default;
	Token(Token&&) noexcept = default;

	Token& operator=(Token const&) = default;
	Token& operator=(Token&&) = default;

	/// Get the type of this token.
	TokenType type(void) const noexcept { return type_; }

	/// Get the text stored in an invalid or string token.
	std::string_view text(void) const&
	{
		assert(value_.has_value());
		return *value_;
	}
	std::string text(void)&&
	{
		assert(value_.has_value());
		return *std::move(value_);
	}

	[[nodiscard]]
	SourcePosition position(void) const noexcept
	{
		return pos_;
	}

private:
	// Construct a token with no stored value.
	Token(SourcePosition pos, TokenType type) noexcept :
		type_(type), value_(std::nullopt), pos_(pos)
	{}

	// Construct a token with a stored value.
	Token(SourcePosition pos, TokenType type, std::string value) :
		type_(type), value_(std::move(value)), pos_(pos)
	{}

	TokenType type_;

	/** The value of a string or invalid token
	 *
	 * This will only exist for a string token, containing its text, or for
	 * an invalid token, containing the reason it is invalid.
	 */
	std::optional<std::string> value_;

	SourcePosition pos_;
};

/// Holds a sequence of characters and allows extraction of tokens.
class Lexer
{
public:
	/** Construct a lexer with a sequence of characters.
	 *
	 * @param chars the sequence of input characters
	 */
	explicit
	Lexer(std::string_view chars) noexcept :
		source_{chars, 0}
	{}

	Lexer(Lexer const&) noexcept = default;
	Lexer(Lexer&&) noexcept = default;

	Lexer& operator=(Lexer const&) = default;

	/// Remove the next token and return it.
	Token extract_token(void);

	/// Get the next token without removing it.
	[[nodiscard]]
	Token peek_token(void);

	/// Indicate whether the next token is of the given type.
	[[nodiscard]]
	bool next_is(TokenType type);

	/** Extract a token only if it is of the given type.
	 *
	 * @return true if the token was extracted and false otherwise
	 */
	bool try_extract_token(TokenType type);

private:
	// True iff no more characters can be read, including whitespace.
	[[nodiscard]]
	bool at_end(void) const noexcept;

	void extract_leading_whitespace(void) noexcept;

	[[nodiscard]]
	char peek_char(void) const noexcept;

	char extract_char(void) noexcept;

	// Ensure that the next token is cached.
	void cache_next_token(void);

	// Extract the next token from the source characters, ignoring `cache_`.
	Token extract_token_from_source(void);

	/*
	 * Extract the end of a string after the leading '"'.
	 *
	 * `token_start` is the start of the full token.
	 */
	Token extract_string(SourcePosition tok_start);

	SourcePosition source_;

	// Keeps track of a peeked token
	std::optional<Token> cache_;
};
} // namespace jsonish

#endif
