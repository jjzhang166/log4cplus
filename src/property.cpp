// Module:  Log4CPLUS
// File:    property.cpp


#include <log4cplus/config.h>

#include <cstring>
#include <cctype>

#include <locale>
#include <fstream>
#include <sstream>

#include <log4cplus/stringhelper.h>
#include <log4cplus/property.h>
#include <log4cplus/internal.h>
#include <log4cplus/environment.h>
#include <log4cplus/loglog.h>


namespace log4cplus { 


const char Properties::PROPERTIES_COMMENT_CHAR = '#';


namespace
{

static void trim_leading_ws(string& str)
{
    string::iterator it = str.begin();
    for(; it != str.end(); ++it)
    {
        if(!std::isspace(*it))
            break;
    }
    str.erase(str.begin(), it);
}


static void trim_trailing_ws(string& str)
{
    string::reverse_iterator rit = str.rbegin();
    for(; rit != str.rend(); ++rit)
    {
        if(!std::isspace(*rit))
            break;
    }
    str.erase(rit.base(), str.end());
}


static void trim_ws(string& str)
{
    trim_trailing_ws(str);
    trim_leading_ws(str);
}

} // namespace



///////////////////////////////////////////////////////////////////////////////
// Properties ctors and dtor
///////////////////////////////////////////////////////////////////////////////

Properties::Properties()
{
}



Properties::Properties(istream& input)
{
    init(input);
}



Properties::Properties(const string& inputFile)
{
    if(inputFile.empty())
        return;

    std::ifstream file;

    file.open(inputFile.c_str(), std::ios::binary);
    if(!file.good())
        getLogLog().error("could not open file " + inputFile);

    init(file);
}



void Properties::init(istream& input) 
{
    if(!input)
        return;

    string buffer;
    while(std::getline(input, buffer))
    {
        trim_leading_ws(buffer);

        string::size_type const buffLen = buffer.size();
        if(buffLen == 0 || buffer[0] == PROPERTIES_COMMENT_CHAR)
            continue;
        
        // Check if we have a trailing \r because we are 
        // reading a properties file produced on Windows.
        if(buffer[buffLen-1] == '\r')
            // Remove trailing 'Windows' \r.
            buffer.resize(buffLen - 1);

        string::size_type const idx = buffer.find('=');
        if(idx != string::npos)
        {
            string key = buffer.substr(0, idx);
            string value = buffer.substr(idx + 1);
            trim_trailing_ws(key);
            trim_ws(value);
            setProperty(key, value);
        }
        else if(buffer.compare(0, 7, "include") == 0 && buffer.size() >= 7 + 1 + 1 && std::isspace(buffer[7]))
        {
            string included(buffer, 8) ;
            trim_ws(included);

			std::ifstream file;

            file.open(included.c_str(), std::ios::binary);
            if(!file.good())
                getLogLog().error("could not open file " + included);

            init(file);
        }
    }
}



Properties::~Properties() 
{
}



///////////////////////////////////////////////////////////////////////////////
// Properties public methods
///////////////////////////////////////////////////////////////////////////////


bool Properties::exists(const string& key) const
{
    return _stringMap.find(key) != _stringMap.end();
}


bool Properties::exists(char const * key) const
{
    return _stringMap.find(key) != _stringMap.end();
}


string const& Properties::getProperty(const string& key) const 
{
    return get_property_worker(key);
}


string const& Properties::getProperty(char const * key) const
{
    return get_property_worker(key);
}


string Properties::getProperty(const string& key, const string& defaultVal) const
{
    StringMap::const_iterator it(_stringMap.find(key));
    if(it == _stringMap.end())
        return defaultVal;
    else
        return it->second;
}


vector<string> Properties::propertyNames() const 
{
    vector<string> tmp;
    for(StringMap::const_iterator it=_stringMap.begin(); it!=_stringMap.end(); ++it)
        tmp.push_back(it->first);

    return tmp;
}



void Properties::setProperty(const string& key, const string& stringvalue)
{
    _stringMap[key] = stringvalue;
}


bool Properties::removeProperty(const string& key)
{
    return _stringMap.erase(key) > 0;
}


Properties Properties::getPropertySubset(const string& prefix) const
{
    Properties ret;
    std::size_t const prefix_len = prefix.size();
    vector<string> keys = propertyNames();
    for(vector<string>::iterator it=keys.begin(); it!=keys.end(); ++it)
    {
        int result = it->compare(0, prefix_len, prefix);
        if(result == 0)
            ret.setProperty(it->substr(prefix_len), getProperty(*it));
    }

    return ret;
}


bool Properties::getInt(int & val, string const& key) const
{
    return get_type_val_worker(val, key);
}


bool Properties::getUInt(unsigned & val, string const& key) const
{
    return get_type_val_worker(val, key);
}


bool Properties::getLong(long & val, string const& key) const
{
	return get_type_val_worker(val, key);
}


bool Properties::getULong(unsigned long & val, string const& key) const
{
    return get_type_val_worker(val, key);
}


bool Properties::getBool(bool& val, string const& key) const
{
    if(!exists(key))
        return false;

    string const& prop_val = getProperty(key);
    return parse_bool(val, prop_val);
}

const string emptystring = "";

string const& Properties::get_property_worker(string const& key) const
{
    StringMap::const_iterator it(_stringMap.find(key));
    if(it == _stringMap.end())
        return emptystring;
    else
        return it->second;
}


template <typename ValType>
bool Properties::get_type_val_worker(ValType & val, string const& key) const
{
    if(!exists(key))
        return false;

    string const& prop_val = getProperty(key);
    istringstream iss(prop_val);
    ValType tmp_val;
    char ch;

    iss >> tmp_val;
    if(!iss)
        return false;
    iss >> ch;
    if(iss)
        return false;

    val = tmp_val;
    return true;
}


}  // namespace log4cplus { 
