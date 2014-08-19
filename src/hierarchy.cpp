// Module:  Log4CPLUS
// File:    hierarchy.cpp
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

#include <log4cplus/hierarchy.h>
#include <log4cplus/helpers/loglog.h>
#include <log4cplus/loggerimpl.h>
#include <log4cplus/rootlogger.h>
#include <utility>
#include <limits>


namespace log4cplus
{


//////////////////////////////////////////////////////////////////////////////
// File "Local" methods
//////////////////////////////////////////////////////////////////////////////

namespace
{

static
bool startsWith(std::string const& teststr, std::string const& substr)
{
    bool val = false;
    std::string::size_type const len = substr.length();
    if (teststr.length() > len)
        val = teststr.compare (0, len, substr) == 0;

    return val;
}

}



//////////////////////////////////////////////////////////////////////////////
// Hierarchy ctor and dtor
//////////////////////////////////////////////////////////////////////////////

Hierarchy::Hierarchy() : defaultFactory(new DefaultLoggerFactory()), root(NULL)
  // Don't disable any LogLevel level by default.
  , disableValue(NOT_SET_LOG_LEVEL), emittedNoAppenderWarning(false)
{
    root = Logger( new RootLogger(*this, DEBUG_LOG_LEVEL));
}


Hierarchy::~Hierarchy()
{
}



//////////////////////////////////////////////////////////////////////////////
// Hierarchy public methods
//////////////////////////////////////////////////////////////////////////////

void 
Hierarchy::clear() 
{
    Mutex::ScopedLock lock(_hashtable_mutex);

    provisionNodes.erase(provisionNodes.begin(), provisionNodes.end());
    loggerPtrs.erase(loggerPtrs.begin(), loggerPtrs.end());
}


bool
Hierarchy::exists(const std::string& name)
{
    // Root logger always does exist.
    if (name.empty ())
        return true;

    Mutex::ScopedLock lock(_hashtable_mutex);

    LoggerMap::iterator it = loggerPtrs.find(name);
    return it != loggerPtrs.end();
}


void 
Hierarchy::disable(const std::string& loglevelStr)
{
    if(disableValue != DISABLE_LOG_OVERRIDE) {
        disableValue = getLogLevelManager().fromString(loglevelStr);
    }
}


void 
Hierarchy::disable(LogLevel ll) 
{
    if(disableValue != DISABLE_LOG_OVERRIDE) {
        disableValue = ll;
    }
}


void 
Hierarchy::disableAll() 
{ 
    disable((std::numeric_limits<LogLevel>::max) ());
}


void 
Hierarchy::disableDebug() 
{ 
    disable(DEBUG_LOG_LEVEL);
}


void 
Hierarchy::disableInfo() 
{ 
    disable(INFO_LOG_LEVEL);
}


void 
Hierarchy::enableAll() 
{ 
    disableValue = NOT_SET_LOG_LEVEL; 
}


Logger 
Hierarchy::getInstance(const std::string& name) 
{ 
    return getInstance(name, *defaultFactory); 
}


Logger 
Hierarchy::getInstance(const std::string& name, LoggerFactory& factory)
{
    Mutex::ScopedLock lock(_hashtable_mutex);

    return getInstanceImpl(name, factory);
}


LoggerList 
Hierarchy::getCurrentLoggers()
{
    LoggerList ret;
    
    {
        Mutex::ScopedLock lock(_hashtable_mutex);
        initializeLoggerList(ret);
    }

    return ret;
}


bool 
Hierarchy::isDisabled(LogLevel level) 
{ 
    return disableValue >= level; 
}


Logger 
Hierarchy::getRoot() const
{ 
    return root; 
}


void 
Hierarchy::resetConfiguration()
{
    getRoot().setLogLevel(DEBUG_LOG_LEVEL);
    disableValue = NOT_SET_LOG_LEVEL;


    LoggerList loggers = getCurrentLoggers();
    for (LoggerList::iterator it = loggers.begin (); it != loggers.end(); ++it)
    {
        Logger & logger = *it;
        logger.setLogLevel(NOT_SET_LOG_LEVEL);
    }

}


void 
Hierarchy::setLoggerFactory(std::auto_ptr<LoggerFactory> factory) 
{ 
    defaultFactory = factory; 
}


LoggerFactory *
Hierarchy::getLoggerFactory()
{
    return defaultFactory.get();
}




//////////////////////////////////////////////////////////////////////////////
// Hierarchy private methods
//////////////////////////////////////////////////////////////////////////////

Logger Hierarchy::getInstanceImpl(const std::string& name, LoggerFactory& factory)
{
    Logger logger;
    LoggerMap::iterator lm_it;

    if (name.empty ())
        logger = root;
    else if ((lm_it = loggerPtrs.find(name)) != loggerPtrs.end())
        logger = lm_it->second;
    else
    {
        // Need to create a new logger
        logger = factory.makeNewLoggerInstance(name, *this);
        bool inserted = loggerPtrs.insert(std::make_pair(name, logger)).second;
        if (!inserted)
        {
            helpers::getLogLog().error("Hierarchy::getInstanceImpl()- Insert failed", true);
        }

        ProvisionNodeMap::iterator pnm_it = provisionNodes.find(name);
        if (pnm_it != provisionNodes.end())
        {
            updateChildren(pnm_it->second, logger);
            bool deleted = (provisionNodes.erase(name) > 0);
            if (!deleted)
            {
                helpers::getLogLog().error("Hierarchy::getInstanceImpl()- Delete failed", true);
            }
        }
        updateParents(logger);
    }

    return logger;
}


void Hierarchy::initializeLoggerList(LoggerList& list) const
{
    for(LoggerMap::const_iterator it=loggerPtrs.begin(); it!= loggerPtrs.end(); ++it) 
    {
        list.push_back((*it).second);
    }
}


void 
Hierarchy::updateParents(Logger const& logger)
{
    std::string const& name = logger.getName();
    std::size_t const length = name.length();
    bool parentFound = false;
    std::string substr;

    // if name = "w.x.y.z", loop thourgh "w.x.y", "w.x" and "w", but not "w.x.y.z"
    for(std::size_t i=name.find_last_of('.', length-1);
        i != std::string::npos && i > 0; 
        i = name.find_last_of('.', i-1)) 
    {
        substr.assign (name, 0, i);

        LoggerMap::iterator it = loggerPtrs.find(substr);
        if(it != loggerPtrs.end()) {
            parentFound = true;
            logger._pLoggerImpl->_parent = it->second._pLoggerImpl;
            break;  // no need to update the ancestors of the closest ancestor
        }
        else {
            ProvisionNodeMap::iterator it2 = provisionNodes.find(substr);
            if(it2 != provisionNodes.end()) {
                it2->second.push_back(logger);
            }
            else {
                ProvisionNode node;
                node.push_back(logger);
                std::pair<ProvisionNodeMap::iterator, bool> tmp = 
                    provisionNodes.insert(std::make_pair(substr, node));
                //bool inserted = provisionNodes.insert(std::make_pair(substr, node)).second;
                if(!tmp.second) {
                    helpers::getLogLog().error(
                        "Hierarchy::updateParents()- Insert failed",
                        true);
                }
            }
        } // end if Logger found
    } // end for loop

    if(!parentFound) {
        logger._pLoggerImpl->_parent = root._pLoggerImpl;
    }
}


void 
Hierarchy::updateChildren(ProvisionNode& pn, Logger const& logger)
{

    for(ProvisionNode::iterator it=pn.begin(); it!=pn.end(); ++it) {
        Logger& c = *it;
        // Unless this child already points to a correct (lower) parent,
        // make logger.parent point to c.parent and c.parent to logger.
        if( !startsWith(c._pLoggerImpl->_parent->getName(), logger.getName())) {
            logger._pLoggerImpl->_parent = c._pLoggerImpl->_parent;
            c._pLoggerImpl->_parent = logger._pLoggerImpl;
        }
    }
}


} // namespace log4cplus
