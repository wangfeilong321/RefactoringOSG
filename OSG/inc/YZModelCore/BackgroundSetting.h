#ifndef __BACKGROUND_SETTING_H__
#define __BACKGROUND_SETTING_H__
#include "IBackgroundSetting.h"
#include "YZModelCoreModule.h"
#include <memory>
#include <osg/Node>

namespace bimWorld
{

	class YIZHU_EXPORT BackgroundSetting: public YZModelCoreModule, public IBackgroundSetting
	{
	public:

		BackgroundSetting(YZModelCoreInternal* host);

		bool getBackgroundColor(int& red, int& green, int& blue, int& alpha) override;

		void setBackgroundColor(int red, int green, int blue, int alpha) override;

		void revertBackgroundColor() override;

		void setSkyBox(const std::string& hdr) override;

		void setSkyBox(const std::string& right, const std::string& left, const std::string& front, const std::string& back, const std::string& up, const std::string& down) override;

		const osg::Vec4f getDefaultBG();

	protected:
		// <Ìì¿ÕºÐ>
		osg::ref_ptr<osg::Node> m_skyBox;
	};
}

#endif // __BACKGROUND_SETTING_H__