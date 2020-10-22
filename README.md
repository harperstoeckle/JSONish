# JSONish
JSONish is a C++ library for parsing a subset of JSON where leaf values can
only be strings.

## Motivation
There are often cases in which an application accepts structured input, perhaps
for something like configuration, but performs all parsing of that data
internally. In these cases, the process is greatly simplified when only string
data has to be considered. The goal of JSONish is to provide this to some
extent.

## Usage
Everything can be accessed by including the header `jsonish/parse.hpp`.

### Parsing a string
A string can be parsed using `jsonish::parse`. This will result in an object of
type `jsonish::Result<jsonish::Value>`. If parsing was successful, a call to
`jsonish::Result::is_valid` will return `true`. The actual value can be
obtained using `jsonish::Result::value`. If the `Result` is an lvalue, a
`const` lvalue reference to the value will be returned. If it is an rvalue, an
rvalue reference to the contained value will be returned. Therefore, it may be
preferable to either bind a reference to the value or move the `Result`.
```cpp
/*
 * Calling `value` will throw an exception if the input was not valid JSONish.
 * However, simply parsing invalid JSONish will not throw.
 */
auto foo = jsonish::parse(R"({"values" : ["one", "two", "three"]})").value();

// `is_valid` can be used to determine whether parsing was successful.
auto maybe_bar = jsonish::parse("invalid json");
if (!maybe_bar.is_valid())
{
	return;
}

// Now that it is known to be valid, the value can be safely extracted.
auto const& bar = maybe_bar.value();
// It can also be moved.
auto bar_moved = std::move(maybe_bar).value();
```

### Accessing values
A `jsonish::Value` can be a string, list, or object. To determine whether it is
one of these, the member functions `is_string`, `is_list`, and `is_object` can
be used.
```cpp
// Parse a top-level string. Since we know it's correct, just get the value.
auto foo = jsonish::parse(R"("")").value();
assert(foo.is_string());

auto bar = jsonish::parse(R"([])");
assert(bar.is_list());

auto baz = jsonish::parse(R"({})");
assert(baz.is_object());
```

Each `is_*` member function has an equivalent `as_*`, which can be used to
convert the `Value` to one of those types.
```cpp
auto foo = jsonish::parse(R"("just a string")").value();

// Prints "just a string".
std::cout << foo.as_string() << '\n';
```

Additionally, `jsonish::List::at` and `jsonish::Object::property` can be used
to access the element at an index in a list and a property with a given key in
an object, respectively. Member functions with the same names also exist in
`jsonish::Value` for convenience. The returned value acts like a
`jsonish::Value`, but also accounts for the fact that the element may not
exist.
```cpp
auto second_wallpaper =
	jsonish::parse(R"({"wallpaper" : {"images" : ["a.png", "b.png"]}})")
		.value()
		.property("wallpaper")
		.property("images")
		.at(1)
		.as_string();
assert(second_wallpaper == "b.png");
```
