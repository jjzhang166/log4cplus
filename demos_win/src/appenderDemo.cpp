#include <exception>
#include <iostream>
#include <string>
#include <sstream>
#include "log4cplus/logger.h"
#include "log4cplus/consoleappender.h"
#include "log4cplus/appenderattachableimpl.h"
#include "log4cplus/loglog.h"
#include "log4cplus/pointer.h"
#include "log4cplus/property.h"
#include "log4cplus/loggingevent.h"

using namespace std;
using namespace log4cplus;
using namespace log4cplus::helpers;

static void 
printAppenderList(const SharedAppenderPtrList& list)
{
   cout << "List size: " << list.size() << endl;
   for(SharedAppenderPtrList::const_iterator it=list.begin(); it!=list.end(); ++it) 
   {
       std::cout << "Loop Body: Appender name = " << (*it)->getName() << endl;
   }
}


// This appender does not call destructorImpl(), which is wrong.
class BadDerivedAppender
    : public Appender
{
public:
    virtual void close ()
    { }

    virtual void append (const log4cplus::InternalLoggingEvent&)
    { }
};


int
main()
{
    log4cplus::initializeLog4cplus();
    LogLog::getLogLog()->setInternalDebugging(true);
    {
        AppenderAttachableImpl aai;
        try {
            SharedObjectPtr<Appender> append_1(new ConsoleAppender(true));
            append_1->setName("First");

            SharedObjectPtr<Appender> append_2(new ConsoleAppender());
            append_2->setName("Second");

            // Test that we get back the same layout as we set.

            Layout * layout_2;
            append_2->setLayout(
                std::auto_ptr<Layout>(layout_2 = new log4cplus::SimpleLayout));
            if (append_2->getLayout () != layout_2)
                return 1;

            // Default errorhandler should be set.

            if (!append_2->getErrorHandler ())
                return 2;

            // Test warning on NULL handler.

            append_2->setErrorHandler (std::auto_ptr<ErrorHandler>());

            // Set working errorhandler.

            std::auto_ptr<ErrorHandler> errorHandler (new OnlyOnceErrorHandler);
            append_2->setErrorHandler (errorHandler);

            // Test logging through instantiated appenders.

            InternalLoggingEvent loggingEvent(
                Logger::getInstance("test").getName(),
                DEBUG_LOG_LEVEL, "This is a test...", __FILE__,
                __LINE__, "main");

            aai.addAppender(append_1);
            aai.addAppender(append_2);
            aai.addAppender(append_1);
            aai.addAppender(append_2);
            aai.addAppender(SharedAppenderPtr());
            printAppenderList(aai.getAllAppenders());

            aai.removeAppender(append_2);
            printAppenderList(aai.getAllAppenders());

            aai.removeAppender("First");
            aai.removeAppender(SharedAppenderPtr());
            printAppenderList(aai.getAllAppenders());

            aai.addAppender(append_1);
            aai.addAppender(append_2);
            aai.addAppender(append_1);
            aai.addAppender(append_2);
            std::cout << "Should be First: "
                             << aai.getAppender("First")->getName() << endl;
            std::cout << "Should be Second: "
                             << aai.getAppender("Second")->getName() << endl
                             << endl;
            append_1->doAppend(loggingEvent);
            append_2->doAppend(loggingEvent);

            // Test appending to closed appender errorhandling.

            append_2->close ();
            append_2->doAppend (loggingEvent);

            // Test appender's errorhandling for wrong layout.

            {
                istringstream propsStream (
                    "layout=log4cplus::WrongLayout");
                Properties props (propsStream);
                SharedObjectPtr<Appender> append (
                    new ConsoleAppender (props));
                append->setName ("Third");
            }

            // Test threshold parsing.

            {
                istringstream propsStream (
                    "Threshold=ERROR");
                Properties props (propsStream);
                SharedObjectPtr<Appender> append (
                    new ConsoleAppender (props));
                append->setName ("Fourth");
            }

            // Test threshold parsing of wrong log level.

            {
                istringstream propsStream (
                    "Threshold=WRONG");
                Properties props (propsStream);
                SharedObjectPtr<Appender> append (
                    new ConsoleAppender (props));
                append->setName ("Fifth");
            }

            // Test wrong filter parsing.

            {
                istringstream propsStream (
                    "filters.1=log4cplus::WrongFilter");
                Properties props (propsStream);
                SharedObjectPtr<Appender> append (
                    new ConsoleAppender (props));
                append->setName ("Sixth");
            }

            // Test errorreporting of badly coded appender.

            {
                BadDerivedAppender appender;
                appender.setName ("Seventh");
            }
        }
        catch(std::exception const& e) {
            std::cout << "**** Exception occured: " << e.what() << endl;
        }
    }

    std::cout << "Exiting main()..." << endl;
    return 0;
}
