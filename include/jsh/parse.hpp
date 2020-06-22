#ifndef JSH_PARSE_HPP_INCLUDED
#define JSH_PARSE_HPP_INCLUDED

#include "jsh/result.hpp"

#include "jsh/tree.hpp"

#include <string_view>

namespace jsh
{
/** Parses a whole string as jsonish.
 *
 * The entire string must be valid. Whitespace is allowed at the end.
 *
 * @param str the string to parse
 *
 * @return an invalid result if `str` could not be parsed, or a valid
 * `jsh::Value` otherwise
 */
[[nodiscard]]
Result<Value> parse(std::string_view str);
} // namespace jsh

#endif
