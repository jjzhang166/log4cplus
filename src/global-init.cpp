// Module:  Log4CPLUS
// File:    global-init.cpp


#include <log4cplus/config.h>

#include <log4cplus/logger.h>
#include <log4cplus/loglog.h>
#include <log4cplus/internal.h>
#include <log4cplus/thread/tls.h>
#include <log4cplus/loglog.h>
#include <log4cplus/factory.h>
#include <log4cplus/hierarchy.h>
#include <cstdio>
#include <iostream>
#include <stdexcept>
#include <log4cplus/thread/Mutex.h>

using namespace log4cplus;
//!Default context.
struct DefaultContext
{
	Mutex console_mutex;
	LogLog loglog;
	LogLevelManager log_level_manager;
	TimeHelper Layout_time_base;
	Hierarchy hierarchy;
	AppenderFactoryRegistry appender_factory_registry;
	LayoutFactoryRegistry layout_factory_registry;
	FilterFactoryRegistry filter_factory_registry;
};


enum DCState
{ 
	DC_UNINITIALIZED,
	DC_INITIALIZED,
	DC_DESTROYED
};


static DCState default_context_state;
static DefaultContext * default_context;


struct destroy_default_context
{
	~destroy_default_context()
	{
		delete default_context;
		default_context = 0;
		default_context_state = DC_DESTROYED;
	}
} static destroy_default_context_;


static void alloc_dc()
{
	assert(!default_context);
	assert(default_context_state == DC_UNINITIALIZED);

	if(default_context)
		throw std::logic_error("alloc_dc() called with non-NULL default_context.");

	if(default_context_state == DC_INITIALIZED)
		throw std::logic_error("alloc_dc() called in DC_INITIALIZED state.");

	default_context = new DefaultContext;

	if(default_context_state == DC_DESTROYED)
		default_context->loglog.error(
		"Re-initializing default context after it has already been destroyed.\n"
		"The memory will be leaked.");

	default_context_state = DC_INITIALIZED;
}


static DefaultContext * get_dc(bool alloc = true)
{
	if(!default_context && alloc)
		alloc_dc();
	return default_context;
}


Mutex const& getConsoleOutputMutex()
{
	return get_dc()->console_mutex;
}

LogLog & log4cplus::getLogLog()
{
	return get_dc()->loglog;
}


TimeHelper const& log4cplus::getLayoutTimeBase()
{
	return get_dc()->Layout_time_base;
}


LogLevelManager & log4cplus::getLogLevelManager() 
{
	return get_dc()->log_level_manager;
}


Hierarchy & log4cplus::getDefaultHierarchy()
{
	return get_dc()->hierarchy;
}



AppenderFactoryRegistry & log4cplus::getAppenderFactoryRegistry()
{
	return get_dc()->appender_factory_registry;
}


LayoutFactoryRegistry & log4cplus::getLayoutFactoryRegistry()
{
	return get_dc()->layout_factory_registry;
}


FilterFactoryRegistry & log4cplus::getFilterFactoryRegistry()
{
	return get_dc()->filter_factory_registry;
}

gft_scratch_pad::gft_scratch_pad()
	: _uc_q_str_valid(false)
	, _q_str_valid(false)
	, _s_str_valid(false)
{ }


gft_scratch_pad::~gft_scratch_pad()
{ }


per_thread_data::per_thread_data()
	: _fnull(0)
{ }


per_thread_data::~per_thread_data()
{
	if(_fnull)
		std::fclose(_fnull);
}


Thread::tls_key_type log4cplus::tls_storage_key;


per_thread_data * log4cplus::alloc_ptd()
{
	per_thread_data * tmp = new per_thread_data;
	set_ptd(tmp);
	return tmp;
}

void initializeFactoryRegistry();


//!Thread local storage clean up function for POSIX threads.
static void ptd_cleanup_func(void * arg)
{
	per_thread_data * const arg_ptd = static_cast<per_thread_data *>(arg);
	per_thread_data * const ptd = get_ptd(false);
	(void) ptd;

	// Either it is a dummy value or it should be the per thread data
	// pointer we get from internal::get_ptd().
	assert(arg == reinterpret_cast<void *>(1) || arg_ptd == ptd ||(!ptd && arg_ptd));

	if(arg == reinterpret_cast<void *>(1))
		// Setting the value through the key here is necessary in case
		// we are using TLS using __thread or __declspec(thread) or
		// similar constructs with POSIX threads.  Otherwise POSIX
		// calls this cleanup routine more than once if the value
		// stays non-NULL after it returns.
		Thread::tls_set_value(log4cplus::tls_storage_key, 0);
	else if(arg)
	{
		// Instead of using internal::get_ptd(false) here we are using
		// the value passed to this function directly.  This is
		// necessary because of the following(from SUSv4):
		//
		// A call to pthread_getspecific() for the thread-specific
		// data key being destroyed shall return the value NULL,
		// unless the value is changed(after the destructor starts)
		// by a call to pthread_setspecific().
		delete arg_ptd;
		Thread::tls_set_value(log4cplus::tls_storage_key, 0);
	}
	else
	{
		// In this case we fall through to threadCleanup() and it does
		// all the necessary work itself.
		;
	}

	log4cplus::threadCleanup();
}


static void threadSetup()
{
	get_ptd(true);
}


void log4cplus::initializeLog4cplus()
{
	static bool initialized = false;
	if(initialized)
		return;

	log4cplus::tls_storage_key = Thread::tls_init(ptd_cleanup_func);
	threadSetup();

	DefaultContext * dc = get_dc(true);
	dc->Layout_time_base = TimeHelper::gettimeofday();
	Logger::getRoot();
	initializeFactoryRegistry();

	initialized = true;
}

void log4cplus::threadCleanup()
{
	// Do thread-specific cleanup.
	per_thread_data * ptd = get_ptd(false);
	delete ptd;
	set_ptd(0);
}


#if defined(_MSC_VER)
static VOID CALLBACK initializeLog4cplusApcProc(ULONG_PTR /*dwParam*/)
{
	log4cplus::initializeLog4cplus();
	threadSetup();
}


static void queueLog4cplusInitializationThroughAPC()
{
	if(!QueueUserAPC(initializeLog4cplusApcProc, GetCurrentThread(), 0))
		throw std::runtime_error("QueueUserAPC() has failed");
}


static void NTAPI thread_callback(LPVOID /*hinstDLL*/, DWORD fdwReason, LPVOID /*lpReserved*/)
{
	// Perform actions based on the reason for calling.
	switch(fdwReason)
	{ 
	case DLL_PROCESS_ATTACH:
		{
			// We cannot initialize log4cplus directly here. This is because
			// DllMain() is called under loader lock. When we are using C++11
			// threads and synchronization primitives then there is a deadlock
			// somewhere in internals of std::mutex::lock().
			queueLog4cplusInitializationThroughAPC();
			break;
		}

	case DLL_THREAD_ATTACH:
		{
			// We could call threadSetup() here but that imposes overhead
			// on threads that do not use log4cplus. Thread local data will
			// be initialized lazily instead.
			break;
		}

	case DLL_THREAD_DETACH:
		{
			// Do thread-specific cleanup.
			log4cplus::threadCleanup();

			break;
		}

	case DLL_PROCESS_DETACH:
		{
			// Perform any necessary cleanup.

			// Do thread-specific cleanup.
			log4cplus::threadCleanup();

			Thread::tls_cleanup(log4cplus::tls_storage_key);
			break;
		}

	} // switch
}

#endif




#if defined(_MSC_VER) 

#if defined(LOG4CPLUS_BUILD_DLL)
extern "C"
	BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved)
{
	thread_callback(hinstDLL, fdwReason, lpReserved);

	return TRUE;  // Successful DLL_PROCESS_ATTACH.
}

#else
struct _static_log4cplus_initializer
{
	_static_log4cplus_initializer()
	{

	}

	~_static_log4cplus_initializer()
	{
		// Last thread cleanup.
		log4cplus::threadCleanup();

		log4cplus::Thread::tls_cleanup(log4cplus::tls_storage_key);

	}
} static initializer;

#endif

#else		//__linux__

struct _static_log4cplus_initializer
{
	_static_log4cplus_initializer()
	{
		log4cplus::initializeLog4cplus();
	}

	~_static_log4cplus_initializer()
	{
		// Last thread cleanup.
		log4cplus::threadCleanup();

		log4cplus::Thread::tls_cleanup(log4cplus::tls_storage_key);
	}
} static initializer;

#endif
