#ifndef JSH_LEX_HPP_INCLUDED
#define JSH_LEX_HPP_INCLUDED

#include <optional>
#include <string_view>

namespace jsh
{
enum struct TokenType
{
	eof, ///< No more characters can be read
	invalid, ///< No token could be extracted

	string, ///< A sequence of characters between quotes

	lbrace, ///< An opening brace, '{'
	rbrace, ///< A closing brace, '}'

	lbracket, ///< An opening bracket, '['
	rbracket, ///< A closing bracket, ']'

	comma ///< A comma, ','
};

struct Token
{
private:
	TokenType type_;

	/** The value of a string or invalid token
	 *
	 * This will only exist for a string token, containing its text, or for
	 * an invalid token, containing the reason it is invalid.
	 */
	std::optional<std::string> value_;

	/// View of the file containing this token
	std::string_view file_;
	/// The offset, in `char`s, of the start of this token
	std::size_t offset_;
};

/// Holds a sequence of characters and allows extraction of tokens.
class Lexer
{
public:
	/** Construct a lexer with a sequence of characters.
	 *
	 * @param chars the sequence of input characters
	 */
	constexpr
	Lexer(std::string_view chars) noexcept;

	constexpr
	Lexer(Lexer const&) noexcept = default;
	constexpr
	Lexer(Lexer&&) noexcept = default;

	constexpr
	Lexer& operator=(Lexer const&) = default;
};
} // namespace jsh

#endif
