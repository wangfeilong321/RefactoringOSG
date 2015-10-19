#ifndef __IVIEWER_DATA_SET_H__
#define __IVIEWER_DATA_SET_H__
#include <osgViewer/Viewer>
#include <osg/Group>
#include <OpenThreads/Mutex>
#include <osg/MatrixTransform>

#include "DataModel.h"
#include <functional>
#include "Event.h"

namespace bimWorld
{

	class YIZHU_EXPORT IViewerDataModel
	{
	public:

		virtual osgViewer::Viewer* ModelViewer() = 0;

#ifdef Android
		virtual void initOSG(int x,int y,int width,int height) = 0;

		// <��������¼�>
		virtual void onMousePress(float x, float y, int button) = 0;

		virtual void onMouseRelease(float x, float y, int button) = 0;

		virtual void onMouseMove(float x, float y) = 0;

		// <���ü����¼�>
		virtual void onKeyPress(int key) = 0;

		virtual void onKeyRelease(int key) = 0;

#else
		virtual void initOSG(std::function<void(osg::GraphicsContext::Traits* traits)> initGraphicContext) = 0;
#endif // Android


		virtual osg::Group* TopRoot() = 0;

		// <������ڵ�Ļص���������>
		virtual std::map<osg::ref_ptr<osg::Node>, const void(*)()>& TopMostCallback() = 0;

		virtual std::map<int, osg::ref_ptr<osg::Node> >& TopMostSet() = 0;

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
		virtual bool isTopMostNode(osg::Node* node) = 0;

		// <��Ļ��ͼ>
		virtual osg::Group* getSceneRoot() = 0;

		virtual void setSceneRoot(osg::Group* root) = 0;

		virtual bool validateSceneRoot() = 0;

		// <ģ����ͼ>
		virtual osg::MatrixTransform* getModelRoot() = 0;

		virtual void setModelRoot(osg::MatrixTransform* transformNode) = 0;

		virtual bool validateModelRoot() = 0;

		// <��Ļ���>
		virtual osg::Camera* getWindowSpace() = 0;

		virtual osg::Camera* getTopRoot() = 0;

		virtual osg::Camera* getBackGroundRoot() = 0;

		//// <��ͼ����������>
		//bimWorld::ListenerSet& ModelViewerListeners();

		// <�Ƿ����>
		virtual bool isDone() = 0;

		// <�̻߳�����>
		virtual OpenThreads::Mutex& Mutex() = 0;

		virtual bool isLoaded() = 0;

		virtual void setLoaded(bool isLoaded) = 0;

		virtual bool getCameraPerspective(CameraPerspective& perspective) = 0;
		
		virtual core::Event<void(std::vector<void*>, ViewerMode)>& SelectNodeEvent() = 0;

	};

}

#endif // __IVIEWER_DATA_SET_H__