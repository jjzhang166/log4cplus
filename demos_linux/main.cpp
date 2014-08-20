#include <log4cplus/loggingmacros.h>
#include <log4cplus/configurator.h>
#include <log4cplus/loglog.h>
#include <log4cplus/stringhelper.h>
#include <log4cplus/loggingmacros.h>
#include <log4cplus/sleep.h>
#include "osp/osp.h"


using namespace std;
using namespace log4cplus;


static Logger logFile1 = Logger::getInstance("logFile1");
static Logger logFile2 = Logger::getInstance("logFile2");


int main()
{
	BOOL isInit = OspInit(TRUE, 2501);
	if (isInit)
	{
		std::cout << "isInit is true" <<endl;
	} 
	else
	{
		std::cout << "isInit is false" <<endl;
	}
	sleepmillis(3000);
	
	log4cplus::PropertyConfigurator::doConfigure("urconfig.properties");

	for (int i = 0; i < 10; i++)
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

		sleepmillis(2000);
	}
	return 0;
}
