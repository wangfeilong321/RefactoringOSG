#include "EntityHelper.h"
#include <sstream>
#include "sqlitehelper.h"

//void EntityHelper::getEntityproperties(const char* dbPath, QVector<QMap<QString, QString> >& result)
//{
//	SqliteHelper helper(dbPath);
//	std::vector<std::map<std::string, std::string> > queryResult;
//	helper.exec("select* from entityproperties where Propertieskey = '×¢ÊÍ'", queryResult);
//	for (int i = 0; i < queryResult.size();i++)
//	{
//		QMap<QString, QString> tempMap;
//		for (auto iter = queryResult[i].begin(); iter != queryResult[i].end();iter++)
//		{
//			tempMap[QString::fromStdString(iter->first)] = QString::fromStdString(iter->second);
//		}
//		result.push_back(tempMap);
//	}
//}
//
//bool EntityHelper::getEntityGroups(const char* dbPath, QString entityId, QMap<QString, QPair<QString, QString> >& outResult)
//{
//	outResult.clear();
//	SqliteHelper helper(dbPath);
//	std::vector<std::map<std::string, std::string> > result;
//	std::string sql = "select e1.Name as GroupName, e2.EntityID, e1.EntityID as GroupID from entity as e1 join entity as e2 on (e1.EntityID == e2.BelongGroupID and e2.IsGroup = 0 and e2.BelongGroupID != -1)";
//	if (!helper.exec(sql, result))
//		return false;
//	if (result.size() == 0 || result[0].size() == 0)
//		return false;
//	for (int rowLength = 0; rowLength < result.size(); rowLength++)
//	{
//		outResult[QString::fromStdString(result[rowLength]["EntityID"])] = QPair<QString, QString>(QString::fromStdString(result[rowLength]["GroupID"]), QString::fromStdString(result[rowLength]["GroupName"]));
//	}
//	return true;
//}
//
//bool EntityHelper::GetAllEntityClassInfo(QString strDBPath, QMap<QString, SEntityDBClass>& outResult)
//{
//	outResult.clear();
//	SqliteHelper helper(strDBPath.toStdString().c_str());
//	std::vector<std::map<std::string, std::string> > result;
//	std::string sql = "select UniqueId, Name ,SystemType, Category, LevelName, EntityID,BelongGroupID from entity";
//	if (!helper.exec(sql, result))
//		return false;
//	if (result.size() == 0 || result[0].size() == 0)
//		return false;
//	for (int rowLength = 0; rowLength < result.size(); rowLength++)
//	{
//		SEntityDBClass sNewData;
//		sNewData.strName = QString::fromStdString(result[rowLength]["Name"]);
//		sNewData.strSysTemType = QString::fromStdString(result[rowLength]["SystemType"]);
//		sNewData.strCategory = QString::fromStdString(result[rowLength]["Category"]);
//		sNewData.strLevelName = QString::fromStdString(result[rowLength]["LevelName"]);
//		sNewData.strGroupId = QString::fromStdString(result[rowLength]["BelongGroupID"]);
//		sNewData.strId = QString::fromStdString(result[rowLength]["EntityID"]);
//		
//		outResult.insert(QString::fromStdString(result[rowLength]["UniqueId"]), sNewData);
//	}
//	return true;
//}
//
//bool EntityHelper::GetEntityAttInfo(QString strDBPath, QString strEntityId, QMap<QString, QString>& outResult)
//{
//	outResult.clear();
//	SqliteHelper helper(strDBPath.toStdString().c_str());
//	std::vector<std::map<std::string, std::string> > result;
//	QString strSql("select * from entityproperties where UniqueId = '%1'");
//	strSql = strSql.arg(strEntityId);
//	std::string sql = strSql.toStdString();
//	if (!helper.exec(sql, result))
//		return false;
//	if (result.size() == 0 || result[0].size() == 0)
//		return false;
//	for (int rowLength = 0; rowLength < result.size(); rowLength++)
//	{
//		QString strKey, strValue;
//		auto ItrFind = result[rowLength].find("Propertieskey");
//		if (ItrFind != result[rowLength].end())
//		{
//			strKey = QString::fromStdString(ItrFind->second);
//		}
//
//		ItrFind = result[rowLength].find("PropertiesValue");
//		if (ItrFind != result[rowLength].end())
//		{
//			strValue = QString::fromStdString(ItrFind->second);
//		}
//
//		outResult.insert(strKey, strValue);
//	}
//	return true;
//}

bool EntityHelper::getAllCategory(const char* dbPath, std::vector<std::map<std::string, std::string> >& queryResult)
{
	SqliteHelper helper(dbPath);
	// select DISTINCT Name, LevelName, Category from entity where IsGroup = 0
	return helper.exec("select DISTINCT entity.Name, floor.LevelName, category.Category from entity, floor, category where entity.IsGroup = 0 and entity.CategoryId = category.CategoryId and entity.FlrId = floor.flrId", queryResult);
}

//void EntityHelper::getIdMaterialMap(const char * dbPath, QVector<QMap<QString, QString>>& result)
//{
//	SqliteHelper helper(dbPath);
//	std::vector<std::map<std::string, std::string> > queryResult;
//	helper.exec("select * from entitycolor", queryResult);
//	for (int i = 0; i < queryResult.size(); i++)
//	{
//		QMap<QString, QString> tempMap;
//		for (auto iter = queryResult[i].begin(); iter != queryResult[i].end(); iter++)
//		{
//			tempMap[QString::fromStdString(iter->first)] = QString::fromStdString(iter->second);
//		}
//		result.push_back(tempMap);
//	}
//}
//
//bool EntityHelper::getGroupsByCategory(const char * dbPath, const QString& levelName, const QString& topCategory, const QString& secCategory, QVector<QMap<QString, QString>>& result)
//{
//	std::wostringstream stream;
//	stream << "select UniqueId from entity INDEXED BY Entity_Index_LCS where ";//INDEXED BY Category_Index 
//	if (!levelName.isEmpty())
//	{
//		stream << "LevelName = '" << levelName.toStdWString() << "'";
//	}
//	else
//	{
//		stream << "LevelName GLOB '*'";
//	}
//	stream << " and ";
//	if (!topCategory.isEmpty())
//	{
//		stream << "Category = '" << topCategory.toStdWString() << "'";
//	}
//	else
//	{
//		stream << "Category GLOB '*'";
//	}
//	stream << " and ";
//	if (!secCategory.isEmpty())
//	{
//		stream << "Name = '" << secCategory.toStdWString() << "'";
//	}
//	else
//	{
//		stream << "Name GLOB '*'";
//	}
//	stream << ";";
//
//	SqliteHelper helper(dbPath);
//	std::vector<std::map<std::string, std::string> > queryResult;
//	if(!helper.exec(QString::fromStdWString(stream.str()).toStdString(), queryResult))
//		return false;
//	for (int i = 0; i < queryResult.size(); i++)
//	{
//		QMap<QString, QString> tempMap;
//		for (auto iter = queryResult[i].begin(); iter != queryResult[i].end(); iter++)
//		{
//			tempMap[QString::fromStdString(iter->first)] = QString::fromStdString(iter->second);
//		}
//		result.push_back(tempMap);
//	}
//	return true;
//}
//
//bool EntityHelper::getNodesByCategory(const char * dbPath, const QString& levelName, const QString& topCategory, const QString& secCategory, QVector<QMap<QString, QString>>& result)
//{
//	std::ostringstream stream;
//	//stream << "select UniqueId from entity where BelongGroupID in (select UniqueId from entity where ";//INDEXED BY Category_Index 
//	//stream << "select e1.UniqueId from entity as e1 where e1.BelongGroupID in (select e2.UniqueId from entity as e2 where ";
//	stream << "select e2.UniqueId from entity as e2 INDEXED BY Entity_Index_LCS where IsGroup = 0 and ";
//	if (!levelName.isEmpty())
//	{
//		stream << "e2.LevelName = '" << levelName.toStdString() << "'";
//	}
//	else
//	{
//		stream << "e2.LevelName in (select DISTINCT e3.LevelName from entity as e3)";
//	}
//	stream << " and ";
//	if (!topCategory.isEmpty())
//	{
//		stream << "e2.Category = '" << topCategory.toStdString() << "'";
//	}
//	else
//	{
//		stream << "e2.Category in (select DISTINCT e3.Category from entity as e3)";
//	}
//	stream << " and ";
//	if (!secCategory.isEmpty())
//	{
//		stream << "e2.Name = '" << secCategory.toStdString() << "'";
//	}
//	else
//	{
//		stream << "e2.Name in (select DISTINCT e3.Name from entity as e3)";
//	}
//	stream << ";";
//
//	SqliteHelper helper(dbPath);
//	std::vector<std::map<std::string, std::string> > queryResult;
//	if (!helper.exec(stream.str(), queryResult))
//		return false;
//	for (int i = 0; i < queryResult.size(); i++)
//	{
//		QMap<QString, QString> tempMap;
//		for (auto iter = queryResult[i].begin(); iter != queryResult[i].end(); iter++)
//		{
//			tempMap[QString::fromStdString(iter->first)] = QString::fromStdString(iter->second);
//		}
//		result.push_back(tempMap);
//	}
//	return true;
//}
