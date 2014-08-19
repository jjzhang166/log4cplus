
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
		LOG4CPLUS_DEBUG(logFile1,	"1中文logFile1.\n");
		LOG4CPLUS_INFO(logFile1,	"2中文logFile1.\n");
		LOG4CPLUS_WARN(logFile1,	"3中文logFile1.\n");
		LOG4CPLUS_ERROR(logFile1,	"4中文logFile1.\n");
		LOG4CPLUS_FATAL(logFile1,	"5中文logFile1.\n");
		LOG4CPLUS_INFO_FMT(logFile1,	"FMT -- %d中文logFile1.\n", 6);

		LOG4CPLUS_DEBUG(logFile2,	"1中文logFile2.\n");
		LOG4CPLUS_INFO(logFile2,	"2中文logFile2.\n");
		LOG4CPLUS_WARN(logFile2,	"3中文logFile2.\n");
		LOG4CPLUS_ERROR(logFile2,	"4中文logFile2.\n");
		LOG4CPLUS_FATAL(logFile2,	"5中文logFile2.\n");
		LOG4CPLUS_INFO_FMT(logFile2,	"FMT -- %d中文logFile2.\n", 6);

		sleepmillis(100);
	}
	return 0;
}
