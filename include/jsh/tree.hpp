#ifndef JSH_TREE_HPP_INCLUDED
#define JSH_TREE_HPP_INCLUDED

#include <map>
#include <string>
#include <variant>
#include <vector>

namespace jsh
{
struct Object;
struct List;

/// Contains either a string, an object, or a list.
using Value = std::variant<std::string, Object, List>;

/// Contains a sequence of jsonish values.
struct List
{
	std::vector<Value> values;
};

/// Contains key-value pairs of strings and jsonish values.
struct Object
{
	/*
	 * Every compiler I have tested this with has allowed it. However, I am
	 * almost completely sure this is undefined behavior, as `Object` (and
	 * therefore `Value`) is not complete until the closing brace of this
	 * class.
	 *
	 * One possible way this could be fixed is to wrap the `Value`s in the
	 * multimap in `std::unique_ptr`.
	 */
	std::multimap<std::string, Value> values;
};
} // namespace jsh

#endif
