// Module:  Log4CPLUS
// File:    loggingmacros.cpp


/** @file
* This file implements support function for loggingmacros.h file. */

#include <log4cplus/internal.h>
#include <log4cplus/loggingmacros.h>


using namespace log4cplus;


snprintf_buf & log4cplus::get_macro_body_snprintf_buf()
{
	return get_ptd()->_snprintf_buf;
}


void log4cplus::macro_forced_log(Logger const& logger, LogLevel log_level, 
	char const * msg, char const * filename, int line, char const * func)
{
	macro_forced_log(logger, log_level,
		get_ptd()->_macros_str = msg, filename, line, func);
}


void log4cplus::macro_forced_log(Logger const& logger, LogLevel log_level, 
	string const& msg, char const * filename, int line, char const * func)
{
	InternalLoggingEvent & loggingEvent = get_ptd()->_forcedLoggingEvent;
	loggingEvent.setLoggingEvent(logger.getName(), log_level, msg, filename, line, func);
	logger.forcedLog(loggingEvent);
}

