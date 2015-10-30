#include "ViewerDataModel.h"
#include "YZModelCoreInternal.h"
#include <osg/Depth>
#include <osgViewer/ViewerEventHandlers>
#include <osgGA/StateSetManipulator>

bimWorld::ViewerDataModel::ViewerDataModel(YZModelCoreInternal* host) :YZModelCoreModule(host)
{
	m_mViewer = new osgViewer::Viewer();
	m_modelRoot = NULL;
	m_sceneRoot = NULL;
	m_backGroundRoot = NULL;
	m_topRoot = NULL;
	m_windowSpace = NULL;
	m_bdone = false;
	m_isLoaded = false;
	m_mViewer = NULL;
	//_isComponentFunc = NULL;

	std::function<void(std::vector<void*>, bimWorld::ViewerMode)> func = std::bind(&bimWorld::ViewerDataModel::onSelectNode, this, std::placeholders::_1, std::placeholders::_2);
	core::InstanceFunction<void(std::vector<void*>, bimWorld::ViewerMode)> insfunc(func, this, "onSelectNode");
	m_selectNodeEvent += insfunc;
}

osgViewer::Viewer* bimWorld::ViewerDataModel::ModelViewer()
{
	if (m_bdone)
	{
		return NULL;
	}
	return m_mViewer.get();
}

void bimWorld::ViewerDataModel::onSelectNode(std::vector<void*> nodes, ViewerMode mode)
{
	m_selectedNodes.clear();

	if (nodes.empty())
	{
		return;
	}

	if (mode == Orbit)
	{
		m_selectedNodes.push_back(nodes[0]);
	}
	else
	{
		m_selectedNodes.resize(nodes.size());
		std::copy(nodes.begin(), nodes.end(), m_selectedNodes.begin());
	}
}

std::vector<void*> bimWorld::ViewerDataModel::getSelectedNodes()
{
	return m_selectedNodes;
}

void bimWorld::ViewerDataModel::setSelectedNodes(const std::vector<void*>& nodes)
{
	m_selectedNodes.clear();
	m_selectedNodes.resize(nodes.size());
	std::copy(nodes.begin(), nodes.end(), m_selectedNodes.begin());
}

osg::Group* bimWorld::ViewerDataModel::TopRoot()
{
	if (m_bdone)
	{
		return NULL;
	}
	return m_topRoot;
}

std::map<osg::ref_ptr<osg::Node>, const void(*)()>& bimWorld::ViewerDataModel::TopMostCallback()
{
	return m_topMostCallback;
}

std::map<int, osg::ref_ptr<osg::Node> >& bimWorld::ViewerDataModel::TopMostSet()
{
	return m_topMostSet;
}

bool bimWorld::ViewerDataModel::isTopMostNode(osg::Node* node)
{
	if (!node)
		return false;

	return m_topRoot->containsNode(node);
}

osg::Group* bimWorld::ViewerDataModel::getSceneRoot()
{
	if (m_bdone)
	{
		return NULL;
	}
	return m_sceneRoot;
}

void bimWorld::ViewerDataModel::setSceneRoot(osg::Group* root)
{
	m_sceneRoot = root;
}

bool bimWorld::ViewerDataModel::validateSceneRoot()
{
	if (m_sceneRoot)
	{
		return true;
	}
	else
	{
		return false;
	}
}


osg::MatrixTransform* bimWorld::ViewerDataModel::getModelRoot()
{
	if (m_bdone)
	{
		return NULL;
	}
	if (!m_modelRoot)
	{
		osg::ref_ptr<osg::MatrixTransform> newRoot = new osg::MatrixTransform();
		newRoot->setName("root");
		setModelRoot(newRoot);
		getSceneRoot()->addChild(newRoot);
	}
	return m_modelRoot;
}

void bimWorld::ViewerDataModel::setModelRoot(osg::MatrixTransform* transformNode)
{
	m_modelRoot = transformNode;
}

bool bimWorld::ViewerDataModel::validateModelRoot()
{
	if (m_modelRoot)
	{
		return true;
	}
	else
	{
		return false;
	}
	return m_modelRoot;
}


osg::Camera* bimWorld::ViewerDataModel::getWindowSpace()
{
	if (m_bdone)
	{
		return NULL;
	}
	return m_windowSpace;
}

//bimWorld::ListenerSet& bimWorld::ViewerDataModel::ModelViewerListeners()
//{
//
//}

bool bimWorld::ViewerDataModel::isDone()
{
	return m_bdone;
}

void bimWorld::ViewerDataModel::setDone(bool isDone)
{
	m_bdone = isDone;
}

OpenThreads::Mutex& bimWorld::ViewerDataModel::Mutex()
{
	return m_mutex;
}

bool bimWorld::ViewerDataModel::isLoaded()
{
	return m_isLoaded;
}

void bimWorld::ViewerDataModel::setLoaded(bool isLoaded)
{
	m_isLoaded = isLoaded;
}

bool bimWorld::ViewerDataModel::getCameraPerspective(bimWorld::CameraPerspective& perspective)
{
	m_mViewer->getCamera()->getProjectionMatrixAsPerspective(perspective.fov,
		perspective.aspectRatio, perspective.zNear, perspective.zFar);
	return true;
}

// <创建一个最顶层的相机>
void bimWorld::ViewerDataModel::createTopMostCamera(osg::Camera* mainCamera)
{
	// create a camera to set up the projection and model view matrices, and the subgraph to draw in the HUD
	osg::Camera* camera = new osg::Camera;

	// set clear color. Since we don't clear color buffer, this setting may be useless.
	auto defaultBG = m_host->_BackgroundSetting()->getDefaultBG();
	camera->setClearColor(defaultBG);
	/// we can't set perspective projection matrix.
	/// after set, we just can't see anything.
	//// set the projection matrix
	//double fovy, aspectRatio, zNear, zFar;
	//mainCamera->getProjectionMatrixAsPerspective(fovy, aspectRatio, zNear, zFar);
	//camera->setProjectionMatrixAsPerspective(fovy, aspectRatio, zNear, zFar);

	// set view port
	auto viewPort = mainCamera->getViewport();
	camera->setViewport(viewPort);

	// set the projection matrix
	camera->setProjectionMatrix(osg::Matrix::ortho2D(0, viewPort->width(), 0, viewPort->height()));

	/// we don't want to set "absolute" this time
	/// because we need to put the objects in the proper position.
	camera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);

	// set the view matrix
	camera->setViewMatrix(osg::Matrix::identity());

	// only clear the depth buffer
	camera->setClearMask(GL_DEPTH_BUFFER_BIT);

	// draw subgraph after main camera view.
	camera->setRenderOrder(osg::Camera::POST_RENDER);

	//// we don't want the camera to grab event focus from the viewers main camera(s).
	// we want the objects can be selected.
	camera->setAllowEventFocus(false);

	// add to scene.
	m_sceneRoot->addChild(camera);

	// store as a member.
	m_topRoot = camera;
}


// <创建一个最底层的相机>
void bimWorld::ViewerDataModel::createBackGroundCamera(osg::Camera* mainCamera)
{
	osg::ref_ptr<osg::Camera> camera = new osg::Camera;
	camera->setCullingActive(false);
	camera->setClearMask(0);
	//camera->setAllowEventFocus(false);
	camera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
	//camera->setClearMask(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	//camera->setClearMask(GL_DEPTH_BUFFER_BIT);
	camera->setRenderOrder(osg::Camera::POST_RENDER);
	// set view port
	auto viewPort = mainCamera->getViewport();
	camera->setViewport(viewPort);

	auto defaultBG = m_host->_BackgroundSetting()->getDefaultBG();
	camera->setClearColor(defaultBG);

	// set the projection matrix
	//camera->setProjectionMatrix(osg::Matrix::ortho2D(0, 1, 0, 1));
	//camera->setProjectionMatrix(osg::Matrix::ortho2D(0, viewPort->width(), 0, viewPort->height()));
	double aspectRatioScale = static_cast<double>(viewPort->width()) / static_cast<double>(viewPort->height());
	camera->setProjectionMatrixAsPerspective(30.0, aspectRatioScale, 1.0, 1000.0);

	osg::StateSet* ss = camera->getOrCreateStateSet();
	ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	//ss->setAttributeAndModes(new osg::Depth(
	//	osg::Depth::LEQUAL, 1.0, 1.0));
	// add to scene.
	//m_sceneRoot->addChild(camera);
	mainCamera->addChild(camera);
	// store as a member.
	m_backGroundRoot = camera;

}


osg::Node* createLightSource(unsigned int num,
	const osg::Vec3& trans,
	const osg::Vec4& color)
{
	osg::ref_ptr<osg::Light> light = new osg::Light;
	light->setLightNum(num);
	light->setDiffuse(color);
	light->setPosition(osg::Vec4(trans, 0.0f));
	osg::ref_ptr<osg::LightSource> lightSource = new
		osg::LightSource;
	lightSource->setLight(light);
	lightSource->setReferenceFrame(osg::LightSource::ABSOLUTE_RF);
	osg::ref_ptr<osg::MatrixTransform> sourceTrans =
		new osg::MatrixTransform;
	//sourceTrans->setMatrix(osg::Matrix::translate(trans));
	sourceTrans->addChild(lightSource.get());
	return sourceTrans.release();
}

#ifdef Android
void bimWorld::ViewerDataModel::initOSG(int x,int y,int width,int height)
{	
	osg::ref_ptr<osg::Group> sceneRoot = new osg::Group();

	m_sceneRoot = sceneRoot.get();

	m_mViewer = new osgViewer::Viewer();
	m_mViewer->setUpViewerAsEmbeddedInWindow(x, y, width, height);
	double aspectRatioScale = static_cast<double>(width) / static_cast<double>(height);
	// Init Master Camera for this View
	osg::ref_ptr<osg::Camera> camera = m_mViewer->getCamera();

	//// Assign Graphics Context to the Camera
	//camera->setGraphicsContext(gc);

	//// Set the view port for the Camera
	//camera->setViewport(new osg::Viewport(traits->x, traits->y, traits->width, traits->height));

	// Set projection matrix and camera attributes
	camera->setClearMask(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	camera->setProjectionMatrixAsPerspective(30.0, aspectRatioScale, 1.0, 1000.0);

	//camera->setCullMask(1);
	m_host->_RenderingThreads()->bindAbortValue(gc->getState());

	m_mViewer->setSceneData(sceneRoot);

	m_host->_CameraManipulator()->init();

	{
		// create a camera to set up the projection and model view matrices, and the subgraph to draw in the HUD
		osg::Camera* camera = new osg::Camera;

		// set the projection matrix
		camera->setProjectionMatrix(osg::Matrix::ortho2D(-1.0, 1.0, -1.0, 1.0));
		//camera->setProjectionMatrix(osg::Matrix::ortho2D(traits->x, traits->width, traits->y, traits->height));
		// set the view matrix
		camera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
		camera->setViewMatrix(osg::Matrix::identity());

		// only clear the depth buffer
		camera->setClearMask(GL_DEPTH_BUFFER_BIT);

		// draw subgraph after main camera view.
		camera->setRenderOrder(osg::Camera::POST_RENDER);

		// we don't want the camera to grab event focus from the viewers main camera(s).
		camera->setAllowEventFocus(false);
		m_sceneRoot->addChild(camera);
		m_windowSpace = camera;
	}
	m_host->CameraManipulator()->setViewerMode(Orbit);

	createTopMostCamera(camera);
	//createBackGroundCamera(camera);

	auto _state = m_sceneRoot->getOrCreateStateSet();
	_state->setMode(GL_LIGHTING, osg::StateAttribute::ON);
	_state->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
	_state->setMode(GL_CULL_FACE, osg::StateAttribute::ON);

	m_mViewer->addEventHandler(new osgViewer::StatsHandler);
	m_mViewer->addEventHandler(
		new osgGA::StateSetManipulator(
		m_mViewer->getCamera()->getOrCreateStateSet()));
	m_mViewer->addEventHandler(new osgViewer::ThreadingHandler);
	m_mViewer->setLightingMode(osgViewer::Viewer::LightingMode::SKY_LIGHT);

	m_mViewer->getViewerStats()->collectStats("scene", true);

	osg::Node* light0 = createLightSource(
		0, osg::Vec3(-1.0f, 1.0f, 1.0f), osg::Vec4(
		1.0f, 0.95f, 0.9f, 1.0f));

	osg::Node* light3 = createLightSource(
		1, osg::Vec3(1.0f, -1.0f, 0.0f), osg::Vec4(0.2, 0.2, 0.25f, 1.0f)
		);

	osg::Node* lightPerson0 = createLightSource(
		2, osg::Vec3(-1.0f, 1.0f, 1.0f), osg::Vec4(0.7f, 0.65f, 0.6f, 1.0f)
		);
	osg::Node* lightPerson1 = createLightSource(
		3, osg::Vec3(1.0f, 1.0f, 1.0f), osg::Vec4(0.5f, 0.45f, 0.4f, 1.0f)
		);
	osg::Node* lightPerson2 = createLightSource(
		4, osg::Vec3(0.0f, -1.0f, 0.0f), osg::Vec4(0.2, 0.2, 0.25f, 1.0f)
		);

	m_sceneRoot->getOrCreateStateSet()->setMode(GL_LIGHT0,
		osg::StateAttribute::ON);
	m_sceneRoot->getOrCreateStateSet()->setMode(GL_LIGHT1,
		osg::StateAttribute::ON);
	m_sceneRoot->getOrCreateStateSet()->setMode(GL_LIGHT2,
		osg::StateAttribute::OFF);
	m_sceneRoot->getOrCreateStateSet()->setMode(GL_LIGHT3,
		osg::StateAttribute::OFF);
	m_sceneRoot->getOrCreateStateSet()->setMode(GL_LIGHT4,
		osg::StateAttribute::OFF);
	m_sceneRoot->addChild(light0);

	m_sceneRoot->addChild(light3);

	m_sceneRoot->addChild(lightPerson0);
	m_sceneRoot->addChild(lightPerson1);
	m_sceneRoot->addChild(lightPerson2);

	m_host->Coordinate()->CoordinateHelper();

#pragma region TODO: Add Animation
	//TODO : to be implemented.
	//auto cameraManip = static_cast<bimWorld::BIMCameraManipulator*>(getCameraManipulator());
	//if (cameraManip)
	//m_animaEditor->setManipulatorContainer(cameraManip->getManipContainer());
#pragma endregion

	m_mViewer->realize();

	camera->setCullingMode(
		camera->getCullingMode() | osg::Camera::SMALL_FEATURE_CULLING);
	// default
	camera->setSmallFeatureCullingPixelSize(2.0);
	m_host->BackgroundSetting()->revertBackgroundColor();
}
#else
void bimWorld::ViewerDataModel::initOSG(std::function<void(osg::GraphicsContext::Traits* traits)> initGraphicContext)
{
	osg::ref_ptr<osg::Group> sceneRoot = new osg::Group();

	m_sceneRoot = sceneRoot.get();

	m_mViewer = new osgViewer::Viewer();
	//m_mainView = new osgViewer::View();
	//m_steps = 30;
	//m_zoomtimes = 0;

	//// Add a Stats Handler to the viewer
	//m_mViewer->addEventHandler(new osgViewer::StatsHandler);
	// Init the GraphicsContext Traits
	osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;

	// Setup the traits parameters
	traits->x = 0;
	traits->y = 0;
	traits->windowDecoration = false;
	traits->doubleBuffer = true;
	traits->sharedContext = 0;
	traits->setInheritedWindowPixelFormat = true;
	traits->samples = 4;
	//traits->sampleBuffers = 1;

	initGraphicContext(traits);
	// Create the Graphics Context
	osg::GraphicsContext* gc = osg::GraphicsContext::createGraphicsContext(traits.get());
	double aspectRatioScale = static_cast<double>(traits->width) / static_cast<double>(traits->height);
	// Init Master Camera for this View
	osg::ref_ptr<osg::Camera> camera = m_mViewer->getCamera();

	// Assign Graphics Context to the Camera
	camera->setGraphicsContext(gc);

	// Set the view port for the Camera
	camera->setViewport(new osg::Viewport(traits->x, traits->y, traits->width, traits->height));

	// Set projection matrix and camera attributes
	camera->setClearMask(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	camera->setProjectionMatrixAsPerspective(30.0, aspectRatioScale, 1.0, 1000.0);

	//camera->setCullMask(1);
	m_host->_RenderingThreads()->bindAbortValue(gc->getState());

	//gc->add(new MotionBlurOperation(0.25));
	//		m_ptrKeySwitchMatrixManipulator = new osgGA::KeySwitchMatrixManipulator;
	/*m_ptrKeySwitchMatrixManipulator->addMatrixManipulator(0, "default", createPersonManipulator());

	m_ptrKeySwitchMatrixManipulator->addMatrixManipulator(1, "person", createPersonManipulator());


	m_ptrKeySwitchMatrixManipulator->addMatrixManipulator(2, "Trackball", new osgGA::TrackballManipulator());
	//m_ptrKeySwitchMatrixManipulator->addMatrixManipulator(3, "Flight", new osgGA::FlightManipulator());
	m_ptrKeySwitchMatrixManipulator->addMatrixManipulator(4, "Drive", new osgGA::DriveManipulator());
	m_ptrKeySwitchMatrixManipulator->addMatrixManipulator(5, "Terrain", new osgGA::TerrainManipulator());
	m_ptrKeySwitchMatrixManipulator->addMatrixManipulator(6, "Orbit", new osgGA::OrbitManipulator());
	m_ptrKeySwitchMatrixManipulator->addMatrixManipulator(7, "FirstPerson", new osgGA::FirstPersonManipulator());
	m_ptrKeySwitchMatrixManipulator->addMatrixManipulator(8, "Spherical", new osgGA::SphericalManipulator());

	m_ptrKeySwitchMatrixManipulator->selectMatrixManipulator(1);*/

	//m_mViewer->setCameraManipulator(m_ptrKeySwitchMatrixManipulator);

	m_host->_CameraManipulator()->init();

	{
		// create a camera to set up the projection and model view matrices, and the subgraph to draw in the HUD
		osg::Camera* camera = new osg::Camera;

		// set the projection matrix
		camera->setProjectionMatrix(osg::Matrix::ortho2D(-1.0, 1.0, -1.0, 1.0));
		//camera->setProjectionMatrix(osg::Matrix::ortho2D(traits->x, traits->width, traits->y, traits->height));
		// set the view matrix
		camera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
		camera->setViewMatrix(osg::Matrix::identity());

		// only clear the depth buffer
		camera->setClearMask(GL_DEPTH_BUFFER_BIT);

		// draw subgraph after main camera view.
		camera->setRenderOrder(osg::Camera::POST_RENDER);

		// we don't want the camera to grab event focus from the viewers main camera(s).
		camera->setAllowEventFocus(false);
		sceneRoot->addChild(camera);
		m_windowSpace = camera;
	}
	m_host->CameraManipulator()->setViewerMode(Orbit);

	createTopMostCamera(camera);
	//createBackGroundCamera(camera);

	//auto mat = m_localViewMat * m_transMat * m_yawMat * m_pitchMat * m_scaleMat;
	m_scale = new osg::MatrixTransform();
	m_scale->setMatrix(m_scaleMat);
	m_scale->addChild(sceneRoot);
	m_localView = new osg::MatrixTransform();
	m_localView->setMatrix(m_localViewMat);
	m_localView->addChild(m_scale);

	//m_mViewer->setSceneData(sceneRoot);
	m_mViewer->setSceneData(m_localView);

	m_mViewer->setLightingMode(osgViewer::Viewer::LightingMode::SKY_LIGHT);

	/*
	osg::ref_ptr<osgShadow::SoftShadowMap > ssm = new osgShadow::SoftShadowMap;
	// ssm->setAmbientBias(osg::Vec2(0.7,0.7));//0.5,0.5
	float bias = ssm->getBias();
	ssm->setBias(bias*2.0);
	osg::ref_ptr<osgShadow::ShadowedScene> ss = new osgShadow::ShadowedScene();
	ss->setShadowTechnique(ssm);

	ss->setReceivesShadowTraversalMask(0x01);
	ss->setCastsShadowTraversalMask(0x02);
	//_buildings->setNodeMask(0x02);
	//_landNode->setNodeMask(0x01);
	//ss->addChild(_landNode);
	//ss->addChild(_buildings);
	m_sceneRoot->addChild(ss);
	*/

	// can't set color_material this way, maybe we should set it in material class.
	// after we load model or deal it in  convert model.
	//m_sceneRoot->getOrCreateStateSet()->setMode(GL_COLOR_MATERIAL, osg::StateAttribute::ON);

	//osg::ref_ptr<osg::LightSource> ls = new osg::LightSource();
	//osg::ref_ptr< osg::Light> lt = new osg::Light;
	//lt->setLightNum(0);
	//lt->setPosition(osg::Vec4(1.0, 1.0, 1.0, 0));
	//lt->setAmbient(osg::Vec4(0.3, 0.3, 0.3, 1.0));
	//lt->setDiffuse(osg::Vec4(0.7, 0.7, 0.7, 1.0));
	//ls->setLight(lt);
	//m_sceneRoot->addChild(ls);
	osg::Node* light0 = createLightSource(
		0, osg::Vec3(-1.0f, 1.0f, 1.0f), osg::Vec4(
		1.0f, 0.95f, 0.9f, 1.0f));
	//osg::Node* light1 = createLightSource(
	//	1, osg::Vec3(0.0f, 1.0f, 0.0f), osg::Vec4(
	//	0.4f, 0.35f, 0.3f, 1.0f));
	//osg::Node* light2 = createLightSource(
	//	2, osg::Vec3(0.0f, 0.0f, -1.0f), osg::Vec4(
	//	0.4f, 0.35f, 0.3f, 1.0f));

	osg::Node* light3 = createLightSource(
		1, osg::Vec3(1.0f, -1.0f, 0.0f), osg::Vec4(0.2, 0.2, 0.25f, 1.0f)
		);
	//osg::Node* light4 = createLightSource(
	//	4, osg::Vec3(0.0f, -1.0f, 0.0f), osg::Vec4(0, 0, 0.1f, 1.0f)
	//	);
	//osg::Node* light5 = createLightSource(
	//	5, osg::Vec3(0.0f, 0.0f, 1.0f), osg::Vec4(0, 0, 0.1f, 1.0f)
	//	);

	osg::Node* lightPerson0 = createLightSource(
		2, osg::Vec3(-1.0f, 1.0f, 1.0f), osg::Vec4(0.7f, 0.65f, 0.6f, 1.0f)
		);
	osg::Node* lightPerson1 = createLightSource(
		3, osg::Vec3(1.0f, 1.0f, 1.0f), osg::Vec4(0.5f, 0.45f, 0.4f, 1.0f)
		);
	osg::Node* lightPerson2 = createLightSource(
		4, osg::Vec3(0.0f, -1.0f, 0.0f), osg::Vec4(0.2, 0.2, 0.25f, 1.0f)
		);

	m_sceneRoot->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::ON);
	m_sceneRoot->getOrCreateStateSet()->setMode(GL_LIGHT0,
		osg::StateAttribute::ON);
	m_sceneRoot->getOrCreateStateSet()->setMode(GL_LIGHT1,
		osg::StateAttribute::ON);
	m_sceneRoot->getOrCreateStateSet()->setMode(GL_LIGHT2,
		osg::StateAttribute::OFF);
	m_sceneRoot->getOrCreateStateSet()->setMode(GL_LIGHT3,
		osg::StateAttribute::OFF);
	m_sceneRoot->getOrCreateStateSet()->setMode(GL_LIGHT4,
		osg::StateAttribute::OFF);
	//m_sceneRoot->getOrCreateStateSet()->setMode(GL_LIGHT5,
	//	osg::StateAttribute::ON);
	m_sceneRoot->addChild(light0);
	//m_sceneRoot->addChild(light1);
	//m_sceneRoot->addChild(light2);
	m_sceneRoot->addChild(light3);
	//m_sceneRoot->addChild(light4);
	//m_sceneRoot->addChild(light5);
	m_sceneRoot->addChild(lightPerson0);
	m_sceneRoot->addChild(lightPerson1);
	m_sceneRoot->addChild(lightPerson2);

	m_localViewMat = osg::Matrix::identity();

	m_transVec = osg::Vec3(0, 0, 0);
	m_transMat = osg::Matrix::translate(m_transVec);

	// Vertical axis rotation.
	m_yawAngle = 0;
	//auto m_yawQuat = osg::Quat(m_yawAngle, osg::Vec3d(0, 1, 0));
	m_yawMat = osg::Matrix::rotate(m_yawAngle, osg::Vec3d(0, 1, 0));
	// Lateral axis rotation.
	m_pitchAngle = 0;
	m_pitchMat = osg::Matrix::rotate(m_pitchAngle, osg::Vec3d(1, 0, 0));

	m_scaleVec = osg::Vec3(1, 1, 1);
	m_scaleMat = osg::Matrix::scale(m_scaleVec);

	// add sky box.
	//m_sceneRoot->addChild(m_skyBox);

	//m_coordHelper = new util::CoordinateHelper(m_mViewer->getCamera());
	m_host->Coordinate()->CoordinateHelper();

#pragma region TODO: Add Animation
	//TODO : to be implemented.
	//auto cameraManip = static_cast<bimWorld::BIMCameraManipulator*>(getCameraManipulator());
	//if (cameraManip)
	//m_animaEditor->setManipulatorContainer(cameraManip->getManipContainer());
#pragma endregion

	//m_mViewer->addView(m_mainView);
	m_mViewer->realize();

	//StoreRelationship().clearAll();
	camera->setCullingMode(
		camera->getCullingMode() | osg::Camera::SMALL_FEATURE_CULLING);
	// default
	camera->setSmallFeatureCullingPixelSize(2.0);
	m_host->BackgroundSetting()->revertBackgroundColor();
}

#endif // Android

core::Event<void(std::vector<void*>, bimWorld::ViewerMode)>& bimWorld::ViewerDataModel::SelectNodeEvent()
{
	return m_selectNodeEvent;
}

osg::Camera* bimWorld::ViewerDataModel::getBackGroundRoot()
{
	if (m_bdone)
	{
		return NULL;
	}
	return m_backGroundRoot;
}

osg::Camera* bimWorld::ViewerDataModel::getTopRoot()
{
	if (m_bdone)
	{
		return NULL;
	}
	return m_topRoot;
}

osg::Vec3 bimWorld::ViewerDataModel::getSelectedCenter()
{
	return m_selectedCenter;
}

void bimWorld::ViewerDataModel::setSelectedCenter(osg::Vec3 center)
{
	m_selectedCenter = center;
}

#ifdef Android

void bimWorld::ViewerDataModel::onMousePress(float x, float y, int button)
{
	m_mViewer->getEventQueue()->mouseButtonPress(x, y, button);
}

void bimWorld::ViewerDataModel::onMouseRelease(float x, float y, int button)
{
	m_mViewer->getEventQueue()->mouseButtonRelease(x, y, button);
}

void bimWorld::ViewerDataModel::onMouseMove(float x, float y)
{
	m_mViewer->getEventQueue()->mouseMotion(x, y);
}

void bimWorld::ViewerDataModel::onKeyPress(int key)
{
	m_mViewer->getEventQueue()->keyPress(key);
}

void bimWorld::ViewerDataModel::onKeyRelease(int key)
{
	m_mViewer->getEventQueue()->keyRelease(key);
}

#endif // Android

void bimWorld::ViewerDataModel::setLocalLookAt(const osg::Vec3& eye, const osg::Vec3& center /*= osg::Vec3(0, 0, 0)*/, const osg::Vec3& up /*= osg::Vec3(0, 1, 0)*/)
{
	m_localViewMat = osg::Matrix::identity();
	m_localViewMat.makeLookAt(eye, center, up);
	m_eye = eye;
	m_center = center;
	m_up = up;
	m_localView->setMatrix(m_localViewMat);
}
