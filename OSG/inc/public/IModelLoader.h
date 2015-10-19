#ifndef __IMODEL_LOADER_H__
#define __IMODEL_LOADER_H__
#include <vector>
#include "config.h"
#include "Event.h"
#include "CategoryNode.h"

namespace bimWorld
{
	class YIZHU_EXPORT IModelLoader
	{
	public:
		virtual bool loadFromDAL(const YZString& dbDir, std::vector<bimWorld::CategoryNode> children) = 0;

		virtual core::Event<void()>& UnLoadEvent() = 0;

		virtual core::Event<void(void)>& LoadEvent() = 0;

		virtual core::Event<void(const std::string[], int)>& SelectionEvent() = 0;

		///////////////////////////////////////////////////////////////////////模型数据加载/////////////////////////////////////////////////////////////////////////////////
		// <加载图形>
		virtual void load(const char *file) = 0;

		virtual void load(const char **files, int n) = 0;
		// <加载图形及构件参数>
		virtual void loadAll(const char *baseDir, const char *metaFileName) = 0;

		//===============================loadBySingleFlr()==============================
		/// @brief <单层加载>
		///
		/// <单层加载模式,加载本层不会卸载掉之前楼层>
		///
		/// @param [in] files <ive文件全路径>
		/// @param [in] dEngineeringLevel <此楼层的工程标高>
		/// @param [out] outFloorName <此楼层的标识名>
		///
		/// @return <返回值0表示加载成功，返回 -1表示加载失败，返回1表示改楼层已经加载过不能重复加载>
		///
		//================================================================================
		virtual int  loadBySingleFlr(const char *files, double dEngineeringLevel, long& outFloorNode) = 0;

		virtual bool  unLoadBySingleFlr(long floorNode) = 0;

		virtual void unLoadAll() = 0;

		//===============================loadByMultiFlr()==============================
		/// @brief <多层加载>
		///
		/// <多层加载模式,加载本层不会卸载掉之前楼层>
		///
		/// @param [in] files		<ive文件全路径指针>
		/// @param [in] ifileSize	<楼层文件个数>
		/// @param [in] dEngineeringLevelvec <楼层的工程标高数组>
		/// @param [in] vecresut	 <楼层名称与实际楼层>
		/// @return <返回值0表示加载成功，返回 -1表示加载失败，返回n表示n个楼层已经加载过不能重复加载>
		/// @ <后两个参数暂时不用调用>
		//================================================================================
		virtual int	  loadByMultiFlr(const char **files,
			int ifileSize,
			std::vector<double>& dEngineeringLevelvec,
			std::vector<std::pair<bimWorld::YZString, std::string> >& vecresut) = 0;

		virtual  void	readNodeByMultiFle(const char **files, int fileNum) = 0;

		// <更新模型视图>
		virtual	void	updateMode() = 0;

	};
}

#endif