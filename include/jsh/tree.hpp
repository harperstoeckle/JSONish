#ifndef JSH_TREE_HPP_INCLUDED
#define JSH_TREE_HPP_INCLUDED

#include <map>
#include <string>
#include <variant>
#include <vector>

namespace jsh
{
class Object;
class List;

/// Contains either a string, an object, or a list.
using Value = std::variant<std::string, Object, List>;

/// Contains a sequence of jsonish values.
class List
{
public:
	List(void) noexcept = default;

	List(List const&) = default;
	List(List&&) noexcept = default;

	List& operator=(List const&) = default;
	List& operator=(List&&) noexcept = default;

	[[nodiscard]]
	auto begin(void) const noexcept { return std::cbegin(values_); }
	[[nodiscard]]
	auto end(void) const noexcept { return std::cend(values_); }

	[[nodiscard]]
	auto begin(void) noexcept { return std::begin(values_); }
	[[nodiscard]]
	auto end(void) noexcept { return std::end(values_); }

	/// Append a value to the end of the list.
	void append(Value const& value) { values_.push_back(value); }
	void append(Value&& value) { values_.push_back(std::move(value)); }

	[[nodiscard]]
	std::size_t size(void) const noexcept { return values_.size(); }

	[[nodiscard]]
	bool is_empty(void) const noexcept { return values_.empty(); }

private:
	std::vector<Value> values_;
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
	std::multimap<std::string, Value> values_;
};
} // namespace jsh

#endif
