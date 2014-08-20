// -*- C++ -*-


#include <log4cplus/config.h>
#include <log4cplus/fileinfo.h>
#include <sys/stat.h>

#if defined(_MSC_VER)
#include <tchar.h>
#endif


using namespace log4cplus;


int log4cplus::getFileInfo(FileInfo* fileinfo, string const& name)
{
#if defined(_MSC_VER)
    struct _stat fileStatus;
    if(_tstat(name.c_str(), &fileStatus) == -1)
        return -1;
    
    fileinfo->mtime = TimeHelper(fileStatus.st_mtime);
    fileinfo->is_link = false;
    fileinfo->size = fileStatus.st_size;
    
#else
    struct stat fileStatus;
    if(stat(name.c_str(), &fileStatus) == -1)
        return -1;

    fileinfo->mtime = TimeHelper(fileStatus.st_mtime);
    fileinfo->is_link = S_ISLNK(fileStatus.st_mode);
    fileinfo->size = fileStatus.st_size;
    
#endif
    
    return 0;
}

