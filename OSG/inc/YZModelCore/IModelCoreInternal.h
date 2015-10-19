#ifndef __YZMODEL_CORE_INTERNAL_H__
#define __YZMODEL_CORE_INTERNAL_H__


#include <memory>
#include "IModelCore.h"
#include "IBillboard.h"
#include "IBackgroundSetting.h"
#include "ICameraManipulators.h"
#include "IRenderingThreads.h"
#include "ICoordinate.h"
#include "INodeController.h"
#include "IFixedFrameRate.h"
#include "IScreenCapture.h"
#include "IZoomModel.h"

#include "IViewerDataModel.h"
#include "ICameraSettings.h"



namespace bimWorld
{
	class YIZHU_EXPORT IModelCoreInternal :public IModelCore
	{
	public:
		// <viewer data model ���������ⲿ>
		virtual std::shared_ptr<bimWorld::IViewerDataModel> ViewerData() = 0;

		virtual std::shared_ptr<bimWorld::ICoordinate> Coordinate() = 0;

	};
}

#endif // __YZMODEL_CORE_INTERNAL_H__
