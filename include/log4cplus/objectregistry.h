// -*- C++ -*-
// Module:  Log4CPLUS
// File:    objectregistry.h


/** @file */

#ifndef LOG4CPLUS_SPI_OBJECT_REGISTRY_HEADER_
#define LOG4CPLUS_SPI_OBJECT_REGISTRY_HEADER_

#include <log4cplus/config.h>


#include <log4cplus/thread/Mutex.h>

#include <map>
#include <memory>
#include <vector>


namespace log4cplus {

        /**
         * This is the base class used to implement the functionality required
         * by the ObjectRegistry template class.
         */
        class LOG4CPLUS_EXPORT ObjectRegistryBase {
        public:
          // public methods
            /**
             * Tests to see whether or not an object is bound in the
             * registry as <code>name</code>.
             */
            bool exists(const string& name) const;

            /**
             * Returns the names of all registered objects.
             */
            vector<string> getAllNames() const;

        protected:
          // Ctor and Dtor
            ObjectRegistryBase();
            virtual ~ObjectRegistryBase();

          // protected methods
            /**
             * Used to enter an object into the registry.  (The registry now
             * owns <code>object</code>.)
             */
            bool putVal(const string& name, void* object);

            /**
             * Used to retrieve an object from the registry.  (The registry
             * owns the returned pointer.)
             */
            void* getVal(const string& name) const;

            /**
             * Deletes <code>object</code>.
             */
            virtual void deleteObject(void *object) const = 0;

            /**
             * Deletes all objects from this registry.
             */
            virtual void clear();

          // Types
            typedef std::map<string, void*> ObjectMap;

          // Data
            Mutex _mutex;
            ObjectMap _ObjectMap;

        private:
            ObjectRegistryBase (ObjectRegistryBase const&);
            ObjectRegistryBase & operator = (ObjectRegistryBase const&);
        };


}


#endif // LOG4CPLUS_SPI_OBJECT_REGISTRY_HEADER_
