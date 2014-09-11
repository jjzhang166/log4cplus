// Module:  Log4CPLUS
// File:    ospappender.h


/** @file */

#ifndef _LOG4CPLUS_OSP_APPENDER_HEADER_
#define _LOG4CPLUS_OSP_APPENDER_HEADER_

#include <log4cplus/platform.h>

#include <log4cplus/appender.h>

#ifdef WIN32
typedef void (__stdcall *pCustomFuncCallBack)(const char*);
#else
typedef void (*pCustomFuncCallBack)(const char*);
#endif	

namespace log4cplus {

	class LOG4CPLUS_EXPORT CustomAppender : public Appender
	{
	public:

		// Ctors
		CustomAppender();
		CustomAppender(const Properties& properties);

		// Dtor
		virtual ~CustomAppender();

		// Methods
		virtual void close();

		static void setCustomFunc(pCustomFuncCallBack pCustomFunc);	

	protected:
		virtual void append(const InternalLoggingEvent& loggingEvent);

	private:
		// Disallow copying of instances of this class
		CustomAppender(const CustomAppender&);
		CustomAppender& operator=(const CustomAppender&);
		static pCustomFuncCallBack _pCustomFunc;
	};

} // end namespace log4cplus

#endif // _LOG4CPLUS_OSP_APPENDER_HEADER_

