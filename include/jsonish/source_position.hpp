#ifndef JSONISH_SOURCE_POSITION_HPP_INCLUDED
#define JSONISH_SOURCE_POSITION_HPP_INCLUDED

#include <string_view>

namespace jsonish
{
struct SourcePosition
{
	/// The sequence of characters in a source unit.
	std::string_view chars;

	/// The index of the current character.
	std::size_t offset;
};
}

#endif
