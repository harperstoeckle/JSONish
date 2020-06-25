#include "jsonish/parse.hpp"

#include "jsonish/lex.hpp"

namespace jsonish
{
// Make an error list with the given error message along with any lexer errors.
static
ErrorList make_errors(std::string reason, Token error_token)
{
	auto pos = error_token.position();

	ErrorList errors;
	errors.push_back(Error{std::move(reason), error_token.position()});

	if (error_token.type() == TokenType::invalid)
	{
		errors.push_back(
			Error{
				std::move(error_token).text(),
				error_token.position()
			});
	}

	return errors;
}

static
Result<Value> parse_value(Lexer& lex);

static
Result<List> parse_list(Lexer& lex)
{
	if (!lex.next_is(TokenType::lbracket))
	{
		return Result<List>(
			make_errors("expected '['", lex.extract_token()));
	}
	lex.extract_token();

	if (lex.next_is(TokenType::rbracket))
	{
		lex.extract_token();
		return Result<List>(List{});
	}

	List values;

	auto first_element = parse_value(lex);
	if (!first_element.is_valid())
	{
		return first_element.forward_errors<List>();
	}
	values.append(std::move(first_element).value());

	while (lex.try_extract_token(TokenType::comma))
	{
		auto cur_element = parse_value(lex);
		if (!cur_element.is_valid())
		{
			return cur_element.forward_errors<List>();
		}
		values.append(std::move(cur_element).value());
	}

	if (!lex.next_is(TokenType::rbracket))
	{
		return Result<List>(
			make_errors("expected ']'", lex.extract_token()));
	}
	lex.extract_token();

	return Result<List>(std::move(values));
}

// Parse a string key and a value separated by a ':'.
static
auto parse_object_entry(Lexer& lex)
	-> Result<std::pair<std::string, Value>>
{
	using ResultType = Result<std::pair<std::string, Value>>;

	auto key_token = lex.extract_token();
	if (key_token.type() != TokenType::string)
	{
		return ResultType(
			make_errors("expected string as key", key_token));
	}

	if (!lex.next_is(TokenType::colon))
	{
		return ResultType(
			make_errors("expected ':'", lex.extract_token()));
	}
	lex.extract_token();

	auto value = parse_value(lex);
	if (!value.is_valid())
	{
		return value.forward_errors<std::pair<std::string, Value>>();
	}

	return ResultType({std::move(key_token).text(), value.value()});
}

static
Result<Object> parse_object(Lexer& lex)
{
	if (!lex.next_is(TokenType::lbrace))
	{
		return Result<Object>(
			make_errors("expected '{'", lex.extract_token()));
	}
	lex.extract_token();

	if (lex.next_is(TokenType::rbrace))
	{
		lex.extract_token();
		return Result<Object>(Object{});
	}

	Object values;

	auto first_entry = parse_object_entry(lex);
	if (!first_entry.is_valid())
	{
		return first_entry.forward_errors<Object>();
	}

	auto [first_key, first_value] = std::move(first_entry).value();
	values.set_value(std::move(first_key), std::move(first_value));

	while (lex.try_extract_token(TokenType::comma))
	{
		auto cur_entry_first_token = lex.peek_token();

		auto cur_entry = parse_object_entry(lex);
		if (!cur_entry.is_valid())
		{
			return cur_entry.forward_errors<Object>();
		}

		auto [cur_key, cur_value] = std::move(cur_entry).value();
		// It is an error if a key is repeated.
		if (!values.try_insert(std::move(cur_key), std::move(cur_value)))
		{
			return Result<Object>(
				make_errors(
					"key already defined",
					cur_entry_first_token));
		}
	}

	if (!lex.next_is(TokenType::rbrace))
	{
		return Result<Object>(
			make_errors("expected '}'", lex.extract_token()));
	}
	lex.extract_token();

	return Result<Object>(std::move(values));
}

// Parse a string, object, or list.
static
Result<Value> parse_value(Lexer& lex)
{
	if (lex.next_is(TokenType::string))
	{
		return Result<Value>(lex.extract_token().text());
	}
	if (lex.next_is(TokenType::lbrace))
	{
		auto object = parse_object(lex);
		if (!object.is_valid())
		{
			return object.forward_errors<Value>();
		}
		return Result<Value>(std::move(object).value());
	}
	if (lex.next_is(TokenType::lbracket))
	{
		auto list = parse_list(lex);
		if (!list.is_valid())
		{
			return list.forward_errors<Value>();
		}
		return Result<Value>(std::move(list).value());
	}
	return Result<Value>(
		make_errors("expected string, '{', or '['", lex.extract_token()));
}

[[nodiscard]]
Result<Value> parse(std::string_view str)
{
	Lexer lex(str);

	auto value = parse_value(lex);
	if (!value.is_valid())
	{
		return std::move(value).forward_errors<Value>();
	}

	auto next_token = lex.extract_token();
	if (next_token.type() != TokenType::eof)
	{
		return Result<Value>(
			make_errors(
				"garbage at end of input", std::move(next_token)));
	}

	return value;
}
} // namespace jsonish
