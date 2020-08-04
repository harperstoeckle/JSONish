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
class Value;
class MaybeValueReference;

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
	auto at(std::size_t index) const noexcept -> MaybeValueReference;

	friend
	bool operator==(List const& a, List const& b);

	friend
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
	auto get_value(std::string_view key) const noexcept -> MaybeValueReference;

	friend
	bool operator==(Object const& a, Object const& b);

	friend
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

/// Holds either an `Object`, `std::string`, or `List`.
class Value
{
public:
	Value(std::string str) : value_(std::move(str)) {}
	Value(List list) : value_(std::move(list)) {}
	Value(Object object) : value_(std::move(object)) {}

	/** Construct a string value.
	 *
	 * This is required to allow values to be constructed using string
	 * literals.
	 */
	Value(char const* str) : value_(str) {}

	/// Indicate whether this value is a string.
	[[nodiscard]]
	bool is_string(void) const noexcept
	{
		return std::holds_alternative<std::string>(value_);
	}

	/// Indicate whether this value is an object.
	[[nodiscard]]
	bool is_object(void) const noexcept
	{
		return std::holds_alternative<Object>(value_);
	}

	/// Indicate whether this value is a list.
	[[nodiscard]]
	bool is_list(void) const noexcept
	{
		return std::holds_alternative<List>(value_);
	}

	/** Get a reference to the contained string value.
	 *
	 * If the contained value is not a string, throw
	 * `std::bad_variant_access`.
	 */
	[[nodiscard]]
	auto get_string(void) const -> std::string const&
	{
		return std::get<std::string>(value_);
	}

	/** Get a reference to the contained object value.
	 *
	 * If the contained value is not an object, throw
	 * `std::bad_variant_access`
	 */
	[[nodiscard]]
	auto get_object(void) const -> Object const&
	{
		return std::get<Object>(value_);
	}

	/** Get a reference to the contained list value.
	 *
	 * If the contained value is not a list, throw
	 * `std::bad_variant_access`
	 */
	[[nodiscard]]
	auto get_list(void) const -> List const&
	{
		return std::get<List>(value_);
	}

	/** Attempt to get a value with the specified key in an object.
	 *
	 * If this value is not an object, or if the key does not exist,
	 * produce an empty `MaybeValueReference`. Otherwise, produce a
	 * `MaybeValueReference` containing a reference to that value.
	 */
	[[nodiscard]]
	auto get_value(std::string_view key) const noexcept -> MaybeValueReference;

	/** Attempt to get a value with the specified index in a list.
	 *
	 * If the value is not a list, or if the index does not exist,
	 * produce an empty `MaybeValueReference`. Otherwise, produce a
	 * `MaybeValueReference` containing a reference to that value.
	 */
	[[nodiscard]]
	auto at(std::size_t index) const noexcept -> MaybeValueReference;

	friend
	bool operator==(Value const& a, Value const& b);

	friend
	bool operator!=(Value const& a, Value const& b);

private:
	std::variant<std::string, Object, List> value_;
};

/// Wraps an optional const reference to a `Value`.
class MaybeValueReference
{
public:
	/// Create an empty `MaybeValueReference`.
	[[nodiscard]] static
	auto empty(void) noexcept -> MaybeValueReference
	{
		return MaybeValueReference();
	}

	/// Create a `MaybeValueReference` referencing the given value.
	[[nodiscard]] static
	auto value(Value const& value) noexcept -> MaybeValueReference
	{
		return MaybeValueReference(value);
	}

	/// Indicate whether this value exists.
	[[nodiscard]]
	bool exists(void) const noexcept
	{
		return maybe_value_.has_value();
	}

	/** Get a reference to the contained value.
	 *
	 * If the value does not exist, throw `std::bad_optional_access`.
	 */
	[[nodiscard]]
	auto value(void) const noexcept -> Value const&
	{
		return maybe_value_.value();
	}

	/** Attempt to get the value associated with a key in an object.
	 *
	 * If no reference is contained, return an empty `MaybeValueReference`.
	 * Otherwise, produce the result of calling `get_value` on the
	 * contained reference.
	 */
	[[nodiscard]]
	auto get_value(std::string_view key) const noexcept -> MaybeValueReference
	{
		if (!exists())
		{
			return MaybeValueReference::empty();
		}
		return maybe_value_->get().get_value(key);
	}

	/** Attempt to get the value associated with an index in a list.
	 *
	 * If no reference is contained, return an empty `MaybeValueReference`.
	 * Otherwise, produce the result of calling `at` on the contained
	 * reference.
	 */
	[[nodiscard]]
	auto at(std::size_t index) const noexcept -> MaybeValueReference
	{
		if (!exists())
		{
			return MaybeValueReference::empty();
		}
		return maybe_value_->get().at(index);
	}

private:
	// Construct empty MaybeValueReference
	explicit
	MaybeValueReference(void) : maybe_value_(std::nullopt) {}

	// Construct MaybeValueReference with a reference.
	explicit
	MaybeValueReference(Value const& value) : maybe_value_(value) {}

	std::optional<std::reference_wrapper<Value const>> maybe_value_;
};
} // namespace jsonish

#endif
