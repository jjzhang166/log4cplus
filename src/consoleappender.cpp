// Module:  Log4CPLUS
// File:    consoleappender.cpp

#include <log4cplus/config.h>
#include <log4cplus/layout.h>
#include <log4cplus/consoleappender.h>

#include <log4cplus/helpers/loglog.h>
#include <log4cplus/helpers/stringhelper.h>
#include <log4cplus/helpers/property.h>
#include <log4cplus/loggingevent.h>
#include <ostream>


namespace log4cplus
{


namespace helpers
{

extern log4cplus::Mutex const& getConsoleOutputMutex ();

} // namespace helpers


log4cplus::Mutex const& ConsoleAppender::getOutputMutex ()
{
	return helpers::getConsoleOutputMutex();
}


//////////////////////////////////////////////////////////////////////////////
// ConsoleAppender ctors and dtor
//////////////////////////////////////////////////////////////////////////////

ConsoleAppender::ConsoleAppender(bool immediateFlush_) : _immediateFlush(immediateFlush_)
{
}



ConsoleAppender::ConsoleAppender(const helpers::Properties & properties)
	: Appender(properties), _immediateFlush(false)
{
    properties.getBool (_immediateFlush, "ImmediateFlush");
}



ConsoleAppender::~ConsoleAppender()
{
    destructorImpl();
}



//////////////////////////////////////////////////////////////////////////////
// ConsoleAppender public methods
//////////////////////////////////////////////////////////////////////////////

void ConsoleAppender::close()
{
    helpers::getLogLog().debug("Entering ConsoleAppender::close()..");
    _isClosed = true;
}



//////////////////////////////////////////////////////////////////////////////
// ConsoleAppender protected methods
//////////////////////////////////////////////////////////////////////////////

void ConsoleAppender::append(const InternalLoggingEvent& loggingEvent)
{
   Mutex::ScopedLock lock(const_cast<Mutex&>(getOutputMutex()));

    _layout->formatAndAppend(std::cout, loggingEvent);
    if(_immediateFlush) 
	{
        std::cout.flush();
    }
}


} // namespace log4cplus
