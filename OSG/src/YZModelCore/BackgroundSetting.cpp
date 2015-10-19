#include "BackgroundSetting.h"
#include "SkyBoxLoader.h"
#include <osg/Vec4f>
#include "YZModelCoreInternal.h"
#include "SkyBox.h"

using namespace bimWorld;
const osg::Vec4f defaultBG = osg::Vec4f(0.18f, 0.3f, 0.47f, 1.0f);


bimWorld::BackgroundSetting::BackgroundSetting(YZModelCoreInternal* host) :YZModelCoreModule(host)
{
	m_skyBox = NULL;
}

bool bimWorld::BackgroundSetting::getBackgroundColor(int& red, int& green, int& blue, int& alpha)
{
	auto viewer = m_host->_ViewerData()->ModelViewer();
	if (!viewer)
	{
		return false;
	}

	auto camera = viewer->getCamera();

	if (!camera)
	{
		return false;
	}

	auto backColor = camera->getClearColor();

	red = (int)(255 * backColor.r());
	green = (int)(255 * backColor.g());
	blue = (int)(255 * backColor.b());
	alpha = (int)(255 * backColor.a());

	return true;
}

void bimWorld::BackgroundSetting::setBackgroundColor(int red, int green, int blue, int alpha)
{
	auto viewer = m_host->_ViewerData()->ModelViewer();
	if (!viewer)
	{
		return;
	}

	auto camera = viewer->getCamera();
	if (!camera)
	{
		return;
	}
	//camera->setComputeNearFarMode(osg::CullSettings::COMPUTE_NEAR_FAR_USING_BOUNDING_VOLUMES);
	osg::Vec4 newColor(red/255.0, green/255.0, blue/255.0, alpha/255.0);
	camera->setClearColor(newColor);
	//m_host->_ViewerData()->getTopRoot()->setClearColor(newColor);
	//m_host->_ViewerData()->getBackGroundRoot()->setClearColor(newColor);
	if (m_skyBox)
	{
		m_host->_ViewerData()->getSceneRoot()->removeChild(m_skyBox);
		m_skyBox = NULL;
	}
	m_host->_RenderingThreads()->updateSeveralTimes();
}

void bimWorld::BackgroundSetting::revertBackgroundColor()
{
	auto viewer = m_host->_ViewerData()->ModelViewer();
	if (!viewer)
	{
		return;
	}

	auto camera = viewer->getCamera();
	if (!camera)
	{
		return;
	}
	camera->setClearColor(defaultBG);
	//m_host->_ViewerData()->getTopRoot()->setClearColor(defaultBG);
	//m_host->_ViewerData()->getBackGroundRoot()->setClearColor(defaultBG);
	if (m_skyBox)
	{
		m_host->_ViewerData()->getSceneRoot()->removeChild(m_skyBox);
		m_skyBox = NULL;
	}
	m_host->_RenderingThreads()->updateSeveralTimes();
}

void bimWorld::BackgroundSetting::setSkyBox(const std::string& hdr)
{
	auto viewer = m_host->_ViewerData()->ModelViewer();
	if (!viewer)
	{
		return;
	}
	m_host->_ViewerData()->getSceneRoot()->removeChild(m_skyBox);

	////m_mViewer->getCamera()->setComputeNearFarMode(osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR);
	//viewer->getCamera()->setNearFarRatio(0.0001);
	//SkyBoxLoader loader(hdr);
	//m_skyBox = loader.createSkyBox();
	////m_mViewer->getCamera()->setNearFarRatio(0.0001);
	osg::ref_ptr<SkyBox> skybox = new SkyBox();
	skybox->setHDR(hdr, m_host->ViewerData()->getModelRoot()->getBound().radius());
	m_skyBox = skybox;
	m_host->_ViewerData()->getSceneRoot()->addChild(m_skyBox);
	m_host->_RenderingThreads()->updateSeveralTimes();
}

void bimWorld::BackgroundSetting::setSkyBox(const std::string& right, const std::string& left, const std::string& front, const std::string& back, const std::string& up, const std::string& down)
{
	auto viewer = m_host->_ViewerData()->ModelViewer();
	if (!viewer)
	{
		return;
	}
	//m_mViewer->getCamera()->setComputeNearFarMode(osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR);
	//viewer->getCamera()->setNearFarRatio(0.0001);
	//SkyBoxLoader loader(right, left, front, back, up, down);
	osg::ref_ptr<SkyBox> skybox = new SkyBox();
	m_host->_ViewerData()->getSceneRoot()->removeChild(m_skyBox);
	skybox->setCubeMap(right, left, front, back, up, down, m_host->ViewerData()->getModelRoot()->getBound().radius());
	m_skyBox = skybox;
	//m_skyBox = loader.createSkyBox();
	m_host->_ViewerData()->getSceneRoot()->addChild(m_skyBox);
	//m_mViewer->getCamera()->setNearFarRatio(0.0001);
	m_host->_RenderingThreads()->updateSeveralTimes();
}

const osg::Vec4f bimWorld::BackgroundSetting::getDefaultBG()
{
	return defaultBG;
}
