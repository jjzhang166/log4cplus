
#include <iostream>

#include <log4cplus/configurator.h>
#include <log4cplus/loggingmacros.h>
#include <log4cplus/sleep.h>
#include "log4cplus/customappender.h"
#include "log4cplus/loglog.h"



using namespace std;
using namespace log4cplus;



static Logger logFile1 = Logger::getInstance("logFile1");
static Logger logFile2 = Logger::getInstance("logFile2");

#ifdef WIN32

void __stdcall customFunc(const char* sz)
{
	OutputDebugString(sz);
}

#else	__linux__
void customFunc(const char* sz)
{
	//
}
#endif


int main()
{
	log4cplus::CustomAppender::setCustomFunc(customFunc);
	log4cplus::PropertyConfigurator::doConfigure("urconfig.properties");
	LogLog::getLogLog()->setInternalDebugging(true);

	sleepmillis(2000);

	for (int i = 0; i < 3; i++)
	{
		LOG4CPLUS_DEBUG(logFile1,	"1����logFile1.\n");
		LOG4CPLUS_INFO(logFile1,	"2����logFile1.\n");
		LOG4CPLUS_WARN(logFile1,	"3����logFile1.\n");
		LOG4CPLUS_ERROR(logFile1,	"4����logFile1.\n");
		LOG4CPLUS_FATAL(logFile1,	"5����logFile1.\n");
		LOG4CPLUS_INFO_FMT(logFile1,	"FMT -- %d����logFile1.\n", 6);

		LOG4CPLUS_DEBUG(logFile2,	"1����logFile2.\n");
		LOG4CPLUS_INFO(logFile2,	"2����logFile2.\n");
		LOG4CPLUS_WARN(logFile2,	"3����logFile2.\n");
		LOG4CPLUS_ERROR(logFile2,	"4����logFile2.\n");
		LOG4CPLUS_FATAL(logFile2,	"5����logFile2.\n");
		LOG4CPLUS_INFO_FMT(logFile2,	"FMT -- %d����logFile2.\n", 6);

		sleepmillis(100);
	}

	return 0;
}

