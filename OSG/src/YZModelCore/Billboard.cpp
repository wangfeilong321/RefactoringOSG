#include "Billboard.h"
#include "YZModelCoreInternal.h"
#include "BillboardFactory.h"
#include "ReadPng.h"

bimWorld::Billboard::Billboard(YZModelCoreInternal* host) :YZModelCoreModule(host)
{
	m_newId = 0;
}

osg::Image* getImage(const char* file_path_name)
{
	if (!file_path_name)
		return NULL;
	ReaderWriterPNG* reader = new ReaderWriterPNG();
	osg::Image* image = NULL;
	std::ifstream file(file_path_name, std::ios::in | std::ios::binary | std::ios::ate);
	if (file.is_open())
	{
		file.seekg(0, std::ios::beg);
		auto result = reader->readPNGStream(file);
		if (!result.success())
			return NULL;
		image = result.takeImage();

		file.close();
	}
	return image;
}

osg::Node* createBillboard(osg::Vec3 position, osg::Geometry * geometry, osg::Image * image)
{
	return BillboardExtension::BillboardFactory::getInstance()->createBillboard(geometry, image, position);
}

int bimWorld::Billboard::addBillboard(void* selectedConstruct[], const void(*callback)(BillboardData*, const void*), const char* img_path_name /*= NULL*/, const void* externalData /*= NULL*/)
{
	if (!selectedConstruct)
		return -1;

	// <选择的器件数组长度>
	auto length = sizeof(selectedConstruct) / sizeof(selectedConstruct[0]);
	osg::Vec3d eye;
	osg::Vec3d center;
	osg::Vec3d up;
	m_host->_ViewerData()->ModelViewer()->getCameraManipulator()->getHomePosition(eye, center, up);
	//double min = DBL_MAX;
	osg::Vec3 pos;

	// <对多个模型的中心求平均值，即平均中心作为绘制的位置>
	auto tcenter = osg::Vec3(0, 0, 0);
	auto container = new osg::DefaultUserDataContainer();
	for (int i = 0; i < length; i++)
	{
		auto node = static_cast<osg::Node*>(selectedConstruct[i]);

		if (node)
			container->addUserObject(node);

	}
	pos = tcenter;

	auto billboard = createBillboard(pos, NULL, getImage(img_path_name));
	if (!billboard)
		return -1;

	billboard->setUserDataContainer(container);
	billboard->addUpdateCallback(new BillboardExtension::UpdateBillboardPosition(m_host->_Coordinate()->CoordinateHelper()));

	BillboardData* bData = new BillboardData();
	
	// TODO: we remove the relationship between uuid and node ptr in IModelCore project, 
	// so the storage structure of the billboard should change.
	// we won't store which constructs connect to this billboard.
	// these kind of work should be done in YZModelDAL project.

	//bData->setConstructIds(selectedConstruct);

	m_billboardDataSet.insert(std::pair<osg::ref_ptr<osg::Node>, BillboardData*>(billboard, bData));

	m_billboardExternalDataSet.insert(std::pair<osg::ref_ptr<osg::Node>, const void*>(billboard, externalData));

	int id = m_newId++;

	m_host->_ViewerData()->TopMostSet()[id] = billboard;

	m_host->_ViewerData()->TopMostCallback().insert(std::pair<osg::ref_ptr<osg::Node>, const void(*)()>(billboard, (const void(*)())(callback)));
	m_host->_ViewerData()->TopRoot()->addChild(billboard);
	m_host->_RenderingThreads()->updateSeveralTimes(1);
	return id;
}

bool bimWorld::Billboard::removeBillboard(int id)
{
	if (id < 1)
		return false;
	auto node = m_host->_ViewerData()->TopMostSet().find(id);
	if (node == m_host->_ViewerData()->TopMostSet().end())
		return false;

	if (!m_host->_ViewerData()->TopRoot()->removeChild(node->second))
		return false;
	m_host->_ViewerData()->TopMostCallback().erase(node->second);
	m_host->_RenderingThreads()->updateSeveralTimes(1);
	return true;
}

void bimWorld::Billboard::triggerBillboardCallback(osg::Node * node)
{
	if (!node)
		return;

	const void* eData = NULL;
	auto eIter = m_billboardExternalDataSet.find(node);
	if (eIter != m_billboardExternalDataSet.end())
		eData = eIter->second;

	BillboardData* bData = NULL;

	auto bIter = m_billboardDataSet.find(node);
	if (bIter != m_billboardDataSet.end())
		bData = bIter->second;

	auto fIter = m_host->_ViewerData()->TopMostCallback().find(node);
	if (m_host->_ViewerData()->TopMostCallback().end() == fIter)
		return;
	auto func = (const void(*)(BillboardData*, const void*))(fIter->second);
	if (func)
		func(bData, eData);
}
