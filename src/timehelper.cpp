// Module:  Log4CPLUS
// File:    timehelper.cpp


#include <log4cplus/helpers/timehelper.h>
#include <log4cplus/helpers/loglog.h>

#include <log4cplus/helpers/stringhelper.h>
#include <log4cplus/helpers/internal.h>

#include <algorithm>
#include <vector>
#include <iomanip>
#include <cassert>
#include <cerrno>

#include <sys/types.h>

#if defined(__linux__)
#include <sys/time.h>
#endif

#include <log4cplus/windowsh-inc.h>


namespace log4cplus { namespace helpers {

const int ONE_SEC_IN_USEC = 1000000;

using std::mktime;
using std::gmtime;
using std::localtime;

using std::strftime;



//////////////////////////////////////////////////////////////////////////////
// Time ctors
//////////////////////////////////////////////////////////////////////////////

TimeHelper::TimeHelper() : tv_sec(0), tv_usec(0)
{
}


TimeHelper::TimeHelper(time_t tv_sec_, long tv_usec_) : tv_sec(tv_sec_), tv_usec(tv_usec_)
{
    assert(tv_usec < ONE_SEC_IN_USEC);
}


TimeHelper::TimeHelper(time_t time) : tv_sec(time), tv_usec(0)
{
}


TimeHelper TimeHelper::gettimeofday()
{

#if defined(__linux__)
    struct timeval tp;
    ::gettimeofday(&tp, 0);

    return TimeHelper(tp.tv_sec, tp.tv_usec);

#elif defined(_MSC_VER)
    FILETIME ft;

    GetSystemTimeAsFileTime(&ft);


    typedef unsigned __int64 uint64_type;
    uint64_type st100ns
        = uint64_type(ft.dwHighDateTime) << 32 | ft.dwLowDateTime;

    // Number of 100-ns intervals between UNIX epoch and Windows system time
    // is 116444736000000000.
    uint64_type const offset = uint64_type(116444736) * 1000 * 1000 * 1000;
    uint64_type fixed_time = st100ns - offset;

    return TimeHelper(fixed_time /(10 * 1000 * 1000), fixed_time %(10 * 1000 * 1000) / 10);

#elif defined(__linux__)
    struct timeb tp;
    ftime(&tp);

    return TimeHelper(tp.time, tp.millitm * 1000);

#else
#warning "Time::gettimeofday()- low resolution timer: gettimeofday and ftime unavailable"
    return TimeHelper(::time(0), 0);
#endif
}


//////////////////////////////////////////////////////////////////////////////
// Time methods
//////////////////////////////////////////////////////////////////////////////

time_t TimeHelper::setTime(tm* t)
{
    time_t time = helpers::mktime(t);
    if(time != -1)
        tv_sec = time;

    return time;
}


time_t TimeHelper::getTime() const
{
    return tv_sec;
}


void TimeHelper::gmtime(tm* t) const
{
    time_t clock = tv_sec;
#if defined(_MSC_VER)
    gmtime_s(t, &clock);

#else 
    gmtime_r(&clock, t);

#endif
}


void TimeHelper::localtime(tm* t) const
{
    time_t clock = tv_sec;
#if defined(__linux__) 
    ::localtime_r(&clock, t);
#else
    tm* tmp = helpers::localtime(&clock);
    *t = *tmp;
#endif
}


namespace 
{


static std::string const padding_zeros[4] =
{
    std::string("000"),
    std::string("00"),
    std::string("0"),
    std::string("")
};


static std::string const uc_q_padding_zeros[4] =
{
    std::string(".000"),
    std::string(".00"),
    std::string(".0"),
    std::string(".")
};


static void build_q_value(std::string& q_str, long tv_usec)
{
    convertIntegerToString(q_str, tv_usec / 1000);
    std::size_t const len = q_str.length();
    if(len <= 2)
        q_str.insert(0, padding_zeros[q_str.length()]);
}


static void build_uc_q_value(std::string& uc_q_str, long tv_usec, std::string& tmp)
{
    build_q_value(uc_q_str, tv_usec);

    convertIntegerToString(tmp, tv_usec % 1000);
    std::size_t const usecs_len = tmp.length();
    tmp.insert(0, usecs_len <= 3 ? uc_q_padding_zeros[usecs_len] : uc_q_padding_zeros[3]);
    uc_q_str.append(tmp);
}


} // namespace


std::string TimeHelper::getFormattedTime(const std::string& fmt_orig, bool use_gmtime) const
{
    if(fmt_orig.empty() || fmt_orig[0] == 0)
        return std::string();

    tm time;
    
    if(use_gmtime)
        gmtime(&time);
    else 
        localtime(&time);
    
    enum State
    {
        TEXT,
        PERCENT_SIGN
    };
    
    internal::gft_scratch_pad & gft_sp = internal::get_gft_scratch_pad();
    gft_sp.reset();

    std::size_t const fmt_orig_size = gft_sp._fmt.size();
    gft_sp._ret.reserve(fmt_orig_size + fmt_orig_size / 3);
    State state = TEXT;

    // Walk the format string and process all occurences of %q, %Q and %s.
    
    for(std::string::const_iterator fmt_it = fmt_orig.begin(); fmt_it != fmt_orig.end(); ++fmt_it)
    {
        switch(state)
        {
        case TEXT:
        {
            if(*fmt_it == '%')
                state = PERCENT_SIGN;
            else
                gft_sp._ret.push_back(*fmt_it);
        }
        break;
            
        case PERCENT_SIGN:
        {
            switch(*fmt_it)
            {
            case 'q':
            {
                if(!gft_sp._q_str_valid)
                {
                    build_q_value(gft_sp._q_str, tv_usec);
                    gft_sp._q_str_valid = true;
                }
                gft_sp._ret.append(gft_sp._q_str);
                state = TEXT;
            }
            break;
            
            case 'Q':
            {
                if(!gft_sp._uc_q_str_valid)
                {
                    build_uc_q_value(gft_sp._uc_q_str, tv_usec, gft_sp._tmp);
                    gft_sp._uc_q_str_valid = true;
                }
                gft_sp._ret.append(gft_sp._uc_q_str);
                state = TEXT;
            }
            break;

            // Windows do not support %s format specifier
            //(seconds since epoch).
            case 's':
            {
                if(!gft_sp._s_str_valid)
                {
                    convertIntegerToString(gft_sp._s_str, tv_sec);
                    gft_sp._s_str_valid = true;
                }
                gft_sp._ret.append(gft_sp._s_str);
                state = TEXT;
            }
            break;

            default:
            {
                gft_sp._ret.push_back('%');
                gft_sp._ret.push_back(*fmt_it);
                state = TEXT;
            }
            }
        }
        break;
        }
    }

    // Finally call strftime/wcsftime to format the rest of the string.

    gft_sp._fmt.swap(gft_sp._ret);
    std::size_t buffer_size = gft_sp._fmt.size() + 1;
    std::size_t len;

    // Limit how far can the buffer grow. This is necessary so that we
    // catch bad format string. Some implementations of strftime() signal
    // both too small buffer and invalid format string by returning 0
    // without changing errno. 
    std::size_t const buffer_size_max = (std::max)(static_cast<std::size_t>(1024), buffer_size * 16);

    do
    {
        gft_sp._buffer.resize(buffer_size);
        errno = 0;
        len = helpers::strftime(&gft_sp._buffer[0], buffer_size,
            gft_sp._fmt.c_str(), &time);
        if(len == 0)
        {
            int const eno = errno;
            buffer_size *= 2;
            if(buffer_size > buffer_size_max)
            {
                LogLog::getLogLog()->error("Error in strftime(): "+ convertIntegerToString(eno), true);
            }
        }
    } 
    while(len == 0);

    return std::string(gft_sp._buffer.begin(), gft_sp._buffer.begin() + len);
}


TimeHelper& TimeHelper::operator+=(const TimeHelper& rhs)
{
    tv_sec += rhs.tv_sec;
    tv_usec += rhs.tv_usec;

    if(tv_usec > ONE_SEC_IN_USEC) 
	{
        ++tv_sec;
        tv_usec -= ONE_SEC_IN_USEC;
    }

    return *this;
}


TimeHelper& TimeHelper::operator-=(const TimeHelper& rhs)
{
    tv_sec -= rhs.tv_sec;
    tv_usec -= rhs.tv_usec;

    if(tv_usec < 0) 
	{
        --tv_sec;
        tv_usec += ONE_SEC_IN_USEC;
    }

    return *this;
}


TimeHelper& TimeHelper::operator/=(long rhs)
{
    long rem_secs = static_cast<long>(tv_sec % rhs);
    tv_sec /= rhs;
    
    tv_usec /= rhs;
    tv_usec += static_cast<long>((rem_secs * ONE_SEC_IN_USEC) / rhs);

    return *this;
}


TimeHelper& TimeHelper::operator*=(long rhs)
{
    long new_usec = tv_usec * rhs;
    long overflow_sec = new_usec / ONE_SEC_IN_USEC;
    tv_usec = new_usec % ONE_SEC_IN_USEC;

    tv_sec *= rhs;
    tv_sec += overflow_sec;

    return *this;
}


//////////////////////////////////////////////////////////////////////////////
// Time globals
//////////////////////////////////////////////////////////////////////////////


const TimeHelper operator+(const TimeHelper& lhs, const TimeHelper& rhs)
{
    return TimeHelper(lhs) += rhs;
}


const TimeHelper operator-(const TimeHelper& lhs, const TimeHelper& rhs)
{
    return TimeHelper(lhs) -= rhs;
}


const TimeHelper operator/(const TimeHelper& lhs, long rhs)
{
    return TimeHelper(lhs) /= rhs;
}


const TimeHelper operator*(const TimeHelper& lhs, long rhs)
{
    return TimeHelper(lhs) *= rhs;
}


bool operator<(const TimeHelper& lhs, const TimeHelper& rhs)
{
    return((lhs.sec() < rhs.sec())||((lhs.sec() == rhs.sec()) && (lhs.usec() < rhs.usec())));
}


bool operator<=(const TimeHelper& lhs, const TimeHelper& rhs)
{
    return((lhs < rhs) ||(lhs == rhs));
}


bool operator>(const TimeHelper& lhs, const TimeHelper& rhs)
{
    return((lhs.sec() > rhs.sec()) || ((lhs.sec() == rhs.sec())&&(lhs.usec() > rhs.usec())));
}


bool operator>=(const TimeHelper& lhs, const TimeHelper& rhs)
{
    return((lhs > rhs) ||(lhs == rhs));
}


bool operator==(const TimeHelper& lhs, const TimeHelper& rhs)
{
    return(   lhs.sec() == rhs.sec()
            && lhs.usec() == rhs.usec());
}


bool operator!=(const TimeHelper& lhs, const TimeHelper& rhs)
{
    return !(lhs == rhs);
}


} } // namespace log4cplus { namespace helpers {
