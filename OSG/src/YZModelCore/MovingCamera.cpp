#include "MovingCamera.h"
#include "YZModelCoreInternal.h"
#include "PersonManipulator.h"

bimWorld::MovingCamera::MovingCamera(YZModelCoreInternal* host) :YZModelCoreModule(host)
{

}

void bimWorld::MovingCamera::moveHorizontal(double distance)
{	
	auto manip = static_cast<PersonManipulator*>(m_host->_CameraManipulator()->getCameraManipulator());
	if (!manip)
		return;
	manip->moveRound(distance);
	m_host->_RenderingThreads()->updateSeveralTimes();
	
}

void bimWorld::MovingCamera::moveVertical(double distance)
{

	auto manip = static_cast<PersonManipulator*>(m_host->_CameraManipulator()->getCameraManipulator());
	if (!manip)
		return;

	manip->moveFront(distance);
	m_host->_RenderingThreads()->updateSeveralTimes();

}

void bimWorld::MovingCamera::moveUp(double distance)
{
	auto manip = static_cast<PersonManipulator*>(m_host->_CameraManipulator()->getCameraManipulator());
	if (!manip)
		return;
	manip->moveUp(distance);
	m_host->_RenderingThreads()->updateSeveralTimes();

}

void bimWorld::MovingCamera::moveRight(double distance)
{
	auto manip = static_cast<PersonManipulator*>(m_host->_CameraManipulator()->getCameraManipulator());
	if (!manip)
		return;
	manip->moveRight(distance);
	m_host->_RenderingThreads()->updateSeveralTimes();

}

void bimWorld::MovingCamera::translation(double startx, double starty, double endx, double endy)
{
	auto manip = static_cast<PersonManipulator*>(m_host->_CameraManipulator()->getCameraManipulator());
	if (!manip)
		return;

	manip->translation(startx, starty, endx, endy, m_host->_CameraManipulator()->getCamera());

	m_host->_RenderingThreads()->updateSeveralTimes();
}