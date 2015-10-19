#include "BIMFirstPersonManipulator.h"
#include "YZModelCoreInternal.h"

bimWorld::BIMFirstPersonManipulator::BIMFirstPersonManipulator(YZModelCoreInternal *host, double selectionPrecision /*= 1e-6*/) :m_cameraManip(host), PickBase(host, selectionPrecision)
{

}

bool bimWorld::BIMFirstPersonManipulator::performMovementLeftMouseButton(const double eventTimeDelta, const double dx, const double dy)
{
	return osgGA::StandardManipulator::performMovementLeftMouseButton(eventTimeDelta, dx, dy);
}

bool bimWorld::BIMFirstPersonManipulator::performMovementRightMouseButton(const double eventTimeDelta, const double dx, const double dy)
{
	return osgGA::FirstPersonManipulator::performMovementLeftMouseButton(eventTimeDelta, dx, dy);
}

bool bimWorld::BIMFirstPersonManipulator::performMouseDeltaMovement(const float /*dx*/, const float /*dy*/)
{
	return true;
}

bool bimWorld::BIMFirstPersonManipulator::handleMouseDrag(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us)
{
	if (PickBase::handleMouseDrag(ea, us))
		return true;
	return Super::handleMouseDrag(ea, us);
}

bool bimWorld::BIMFirstPersonManipulator::handleMousePush(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us)
{
	if (PickBase::handleMousePush(ea, us))
		return true;
	return Super::handleMousePush(ea, us);
}

bool bimWorld::BIMFirstPersonManipulator::handleMouseRelease(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us)
{
	if (PickBase::handleMouseRelease(ea, us))
		return true;
	return Super::handleMouseRelease(ea, us);
}

bool bimWorld::BIMFirstPersonManipulator::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
{
	if (!m_host || !m_host->_ViewerData()->isLoaded())
		return false;
	osgGA::GUIEventAdapter::ScrollingMotion sm = ea.getScrollingMotion();
	switch (sm)
	{
	case osgGA::GUIEventAdapter::SCROLL_DOWN:
	{

	}
		break;
	case osgGA::GUIEventAdapter::SCROLL_UP:
	{

	}
		break;
	}
	if (!ea.getHandled())
	{
		//m_pImpl->frame();
		switch (ea.getEventType())
		{
		case osgGA::GUIEventAdapter::KEYDOWN:
		{
			switch (ea.getKey())
			{
			default:
				break;
				//		case osgGA::GUIEventAdapter::KEY_Control_L:
				//     case osgGA::GUIEventAdapter::KEY_Control_R:
				//            m_ctrl = false;
				//            break;
				//
				//       case osgGA::GUIEventAdapter::KEY_Shift_L :
				//       case osgGA::GUIEventAdapter::KEY_Shift_R :
				//            m_shift = false;
				//            break;
				//case '2':mode = bimWorld::MultiSelect; return true;
				//case '1':mode = bimWorld::Orbit; return true;
				//case 'w':
				//{
				//	this->moveForward(0.4);
				//	break;
				//}
				//case 's':
				//{
				//	this->moveForward(-0.4);
				//	break;
				//}
				//case 'a':
				//{
				//	this->moveRight(-0.4);
				//	break;
				//}
				//case 'd':
				//{
				//	this->moveRight(0.4);
				//	break;
				//}
			}
			m_host->_RenderingThreads()->setNeedUpdateAndDraw(true);
		}
			break;

		case osgGA::GUIEventAdapter::DRAG:
		case osgGA::GUIEventAdapter::RESIZE:
		case osgGA::GUIEventAdapter::SCROLL:

		case osgGA::GUIEventAdapter::MOVE:
		case osgGA::GUIEventAdapter::DOUBLECLICK:

			m_host->_RenderingThreads()->updateSeveralTimes(1);
			break;
		case osgGA::GUIEventAdapter::PUSH:
		case osgGA::GUIEventAdapter::PEN_PROXIMITY_ENTER:
			m_host->_RenderingThreads()->setNeedUpdateAndDraw(true);
			break;
		case osgGA::GUIEventAdapter::RELEASE:
		case osgGA::GUIEventAdapter::PEN_PROXIMITY_LEAVE:
		case osgGA::GUIEventAdapter::KEYUP:
			m_host->_RenderingThreads()->setNeedUpdateAndDraw(false, 3);
			//if (ea.isMultiTouchEvent() && ea.getTouchData()->getNumTouchPoints() == 4)
			//{
			//	setMode(this->mode == bimWorld::Orbit ? bimWorld::MultiSelect : bimWorld::Orbit);
			//}
			break;
		default:
			break;
		}
	}

	return Super::handle(ea, aa);
}
