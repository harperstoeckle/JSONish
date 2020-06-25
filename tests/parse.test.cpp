// Just ensure this compiles
#include "jsonish/parse.hpp"
#include "jsonish/result.hpp"
#include "jsonish/tree.hpp"

#include <catch2/catch.hpp>

#include <iostream>

TEST_CASE("Parse jsonish", "[parse]")
{
	auto r0 = jsonish::parse("");
	REQUIRE(!r0.is_valid());

	auto r1 = jsonish::parse("{}");
	REQUIRE(r1.is_valid());
	auto v1 = std::move(r1).value();
	REQUIRE(std::holds_alternative<jsonish::Object>(v1));
	REQUIRE(std::get<jsonish::Object>(v1).is_empty());

	auto r2 = jsonish::parse(R"(["one", "two", "three"])");
	REQUIRE(r2.is_valid());
	auto v2 = std::move(r2).value();
	REQUIRE(std::holds_alternative<jsonish::List>(v2));
	REQUIRE(std::get<jsonish::List>(v2).size() == 3);
	{
		jsonish::List l;
		l.append("one");
		l.append("two");
		l.append("three");

		REQUIRE(std::get<jsonish::List>(v2) == l);
	}

	auto r3 = jsonish::parse("{");
	REQUIRE(!r3.is_valid());

	auto r4 = jsonish::parse(R"([{}, {"key":"value"}],)");
	REQUIRE(!r4.is_valid());

	auto r5 = jsonish::parse(R"("key":"value")");
	REQUIRE(!r5.is_valid());

	auto r6 = jsonish::parse(R"({"key"})");
	REQUIRE(!r6.is_valid());

	auto r7 = jsonish::parse(R"(["one", "two",])");
	REQUIRE(!r7.is_valid());

	auto r8 = jsonish::parse(R"(   { "a thing\r\n" : {} , "" : "" }   )");
	REQUIRE(r8.is_valid());
	auto v8 = r8.value();
	REQUIRE(std::holds_alternative<jsonish::Object>(v8));
	REQUIRE(std::get<jsonish::Object>(v8).size() == 2);
	{
		jsonish::Object o;
		o.set_value("a thing\r\n", jsonish::Object{});
		o.set_value("", "");
		REQUIRE(std::get<jsonish::Object>(v8) == o);
	}

	auto r9 = jsonish::parse(R"( { "key" : "value", "key" : "other" } )");
	REQUIRE(!r9.is_valid());

	auto r10 = jsonish::parse(R"( { "key" : "value", "2key" : "other" } )");
	REQUIRE(r10.is_valid());
	auto v10 = r10.value();
	REQUIRE(std::holds_alternative<jsonish::Object>(v10));
	REQUIRE(std::get<jsonish::Object>(v10).size() == 2);
	{
		jsonish::Object o;
		o.set_value("key", "value");
		o.set_value("2key", "other");
		REQUIRE(std::get<jsonish::Object>(v10) == o);
	}

	auto r11 = jsonish::parse(R"([{"key" : {}}, ["one", "two", {}], "key", {"key" : []}])");
	REQUIRE(r11.is_valid());
	auto v11 = r11.value();
	REQUIRE(std::holds_alternative<jsonish::List>(v11));
	REQUIRE(std::get<jsonish::List>(v11).size() == 4);
	{
		jsonish::List l;
		jsonish::Object l0;
		l0.set_value("key", jsonish::Object{});
		l.append(std::move(l0));
		jsonish::List l1;
		l1.append("one");
		l1.append("two");
		l1.append(jsonish::Object{});
		l.append(std::move(l1));
		l.append("key");
		jsonish::Object l3;
		l3.set_value("key", jsonish::List{});
		l.append(std::move(l3));
		REQUIRE(std::get<jsonish::List>(v11) == l);
	}
}
