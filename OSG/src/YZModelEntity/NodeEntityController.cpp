#include "NodeEntityController.h"
//#include <boost/locale.hpp>
#include "StringExt.h"
#include "VectorExt.h"
#include "NodeSearcher.h"

#ifdef _WIN32
//#include "StringConverter.h"
#include "EntityHelper.h"
//#include <QStringList>
#endif // _WIN32
#include "YZModelEntityInternal.h"
#include "IYZComponentCore.h"
#include "IYZComponentMgr.h"
#include "YZGroupElement.h"
//#include "IYZComponentDBMgr.h"

bimWorld::NodeEntityController::NodeEntityController(bimWorld::YZModelEntityInternal* host) :
NodeControllerWrapper(host)
{
	std::function<void(std::vector<void*>, bimWorld::ViewerMode)> func = std::bind(&bimWorld::NodeEntityController::onSelectNode, this, std::placeholders::_1, std::placeholders::_2);
	core::InstanceFunction<void(std::vector<void*>, bimWorld::ViewerMode)> insfunc(func, this, "onSelectNode");
	m_host->_modelCore->ViewerData()->SelectNodeEvent() += insfunc;
	m_host->_modelCore->_ViewerData()->_isComponentFunc = std::bind(&bimWorld::NodeEntityController::isComponent, this, std::placeholders::_1);
}

bimWorld::NodeEntityController::~NodeEntityController()
{
	std::function<void(std::vector<void*>, bimWorld::ViewerMode)> func = std::bind(&bimWorld::NodeEntityController::onSelectNode, this, std::placeholders::_1, std::placeholders::_2);
	core::InstanceFunction<void(std::vector<void*>, bimWorld::ViewerMode)> insfunc(func, this, "onSelectNode");
	m_host->_modelCore->ViewerData()->SelectNodeEvent() -= insfunc;
}

void bimWorld::NodeEntityController::hide(const std::string & id)
{
	if (id.empty())
		return;
	auto ptr = findNodeById(id);
	if (!ptr)
		return;
	m_controller->hide(ptr);
}

void bimWorld::NodeEntityController::unHide(const std::string & id)
{
	if (id.empty())
		return;
	auto ptr = findNodeById(id);
	if (!ptr)
		return;
	m_controller->unHide(ptr);
}

void bimWorld::NodeEntityController::hideOthers(const std::string & id)
{
	if (id.empty())
		return;
	auto ptr = findNodeById(id);
	if (!ptr)
		return;

	auto n = static_cast<osg::Node*>(ptr);
	if (!n)
		return;
	auto coms = YZ::getAllComponents();
    for(auto iter = coms.begin(); iter!=coms.end(); iter++)
	{
		if (iter->second != n)
			m_controller->hide(iter->second);
	}
	//m_controller->hideOthers(ptr);
}

void bimWorld::NodeEntityController::unHideOthers(const std::string & id)
{
	if (id.empty())
		return;
	auto ptr = findNodeById(id);
	if (!ptr)
		return;
	auto n = static_cast<osg::Node*>(ptr);
	if (!n)
		return;
    auto coms = YZ::getAllComponents();
    for(auto iter = coms.begin(); iter!=coms.end(); iter++)
	{
		if (iter->second != n)
			m_controller->unHide(iter->second);
	}
	//m_controller->unHideOthers(ptr);
}

void bimWorld::NodeEntityController::hideOthers(const std::vector<std::string>& ids)
{
	if (ids.empty())
		return;
	auto coms = YZ::getAllComponents();
	for (auto iter = coms.begin(); iter != coms.end(); iter++)
	{
		std::function<bool(std::string)> func = [this, iter](const std::string& id){
			if (id.empty())
			{
				return false;
			}

			auto ptr = findNodeById(id);
			if (!ptr)
				return false;

			auto n = static_cast<osg::Node*>(ptr);
			if (!n)
				return false;
			return iter->second == n;
		};

		if (util::VectorExtension::contains(ids, func))
		{
			continue;
		}

		m_controller->hide(iter->second);
	}
	//m_controller->hideOthers(ptr);
}

void bimWorld::NodeEntityController::unHideOthers(const std::vector<std::string>& ids)
{
	if (ids.empty())
		return;
	auto coms = YZ::getAllComponents();
	for (auto iter = coms.begin(); iter != coms.end(); iter++)
	{
		std::function<bool(std::string)> func = [this, iter](const std::string& id){
			if (id.empty())
			{
				return false;
			}

			auto ptr = findNodeById(id);
			if (!ptr)
				return false;

			auto n = static_cast<osg::Node*>(ptr);
			if (!n)
				return false;
			return iter->second == n;
		};

		if (util::VectorExtension::contains(ids, func))
		{
			continue;
		}

		m_controller->unHide(iter->second);
	}
	//m_controller->unHideOthers(ptr);
}

void bimWorld::NodeEntityController::toggleHide(const std::string & id)
{
	if (id.empty())
		return;
	auto ptr = findNodeById(id);
	if (!ptr)
		return;
	m_controller->toggleHide(ptr);
}

//void bimWorld::NodeEntityController::setOtherNodeToTransparent(const std::string & id)
//{
//}
//
//void bimWorld::NodeEntityController::setOtherNodeToTransparent(const std::string * ids, int length)
//{
//}

void bimWorld::NodeEntityController::onSelectNode(std::vector<void*> ptrs, bimWorld::ViewerMode mode)
{
	std::vector<std::string> ids;
	for (int i = 0; i < ptrs.size(); i++)
	{
		auto node = static_cast<osg::Node*>(ptrs[i]);
		if (!node)
		{
			continue;
		}
		ids.push_back(findIdByNode(ptrs[i]));
	}
	if (ids.size() == 0)
	{
		return;
	}
	core_Event_Trigger(m_selectNodeEvent, ids, mode);
}


bool bimWorld::NodeEntityController::getCategoryKey(const std::string& id, ConstructKey& outKey)
{
	auto node = findNodeById(id);
	if (!node)
		return false;
	return false;
}

std::vector<bimWorld::ConstructKey> bimWorld::NodeEntityController::getCategoryKeys()
{
	//return StoreRelationship().getCategoryKeys();
	std::vector<ConstructKey> keys;
#ifdef _WIN32
	// StoreRelationship().getAllCategory(m_currentPrjDBPath, keys)

	std::vector<std::map<std::string, std::string>> queryResult;
	EntityHelper::getAllCategory(m_currentPrjDBPath.c_str(), queryResult);
	for (int i = 0; i < queryResult.size(); i++)
	{
		bimWorld::ConstructKey key;
		key.levelName = util::StringExtension::toYZString(queryResult[i]["LevelName"]);
		key.topCategory = util::StringExtension::toYZString(queryResult[i]["Category"]);
		key.secondaryCategory = util::StringExtension::toYZString(queryResult[i]["Name"]);
		keys.push_back(key);
	}

#endif
	return keys;
}

bool bimWorld::NodeEntityController::getVisibility(const std::string& id, bool& visible)
{
	auto node = static_cast<osg::Node*>(findNodeById(id));
	if (!node)
		return false;
	std::mutex _lock;
	_lock.lock();
	bool result = node->getUserValue(NODE_VISIBILITY, visible);
	_lock.unlock();
	if (!result)
		return false;
	return true;
}

bool bimWorld::NodeEntityController::setHighLightByCategory(const YZString& floor, const YZString& topCategory, const YZString& secondaryCategory, bool isHighLight){ return false; }
/*
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
	osg::MatrixTransform* root;
	root = m_host->_modelCore->ViewerData()->getModelRoot();
	m_mutex.lock();
	NodeSearcher::traverseSubNodesAndFindFirst(root, [&](osg::Node* child){
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
	m_mutex.unlock();
	for (int i = 0; i < nodevec.size(); i++)
	{
		if (isHighLight)
		{
			m_host->_modelCore->NodeControl()->highlight(nodevec[i]);
		}
		else
		{
			m_host->_modelCore->NodeControl()->unHighlight(nodevec[i]);
		}
	}
	return true;
}*/

void traverseCom(osg::Group* group, std::function<void(osg::Node*)> func)
{
	for (int i = 0; i < group->getNumChildren(); i++)
	{
		auto node = group->getChild(i);
		if (!node)
		{
			continue;
		}
		void* com = NULL;
		try
		{
			com = dynamic_cast<YZ::Component*>(node);
		}
		catch (...)
		{
			com = NULL;
		}
		if (com)
		{
			func(node);
		}
		else
		{
			if (node->asGroup())
			{
				traverseCom(node->asGroup(), func);
			}
		}
	}
}

bool bimWorld::NodeEntityController::setHighLightByCategory(const CategoryQueryKey& queryKey, bool isHighLight)
{
	auto group = m_host->_ModelLoader()->select(queryKey);
	if (!group)
	{
		return false;
	}
	if (isHighLight)
	{
		traverseCom(group, [this](osg::Node* node){
			m_controller->highlight(node);
		});
	}
	else
	{
		traverseCom(group, [this](osg::Node* node){
			m_controller->unHighlight(node);
		});
	}
	return true;
}

void bimWorld::NodeEntityController::getAll(std::vector<std::string>& ids) const
{
	auto begin = m_host->_SharedData()->IdNodeptr().begin(), end = m_host->_SharedData()->IdNodeptr().end();
	while (begin != end)
	{
		ids.push_back(begin->first);
		++begin;
	}
}

std::string bimWorld::NodeEntityController::findIdByNode(void* ptr)
{
	auto node = static_cast<YZ::Component*>(ptr);
	if (!node)
	{
		return "";
	}
	return node->getStrValue(YZ::YZ_GUID);
	//return node->getUniqueID();
}

void* bimWorld::NodeEntityController::findNodeById(const std::string& id)
{
	//if (m_host->_SharedData()->IdNodeptr().size() == 0)
	//{
	//	return NULL;
	//}
	//auto iter = m_host->_SharedData()->IdNodeptr().find(id);
	//if (iter == m_host->_SharedData()->IdNodeptr().end())
	//{
	//	return NULL;
	//}
	//return iter->second;
	return YZ::getComponentByUniqueId(id);
}

void bimWorld::NodeEntityController::setStateColorToConstructs(const std::string& nodeId, int red, int green, int blue)
{
	if (nodeId.empty())
		return;
	auto ptr = findNodeById(nodeId);
	m_host->_modelCore->NodeControl()->setStateColorToConstructs(ptr, red, green, blue);
}

bool bimWorld::NodeEntityController::revertStateColorOfConstructs(const std::string& nodeId)
{
	auto ptr = findNodeById(nodeId);
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
	m_host->_modelCore->RenderingThreads()->updateSeveralTimes(1);
	return true;
}

void bimWorld::NodeEntityController::setCurrentDBPath(const std::string& path)
{
	m_currentPrjDBPath = path;
}

core::Event<void(std::vector<std::string>, bimWorld::ViewerMode)>& bimWorld::NodeEntityController::SelectNodeEvent()
{
	return m_selectNodeEvent;
}

inline void bimWorld::NodeEntityController::setVisible(const std::string & id, bool visible)
{
	if (visible)
		unHide(id);
	else
		hide(id);
}

void bimWorld::NodeEntityController::setVisible(const std::string ids[], int n, bool visible)
{
	if (m_host && ids && n > 0)
	{
		std::vector<void*> nodes;
		findNodesByIds(ids, n, nodes);
		if (!nodes.empty())
		{
			if (visible)
				m_controller->unHide(nodes);
			else
				m_controller->hide(nodes);
		}
	}
}

bool bimWorld::NodeEntityController::isVisible(const std::string & id)
{
	void* node = findNodeById(id);
	if (!node)
		return false;
	return m_controller->isVisible(node);
}

//void bimWorld::NodeEntityController::setVisibleByFloor(const YZString& floor, bool visible)
//{
//
//}

//void bimWorld::NodeEntityController::setVisibleByCategory(const YZString& category, bool visible)
//{
//
//}

void bimWorld::NodeEntityController::highlight(const std::string & id)
{
	if (id.empty())
		return;
	auto ptr = findNodeById(id);
	if (!ptr)
		return;
	m_controller->highlight(ptr);
}

void bimWorld::NodeEntityController::unHighlight(const std::string & id)
{
	if (id.empty())
		return;
	auto ptr = findNodeById(id);
	if (!ptr)
		return;
	m_controller->unHighlight(ptr);
}
//
//void bimWorld::NodeEntityController::unTransparent(const std::string & id)
//{
//	if (id.empty())
//		return;
//	auto ptr = findNodeById(id);
//	if (!ptr)
//		return;
//	m_controller->unTransParent(ptr);
//}

void bimWorld::NodeEntityController::setToTransparent(const std::string & id)
{
	if (id.empty())
		return;
	auto ptr = findNodeById(id);
	if (!ptr)
		return;
	m_controller->setToTransparent(ptr);
}

bool bimWorld::NodeEntityController::revertColorOfNode(const std::string & id)
{
	if (id.empty())
		return false;
	auto ptr = findNodeById(id);
	if (!ptr)
		return false;
	return m_controller->revertColorOfNode(ptr);
}

bool bimWorld::NodeEntityController::revertColorOfNodes(const std::vector<std::string>& ids)
{
	if (ids.size() < 1)
	{
		return false;
	}

	auto failed = false;
	for (int i = 0; i < ids.size(); i++)
	{
		auto id = ids[i];
		if (id.empty())
		{
			failed = true;
			continue;
		}
		if (!revertColorOfNode(id))
		{
			failed = true;
			continue;
		}
	}

	return !failed;
}

bool bimWorld::NodeEntityController::traverseSubNodesAndFindFirst(osg::Node* node, std::function<bool(osg::Node*)> isGotCallBack, bool includeItsSelf)
{
	//OpenThreads::ScopedLock<OpenThreads::Mutex> lock(m_mutex);
	//std::lock_guard<std::mutex> lock(m_mutex);
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

// <获取分类组节点>
void	bimWorld::NodeEntityController::getCategoryGroups(bimWorld::YZString level, bimWorld::YZString category, std::vector<osg::Group*>& nodevec)
{
	//std::string levelName, categoryName;
	//levelName = util::StringExtension::toStdString(level);
	//categoryName = util::StringExtension::toStdString(category);
	traverseSubNodesAndFindFirst(m_host->_modelCore->ViewerData()->getModelRoot(), [&](osg::Node* child){
		auto name = util::StringExtension::toYZString(child->getName());
		if (name != level)
		{
			return false;
		}
		auto group = child->asGroup();
		if (category.empty())
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
			auto name = util::StringExtension::toYZString(node->getName());
			// <先把系统类型拆分出来>
			std::vector < YZString> sysSplited;
#ifdef YIZHU_BIMWORLD_USE_WCHAR
			if (!util::StringExtension::split(name, L"$", sysSplited))
#else
			if (!util::StringExtension::split(name, "$", sysSplited))
#endif // YIZHU_BIMWORLD_USE_WCHAR

				continue;
			if (sysSplited.size() < 1)
				continue;
			// <拆分一下参数>
			std::vector<YZString> paramSplited;
#ifdef YIZHU_BIMWORLD_USE_WCHAR
			if (!util::StringExtension::split(category, L"\\", paramSplited))
#else
			if (!util::StringExtension::split(category, "\\", paramSplited))
#endif // YIZHU_BIMWORLD_USE_WCHAR

			{
				// <如果只有大类，则拆分一下名称，取名称中的大类与参数进行比较>
				std::vector<YZString> cateSplited;
#ifdef YIZHU_BIMWORLD_USE_WCHAR
				if (!util::StringExtension::split(sysSplited[0], L"\\", cateSplited))
#else
				if (!util::StringExtension::split(sysSplited[0], "\\", cateSplited))
#endif // YIZHU_BIMWORLD_USE_WCHAR

					continue;
				if (cateSplited.size() < 1)
				{
					continue;
				}

				if (cateSplited[0] != category)
				{
					continue;
				}
			}
			else
			{
				// <如果有大小类， 直接与名称中的大小类进行比较>
				if (sysSplited[0] != category)
				{
					continue;
				}

			}
			nodevec.push_back(node->asGroup());
		}
		return true;
	}, true);

}

void bimWorld::NodeEntityController::setVisibleByFloorCategory(const YZString& floor, const YZString& category, bool visible)
{
	std::vector<osg::Group*> nodevec;
	getCategoryGroups(floor, category, nodevec);

	if (visible)
	{
		// traverse all parent in case the parent is hidden, and we want it's child shows up.
		std::map<long, osg::Group*> parents;
		for (int i = 0; i < nodevec.size(); i++)
		{
			if (!nodevec[i])
			{
				continue;
			}
			auto group = nodevec[i]->asGroup();
			if (!group)
				continue;
			auto parentNum = group->getNumParents();
			while (parentNum == 1)
			{
				auto parentGroup = group->getParent(0);
				if (!parentGroup || !parentGroup->asGroup())
				{
					break;
				}
				long tid = (long)parentGroup;
				parents[tid] = parentGroup;
				group = parentGroup;
				parentNum = group->getNumParents();
			}
		}
		for (auto iter = parents.begin(); iter != parents.end(); iter++)
		{
			m_controller->unHide(iter->second);
		}
	}

	// <简单处理，遍历三层结构>
	for (int i = 0; i < nodevec.size(); i++)
	{
		auto current = nodevec[i];
		if (current != NULL)
		{
			if (visible)
			{
				NodeSearcher::traverseThreeLayer(current, 0, [this](osg::Group* group){
					m_controller->unHide(group);
				});
			}
			else
			{
				NodeSearcher::traverseThreeLayer(current, 0, [this](osg::Group* group){
					m_controller->hide(group);
				});
			}
		}
	}
}

void bimWorld::NodeEntityController::findNodesByIds(const std::string ids[], int n, std::vector<void*>& nodes)
{
	for (int i = 0; i < n; ++i) {
		auto node = static_cast<osg::Node*>(findNodeById(ids[i]));
		if (node)
			nodes.push_back(node);
	}
}

bool bimWorld::NodeEntityController::isComponent(void* ptr)
{
	osg::Node* node = static_cast<osg::Node*>(ptr);
	if (!node)
	{
		return false;
	}
	return YZ::isComponent(node);
}

bool bimWorld::NodeEntityController::setVisibleByCategory(const CategoryQueryKey& queryKey, bool visible)
{
	auto group = m_host->_ModelLoader()->select(queryKey);
	if (!group)
	{
		return false;
	}
	if (visible)
	{
		m_controller->unHide(group);
	}
	else
	{
		m_controller->hide(group);
	}
	return true;
}

void bimWorld::NodeEntityController::highlightWithTopGroup(const std::string& id)
{
	if (id.empty())
		return;
	auto ptr = static_cast<YZ::Component*>(findNodeById(id));
	if (!ptr)
		return;
	YZ::GroupElement* g = ptr->getGroup();
	if (!g)
	{
		m_controller->highlight(ptr);
		return;
	}
	while (g&&g->getGroup())
	{
		g = g->getGroup();
	}
	Search(g->getRelComponent(), [this](YZ::Component* com){
		m_controller->highlight(com);
	});
	g->setUserValue(NODE_HIGHLIGHTED, true);
	//m_controller->highlight(g);
}


void bimWorld::NodeEntityController::Search(std::vector<YZ::Component*> coms, std::function<void(YZ::Component*)> func)
{
    for(auto iter = coms.begin(); iter!=coms.end(); iter++)
	{
		try
		{
			if (!(*iter))
			{
				continue;
			}
			auto group = dynamic_cast<YZ::GroupElement*>((*iter));
			if (!group)
			{
				func((*iter));
			}
			else
			{
				Search(group->getRelComponent(), func);
			}
		}
		catch (...)
		{
			func((*iter));
		}
	}
}

void bimWorld::NodeEntityController::unHighlightWithTopGroup(const std::string& id)
{
	if (id.empty())
		return;
	auto ptr = static_cast<YZ::Component*>(findNodeById(id));
	if (!ptr)
		return;
	YZ::GroupElement* g = ptr->getGroup();
	if (!g)
	{
		m_controller->unHighlight(ptr);
		return;
	}
	while (g&&g->getGroup())
	{
		g = g->getGroup();
	}
	Search(g->getRelComponent(), [this](YZ::Component* com){
		m_controller->unHighlight(com);
	});
	g->setUserValue(NODE_HIGHLIGHTED, false);

	//m_controller->unHighlight(g);
}

void bimWorld::NodeEntityController::unHideAll()
{

	auto constructMap = YZ::getAllComponents();

	std::vector<YZ::Component*> components;
	YZ::IComponentMgr::MapComponent::const_iterator it = constructMap.begin();
	for (; it != constructMap.end(); it++)
	{
		osg::ref_ptr<YZ::Component> comp = it->second;
		//model->addChild(comp.get());
		components.push_back(comp.get());
	}
    for(auto iter = components.begin(); iter!= components.end(); iter++)
	{
		if (!(*iter))
			continue;
		std::mutex _lock;
		_lock.lock();

		(*iter)->setUserValue(NODE_VISIBILITY, true);
		(*iter)->setNodeMask((unsigned int)(-1));
		_lock.unlock();
	}
	//NodeSearcher::traverseAllSubNodes(m_host->ViewerData()->getModelRoot(), [](osg::Node* node)->bool{
	//	if (!node)
	//		return false;
	//	std::mutex _lock;
	//	_lock.lock();

	//	node->setUserValue(NODE_VISIBILITY, true);
	//	node->setNodeMask(unsigned int(-1));
	//	_lock.unlock();
	//	return true;
	//}, true);
	m_host->_modelCore->RenderingThreads()->updateSeveralTimes(1);
}

void bimWorld::NodeEntityController::setSelectedCenter(const std::string& id)
{
	if (id.empty())
		return;
	auto com = static_cast<YZ::Component*>(findNodeById(id));
	if (!com)
		return;
	auto center = com->getBound().center();
	m_host->_modelCore->_ViewerData()->setSelectedCenter(center);
}
