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

		///////////////////////////////////////////////////////////////////////ģ�����ݼ���/////////////////////////////////////////////////////////////////////////////////
		// <����ͼ��>
		virtual void load(const char *file) override;

		virtual void load(const char **files, int n) override;
		// <����ͼ�μ���������>
		virtual void loadAll(const char *baseDir, const char *metaFileName) override;

		//===============================loadBySingleFlr()==============================
		/// @brief <�������>
		///
		/// <�������ģʽ,���ر��㲻��ж�ص�֮ǰ¥��>
		///
		/// @param [in] files <ive�ļ�ȫ·��>
		/// @param [in] dEngineeringLevel <��¥��Ĺ��̱��>
		/// @param [out] outFloorName <��¥��ı�ʶ��>
		///
		/// @return <����ֵ0��ʾ���سɹ������� -1��ʾ����ʧ�ܣ�����1��ʾ��¥���Ѿ����ع������ظ�����>
		///
		//================================================================================
		virtual int loadBySingleFlr(const char *files, double dEngineeringLevel, long& outFloorNode) override;

		virtual bool unLoadBySingleFlr(long floorNode) override;

		virtual void unLoadAll() override;

		//===============================loadByMultiFlr()==============================
		/// @brief <������>
		///
		/// <������ģʽ,���ر��㲻��ж�ص�֮ǰ¥��>
		///
		/// @param [in] files		<ive�ļ�ȫ·��ָ��>
		/// @param [in] ifileSize	<¥���ļ�����>
		/// @param [in] dEngineeringLevelvec <¥��Ĺ��̱������>
		/// @param [in] vecresut	 <¥��������ʵ��¥��>
		/// @return <����ֵ0��ʾ���سɹ������� -1��ʾ����ʧ�ܣ�����n��ʾn��¥���Ѿ����ع������ظ�����>
		/// @ <������������ʱ���õ���>
		//================================================================================
		virtual int loadByMultiFlr(const char **files,
			int ifileSize,
			std::vector<double>& dEngineeringLevelvec,
			std::vector<std::pair<bimWorld::YZString, std::string> >& vecresut) override;

		osg::Node* readNode(const char *file);

		osg::Node* readNode(const char **files, int fileNum);

		virtual  void	readNodeByMultiFle(const char **files, int fileNum) override;

		// <����ģ����ͼ>
		virtual	void	updateMode() override;

		//// <��ȡ��������>
		//virtual const AttrConstruct *		getConstruct(const std::string& id);

		core::Event<void()>& UnLoadEvent() override;
		
		core::Event<void(void)>& LoadEvent() override;

		core::Event<void(const std::string[], int)>& SelectionEvent() override;

		// graph select.
		osg::Group* select(const CategoryQueryKey& queryKey);

	protected:
		void initializeCategoryGroups(osg::Node *root);
		
		// <�Ƿ��Ѿ�����>
		//bool m_isLoaded;

		// <ж���¼�>
		core::Event<void()> m_unLoadEvent;

		// <�����¼�>
		core::Event<void(void)> m_loadEvent;

		// <ѡ���¼�>
		core::Event<void(const std::string[], int)> m_selectionEvent;
		
		std::unique_ptr<bimWorld::GraphFactory> m_graphFactory;
	};
}

#endif // !__MODEL_LOADER_H__
