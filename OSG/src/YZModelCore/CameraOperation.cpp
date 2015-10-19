#include "CameraOperation.h"
#include "Event.h"
#include "YZModelCoreInternal.h"

bimWorld::CameraOperation::CameraOperation(BIMCameraManipulator* host)
{
	m_host = host;
	m_firstMoveForward = false;
	m_firstMoveBackward = false;
	m_firstMoveLeft = false;
	m_firstMoveRight = false;
}


void bimWorld::CameraOperation::onMoveBackward(std::chrono::system_clock::time_point tick)
{
	double time;
	if (m_firstMoveBackward)
	{
		time = 1;
		m_firstMoveBackward = false;
		m_host->beginPushForward(-getMoveFactor() * 0.5 * time);
	}
	else
	{
		time = std::chrono::duration_cast<std::chrono::milliseconds>(tick - m_lastOnMoveBackwardTime).count()/* / 33.0*/;
		time = time / 33.0 + 1;
		m_host->pushForward(-getMoveFactor() * 0.5 * time);
	}
	m_lastOnMoveBackwardTime = tick;
}

void bimWorld::CameraOperation::onMoveLeft(std::chrono::system_clock::time_point tick)
{
	double time;
	if (m_firstMoveLeft)
	{
		time = 1;
		m_firstMoveLeft = false;
		auto delta = getMoveFactor() *0.5* (-m_host->_distance) * time;
		m_host->beginPushSide(delta, 0);
	}
	else
	{
		time = std::chrono::duration_cast<std::chrono::milliseconds>(tick - m_lastOnMoveLeftTime).count();
		time = time / 33.0 + 1;
		auto delta = getMoveFactor() *0.5* (-m_host->_distance) * time;
		m_host->pushSide(delta, 0);
	}
	m_lastOnMoveLeftTime = tick;
}

void bimWorld::CameraOperation::onMoveRight(std::chrono::system_clock::time_point tick)
{
	double time;
	if (m_firstMoveRight)
	{
		time = 1;
		m_firstMoveRight = false;
		auto delta = getMoveFactor() *0.5* (m_host->_distance) * time;
		//auto delta = getMoveFactor()*(0.00088*m_host->_distance + m_host->m_moveDelta);
		m_host->beginPushSide(delta, 0);
	}
	else
	{
		time = std::chrono::duration_cast<std::chrono::milliseconds>(tick - m_lastOnMoveRightTime).count();
		time = time / 33.0 + 1;
		auto delta = getMoveFactor() *0.5* (m_host->_distance) * time;
		//auto delta = getMoveFactor()*(0.00088*m_host->_distance + m_host->m_moveDelta);
		m_host->pushSide(delta, 0);
	}
	m_lastOnMoveRightTime = tick;
}

void bimWorld::CameraOperation::onMoveForward(std::chrono::system_clock::time_point tick)
{
	double time;
	if (m_firstMoveForward)
	{
		time = 1;
		m_firstMoveForward = false;
		m_host->beginPushForward(getMoveFactor() * 0.5 * time);
	}
	else
	{
		time = std::chrono::duration_cast<std::chrono::milliseconds>(tick - m_lastOnMoveForwardTime).count();
		time = time / 33.0 + 1;
		m_host->pushForward(getMoveFactor() * 0.5 * time);
	}
	m_lastOnMoveForwardTime = tick;
}

float bimWorld::CameraOperation::getZoomDelta(float totalFrame)
{
	// minimum distance
	float minDist = m_host->_minimumDistance;
	if (m_host->getRelativeFlag(m_host->_minimumDistanceFlagIndex))
		minDist *= m_host->_modelSize;

	m_host->m_lastDist = m_host->_distance;

	if (m_host->_distance > minDist)
	{
		return (m_host->_distance - minDist) / totalFrame;
	}
	else
	{
		return 0;
	}
}

void bimWorld::CameraOperation::zoomForward(float delta, float frame)
{
	m_host->_distance = m_host->m_lastDist - delta*frame;
}

void bimWorld::CameraOperation::zoomBackward(float delta, float frame)
{
	// minimum distance
	float minDist = m_host->_minimumDistance;
	if (m_host->getRelativeFlag(m_host->_minimumDistanceFlagIndex))
		minDist *= m_host->_modelSize;
	m_host->_distance = minDist + delta*frame;
}

bool bimWorld::CameraOperation::pauseRendering()
{
	return !(m_host->m_movingForward || m_host->m_movingBackward || m_host->m_movingRight || m_host->m_movingLeft || m_host->m_isPushed);
}

float bimWorld::CameraOperation::getMoveFactor()
{
	auto moveFactor = 0.2f;
	if (m_host->m_shiftDown)
	{
		moveFactor = 1.0f;
	}
	return moveFactor;
}

bool bimWorld::CameraOperation::beginMoveForward(const osgGA::GUIEventAdapter& /*ea*/, osgGA::GUIActionAdapter& /*us*/)
{
	if (m_host->m_movingForward)
	{
		return true;
	}
	std::function<void(std::chrono::system_clock::time_point)> func = std::bind(&bimWorld::CameraOperation::onMoveForward, this, std::placeholders::_1);
	core::InstanceFunction<void(std::chrono::system_clock::time_point)> insfunc(func, this, "onMoveForward");
	m_host->m_host->RenderingThreads()->PreFrameUpdateEvent() += insfunc;
	m_host->m_host->RenderingThreads()->NoneFrameUpdateEvent() += insfunc;
	m_host->m_movingForward = true;
	m_firstMoveForward = true;
	return true;
}

bool bimWorld::CameraOperation::endMoveForward(const osgGA::GUIEventAdapter& /*ea*/, osgGA::GUIActionAdapter& /*us*/)
{
	std::function<void(std::chrono::system_clock::time_point)> func = std::bind(&bimWorld::CameraOperation::onMoveForward, this, std::placeholders::_1);
	core::InstanceFunction<void(std::chrono::system_clock::time_point)> insfunc(func, this, "onMoveForward");
	m_host->m_host->RenderingThreads()->PreFrameUpdateEvent() -= insfunc;
	m_host->m_host->RenderingThreads()->NoneFrameUpdateEvent() -= insfunc;
	m_host->m_movingForward = false;
	m_firstMoveForward = false;
	m_host->endPushForward();
	return true;
}

bool bimWorld::CameraOperation::beginMoveLeft(const osgGA::GUIEventAdapter& /*ea*/, osgGA::GUIActionAdapter& /*us*/)
{
	if (m_host->m_movingLeft)
	{
		return true;
	}
	std::function<void(std::chrono::system_clock::time_point)> func = std::bind(&bimWorld::CameraOperation::onMoveLeft, this, std::placeholders::_1);
	core::InstanceFunction<void(std::chrono::system_clock::time_point)> insfunc(func, this, "onMoveLeft");
	m_host->m_host->RenderingThreads()->PreFrameUpdateEvent() += insfunc;
	m_host->m_host->RenderingThreads()->NoneFrameUpdateEvent() += insfunc;
	m_host->m_movingLeft = true;
	m_firstMoveLeft = true;
	return true;
}

bool bimWorld::CameraOperation::endMoveLeft(const osgGA::GUIEventAdapter& /*ea*/, osgGA::GUIActionAdapter& /*us*/)
{
	std::function<void(std::chrono::system_clock::time_point)> func = std::bind(&bimWorld::CameraOperation::onMoveLeft, this, std::placeholders::_1);
	core::InstanceFunction<void(std::chrono::system_clock::time_point)> insfunc(func, this, "onMoveLeft");
	m_host->m_host->RenderingThreads()->PreFrameUpdateEvent() -= insfunc;
	m_host->m_host->RenderingThreads()->NoneFrameUpdateEvent() -= insfunc;
	m_host->m_movingLeft = false;
	m_firstMoveLeft = false;
	m_host->endPushSide();
	return true;
}

bool bimWorld::CameraOperation::beginMoveRight(const osgGA::GUIEventAdapter& /*ea*/, osgGA::GUIActionAdapter& /*us*/)
{
	if (m_host->m_movingRight)
	{
		return true;
	}
	std::function<void(std::chrono::system_clock::time_point)> func = std::bind(&bimWorld::CameraOperation::onMoveRight, this, std::placeholders::_1);
	core::InstanceFunction<void(std::chrono::system_clock::time_point)> insfunc(func, this, "onMoveRight");
	m_host->m_host->RenderingThreads()->PreFrameUpdateEvent() += insfunc;
	m_host->m_host->RenderingThreads()->NoneFrameUpdateEvent() += insfunc;
	m_host->m_movingRight = true;
	m_firstMoveRight = true;
	return true;
}

bool bimWorld::CameraOperation::endMoveRight(const osgGA::GUIEventAdapter& /*ea*/, osgGA::GUIActionAdapter& /*us*/)
{
	std::function<void(std::chrono::system_clock::time_point)> func = std::bind(&bimWorld::CameraOperation::onMoveRight, this, std::placeholders::_1);
	core::InstanceFunction<void(std::chrono::system_clock::time_point)> insfunc(func, this, "onMoveRight");
	m_host->m_host->RenderingThreads()->PreFrameUpdateEvent() -= insfunc;
	m_host->m_host->RenderingThreads()->NoneFrameUpdateEvent() -= insfunc;
	m_host->m_movingRight = false;
	m_firstMoveRight = false;
	m_host->endPushSide();
	return true;
}

bool bimWorld::CameraOperation::beginMoveBackward(const osgGA::GUIEventAdapter& /*ea*/, osgGA::GUIActionAdapter& /*us*/)
{
	if (m_host->m_movingBackward)
	{
		return true;
	}
	std::function<void(std::chrono::system_clock::time_point)> func = std::bind(&bimWorld::CameraOperation::onMoveBackward, this, std::placeholders::_1);
	core::InstanceFunction<void(std::chrono::system_clock::time_point)> insfunc(func, this, "onMoveBackward");
	m_host->m_host->RenderingThreads()->PreFrameUpdateEvent() += insfunc;
	m_host->m_host->RenderingThreads()->NoneFrameUpdateEvent() += insfunc;
	m_host->m_movingBackward = true;
	m_firstMoveBackward = true;
	return true;
}

bool bimWorld::CameraOperation::endMoveBackward(const osgGA::GUIEventAdapter& /*ea*/, osgGA::GUIActionAdapter& /*us*/)
{
	std::function<void(std::chrono::system_clock::time_point)> func = std::bind(&bimWorld::CameraOperation::onMoveBackward, this, std::placeholders::_1);
	core::InstanceFunction<void(std::chrono::system_clock::time_point)> insfunc(func, this, "onMoveBackward");
	m_host->m_host->RenderingThreads()->PreFrameUpdateEvent() -= insfunc;
	m_host->m_host->RenderingThreads()->NoneFrameUpdateEvent() -= insfunc;
	m_host->m_movingBackward = false;
	m_firstMoveBackward = false;
	m_host->endPushForward();
	return true;
}

bool bimWorld::CameraOperation::onHome(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us)
{
	m_host->home(ea, us);
	m_host->scaleOption = true;
	return true;
}

bool bimWorld::CameraOperation::onZoomModel(const double eventTimeDelta, const float /*dx*/, const float dy)
{
	// zoom model
	m_host->zoomModel(dy * m_host->getThrowScale(eventTimeDelta), false);
	return true;
}

bool bimWorld::CameraOperation::onPanModel(const double eventTimeDelta, const float dx, const float dy)
{
	// pan model
	float scale = -0.4f * m_host->_distance * m_host->getThrowScale(eventTimeDelta);
	m_host->panModel(dx*scale, dy*scale);
	return true;
}

bool bimWorld::CameraOperation::onRotateCamera(const double eventTimeDelta, const float dx, const float dy)
{
	// rotate camera
	if (m_host->getVerticalAxisFixed())
		m_host->rotateWithFixedVertical(dx, dy);
	else
		m_host->rotateTrackball(m_host->_ga_t0->getXnormalized(), m_host->_ga_t0->getYnormalized(),
		m_host->_ga_t1->getXnormalized(), m_host->_ga_t1->getYnormalized(),
		m_host->getThrowScale(eventTimeDelta));
	return true;
}
