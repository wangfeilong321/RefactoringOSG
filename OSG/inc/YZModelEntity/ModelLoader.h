#ifndef __MODEL_LOADER_H__
#define __MODEL_LOADER_H__
#include <vector>
#include "config.h"
#include "YZModelEntityModule.h"
#include <osg/Node>
#include "IModelLoader.h"
#include "Event.h"
#include <memory>
#include "GraphFactory.h"

namespace bimWorld
{
	class YIZHU_EXPORT ModelLoader : public IModelLoader, public YZModelEntityModule
	{
	public:
		ModelLoader(YZModelEntityInternal* core);
		~ModelLoader();

		virtual bool loadFromDAL(const std::string& dbDir, std::vector<bimWorld::CategoryNode> children) override;

		///////////////////////////////////////////////////////////////////////模型数据加载/////////////////////////////////////////////////////////////////////////////////
		// <加载图形>
		virtual void load(const char *file) override;

		virtual void load(const char **files, int n) override;
		// <加载图形及构件参数>
		virtual void loadAll(const char *baseDir, const char *metaFileName) override;

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
		virtual int loadBySingleFlr(const char *files, double dEngineeringLevel, long& outFloorNode) override;

		virtual bool unLoadBySingleFlr(long floorNode) override;

		virtual void unLoadAll() override;

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
		virtual int loadByMultiFlr(const char **files,
			int ifileSize,
			std::vector<double>& dEngineeringLevelvec,
			std::vector<std::pair<bimWorld::YZString, std::string> >& vecresut) override;

		osg::Node* readNode(const char *file);

		osg::Node* readNode(const char **files, int fileNum);

		virtual  void	readNodeByMultiFle(const char **files, int fileNum) override;

		// <更新模型视图>
		virtual	void	updateMode() override;

		//// <获取构件属性>
		//virtual const AttrConstruct *		getConstruct(const std::string& id);

		core::Event<void()>& UnLoadEvent() override;
		
		core::Event<void(void)>& LoadEvent() override;

		core::Event<void(const std::string[], int)>& SelectionEvent() override;

		// graph select.
		osg::Group* select(const CategoryQueryKey& queryKey);

	protected:
		void initializeCategoryGroups(osg::Node *root);
		
		// <是否已经加载>
		//bool m_isLoaded;

		// <卸载事件>
		core::Event<void()> m_unLoadEvent;

		// <加载事件>
		core::Event<void(void)> m_loadEvent;

		// <选中事件>
		core::Event<void(const std::string[], int)> m_selectionEvent;
		
		std::unique_ptr<bimWorld::GraphFactory> m_graphFactory;
	};
}

#endif // !__MODEL_LOADER_H__
