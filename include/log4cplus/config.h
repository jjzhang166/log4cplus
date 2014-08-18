
#ifndef LOG4CPLUS_CONFIG_HXX
#define LOG4CPLUS_CONFIG_HXX

#include <string>
#include <iostream>
using namespace std;

#if defined (_MSC_VER)
	#if !defined (LOG4CPLUS_STATIC)
		#undef LOG4CPLUS_BUILD_DLL
		#define LOG4CPLUS_BUILD_DLL
	#endif

	#if !defined (LOG4CPLUS_BUILD_DLL)
		#undef LOG4CPLUS_STATIC
		#define LOG4CPLUS_STATIC
	#endif

	#if defined (LOG4CPLUS_STATIC) && defined (LOG4CPLUS_BUILD_DLL)
		#error LOG4CPLUS_STATIC and LOG4CPLUS_BUILD_DLL cannot be defined both.
	#endif


	#if defined (LOG4CPLUS_BUILD_DLL)
		#if defined (INSIDE_LOG4CPLUS)
			#define LOG4CPLUS_EXPORT __declspec(dllexport)
		#else
			#define LOG4CPLUS_EXPORT __declspec(dllimport)
		#endif
	#else
		#define LOG4CPLUS_EXPORT	/* empty */
	#endif

	  // Warning about: <type1> needs to have dll-interface to be used by clients of class <type2>
	#pragma warning( disable : 4251 )

#else	//linux

	#define LOG4CPLUS_EXPORT /* empty */
#endif


#if defined(__cplusplus)
namespace log4cplus
{

//! Per thread cleanup function. Users should call this function before
//! a thread ends its execution. It frees resources allocated in thread local
//! storage. It is important only for multi-threaded static library builds
//! of log4cplus and user threads. In all other cases the clean up is provided
//! automatically by other means.
LOG4CPLUS_EXPORT void threadCleanup ();

//! Initializes log4cplus.
LOG4CPLUS_EXPORT void initializeLog4cplus ();

} // namespace log4cplus

#endif

#endif // LOG4CPLUS_CONFIG_HXX
