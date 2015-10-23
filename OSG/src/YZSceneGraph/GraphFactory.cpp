#include "GraphFactory.h"
#include "osg/Group"
#include "osg/ref_ptr"
#include "GroupExt.h"
#include "VectorExt.h"
#include "MapExt.h"
#include "StringExt.h"
#include <assert.h>
#include <map>
#include "VisibleControlCallback.h"
#include "YZGroupElement.h"
#include "MaterialCallback.h"

using namespace bimWorld;

GraphFactory::GraphFactory(YZModelCoreInternal* core)
{
	m_host = core;
}

GraphFactory::~GraphFactory()
{
}

//void GraphFactory::setCategoryAndRelation(const std::vector<std::vector<std::string> >& categories)
//{
//	// <��Ҫ����������>
//	auto treeCout = categories.size();
//
//}
//
//void GraphFactory::fillTree(osg::Group* tree, const std::map<std::string, bimWorld::YZString>& construct)
//{
//	//if (!tree||tree->getName()!="treeRoot")
//	//{
//	//	// invalid tree.
//	//	assert(false);
//	//	return;
//	//}
//	//GroupExt::for_each(tree, [construct](osg::Node* node){
//	//	if (util::MapExtension::containsKey(construct, node->getName()))
//	//	{
//	//		std::string name = node->getName();
//	//		GroupExt::containsChild(node->asGroup(), [name](osg::Node* n){
//	//			return n->getName() == name;
//	//		}, NULL);
//	//		construct[name]
//	//	}
//	//});
//}
//
//void GraphFactory::fillTree(const std::vector<YZ::Component*>& constructs)
//{
//	for each(auto com in constructs)
//	{
//		for each (auto tree in trees)
//		{
//			std::map<std::string, bimWorld::YZString> tags;
//			for each (auto tag in treeTags[tree])
//			{
//				bimWorld::YZString ta = L"@tag";
//				bimWorld::YZString value;
//				if (!com->getProperty(ta, value))
//				{
//					// <û�ҵ���Ӧ����ֵ>
//					assert(false);
//				}
//				tags[tag] = value;
//			}
//			fillTree(tree, tags);
//		}
//	}
//}

//osg::Group* modelRoot = new osg::Group();
//std::vector<std::string> categoryList;
//std::map<std::string, osg::Group*> tagTrees;
//std::vector<osg::Group*> trees;
//std::map<osg::Group*, std::vector<std::string> > treeTags;
void bimWorld::GraphFactory::constructTree(const std::vector<YZ::Component*>& constructs, const std::vector<YZ::Component*>& groups, const std::vector<CategoryNode>& children)
{
	//m_host->_MaterialSetting()->isModified();
    for(auto iter = constructs.begin(); iter!=constructs.end(); iter++)
	{
		(*iter)->setUserValue(NODE_CURRENT_COLOR, std::string(NODE_ORIGINAL_COLOR));

		//com->getOrCreateUserDataContainer()->setUserObject(NODE_CURRENT_STATESET, com->getStateSet());
		//com->getOrCreateUserDataContainer()->setUserObject(NODE_TEMP_STATESET, NULL);
		//com->setUpdateCallback(new MaterialCallback(m_host));
		if (!(*iter)->isMultiFloor())
		{
			(*iter)->setUserValue(NODE_VISIBILITY, true);
			(*iter)->setCullCallback(new VisibleControlCallback());
			//for each (auto drawable in com->getChild(0)->asGeode()->getDrawableList())
			//{
			//	drawable->setCullCallback(new DrawableCullCallback());
			//}
		}
	}

    for(auto iter = children.begin(); iter!=children.end(); iter++)
	{
		osg::ref_ptr<osg::Group> tree = new osg::Group();
		tree->setName("tree:" + (*iter).Value());
        for(auto comIter = constructs.begin(); comIter!=constructs.end(); comIter++)
		{
			AddGroups((*iter), (*comIter), tree);
		}
		m_host->_ViewerData()->getModelRoot()->addChild(tree);
	}

	osg::ref_ptr<osg::Group> groupTree = new osg::Group();
	groupTree->setName("tree:group");
    for(auto groupIter = groups.begin(); groupIter!=groups.end(); groupIter++)
	{
		auto g = dynamic_cast<YZ::GroupElement*>(*groupIter);
		if (!g->getGroup())
		{
			groupTree->addChild(*groupIter);
		}		
		g->setUserValue(NODE_HIGHLIGHTED, false);
		g->setName("group:" + g->getStrValue(YZ::YZ_GUID));
        for(auto comIter = g->getRelComponent().begin(); comIter!= g->getRelComponent().end(); comIter++)
		{
			g->addChild(*comIter);
		}
	}
	m_host->_ViewerData()->getModelRoot()->addChild(groupTree);
}

void bimWorld::GraphFactory::AddGroups(CategoryNode child, YZ::Component* com, osg::Group* tree)
{
	auto category = child.Value();
	//if (!YZ::isValid(category))
	//{
	//	// <û�ҵ���Ӧ����ֵ>
	//	assert(false);
	//}
	std::string categoryValue = com->getStrValue(category);
	//osg::ref_ptr<osg::Group> treeChild = new osg::Group();
	std::vector<bimWorld::MultiStringKey> keys;
	if (!child.getCategoryKey(keys))
	{
		assert(false);
		return;
	}
    for(auto strKeyIter = keys.begin(); strKeyIter!=keys.end(); strKeyIter++)
	{
        auto strKey = *strKeyIter;
		bimWorld::MultiStringKey subValueKey;
		bimWorld::MultiStringKey tempParentStrKey;
		auto base = tree;
        for(auto strIter = strKey.begin(); strIter!=strKey.end(); strIter++)
		{
            auto str = *strIter;
			// order
			// 1f
			// \|/
			// 1f->�ܵ�
			std::string categoryValue = com->getStrValue(str);
			subValueKey.push_back(categoryValue);
			tempParentStrKey.push_back(str);
			if (!m_queryMap[tempParentStrKey][subValueKey])
			{
				osg::ref_ptr<osg::Group> group = new osg::Group();
				group->setUserValue(NODE_HIGHLIGHTED, false);
				group->setName(subValueKey.toString());
				m_queryMap[tempParentStrKey][subValueKey] = group;
				base->addChild(group);
				base = group;
			}
			else
			{
				base = m_queryMap[tempParentStrKey][subValueKey];
			}
		}
		base->addChild(com);
	}

	//treeChild->setName(categoryValue);
	//if (lastChild)
	//{
	//	treeChild->addChild(lastChild);
	//}
	//if (child.Parents().size() > 0)
	//{
	//	for each (auto parent in child.Parents())
	//	{
	//		AddGroups(parent, com, treeChild);
	//	}
	//}
	//else
	//{
	//	m_host->_ViewerData()->getModelRoot()->addChild(treeChild);
	//}
}

#pragma region ��ʹ�����ַ�ʽ
/*
// <��ʹ�ã� ��Ϊ���������¥��-����-�������group��������ʵ¥�㡢���͡�������ֱ�ӿ��ƽڵ�>
void bimWorld::GraphFactory::generateLevelOrderList(const std::vector<CategoryNode>& children)
{
for each (auto child in children)
{
auto category = child.Value();
std::vector<std::string> list;
list.push_back(category);
std::vector <bimWorld::MultiStringKey> outKeys;
if (!child.getCategoryKey(outKeys))
{
// <δ��ȡ���������debug�жϣ�release����>
assert(false);
}

//  ÿ���ӽڵ��Ӧһ����
osg::ref_ptr<osg::Group> tree = new osg::Group();

// ��ײ���ӽڵ㣬���μӸ��ڵ㣬���ӽ�����
osg::ref_ptr<osg::Group> botChild = new osg::Group();

// ������������ϣ���Ӽ���// ÿ��������ж��������
for each (auto multiKey in outKeys)
{
//auto parent = tree;
//m_queryGroup[multiKey] = botChild;
//for (int i = 0; i < multiKey.size(); i++)
//{
//	bimWorld::MultiStringKey newKey;
//	newKey.insert(multiKey.begin(), multiKey.begin() + i);
//	if (!m_queryGroup[newKey])
//	{
//		osg::Group* newGroup = new osg::Group();
//		m_queryGroup[newKey] = newGroup;
//		parent->addChild(newGroup);
//		parent = newGroup;
//	}
//}
}


// �����б������ָ���Ӧ

// �����ӽڵ㼯�Ϲ�����
// �����ӽڵ㼯�ϣ�ÿ���ӽڵ���Ի�ȡ������б�
// �ֱ�˳����һ�ȡ���ķ�����б�.
// �����в����ڣ���ӣ������������ҵ������ж�Ӧ���ӽڵ㣬��������ӽ�ȥ��

m_levelOrderSet.push_back(list);
}
}

void GraphFactory::constructTree(const std::vector<CategoryNode>& children)
{
//for each (auto child in children)
//{
//	auto category = child.Value();
//	bimWorld::YZString tag = util::StringExtension::toYZString(category);
//	bimWorld::YZString value;
//	if (!com->getProperty(tag, value))
//	{
//		// <û�ҵ���Ӧ����ֵ>
//		assert(false);
//	}
//	auto categoryValue = util::StringExtension::toStdString(value);
//	osg::ref_ptr<osg::Group> tree = new osg::Group();
//	tree->setName("treeRoot");
//	osg::ref_ptr<osg::Group> currentGroup = tree;
//	treeTags[tree] = list;
//	trees.push_back(tree);
//	for each (auto com in constructs)
//	{
//		for each (auto category in list)
//		{
//			if (!util::VectorExtension::contains(categoryList, category))
//			{
//				// <��������ֵ����ȷ: category>
//				assert(false);
//			}
//			tagTrees[category] = tree;
//			osg::Node* firstChild = nullptr;
//			bimWorld::YZString ta = L"@tag";
//			bimWorld::YZString value;
//			if (!com->getProperty(ta, value))
//			{
//				// <û�ҵ���Ӧ����ֵ>
//				assert(false);
//			}
//			auto categoryValue = util::StringExtension::toStdString(value);
//			if (GroupExt::containsChild(currentGroup, [categoryValue](osg::Node* node){
//				if (!node)
//				{
//					return false;
//				}
//				return node->getName() == categoryValue;
//			}, firstChild))
//			{
//				currentGroup = firstChild->asGroup();
//			}
//			else
//			{
//				osg::ref_ptr<osg::Group> newNode = new osg::Group();
//				newNode->setName(category);
//				currentGroup->addChild(newNode.get());
//				currentGroup = newNode;
//			}
//		}
//	}
//	modelRoot->addChild(tree);
//}
}

void bimWorld::GraphFactory::fillTree()
{

}
*/

#pragma endregion

osg::Group* bimWorld::GraphFactory::select(const CategoryQueryKey& queryKey)
{
	try
	{
		auto group = m_queryMap.at(queryKey.getTypes()).at(queryKey.getKeys());
		return group;
	}
	catch (const std::out_of_range& oor)
	{
		std::cerr << "Out of range when querying bimWorld::GraphFactory. error: " << oor.what() << '\n';
		assert(false);
		return NULL;
	}
}

void bimWorld::GraphFactory::clear()
{
	m_queryMap.clear();
}

//// construct tree.
//// [¥�㣬 ���࣬ С��] [רҵ]
//// construct 1st tree.
//// if has level: check exist level group; add if not.
//// if has category: check exist group; add if not.
//// ...
//// when came to last one(child), add construct to that group.
//
//// construct 2nd tree.
//void GraphFactory::constructTree(const std::vector<std::vector<std::string> >& categories, const std::vector<YZ::Component*>& constructs)
//{
//	for each (auto list in categories)
//	{
//		osg::ref_ptr<osg::Group> tree = new osg::Group();
//		tree->setName("treeRoot");
//		osg::ref_ptr<osg::Group> currentGroup = tree;
//		treeTags[tree] = list;
//		trees.push_back(tree);
//		for each (auto com in constructs)
//		{
//			for each (auto category in list)
//			{
//				if (!util::VectorExtension::contains(categoryList, category))
//				{
//					// <��������ֵ����ȷ: category>
//					assert(false);
//				}
//				tagTrees[category] = tree;
//				osg::Node* firstChild = nullptr;
//				bimWorld::YZString ta = L"@tag";
//				bimWorld::YZString value;
//				if (!com->getProperty(ta, value))
//				{
//					// <û�ҵ���Ӧ����ֵ>
//					assert(false);
//				}
//				auto categoryValue = util::StringExtension::toStdString(value);
//				if (GroupExt::containsChild(currentGroup, [categoryValue](osg::Node* node){
//					if (!node)
//					{
//						return false;
//					}
//					return node->getName() == categoryValue;
//				}, firstChild))
//				{
//					currentGroup = firstChild->asGroup();
//				}
//				else
//				{
//					osg::ref_ptr<osg::Group> newNode = new osg::Group();
//					newNode->setName(category);
//					currentGroup->addChild(newNode.get());
//					currentGroup = newNode;
//				}
//			}
//		}
//		modelRoot->addChild(tree);
//	}
//}