#ifndef __FIXED_FRAME_RATE_H__
#define __FIXED_FRAME_RATE_H__
#include "YZModelCoreModule.h"
#include "FixedFramerateController.h"
#include "IFixedFrameRate.h"

namespace bimWorld
{
	class YIZHU_EXPORT FixedFrameRate :public YZModelCoreModule, public IFixedFrameRate
	{
	public:
		FixedFrameRate(YZModelCoreInternal* host);

		void setupFrameRateController(osg::Node *root);

		virtual void cancelFixedFramerate() override;
		
		virtual void startFixedFramerate() override;

	protected:
		bimWorld::FixedFramerateController	 fixedFramerateController;
	};
}

#endif // __FIXED_FRAME_RATE_H__