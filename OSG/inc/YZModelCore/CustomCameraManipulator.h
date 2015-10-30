#ifndef __CUSTOM_CAMERA_MANIPULATOR_H__
#define __CUSTOM_CAMERA_MANIPULATOR_H__
#include <osgGA/GUIEventHandler>
#include "CameraManipulatorPickBase.h"
#include "MouseManipulatorContainer.h"

namespace bimWorld
{
	class CameraOperation;
	class YIZHU_EXPORT CustomCameraManipulator :public osgGA::CameraManipulator, public CameraManipulatoerPickBase
	{
		typedef CameraManipulatoerPickBase PickBase;
	public:
		CustomCameraManipulator(YZModelCoreInternal *host = NULL);
		CustomCameraManipulator(const CustomCameraManipulator& om,
			const osg::CopyOp& copyOp = osg::CopyOp::SHALLOW_COPY);
		META_Object(bimWorld, CustomCameraManipulator);

		virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa) override;

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
		
		virtual bool performMovementLeftMouseButton(const double dx, const double dy);
		virtual bool performMovementMiddleMouseButton(const double dx, const double dy);
		virtual bool performMovementRightMouseButton(const double dx, const double dy);
		virtual bool performMouseDeltaMovement(const float dx, const float dy);
		virtual bool performMovement(const osgGA::GUIEventAdapter& ea);

		osg::ref_ptr<osgGA::GUIEventAdapter::TouchData> _lastTouchData;
		bool m_isPushed;
		osg::ref_ptr<bimWorld::Manipulator::MouseManipulatorContainer> _manipContainer;
	protected:
		float distance();
		void zoomModelLocally(const float pointX, const float pointY, const float cdy, bool pushForwardIfNeeded);
		void zoomModelWithSelectedCenter(const float cdy, bool pushForwardIfNeeded = true);
		void panModel(const float dx, const float dy, const float dz = 0);
		void pushSide(const float dx, const float dy, const float dz = 0);
		void beginPushSide(const float dx, const float dy, const float dz = 0);
		void endPushSide();
		bool KeyUpEvent(int key, const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us);
		bool KeyDownEvent(int key, const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us);

		/**
		Attach a node to the manipulator, automatically detaching any previously attached node.
		setNode(NULL) detaches previous nodes.
		May be ignored by manipulators which do not require a reference model.
		*/
		virtual void setNode(osg::Node* node) { m_model = node; _modelSize = m_model->getBound().radius(); }

		/** Return const node if attached.*/
		virtual const osg::Node* getNode() const { return m_model; }

		/** Return node if attached.*/
		virtual osg::Node* getNode() { return m_model; }

		virtual void setByMatrix(const osg::Matrixd& matrix) override;

		virtual void setByInverseMatrix(const osg::Matrixd& matrix) override;

		virtual osg::Matrixd getMatrix() const override;

		virtual osg::Matrixd getInverseMatrix() const override;

		std::map<int, std::function<bool(const osgGA::GUIEventAdapter&, osgGA::GUIActionAdapter&)> > m_keyUpEventMap;
		std::map<int, std::function<bool(const osgGA::GUIEventAdapter&, osgGA::GUIActionAdapter&)> > m_keyDownEventMap;
		std::map<int, std::function<bool(const float dx, const float dy)> > m_mouseEventMap;

	private:
		friend class CameraOperation;
		//std::unique_ptr<CameraOperation> m_operation;
		CameraOperation* m_operation;
		double _modelSize;
		double _wheelZoomFactor;
		bool m_movingForward;
		bool m_movingBackward;
		bool m_movingLeft;
		bool m_shiftDown;
		bool m_movingRight;
		float _minimumDistance;
		osg::Node* m_model;
	};

}
#endif // __CUSTOM_CAMERA_MANIPULATOR_H__