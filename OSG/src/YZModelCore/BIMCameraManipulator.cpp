#include "BIMCameraManipulator.h"
#include "YZModelCoreInternal.h"
#include <math.h>
#include "CameraOperation.h"
#include <osg/ShapeDrawable>

bimWorld::BIMCameraManipulator::BIMCameraManipulator(YZModelCoreInternal *host, bool limitedSpace /*= false*/, double selectionPrecision /*= 1e-6*/) :
limitedSpace(limitedSpace), PickBase(host, selectionPrecision), osgGA::MultiTouchTrackballManipulator(), m_operation(new bimWorld::CameraOperation(this))
{
	this->setAllowThrow(false);
	m_isSettingCameraMatrix = false;
	_manipContainer = new Manipulator::MouseManipulatorContainer();
	_allowThrow = false;
	PickBase::CameraManipulatoerPickBase(host, selectionPrecision);
	setMinimumDistance(1);
	setVerticalAxisFixed(true);
	m_moveDelta = 0.07;
	m_movingForward = false;
	m_movingBackward = false;
	m_movingLeft = false;
	m_movingRight = false;
	m_isPushed = false;
	m_shiftDown = false;
	m_lastDist = 0;
	setRotationHandleBtn(osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON);
	scaleOption = true;

	bindDefaultEvent();
}

bimWorld::BIMCameraManipulator::~BIMCameraManipulator()
{
	if (m_operation)
	{
		delete m_operation;
		m_operation = NULL;
	}

	if (_manipContainer)
	{
		delete _manipContainer;
		_manipContainer = NULL;
	}
}

void bimWorld::BIMCameraManipulator::updateModelSize()
{
	this->_modelSize = (m_host->_ViewerData()->getModelRoot() == NULL ? 0.0 : m_host->_ViewerData()->getModelRoot()->getBound().radius());
}

void bimWorld::BIMCameraManipulator::beginSetCameraMatrix()
{
	m_isSettingCameraMatrix = true;
}

void bimWorld::BIMCameraManipulator::endSetCameraMatrix()
{
	m_isSettingCameraMatrix = false;
}

void bimWorld::BIMCameraManipulator::pushForward(const float cdy)
{
	auto dy = cdy;

#ifdef TARGET_IPHONE_SIMULATOR
	dy = 0.5 * dy;
#elif _WIN32
	dy = -dy;
#else
	dy = dy;
#endif
	//dy = (scaleOption ? dy : -dy);


	// minimum distance
	float minDist = _minimumDistance * 1000;
	if (getRelativeFlag(_minimumDistanceFlagIndex))
		minDist *= _modelSize;

	if (_distance > minDist)
	{
#ifdef _WIN32
		float delta = _distance - minDist;
#else
		float delta = minDist - _distance;
#endif // _WIN32

		osg::Matrixd rotation_matrix(_rotation);
		osg::Vec3d dv = (osg::Vec3d(0.0f, 0.0f, -1.0f) * rotation_matrix) * (delta);
		_center -= dv;
		_distance = minDist;
	}
	else
	{
		// push the camera forward
		float delta = -minDist;
		osg::Matrixd rotation_matrix(_rotation);
		osg::Vec3d dv = (osg::Vec3d(0.0f, 0.0f, -1.0f) * rotation_matrix) * (dy * delta);
		_center += dv;
	}

	// limit if needed, keep max size as.
	if (limitedSpace && m_host->_ViewerData()->getModelRoot() != NULL && this->_distance > 3 * _modelSize)
		this->_distance = 3 * _modelSize;
}

void bimWorld::BIMCameraManipulator::beginPushForward(const float cdy)
{
	auto dy = cdy;

#ifdef TARGET_IPHONE_SIMULATOR
	dy = 0.5 * dy;
#elif _WIN32
	dy = -dy;
#else
	dy = dy;
#endif
	dy = (scaleOption ? dy : -dy);

	// minimum distance
	float minDist = _minimumDistance * 1000;
	if (getRelativeFlag(_minimumDistanceFlagIndex))
		minDist *= _modelSize;

#ifdef TARGET_IPHONE_SIMULATOR
	float delta = 0.5 * (minDist - _distance);
#elif  _WIN32
	float delta = _distance - minDist;
#else
	float delta = minDist - _distance;
#endif // _WIN32
	_lastDistance = _distance;
	osg::Matrixd rotation_matrix(_rotation);
	osg::Vec3d dv = (osg::Vec3d(0.0f, 0.0f, -1.0f) * rotation_matrix) * (delta);
	_center -= dv;
	_lastDeltaVec = dv;
	_distance = minDist;

	// limit if needed, keep max size as.
	if (limitedSpace && m_host->_ViewerData()->getModelRoot() != NULL && this->_distance > 3 * _modelSize)
		this->_distance = 3 * _modelSize;
}

void bimWorld::BIMCameraManipulator::endPushForward()
{
	if (m_movingBackward || m_movingForward || m_movingLeft || m_movingRight)
		return;
	//_distance = _lastDistance;
	//_center += _lastDeltaVec;
	// minimum distance
	float minDist = _minimumDistance * 1000;
	if (getRelativeFlag(_minimumDistanceFlagIndex))
		minDist *= _modelSize;
	_distance = minDist;
}

void bimWorld::BIMCameraManipulator::zoomModel(const float cdy, bool pushForwardIfNeeded)
{
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
}
#define CodingZoom 1
void bimWorld::BIMCameraManipulator::zoomModelLocally(const float pointX, const float pointY, const float cdy, bool pushForwardIfNeeded)
{
#if CodingZoom

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

void bimWorld::BIMCameraManipulator::zoomModelWithSelectedCenter(const float cdy, bool pushForwardIfNeeded /*= true*/)
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
	up = _rotation * osg::Vec3d(0., 1., 0.);
	eye = _center - _rotation * osg::Vec3d(0., 0., -_distance);

	auto tcenter = m_host->_ViewerData()->getSelectedCenter();//osg::Vec3f(pointX, pointY, z);
	auto vec = tcenter - eye;
	vec.normalize();
	auto distVec = osg::Vec3(tcenter.x()*vec.x(), tcenter.y()*vec.y(), tcenter.z()*vec.z()) - osg::Vec3(eye.x()*vec.x(), eye.y()*vec.y(), eye.z()*vec.z());
	auto length = distVec.length();
	float delta = dy * -_distance*0.2;
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
	setTransformation(eye + dv, _rotation);


}

void bimWorld::BIMCameraManipulator::panModel(const float dx, const float dy, const float dz/* = 0*/)
{
#ifndef TARGET_IPHONE_SIMULATOR

	osg::Matrix rotation_matrix;
	rotation_matrix.makeRotate(_rotation);

	osg::Vec3d dv(dx, dy, dz);

	_center += dv * rotation_matrix;

	// limit if needed, keep pan center inside model binding sphere.
	osg::Vec3d deltaDist = this->_center - this->_homeCenter;
	if (limitedSpace && deltaDist.length() > 0.5 * _modelSize)
	{
		this->_center = this->_homeCenter + deltaDist * 0.5 * _modelSize / deltaDist.length();
	}

#endif
}

void bimWorld::BIMCameraManipulator::pushSide(const float dx, const float dy, const float dz/* = 0*/)
{
#ifndef TARGET_IPHONE_SIMULATOR

	osg::Matrix rotation_matrix;
	rotation_matrix.makeRotate(_rotation);

	osg::Vec3d dv(dx, dy, dz);

	_center += dv * 1000 * rotation_matrix;

	// limit if needed, keep pan center inside model binding sphere.
	osg::Vec3d deltaDist = this->_center - this->_homeCenter;
	if (limitedSpace && deltaDist.length() > 0.5 * _modelSize)
	{
		this->_center = this->_homeCenter + deltaDist * 0.5 * _modelSize / deltaDist.length();
	}

#endif
}

void bimWorld::BIMCameraManipulator::beginPushSide(const float dx, const float dy, const float dz /*= 0*/)
{

	// minimum distance
	float minDist = _minimumDistance;
	if (getRelativeFlag(_minimumDistanceFlagIndex))
		minDist *= _modelSize;

#ifdef TARGET_IPHONE_SIMULATOR
	float delta = 0.5 * (minDist - _distance);
#elif  _WIN32
	float delta = _distance - minDist;
#else
	float delta = minDist - _distance;
#endif // _WIN32
	_lastDistance = _distance;
	osg::Matrixd rotation_matrix(_rotation);
	osg::Vec3d dv = (osg::Vec3d(0.0f, 0.0f, -1.0f) * rotation_matrix) * (delta);
	_center -= dv;
	_lastDeltaVec = dv;
	_distance = minDist;
}

void bimWorld::BIMCameraManipulator::endPushSide()
{
	if (m_movingBackward || m_movingForward || m_movingLeft || m_movingRight)
	{
		return;
	}
	//_distance = _lastDistance;
	//_center += _lastDeltaVec;
	// minimum distance
	float minDist = _minimumDistance;
	if (getRelativeFlag(_minimumDistanceFlagIndex))
		minDist *= _modelSize;
	_distance = minDist;
}

void bimWorld::BIMCameraManipulator::vTranslation(double dy)
{
	this->_homeCenter += _rotation*osg::Vec3d(0.0, dy, 0.0);
}

void bimWorld::BIMCameraManipulator::hTranslation(double dy)
{
	this->_homeCenter += _rotation*osg::Vec3d(dy, 0.0, 0.0);
}

void bimWorld::BIMCameraManipulator::setPosition(const osg::Vec3& eye, const osg::Vec3& center, const osg::Vec3& up, const osg::Matrix& matrix)
{
	setHomePosition(eye, center, up);
	home(0);
	_rotation = matrix.getRotate();

	//            _rotation = matrix.getRotate();
	auto mat = matrix * osg::Matrix::inverse(osg::Matrix::translate(_center)) * osg::Matrix::inverse(osg::Matrix::rotate(_rotation));
	auto trans = mat.getTrans();
	_distance = trans.z();
	_center = osg::Vec3d(0., 0., -_distance) * matrix;
}

void bimWorld::BIMCameraManipulator::setByMatrix(const osg::Matrixd& matrix)
{
	if (m_isSettingCameraMatrix)
	{
		_rotation = matrix.getRotate();
		auto mat = matrix * osg::Matrix::inverse(osg::Matrix::translate(_center)) * osg::Matrix::inverse(osg::Matrix::rotate(_rotation));
		auto trans = mat.getTrans();
		_distance = trans.z();
	}
	else
	{
		osgGA::MultiTouchTrackballManipulator::setByMatrix(matrix);
	}
}

void bimWorld::BIMCameraManipulator::getStandardManipulatorParam(osg::Vec3d& eye, osg::Vec3d& center, osg::Vec3d& up)
{
	//            m_isSettingCameraMatrix = true;
	//            setByMatrix(getMatrix());
	//            m_isSettingCameraMatrix = false;

	getHomePosition(eye, center, up);
}

bool bimWorld::BIMCameraManipulator::handleMouseMove(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us)
{
	return PickBase::handleMouseMove(ea, us);
}

bool bimWorld::BIMCameraManipulator::handleMouseDrag(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us)
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

	addMouseEvent(ea);

	if (performMovement())
	{
		//us.requestRedraw();
		m_host->RenderingThreads()->updateSeveralTimes(1);
	}

	//us.requestContinuousUpdate(false);
	//m_host->RenderingThreads()->setNeedUpdateAndDraw(false, 3);
	_thrown = false;

	return true;
}

bool bimWorld::BIMCameraManipulator::handleMousePush(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us)
{
	if (!m_host || !m_host->_ViewerData()->isLoaded())
		return false;

	if (_manipContainer->handleMousePush(ea, us))
		return true;


	if (PickBase::handleMousePush(ea, us))
		return true;

	flushMouseEventStack();
	addMouseEvent(ea);

	if (performMovement())
	{
		//us.requestRedraw();
		m_host->RenderingThreads()->updateSeveralTimes(1);
	}

	//us.requestContinuousUpdate(false);
	//m_host->RenderingThreads()->setNeedUpdateAndDraw(false, 3);
	_thrown = false;

	return true;
}

bool bimWorld::BIMCameraManipulator::handleMouseRelease(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us)
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

		double timeSinceLastRecordEvent = _ga_t0.valid() ? (ea.getTime() - _ga_t0->getTime()) : DBL_MAX;
		if (timeSinceLastRecordEvent > 0.02)
			flushMouseEventStack();

		if (isMouseMoving())
		{

			if (performMovement() && _allowThrow)
			{
				//us.requestRedraw();
				//us.requestContinuousUpdate(true);
				m_host->RenderingThreads()->setNeedUpdateAndDraw(true);
				_thrown = true;
			}

			return true;
		}
	}

	flushMouseEventStack();
	addMouseEvent(ea);
	if (performMovement())
	{
		m_host->RenderingThreads()->updateSeveralTimes(1);
		//us.requestRedraw();
	}
	//us.requestContinuousUpdate(false);
	//m_host->RenderingThreads()->setNeedUpdateAndDraw(false, 3);
	_thrown = false;

	return true;
}

bool bimWorld::BIMCameraManipulator::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us)
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
				flushMouseEventStack();
				_thrown = false;
				home(ea, us);
				handled = true;
			}

			else if (data->getNumTouchPoints() >= 2)
			{
				if ((_lastTouchData.valid()) && (_lastTouchData->getNumTouchPoints() >= 2))
				{
					handleMultiTouchDrag(data, _lastTouchData.get(), eventTimeDelta);
				}

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
		if (_flags & PROCESS_MOUSE_WHEEL)
			return handleMouseWheel(ea, us);
		else
			return false;

	default:
		return false;
	}

	//return Super::handle(ea, us);
}

bimWorld::Manipulator::MouseManipulatorContainer* bimWorld::BIMCameraManipulator::getManipContainer()
{
	return _manipContainer.get();
}

bool bimWorld::BIMCameraManipulator::handleKeyDown(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us)
{

	switch (ea.getKey())
	{
	case osgGA::GUIEventAdapter::KEY_Shift_L:
	{
		m_shiftDown = true;
	}
	}
	flushMouseEventStack();
	_thrown = false;
	if (KeyDownEvent(ea.getUnmodifiedKey(), ea, us))
		return true;
	if (KeyDownEvent(ea.getKey(), ea, us))
		return true;
	return false;
}

bool bimWorld::BIMCameraManipulator::handleKeyUp(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us)
{
	switch (ea.getKey())
	{
	case osgGA::GUIEventAdapter::KEY_Shift_L:
	{
		m_shiftDown = false;
	}
	}
	flushMouseEventStack();
	_thrown = false;
	if (KeyUpEvent(ea.getUnmodifiedKey(), ea, us))
		return true;
	if (KeyUpEvent(ea.getKey(), ea, us))
		return true;

	return false;
}

bool bimWorld::BIMCameraManipulator::handleFrame(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us)
{
	double current_frame_time = ea.getTime();

	_delta_frame_time = current_frame_time - _last_frame_time;
	_last_frame_time = current_frame_time;

	if (_thrown && performMovement())
	{
		//us.requestRedraw();
		m_host->RenderingThreads()->updateSeveralTimes(1);
	}

	if (_animationData && _animationData->_isAnimating)
	{
		performAnimationMovement(ea, us);
	}

	return false;
}

bool bimWorld::BIMCameraManipulator::handleResize(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us)
{
	init(ea, us);
	//us.requestRedraw();
	m_host->RenderingThreads()->updateSeveralTimes(1);

	return true;
}

bool bimWorld::BIMCameraManipulator::handleMouseWheel(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us)
{
	osgGA::GUIEventAdapter::ScrollingMotion sm = ea.getScrollingMotion();

	// handle centering
	if (_flags & SET_CENTER_ON_WHEEL_FORWARD_MOVEMENT)
	{

		if (((sm == osgGA::GUIEventAdapter::SCROLL_DOWN && _wheelZoomFactor > 0.)) ||
			((sm == osgGA::GUIEventAdapter::SCROLL_UP   && _wheelZoomFactor < 0.)))
		{

			if (getAnimationTime() <= 0.)
			{
				// center by mouse intersection (no animation)
				setCenterByMousePointerIntersection(ea, us);
			}
			else
			{
				// start new animation only if there is no animation in progress
				if (!isAnimating())
					startAnimationByMousePointerIntersection(ea, us);

			}

		}
	}

	switch (sm)
	{
		// mouse scroll up event
	case osgGA::GUIEventAdapter::SCROLL_UP:
	{
		// perform zoom
		//zoomModel(_wheelZoomFactor, false);
		//zoomModelLocally(ea.getX(), ea.getY(), _wheelZoomFactor, false);
		zoomModelWithSelectedCenter(_wheelZoomFactor, false);
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
		//zoomModelLocally(ea.getX(), ea.getY(), -_wheelZoomFactor, false);
		zoomModelWithSelectedCenter(-_wheelZoomFactor, false);
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

void bimWorld::BIMCameraManipulator::handleMultiTouchDrag(osgGA::GUIEventAdapter::TouchData* now, osgGA::GUIEventAdapter::TouchData* last, double eventTimeDelta)
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
		zoomModel((gap_last - gap_now) * eventTimeDelta, false);
	}

	// drag gesture

	osg::Vec2 delta = ((pt_1_last - pt_1_now) + (pt_2_last - pt_2_now)) / 2.0f;

	float scale = 0.2f * _distance * eventTimeDelta;

	// osg::notify(osg::ALWAYS) << "drag: " << delta << " scale: " << scale << std::endl;

	panModel(delta.x() * scale, delta.y() * scale * (-1)); // flip y-coordinate because of different origins.
}

bool bimWorld::BIMCameraManipulator::performMovementLeftMouseButton(const double eventTimeDelta, const double dx, const double dy)
{
	auto iter = m_mouseEventMap.find(bimWorld::LEFT_MOUSE_BUTTON);
	if (iter == m_mouseEventMap.end())
	{
		return false;
	}
	else
	{
		return (iter->second)(eventTimeDelta, dx, dy);
	}
}

bool bimWorld::BIMCameraManipulator::performMovementMiddleMouseButton(const double eventTimeDelta, const double dx, const double dy)
{
	auto iter = m_mouseEventMap.find(bimWorld::MIDDLE_MOUSE_BUTTON);
	if (iter == m_mouseEventMap.end())
	{
		return false;
	}
	else
	{
		return (iter->second)(eventTimeDelta, dx, dy);
	}
}

bool bimWorld::BIMCameraManipulator::performMovementRightMouseButton(const double eventTimeDelta, const double dx, const double dy)
{
	auto iter = m_mouseEventMap.find(bimWorld::RIGHT_MOUSE_BUTTON);
	if (iter == m_mouseEventMap.end())
	{
		return false;
	}
	else
	{
		return (iter->second)(eventTimeDelta, dx, dy);
	}
}

bool bimWorld::BIMCameraManipulator::performMouseDeltaMovement(const float dx, const float dy)
{
	// rotate camera
	if (getVerticalAxisFixed())
		rotateWithFixedVertical(dx, dy);
	else
		rotateTrackball(0.f, 0.f, dx, dy, 1.f);

	return true;
}

bool bimWorld::BIMCameraManipulator::performMovement()
{
	// return if less then two events have been added
	if (_ga_t0.get() == NULL || _ga_t1.get() == NULL)
		return false;

	// get delta time
	double eventTimeDelta = _ga_t0->getTime() - _ga_t1->getTime();
	if (eventTimeDelta < 0.)
	{
		OSG_WARN << "Manipulator warning: eventTimeDelta = " << eventTimeDelta << std::endl;
		eventTimeDelta = 0.;
	}

	// get deltaX and deltaY
	float dx = _ga_t0->getXnormalized() - _ga_t1->getXnormalized();
	float dy = _ga_t0->getYnormalized() - _ga_t1->getYnormalized();

	// return if there is no movement.
	if (dx == 0. && dy == 0.)
		return false;


	// call appropriate methods
	unsigned int buttonMask = _ga_t1->getButtonMask();
	if (buttonMask == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
	{
		return performMovementLeftMouseButton(eventTimeDelta, dx, dy);
	}
	else if (buttonMask == osgGA::GUIEventAdapter::MIDDLE_MOUSE_BUTTON/* ||
		buttonMask == (osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON | osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON)*/)
	{
		return performMovementMiddleMouseButton(eventTimeDelta, dx, dy);
	}
	else if (buttonMask == osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON)
	{
		return performMovementRightMouseButton(eventTimeDelta, dx, dy);
	}

	return false;
}

//void bimWorld::BIMCameraManipulator::key_w_up()
//{
//	flushMouseEventStack();
//	_thrown = false;
//	KeyUpEvent();
//	//std::function<void()> func = std::bind(&bimWorld::BIMCameraManipulator::onMoveForward, this);
//	//core::InstanceFunction<void()> insfunc(func, this, "onMoveForward");
//	//m_host->RenderingThreads()->PreFrameUpdateEvent() -= insfunc;
//	//m_host->RenderingThreads()->NoneFrameUpdateEvent() -= insfunc;
//	//m_movingForward = false;
//}
//
//void bimWorld::BIMCameraManipulator::key_s_up()
//{
//	flushMouseEventStack();
//	_thrown = false;
//	key_s_up_event();
//	//std::function<void()> func = std::bind(&bimWorld::BIMCameraManipulator::onMoveBackward, this);
//	//core::InstanceFunction<void()> insfunc(func, this, "onMoveBackward");
//	//m_host->RenderingThreads()->PreFrameUpdateEvent() -= insfunc;
//	//m_host->RenderingThreads()->NoneFrameUpdateEvent() -= insfunc;
//	//m_movingBackward = false;
//}
//
//void bimWorld::BIMCameraManipulator::key_a_up()
//{
//	flushMouseEventStack();
//	_thrown = false;
//	key_a_up_event();
//	//std::function<void()> func = std::bind(&bimWorld::BIMCameraManipulator::onMoveLeft, this);
//	//core::InstanceFunction<void()> insfunc(func, this, "onMoveLeft");
//	//m_host->RenderingThreads()->PreFrameUpdateEvent() -= insfunc;
//	//m_host->RenderingThreads()->NoneFrameUpdateEvent() -= insfunc;
//	//m_movingLeft = false;
//}
//
//void bimWorld::BIMCameraManipulator::key_d_up()
//{
//	flushMouseEventStack();
//	_thrown = false;
//	key_d_up_event();
//	//std::function<void()> func = std::bind(&bimWorld::BIMCameraManipulator::onMoveRight, this);
//	//core::InstanceFunction<void()> insfunc(func, this, "onMoveRight");
//	//m_host->RenderingThreads()->PreFrameUpdateEvent() -= insfunc;
//	//m_host->RenderingThreads()->NoneFrameUpdateEvent() -= insfunc;
//	//m_movingRight = false;
//}
//
//void bimWorld::BIMCameraManipulator::key_w_down()
//{
//	flushMouseEventStack();
//	_thrown = false;
//	key_w_down_event();
//
//	//std::function<void()> func = std::bind(&bimWorld::BIMCameraManipulator::onMoveForward, this);
//	//core::InstanceFunction<void()> insfunc(func, this, "onMoveForward");
//	//m_host->RenderingThreads()->PreFrameUpdateEvent() += insfunc;
//	//m_host->RenderingThreads()->NoneFrameUpdateEvent() += insfunc;
//	//m_movingForward = true;
//}
//
//void bimWorld::BIMCameraManipulator::key_s_down()
//{
//	flushMouseEventStack();
//	_thrown = false;
//	key_s_down_event();
//
//	//std::function<void()> func = std::bind(&bimWorld::BIMCameraManipulator::onMoveBackward, this);
//	//core::InstanceFunction<void()> insfunc(func, this, "onMoveBackward");
//	//m_host->RenderingThreads()->PreFrameUpdateEvent() += insfunc;
//	//m_host->RenderingThreads()->NoneFrameUpdateEvent() += insfunc;
//	//m_movingBackward = true;
//}
//
//void bimWorld::BIMCameraManipulator::key_a_down()
//{
//	flushMouseEventStack();
//	_thrown = false;
//	key_a_down_event();
//
//	//std::function<void()> func = std::bind(&bimWorld::BIMCameraManipulator::onMoveLeft, this);
//	//core::InstanceFunction<void()> insfunc(func, this, "onMoveLeft");
//	//m_host->RenderingThreads()->PreFrameUpdateEvent() += insfunc;
//	//m_host->RenderingThreads()->NoneFrameUpdateEvent() += insfunc;
//	//m_movingLeft = true;
//}
//
//void bimWorld::BIMCameraManipulator::key_d_down()
//{
//	flushMouseEventStack();
//	_thrown = false;
//	key_d_down_event();
//
//	//std::function<void()> func = std::bind(&bimWorld::BIMCameraManipulator::onMoveRight, this);
//	//core::InstanceFunction<void()> insfunc(func, this, "onMoveRight");
//	//m_host->RenderingThreads()->PreFrameUpdateEvent() += insfunc;
//	//m_host->RenderingThreads()->NoneFrameUpdateEvent() += insfunc;
//	//m_movingRight = true;
//}

void bimWorld::BIMCameraManipulator::setRotationHandleBtn(osgGA::GUIEventAdapter::MouseButtonMask btn)
{
	switch (btn)
	{
	case osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON:
		m_leftBtnHandleRotate = true;
		m_rightBtnHandleRotate = false;
		break;
	default:
	case osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON:
		m_leftBtnHandleRotate = false;
		m_rightBtnHandleRotate = true;
		break;
	}
}

void bimWorld::BIMCameraManipulator::setModelCenterKeepDist(const osg::Vec3d& center)
{
	_center = center;
}

void bimWorld::BIMCameraManipulator::setModelCenter(const osg::Vec3d& center)
{
	auto eye = _center - _rotation * osg::Vec3d(0., 0., -_distance);
	_distance = (eye - center).length();
	_center = center;
}

void bimWorld::BIMCameraManipulator::setRotationCenter(const osg::Vec3d& center)
{
	auto cc = _center - center;
	auto ec = this->_homeEye - center;
	auto e_c = this->_homeEye - _center;
	auto a = center;
	auto b = a - e_c;
	//auto ccLength = cc.length();
	//auto ecLength = ec.length();
	//auto angle = asin(ccLength / ecLength);
	//auto axis = osg::Vec3(y1z2 - z1y2, z1x2 - x1z2, x1y2 - y1x2)
	//auto e_c = this->_homeEye - _center;
	//auto axis = osg::Vec3(e_c.y()*ec.z() - e_c.z()*ec.y(), e_c.z()*ec.x() - e_c.x()*ec.z(), e_c.x()*ec.y() - e_c.y()*ec.x());
	////auto a1 = osg::RadiansToDegrees(angle);
	//_rotation.makeRotate(angle, axis);
	//_rotation.makeRotate(e_c, ec);
	//_center - _rotation * osg::Vec3d(0., 0., -_distance) = _center - _rotation * osg::Vec3d(0., 0., -_distance);
	//_rotation * osg::Vec3d(0., 0., -_distance) = _center - (center - _rotation * osg::Vec3d(0., 0., -ecLength));
	//auto res = _center - (center - _rotation * osg::Vec3d(0., 0., -ecLength));
	//auto rot = osg::Matrix::rotate(osg::Vec3d(0., 0., -_distance), res);
	auto rot = _rotation;
	_center = center;


	osg::Matrix rotation_matrix;
	rotation_matrix.makeRotate(rot);

	_center += cc * rotation_matrix;

	//_distance = ecLength;
	//_rotation = (_center - eye) / osg::Vec3d(0., 0., -_distance);
	//auto rot = osg::Matrix::rotate(osg::Vec3d(0., 0., -_distance), _center - _homeEye);
	//_rotation = rot.getRotate();
	//_rotation = (_center - eye) / osg::Vec3d(0., 0., -_distance);

	//setHomePosition(_homeEye, center, _homeUp);
	//home(1);

	////_rotation.makeRotate(angle, axis);
}

bool bimWorld::BIMCameraManipulator::KeyUpEvent(int key, const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us)
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

bool bimWorld::BIMCameraManipulator::KeyDownEvent(int key, const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us)
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

void bimWorld::BIMCameraManipulator::bindKeyUpEvent(bimWorld::KeySymbol key, bimWorld::CameraOperationTypes operation)
{
	std::function<bool(const osgGA::GUIEventAdapter&, osgGA::GUIActionAdapter&)> func;
	if (!getKeyOperationFunc(operation, func))
		return;
	m_keyUpEventMap[key] = func;
}

void bimWorld::BIMCameraManipulator::bindKeyDownEvent(bimWorld::KeySymbol key, bimWorld::CameraOperationTypes operation)
{
	std::function<bool(const osgGA::GUIEventAdapter&, osgGA::GUIActionAdapter&)> func;
	if (!getKeyOperationFunc(operation, func))
		return;
	m_keyDownEventMap[key] = func;
}

void bimWorld::BIMCameraManipulator::unbindKeyUpEvent(bimWorld::KeySymbol key)
{
	m_keyUpEventMap.erase(key);
}

void bimWorld::BIMCameraManipulator::unbindKeyDownEvent(bimWorld::KeySymbol key)
{
	m_keyDownEventMap.erase(key);
}

void bimWorld::BIMCameraManipulator::unbindMouseEvent(bimWorld::MouseButtonMask mouse)
{
	m_mouseEventMap.erase(mouse);
}

bool bimWorld::BIMCameraManipulator::getKeyOperationFunc(bimWorld::CameraOperationTypes operation, std::function<bool(const osgGA::GUIEventAdapter&, osgGA::GUIActionAdapter&)> &func)
{
	switch (operation)
	{
	case bimWorld::emptyOperation:
		func = [](const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us){ return false; };
		break;
	case bimWorld::beginMoveForward:
		func = std::bind(&bimWorld::CameraOperation::beginMoveForward, m_operation, std::placeholders::_1, std::placeholders::_2);
		break;
	case bimWorld::endMoveForward:
		func = std::bind(&bimWorld::CameraOperation::endMoveForward, m_operation, std::placeholders::_1, std::placeholders::_2);
		break;
	case bimWorld::beginMoveLeft:
		func = std::bind(&bimWorld::CameraOperation::beginMoveLeft, m_operation, std::placeholders::_1, std::placeholders::_2);
		break;
	case bimWorld::endMoveLeft:
		func = std::bind(&bimWorld::CameraOperation::endMoveLeft, m_operation, std::placeholders::_1, std::placeholders::_2);
		break;
	case bimWorld::beginMoveRight:
		func = std::bind(&bimWorld::CameraOperation::beginMoveRight, m_operation, std::placeholders::_1, std::placeholders::_2);
		break;
	case bimWorld::endMoveRight:
		func = std::bind(&bimWorld::CameraOperation::endMoveRight, m_operation, std::placeholders::_1, std::placeholders::_2);
		break;
	case bimWorld::beginMoveBackward:
		func = std::bind(&bimWorld::CameraOperation::beginMoveBackward, m_operation, std::placeholders::_1, std::placeholders::_2);
		break;
	case bimWorld::endMoveBackward:
		func = std::bind(&bimWorld::CameraOperation::endMoveBackward, m_operation, std::placeholders::_1, std::placeholders::_2);
		break;
	case bimWorld::onHome:
		func = std::bind(&bimWorld::CameraOperation::onHome, m_operation, std::placeholders::_1, std::placeholders::_2);
		break;
	default:
		assert(false);
		return false;
	}
	return true;
}

void bimWorld::BIMCameraManipulator::bindDefaultEvent()
{

	bindKeyUpEvent(KEY_Space, onHome);

	// bindMouseEvent(LEFT_MOUSE_BUTTON, onRotateCamera);
	bindMouseEvent(MIDDLE_MOUSE_BUTTON, onPanModel);
	bindMouseEvent(RIGHT_MOUSE_BUTTON, onRotateCamera);
}

void bimWorld::BIMCameraManipulator::bindMouseEvent(bimWorld::MouseButtonMask mouse, bimWorld::CameraOperationTypes operation)
{
	std::function<bool(const double, const double, const double)> func;
	if (!getMouseOperationFunc(operation, func))
		return;
	m_mouseEventMap[mouse] = func;
}

bool bimWorld::BIMCameraManipulator::getMouseOperationFunc(bimWorld::CameraOperationTypes operation, std::function<bool(const double, const double, const double)> &func)
{
	switch (operation)
	{
	case bimWorld::emptyOperation:
		func = [](const double eventTimeDelta, const float dx, const float dy){ return false; };
		break;
	case bimWorld::onZoomModel:
		func = std::bind(&bimWorld::CameraOperation::onZoomModel, m_operation, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
		break;
	case bimWorld::onPanModel:
		func = std::bind(&bimWorld::CameraOperation::onPanModel, m_operation, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
		break;
	case bimWorld::onRotateCamera:
		func = std::bind(&bimWorld::CameraOperation::onRotateCamera, m_operation, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
		break;
	default:
		assert(false);
		return false;
	}
	return true;
}
