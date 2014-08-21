// Module:  Log4CPLUS
// File:    consoleappender.cpp

#include <log4cplus/config.h>
#include <log4cplus/layout.h>
#include <log4cplus/consoleappender.h>

#include <log4cplus/loglog.h>
#include <log4cplus/stringhelper.h>
#include <log4cplus/property.h>
#include <log4cplus/loggingevent.h>
#include <ostream>

using namespace log4cplus;

extern log4cplus::Mutex const& getConsoleOutputMutex ();

log4cplus::Mutex const& ConsoleAppender::getOutputMutex ()
{
	return getConsoleOutputMutex();
}


ConsoleAppender::ConsoleAppender(bool immediateFlush_) : _immediateFlush(immediateFlush_) {}


ConsoleAppender::ConsoleAppender(const Properties & properties)
	: Appender(properties), _immediateFlush(false)
{
	properties.getBool (_immediateFlush, "ImmediateFlush");
}


ConsoleAppender::~ConsoleAppender()
{
	destructorImpl();
}


void ConsoleAppender::close()
{
	_isClosed = true;
}


void ConsoleAppender::append(const InternalLoggingEvent& loggingEvent)
{
	Mutex::ScopedLock lock(const_cast<Mutex&>(getOutputMutex()));

	_layout->formatAndAppend(std::cout, loggingEvent);
	if(_immediateFlush) 
	{
		std::cout.flush();
	}
}

