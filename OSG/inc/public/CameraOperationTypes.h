#ifndef __CAMERA_OPERATION_TYPES_H__
#define __CAMERA_OPERATION_TYPES_H__

namespace bimWorld
{

	enum CameraOperationTypes
	{
		emptyOperation,
		beginMoveForward,
		endMoveForward,
		beginMoveLeft,
		endMoveLeft,
		beginMoveRight,
		endMoveRight,
		beginMoveBackward,
		endMoveBackward,
		onHome,
		onZoomModel,
		onPanModel,
		onRotateCamera,
	};
}
#endif // __CAMERA_OPERATION_TYPES_H__