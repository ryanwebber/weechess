#include <string>
#include <locale>
#include <codecvt>

#include "string_utils.h"

std::string to_string(char16_t codepoint)
{
    // C++ string encoding APIs suck. This is deprecated, and probably could be better
    char16_t buf[] = { codepoint, '\0' };
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> utfconv;
    return utfconv.to_bytes( buf );
}
