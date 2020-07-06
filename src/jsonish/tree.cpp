#include "jsonish/tree.hpp"

namespace jsonish
{
[[nodiscard]] inline
auto List::at(std::size_t index) const
	-> std::optional<std::reference_wrapper<Value const>>
{
	if (index >= values_.size())
	{
		return std::nullopt;
	}
	return values_[index];
}

bool Object::try_insert(std::string key, Value const& value)
{
	return values_.emplace(std::move(key), value).second;
}

bool Object::try_insert(std::string key, Value&& value)
{
	return values_.emplace(std::move(key), std::move(value)).second;
}

void Object::set_value(std::string key, Value const& value)
{
	values_.insert_or_assign(std::move(key), value);
}

void Object::set_value(std::string key, Value&& value)
{
	values_.insert_or_assign(std::move(key), std::move(value));
}

[[nodiscard]]
auto Object::get_value(std::string_view key) const noexcept
	-> std::optional<std::reference_wrapper<Value const>>
{
	auto const value_pos = values_.find(key);
	if (value_pos == std::cend(values_))
	{
		return std::nullopt;
	}
	return value_pos->second;
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
} // namespace jsonish
