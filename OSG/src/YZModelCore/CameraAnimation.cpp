#include "CameraAnimation.h"
#include "YZModelCoreInternal.h"

bimWorld::CameraAnimation::CameraAnimation(YZModelCoreInternal* host) :YZModelCoreModule(host)
{

}

void bimWorld::CameraAnimation::play(osg::Vec3d newposition, osg::Vec3d center, osg::Vec3d up)
{
	if (!m_cameraAnimator)
	{
		return;
	}
	m_cameraAnimator->setPath(createAnimationPath(newposition, center, up));
	m_cameraAnimator->addUpdateCallback([this](const bimWorld::CameraAnimator* /*ca*/) {
		m_host->_RenderingThreads()->updateSeveralTimes(2);
	});
	origin_draw = m_host->_RenderingThreads()->getNeedUpdateAndDraw();
	m_cameraAnimator->addCompleteCallback([this](){
		if (!origin_draw)
			m_host->_RenderingThreads()->setNeedUpdateAndDraw(false);
		m_host->_FixedFrameRate()->cancelFixedFramerate();
	});

	m_host->_RenderingThreads()->setNeedUpdateAndDraw(true);
	m_host->_FixedFrameRate()->startFixedFramerate();
	m_cameraAnimator->play();
}

std::vector<bimWorld::CameraVector> bimWorld::CameraAnimation::createAnimationPath(osg::Vec3d pos, osg::Vec3d center, osg::Vec3d up, float /*delta_time*/)
{
	osg::Vec3d vPosEye, vCenter, vUp;
	m_host->_ViewerData()->ModelViewer()->getCamera()->getViewMatrixAsLookAt(vPosEye, vCenter, vUp);
	osg::Vec3d frontvector = pos - vPosEye;
	double length = frontvector.length();
	//		double p_delta = length / delta_time;
	double p_delta = 50;
	double step = length / p_delta;
	frontvector.normalize();
	osg::Vec3d myposition(vPosEye);

	std::vector<CameraVector> result;
	for (int i = 0; i < step; i++)
	{
		CameraVector camera;
		camera.eye = myposition;
		camera.center = center;
		camera.up = up;
		result.push_back(camera);
		myposition = myposition + frontvector * p_delta;
	}
	return result;
}
