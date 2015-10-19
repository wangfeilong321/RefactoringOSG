#include "CameraSettings.h"
#include "YZModelCoreInternal.h"
#include "BIMCameraManipulator.h"

bimWorld::CameraSettings::CameraSettings(YZModelCoreInternal* host) :YZModelCoreModule(host)
{

}

void copyData(double arr[3], double* ptr)
{
	for (int i = 0; i < 3; i++)
	{
		arr[i] = ptr[i];
	}
}

bool bimWorld::CameraSettings::getCameraView(CameraView& cameraView)
{
	auto cameraManipulator = m_host->_CameraManipulator()->getCameraManipulator();
	if (!cameraManipulator)
		return false;

	osg::Vec3d eye, center, up;
	cameraManipulator->getHomePosition(eye, center, up);
	copyData(cameraView.position, eye.ptr());
	copyData(cameraView.target, center.ptr());
	copyData(cameraView.upVector, up.ptr());

	auto matrix = cameraManipulator->getMatrix();
	for (int i = 0; i < 16; i++)
		cameraView.matrix[i] = matrix.ptr()[i];
	//m_mViewer->getCameraManipulator()->getHomePosition(*(osg::Vec3d*)&cameraView.position,
	//*(osg::Vec3d*)&cameraView.target,
	//*(osg::Vec3d*)&cameraView.upVector);
	return true;
}

bool bimWorld::CameraSettings::getCameraView(CameraView& cameraView, const char* strFileAllPath, unsigned char** buffer, int &width, int &height, int &channels)
{
	if (!m_host)
		return false;

	getCameraView(cameraView);
	return m_host->ScreenCapture()->CaptureScreen(buffer, width, height, channels, 0, 0, strFileAllPath);
}

bool bimWorld::CameraSettings::getCameraPerspective(CameraPerspective& cameraPerspective)
{
	m_host->_ViewerData()->ModelViewer()->getCamera()->getProjectionMatrixAsPerspective(cameraPerspective.fov,
		cameraPerspective.aspectRatio, cameraPerspective.zNear, cameraPerspective.zFar);
	return true;
}

bool bimWorld::CameraSettings::getCameraSetting(CameraSetting& cameraSetting)
{
	return getCameraView(cameraSetting.view) && getCameraPerspective(cameraSetting.perspective);
}

bool bimWorld::CameraSettings::setCameraView(const CameraView& cameraView)
{
	// <暂不支持其他相机操作器, 如后续需要，则重载相应setByMatrix方法。>
	auto cameraManipulator = m_host->_CameraManipulator()->getBIMCameraManip();
	if (!cameraManipulator)
		return false;

	cameraManipulator->setHomePosition(
		*(osg::Vec3d*)&cameraView.position,
		*(osg::Vec3d*)&cameraView.target,
		*(osg::Vec3d*)&cameraView.upVector);
	m_host->ViewerData()->ModelViewer()->home();
	cameraManipulator->beginSetCameraMatrix();
	cameraManipulator->setByMatrix(osg::Matrix(cameraView.matrix));
	cameraManipulator->endSetCameraMatrix();
	m_host->RenderingThreads()->updateSeveralTimes(1);
	return true;
}

bool bimWorld::CameraSettings::setCameraPerspective(const CameraPerspective& cameraPerspective)
{
	m_host->ViewerData()->ModelViewer()->getCamera()->setProjectionMatrixAsPerspective(cameraPerspective.fov,
		cameraPerspective.aspectRatio, cameraPerspective.zNear, cameraPerspective.zFar);

	m_host->RenderingThreads()->updateSeveralTimes(1);
	return true;
}

bool bimWorld::CameraSettings::setCameraSetting(const CameraSetting& cameraSetting)
{
	return setCameraView(cameraSetting.view) && setCameraPerspective(cameraSetting.perspective);
}

//void* bimWorld::CameraSettings::createDefaultTraits()
//{
//
//	return NULL;
//}
//
//void bimWorld::CameraSettings::addSlaveCamera(void* traits)
//{
//
//}
