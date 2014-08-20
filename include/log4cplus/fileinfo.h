// -*- C++ -*-

#if !defined (LOG4CPLUS_HELPERS_FILEINFO_H)
#define LOG4CPLUS_HELPERS_FILEINFO_H

#include <log4cplus/config.h>
#include <log4cplus/timehelper.h>



namespace log4cplus { 


struct FileInfo
{
    TimeHelper mtime;
    bool is_link;
    off_t size;
};

int getFileInfo (FileInfo* fileinfo, string const& name);


}  // namespace log4cplus 

#endif // LOG4CPLUS_HELPERS_FILEINFO_H
