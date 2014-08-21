// -*- C++ -*-
// Module:  Log4CPLUS
// File:    stringhelper.h


/** @file */

#ifndef LOG4CPLUS_HELPERS_STRINGHELPER_HEADER_
#define LOG4CPLUS_HELPERS_STRINGHELPER_HEADER_

#include <log4cplus/config.h>
#include <algorithm>
#include <vector>
#include <iterator>


namespace log4cplus { 
	/**
	* Returns <code>s</code> in upper case.
	*/
	string toUpper(const string& s);
	char toUpper(char);

	/**
	* Returns <code>s</code> in lower case.
	*/
	string toLower(const string& s);
	char toLower(char);

	
	typedef std::back_insert_iterator<vector<string> > OutputIter;

	void tokenize(const string& s, char c, OutputIter result, bool collapseTokens = true);
	
	void convertIntegerToString(string & str, int intValue);
	
	string convertIntegerToString(int intValue);

	//!Join a list of items into a string.
	typedef vector<string, allocator<string> >::iterator Iterator;

	void join(string& result, Iterator start, Iterator last, string const& sep);

} // namespace log4cplus 

#endif // LOG4CPLUS_HELPERS_STRINGHELPER_HEADER_
