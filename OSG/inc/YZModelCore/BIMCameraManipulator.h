#ifndef __BIM_CAMERA_MANIPULATOR_H__
#define __BIM_CAMERA_MANIPULATOR_H__

#include <osgGA/MultiTouchTrackballManipulator>
#include "CameraManipulatorPickBase.h"
#include "MouseManipulatorContainer.h"
#include <functional>
#include "MouseKeySymbol.h"
#include "CameraOperationTypes.h"
#include <memory>

namespace bimWorld
{
    class CameraOperation;

	class YIZHU_EXPORT BIMCameraManipulator : public osgGA::MultiTouchTrackballManipulator, public CameraManipulatoerPickBase
	{

		typedef osgGA::MultiTouchTrackballManipulator Super;
		typedef CameraManipulatoerPickBase PickBase;
	public:
		BIMCameraManipulator(YZModelCoreInternal *host, bool limitedSpace = false, double selectionPrecision = 1e-6);
		
		~BIMCameraManipulator();

		void updateModelSize();

		void beginSetCameraMatrix();

		void endSetCameraMatrix();

		void vTranslation(double dy);

		void hTranslation(double dy);

		void setPosition(const osg::Vec3& eye, const osg::Vec3& center, const osg::Vec3& up, const osg::Matrix& matrix);
		
		virtual void setByMatrix(const osg::Matrixd& matrix) override;

		void getStandardManipulatorParam(osg::Vec3d& eye, osg::Vec3d& center, osg::Vec3d& up);

		//Unluckily handleMultiToudhDrag is not a virtual function in OpenSceneGraph 3.2.1
		//we have to override zoomModel and panModel, change it when OSG is updated.
		virtual void pushForward(const float dy);
		virtual void beginPushForward(const float dy);
		virtual void endPushForward();
		
		virtual void zoomModel(const float dy, bool pushForwardIfNeeded = true) override;
		
		virtual void zoomModelLocally(const float pointX, const float pointY, const float dy, bool pushForwardIfNeeded = true);
		
		virtual void zoomModelWithSelectedCenter(const float dy, bool pushForwardIfNeeded = true);

		virtual void panModel(const float dx, const float dy, const float dz = 0);
		virtual void pushSide(const float dx, const float dy, const float dz = 0);
		virtual void beginPushSide(const float dx, const float dy, const float dz = 0);
		virtual void endPushSide();

		bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

		virtual bool handleFrame(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us);
		virtual bool handleResize(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us);
		virtual bool handleMouseMove(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us);
		virtual bool handleMouseDrag(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us);
		virtual bool handleMousePush(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us);
		virtual bool handleMouseRelease(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us);
		virtual bool handleKeyDown(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us);

		virtual bool handleKeyUp(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us);

		virtual bool handleMouseWheel(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us);

		virtual void handleMultiTouchDrag(osgGA::GUIEventAdapter::TouchData* now, osgGA::GUIEventAdapter::TouchData* last, const double eventTimeDelta);

		virtual bool performMovementLeftMouseButton(const double eventTimeDelta, const double dx, const double dy);
		virtual bool performMovementMiddleMouseButton(const double eventTimeDelta, const double dx, const double dy);
		virtual bool performMovementRightMouseButton(const double eventTimeDelta, const double dx, const double dy);
		virtual bool performMouseDeltaMovement(const float dx, const float dy);
		virtual bool performMovement() override;

		Manipulator::MouseManipulatorContainer* getManipContainer();

		void setRotationHandleBtn(osgGA::GUIEventAdapter::MouseButtonMask btn);

		// only set rotation center, without moving the camera center.
		void setRotationCenter(const osg::Vec3d& center);

		void setModelCenter(const osg::Vec3d& center);

		void setModelCenterKeepDist(const osg::Vec3d& center);

		void bindKeyUpEvent(bimWorld::KeySymbol key, bimWorld::CameraOperationTypes operation);
		
		void bindKeyDownEvent(bimWorld::KeySymbol key, bimWorld::CameraOperationTypes operation);

		void bindMouseEvent(bimWorld::MouseButtonMask mouse, bimWorld::CameraOperationTypes operation);

		void unbindKeyUpEvent(bimWorld::KeySymbol key);

		void unbindKeyDownEvent(bimWorld::KeySymbol key);

		void unbindMouseEvent(bimWorld::MouseButtonMask mouse);

		bool getKeyOperationFunc(bimWorld::CameraOperationTypes operation, std::function<bool(const osgGA::GUIEventAdapter&, osgGA::GUIActionAdapter&)> &func);
		
		bool getMouseOperationFunc(bimWorld::CameraOperationTypes operation, std::function<bool(const double, const double, const double)> &func);

	protected:

		//void key_d_down();

		//void key_a_down();

		//void key_s_down();

		//void key_w_down();

		//void key_d_up();

		//void key_a_up();

		//void key_s_up();

		//void key_w_up();

		bool KeyUpEvent(int key, const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us);
		
		bool KeyDownEvent(int key, const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us);
		
		void bindDefaultEvent();

		// <������������>
		osg::ref_ptr<Manipulator::MouseManipulatorContainer> _manipContainer;

	private:
		bool limitedSpace;
		bool m_isSettingCameraMatrix;
		
		// <ÿ���ƶ��ľ���>
		float m_moveDelta;
		bool m_movingForward;
		bool m_movingBackward;
		bool m_movingLeft;
		bool m_movingRight;
		bool m_isPushed;
		bool m_shiftDown;
		bool m_rightBtnHandleRotate;
		bool m_leftBtnHandleRotate;
		double m_lastDist;

		std::map<int, std::function<bool(const osgGA::GUIEventAdapter&, osgGA::GUIActionAdapter&)> > m_keyUpEventMap;
		std::map<int, std::function<bool(const osgGA::GUIEventAdapter&, osgGA::GUIActionAdapter&)> > m_keyDownEventMap;
		std::map<int, std::function<bool(const double eventTimeDelta, const float dx, const float dy)> > m_mouseEventMap;
        
        friend class CameraOperation;
		//std::unique_ptr<CameraOperation> m_operation;
		CameraOperation* m_operation;
		bool scaleOption;
		double _lastDistance;
		osg::Vec3d _lastDeltaVec;
		float _mBeginX;
		float _mBeginY;
		float _mDeltaX;
		float _mDeltaY;
	};

}


#endif // !__BIM_CAMERA_MANIPULATOR_H__
