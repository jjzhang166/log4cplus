//
// Mutex.cpp
//


#include "log4cplus/thread/Mutex.h"

#ifdef _MSC_VER
#include "Mutex_WIN32.cpp"
#else
#include "Mutex_LINUX.cpp"
#endif


namespace log4cplus {


Mutex::Mutex()
{
}

Mutex::~Mutex()
{
}

} // namespace log4cplus
