#ifndef JSH_TREE_HPP_INCLUDED
#define JSH_TREE_HPP_INCLUDED

#include <functional>
#include <map>
#include <optional>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

namespace jsonish
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

	/** Attempt to get the value at the given index.
	 *
	 * If the index does not exist, return an empty optional. Otherwise,
	 * return a `std::reference_wrapper` to the specified value.
	 */
	[[nodiscard]]
	auto at(std::size_t index) const
		-> std::optional<std::reference_wrapper<Value const>>;

	[[nodiscard]] friend
	bool operator==(List const& a, List const& b);

	[[nodiscard]] friend
	bool operator!=(List const& a, List const& b);

private:
	std::vector<Value> values_;
};

/** Contains key-value pairs of strings and jsonish values.
 *
 * An `Object` does not allow duplicate keys.
 */
class Object
{
public:
	Object(void) noexcept = default;

	Object(Object const&) = default;
	Object(Object&&) noexcept = default;

	Object& operator=(Object const&) = default;
	Object& operator=(Object&&) noexcept = default;

	[[nodiscard]]
	auto begin(void) const noexcept { return std::cbegin(values_); }
	[[nodiscard]]
	auto end(void) const noexcept { return std::cend(values_); }

	[[nodiscard]]
	auto begin(void) noexcept { return std::begin(values_); }
	[[nodiscard]]
	auto end(void) noexcept { return std::end(values_); }

	[[nodiscard]]
	std::size_t size(void) const noexcept { return values_.size(); }

	[[nodiscard]]
	bool is_empty(void) const noexcept { return values_.empty(); }

	/** Attempt to insert a key-value pair into the object.
	 *
	 * If a value with the given key already exists in this object,
	 *
	 * @return `true` if the key and value were inserted and `false`
	 * otherwise.
	 */
	bool try_insert(std::string key, Value const& value);
	bool try_insert(std::string key, Value&& value);

	/** Unconditionally set a key and value.
	 *
	 * If the key already exists, its value is changed to `value`. If the
	 * key does not exist, a new key-value pair is created with `key` and
	 * `value`.
	 */
	void set_value(std::string key, Value const& value);
	void set_value(std::string key, Value&& value);

	/** Attempt to get the value associated with a key.
	 *
	 * If `key` does not exist in this object, produces an empty optional.
	 * If it does exist, produces an optional containing a
	 * `std::reference_wrapper` referencing the associated value.
	 */
	[[nodiscard]]
	auto get_value(std::string_view key) const noexcept
		-> std::optional<std::reference_wrapper<Value const>>;

	[[nodiscard]] friend
	bool operator==(Object const& a, Object const& b);

	[[nodiscard]] friend
	bool operator!=(Object const& a, Object const& b);

private:
	// Comparator to allow finding elements with `std::string_view`.
	struct StringViewComparator
	{
		using is_transparent = void;
		bool operator()(
			std::string_view a, std::string_view b) const noexcept
		{
			return std::less<>{}(a, b);
		}
	};

	/*
	 * Every compiler I have tested this with has allowed it. However, I am
	 * almost completely sure this is undefined behavior, as `Object` (and
	 * therefore `Value`) is not complete until the closing brace of this
	 * class.
	 *
	 * One possible way this could be fixed is to wrap the `Value`s in the
	 * multimap in `std::unique_ptr`.
	 */
	std::map<std::string, Value, StringViewComparator> values_;
};
} // namespace jsonish

#endif
