#pragma once

namespace ECSEngine
{
namespace UI
{
namespace UTF8Helpers
{
//-----------------------------------------------------------------------------
// [SECTION] MISC HELPERS/UTILITIES (ImText* functions)
//-----------------------------------------------------------------------------

constexpr u16 UTF_INVALID_CODEPOINT = 0xFFDD;
constexpr u16 UTF_CODEPOINT_MAX = 0xFFFF;

// Convert UTF-8 to 32-bit character, process single character input.
// Based on stb_from_utf8() from github.com/nothings/stb/
// We handle UTF-8 decoding error by skipping forward.
i32 CodepointFromUtf8Char(u32* out_char, const c8* in_text, const c8* in_text_end);

i32 TextCountCodepointsFromUtf8(const c8* in_text, const c8* in_text_end);

// Based on stb_to_utf8() from github.com/nothings/stb/
inline i32 CodepointToUtf8(c8* buf, i32 buf_size, u32 c);

// Not optimal but we very rarely use this function.
i32 CharCountUtf8BytesFromChar(const c8* in_text, const c8* in_text_end);
} // namespace UTF8Helpers
} // namespace UI
} // namespace ECSEngine
