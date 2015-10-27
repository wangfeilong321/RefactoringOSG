#include "ModelLoader.h"
#include <osgDB/DatabasePager>
#include <osgDB/Registry>
#include <osgDB/ReadFile>
#include <osgDB/FileUtils>
#include <osgDB/FileNameUtils>
#include <osg/MatrixTransform>
//#include "StoreRelationship.h"
#include "ViewerDataModel.h"
#include "YZModelCoreInternal.h"
#include "YZModelEntityInternal.h"

//BIM
#include "YZPrerequire.h"
#include "IYZModelDB.h"
#include "IYZModelDBMgr.h"
#include "IYZComponentDBMgr.h"
//#include "IYZBlockRefDBMgr.h"
#include "IYZModelMgr.h"
#include "IYZComponentMgr.h"
//#include "IYZBlockRefMgr.h"
#include "YZComponent.h"
//#include "YZGeometryElement.h"
#include "YZBlockyElement.h"
#include "YZPolygonMesh.h"
#include "YZPosition3.h"

#include "IYZModelCore.h"
#include "IYZComponentCore.h"

//#include "GraphFactory.h"


bimWorld::ModelLoader::ModelLoader(YZModelEntityInternal* host) :
YZModelEntityModule(host),
m_graphFactory(new bimWorld::GraphFactory(host->_modelCore))
{
}

bimWorld::ModelLoader::~ModelLoader()
{
}

void bimWorld::ModelLoader::load(const char *file)
{
	load(&file, 1);
}

osg::Node* bimWorld::ModelLoader::readNode(const char *file)
{
	osg::ref_ptr<osg::Node> result;
#ifdef _WIN32

	std::auto_ptr<probuf> pb(new probuf(osgDB::findDataFile(std::string(file))));
	if (pb->is_open())
	{
		std::istream mis(pb.get());
		auto registry = osgDB::Registry::instance();
		if (!registry)
		{
			return NULL;
		}
		auto extension = osgDB::getLowerCaseFileExtension(std::string(file));
		if (extension.empty())
		{
			return NULL;
		}
		osg::ref_ptr<osgDB::ReaderWriter> reader = registry->getReaderWriterForExtension(extension);
		if (!reader)
		{
			return NULL;
		}
		osgDB::ReaderWriter::ReadResult readResult = reader->readNode(mis);
		result = readResult.getNode();
	}
	else
	{
		result = osgDB::readNodeFile(file);
	}
#else
	result = osgDB::readNodeFile(file);
#endif
	return result.release();
}

osg::Node* bimWorld::ModelLoader::readNode(const char **files, int fileNum)
{
	typedef std::vector<osg::ref_ptr<osg::Node> > NodeList;
	NodeList nodeList;
	osg::ref_ptr<osg::Node> node;
	for (int i = 0; i < fileNum; ++i)
	{
		std::cout << "read node file : " << files[i] << std::endl;
		node = readNode(files[i]);

		if (node != NULL)
		{
			if (node->getName().empty())
			{
				node->setName(files[i]);
				node->setDataVariance(osg::Object::DYNAMIC);
			}
			nodeList.push_back(node.release());
		}
	}

	if (nodeList.empty())
		return NULL;
	// <初始化节点指针与ID 双向bimap>

	//GetIdVisitor<NodeptrIdBimaps, NodePtrNameBimaps> visitor(&m_nodeptrToIdBimaps, &m_nodePtrNameBimaps);
	if (nodeList.size() == 1)
	{

		//nodeList[0]->accept(visitor);
		return nodeList[0].release();
	}

	osg::ref_ptr<osg::Group> group = new osg::Group;
	group->setName("root");
	for (int i = 0; i < nodeList.size(); ++i)
		group->addChild(nodeList[i]);
	//group->accept(visitor);
	return group.release();
}

void bimWorld::ModelLoader::readNodeByMultiFle(const char **files, int fileNum)
{

	// <初始化节点指针与ID 双向bimap>

	//if (!m_host->_modelCore->ViewerData()->getModelRoot())
	//{
	//	m_host->_modelCore->ViewerData()->setModelRoot(new osg::MatrixTransform());
	//	m_host->_modelCore->ViewerData()->getModelRoot()->setName("root");
	//}

	std::set<std::string> nodeNameSet;
	int numChild = m_host->_modelCore->ViewerData()->getModelRoot()->getNumChildren();
	for (int i = 0; i < numChild; ++i)
	{
		osg::Node *child = m_host->_modelCore->ViewerData()->getModelRoot()->getChild(i);
		nodeNameSet.insert(child->getName());
	}

	osg::ref_ptr<osg::Node> node;
	for (int i = 0; i < fileNum; ++i)
	{
		std::cout << "read node file : " << files[i] << std::endl;
		node = readNode(files[i]);

		if (node != NULL)
		{
			if (node->getName().empty())
				node->setName(files[i]);
			// <防止重复加载，遍历构件及属性信息>
			if (!(nodeNameSet.find(node->getName()) != nodeNameSet.end()))
			{
				if ((node->getName() == "root") && (node->asGroup()->getNumChildren() == 1))
				{
					m_host->_modelCore->ViewerData()->getModelRoot()->addChild(node->asGroup()->getChild(0));
				}
				else
				{
					m_host->_modelCore->ViewerData()->getModelRoot()->addChild(node);
				}

			}
			else
			{


			}
		}
	}

	return;
}

void bimWorld::ModelLoader::load(const char **files, int fileNum)
{
	m_host->ProgressInfo()->resetProgress();
	//m_isLoaded = false;
	m_host->_modelCore->ViewerData()->setLoaded(false);
	// Init the main Root Node/Group

	// Load the Model from the model name
	osg::ref_ptr<osg::Node> model = readNode(files, fileNum);


	if (!model.get())
	{
		m_host->_modelCore->ViewerData()->setLoaded(true);
		//m_isLoaded = true;
		return;
	}
	//StoreRelationship().clearAll();

	//osg::ref_ptr<osg::Group> reorganized = reorganize();
	//if (reorganized)
	//	model = reorganized;
	//else



	initializeCategoryGroups(model);
	//fireModelUnloadEvent(_modelCore->ViewerData()->ModelViewerListeners());
	core_Event_Trigger(m_unLoadEvent);
	// Optimize the model
	//osgUtil::Optimizer optimizer;
	//optimizer.optimize(mModel.get());
	//optimizer.reset();

	//unLoadAll();


	//if (!m_host->_modelCore->ViewerData()->getModelRoot())
	//{
	//	m_host->_modelCore->ViewerData()->setModelRoot(new osg::MatrixTransform());
	//	m_host->_modelCore->ViewerData()->getModelRoot()->setName("root");
	//	m_host->_modelCore->ViewerData()->getSceneRoot()->addChild(m_host->_modelCore->ViewerData()->getModelRoot());
	//}
	//else
	//{
		m_host->_modelCore->ViewerData()->getModelRoot()->removeChildren(0, m_host->_modelCore->ViewerData()->getModelRoot()->getNumChildren());
	//}
	m_host->_modelCore->ViewerData()->getModelRoot()->addChild(model);

	// Add the model to the scene

	m_host->_modelCore->CameraManipulator()->ZoomModel()->zoomTo(m_host->_modelCore->ViewerData()->getModelRoot());

	updateMode();

	//m_isLoaded = true;
	m_host->_modelCore->ViewerData()->setLoaded(true);
}

void bimWorld::ModelLoader::loadAll(const char *baseDir, const char *metaFileName)
{
	if (m_host->_modelCore)
	{
		m_host->ProgressInfo()->resetProgress();
		std::string directory = baseDir;
		std::string fileName = directory;
		fileName += metaFileName;

		std::ifstream in(fileName.c_str());

		int n = 0;
		if (in >> n && n >= 0)
		{
			std::getline(in, fileName);
			std::vector<std::string> paramFileNames, modelFileNames;
			for (int i = 0; i < n && std::getline(in, fileName); ++i)
			{
				paramFileNames.push_back(directory);
				paramFileNames.back() += fileName;
			}

			if (in >> n && n > 0)
			{
				std::getline(in, fileName);
				for (int i = 0; i < n && std::getline(in, fileName); ++i)
				{
					modelFileNames.push_back(directory);
					modelFileNames.back() += fileName;
				}

				if (in && !modelFileNames.empty())
				{
					std::vector<const char*> files;
					for (size_t i = 0; i < paramFileNames.size(); ++i)
					{
						files.push_back(paramFileNames[i].c_str());
					}
					//if (!files.empty())
					//	m_pImpl->loadParam(&files[0], files.size());

					files.clear();
					for (size_t i = 0; i < modelFileNames.size(); ++i)
					{
						files.push_back(modelFileNames[i].c_str());
						//m_pImpl->loadBySingleFlr(modelFileNames[i].c_str(),0.0);
					}

					std::vector<double>						dEngineeringLevelvec;
					std::vector<std::pair<YZString, std::string> >	vecresut;
					loadByMultiFlr(&files[0], files.size(), dEngineeringLevelvec, vecresut);

				}

			}
		}
	}
}

int bimWorld::ModelLoader::loadBySingleFlr(const char *files, double dEngineeringLevel, long& outFloorNode)
{
	m_host->_modelCore->ViewerData()->setLoaded(false);
	//m_isLoaded = false;
	m_host->ProgressInfo()->resetProgress();

	osg::ref_ptr<osg::Node> model = readNode(files);
	if (!model.get())
	{
		m_host->_modelCore->ViewerData()->setLoaded(false);
		//m_isLoaded = true;
		return -1;
	}

	//fireModelUnloadEvent(m_modelViewerListeners);
	core_Event_Trigger(m_unLoadEvent);

	//bool needReadd = false;

	//if (!m_host->_modelCore->ViewerData()->getModelRoot())
	//{
	//	m_host->_modelCore->ViewerData()->setModelRoot(new osg::MatrixTransform());
	//	m_host->_modelCore->ViewerData()->getModelRoot()->setName("root");
	//	needReadd = true;
	//}
	int numChild = m_host->_modelCore->ViewerData()->getModelRoot()->getNumChildren();
#if _DEBUG
	int refCount = model->referenceCount();
#endif // _DEBUG
	outFloorNode = (long)(model.get());
#if _DEBUG
	refCount = model->referenceCount();
#endif // _DEBUG
	for (int i = 0; i < numChild; ++i)
	{
		osg::Node *child = m_host->_modelCore->ViewerData()->getModelRoot()->getChild(i);
		if (child->getName() == model->getName())
		{
			m_host->_modelCore->ViewerData()->setLoaded(true);
			//m_isLoaded = true;
			return 1;
		}
	}
	//osg::Matrix mat;
	//mat.makeTranslate(osg::Vec3(0.0f,0.0f,dEngineeringLevel));
	//osg::ref_ptr<osg::MatrixTransform> transform =new osg::MatrixTransform();
	//transform->setMatrix(mat);
	//transform->addChild(model);
	//m_modelRoot->addChild(transform);
	m_host->_modelCore->ViewerData()->getModelRoot()->addChild(model.get());

	// <初始化节点指针与ID 双向bimap>
	//GetIdVisitor<NodeptrIdBimaps, NodePtrNameBimaps> visitor(&m_nodeptrToIdBimaps, &m_nodePtrNameBimaps);
	//model->accept(visitor);

#if _DEBUG
	refCount = model->referenceCount();
#endif // _DEBUG

	initializeCategoryGroups(m_host->_modelCore->ViewerData()->getModelRoot());

	// Add the model to the scene
	//if (needReadd)
	//{
	//	m_host->_modelCore->ViewerData()->getSceneRoot()->addChild(m_host->_modelCore->ViewerData()->getModelRoot());
	//}
	updateMode();
	m_host->_modelCore->ViewerData()->setLoaded(true);
	//m_isLoaded = true;
	return 0;

}

bool bimWorld::ModelLoader::unLoadBySingleFlr(long floorNode)
{
	auto node = (osg::Node*)(floorNode);
	if (!node)
		return false;

	if (!m_host->_modelCore->ViewerData()->getModelRoot())
		return false;

	// <移除节点指针与ID 双向bimap>
	//RemoveIdVisitor<NodeptrIdBimaps, NodePtrNameBimaps> visitor(&m_nodeptrToIdBimaps, &m_nodePtrNameBimaps);
	//node->accept(visitor);

	// TODO: < 实现在关联关系中卸载楼层 >
	//// < 移除ConstructMaterialSet >

	bool result = m_host->_modelCore->ViewerData()->getModelRoot()->removeChild(node);
	int refCount = node->referenceCount();

	if (refCount != 0 && refCount > 0)
	{
		osg::notify(osg::WARN) << "The floor root node has reference count : " << refCount << ". Manually unref() it." << std::endl;
		//for (int i = 0; i < refCount; i++)
		//{
		//	node->unref();
		//	refCount = node->referenceCount();
		//}
	}

	updateMode();
	m_host->_modelCore->RenderingThreads()->updateSeveralTimes(1);
	return result;
}

void bimWorld::ModelLoader::unLoadAll()
{
	if (!m_host->_modelCore->ViewerData()->getModelRoot() || !m_host->_modelCore->ViewerData()->getSceneRoot())
	{
		return;
	}
	m_graphFactory->clear();
	YZ::CloseBIMFile();

	//m_modelRoot->removeChildren(0, m_modelRoot->getNumChildren());
	//auto manip = static_cast<BIMCameraManipulator*>(getCameraManipulator());
	//if (manip)
	//{
	//m_host->_modelCore->ViewerData()->setModelRoot(NULL);
	//}
#ifdef _DEBUG
	int iParant = m_host->_modelCore->ViewerData()->getModelRoot()->getNumParents();
	int iRef = m_host->_modelCore->ViewerData()->getModelRoot()->referenceCount();
#endif // _DEBUG

	m_host->_modelCore->ViewerData()->getSceneRoot()->removeChild(m_host->_modelCore->ViewerData()->getModelRoot());
	m_host->_modelCore->ViewerData()->setModelRoot(NULL);
	updateMode();
}

int bimWorld::ModelLoader::loadByMultiFlr(const char **files,
	int ifileSize,
	std::vector<double>& dEngineeringLevelvec,
	std::vector<std::pair<bimWorld::YZString, std::string> >& vecresut)
{
	m_host->_modelCore->ViewerData()->setLoaded(false);
	//m_isLoaded = false;
	m_host->ProgressInfo()->resetProgress();
	readNodeByMultiFle(files, ifileSize);

	//StoreRelationship().clearAll();
	//int numChild = m_modelRoot->getNumChildren();
	//for (int i = 0; i < numChild; ++i)
	//	initializeCategoryGroup(m_modelRoot->getChild(i));
	initializeCategoryGroups(m_host->_modelCore->ViewerData()->getModelRoot());
	//fireModelUnloadEvent(m_modelViewerListeners);
	core_Event_Trigger(m_unLoadEvent);

	// Add the model to the scene

	//m_host->_modelCore->ViewerData()->getSceneRoot()->addChild(m_host->_modelCore->ViewerData()->getModelRoot());
	m_host->_modelCore->CameraManipulator()->ZoomModel()->zoomTo(m_host->_modelCore->ViewerData()->getModelRoot());


	updateMode();

	m_host->_modelCore->ViewerData()->setLoaded(true);
	//m_isLoaded = true;
	return 0;
}


void bimWorld::ModelLoader::initializeCategoryGroups(osg::Node *root)
{
	osg::Group *rootGroup = root->asGroup();
	if (!rootGroup) {
		return;
	}
	//StoreRelationship store;
	//std::string insertSql;
	//std::map<std::string, osg::Node*> idNptr;
	m_host->_SharedData()->IdNodeptr().clear();
	//store.begin();
	if (rootGroup)
	{
		int numChild = rootGroup->getNumChildren();
		while (numChild == 1)
		{
			auto n = rootGroup->getChild(0);
			if (n->getName() == "root")
			{
				rootGroup = n->asGroup();
				if (!rootGroup)
				{
					return;
				}
				numChild = rootGroup->getNumChildren();
			}
			else
			{
				break;
			}
		}

		for (int i = 0; i < numChild; ++i)
		{
			osg::Group * floorGroup = dynamic_cast<osg::Group*>(rootGroup->getChild(i));
			if (floorGroup != NULL)
			{
				int numChild = floorGroup->getNumChildren();
				std::string floorName = floorGroup->getName();

				for (int groupIndex = 0; groupIndex < floorGroup->getNumChildren(); groupIndex++)
				{
					auto category = floorGroup->getChild(groupIndex);
					if (!category)
					{
						continue;
					}
					auto categoryGroup = category->asGroup();
					if (!categoryGroup)
					{
						continue;
					}
					for (int nodeIndex = 0; nodeIndex < categoryGroup->getNumChildren(); nodeIndex++)
					{
						auto node = categoryGroup->getChild(nodeIndex);
						if (!node)
						{
							continue;
						}
						auto id = node->getName();
						m_host->_SharedData()->IdNodeptr()[id] = node;

#ifndef READ_COLOR

						auto stateset = node->getStateSet();
						node->setUserData(stateset);
						std::string value = NODE_ORIGINAL_COLOR;
						node->setUserValue(NODE_CURRENT_COLOR, value);

#endif // !READ_COLOR

						//m_idNodeptr[id] = (int)(util::IntPtr::toVoidPtr(node));
						//insertSql += store.getAddNodeSql(id, node);

					}
				}
				//}
				//}
			}
		}
	}
	//store.execAsOneCommand(insertSql);

	//store.analyzeGroupTable();
	//store.commit();

#if _WIN32 && READ_COLOR
	// init material
	std::map<std::string, osg::ref_ptr<osg::Material>> idMat;
	store.getIdMaterialMap(m_currentPrjDBPath, idMat);
	for (auto iter = m_idNodeptr.begin(); iter != m_idNodeptr.end(); iter++)
	{
		auto stateset = createStateSetByMaterial(idMat[iter->first]);
		if (!stateset)
		{
			continue;
		}
		NodeSearcher::traverseAllSubNodes(iter->second, [stateset](osg::Node* n){
			OpenThreads::Mutex mutex;
			mutex.lock();
			n->setStateSet(stateset);
			n->setUserData(stateset);
			mutex.unlock();
			if (!n->asGeode())
			{
				return true;
			}
			return false;// stop
		}, true);
	}
#endif // _WIN32

}

void bimWorld::ModelLoader::updateMode()
{
#if USING_FIXED_FRAMERATE
	if (m_host->_modelCore->ViewerData()->validateSceneRoot() && m_host->_modelCore->ViewerData()->validateModelRoot())
	{
		//			osgGA::KeySwitchMatrixManipulator *switchmanipulator = dynamic_cast<osgGA::KeySwitchMatrixManipulator*>(m_mViewer->getCameraManipulator());
		//			switchmanipulator->selectMatrixManipulator(0);
		//PersonManipulator* manipulator= dynamic_cast<PersonManipulator*>(switchmanipulator->getCurrentMatrixManipulator());
		//			BIMCameraManipulator *manipulator = dynamic_cast<BIMCameraManipulator*>(m_mViewer->getCameraManipulator());
		//auto manipulator = dynamic_cast<ICameraManipulators*>(_modelCore->CameraManipulator()->getCameraManipulator());

		//if (manipulator)
		//{
		m_host->_modelCore->FixedFrameRate()->setupFrameRateController(m_host->_modelCore->ViewerData()->getModelRoot());
		m_host->_modelCore->CameraManipulator()->updateModelSize();
		//}
	}

#endif
	m_host->_modelCore->CameraManipulator()->ZoomModel()->zoomTo(m_host->_modelCore->ViewerData()->getModelRoot());
	//fireModelLoadEvent(m_modelViewerListeners);
	core_Event_Trigger(m_loadEvent);
	m_host->_modelCore->RenderingThreads()->updateSeveralTimes(1);
}

core::Event<void()>& bimWorld::ModelLoader::UnLoadEvent()
{
	return m_unLoadEvent;
}

core::Event<void(void)>& bimWorld::ModelLoader::LoadEvent()
{
	return m_loadEvent;
}

core::Event<void(const std::string[], int)>& bimWorld::ModelLoader::SelectionEvent()
{
	return m_selectionEvent;
}

bool bimWorld::ModelLoader::loadFromDAL(const std::string& dbDir, std::vector<bimWorld::CategoryNode> children)
{
	if (!YZ::OpenBIMFile(dbDir))
		return false;
	auto constructMap = YZ::getAllComponents();

	//YZ::IModelMgr* yzModelMgr = NULL;
	//YZ::IModelDBMgr* yzModelDBMgr = NULL;
	//if (!boundModelDB(dbDir, yzModelMgr, yzModelDBMgr))
	//	return false;

	//yzModelDBMgr->loadModel();

	//yzModelMgr->synchronize();

	//osg::ref_ptr<osg::Group> model = new osg::Group();
	std::vector<YZ::Component*> components;
	//const YZ::IComponentMgr::MapComponent& mapComp = yzModelMgr->getComponentMgr()->getComponent();
	YZ::IComponentMgr::MapComponent::const_iterator it = constructMap.begin();
	for (; it != constructMap.end(); it++)
	{
		osg::ref_ptr<YZ::Component> comp = it->second;
		//model->addChild(comp.get());
		components.push_back(comp.get());
	}
	
	auto groupMap = YZ::getAllModelGroups();
	std::vector<YZ::Component*> groups;
	it = groupMap.begin();
	for (; it != groupMap.end(); it++)
	{
		osg::ref_ptr<YZ::Component> comp = it->second;
		//model->addChild(comp.get());
		groups.push_back(comp.get());
	}

	m_graphFactory->constructTree(components, groups, children);
	m_host->_modelCore->ViewerData()->setLoaded(true);
	//m_isLoaded = true;
	//model->setName("root");
	//m_host->_modelCore->ViewerData()->getModelRoot()->addChild(model.release());
	m_host->ZoomEntityModel()->zoomRoot();
	return true;
}

osg::Group* bimWorld::ModelLoader::select(const CategoryQueryKey& queryKey)
{
	return m_graphFactory->select(queryKey);
}
