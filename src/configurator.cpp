// Module:  LOG4CPLUS
// File:    configurator.cpp


#include <log4cplus/configurator.h>
#include <log4cplus/hierarchy.h>
#include <log4cplus/helpers/loglog.h>
#include <log4cplus/helpers/sleep.h>
#include <log4cplus/helpers/stringhelper.h>
#include <log4cplus/helpers/property.h>
#include <log4cplus/helpers/timehelper.h>
#include <log4cplus/helpers/fileinfo.h>
#include <log4cplus/spi/factory.h>
#include <log4cplus/spi/loggerimpl.h>
#include <log4cplus/internal/env.h>

#if defined (_MSC_VER)
#include <tchar.h>
#endif

#include <iterator>



namespace log4cplus
{


void initializeLog4cplus();


//////////////////////////////////////////////////////////////////////////////
// PropertyConfigurator ctor and dtor
//////////////////////////////////////////////////////////////////////////////

PropertyConfigurator::PropertyConfigurator(const std::string& propertyFile, Hierarchy& hier)
    : _hierarchy(hier), _propertyFilename(propertyFile), _properties(propertyFile)
{
    init();
}


PropertyConfigurator::PropertyConfigurator(const helpers::Properties& props, Hierarchy& hier)
    : _hierarchy(hier), _propertyFilename("UNAVAILABLE"), _properties( props )
{
    init();
}


PropertyConfigurator::PropertyConfigurator(istream& propertyStream, Hierarchy& hier)
    : _hierarchy(hier), _propertyFilename("UNAVAILABLE"), _properties(propertyStream)
{
    init();
}


void PropertyConfigurator::init()
{
    _properties = _properties.getPropertySubset("log4cplus.");
}


PropertyConfigurator::~PropertyConfigurator()
{
}



//////////////////////////////////////////////////////////////////////////////
// PropertyConfigurator static methods
//////////////////////////////////////////////////////////////////////////////

void PropertyConfigurator::doConfigure(const std::string& file, Hierarchy& h)
{
    PropertyConfigurator tmp(file, h);
    tmp.configure();
}



//////////////////////////////////////////////////////////////////////////////
// PropertyConfigurator public methods
//////////////////////////////////////////////////////////////////////////////

void PropertyConfigurator::configure()
{
    // Configure log4cplus internals.
    bool internal_debugging = false;
    if (_properties.getBool (internal_debugging, "configDebug"))
        helpers::getLogLog().setInternalDebugging (internal_debugging);

    bool quiet_mode = false;
    if (_properties.getBool (quiet_mode, "quietMode"))
        helpers::getLogLog().setQuietMode (quiet_mode);

    bool disable_override = false;
    _properties.getBool (disable_override, "disableOverride");

    initializeLog4cplus();
    configureAppenders();
    configureLoggers();

    if (disable_override)
        _hierarchy.disable (DISABLE_LOG_OVERRIDE);

    // Erase the appenders so that we are not artificially keeping them "alive".
    _appenders.clear ();
}


helpers::Properties const& PropertyConfigurator::getProperties () const
{
    return _properties;
}


std::string const& PropertyConfigurator::getPropertyFilename () const
{
    return _propertyFilename;
}


//////////////////////////////////////////////////////////////////////////////
// PropertyConfigurator protected methods
//////////////////////////////////////////////////////////////////////////////

void PropertyConfigurator::configureLoggers()
{
    if(_properties.exists("rootLogger"))
    {
        Logger root = _hierarchy.getRoot();
        configureLogger(root, _properties.getProperty("rootLogger"));
    }

    helpers::Properties loggerProperties = _properties.getPropertySubset("logger.");
    std::vector<std::string> loggers = loggerProperties.propertyNames();

    for(std::vector<std::string>::iterator it=loggers.begin(); it!=loggers.end(); ++it)
    {
        Logger log = getLogger(*it);
        configureLogger(log, loggerProperties.getProperty(*it));
    }
}


void PropertyConfigurator::configureLogger(Logger logger, const std::string& config)
{
    // Remove all spaces from config
    std::string configString;
    std::remove_copy_if(config.begin(), config.end(), 
		std::back_inserter (configString), std::bind1st(std::equal_to<char>(), ' '));

    // "Tokenize" configString
    std::vector<std::string> tokens;
    helpers::tokenize(configString, ',', std::back_insert_iterator<std::vector<std::string> >(tokens));

    if (tokens.empty ())
    {
        helpers::getLogLog().error(
            "PropertyConfigurator::configureLogger()- Invalid config string Logger = "
            + logger.getName());
        return;
    }

    // Set the loglevel
    std::string const& loglevel = tokens[0];
    if (loglevel != "INHERITED")
        logger.setLogLevel( getLogLevelManager().fromString(loglevel));
    else
        logger.setLogLevel (NOT_SET_LOG_LEVEL);

    // Remove all existing appenders first so that we do not duplicate output.
    logger.removeAllAppenders ();

    // Set the Appenders
    for(std::vector<std::string>::size_type j=1; j<tokens.size(); ++j)
    {
        AppenderMap::iterator appenderIt = _appenders.find(tokens[j]);
        if (appenderIt == _appenders.end())
        {
            helpers::getLogLog().error(
                "PropertyConfigurator::configureLogger()- Invalid appender: " + tokens[j]);
            continue;
        }
        addAppender(logger, appenderIt->second);
    }
}



void PropertyConfigurator::configureAppenders()
{
    helpers::Properties appenderProperties = _properties.getPropertySubset("appender.");
    std::vector<std::string> appendersProps = appenderProperties.propertyNames();
    std::string factoryName;

    for(std::vector<std::string>::iterator it=appendersProps.begin(); it != appendersProps.end(); ++it)
    {
        if( it->find('.') == std::string::npos )
        {
            factoryName = appenderProperties.getProperty(*it);
            AppenderFactory* factory = getAppenderFactoryRegistry().get(factoryName);
            if (!factory)
            {
                std::string err = "PropertyConfigurator::configureAppenders()- Cannot find AppenderFactory: ";
                helpers::getLogLog().error(err + factoryName);
                continue;
            }

            helpers::Properties props_subset = appenderProperties.getPropertySubset((*it) + ".");
            try
            {
                SharedAppenderPtr appender = factory->createObject(props_subset);
                if (!appender)
                {
                    std::string err =
                        "PropertyConfigurator::configureAppenders() - Failed to create appender: ";
                    helpers::getLogLog().error(err + *it);
                }
                else
                {
                    appender->setName(*it);
                    _appenders[*it] = appender;
                }
            }
            catch(std::exception const& e)
            {
                std::string err =
                    "PropertyConfigurator::configureAppenders() - Error while creating Appender: ";
                helpers::getLogLog().error(err + std::string(e.what()));
            }
        }
    } // end for loop
}

Logger PropertyConfigurator::getLogger(const std::string& name)
{
    return _hierarchy.getInstance(name);
}

void PropertyConfigurator::addAppender(Logger &logger, SharedAppenderPtr& appender)
{
    logger.addAppender(appender);
}


//////////////////////////////////////////////////////////////////////////////
// BasicConfigurator ctor and dtor
//////////////////////////////////////////////////////////////////////////////


BasicConfigurator::BasicConfigurator(Hierarchy& hier, bool logToStdErr)
    : PropertyConfigurator("", hier )
{
    _properties.setProperty("rootLogger", "DEBUG, STDOUT");
    _properties.setProperty("appender.STDOUT", "log4cplus::ConsoleAppender");
    _properties.setProperty("appender.STDOUT.logToStdErr", logToStdErr ? "1" : "0");
}

BasicConfigurator::~BasicConfigurator()
{
}


//////////////////////////////////////////////////////////////////////////////
// BasicConfigurator static methods
//////////////////////////////////////////////////////////////////////////////

void BasicConfigurator::doConfigure(Hierarchy& h, bool logToStdErr)
{
    BasicConfigurator tmp(h, logToStdErr);
    tmp.configure();
}


} // namespace log4cplus
