#include "SwitchView.h"
#include "YZModelCoreInternal.h"

#pragma region SwitchView

bimWorld::SwitchView::SwitchView(YZModelCoreInternal* host) :YZModelCoreModule(host)
{

}

void bimWorld::SwitchView::switchToFrontView()
{
	m_host->_CameraManipulator()->ZoomModel()->zoomTo(
		m_host->_ViewerData()->getModelRoot(), true, ViewDirection::Front
		);
}

void bimWorld::SwitchView::switchToBackView()
{
	m_host->_CameraManipulator()->ZoomModel()->zoomTo(
		m_host->_ViewerData()->getModelRoot(), true, ViewDirection::Back
		);
}

void bimWorld::SwitchView::switchToLeftView()
{
	m_host->_CameraManipulator()->ZoomModel()->zoomTo(
		m_host->_ViewerData()->getModelRoot(), true, ViewDirection::Left
		);
}

void bimWorld::SwitchView::switchToRightView()
{
	m_host->_CameraManipulator()->ZoomModel()->zoomTo(
		m_host->_ViewerData()->getModelRoot(), true, ViewDirection::Right
		);
}

void bimWorld::SwitchView::switchToTopView()
{
	m_host->_CameraManipulator()->ZoomModel()->zoomTo(
		m_host->_ViewerData()->getModelRoot(), true, ViewDirection::Top
		);
}

void bimWorld::SwitchView::switchToBottomView()
{
	m_host->_CameraManipulator()->ZoomModel()->zoomTo(
		m_host->_ViewerData()->getModelRoot(), true, ViewDirection::Bottom
		);
}

#pragma endregion
