// -*- C++ -*-

#if !defined (LOG4CPLUS_HELPERS_FILEINFO_H)
#define LOG4CPLUS_HELPERS_FILEINFO_H

#include <log4cplus/config.h>

#include <log4cplus/helpers/timehelper.h>
#include <sys/types.h>


namespace log4cplus { namespace helpers {

//!FileInfo structure is OS independent abstraction of the
//!<code>stat()</code> function.
struct LOG4CPLUS_EXPORT FileInfo
{
    helpers::TimeHelper mtime;
    bool is_link;
    off_t size;
};


//!OS independent abstraction of <code>stat()</code> function.
LOG4CPLUS_EXPORT int getFileInfo (FileInfo * fi, std::string const& name);


} } // namespace log4cplus { namespace helpers {

#endif // LOG4CPLUS_HELPERS_FILEINFO_H
