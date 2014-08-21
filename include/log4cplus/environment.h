// -*- C++ -*-
// Module:  Log4CPLUS
// File:    env.h


#ifndef LOG4CPLUS_INTERNAL_ENV_H
#define LOG4CPLUS_INTERNAL_ENV_H

#include <log4cplus/config.h>
#include <vector>


using namespace log4cplus;


//!Get environment variable value.
bool getEnvString(string& envString, string const& name);

//!Parse a string as a boolean value.
bool parse2bool(bool& val, string const& str);

//!Parse a path into path components.
bool splitFilePath(vector<string>& components, std::size_t& special, string const& path);

//!Makes directories leading to file.
void make_dirs(string const& file_path);



#endif // LOG4CPLUS_INTERNAL_ENV_H
