// Module:  Log4CPLUS
// File:    appender.cpp


#include <log4cplus/appender.h>
#include <log4cplus/layout.h>
#include <log4cplus/helpers/loglog.h>
#include <log4cplus/helpers/pointer.h>
#include <log4cplus/helpers/stringhelper.h>
#include <log4cplus/helpers/property.h>
#include <log4cplus/factory.h>
#include <log4cplus/loggingevent.h>
#include <log4cplus/helpers/internal.h>
#include <stdexcept>


namespace log4cplus
{


///////////////////////////////////////////////////////////////////////////////
// log4cplus::ErrorHandler dtor
///////////////////////////////////////////////////////////////////////////////

ErrorHandler::ErrorHandler ()
{ }


ErrorHandler::~ErrorHandler()
{ }



///////////////////////////////////////////////////////////////////////////////
// log4cplus::OnlyOnceErrorHandler 
///////////////////////////////////////////////////////////////////////////////

OnlyOnceErrorHandler::OnlyOnceErrorHandler()
    : firstTime(true)
{ }


OnlyOnceErrorHandler::~OnlyOnceErrorHandler ()
{ }


void
OnlyOnceErrorHandler::error(const std::string& err)
{
    if(firstTime) 
	{
        helpers::getLogLog().error(err);
        firstTime = false;
    }
}



void
OnlyOnceErrorHandler::reset()
{
    firstTime = true;
}



///////////////////////////////////////////////////////////////////////////////
// log4cplus::Appender ctors
///////////////////////////////////////////////////////////////////////////////

Appender::Appender()
 : _layout(new SimpleLayout()),
   _name(""),
   _threshold(NOT_SET_LOG_LEVEL),
   _errorHandler(new OnlyOnceErrorHandler),
   _isClosed(false)
{
}



Appender::Appender(const log4cplus::helpers::Properties & properties)
    : _layout(new SimpleLayout())
    , _name()
    , _threshold(NOT_SET_LOG_LEVEL)
    , _errorHandler(new OnlyOnceErrorHandler)
    , _isClosed(false)
{
    if(properties.exists("layout"))
    {
        std::string const& factoryName = properties.getProperty("layout");
        LayoutFactory* factory = getLayoutFactoryRegistry().get(factoryName);
        if(factory == 0) {
            helpers::getLogLog().error("Cannot find LayoutFactory: " + factoryName );
            return;
        }

        helpers::Properties layoutProperties = properties.getPropertySubset("layout.");
        try {
            std::auto_ptr<Layout> newLayout(factory->createObject(layoutProperties));
            if(newLayout.get() == 0)
			{
                helpers::getLogLog().error("Failed to create appender: " + factoryName);
            }
            else 
			{
                _layout = newLayout;
            }
        }
        catch(std::exception const& e) {
            helpers::getLogLog().error("Error while creating Layout: " + std::string(e.what()));
            return;
        }

    }

    // Support for appender.Threshold in properties configuration file
    if(properties.exists("Threshold")) 
	{
        std::string tmp = properties.getProperty("Threshold");
        tmp = log4cplus::helpers::toUpper(tmp);
        _threshold = log4cplus::getLogLevelManager().fromString(tmp);
    }

    // Configure the filters
    helpers::Properties filterProps = properties.getPropertySubset("filters.");
    unsigned filterCount = 0;
    FilterPtr filterChain;
    std::string filterName;
    while (filterProps.exists(filterName = helpers::convertIntegerToString (++filterCount)))
    {
        std::string const& factoryName = filterProps.getProperty(filterName);
        FilterFactory* factory = getFilterFactoryRegistry().get(factoryName);

        if(!factory)
        {
            std::string err = "Appender::ctor()- Cannot find FilterFactory: ";
            helpers::getLogLog().error(err + factoryName);
            continue;
        }
        FilterPtr tmpFilter = factory->createObject(filterProps.getPropertySubset(filterName + "."));
        if (!tmpFilter)
        {
            std::string err = "Appender::ctor()- Failed to create filter: ";
            helpers::getLogLog().error(err + filterName);
        }
        if (!filterChain)
            filterChain = tmpFilter;
        else
            filterChain->appendFilter(tmpFilter);
    }
    setFilter(filterChain);
}


Appender::~Appender()
{
    helpers::LogLog & loglog = helpers::getLogLog();

    loglog.debug("Destroying appender named [" + _name + "].");

    if (!_isClosed)
        loglog.error("Derived Appender did not call destructorImpl().");
}



///////////////////////////////////////////////////////////////////////////////
// log4cplus::Appender public methods
///////////////////////////////////////////////////////////////////////////////

void Appender::destructorImpl()
{
    // An appender might be closed then destroyed. There is no point
    // in closing twice. It can actually be a wrong thing to do, e.g.,
    // files get rolled more than once.
    if (_isClosed)
        return;

    close();
    _isClosed = true;
}


bool Appender::isClosed() const
{
    return _isClosed;
}


void
Appender::doAppend(const log4cplus::InternalLoggingEvent& loggingEvent)
{
	Mutex::ScopedLock lock(_access_mutex);

    if(_isClosed) {
        helpers::getLogLog().error("Attempted to append to closed appender named [" + _name + "].");
        return;
    }

    // Check appender's threshold logging level.

    if (!isAsSevereAsThreshold(loggingEvent.getLogLevel()))
        return;

    // Evaluate filters attached to this appender.

    if (checkFilter(_filter.get(), loggingEvent) == DENY)
        return;
    // Finally append given loggingEvent.

    append(loggingEvent);
}


std::string Appender::getName()
{
    return _name;
}



void Appender::setName(const std::string& n)
{
    this->_name = n;
}


ErrorHandler* Appender::getErrorHandler()
{
    return _errorHandler.get();
}



void Appender::setErrorHandler(std::auto_ptr<ErrorHandler> eh)
{
    if (!eh.get())
    {
        // We do not throw exception here since the cause is probably a
        // bad config file.
        helpers::getLogLog().warn("You have tried to set a null error-handler.");
        return;
    }

    Mutex::ScopedLock lock(_access_mutex);

    this->_errorHandler = eh;
}



void Appender::setLayout(std::auto_ptr<Layout> lo)
{
   Mutex::ScopedLock lock(_access_mutex);

    this->_layout = lo;
}



Layout* Appender::getLayout()
{
    return _layout.get();
}


} // namespace log4cplus
