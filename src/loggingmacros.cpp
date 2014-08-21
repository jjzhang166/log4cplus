// Module:  Log4CPLUS
// File:    loggingmacros.cpp


/** @file
* This file implements support function for loggingmacros.h file. */

#include <log4cplus/internal.h>
#include <log4cplus/loggingmacros.h>


using namespace log4cplus;


snprintfBuf & log4cplus::macro_getStringBuf()
{
	return getPerThreadData()->_snprintfBuf;
}


void log4cplus::macro_forcedLog(Logger const& logger, LogLevel log_level, 
	char const* msg, char const* filename, int line, char const* func)
{
	macro_forcedLog(logger, log_level, getPerThreadData()->_macrosStr = msg, filename, line, func);
}


void log4cplus::macro_forcedLog(Logger const& logger, LogLevel logLevel, 
	string const& msg, char const* filename, int line, char const* func)
{
	InternalLoggingEvent & loggingEvent = getPerThreadData()->_forcedLoggingEvent;
	loggingEvent.setLoggingEvent(logger.getName(), logLevel, msg, filename, line, func);
	logger.forcedLog(loggingEvent);
}

