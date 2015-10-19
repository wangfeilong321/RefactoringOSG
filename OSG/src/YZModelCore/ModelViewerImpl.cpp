#include "StoreRelationship.h"
#include "CameraManipulators.h"
#include "ModelViewerImpl.h"
//#include "Axis.h"
#include <osg/Group>
#include <osg/Geode>
//#include <osg/Axis.h>
#include <osg/Drawable>
#include <osg/Geometry>
#include <osg/PolygonMode>
#include <osg/MatrixTransform>
#include <osgShadow/SoftShadowMap>
#include <osgShadow/ShadowedScene>
#include <osg/Material>
#include <osg/LineWidth>
#include <osgUtil/SmoothingVisitor>
#include <osg/ValueObject>
#include <osg/Depth>
#include <osg/ComputeBoundsVisitor>
#include <osgGA/FlightManipulator>
#include <osgGA/DriveManipulator>
#include <osgGA/TerrainManipulator>
#include <osgGA/OrbitManipulator>
#include <osgGA/FirstPersonManipulator>
#include <osgGA/SphericalManipulator>
#include <osgGA/DriveManipulator>
#include <cassert>
#include <boost/utility.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/categories.hpp> 
#include <boost/iostreams/code_converter.hpp>
#include <boost/locale.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include <boost/thread.hpp>
#include <iostream>
#include <algorithm>
#include "RenderingThread.h"
#include "GetIdVisitor.h"
#include "ScreenCaptureHandler.h"
#include "PersonManipulator.h"
#include <math.h>
#include "BillboardFactory.h"
#include "ReadPng.h"
#include <fstream>
#include "StringExt.h"
#include <osg/LineSegment>
#include <osgUtil/LineSegmentIntersector>
#include "VectorExt.h"
#include "GlobalStringDefine.h"
#include <functional>
#include "SkyBoxLoader.h"
#include "IntPtr.h"

#ifdef YIZHU_BIMWORLD_USE_WCHAR
#include <tchar.h>
#endif

#define BILLBOARD_DATA "billboard_data"
#define EXTERNAL_DATA "external_data"
#include<iostream>
#include<memory>

static long long count_size = 0;
namespace bimWorld
{
	const osg::Vec4f defaultBG = osg::Vec4f(0.18f, 0.3f, 0.47f, 1.0f);
#ifdef WIN32
	template<typename Elem, typename Tr = std::char_traits< Elem > >
	class progress_streambuf :public std::basic_filebuf < Elem, Tr >
	{
	public:
		typedef std::basic_filebuf<Elem, Tr> base_type;
		explicit progress_streambuf(const std::string &filename)
			:base_type(), size_(0)
		{
			if (open(filename.c_str(), std::ios_base::in | std::ios_base::binary))
			{
				size_ += static_cast<long long>(pubseekoff(0, std::ios_base::end, std::ios_base::in));
				pubseekoff(0, std::ios_base::beg, std::ios_base::in);
			}

		}
	protected:
		virtual int_type uflow()
		{
			int_type v = base_type::uflow();
			count_size += (long long)(egptr() - gptr());
			return v;
		}
	private:
		long long size_;
	};
	typedef progress_streambuf<char> probuf;
#endif
	class InfoVisitor : public osg::NodeVisitor
	{
	public:
		InfoVisitor(osg::Node **nodes, int length, osg::StateSet* selectionStateSet, bool isHighlight, std::function<void(void*)> unTransparentFunc)
			:osg::NodeVisitor(TRAVERSE_ALL_CHILDREN), m_length(length), m_selectionState(selectionStateSet), m_switch(isHighlight), m_unTransparentFunc(unTransparentFunc)
		{
			if (nodes)
			{
				m_pNodes = new osg::Node*[m_length];
				for (int i = 0; i < m_length; i++)
				{
					m_pNodes[i] = nodes[i];
				}
			}
			else
			{
				m_pNodes = NULL;
				m_length = 0;
			}
		}

		~InfoVisitor()
		{
			if (m_pNodes)
			{
				delete m_pNodes;
				m_pNodes = NULL;
			}
		}

		virtual void apply(osg::Node& node)
		{

			if (!m_switch)
			{
				m_unTransparentFunc(&node);
			}
			else
			{
				if (m_pNodes)
				{
					for (int i = 0; i < m_length; i++)
					{
						if (!m_pNodes[i])
						{
							continue;
						}
						if (m_pNodes[i]->getName() == node.getName())
						{
							node.setStateSet(NULL);
							node.setStateSet(m_selectionState);
							traverse(node);
							return;
						}
					}
				}

				setTransparency(&node);

			}
			traverse(node);
		}

		static void setTransparency(osg::Node* node, float opacity = 0.8)
		{
			osg::ref_ptr<osg::Material> otherMat;
			osg::ref_ptr<osg::BlendFunc> blendFunc;

			otherMat = new osg::Material;
			otherMat->setTransparency(osg::Material::FRONT_AND_BACK, opacity);
			blendFunc = new osg::BlendFunc();

			blendFunc->setSource(osg::BlendFunc::SRC_ALPHA);
			blendFunc->setDestination(osg::BlendFunc::ONE_MINUS_SRC_ALPHA);

			osg::ref_ptr<osg::StateSet> state = new osg::StateSet();

			if (state != NULL&&otherMat != NULL)
			{

				state->setAttribute(otherMat, osg::StateAttribute::PROTECTED);
				state->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);

				state->setMode(GL_BLEND, osg::StateAttribute::ON);

				state->setAttributeAndModes(blendFunc);
				state->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
				node->setStateSet(state);
			}
		}

	private:
		size_t m_length;
		osg::Node** m_pNodes;
		bool m_switch;
		osg::ref_ptr<osg::StateSet> m_selectionState;
		std::function<void(osg::Node*)> m_unTransparentFunc;
	};

	//	class HiddenVisitor : public osg::NodeVisitor
	//	{
	//	public:
	//		HiddenVisitor(osg::Node *node, bool isHide, const std::vector<osg::Node*>& exceptNodes)
	//			:osg::NodeVisitor(TRAVERSE_ALL_CHILDREN), m_pNode(node), m_isHide(isHide), m_exceptNodes(exceptNodes)
	//		{
	//		}
	//		virtual void apply(osg::Node& node)
	//		{
	//			if (!m_isHide)
	//			{
	//				if (!util::VectorExtension<osg::Node*>::contains(m_exceptNodes, &node) || m_pNode != &node)
	//				{
	//					node->setUserValue(NODE_VISIBILITY, true);
	//					node.setNodeMask(-1);
	//				}
	//			}
	//			else
	//			{
	//				if (!util::VectorExtension<osg::Node*>::contains(m_exceptNodes, &node) && m_pNode != &node)
	//				{
	//					node->setUserValue(NODE_VISIBILITY, false);
	//					node.setNodeMask(0);
	//				}
	//			}
	//			traverse(node);
	//		}
	//	private:
	//		osg::Node* m_pNode;
	//		bool m_isHide;
	//		std::vector<osg::Node*> m_exceptNodes;
	//	};


#ifdef SUPPORT_CPP11

	// @param callBack: return false to stop traverse this node.
	void ModelViewerImpl::traverseAllSubNodes(osg::Node* node, std::function<bool(osg::Node*)> callBack, bool includeItsSelf)
	{
		OpenThreads::ScopedLock<OpenThreads::Mutex> lock(m_mutex);
		if (!node)
			return;

		if (includeItsSelf)
		{
			if (!callBack(node))
				return;
		}

		auto group = node->asGroup();
		if (!group)
			return;
		unsigned int num = group->getNumChildren();
		if (node->asGeode())
			return;

		for (int i = 0; i < num; i++)
		{
			osg::Node* n = NULL;
			n = group->getChild(i);

			if (!n)
				continue;
			traverseAllSubNodes(n, callBack, true);
		}
	}

	bool ModelViewerImpl::traverseSubNodesAndFindFirst(osg::Node* node, std::function<bool(osg::Node*)> isGotCallBack, bool includeItsSelf)
	{
		OpenThreads::ScopedLock<OpenThreads::Mutex> lock(m_mutex);
		if (!node)
			return false;

		if (includeItsSelf)
		{
			if (isGotCallBack(node))
				return true;
		}

		auto group = node->asGroup();
		if (!group)
			return false;
		unsigned int num = group->getNumChildren();
		if (node->asGeode())
			return false;

		for (int i = 0; i < num; i++)
		{
			osg::Node* n = NULL;
			n = group->getChild(i);

			if (!n)
				continue;
			if (isGotCallBack(node))
				return true;
		}

		for (int i = 0; i < num; i++)
		{
			auto n = group->getChild(i);
			if (!n)
			{
				continue;
			}

			if (traverseSubNodesAndFindFirst(n, isGotCallBack, false))
				return true;
		}
		return false;
	}
#endif // SUPPORT_CPP11


	/* // <创建坐标轴>
   osg::Node* createAxis()
   {
   osg::Geode *axes = new osg::Geode();
   osg::Vec3d origin(0.0, 0.0, 0.0);
   Axis *axis = new Axis(Axis::XAXIS, origin);
   osg::StateSet *axisStates = axis->getOrCreateStateSet();
   osg::LineWidth *lineWidth = new osg::LineWidth();
   lineWidth->setWidth(3.0);
   axisStates->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
   axisStates->setAttribute(lineWidth, osg::StateAttribute::OVERRIDE);
   axis->setDataVariance(osg::Object::DYNAMIC);
   axes->addDrawable(axis);

   axis = new Axis(Axis::YAXIS, origin);
   axisStates = axis->getOrCreateStateSet();
   axisStates->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
   axisStates->setAttribute(lineWidth, osg::StateAttribute::OVERRIDE);
   axis->setDataVariance(osg::Object::DYNAMIC);
   axes->addDrawable(axis);

   axis = new Axis(Axis::ZAXIS, origin);
   axisStates = axis->getOrCreateStateSet();
   axisStates->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
   axisStates->setAttribute(lineWidth, osg::StateAttribute::OVERRIDE);
   axis->setDataVariance(osg::Object::DYNAMIC);
   axes->addDrawable(axis);
   axes->setUserValue(NODE_VISIBILITY, true);
   axes->setNodeMask(2);
   axes->setName("axes");
   return axes;
   }
   */


	void ModelViewerImpl::initCamera(osg::Camera* camera)
	{

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
		// Assign Graphics Context to the Camera
		camera->setGraphicsContext(gc);

		// Set the view port for the Camera
		camera->setViewport(new osg::Viewport(traits->x, traits->y, traits->width, traits->height));

		// Set projection matrix and camera attributes
		camera->setClearMask(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		camera->setClearColor(osg::Vec4f(0.18f, 0.3f, 0.47f, 1.0f));
		camera->setProjectionMatrixAsPerspective(30.0f, aspectRatioScale, 1.0, 1000.0);
		//camera->setCullMask(1);

	}

	// <创建一个最顶层的相机>
	long long  ModelViewerImpl::getCurrentProgressInfo()
	{
		return count_size;

	}

	void ModelViewerImpl::createTopMostCamera(osg::Camera* mainCamera)
	{
		// create a camera to set up the projection and model view matrices, and the subgraph to draw in the HUD
		osg::Camera* camera = new osg::Camera;

		// set clear color. Since we don't clear color buffer, this setting may be useless.
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

	void ModelViewerImpl::initOSG()
	{
		if (m_sceneRoot == NULL)
			m_sceneRoot = new osg::Group();

		m_mViewer = new osgViewer::Viewer();
		//m_mainView = new osgViewer::View();
		m_steps = 30;
		m_zoomtimes = 0;

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
		camera->setClearColor(defaultBG);
		camera->setProjectionMatrixAsPerspective(30.0f, aspectRatioScale, 1.0f, 1000.0f);

		//camera->setCullMask(1);
		gc->getState()->setAbortRenderingPtr(&m_babortRendering);

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

		//
		m_d = createCameraManipulator();
		m_mViewer->setCameraManipulator(m_d);
		m_p = createPersonManipulator();
		//        m_mViewer->setCameraManipulator(createPersonManipulator());
		m_mViewer->setKeyEventSetsDone(0);

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
		setViewerMode(Orbit);

		createTopMostCamera(camera);

		m_mViewer->setSceneData(m_sceneRoot);

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

		// <启用颜色材质>
		m_sceneRoot->getOrCreateStateSet()->setMode(GL_COLOR_MATERIAL, osg::StateAttribute::ON);

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

		// add sky box.
		m_sceneRoot->addChild(m_skyBox);

		m_coordHelper = new util::CoordinateHelper(m_mViewer->getCamera());
		auto cameraManip = static_cast<bimWorld::BIMCameraManipulator*>(getCameraManipulator());
		if (cameraManip)
			m_animaEditor->setManipulatorContainer(cameraManip->getManipContainer());
		//m_mViewer->addView(m_mainView);
		m_mViewer->realize();

		//StoreRelationship().clearAll();
	}

	osg::GraphicsContext::Traits* ModelViewerImpl::createDefaultTraits()
	{
		osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits();

		// Setup the traits parameters
		traits->x = 0;
		traits->y = 0;
		traits->windowDecoration = false;
		traits->doubleBuffer = true;
		traits->sharedContext = 0;
		traits->setInheritedWindowPixelFormat = true;
		traits->samples = 4;
		traits->windowDecoration = true;
		return traits.release();
	}

	void ModelViewerImpl::addSlaveCamera(osg::GraphicsContext::Traits* traits)
	{
		osg::ref_ptr<osg::GraphicsContext> gc = osg::GraphicsContext::createGraphicsContext(traits);

		osg::ref_ptr<osgViewer::View> view = new osgViewer::View();
		//osg::ref_ptr<osg::Camera> camera = new osg::Camera();
		auto camera = view->getCamera();
		camera->setGraphicsContext(gc.get());
		camera->setViewport(new osg::Viewport(traits->x, traits->y, traits->width, traits->height));
		double aspectRatioScale = static_cast<double>(traits->width) / static_cast<double>(traits->height);
		// Set projection matrix and camera attributes
		camera->setClearMask(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		camera->setClearColor(osg::Vec4f(0.18f, 0.3f, 0.47f, 1.0f));
		//camera->setProjectionMatrixAsPerspective(30.0f, aspectRatioScale, 1, 1000.0);
		GLenum buffer = traits->doubleBuffer ? GL_BACK : GL_FRONT;
		camera->setDrawBuffer(buffer);
		camera->setReadBuffer(buffer);
		camera->setAllowEventFocus(false);
		//camera->setProjectionMatrixAsPerspective(30.0f, aspectRatioScale, 1, 1000.0);
		//m_mainView->addSlave(camera);
		auto mainCamera = m_mViewer->getCamera();
		camera->setViewMatrix(mainCamera->getViewMatrix());
		camera->setProjectionMatrix(mainCamera->getProjectionMatrix());
		view->setSceneData(m_sceneRoot.get());
		//m_mViewer->addView(view.get());
	}

	void ModelViewerImpl::setSkyBox(const std::string& right, const std::string& left, const std::string& front, const std::string& back, const std::string& up, const std::string& down)
	{
		//m_mViewer->getCamera()->setComputeNearFarMode(osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR);
		m_mViewer->getCamera()->setNearFarRatio(0.0001);
		SkyBoxLoader loader(right, left, front, back, up, down);
		m_sceneRoot->removeChild(m_skyBox);
		m_skyBox = loader.createSkyBox();
		m_sceneRoot->addChild(m_skyBox);
		//m_mViewer->getCamera()->setNearFarRatio(0.0001);
		updateSeveralTimes();
	}

	void ModelViewerImpl::setSkyBox(const std::string& hdr)
	{
		//m_mViewer->getCamera()->setComputeNearFarMode(osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR);
		m_mViewer->getCamera()->setNearFarRatio(0.0001);
		SkyBoxLoader loader(hdr);
		m_sceneRoot->removeChild(m_skyBox);
		m_skyBox = loader.createSkyBox();
		m_sceneRoot->addChild(m_skyBox);
		//m_mViewer->getCamera()->setNearFarRatio(0.0001);
		updateSeveralTimes();
	}

	bool ModelViewerImpl::getBackgroundColor(osg::Vec4f& backColor)
	{
		if (!m_mViewer)
		{
			return false;
		}

		auto camera = m_mViewer->getCamera();

		if (!camera)
		{
			return false;
		}

		backColor = camera->getClearColor();

		return true;
	}

	void ModelViewerImpl::setBackgroundColor(const osg::Vec4f& newColor)
	{
		if (!m_mViewer)
		{
			return;
		}

		auto camera = m_mViewer->getCamera();
		if (!camera)
		{
			return;
		}
		camera->setComputeNearFarMode(osg::CullSettings::COMPUTE_NEAR_FAR_USING_BOUNDING_VOLUMES);
		camera->setClearColor(newColor);
		m_sceneRoot->removeChild(m_skyBox);
		m_skyBox = NULL;
		updateSeveralTimes();
	}

	void ModelViewerImpl::revertBackgroundColor()
	{
		if (!m_mViewer)
		{
			return;
		}

		auto camera = m_mViewer->getCamera();
		if (!camera)
		{
			return;
		}
		camera->setClearColor(defaultBG);
		m_sceneRoot->removeChild(m_skyBox);
		m_skyBox = NULL;
		updateSeveralTimes();
	}

	void ModelViewerImpl::setSteps(int Steps)
	{
		m_steps = Steps;
	}

#pragma region ---- Render When Needed Events ----

	void ModelViewerImpl::setNeedUpdateAndDraw(bool value, int delay)
	{
		if (m_rendering)
			m_rendering->setNeedUpdateAndDraw(value, delay);
	}

	bool ModelViewerImpl::getNeedUpdateAndDraw()
	{
		if (m_rendering)
			return m_rendering->getNeedUpdateAndDraw();
		return false;
	}

	void ModelViewerImpl::updateSeveralTimes(int times)
	{
		if (m_rendering)
			m_rendering->updateSeveralTimes(times);
	}

	void ModelViewerImpl::setIsExternalRendering(bool value)
	{
		if (m_rendering)
			m_rendering->setIsExternalRendering(value);
	}

	void ModelViewerImpl::onlyUpdateFrame()
	{
		if (!m_mViewer)
			return;
		m_mViewer->advance();
		m_mViewer->eventTraversal();
		m_mViewer->updateTraversal();
		m_animaEditor->update();
	}

#pragma endregion

	void copyData(double arr[3], double* ptr)
	{
		for (int i = 0; i < 3; i++)
		{
			arr[i] = ptr[i];
		}
	}

	std::vector<bimWorld::CameraVector> ModelViewerImpl::createAnimationPath(osg::Vec3d pos, osg::Vec3d center, osg::Vec3d up, float delta_time)
	{
		osg::Vec3d vPosEye, vCenter, vUp;
		m_mViewer->getCamera()->getViewMatrixAsLookAt(vPosEye, vCenter, vUp);
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

	void ModelViewerImpl::updateCameraAnimator(const CameraAnimator* ca)
	{
		updateSeveralTimes(2);
	}

	void ModelViewerImpl::completeCameraAnimator()
	{

		if (!origin_draw)
			setNeedUpdateAndDraw(false);
		cancelFixedFramerate();
	}

	bool ModelViewerImpl::zoomTo(void* node, bool withoutAnimation, ViewDirection direction)
	{
#pragma region Calculate Zoom Parameter

		/*osg::Node *n = static_cast<osg::Node*>(node);
		if (!n)
		{
		osg::notify(osg::WARN) << "The root node is null." << std::endl << "In ModelViewerImpl::zoomTo." << std::endl;
		return;
		}
		n = (osg::Node*)node;
		osg::BoundingSphere bound = n->getBound();
		osg::Vec3d center = bound.center();
		osg::Vec3d position(center.x(), center.y() - 2.8 * bound.radius(), center.z());
		do
		{
		osg::MatrixTransform *mt = ((n == node) ? NULL : dynamic_cast<osg::MatrixTransform*>(n));
		if (mt != NULL) {
		center = mt->getMatrix().preMult(center);
		position = mt->getMatrix().preMult(position);
		}
		} while (n->getNumParents() == 1 && (n = n->getParent(0)));
		AnimationPath(center,position);*/
		//m_mViewer->getCameraManipulator()->setHomePosition(position, center, osg::Vec3d(0, 0, 1), false);
		//m_mViewer->home();



#ifndef USING_BOUNDING_SPHERE
#define USING_BOUNDING_SPHERE
#endif
		osg::Node *n = (osg::Node*)node;
		osg::Node *newNode = n;
		if (!n)
		{
			osg::notify(osg::WARN) << "The root node is null." << std::endl << "In ModelViewerImpl::zoomTo." << std::endl;
			return false;
		}

		CameraPerspective pers;
		bool got = getCameraPerspective(pers);
		if (!got)
		{
			osg::notify(osg::FATAL) << "Get camera perspective failed, in zoomTo at ModelViewerImpl." << std::endl;
			return false;
		}

#if defined(USING_BOUNDING_SPHERE) // defined(OSG_GLES1_AVAILABLE) || defined(OSG_GLES2_AVAILABLE)

		auto sphere = n->getBound();
		auto radius = sphere.radius();
		auto center = sphere.center();
#else
		osg::ComputeBoundsVisitor* v = new osg::ComputeBoundsVisitor();
		v->apply(*n);
		auto box = v->getBoundingBox();
		auto xLength = box.xMax() - box.xMin();
		auto yLength = box.yMax() - box.yMin();
		float max;
		if (yLength >= xLength)
		{
			max = yLength;
		}
		else
		{
			max = xLength / pers.aspectRatio;
		}
		auto radius = max / 2.0;
		auto center = box.center();
#endif

		auto tx = radius / std::tan(pers.fov / 2.0 / 180.0 * 3.14159265358979323846);

		osg::Matrix rotation = osg::Matrix::rotate(osg::DegreesToRadians(90.0), osg::Vec3d(1, 0, 0));
		osg::Vec3d position;
		osg::Vec3d up;
		switch (direction)
		{
		case bimWorld::ModelViewerImpl::Front:
			up = osg::Vec3d(0, 1, 0);
			position = osg::Vec3d(0, 0, 0 + tx);
			break;
		case bimWorld::ModelViewerImpl::Back:
			up = osg::Vec3d(0, 1, 0);
			position = osg::Vec3d(0, 0, 0 - tx);
			break;
		case bimWorld::ModelViewerImpl::Left:
			up = osg::Vec3d(0, 1, 0);
			position = osg::Vec3d(0 - tx, 0, 0);
			break;
		case bimWorld::ModelViewerImpl::Right:
			up = osg::Vec3d(0, 1, 0);
			position = osg::Vec3d(0 + tx, 0, 0);
			break;
		case bimWorld::ModelViewerImpl::Top:
			up = osg::Vec3d(0, 0, -1);
			position = osg::Vec3d(0, 0 + tx, 0);
			break;
		case bimWorld::ModelViewerImpl::Bottom:
			up = osg::Vec3d(0, 0, 1);
			position = osg::Vec3d(0, 0 - tx, 0);
			break;
		default:
			break;
		}

		up = rotation.preMult(up);
		position = rotation.preMult(position);
		position += rotation.preMult(center);

		do
		{
			osg::MatrixTransform *mt = ((n == node) ? NULL : dynamic_cast<osg::MatrixTransform*>(n));
			if (mt != NULL) {
				center = mt->getMatrix().preMult(center);
				position = mt->getMatrix().preMult(position);
			}
		} while (n->getNumParents() == 1 && (n = n->getParent(0)));

#pragma endregion

		if (withoutAnimation)
		{
			m_mViewer->getCameraManipulator()->setHomePosition(position, center, up, false);
			m_mViewer->home();
			m_zoomtimes++;
		}
		else
		{
			double raduis = newNode->getBound().radius();
			osg::Vec3d frontVector = position - center;
			frontVector.normalize();
			osg::Vec3d newposition = center + frontVector*raduis*2.5;

			if (m_cameraAnimator)
			{
				m_cameraAnimator->setPath(createAnimationPath(newposition, center, up));
				m_cameraAnimator->addUpdateCallback(&ModelViewerImpl::updateCameraAnimator);
				origin_draw = getNeedUpdateAndDraw();
				m_cameraAnimator->addCompleteCallback(&ModelViewerImpl::completeCameraAnimator);

				setNeedUpdateAndDraw(true);
				startFixedFramerate();
				m_cameraAnimator->play();
			}
		}

		// we need refresh at last.
		updateSeveralTimes(1);
		return true;
	}

#pragma region // old
	//	void ModelViewerImpl::zoomTo(void* nodes[], bool withoutAnimation, ViewDirection direction)
	//	{
	//		if (!nodes)
	//			return;
	//#pragma region Calculate Zoom Parameter
	//
	//		CameraPerspective pers;
	//		bool got = getCameraPerspective(pers);
	//		if (!got)
	//		{
	//			osg::notify(osg::FATAL) << "Get camera perspective failed, in zoomTo(void* nodes[] ... ) at ModelViewerImpl." << std::endl;
	//			return;
	//		}
	//		float radius = 0;
	//		osg::Vec3 center;
	//
	//		float centerCalcus = 0; // <计算center时，计算了几次。除以此参数已获得中点。>
	//
	//		auto length = sizeof(nodes) / sizeof(nodes[0]);
	//		for (size_t i = 0; i < length; i++)
	//		{
	//			auto node = nodes[i];
	//
	//			osg::Node *n = (osg::Node*)node;
	//			if (!n)
	//			{
	//				osg::notify(osg::FATAL) << "The sub node is null, but we continued. In ModelViewerImpl::zoomTo(void* nodes[] ... )." << std::endl;
	//				continue;
	//			}
	//
	//#if defined(USING_BOUNDING_SPHERE) // defined(OSG_GLES1_AVAILABLE) || defined(OSG_GLES2_AVAILABLE)
	//			//auto transMatrix = n->getWorldMatrices()[0];
	//			auto sphere = n->getBound();
	//			double temp_radius = sphere.radius();
	//			//osg::Vec3 world = pos * worldMatrix;
	//			auto temp_center = sphere.center();
	//			auto tempPtr = n;
	//			do
	//			{
	//				osg::MatrixTransform *mt = ((tempPtr == node) ? NULL : dynamic_cast<osg::MatrixTransform*>(tempPtr));
	//				if (mt != NULL) {
	//					temp_center = mt->getMatrix().preMult(temp_center);
	//					auto scale = mt->getMatrix().getScale();
	//					double max = scale.x();
	//					max = scale.y() > max ? scale.y() : max;
	//					max = scale.z() > max ? scale.z() : max;
	//					temp_radius *= max;
	//				}
	//				} while (tempPtr->getNumParents() == 1 && (tempPtr = tempPtr->getParent(0)));
	//
	//
	//#else
	//			// TODO: to be implemented.
	//			assert(false);
	//#endif
	//			center += temp_center * temp_radius;
	//			centerCalcus += temp_radius;
	//			}
	//		if (centerCalcus == 0)
	//		{
	//			centerCalcus = 1;
	//			osg::notify(osg::FATAL) << "The center calcus is zero. It is invalid. in ModelViewerImpl::zoomTo(void* nodes[] ... )." << std::endl;
	//		}
	//		center /= centerCalcus;
	//
	//#pragma region Calcu Radius
	//
	//		for (size_t i = 0; i < length; i++)
	//		{
	//			osg::Node *n = (osg::Node*)nodes[i];
	//			if (!n)
	//			{
	//				osg::notify(osg::FATAL) << "The sub node is null, but we continued. In ModelViewerImpl::zoomTo(void* nodes[] ... )." << std::endl;
	//				continue;
	//			}
	//
	//#if defined(USING_BOUNDING_SPHERE)
	//			auto sphere = n->getBound();
	//			auto temp_center = sphere.center();
	//			double temp_radius = sphere.radius();
	//			auto tempPtr = n;
	//			do
	//			{
	//				osg::MatrixTransform *mt = ((tempPtr == n) ? NULL : dynamic_cast<osg::MatrixTransform*>(tempPtr));
	//				if (mt != NULL) {
	//					temp_center = mt->getMatrix().preMult(temp_center);
	//					auto scale = mt->getMatrix().getScale();
	//					double max = scale.x();
	//					max = scale.y() > max ? scale.y() : max;
	//					max = scale.z() > max ? scale.z() : max;
	//					temp_radius *= max;
	//				}
	//				} while (tempPtr->getNumParents() == 1 && (tempPtr = tempPtr->getParent(0)));
	//
	//			temp_radius = (center - temp_center).length() + temp_radius;
	//#else
	//			// TODO: to be implemented.
	//			assert(false);
	//#endif
	//
	//			if (temp_radius > radius)
	//				radius = temp_radius;
	//			}
	//
	//#pragma endregion
	//
	//
	//		auto tx = radius / std::tan(pers.fov / 2.0 / 180.0 * 3.14159265358979323846);
	//
	//		osg::Matrix rotation = osg::Matrix::rotate(osg::DegreesToRadians(90.0), osg::Vec3d(1, 0, 0));
	//		osg::Vec3d position;
	//		osg::Vec3d up;
	//		switch (direction)
	//		{
	//		case bimWorld::ModelViewerImpl::Front:
	//			up = osg::Vec3d(0, 1, 0);
	//			position = osg::Vec3d(0, 0, 0 + tx);
	//			break;
	//		case bimWorld::ModelViewerImpl::Back:
	//			up = osg::Vec3d(0, 1, 0);
	//			position = osg::Vec3d(0, 0, 0 - tx);
	//			break;
	//		case bimWorld::ModelViewerImpl::Left:
	//			up = osg::Vec3d(0, 1, 0);
	//			position = osg::Vec3d(0 - tx, 0, 0);
	//			break;
	//		case bimWorld::ModelViewerImpl::Right:
	//			up = osg::Vec3d(0, 1, 0);
	//			position = osg::Vec3d(0 + tx, 0, 0);
	//			break;
	//		case bimWorld::ModelViewerImpl::Top:
	//			up = osg::Vec3d(0, 0, -1);
	//			position = osg::Vec3d(0, 0 + tx, 0);
	//			break;
	//		case bimWorld::ModelViewerImpl::Bottom:
	//			up = osg::Vec3d(0, 0, 1);
	//			position = osg::Vec3d(0, 0 - tx, 0);
	//			break;
	//		default:
	//			break;
	//		}
	//
	//		up = rotation.preMult(up);
	//		position = rotation.preMult(position);
	//		position += rotation.preMult(center);
	//	
	//#pragma endregion
	//
	//		if (withoutAnimation)
	//		{
	//			m_mViewer->getCameraManipulator()->setHomePosition(position, center, up, false);
	//			m_mViewer->home();
	//			m_zoomtimes++;
	//		}
	//		else
	//		{
	//			osg::Vec3d frontVector = position - center;
	//			frontVector.normalize();
	//			osg::Vec3d newposition = center + frontVector*radius*2.5;
	//
	//			if (m_cameraAnimator)
	//			{
	//				m_cameraAnimator->setPath(createAnimationPath(newposition, center, up));
	//				m_cameraAnimator->addUpdateCallback(&ModelViewerImpl::updateCameraAnimator);
	//				origin_draw = getNeedUpdateAndDraw();
	//				m_cameraAnimator->addCompleteCallback(&ModelViewerImpl::completeCameraAnimator);
	//
	//				setNeedUpdateAndDraw(true);
	//				startFixedFramerate();
	//				m_cameraAnimator->play();
	//			}
	//		}
	//
	//		// we need refresh at last.
	//		updateSeveralTimes(1);
	//		}
#pragma endregion

#pragma region 先按半径权重遍历计算中点，将最远对角线的一半作为最后的半径


	bool ModelViewerImpl::zoomTo(const std::string* ids, int length, bool withoutAnimation, ViewDirection direction)
	{
		if (!ids)
			return false;
#pragma region Calculate Zoom Parameter

		CameraPerspective pers;
		bool got = getCameraPerspective(pers);
		if (!got)
		{
			osg::notify(osg::FATAL) << "Get camera perspective failed, in zoomTo(void* nodes[] ... ) at ModelViewerImpl." << std::endl;
			return false;
		}
		float radius = 0;
		osg::Vec3 center;
		osg::Node* node;
		float centerCalcus = 0; // <计算center时，计算了几次。除以此参数以获得中点。>

		osg::Vec3 minPos;
		osg::Vec3 maxPos(0, 0, 0);
		bool isFirstSetMin = true;

		for (size_t i = 0; i < length; i++)
		{
			osg::Node *n = findNodeById(ids[i]);
			if (!n)
			{
				osg::notify(osg::FATAL) << "The sub node is null, but we continued. In ModelViewerImpl::zoomTo(void* nodes[] ... )." << std::endl;
				continue;
			}

#if defined(USING_BOUNDING_SPHERE) // defined(OSG_GLES1_AVAILABLE) || defined(OSG_GLES2_AVAILABLE)
			//auto transMatrix = n->getWorldMatrices()[0];
			auto sphere = n->getBound();
			double temp_radius = sphere.radius();
			//osg::Vec3 world = pos * worldMatrix;
			auto temp_center = sphere.center();
#else
			// TODO: to be implemented.
			assert(false);
#endif
			//auto tempPtr = n;
			//do
			//{
			//	osg::MatrixTransform *mt = ((tempPtr == node) ? NULL : dynamic_cast<osg::MatrixTransform*>(tempPtr));
			//	if (mt != NULL) {
			//		temp_center = mt->getMatrix().preMult(temp_center);
			//	}
			//} while (tempPtr->getNumParents() == 1 && (tempPtr = tempPtr->getParent(0)));

			auto tmaxX = temp_center.x() + temp_radius;
			auto tmaxY = temp_center.y() + temp_radius;
			auto tmaxZ = temp_center.z() + temp_radius;

			auto tminX = temp_center.x() - temp_radius;
			auto tminY = temp_center.y() - temp_radius;
			auto tminZ = temp_center.z() - temp_radius;

			tmaxX = (maxPos.x() < tmaxX) ? tmaxX : maxPos.x();
			tmaxY = (maxPos.y() < tmaxY) ? tmaxY : maxPos.y();
			tmaxZ = (maxPos.z() < tmaxZ) ? tmaxZ : maxPos.z();
			if (!isFirstSetMin)
			{
				tminX = (minPos.x() > tminX) ? tminX : minPos.x();
				tminY = (minPos.y() > tminY) ? tminY : minPos.y();
				tminZ = (minPos.z() > tminZ) ? tminZ : minPos.z();
			}
			maxPos.set(tmaxX, tmaxY, tmaxZ);
			minPos.set(tminX, tminY, tminZ);

			center += temp_center * temp_radius;
			centerCalcus += temp_radius;
		}
		if (centerCalcus == 0)
		{
			centerCalcus = 1;
			osg::notify(osg::FATAL) << "The center calcus is zero. It is invalid. in ModelViewerImpl::zoomTo(void* nodes[] ... )." << std::endl;
		}
		center /= centerCalcus;

#pragma region Calcu Radius

		for (size_t i = 0; i < length; i++)
		{
			osg::Node *n = findNodeById(ids[i]);
			if (!n)
			{
				osg::notify(osg::FATAL) << "The sub node is null, but we continued. In ModelViewerImpl::zoomTo(void* nodes[] ... )." << std::endl;
				continue;
			}

#if defined(USING_BOUNDING_SPHERE)
			auto sphere = n->getBound();
			auto temp_center = sphere.center();
			double temp_radius = sphere.radius();
			//temp_radius = (center - temp_center).length() + temp_radius;
#else
			// TODO: to be implemented.
			assert(false);
#endif

			if (temp_radius > radius)
			{
				node = n;
				//radius = temp_radius;
			}
		}

		radius = (maxPos - minPos).length() / 2;

#pragma endregion


		auto tx = radius / std::tan(pers.fov / 2.0 / 180.0 * 3.14159265358979323846);

		osg::Matrix rotation = osg::Matrix::rotate(osg::DegreesToRadians(90.0), osg::Vec3d(1, 0, 0));
		osg::Vec3d position;
		osg::Vec3d up;
		switch (direction)
		{
		case bimWorld::ModelViewerImpl::Front:
			up = osg::Vec3d(0, 1, 0);
			position = osg::Vec3d(0, 0, 0 + tx);
			break;
		case bimWorld::ModelViewerImpl::Back:
			up = osg::Vec3d(0, 1, 0);
			position = osg::Vec3d(0, 0, 0 - tx);
			break;
		case bimWorld::ModelViewerImpl::Left:
			up = osg::Vec3d(0, 1, 0);
			position = osg::Vec3d(0 - tx, 0, 0);
			break;
		case bimWorld::ModelViewerImpl::Right:
			up = osg::Vec3d(0, 1, 0);
			position = osg::Vec3d(0 + tx, 0, 0);
			break;
		case bimWorld::ModelViewerImpl::Top:
			up = osg::Vec3d(0, 0, -1);
			position = osg::Vec3d(0, 0 + tx, 0);
			break;
		case bimWorld::ModelViewerImpl::Bottom:
			up = osg::Vec3d(0, 0, 1);
			position = osg::Vec3d(0, 0 - tx, 0);
			break;
		default:
			break;
		}

		up = rotation.preMult(up);
		position = rotation.preMult(position);
		position += rotation.preMult(center);

		osg::Node* n = node;
		do
		{
			osg::MatrixTransform *mt = ((n == node) ? NULL : dynamic_cast<osg::MatrixTransform*>(n));
			if (mt != NULL) {
				center = mt->getMatrix().preMult(center);
				position = mt->getMatrix().preMult(position);
			}
		} while (n->getNumParents() == 1 && (n = n->getParent(0)));

#pragma endregion

		if (withoutAnimation)
		{
			m_mViewer->getCameraManipulator()->setHomePosition(position, center, up, false);
			m_mViewer->home();
			m_zoomtimes++;
		}
		else
		{
			osg::Vec3d frontVector = position - center;
			frontVector.normalize();
			osg::Vec3d newposition = center + frontVector*radius*2.5;

			if (m_cameraAnimator)
			{
				m_cameraAnimator->setPath(createAnimationPath(newposition, center, up));
				m_cameraAnimator->addUpdateCallback(&ModelViewerImpl::updateCameraAnimator);
				origin_draw = getNeedUpdateAndDraw();
				m_cameraAnimator->addCompleteCallback(&ModelViewerImpl::completeCameraAnimator);

				setNeedUpdateAndDraw(true);
				startFixedFramerate();
				m_cameraAnimator->play();
			}
		}

		// we need refresh at last.
		updateSeveralTimes(1);
		return true;
	}

	bool ModelViewerImpl::zoomTo(void** nodes, int length, bool withoutAnimation, ViewDirection direction)
	{
		if (!nodes)
			return false;
#pragma region Calculate Zoom Parameter

		CameraPerspective pers;
		bool got = getCameraPerspective(pers);
		if (!got)
		{
			osg::notify(osg::FATAL) << "Get camera perspective failed, in zoomTo(void* nodes[] ... ) at ModelViewerImpl." << std::endl;
			return false;
		}
		float radius = 0;
		osg::Vec3 center;
		osg::Node* node;
		float centerCalcus = 0; // <计算center时，计算了几次。除以此参数以获得中点。>

		osg::Vec3 minPos;
		osg::Vec3 maxPos(0, 0, 0);
		bool isFirstSetMin = true;

		for (size_t i = 0; i < length; i++)
		{
			osg::Node *n = (osg::Node*)nodes[i];
			if (!n)
			{
				osg::notify(osg::FATAL) << "The sub node is null, but we continued. In ModelViewerImpl::zoomTo(void* nodes[] ... )." << std::endl;
				continue;
			}

#if defined(USING_BOUNDING_SPHERE) // defined(OSG_GLES1_AVAILABLE) || defined(OSG_GLES2_AVAILABLE)
			//auto transMatrix = n->getWorldMatrices()[0];
			auto sphere = n->getBound();
			double temp_radius = sphere.radius();
			//osg::Vec3 world = pos * worldMatrix;
			auto temp_center = sphere.center();
#else
			// TODO: to be implemented.
			assert(false);
#endif
			//auto tempPtr = n;
			//do
			//{
			//	osg::MatrixTransform *mt = ((tempPtr == node) ? NULL : dynamic_cast<osg::MatrixTransform*>(tempPtr));
			//	if (mt != NULL) {
			//		temp_center = mt->getMatrix().preMult(temp_center);
			//	}
			//} while (tempPtr->getNumParents() == 1 && (tempPtr = tempPtr->getParent(0)));

			auto tmaxX = temp_center.x() + temp_radius;
			auto tmaxY = temp_center.y() + temp_radius;
			auto tmaxZ = temp_center.z() + temp_radius;

			auto tminX = temp_center.x() - temp_radius;
			auto tminY = temp_center.y() - temp_radius;
			auto tminZ = temp_center.z() - temp_radius;

			tmaxX = (maxPos.x() < tmaxX) ? tmaxX : maxPos.x();
			tmaxY = (maxPos.y() < tmaxY) ? tmaxY : maxPos.y();
			tmaxZ = (maxPos.z() < tmaxZ) ? tmaxZ : maxPos.z();
			if (!isFirstSetMin)
			{
				tminX = (minPos.x() > tminX) ? tminX : minPos.x();
				tminY = (minPos.y() > tminY) ? tminY : minPos.y();
				tminZ = (minPos.z() > tminZ) ? tminZ : minPos.z();
			}
			maxPos.set(tmaxX, tmaxY, tmaxZ);
			minPos.set(tminX, tminY, tminZ);

			center += temp_center * temp_radius;
			centerCalcus += temp_radius;
		}
		if (centerCalcus == 0)
		{
			centerCalcus = 1;
			osg::notify(osg::FATAL) << "The center calcus is zero. It is invalid. in ModelViewerImpl::zoomTo(void* nodes[] ... )." << std::endl;
		}
		center /= centerCalcus;

#pragma region Calcu Radius

		for (size_t i = 0; i < length; i++)
		{
			osg::Node *n = (osg::Node*)nodes[i];
			if (!n)
			{
				osg::notify(osg::FATAL) << "The sub node is null, but we continued. In ModelViewerImpl::zoomTo(void* nodes[] ... )." << std::endl;
				continue;
			}

#if defined(USING_BOUNDING_SPHERE)
			auto sphere = n->getBound();
			auto temp_center = sphere.center();
			double temp_radius = sphere.radius();
			//temp_radius = (center - temp_center).length() + temp_radius;
#else
			// TODO: to be implemented.
			assert(false);
#endif

			if (temp_radius > radius)
			{
				node = n;
				//radius = temp_radius;
			}
		}

		radius = (maxPos - minPos).length() / 2;

#pragma endregion


		auto tx = radius / std::tan(pers.fov / 2.0 / 180.0 * 3.14159265358979323846);

		osg::Matrix rotation = osg::Matrix::rotate(osg::DegreesToRadians(90.0), osg::Vec3d(1, 0, 0));
		osg::Vec3d position;
		osg::Vec3d up;
		switch (direction)
		{
		case bimWorld::ModelViewerImpl::Front:
			up = osg::Vec3d(0, 1, 0);
			position = osg::Vec3d(0, 0, 0 + tx);
			break;
		case bimWorld::ModelViewerImpl::Back:
			up = osg::Vec3d(0, 1, 0);
			position = osg::Vec3d(0, 0, 0 - tx);
			break;
		case bimWorld::ModelViewerImpl::Left:
			up = osg::Vec3d(0, 1, 0);
			position = osg::Vec3d(0 - tx, 0, 0);
			break;
		case bimWorld::ModelViewerImpl::Right:
			up = osg::Vec3d(0, 1, 0);
			position = osg::Vec3d(0 + tx, 0, 0);
			break;
		case bimWorld::ModelViewerImpl::Top:
			up = osg::Vec3d(0, 0, -1);
			position = osg::Vec3d(0, 0 + tx, 0);
			break;
		case bimWorld::ModelViewerImpl::Bottom:
			up = osg::Vec3d(0, 0, 1);
			position = osg::Vec3d(0, 0 - tx, 0);
			break;
		default:
			break;
		}

		up = rotation.preMult(up);
		position = rotation.preMult(position);
		position += rotation.preMult(center);

		osg::Node* n = node;
		do
		{
			osg::MatrixTransform *mt = ((n == node) ? NULL : dynamic_cast<osg::MatrixTransform*>(n));
			if (mt != NULL) {
				center = mt->getMatrix().preMult(center);
				position = mt->getMatrix().preMult(position);
			}
		} while (n->getNumParents() == 1 && (n = n->getParent(0)));

#pragma endregion

		if (withoutAnimation)
		{
			m_mViewer->getCameraManipulator()->setHomePosition(position, center, up, false);
			m_mViewer->home();
			m_zoomtimes++;
		}
		else
		{
			osg::Vec3d frontVector = position - center;
			frontVector.normalize();
			osg::Vec3d newposition = center + frontVector*radius*2.5;

			if (m_cameraAnimator)
			{
				m_cameraAnimator->setPath(createAnimationPath(newposition, center, up));
				m_cameraAnimator->addUpdateCallback(&ModelViewerImpl::updateCameraAnimator);
				origin_draw = getNeedUpdateAndDraw();
				m_cameraAnimator->addCompleteCallback(&ModelViewerImpl::completeCameraAnimator);

				setNeedUpdateAndDraw(true);
				startFixedFramerate();
				m_cameraAnimator->play();
			}
		}

		// we need refresh at last.
		updateSeveralTimes(1);
		return true;
	}

#pragma endregion


	osgGA::CameraManipulator* ModelViewerImpl::getCameraManipulator()
	{
		if (!m_mViewer)
			return NULL;
		return m_mViewer->getCameraManipulator();


		//		if (!m_ptrKeySwitchMatrixManipulator)
		//			return NULL;
		//		return m_ptrKeySwitchMatrixManipulator->getCurrentMatrixManipulator();
	}

	bimWorld::ViewerMode ModelViewerImpl::getCameraManipulatorMode()
	{
		return m_cameraManipulatorMode;
	}

	osg::Camera*ModelViewerImpl::getCamera()
	{

		if (!m_mViewer)
			return NULL;
		return m_mViewer->getCamera();
	}


	void ModelViewerImpl::startFixedFramerate()
	{
		auto manipulator = static_cast<bimWorld::BIMCameraManipulator*>(getCameraManipulator());
		if (manipulator)
			manipulator->startFixedFramerate();
	}

	void ModelViewerImpl::cancelFixedFramerate()
	{
		auto manipulator = static_cast<bimWorld::BIMCameraManipulator*>(getCameraManipulator());
		if (manipulator)
			manipulator->cancelFixedFramerate();
	}

	bool ModelViewerImpl::getCategoryKey(const std::string& id, ConstructKey& outKey)
	{
		auto node = findNodeById(id);
		if (!node)
			return false;
		return false;
		//return StoreRelationship().getNodeConstructKey(node, outKey);
	}


	bool ModelViewerImpl::getVisibility(const std::string& id, bool& visible)
	{
		auto node = findNodeById(id);
		if (!node)
			return false;
		boost::mutex _lock;
		_lock.lock();
		bool result = node->getUserValue(NODE_VISIBILITY, visible);
		_lock.unlock();
		if (!result)
			return false;
		return true;
	}

	std::vector<ConstructKey> ModelViewerImpl::getCategoryKeys()
	{
		//return StoreRelationship().getCategoryKeys();
		std::vector<ConstructKey> keys;
#ifdef WIN32
		StoreRelationship().getAllCategory(m_currentPrjDBPath, keys);
#endif
		return keys;
		//StoreRelationship store;
		//std::vector<osg::Group*> outGroups;
		//store.getAllGroups(outGroups);
		//auto iter = outGroups.begin();
		//auto end = outGroups.end();

		////auto iter = m_nodePtrNameBimaps.begin();
		////auto end = m_nodePtrNameBimaps.end();

		//std::vector<ConstructKey> keys;
		//for (; iter != end; iter++)
		//{
		//	//auto collection = (*iter).second;
		//	ConstructKey key;
		//	if (!store.getGroupConstructKey(*iter, key))
		//		continue;
		//	keys.push_back(key);
		//}
		//return keys;
	}

	bool ModelViewerImpl::setHighLightByCategory(const YZString& floor, const YZString& topCategory, const YZString& secondaryCategory, bool isHighLight)
	{
		std::string levelName, categoryName;
#ifdef YIZHU_BIMWORLD_USE_WCHAR
		levelName = boost::locale::conv::from_utf(floor.c_str(), "utf-8");
		if (topCategory.empty())
		{
			categoryName = "";
		}
		else if (secondaryCategory.empty())
		{
			categoryName = boost::locale::conv::from_utf(topCategory.c_str(), "utf-8");
		}
		else
		{
			categoryName = boost::locale::conv::from_utf((topCategory + L"\\" + secondaryCategory).c_str(), "utf-8");
		}
#else
		levelName = floor;
		if (topCategory.empty())
		{
			categoryName = "";
		}
		else if (secondaryCategory.empty())
		{
			categoryName = topCategory;
		}
		else
		{
			categoryName = topCategory + "\\" + secondaryCategory;
		}
#endif
		std::vector<osg::Node*> nodevec;
		traverseSubNodesAndFindFirst(m_modelRoot.get(), [&](osg::Node* child){
			if (child->getName() != levelName)
			{
				return false;
			}
			auto group = child->asGroup();
			if (categoryName.empty())
			{
				nodevec.push_back(group);
				return true;
			}
			for (int i = 0; i < group->getNumChildren(); i++)
			{
				auto node = group->getChild(i);
				if (!node)
				{
					continue;
				}
				auto name = node->getName();
				// <先把系统类型拆分出来>
				std::vector < std::string> sysSplited;
				if (!util::StringExtension::split(name, "$", sysSplited))
					continue;
				if (sysSplited.size() < 1)
					continue;
				// <拆分一下参数>
				std::vector<std::string> paramSplited;
				if (!util::StringExtension::split(categoryName, "\\", paramSplited))
				{
					// <如果只有大类，则拆分一下名称，取名称中的大类与参数进行比较>
					std::vector<std::string> cateSplited;
					if (!util::StringExtension::split(sysSplited[0], "\\", cateSplited))
						continue;
					if (cateSplited.size() < 1)
					{
						continue;
					}

					if (cateSplited[0] != categoryName)
					{
						continue;
					}
				}
				else
				{
					// <如果有大小类， 直接与名称中的大小类进行比较>
					if (sysSplited[0] != categoryName)
					{
						continue;
					}

				}
				nodevec.push_back(node->asGroup());
			}
			return true;

		}, true);
		for (int i = 0; i < nodevec.size(); i++)
		{
			traverseAllSubNodes(nodevec[i], [this, isHighLight](osg::Node* n){
				if (!n || !n->asGeode())
				{
					return true;
				}
				osg::Geode* geod = n->asGeode();
				if (isHighLight)
				{
					highlight(n);
				}
				else
				{
					unHighlight(n);
				}
				return false;
			}, true);
		}
		return true;
		//
		//		GroupCollection groups;
		//		StoreRelationship store;
		//		NodeCollection nodes;
		//		if (!store.getNodesByCategory(m_currentPrjDBPath, topCategory, secondaryCategory, SystemType(), floor, m_idNodeptr, nodes))
		//			return false;
		//
		//		for (size_t n = 0; n < nodes.size(); n++)
		//		{
		//			if (!nodes[n])
		//				continue;
		//			if (isHighLight)
		//				highlight(nodes[n]);
		//			else
		//				unHighlight(nodes[n]);
		//		}
		//		return true;
		//		if (!store.getGroups(topCategory, secondaryCategory, SystemType(), floor, groups))
		//			return false;
		//		for (size_t g = 0; g < groups.size(); g++)
		//		{
		//			if (!groups[g])
		//				continue;
		//
		//#ifdef SUPPORT_CPP11
		//			traverseAllSubNodes(groups[g], [this, isHighLight](osg::Node* cur_node)->bool{
		//				if (!cur_node)
		//					return false;
		//
		//				//if (util::VectorExtension<osg::Node*>::contains(parents, cur_node))
		//				//	return true;
		//
		//				//if (cur_node == n)
		//				//	return false;
		//
		//				if (isHighLight)
		//					highlight(cur_node);
		//				else
		//					unHighlight(cur_node);
		//				return true;
		//		}, true);
		//#else
		//			NodeCollection nodes;
		//			if (!store.getNodes(groups[g], nodes))
		//				continue;
		//			for (size_t n = 0; n < nodes.size(); n++)
		//			{
		//				if (!nodes[n])
		//					continue;
		//
		//				if (isHighLight)
		//					highlight(nodes[n]);
		//				else
		//					unHighlight(nodes[n]);
		//			}
		//#endif
		//	}
		//		return true;
	}

	bool ModelViewerImpl::findAllParentsOfNode(osg::Node* node, std::vector<osg::Node*>& outVector)
	{
		if (!node)
			return false;

		boost::mutex _lock;
		_lock.lock();

		if (node->getNumParents() != 1)
		{
			osg::notify(osg::FATAL) << "We suppose each node only has one parent node, but it is not true, change the data associated with it."
				<< " in ModelViewerImpl::findAllParentsOfNode" << std::endl;
		}

		if (node->getNumParents() < 1)
		{
			_lock.unlock();
			return false;
		}

		auto parent = node->getParent(0);
		while (parent)
		{
			outVector.push_back(parent);
			if (parent->getNumParents() > 0)
			{
				if (parent->getNumParents() != 1)
				{
					osg::notify(osg::FATAL) << "We suppose each node only has one parent node, but it is not true, change the data associated with it."
						<< " in ModelViewerImpl::findAllParentsOfNode" << std::endl;
				}

				parent = parent->getParent(0);
			}
			else
			{
				parent = NULL;
			}

		}
		_lock.unlock();
		return true;
	}


	void ModelViewerImpl::hideOthers(const std::string& id)
	{
		auto node = findNodeById(id);
		if (!node)
			return;
		hideOthers(node);
	}

	void ModelViewerImpl::hideOthers(void* node)
	{
		auto n = static_cast<osg::Node*>(node);
		if (!n)
			return;
		StoreRelationship store;
		std::vector<osg::Node*> parents;
		findAllParentsOfNode(n, parents);
#ifdef SUPPORT_CPP11
		traverseAllSubNodes(m_modelRoot, [parents, n](osg::Node* cur_node)->bool{
			if (!cur_node)
				return false;

			if (util::VectorExtension<osg::Node*>::contains(parents, cur_node))
				return true;

			if (cur_node == n)
				return false;

			cur_node->setUserValue(NODE_VISIBILITY, false);
			cur_node->setNodeMask(0);
			return true;
		}, true);
#else
		parents.push_back(n);
		std::vector<osg::Group*> groups;
		store.getAllGroups(groups);
		//for (; iter != end; iter++)
		//{
		//	auto groups = *iter;
		for (int i = 0; i < groups.size(); i++)
		{
			if (!groups[i])
				continue;
			if (util::VectorExtension<osg::Node*>::contains(parents, groups[i]))
				continue;
			groups[i]->setUserValue(NODE_VISIBILITY, false);
			groups[i]->setNodeMask(0);
			NodeCollection nodes;

			if (store.getNodes(groups[i], nodes))
			{
				for (int j = 0; j < nodes.size(); j++)
				{
					if (!nodes[j])
						continue;
					if (util::VectorExtension<osg::Node*>::contains(parents, nodes[j]))
						continue;
					nodes[j]->setUserValue(NODE_VISIBILITY, false);
					nodes[j]->setNodeMask(0);
				}
			}
		}
		//}
#endif
		updateSeveralTimes(1);
		}


	void ModelViewerImpl::unHideOthers(const std::string& id)
	{
		auto node = findNodeById(id);
		if (!node)
			return;
		unHideOthers(node);
	}

	void ModelViewerImpl::unHideOthers(void* node)
	{
		auto n = static_cast<osg::Node*>(node);
		if (!n)
			return;
		std::vector<osg::Node*> parents;
		StoreRelationship store;
		findAllParentsOfNode(n, parents);

#ifdef SUPPORT_CPP11
		traverseAllSubNodes(m_modelRoot, [parents, n](osg::Node* cur_node)->bool{
			if (!cur_node)
				return false;

			if (util::VectorExtension<osg::Node*>::contains(parents, cur_node))
				return true;

			if (cur_node == n)
				return false;

			cur_node->setUserValue(NODE_VISIBILITY, true);
			cur_node->setNodeMask(-1);
			return true;
		}, true);
#else

		parents.push_back(n);
		std::vector<osg::Group*> groups;
		store.getAllGroups(groups);
		for (int i = 0; i < groups.size(); i++)
		{
			if (!groups[i])
				continue;
			if (util::VectorExtension<osg::Node*>::contains(parents, groups[i]))
				continue;
			groups[i]->setUserValue(NODE_VISIBILITY, true);
			groups[i]->setNodeMask(-1);
			NodeCollection nodes;
			if (store.getNodes(groups[i], nodes))
			{
				for (int j = 0; j < nodes.size(); j++)
				{
					if (!nodes[j])
						continue;
					if (util::VectorExtension<osg::Node*>::contains(parents, nodes[j]))
						continue;
					nodes[j]->setUserValue(NODE_VISIBILITY, true);
					nodes[j]->setNodeMask(-1);
				}
			}
		}

#endif // SUPPORT_CPP11

		updateSeveralTimes(1);
		}

	void ModelViewerImpl::unHideAll()
	{
		StoreRelationship store;
#ifdef SUPPORT_CPP11
		traverseAllSubNodes(m_modelRoot, [](osg::Node* node)->bool{
			if (!node)
				return false;
			boost::mutex _lock;
			_lock.lock();

			node->setUserValue(NODE_VISIBILITY, true);
			node->setNodeMask(-1);
			_lock.unlock();
			return true;
		}, true);
#else
		std::vector<osg::Group*> groups;
		store.getAllGroups(groups);
		for (int i = 0; i < groups.size(); i++)
		{
			if (!groups[i])
				continue;
			groups[i]->setUserValue(NODE_VISIBILITY, true);
			groups[i]->setNodeMask(-1);
			NodeCollection nodes;
			if (store.getNodes(groups[i], nodes))
			{
				for (int j = 0; j < nodes.size(); j++)
				{
					if (!nodes[j])
						continue;
					nodes[j]->setUserValue(NODE_VISIBILITY, true);
					nodes[j]->setNodeMask(-1);
				}
			}
		}

#endif // SUPPORT_CPP11

		updateSeveralTimes(1);
		}

	void  ModelViewerImpl::hideAll()
	{
		StoreRelationship store;
#ifdef SUPPORT_CPP11
		traverseAllSubNodes(m_modelRoot, [](osg::Node* node)->bool{
			if (!node)
				return false;
			boost::mutex _lock;
			_lock.lock();

			node->setUserValue(NODE_VISIBILITY, false);
			node->setNodeMask(0);
			_lock.unlock();
			return true;
		}, true);
#else

		std::vector<osg::Group*> groups;
		store.getAllGroups(groups);
		for (int i = 0; i < groups.size(); i++)
		{
			if (!groups[i])
				continue;
			groups[i]->setUserValue(NODE_VISIBILITY, false);
			groups[i]->setNodeMask(0);
			NodeCollection nodes;

			if (store.getNodes(groups[i], nodes))
			{
				for (int j = 0; j < nodes.size(); j++)
				{
					if (!nodes[j])
						continue;
					nodes[i]->setUserValue(NODE_VISIBILITY, false);
					nodes[j]->setNodeMask(0);
				}
			}
		}

#endif // DEBUG

		updateSeveralTimes(1);

		/*
		for (; iter != end; iter++)
		{
		auto collection = (*iter).second;

		for (auto colBegin = collection.begin(); colBegin != collection.end(); ++colBegin)
		{
		osg::Node *n = static_cast<osg::Node*>(*colBegin);
		if (!n)
		continue;
		n->setUserValue(NODE_VISIBILITY, false);
		n->setNodeMask(0);
		}
		updateSeveralTimes(1);
		}*/
		/*
		std::vector< osg::ref_ptr<osg::Node> > nodevec;
		NodePtrNameBimaps::iterator posst = m_nodePtrNameBimaps.begin();
		NodePtrNameBimaps::iterator posed = m_nodePtrNameBimaps.end();
		NodePtrNameBimaps tempBimaps(m_nodePtrNameBimaps);
		for (; posst != posed; posst++)
		{
		YZString strname = posst->first;
		#ifdef YIZHU_BIMWORLD_USE_WCHAR
		int rpos = strname.rfind(_T("\\"));
		YZString strflrclass = strname.substr(0, rpos);
		#else
		int rpos = strname.rfind(("\\"));
		YZString strflrclass = strname.substr(0, rpos);
		#endif
		NodePtrNameBimaps::iterator postempst =tempBimaps.begin();
		NodePtrNameBimaps::iterator postemped =tempBimaps.end();

		#ifdef YIZHU_BIMWORLD_USE_WCHAR
		int rpos = strname.rfind(_T("\\"));
		YZString nodename = strname.substr(0, rpos);
		#else
		int rpos = strname.rfind(("\\"));
		YZString nodename = strname.substr(0, rpos);
		#endif
		if (strflrclass == nodename)
		{
		nodevec.push_back(posst->second);
		}
		}

		for(int i=0;i<nodevec.size();i++)
		{
		if(nodevec[i] != NULL)
		{
		//if(visible)
		//{
		//	unHide(nodevec[i]);
		//}
		//else
		//{
		hide(nodevec[i]);
		//}
		}
		}
		nodevec.clear();
		}
		*/

		}

	void ModelViewerImpl::hide(void **nodes, int n)
	{
		if (!nodes || !*nodes)
			return;
		boost::mutex _lock;
		_lock.lock();

		for (int i = 0; i < n; ++i) {
			osg::Node *node = static_cast<osg::Node*>(nodes[i]);
			if (!node)
				continue;
			node->setUserValue(NODE_VISIBILITY, false);
			node->setNodeMask(0);
		}
		_lock.unlock();
		updateSeveralTimes(1);
	}

	void ModelViewerImpl::unHide(void **nodes, int n)
	{
		if (!nodes || !*nodes)
			return;

		boost::mutex _lock;
		_lock.lock();

		for (int i = 0; i < n; ++i) {
			osg::Node *node = static_cast<osg::Node*>(nodes[i]);
			if (!node)
				continue;
			node->setUserValue(NODE_VISIBILITY, true);
			node->setNodeMask(-1);
		}
		_lock.unlock();
		updateSeveralTimes(1);
	}

	void ModelViewerImpl::toggleHide(void **nodes, int n)
	{
		if (!nodes || !*nodes)
			return;
		boost::mutex _lock;
		_lock.lock();
		for (int i = 0; i < n; ++i)
		{
			osg::Node *n = static_cast<osg::Node*>(nodes[i]);
			if (!n)
				continue;

			if (n->getNodeMask() == 0)
			{
				n->setUserValue(NODE_VISIBILITY, true);
				n->setNodeMask(-1);
			}
			else if (n->getNodeMask() == -1)
			{
				n->setUserValue(NODE_VISIBILITY, false);
				n->setNodeMask(0);
			}
		}
		_lock.unlock();
		updateSeveralTimes(1);
	}

	bool ModelViewerImpl::isVisible(void *node)
	{
		if (!node)
			return false;
		osg::Node *n = static_cast<osg::Node*>(node);
		if (!n)
			return false;
		return n->getNodeMask() != 0;
	}


	bool ModelViewerImpl::getCameraView(CameraView& cameraView)
	{
		auto cameraManipulator = m_mViewer->getCameraManipulator();
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



	bool ModelViewerImpl::getCameraView(CameraView& cameraView, const char* file, unsigned char** buffer, int &width, int &height, int &channels)
	{
		//        boost::mutex _lock;
		//        _lock.lock();
		if (!m_rendering)
			return false;

		getCameraView(cameraView);
		return CaptureScreen(buffer, width, height, channels, 0, 0, file);

	}

	bool ModelViewerImpl::CaptureScreen(unsigned char** buffer, int & width, int & height, int & channels, int x, int y, const char* file)
	{

		boost::mutex _lock;
		_lock.lock();
		setIsExternalRendering(true);

		auto originalThreadingModel = m_mViewer->getThreadingModel();

		m_mViewer->setThreadingModel(osgViewer::Viewer::SingleThreaded);



		//        m_mViewer->frame();

		auto camera = m_mViewer->getCamera();

		if (width == 0 || height == 0)
		{
			width = camera->getViewport()->width();
			height = camera->getViewport()->height();
		}
		int length;

		//        OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_mutex);

		std::cerr << "W : " << width << "H : " << height;
		auto gc = camera->getGraphicsContext();
		if (!gc)
			return false;

		if (!gc->getTraits())
			return false;

		if (!gc->isCurrent())
			gc->makeCurrent();

		GLenum pixelFormat;

		if (gc->getTraits()->alpha)
			pixelFormat = GL_RGBA;
		else
			pixelFormat = GL_RGB;

#if defined(OSG_GLES1_AVAILABLE) || defined(OSG_GLES2_AVAILABLE)
		if (pixelFormat == GL_RGB)
		{
			GLint value = 0;
#ifndef GL_IMPLEMENTATION_COLOR_READ_FORMAT
#define GL_IMPLEMENTATION_COLOR_READ_FORMAT 0x8B9B
#endif
			glGetIntegerv(GL_IMPLEMENTATION_COLOR_READ_FORMAT, &value);
			if (value != GL_RGB ||
				value != GL_UNSIGNED_BYTE)
			{
				pixelFormat = GL_RGBA;//always supported
			}
	}
#endif

		// set channels.
		if (pixelFormat == GL_RGBA)
			channels = 4;
		else if (pixelFormat == GL_RGB)
			channels = 3;
		else
		{
			osg::notify(osg::FATAL) << "The pixel format is not usual, may be get failed." << std::endl;
			return false;
		}

		length = (width)* (height)* (channels);

		auto buf = new unsigned char[length];

		osg::ref_ptr<ScreenCaptureHandler> handler = new ScreenCaptureHandler(file, &buf, width, height, channels, pixelFormat, x, y);

		m_mViewer->getCamera()->setFinalDrawCallback(handler.get());
		m_mViewer->renderingTraversals();
		m_mViewer->getCamera()->setFinalDrawCallback(NULL);

		//		handler->capture(m_mViewer->getCamera());

		m_mViewer->setThreadingModel(originalThreadingModel);

		setIsExternalRendering(false);

		// judge whether it is a black picture.
		for (int i = 0; i < width*height*channels; i++)
		{
			if ((int)(buf)[i] != 0)
			{
				if (channels == 4 && ((i + 1) % 4 == 0))
				{
					continue;
				}
				else
				{
					*buffer = buf;
					_lock.unlock();
					return true;
				}
			}
		}

		*buffer = buf;
		_lock.unlock();
		return false;
	}

	bool ModelViewerImpl::getCameraPerspective(CameraPerspective& cameraPerspective)
	{
		m_mViewer->getCamera()->getProjectionMatrixAsPerspective(cameraPerspective.fov,
			cameraPerspective.aspectRatio, cameraPerspective.zNear, cameraPerspective.zFar);
		return true;
	}

	bool ModelViewerImpl::setCameraView(const CameraView& cameraView)
	{
		// <暂不支持其他相机操作器, 如后续需要，则重载相应setByMatrix方法。>
		auto cameraManipulator = static_cast<BIMCameraManipulator*>(m_mViewer->getCameraManipulator());
		if (!cameraManipulator)
			return false;

		cameraManipulator->setHomePosition(
			*(osg::Vec3d*)&cameraView.position,
			*(osg::Vec3d*)&cameraView.target,
			*(osg::Vec3d*)&cameraView.upVector);
		m_mViewer->home();
		cameraManipulator->beginSetCameraMatrix();
		cameraManipulator->setByMatrix(osg::Matrix(cameraView.matrix));
		cameraManipulator->endSetCameraMatrix();
		updateSeveralTimes(1);
		return true;
	}

	bool ModelViewerImpl::setCameraPerspective(const CameraPerspective& cameraPerspective)
	{
		m_mViewer->getCamera()->setProjectionMatrixAsPerspective(cameraPerspective.fov,
			cameraPerspective.aspectRatio, cameraPerspective.zNear, cameraPerspective.zFar);

		updateSeveralTimes(1);
		return true;
	}

	// <切换漫游器>
	void ModelViewerImpl::switchMatrixManipulator(EMANIPULATOR emanip)
	{
		if (!m_mViewer || !m_sceneRoot || !m_modelRoot)
		{
			return;
		}
		setIsExternalRendering(true);
		auto manip = m_mViewer->getCameraManipulator();
		osg::Vec3d eye, center, up;
		auto matrix = manip->getMatrix();
		manip->getHomePosition(eye, center, up);
		switch (emanip)
		{
            case deflaut:
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
                ((BIMCameraManipulator*)m_d.get())->setPosition(eye, center, up, matrix);

                ////                BIMCameraManipulator* mat = (BIMCameraManipulator*)(m_d.get());
                //                 ((BIMCameraManipulator*)m_d.get())->beginSetCameraMatrix();
                //                ((BIMCameraM-anipulator*)m_d.get())->setByMatrix(matrix);
                //               ((BIMCameraManipulator*)m_d.get())->endSetCameraMatrix();
                m_mViewer->setCameraManipulator(((BIMCameraManipulator*)m_d.get()), false);
                break;
            case person:
            {
                auto oldManip = static_cast<BIMCameraManipulator*>(manip);
                if(!oldManip)
                    break;
                auto rotation = oldManip->getRotation();
                auto dist = oldManip->getDistance();
                auto personManip = static_cast<PersonManipulator*>(m_p.get());
                if (!personManip)
                    break;
                personManip->setScreenRotation(rotation);
                personManip->setScreenDistance(dist);
                personManip->setPosition(eye, center, up, matrix);
                m_mViewer->setCameraManipulator(personManip, false);
                break;
            }
            case FirstPerson:
            {
                m_sceneRoot->getOrCreateStateSet()->setMode(GL_LIGHT0,
                    osg::StateAttribute::OFF);
                m_sceneRoot->getOrCreateStateSet()->setMode(GL_LIGHT1,
                    osg::StateAttribute::OFF);
                m_sceneRoot->getOrCreateStateSet()->setMode(GL_LIGHT2,
                    osg::StateAttribute::ON);
                m_sceneRoot->getOrCreateStateSet()->setMode(GL_LIGHT3,
                    osg::StateAttribute::ON);
                m_sceneRoot->getOrCreateStateSet()->setMode(GL_LIGHT4,
                    osg::StateAttribute::ON);

                osgGA::CameraManipulator* first = new BIMFirstPersonManipulator(this);
                m_mViewer->setCameraManipulator(first, true);

                first->setHomePosition(m_modelRoot->getBound().center(), osg::Vec3(0, 0, 0), up);
                m_mViewer->home();
                break;
            }
		}
		setIsExternalRendering(false);

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

		updateSeveralTimes(1);
    }
    
    void ModelViewerImpl::enableFirstPersonControl()
    {
        switchMatrixManipulator(person);
        
    }
    
    void ModelViewerImpl::disableFirstPersonControl()
    {
        switchMatrixManipulator(deflaut);
    }

#pragma region SwitchView

	void ModelViewerImpl::switchToFrontView()
	{
		zoomTo(m_modelRoot.get(), true, Front);
	}

	void ModelViewerImpl::switchToBackView()
	{
		zoomTo(m_modelRoot.get(), true, Back);
	}

	void ModelViewerImpl::switchToLeftView()
	{
		zoomTo(m_modelRoot.get(), true, Left);
	}

	void ModelViewerImpl::switchToRightView()
	{
		zoomTo(m_modelRoot.get(), true, Right);
	}

	void ModelViewerImpl::switchToTopView()
	{
		zoomTo(m_modelRoot.get(), true, Top);
	}

	void ModelViewerImpl::switchToBottomView()
	{
		zoomTo(m_modelRoot.get(), true, Bottom);
	}

#pragma endregion

	void ModelViewerImpl::getAll(std::vector<std::string>& ids)
	{
		UuidAttrHash::const_iterator begin = m_UuidAttrHash.begin(), end = m_UuidAttrHash.end();
		while (begin != end)
		{
			assert(begin->first == begin->second.id);
			ids.push_back(begin->second.id);
			++begin;
		}
	}


	void ModelViewerImpl::getByFloor(std::vector<std::string>& ids, const Floor* floor)
	{
		UuidAttrHash::const_iterator begin = m_UuidAttrHash.begin(), end = m_UuidAttrHash.end();
		while (begin != end)
		{
			assert(begin->first == begin->second.id);
			const Floor *f = begin->second.getFloor();
			if (f == floor || (f != NULL && floor != NULL && *f == *floor))
				ids.push_back(begin->second.id);
			++begin;
		}
	}

	void ModelViewerImpl::getByCategory(std::vector<std::string>& ids, const Category* category)
	{
		UuidAttrHash::const_iterator begin = m_UuidAttrHash.begin(), end = m_UuidAttrHash.end();
		while (begin != end)
		{
			assert(begin->first == begin->second.id);
			const Category *c = begin->second.getCategory();
			if (c == category || (c != NULL && category != NULL && *c == *category))
				ids.push_back(begin->second.id);
			++begin;
		}
	}

	void dosth()
	{
		int i = 0;
		i++;
	}

	void ModelViewerImpl::updateMode()
	{
#if WIN32 || IOS_USING_FIXED_FRAMERATE
		if (m_sceneRoot.valid() && m_mViewer.valid())
		{


			//			osgGA::KeySwitchMatrixManipulator *switchmanipulator = dynamic_cast<osgGA::KeySwitchMatrixManipulator*>(m_mViewer->getCameraManipulator());
			//			switchmanipulator->selectMatrixManipulator(0);
			//PersonManipulator* manipulator= dynamic_cast<PersonManipulator*>(switchmanipulator->getCurrentMatrixManipulator());
			//			BIMCameraManipulator *manipulator = dynamic_cast<BIMCameraManipulator*>(m_mViewer->getCameraManipulator());
			BIMCameraManipulator* manipulator = dynamic_cast<BIMCameraManipulator*>(getCameraManipulator());

			if (manipulator)
			{
				manipulator->setupFrameRateController(m_modelRoot.get());
				manipulator->setModelRoot(m_modelRoot.get());
			}
		}

#endif
		zoomTo(m_modelRoot.get());
		fireModelLoadEvent(m_modelViewerListeners);
		updateSeveralTimes(1);
	}

	void ModelViewerImpl::load(const char **files, int fileNum)
	{
		m_isLoaded = false;
		// Init the main Root Node/Group

		// Load the Model from the model name
		osg::ref_ptr<osg::Node> model = readNode(files, fileNum);
        
        
		if (!model.get())
		{
			m_isLoaded = true;
			return;
		}

		StoreRelationship().clearAll();

		//osg::ref_ptr<osg::Group> reorganized = reorganize();
		//if (reorganized)
		//	model = reorganized;
		//else



		initializeCategoryGroups(model);
		fireModelUnloadEvent(m_modelViewerListeners);
		// Optimize the model
		//osgUtil::Optimizer optimizer;
		//optimizer.optimize(mModel.get());
		//optimizer.reset();

		//unLoadAll();
		m_sceneRoot->removeChild(m_modelRoot);
		// Add the model to the scene

		m_sceneRoot->addChild(model);
        m_modelRoot->removeChildren(0, m_modelRoot->getNumChildren());
        m_modelRoot->addChild(model);
        zoomTo(m_modelRoot.get());
		
		updateMode();

		m_isLoaded = true;
	}
	void ModelViewerImpl::readNodeByMultiFle(const char **files, int fileNum)
	{

		// <初始化节点指针与ID 双向bimap>

		if (!m_modelRoot)
		{
			m_modelRoot = new osg::MatrixTransform;
			m_modelRoot->setName("root");
		}

		std::set<std::string> nodeNameSet;
		int numChild = m_modelRoot->getNumChildren();
		for (int i = 0; i < numChild; ++i)
		{
			osg::Node *child = m_modelRoot->getChild(i);
			nodeNameSet.insert(child->getName());
		}

		osg::ref_ptr<osg::Node> node;
		for (int i = 0; i < fileNum; ++i)
		{
			std::cout << "read node file : " << files[i] << std::endl;
			node = readNode(files[i]);

			if (node != (osg::Node *)0L)
			{
				if (node->getName().empty())
					node->setName(files[i]);
				// <防止重复加载，遍历构件及属性信息>
				if (!(nodeNameSet.find(node->getName()) != nodeNameSet.end()))
				{
					if ((node->getName() == "root") && (node->asGroup()->getNumChildren() == 1))
					{
						m_modelRoot->addChild(node->asGroup()->getChild(0));
					}
					else
					{
						m_modelRoot->addChild(node);
					}

				}
				else
				{


				}
			}
		}

		return;
	}

	int  ModelViewerImpl::loadBySingleFlr(const char *files, double dEngineeringLevel, long& outFloorName)
	{
		m_isLoaded = false;

		osg::ref_ptr<osg::Node> model = readNode(files);
		if (!model.get())
		{
			m_isLoaded = true;
			return -1;
		}

		fireModelUnloadEvent(m_modelViewerListeners);

		bool needReadd = false;

		if (!m_modelRoot)
		{
			m_modelRoot = new osg::MatrixTransform();
			m_modelRoot->setName("root");
			needReadd = true;
		}
		int numChild = m_modelRoot->getNumChildren();
#if _DEBUG
		int refCount = model->referenceCount();
#endif // _DEBUG
		outFloorName = (long)(model.get());
#if _DEBUG
		refCount = model->referenceCount();
#endif // _DEBUG
		for (int i = 0; i < numChild; ++i)
		{
			osg::Node *child = m_modelRoot->getChild(i);
			if (child->getName() == model->getName())
			{
				m_isLoaded = true;
				return 1;
			}
		}
		//osg::Matrix mat;
		//mat.makeTranslate(osg::Vec3(0.0f,0.0f,dEngineeringLevel));
		//osg::ref_ptr<osg::MatrixTransform> transform =new osg::MatrixTransform();
		//transform->setMatrix(mat);
		//transform->addChild(model);
		//m_modelRoot->addChild(transform);
		m_modelRoot->addChild(model.get());

		//初始化节点指针与ID 双向bimap
		//GetIdVisitor<NodeptrIdBimaps, NodePtrNameBimaps> visitor(&m_nodeptrToIdBimaps, &m_nodePtrNameBimaps);
		//model->accept(visitor);

#if _DEBUG
		refCount = model->referenceCount();
#endif // _DEBUG

		initializeCategoryGroups(m_modelRoot.get());

		// Add the model to the scene
		if (needReadd)
		{
			m_sceneRoot->addChild(m_modelRoot.get());
		}
		updateMode();
		m_isLoaded = true;
		return 0;

	}

	void ModelViewerImpl::unLoadAll()
	{
		if (!m_modelRoot || !m_sceneRoot)
		{
			return;
		}
		//m_modelRoot->removeChildren(0, m_modelRoot->getNumChildren());
		auto manip = static_cast<BIMCameraManipulator*>(getCameraManipulator());
		if (manip)
		{
			manip->setModelRoot(NULL);
		}
		m_sceneRoot->removeChild(m_modelRoot);
        m_modelRoot = NULL;
        updateMode();
	}

	bool ModelViewerImpl::unLoadSingleFlr(osg::Node* floorNode)
	{
		if (!m_modelRoot)
			return false;

		auto node = (osg::Node*)(floorNode);
		if (!node)
			return false;

		// 移除节点指针与ID 双向bimap
		//RemoveIdVisitor<NodeptrIdBimaps, NodePtrNameBimaps> visitor(&m_nodeptrToIdBimaps, &m_nodePtrNameBimaps);
		//node->accept(visitor);

		// TODO: < 实现在关联关系中卸载楼层 >
		//// < 移除ConstructMaterialSet >

		bool result = m_modelRoot->removeChild(floorNode);
		int refCount = floorNode->referenceCount();

		if (refCount != 0 && refCount > 0)
		{
			osg::notify(osg::WARN) << "The floor root node has reference count : " << refCount << ". Manually unref() it." << std::endl;
			//for (int i = 0; i < refCount; i++)
			//{
			//	node->unref();
			//	refCount = node->referenceCount();
			//}
		}
        
        updateMode();
		updateSeveralTimes(1);
		return result;
	}

	// <多层批量加载模式>
	int ModelViewerImpl::loadByMultiFlr(const char **								files,
		int										ifileSize,
		std::vector<double>&						dEngineeringLevelvec,

		std::vector<std::pair<YZString, std::string> >&	vecresut)
	{
		m_isLoaded = false;
		count_size = 0;
		readNodeByMultiFle(files, ifileSize);

		StoreRelationship().clearAll();
		//int numChild = m_modelRoot->getNumChildren();
		//for (int i = 0; i < numChild; ++i)
		//	initializeCategoryGroup(m_modelRoot->getChild(i));
		initializeCategoryGroups(m_modelRoot.get());
		fireModelUnloadEvent(m_modelViewerListeners);

		// Add the model to the scene

		m_sceneRoot->addChild(m_modelRoot);
		zoomTo(m_modelRoot.get());


		updateMode();

		m_isLoaded = true;
		return 0;
	}
	osg::ref_ptr<osg::Node> ModelViewerImpl::readFBXNode(const char *file)
	{

		return osgDB::readNodeFile(file);
	}


	osg::Node* ModelViewerImpl::readNode(const char *file)
	{
		osg::ref_ptr<osg::Node> result;
#ifdef WIN32

		std::auto_ptr<probuf> pb(new probuf(osgDB::findDataFile(std::string(file))));
		if (pb->is_open())
		{
			std::istream mis(pb.get());
			osg::ref_ptr<osgDB::ReaderWriter> reader = osgDB::Registry::instance()->getReaderWriterForExtension(osgDB::getLowerCaseFileExtension(std::string(file)));
			osgDB::ReaderWriter::ReadResult readResult = reader->readNode(mis);
			result = readResult.getNode();
		}
		else
		{
			result = osgDB::readNodeFile(file);
		}
#else
		result = osgDB::readNodeFile(file);
#endif
		return result.release();
		}

	void ModelViewerImpl::loadParam(const char *file)
	{
		loadParam(&file, 1);
	}

	osg::Node* ModelViewerImpl::readNode(const char **files, int fileNum)
	{
		typedef std::vector<osg::ref_ptr<osg::Node> > NodeList;
		NodeList nodeList;
		osg::ref_ptr<osg::Node> node;
		for (int i = 0; i < fileNum; ++i)
		{
			std::cout << "read node file : " << files[i] << std::endl;
			node = readNode(files[i]);

			if (node != (osg::Node *)0L)
			{
				if (node->getName().empty())
				{
					node->setName(files[i]);
					node->setDataVariance(osg::Object::DYNAMIC);
				}
				nodeList.push_back(node.release());
			}
		}

		if (nodeList.empty())
			return NULL;
		//初始化节点指针与ID 双向bimap

		//GetIdVisitor<NodeptrIdBimaps, NodePtrNameBimaps> visitor(&m_nodeptrToIdBimaps, &m_nodePtrNameBimaps);
		if (nodeList.size() == 1)
		{

			//nodeList[0]->accept(visitor);
			return nodeList[0].release();
		}

        osg::ref_ptr<osg::Group> group = new osg::Group;
		group->setName("root");
		for (int i = 0; i < nodeList.size(); ++i)
			group->addChild(nodeList[i]);
		//group->accept(visitor);
		return group.release();
	}
#include <iostream>
#include <fstream>
	osg::ref_ptr<osg::Node> ModelViewerImpl::readFBXNode(const char **files, int fileNum)
	{
		typedef std::vector<osg::ref_ptr<osg::Node> > NodeList;
		NodeList nodeList;
		osg::ref_ptr<osg::Node> node;
		for (int i = 0; i < fileNum; ++i)
		{
			std::cout << "read node file : " << files[i] << std::endl;
			node = readFBXNode(files[i]);
			std::string  NodeName = node->getName();
			//if (node != (osg::Node *)0L)
			//{
			//	if (node->getName().empty())
			//	{
			//		node->setName(files[i]);
			//	node->setDataVariance(osg::Object::DYNAMIC);
			//	}
			nodeList.push_back(node);
			//}
		}

		if (nodeList.empty())
			return NULL;
		//打开文件
		std::ofstream file("log.txt", std::ios::out | std::ios::app);
		if (!file)
		{
			std::cout << "can't open file" << std::endl;
			exit(1);
		}

		file << "fbxtoivestart\n";// << std::endl;
		//初始化节点指针与ID 双向bimap
		GetFBXIdVisitor visitor;
		if (nodeList.size() == 1)
		{
			osg::Group* pGroup = nodeList[0]->asGroup();
			if (pGroup)
			{
				if (pGroup->getNumChildren() == 1)
				{
					osg::Group* pGroup2 = (pGroup->getChild(0))->asGroup();
					if (pGroup2)
					{
						//osg::Node* pNode = pGroup2->getChild(0);
						int iSize = pGroup2->getNumChildren();
						for (int i = 0; i < iSize; i++)
						{
							osg::Node* pNode = pGroup2->getChild(i);

							std::string revitName("0");
							std::string  entName;
							if (pNode->asGroup())
							{
								revitName = pNode->getName();
								int iSub = pNode->asGroup()->getNumChildren();
								if (iSub == 0)
									continue;
								for (int j = 0; j < iSub; j++)
								{
									osg::Node* pChildNode = pNode->asGroup()->getChild(j);
									entName = pChildNode->getName();
									std::string strKey = revitName + "&" + entName;
									if ((revitName == entName))
									{
										strKey = "0&" + entName;
									}
									IdsToUuidBimaps::left_const_iterator pos = m_IdsToUuidBimaps.left.find(strKey);
									if (pos == m_IdsToUuidBimaps.left.end())
									{
										file << strKey << "not attribute" << std::endl;
										continue;
									}
									if (iSub == 1)
									{
										pNode->setName(pos->second);
										m_NodeptrUUIDBimaps.insert(NodeptrUUIDBimaps::value_type(pNode, pos->second));
									}
									else
									{
										pChildNode->setName(pos->second);
										//entName = pChildNode->getName();
										m_NodeptrUUIDBimaps.insert(NodeptrUUIDBimaps::value_type(pChildNode, pos->second));
									}

									//std::string strtemp = pos->second;
									//if (!pChildNode->asGeode())
									//{	
									//	std::string id =pChildNode->asGroup()->getChild(0)->getName();
									//	if (pChildNode->asGroup()->getNumChildren() == 1)
									//		pChildNode->asGroup()->getChild(0)->setName(strtemp);
									//
									//}

								}

							}
							else
							{
								entName = pNode->getName();
								if (pNode->asGroup())
								{
									std::string strKey = revitName + "&" + entName;
								}
								std::string strKey = revitName + "&" + entName;
								IdsToUuidBimaps::left_const_iterator pos = m_IdsToUuidBimaps.left.find(strKey);
								if (pos == m_IdsToUuidBimaps.left.end())
								{
									file << strKey << std::endl;
									continue;
								}
								std::string strtemp = pos->second;
								pNode->setName(pos->second);
								m_NodeptrUUIDBimaps.insert(NodeptrUUIDBimaps::value_type(pNode, pos->second));
							}
						}
					}
					//pGroup2->accept(visitor);
				}
			}
			int iChild = pGroup->getNumChildren();
		}
		file << "fbxtoiveend\n";
		file.close();
		// nodeList[0]->accept(visitor);
		return nodeList[0];


		osg::Group* group = new osg::Group;
		group->setName("root");
		for (int i = 0; i < nodeList.size(); ++i)
			group->addChild(nodeList[i]);
		//group->accept(visitor);
		return group;
	}
	void ModelViewerImpl::writeNode(osg::Node *node, const char *file)
	{
		osg::ref_ptr<osgDB::Options> options(new osgDB::Options);
		options->setOptionString("compressed");
		options->setOptionString("noTexturesInIVEFile");
		//options->setOptionString("compressImageData");
		//options->setOptionString("noWriteExternalReferenceFiles");
		//std::string strClassname(osgDB::Registry::instance()->className());

		osgDB::ReaderWriter::WriteResult result = osgDB::Registry::instance()->writeNode(*node, file, options);
		int i = options->getPrecisionHint();

		if (result.success())
		{
			osg::notify(osg::NOTICE) << "Data written to '" << file << "'." << std::endl;
		}
		else if (result.message().empty())
		{
			osg::notify(osg::NOTICE) << "Warning: file write to '" << file << "' not supported." << std::endl;
		}
		else
		{
			osg::notify(osg::NOTICE) << result.message() << std::endl;
		}
	}

	void ModelViewerImpl::loadParam(const char **files, int fileNum)
	{
		m_UuidAttrHash.clear();
		m_IdsToUuidBimaps.clear();


		m_IdsToUuidBimaps.clear();
		m_newId = 0;
		for (int i = 0; i < fileNum; ++i)
		{
			std::cout << "read param file : " << files[i] << std::endl;

			namespace loc = boost::locale;
			std::ifstream in(files[i]);
			typedef boost::property_tree::ptree PTree;
			PTree pt;
			boost::property_tree::read_json(in, pt);
			in.close();
#ifdef YIZHU_BIMWORLD_USE_WCHAR
			std::string encoding = "utf-8";
#endif
			size_t count = pt.size();
			for (PTree::iterator begin = pt.begin(); begin != pt.end(); ++begin)
			{

				std::string	strId;
				std::string	strRevitId;
				std::string	strUUId;

#ifdef YIZHU_BIMWORLD_USE_WCHAR
				std::wstring ws1 = boost::locale::conv::to_utf<wchar_t>(begin->first, encoding);
#endif
				AttrConstruct construct;
				construct.id = -1;
				for (PTree::iterator begin1 = begin->second.begin(); begin1 != begin->second.end(); ++begin1)
				{
					std::string strTemp = begin1->first;
					std::transform(strTemp.begin(), strTemp.end(), strTemp.begin(), tolower);

					if (strTemp == "id")
					{
						assert(begin1->second.empty());
						std::string strTemp = begin1->second.data().c_str();
						strId = strTemp;
						construct.id = std::atoi(begin1->second.data().c_str());
					}
					else if (strTemp == "guid")
					{
						assert(begin1->second.empty());
#ifdef YIZHU_BIMWORLD_USE_WCHAR
						YZString strTemp = boost::locale::conv::to_utf<wchar_t>(begin1->second.data(), encoding);
						construct.guid = boost::locale::conv::to_utf<wchar_t>(begin1->second.data(), encoding);
#else
						construct.guid.swap(begin1->second.data());
#endif
					}
					else if (strTemp == "name")
					{
						assert(begin1->second.empty());
#ifdef YIZHU_BIMWORLD_USE_WCHAR
						YZString strTemp = boost::locale::conv::to_utf<wchar_t>(begin1->second.data(), encoding);
						construct.name = boost::locale::conv::to_utf<wchar_t>(begin1->second.data(), encoding);
#else
						construct.name.swap(begin1->second.data());
#endif
					}
					else if (strTemp == "Properties")
					{
						assert(begin1->second.data().empty());
						for (PTree::iterator begin2 = begin1->second.begin(); begin2 != begin1->second.end(); ++begin2) {
							ConstructParameter parameter;
#ifdef YIZHU_BIMWORLD_USE_WCHAR
							parameter.name = boost::locale::conv::to_utf<wchar_t>(begin2->first, encoding);
							parameter.value = boost::locale::conv::to_utf<wchar_t>(begin2->second.data(), encoding);
#else
							parameter.name = begin2->first;
							parameter.value.swap(begin2->second.data());
#endif
							/*parameter.id = -1;
							for(PTree::iterator begin3 = begin2->second.begin(); begin3 != begin2->second.end();++ begin3){
							if(begin3->first == "id"){
							assert(begin3->second.empty());
							parameter.id = std::atoi(begin3->second.data().c_str());
							}else if(begin3->first == "name"){
							assert(begin3->second.empty());
							#ifdef YIZHU_BIMWORLD_USE_WCHAR
							parameter.name = boost::locale::conv::to_utf<wchar_t>( begin3->second.data(), encoding);
							#else
							parameter.name.swap(begin3->second.data());
							#endif
							}else if(begin3->first == "value"){
							assert(begin3->second.empty());
							#ifdef YIZHU_BIMWORLD_USE_WCHAR
							parameter.value = boost::locale::conv::to_utf<wchar_t>( begin3->second.data(), encoding);
							#else
							parameter.value.swap(begin3->second.data());
							#endif
							}else{
							#ifdef YIZHU_BIMWORLD_USE_WCHAR
							std::wstring ws2 = boost::locale::conv::to_utf<wchar_t>( begin3->first, encoding);
							#endif
							assert(false);
							}
							#ifdef YIZHU_BIMWORLD_USE_WCHAR
							parameter.name = boost::locale::conv::to_utf<wchar_t>( begin3->first.data(), encoding);
							#else
							parameter.name.swap(begin3->second.data());
							#endif
							}*/
							//if(parameter.id != -1)

							construct.parameters.push_back(parameter);
						}
					}
					else if (strTemp == "category")
					{

						ConstructParameter parameter;
#ifdef YIZHU_BIMWORLD_USE_WCHAR
						parameter.name = boost::locale::conv::to_utf<wchar_t>(begin1->first, encoding);
						YZString strTemp(_T("\\"));
						parameter.value = boost::locale::conv::to_utf<wchar_t>(begin1->second.data(), encoding) + strTemp + construct.name;
#else
						YZString strTemp("\\");
						parameter.name = begin1->first;
						YZString temp = begin1->second.data() + strTemp + construct.name;
						parameter.value.swap(temp);
#endif
						construct.parameters.push_back(parameter);
						//assert(false);

					}
					else
					{
						ConstructParameter parameter;
#ifdef YIZHU_BIMWORLD_USE_WCHAR
						parameter.name = boost::locale::conv::to_utf<wchar_t>(begin1->first, encoding);
						parameter.value = boost::locale::conv::to_utf<wchar_t>(begin1->second.data(), encoding);

						std::string strKey = begin1->first;
						std::string strValue = begin1->second.data().c_str();
						std::transform(strKey.begin(), strKey.end(), strKey.begin(), tolower);
						if (strKey == "linkrvtid")
						{
							strRevitId = strValue;

						}
						if (strKey == "uniqueid")
						{
							strUUId = strValue;
						}
#endif
						construct.parameters.push_back(parameter);
					}
					//else
					//{
					//	auto begin1_first = begin1->first;
					//	assert(false);
					//}
				}

				if (!construct.id.empty())
				{


					if (m_UuidAttrHash.find(construct.id) != m_UuidAttrHash.end())
					{
						std::cout << "duplicate id in param files : " << construct.id << std::endl;
					}
					m_UuidAttrHash[construct.id] = construct;
					//if (construct.id > m_newId)
					//	m_newId = construct.id;
					m_UuidAttrHash[strUUId] = construct;
					m_IdsToUuidBimaps.insert(IdsToUuidBimaps::value_type(strRevitId + "&" + strId, strUUId));
					int isize = m_IdsToUuidBimaps.size();
				}

			}
		}

	}

	osg::ref_ptr<osg::Group> ModelViewerImpl::reorganize()
	{
		//
		//		//typedef boost::unordered_map<Category, osg::Group*, IdHash<Category> > CategoryGroup;
		//		typedef std::map<CategoryName, osg::ref_ptr<osg::Group> >		CategoryGroup;	// <模型按照分类映射>
		//		typedef std::pair<osg::ref_ptr<osg::Group>, CategoryGroup>		FloorGroup;		// <楼层分类映射>
		//		//typedef boost::unordered_map<Floor, FloorGroup, IdHash<Floor> > FloorToNodes;
		//		typedef std::map<YZString, FloorGroup>							FloorToNodes;   // <模型根节点分层map>
		//		FloorToNodes													floorToNodes;
		//		//NodeptrIdBimaps::left_const_iterator begin = m_nodeptrToIdBimaps.left.begin(), end = m_nodeptrToIdBimaps.left.end();
		//		auto map = StoreRelationship().getAllNodePtrIdMap();
		//		auto begin = map.begin();
		//		auto end = map.end();
		//
		//
		//#ifdef YIZHU_BIMWORLD_USE_WCHAR
		//		std::string encoding = "utf-8";
		//#endif
		//		if (!m_UuidAttrHash.empty())
		//		{
		//			Floor		defaultFloor;// <默认楼层>
		//#ifdef YIZHU_BIMWORLD_USE_WCHAR
		//			YZString	strDefault(_T("其他楼层"));
		//			defaultFloor.name = _T("LevelName");
		//			defaultFloor.value = _T("其他楼层");
		//
		//			Category		defaultCategory;
		//			defaultCategory.name = _T("Category");// <默认分类>
		//			defaultCategory.value = _T("其他分类");
		//#else
		//			YZString	strDefault(("其他楼层"));
		//			defaultFloor.name = ("LevelName");
		//			defaultFloor.value = ("其他楼层");
		//
		//			Category		defaultCategory;
		//			defaultCategory.name = ("Category");// <默认分类>
		//			defaultCategory.value = ("其他分类");
		//
		//#endif
		//			//int icount(0);
		//			int ipos = 0;
		//			int floorId = 0;
		//			while (begin != end)
		//			{
		//				ipos++;
		//				std::cout << ipos << std::endl;
		//				// <节点指针>
		//				osg::ref_ptr<osg::Node>		node = begin->first;
		//				const AttrConstruct			*pConstruct = getConstruct(begin->second);
		//				// <楼层分组>
		//				FloorGroup					*floorGroup = NULL;
		//				// <大类分组>
		//				osg::ref_ptr<osg::Group>	categoryGroup = NULL;
		//
		//				const Floor					*floor = NULL;
		//				const Category				*category = NULL;
		//				const SystemTypeParameter	*systemType = NULL;
		//
		//				// <没有属性时>
		//				if (pConstruct == NULL)
		//				{
		//					++begin;
		//					continue;
		//					floorGroup = &floorToNodes[defaultFloor.value];
		//					categoryGroup = floorGroup->second[defaultCategory.value];
		//				}
		//				else
		//				{
		//
		//					floor = pConstruct->getFloor();
		//					category = pConstruct->getCategory();
		//					systemType = pConstruct->getSystemType();
		//					floorGroup = (floor == NULL ? &floorToNodes[defaultFloor.name] : &floorToNodes[floor->value]);
		//					// <增加对大小类层级处理>
		//
		//#ifdef YIZHU_BIMWORLD_USE_WCHAR
		//					categoryGroup = (category == NULL ? floorGroup->second[defaultCategory.name] : floorGroup->second[category->value]);
		//#else
		//					categoryGroup = (category == NULL ? floorGroup->second[defaultCategory.name] : (floorGroup->second[category->value]));
		//#endif
		//				}
		//
		//				if (floor == NULL)
		//					floor = &defaultFloor;
		//				if (category == NULL)
		//					category = &defaultCategory;
		//
		//				if (floorGroup->first == NULL)
		//				{
		//					floorGroup->first = new osg::Group();
		//
		//#ifdef YIZHU_BIMWORLD_USE_WCHAR
		//					//YZString strflr(_T("floor:"));
		//					floorGroup->first->setName(boost::locale::conv::from_utf((floor->value).c_str(), encoding));
		//					//floorGroup->first->setUserValue("floor", floorId);//reinterpret_cast<int>(floor));//->id);
		//					//++floorId;
		//#else
		//					floorGroup->first->setName(floor->value);
		//#endif
		//				}
		//
		//				//int index(-1);
		//				//floorGroup->first->getUserValue("floor", floorId);
		//
		//				if (categoryGroup == NULL)
		//				{
		//
		//					categoryGroup = new osg::Group();
		//					floorGroup->first->addChild(categoryGroup);
		//					//(*categoryGroup)->setUserValue("category", reinterpret_cast<int>(category));//->id);
		//					//m_categoryGroups[std::pair<CategoryName, SystemType>(category->value, systemType->value)].push_back(categoryGroup);
		//
		//
		//					//// add Group* and category systemType relationship.
		//
		//					if (category != NULL)
		//					{
		//						std::string name;
		//#ifdef YIZHU_BIMWORLD_USE_WCHAR
		//						name = boost::locale::conv::from_utf(category->value.c_str(), encoding);
		//						name += "$";//boost::locale::conv::from_utf(_T("\\"), encoding);
		//						name += boost::locale::conv::from_utf(systemType->value.c_str(), encoding);
		//#else
		//						name = category->value;
		//						name += "$" + systemType->value;
		//#endif
		//						(categoryGroup)->setName(name);
		//					}
		//				}
		//				(categoryGroup)->addChild(node);
		//				++begin;
		//			}
		//
		//			if (!floorToNodes.empty())
		//			{
		//
		//				osg::ref_ptr<osg::Group> root = new osg::Group();
		//				root->setName("root");
		//				FloorToNodes::iterator begin = floorToNodes.begin(), end = floorToNodes.end();
		//				int flrIndex(0);
		//				while (begin != end)
		//				{
		//
		//					begin->second.first->setUserValue("floor", flrIndex++);
		//					root->addChild(begin->second.first);
		//
		//					++begin;
		//				}
		//				updateMaterials();
		//				return root;
		//
		//			}
		//
		//		}
		//
		return NULL;
	}

	osg::ref_ptr<osg::Group> ModelViewerImpl::reorganizeFbx()
	{

		//typedef boost::unordered_map<Category, osg::Group*, IdHash<Category> > CategoryGroup;
		typedef std::map<CategoryName, osg::ref_ptr<osg::Group> >		CategoryGroup;	// <模型按照分类映射>
		typedef std::pair<osg::ref_ptr<osg::Group>, CategoryGroup>		FloorGroup;		// <楼层分类映射>
		//typedef boost::unordered_map<Floor, FloorGroup, IdHash<Floor> > FloorToNodes;
		typedef std::map<YZString, FloorGroup>							FloorToNodes;   // <模型根节点分层map>
		FloorToNodes													floorToNodes;


		NodeptrUUIDBimaps::left_const_iterator begin = m_NodeptrUUIDBimaps.left.begin(), end = m_NodeptrUUIDBimaps.left.end();

#ifdef YIZHU_BIMWORLD_USE_WCHAR
		std::string encoding = "utf-8";
#endif
		if (!m_UuidAttrHash.empty())
		{
			Floor		defaultFloor;// <默认楼层>
#ifdef YIZHU_BIMWORLD_USE_WCHAR
			YZString	strDefault(_T("其他楼层"));
			defaultFloor.name = _T("LevelName");
			defaultFloor.value = _T("其他楼层");

			Category		defaultCategory;
			defaultCategory.name = _T("Category");// <默认分类>
			defaultCategory.value = _T("其他分类");
#else
			YZString	strDefault(("其他楼层"));
			defaultFloor.name = ("LevelName");
			defaultFloor.value = ("其他楼层");

			Category		defaultCategory;
			defaultCategory.name = ("Category");// <默认分类>
			defaultCategory.value = ("其他分类");

#endif
			//int icount(0);
			int ipos = 0;
			int floorId = 0;
			while (begin != end)
			{
				ipos++;
				std::cout << ipos << std::endl;
				// <节点指针>
				osg::ref_ptr<osg::Node>		node = begin->first;
				std::string strName = node->getName();
				const AttrConstruct			*pConstruct = getConstruct(begin->second);
				// <楼层分组>
				FloorGroup					*floorGroup = NULL;
				// <大类分组>
				osg::ref_ptr<osg::Group>	categoryGroup = NULL;

				const Floor					*floor = NULL;
				const Category				*category = NULL;
				const SystemTypeParameter	*systemType = NULL;

				// <没有属性时>
				if (pConstruct == NULL)
				{
					++begin;
					continue;
					floorGroup = &floorToNodes[defaultFloor.value];
					categoryGroup = floorGroup->second[defaultCategory.value];
				}
				else
				{

					floor = pConstruct->getFloor();
					category = pConstruct->getCategory();
					systemType = pConstruct->getSystemType();
					floorGroup = (floor == NULL ? &floorToNodes[defaultFloor.name] : &floorToNodes[floor->value]);
					// <增加对大小类层级处理>

#ifdef YIZHU_BIMWORLD_USE_WCHAR
					categoryGroup = (category == NULL ? floorGroup->second[defaultCategory.name] : floorGroup->second[category->value]);
#else
					categoryGroup = (category == NULL ? floorGroup->second[defaultCategory.name] : (floorGroup->second[category->value]));
#endif
				}

				if (floor == NULL)
					floor = &defaultFloor;
				if (category == NULL)
					category = &defaultCategory;

				if (floorGroup->first == NULL)
				{
					floorGroup->first = new osg::Group();

#ifdef YIZHU_BIMWORLD_USE_WCHAR
					floorGroup->first->setName(boost::locale::conv::from_utf((floor->value).c_str(), encoding));

#else
					floorGroup->first->setName(floor->value);
#endif
				}

				//int index(-1);
				//floorGroup->first->getUserValue("floor", floorId);

				if (categoryGroup == NULL)
				{

					categoryGroup = new osg::Group();
					floorGroup->first->addChild(categoryGroup);
					//(*categoryGroup)->setUserValue("category", reinterpret_cast<int>(category));//->id);
					//m_categoryGroups[std::pair<CategoryName, SystemType>(category->value, systemType->value)].push_back(categoryGroup);

					//m_constructMaterialSet->addConstructGroup(category->value, systemType->value, categoryGroup);

					//// add Group* and category systemType relationship.
					//m_constructMaterialSet->setGroupConstruct(categoryGroup, category->value, systemType->value);

					if (category != NULL)
					{
						std::string name;
#ifdef YIZHU_BIMWORLD_USE_WCHAR
						name = boost::locale::conv::from_utf(category->value.c_str(), encoding);
						name += "$";//boost::locale::conv::from_utf(_T("\\"), encoding);
						name += boost::locale::conv::from_utf(systemType->value.c_str(), encoding);
#else
						name = category->value;
						name += "$" + systemType->value;
#endif
						(categoryGroup)->setName(name);
					}
				}
				(categoryGroup)->addChild(node);
				++begin;
			}

			if (!floorToNodes.empty())
			{

				osg::ref_ptr<osg::Group> root = new osg::Group();
				root->setName("root");
				FloorToNodes::iterator begin = floorToNodes.begin(), end = floorToNodes.end();
				int flrIndex(0);
				while (begin != end)
				{

					begin->second.first->setUserValue("floor", flrIndex++);
					root->addChild(begin->second.first);

					++begin;
				}
				//updateMaterials();
				return root;

			}

		}

		return NULL;
	}
	osg::Node* ModelViewerImpl::readIveNode(const char *files, int fileNum)
	{
		typedef std::vector<osg::ref_ptr<osg::Node> > NodeList;
		NodeList nodeList;
		osg::ref_ptr<osg::Node> node;

		node = readNode(files);
		std::string entName = node->getName();
		if (node->asGroup())
		{
			osg::Group* pGroup2 = ((node->asGroup())->getChild(0))->asGroup();
			int iSub = pGroup2->getNumChildren();
			entName = pGroup2->getName();
#ifdef YIZHU_BIMWORLD_USE_WCHAR
			YZString strFlr = boost::locale::conv::to_utf<wchar_t>(entName, "utf-8");
#else
			YZString strFlr = entName;
#endif
			YZString strname;
			for (int j = 0; j < iSub; j++)
			{
				osg::Node* pChildNode = pGroup2->getChild(j);
				entName = pChildNode->getName();
				int iChild(0);
				if (pChildNode->asGroup())
				{
					iChild = (pChildNode->asGroup())->getNumChildren();

#ifdef YIZHU_BIMWORLD_USE_WCHAR
					YZString strId = boost::locale::conv::to_utf<wchar_t>((pChildNode->asGroup())->getChild(0)->getName(), "utf-8");
#else
					YZString strId = (pChildNode->asGroup())->getChild(0)->getName();
#endif
					if ((pChildNode->asGroup())->getChild(0)->asGeode())
					{
#ifdef YIZHU_BIMWORLD_USE_WCHAR
						strId = boost::locale::conv::to_utf<wchar_t>((pChildNode->asGroup())->getChild(0)->getName(), "utf-8");
#else
						strId = (pChildNode->asGroup())->getChild(0)->getName();
#endif
					}

				}
#ifdef YIZHU_BIMWORLD_USE_WCHAR
				YZString strname = boost::locale::conv::to_utf<wchar_t>(entName, "utf-8");
#else
				YZString strname = entName;
#endif
			}

		}
		return node.release();

	}

	void ModelViewerImpl::preFrameUpdate()
	{
		// Due any previous frame updates in this routine
		if (m_cameraAnimator->isAnimating())
		{
			auto cw = m_cameraAnimator->getCameraViewStruct();
			if (cw)
			{
				m_mViewer->getCameraManipulator()->setHomePosition(cw->eye, cw->center, cw->up);
				m_mViewer->home();
			}
		}
	}

	void ModelViewerImpl::postFrameUpdate()
	{
		// Due any post frame updates in this routine
	}

	void ModelViewerImpl::startRendering()
	{
		if (m_rendering == 0)
		{
			m_rendering = new CRenderingThread(this);
			m_rendering->start();
		}
	}

	std::string  ModelViewerImpl::findIdByNode(osg::Node *node) const
	{
		return node->getName();
		//auto iter = m_nodeIdptr.find(node);
		//if (iter == m_nodeIdptr.end())
		//{
		//	return "";
		//}
		//return iter->second;

		//NodeptrIdBimaps::left_const_iterator pos = m_nodeptrToIdBimaps.left.find(node);
		//return StoreRelationship().getNodeId(node);
	}

	osg::Node* ModelViewerImpl::findNodeById(const std::string& id) const
	{
		auto iter = m_idNodeptr.find(id);
		if (iter == m_idNodeptr.end())
		{
			return NULL;
		}
		return iter->second;
		//NodeptrIdBimaps::right_const_iterator pos = m_nodeptrToIdBimaps.right.find(id);
		//return StoreRelationship().getNodePtr(id);

	}

	void ModelViewerImpl::findNodesByIds(const std::string *ids, int n, std::vector<void*>& nodes)
	{
		for (int i = 0; i < n; ++i) {
			osg::Node *node = findNodeById(ids[i]);
			if (node)
				nodes.push_back(node);
		}
	}


	const AttrConstruct *ModelViewerImpl::getConstruct(std::string strUuid) const
	{
		UuidAttrHash::const_iterator pos = m_UuidAttrHash.find(strUuid);
		if (pos != m_UuidAttrHash.end())
			return &pos->second;
		return NULL;
	}


	void ModelViewerImpl::setViewerMode(ViewerMode viewerMode)
	{
		BIMCameraManipulator *manipulator = dynamic_cast<BIMCameraManipulator*>(getCameraManipulator());
		if (manipulator != NULL)
		{
			manipulator->setMode(viewerMode);
			m_cameraManipulatorMode = viewerMode;
		}
		clearSelection();
		updateSeveralTimes(1);
	}

	void ModelViewerImpl::frame(const bool isRenderingThread)
	{
		OpenThreads::ScopedLock<OpenThreads::Mutex> lock(m_mutex);
		if (!isRenderingThread)
		{
			setIsExternalRendering(true);
		}

		m_babortRendering = false;
		m_mViewer->frame();

		if (!isRenderingThread)
			setIsExternalRendering(false);
	}

	osg::Camera*	ModelViewerImpl::getWindowSpace()
	{
		return m_windowSpace;
	}

	static osg::Image* RGB2RGBA(osg::Image* image_rgb, double transparency)
	{
		if (image_rgb->getPixelFormat() == GL_RGBA)
			return image_rgb;
		osg::Image * image_rgba = new osg::Image;

		int height = image_rgb->s();
		int width = image_rgb->t();
		int length = image_rgb->r();
		const long size = width * height * length;

		unsigned char* rgb_data = image_rgb->data();
		unsigned char* rgba_data = (unsigned char*)calloc(size * 4, sizeof(unsigned char));
		unsigned char val = transparency * 255;
		for (long i = 0; i < size; ++i) {
			rgba_data[i * 4 + 0] = rgb_data[i * 3 + 0]; //red
			rgba_data[i * 4 + 1] = rgb_data[i * 3 + 1]; //green
			rgba_data[i * 4 + 2] = rgb_data[i * 3 + 2]; //blue
			rgba_data[i * 4 + 3] = val; //alpha
		}

		image_rgba->setImage(width, height, length, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE,
			rgba_data, osg::Image::USE_MALLOC_FREE);
		return image_rgba;
	}

	static osg::StateSet *imageFromFile(const char *imageFile)
	{
		osg::Texture2D* texture = new osg::Texture2D;
		osg::ref_ptr<osg::Image> image = osgDB::readImageFile(imageFile);
		osg::TexEnv *texEnv = new osg::TexEnv();
		osg::Material *mat = new osg::Material;

		double transparency = 0.1;
		texEnv->setMode(osg::TexEnv::DECAL);
		if (image != NULL)
			image = RGB2RGBA(image, transparency);
		texture->setDataVariance(osg::Object::DYNAMIC); // protect from being optimized away as static state.
		texture->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR);
		texture->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);
		texture->setWrap(osg::Texture::WRAP_R, osg::Texture::REPEAT);
		texture->setResizeNonPowerOfTwoHint(false);
		texture->setImage(image);
		mat->setTransparency(osg::Material::FRONT_AND_BACK, transparency);

		osg::StateSet* ss = new osg::StateSet();
		ss->setMode(GL_BLEND, osg::StateAttribute::ON);
		ss->setTextureAttributeAndModes(0, texture, osg::StateAttribute::ON);
		ss->setTextureAttributeAndModes(0, texEnv, osg::StateAttribute::ON);
		ss->setAttribute(mat, osg::StateAttribute::PROTECTED);
		ss->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
		return ss;
	}

	void ModelViewerImpl::insertImage(double x, double y, double z, const char *imageFile)
	{
		osg::Geometry* geom = new osg::Geometry;
		osg::Vec3 top_left(-0.1f, 0.1f, 0.0f);
		osg::Vec3 bottom_left(-0.1f, -0.1f, 0.0f);
		osg::Vec3 bottom_right(0.1f, -0.1f, 0.0f);
		osg::Vec3 top_right(0.1f, 0.1f, 0.0f);
		osg::Vec3Array* vertices = new osg::Vec3Array(4);
		(*vertices)[0] = top_left;
		(*vertices)[1] = bottom_left;
		(*vertices)[2] = bottom_right;
		(*vertices)[3] = top_right;
		geom->setVertexArray(vertices);

		osg::Vec2Array* texcoords = new osg::Vec2Array(4);
		(*texcoords)[0].set(0.0f, 1.0f);
		(*texcoords)[1].set(0.0f, 0.0f);
		(*texcoords)[2].set(1.0f, 0.0f);
		(*texcoords)[3].set(1.0f, 1.0f);
		geom->setTexCoordArray(0, texcoords);

		osg::Vec3Array* normals = new osg::Vec3Array(1);
		(*normals)[0].set(0.0f, 0.0f, 1.0f);
		geom->setNormalArray(normals, osg::Array::BIND_OVERALL);

		osg::Vec4Array* colors = new osg::Vec4Array(1);
		(*colors)[0].set(1.0f, 1.0f, 1.0f, 1.0f);
		geom->setColorArray(colors, osg::Array::BIND_OVERALL);

		geom->addPrimitiveSet(new osg::DrawArrays(GL_QUADS, 0, 4));
		geom->setStateSet(imageFromFile(imageFile));

		osg::Geode* geom_geode = new osg::Geode;
		geom_geode->addDrawable(geom);

		/*osg::Vec4 layoutColor(1.0f,1.0f,0.0f,1.0f);
		float layoutCharacterSize = 20.0f;

		{
		osgText::Font* font = osgText::readFontFile("fonts/arial.ttf");
		osgText::Text* text = new osgText::Text;
		text->setFont(font);
		text->setColor(layoutColor);
		text->setCharacterSize(layoutCharacterSize);
		text->setPosition(osg::Vec3(0.0, 0.0, 0.0f));

		// the default layout is left to right, typically used in languages
		// originating from europe such as English, French, German, Spanish etc..
		text->setLayout(osgText::Text::LEFT_TO_RIGHT);

		text->setText("text->setLayout(osgText::Text::LEFT_TO_RIGHT);");
		geom_geode->addDrawable(text);
		}*/

		this->getWindowSpace()->addChild(geom_geode);
	}

	osg::StateSet* ModelViewerImpl::createStateSetByMaterial(osg::Material* material)
	{
		auto state = new osg::StateSet();
		state->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
		state->setMode(GL_BLEND, osg::StateAttribute::ON);
		state->setAttribute(getBlendFunc());

		state->setAttribute(material, osg::StateAttribute::PROTECTED);
		return state;
	}

	void ModelViewerImpl::setCategoryMaterial(const std::vector<std::string>& ids, const MaterialSetting& materialSetting)
	{
		MaterialState /*newMat, */*material;
		material = NULL;
		//std::vector<osg::Node*> nodes;
		StoreRelationship store;
		for (int i = 0; i < ids.size(); i++)
		{
			auto node = findNodeById(ids[i]);
			if (!node)
			{
				osg::notify(osg::FATAL) << "can't find specified node, it could cause no material." << std::endl;
				continue;
			}
			//nodes.push_back(node);
			auto stateSet = createStateSetByMaterial(constructMaterial(materialSetting));
			node->setStateSet(stateSet);
			node->setUserData(stateSet);
		}
		//if (nodes.size() < 1)
		//{
		//	osg::notify(osg::FATAL) << "Specified child node is empty, it is useless material." << std::endl;
		//	return;
		//}
		//auto subMatNum = store.getMaterialId(nodes[0]);
		//bool hasMaterial;
		//if (subMatNum < 1)
		//{
		//	hasMaterial = false;
		//}
		//else
		//{
		//	hasMaterial = store.getMaterialState(subMatNum, material);
		//}
		//if (!hasMaterial)
		//{
		//	//material = &newMat;
		//	material = new MaterialState();
		//}
		////std::pair<osg::ref_ptr<osg::Material>, osg::ref_ptr<osg::StateSet> > &material = m_categoryMaterial[std::pair<CategoryName, SystemType>(categoryName, systemType)];
		//material->first = constructMaterial(materialSetting);
		//if (material->second == NULL)
		//{
		//	material->second = new osg::StateSet();
		//	material->second->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
		//	material->second->setMode(GL_BLEND, osg::StateAttribute::ON);
		//	material->second->setAttribute(getBlendFunc());
		//}
		//material->second->setAttribute(material->first, osg::StateAttribute::PROTECTED);
		////setCategoryMaterial(categoryName, systemType, levelName, subMatNum, material->second.get());
		//if (!hasMaterial)
		//	store.setConstructMaterialState(subMatNum, material, nodes);
		updateSeveralTimes(1);
	}

	void ModelViewerImpl::setCategoryMaterial(const CategoryName& categoryName, const SystemType& systemType, const LevelName& levelName, const std::vector<std::string>& ids, const MaterialSetting& materialSetting)
	{
		MaterialState /*newMat, */*material;
		material = NULL;
		std::vector<osg::Node*> nodes;
		StoreRelationship store;
		for (int i = 0; i < ids.size(); i++)
		{
			auto node = findNodeById(ids[i]);
			if (!node)
			{
				osg::notify(osg::FATAL) << "can't find specified node, it could cause no material." << std::endl;
				continue;
			}
			auto state = createStateSetByMaterial(constructMaterial(materialSetting));
			if (!state)
			{
				continue;
			}
			node->setStateSet(state);
			node->setUserData(state);
		}
	}

	void ModelViewerImpl::setCategoryAndSystemTypes(const CategoryName & categoryName, const SystemType & systemType)
	{
		//m_constructMaterialSet->addCategorySystemType(categoryName, systemType);
		//m_constructMaterialSet->addSystemTypeCategory(systemType, categoryName);
	}

	//osg::Material* ModelViewerImpl::getCategoryMaterial(const CategoryName& categoryName, const SystemType& systemType, const LevelName& levelName, int subMatNum) const
	//{
	//	MaterialState *materialState = NULL;
	//	if (!StoreRelationship().getMaterialState(subMatNum, materialState))
	//		return NULL;
	//	return materialState->first.get();
	//}
	/*

		void ModelViewerImpl::updateCategoryMaterial(const CategoryName& categoryName, const SystemType& systemType, const LevelName& levelName, int subMaterialNum)
		{
		MaterialState* materialState = NULL;
		if (StoreRelationship().getMaterialState(subMaterialNum, materialState))
		{
		setCategoryMaterial(categoryName, systemType, levelName, subMaterialNum, materialState->second.get());
		}
		updateSeveralTimes(1);
		}

		void ModelViewerImpl::setCategoryMaterial(const CategoryName& categoryName, const SystemType& systemType, const LevelName& levelName, int subMaterialNum, osg::StateSet* material)
		{
		NodeCollection nodes;
		if (!StoreRelationship().getNodesByMaterial(subMaterialNum, nodes))
		{
		osg::notify(osg::DEBUG_INFO) << "get groups failed. in ModelViewerImpl::setCategoryMaterial." << std::endl;
		return;
		}
		auto start = nodes.begin();
		auto end = nodes.end();

		while (start != end)
		{
		(*start)->setStateSet(material);
		++start;
		}
		updateSeveralTimes(1);
		}*/

	void ModelViewerImpl::updateMaterials()
	{
		//boost::mutex _mutex;
		//_mutex.lock();
		//std::vector<osg::Group*> groups;
		//StoreRelationship store;
		//store.getAllGroups(groups);
		//for (int i = 0; i < groups.size(); i++)
		//{
		//	if (!groups[i])
		//		continue;
		//	groups[i]->setStateSet(NULL);
		//	NodeCollection nodes;
		//	if (store.getNodes(groups[i], nodes))
		//	{
		//		for (int j = 0; j < nodes.size(); j++)
		//		{
		//			if (!nodes[j])
		//				continue;
		//			if (!nodes[j])
		//				continue;
		//			//int matId;
		//			//if (!store.getConstructMatKey(nodes[j], matId))
		//			//{
		//			//	osg::notify(osg::FATAL) << "The node doesn't exist in material set. Maybe not in color.xml. in ModelViewerImpl::updateMaterials." << std::endl;
		//			//	nodes[j]->setStateSet(NULL);
		//			//	continue;
		//			//}
		//			//MaterialState* matState = NULL;
		//			//if (!store.getMaterialState(matId, matState))
		//			//{
		//			//	osg::notify(osg::FATAL) << "The node doesn't exist in material set. Maybe not in color.xml. in ModelViewerImpl::unHighlight." << std::endl;
		//			//	nodes[j]->setStateSet(NULL);
		//			//	continue;
		//			//}
		//			osg::StateSet* state = NULL;
		//			try
		//			{
		//				auto state = dynamic_cast<osg::StateSet*>(nodes[j]->getUserData());
		//			}
		//			catch (...)
		//			{
		//				state = NULL;
		//			}
		//			nodes[j]->setStateSet(state);
		//		}
		//	}
		//}


		//_mutex.unlock();
		//updateSeveralTimes(1);

	}

	void ModelViewerImpl::setCurrentDBPath(const std::string& path)
	{
		m_currentPrjDBPath = path;
	}

	void ModelViewerImpl::initializeCategoryGroups(osg::Node *root)
	{
		osg::Group *rootGroup = root->asGroup();
        if (!rootGroup) {
            return;
        }
		StoreRelationship store;
		//std::string insertSql;
		//std::map<std::string, osg::Node*> idNptr;
		m_idNodeptr.clear();
		//store.begin();
		if (rootGroup)
		{
			int numChild = rootGroup->getNumChildren();
			while (numChild == 1)
			{
				auto n = rootGroup->getChild(0);
				if (n->getName() == "root")
				{
					rootGroup = n->asGroup();
					if (!rootGroup)
					{
						return;
					}
					numChild = rootGroup->getNumChildren();
				}
				else
				{
					break;
				}
			}

			for (int i = 0; i < numChild; ++i)
			{
				osg::Group * floorGroup = dynamic_cast<osg::Group*>(rootGroup->getChild(i));
				if (floorGroup != NULL)
				{
					int numChild = floorGroup->getNumChildren();
					std::string floorName = floorGroup->getName();
					//					for (int i = 0; i < numChild; ++i)
					//					{
					//						osg::Group *categoryGroup = dynamic_cast<osg::Group*>(floorGroup->getChild(i));
					//						if (categoryGroup != NULL)
					//						{
					//							CategoryName categoryName;
					//							SystemType systemType;
					//							LevelName levelName;
					//							std::string name = categoryGroup->getName();
					//							std::vector<std::string> outResult;
					//							bool splitSuccess = util::StringExtension::split(name, "$", outResult);
					//#ifdef YIZHU_BIMWORLD_USE_WCHAR
					//							if (splitSuccess)
					//							{
					//								categoryName = boost::locale::conv::to_utf<wchar_t>(outResult[0], "utf-8");
					//							}
					//							else
					//							{
					//								categoryName = boost::locale::conv::to_utf<wchar_t>(name, "utf-8");
					//							}
					//							levelName = boost::locale::conv::to_utf<wchar_t>(floorName, "utf-8");
					//#else
					//							if (splitSuccess)
					//							{
					//								categoryName = outResult[0];
					//							}
					//							else
					//							{
					//								categoryName = name;
					//							}
					//							levelName = floorName;
					//#endif
					//							if (outResult.size() != 2)
					//							{
					//								store.addConstructGroup(categoryName, systemType, levelName, categoryGroup);
					//								osg::notify(osg::NOTICE) << "there isn't any split string in osg::Node's name. in ModelViewerImpl::initializeCategoryGroup().";
					//								continue;
					//							}
					//
					//#ifdef YIZHU_BIMWORLD_USE_WCHAR
					//							systemType = boost::locale::conv::to_utf<wchar_t>(outResult[1], "utf-8");
					//#else
					//							systemType = outResult[1];
					//#endif
					//
					//							store.addConstructGroup(categoryName, systemType, levelName, categoryGroup);
					//insertSql += store.getAddNodeSql(floorName, floorGroup);
					for (int groupIndex = 0; groupIndex < floorGroup->getNumChildren(); groupIndex++)
					{
						auto category = floorGroup->getChild(groupIndex);
						if (!category)
						{
							continue;
						}
						auto categoryGroup = category->asGroup();
						if (!categoryGroup)
						{
							continue;
						}
						for (int nodeIndex = 0; nodeIndex < categoryGroup->getNumChildren(); nodeIndex++)
						{
							auto node = categoryGroup->getChild(nodeIndex);
							if (!node)
							{
								continue;
							}
							auto id = node->getName();
							m_idNodeptr[id] = node;
							//m_idNodeptr[id] = (int)(util::IntPtr::toVoidPtr(node));
							//insertSql += store.getAddNodeSql(id, node);

						}
					}
					//}
					//}
				}
			}
		}
		//store.execAsOneCommand(insertSql);

		//store.analyzeGroupTable();
		//store.commit();

#ifdef WIN32
		// init material
		std::map<std::string, osg::ref_ptr<osg::Material>> idMat;
		store.getIdMaterialMap(m_currentPrjDBPath, idMat);
		for (auto iter = m_idNodeptr.begin(); iter != m_idNodeptr.end(); iter++)
		{
			auto stateset = createStateSetByMaterial(idMat[iter->first]);
			if (!stateset)
			{
				continue;
			}
			traverseAllSubNodes(iter->second, [stateset](osg::Node* n){
				if (!n->asGeode())
				{
					return true;
				}
				n->setStateSet(stateset);
				n->setUserData(stateset);
				return false;// stop
			}, true);
		}
#endif // _WIN32

	}

	/*
		void ModelViewerImpl::initializeCategoryGroups1(osg::Node *root)
		{
		osg::Group *rootGroup = dynamic_cast<osg::Group*>(root);
		if (rootGroup)
		{
		int numChild = rootGroup->getNumChildren();
		for (int i = 0; i < numChild; ++i)
		{
		osg::Group * floorGroup = dynamic_cast<osg::Group*>(rootGroup->getChild(i));
		if (floorGroup != NULL)
		{
		int numChild = floorGroup->getNumChildren();
		std::string floorName = floorGroup->getName();
		for (int i = 0; i < numChild; ++i)
		{
		osg::Group *categoryGroup = dynamic_cast<osg::Group*>(floorGroup->getChild(i));
		if (categoryGroup != NULL)
		{
		CategoryName categoryName;
		SystemType systemType;
		LevelName levelName;
		std::string name = categoryGroup->getName();
		std::vector<std::string> outResult;
		bool splitSuccess = util::StringExtension::split(name, "$", outResult);
		#ifdef YIZHU_BIMWORLD_USE_WCHAR
		if (splitSuccess)
		{
		categoryName = boost::locale::conv::to_utf<wchar_t>(outResult[0], "utf-8");
		}
		else
		{
		categoryName = boost::locale::conv::to_utf<wchar_t>(name, "utf-8");
		}
		levelName = boost::locale::conv::to_utf<wchar_t>(floorName, "utf-8");
		#else
		if (splitSuccess)
		{
		categoryName = outResult[0];
		}
		else
		{
		categoryName = name;
		}
		levelName = floorName;
		#endif
		if (outResult.size() != 2)
		{
		m_constructMaterialSet->addConstructGroup(categoryName, systemType, levelName, categoryGroup);
		osg::notify(osg::NOTICE) << "there isn't any split string in osg::Node's name. in ModelViewerImpl::initializeCategoryGroup().";
		continue;
		}

		#ifdef YIZHU_BIMWORLD_USE_WCHAR
		systemType = boost::locale::conv::to_utf<wchar_t>(outResult[1], "utf-8");
		#else
		systemType = outResult[1];
		#endif

		m_constructMaterialSet->addConstructGroup(categoryName, systemType, levelName, categoryGroup);

		}
		}
		}
		}
		}
		}*/

#pragma region add billboard
	osg::Image* getImage(const char* file_path_name)
	{
		if (!file_path_name)
			return NULL;
		ReaderWriterPNG* reader = new ReaderWriterPNG();
		osg::Image* image = NULL;
		std::ifstream file(file_path_name, std::ios::in | std::ios::binary | std::ios::ate);
		if (file.is_open())
		{
			file.seekg(0, std::ios::beg);
			auto result = reader->readPNGStream(file);
			if (!result.success())
				return NULL;
			image = result.takeImage();

			file.close();
		}
		return image;
	}

	osg::Node* ModelViewerImpl::createBillboard(osg::Vec3 position, osg::Geometry * geometry, osg::Image * image)
	{
		return BillboardExtension::BillboardFactory::getInstance()->createBillboard(geometry, image, position);
	}

	int ModelViewerImpl::addBillboard(std::string selectedConstruct[], const void(*callback)(BillboardData*, const void*), const char* img_path_name, const void* externalData)
	{

		if (!selectedConstruct)
			return -1;

		// 选择的器件数组长度
		auto length = sizeof(selectedConstruct) / sizeof(selectedConstruct[0]);
		osg::Vec3d eye;
		osg::Vec3d center;
		osg::Vec3d up;
		m_mViewer->getCameraManipulator()->getHomePosition(eye, center, up);
		double min = DBL_MAX;
		osg::Vec3 pos;

		// 对多个模型的中心求平均值，即平均中心作为绘制的位置
		auto tcenter = osg::Vec3(0, 0, 0);
		auto container = new osg::DefaultUserDataContainer();
		for (int i = 0; i < length; i++)
		{
			auto node = findNodeById(selectedConstruct[i]);


			if (node)
				container->addUserObject(node);

		}
		pos = tcenter;

		auto billboard = createBillboard(pos, NULL, getImage(img_path_name));
		if (!billboard)
			return -1;


		billboard->setUserDataContainer(container);
		billboard->addUpdateCallback(new BillboardExtension::UpdateBillboardPosition(m_coordHelper));


		BillboardData* bData = new BillboardData();
		bData->setConstructIds(selectedConstruct);
		m_billboardDataSet.insert(std::pair<osg::ref_ptr<osg::Node>, BillboardData*>(billboard, bData));

		m_billboardExternalDataSet.insert(std::pair<osg::ref_ptr<osg::Node>, const void*>(billboard, externalData));

		int id = m_newId++;

		m_topMostSet[id] = billboard;

		m_topMostCallback.insert(std::pair<osg::ref_ptr<osg::Node>, const void(*)()>(billboard, (const void(*)())(callback)));
		m_topRoot->addChild(billboard);
		updateSeveralTimes(1);
		return id;
	}

	bool ModelViewerImpl::removeBillboard(int id)
	{
		if (id < 1)
			return false;
		auto node = m_topMostSet.find(id);
		if (node == m_topMostSet.end())
			return false;

		if (!m_topRoot->removeChild(node->second))
			return false;
		m_topMostCallback.erase(node->second);
		updateSeveralTimes(1);
		return true;
	}

	bool ModelViewerImpl::isTopMostNode(osg::Node* node)
	{
		if (!node)
			return false;

		return m_topRoot->containsNode(node);
	}

	void ModelViewerImpl::triggerBillboardCallback(osg::Node* node)
	{
		if (!node)
			return;

		const void* eData = NULL;
		auto eIter = m_billboardExternalDataSet.find(node);
		if (eIter != m_billboardExternalDataSet.end())
			eData = eIter->second;

		BillboardData* bData = NULL;

		auto bIter = m_billboardDataSet.find(node);
		if (bIter != m_billboardDataSet.end())
			bData = bIter->second;

		auto fIter = m_topMostCallback.find(node);
		if (m_topMostCallback.end() == fIter)
			return;
		auto func = (const void(*)(BillboardData*, const void*))(fIter->second);
		if (func)
			func(bData, eData);
	}

#pragma endregion


	ModelViewerImpl::ModelViewerImpl() :
		m_rendering(0),
		m_modelViewerListeners(defaultModelListener),
		m_mViewer(NULL),
		m_babortRendering(false),
		m_topRoot(NULL),
		m_isLoaded(false),
		m_cameraAnimator(new CameraAnimator(this))
	{
		m_modelRoot = NULL;
		m_animaEditor = new Animation::AnimationEditor();
		m_fixedAnimationPath = new Animation::FixedPathAnimation();

		m_skyBox = new osg::Node();
	}

	ModelViewerImpl::~ModelViewerImpl()
	{
		if (m_mViewer != NULL)
			m_mViewer->setDone(true);
		boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
		if (m_mViewer != NULL)
			m_mViewer->stopThreading();
	}


	const char *ModelViewerImpl::getName(void* node)
	{
		osg::Node* pNode = ((osg::Node*)node);

		if (node)
		{
			return ((osg::Node*)node)->getName().c_str();
		}
		return NULL;
	}

	//根据名称获得节点
	osg::Node* ModelViewerImpl::getNode(bimWorld::YZString nodename)
	{

		//return StoreRelationship::getNodePtr(nodename);
		//NodePtrNameBimaps::iterator pos = m_nodePtrNameBimaps.find(nodename);
		//if (pos != m_nodePtrNameBimaps.end())
		//{
		//	return pos->second;
		//}
		return NULL;
	}

	//获取大类全部节点
	void	ModelViewerImpl::getClassNode(bimWorld::YZString nodename, std::vector<osg::Node*>& nodevec)
	{
		//
		//		NodePtrNameBimaps::iterator posst = m_nodePtrNameBimaps.begin();
		//		NodePtrNameBimaps::iterator posed = m_nodePtrNameBimaps.end();
		//		for (; posst != posed; posst++)
		//		{
		//			YZString strname = posst->first;
		//#ifdef YIZHU_BIMWORLD_USE_WCHAR
		//			int rpos = strname.rfind(_T("\\"));
		//			YZString strflrclass = strname.substr(0, rpos);
		//#else
		//			int rpos = strname.rfind(("\\"));
		//			YZString strflrclass = strname.substr(0, rpos);
		//#endif
		//			if (nodename == strflrclass)
		//			{
		//				nodevec.push_back(posst->second);
		//			}
		//		}
		//
	}

	// <获取分类组节点>
	void	ModelViewerImpl::getCategoryGroups(LevelName level, CategoryName category, std::vector<osg::Group*>& nodevec)
	{
		std::string levelName, categoryName;
#ifdef YIZHU_BIMWORLD_USE_WCHAR
		levelName = boost::locale::conv::from_utf(level.c_str(), "utf-8");
		categoryName = boost::locale::conv::from_utf(category.c_str(), "utf-8");
#else
		levelName = level;
		categoryName = category;
#endif
		traverseSubNodesAndFindFirst(m_modelRoot.get(), [&](osg::Node* child){
			if (child->getName() != levelName)
			{
				return false;
			}
			auto group = child->asGroup();
			if (categoryName.empty())
			{
				nodevec.push_back(group);
				return true;
			}
			for (int i = 0; i < group->getNumChildren(); i++)
			{
				auto node = group->getChild(i);
				if (!node)
				{
					continue;
				}
				auto name = node->getName();
				// <先把系统类型拆分出来>
				std::vector < std::string> sysSplited;
				if (!util::StringExtension::split(name, "$", sysSplited))
					continue;
				if (sysSplited.size() < 1)
					continue;
				// <拆分一下参数>
				std::vector<std::string> paramSplited;
				if (!util::StringExtension::split(categoryName, "\\", paramSplited))
				{
					// <如果只有大类，则拆分一下名称，取名称中的大类与参数进行比较>
					std::vector<std::string> cateSplited;
					if (!util::StringExtension::split(sysSplited[0], "\\", cateSplited))
						continue;
					if (cateSplited.size() < 1)
					{
						continue;
					}

					if (cateSplited[0] != categoryName)
					{
						continue;
					}
				}
				else
				{
					// <如果有大小类， 直接与名称中的大小类进行比较>
					if (sysSplited[0] != categoryName)
					{
						continue;
					}

				}
				nodevec.push_back(node->asGroup());
			}
			return true;
			/*traverseSubNodesAndFindFirst(child, [&](osg::Node* node){
				if (node->getName() != categoryName)
				{
				return false;
				}
				auto group = node->asGroup();
				if (!group)
				{
				return false;
				}
				nodevec.push_back(group);
				return true;
				}, true);*/
		}, true);

		return;

		//ConstructKey key(category, SystemType(), level);
		//if (!StoreRelationship().getGroups(m_currentPrjDBPath, key.topCategory, key.secondaryCategory, key.systemType, key.levelName, nodevec))
		//	return;
		//auto posst = m_constructMaterialSet->getConstructGroupsBegin();
		//auto posed = m_constructMaterialSet->getConstructGroupsEnd();
		//for (; posst != posed; posst++)
		//{
		//	YZString strname = posst->first.topCategory;

		//	if (level == posst->first.levelName && category == strname)
		//	{
		//		for (auto iter = posst->second.begin(); iter != posst->second.end(); ++iter)
		//			nodevec.push_back(*iter);
		//	}
		//}

	}

	// <获取分类全部节点>
	void	ModelViewerImpl::getCategoryNodes(LevelName level, CategoryName category, std::vector<osg::Node*>& nodevec)
	{
		//GroupCollection groups;
		//StoreRelationship store;
		//ConstructKey key(category, SystemType(), level);
		//if (!store.getGroups(key.topCategory, key.secondaryCategory, key.systemType, key.levelName, groups))
		//	return;
		//for (int i = 0; i < groups.size(); i++)
		//	store.getNodes(groups[i], nodevec, false);
		////for (; posst != posed; posst++)
		////{
		////	YZString strname = posst->first.topCategory;

		////	if (level == posst->first.levelName && category == strname)
		////	{
		////		for (auto iter = posst->second.begin(); iter != posst->second.end(); ++iter)
		////			nodevec.push_back(*iter);
		////	}
		////}

	}


	osg::MatrixTransform* ModelViewerImpl::getModelRoot()
	{
		return m_modelRoot.get();
	}

	void ModelViewerImpl::transparentAll()
	{
		InfoVisitor afterVisitor(NULL, 0, getSelectionRenderModel(), true, [this](void* node){setToTransparent((osg::Node*)node); });
		if (m_modelRoot)
			m_modelRoot->accept(afterVisitor);
		updateSeveralTimes(1);
	}

	void ModelViewerImpl::unTransParentAll()
	{
		InfoVisitor afterVisitor(NULL, 0, NULL, false, [this](void* node){unTransparent(node); });
		if (m_modelRoot)
			m_modelRoot->accept(afterVisitor);
	}

	void ModelViewerImpl::traverseChildren(osg::Node* n)
	{
		if (!n)
			return;
		revertColorOfNode(n);
		auto group = n->asGroup();
		if (!group)
		{
			return;
		}
		if (group->getNumChildren() <= 0)
			return;
		int id = 0;
		if (n->asGeode())
			return;
		for (int i = 0; i < group->getNumChildren(); i++)
		{
			traverseChildren(group->getChild(i));
		}
	}

	void ModelViewerImpl::unTransparent(void* ptr)
	{
		auto node = static_cast<osg::Node*>(ptr);
		if (!node)
		{
			osg::notify(osg::WARN) << "The specified node is null. in ModelViewerImpl::unTransparent." << std::endl;
			return;
		}
		traverseChildren(node);
	}

	void ModelViewerImpl::setToTransparent(osg::Node* node)
	{
		InfoVisitor::setTransparency(node);
	}

	void ModelViewerImpl::setOtherNodeToTransparent(void*node)
	{
		osg::Node *aNode = static_cast<osg::Node*>(node);
		if (!aNode)
		{
			return;
		}
		const int length = 1;
		osg::Node ** nodes = new osg::Node*[length];
		nodes[0] = aNode;
		InfoVisitor afterVisitor(nodes, length, getSelectionRenderModel(), true, [this](void* node){unTransparent(node); });
		delete nodes;
		if (m_modelRoot)
			m_modelRoot->accept(afterVisitor);
		updateSeveralTimes(1);
	}

	void ModelViewerImpl::setOtherNodeToTransparent(void** nodes, int length)
	{
		if (!nodes)
			return;
		std::vector<osg::Node*> node_set;
		for (int i = 0; i < length; i++)
		{
			osg::Node *node = static_cast<osg::Node*>(nodes[i]);
			if (!node)
			{
				continue;
			}
			node_set.push_back(node);
		}
		InfoVisitor afterVisitor(node_set.data(), node_set.size(), getSelectionRenderModel(), true, [this](void* node){unTransparent(node); });
		if (m_modelRoot)
			m_modelRoot->accept(afterVisitor);
		updateSeveralTimes(1);
	}

	void ModelViewerImpl::setNodeState(osg::Node* node, osg::Vec4 color)
	{
		if (!node)
			return;
		boost::mutex _lock;
		_lock.lock();

		auto red = color.x();
		auto green = color.y();
		auto blue = color.z();
		auto material = new osg::Material();
		material->setDiffuse(osg::Material::FRONT_AND_BACK, color);
		auto stateSet = new osg::StateSet();
		stateSet->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
		stateSet->setMode(GL_BLEND, osg::StateAttribute::ON);
		_lock.unlock();

		auto blendFunc = getBlendFunc();

		_lock.lock();
		stateSet->setAttribute(blendFunc);
		stateSet->setAttribute(material, osg::StateAttribute::PROTECTED);
		node->setStateSet(stateSet);
		_lock.unlock();
		return;
	}

	void ModelViewerImpl::setStateColorToConstructs(void* ptr, int red, int green, int blue)
	{
		unHighlight(ptr);
		//clearSelection();

		auto node = static_cast<osg::Node*>(ptr);
		if (!node)
			return;
		auto color = osg::Vec4(red / 255.0f, green / 255.0f, blue / 255.0f, 1.0f);
		setNodeState(node, color);

		boost::mutex _lock;
		_lock.lock();

		node->setUserValue(NODE_ADDITIONAL_COLOR0, color);
		_lock.unlock();
		updateSeveralTimes(1);
	}

	bool ModelViewerImpl::setLastAdditionalColor(void* ptr, int port)
	{
		auto node = static_cast<osg::Node*>(ptr);
		if (!node)
			return false;
		osg::Vec4 color;
		const char* key;

		switch (port)
		{
		default:
		case 0:
			key = NODE_ADDITIONAL_COLOR0;
			break;
		case 1:
			key = NODE_ADDITIONAL_COLOR1;
			break;
		case 2:
			key = NODE_ADDITIONAL_COLOR2;
			break;
		}

		boost::mutex _lock;
		_lock.lock();

		bool getUserValueResult = node->getUserValue(key, color);
		_lock.unlock();

		if (!getUserValueResult)
			return false;
		setNodeState(node, color);
		updateSeveralTimes(1);
		return true;
	}

	bool ModelViewerImpl::revertColorOfNode(void* ptr)
	{
		if (!ptr)
		{
			osg::notify(osg::WARN) << "The node is null. in ModelViewerImpl::revertColorOfNode." << std::endl;
			return false;
		}
		auto node = static_cast<osg::Node*>(ptr);
		if (!node)
		{
			osg::notify(osg::WARN) << "The node is null. in ModelViewerImpl::revertColorOfNode." << std::endl;
			return false;
		}

		boost::mutex _lock;

		osg::StateSet* state;
		_lock.lock();
		try
		{
			state = dynamic_cast<osg::StateSet*>(node->getUserData());
		}
		catch (...)
		{
			osg::notify(osg::FATAL) << "The material doesn't exist in material set. Maybe not in color.xml. in ModelViewerImpl::revertColorOfNode." << std::endl;
			state = NULL;
		}
		_lock.unlock();

		traverseAllSubNodes(node, [state](osg::Node* node){
			boost::mutex _lock;
			_lock.lock();
			node->setStateSet(state);
			_lock.unlock();
			return true;
		}, true);

		updateSeveralTimes(1);
		return true;
	}

	void ModelViewerImpl::highlight(void* node)
	{
		if (node)
		{
			auto n = static_cast<osg::Node*>(node);

			traverseAllSubNodes(n, [this](osg::Node* subNode){
				if (subNode->asGeode())
				{
					auto selectionState = getSelectionRenderModel();
					boost::mutex _lock;
					_lock.lock();
					subNode->setStateSet(selectionState);
					_lock.unlock();
					return false;
				}
				return true;
			}, true);

		}
		updateSeveralTimes(1);
	}

	void ModelViewerImpl::unHighlight(void* node)
	{
		revertColorOfNode(node);
	}

	bool ModelViewerImpl::registerModelListener(ModelViewerListener *listener)
	{
		return addToSet(m_modelViewerListeners, listener);
	}

	bool ModelViewerImpl::removeModelListener(ModelViewerListener *listener)
	{
		return removeFromSet(m_modelViewerListeners, listener);
	}

	void ModelViewerImpl::clearSelection()
	{
		boost::mutex _lock;
		_lock.lock();

		for (int i = 0; i < m_modelViewerListeners.size(); i++)
		{
			if (!m_modelViewerListeners[i])
				continue;
			m_modelViewerListeners[i]->clearSelection();
		}
		_lock.unlock();
	}

	osg::StateSet* ModelViewerImpl::getSelectionRenderModel()
	{
		boost::mutex _lock;
		_lock.lock();

		if (m_selectionRenderMode == NULL) {
			double rgba[4] = { 1.0, 0.0, 0.0, 0.6 };

			m_selectionRenderMode = new osg::StateSet();
			osg::ref_ptr<osg::Material> selectionMat = new osg::Material;
			selectionMat->setDiffuse(osg::Material::FRONT_AND_BACK, osg::Vec4(rgba[0], rgba[1], rgba[2], rgba[3]));
			selectionMat->setShininess(osg::Material::FRONT_AND_BACK, 90);
			selectionMat->setAmbient(osg::Material::FRONT_AND_BACK, osg::Vec4(rgba[0], rgba[1], rgba[2], rgba[3]));
			selectionMat->setSpecular(osg::Material::FRONT_AND_BACK, osg::Vec4(rgba[0], rgba[1], rgba[2], rgba[3]));
			m_selectionRenderMode->setAttribute(selectionMat, osg::StateAttribute::PROTECTED);
			//m_selectionRenderMode->setAttribute(new osg::Depth(osg::Depth::LESS, 0, 0, false));
		}
		_lock.unlock();
		return m_selectionRenderMode;
	}

	osg::BlendFunc* ModelViewerImpl::getBlendFunc()
	{
		boost::mutex _lock;
		_lock.lock();

		if (m_blendFunc == NULL)
		{
			m_blendFunc = new osg::BlendFunc();
			m_blendFunc->setFunction(osg::BlendFunc::ALPHAFUNC, osg::BlendFunc::ONE_MINUS_SRC_ALPHA,
				osg::BlendFunc::ALPHAFUNC, osg::BlendFunc::ONE_MINUS_SRC_ALPHA);
		}
		_lock.unlock();
		return m_blendFunc;
	}

	bool ModelViewerImpl::registerDefaultModelListener(ModelViewerListener *listener)
	{
		return addToSet(defaultModelListener, listener);
	}

	bool ModelViewerImpl::removeDefaultModelListener(ModelViewerListener *listener)
	{
		return removeFromSet(defaultModelListener, listener);
	}

	bool ModelViewerImpl::getChildren(osg::Group* node, std::vector<void*>& children)
	{
		osg::ref_ptr<osg::Group> group = node;
		if (group != NULL)
		{
			boost::mutex _lock;
			_lock.lock();

			int numChild = group->getNumChildren();
			for (int i = 0; i < numChild; ++i)
			{
				osg::ref_ptr<osg::Node> pChildrenNode = group->getChild(i);
				if (pChildrenNode->getName() != "")
					children.push_back(pChildrenNode);
			}

			_lock.unlock();
			return true;
		}
		return false;
	}

	osg::Material *constructMaterial(const MaterialSetting& setting)
	{
		osg::ref_ptr<osg::Material> material = new osg::Material;
		constructMaterial(setting, material.get());
		return material.release();
	}

	void constructMaterial(const MaterialSetting& setting, osg::Material* material)
	{
		const double *val = NULL;
		if ((val = setting.get(MaterialName::diffuse())) != NULL)
			material->setDiffuse(osg::Material::FRONT_AND_BACK, osg::Vec4(val[0], val[1], val[2], val[3]));
		if ((val = setting.get(MaterialName::shininess())) != NULL)
			material->setShininess(osg::Material::FRONT_AND_BACK, *val);
		if ((val = setting.get(MaterialName::ambient())) != NULL)
			material->setAmbient(osg::Material::FRONT_AND_BACK, osg::Vec4(val[0], val[1], val[2], val[3]));
		if ((val = setting.get(MaterialName::specular())) != NULL)
			material->setSpecular(osg::Material::FRONT_AND_BACK, osg::Vec4(val[0], val[1], val[2], val[3]));
		if ((val = setting.get(MaterialName::emission())) != NULL)
			material->setEmission(osg::Material::FRONT_AND_BACK, osg::Vec4(val[0], val[1], val[2], val[3]));
	}

	osgGA::CameraManipulator* ModelViewerImpl::createCameraManipulator()
	{
		return new BIMCameraManipulator(this);
	}

	osgGA::CameraManipulator* ModelViewerImpl::createPersonManipulator()
	{

		return new PersonManipulator();
		//return new osgGA::FirstPersonManipulator();
	}

	bool ModelViewerImpl::isLoaded()
	{
		return m_isLoaded;
	}

	void ModelViewerImpl::setKeyFrame(int framePosition)
	{
		m_animaEditor->SetKeyFrame(framePosition);
	}

	void ModelViewerImpl::enableAnimationEditor()
	{
		m_animaEditor->Enable();
	}

	void ModelViewerImpl::disableAnimationEditor()
	{
		m_animaEditor->Disable();
	}

	void ModelViewerImpl::setAnimatingNodes(osg::Node* node)
	{
		m_animaEditor->setSelectedNodes(node);
	}

	void ModelViewerImpl::setAnimatingNodes(std::vector<osg::Node*> nodes)
	{
		m_animaEditor->setSelectedNodes(nodes);
	}

	void ModelViewerImpl::addActionToSequece(const std::function<void()>& action, double delayMilliseconds)
	{
		m_animaEditor->addActionToSequece(action, delayMilliseconds);
	}

	void ModelViewerImpl::clearActionSequece()
	{
		m_animaEditor->clearActionSequece();
	}

	void ModelViewerImpl::play()
	{
		m_animaEditor->play();
	}

	void ModelViewerImpl::stop()
	{
		m_animaEditor->stop();

	}

	void ModelViewerImpl::playKeyFrame(int framePosition)
	{
		m_animaEditor->playKeyFrame(framePosition);
	}

	void ModelViewerImpl::setFrameMode(bool isFrameMode)
	{
		if (!m_modelRoot)
			return;

		// 设置线框模式
		osg::StateSet* stateset = m_modelRoot->getOrCreateStateSet();
		if (!stateset)
			return;
		osg::ref_ptr<osg::PolygonMode> polygonMode = new osg::PolygonMode();
		if (!isFrameMode)
			polygonMode->setMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::FILL);
		else
			polygonMode->setMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE);
		stateset->setAttribute(polygonMode.get(), osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);
	}

	void ModelViewerImpl::beginRecordAnimationControlPoint()
	{
		if (m_fixedAnimationPath)
		{
			switchMatrixManipulator(deflaut);
		}
		auto cameraManip = static_cast<bimWorld::BIMCameraManipulator*>(getCameraManipulator());
		if (!cameraManip)
			return;
		if (!m_modelRoot)
			return;
		osg::Matrix rotation = osg::Matrix::rotate(osg::DegreesToRadians(90.0), osg::Vec3d(1, 0, 0));
		osg::Vec3 downVec = osg::Vec3(0, -1, 0);
		downVec = rotation.preMult(downVec);
		m_fixedAnimationPath->Initialize(m_modelRoot, [this](float x, float y){
			osg::Vec3f screenPos = osg::Vec3f(x, y, 0);
			return m_coordHelper->ScreenToWorld(screenPos);
		}, downVec);
		switchToTopView();
		m_fixedAnimationPath->SetManipulatorContainer(cameraManip->getManipContainer());
	}

	void ModelViewerImpl::endRecordAnimationControlPoint()
	{
		if (!m_modelRoot)
			return;
		m_fixedAnimationPath->SetManipulatorContainer(NULL);
		auto manip = m_fixedAnimationPath->Apply();
		if (manip)
		{
			m_mViewer->setCameraManipulator(manip);
		}
		else
		{
			switchMatrixManipulator(deflaut);
		}
		setNeedUpdateAndDraw(true);
	}

	void ModelViewerImpl::applyAnimationPath(osg::Node* node)
	{
		//m_fixedAnimationPath->Apply(node);
	}

	}
