#include "ModelViewer.h"
#include "ModelCoreFactory.h"
#include "ModelEntityFactory.h"
#include "AnimationFactory.h"
#include "SelectionController.h"

#ifdef _WIN32
bimWorld::ModelViewer::ModelViewer(HWND hwnd)
{
	m_animation = AniamtionFactory().getModule();
	//m_billboard = new bimWorld::Billboard();
	//m_backgroundSetting = new bimWorld::BackgroundSetting();
	//m_cameraManipulator = new bimWorld::CameraManipulator();
	//m_pick = new bimWorld::Pick();
	//m_coordinate = new bimWorld::Coordinate();
	//m_screenCapture = new bimWorld::ScreenCapture();
	//m_renderingThread = new bimWorld::RenderingThread();
	//m_nodeControl = new bimWorld::NodeController();
	//m_progressInfo = new bimWorld::ProgressInfo();
	//m_cameraSetting = new bimWorld::CameraSetting();
	//m_config = new bimWorld::Config();
	//m_animation = new bimWorld::AniamtionBLL();
	m_modelCore = ModelCoreFactory().getModule(hwnd);
	m_modelEntity = ModelEntityFactory().getModule(m_modelCore.get());
	//m_material = std::make_shared<bimWorld::MaterialSetting>();
	m_selectionCtrler = std::make_shared<bimWorld::SelectionController>(this);
}
#elif Android
bimWorld::ModelViewer::ModelViewer(int x,int y,int width,int height)
{
	m_animation = AniamtionFactory().getModule();
	m_modelCore = ModelCoreFactory().getModule(x, y, width, height);
	m_modelEntity = ModelEntityFactory().getModule(m_modelCore.get());
}
#else

#endif // _WIN32


//bimWorld::ModelViewer::~ModelViewer()
//{
	//m_modelCore->RenderingThreads()->setDone(true);
//}


bimWorld::IBillboard* bimWorld::ModelViewer::Billboard()
{
	return m_modelCore->Billboard().get();
}

bimWorld::IBackgroundSetting* bimWorld::ModelViewer::BackgroundSetting()
{
	return m_modelCore->BackgroundSetting().get();
}

bimWorld::ICameraManipulators* bimWorld::ModelViewer::CameraManipulator()
{
	return m_modelCore->CameraManipulator().get();
}

bimWorld::IScreenCapture* bimWorld::ModelViewer::ScreenCapture()
{
	return m_modelCore->ScreenCapture().get();
}

bimWorld::IRenderingThreads* bimWorld::ModelViewer::RenderingThread()
{
	return m_modelCore->RenderingThreads().get();
}

//bimWorld::INodeController* bimWorld::ModelViewer::NodeControl()
//{
//	return m_modelCore->NodeControl().get();
//}

bimWorld::IProgressInfo* bimWorld::ModelViewer::ProgressInfo()
{
	//return m_modelCore->pro().get();
	return NULL;
}

bimWorld::ICameraSettings* bimWorld::ModelViewer::CameraSetting()
{
	return m_modelCore->CameraSettings().get();
}

bimWorld::Animation::ICameraAnimationPath* bimWorld::ModelViewer::CameraPathAnimation()
{
	return m_animation->CameraPathAnimation().get();
}

bimWorld::Animation::IKeyFrameAnimation* bimWorld::ModelViewer::KeyFrameAnimation()
{
	return m_animation->KeyFrameAnimation().get();
}

bimWorld::IFixedFrameRate* bimWorld::ModelViewer::FixedFrameRate()
{
	return m_modelCore->FixedFrameRate().get();
}

bimWorld::IZoomEntityModel* bimWorld::ModelViewer::ZoomModel()
{
	return m_modelEntity->ZoomEntityModel().get();
}

bimWorld::INodeEntityController* bimWorld::ModelViewer::NodeControl()
{
	return m_modelEntity->ControlByData().get();
}

bimWorld::IModelLoader* bimWorld::ModelViewer::ModelLoader()
{
	return m_modelEntity->ModelLoader().get();
}

bimWorld::ISwitchView* bimWorld::ModelViewer::SwitchView()
{
	return m_modelCore->SwitchView().get();
}

bimWorld::IMovingCamera* bimWorld::ModelViewer::MovingCamera()
{
	return m_modelCore->MovingCamera().get();
}

bimWorld::ISelectionController* bimWorld::ModelViewer::SelectionController()
{
	return m_selectionCtrler.get();
}
