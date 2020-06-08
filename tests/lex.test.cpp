#include "jsh/lex.hpp"

#include <catch2/catch.hpp>

TEST_CASE("Extract tokens", "[lex]")
{
	jsh::Lexer l0(R"({ ["one string", "two string"] :[{}]}   )");
	REQUIRE(l0.extract_token().type() == jsh::TokenType::lbrace);
	REQUIRE(l0.extract_token().type() == jsh::TokenType::lbracket);
	REQUIRE(l0.extract_token().type() == jsh::TokenType::string);
	REQUIRE(l0.extract_token().type() == jsh::TokenType::comma);
	REQUIRE(l0.extract_token().type() == jsh::TokenType::string);
	REQUIRE(l0.extract_token().type() == jsh::TokenType::rbracket);
	REQUIRE(l0.extract_token().type() == jsh::TokenType::colon);
	REQUIRE(l0.extract_token().type() == jsh::TokenType::lbracket);
	REQUIRE(l0.extract_token().type() == jsh::TokenType::lbrace);
	REQUIRE(l0.extract_token().type() == jsh::TokenType::rbrace);
	REQUIRE(l0.extract_token().type() == jsh::TokenType::rbracket);
	REQUIRE(l0.extract_token().type() == jsh::TokenType::rbrace);
	REQUIRE(l0.extract_token().type() == jsh::TokenType::eof);

	jsh::Lexer l1("   a.  \r\n\t   **;  ");
	REQUIRE(l1.extract_token().type() == jsh::TokenType::invalid);
	REQUIRE(l1.extract_token().type() == jsh::TokenType::invalid);
	REQUIRE(l1.extract_token().type() == jsh::TokenType::invalid);
	REQUIRE(l1.extract_token().type() == jsh::TokenType::invalid);
	REQUIRE(l1.extract_token().type() == jsh::TokenType::invalid);
	REQUIRE(l1.extract_token().type() == jsh::TokenType::eof);

	jsh::Lexer l2("");
	REQUIRE(l2.extract_token().type() == jsh::TokenType::eof);
	REQUIRE(l2.extract_token().type() == jsh::TokenType::eof);
	REQUIRE(l2.extract_token().type() == jsh::TokenType::eof);

	jsh::Lexer l3("{a}");
	REQUIRE(l3.extract_token().type() == jsh::TokenType::lbrace);
	REQUIRE(l3.extract_token().type() == jsh::TokenType::invalid);
	REQUIRE(l3.extract_token().type() == jsh::TokenType::rbrace);
	REQUIRE(l3.extract_token().type() == jsh::TokenType::eof);

	jsh::Lexer l4(R"("\\\/string\b\f\n\r\t\ufa12 with \"escapes\"")");
	REQUIRE(l4.extract_token().type() == jsh::TokenType::string);
	REQUIRE(l4.extract_token().type() == jsh::TokenType::eof);

	jsh::Lexer l5("\"string with \n newline\"");
	REQUIRE(l5.extract_token().type() == jsh::TokenType::invalid);

	jsh::Lexer l6(R"("invalid utf8: \uaaax")");
	REQUIRE(l6.extract_token().type() == jsh::TokenType::invalid);
}

TEST_CASE("Correctly store strings", "[lex]")
{
	jsh::Lexer l0(R"("" "a" "abc" "\"\\\/\b\f\n\r\t")");
	REQUIRE(l0.extract_token().text() == "");
	REQUIRE(l0.extract_token().text() == "a");
	REQUIRE(l0.extract_token().text() == "abc");
	REQUIRE(l0.extract_token().text() == "\"\\/\b\f\n\r\t");

	jsh::Lexer l1(R"("\u221E\u0020\u00ac\u00ac\u6570")");
	REQUIRE(l1.extract_token().text() == "∞ ¬¬数");
}
