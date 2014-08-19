// Module:  Log4CPLUS
// File:    loglog.cpp



#include <log4cplus/helpers/loglog.h>
#include <log4cplus/helpers/environment.h>
#include <log4cplus/consoleappender.h>
#include <ostream>
#include <stdexcept>


namespace log4cplus { namespace helpers {

namespace
{

static char const PREFIX[] = "log4cplus: ";
static char const WARN_PREFIX[] = "log4cplus:WARN ";
static char const ERR_PREFIX[] = "log4cplus:ERROR ";

} // namespace


LogLog * LogLog::getLogLog()
{
    return &helpers::getLogLog();
}


LogLog::LogLog() : _debugEnabled(TriUndef), _quietMode(TriUndef)
{ }


LogLog::~LogLog()
{ }


void LogLog::setInternalDebugging(bool enabled)
{
     Mutex::ScopedLock lock(_mutex);

    _debugEnabled = enabled ? TriTrue : TriFalse;
}


void LogLog::setQuietMode(bool quietModeVal)
{
     Mutex::ScopedLock lock(_mutex);

    _quietMode = quietModeVal ? TriTrue : TriFalse;
}


void LogLog::debug(const string& msg) const
{
    logging_worker(&LogLog::get_debug_mode, PREFIX, msg);
}


void LogLog::debug(char const * msg) const
{
    logging_worker(&LogLog::get_debug_mode, PREFIX, msg);
}


void LogLog::warn(const string& msg) const
{
    logging_worker(&LogLog::get_not_quiet_mode, WARN_PREFIX, msg);
}


void LogLog::warn(char const * msg) const
{
    logging_worker(&LogLog::get_not_quiet_mode, WARN_PREFIX, msg);
}


void LogLog::error(const string& msg, bool throw_flag) const
{
    logging_worker(&LogLog::get_not_quiet_mode, ERR_PREFIX, msg, throw_flag);
}


void LogLog::error(char const * msg, bool throw_flag) const
{
    logging_worker(&LogLog::get_not_quiet_mode, ERR_PREFIX, msg, throw_flag);
}


bool LogLog::get_quiet_mode() const
{
    if(_quietMode == TriUndef)
        set_tristate_from_env(&_quietMode, "LOG4CPLUS_LOGLOG_QUIETMODE");

    return _quietMode == TriTrue;
}


bool LogLog::get_not_quiet_mode() const
{
    return !get_quiet_mode();
}


bool LogLog::get_debug_mode() const
{
    if(_debugEnabled == TriUndef)
        set_tristate_from_env(&_debugEnabled, "LOG4CPLUS_LOGLOG_DEBUGENABLED");

    return _debugEnabled && !get_quiet_mode();
}


void LogLog::set_tristate_from_env(TriState * result, char const * envvar_name)
{
    string envvar_value;
    bool exists = helpers::get_env_var(envvar_value, envvar_name);
    bool value = false;
    if(exists && helpers::parse_bool(value, envvar_value) && value)
        *result = TriTrue;
    else
        *result = TriFalse;
}


template <typename StringType>
void LogLog::logging_worker( bool(LogLog:: * cond)() const, 
	char const * prefix, StringType const& msg, bool throw_flag) const
{
    bool output;
    {
		Mutex::ScopedLock lock(const_cast<Mutex&>(_mutex));
		output =(this->*cond)();
    }

    if(output)
    {
        // XXX This is potential recursive lock of
        // ConsoleAppender::outputMutex.
		Mutex::ScopedLock lock(const_cast<Mutex&>(ConsoleAppender::getOutputMutex()));
		std::cout << prefix << msg << std::endl;
    }

    if(throw_flag)
        throw std::runtime_error(msg);
}


} } // namespace log4cplus { namespace helpers {
