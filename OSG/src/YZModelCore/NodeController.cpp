#include "StringExt.h"
#include "NodeController.h"
#include "YZModelCoreInternal.h"
#include <osg/Material>
#include <osg/StateSet>
#include <mutex>
#include "NodeSearcher.h"
#include "VectorExt.h"


bimWorld::NodeController::NodeController(bimWorld::YZModelCoreInternal * host) :
bimWorld::YZModelCoreModule(host)
{
}

void bimWorld::NodeController::hideAll()
{
	NodeSearcher::traverseAllSubNodes(m_host->ViewerData()->getModelRoot(), [](osg::Node* node)->bool{
		if (!node)
			return false;
		std::mutex _lock;
		_lock.lock();

		node->setUserValue(NODE_VISIBILITY, false);
		node->setNodeMask(0);
		_lock.unlock();
		return true;
	}, true);

	m_host->RenderingThreads()->updateSeveralTimes(1);
}

void bimWorld::NodeController::unHideAll()
{
	NodeSearcher::traverseAllSubNodes(m_host->ViewerData()->getModelRoot(), [](osg::Node* node)->bool{
		if (!node)
			return false;
		std::mutex _lock;
		_lock.lock();

		node->setUserValue(NODE_VISIBILITY, true);
		node->setNodeMask((unsigned int)(-1));
		_lock.unlock();
		return true;
	}, true);
	m_host->RenderingThreads()->updateSeveralTimes(1);
}

void bimWorld::NodeController::hide(void * node)
{
	std::vector<void*> vec;
	vec.push_back(node);
	hide(vec);
}

void bimWorld::NodeController::hide(const std::vector<void*>& nodes)
{
	if (nodes.size() < 1)
		return;
	std::mutex _lock;
	_lock.lock();

	for (int i = 0; i < nodes.size(); ++i) {
		osg::Node *node = static_cast<osg::Node*>(nodes[i]);
		if (!node)
			continue;
		node->setUserValue(NODE_VISIBILITY, false);
		node->setNodeMask(0);
	}
	_lock.unlock();
	m_host->RenderingThreads()->updateSeveralTimes(1);
}

void bimWorld::NodeController::unHide(void * node)
{
	std::vector<void*> vec;
	vec.push_back(node);
	unHide(vec);
}

void bimWorld::NodeController::unHide(const std::vector<void*>& nodes)
{
	if (nodes.size() < 1)
		return;

	std::mutex _lock;
	_lock.lock();

	for (int i = 0; i < nodes.size(); ++i) {
		osg::Node *node = static_cast<osg::Node*>(nodes[i]);
		if (!node)
			continue;
		node->setUserValue(NODE_VISIBILITY, true);
		node->setNodeMask((unsigned int)(-1));
	}
	_lock.unlock();
	m_host->RenderingThreads()->updateSeveralTimes(1);
}

bool findAllParentsOfNode(osg::Node* node, std::vector<osg::Node*>& outVector)
{
	if (!node)
		return false;

	std::mutex _lock;
	_lock.lock();

	//if (node->getNumParents() != 1)
	//{
	//	osg::notify(osg::FATAL) << "We suppose each node only has one parent node, but it is not true, change the data associated with it."
	//		<< " in ModelViewerImpl::findAllParentsOfNode" << std::endl;
	//}

	if (node->getNumParents() < 1)
	{
		_lock.unlock();
		return false;
	}

	//auto parent = node->getParent(0);
    for(auto parentIter = node->getParents().begin(); parentIter!=node->getParents().end(); parentIter++)
	{
        auto parent = *parentIter;
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
	}
	_lock.unlock();
	return true;
}

//// @param callBack: return false to stop traverse this node.
//void bimWorld::NodeController::traverseAllSubNodes(osg::Node* node, std::function<bool(osg::Node*)> callBack, bool includeItsSelf)
//{
//	OpenThreads::ScopedLock<OpenThreads::Mutex> lock(m_host->ViewerData()->Mutex());
//	if (!node)
//		return;
//
//	if (includeItsSelf)
//	{
//		if (!callBack(node))
//			return;
//	}
//
//	auto group = node->asGroup();
//	if (!group)
//		return;
//	unsigned int num = group->getNumChildren();
//	if (node->asGeode())
//		return;
//
//	for (int i = 0; i < num; i++)
//	{
//		osg::Node* n = NULL;
//		n = group->getChild(i);
//
//		if (!n)
//			continue;
//		traverseAllSubNodes(n, callBack, true);
//	}
//}

void bimWorld::NodeController::hideOthers(void * node)
{
	auto n = static_cast<osg::Node*>(node);
	if (!n)
		return;
	//StoreRelationship store;
	std::vector<osg::Node*> parents;
	findAllParentsOfNode(n, parents);
	NodeSearcher::traverseAllSubNodes(m_host->ViewerData()->getModelRoot(), [parents, n](osg::Node* cur_node)->bool{
		if (!cur_node)
			return false;

		if (util::VectorExtension::contains(parents, cur_node))
			return true;

		if (cur_node == n)
			return false;

		cur_node->setUserValue(NODE_VISIBILITY, false);
		cur_node->setNodeMask(0);
		return true;
	}, true);
	m_host->RenderingThreads()->updateSeveralTimes(1);
}

void bimWorld::NodeController::unHideOthers(void * node)
{
	auto n = static_cast<osg::Node*>(node);
	if (!n)
		return;
	std::vector<osg::Node*> parents;
	findAllParentsOfNode(n, parents);

	NodeSearcher::traverseAllSubNodes(m_host->ViewerData()->getModelRoot(), [parents, n](osg::Node* cur_node)->bool{
		if (!cur_node)
			return false;

		if (util::VectorExtension::contains(parents, cur_node))
			return true;

		if (cur_node == n)
			return false;

		cur_node->setUserValue(NODE_VISIBILITY, true);
		cur_node->setNodeMask((unsigned int)(-1));
		return true;
	}, true);

	m_host->RenderingThreads()->updateSeveralTimes(1);
}

void bimWorld::NodeController::toggleHide(void * node)
{
	std::vector<void*> vec;
	vec.push_back(node);
	toggleHide(vec);
}

void bimWorld::NodeController::toggleHide(const std::vector<void*>& nodes)
{
	if (nodes.size() < 1)
		return;
	std::mutex _lock;
	_lock.lock();
	for (int i = 0; i < nodes.size(); ++i)
	{
		osg::Node *n = static_cast<osg::Node*>(nodes[i]);
		if (!n)
			continue;

		if (n->getNodeMask() == 0)
		{
			n->setUserValue(NODE_VISIBILITY, true);
			n->setNodeMask((unsigned int)(-1));
		}
		else if (n->getNodeMask() == -1)
		{
			n->setUserValue(NODE_VISIBILITY, false);
			n->setNodeMask(0);
		}
	}
	_lock.unlock();
	m_host->RenderingThreads()->updateSeveralTimes(1);
}

inline void bimWorld::NodeController::setVisible(void * node, bool visible)
{
	if (visible)
		hide(node);
	else
		unHide(node);
}

void bimWorld::NodeController::setVisible(const std::vector<void*>& nodes, bool visible)
{
	if (nodes.size())
	{
		return;
	}

	if (visible)
		hide(nodes);
	else
		unHide(nodes);
}

//bool bimWorld::NodeController::traverseSubNodesAndFindFirst(osg::Node* node, std::function<bool(osg::Node*)> isGotCallBack, bool includeItsSelf)
//{
//	OpenThreads::ScopedLock<OpenThreads::Mutex> lock(m_host->ViewerData()->Mutex());
//	if (!node)
//		return false;
//
//	if (includeItsSelf)
//	{
//		if (isGotCallBack(node))
//			return true;
//	}
//
//	auto group = node->asGroup();
//	if (!group)
//		return false;
//	unsigned int num = group->getNumChildren();
//	if (node->asGeode())
//		return false;
//
//	for (int i = 0; i < num; i++)
//	{
//		osg::Node* n = NULL;
//		n = group->getChild(i);
//
//		if (!n)
//			continue;
//		if (isGotCallBack(node))
//			return true;
//	}
//
//	for (int i = 0; i < num; i++)
//	{
//		auto n = group->getChild(i);
//		if (!n)
//		{
//			continue;
//		}
//
//		if (traverseSubNodesAndFindFirst(n, isGotCallBack, false))
//			return true;
//	}
//	return false;
//}

bool bimWorld::NodeController::isVisible(void * node)
{
	if (!node)
		return false;
	osg::Node *n = static_cast<osg::Node*>(node);
	if (!n)
		return false;
	return n->getNodeMask() != 0;
}

void bimWorld::NodeController::highlight(void * node)
{
	auto n = static_cast<osg::Node*>(node);
	if (n)
	{
		auto selectionState = m_host->_MaterialSetting()->getSelectionRenderModel();
		std::mutex _lock;
		_lock.lock();
		n->setStateSet(selectionState);
		_lock.unlock();
		m_host->_MaterialSetting()->setIsModified(true);
	}
	m_host->RenderingThreads()->updateSeveralTimes(1);
}

void bimWorld::NodeController::unHighlight(void * node)
{
	revertColorOfNode(node);
}

void bimWorld::NodeController::transparentAll()
{
	setToTransparent(m_host->ViewerData()->getModelRoot());
	m_host->RenderingThreads()->updateSeveralTimes(1);
}

void bimWorld::NodeController::unTransParentAll()
{
	m_host->ViewerData()->getModelRoot()->setStateSet(NULL);
	m_host->RenderingThreads()->updateSeveralTimes(1);
}

//void bimWorld::NodeController::unTransparent(void * node)
//{
//}

void bimWorld::NodeController::setToTransparent(void * ptr, float opacity/* = 0.8f*/)
{
	auto node = static_cast<osg::Node*>(ptr);
	if (!node)
	{
		return;
	}

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

		state->setAttribute(otherMat, osg::StateAttribute::OVERRIDE);
		state->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);

		state->setMode(GL_BLEND, osg::StateAttribute::ON);

		state->setAttributeAndModes(blendFunc);
		state->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
		node->setStateSet(NULL);
		node->setStateSet(state);
	}
}

void * bimWorld::NodeController::getModelRoot()
{
	return m_host->ViewerData()->getModelRoot();
}

std::string bimWorld::NodeController::getName(void * ptr)
{
	auto node = static_cast<osg::Node*>(ptr);
	if (!node)
	{
		return "";
	}
	return node->getName();
}

bool bimWorld::NodeController::getChildren(void * pnode, std::vector<void*>& children)
{
	auto node = static_cast<osg::Node*>(pnode);
	if (!node || !node->asGroup())
	{
		return false;
	}
	auto group = node->asGroup();

	if (group != NULL)
	{
		std::mutex _lock;
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

bool bimWorld::NodeController::revertColorOfNode(void * ptr)
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
	m_host->_MaterialSetting()->setIsModified(true);

	osg::StateSet* state;
	std::mutex _lock;
	std::string colorPort;
	_lock.lock();
	if (!node->getUserValue(NODE_CURRENT_COLOR, colorPort))
	{
		//state = NULL;
		node->setStateSet(NULL);
		_lock.unlock();
		m_host->RenderingThreads()->updateSeveralTimes(1);
		return true;
	}
	_lock.unlock();

	if (colorPort == NODE_ORIGINAL_COLOR)
	{
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
	}
	else if (colorPort == NODE_ADDITIONAL_COLOR0)
	{
		osg::Vec4 color;
		bool getResult;
		_lock.lock();
		getResult = node->getUserValue(NODE_ADDITIONAL_COLOR0, color);
		_lock.unlock();

		if (!getResult)
		{
			//state = NULL;
			_lock.lock();
			node->setStateSet(NULL);
			_lock.unlock();
		}
		else
		{
			setNodeState(node, color);
		}
		m_host->RenderingThreads()->updateSeveralTimes(1);
		return true;
	}
	else
	{
		state = NULL;
	}
	_lock.lock();
	node->setStateSet(state);
	_lock.unlock();

	m_host->RenderingThreads()->updateSeveralTimes(1);
	return true;
}

bool bimWorld::NodeController::revertColorOfNodes(const std::vector<void *>& nodes)
{
	if (nodes.size() < 1)
		return false;

	for (int i = 0; i < nodes.size(); i++)
	{
		revertColorOfNode(nodes[i]);
	}
	return true;
}

osg::BlendFunc* bimWorld::NodeController::getBlendFunc()
{
	std::mutex _lock;
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

void bimWorld::NodeController::setNodeState(osg::Node* node, osg::Vec4f color)
{
	if (!node)
		return;
	std::mutex _lock;
	_lock.lock();

	//auto red = color.x();
	//auto green = color.y();
	//auto blue = color.z();
	auto material = new osg::Material();
	material->setDiffuse(osg::Material::FRONT_AND_BACK, color);
	auto stateSet = new osg::StateSet();
	stateSet->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
	stateSet->setMode(GL_BLEND, osg::StateAttribute::ON);
	_lock.unlock();

	auto blendFunc = getBlendFunc();

	_lock.lock();
	stateSet->setAttribute(blendFunc);
	stateSet->setAttribute(material, osg::StateAttribute::PROTECTED | osg::StateAttribute::OVERRIDE);
	node->setStateSet(stateSet);
	_lock.unlock();
	return;
}

bool bimWorld::NodeController::revertStateColorOfConstructs(void* ptr)
{
	auto node = static_cast<osg::Node*>(ptr);
	if (!node)
	{
		osg::notify(osg::WARN) << "The node is null. in ModelViewerImpl::revertStateColorOfConstructs." << std::endl;
		return false;
	}
	node->setUserValue(NODE_CURRENT_COLOR, std::string(NODE_ORIGINAL_COLOR));
	osg::StateSet* state = static_cast<osg::StateSet*>(node->getUserData());
	if (!state)
	{
		state = NULL;
	}
	node->setStateSet(state);
	m_host->_RenderingThreads()->updateSeveralTimes(1);
	return true;
}

void bimWorld::NodeController::setStateColorToConstructs(void* ptr, int red, int green, int blue)
{
	auto node = static_cast<osg::Node*>(ptr);
	if (!node)
		return;
	auto color = osg::Vec4(red / 255.0f, green / 255.0f, blue / 255.0f, 1.0f);
	setNodeState(node, color);

	std::mutex _lock;
	_lock.lock();

	node->setUserValue(NODE_ADDITIONAL_COLOR0, color);
	std::string value = NODE_ADDITIONAL_COLOR0;
	node->setUserValue(NODE_CURRENT_COLOR, value);
	_lock.unlock();
	m_host->_RenderingThreads()->updateSeveralTimes(1);
}
