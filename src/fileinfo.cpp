// -*- C++ -*-


#include <log4cplus/config.h>
#include <log4cplus/helpers/fileinfo.h>

#include <sys/types.h>
#include <sys/stat.h>

#if defined(_MSC_VER)
#include <tchar.h>
#endif


namespace log4cplus { namespace helpers {


int getFileInfo(FileInfo * fi, string const& name)
{
#if defined(_MSC_VER)
    struct _stat fileStatus;
    if(_tstat(name.c_str(), &fileStatus) == -1)
        return -1;
    
    fi->mtime = helpers::TimeHelper(fileStatus.st_mtime);
    fi->is_link = false;
    fi->size = fileStatus.st_size;
    
#else
    struct stat fileStatus;
    if(stat(name.c_str(), &fileStatus) == -1)
        return -1;

    fi->mtime = helpers::TimeHelper(fileStatus.st_mtime);
    fi->is_link = S_ISLNK(fileStatus.st_mode);
    fi->size = fileStatus.st_size;
    
#endif
    
    return 0;
}

} } // namespace log4cplus { namespace helpers {
