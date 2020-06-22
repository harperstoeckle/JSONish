// Just ensure this compiles
#include "jsh/parse.hpp"
#include "jsh/result.hpp"
#include "jsh/tree.hpp"

#include <catch2/catch.hpp>

#include <iostream>

TEST_CASE("Parse jsonish", "[parse]")
{
	auto r0 = jsh::parse("");
	REQUIRE(!r0.is_valid());

	auto r1 = jsh::parse("{}");
	REQUIRE(r1.is_valid());
	auto v1 = std::move(r1).value();
	REQUIRE(std::holds_alternative<jsh::Object>(v1));
	REQUIRE(std::get<jsh::Object>(v1).is_empty());

	auto r2 = jsh::parse(R"(["one", "two", "three"])");
	REQUIRE(r2.is_valid());
	auto v2 = std::move(r2).value();
	REQUIRE(std::holds_alternative<jsh::List>(v2));
	REQUIRE(std::get<jsh::List>(v2).size() == 3);
}
