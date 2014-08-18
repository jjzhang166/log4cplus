

#include <log4cplus/helpers/snprintf.h>
#include <log4cplus/helpers/loglog.h>
#include <log4cplus/internal/internal.h>
#include <cstdarg>
#include <cstdio>
#include <stdarg.h>
#include <wchar.h>
#include <stdio.h>


namespace log4cplus { namespace helpers {


std::size_t const START_BUF_SIZE = 512;

#if defined(_MSC_VER)
char const NULL_FILE[] = "NUL";
#else
char const NULL_FILE[] = "/dev/null";
#endif


namespace
{

static inline int vftprintf(std::FILE * file, char const * fmt, std::va_list args)
{

#if defined(_MSC_VER)
    return vfprintf_s(file, fmt, args);
#else
    return std::vfprintf(file, fmt, args);
#endif
}

static inline int vstprintf(char * dest, std::size_t dest_size, char const * fmt, std::va_list args)
{
    int ret;

#if defined(_MSC_VER)
    ret = vsprintf_s(dest, dest_size, fmt, args);
#else
    ret = std::vsprintf(dest, fmt, args);
#endif

    if(ret >= 0)
        assert(static_cast<std::size_t>(ret) <= dest_size);

    return ret;
}


}


snprintf_buf::snprintf_buf() : buf(START_BUF_SIZE)
{ }


char const * snprintf_buf::print(char const * fmt, ...)
{
    assert(fmt);

    char const * str = 0;
    int ret = 0;
    std::va_list args;

    do
    {
        va_start(args, fmt);
        ret = print_va_list(str, fmt, args);
        va_end(args);
    }
    while(ret == -1);

    return str;
}


int snprintf_buf::print_va_list(char const * & str, char const * fmt, std::va_list args)
{
    int printed;
    std::size_t const fmt_len = std::char_traits<char>::length(fmt);
    std::size_t buf_size = buf.size();
    std::size_t const output_estimate = fmt_len + fmt_len / 2 + 1;
    if(output_estimate > buf_size)
        buf.resize(buf_size = output_estimate);

    std::FILE * & fnull = internal::get_ptd()->_fnull;
    if(!fnull)
    {
        fnull = std::fopen(NULL_FILE, "wb");
        if(!fnull)
            LogLog::getLogLog()->error("Could not open NULL_FILE.", true);
    }

    printed = vftprintf(fnull, fmt, args);
    if(printed == -1)
        LogLog::getLogLog()->error("Error printing into NULL_FILE.", true);

    buf.resize(printed + 1);
    int sprinted = vstprintf(&buf[0], buf.size(), fmt, args);
    if(sprinted == -1 || sprinted >=(int)buf.size())
        LogLog::getLogLog()->error("Error printing into string.", true);

    assert(printed == sprinted);

    buf[sprinted] = 0;

    str = &buf[0];
    return printed;
}


} } // namespace log4cplus { namespace helpers
