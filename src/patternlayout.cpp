// Module:  Log4CPLUS
// File:    patternlayout.cpp


#include <log4cplus/layout.h>
#include <log4cplus/helpers/loglog.h>
#include <log4cplus/helpers/timehelper.h>
#include <log4cplus/helpers/stringhelper.h>
#include <log4cplus/helpers/property.h>
#include <log4cplus/loggingevent.h>
#include <log4cplus/helpers/internal.h>
#include <log4cplus/helpers/environment.h>
#include <cstdlib>


namespace
{


static
std::string
get_basename(const std::string& filename)
{
#if defined(_MSC_VER)
    char const dir_sep('\\');
#else
    char const dir_sep('/');
#endif

    std::string::size_type pos = filename.rfind(dir_sep);
    if(pos != std::string::npos)
        return filename.substr(pos+1);
    else
        return filename;
}


} // namespace


namespace log4cplus
{

static char const ESCAPE_CHAR = '%';

extern void formatRelativeTimestamp(ostream & output,
    log4cplus::InternalLoggingEvent const& loggingEvent);


namespace pattern
{


/**
 * This is used by PatternConverter class to inform them how to format
 * their output.
 */
struct FormattingInfo {
    int minLen;
    std::size_t maxLen;
    bool leftAlign;
    FormattingInfo() { reset(); }

    void reset();
    void dump(helpers::LogLog&);
};



/**
 * This is the base class of all "Converter" classes that format a
 * field of InternalLoggingEvent objects.  In fact, the PatternLayout
 * class simply uses an array of PatternConverter objects to format
 * and append a logging loggingEvent.
 */
class PatternConverter
{
public:
    explicit PatternConverter(const FormattingInfo& info);
    virtual ~PatternConverter() {}
    void formatAndAppend(ostream& output, 
        const InternalLoggingEvent& loggingEvent);

    virtual void convert(std::string& result,
        const InternalLoggingEvent& loggingEvent) = 0;

private:
    int minLen;
    std::size_t maxLen;
    bool leftAlign;
};


typedef std::vector<pattern::PatternConverter*> PatternConverterList;


/**
 * This PatternConverter returns a constant string.
 */
class LiteralPatternConverter : public PatternConverter
{
public:
    LiteralPatternConverter(const std::string& str);
    virtual void convert(std::string& result,
        const InternalLoggingEvent&)
    {
        result = str;
    }

private:
    std::string str;
};


/**
 * This PatternConverter is used to format most of the "simple" fields
 * found in the InternalLoggingEvent object.
 */
class BasicPatternConverter
    : public PatternConverter
{
public:
	enum Type 
	{
		PROCESS_CONVERTER,
		LOGLEVEL_CONVERTER,
		MESSAGE_CONVERTER,
		NEWLINE_CONVERTER,
		BASENAME_CONVERTER,
		FILE_CONVERTER,
		LINE_CONVERTER,
		FULL_LOCATION_CONVERTER,
		FUNCTION_CONVERTER 
	};
    BasicPatternConverter(const FormattingInfo& info, Type type);
    virtual void convert(std::string& result, 
        const InternalLoggingEvent& loggingEvent);

private:
  // Disable copy
    BasicPatternConverter(const BasicPatternConverter&);
    BasicPatternConverter& operator= (BasicPatternConverter&);
    
    LogLevelManager& llmCache;
    Type type;
};



/**
 * This PatternConverter is used to format the Logger field found in
 * the InternalLoggingEvent object.
 */
class LoggerPatternConverter : public PatternConverter {
public:
    LoggerPatternConverter(const FormattingInfo& info, int precision);
    virtual void convert(std::string& result,
        const InternalLoggingEvent& loggingEvent);

private:
    int precision;
};



/**
 * This PatternConverter is used to format the timestamp field found in
 * the InternalLoggingEvent object.  It will be formatted according to
 * the specified "pattern".
 */
class DatePatternConverter : public PatternConverter {
public:
    DatePatternConverter(const FormattingInfo& info, 
                         const std::string& pattern, 
                         bool use_gmtime);
    virtual void convert(std::string& result,
        const InternalLoggingEvent& loggingEvent);

private:
    bool use_gmtime;
    std::string format;
};


/**
 * This PatternConverter is used to format an environment variable
 */
class EnvPatternConverter : public PatternConverter {
public:
    EnvPatternConverter(const FormattingInfo& info, 
                        const std::string& env);
    virtual void convert(std::string& result,
        const InternalLoggingEvent& loggingEvent);

private:
    std::string envKey;
};


/**
 * This class parses a "pattern" string into an array of
 * PatternConverter objects.
 * <p>
 * @see PatternLayout for the formatting of the "pattern" string.
 */
class PatternParser
{
public:
    PatternParser(const std::string& pattern, unsigned ndcMaxDepth);
    std::vector<PatternConverter*> parse();

private:
  // Types
    enum ParserState { LITERAL_STATE, 
                       CONVERTER_STATE,
                       DOT_STATE,
                       MIN_STATE,
                       MAX_STATE };

  // Methods
    std::string extractOption();
    int extractPrecisionOption();
    void finalizeConverter(char c);

  // Data
    std::string pattern;
    FormattingInfo formattingInfo;
    std::vector<PatternConverter*> list;
    ParserState state;
    std::string::size_type pos;
    std::string currentLiteral;
    unsigned ndcMaxDepth;
};


////////////////////////////////////////////////
// FormattingInfo methods:
////////////////////////////////////////////////

void 
FormattingInfo::reset() {
    minLen = -1;
    maxLen = 0x7FFFFFFF;
    leftAlign = false;
}


void 
FormattingInfo::dump(helpers::LogLog& loglog) {
    ostringstream buf;
    buf << "min=" << minLen
        << ", max=" << maxLen
        << ", leftAlign=" << std::boolalpha << leftAlign;
    loglog.debug(buf.str());
}




////////////////////////////////////////////////
// PatternConverter methods:
////////////////////////////////////////////////

PatternConverter::PatternConverter(const FormattingInfo& i)
{
    minLen = i.minLen;
    maxLen = i.maxLen;
    leftAlign = i.leftAlign;
}



void
PatternConverter::formatAndAppend(
    ostream& output, const InternalLoggingEvent& loggingEvent)
{
    std::string& s = internal::get_ptd()->_faa_str;
    convert(s, loggingEvent);
    std::size_t len = s.length();

    if(len > maxLen)
        output << s.substr(len - maxLen);
    else if(static_cast<int>(len) < minLen)
    {
        std::ios_base::fmtflags const original_flags = output.flags();
        char const fill = output.fill(' ');
        output.setf(leftAlign ? std::ios_base::left : std::ios_base::right,
            std::ios_base::adjustfield);
        output.width(minLen);
        output << s;
        output.fill(fill);
        output.flags(original_flags);
    }
    else
        output << s;
}



////////////////////////////////////////////////
// LiteralPatternConverter methods:
////////////////////////////////////////////////

LiteralPatternConverter::LiteralPatternConverter(
    const std::string& str_)
    : PatternConverter(FormattingInfo())
    , str(str_)
{
}



////////////////////////////////////////////////
// BasicPatternConverter methods:
////////////////////////////////////////////////

BasicPatternConverter::BasicPatternConverter(
    const FormattingInfo& info, Type type_)
    : PatternConverter(info)
    , llmCache(getLogLevelManager())
    , type(type_)
{
}



void
BasicPatternConverter::convert(std::string& result,
    const InternalLoggingEvent& loggingEvent)
{
    switch(type)
    {
    case LOGLEVEL_CONVERTER:
        result = llmCache.toString(loggingEvent.getLogLevel());
        return;

    case BASENAME_CONVERTER:
        result = get_basename(loggingEvent.getFile());
        return;

    case PROCESS_CONVERTER:
		{		
			int pid = 0; 
			#if defined(_MSC_VER)
				pid = (int)GetCurrentProcessId();
			#else
				pid = (int)getpid();
			#endif

			helpers::convertIntegerToString(result, pid); 
		}
        return;

    case MESSAGE_CONVERTER:
        result = loggingEvent.getMessage();
        return;

    case NEWLINE_CONVERTER:
        result = "\n";
        return; 

    case FILE_CONVERTER:
        result = loggingEvent.getFile();
        return;

    case LINE_CONVERTER:
        {
            if(loggingEvent.getLine() != -1)
                helpers::convertIntegerToString(result, loggingEvent.getLine());
            else
                result.clear();
            return;
        }

    case FULL_LOCATION_CONVERTER:
        {
            std::string const& file = loggingEvent.getFile();
            if(!file.empty())
            {
                result = file;
                result += ":";
                result += helpers::convertIntegerToString(loggingEvent.getLine());
            }
            else
                result = ":";
            return;
        }
        
    case FUNCTION_CONVERTER:
        result = loggingEvent.getFunction();
        return;
    }

    result = "INTERNAL LOG4CPLUS ERROR";
}



////////////////////////////////////////////////
// LoggerPatternConverter methods:
////////////////////////////////////////////////

LoggerPatternConverter::LoggerPatternConverter(
    const FormattingInfo& info, int prec)
    : PatternConverter(info)
    , precision(prec)
{
}



void
LoggerPatternConverter::convert(std::string& result,
    const InternalLoggingEvent& loggingEvent)
{
    const std::string& name = loggingEvent.getLoggerName();
    if(precision <= 0) {
        result = name;
    }
    else {
        std::size_t len = name.length();

        // We substract 1 from 'len' when assigning to 'end' to avoid out of
        // bounds exception in return r.substring(end+1, len). This can happen
        // if precision is 1 and the logger name ends with a dot. 
        std::string::size_type end = len - 1;
        for(int i = precision; i > 0; --i)
        {
            end = name.rfind('.', end - 1);
            if(end == std::string::npos) {
                result = name;
                return;
            }
        }
        result = name.substr(end + 1);
    }
}



////////////////////////////////////////////////
// DatePatternConverter methods:
////////////////////////////////////////////////


DatePatternConverter::DatePatternConverter(
    const FormattingInfo& info, const std::string& pattern,
    bool use_gmtime_)
    : PatternConverter(info)
    , use_gmtime(use_gmtime_)
    , format(pattern)
{
}



void DatePatternConverter::convert(std::string& result, const InternalLoggingEvent& loggingEvent)
{
    result = loggingEvent.getTimestamp().getFormattedTime(format, use_gmtime);
}


////////////////////////////////////////////////
// EnvPatternConverter methods:
////////////////////////////////////////////////


EnvPatternConverter::EnvPatternConverter(
    const FormattingInfo& info, const std::string& env)
    : PatternConverter(info)
    , envKey(env)
{ }


void
EnvPatternConverter::convert(std::string& result,
    const InternalLoggingEvent&)
{
    if(!internal::get_env_var(result, envKey))
    {
        // Variable doesn't exist, return empty string.
        result.clear();
    }
}


////////////////////////////////////////////////
// PatternParser methods:
////////////////////////////////////////////////

PatternParser::PatternParser(
    const std::string& pattern_, unsigned ndcMaxDepth_)
    : pattern(pattern_)
    , state(LITERAL_STATE)
    , pos(0)
    , ndcMaxDepth(ndcMaxDepth_)
{
}



std::string 
PatternParser::extractOption() 
{
    if(  (pos < pattern.length()) 
        &&(pattern[pos] == '{')) 
    {
        std::string::size_type end = pattern.find_first_of('}', pos);
        if(end != std::string::npos) {
            std::string r = pattern.substr(pos + 1, end - pos - 1);
            pos = end + 1;
            return r;
        }
        else {
            ostringstream buf;
            buf << "No matching '}' found in conversion pattern string "
                << pattern;
            helpers::getLogLog().error(buf.str());
            pos = pattern.length();
        }
    }

    return "";
}


int 
PatternParser::extractPrecisionOption() 
{
    std::string opt = extractOption();
    int r = 0;
    if(!opt.empty())
        r = std::atoi(opt.c_str());

    return r;
}



PatternConverterList
PatternParser::parse() 
{
    char c;
    pos = 0;
    while(pos < pattern.length()) {
        c = pattern[pos++];
        switch(state) {
        case LITERAL_STATE :
            // In literal state, the last char is always a literal.
            if(pos == pattern.length()) {
                currentLiteral += c;
                continue;
            }
            if(c == ESCAPE_CHAR) {
                // peek at the next char. 
                switch(pattern[pos]) {
                case ESCAPE_CHAR:
                    currentLiteral += c;
                    pos++; // move pointer
                    break;
                default:
                    if(!currentLiteral.empty()) {
                        list.push_back
                            (new LiteralPatternConverter(currentLiteral));
                        //getLogLog().debug("Parsed LITERAL converter: \"" 
                        //                  +currentLiteral+"\".");
                    }
                    currentLiteral.resize(0);
                    currentLiteral += c; // append %
                    state = CONVERTER_STATE;
                    formattingInfo.reset();
                }
            }
            else {
                currentLiteral += c;
            }
            break;

        case CONVERTER_STATE:
            currentLiteral += c;
            switch(c) {
            case '-':
                formattingInfo.leftAlign = true;
                break;
            case '.':
                state = DOT_STATE;
                break;
            default:
                if(c >= '0' && c <= '9') {
                    formattingInfo.minLen = c - '0';
                    state = MIN_STATE;
                }
                else {
                    finalizeConverter(c);
                }
            } // switch
            break;

        case MIN_STATE:
            currentLiteral += c;
            if(c >= '0' && c <= '9') {
                formattingInfo.minLen = formattingInfo.minLen * 10 +(c - '0');
            }
            else if(c == '.') {
                state = DOT_STATE;
            }
            else {
                finalizeConverter(c);
            }
            break;

        case DOT_STATE:
            currentLiteral += c;
            if(c >= '0' && c <= '9') {
                formattingInfo.maxLen = c - '0';
                state = MAX_STATE;
            }
            else 
			{
                ostringstream buf;
                buf << "Error occured in position "
                    << pos
                    << ".\n Was expecting digit, instead got char "
                    << c;
                helpers::getLogLog().error(buf.str());
                state = LITERAL_STATE;
            }
            break;

         case MAX_STATE:
            currentLiteral += c;
            if(c >= '0' && c <= '9')
                formattingInfo.maxLen = formattingInfo.maxLen * 10 +(c - '0');
            else {
                finalizeConverter(c);
                state = LITERAL_STATE;
            }
            break;
        } // end switch
    } // end while

    if(!currentLiteral.empty()) {
        list.push_back(new LiteralPatternConverter(currentLiteral));
      //getLogLog().debug("Parsed LITERAL converter: \""+currentLiteral+"\".");
    }

    return list;
}



void
PatternParser::finalizeConverter(char c) 
{
    PatternConverter* pc = 0;
    switch(c) 
	{
        case 'b':
            pc = new BasicPatternConverter(formattingInfo, BasicPatternConverter::BASENAME_CONVERTER);
            //getLogLog().debug("BASENAME converter.");
            //formattingInfo.dump(getLogLog());      
            break;
            
        case 'c':
            pc = new LoggerPatternConverter(formattingInfo, extractPrecisionOption());
            //getLogLog().debug( _TEXT("LOGGER converter."));
            //formattingInfo.dump(getLogLog());      
            break;

        case 'd':
        case 'D':
            {
                std::string dOpt = extractOption();
                if(dOpt.empty()) 
				{
                    dOpt = "%Y-%m-%d %H:%M:%S";
                }
                bool use_gmtime = c == 'd';
                pc = new DatePatternConverter(formattingInfo, dOpt, use_gmtime);
                //if(use_gmtime) {
                //    getLogLog().debug("GMT DATE converter.");
                //}
                //else {
                //    getLogLog().debug("LOCAL DATE converter.");
                //}
                //formattingInfo.dump(getLogLog());      
            }
            break;

        case 'E':
            pc = new EnvPatternConverter(formattingInfo, extractOption());
            //getLogLog().debug("Environment converter.");
            //formattingInfo.dump(getLogLog());      
            break;

        case 'F':
            pc = new BasicPatternConverter(formattingInfo, BasicPatternConverter::FILE_CONVERTER);
            //getLogLog().debug("FILE NAME converter.");
            //formattingInfo.dump(getLogLog());      
            break;

        case 'i':
            pc = new BasicPatternConverter(formattingInfo, BasicPatternConverter::PROCESS_CONVERTER);
            //getLogLog().debug("PROCESS_CONVERTER converter.");
            //formattingInfo.dump(getLogLog());      
            break;

        case 'l':
            pc = new BasicPatternConverter(formattingInfo, BasicPatternConverter::FULL_LOCATION_CONVERTER);
            //getLogLog().debug("FULL LOCATION converter.");
            //formattingInfo.dump(getLogLog());      
            break;

        case 'L':
            pc = new BasicPatternConverter(formattingInfo, BasicPatternConverter::LINE_CONVERTER);
            //getLogLog().debug("LINE NUMBER converter.");
            //formattingInfo.dump(getLogLog());      
            break;

        case 'm':
            pc = new BasicPatternConverter(formattingInfo, BasicPatternConverter::MESSAGE_CONVERTER);
            //getLogLog().debug("MESSAGE converter.");
            //formattingInfo.dump(getLogLog());      
            break;

        case 'M':
            pc = new BasicPatternConverter(formattingInfo, BasicPatternConverter::FUNCTION_CONVERTER);
            //getLogLog().debug("METHOD(function name) converter.");
            //formattingInfo.dump(getLogLog());   
            break;

        case 'n':
            pc = new BasicPatternConverter(formattingInfo, BasicPatternConverter::NEWLINE_CONVERTER);
            //getLogLog().debug("MESSAGE converter.");
            //formattingInfo.dump(getLogLog());      
            break;

        case 'p':
            pc = new BasicPatternConverter(formattingInfo, BasicPatternConverter::LOGLEVEL_CONVERTER);
            //getLogLog().debug("LOGLEVEL converter.");
            //formattingInfo.dump(getLogLog());
            break;

        default:
            ostringstream buf;
            buf << "Unexpected char [" << c
                << "] at position " << pos
                << " in conversion patterrn.";
            helpers::getLogLog().error(buf.str());
            pc = new LiteralPatternConverter(currentLiteral);
    }

    list.push_back(pc);
    currentLiteral.resize(0);
    state = LITERAL_STATE;
    formattingInfo.reset();
}


} // namespace pattern


typedef pattern::PatternConverterList PatternConverterList;


////////////////////////////////////////////////
// PatternLayout methods:
////////////////////////////////////////////////

PatternLayout::PatternLayout(const std::string& pattern_)
{
    init(pattern_, 0);
}


PatternLayout::PatternLayout(const helpers::Properties& properties)
{
    unsigned ndcMaxDepth = 0;
    properties.getUInt(ndcMaxDepth, "NDCMaxDepth");

    bool hasPattern = properties.exists("Pattern");
    bool hasConversionPattern = properties.exists("ConversionPattern");
    
    if(hasPattern) 
	{
		helpers::getLogLog().warn("PatternLayout- the Pattern property has been"
            " deprecated.  Use ConversionPattern instead.");
    }
    
    if(hasConversionPattern) 
	{
        init(properties.getProperty("ConversionPattern"), ndcMaxDepth);
    }
    else if(hasPattern) 
	{
        init(properties.getProperty("Pattern"), ndcMaxDepth);
    }
    else
	{
        helpers::getLogLog().error("ConversionPattern not specified in properties", true);
    }

}


void
PatternLayout::init(const std::string& pattern_, unsigned ndcMaxDepth)
{
    pattern = pattern_;
    parsedPattern = pattern::PatternParser(pattern, ndcMaxDepth).parse();

    // Let's validate that our parser didn't give us any NULLs.  If it did,
    // we will convert them to a valid PatternConverter that does nothing so
    // at least we don't core.
    for(PatternConverterList::iterator it=parsedPattern.begin(); 
        it!=parsedPattern.end(); 
        ++it)
    {
        if((*it) == 0 ) 
		{
            helpers::getLogLog().error("Parsed Pattern created a NULL PatternConverter");
           (*it) = new pattern::LiteralPatternConverter("");
        }
    }
    if(parsedPattern.empty()) 
	{
        helpers::getLogLog().warn("PatternLayout pattern is empty.  Using default...");
        parsedPattern.push_back(
            new pattern::BasicPatternConverter(pattern::FormattingInfo(), 
            pattern::BasicPatternConverter::MESSAGE_CONVERTER));
    }
}



PatternLayout::~PatternLayout()
{
    for(PatternConverterList::iterator it=parsedPattern.begin(); 
        it!=parsedPattern.end(); 
        ++it)
    {
        delete(*it);
    }
}



void PatternLayout::formatAndAppend(ostream& output, const InternalLoggingEvent& loggingEvent)
{
    for(PatternConverterList::iterator it=parsedPattern.begin(); it!=parsedPattern.end(); ++it)
    {
       (*it)->formatAndAppend(output, loggingEvent);
    }
}


} // namespace log4cplus
