#include "YZModelCoreInternal.h"
#include "NodeController.h"
#include "NodeControllerWrapper.h"
#include "YZModelEntityInternal.h"

bimWorld::NodeControllerWrapper::NodeControllerWrapper(YZModelEntityInternal* host) :
YZModelEntityModule(host),
m_controller(new bimWorld::NodeController(host->_modelCore))
{

}

void bimWorld::NodeControllerWrapper::setStateColorToConstructs(void* ptr, int red, int green, int blue)
{
	m_controller->setStateColorToConstructs(ptr, red, green, blue);
}

bool bimWorld::NodeControllerWrapper::revertStateColorOfConstructs(void* ptr)
{
	return m_controller->revertStateColorOfConstructs(ptr);
}

bool bimWorld::NodeControllerWrapper::revertColorOfNodes(const std::vector<void*>& nodes)
{
	return m_controller->revertColorOfNodes(nodes);
}

bool bimWorld::NodeControllerWrapper::revertColorOfNode(void* node)
{
	return m_controller->revertColorOfNode(node);
}

void* bimWorld::NodeControllerWrapper::getModelRoot()
{
	if (!m_controller)
	{
		return NULL;
	}
	return m_controller->getModelRoot();
}

void bimWorld::NodeControllerWrapper::unTransParentAll()
{
	if (!m_controller)
	{
		return;
	}
	m_controller->unTransParentAll();
}

void bimWorld::NodeControllerWrapper::transparentAll()
{
	if (!m_controller)
	{
		return;
	}
	m_controller->transparentAll();
}

void bimWorld::NodeControllerWrapper::unHighlight(void*node)
{
	if (!m_controller)
	{
		return;
	}
	m_controller->unHighlight(node);
}

void bimWorld::NodeControllerWrapper::highlight(void*node)
{
	if (!m_controller)
	{
		return;
	}
	m_controller->highlight(node);
}

bool bimWorld::NodeControllerWrapper::isVisible(void *node)
{
	if (!m_controller)
	{
		return false;
	}
	return m_controller->isVisible(node);
}

void bimWorld::NodeControllerWrapper::setVisible(void * node, bool visible)
{
	if (!m_controller)
	{
		return;
	}
	m_controller->setVisible(node, visible);
}

void bimWorld::NodeControllerWrapper::setVisible(const std::vector<void*>& nodes, bool visible)
{
	if (!m_controller)
	{
		return;
	}
	m_controller->setVisible(nodes, visible);
}

void bimWorld::NodeControllerWrapper::toggleHide(void *node)
{
	if (!m_controller)
	{
		return;
	}
	m_controller->toggleHide(node);
}

void bimWorld::NodeControllerWrapper::toggleHide(const std::vector<void*>& nodes)
{
	if (!m_controller)
	{
		return;
	}
	m_controller->toggleHide(nodes);
}

void bimWorld::NodeControllerWrapper::unHideOthers(void *node)
{
	if (!m_controller)
	{
		return;
	}
	m_controller->unHideOthers(node);
}

void bimWorld::NodeControllerWrapper::hideOthers(void *node)
{
	if (!m_controller)
	{
		return;
	}
	m_controller->hideOthers(node);
}

void bimWorld::NodeControllerWrapper::unHide(void *node)
{
	if (!m_controller)
	{
		return;
	}
	m_controller->unHide(node);
}

void bimWorld::NodeControllerWrapper::unHide(const std::vector<void*>& nodes)
{
	if (!m_controller)
	{
		return;
	}
	m_controller->unHide(nodes);
}

void bimWorld::NodeControllerWrapper::hide(void *node)
{
	if (!m_controller)
	{
		return;
	}
	m_controller->hide(node);
}

void bimWorld::NodeControllerWrapper::hide(const std::vector<void*>& nodes)
{
	if (!m_controller)
	{
		return;
	}
	m_controller->hide(nodes);
}

void bimWorld::NodeControllerWrapper::unHideAll()
{
	if (!m_controller)
	{
		return;
	}
	m_controller->unHideAll();
}

void bimWorld::NodeControllerWrapper::hideAll()
{
	if (!m_controller)
	{
		return;
	}
	m_controller->hideAll();
}

void bimWorld::NodeControllerWrapper::setToTransparent(void* node, float opacity /*= 0.8*/)
{
	if (!m_controller)
	{
		return;
	}
	m_controller->setToTransparent(node, opacity);
}
