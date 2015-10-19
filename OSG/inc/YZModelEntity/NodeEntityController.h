#ifndef __CONTROL_BY_DATA_H__
#define __CONTROL_BY_DATA_H__
#include "YZModelEntityModule.h"
#include <mutex>
#include "NodeControllerWrapper.h"
#include <osg/Group>
#include "YZComponent.h"
#include "Event.h"

namespace bimWorld
{
	class YIZHU_EXPORT NodeEntityController :public NodeControllerWrapper
	{
	public:
		NodeEntityController(YZModelEntityInternal* host);

		~NodeEntityController();

		virtual void hide(const std::string& id) override;

		virtual void unHide(const std::string& id) override;
		
		virtual void hideOthers(const std::string& id) override;
		
		virtual void unHideOthers(const std::string& id) override;

		virtual void unHideAll() override;
		
		virtual void toggleHide(const std::string& id) override;

		//// <͸��node�ڵ���������нڵ�>
		//virtual void setOtherNodeToTransparent(const std::string& id) override;

		//// <͸��node�ڵ���������нڵ�>
		//virtual void setOtherNodeToTransparent(const std::string* ids, int length) override;

		virtual void setCurrentDBPath(const std::string& path) override;

		virtual void highlight(const std::string& id) override;

		virtual void unHighlight(const std::string& id) override;

		virtual void highlightWithTopGroup(const std::string& id) override;

		virtual void unHighlightWithTopGroup(const std::string& id) override;

		//virtual void unTransparent(const std::string& id) override;

		virtual void setToTransparent(const std::string& id) override;

		virtual bool revertColorOfNode(const std::string& id) override;

		virtual bool revertColorOfNodes(const std::vector<std::string>& ids) override;
		
		inline void setVisible(const std::string & id, bool visible) override;
		
		void setVisible(const std::string ids[], int n, bool visible) override;
		
		bool isVisible(const std::string & id) override;

		//================================getCategoryKey()================================
		/// @brief <��ȡָ����������ϸ��Ϣ>
		///
		/// <��ȡָ����������ϸ��Ϣ>
		///
		/// @param [in] id <ָ��������id>
		/// @param [out] outKey <Ҫ�����Ĺ�����Ϣ>
		///
		/// @return <����ֵΪTrue˵����ȡ�ɹ��� ����ʧ�ܡ�ʧ�ܿ���������idֵ�Ƿ���ڵ㲻�ٴ��ڵ�ԭ����ɡ�>
		//================================================================================
		virtual bool getCategoryKey(const std::string& id, ConstructKey& outKey) override;

		virtual std::vector<ConstructKey> getCategoryKeys() override;

		virtual bool getVisibility(const std::string& id, bool& visible) override;

		virtual bool setHighLightByCategory(const YZString& floor, const YZString& topCategory, const YZString& secondaryCategory, bool isHighLight) override;

		virtual bool setHighLightByCategory(const CategoryQueryKey& queryKey, bool isHighLight) override;

		virtual void getAll(std::vector<std::string>& ids) const override;

		virtual std::string findIdByNode(void* node) override;

		virtual void* findNodeById(const std::string& id) override;

		//virtual void setCategoryMaterial(const CategoryName& categoryName, const SystemType& systemType, const LevelName& levelName, const MaterialSetting& materialSetting, bool clear = false);
		//virtual void setCategoryMaterial(const ColorCategory& materialSetting);

		//virtual const MaterialSetting*		getCategoryMaterial(const CategoryName& categoryName) const;


		//// <��¥������>
		//virtual void setVisibleByFloor(const YZString& floor, bool visible) override;

		//// <����������>
		//virtual void setVisibleByCategory(const YZString& category, bool visible) override;

		// <��¥�������ʾ����>
		virtual void setVisibleByFloorCategory(const YZString& floor, const YZString& category, bool visible) override;
		
		virtual bool setVisibleByCategory(const CategoryQueryKey& queryKey, bool visible) override;

		virtual void setStateColorToConstructs(const std::string& nodeId, int red, int green, int blue) override;

		virtual bool revertStateColorOfConstructs(const std::string& nodeId) override;

		virtual core::Event<void(std::vector<std::string>, ViewerMode)>& SelectNodeEvent() override;
	
	protected:

		void onSelectNode(std::vector<void*> ids, ViewerMode mode);
		
		bool traverseSubNodesAndFindFirst(osg::Node* node, std::function<bool(osg::Node*)> isGotCallBack, bool includeItsSelf);

		void getCategoryGroups(bimWorld::YZString level, bimWorld::YZString category, std::vector<osg::Group*>& nodevec);
		void findNodesByIds(const std::string ids[], int n, std::vector<void*>& nodes);
		bool isComponent(void* ptr);
		void Search(std::vector<YZ::Component*> coms, std::function<void(YZ::Component*)> func);

		std::string m_currentPrjDBPath;
		std::mutex m_mutex;
		core::Event<void(std::vector<std::string>, bimWorld::ViewerMode)> m_selectNodeEvent;
	};
}
#endif // __CONTROL_BY_DATA_H__