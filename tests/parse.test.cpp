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
	{
		jsh::List l;
		l.append("one");
		l.append("two");
		l.append("three");

		REQUIRE(std::get<jsh::List>(v2) == l);
	}

	auto r3 = jsh::parse("{");
	REQUIRE(!r3.is_valid());

	auto r4 = jsh::parse(R"([{}, {"key":"value"}],)");
	REQUIRE(!r4.is_valid());

	auto r5 = jsh::parse(R"("key":"value")");
	REQUIRE(!r5.is_valid());

	auto r6 = jsh::parse(R"({"key"})");
	REQUIRE(!r6.is_valid());

	auto r7 = jsh::parse(R"(["one", "two",])");
	REQUIRE(!r7.is_valid());

	auto r8 = jsh::parse(R"(   { "a thing\r\n" : {} , "" : "" }   )");
	REQUIRE(r8.is_valid());
	auto v8 = r8.value();
	REQUIRE(std::holds_alternative<jsh::Object>(v8));
	REQUIRE(std::get<jsh::Object>(v8).size() == 2);
	{
		jsh::Object o;
		o.set_value("a thing\r\n", jsh::Object{});
		o.set_value("", "");
		REQUIRE(std::get<jsh::Object>(v8) == o);
	}

	auto r9 = jsh::parse(R"( { "key" : "value", "key" : "other" } )");
	REQUIRE(!r9.is_valid());

	auto r10 = jsh::parse(R"( { "key" : "value", "2key" : "other" } )");
	REQUIRE(r10.is_valid());
	auto v10 = r10.value();
	REQUIRE(std::holds_alternative<jsh::Object>(v10));
	REQUIRE(std::get<jsh::Object>(v10).size() == 2);
	{
		jsh::Object o;
		o.set_value("key", "value");
		o.set_value("2key", "other");
		REQUIRE(std::get<jsh::Object>(v10) == o);
	}

	auto r11 = jsh::parse(R"([{"key" : {}}, ["one", "two", {}], "key", {"key" : []}])");
	REQUIRE(r11.is_valid());
	auto v11 = r11.value();
	REQUIRE(std::holds_alternative<jsh::List>(v11));
	REQUIRE(std::get<jsh::List>(v11).size() == 4);
	{
		jsh::List l;
		jsh::Object l0;
		l0.set_value("key", jsh::Object{});
		l.append(std::move(l0));
		jsh::List l1;
		l1.append("one");
		l1.append("two");
		l1.append(jsh::Object{});
		l.append(std::move(l1));
		l.append("key");
		jsh::Object l3;
		l3.set_value("key", jsh::List{});
		l.append(std::move(l3));
		REQUIRE(std::get<jsh::List>(v11) == l);
	}
}
