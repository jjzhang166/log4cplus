// -*- C++ -*-
// Module:  Log4CPLUS
// File:    internal.h


/** @file 
* This header contains declaration internal to log4cplus. They must never be
* visible from user accesible headers or exported in DLL/shared libray.
*/


#ifndef LOG4CPLUS_INTERNAL_INTERNAL_HEADER_
#define LOG4CPLUS_INTERNAL_INTERNAL_HEADER_

#include <log4cplus/config.h>

#include <memory>
#include <vector>
#include <sstream>
#include <cstdio>

#include <log4cplus/loggingevent.h>
#include <log4cplus/thread/tls.h>
#include <log4cplus/snprintf.h>


namespace log4cplus { 

	struct gft_scratch_pad
	{
		gft_scratch_pad();
		~gft_scratch_pad();

		void reset()
		{
			_uc_q_str_valid = false;
			_q_str_valid = false;
			_s_str_valid = false;
			_ret.clear();
		}

		string _q_str;
		string _uc_q_str;
		string _s_str;
		string _ret;
		string _fmt;
		string _tmp;
		vector<char> _buffer;
		bool _uc_q_str_valid;
		bool _q_str_valid;
		bool _s_str_valid;
	};

	//!Per thread data.
	struct per_thread_data
	{
		per_thread_data();
		~per_thread_data();

		string _macros_str;
		gft_scratch_pad _gft_sp;
		string _faa_str;
		InternalLoggingEvent _forcedLoggingEvent;
		std::FILE * _fnull;
		snprintf_buf _snprintf_buf;
	};

	per_thread_data * alloc_ptd();

	// TLS key whose value is pointer struct per_thread_data.
	extern Thread::tls_key_type tls_storage_key;


	inline void set_ptd(per_thread_data * p)
	{
		Thread::tls_set_value(tls_storage_key, p);
	}

	inline per_thread_data * get_ptd(bool alloc = true)
	{
		per_thread_data * ptd = reinterpret_cast<per_thread_data *>(Thread::tls_get_value(tls_storage_key));

		if(!ptd && alloc)
			return alloc_ptd();

		return ptd;
	}

	inline gft_scratch_pad & get_gft_scratch_pad()
	{
		return get_ptd()->_gft_sp;
	}

}  // namespace log4cplus 


#endif // LOG4CPLUS_INTERNAL_INTERNAL_HEADER_