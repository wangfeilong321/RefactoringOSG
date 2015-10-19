#ifndef __MATERIAL_SETTING_H__
#define __MATERIAL_SETTING_H__
#include "config.h"
#include <osg/StateSet>
#include <mutex>
#include "YZModelCoreModule.h"

namespace bimWorld
{
	class YIZHU_EXPORT MaterialsSetting :public YZModelCoreModule
	{
	public:
		MaterialsSetting(YZModelCoreInternal* host);

		virtual bool						saveMaterialSettingToFile(const char *fileName);

		virtual bool						readMaterialSettingFromFile(const char *fileName);

		osg::StateSet* getSelectionRenderModel();

		void setIsModified(bool modified);

		bool isModified();

	protected:
		// <ʰȡ״̬>
		osg::ref_ptr<osg::StateSet> m_selectionRenderMode;
		std::mutex _lock;
		bool m_materialModified;
	};
}

#endif // __MATERIAL_SETTING_H__