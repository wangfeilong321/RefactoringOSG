#ifndef INTERNAL_OF_MODEL_CORE
#define INTERNAL_OF_MODEL_CORE

#include "Billboard.h"
#include "BackgroundSetting.h"
#include "CameraManipulators.h"
#include "ViewerDataModel.h"
#include "RenderingThreads.h"
#include "Coordinate.h"
#include "NodeController.h"
#include "FixedFrameRate.h"
#include "ScreenCapture.h"
#include "CameraSettings.h"
#include "ZoomModel.h"
#include "SwitchView.h"
#include "MovingCamera.h"

#include "IModelCoreInternal.h"
#include "IBillboard.h"
#include "IBackgroundSetting.h"
#include "ICameraManipulators.h"
#include "IRenderingThreads.h"
#include "ICoordinate.h"
#include "INodeController.h"
#include "IFixedFrameRate.h"
#include "IScreenCapture.h"
#include "ISwitchView.h"
#include "IMovingCamera.h"

#include "IViewerDataModel.h"

#include "InteractionDefine.h"
#include "MaterialSetting.h"

namespace bimWorld
{
	class YIZHU_EXPORT YZModelCoreInternal : public IModelCoreInternal
	{
	public:
		YZModelCoreInternal();

		~YZModelCoreInternal();

		SHARED_PROPERTY(bimWorld::ViewerDataModel, _ViewerData, m_viewerData, this);

		SHARED_PROPERTY(bimWorld::CameraManipulator, _CameraManipulator, m_cameraManip, this);

		SHARED_PROPERTY(bimWorld::FixedFrameRate, _FixedFrameRate, m_fixedFramerate, this);

		SHARED_PROPERTY(bimWorld::RenderingThreads, _RenderingThreads, m_renderingThreads, this);

		SHARED_PROPERTY(bimWorld::BackgroundSetting, _BackgroundSetting, m_backgroundSetting, this);

		SHARED_PROPERTY(bimWorld::Coordinate, _Coordinate, m_coordinate, this);

		SHARED_PROPERTY(bimWorld::Billboard, _Billboard, m_billboard, this);

		SHARED_PROPERTY(bimWorld::NodeController, _NodeControl, m_nodeController, this);

		SHARED_PROPERTY(bimWorld::ScreenCapture, _ScreenCapture, m_screenCapture, this);

		SHARED_PROPERTY(bimWorld::CameraSettings, _CameraSettings, m_cameraSettings, this);

		SHARED_PROPERTY(bimWorld::ZoomModel, _ZoomModel, m_zoomModel, this);

		SHARED_PROPERTY(bimWorld::SwitchView, _SwitchView, m_switchView, this);

		SHARED_PROPERTY(bimWorld::MovingCamera, _MovingCamera, m_movingCamera, this);

		IMPLEMENT_INTERFACE(bimWorld::IViewerDataModel, _ViewerData, ViewerData);


		IMPLEMENT_INTERFACE(bimWorld::IScreenCapture, _ScreenCapture, ScreenCapture);
		
		IMPLEMENT_INTERFACE(bimWorld::ICameraSettings, _CameraSettings, CameraSettings);
		
		IMPLEMENT_INTERFACE(bimWorld::ICameraManipulators, _CameraManipulator, CameraManipulator);

		IMPLEMENT_INTERFACE(bimWorld::IFixedFrameRate, _FixedFrameRate, FixedFrameRate);

		IMPLEMENT_INTERFACE(bimWorld::IRenderingThreads, _RenderingThreads, RenderingThreads);

		IMPLEMENT_INTERFACE(bimWorld::IBackgroundSetting, _BackgroundSetting, BackgroundSetting);

		IMPLEMENT_INTERFACE(bimWorld::ICoordinate, _Coordinate, Coordinate);

		IMPLEMENT_INTERFACE(bimWorld::IBillboard, _Billboard, Billboard);

		IMPLEMENT_INTERFACE(bimWorld::INodeController, _NodeControl, NodeControl);

		IMPLEMENT_INTERFACE(bimWorld::IZoomModel, _ZoomModel, ZoomModel);
		
		IMPLEMENT_INTERFACE(bimWorld::ISwitchView, _SwitchView, SwitchView);

		IMPLEMENT_INTERFACE(bimWorld::IMovingCamera, _MovingCamera, MovingCamera);

	//internal:

		SHARED_PROPERTY(bimWorld::MaterialsSetting, _MaterialSetting, m_materialSetting, this);

	};
}


#endif // INTERNAL_OF_MODEL_CORE
