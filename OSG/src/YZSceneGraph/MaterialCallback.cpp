#include "MaterialCallback.h"


bimWorld::MaterialCallback::MaterialCallback(bimWorld::YZModelCoreInternal* host)
{
	m_host = host;
}

void bimWorld::MaterialCallback::operator()(osg::Node* node, osg::NodeVisitor* nv)
{
	update(node);

	traverse(node, nv);
}

void bimWorld::MaterialCallback::update(osg::Node* node)
{
	if (!m_host->_MaterialSetting()->isModified())
	{
		return;
	}

	auto state = getTopStateSet(node);
	if (state)
	{
		std::mutex _mutex;
		_mutex.lock();
		//node->getOrCreateUserDataContainer()->setUserObject(NODE_CURRENT_STATESET, node->getStateSet());
		node->getOrCreateUserDataContainer()->setUserObject(NODE_TEMP_STATESET, state);
		//node->setUserValue("now_material", state);
		node->setStateSet(state);
		_mutex.unlock();
	}
	else
	{
		auto container = node->getOrCreateUserDataContainer();
		if (container->getNumUserObjects() <= 0)
		{
			return;
		}

		auto state = static_cast<osg::StateSet*>(container->getUserObject(NODE_CURRENT_STATESET));

		std::mutex _mutex;
		_mutex.lock();
		container->setUserObject(NODE_TEMP_STATESET, NULL);
		node->setStateSet(NULL);
		node->setStateSet(state);
		_mutex.unlock();
	}
}

osg::StateSet* bimWorld::MaterialCallback::getTopStateSet(osg::Node* child)
{
	if (!child)
	{
		return NULL;
	}
	osg::StateSet* state = NULL;
	auto parents = child->getParents();

    for(auto iter = parents.begin(); iter!=parents.end(); iter++)
	{
		auto s = (*iter)->getStateSet();
		if (s)
		{
			state = s;
			break;
		}

	}
    
    for(auto iter = parents.begin(); iter!=parents.end(); iter++)
	{
		if ((*iter) == m_host->_ViewerData()->getModelRoot())
		{
			break;
		}
		auto s = getTopStateSet((*iter));
		if (!s)
		{
			continue;
		}
		state = s;
		break;
	}
	return state;
}
