// Module:  Log4CPLUS
// File:    pointer.cpp



#include <log4cplus/helpers/pointer.h>
#include <log4cplus/windowsh-inc.h>
#include <cassert>


namespace log4cplus { namespace helpers {


///////////////////////////////////////////////////////////////////////////////
// log4cplus::helpers::SharedObject dtor
///////////////////////////////////////////////////////////////////////////////

SharedObject::~SharedObject()
{
    assert(_count == 0);
}



///////////////////////////////////////////////////////////////////////////////
// log4cplus::helpers::SharedObject public methods
///////////////////////////////////////////////////////////////////////////////

void SharedObject::addReference() const
{

#if defined (_MSC_VER) 
    _InterlockedIncrement (&_count);

#else
    Mutex::ScopedLock lock(const_cast<Mutex&>(_access_mutex));
    ++_count;

#endif
}


void SharedObject::removeReference() const
{
    assert (_count > 0);
    bool isDestroy;

#if defined (_MSC_VER) 
    isDestroy = _InterlockedDecrement (&_count) == 0;

#else
    {
        Mutex::ScopedLock lock(const_cast<Mutex&>(_access_mutex));
        isDestroy = --_count == 0;
    }

#endif
    if (isDestroy)
        delete this;
}


} } // namespace log4cplus { namespace helpers
