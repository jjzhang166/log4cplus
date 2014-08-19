// -*- C++ -*-
// Module:  Log4CPLUS
// File:    logloguser.h

/** @file */

#ifndef LOG4CPLUS_HELPERS_LOGLOG_USER
#define LOG4CPLUS_HELPERS_LOGLOG_USER

#include <log4cplus/config.h>

namespace log4cplus { namespace helpers {
	// forward declarations
	class LogLog;

	/**
	* This class used to simplify the use of the LogLog class.  Any class
	* that uses the LogLog class should extend this class and retrieve
	* their reference to LogLog using the method provided.
	*/
	class LOG4CPLUS_EXPORT LogLogUser
	{
	public:
		// ctor and dtor
		LogLogUser();
		LogLogUser(const LogLogUser&);
		virtual ~LogLogUser();

		// public methods
		LogLog& getLogLog() const;

		// operators
		LogLogUser& operator= (const LogLogUser& rhs);
	};

} }// end namespace log4cplus end namespace helpers


#endif // LOG4CPLUS_HELPERS_LOGLOG_USER

