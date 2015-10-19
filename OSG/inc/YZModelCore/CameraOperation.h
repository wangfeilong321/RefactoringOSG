#ifndef __CAMERA_OPERATION_H__
#define __CAMERA_OPERATION_H__
#include "BIMCameraManipulator.h"
#include "CameraOperationTypes.h"
#include <chrono>

namespace bimWorld
{
	class CameraOperation
	{
	public:
		CameraOperation(BIMCameraManipulator* host);

		bool beginMoveForward(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us);
		bool endMoveForward(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us);

		bool beginMoveLeft(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us);
		bool endMoveLeft(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us);

		bool beginMoveRight(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us);
		bool endMoveRight(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us);

		bool beginMoveBackward(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us);
		bool endMoveBackward(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us);

		bool onHome(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us);

		bool onZoomModel(const double eventTimeDelta, const float dx, const float dy);
		bool onPanModel(const double eventTimeDelta, const float dx, const float dy);
		bool onRotateCamera(const double eventTimeDelta, const float dx, const float dy);

		bool pauseRendering();

	// internal:
		void onMoveForward(std::chrono::system_clock::time_point tick);
		
		void onMoveLeft(std::chrono::system_clock::time_point tick);
		
		void onMoveRight(std::chrono::system_clock::time_point tick);
		
		void onMoveBackward(std::chrono::system_clock::time_point tick);

		void zoomForward(float delta, float frame);

		void zoomBackward(float delta, float frame);

		float getZoomDelta(float totalFrame);

		float getMoveFactor();

	protected:
		BIMCameraManipulator* m_host;
		std::chrono::system_clock::time_point m_lastOnMoveBackwardTime;
		std::chrono::system_clock::time_point m_lastOnMoveForwardTime;
		std::chrono::system_clock::time_point m_lastOnMoveLeftTime;
		std::chrono::system_clock::time_point m_lastOnMoveRightTime;

		bool m_firstMoveForward;
		bool m_firstMoveBackward;
		bool m_firstMoveLeft;
		bool m_firstMoveRight;
	};

}
#endif // __CAMERA_OPERATION_H__