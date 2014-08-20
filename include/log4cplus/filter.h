// -*- C++ -*-
// Module:  Log4CPLUS
// File:    filter.h


/** @file 
* This header defines Filter and all of it's subclasses. */

#ifndef LOG4CPLUS_SPI_FILTER_HEADER_
#define LOG4CPLUS_SPI_FILTER_HEADER_

#include <log4cplus/config.h>

#include <log4cplus/pointer.h>
#include <log4cplus/loglevel.h>


namespace log4cplus {

	class Properties;
	enum FilterResult 
	{ 
		DENY, /**< The log loggingEvent must be dropped immediately 
			  *  without consulting with the remaining 
			  *  filters, if any, in the chain. */
			  NEUTRAL, /**< This filter is neutral with respect to
					   *  the log loggingEvent; the remaining filters, if 
					   *  if any, should be consulted for a final 
					   *  decision. */
					   ACCEPT /**< The log loggingEvent must be logged immediately 
							  *  without consulting with the remaining 
							  *  filters, if any, in the chain. */
	};

	// Forward Declarations
	class Filter;
	class InternalLoggingEvent;


	/**
	* This method is used to filter an InternalLoggingEvent.
	*
	* Note: <code>filter</code> can be NULL.
	*/
	LOG4CPLUS_EXPORT FilterResult checkFilter(const Filter* filter,  const InternalLoggingEvent& loggingEvent);

	typedef SharedObjectPtr<Filter> FilterPtr;


	/**
	* Users should extend this class to implement customized logging
	* loggingEvent filtering. Note that the {@link Logger} and {@link
	* Appender} classes have built-in filtering rules. It is suggested 
	* that you first use and understand the built-in rules before rushing
	* to write your own custom filters.
	*
	* This abstract class assumes and also imposes that filters be
	* organized in a linear chain. The {@link #decide
	* decide(LoggingEvent)} method of each filter is called sequentially,
	* in the order of their addition to the chain.
	*
	* If the value {@link #DENY} is returned, then the log loggingEvent is
	* dropped immediately without consulting with the remaining
	* filters. 
	*
	* If the value {@link #NEUTRAL} is returned, then the next filter
	* in the chain is consulted. If there are no more filters in the
	* chain, then the log loggingEvent is logged. Thus, in the presence of no
	* filters, the default behaviour is to log all logging events.
	*
	* If the value {@link #ACCEPT} is returned, then the log
	* loggingEvent is logged without consulting the remaining filters. 
	*
	* The philosophy of log4cplus filters is largely inspired from the
	* Linux ipchains. 
	*/
	class LOG4CPLUS_EXPORT Filter : public virtual SharedObject 
	{
	public:
		// ctor and dtor
		Filter();
		virtual ~Filter();

		// Methods
		/**
		* Appends <code>filter</code> to the end of this filter chain.
		*/
		void appendFilter(FilterPtr filter);

		/**     
		* If the decision is <code>DENY</code>, then the loggingEvent will be
		* dropped. If the decision is <code>NEUTRAL</code>, then the next
		* filter, if any, will be invoked. If the decision is ACCEPT then
		* the loggingEvent will be logged without consulting with other filters in
		* the chain.
		*
		* @param loggingEvent The LoggingEvent to decide upon.
		* @return The decision of the filter.  
		*/
		virtual FilterResult decide(const InternalLoggingEvent& loggingEvent) const = 0;

		// Data
		/**
		* Points to the next filter in the filter chain.
		*/
		FilterPtr _nextFilter;
	};



	/**
	* This filter drops all logging events.
	*
	* You can add this filter to the end of a filter chain to
	* switch from the default "accept all unless instructed otherwise"
	* filtering behaviour to a "deny all unless instructed otherwise"
	* behaviour.
	*/
	class LOG4CPLUS_EXPORT DenyAllFilter : public Filter 
	{
	public:
		DenyAllFilter ();
		DenyAllFilter (const Properties&);

		/**
		* Always returns the {@link #DENY} regardless of the 
		* {@link InternalLoggingEvent} parameter.
		*/
		virtual FilterResult decide(const InternalLoggingEvent& loggingEvent) const;
	};


	/**
	* This is a very simple filter based on LogLevel matching.
	*
	* The filter admits two options <b>LogLevelToMatch</b> and
	* <b>AcceptOnMatch</b>. If there is an exact match between the value
	* of the LogLevelToMatch option and the LogLevel of the {@link
	* InternalLoggingEvent}, then the {@link #decide} method returns 
	* {@link #ACCEPT} in case the <b>AcceptOnMatch</b> option value is set
	* to <code>true</code>, if it is <code>false</code> then {@link #DENY}
	* is returned. If there is no match, {@link #NEUTRAL} is returned.
	*/
	class LOG4CPLUS_EXPORT LogLevelMatchFilter : public Filter 
	{
	public:
		LogLevelMatchFilter();
		LogLevelMatchFilter(const Properties& p);

		/**
		* Return the decision of this filter.
		*
		* Returns {@link #NEUTRAL} if the <b>LogLevelToMatch</b>
		* option is not set or if there is no match.  Otherwise, if
		* there is a match, then the returned decision is {@link #ACCEPT}
		* if the <b>AcceptOnMatch</b> property is set to <code>true</code>. 
		* The returned decision is {@link #DENY} if the <b>AcceptOnMatch</b>
		* property is set to <code>false</code>.
		*/
		virtual FilterResult decide(const InternalLoggingEvent& loggingEvent) const;

	private:
		// Methods
		void init();

		LogLevel _logLevelToMatch;
	};



	/**
	* This is a very simple filter based on LogLevel matching, which can be
	* used to reject messages with LogLevels outside a certain range.
	*
	* The filter admits three options <b>LogLevelMin</b>, <b>LogLevelMax</b>
	* and <b>AcceptOnMatch</b>.
	*
	* If the LogLevel of the Logging loggingEvent is not between Min and Max
	* (inclusive), then {@link #DENY} is returned.
	*
	* If the Logging loggingEvent LogLevel is within the specified range, then if
	* <b>AcceptOnMatch</b> is true, {@link #ACCEPT} is returned, and if
	* <b>AcceptOnMatch</b> is false, {@link #NEUTRAL} is returned.
	*
	* If <code>LogLevelMin</code> is not defined, then there is no
	* minimum acceptable LogLevel (ie a LogLevel is never rejected for
	* being too "low"/unimportant).  If <code>LogLevelMax</code> is not
	* defined, then there is no maximum acceptable LogLevel (ie a
	* LogLevel is never rejected for beeing too "high"/important).
	*
	* Refer to the {@link
	* Appender#setThreshold setThreshold} method
	* available to <code>all</code> appenders for a more convenient way to
	* filter out events by LogLevel.
	*/
	class LOG4CPLUS_EXPORT LogLevelRangeFilter : public Filter 
	{
	public:
		// ctors
		LogLevelRangeFilter();
		LogLevelRangeFilter(const Properties& p);

		/**
		* Return the decision of this filter.
		*/
		virtual FilterResult decide(const InternalLoggingEvent& loggingEvent) const;

	private:
		// Methods
		void init();

		LogLevel _logLevelMin;
		LogLevel _logLevelMax;
	};
} // end namespace log4cplus

#endif /* LOG4CPLUS_SPI_FILTER_HEADER_ */


