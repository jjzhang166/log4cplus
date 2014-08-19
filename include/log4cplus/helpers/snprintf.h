// -*- C++ -*-


#ifndef LOG4CPLUS_HELPERS_SNPRINTF_H
#define LOG4CPLUS_HELPERS_SNPRINTF_H

#include <log4cplus/config.h>

#include <cstdarg>
#include <vector>


namespace log4cplus { namespace helpers {


class LOG4CPLUS_EXPORT snprintf_buf
{
public:
    snprintf_buf();

    char const * print(char const * fmt, ...);


    int print_va_list(char const * & str, char const * fmt, std::va_list args);


private:
    vector<char> buf;
};
  

} } // namespace log4cplus { namespace helpers



#endif // LOG4CPLUS_HELPERS_SNPRINTF_H
