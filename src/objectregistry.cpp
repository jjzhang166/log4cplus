// Module:  Log4CPLUS
// File:    objectregistry.cpp


#include <log4cplus/objectregistry.h>


namespace log4cplus { 


///////////////////////////////////////////////////////////////////////////////
// ObjectRegistryBase ctor and dtor
///////////////////////////////////////////////////////////////////////////////

ObjectRegistryBase::ObjectRegistryBase()
{ }


ObjectRegistryBase::~ObjectRegistryBase()
{ }



///////////////////////////////////////////////////////////////////////////////
// ObjectRegistryBase public methods
///////////////////////////////////////////////////////////////////////////////

bool ObjectRegistryBase::exists(const string& name) const
{
     Mutex::ScopedLock lock(const_cast<Mutex&>(_mutex));

    return _ObjectMap.find(name) != _ObjectMap.end();
}


vector<string> ObjectRegistryBase::getAllNames() const
{
    vector<string> tmp;

    {
         Mutex::ScopedLock lock(const_cast<Mutex&>(_mutex));
        for(ObjectMap::const_iterator it=_ObjectMap.begin(); it!=_ObjectMap.end(); ++it)
            tmp.push_back( (*it).first );
    }

    return tmp;
}



///////////////////////////////////////////////////////////////////////////////
// ObjectRegistryBase protected methods
///////////////////////////////////////////////////////////////////////////////

bool ObjectRegistryBase::putVal(const string& name, void* object)
{
    ObjectMap::value_type v(name, object);
    std::pair<ObjectMap::iterator, bool> ret;

    {
        Mutex::ScopedLock lock(_mutex);

        ret = _ObjectMap.insert(v);
    }

    if (!ret.second)
        deleteObject(v.second);
    return ret.second;
}


void* ObjectRegistryBase::getVal(const string& name) const
{
     Mutex::ScopedLock lock(const_cast<Mutex&>(_mutex));

    ObjectMap::const_iterator it (_ObjectMap.find (name));
    if (it != _ObjectMap.end ())
        return it->second;
    else
        return 0;
}




void ObjectRegistryBase::clear()
{
     Mutex::ScopedLock lock(_mutex);

    for(ObjectMap::iterator it=_ObjectMap.begin(); it!=_ObjectMap.end(); ++it)
        deleteObject( it->second );
}

} // namespace log4cplus { 
