#include "PointerAccess.h"
#include "sqlitehelper.h"
//#include <functional>
//#include <std::mutex>
#include <sqlite3.h>

std::vector<std::map<std::string, std::string>>* SqliteHelper::m_selectResult = new std::vector < std::map<std::string, std::string> >();
std::mutex _lock;

SqliteHelper::SqliteHelper(const char* dbPath)
{
	m_selectResult = NULL;
	m_dataBase = nullptr;
	m_path = dbPath;
	init();
}

SqliteHelper::~SqliteHelper()
{
	close();
}

////////////////////////////////////////////////////////////////////////////////////////////
// <参数说明>:
// pInMemory: <指向内存数据库指针>
// zFilename: <指向文件数据库目录的字符串指针>
// isSave  0: <从文件数据库载入到内存数据库 1：从内存数据库备份到文件数据库>
////////////////////////////////////////////////////////////////////////////////////////////
int loadOrSaveDb(sqlite3 *pInMemory, const char *zFilename, int isSave){
	int rc;                   /* Function return code */
	sqlite3 *pFile;           /* Database connection opened on zFilename */
	sqlite3_backup *pBackup;  /* Backup object used to copy data */
	sqlite3 *pTo;             /* Database to copy to (pFile or pInMemory) */
	sqlite3 *pFrom;           /* Database to copy from (pFile or pInMemory) */

	/* Open the database file identified by zFilename. Exit early if this fails
	** for any reason. */
	rc = sqlite3_open(zFilename, &pFile);
	if (rc == SQLITE_OK){

		/* If this is a 'load' operation (isSave==0), then data is copied
		** from the database file just opened to database pInMemory.
		** Otherwise, if this is a 'save' operation (isSave==1), then data
		** is copied from pInMemory to pFile.  Set the variables pFrom and
		** pTo accordingly. */
		pFrom = (isSave ? pInMemory : pFile);
		pTo = (isSave ? pFile : pInMemory);

		/* Set up the backup procedure to copy from the "main" database of
		** connection pFile to the main database of connection pInMemory.
		** If something goes wrong, pBackup will be set to NULL and an error
		** code and  message left in connection pTo.
		**
		** If the backup object is successfully created, call backup_step()
		** to copy data from pFile to pInMemory. Then call backup_finish()
		** to release resources associated with the pBackup object.  If an
		** error occurred, then  an error code and message will be left in
		** connection pTo. If no error occurred, then the error code belonging
		** to pTo is set to SQLITE_OK.
		*/
		pBackup = sqlite3_backup_init(pTo, "main", pFrom, "main");
		if (pBackup){
			(void)sqlite3_backup_step(pBackup, -1);
			(void)sqlite3_backup_finish(pBackup);
		}
		rc = sqlite3_errcode(pTo);
	}

	/* Close the database connection opened on database file zFilename
	** and return the result of this function. */
	(void)sqlite3_close(pFile);
	return rc;
}


bool SqliteHelper::init()
//: QObject(parent)
{
	int rc;
	sqlite3* db;
	rc = sqlite3_enable_shared_cache(true);
	if (rc){
#ifdef TraceSql
		fprintf(stderr, "Can't enable shared cache.\n");
#endif // TraceSql

		//exit(0);
	}
	//rc = sqlite3_open(m_path, &db);//"E:\\software\\Sqliteman-1.2.2\\test.db"
	rc = sqlite3_open(m_path.c_str(), &db);//:memory://file::memory:?cache=shared

	if (rc){
#ifdef TraceSql
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
#endif // TraceSql

		//exit(0);
		return false;
	}

#ifdef TraceSql
	fprintf(stderr, "Opened database successfully\n");
#endif // TraceSql

	//loadOrSaveDb(db, m_path.c_str(), 0);

	m_dataBase = db;
	return true;
}


void SqliteHelper::close()
{
	auto db = static_cast<sqlite3*>(m_dataBase);

	//loadOrSaveDb(db, m_path.c_str(), 0);
	if (!db)
		return;
	sqlite3_close(db);
	//if (s_selectResult)
	//{
	//	delete s_selectResult;
	//	s_selectResult = nullptr;
	//}
}

typedef int(*sqlite3_callback)(
	void*,    /* Data provided in the 4th argument of sqlite3_exec() */
	int,      /* The number of columns in row */
	char**,   /* An array of strings representing fields in the row */
	char**    /* An array of strings representing column names */
	);

int callback(void *externData, int columnNum, char **data, char **columnNames){
	//_lock.lock();
	// fprintf(stderr, "%s: ", (const char*)externData);
	auto helper = static_cast<SqliteHelper*>(externData);
	helper->handleSelectCallBack(helper, columnNum, data, columnNames);
	//auto func_ptr = static_cast<std::function<int(void *, int, char **, char **)>*>(externData);
	//return (*func_ptr)(nullptr, columnNum, data, columnNames);
	//for (int i = 0; i < columnNum; i++){
	//	printf("%s = %s\n", columnNames[i], data[i] ? data[i] : "NULL");
	//}
	//printf("\n");
	//_lock.unlock();
	return 0;
}

bool SqliteHelper::exec(const std::string& sqlStr, std::vector<std::map<std::string, std::string>>& result)
{

	const char* data = "Callback function called";
	int rc;
	char *zErrMsg = 0;
	const char *sql;
	sql = sqlStr.c_str();
	m_selectResult = new std::vector<std::map<std::string, std::string>>();

	auto db = static_cast<sqlite3*>(m_dataBase);
	if (!db)
		return false;
	/* Execute SQL statement */
	rc = sqlite3_exec(db, sql, callback, (void*)callback, &zErrMsg);
	if (rc != SQLITE_OK){
#ifdef TraceSql
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
#endif // TraceSql

		sqlite3_free(zErrMsg);
		return false;
	}
	else{
#ifdef TraceSql
		fprintf(stdout, "Operation done successfully\n");
#endif // TraceSql

	}
	if (!m_selectResult || 0 != IsBadWritePtr(m_selectResult, 1) || 0 != IsBadReadPtr(m_selectResult, 1))
	{
		auto badW = IsBadWritePtr(m_selectResult, 1);
		auto badR = IsBadReadPtr(m_selectResult, 1);
		return false;
	}
	result = *m_selectResult;
	delete m_selectResult;
	return true;
}

void SqliteHelper::handleSelectCallBack(SqliteHelper* sender, int columnNum, char **data, char **columnNames)
{
	_lock.lock();
	int i;
	//fprintf(stderr, "%s: ", (const char*)externData);
	std::map<std::string, std::string> map;
	for (i = 0; i < columnNum; i++){
#ifdef TraceSql
		printf("%s = %s\n", columnNames[i], data[i] ? data[i] : "NULL");
#endif // TraceSql

		if (data[i])
			map[columnNames[i]] = data[i];
	}
	sender->m_selectResult->push_back(map);
#ifdef TraceSql
	printf("\n");
#endif // TraceSql

	//return 0;
	_lock.unlock();
}

bool SqliteHelper::execNoneQuery(const std::string& sqlStr)
{
	int rc;
	char *zErrMsg = 0;
	const char *sql;
	sql = sqlStr.c_str();
	auto db = static_cast<sqlite3*>(m_dataBase);
	if (!db)
		return false;
	/* Execute SQL statement */
#ifdef TraceSql
	rc = sqlite3_exec(db, sql, NULL, NULL, &zErrMsg);
#else
	rc = sqlite3_exec(db, sql, NULL, NULL, NULL);
#endif // TraceSql

	if (rc != SQLITE_OK){

#ifdef TraceSql
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
#endif // TraceSql

		return false;
	}
	else{
#ifdef TraceSql
		fprintf(stdout, "Operation done successfully\n");
#endif // TraceSql

	}
	return true;
}
