// Module:  Log4CPLUS
// File:    loggerimpl.cpp
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

#include <log4cplus/internal/internal.h>
#include <log4cplus/spi/loggerimpl.h>
#include <log4cplus/appender.h>
#include <log4cplus/hierarchy.h>
#include <log4cplus/helpers/loglog.h>
#include <log4cplus/spi/loggingevent.h>
#include <log4cplus/spi/rootlogger.h>


namespace log4cplus { 

//////////////////////////////////////////////////////////////////////////////
// Logger Constructors and Destructor
//////////////////////////////////////////////////////////////////////////////
LoggerImpl::LoggerImpl(const std::string& name_, Hierarchy& h)
  : _name(name_),
    _ll(NOT_SET_LOG_LEVEL),
    _parent(NULL),
    _hierarchy(h)
{
}


LoggerImpl::~LoggerImpl() 
{ 
}


//////////////////////////////////////////////////////////////////////////////
// Logger Methods
//////////////////////////////////////////////////////////////////////////////

void LoggerImpl::callAppenders(const InternalLoggingEvent& loggingEvent)
{
    int writes = 0;
    for(const LoggerImpl* c = this; c != NULL; c=c->_parent.get())
	{
        writes += c->appendLoopOnAppenders(loggingEvent);
    }

    // No appenders in hierarchy, warn user only once.
    if(!_hierarchy.emittedNoAppenderWarning && writes == 0) 
	{
        helpers::getLogLog().error("No appenders could be found for logger (" + getName() + ").");
        helpers::getLogLog().error("Please initialize the log4cplus system properly.");
        _hierarchy.emittedNoAppenderWarning = true;
    }
}


void LoggerImpl::closeNestedAppenders()
{
    SharedAppenderPtrList appenders = getAllAppenders();
    for(SharedAppenderPtrList::iterator it=appenders.begin(); it!=appenders.end(); ++it)
    {
        Appender & appender = **it;
        if (!appender.isClosed())
            appender.close();
    }
}


bool LoggerImpl::isEnabledFor(LogLevel loglevel) const
{
    if(_hierarchy.disableValue >= loglevel) 
	{
        return false;
    }
    return loglevel >= getChainedLogLevel();
}


void LoggerImpl::log(LogLevel loglevel, const std::string& message,
                const char* file, int line, const char* _function)
{
    if(isEnabledFor(loglevel))
	{
        forcedLog(loglevel, message, file, line, _function ? _function : "");
    }
}

void LoggerImpl::log(InternalLoggingEvent const& ev)
{
    if (isEnabledFor(ev.getLogLevel ()))
        forcedLog(ev);
}


LogLevel LoggerImpl::getChainedLogLevel() const
{
    for(const LoggerImpl *c=this; c != NULL; c=c->_parent.get()) 
	{
        if(c->_ll != NOT_SET_LOG_LEVEL) 
		{
            return c->_ll;
        }
    }

    helpers::getLogLog().error("LoggerImpl::getChainedLogLevel()- No valid LogLevel found", true);
    return NOT_SET_LOG_LEVEL;
}


Hierarchy& LoggerImpl::getHierarchy() const
{ 
    return _hierarchy; 
}


void LoggerImpl::forcedLog(LogLevel loglevel, const std::string& message,
                      const char* file, int line, const char* _function)
{
    InternalLoggingEvent & loggingEvent = internal::get_ptd()->_forcedLoggingEvent;
    assert (_function);
    loggingEvent.setLoggingEvent (this->getName(), loglevel, message, file, line,
        _function);
    callAppenders(loggingEvent);
}


void LoggerImpl::forcedLog(InternalLoggingEvent const& ev)
{
    callAppenders(ev);
}

} // namespace log4cplus { 
