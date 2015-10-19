#include <ctime>  
#include "yzlogs.h"
#include "sqlitehelper.h"
#include <sstream>
#include <direct.h>
#include "StringExt.h"
#include <assert.h>
//#include <fstream>
#define  LogDBPath "logs\\applog.db"

SqliteHelper* helper = NULL;

YZLogs::YZLogs(const std::string& author)
{
	m_author = author;
	mkdir("logs");
	//std::fstream fs;
	//fs.open(LogDBPath, std::fstream::out | std::fstream::app);
	//fs.close();

	helper = new SqliteHelper(LogDBPath);
	std::vector<std::map<std::string, std::string>> queryResult;
	helper->exec("CREATE TABLE IF NOT EXISTS ExcutionDetails(\"Id\" INTEGER PRIMARY KEY AUTOINCREMENT,\"Msg\" TEXT,\"ClassPos\" TEXT,\"FunctionPos\" TEXT,\"Author\" TEXT,\"StartTime\" DATETIME,\"EndTime\" DATETIME,\"ElapsedSeconds\" DATETIME, \"ElapsedMilliseconds\" DATETIME);", queryResult);
	helper->exec("CREATE TABLE IF NOT EXISTS Infos(\"Id\" INTEGER PRIMARY KEY AUTOINCREMENT,\"Msg\" TEXT,\"ClassPos\" TEXT,\"FunctionPos\" TEXT,\"Author\" TEXT,\"RecordTime\" DATETIME);", queryResult);
	helper->exec("CREATE TABLE IF NOT EXISTS Warnings(\"Id\" INTEGER PRIMARY KEY AUTOINCREMENT,\"Msg\" TEXT,\"ClassPos\" TEXT,\"FunctionPos\" TEXT,\"Author\" TEXT,\"RecordTime\" DATETIME);", queryResult);
	helper->exec("CREATE TABLE IF NOT EXISTS Errors(\"Id\" INTEGER PRIMARY KEY AUTOINCREMENT,\"Msg\" TEXT,\"ClassPos\" TEXT,\"FunctionPos\" TEXT,\"Author\" TEXT,\"RecordTime\" DATETIME);", queryResult);
}

YZLogs::~YZLogs()
{
	if (helper)
	{
		delete helper;
		helper = NULL;
	}
}

int YZLogs::ExcutionStart(const std::ostringstream& msg, const std::string& classPos, const std::string& funcPos) const
{
#ifdef YZLOGS_RECORD_EXECUTION
	std::vector<std::map<std::string, std::string>> queryResult;
	std::ostringstream stream;
	stream << "insert into ExcutionDetails(Msg, ClassPos, FunctionPos, Author, StartTime)VALUES(";
	stream << "'" << msg.str() << "', ";
	stream << "'" << classPos << "', ";
	stream << "'" << funcPos << "', ";
	stream << "'" << m_author << "', ";
	stream << "STRFTIME('%Y-%m-%d %H:%M:%f', 'NOW')); select last_insert_rowid() as Id;";
	if (!helper->exec(stream.str(), queryResult))
	{
		assert(false);
		return -1;
	}
	if (queryResult.size() == 0 || queryResult[0].size() == 0)
	{
		assert(false);
		return -1;
	}
	auto idStr = queryResult[0]["Id"];
	int result;
	if(!util::StringExtension::parseToInt(idStr, result))
		return -1;
	return result;
#else
	return -1;
#endif
}

void YZLogs::ExcutionEnd(int excutionId) const
{
#ifdef YZLOGS_RECORD_EXECUTION
	std::vector<std::map<std::string, std::string>> queryResult;
	std::ostringstream stream;
	stream << "update ExcutionDetails set EndTime = STRFTIME('%Y-%m-%d %H:%M:%f', 'NOW') where id = " << excutionId << ";";
	stream << "update ExcutionDetails set ElapsedSeconds = strftime('%s',EndTime) - strftime('%s',StartTime) where id = " << excutionId << ";";
	stream << "update ExcutionDetails set ElapsedMilliseconds = strftime('%f',EndTime)*1000 - strftime('%f',StartTime)*1000 where id = " << excutionId << ";";
	if (!helper->exec(stream.str(), queryResult))
	{
		assert(false);
		return;
	}
#endif
}

void YZLogs::Infos(const std::ostringstream& msg, const std::string& classPos, const std::string& funcPos)
{
#ifdef YZLOGS_RECORD_INFOS
	std::vector<std::map<std::string, std::string>> queryResult;
	std::ostringstream stream;
	stream << "insert into Infos(Msg, ClassPos, FunctionPos, Author, RecordTime)VALUES(";
	stream << "'" << msg.str() << "', ";
	stream << "'" << classPos << "', ";
	stream << "'" << funcPos << "', ";
	stream << "'" << m_author << "', ";
	stream << "STRFTIME('%Y-%m-%d %H:%M:%f', 'NOW'));";
	helper->exec(stream.str(), queryResult);
#endif
}

void YZLogs::Warn(const std::ostringstream& msg, const std::string& classPos, const std::string& funcPos)
{
#ifdef YZLOGS_RECORD_WARNS
	std::vector<std::map<std::string, std::string>> queryResult;
	std::ostringstream stream;
	stream << "insert into Warnings(Msg,ClassPos,FunctionPos, Author,RecordTime)VALUES(";
	stream << "'" << msg.str() << "', ";
	stream << "'" << classPos << "', ";
	stream << "'" << funcPos << "', ";
	stream << "'" << m_author << "', ";
	stream << "STRFTIME('%Y-%m-%d %H:%M:%f', 'NOW'));";
	helper->exec(stream.str(), queryResult);
#endif
}

void YZLogs::Errors(const std::ostringstream& msg, const std::string& classPos, const std::string& funcPos)
{
#ifdef YZLOGS_RECORD_ERRORS
	std::vector<std::map<std::string, std::string>> queryResult;
	std::ostringstream stream;
	stream << "insert into Errors(Msg,ClassPos,FunctionPos, Author,RecordTime)VALUES(";
	stream << "'" << msg.str() << "', ";
	stream << "'" << classPos << "', ";
	stream << "'" << funcPos << "', ";
	stream << "'" << m_author << "', ";
	stream << "STRFTIME('%Y-%m-%d %H:%M:%f', 'NOW'));";
	helper->exec(stream.str(), queryResult);
#endif
}
