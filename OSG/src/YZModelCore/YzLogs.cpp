#include "YzLogs.h"

using namespace log4cplus;
using namespace log4cplus::helpers;

//#define WARN(function, message) \
//Logger logger = Logger::getInstance(LOG4CPLUS_TEXT(#function));\
//LOG4CPLUS_WARN(logger, LOG4CPLUS_TEXT(message));
//
//#define ERROR(function, message) \
//Logger logger = Logger::getInstance(LOG4CPLUS_TEXT(#function));\
//LOG4CPLUS_ERROR(logger, LOG4CPLUS_TEXT(message));

#define LOG(level, functionName, message) \
initInstance(); \
Logger logger = Logger::getInstance(functionName); \
level(logger, message);

//static YzLogs* s_instance = NULL;

YzLogs::YzLogs(void)
{
	log4cplus::initialize();
	//BasicConfigurator config;
	//config.configure();
	
	Logger root = Logger::getRoot();
	//log4cplus::Logger logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("main"));
	//LOG4CPLUS_WARN(root, LOG4CPLUS_TEXT("hi"));
	LOG4CPLUS_DEBUG(root, "This is a bool: " << true);
}


YzLogs::~YzLogs(void)
{
}

//void YzLogs::trace(const log4cplus::tstring& functionName, const log4cplus::tstring& message)
//{
//	LOG(LOG4CPLUS_TRACE, functionName, message);
//}
//
//void YzLogs::debug(const log4cplus::tstring& functionName, const log4cplus::tstring& message)
//{
//	LOG(LOG4CPLUS_DEBUG, functionName, message);
//}
//
//void YzLogs::info(const log4cplus::tstring& functionName, const log4cplus::tstring& message)
//{
//	LOG(LOG4CPLUS_INFO, functionName, message);
//}
//
//void YzLogs::warn(const log4cplus::tstring& functionName, const log4cplus::tstring& message)
//{
//	//LOG(LOG4CPLUS_WARN, functionName, message);
//	initInstance();
//	Logger logger = Logger::getInstance(functionName);
//	LOG4CPLUS_WARN(logger, message);
//}
//
//void YzLogs::error(const log4cplus::tstring& functionName, const log4cplus::tstring& message)
//{
//	LOG(LOG4CPLUS_ERROR, functionName, message);
//}
//
//void YzLogs::fatal(const log4cplus::tstring& functionName, const log4cplus::tstring& message)
//{
//	LOG(LOG4CPLUS_FATAL, functionName, message);
//}