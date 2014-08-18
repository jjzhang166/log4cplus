// Module:  Log4CPLUS
// File:    rootlogger.cpp

#include <log4cplus/spi/rootlogger.h>
#include <log4cplus/helpers/loglog.h>


namespace log4cplus { 


//////////////////////////////////////////////////////////////////////////////
// RootLogger Constructor
//////////////////////////////////////////////////////////////////////////////

RootLogger::RootLogger(Hierarchy& h, LogLevel loglevel) : LoggerImpl("root", h)
{
    setLogLevel(loglevel);
}



//////////////////////////////////////////////////////////////////////////////
// Logger Methods
//////////////////////////////////////////////////////////////////////////////

LogLevel RootLogger::getAssignedLogLevel() const
{
    return _ll;
}


void RootLogger::setLogLevel(LogLevel loglevel)
{
    if(loglevel == NOT_SET_LOG_LEVEL) 
	{
        helpers::getLogLog().error("You have tried to set NOT_SET_LOG_LEVEL to root.");
    }
    else 
	{
        LoggerImpl::setLogLevel(loglevel);
    }
}


} // namespace log4cplus { 
