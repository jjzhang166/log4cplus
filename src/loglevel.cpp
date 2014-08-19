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
#include <log4cplus/helpers/internal.h>
#include <algorithm>


namespace log4cplus
{

static string const ALL_STRING ("ALL");
static string const TRACE_STRING ("TRACE");
static string const DEBUG_STRING ("DEBUG");
static string const INFO_STRING ("INFO");
static string const WARN_STRING ("WARN");
static string const ERROR_STRING ("ERROR");
static string const FATAL_STRING ("FATAL");
static string const OFF_STRING ("OFF");
static string const NOTSET_STRING ("NOTSET");
static string const UNKNOWN_STRING ("UNKNOWN");


LogLevelManager::LogLevelManager() 
{
}


LogLevelManager::~LogLevelManager() 
{ }



//////////////////////////////////////////////////////////////////////////////
// LogLevelManager public methods
//////////////////////////////////////////////////////////////////////////////

string const& LogLevelManager::toString(LogLevel ll) const
{
	switch(ll) 
	{
	case OFF_LOG_LEVEL:     return OFF_STRING;
	case FATAL_LOG_LEVEL:   return FATAL_STRING;
	case ERROR_LOG_LEVEL:   return ERROR_STRING;
	case WARN_LOG_LEVEL:    return WARN_STRING;
	case INFO_LOG_LEVEL:    return INFO_STRING;
	case DEBUG_LOG_LEVEL:   return DEBUG_STRING;
	case TRACE_LOG_LEVEL:   return TRACE_STRING;
	case NOT_SET_LOG_LEVEL: return NOTSET_STRING;
	};

	return UNKNOWN_STRING;
}


LogLevel LogLevelManager::fromString(const string& llstring) const
{
	if (llstring == OFF_STRING)
		return OFF_LOG_LEVEL;
	else if (llstring == FATAL_STRING)
		return FATAL_LOG_LEVEL;
	else if (llstring == ERROR_STRING)
		return ERROR_LOG_LEVEL;
	else if (llstring == WARN_STRING)
		return WARN_LOG_LEVEL;
	else if (llstring == INFO_STRING)
		return INFO_LOG_LEVEL;
	else if (llstring == DEBUG_STRING)
		return DEBUG_LOG_LEVEL;
	else if (llstring == TRACE_STRING)
		return TRACE_LOG_LEVEL;
	else 
		return NOT_SET_LOG_LEVEL;

}
 
} // namespace log4cplus
