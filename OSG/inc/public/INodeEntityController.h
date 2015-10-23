#ifndef __ICONTROL_BY_DATA_H__
#define __ICONTROL_BY_DATA_H__
#include <vector>
#include <string>
#include "config.h"
#include "ConstructKey.h"
#include "INodeController.h"
#include "Event.h"
#include "DataModel.h"
#include "CategoryQueryKey.h"

namespace bimWorld
{
	class YIZHU_EXPORT INodeEntityController:public INodeController
	{
	public:

		virtual void hide(const std::string& id) = 0;

		virtual void unHide(const std::string& id) = 0;

		virtual void hideOthers(const std::string& id) = 0;

		virtual void unHideOthers(const std::string& id) = 0;

		virtual void hideOthers(const std::vector<std::string>& ids) = 0;

		virtual void unHideOthers(const std::vector<std::string>& ids) = 0;

		virtual void toggleHide(const std::string& id) = 0;

		//// <͸��node�ڵ���������нڵ�>
		//virtual void setOtherNodeToTransparent(const std::string& id) = 0;

		//// <͸��node�ڵ���������нڵ�>
		//virtual void setOtherNodeToTransparent(const std::string* ids, int length) = 0;

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
		virtual bool getCategoryKey(const std::string& id, ConstructKey& outKey) = 0;

		virtual std::vector<ConstructKey> getCategoryKeys() = 0;

		virtual bool getVisibility(const std::string& id, bool& visible) = 0;

		virtual bool setHighLightByCategory(const YZString& floor, const YZString& topCategory, const YZString& secondaryCategory, bool isHighLight) = 0;
		
		virtual bool setHighLightByCategory(const CategoryQueryKey& queryKey, bool isHighLight) = 0;

		virtual void getAll(std::vector<std::string>& ids) const = 0;

		virtual std::string findIdByNode(void* node) = 0;

		virtual void* findNodeById(const std::string& id) = 0;

		//virtual void setCategoryMaterial(const CategoryName& categoryName, const SystemType& systemType, const LevelName& levelName, const MaterialSetting& materialSetting, bool clear = false);
		//virtual void setCategoryMaterial(const ColorCategory& materialSetting);

		//virtual const MaterialSetting*		getCategoryMaterial(const CategoryName& categoryName) const;

		//// <��¥������>
		//virtual void setVisibleByFloor(const YZString& floor, bool visible) = 0;

		//// <����������>
		//virtual void setVisibleByCategory(const YZString& category, bool visible) = 0;

		// <��¥�������ʾ����>
		virtual void setVisibleByFloorCategory(const YZString& floor, const YZString& category, bool visible) = 0;
		
		virtual bool setVisibleByCategory(const CategoryQueryKey& queryKey, bool visible) = 0;

		virtual void setVisible(const std::string& id, bool visible) = 0;

		virtual void setVisible(const std::string *ids, int n, bool visible) = 0;

		virtual bool isVisible(const std::string& id) = 0;

		///////////////////////////////////////�ڵ������ز���//////////////////////////////////////

		virtual void setStateColorToConstructs(const std::string& nodeId, int red, int green, int blue) = 0;

		virtual bool revertStateColorOfConstructs(const std::string& nodeId) = 0;

		virtual void highlight(const std::string& id) = 0;

		virtual void unHighlight(const std::string& id) = 0;

		virtual void highlightWithTopGroup(const std::string& id) = 0;

		virtual void unHighlightWithTopGroup(const std::string& id) = 0;

		//virtual void unTransparent(const std::string& id) = 0;

		virtual void setToTransparent(const std::string& id) = 0;
		
		virtual void setCurrentDBPath(const std::string& path) = 0;

		virtual bool revertColorOfNode(const std::string& id) = 0;

		virtual bool revertColorOfNodes(const std::vector<std::string>& ids) = 0;
		
		virtual core::Event<void(std::vector<std::string>, ViewerMode)>& SelectNodeEvent() = 0;

		virtual void setSelectedCenter(const std::string& id) = 0;
	};
}
#endif // __ICONTROL_BY_DATA_H__