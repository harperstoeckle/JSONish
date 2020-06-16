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

	jsh::Lexer l7(R"(  "a string" : "a value"  )");
	REQUIRE(l7.try_extract_token(jsh::TokenType::string));
	REQUIRE(l7.try_extract_token(jsh::TokenType::colon));
	REQUIRE(l7.try_extract_token(jsh::TokenType::string));
	REQUIRE(l7.try_extract_token(jsh::TokenType::eof));

	jsh::Lexer l8("{}[]");
	REQUIRE(l8.try_extract_token(jsh::TokenType::lbrace));
	REQUIRE(l8.try_extract_token(jsh::TokenType::rbrace));
	REQUIRE(l8.try_extract_token(jsh::TokenType::lbracket));
	REQUIRE(l8.try_extract_token(jsh::TokenType::rbracket));
	REQUIRE(l8.try_extract_token(jsh::TokenType::eof));

	jsh::Lexer l9("what");
	REQUIRE(!l9.try_extract_token(jsh::TokenType::string));
	REQUIRE(!l9.try_extract_token(jsh::TokenType::eof));
	REQUIRE(!l9.try_extract_token(jsh::TokenType::lbrace));
	REQUIRE(!l9.try_extract_token(jsh::TokenType::lbracket));
	REQUIRE(l9.try_extract_token(jsh::TokenType::invalid));

	jsh::Lexer l10("[]");
	REQUIRE(!l10.try_extract_token(jsh::TokenType::lbrace));
	REQUIRE(!l10.try_extract_token(jsh::TokenType::rbracket));
	REQUIRE(!l10.try_extract_token(jsh::TokenType::eof));
	REQUIRE(!l10.try_extract_token(jsh::TokenType::comma));
	REQUIRE(!l10.try_extract_token(jsh::TokenType::colon));
	REQUIRE(l10.try_extract_token(jsh::TokenType::lbracket));
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

TEST_CASE("Peek tokens", "[lex]")
{
	jsh::Lexer l0(R"("string", "string")");
	REQUIRE(l0.next_is(jsh::TokenType::string));
	REQUIRE(l0.next_is(jsh::TokenType::string));
	REQUIRE(l0.next_is(jsh::TokenType::string));
	l0.extract_token();
	REQUIRE(l0.next_is(jsh::TokenType::comma));
	REQUIRE(l0.next_is(jsh::TokenType::comma));
	REQUIRE(l0.next_is(jsh::TokenType::comma));
	l0.extract_token();
	REQUIRE(l0.next_is(jsh::TokenType::string));
	REQUIRE(l0.next_is(jsh::TokenType::string));
	REQUIRE(l0.next_is(jsh::TokenType::string));
	l0.extract_token();
	REQUIRE(l0.next_is(jsh::TokenType::eof));
	REQUIRE(l0.next_is(jsh::TokenType::eof));
	REQUIRE(l0.next_is(jsh::TokenType::eof));

	jsh::Lexer l1("abc");
	REQUIRE(l1.next_is(jsh::TokenType::invalid));
	REQUIRE(l1.next_is(jsh::TokenType::invalid));
	REQUIRE(l1.next_is(jsh::TokenType::invalid));
	l1.extract_token();
	REQUIRE(l1.next_is(jsh::TokenType::invalid));
	REQUIRE(l1.next_is(jsh::TokenType::invalid));
	REQUIRE(l1.next_is(jsh::TokenType::invalid));
	l1.extract_token();
	REQUIRE(l1.next_is(jsh::TokenType::invalid));
	REQUIRE(l1.next_is(jsh::TokenType::invalid));
	REQUIRE(l1.next_is(jsh::TokenType::invalid));
	l1.extract_token();
	REQUIRE(l1.next_is(jsh::TokenType::eof));
	REQUIRE(l1.next_is(jsh::TokenType::eof));
	REQUIRE(l1.next_is(jsh::TokenType::eof));

	jsh::Lexer l2(R"("string")");
	REQUIRE(!l2.next_is(jsh::TokenType::invalid));
	REQUIRE(!l2.next_is(jsh::TokenType::lbrace));
	REQUIRE(!l2.next_is(jsh::TokenType::rbrace));
	REQUIRE(!l2.next_is(jsh::TokenType::lbracket));
	REQUIRE(!l2.next_is(jsh::TokenType::rbracket));
	REQUIRE(!l2.next_is(jsh::TokenType::eof));
}
