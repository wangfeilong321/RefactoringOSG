#ifndef __IMODEL_VIEWER_H__
#define __IMODEL_VIEWER_H__
#include "config.h"
#include "IBackgroundSetting.h"
#include "IBillboard.h"
#include "ICameraManipulators.h"
#include "ICameraSettings.h"
#include "IFixedFrameRate.h"
#include "IRenderingThreads.h"
#include "IScreenCapture.h"
//#include "IZoomModel.h"
#include "ISwitchView.h"
#include "IMovingCamera.h"

#include "IKeyFrameAnimation.h"
#include "ICameraAnimationPath.h"

#include "INodeEntityController.h"
#include "IModelLoader.h"
#include "IZoomEntityModel.h"
#include "IProgressInfo.h"

#include "ISelectionController.h"

namespace bimWorld
{
	class YIZHU_EXPORT IModelViewer
	{
	public:

#pragma region Core
		virtual bimWorld::IBackgroundSetting* BackgroundSetting() = 0;
		virtual bimWorld::IBillboard* Billboard() = 0;
		virtual bimWorld::ICameraManipulators* CameraManipulator() = 0;
		virtual bimWorld::ICameraSettings* CameraSetting() = 0;
		virtual bimWorld::IFixedFrameRate* FixedFrameRate() = 0;
		virtual bimWorld::IRenderingThreads* RenderingThread() = 0;
		virtual bimWorld::IScreenCapture* ScreenCapture() = 0;
		//virtual bimWorld::IZoomModel* ZoomModel() = 0;
		virtual bimWorld::ISwitchView* SwitchView() = 0;
		virtual bimWorld::IMovingCamera* MovingCamera() = 0;
#pragma endregion

#pragma region Animation
		virtual bimWorld::Animation::IKeyFrameAnimation* KeyFrameAnimation() = 0;
		virtual bimWorld::Animation::ICameraAnimationPath* CameraPathAnimation() = 0;
#pragma endregion

#pragma region ENTITY
		//bimWorld::IModelENTITY* ModelENTITY() = 0; 
		virtual bimWorld::INodeEntityController* NodeControl() = 0;
		virtual bimWorld::IModelLoader* ModelLoader() = 0;
		virtual bimWorld::IZoomEntityModel* ZoomModel() = 0;
		virtual bimWorld::IProgressInfo* ProgressInfo() = 0;
#pragma endregion
#pragma region BLL
		virtual bimWorld::ISelectionController* SelectionController() = 0;
#pragma endregion

	protected:
	private:
	};
}
#endif // __IMODEL_VIEWER_H__