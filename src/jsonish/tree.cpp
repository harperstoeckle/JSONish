#include "jsonish/tree.hpp"

namespace jsonish
{
[[nodiscard]]
auto List::at(std::size_t index) const noexcept -> MaybeValueReference
{
	if (index >= values_.size())
	{
		return MaybeValueReference::empty();
	}
	return MaybeValueReference::value(values_[index]);
}

bool Object::try_insert(std::string key, Value const& value)
{
	return values_.emplace(std::move(key), value).second;
}

bool Object::try_insert(std::string key, Value&& value)
{
	return values_.emplace(std::move(key), std::move(value)).second;
}

void Object::set_property(std::string key, Value const& value)
{
	values_.insert_or_assign(std::move(key), value);
}

void Object::set_property(std::string key, Value&& value)
{
	values_.insert_or_assign(std::move(key), std::move(value));
}

[[nodiscard]]
auto Object::property(std::string_view key) const noexcept -> MaybeValueReference
{
	auto const value_pos = values_.find(key);
	if (value_pos == std::cend(values_))
	{
		return MaybeValueReference::empty();
	}
	return MaybeValueReference::value(value_pos->second);
}

[[nodiscard]]
auto Value::property(std::string_view key) const noexcept -> MaybeValueReference
{
	if (!is_object())
	{
		return MaybeValueReference::empty();
	}
	return as_object().property(key);
}

[[nodiscard]]
auto Value::at(std::size_t index) const noexcept -> MaybeValueReference
{
	if (!is_list())
	{
		return MaybeValueReference::empty();
	}
	return as_list().at(index);
}

[[nodiscard]]
bool operator==(List const& a, List const& b)
{
	return a.values_ == b.values_;
}

[[nodiscard]]
bool operator!=(List const& a, List const& b)
{
	return !(a == b);
}

[[nodiscard]]
bool operator==(Object const& a, Object const& b)
{
	return a.values_ == b.values_;
}

[[nodiscard]]
bool operator!=(Object const& a, Object const& b)
{
	return !(a == b);
}

[[nodiscard]]
bool operator==(Value const& a, Value const& b)
{
	return a.value_ == b.value_;
}

[[nodiscard]]
bool operator!=(Value const& a, Value const& b)
{
	return !(a == b);
}
} // namespace jsonish
