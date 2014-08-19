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
    return std::toupper(ch);
}


std::string toUpper(const std::string& s)
{
    std::string ret;
    std::transform(s.begin(), s.end(), std::back_inserter(ret), (int(*)(int))std::toupper);
    return ret;
}


char toLower(char ch)
{
    return std::tolower(ch);
}


std::string toLower(const std::string& s)
{
    std::string ret;
    std::transform(s.begin(), s.end(), std::back_inserter(ret), (int(*)(int))std::tolower);
    return ret;
}


} // namespace helpers

} // namespace log4cplus
