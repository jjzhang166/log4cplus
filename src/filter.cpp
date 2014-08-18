// Module:  Log4CPLUS
// File:    filter.cpp


#include <log4cplus/spi/filter.h>
#include <log4cplus/helpers/loglog.h>
#include <log4cplus/helpers/stringhelper.h>
#include <log4cplus/helpers/property.h>
#include <log4cplus/spi/loggingevent.h>


namespace log4cplus { 

///////////////////////////////////////////////////////////////////////////////
// global methods
///////////////////////////////////////////////////////////////////////////////

FilterResult checkFilter(const Filter* filter, const InternalLoggingEvent& loggingEvent)
{
    const Filter* currentFilter = filter;
    while(currentFilter) 
	{
        FilterResult result = currentFilter->decide(loggingEvent);
        if(result != NEUTRAL) 
		{
            return result;
        }

        currentFilter = currentFilter->_nextFilter.get();
    }

    return ACCEPT;
}



///////////////////////////////////////////////////////////////////////////////
// Filter implementation
///////////////////////////////////////////////////////////////////////////////

Filter::Filter()
{
}


Filter::~Filter()
{
}


void Filter::appendFilter(FilterPtr filter)
{
    if(!_nextFilter)
        _nextFilter = filter;
    else
        _nextFilter->appendFilter(filter);
}



///////////////////////////////////////////////////////////////////////////////
// DenyAllFilter implementation
///////////////////////////////////////////////////////////////////////////////

DenyAllFilter::DenyAllFilter()
{ }


DenyAllFilter::DenyAllFilter(const helpers::Properties&)
{ }


FilterResult DenyAllFilter::decide(const InternalLoggingEvent&) const
{
    return DENY;
}



///////////////////////////////////////////////////////////////////////////////
// LogLevelMatchFilter implementation
///////////////////////////////////////////////////////////////////////////////

LogLevelMatchFilter::LogLevelMatchFilter()
{
    init();
}

LogLevelMatchFilter::LogLevelMatchFilter(const helpers::Properties& properties)
{
    init();

    std::string const& log_level_to_match = properties.getProperty("LogLevelToMatch");
    _logLevelToMatch = getLogLevelManager().fromString(log_level_to_match);
}


void LogLevelMatchFilter::init()
{
    _logLevelToMatch = NOT_SET_LOG_LEVEL;
}


FilterResult LogLevelMatchFilter::decide(const InternalLoggingEvent& loggingEvent) const
{
    if(_logLevelToMatch == NOT_SET_LOG_LEVEL) 
	{
        return NEUTRAL;
    }

    bool matchOccured = (_logLevelToMatch == loggingEvent.getLogLevel());
       
    if(matchOccured)
	{
        return ACCEPT;
    }
    else
	{
        return NEUTRAL;
    }
}



///////////////////////////////////////////////////////////////////////////////
// LogLevelRangeFilter implementation
///////////////////////////////////////////////////////////////////////////////

LogLevelRangeFilter::LogLevelRangeFilter()
{
    init();
}



LogLevelRangeFilter::LogLevelRangeFilter(const helpers::Properties& properties)
{
    init();

    std::string const& log_level_min = properties.getProperty("LogLevelMin");
    _logLevelMin = getLogLevelManager().fromString(log_level_min);

    std::string const& log_level_max = properties.getProperty("LogLevelMax");
    _logLevelMax = getLogLevelManager().fromString(log_level_max);
}


void LogLevelRangeFilter::init()
{
    _logLevelMin = NOT_SET_LOG_LEVEL;
    _logLevelMax = NOT_SET_LOG_LEVEL;
}


FilterResult LogLevelRangeFilter::decide(const InternalLoggingEvent& loggingEvent) const
{
    if((_logLevelMin != NOT_SET_LOG_LEVEL) &&(loggingEvent.getLogLevel() < _logLevelMin)) 
	{
        // priority of loggingEvent is less than minimum
        return DENY;
    }

    if((_logLevelMax != NOT_SET_LOG_LEVEL) &&(loggingEvent.getLogLevel() > _logLevelMax)) 
	{
        // priority of loggingEvent is greater than maximum
        return DENY;
    }

     return ACCEPT;
}

} // namespace log4cplus { 