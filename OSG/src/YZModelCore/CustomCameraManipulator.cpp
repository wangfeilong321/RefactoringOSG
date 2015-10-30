#include "CustomCameraManipulator.h"
#include "YZModelCoreInternal.h"
#include "CameraOperation.h"

using namespace bimWorld;


bimWorld::CustomCameraManipulator::CustomCameraManipulator(YZModelCoreInternal *host) :
PickBase(host, selectionPrecision), m_operation(new bimWorld::CameraOperation(NULL))
{
	_lastTouchData = NULL;
	_modelSize = 0;
	_wheelZoomFactor = 0.1;
	_minimumDistance = 1000;
	m_model = NULL;
}

bimWorld::CustomCameraManipulator::CustomCameraManipulator(const CustomCameraManipulator& om, const osg::CopyOp& copyOp /*= osg::CopyOp::SHALLOW_COPY*/):
PickBase(om.m_host, selectionPrecision)
{
	m_operation = om.m_operation;
	_modelSize = om._modelSize;
	_wheelZoomFactor = om._wheelZoomFactor;
	m_movingForward = om.m_movingForward;
	m_movingBackward = om.m_movingBackward;
	m_movingLeft = om.m_movingLeft;
	m_shiftDown = om.m_shiftDown;
	m_movingRight = om.m_movingRight;
	_minimumDistance = om._minimumDistance;
	m_model = om.m_model;
}

bool CustomCameraManipulator::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us)
{
	switch (ea.getEventType())
	{
	case osgGA::GUIEventAdapter::FRAME:
		return handleFrame(ea, us);

	case osgGA::GUIEventAdapter::RESIZE:
		return handleResize(ea, us);

	default:
		break;
	}

	if (!m_host || !m_host->_ViewerData()->isLoaded())
		return false;

	bool handled(false);

	switch (ea.getEventType())
	{

	case osgGA::GUIEventAdapter::PUSH:
	case osgGA::GUIEventAdapter::DRAG:
	case osgGA::GUIEventAdapter::RELEASE:
		if (ea.isMultiTouchEvent())
		{
			double eventTimeDelta = 1 / 60.0; //_ga_t0->getTime() - _ga_t1->getTime();
			if (eventTimeDelta < 0.)
			{
				OSG_WARN << "Manipulator warning: eventTimeDelta = " << eventTimeDelta << std::endl;
				eventTimeDelta = 0.;
			}
			osgGA::GUIEventAdapter::TouchData* data = ea.getTouchData();

			// three touches or two taps for home position
			if ((data->getNumTouchPoints() == 3) || ((data->getNumTouchPoints() == 1) && (data->get(0).tapCount >= 2)))
			{
				//flushMouseEventStack();
				//_thrown = false;
				//home(ea, us);
				//handled = true;
			}

			else if (data->getNumTouchPoints() >= 2)
			{
				//if ((_lastTouchData.valid()) && (_lastTouchData->getNumTouchPoints() >= 2))
				//{
					handleMultiTouchDrag(data, _lastTouchData.get(), eventTimeDelta);
				//}

				handled = true;
			}

			_lastTouchData = data;

			// check if all touches ended
			unsigned int num_touches_ended(0);
			for (osgGA::GUIEventAdapter::TouchData::iterator i = data->begin(); i != data->end(); ++i)
			{
				if ((*i).phase == osgGA::GUIEventAdapter::TOUCH_ENDED)
					num_touches_ended++;
			}

			if (num_touches_ended == data->getNumTouchPoints())
			{
				_lastTouchData = NULL;
			}

		}
		break;
	default:
		break;
	}

	if (handled)
		return true;

	if (ea.getHandled())
		return false;

	switch (ea.getEventType())
	{
	case osgGA::GUIEventAdapter::DOUBLECLICK:
	case osgGA::GUIEventAdapter::PEN_PROXIMITY_ENTER:
		return false;
	case osgGA::GUIEventAdapter::PEN_PROXIMITY_LEAVE:
		if (m_operation->pauseRendering())
		{
			m_host->_RenderingThreads()->setNeedUpdateAndDraw(false, 3);
		}
		return false;
	case osgGA::GUIEventAdapter::MOVE:
		return handleMouseMove(ea, us);

	case osgGA::GUIEventAdapter::DRAG:
		return handleMouseDrag(ea, us);

	case osgGA::GUIEventAdapter::PUSH:
		m_isPushed = true;
		m_host->_RenderingThreads()->setNeedUpdateAndDraw(true);
		return handleMousePush(ea, us);

	case osgGA::GUIEventAdapter::RELEASE:
		handled = handleMouseRelease(ea, us);
		m_isPushed = false;
		if (m_operation->pauseRendering())
		{
			m_host->_RenderingThreads()->setNeedUpdateAndDraw(false, 3);
		}
		return handled;

	case osgGA::GUIEventAdapter::KEYDOWN:
		handled = handleKeyDown(ea, us);
		m_host->_RenderingThreads()->setNeedUpdateAndDraw(true);
		return handled;

	case osgGA::GUIEventAdapter::KEYUP:
		handled = handleKeyUp(ea, us);
		if (m_operation->pauseRendering())
		{
			m_host->_RenderingThreads()->setNeedUpdateAndDraw(false, 3);
		}
		return handled;

	case osgGA::GUIEventAdapter::SCROLL:
		//if (_flags & PROCESS_MOUSE_WHEEL)
			return handleMouseWheel(ea, us);
		//else
		//	return false;

	default:
		return false;
	}

	//return Super::handle(ea, us);
}

bool bimWorld::CustomCameraManipulator::handleFrame(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us)
{
	//double current_frame_time = ea.getTime();

	//_delta_frame_time = current_frame_time - _last_frame_time;
	//_last_frame_time = current_frame_time;

	if (/*_thrown && */performMovement(ea))
	{
		//us.requestRedraw();
		m_host->RenderingThreads()->updateSeveralTimes(1);
	}

	//if (_animationData && _animationData->_isAnimating)
	//{
	//	performAnimationMovement(ea, us);
	//}

	return false;
}

bool bimWorld::CustomCameraManipulator::handleResize(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us)
{
	//init(ea, us);
	//us.requestRedraw();
	m_host->RenderingThreads()->updateSeveralTimes(1);

	return true;
}

bool bimWorld::CustomCameraManipulator::handleMouseMove(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us)
{
	return PickBase::handleMouseMove(ea, us);
}

bool bimWorld::CustomCameraManipulator::handleMouseDrag(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us)
{
	if (!m_host || !m_host->_ViewerData()->isLoaded())
		return false;

	m_host->_ViewerData()->ModelViewer()->getCamera()->setSmallFeatureCullingPixelSize(_modelSize*0.4);

	if (_manipContainer->handleMouseDrag(ea, us))
		return true;

	if (PickBase::handleMouseDrag(ea, us))
		return true;

#if USING_FIXED_FRAMERATE
	if (fixedFramerateController.hide())
		m_pImpl->setAbortRendering();
#endif

	//addMouseEvent(ea);

	if (performMovement(ea))
	{
		//us.requestRedraw();
		m_host->RenderingThreads()->updateSeveralTimes(1);
	}

	//us.requestContinuousUpdate(false);
	//m_host->RenderingThreads()->setNeedUpdateAndDraw(false, 3);
	//_thrown = false;

	return true;
}

bool bimWorld::CustomCameraManipulator::handleMousePush(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us)
{
	if (!m_host || !m_host->_ViewerData()->isLoaded())
		return false;

	if (_manipContainer->handleMousePush(ea, us))
		return true;


	if (PickBase::handleMousePush(ea, us))
		return true;

	//flushMouseEventStack();
	//addMouseEvent(ea);

	if (performMovement(ea))
	{
		//us.requestRedraw();
		m_host->RenderingThreads()->updateSeveralTimes(1);
	}

	//us.requestContinuousUpdate(false);
	//m_host->RenderingThreads()->setNeedUpdateAndDraw(false, 3);
	//_thrown = false;

	return true;
}

bool bimWorld::CustomCameraManipulator::handleMouseRelease(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us)
{
	if (!m_host || !m_host->_ViewerData()->isLoaded())
		return false;
	m_host->_ViewerData()->ModelViewer()->getCamera()->setSmallFeatureCullingPixelSize(2.0);

	if (_manipContainer->handleMouseRelease(ea, us))
		return true;

	if (PickBase::handleMouseRelease(ea, us))
		return true;
	//osg::Geode* box = new osg::Geode();
	//auto point = m_host->Coordinate()->CoordinateHelper()->ScreenToWorld(osg::Vec3(ea.getX(), ea.getY(), 0));
	//box->addDrawable(new osg::ShapeDrawable(new osg::Box(point, 1000)));
	//m_host->ViewerData()->getModelRoot()->addChild(box);
	if (ea.getButtonMask() == 0)
	{

		//double timeSinceLastRecordEvent = _ga_t0.valid() ? (ea.getTime() - _ga_t0->getTime()) : DBL_MAX;
		//if (timeSinceLastRecordEvent > 0.02)
		//	flushMouseEventStack();

		//if (isMouseMoving())
		//{

		//	if (performMovement() && _allowThrow)
		//	{
		//		//us.requestRedraw();
		//		//us.requestContinuousUpdate(true);
		//		m_host->RenderingThreads()->setNeedUpdateAndDraw(true);
		//		//_thrown = true;
		//	}

		//	return true;
		//}
	}

	//flushMouseEventStack();
	//addMouseEvent(ea);
	if (performMovement(ea))
	{
		m_host->RenderingThreads()->updateSeveralTimes(1);
		//us.requestRedraw();
	}
	//us.requestContinuousUpdate(false);
	//m_host->RenderingThreads()->setNeedUpdateAndDraw(false, 3);
	//_thrown = false;

	return true;
}

bool bimWorld::CustomCameraManipulator::handleKeyDown(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us)
{

	switch (ea.getKey())
	{
	case osgGA::GUIEventAdapter::KEY_Shift_L:
	{
		m_shiftDown = true;
	}
	}
	//flushMouseEventStack();
	//_thrown = false;
	if (KeyDownEvent(ea.getUnmodifiedKey(), ea, us))
		return true;
	if (KeyDownEvent(ea.getKey(), ea, us))
		return true;
	return false;
}

bool bimWorld::CustomCameraManipulator::handleKeyUp(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us)
{
	switch (ea.getKey())
	{
	case osgGA::GUIEventAdapter::KEY_Shift_L:
	{
		m_shiftDown = false;
	}
	}
	//flushMouseEventStack();
	//_thrown = false;
	if (KeyUpEvent(ea.getUnmodifiedKey(), ea, us))
		return true;
	if (KeyUpEvent(ea.getKey(), ea, us))
		return true;

	return false;
}

bool bimWorld::CustomCameraManipulator::handleMouseWheel(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us)
{
	osgGA::GUIEventAdapter::ScrollingMotion sm = ea.getScrollingMotion();

	// handle centering
	//if (_flags & SET_CENTER_ON_WHEEL_FORWARD_MOVEMENT)
	{

		if (((sm == osgGA::GUIEventAdapter::SCROLL_DOWN && _wheelZoomFactor > 0.)) ||
			((sm == osgGA::GUIEventAdapter::SCROLL_UP   && _wheelZoomFactor < 0.)))
		{

			//if (getAnimationTime() <= 0.)
			//{
			//	// center by mouse intersection (no animation)
			//	setCenterByMousePointerIntersection(ea, us);
			//}
			//else
			//{
			//	// start new animation only if there is no animation in progress
			//	if (!isAnimating())
			//		startAnimationByMousePointerIntersection(ea, us);

			//}

		}
	}

	switch (sm)
	{
		// mouse scroll up event
	case osgGA::GUIEventAdapter::SCROLL_UP:
	{
		// perform zoom
		//zoomModel(_wheelZoomFactor, false);
		if (m_host->_ViewerData()->getSelectedNodes().empty())
		{
			zoomModelLocally(ea.getX(), ea.getY(), _wheelZoomFactor, false);
		}
		else
		{
			zoomModelWithSelectedCenter(_wheelZoomFactor, false);
		}
		m_host->_RenderingThreads()->updateSeveralTimes(1);
		//us.requestContinuousUpdate(isAnimating() || _thrown);
		//m_host->RenderingThreads()->setNeedUpdateAndDraw(isAnimating() || _thrown, 3);
		return true;
	}

		// mouse scroll down event
	case osgGA::GUIEventAdapter::SCROLL_DOWN:
	{
		// perform zoom
		//zoomModel(-_wheelZoomFactor, false);
		if (m_host->_ViewerData()->getSelectedNodes().empty())
		{
			zoomModelLocally(ea.getX(), ea.getY(), -_wheelZoomFactor, false);
		}
		else
		{
			zoomModelWithSelectedCenter(-_wheelZoomFactor, false);
		}
		m_host->_RenderingThreads()->updateSeveralTimes(1);
		//us.requestContinuousUpdate(isAnimating() || _thrown);
		//m_host->RenderingThreads()->setNeedUpdateAndDraw(isAnimating() || _thrown, 3);
		return true;
	}

		// unhandled mouse scrolling motion
	default:
		return false;
	}
}

void bimWorld::CustomCameraManipulator::handleMultiTouchDrag(osgGA::GUIEventAdapter::TouchData* now, osgGA::GUIEventAdapter::TouchData* last, const double eventTimeDelta)
{
	const float zoom_threshold = 1.0f;

	osg::Vec2 pt_1_now(now->get(0).x, now->get(0).y);
	osg::Vec2 pt_2_now(now->get(1).x, now->get(1).y);
	osg::Vec2 pt_1_last(last->get(0).x, last->get(0).y);
	osg::Vec2 pt_2_last(last->get(1).x, last->get(1).y);



	float gap_now((pt_1_now - pt_2_now).length());
	float gap_last((pt_1_last - pt_2_last).length());

	// osg::notify(osg::ALWAYS) << gap_now << " " << gap_last << std::endl;

	if (fabs(gap_last - gap_now) >= zoom_threshold)
	{
		// zoom gesture
		zoomModelWithSelectedCenter((gap_last - gap_now) * eventTimeDelta, false);
	}

	// drag gesture

	osg::Vec2 delta = ((pt_1_last - pt_1_now) + (pt_2_last - pt_2_now)) / 2.0f;

	float scale = 0.2f * distance() * eventTimeDelta;

	// osg::notify(osg::ALWAYS) << "drag: " << delta << " scale: " << scale << std::endl;

	panModel(delta.x() * scale, delta.y() * scale * (-1)); // flip y-coordinate because of different origins.
}

bool bimWorld::CustomCameraManipulator::performMovementLeftMouseButton(const double dx, const double dy)
{
	auto iter = m_mouseEventMap.find(bimWorld::LEFT_MOUSE_BUTTON);
	if (iter == m_mouseEventMap.end())
	{
		return false;
	}
	else
	{
		return (iter->second)(dx, dy);
	}
}

bool bimWorld::CustomCameraManipulator::performMovementMiddleMouseButton(const double dx, const double dy)
{
	auto iter = m_mouseEventMap.find(bimWorld::MIDDLE_MOUSE_BUTTON);
	if (iter == m_mouseEventMap.end())
	{
		return false;
	}
	else
	{
		return (iter->second)(dx, dy);
	}
}

bool bimWorld::CustomCameraManipulator::performMovementRightMouseButton(const double dx, const double dy)
{
	auto iter = m_mouseEventMap.find(bimWorld::RIGHT_MOUSE_BUTTON);
	if (iter == m_mouseEventMap.end())
	{
		return false;
	}
	else
	{
		return (iter->second)(dx, dy);
	}
}

bool bimWorld::CustomCameraManipulator::performMouseDeltaMovement(const float dx, const float dy)
{
	// rotate camera
	//if (getVerticalAxisFixed())
	//	rotateWithFixedVertical(dx, dy);
	//else
	//	rotateTrackball(0.f, 0.f, dx, dy, 1.f);

	return true;
}

bool bimWorld::CustomCameraManipulator::performMovement(const osgGA::GUIEventAdapter& ea)
{
	float dx = ea.getX();
	float dy = ea.getY();
	auto buttonMask = ea.getButtonMask();
	//// return if less then two events have been added
	//if (_ga_t0.get() == NULL || _ga_t1.get() == NULL)
	//	return false;

	//// get delta time
	//double eventTimeDelta = _ga_t0->getTime() - _ga_t1->getTime();
	//if (eventTimeDelta < 0.)
	//{
	//	OSG_WARN << "Manipulator warning: eventTimeDelta = " << eventTimeDelta << std::endl;
	//	eventTimeDelta = 0.;
	//}

	//// get deltaX and deltaY
	//float dx = _ga_t0->getXnormalized() - _ga_t1->getXnormalized();
	//float dy = _ga_t0->getYnormalized() - _ga_t1->getYnormalized();

	//// return if there is no movement.
	//if (dx == 0. && dy == 0.)
	//	return false;


	// call appropriate methods
	//unsigned int buttonMask = _ga_t1->getButtonMask();
	if (buttonMask == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
	{
		return performMovementLeftMouseButton(dx, dy);
	}
	else if (buttonMask == osgGA::GUIEventAdapter::MIDDLE_MOUSE_BUTTON/* ||
																	  buttonMask == (osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON | osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON)*/)
	{
		return performMovementMiddleMouseButton(dx, dy);
	}
	else if (buttonMask == osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON)
	{
		return performMovementRightMouseButton(dx, dy);
	}

	return false;
}

float bimWorld::CustomCameraManipulator::distance()
{
	return (_homeEye - _homeCenter).length();
}
#define CodingZoom 1
void bimWorld::CustomCameraManipulator::zoomModelLocally(const float pointX, const float pointY, const float cdy, bool pushForwardIfNeeded)
{
#if 1
	//osg::Vec3d eye;
	osg::Vec3d center;
	//osg::Vec3d up;
	//up = /*_rotation * */osg::Vec3d(0., 1., 0.);
	//eye = _center - /*_rotation * */osg::Vec3d(0., 0., -_distance);
	center = m_host->_Coordinate()->CoordinateHelper()->ScreenToWorld(osg::Vec3f(pointX, pointY, 0));
	m_host->_ViewerData()->setLocalLookAt(m_host->_ViewerData()->m_eye, center, m_host->_ViewerData()->m_up);

	auto delta = cdy;

#ifdef TARGET_IPHONE_SIMULATOR
	delta = 0.5 * delta;
#elif _WIN32
	delta = -delta;
#else
	delta = delta;
#endif
	auto scale = 1 + delta;
	auto sv = m_host->_ViewerData()->m_scaleVec;
	m_host->_ViewerData()->m_scaleVec += osg::Vec3(delta, delta, delta);
	m_host->_ViewerData()->m_scale->setMatrix(osg::Matrix::scale(m_host->_ViewerData()->m_scaleVec));
	return;

#elif CodingZoom

	osg::Vec3d eye;
	//osg::Vec3d center;
	osg::Vec3d up;
	up = _rotation * osg::Vec3d(0., 1., 0.);
	eye = _center - _rotation * osg::Vec3d(0., 0., -_distance);
	//m_host->_ViewerData()->ModelViewer()->getCameraManipulator()->getHomePosition(eye, center, up);
	auto scenter = m_host->_Coordinate()->CoordinateHelper()->WorldToScreen(osg::Vec3f(_center.x(), _center.y(), _center.z()));
	auto seye = m_host->_Coordinate()->CoordinateHelper()->WorldToScreen(osg::Vec3f(eye.x(), eye.y(), eye.z()));
	float z = (scenter.z() - seye.z()) ? (scenter.z() + 1000) : (scenter.z() - 1000);
	auto vec = m_host->_Coordinate()->CoordinateHelper()->ScreenToWorld(osg::Vec3f(pointX, pointY, z)) - eye;
	vec.normalize();

	auto dy = cdy;

#ifdef TARGET_IPHONE_SIMULATOR
	dy = 0.5 * dy;
#elif _WIN32
	dy = -dy;
#else
	dy = dy;
#endif
	// scale
	//float scale = 1.0f + (scaleOption ? dy : -dy);

	// minimum distance
	float minDist = _minimumDistance * 1000;
	if (getRelativeFlag(_minimumDistanceFlagIndex))
		minDist *= _modelSize;
	if (_distance > minDist)
	{
		minDist = _distance;
	}
	osg::Matrixd rotation_matrix(_rotation);
	osg::Vec3d dv = (/*osg::Vec3d(0.0f, 0.0f, -1.0f)*/vec /** rotation_matrix*/)* (dy * -minDist*0.2);
	////setTransformation(eye + dv, center, up);
	//fixVerticalAxis(_center, _rotation, true);
	//{
	//	auto center = _center;
	//	osg::Vec3d lv(center - (eye + dv));

	//	osg::Vec3d f(lv);
	//	f.normalize();
	//	osg::Vec3d s(f^up);
	//	s.normalize();
	//	osg::Vec3d u(s^f);
	//	u.normalize();

	//	osg::Matrixd rotation_matrix(s[0], u[0], -f[0], 0.0f,
	//		s[1], u[1], -f[1], 0.0f,
	//		s[2], u[2], -f[2], 0.0f,
	//		0.0f, 0.0f, 0.0f, 1.0f);

	//	_center = center;
	//	_distance = lv.length();
	//	_rotation = rotation_matrix.getRotate().inverse();
	//}
	//fixVerticalAxis(_center, _rotation, true);
	//_homeEye += dv;
	//_distance = (_homeEye - _center).length();
	////home(0);
	//_center += dv;
	setTransformation(eye + dv, _rotation);
#else

	auto dy = cdy;

#ifdef TARGET_IPHONE_SIMULATOR
	dy = 0.5 * dy;
#elif _WIN32
	dy = -dy;
#else
	dy = dy;
#endif
	// scale
	float scale = 1.0f + (scaleOption ? dy : -dy);

	// minimum distance
	float minDist = _minimumDistance * 1000;
	if (getRelativeFlag(_minimumDistanceFlagIndex))
		minDist *= _modelSize;

	if (_distance * scale > minDist || _distance * scale < -minDist)
	{
		// regular zoom
		_distance *= scale;
	}
	else
	{
		if (pushForwardIfNeeded)
		{
			// push the camera forward
			float scale = -_distance;
			osg::Matrixd rotation_matrix(_rotation);
			osg::Vec3d dv = (osg::Vec3d(0.0f, 0.0f, -1.0f) * rotation_matrix) * (dy * scale);
			_center += dv;
		}
		else
		{
			//// set distance on its minimum value
			//_distance = minDist;

			// moving to opposite direction.
			_distance = -_distance;
			scaleOption = !scaleOption;
		}
	}

	// limit if needed, keep max size as.
	if (limitedSpace && m_host->_ViewerData()->getModelRoot() != NULL && this->_distance > 3 * _modelSize)
		this->_distance = 3 * _modelSize;
#endif // CodingZoom

}

void bimWorld::CustomCameraManipulator::zoomModelWithSelectedCenter(const float cdy, bool pushForwardIfNeeded /*= true*/)
{
	auto dy = cdy;

#ifdef TARGET_IPHONE_SIMULATOR
	dy = 0.5 * dy;
#elif _WIN32
	dy = -dy;
#else
	dy = dy;
#endif

	osg::Vec3d eye;
	//osg::Vec3d center;
	osg::Vec3d up;
	up = _homeUp;//_rotation * osg::Vec3d(0., 1., 0.);
	eye = _homeEye;//_center - _rotation * osg::Vec3d(0., 0., -_distance);

	auto tcenter = m_host->_ViewerData()->getSelectedCenter();//osg::Vec3f(pointX, pointY, z);
	auto vec = tcenter - eye;
	vec.normalize();
	auto distVec = osg::Vec3(tcenter.x()*vec.x(), tcenter.y()*vec.y(), tcenter.z()*vec.z()) - osg::Vec3(eye.x()*vec.x(), eye.y()*vec.y(), eye.z()*vec.z());
	auto length = distVec.length();
	float delta = dy * -distance()*0.2;
	if (dy < 0 && length < delta)
	{
		return;
	}
	//delta = dy * -(0.000035*_distance*_distance + 150);
	osg::Vec3d dv = vec * delta;

	// scale
	//float scale = 1.0f + (scaleOption ? dy : -dy);

	//// minimum distance
	//float minDist = _minimumDistance * 1000;
	//if (getRelativeFlag(_minimumDistanceFlagIndex))
	//	minDist *= _modelSize;
	//osg::Matrixd rotation_matrix(_rotation);
	//osg::Vec3d dv = (/*osg::Vec3d(0.0f, 0.0f, -1.0f)*/vec /** rotation_matrix*/)* (dy * -delta);
	////setTransformation(eye + dv, center, up);
	//fixVerticalAxis(_center, _rotation, true);
	//{
	//	auto center = _center;
	//	osg::Vec3d lv(center - (eye + dv));

	//	osg::Vec3d f(lv);
	//	f.normalize();
	//	osg::Vec3d s(f^up);
	//	s.normalize();
	//	osg::Vec3d u(s^f);
	//	u.normalize();

	//	osg::Matrixd rotation_matrix(s[0], u[0], -f[0], 0.0f,
	//		s[1], u[1], -f[1], 0.0f,
	//		s[2], u[2], -f[2], 0.0f,
	//		0.0f, 0.0f, 0.0f, 1.0f);

	//	_center = center;
	//	_distance = lv.length();
	//	_rotation = rotation_matrix.getRotate().inverse();
	//}
	//fixVerticalAxis(_center, _rotation, true);
	//_homeEye += dv;
	//_distance = (_homeEye - _center).length();
	////home(0);
	//_center += dv;

	//TODO: setTransformation(eye + dv, _rotation);


}

void bimWorld::CustomCameraManipulator::panModel(const float dx, const float dy, const float dz/* = 0*/)
{
#ifndef TARGET_IPHONE_SIMULATOR
	pushSide(dx, dy);
	//osg::Matrix rotation_matrix;
	//rotation_matrix.makeRotate(_rotation);

	//osg::Vec3d dv(dx, dy, dz);

	//_center += dv * rotation_matrix;

	//// limit if needed, keep pan center inside model binding sphere.
	//osg::Vec3d deltaDist = this->_center - this->_homeCenter;
	//if (limitedSpace && deltaDist.length() > 0.5 * _modelSize)
	//{
	//	this->_center = this->_homeCenter + deltaDist * 0.5 * _modelSize / deltaDist.length();
	//}

#endif
}

void bimWorld::CustomCameraManipulator::pushSide(const float dx, const float dy, const float dz/* = 0*/)
{
#ifndef TARGET_IPHONE_SIMULATOR

	//osg::Matrix rotation_matrix;
	//rotation_matrix.makeRotate(_rotation);

	//osg::Vec3d dv(dx, dy, dz);
	osg::Vec3d right(1, 0, 0);
	float delta = dx*dy;// TODO: make delta as input param.
	_homeCenter += right * 1000 * delta;

	// limit if needed, keep pan center inside model binding sphere.
	//osg::Vec3d deltaDist = m_center - this->_homeCenter;
	//if (limitedSpace && deltaDist.length() > 0.5 * _modelSize)
	//{
	//	this->_center = this->_homeCenter + deltaDist * 0.5 * _modelSize / deltaDist.length();
	//}

#endif
}

void bimWorld::CustomCameraManipulator::beginPushSide(const float dx, const float dy, const float dz /*= 0*/)
{

//	// minimum distance
//	float minDist = _minimumDistance;
//	//if (getRelativeFlag(_minimumDistanceFlagIndex))
//	//	minDist *= _modelSize;
//
//#ifdef TARGET_IPHONE_SIMULATOR
//	float delta = 0.5 * (minDist - _distance);
//#elif  _WIN32
//	float delta = _distance - minDist;
//#else
//	float delta = minDist - _distance;
//#endif // _WIN32
//	_lastDistance = _distance;
//	osg::Matrixd rotation_matrix(_rotation);
//	osg::Vec3d dv = (osg::Vec3d(0.0f, 0.0f, -1.0f) * rotation_matrix) * (delta);
//	_center -= dv;
//	_lastDeltaVec = dv;
//	_distance = minDist;
}

void bimWorld::CustomCameraManipulator::endPushSide()
{
	if (m_movingBackward || m_movingForward || m_movingLeft || m_movingRight)
	{
		return;
	}
	//_distance = _lastDistance;
	//_center += _lastDeltaVec;
	// minimum distance
	float minDist = _minimumDistance;
	//if (getRelativeFlag(_minimumDistanceFlagIndex))
	//	minDist *= _modelSize;
	//_distance = minDist;
}

bool bimWorld::CustomCameraManipulator::KeyUpEvent(int key, const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us)
{
	auto iter = m_keyUpEventMap.find(key);
	if (iter == m_keyUpEventMap.end())
	{
		return false;
	}
	else
	{
		return (iter->second)(ea, us);
	}
}

bool bimWorld::CustomCameraManipulator::KeyDownEvent(int key, const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us)
{
	auto iter = m_keyDownEventMap.find(key);
	if (iter == m_keyDownEventMap.end())
	{
		return false;
	}
	else
	{
		return (iter->second)(ea, us);
	}
}

void bimWorld::CustomCameraManipulator::setByMatrix(const osg::Matrixd& matrix)
{
	//throw std::exception("The method or operation is not implemented.");
}

void bimWorld::CustomCameraManipulator::setByInverseMatrix(const osg::Matrixd& matrix)
{
	//throw std::exception("The method or operation is not implemented.");
}

osg::Matrixd bimWorld::CustomCameraManipulator::getMatrix() const
{
	//throw std::exception("The method or operation is not implemented.");
	return osg::Matrixd();
}

osg::Matrixd bimWorld::CustomCameraManipulator::getInverseMatrix() const
{
	//throw std::exception("The method or operation is not implemented.");
	return osg::Matrixd();
}
