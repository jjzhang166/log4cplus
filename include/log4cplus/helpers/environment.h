// -*- C++ -*-
// Module:  Log4CPLUS
// File:    env.h


#ifndef LOG4CPLUS_INTERNAL_ENV_H
#define LOG4CPLUS_INTERNAL_ENV_H

#include <log4cplus/config.h>

#include <vector>


#if defined(_MSC_VER)

#endif



namespace log4cplus { namespace helpers {


	//!Get environment variable value.
	bool get_env_var(string& envString, string const& name);

	//!Parse a string as a boolean value.
	bool parse_bool(bool & val, string const& str);

	//!Parse a path into path components.
	bool split_path(vector<string>& components, std::size_t& special, string const& path);

	//!Makes directories leading to file.
	void make_dirs(string const& file_path);

} } // namespace log4cplus { namespace internal {


#endif // LOG4CPLUS_INTERNAL_ENV_H
