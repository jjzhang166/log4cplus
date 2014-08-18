// -*- C++ -*-
// Module:  Log4CPLUS
// File:    appenderattachable.h


/** @file */

#ifndef LOG4CPLUS_SPI_APPENDER_ATTACHABLE_HEADER_
#define LOG4CPLUS_SPI_APPENDER_ATTACHABLE_HEADER_

#include <log4cplus/config.h>

#include <log4cplus/appender.h>

#include <log4cplus/helpers/pointer.h>
#include <vector>

namespace log4cplus {
    // Forward Declarations
    typedef helpers::SharedObjectPtr<Appender> SharedAppenderPtr;
    typedef std::vector<log4cplus::SharedAppenderPtr> SharedAppenderPtrList;


        /**
         * This Interface is for attaching Appenders to objects.
         */
        class LOG4CPLUS_EXPORT AppenderAttachable 
		{
        public:
          // Methods
            /**
             * Add an appender.
             */
            virtual void addAppender(SharedAppenderPtr newAppender) = 0;

            /**
             * Get all previously added appenders as an Enumeration.  
             */
            virtual SharedAppenderPtrList getAllAppenders() = 0;

            /**
             * Get an appender by name.
             */
            virtual SharedAppenderPtr getAppender(const std::string& name) = 0;

            /**
             * Remove all previously added appenders.
             */
            virtual void removeAllAppenders() = 0;

            /**
             * Remove the appender passed as parameter from the list of appenders.
             */
            virtual void removeAppender(SharedAppenderPtr appender) = 0;

            /**
             * Remove the appender with the name passed as parameter from the
             * list of appenders.  
             */
            virtual void removeAppender(const std::string& name) = 0;

          // Dtor
            virtual ~AppenderAttachable() = 0;
        };

} // end namespace log4cplus

#endif // LOG4CPLUS_SPI_APPENDER_ATTACHABLE_HEADER_

