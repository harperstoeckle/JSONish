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
	REQUIRE(v1.is_object());
	REQUIRE(v1.as_object().is_empty());

	auto r2 = jsonish::parse(R"(["one", "two", "three"])");
	REQUIRE(r2.is_valid());
	auto v2 = std::move(r2).value();
	REQUIRE(v2.is_list());
	REQUIRE(v2.as_list().size() == 3);
	{
		jsonish::List l;
		l.append("one");
		l.append("two");
		l.append("three");

		REQUIRE(v2.as_list() == l);
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
	REQUIRE(v8.is_object());
	REQUIRE(v8.as_object().size() == 2);
	{
		jsonish::Object o;
		o.set_value("a thing\r\n", jsonish::Object{});
		o.set_value("", "");
		REQUIRE(v8.as_object() == o);
	}

	auto r9 = jsonish::parse(R"( { "key" : "value", "key" : "other" } )");
	REQUIRE(!r9.is_valid());

	auto r10 = jsonish::parse(R"( { "key" : "value", "2key" : "other" } )");
	REQUIRE(r10.is_valid());
	auto v10 = r10.value();
	REQUIRE(v10.is_object());
	REQUIRE(v10.as_object().size() == 2);
	{
		jsonish::Object o;
		o.set_value("key", "value");
		o.set_value("2key", "other");
		REQUIRE(v10.as_object() == o);
	}

	auto r11 = jsonish::parse(R"([{"key" : {}}, ["one", "two", {}], "key", {"key" : []}])");
	REQUIRE(r11.is_valid());
	auto v11 = r11.value();
	REQUIRE(v11.is_list());
	REQUIRE(v11.as_list().size() == 4);
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
		REQUIRE(v11.as_list() == l);
	}
	// Make sure at and get_value also do their jobs correctly.
	{
		auto d0 = v11.at(0).get_value("key");
		REQUIRE(d0.exists());
		REQUIRE(d0.value().is_object());
		REQUIRE(d0.value().as_object() == jsonish::Object{});

		auto d1 = v11.at(2);
		REQUIRE(d1.exists());
		REQUIRE(d1.value().is_string());
		REQUIRE(d1.value().as_string() == "key");

		auto d2 = v11.at(1).at(1);
		REQUIRE(d2.exists());
		REQUIRE(d2.value().is_string());
		REQUIRE(d2.value().as_string() == "two");

		auto d3 = v11.at(3).get_value("key");
		REQUIRE(d3.exists());
		REQUIRE(d3.value().is_list());
		REQUIRE(d3.value().as_list() == jsonish::List());

		REQUIRE(!v11.at(4).exists());
		REQUIRE(!v11.get_value("dummy").exists());
		REQUIRE(!v11.at(1).at(2).get_value("thing").exists());
	}
}
