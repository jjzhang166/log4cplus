
#include <log4cplus/configurator.h>
#include <log4cplus/loggingmacros.h>
#include <log4cplus/helpers/sleep.h>
#include "osp/osp.h"
// #include "vld.h"


using namespace std;
using namespace log4cplus;
using namespace log4cplus::helpers;


static Logger logFile1 = Logger::getInstance("logFile1");
static Logger logFile2 = Logger::getInstance("logFile2");


int main()
{
	OspInit(TRUE, 2501);

	log4cplus::PropertyConfigurator::doConfigure("urconfig.properties");

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
