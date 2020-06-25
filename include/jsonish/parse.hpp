#ifndef JSH_PARSE_HPP_INCLUDED
#define JSH_PARSE_HPP_INCLUDED

#include "jsonish/result.hpp"

#include "jsonish/tree.hpp"

#include <string_view>

namespace jsonish
{
/** Parses a whole string as jsonish.
 *
 * The entire string must be valid. Whitespace is allowed at the end.
 *
 * @param str the string to parse
 *
 * @return an invalid result if `str` could not be parsed, or a valid
 * `jsonish::Value` otherwise
 */
[[nodiscard]]
Result<Value> parse(std::string_view str);
} // namespace jsonish

#endif
