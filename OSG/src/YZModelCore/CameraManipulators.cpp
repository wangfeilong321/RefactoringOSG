//
//  CameraManipulators.cpp
//  IModelCore
//  Created by Sqh on 2015-8-4.
//

#include "CameraManipulators.h"
#include "BIMCameraManipulator.h"
#include "PersonManipulator.h"
#include "BIMFirstPersonManipulator.h"
#include "YZModelCoreInternal.h"
#include "CameraAnimation.h"
#include "CameraOperation.h"
#include "CustomCameraManipulator.h"

bimWorld::CameraManipulator::CameraManipulator(YZModelCoreInternal* host) :YZModelCoreModule(host),
m_isFirstPersonManipEnabled(false)
{

}

void bimWorld::CameraManipulator::switchMatrixManipulator(ManipulatorType emanip)
{
	auto viewer = m_host->_ViewerData()->ModelViewer();
	auto sceneRoot = m_host->_ViewerData()->getSceneRoot();
	auto modelRoot = m_host->_ViewerData()->getModelRoot();
	if (!viewer || !sceneRoot || !modelRoot)
	{
		return;
	}
	m_host->_RenderingThreads()->setIsExternalRendering(true);
	auto manip = viewer->getCameraManipulator();
	osg::Vec3d eye, center, up;
	auto matrix = manip->getMatrix();
	manip->getHomePosition(eye, center, up);
	switch (emanip)
	{
	case ManipulatorType::Default:
		sceneRoot->getOrCreateStateSet()->setMode(GL_LIGHT0,
			osg::StateAttribute::ON);
		sceneRoot->getOrCreateStateSet()->setMode(GL_LIGHT1,
			osg::StateAttribute::ON);
		sceneRoot->getOrCreateStateSet()->setMode(GL_LIGHT2,
			osg::StateAttribute::OFF);
		sceneRoot->getOrCreateStateSet()->setMode(GL_LIGHT3,
			osg::StateAttribute::OFF);
		sceneRoot->getOrCreateStateSet()->setMode(GL_LIGHT4,
			osg::StateAttribute::OFF);
		((BIMCameraManipulator*)m_d.get())->setPosition(eye, center, up, matrix);

		////                BIMCameraManipulator* mat = (BIMCameraManipulator*)(m_d.get());
		//                 ((BIMCameraManipulator*)m_d.get())->beginSetCameraMatrix();
		//                ((BIMCameraM-anipulator*)m_d.get())->setByMatrix(matrix);
		//               ((BIMCameraManipulator*)m_d.get())->endSetCameraMatrix();
		viewer->setCameraManipulator(((BIMCameraManipulator*)m_d.get()), false);
		break;
	case ManipulatorType::Person:
	{
		auto oldManip = static_cast<BIMCameraManipulator*>(manip);
		if (!oldManip)
			break;
		auto rotation = oldManip->getRotation();
		auto dist = oldManip->getDistance();
		auto personManip = static_cast<PersonManipulator*>(m_p.get());
		if (!personManip)
			break;
		personManip->setScreenRotation(rotation);
		personManip->setScreenDistance(dist);
		personManip->setPosition(eye, center, up, matrix);
		viewer->setCameraManipulator(personManip, false);
		break;
	}
	case ManipulatorType::FirstPerson:
	{
		sceneRoot->getOrCreateStateSet()->setMode(GL_LIGHT0,
			osg::StateAttribute::OFF);
		sceneRoot->getOrCreateStateSet()->setMode(GL_LIGHT1,
			osg::StateAttribute::OFF);
		sceneRoot->getOrCreateStateSet()->setMode(GL_LIGHT2,
			osg::StateAttribute::ON);
		sceneRoot->getOrCreateStateSet()->setMode(GL_LIGHT3,
			osg::StateAttribute::ON);
		sceneRoot->getOrCreateStateSet()->setMode(GL_LIGHT4,
			osg::StateAttribute::ON);

		osgGA::CameraManipulator* first = new BIMFirstPersonManipulator(m_host);
		viewer->setCameraManipulator(first, true);

		first->setHomePosition(modelRoot->getBound().center(), osg::Vec3(0, 0, 0), up);
		viewer->home();
		break;
	}
	}
	m_host->_RenderingThreads()->setIsExternalRendering(false);

	return;

	//		osgGA::KeySwitchMatrixManipulator *switchmanipulator = dynamic_cast<osgGA::KeySwitchMatrixManipulator*>(m_mViewer->getCameraManipulator());
	//
	//		//m_ptrKeySwitchMatrixManipulator=new osgGA::KeySwitchMatrixManipulator;
	//		//m_ptrKeySwitchMatrixManipulator->addMatrixManipulator(1,"deflaut",createCameraManipulator());
	//		//m_ptrKeySwitchMatrixManipulator->addMatrixManipulator(2,"person",createPersonManipulator());
	//		switchmanipulator->selectMatrixManipulator((int)emanip);
	//		if (emanip == deflaut)
	//		{
	//			BIMCameraManipulator* manipulator = dynamic_cast<BIMCameraManipulator*>(switchmanipulator->getCurrentMatrixManipulator());
	//
	//			zoomTo(m_modelRoot);
	//			//if (manipulator)
	//			//{
	//			//	manipulator->setupFrameRateController(m_modelRoot.get());
	//			//	manipulator->setModelRoot(m_modelRoot);
	//			//}
	//		}
	//		else if (emanip == person)
	//		{
	//			PersonManipulator* manipulator = dynamic_cast<PersonManipulator*>(switchmanipulator->getCurrentMatrixManipulator());
	//			zoomTo(m_modelRoot);
	//			//if (manipulator)
	//			//{
	//			//	manipulator->setupFrameRateController(m_modelRoot.get());
	//			//	manipulator->setModelRoot(m_modelRoot);
	//			//}
	//		}

	m_host->_RenderingThreads()->updateSeveralTimes(1);
}

bimWorld::ViewerMode bimWorld::CameraManipulator::getCameraManipulatorMode()
{
	return m_cameraManipulatorMode;
}

float totalFrame_FP = 60.0f;

void bimWorld::CameraManipulator::enableFirstPersonControl()
{

	bindKeyDownEvent(KEY_W, beginMoveForward);
	bindKeyUpEvent(KEY_W, endMoveForward);

	bindKeyDownEvent(KEY_S, beginMoveBackward);
	bindKeyUpEvent(KEY_S, endMoveBackward);

	bindKeyDownEvent(KEY_A, beginMoveLeft);
	bindKeyUpEvent(KEY_A, endMoveLeft);

	bindKeyDownEvent(KEY_D, beginMoveRight);
	bindKeyUpEvent(KEY_D, endMoveRight);
	unbindMouseEvent(MIDDLE_MOUSE_BUTTON);

	return;
	//if (m_isFirstPersonManipEnabled)
	//	return;
	//switchMatrixManipulator(ManipulatorType::Person);
	//m_isFirstPersonManipEnabled = true;
	//getBIMCameraManip()->setRotationHandleBtn(osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON);
	//float totalFrame = 60.0f;
	bimWorld::CameraOperation operation(getBIMCameraManip());
	m_zoomForwardDelta = operation.getZoomDelta(totalFrame_FP);
	std::function<void(float)> prefunc = [this/*, totalFrame*/](float frame){
		//static int frame = 1;
		//static float newF = 1;//-1 / totalFrame * frame*frame + frame;
		bimWorld::CameraOperation operation(getBIMCameraManip());
		operation.zoomForward(m_zoomForwardDelta, frame);
		//if (newF < (totalFrame / 2.0f))
		//{
		//	frame++;
		//	newF += frame;
		//}
		//else
		//{
		//	frame--;
		//	newF -= frame;
		//}
	};
	core::InstanceFunction<void(float)> pre(prefunc, this, "onPreEnableFirstPersonControl");
	std::function<void(float)> postfunc = [](float){};
	core::InstanceFunction<void(float)> post(postfunc, this, "onPostEnableFirstPersonControl");
	m_host->_RenderingThreads()->setBlockAnimation((unsigned int)totalFrame_FP, pre, post);
}

void bimWorld::CameraManipulator::disableFirstPersonControl()
{
	unbindKeyDownEvent(KEY_W);
	unbindKeyUpEvent(KEY_W);

	unbindKeyDownEvent(KEY_S);
	unbindKeyUpEvent(KEY_S);

	unbindKeyDownEvent(KEY_A);
	unbindKeyUpEvent(KEY_A);

	unbindKeyDownEvent(KEY_D);
	unbindKeyUpEvent(KEY_D);
	bindMouseEvent(MIDDLE_MOUSE_BUTTON, onPanModel);
	setModelCenterKeepViewPoint(m_host->ViewerData()->getModelRoot());
	m_host->_RenderingThreads()->updateSeveralTimes();

	return;
	//if (!m_isFirstPersonManipEnabled)
	//{
	//	return;
	//}
	//switchMatrixManipulator(ManipulatorType::Default);
	//m_isFirstPersonManipEnabled = false;
	//float totalFrame = 60.0f;
	//getBIMCameraManip()->setRotationHandleBtn(osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON);
	std::function<void(float)> prefunc = [this/*, totalFrame*/](float frame){
		//static int frame = 1;
		//static float newF = 1;//-1 / totalFrame * frame*frame + frame;
		bimWorld::CameraOperation operation(getBIMCameraManip());
		operation.zoomBackward(m_zoomForwardDelta, frame);
		//if (newF < (totalFrame / 2.0f))
		//{
		//	frame++;
		//	newF += frame;
		//}
		//else
		//{
		//	frame--;
		//	newF -= frame;
		//}
	};
	core::InstanceFunction<void(float)> pre(prefunc, this, "onPreDisableFirstPersonControl");
	std::function<void(float)> postfunc = [](float /*frame*/){};
	core::InstanceFunction<void(float)> post(postfunc, this, "onPostDisableFirstPersonControl");
	m_host->_RenderingThreads()->setBlockAnimation((unsigned int)totalFrame_FP, pre, post);
}

void bimWorld::CameraManipulator::setViewerMode(ViewerMode viewerMode)
{
	BIMCameraManipulator *manipulator = dynamic_cast<BIMCameraManipulator*>(getCameraManipulator());
	if (manipulator != NULL)
	{
		manipulator->setMode(viewerMode);
		m_cameraManipulatorMode = viewerMode;
	}
	clearSelection();
	m_host->_RenderingThreads()->updateSeveralTimes(1);
}

std::shared_ptr<bimWorld::IZoomModel> bimWorld::CameraManipulator::ZoomModel()
{
	if (!m_zoomModel)
	{
		m_zoomModel = std::make_shared<bimWorld::ZoomModel>(m_host);
	}
	return m_zoomModel;
}

osg::Camera* bimWorld::CameraManipulator::getCamera()
{
	return m_host->_ViewerData()->ModelViewer()->getCamera();
}

void bimWorld::CameraManipulator::clearSelection()
{
	std::mutex _lock;
	_lock.lock();

	for (int i = 0; i < m_modelViewerListeners.size(); i++)
	{
		if (!m_modelViewerListeners[i])
			continue;
		m_modelViewerListeners[i]->clearSelection();
	}
	_lock.unlock();
}

osgGA::CameraManipulator* bimWorld::CameraManipulator::getCameraManipulator()
{
	if (!m_host)
		return NULL;
	return m_host->_ViewerData()->ModelViewer()->getCameraManipulator();
}

const bimWorld::ListenerSet& bimWorld::CameraManipulator::getListeners() const
{
	return m_modelViewerListeners;
}

std::shared_ptr<bimWorld::CameraAnimation> bimWorld::CameraManipulator::CameraAnimation()
{
	if (!m_cameraAnimation)
	{
		m_cameraAnimation = std::make_shared<bimWorld::CameraAnimation>(m_host);
	}
	return m_cameraAnimation;
}

void bimWorld::CameraManipulator::updateModelSize()
{
	auto manip = dynamic_cast<bimWorld::BIMCameraManipulator*>(getCameraManipulator());
	manip->updateModelSize();
}

void bimWorld::CameraManipulator::init()
{
	m_d = new BIMCameraManipulator(m_host);
	m_host->ViewerData()->ModelViewer()->setCameraManipulator(m_d);
	m_p = new PersonManipulator();
	//        m_host->ViewerData()->ModelViewer()->setCameraManipulator(createPersonManipulator());
	m_host->ViewerData()->ModelViewer()->setKeyEventSetsDone(0);
}

bimWorld::BIMCameraManipulator* bimWorld::CameraManipulator::getBIMCameraManip()
{
	auto manip = static_cast<bimWorld::BIMCameraManipulator*>(getCameraManipulator());
	return manip;
}

void bimWorld::CameraManipulator::setModelCenter(void* ptr)
{
	auto node = static_cast<osg::Node*>(ptr);
	if (!ptr)
	{
		return;
	}
	//getBIMCameraManip()->setRotationCenter(node->getBound().center());
	getBIMCameraManip()->setModelCenterKeepDist(node->getBound().center());
}

void bimWorld::CameraManipulator::setModelCenterKeepViewPoint(void* ptr)
{
	auto node = static_cast<osg::Node*>(ptr);
	if (!ptr)
	{
		return;
	}
	getBIMCameraManip()->setModelCenter(node->getBound().center());
}

void bimWorld::CameraManipulator::bindKeyUpEvent(bimWorld::KeySymbol key, bimWorld::CameraOperationTypes operation)
{
	getBIMCameraManip()->bindKeyUpEvent(key, operation);
}

void bimWorld::CameraManipulator::bindKeyDownEvent(bimWorld::KeySymbol key, bimWorld::CameraOperationTypes operation)
{
	getBIMCameraManip()->bindKeyDownEvent(key, operation);
}

void bimWorld::CameraManipulator::bindMouseEvent(bimWorld::MouseButtonMask mouse, bimWorld::CameraOperationTypes operation)
{
	getBIMCameraManip()->bindMouseEvent(mouse, operation);
}

void bimWorld::CameraManipulator::unbindKeyUpEvent(bimWorld::KeySymbol key)
{
	getBIMCameraManip()->unbindKeyUpEvent(key);
}

void bimWorld::CameraManipulator::unbindKeyDownEvent(bimWorld::KeySymbol key)
{
	getBIMCameraManip()->unbindKeyDownEvent(key);
}

void bimWorld::CameraManipulator::unbindMouseEvent(bimWorld::MouseButtonMask mouse)
{
	getBIMCameraManip()->unbindMouseEvent(mouse);
}
