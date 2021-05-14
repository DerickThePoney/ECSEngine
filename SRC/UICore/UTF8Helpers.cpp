#include "stdafx.h"

#include "UTF8Helpers.h"

namespace ECSEngine
{
namespace UI
{
namespace UTF8Helpers
{
//-----------------------------------------------------------------------------
// [SECTION] MISC HELPERS/UTILITIES (ImText* functions)
//-----------------------------------------------------------------------------

// Convert UTF-8 to 32-bit character, process single character input.
// Based on stb_from_utf8() from github.com/nothings/stb/
// We handle UTF-8 decoding error by skipping forward.
i32 CodepointFromUtf8Char(u32* out_char, const c8* in_text, const c8* in_text_end)
{
    u32 c = (u32)-1;
    const uc8* str = (const uc8*)in_text;
    if (!(*str & 0x80))
    {
        c = (u32)(*str++);
        *out_char = c;
        return 1;
    }
    if ((*str & 0xe0) == 0xc0)
    {
        *out_char = UTF_INVALID_CODEPOINT; // will be invalid but not end of string
        if (in_text_end && in_text_end - (const c8*)str < 2)
            return 1;
        if (*str < 0xc2)
            return 2;
        c = (u32)((*str++ & 0x1f) << 6);
        if ((*str & 0xc0) != 0x80)
            return 2;
        c += (*str++ & 0x3f);
        *out_char = c;
        return 2;
    }
    if ((*str & 0xf0) == 0xe0)
    {
        *out_char = UTF_INVALID_CODEPOINT; // will be invalid but not end of string
        if (in_text_end && in_text_end - (const c8*)str < 3)
            return 1;
        if (*str == 0xe0 && (str[1] < 0xa0 || str[1] > 0xbf))
            return 3;
        if (*str == 0xed && str[1] > 0x9f)
            return 3; // str[1] < 0x80 is checked below
        c = (u32)((*str++ & 0x0f) << 12);
        if ((*str & 0xc0) != 0x80)
            return 3;
        c += (u32)((*str++ & 0x3f) << 6);
        if ((*str & 0xc0) != 0x80)
            return 3;
        c += (*str++ & 0x3f);
        *out_char = c;
        return 3;
    }
    if ((*str & 0xf8) == 0xf0)
    {
        *out_char = UTF_INVALID_CODEPOINT; // will be invalid but not end of string
        if (in_text_end && in_text_end - (const c8*)str < 4)
            return 1;
        if (*str > 0xf4)
            return 4;
        if (*str == 0xf0 && (str[1] < 0x90 || str[1] > 0xbf))
            return 4;
        if (*str == 0xf4 && str[1] > 0x8f)
            return 4; // str[1] < 0x80 is checked below
        c = (u32)((*str++ & 0x07) << 18);
        if ((*str & 0xc0) != 0x80)
            return 4;
        c += (u32)((*str++ & 0x3f) << 12);
        if ((*str & 0xc0) != 0x80)
            return 4;
        c += (u32)((*str++ & 0x3f) << 6);
        if ((*str & 0xc0) != 0x80)
            return 4;
        c += (*str++ & 0x3f);
        // utf-8 encodings of values used in surrogate pairs are invalid
        if ((c & 0xFFFFF800) == 0xD800)
            return 4;
        // If codepoint does not fit in ImWchar, use replacement character U+FFFD instead
        if (c > UTF_CODEPOINT_MAX)
            c = UTF_INVALID_CODEPOINT;
        *out_char = c;
        return 4;
    }
    *out_char = 0;
    return 0;
}

int TextCountCodepointsFromUtf8(const c8* in_text, const c8* in_text_end)
{
    i32 char_count = 0;
    while ((!in_text_end || in_text < in_text_end) && *in_text)
    {
        u32 c;
        in_text += CodepointFromUtf8Char(&c, in_text, in_text_end);
        if (c == 0)
            break;
        char_count++;
    }
    return char_count;
}

// Based on stb_to_utf8() from github.com/nothings/stb/
inline i32 CodepointToUtf8(c8* buf, i32 buf_size, u32 c)
{
    if (c < 0x80)
    {
        buf[0] = (c8)c;
        return 1;
    }
    if (c < 0x800)
    {
        if (buf_size < 2)
            return 0;
        buf[0] = (c8)(0xc0 + (c >> 6));
        buf[1] = (c8)(0x80 + (c & 0x3f));
        return 2;
    }
    if (c < 0x10000)
    {
        if (buf_size < 3)
            return 0;
        buf[0] = (c8)(0xe0 + (c >> 12));
        buf[1] = (c8)(0x80 + ((c >> 6) & 0x3f));
        buf[2] = (c8)(0x80 + ((c)&0x3f));
        return 3;
    }
    if (c <= 0x10FFFF)
    {
        if (buf_size < 4)
            return 0;
        buf[0] = (c8)(0xf0 + (c >> 18));
        buf[1] = (c8)(0x80 + ((c >> 12) & 0x3f));
        buf[2] = (c8)(0x80 + ((c >> 6) & 0x3f));
        buf[3] = (c8)(0x80 + ((c)&0x3f));
        return 4;
    }
    // Invalid code point, the max unicode is 0x10FFFF
    return 0;
}

// Not optimal but we very rarely use this function.
i32 CharCountUtf8BytesFromChar(const c8* in_text, const c8* in_text_end)
{
    u32 unused = 0;
    return CodepointFromUtf8Char(&unused, in_text, in_text_end);
}
} // namespace UTF8Helpers
} // namespace UI
} // namespace ECSEngine
