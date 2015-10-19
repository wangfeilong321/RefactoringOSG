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

		///////////////////////////////////////////////////////////////////////ģ�����ݼ���/////////////////////////////////////////////////////////////////////////////////
		// <����ͼ��>
		virtual void load(const char *file) = 0;

		virtual void load(const char **files, int n) = 0;
		// <����ͼ�μ���������>
		virtual void loadAll(const char *baseDir, const char *metaFileName) = 0;

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
		virtual int  loadBySingleFlr(const char *files, double dEngineeringLevel, long& outFloorNode) = 0;

		virtual bool  unLoadBySingleFlr(long floorNode) = 0;

		virtual void unLoadAll() = 0;

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
		virtual int	  loadByMultiFlr(const char **files,
			int ifileSize,
			std::vector<double>& dEngineeringLevelvec,
			std::vector<std::pair<bimWorld::YZString, std::string> >& vecresut) = 0;

		virtual  void	readNodeByMultiFle(const char **files, int fileNum) = 0;

		// <����ģ����ͼ>
		virtual	void	updateMode() = 0;

	};
}

#endif