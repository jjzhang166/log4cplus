// -*- C++ -*-
// Module:  Log4CPLUS
// File:    stringhelper.h


/** @file */

#ifndef LOG4CPLUS_HELPERS_STRINGHELPER_HEADER_
#define LOG4CPLUS_HELPERS_STRINGHELPER_HEADER_

#include <log4cplus/config.h>
#include <algorithm>



namespace log4cplus { 
	/**
	* Returns <code>s</code> in upper case.
	*/
	LOG4CPLUS_EXPORT string toUpper(const string& s);
	LOG4CPLUS_EXPORT char toUpper(char);


	/**
	* Returns <code>s</code> in lower case.
	*/
	LOG4CPLUS_EXPORT string toLower(const string& s);
	LOG4CPLUS_EXPORT char toLower(char);


	/**
	* Tokenize <code>s</code> using <code>c</code> as the delimiter and
	* put the resulting tokens in <code>_result</code>.  If
	* <code>collapseTokens</code> is false, multiple adjacent delimiters
	* will result in zero length tokens.
	*
	* <b>Example:</b>
	* <pre>
	*   string s = // Set string with '.' as delimiters
	*   list<string> tokens;
	*   tokenize(s, '.', back_insert_iterator<list<string> >(tokens));
	* </pre>
	*/
	template <class string, class OutputIter>
	inline void tokenize(const string& s, 
		typename string::value_type c, OutputIter result, bool collapseTokens = true)
	{
		typedef typename string::size_type size_type;
		size_type const slen = s.length();
		size_type first = 0;
		size_type i = 0;
		for(i=0; i < slen; ++i)
		{
			if(s[i] == c)
			{
				*result = string(s, first, i - first);
				++result;
				if(collapseTokens)
					while(i+1 < slen && s[i+1] == c)
						++i;
				first = i + 1;
			}
		}
		if(first != i)
			*result = string(s, first, i - first);
	}

	template <class stringType, class intType>
	inline void convertIntegerToString(stringType & str, intType intValue)
	{
		typedef std::numeric_limits<intType> intTypeLimits;
		typedef typename stringType::value_type charType;
		// typedef ConvertIntegerToStringHelper<intType, charType, intTypeLimits::is_signed> HelperType;

		charType buffer[intTypeLimits::digits10 + 2];
		// We define buffer_size from buffer using sizeof operator
		// to apease HP aCC compiler.
		const std::size_t buffer_size = sizeof(buffer) / sizeof(charType);

		charType * it = &buffer[buffer_size];
		charType const * const buf_end = &buffer[buffer_size];

		if(intValue == 0)
		{
			--it;
			*it = '0';
		}
		else
		{
			bool const isNegative = intValue < 0;

			for(; intValue != 0; --it)
			{
				intType mod = intValue % 10;
				intValue = intValue / 10;
				*(it - 1) = static_cast<charType>('0' + mod);
			}

			if(isNegative)
			{
				--it;
				*it = '-';
			}
		}

		str.assign(static_cast<charType const *>(it), buf_end);
	}


	template<class intType>
	inline string convertIntegerToString(intType intValue)
	{
		string result;
		convertIntegerToString(result, intValue);
		return result;
	}


	//!Join a list of items into a string.
	template <typename Iterator>
	inline void join(string& result, Iterator start, Iterator last, string const& sep)
	{
		if(start != last)
			result = *start++;

		for(; start != last; ++start)
		{
			result += sep;
			result += *start;
		}
	}

} // namespace log4cplus 

#endif // LOG4CPLUS_HELPERS_STRINGHELPER_HEADER_
