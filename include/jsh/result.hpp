#ifndef JSH_RESULT_HPP_INCLUDED
#define JSH_RESULT_HPP_INCLUDED

#include "jsh/lex.hpp"

#include <stdexcept>
#include <type_traits>
#include <utility>
#include <variant>
#include <vector>

namespace jsh
{
/// An error encountered during the compilation process
struct Error
{
	std::string reason;

	SourcePosition position;
};

using ErrorList = std::vector<Error>;

/// Represents either a value of type `ValidType` or a list of errors.
template <typename ValidType>
class Result
{
public:
	explicit
	Result(ValidType&& value) : value_(std::move(value))
	{
	}
	explicit
	Result(ValidType const& value) : value_(value)
	{
	}

	explicit
	Result(ErrorList&& errors) : value_(std::move(errors))
	{
	}
	explicit
	Result(ErrorList const& errors) : value_(errors)
	{
	}

	Result(Result&&) = default;
	Result(Result const&) = default;

	Result& operator=(Result&&) = default;
	Result& operator=(Result const&) = default;

	/** Converts an invalid `Result` to a `Result<NewType>`.
	 *
	 * This creates a new result with the same errors but a different valid
	 * type.
	 *
	 * If errors cannot be forwarded because this result is valid, throws
	 * `std::runtime_error`.
	 */
	template <typename NewType>
	Result<NewType> forward_errors(void) const&
	{
		ensure_can_forward_errors();
		return Result<NewType>(std::get<ErrorList>(value_));
	}

	template <typename NewType>
	Result<NewType> forward_errors(void)&&
	{
		ensure_can_forward_errors();
		return Result<NewType>(std::get<ErrorList>(std::move(value_)));
	}

	/** Gets the value from a valid result.
	 *
	 * Throws `std::runtime_error` if this result is invalid.
	 */
	ValidType const& value(void) const&
	{
		ensure_has_value();
		return std::get<ValidType>(value_);
	}
	ValidType&& value(void)&&
	{
		ensure_has_value();
		return std::get<ValidType>(std::move(value_));
	}

	/** Gets the errors from an invalid result.
	 *
	 * Throws `std::runtime_error` if this result is valid.
	 */
	ErrorList const& errors(void) const&
	{
		ensure_has_errors();
		return std::get<ErrorList>(value_);
	}
	ErrorList&& errors(void)&&
	{
		ensure_has_errors();
		return std::get<ErrorList>(std::move(value_));
	}

	[[nodiscard]]
	bool is_valid(void) noexcept
	{
		return !std::holds_alternative<ErrorList>(value_);
	}

private:
	/*
	 * Called when attampting to forward errors. Throws an exception if
	 * errors cannot be forwarded.
	 */
	void ensure_can_forward_errors(void)
	{
		if (is_valid())
		{
			throw std::runtime_error(
				"attempted to forward errors from valid result");
		}
	}

	/*
	 * Called when attempting to get a valid value. Throws an exception if
	 * it cannot be obtained.
	 */
	void ensure_has_value(void)
	{
		if (!is_valid)
		{
			throw std::runtime_error(
				"attempted to get value from invalid result");
		}
	}

	/*
	 * Called when attempting to get errors. Throws an exception if they
	 * cannot be obtained
	 */
	void ensure_has_errors(void)
	{
		if (is_valid)
		{
			throw std::runtime_error(
				"attempted to get errors from valid result");
		}
	}

	std::variant<ValidType, ErrorList> value_;
};
} // namespace jsh

#endif
