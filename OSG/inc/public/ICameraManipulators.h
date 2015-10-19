#ifndef __ICAMERA_MANIPULATORS_H__
#define __ICAMERA_MANIPULATORS_H__
#include "config.h"
#include "ManipulatorType.h"
#include "DataModel.h"
#include "IZoomModel.h"
#include <memory>
#include "MouseKeySymbol.h"
#include "CameraOperationTypes.h"
//#include "ICameraAnimation.h"

namespace bimWorld
{
	typedef std::vector<bimWorld::ModelViewerListener*> ListenerSet;

	class YIZHU_EXPORT ICameraManipulators
	{
	public:
		///-------------------------------------------------------------------------------------------------
		/// <summary>	切换漫游器. </summary>
		///
		/// <remarks>	Sqh, 2015/5/6. </remarks>
		///
		/// <param name="emanip">	The emanip. </param>
		///-------------------------------------------------------------------------------------------------
		virtual void switchMatrixManipulator(ManipulatorType emanip) = 0;

		virtual bimWorld::ViewerMode getCameraManipulatorMode() = 0;

		virtual void enableFirstPersonControl() = 0;

		virtual void disableFirstPersonControl() = 0;

		////////////////////////////////////////////////////////视图模式//////////////////////////////////////////////////////////////////////////////////
		virtual void setViewerMode(bimWorld::ViewerMode viewerMode) = 0;

		virtual std::shared_ptr<bimWorld::IZoomModel> ZoomModel() = 0;

		virtual const bimWorld::ListenerSet& getListeners() const = 0;

		virtual void clearSelection() = 0;

		virtual void updateModelSize() = 0;

		virtual void setModelCenter(void* node) = 0;

		virtual void bindKeyUpEvent(bimWorld::KeySymbol key, bimWorld::CameraOperationTypes operation) = 0;

		virtual void bindKeyDownEvent(bimWorld::KeySymbol key, bimWorld::CameraOperationTypes operation) = 0;

		virtual void bindMouseEvent(bimWorld::MouseButtonMask mouse, bimWorld::CameraOperationTypes operation) = 0;

		virtual void unbindKeyUpEvent(bimWorld::KeySymbol key) = 0;

		virtual void unbindKeyDownEvent(bimWorld::KeySymbol key) = 0;

		virtual void unbindMouseEvent(bimWorld::MouseButtonMask mouse) = 0;

		//virtual std::shared_ptr<bimWorld::ICameraAnimation> CameraAnimation() = 0;
	};

}
#endif // __ICAMERA_MANIPULATORS_H__