// Module:  Log4CPLUS
// File:    ospappender.cxx

#ifdef LOG4CPLUS_USE_OSP

#include <log4cplus/layout.h>
#include <log4cplus/ospappender.h>
#include <log4cplus/loglog.h>
#include <log4cplus/stringhelper.h>
#include <log4cplus/loggingevent.h>
#include "osp/osp.h"
#include <strstream>

using namespace log4cplus;


OspAppender::OspAppender()
{
}


OspAppender::OspAppender(const Properties& properties) : Appender(properties)
{
}


OspAppender::~OspAppender()
{
    destructorImpl();
}


void OspAppender::close()
{
	_isClosed = true;
}


// This method does not need to be locked since it is called by
// doAppend() which performs the locking
void OspAppender::append(const InternalLoggingEvent& loggingEvent)
{
    ostrstream output;
    _layout->formatAndAppend(output, loggingEvent);

	string outstring = output.str();

	OspPrintf(TRUE, FALSE, const_cast<char*>(outstring.c_str()));
}

#endif
