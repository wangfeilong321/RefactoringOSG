#include "MaterialSetting.h"
#include <osg/Material>
#include "YZModelCoreInternal.h"


bimWorld::MaterialsSetting::MaterialsSetting(YZModelCoreInternal* host) :
YZModelCoreModule(host),
m_selectionRenderMode(NULL)
{
	m_materialModified = false;
	//std::function<void(std::chrono::system_clock::time_point)> func = [this](std::chrono::system_clock::time_point time){
	//	m_materialModified = false;
	//};
	//core::InstanceFunction<void(std::chrono::system_clock::time_point)> insfunc(func, this, "onMoveBackward");
	//m_host->_RenderingThreads()->PostFrameUpdateEvent() += insfunc;
}

bool bimWorld::MaterialsSetting::saveMaterialSettingToFile(const char *fileName)
{
	return false;
}

bool bimWorld::MaterialsSetting::readMaterialSettingFromFile(const char *fileName)
{

	return false;
}

osg::StateSet* bimWorld::MaterialsSetting::getSelectionRenderModel()
{

	if (m_selectionRenderMode == NULL) {
		double rgba[4] = { 1.0, 0.0, 0.0, 0.6 };

		m_selectionRenderMode = new osg::StateSet();
		osg::ref_ptr<osg::Material> selectionMat = new osg::Material;
		selectionMat->setDiffuse(osg::Material::FRONT_AND_BACK, osg::Vec4(rgba[0], rgba[1], rgba[2], rgba[3]));
		selectionMat->setShininess(osg::Material::FRONT_AND_BACK, 90);
		selectionMat->setAmbient(osg::Material::FRONT_AND_BACK, osg::Vec4(rgba[0], rgba[1], rgba[2], rgba[3]));
		selectionMat->setSpecular(osg::Material::FRONT_AND_BACK, osg::Vec4(rgba[0], rgba[1], rgba[2], rgba[3]));
		_lock.lock();
		m_selectionRenderMode->setAttribute(selectionMat, osg::StateAttribute::PROTECTED | osg::StateAttribute::OVERRIDE);
		//m_selectionRenderMode->setAttribute(new osg::Depth(osg::Depth::LESS, 0, 0, false));
		_lock.unlock();
	}
	return m_selectionRenderMode;
}

void bimWorld::MaterialsSetting::setIsModified(bool modified)
{
	m_materialModified = modified;
}

bool bimWorld::MaterialsSetting::isModified()
{
	return m_materialModified;
}
