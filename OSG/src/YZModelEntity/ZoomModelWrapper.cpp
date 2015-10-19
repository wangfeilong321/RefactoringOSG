#include "ZoomModelWrapper.h"
#include "YZModelCoreInternal.h"
#include "ZoomModel.h"
#include "YZModelEntityInternal.h"

bimWorld::ZoomModelWrapper::ZoomModelWrapper(YZModelEntityInternal* host):
YZModelEntityModule(host),
m_controller(new bimWorld::ZoomModel(host->_modelCore))
{

}

bool bimWorld::ZoomModelWrapper::zoomRoot(bool withoutAnimation /*= true*/, ViewDirection direction /*= Front*/)
{
	if (!m_controller)
	{
		return false;
	}
	return m_controller->zoomRoot(withoutAnimation, direction);
}

bool bimWorld::ZoomModelWrapper::zoomTo(void* node, bool withoutAnimation /*= true*/, ViewDirection direction /*= Front*/)
{
	if (!m_controller)
	{
		return false;
	}
	return m_controller->zoomTo(node, withoutAnimation, direction);
}

bool bimWorld::ZoomModelWrapper::zoomTo(void** nodes, int length, bool withoutAnimation /*= true*/, ViewDirection direction /*= Front*/)
{
	if (!m_controller)
	{
		return false;
	}
	return m_controller->zoomTo(nodes, length, withoutAnimation, direction);

}

void bimWorld::ZoomModelWrapper::zoom(double dy)
{
	if (!m_controller)
	{
		return;
	}
	m_controller->zoom(dy);
}
