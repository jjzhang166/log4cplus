// -*- C++ -*-
// Module:  Log4CPLUS
// File:    property.h

/** @file */

#ifndef LOG4CPLUS_HELPERS_PROPERTY_HEADER_
#define LOG4CPLUS_HELPERS_PROPERTY_HEADER_

#include <log4cplus/config.h>

#include <map>
#include <vector>


namespace log4cplus { 

	//!\sa log4cplus::PropertyConfigurator
	class LOG4CPLUS_EXPORT Properties {
	public:

		Properties();
		explicit Properties(istream& input);
		explicit Properties(const string& inputFile);
		virtual ~Properties();

		// constants
		static const char PROPERTIES_COMMENT_CHAR;

		// methods
		/**
		* Tests to see if <code>key</code> can be found in this map.
		*/
		bool exists(const string& key) const;
		bool exists(char const * key) const;

		/**
		* Returns the number of entries in this map.
		*/
		std::size_t size() const
		{
			return _stringMap.size();
		}

		/**
		* Searches for the property with the specified key in this property
		* list. If the key is not found in this property list, the default
		* property list, and its defaults, recursively, are then checked. 
		* The method returns <code>null</code> if the property is not found.
		*/
		string const& getProperty(const string& key) const;
		string const& getProperty(char const * key) const;

		/**
		* Searches for the property with the specified key in this property
		* list. If the key is not found in this property list, the default
		* property list, and its defaults, recursively, are then checked. 
		* The method returns the default value argument if the property is 
		* not found.
		*/
		string getProperty(const string& key, const string& defaultVal) const;

		/**
		* Returns all the keys in this property list.
		*/
		vector<string> propertyNames() const;

		/**
		* Inserts <code>value</code> into this map indexed by <code>key</code>.
		*/
		void setProperty(const string& key, const string& value);

		/**
		* Removed the property index by <code>key</code> from this map.
		*/
		bool removeProperty(const string& key);

		/**
		* Returns a subset of the "properties" whose keys start with
		* "prefix".  The returned "properties" have "prefix" trimmed from
		* their keys.
		*/
		Properties getPropertySubset(const string& prefix) const;

		bool getInt(int & val, string const& key) const;
		bool getUInt(unsigned & val, string const& key) const;
		bool getLong(long & val, string const& key) const;
		bool getULong(unsigned long & val, string const& key) const;
		bool getBool(bool& val, string const& key) const;

	protected:
		// Types
		typedef std::map<string, string> StringMap;

		// Methods
		void init(istream& input);

		// Data
		StringMap _stringMap;

	private:
		string const& get_property_worker(string const& key) const;

		template <typename ValType>
		bool get_type_val_worker(ValType & val, string const& key) const;
	};
} 


#endif // LOG4CPLUS_HELPERS_PROPERTY_HEADER_

