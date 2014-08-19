// Module:  Log4CPLUS
// File:    stringhelper.cpp

#include <log4cplus/helpers/stringhelper.h>

#include <log4cplus/helpers/internal.h>

#include <iterator>
#include <algorithm>
#include <cstring>
#include <cctype>
#include <cassert>

//////////////////////////////////////////////////////////////////////////////
// Global Methods
//////////////////////////////////////////////////////////////////////////////


namespace log4cplus
{

namespace helpers
{

char toUpper(char ch)
{
    return (char)std::toupper(ch);
}


string toUpper(const string& s)
{
    string ret;
    std::transform(s.begin(), s.end(), std::back_inserter(ret), (int(*)(int))std::toupper);
    return ret;
}


char toLower(char ch)
{
    return (char)std::tolower(ch);
}


string toLower(const string& s)
{
    string ret;
    std::transform(s.begin(), s.end(), std::back_inserter(ret), (int(*)(int))std::tolower);
    return ret;
}


} // namespace helpers

} // namespace log4cplus
