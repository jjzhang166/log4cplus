// Module:  Log4CPLUS
// File:    appenderattachableimpl.cpp



#include <log4cplus/appender.h>
#include <log4cplus/helpers/appenderattachableimpl.h>
#include <log4cplus/helpers/loglog.h>
#include <log4cplus/spi/loggingevent.h>

#include <algorithm>


namespace log4cplus
{


AppenderAttachable::~AppenderAttachable()
{ }




namespace helpers
{


//////////////////////////////////////////////////////////////////////////////
// log4cplus::helpers::AppenderAttachableImpl ctor and dtor
//////////////////////////////////////////////////////////////////////////////

AppenderAttachableImpl::AppenderAttachableImpl()
{ }


AppenderAttachableImpl::~AppenderAttachableImpl()
{ }



///////////////////////////////////////////////////////////////////////////////
// log4cplus::helpers::AppenderAttachableImpl public methods
///////////////////////////////////////////////////////////////////////////////

void AppenderAttachableImpl::addAppender(SharedAppenderPtr newAppender)
{
    if(newAppender == NULL) 
	{
        getLogLog().warn("Tried to add NULL appender");
        return;
    }

   Mutex::ScopedLock lock(appender_list_mutex);

    ListType::iterator it = std::find(appenderList.begin(), appenderList.end(), newAppender);
    if(it == appenderList.end()) 
	{
        appenderList.push_back(newAppender);
    }
}



AppenderAttachableImpl::ListType AppenderAttachableImpl::getAllAppenders()
{
    Mutex::ScopedLock lock(appender_list_mutex);
    
    return appenderList;
}



SharedAppenderPtr AppenderAttachableImpl::getAppender(const std::string& name)
{
    Mutex::ScopedLock lock(appender_list_mutex);

    for(ListType::iterator it=appenderList.begin(); it!=appenderList.end(); ++it)
    {
        if((*it)->getName() == name) {
            return *it;
        }
    }

    return SharedAppenderPtr(NULL);
}



void AppenderAttachableImpl::removeAllAppenders()
{
   Mutex::ScopedLock lock(appender_list_mutex);

    appenderList.erase(appenderList.begin(), appenderList.end());
}



void AppenderAttachableImpl::removeAppender(SharedAppenderPtr appender)
{
    if(appender == NULL) {
        getLogLog().warn("Tried to remove NULL appender");
        return;
    }

    Mutex::ScopedLock lock(appender_list_mutex);

    ListType::iterator it =
        std::find(appenderList.begin(), appenderList.end(), appender);
    if(it != appenderList.end()) 
	{
        appenderList.erase(it);
    }
}



void AppenderAttachableImpl::removeAppender(const std::string& name)
{
    removeAppender(getAppender(name));
}



int AppenderAttachableImpl::appendLoopOnAppenders(const InternalLoggingEvent& loggingEvent) const
{
    int count = 0;

   Mutex::ScopedLock lock(const_cast<Mutex&>(appender_list_mutex));

    for(ListType::const_iterator it=appenderList.begin(); it!=appenderList.end(); ++it)
    {
        ++count;
       (*it)->doAppend(loggingEvent);
    }

    return count;
}


} // namespace helpers


} // namespace log4cplus
