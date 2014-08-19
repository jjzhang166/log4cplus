// Module:  Log4CPLUS
// File:    loggingmacros.cpp


/** @file
 * This file implements support function for loggingmacros.h file. */

#include <log4cplus/helpers/internal.h>
#include <log4cplus/loggingmacros.h>


namespace log4cplus { 


helpers::snprintf_buf & get_macro_body_snprintf_buf()
{
    return internal::get_ptd()->_snprintf_buf;
}


void macro_forced_log(Logger const& logger, LogLevel log_level, 
			char const * msg, char const * filename, int line, char const * func)
{
    macro_forced_log(logger, log_level,
        internal::get_ptd()->_macros_str = msg, filename, line, func);
}


void macro_forced_log(Logger const& logger, LogLevel log_level, 
			std::string const& msg, char const * filename, int line, char const * func)
{
    InternalLoggingEvent & loggingEvent = internal::get_ptd()->_forcedLoggingEvent;
    loggingEvent.setLoggingEvent(logger.getName(), log_level, msg, filename, line, func);
    logger.forcedLog(loggingEvent);
}


} // namespace log4cplus { 
