// -*- C++ -*-
// Module:  Log4CPLUS
// File:    env.h


#ifndef LOG4CPLUS_INTERNAL_ENV_H
#define LOG4CPLUS_INTERNAL_ENV_H

#include <log4cplus/config.h>

#include <vector>


#if defined(_MSC_VER)
#include <log4cplus/windowsh-inc.h>
#endif



namespace log4cplus { namespace internal {


//!Get environment variable value.
bool get_env_var(std::string& envString, std::string const& name);

//!Parse a string as a boolean value.
bool parse_bool(bool & val, std::string const& str);

//!Parse a path into path components.
bool split_path(std::vector<std::string>& components, std::size_t& special, std::string const& path);

//!Makes directories leading to file.
void make_dirs(std::string const& file_path);

} } // namespace log4cplus { namespace internal {


#endif // LOG4CPLUS_INTERNAL_ENV_H
