// Module:  Log4CPLUS
// File:    stringhelper.cpp

#include <log4cplus/stringhelper.h>

#include <log4cplus/internal.h>

#include <iterator>
#include <algorithm>
#include <cstring>
#include <cctype>
#include <cassert>



using namespace log4cplus;

char log4cplus::toUpper(char ch)
{
	return (char)std::toupper(ch);
}


string log4cplus::toUpper(const string& s)
{
	string ret;
	std::transform(s.begin(), s.end(), std::back_inserter(ret), (int(*)(int))std::toupper);
	return ret;
}


char log4cplus::toLower(char ch)
{
	return (char)std::tolower(ch);
}


string log4cplus::toLower(const string& s)
{
	string ret;
	std::transform(s.begin(), s.end(), std::back_inserter(ret), (int(*)(int))std::tolower);
	return ret;
}


