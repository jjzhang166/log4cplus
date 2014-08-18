// -*- C++ -*-
// Module:  Log4CPLUS
// File:    hierarchy.h


/** @file */

#ifndef LOG4CPLUS_HIERARCHY_HEADER_
#define LOG4CPLUS_HIERARCHY_HEADER_

#include <log4cplus/config.h>
#include <log4cplus/thread/Mutex.h>

#include <log4cplus/logger.h>
#include <map>
#include <memory>
#include <vector>


namespace log4cplus {
    // Forward Declarations
    class HierarchyLocker;

    /**
     * This class is specialized in retrieving loggers by name and
     * also maintaining the logger hierarchy.
     *
     * <em>The casual user should not have to deal with this class
     * directly.</em>  However, if you are in an environment where
     * multiple applications run in the same process, then read on.
     *
     * The structure of the logger hierarchy is maintained by the
     * {@link #getInstance} method. The hierarchy is such that children
     * link to their parent but parents do not have any pointers to their
     * children. Moreover, loggers can be instantiated in any order, in
     * particular descendant before ancestor.
     *
     * In case a descendant is created before a particular ancestor,
     * then it creates a provision node for the ancestor and adds itself
     * to the provision node. Other descendants of the same ancestor add
     * themselves to the previously created provision node.
     */
    class LOG4CPLUS_EXPORT Hierarchy
    {
    public:

      // Ctors
        /**
         * Create a new Logger hierarchy.
         */
        Hierarchy();

      // Dtor
        virtual ~Hierarchy();

      // Methods
        /**
         * This call will clear all logger definitions from the internal
         * hashtable. Invoking this method will irrevocably mess up the
         * logger hierarchy.
         *                     
         * You should <em>really</em> know what you are doing before
         * invoking this method.
         */
        virtual void clear();

        /**
         * Returns <code>true </code>if the named logger exists 
         * (in the default hierarchy).
         *                
         * @param name The name of the logger to search for.
         */
        virtual bool exists(const std::string& name);

        /**
         * Similar to {@link #disable(LogLevel)} except that the LogLevel
         * argument is given as a std::string.  
         */
        virtual void disable(const std::string& loglevelStr);

        /**
         * Disable all logging requests of LogLevel <em>equal to or
         * below</em> the ll parameter <code>p</code>, for
         * <em>all</em> loggers in this hierarchy. Logging requests of
         * higher LogLevel then <code>p</code> remain unaffected.
         *
         * Nevertheless, if the
         * BasicConfigurator::DISABLE_OVERRIDE_KEY property is set to
         * true, then logging requests are evaluated as usual.
         *
         * The "disable" family of methods are there for speed. They
         * allow printing methods such as debug, info, etc. to return
         * immediately after an integer comparison without walking the
         * logger hierarchy. In most modern computers an integer
         * comparison is measured in nanoseconds where as a logger walk is
         * measured in units of microseconds.
         */
        virtual void disable(LogLevel ll);

        /**
         * Disable all logging requests regardless of logger and LogLevel.
         * This method is equivalent to calling {@link #disable} with the
         * argument FATAL_LOG_LEVEL, the highest possible LogLevel.
         */
        virtual void disableAll();

        /**
         * Disable all Debug logging requests regardless of logger.
         * This method is equivalent to calling {@link #disable} with the
         * argument DEBUG_LOG_LEVEL.
         */
        virtual void disableDebug();

        /**
         * Disable all Info logging requests regardless of logger.
         * This method is equivalent to calling {@link #disable} with the
         * argument INFO_LOG_LEVEL.
         */
        virtual void disableInfo();

        /**
         * Undoes the effect of calling any of {@link #disable}, {@link
         * #disableAll}, {@link #disableDebug} and {@link #disableInfo}
         * methods. More precisely, invoking this method sets the Logger
         * class internal variable called <code>disable</code> to its
         * default "off" value.
         */
        virtual void enableAll();

        /**
         * Return a new logger instance named as the first parameter using
         * the default factory. 
         *                
         * If a logger of that name already exists, then it will be
         * returned.  Otherwise, a new logger will be instantiated and
         * then linked with its existing ancestors as well as children.
         *                                    
         * @param name The name of the logger to retrieve.
         */
        virtual Logger getInstance(const std::string& name);

        /**
         * Return a new logger instance named as the first parameter using
         * <code>factory</code>.
         *                
         * If a logger of that name already exists, then it will be
         * returned.  Otherwise, a new logger will be instantiated by the
         * <code>factory</code> parameter and linked with its existing
         * ancestors as well as children.
         *                                         
         * @param name The name of the logger to retrieve.
         * @param factory The factory that will make the new logger instance.
         */
        virtual Logger getInstance(const std::string& name, LoggerFactory& factory);

        /**
         * Returns all the currently defined loggers in this hierarchy.
         *
         * The root logger is <em>not</em> included in the returned list. 
         */
        virtual LoggerList getCurrentLoggers();

        /** 
         * Is the LogLevel specified by <code>level</code> enabled? 
         */
        virtual bool isDisabled(LogLevel level);

        /**
         * Get the root of this hierarchy.
         */
        virtual Logger getRoot() const;

        /**
         * Reset all values contained in this hierarchy instance to their
         * default.  This removes all appenders from all loggers, sets
         * the LogLevel of all non-root loggers to <code>NOT_SET_LOG_LEVEL</code>,
         * sets their additivity flag to <code>true</code> and sets the LogLevel
         * of the root logger to DEBUG_LOG_LEVEL.  Moreover, message disabling
         * is set its default "off" value.
         *
         * Existing loggers are not removed. They are just reset.
         *
         * This method should be used sparingly and with care as it will
         * block all logging until it is completed.</p>
         */
        virtual void resetConfiguration(); 

        /**
         * Set the default LoggerFactory instance.
         */
        virtual void setLoggerFactory(std::auto_ptr<LoggerFactory> factory);
        
        /**
         * Returns the default LoggerFactory instance.
         */
        virtual LoggerFactory* getLoggerFactory();

    private:
      // Types
        typedef std::vector<Logger> ProvisionNode;
        typedef std::map<std::string, ProvisionNode> ProvisionNodeMap;
        typedef std::map<std::string, Logger> LoggerMap;

      // Methods
        /**
         * This is the implementation of the <code>getInstance()</code> method.
         * NOTE: This method does not lock the <code>hashtable_mutex</code>.
         */
        
        virtual Logger getInstanceImpl(const std::string& name, 
            LoggerFactory& factory);
        
        /**
         * This is the implementation of the <code>getCurrentLoggers()</code>.
         * NOTE: This method does not lock the <code>hashtable_mutex</code>.
         */
        
        virtual void initializeLoggerList(LoggerList& list) const;
        
        /**
         * This method loops through all the *potential* parents of
         * logger'. There 3 possible cases:
         *
         * 1) No entry for the potential parent of 'logger' exists
         *
         *    We create a ProvisionNode for this potential parent and insert
         *    'logger' in that provision node.
         *
         * 2) There is an entry of type Logger for the potential parent.
         *
         *    The entry is 'logger's nearest existing parent. We update logger's
         *    parent field with this entry. We also break from the loop
         *    because updating our parent's parent is our parent's
         *    responsibility.
         *
         * 3) There entry is of type ProvisionNode for this potential parent.
         *
         *    We add 'logger' to the list of children for this potential parent.
         */
        void updateParents(Logger const& logger);

        /**
         * We update the links for all the children that placed themselves
         * in the provision node 'pn'. The second argument 'logger' is a
         * reference for the newly created Logger, parent of all the
         * children in 'pn'
         *
         * We loop on all the children 'c' in 'pn':
         *
         *    If the child 'c' has been already linked to a child of
         *    'logger' then there is no need to update 'c'.
         *
         *   Otherwise, we set logger's parent field to c's parent and set
         *   c's parent field to logger.
         */
        void updateChildren(ProvisionNode& pn,
            Logger const& logger);

     // Data
        Mutex _hashtable_mutex;
        std::auto_ptr<LoggerFactory> defaultFactory;
        ProvisionNodeMap provisionNodes;
        LoggerMap loggerPtrs;
        Logger root;

        int disableValue;

        bool emittedNoAppenderWarning;

        // Disallow copying of instances of this class
        Hierarchy(const Hierarchy&);
        Hierarchy& operator= (const Hierarchy&);

     // Friends
        friend class log4cplus::LoggerImpl;
        friend class log4cplus::HierarchyLocker;
    };


    LOG4CPLUS_EXPORT Hierarchy & getDefaultHierarchy ();


} // end namespace log4cplus

#endif // LOG4CPLUS_HIERARCHY_HEADER_

