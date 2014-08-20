// -*- C++ -*-
// Module:  Log4CPLUS
// File:    timehelper.h


/** @file */

#ifndef LOG4CPLUS_HELPERS_TIME_HELPER_HEADER_
#define LOG4CPLUS_HELPERS_TIME_HELPER_HEADER_

#include <log4cplus/config.h>
#include <log4cplus/timehelper.h>
#include <ctime>


namespace log4cplus { 


	using std::time_t;
	using std::tm;


	/**
	* This class represents a Epoch time with microsecond accuracy.
	*/
	class LOG4CPLUS_EXPORT TimeHelper 
	{
	public:
		TimeHelper();
		TimeHelper(time_t tv_sec, long tv_usec);
		explicit TimeHelper(time_t time);

		/**
		* Returns the current time using the <code>gettimeofday()</code>
		* method if it is available on the current platform.  (Not on 
		* WIN32.)
		*/
		static TimeHelper gettimeofday();

		// Methods
		/**
		* Returns <i>seconds</i> value.
		*/
		time_t sec() const { return tv_sec; }

		/**
		* Returns <i>microseconds</i> value.
		*/
		long usec() const { return tv_usec; }

		/**
		* Sets the <i>seconds</i> value.
		*/
		void sec(time_t s) { tv_sec = s; }

		/**
		* Sets the <i>microseconds</i> value.
		*/
		void usec(long us) { tv_usec = us; }

		/**
		* Sets this Time using the <code>mktime</code> function.
		*/
		time_t setTime(tm* t);

		/**
		* Returns this Time as a <code>time_t</code> value.
		*/
		time_t getTime() const;

		/**
		* Populates <code>tm</code> using the <code>gmtime()</code>
		* function.
		*/
		void gmtime(tm* t) const;

		/**
		* Populates <code>tm</code> using the <code>localtime()</code>
		* function.
		*/
		void localtime(tm* t) const;

		/**
		* Returns a string with a "formatted time" specified by
		* <code>fmt</code>.  It used the <code>strftime()</code>
		* function to do this.  
		* 
		* Look at your platform's <code>strftime()</code> documentation
		* for the formatting options available.
		* 
		* The following additional options are provided:<br>
		* <code>%q</code> - 3 character field that provides milliseconds
		* <code>%Q</code> - 7 character field that provides fractional 
		* milliseconds.
		*/
		string getFormattedTime(const string& fmt, bool use_gmtime = false) const;

		// Operators
		TimeHelper& operator+= (const TimeHelper& rhs);
		TimeHelper& operator-= (const TimeHelper& rhs);
		TimeHelper& operator/= (long rhs);
		TimeHelper& operator*= (long rhs);

	private:
		// Data
		time_t tv_sec;  /* seconds */
		long tv_usec;  /* microseconds */
	};
} // namespace log4cplus 
using namespace log4cplus;

	LOG4CPLUS_EXPORT const TimeHelper operator+(const TimeHelper& lhs, const TimeHelper& rhs);

	LOG4CPLUS_EXPORT const TimeHelper operator-(const TimeHelper& lhs, const TimeHelper& rhs);

	LOG4CPLUS_EXPORT const TimeHelper operator/(const TimeHelper& lhs, long rhs);

	LOG4CPLUS_EXPORT const TimeHelper operator*(const TimeHelper& lhs, long rhs);

	LOG4CPLUS_EXPORT bool operator < (const TimeHelper& lhs, const TimeHelper& rhs);

	LOG4CPLUS_EXPORT bool operator <= (const TimeHelper& lhs, const TimeHelper& rhs);

	LOG4CPLUS_EXPORT bool operator > (const TimeHelper& lhs, const TimeHelper& rhs);

	LOG4CPLUS_EXPORT bool operator >= (const TimeHelper& lhs, const TimeHelper& rhs);

	LOG4CPLUS_EXPORT bool operator == (const TimeHelper& lhs, const TimeHelper& rhs);

	LOG4CPLUS_EXPORT bool operator != (const TimeHelper& lhs, const TimeHelper& rhs);




#endif // LOG4CPLUS_HELPERS_TIME_HELPER_HEADER_

