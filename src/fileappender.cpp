// Module:  Log4CPLUS
// File:    fileappender.cpp


#include <log4cplus/fileappender.h>
#include <log4cplus/layout.h>

#include <log4cplus/helpers/loglog.h>
#include <log4cplus/helpers/stringhelper.h>
#include <log4cplus/helpers/timehelper.h>
#include <log4cplus/helpers/property.h>
#include <log4cplus/helpers/fileinfo.h>
#include <log4cplus/spi/loggingevent.h>
#include <log4cplus/spi/factory.h>
#include <log4cplus/internal/internal.h>
#include <log4cplus/internal/env.h>
#include <algorithm>
#include <sstream>
#include <cstdio>
#include <stdexcept>

#include <cerrno>


namespace log4cplus
{

using helpers::Properties;
using helpers::TimeHelper;


const long DEFAULT_ROLLING_LOG_SIZE = 10 * 1024 * 1024L;
const long MINIMUM_ROLLING_LOG_SIZE = 200*1024L;


///////////////////////////////////////////////////////////////////////////////
// File LOCAL definitions
///////////////////////////////////////////////////////////////////////////////

namespace
{

long const LOG4CPLUS_FILE_NOT_FOUND = ENOENT;


static long file_rename(std::string const& src, std::string const& target)
{
    if(std::rename(src.c_str(),target.c_str()) == 0)
        return 0;
    else
        return errno;
}


static long file_remove(std::string const& src)
{
    if(std::remove(src.c_str()) == 0)
        return 0;
    else
        return errno;

}


static void loglog_renaming_result(helpers::LogLog & loglog, std::string const& src, std::string const& target, long ret)
{
    if(ret == 0)
    {
        loglog.debug("Renamed file " + src + " to " + target);
    }
    else if(ret != LOG4CPLUS_FILE_NOT_FOUND)
    {
        ostringstream oss;
        oss << "Failed to rename file from " << src << " to " << target << "; error" << ret;
        loglog.error(oss.str());
    }
}


static void loglog_opening_result(helpers::LogLog & loglog, ostream const& os, std::string const& filename)
{
    if(!os)
    {
        loglog.error("Failed to open file " + filename);
    }
}


static void rolloverFiles(const std::string& filename, unsigned int maxBackupIndex)
{
    helpers::LogLog * loglog = helpers::LogLog::getLogLog();

    // Delete the oldest file
    ostringstream buffer;
    buffer << filename << "." << maxBackupIndex;
    long ret = file_remove(buffer.str());

    ostringstream source_oss;
    ostringstream target_oss;

    // Map {(maxBackupIndex - 1), ..., 2, 1} to {maxBackupIndex, ..., 3, 2}
    for(int i = maxBackupIndex - 1; i >= 1; --i)
    {
        source_oss.str("");
        target_oss.str("");

        source_oss << filename << "." << i;
        target_oss << filename << "." <<(i+1);

        std::string const source(source_oss.str());
        std::string const target(target_oss.str());

#if defined(_MSC_VER)
        // Try to remove the target first. It seems it is not
        // possible to rename over existing file.
        ret = file_remove(target);
#endif

        ret = file_rename(source, target);
        loglog_renaming_result(*loglog, source, target, ret);
    }
} // end rolloverFiles()

} // namespace


///////////////////////////////////////////////////////////////////////////////
// FileAppender ctors and dtor
///////////////////////////////////////////////////////////////////////////////

FileAppender::FileAppender(const std::string& filename_, std::ios_base::openmode mode_, bool immediateFlush_, bool createDirs_)
    : _immediateFlush(immediateFlush_), _isCreateDirs(createDirs_)
    , _reopenDelay(1), _ofstreamBufferSize(0)
    , _ofstreamBuffer(0)
{
    init(filename_, mode_);
}


FileAppender::FileAppender(const Properties& props, std::ios_base::openmode mode_)
    : Appender(props), _immediateFlush(true)
    , _isCreateDirs(false), _reopenDelay(1)
    , _ofstreamBufferSize(0), _ofstreamBuffer(0)
{
    bool app =(mode_ &(std::ios_base::app | std::ios_base::ate)) != 0;
    std::string const& fn = props.getProperty("File");
    if(fn.empty())
    {
        getErrorHandler()->error("Invalid filename");
        return;
    }

    props.getBool(_immediateFlush, "ImmediateFlush");
    props.getBool(_isCreateDirs, "CreateDirs");
    props.getBool(app, "Append");
    props.getInt(_reopenDelay, "ReopenDelay");
    props.getULong(_ofstreamBufferSize, "BufferSize");

    init(fn,(app ? std::ios::app : std::ios::trunc));
}



void FileAppender::init(const std::string& filename_, std::ios_base::openmode mode_)
{
    _filename = filename_;

    if(_ofstreamBufferSize != 0)
    {
        delete[] _ofstreamBuffer;
        _ofstreamBuffer = new char[_ofstreamBufferSize];
        _out.rdbuf()->pubsetbuf(_ofstreamBuffer, _ofstreamBufferSize);
    }

    open(mode_);

    if(!_out.good()) {
        getErrorHandler()->error("Unable to open file: " + _filename);
        return;
    }
    helpers::getLogLog().debug("Just opened file: " + _filename);
}



FileAppender::~FileAppender()
{
    destructorImpl();
}



///////////////////////////////////////////////////////////////////////////////
// FileAppender public methods
///////////////////////////////////////////////////////////////////////////////

void FileAppender::close()
{
   Mutex::ScopedLock lock(_access_mutex);

    _out.close();
    delete[] _ofstreamBuffer;
    _ofstreamBuffer = 0;
    _isClosed = true;
}

std::locale FileAppender::getloc() const
{
    return _out.getloc();
}


///////////////////////////////////////////////////////////////////////////////
// FileAppender protected methods
///////////////////////////////////////////////////////////////////////////////

// This method does not need to be locked since it is called by
// doAppend() which performs the locking
void FileAppender::append(const InternalLoggingEvent& loggingEvent)
{
    if(!_out.good())
	{
        if(!reopen()) 
		{
            getErrorHandler()->error("file is not open: " + _filename);
            return;
        }
        // Resets the errorhandler to make it 
        // ready to handle a future append error.
        else
		{
            getErrorHandler()->reset();
		}
    }

    _layout->formatAndAppend(_out, loggingEvent);

    if(_immediateFlush)
        _out.flush();
}

void FileAppender::open(std::ios_base::openmode mode)
{
    if(_isCreateDirs)
        internal::make_dirs(_filename);

    _out.open(_filename.c_str(), mode);
}

bool FileAppender::reopen()
{
    // When append never failed and the file re-open attempt must
    // be delayed, set the time when reopen should take place.
    if(_reopen_time == helpers::TimeHelper() && _reopenDelay != 0)
	{
        _reopen_time = helpers::TimeHelper::gettimeofday() + helpers::TimeHelper(_reopenDelay);
	}
    else
    {
        // Otherwise, check for end of the delay(or absence of delay)
        // to re-open the file.
        if(_reopen_time <= helpers::TimeHelper::gettimeofday() || _reopenDelay == 0)
        {
            // Close the current file
            _out.close();
            // reset flags since the C++ standard specified that all
            // the flags should remain unchanged on a close
            _out.clear();

            // Re-open the file.
            open(std::ios_base::out | std::ios_base::ate | std::ios_base::app);

            // Reset last fail time.
            _reopen_time = helpers::TimeHelper();

            // Succeed if no errors are found.
            if(_out.good())
                return true;
        }
    }
    return false;
}

///////////////////////////////////////////////////////////////////////////////
// RollingFileAppender ctors and dtor
///////////////////////////////////////////////////////////////////////////////

RollingFileAppender::RollingFileAppender(const std::string& filename_,
	long maxFileSize_, int maxBackupIndex_, bool immediateFlush_, bool createDirs_)
    : FileAppender(filename_, std::ios_base::app, immediateFlush_, createDirs_)
{
    init(maxFileSize_, maxBackupIndex_);
}


RollingFileAppender::RollingFileAppender(const Properties& properties)
    : FileAppender(properties, std::ios_base::app)
{
    long tmpMaxFileSize = DEFAULT_ROLLING_LOG_SIZE;
    int tmpMaxBackupIndex = 1;
    std::string tmp(helpers::toUpper(properties.getProperty("MaxFileSize")));
    if(!tmp.empty())
    {
        tmpMaxFileSize = std::atoi(tmp.c_str());
        if(tmpMaxFileSize != 0)
        {
            std::string::size_type const len = tmp.length();
            if(len > 2 && tmp.compare(len - 2, 2, "MB") == 0)
                tmpMaxFileSize *=(1024 * 1024); // convert to megabytes
            else if(len > 2 && tmp.compare(len - 2, 2, "KB") == 0)
                tmpMaxFileSize *= 1024; // convert to kilobytes
        }
    }

    properties.getInt(tmpMaxBackupIndex, "MaxBackupIndex");

    init(tmpMaxFileSize, tmpMaxBackupIndex);
}


void RollingFileAppender::init(long maxFileSize_, int maxBackupIndex_)
{
    if(maxFileSize_ < MINIMUM_ROLLING_LOG_SIZE)
    {
        ostringstream oss;
        oss << "RollingFileAppender: MaxFileSize property value is too small. Resetting to"
            << MINIMUM_ROLLING_LOG_SIZE << ".";
        helpers::getLogLog().warn(oss.str());
        maxFileSize_ = MINIMUM_ROLLING_LOG_SIZE;
    }

    _maxFileSize = maxFileSize_;
    _maxBackupIndex =(std::max)(maxBackupIndex_, 1);
}


RollingFileAppender::~RollingFileAppender()
{
    destructorImpl();
}


///////////////////////////////////////////////////////////////////////////////
// RollingFileAppender protected methods
///////////////////////////////////////////////////////////////////////////////

// This method does not need to be locked since it is called by
// doAppend() which performs the locking
void RollingFileAppender::append(const InternalLoggingEvent& loggingEvent)
{
    // Rotate log file if needed before appending to it.
    if(_out.tellp() > _maxFileSize)
        rollover();

    FileAppender::append(loggingEvent);

    // Rotate log file if needed after appending to it.
    if(_out.tellp() > _maxFileSize)
        rollover();
}


void RollingFileAppender::rollover()
{
    helpers::LogLog & loglog = helpers::getLogLog();

    // Close the current file
    _out.close();
    // Reset flags since the C++ standard specified that all the flags
    // should remain unchanged on a close.
    _out.clear(); 

    // If maxBackups <= 0, then there is no file renaming to be done.
    if(_maxBackupIndex > 0)
    {
        rolloverFiles(_filename, _maxBackupIndex);

        // Rename fileName to fileName.1
        std::string target = _filename + ".1";

        long ret;

#if defined(_MSC_VER)
        // Try to remove the target first. It seems it is not
        // possible to rename over existing file.
        ret = file_remove(target);
#endif

        loglog.debug("Renaming file " + _filename + " to " + target);
        ret = file_rename(_filename, target);
        loglog_renaming_result(loglog, _filename, target, ret);
    }
    else
    {
        loglog.debug(_filename + " has no backups specified");
    }

    // Open it up again in truncation mode
    open(std::ios::out | std::ios::trunc);
    loglog_opening_result(loglog, _out, _filename);
}


///////////////////////////////////////////////////////////////////////////////
// DailyRollingFileAppender ctors and dtor
///////////////////////////////////////////////////////////////////////////////

DailyRollingFileAppender::DailyRollingFileAppender(
	const std::string& filename_, DailyRollingFileSchedule schedule_,
    bool immediateFlush_, int maxBackupIndex_, bool createDirs_)
    : FileAppender(filename_, std::ios_base::app, immediateFlush_, createDirs_)
    , _maxBackupIndex(maxBackupIndex_)
{
    init(schedule_);
}



DailyRollingFileAppender::DailyRollingFileAppender(const Properties& properties)
    : FileAppender(properties, std::ios_base::app)
    , _maxBackupIndex(10)
{
    DailyRollingFileSchedule theSchedule = DAILY;
    std::string scheduleStr(helpers::toUpper(
        properties.getProperty("Schedule")));

    if(scheduleStr == "MONTHLY")
        theSchedule = MONTHLY;
    else if(scheduleStr == "WEEKLY")
        theSchedule = WEEKLY;
    else if(scheduleStr == "DAILY")
        theSchedule = DAILY;
    else if(scheduleStr == "TWICE_DAILY")
        theSchedule = TWICE_DAILY;
    else if(scheduleStr == "HOURLY")
        theSchedule = HOURLY;
    else if(scheduleStr == "MINUTELY")
        theSchedule = MINUTELY;
    else {
        helpers::getLogLog().warn("DailyRollingFileAppender::ctor() - Schedule not valid: " + properties.getProperty("Schedule"));
        theSchedule = DAILY;
    }
    
    properties.getInt(_maxBackupIndex, "MaxBackupIndex");

    init(theSchedule);
}



void DailyRollingFileAppender::init(DailyRollingFileSchedule sch)
{
    this->_schedule = sch;

    TimeHelper now = TimeHelper::gettimeofday();
    now.usec(0);
    struct tm time;
    now.localtime(&time);

    time.tm_sec = 0;
    switch(_schedule)
    {
    case MONTHLY:
        time.tm_mday = 1;
        time.tm_hour = 0;
        time.tm_min = 0;
        break;

    case WEEKLY:
        time.tm_mday -=(time.tm_wday % 7);
        time.tm_hour = 0;
        time.tm_min = 0;
        break;

    case DAILY:
        time.tm_hour = 0;
        time.tm_min = 0;
        break;

    case TWICE_DAILY:
        if(time.tm_hour >= 12) 
		{
            time.tm_hour = 12;
        }
        else 
		{
            time.tm_hour = 0;
        }
        time.tm_min = 0;
        break;

    case HOURLY:
        time.tm_min = 0;
        break;

    case MINUTELY:
        break;
    };
    now.setTime(&time);

    _scheduledFilename = getFilename(now);
    _nextRolloverTime = calculateNextRolloverTime(now);
}



DailyRollingFileAppender::~DailyRollingFileAppender()
{
    destructorImpl();
}




///////////////////////////////////////////////////////////////////////////////
// DailyRollingFileAppender public methods
///////////////////////////////////////////////////////////////////////////////

void DailyRollingFileAppender::close()
{
    rollover();
    FileAppender::close();
}



///////////////////////////////////////////////////////////////////////////////
// DailyRollingFileAppender protected methods
///////////////////////////////////////////////////////////////////////////////

// This method does not need to be locked since it is called by
// doAppend() which performs the locking
void DailyRollingFileAppender::append(const InternalLoggingEvent& loggingEvent)
{
    if(loggingEvent.getTimestamp() >= _nextRolloverTime) 
	{
        rollover();
    }

    FileAppender::append(loggingEvent);
}



void DailyRollingFileAppender::rollover()
{
    // Close the current file
    _out.close();
    // reset flags since the C++ standard specified that all the flags
    // should remain unchanged on a close
    _out.clear();

    // If we've already rolled over this time period, we'll make sure that we
    // don't overwrite any of those previous files.
    // E.g. if "log.2009-11-07.1" already exists we rename it
    // to "log.2009-11-07.2", etc.
    rolloverFiles(_scheduledFilename, _maxBackupIndex);

    // Do not overwriet the newest file either, e.g. if "log.2009-11-07"
    // already exists rename it to "log.2009-11-07.1"
    ostringstream backup_target_oss;
    backup_target_oss << _scheduledFilename << "." << 1;
    std::string backupTarget = backup_target_oss.str();

    helpers::LogLog & loglog = helpers::getLogLog();
    long ret;

#if defined(_MSC_VER)
    // Try to remove the target first. It seems it is not
    // possible to rename over existing file, e.g. "log.2009-11-07.1".
    ret = file_remove(backupTarget);
#endif

    // Rename e.g. "log.2009-11-07" to "log.2009-11-07.1".
    ret = file_rename(_scheduledFilename, backupTarget);
    loglog_renaming_result(loglog, _scheduledFilename, backupTarget, ret);

#if defined(_MSC_VER)
    // Try to remove the target first. It seems it is not
    // possible to rename over existing file, e.g. "log.2009-11-07".
    ret = file_remove(_scheduledFilename);
#endif
   
    // Rename filename to scheduledFilename,
    // e.g. rename "log" to "log.2009-11-07".
    loglog.debug("Renaming file " + _filename + " to " + _scheduledFilename);
    ret = file_rename(_filename, _scheduledFilename);
    loglog_renaming_result(loglog, _filename, _scheduledFilename, ret);

    // Open a new file, e.g. "log".
    open(std::ios::out | std::ios::trunc);
    loglog_opening_result(loglog, _out, _filename);

    // Calculate the next rollover time
    helpers::TimeHelper now = TimeHelper::gettimeofday();
    if(now >= _nextRolloverTime)
    {
        _scheduledFilename = getFilename(now);
        _nextRolloverTime = calculateNextRolloverTime(now);
    }
}



TimeHelper DailyRollingFileAppender::calculateNextRolloverTime(const TimeHelper& t) const
{
    switch(_schedule)
    {
    case MONTHLY: 
    {
        struct tm nextMonthTime;
        t.localtime(&nextMonthTime);
        nextMonthTime.tm_mon += 1;
        nextMonthTime.tm_isdst = 0;

        TimeHelper ret;
        if(ret.setTime(&nextMonthTime) == -1) 
		{
            helpers::getLogLog().error(
				"DailyRollingFileAppender::calculateNextRolloverTime()- setTime() returned error");
            // Set next rollover to 31 days in future.
            ret =(t + TimeHelper(2678400));
        }

        return ret;
    }

    case WEEKLY:
        return(t + TimeHelper(7 * 24 * 60 * 60));

    default:
        helpers::getLogLog().error(
            "DailyRollingFileAppender::calculateNextRolloverTime()- invalid schedule value");
        // Fall through.

    case DAILY:
        return(t + TimeHelper(24 * 60 * 60));

    case TWICE_DAILY:
        return(t + TimeHelper(12 * 60 * 60));

    case HOURLY:
        return(t + TimeHelper(60 * 60));

    case MINUTELY:
        return(t + TimeHelper(60));
    };
}



std::string DailyRollingFileAppender::getFilename(const TimeHelper& t) const
{
    char const * pattern = 0;
    switch(_schedule)
    {
    case MONTHLY:
        pattern = "%Y-%m";
        break;

    case WEEKLY:
        pattern = "%Y-%W";
        break;

    default:
        helpers::getLogLog().error(
            "DailyRollingFileAppender::getFilename()- invalid schedule value");
        // Fall through.

    case DAILY:
        pattern = "%Y-%m-%d";
        break;

    case TWICE_DAILY:
        pattern = "%Y-%m-%d-%p";
        break;

    case HOURLY:
        pattern = "%Y-%m-%d-%H";
        break;

    case MINUTELY:
        pattern = "%Y-%m-%d-%H-%M";
        break;
    };

    std::string result(_filename);
    result += ".";
    result += t.getFormattedTime(pattern, false);
    return result;
}

} // namespace log4cplus
