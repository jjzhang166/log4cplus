// -*- C++ -*-
// Module:  Log4CPLUS
// File:    factory.h


/** @file */

#ifndef LOG4CPLUS_SPI_FACTORY_HEADER_
#define LOG4CPLUS_SPI_FACTORY_HEADER_

#include <log4cplus/config.h>

#include <log4cplus/appender.h>
#include <log4cplus/layout.h>

#include <log4cplus/spi/filter.h>
#include <log4cplus/spi/objectregistry.h>
#include <memory>
#include <vector>
#include <locale>


namespace log4cplus {

        /**
         * This is the base class for all factories.
         */
        class LOG4CPLUS_EXPORT BaseFactory 
		{
        public:
            virtual ~BaseFactory() = 0;

            /**
             * Returns the typename of the objects this factory creates.
             */
            virtual std::string const& getTypeName() const = 0;
        };


        /**
         * This abstract class defines the "Factory" interface to create "Appender"
         * objects.
         */
        class LOG4CPLUS_EXPORT AppenderFactory : public BaseFactory 
		{
        public:
            typedef Appender ProductType;
            typedef SharedAppenderPtr ProductPtr;

            AppenderFactory();
            virtual ~AppenderFactory() = 0;

            /**
             * Create an "Appender" object.
             */
            virtual SharedAppenderPtr createObject(const log4cplus::helpers::Properties& props) = 0;
        };



        /**
         * This abstract class defines the "Factory" interface to create "Layout"
         * objects.
         */
        class LOG4CPLUS_EXPORT LayoutFactory : public BaseFactory {
        public:
            typedef Layout ProductType;
            typedef std::auto_ptr<Layout> ProductPtr;

            LayoutFactory();
            virtual ~LayoutFactory() = 0;

            /**
             * Create a "Layout" object.
             */
            virtual std::auto_ptr<Layout> createObject(const log4cplus::helpers::Properties& props) = 0;
        };



        /**
         * This abstract class defines the "Factory" interface to create "Appender"
         * objects.
         */
        class LOG4CPLUS_EXPORT FilterFactory : public BaseFactory 
		{
        public:
            typedef Filter ProductType;
            typedef FilterPtr ProductPtr;

            FilterFactory();
            virtual ~FilterFactory() = 0;

            /**
             * Create a "Filter" object.
             */
            virtual FilterPtr createObject(const log4cplus::helpers::Properties& props) = 0;
        };

        /**
         * This template class is used as a "Factory Registry".  Objects are
         * "entered" into the registry with a "name" using the 
         * <code>put()</code> method.  (The registry then owns the object.)  
         * These object can then be retrieved using the <code>get()</code> 
         * method.
         * 
         * <b>Note:</b>  This class is Thread-safe.
         */
        template<class T>
        class LOG4CPLUS_EXPORT FactoryRegistry : public ObjectRegistryBase
        {
        public:
            typedef T product_type;

            virtual ~FactoryRegistry() 
			{
                clear();
            }

          // public methods
            /**
             * Used to enter an object into the registry.  (The registry now
             *  owns <code>object</code>.)
             */
            bool put(std::auto_ptr<T> object) 
			{
                 bool putValResult = putVal(object->getTypeName(), object.get());
                 object.release();
                 return putValResult; 
            }

            /**
             * Used to retrieve an object from the registry.  (The registry
             * owns the returned pointer.)
             */
            T* get(const std::string& name) const 
			{
                return static_cast<T*>(getVal(name));
            }

        protected:
            virtual void deleteObject(void *object) const 
			{
                delete static_cast<T*>(object);
            }
        };


        typedef FactoryRegistry<AppenderFactory> AppenderFactoryRegistry;
        typedef FactoryRegistry<LayoutFactory> LayoutFactoryRegistry;
        typedef FactoryRegistry<FilterFactory> FilterFactoryRegistry;


        /**
         * Returns the "singleton" <code>AppenderFactoryRegistry</code>.
         */
        LOG4CPLUS_EXPORT AppenderFactoryRegistry& getAppenderFactoryRegistry();

        /**
         * Returns the "singleton" <code>LayoutFactoryRegistry</code>.
         */
        LOG4CPLUS_EXPORT LayoutFactoryRegistry& getLayoutFactoryRegistry();

        /**
         * Returns the "singleton" <code>FilterFactoryRegistry</code>.
         */
        LOG4CPLUS_EXPORT FilterFactoryRegistry& getFilterFactoryRegistry();

        template <typename ProductFactoryBase>
        class LocalFactoryBase : public ProductFactoryBase
        {
        public:
            LocalFactoryBase (char const * n) : _name (n)
            { }

            virtual std::string const& getTypeName() const
            {
                return _name;
            }

        private:
            std::string _name;
        };


        template <typename LocalProduct, typename ProductFactoryBase>
        class FactoryTempl : public LocalFactoryBase<ProductFactoryBase>
        {
        public:
            typedef typename ProductFactoryBase::ProductPtr ProductPtr;

            FactoryTempl (char const * n)
                : LocalFactoryBase<ProductFactoryBase> (n)
            { }

            virtual ProductPtr createObject(helpers::Properties const& props)
            {
                return ProductPtr (new LocalProduct (props));
            }
        };


        #define LOG4CPLUS_REG_PRODUCT(reg, productprefix, productname, productns, productfact) \
        reg.put (																               \
            std::auto_ptr<productfact> (                                                       \
                    new log4cplus::FactoryTempl<productns productname, productfact> (     \
                    productprefix                                            \
                    #productname)))

        #define LOG4CPLUS_REG_APPENDER(reg, appendername)                             \
        LOG4CPLUS_REG_PRODUCT (reg, "log4cplus::", appendername, log4cplus::,         \
            log4cplus::AppenderFactory) 

        #define LOG4CPLUS_REG_LAYOUT(reg, layoutname)                                 \
        LOG4CPLUS_REG_PRODUCT (reg, "log4cplus::", layoutname, log4cplus::,           \
            log4cplus::LayoutFactory)

        #define LOG4CPLUS_REG_FILTER(reg, filtername)                                 \
        LOG4CPLUS_REG_PRODUCT (reg, "log4cplus::", filtername, log4cplus::, \
            log4cplus::FilterFactory)
}


#endif // LOG4CPLUS_SPI_FACTORY_HEADER_
