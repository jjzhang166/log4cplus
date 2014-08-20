// -*- C++ -*-


#ifndef LOG4CPLUS_THREAD_IMPL_TLS_H
#define LOG4CPLUS_THREAD_IMPL_TLS_H

#include <log4cplus/config.h>

#include <new>
#include <cassert>

#ifdef __linux__
#include <pthread.h>

#elif defined(_MSC_VER)


#endif


namespace log4cplus { namespace Thread {


typedef void * tls_value_type;
typedef void(* tls_init_cleanup_func_type)(void *);

#ifdef __linux__
typedef pthread_key_t * tls_key_type;

#elif defined(_MSC_VER)
typedef DWORD tls_key_type;


#endif


inline tls_key_type tls_init(tls_init_cleanup_func_type);
inline tls_value_type tls_get_value(tls_key_type);
inline void tls_set_value(tls_key_type, tls_value_type);
inline void tls_cleanup(tls_key_type);


#if defined(__linux__)
tls_key_type tls_init(tls_init_cleanup_func_type cleanupfunc)
{
    pthread_key_t * key = new pthread_key_t;
    pthread_key_create(key, cleanupfunc);
    return key;
}

tls_value_type tls_get_value(tls_key_type key)
{
    return pthread_getspecific(*key);
}

void tls_set_value(tls_key_type key, tls_value_type value)
{
    pthread_setspecific(*key, value);
}

void tls_cleanup(tls_key_type key)
{
    pthread_key_delete(*key);
    delete key;
}

#elif defined(_MSC_VER)

tls_key_type tls_init(tls_init_cleanup_func_type)
{
    return TlsAlloc();
}

tls_value_type tls_get_value(tls_key_type k)
{
    return TlsGetValue(k);
}

void tls_set_value(tls_key_type k, tls_value_type value)
{
    TlsSetValue(k, value);
}

void tls_cleanup(tls_key_type k)
{
    TlsFree(k);
}

#endif

} } // namespace log4cplus { namespace thread { 

#endif // LOG4CPLUS_THREAD_IMPL_TLS_H
