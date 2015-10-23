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

		// <��������¼�>
		virtual void onMousePress(float x, float y, int button) override;

		virtual void onMouseRelease(float x, float y, int button) override;

		virtual void onMouseMove(float x, float y) override;

		// <���ü����¼�>
		virtual void onKeyPress(int key) override;

		virtual void onKeyRelease(int key) override;

#else
		virtual void initOSG(std::function<void(osg::GraphicsContext::Traits* traits)> initGraphicContext) override;
#endif // Android


		virtual osgViewer::Viewer* ModelViewer() override;

		osg::Group* TopRoot() override;

		// <������ڵ�Ļص���������>
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

		// <��Ļ��ͼ>
		osg::Group* getSceneRoot() override;

		void setSceneRoot(osg::Group* root) override;

		bool validateSceneRoot() override;

		// <ģ����ͼ>
		osg::MatrixTransform* getModelRoot() override;

		void setModelRoot(osg::MatrixTransform* transformNode) override;

		bool validateModelRoot() override;

		// <��Ļ���>
		osg::Camera* getWindowSpace() override;

		osg::Camera* getTopRoot() override;

		osg::Camera* getBackGroundRoot() override;

		//// <��ͼ����������>
		//bimWorld::ListenerSet& ModelViewerListeners();

		// <�Ƿ����>
		bool isDone() override;

		virtual void setDone(bool isDone);

		// <�̻߳�����>
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

		// <OSG��ͼ>
		osg::ref_ptr<osgViewer::Viewer> m_mViewer;

		// <�ײ���ڵ�>
		osg::Camera* m_backGroundRoot;

		// <������ڵ�>
		osg::Camera* m_topRoot;

		// <������ڵ�Ļص���������>
		std::map<osg::ref_ptr<osg::Node>, const void(*)()> m_topMostCallback;

		std::map<int, osg::ref_ptr<osg::Node> > m_topMostSet;

		// <��Ļ��ͼ>
		osg::Group* m_sceneRoot;

		// <ģ�͸��ڵ�>
		osg::MatrixTransform* m_modelRoot;

		// <�����>
		osg::Camera* m_windowSpace;

		//// <��ͼ����������>
		//ListenerSet m_modelViewerListeners;

		// <�Ƿ����>
		bool m_bdone;

		// <�̻߳�����>
		OpenThreads::Mutex m_mutex;

		// <�Ƿ������ģ��>
		bool m_isLoaded;

		core::Event<void(std::vector<void*>, ViewerMode)> m_selectNodeEvent;

		osg::Vec3 m_selectedCenter;
	};

}

#endif // __VIEWER_DATA_MODEL_H__