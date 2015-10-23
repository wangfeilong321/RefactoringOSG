#ifndef __VIEWER_DATA_MODEL_H__
#define __VIEWER_DATA_MODEL_H__
#include <osgViewer/Viewer>
#include <osg/Group>
#include <OpenThreads/Mutex>
#include <osg/MatrixTransform>

#include "IViewerDataModel.h"
#include "DataModel.h"
#include "YZModelCoreModule.h"
#include "Event.h"

namespace bimWorld
{

	class YIZHU_EXPORT ViewerDataModel : public YZModelCoreModule, public IViewerDataModel
	{
	public:
		ViewerDataModel(YZModelCoreInternal* host);

#ifdef Android
		virtual void initOSG(int x,int y,int width,int height) override;

		// <设置鼠标事件>
		virtual void onMousePress(float x, float y, int button) override;

		virtual void onMouseRelease(float x, float y, int button) override;

		virtual void onMouseMove(float x, float y) override;

		// <设置键盘事件>
		virtual void onKeyPress(int key) override;

		virtual void onKeyRelease(int key) override;

#else
		virtual void initOSG(std::function<void(osg::GraphicsContext::Traits* traits)> initGraphicContext) override;
#endif // Android


		virtual osgViewer::Viewer* ModelViewer() override;

		osg::Group* TopRoot() override;

		// <顶层根节点的回调函数集合>
		std::map<osg::ref_ptr<osg::Node>, const void(*)()>& TopMostCallback() override;

		std::map<int, osg::ref_ptr<osg::Node> >& TopMostSet() override;

		/************************************************************************************************
		* @fn	bool isTopMostNode(osg::Node* node);
		*
		* @brief	Query if 'node' is top most node.
		*
		* @author	Sqh
		* @date	2015/5/13
		*
		* @param [in,out]	node	If non-null, the node.
		*
		* @return	true if top most node, false if not.
		*
		* ### remarks	Sqh, 2015/5/6.
		**************************************************************************************************/
		bool isTopMostNode(osg::Node* node) override;

		// <屏幕视图>
		osg::Group* getSceneRoot() override;

		void setSceneRoot(osg::Group* root) override;

		bool validateSceneRoot() override;

		// <模型视图>
		osg::MatrixTransform* getModelRoot() override;

		void setModelRoot(osg::MatrixTransform* transformNode) override;

		bool validateModelRoot() override;

		// <屏幕相机>
		osg::Camera* getWindowSpace() override;

		osg::Camera* getTopRoot() override;

		osg::Camera* getBackGroundRoot() override;

		//// <视图监听器集合>
		//bimWorld::ListenerSet& ModelViewerListeners();

		// <是否完成>
		bool isDone() override;

		virtual void setDone(bool isDone);

		// <线程互斥锁>
		OpenThreads::Mutex& Mutex() override;

		bool isLoaded() override;

		void setLoaded(bool isLoaded) override;

		bool getCameraPerspective(CameraPerspective& perspective) override;

		virtual core::Event<void(std::vector<void*>, ViewerMode)>& SelectNodeEvent() override;

		virtual osg::Vec3 getSelectedCenter();

		virtual void setSelectedCenter(osg::Vec3 center);

// internal:
		void createTopMostCamera(osg::Camera* mainCamera);

		void createBackGroundCamera(osg::Camera* mainCamera);

		std::function<bool(void*)> _isComponentFunc;

	protected:

		// <OSG视图>
		osg::ref_ptr<osgViewer::Viewer> m_mViewer;

		// <底层根节点>
		osg::Camera* m_backGroundRoot;

		// <顶层根节点>
		osg::Camera* m_topRoot;

		// <顶层根节点的回调函数集合>
		std::map<osg::ref_ptr<osg::Node>, const void(*)()> m_topMostCallback;

		std::map<int, osg::ref_ptr<osg::Node> > m_topMostSet;

		// <屏幕视图>
		osg::Group* m_sceneRoot;

		// <模型根节点>
		osg::MatrixTransform* m_modelRoot;

		// <主相机>
		osg::Camera* m_windowSpace;

		//// <视图监听器集合>
		//ListenerSet m_modelViewerListeners;

		// <是否完成>
		bool m_bdone;

		// <线程互斥锁>
		OpenThreads::Mutex m_mutex;

		// <是否加载了模型>
		bool m_isLoaded;

		core::Event<void(std::vector<void*>, ViewerMode)> m_selectNodeEvent;

		osg::Vec3 m_selectedCenter;
	};

}

#endif // __VIEWER_DATA_MODEL_H__