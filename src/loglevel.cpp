// Module:  Log4CPLUS
// File:    loglevel.cpp
// Created: 6/2001
// Author:  Tad E. Smith
//
//
// Copyright 2001-2013 Tad E. Smith
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <log4cplus/loglevel.h>
#include <log4cplus/helpers/loglog.h>
#include <log4cplus/helpers/stringhelper.h>
#include <log4cplus/internal/internal.h>
#include <algorithm>


namespace log4cplus
{


namespace
{

static std::string const ALL_STRING ("ALL");
static std::string const TRACE_STRING ("TRACE");
static std::string const DEBUG_STRING ("DEBUG");
static std::string const INFO_STRING ("INFO");
static std::string const WARN_STRING ("WARN");
static std::string const ERROR_STRING ("ERROR");
static std::string const FATAL_STRING ("FATAL");
static std::string const OFF_STRING ("OFF");
static std::string const NOTSET_STRING ("NOTSET");
static std::string const UNKNOWN_STRING ("UNKNOWN");


struct log_levels_table_rec
{
    LogLevel const ll;
    std::string const * const str;
};


#define DEF_LLTAB_REC(x) { x ##_LOG_LEVEL, &(x ##_STRING) }

static log_levels_table_rec const log_levels_table[8] = {
    DEF_LLTAB_REC (OFF),
    DEF_LLTAB_REC (FATAL),
    DEF_LLTAB_REC (ERROR),
    DEF_LLTAB_REC (WARN),
    DEF_LLTAB_REC (INFO),
    DEF_LLTAB_REC (DEBUG),
    DEF_LLTAB_REC (TRACE),
    DEF_LLTAB_REC (ALL),
};

#undef DEF_LLTAB_REC


} // namespace



//////////////////////////////////////////////////////////////////////////////
// LogLevelManager ctors and dtor
//////////////////////////////////////////////////////////////////////////////

LogLevelManager::LogLevelManager() 
{
}


LogLevelManager::~LogLevelManager() 
{ }



//////////////////////////////////////////////////////////////////////////////
// LogLevelManager public methods
//////////////////////////////////////////////////////////////////////////////

std::string const&
LogLevelManager::toString(LogLevel ll) const
{
	switch(ll) {
	case OFF_LOG_LEVEL:     return OFF_STRING;
	case FATAL_LOG_LEVEL:   return FATAL_STRING;
	case ERROR_LOG_LEVEL:   return ERROR_STRING;
	case WARN_LOG_LEVEL:    return WARN_STRING;
	case INFO_LOG_LEVEL:    return INFO_STRING;
	case DEBUG_LOG_LEVEL:   return DEBUG_STRING;
	case TRACE_LOG_LEVEL:   return TRACE_STRING;
		//case ALL_LOG_LEVEL:     return ALL_STRING;
	case NOT_SET_LOG_LEVEL: return NOTSET_STRING;
	};

    return UNKNOWN_STRING;
}


LogLevel LogLevelManager::fromString(const std::string& arg) const
{
	std::size_t const tbl_size = sizeof (log_levels_table) / sizeof (log_levels_table[0]);

	for (log_levels_table_rec const * it = log_levels_table; it != log_levels_table + tbl_size; ++it)
	{
		if (*it->str == arg)
			return it->ll;
	}

	return NOT_SET_LOG_LEVEL;
}
 
} // namespace log4cplus
