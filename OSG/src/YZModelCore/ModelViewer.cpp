#include "StoreRelationship.h"
#include "ModelViewer.h"
#include "ModelViewerImpl.h"
#include "tinyxml.h"
#include <osg/ValueObject>
#include <osgUtil/Simplifier>
#include <fstream>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/parsers.hpp>
#include <iostream>  
#include "config.h"
#include <boost/locale.hpp>
#include "PrjConfig.h"
#include "PersonManipulator.h"
#ifdef TRACE_ALL
#include "yzlogs.h"
#endif
namespace
{
	using namespace boost::program_options;
	struct ConversionParameters : public value_semantic_codecvt_helper < char >
	{
	public:
		ConversionParameters(std::string *paramFile, std::string *modelFile,
			std::string *outputDir) : paramFile(paramFile), modelFile(modelFile), outputDir(outputDir)
		{

		}
		virtual std::string name() const
		{
			return "ConversionParameters";
		}
		virtual unsigned min_tokens() const
		{
			return 3;
		}
		virtual unsigned max_tokens() const
		{
			return 3;
		}
		virtual bool is_composing() const
		{
			return false;
		}
		virtual bool is_required() const
		{
			return false;
		}
		virtual bool apply_default(boost::any& value_store) const
		{
			return false;
		}
		virtual void notify(const boost::any& value_store) const
		{

		}
		std::string *paramFile, *modelFile, *outputDir;
	protected:
		void xparse(boost::any& value_store, const std::vector<std::string>& new_tokens) const
		{
			*paramFile = new_tokens[0];
			*modelFile = new_tokens[1];
			*outputDir = new_tokens[2];
			assert(new_tokens.size() == 3);
		}
	};
}
namespace bimWorld
{

	ModelViewer::ModelViewer(ModelViewerImpl *m_pImpl) : m_pImpl(m_pImpl),
		m_CategoryMaterialMap(NULL), _class_Name("ModelViewer")
	{
#ifdef TRACE_ALL
		m_pSqhLogs = new YZLogs("sqh");
#endif
	}
	ModelViewer::~ModelViewer()
	{
		if (m_pImpl)
		{
			delete m_pImpl;
			m_pImpl = NULL;
		}
        
        #ifdef TRACE_ALL
		if (m_pSqhLogs)
		{
			delete m_pSqhLogs;
			m_pSqhLogs = NULL;
		}
        #endif

		if (m_CategoryMaterialMap)
		{
			delete m_CategoryMaterialMap;
			m_CategoryMaterialMap = NULL;
		}

	}

	bool ModelViewer::zoomTo(const std::string& id, bool withoutAnimation)
	{
		if (!m_pImpl)
			return false;

#ifdef TRACE_ALL
		std::wostringstream stream;
		stream << "setFrameMode.";
		auto eid = m_sqhLogs.ExcutionStart(stream, _class_Name, "setFrameMode");
#endif // TRACE_ALL

		bool result = zoomTo(m_pImpl->findNodeById(id), withoutAnimation);

#ifdef TRACE_ALL
		m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL
		return result;
	}

	void ModelViewer::setFrameMode(bool isFrameMode)
	{
		if (!m_pImpl)
			return;
#ifdef TRACE_ALL
		std::wostringstream stream;
		stream << "setFrameMode.";
		auto eid = m_sqhLogs.ExcutionStart(stream, _class_Name, "setFrameMode");
#endif // TRACE_ALL

		m_pImpl->setFrameMode(isFrameMode);
#ifdef TRACE_ALL
		m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL
	}

	void ModelViewer::hide(void *node)
	{
#ifdef TRACE_ALL
		std::wostringstream stream;
		stream << "hide.";
		auto eid = m_sqhLogs.ExcutionStart(stream, _class_Name, "hide");
#endif // TRACE_ALL

		if (m_pImpl && node)
			m_pImpl->hide(node);

#ifdef TRACE_ALL
		m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL

	}

	std::vector<ConstructKey> ModelViewer::getCategoryKeys()
	{
		std::vector<ConstructKey> result;
#ifdef TRACE_ALL
		std::wostringstream stream;
		stream << "getCategoryKeys.";
		auto eid = m_sqhLogs.ExcutionStart(stream, _class_Name, "getCategoryKeys");
#endif // TRACE_ALL

		if (m_pImpl)
			result = m_pImpl->getCategoryKeys();

#ifdef TRACE_ALL
		m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL

		return result;
	}

	bool ModelViewer::getCategoryKey(const std::string& id, ConstructKey& outKey)
	{
		if (!m_pImpl)
			return false;

#ifdef TRACE_ALL
		std::wostringstream stream;
		stream << "getCategoryKey.";
		stream << "id:" << id << ".";
		stream << "levelName:" << outKey.levelName.c_str() << ".";
		stream << "topCategory:" << outKey.topCategory.c_str() << ".";
		stream << "secondaryCategory:" << outKey.secondaryCategory.c_str() << ".";
		stream << "systemType:" << outKey.systemType.c_str() << ".";
		auto eid = m_sqhLogs.ExcutionStart(stream, _class_Name, "getCategoryKey");
#endif // TRACE_ALL

		bool result = m_pImpl->getCategoryKey(id, outKey);

#ifdef TRACE_ALL
		m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL
		return result;
	}

	bool ModelViewer::getVisibility(const std::string& id, bool& visible)
	{
		if (!m_pImpl)
			return false;

#ifdef TRACE_ALL
		std::wostringstream stream;
		stream << "getVisibility.";
		stream << "id:" << id << ".";
		stream << "visible:" << visible << ".";
		auto eid = m_sqhLogs.ExcutionStart(stream, _class_Name, "getVisibility");
#endif // TRACE_ALL

		bool result = m_pImpl->getVisibility(id, visible);

#ifdef TRACE_ALL
		m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL
		return result;
	}

	bool ModelViewer::setHighLightByCategory(const YZString& floor, const YZString& topCategory, const YZString& secondaryCategory, bool isHighLight)
	{
		if (!m_pImpl)
			return false;

#ifdef TRACE_ALL
		std::wostringstream stream;
		stream << "setHighLightByCategory.";
		stream << "floor:" << floor.c_str() << ".";
		stream << "topCategory:" << topCategory.c_str() << ".";
		stream << "secondaryCategory:" << secondaryCategory.c_str() << ".";
		stream << "isHighLight:" << isHighLight << ".";
		auto eid = m_sqhLogs.ExcutionStart(stream, _class_Name, "setHighLightByCategory");
#endif // TRACE_ALL

		bool result = m_pImpl->setHighLightByCategory(floor, topCategory, secondaryCategory, isHighLight);

#ifdef TRACE_ALL
		m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL
		return result;
	}

	void  ModelViewer::hideAll()
	{
		if (!m_pImpl)
			return;

#ifdef TRACE_ALL
		std::wostringstream stream;
		stream << "hideAll.";
		auto eid = m_sqhLogs.ExcutionStart(stream, _class_Name, "hideAll");
#endif // TRACE_ALL

		m_pImpl->hideAll();

#ifdef TRACE_ALL
		m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL

	}

	void  ModelViewer::unHideAll()
	{
		if (!m_pImpl)
			return;

#ifdef TRACE_ALL
		std::wostringstream stream;
		stream << "unHideAll.";
		auto eid = m_sqhLogs.ExcutionStart(stream, _class_Name, "unHideAll");
#endif // TRACE_ALL

		m_pImpl->unHideAll();

#ifdef TRACE_ALL
		m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL

	}

	void ModelViewer::unHide(void *node)
	{

#ifdef TRACE_ALL
		std::wostringstream stream;
		stream << "unHide.";
		stream << " node:" << (int)node << ".";
		auto eid = m_sqhLogs.ExcutionStart(stream, _class_Name, "unHide");
#endif // TRACE_ALL

		if (m_pImpl && node)
			m_pImpl->unHide(node);

#ifdef TRACE_ALL
		m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL

	}

	void ModelViewer::toggleHide(void *node)
	{

#ifdef TRACE_ALL
		std::wostringstream stream;
		stream << "toggleHide.";
		stream << " node:" << (int)node << ".";
		auto eid = m_sqhLogs.ExcutionStart(stream, _class_Name, "toggleHide");
#endif // TRACE_ALL

		if (m_pImpl && node)
			m_pImpl->toggleHide(node);

#ifdef TRACE_ALL
		m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL

	}

	void ModelViewer::setVisible(void **nodes, int n, bool visible)
	{
		if (!m_pImpl || !nodes || n <= 0)
		{
			return;
		}

#ifdef TRACE_ALL
		std::wostringstream stream;
		stream << "set the visible of given nodes.";
		stream << " n:" << n << ".";
		stream << " visible:" << visible << ".";
		auto eid = m_sqhLogs.ExcutionStart(stream, _class_Name, "setVisible");
#endif // TRACE_ALL

		if (visible)
			m_pImpl->hide(nodes, n);
		else
			m_pImpl->unHide(nodes, n);

#ifdef TRACE_ALL
		m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL

	}

	bool ModelViewer::isVisible(void *node)
	{
		if (!m_pImpl || !node)
			return false;

#ifdef TRACE_ALL
		std::wostringstream stream;
		stream << "isVisible.";
		stream << " node:" << (int)node << ".";
		auto eid = m_sqhLogs.ExcutionStart(stream, _class_Name, "isVisible");
#endif // TRACE_ALL

		bool result = m_pImpl->isVisible(node);

#ifdef TRACE_ALL
		m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL

		return result;
	}

	void ModelViewer::hide(const std::string& id)
	{
		if (!m_pImpl)
		{
			return;
		}

#ifdef TRACE_ALL
		std::wostringstream stream;
		stream << "hide.";
		stream << " id:" << id << ".";
		auto eid = m_sqhLogs.ExcutionStart(stream, _class_Name, "hide");
#endif // TRACE_ALL

		void* node = m_pImpl->findNodeById(id);
		if (node)
			m_pImpl->hide(node);

#ifdef TRACE_ALL
		m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL

	}

	void ModelViewer::unHide(const std::string& id)
	{
		if (!m_pImpl) {
			return;
		}

#ifdef TRACE_ALL
		std::wostringstream stream;
		stream << "unHide.";
		stream << " id:" << id << ".";
		auto eid = m_sqhLogs.ExcutionStart(stream, _class_Name, "unHide");
#endif // TRACE_ALL

		void* node = m_pImpl->findNodeById(id);
		if (node)
			m_pImpl->unHide(node);

#ifdef TRACE_ALL
		m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL

	}

	void ModelViewer::hideOthers(const std::string& id)
	{
		if (!m_pImpl)
			return;

#ifdef TRACE_ALL
		std::wostringstream stream;
		stream << "hideOthers.";
		stream << " id:" << id << ".";
		auto eid = m_sqhLogs.ExcutionStart(stream, _class_Name, "hideOthers");
#endif // TRACE_ALL

		m_pImpl->hideOthers(id);

#ifdef TRACE_ALL
		m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL

	}

	void ModelViewer::hideOthers(void* node)
	{
		if (!m_pImpl)
			return;

#ifdef TRACE_ALL
		std::wostringstream stream;
		stream << "hideOthers.";
		stream << " node:" << (int)node << ".";
		auto eid = m_sqhLogs.ExcutionStart(stream, _class_Name, "hideOthers");
#endif // TRACE_ALL

		m_pImpl->hideOthers(node);

#ifdef TRACE_ALL
		m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL

	}

	void ModelViewer::unHideOthers(const std::string& id)
	{
		if (!m_pImpl)
			return;

#ifdef TRACE_ALL
		std::wostringstream stream;
		stream << "unHideOthers.";
		stream << " id:" << id << ".";
		auto eid = m_sqhLogs.ExcutionStart(stream, _class_Name, "unHideOthers");
#endif // TRACE_ALL

		m_pImpl->unHideOthers(id);

#ifdef TRACE_ALL
		m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL

	}

	void ModelViewer::unHideOthers(void* node)
	{
		if (!m_pImpl)
			return;

#ifdef TRACE_ALL
		std::wostringstream stream;
		stream << "unHideOthers.";
		stream << " node:" << (int)node << ".";
		auto eid = m_sqhLogs.ExcutionStart(stream, _class_Name, "unHideOthers");
#endif // TRACE_ALL

		m_pImpl->unHideOthers(node);

#ifdef TRACE_ALL
		m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL

	}


	void ModelViewer::toggleHide(const std::string& id)
	{
		if (!m_pImpl) {
			return;
		}
#ifdef TRACE_ALL
		std::wostringstream stream;
		stream << "toggleHide.";
		stream << " id:" << id << ".";
		auto eid = m_sqhLogs.ExcutionStart(stream, _class_Name, "toggleHide");
#endif // TRACE_ALL


		void* node = m_pImpl->findNodeById(id);
		if (node)
			m_pImpl->toggleHide(node);

#ifdef TRACE_ALL
		m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL

	}

	bool ModelViewer::isVisible(const std::string& id)
	{
		if (!m_pImpl)
		{
			return false;
		}
#ifdef TRACE_ALL
		std::wostringstream stream;
		stream << "isVisible.";
		stream << " id:" << id << ".";
		auto eid = m_sqhLogs.ExcutionStart(stream, _class_Name, "isVisible");
#endif // TRACE_ALL


		void* node = m_pImpl->findNodeById(id);
		if (!node)
		{
#ifdef TRACE_ALL
			m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL
			return false;
		}
		auto result = m_pImpl->isVisible(node);

#ifdef TRACE_ALL
		m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL

		return result;

	}

	void ModelViewer::setVisibleByFloor(const Floor* floor, bool visible)
	{
		//if (m_pImpl)
		//{

		//	osg::ref_ptr<osg::Node> pNode = m_pImpl->getNode(floor->value);
		//	if (pNode != NULL)
		//	{
		//		if (visible)
		//		{
		//			unHide(pNode);
		//		}
		//		else
		//		{
		//			hide(pNode);
		//		}
		//	}

		//}
	}

	void ModelViewer::setVisibleByFloorCategory(const Floor* floor, const Category* category, bool visible)
	{
		if (!m_pImpl)
		{
			return;
		}
#ifdef TRACE_ALL
		std::wostringstream stream;
		stream << "Change model visibility by floor category.";
		stream << " floor:" << floor->value.c_str() << ".";
		stream << " category:" << category->value.c_str() << ".";
		auto eid = m_sqhLogs.ExcutionStart(stream, _class_Name, "setVisibleByFloorCategory");
#endif // TRACE_ALL


		std::vector<osg::Group*> nodevec;
		m_pImpl->getCategoryGroups(floor->value, category->value, nodevec);
		for (int i = 0; i < nodevec.size(); i++)
		{
			if (nodevec[i] != NULL)
			{
				if (visible)
				{
					unHide(nodevec[i]);
				}
				else
				{
					hide(nodevec[i]);
				}
			}
		}

#ifdef TRACE_ALL
		m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL


	}

	void ModelViewer::setVisibleByCategory(const Category* category, bool visible)
	{
		if (m_pImpl)
		{
			return;
		}

#ifdef TRACE_ALL
		std::wostringstream stream;
		stream << "Change model visibility by category.";
		stream << " category:" << category->value.c_str() << ".";
		stream << " visible:" << visible << ".";
		auto eid = m_sqhLogs.ExcutionStart(stream, _class_Name, "setVisibleByCategory");
#endif // TRACE_ALL


		std::vector<std::string> ids;
		m_pImpl->getByCategory(ids, category);
		if (!ids.empty())
			this->setVisible(&ids[0], (int)ids.size(), visible);

#ifdef TRACE_ALL
		m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL


	}

	void ModelViewer::setVisible(const std::string* ids, int n, bool visible)
	{
#ifdef TRACE_ALL
		std::wostringstream stream;
		stream << "set visible of given ids.";
		stream << " n:" << n << ".";
		stream << " visible:" << visible << ".";
		auto eid = m_sqhLogs.ExcutionStart(stream, _class_Name, "setVisible");
#endif // TRACE_ALL

		if (m_pImpl && ids && n > 0)
		{
			std::vector<void*> nodes;
			m_pImpl->findNodesByIds(ids, n, nodes);
			if (!nodes.empty())
			{
				if (visible)
					m_pImpl->unHide(&nodes[0], nodes.size());
				else
					m_pImpl->hide(&nodes[0], nodes.size());
			}
		}

#ifdef TRACE_ALL
		m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL

	}

	bool ModelViewer::getCameraView(CameraView& cameraView)
	{
		if (!m_pImpl)
			return false;

#ifdef TRACE_ALL
		std::wostringstream stream;
		stream << "getCameraView.";
		auto eid = m_sqhLogs.ExcutionStart(stream, _class_Name, "getCameraView");
#endif // TRACE_ALL

		bool result = m_pImpl->getCameraView(cameraView);

#ifdef TRACE_ALL
		m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL

		return result;
	}

	bool ModelViewer::getCameraView(CameraView& cameraView, const char* strFileAllPath, unsigned char** buffer, int &width, int &height, int &channels)
	{
		if (!m_pImpl)
			return false;

#ifdef TRACE_ALL
		std::wostringstream stream;
		stream << "getCameraView.";
		auto eid = m_sqhLogs.ExcutionStart(stream, _class_Name, "getCameraView");
#endif // TRACE_ALL

		bool result = m_pImpl->getCameraView(cameraView, strFileAllPath, buffer, width, height, channels);

#ifdef TRACE_ALL
		m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL

		return result;
	}

	bool ModelViewer::getCameraPerspective(CameraPerspective& cameraPerspective)
	{
		if (!m_pImpl)
			return false;

#ifdef TRACE_ALL
		std::wostringstream stream;
		stream << "getCameraPerspective.";
		auto eid = m_sqhLogs.ExcutionStart(stream, _class_Name, "getCameraPerspective");
#endif // TRACE_ALL

		bool result = m_pImpl->getCameraPerspective(cameraPerspective);

#ifdef TRACE_ALL
		m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL

		return result;
	}

	bool ModelViewer::setCameraView(const CameraView& cameraView)
	{
		if (!m_pImpl)
			return false;

#ifdef TRACE_ALL
		std::wostringstream stream;
		stream << "setCameraView.";
		auto eid = m_sqhLogs.ExcutionStart(stream, _class_Name, "setCameraView");
#endif // TRACE_ALL

		bool result = m_pImpl->setCameraView(cameraView);
#ifdef TRACE_ALL
		m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL

		return result;
	}

	bool ModelViewer::setCameraPerspective(const CameraPerspective& cameraPerspective)
	{
		if (!m_pImpl)
			return false;

#ifdef TRACE_ALL
		std::wostringstream stream;
		stream << "setCameraPerspective.";
		auto eid = m_sqhLogs.ExcutionStart(stream, _class_Name, "setCameraPerspective");
#endif // TRACE_ALL

		bool result = m_pImpl->setCameraPerspective(cameraPerspective);

#ifdef TRACE_ALL
		m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL

		return result;
	}


	void ModelViewer::switchMatrixManipulator(EMANIPULATOR emanip)
	{
		if (!m_pImpl)
			return;

#ifdef TRACE_ALL
		std::wostringstream stream;
		stream << "switchMatrixManipulator.";
		auto eid = m_sqhLogs.ExcutionStart(stream, _class_Name, "switchMatrixManipulator");
#endif // TRACE_ALL

		m_pImpl->switchMatrixManipulator(emanip);

#ifdef TRACE_ALL
		m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL

	}

	bimWorld::ViewerMode ModelViewer::getCameraManipulatorMode()
	{
		if (!m_pImpl)
		{
			osg::notify(osg::FATAL) << "The ModelViewerImpl is null. in ModeViewer::getCameraManipulatorMode()." << std::endl;
			return bimWorld::Orbit;
		}

#ifdef TRACE_ALL
		std::wostringstream stream;
		stream << "getCameraManipulatorMode.";
		auto eid = m_sqhLogs.ExcutionStart(stream, _class_Name, "getCameraManipulatorMode");
#endif // TRACE_ALL

		bimWorld::ViewerMode result = m_pImpl->getCameraManipulatorMode();

#ifdef TRACE_ALL
		m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL

		return result;
	}

	void ModelViewer::enableFirstPersonControl()
	{
		if (!m_pImpl)
			return;

#ifdef TRACE_ALL
		std::wostringstream stream;
		stream << "enableFirstPersonControl.";
		auto eid = m_sqhLogs.ExcutionStart(stream, _class_Name, "enableFirstPersonControl");
#endif // TRACE_ALL

        m_pImpl->enableFirstPersonControl();

#ifdef TRACE_ALL
		m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL


	}

	void ModelViewer::disableFirstPersonControl()
	{
		if (!m_pImpl)
			return;

#ifdef TRACE_ALL
		std::wostringstream stream;
		stream << "disableFirstPersonControl.";
		auto eid = m_sqhLogs.ExcutionStart(stream, _class_Name, "disableFirstPersonControl");
#endif // TRACE_ALL

		m_pImpl->disableFirstPersonControl();

#ifdef TRACE_ALL
		m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL

	}
	void ModelViewer::zoomModel(double distance)
	{
		if (!m_pImpl)
			return;
		auto manip = static_cast<BIMCameraManipulator*>(m_pImpl->getCameraManipulator());
		if (!manip)
			return;
		manip->zoomModel(distance);
		m_pImpl->updateSeveralTimes();
	}

	void ModelViewer::translation(double startx, double starty, double endx, double endy)
	{
		if (!m_pImpl)
			return;

		auto manip = static_cast<PersonManipulator*>(m_pImpl->getCameraManipulator());
		if (!manip)
			return;

#ifdef TRACE_ALL
		std::wostringstream stream;
		stream << "translation.";
		auto eid = m_sqhLogs.ExcutionStart(stream, _class_Name, "translation");
#endif // TRACE_ALL

		manip->translation(startx, starty, endx, endy, m_pImpl->getCamera());

		m_pImpl->updateSeveralTimes();

#ifdef TRACE_ALL
		m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL

	}
	void ModelViewer::moveVertical(double distance)
	{
		if (!m_pImpl)
			return;

#ifdef TRACE_ALL
		std::wostringstream stream;
		stream << "moveVertical.";
		auto eid = m_sqhLogs.ExcutionStart(stream, _class_Name, "moveVertical");
#endif // TRACE_ALL

		auto manip = static_cast<PersonManipulator*>(m_pImpl->getCameraManipulator());
		if (!manip)
			return;

		manip->moveFront(distance);
		m_pImpl->updateSeveralTimes();

#ifdef TRACE_ALL
		m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL

	}

	void ModelViewer::moveHorizontal(double distance)
	{
		if (!m_pImpl)
			return;

#ifdef TRACE_ALL
		std::wostringstream stream;
		stream << "moveHorizontal.";
		auto eid = m_sqhLogs.ExcutionStart(stream, _class_Name, "moveHorizontal");
#endif // TRACE_ALL

		auto manip = static_cast<PersonManipulator*>(m_pImpl->getCameraManipulator());
		if (!manip)
			return;
		manip->moveRound(distance);
		m_pImpl->updateSeveralTimes();

#ifdef TRACE_ALL
		m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL

	}

	void ModelViewer::moveRight(double distance)
	{
		if (!m_pImpl)
			return;

#ifdef TRACE_ALL
		std::wostringstream stream;
		stream << "moveHorizontal.";
		auto eid = m_sqhLogs.ExcutionStart(stream, _class_Name, "moveHorizontal");
#endif // TRACE_ALL

		auto manip = static_cast<PersonManipulator*>(m_pImpl->getCameraManipulator());
		if (!manip)
			return;
		manip->moveRight(distance);
		m_pImpl->updateSeveralTimes();

#ifdef TRACE_ALL
		m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL

	}

	void ModelViewer::moveUp(double distance)
	{
		if (!m_pImpl)
			return;

#ifdef TRACE_ALL
		std::wostringstream stream;
		stream << "moveHorizontal.";
		auto eid = m_sqhLogs.ExcutionStart(stream, _class_Name, "moveHorizontal");
#endif // TRACE_ALL

		auto manip = static_cast<PersonManipulator*>(m_pImpl->getCameraManipulator());
		if (!manip)
			return;
		manip->moveUp(distance);
		m_pImpl->updateSeveralTimes();

#ifdef TRACE_ALL
		m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL

	}

	void ModelViewer::switchToFrontView()
	{
		if (!m_pImpl)
			return;

#ifdef TRACE_ALL
		std::wostringstream stream;
		stream << "switchToFrontView.";
		auto eid = m_sqhLogs.ExcutionStart(stream, _class_Name, "switchToFrontView");
#endif // TRACE_ALL

		m_pImpl->switchToFrontView();

#ifdef TRACE_ALL
		m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL

	}

	void ModelViewer::switchToBackView()
	{
		if (!m_pImpl)
			return;

#ifdef TRACE_ALL
		std::wostringstream stream;
		stream << "switchToBackView.";
		auto eid = m_sqhLogs.ExcutionStart(stream, _class_Name, "switchToBackView");
#endif // TRACE_ALL

		m_pImpl->switchToBackView();

#ifdef TRACE_ALL
		m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL

	}

	void ModelViewer::switchToLeftView()
	{
		if (!m_pImpl)
			return;

#ifdef TRACE_ALL
		std::wostringstream stream;
		stream << "switchToLeftView.";
		auto eid = m_sqhLogs.ExcutionStart(stream, _class_Name, "switchToLeftView");
#endif // TRACE_ALL

		m_pImpl->switchToLeftView();

#ifdef TRACE_ALL
		m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL

	}

	void ModelViewer::switchToRightView()
	{
		if (!m_pImpl)
			return;

#ifdef TRACE_ALL
		std::wostringstream stream;
		stream << "switchToRightView.";
		auto eid = m_sqhLogs.ExcutionStart(stream, _class_Name, "switchToRightView");
#endif // TRACE_ALL

		m_pImpl->switchToRightView();

#ifdef TRACE_ALL
		m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL

	}

	void ModelViewer::switchToTopView()
	{
		if (!m_pImpl)
			return;

#ifdef TRACE_ALL
		std::wostringstream stream;
		stream << "switchToTopView.";
		auto eid = m_sqhLogs.ExcutionStart(stream, _class_Name, "switchToTopView");
#endif // TRACE_ALL

		m_pImpl->switchToTopView();

#ifdef TRACE_ALL
		m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL

	}

	void ModelViewer::switchToBottomView()
	{
#ifdef TRACE_ALL
		std::wostringstream stream;
		stream << "switchToBottomView.";
		auto eid = m_sqhLogs.ExcutionStart(stream, _class_Name, "switchToBottomView");
#endif // TRACE_ALL

		if (m_pImpl)
			m_pImpl->switchToBottomView();

#ifdef TRACE_ALL
		m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL
	}

	void ModelViewer::getAll(std::vector<std::string>& ids) const
	{
#ifdef TRACE_ALL
		std::wostringstream stream;
		stream << "getAll.";
		auto eid = m_sqhLogs.ExcutionStart(stream, _class_Name, "getAll");
#endif // TRACE_ALL

		if (m_pImpl)
			m_pImpl->getAll(ids);

#ifdef TRACE_ALL
		m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL
	}
	void ModelViewer::getByFloor(std::vector<std::string>& ids, const Floor* floor)
	{
#ifdef TRACE_ALL
		std::wostringstream stream;
		stream << "getByFloor.";
		auto eid = m_sqhLogs.ExcutionStart(stream, _class_Name, "getByFloor");
#endif // TRACE_ALL

		if (m_pImpl)
			m_pImpl->getByFloor(ids, floor);

#ifdef TRACE_ALL
		m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL
	}

	void ModelViewer::getByCategory(std::vector<std::string>& ids, const Category* category)
	{
#ifdef TRACE_ALL
		std::wostringstream stream;
		stream << "getByFloor.";
		auto eid = m_sqhLogs.ExcutionStart(stream, _class_Name, "getByFloor");
#endif // TRACE_ALL

		if (m_pImpl)
			m_pImpl->getByCategory(ids, category);

#ifdef TRACE_ALL
		m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL
	}

	void ModelViewer::load(const char *file)
	{
#ifdef TRACE_ALL
		std::wostringstream stream;
		stream << "load with file.";
		auto eid = m_sqhLogs.ExcutionStart(stream, _class_Name, "load");
#endif // TRACE_ALL

		if (m_pImpl)
			m_pImpl->load(file);

#ifdef TRACE_ALL
		m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL
	}

	void ModelViewer::load(const char **files, int n)
	{
#ifdef TRACE_ALL
		std::wostringstream stream;
		stream << "load files.";
		auto eid = m_sqhLogs.ExcutionStart(stream, _class_Name, "load");
#endif // TRACE_ALL

		if (m_pImpl)
			m_pImpl->load(files, n);

#ifdef TRACE_ALL
		m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL
	}

	void ModelViewer::loadParam(const char *file)
	{
#ifdef TRACE_ALL
		std::wostringstream stream;
		stream << "loadParam with file.";
		auto eid = m_sqhLogs.ExcutionStart(stream, _class_Name, "loadParam");
#endif // TRACE_ALL
		if (m_pImpl)
			m_pImpl->loadParam(file);

#ifdef TRACE_ALL
		m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL
	}

	void ModelViewer::startRendering()
	{
#ifdef TRACE_ALL
		std::wostringstream stream;
		stream << "startRendering.";
		auto eid = m_sqhLogs.ExcutionStart(stream, _class_Name, "startRendering");
#endif // TRACE_ALL

		if (m_pImpl)
			m_pImpl->startRendering();

#ifdef TRACE_ALL
		m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL
	}

	void ModelViewer::updateSeveralTimes()
	{
		if (!m_pImpl)
			return;

#ifdef TRACE_ALL
		std::wostringstream stream;
		stream << "updateSeveralTimes.";
		auto eid = m_sqhLogs.ExcutionStart(stream, _class_Name, "updateSeveralTimes");
#endif // TRACE_ALL

		m_pImpl->updateSeveralTimes();

#ifdef TRACE_ALL
		m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL
	}

	const AttrConstruct *ModelViewer::getConstruct(const std::string& id)
	{
		if (!m_pImpl)
			return NULL;

#ifdef TRACE_ALL
		std::wostringstream stream;
		stream << "getConstruct.";
		stream << "id:" << id << ".";
		auto eid = m_sqhLogs.ExcutionStart(stream, _class_Name, "getConstruct");
#endif // TRACE_ALL

		auto result = m_pImpl->getConstruct(id);

#ifdef TRACE_ALL
		m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL
		return result;
	}



	bool ModelViewer::registerModelListener(ModelViewerListener *listener)
	{
		if (!m_pImpl)
			return false;

#ifdef TRACE_ALL
		std::wostringstream stream;
		stream << "registerModelListener.";
		auto eid = m_sqhLogs.ExcutionStart(stream, _class_Name, "registerModelListener");
#endif // TRACE_ALL

		auto result = m_pImpl->registerModelListener(listener);
#ifdef TRACE_ALL
		m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL
		return result;
	}

	bool ModelViewer::removeModelListener(ModelViewerListener *listener)
	{
		if (!m_pImpl)
			return false;

#ifdef TRACE_ALL
		std::wostringstream stream;
		stream << "removeModelListener.";
		auto eid = m_sqhLogs.ExcutionStart(stream, _class_Name, "removeModelListener");
#endif // TRACE_ALL

		auto result = m_pImpl->removeModelListener(listener);
#ifdef TRACE_ALL
		m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL
		return result;
	}

	bool ModelViewer::registerDefaultModelListener(ModelViewerListener *listener)
	{
		return ModelViewerImpl::registerDefaultModelListener(listener);
	}

	bool ModelViewer::removeDefaultModelListener(ModelViewerListener *listener)
	{
		return ModelViewerImpl::removeDefaultModelListener(listener);
	}

	void* ModelViewer::getModelRoot()
	{
		if (!m_pImpl)
			return NULL;

#ifdef TRACE_ALL
		std::wostringstream stream;
		stream << "getModelRoot.";
		auto eid = m_sqhLogs.ExcutionStart(stream, _class_Name, "getModelRoot");
#endif // TRACE_ALL

		auto result = m_pImpl->getModelRoot();

#ifdef TRACE_ALL
		m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL
		return result;
	}

	const char *ModelViewer::getName(void* node)
	{
		if (!m_pImpl)
			return NULL;

#ifdef TRACE_ALL
		std::wostringstream stream;
		stream << "getName.";
		auto eid = m_sqhLogs.ExcutionStart(stream, _class_Name, "getName");
#endif // TRACE_ALL

		auto result = m_pImpl->getName(node);

#ifdef TRACE_ALL
		m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL
		return result;
	}

	bool ModelViewer::getChildren(void* node, std::vector<void*>& children)
	{
		if (m_pImpl)
		{
			osg::Group* group = dynamic_cast<osg::Group *>((osg::Node*)node);
			if (group)
			{

#ifdef TRACE_ALL
				std::wostringstream stream;
				stream << "getChildren.";
				auto eid = m_sqhLogs.ExcutionStart(stream, _class_Name, "getChildren");
#endif // TRACE_ALL

				auto result = m_pImpl->getChildren(group, children);

#ifdef TRACE_ALL
				m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL
				return result;
			}
		}
		return false;
	}
	void ModelViewer::setSteps(int steps)
	{
#ifdef TRACE_ALL
		std::wostringstream stream;
		stream << "setSteps.";
		auto eid = m_sqhLogs.ExcutionStart(stream, _class_Name, "setSteps");
#endif // TRACE_ALL

		if (m_pImpl)
			return m_pImpl->setSteps(steps);

#ifdef TRACE_ALL
		m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL
	}

	bool ModelViewer::zoomTo(void* node, bool withoutAnimation)
	{
		if (!m_pImpl)
			return false;
#ifdef TRACE_ALL
		std::wostringstream stream;
		stream << "zoomTo.";
		auto eid = m_sqhLogs.ExcutionStart(stream, _class_Name, "zoomTo");
#endif // TRACE_ALL

		bool result = m_pImpl->zoomTo(node, withoutAnimation);

#ifdef TRACE_ALL
		m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL
		return result;
	}

	bool ModelViewer::zoomTo(const std::string* nodes, int length, bool withoutAnimation)
	{
		if (!m_pImpl)
			return false;

#ifdef TRACE_ALL
		std::wostringstream stream;
		stream << "zoomTo.";
		auto eid = m_sqhLogs.ExcutionStart(stream, _class_Name, "zoomTo");
#endif // TRACE_ALL

		bool result = m_pImpl->zoomTo(nodes, length, withoutAnimation);

#ifdef TRACE_ALL
		m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL
		return result;
	}

	bool ModelViewer::zoomTo(void** nodes, int length, bool withoutAnimation)
	{
		if (!m_pImpl)
			return false;
#ifdef TRACE_ALL
		std::wostringstream stream;
		stream << "zoomTo.";
		auto eid = m_sqhLogs.ExcutionStart(stream, _class_Name, "zoomTo");
#endif // TRACE_ALL
		auto result = m_pImpl->zoomTo(nodes, length, withoutAnimation);
#ifdef TRACE_ALL
		m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL
		return result;
	}

	void ModelViewer::highlight(void* node)
	{
#ifdef TRACE_ALL
		std::wostringstream stream;
		stream << "highlight.";
		auto eid = m_sqhLogs.ExcutionStart(stream, _class_Name, "highlight");
#endif // TRACE_ALL
		if (m_pImpl)
			return m_pImpl->highlight(node);
#ifdef TRACE_ALL
		m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL
	}

	void ModelViewer::unHighlight(void* node)
	{
#ifdef TRACE_ALL
		std::wostringstream stream;
		stream << "unHighlight.";
		auto eid = m_sqhLogs.ExcutionStart(stream, _class_Name, "unHighlight");
#endif // TRACE_ALL
		if (m_pImpl)
			m_pImpl->unHighlight(node);
#ifdef TRACE_ALL
		m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL
	}

	void ModelViewer::setStateColorToConstructs(const std::string& nodeId, int red, int green, int blue)
	{
		if (!m_pImpl)
			return;

#ifdef TRACE_ALL
		std::wostringstream stream;
		stream << "setStateColorToConstructs.";
		stream << "nodeId:" << nodeId << ".";
		stream << "red:" << red << ".";
		stream << "green:" << green << ".";
		stream << "blue:" << blue << ".";
		auto eid = m_sqhLogs.ExcutionStart(stream, _class_Name, "setStateColorToConstructs");
#endif // TRACE_ALL

		m_pImpl->setStateColorToConstructs(findNodeById(nodeId), red, green, blue);

#ifdef TRACE_ALL
		m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL
	}

	void ModelViewer::setStateColorToConstructs(void* node, int red, int green, int blue)
	{
		if (!m_pImpl)
			return;

#ifdef TRACE_ALL
		std::wostringstream stream;
		stream << "setStateColorToConstructs.";
		stream << "red:" << red << ".";
		stream << "green:" << green << ".";
		stream << "blue:" << blue << ".";
		auto eid = m_sqhLogs.ExcutionStart(stream, _class_Name, "setStateColorToConstructs");
#endif // TRACE_ALL

		m_pImpl->setStateColorToConstructs(node, red, green, blue);

#ifdef TRACE_ALL
		m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL
	}

	bool ModelViewer::revertColorOfNode(const std::string& id)
	{
		if (!m_pImpl)
			return false;
#ifdef TRACE_ALL
		std::wostringstream stream;
		stream << "revertColorOfNode.";
		stream << "id:" << id << ".";
		auto eid = m_sqhLogs.ExcutionStart(stream, _class_Name, "revertColorOfNode");
#endif // TRACE_ALL
		auto node = findNodeById(id);
		if (!node)
		{

#ifdef TRACE_ALL
			m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL
			return false;
		}
		m_pImpl->revertColorOfNode(node);

#ifdef TRACE_ALL
		m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL

		return true;
	}

	bool ModelViewer::revertColorOfNode(void* node)
	{
		if (!m_pImpl)
			return false;
#ifdef TRACE_ALL
		std::wostringstream stream;
		stream << "revertColorOfNode.";
		auto eid = m_sqhLogs.ExcutionStart(stream, _class_Name, "revertColorOfNode");
#endif // TRACE_ALL
		m_pImpl->revertColorOfNode(node);

#ifdef TRACE_ALL
		m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL
		return true;
	}

	bool ModelViewer::revertColorOfNodes(void** nodes, int length)
	{
		if (!m_pImpl)
			return false;
		if (!nodes)
			return false;

#ifdef TRACE_ALL
		std::wostringstream stream;
		stream << "revertColorOfNodes.";
		stream << "length:" << length << ".";
		auto eid = m_sqhLogs.ExcutionStart(stream, _class_Name, "revertColorOfNodes");
#endif // TRACE_ALL
		for (int i = 0; i < length; i++)
		{
			m_pImpl->revertColorOfNode(nodes[i]);
		}

#ifdef TRACE_ALL
		m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL
		return true;
	}

	bool ModelViewer::revertColorOfNodes(const std::string* ids, int length)
	{
		if (!m_pImpl)
			return false;
		if (!ids)
			return false;

#ifdef TRACE_ALL
		std::wostringstream stream;
		stream << "revertColorOfNodes.";
		stream << "length:" << length << ".";
		auto eid = m_sqhLogs.ExcutionStart(stream, _class_Name, "revertColorOfNodes");
#endif // TRACE_ALL

		for (int i = 0; i < length; i++)
		{
			revertColorOfNode(ids[i]);
		}
#ifdef TRACE_ALL
		m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL
		return true;
	}

	void ModelViewer::transparentAll()
	{
		if (!m_pImpl)
			return;

#ifdef TRACE_ALL
		std::wostringstream stream;
		stream << "transparentAll.";
		auto eid = m_sqhLogs.ExcutionStart(stream, _class_Name, "transparentAll");
#endif // TRACE_ALL

		m_pImpl->transparentAll();

#ifdef TRACE_ALL
		m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL
	}

	void ModelViewer::unTransParency()
	{
		if (!m_pImpl)
			return;

#ifdef TRACE_ALL
		std::wostringstream stream;
		stream << "unTransParency.";
		auto eid = m_sqhLogs.ExcutionStart(stream, _class_Name, "unTransParency");
#endif // TRACE_ALL

		//if (!m_CategoryMaterialMap)
		//	return;
		m_pImpl->unTransParentAll();
		/*
				CategoryMaterialMap::const_iterator iterator = m_CategoryMaterialMap->begin(), end = m_CategoryMaterialMap->end();
				while (iterator != end)
				{
				setCategoryMaterial(iterator->second);
				++iterator;
				}

				m_pImpl->updateSeveralTimes();*/

#ifdef TRACE_ALL
		m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL
	}

	void ModelViewer::unTransparent(const std::string& id)
	{
		if (!m_pImpl)
			return;
#ifdef TRACE_ALL
		std::wostringstream stream;
		stream << "unTransparent.";
		stream << "id:" << id << ".";
		auto eid = m_sqhLogs.ExcutionStart(stream, _class_Name, "unTransparent");
#endif // TRACE_ALL
		m_pImpl->unTransparent(findNodeById(id));

#ifdef TRACE_ALL
		m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL
	}

	void ModelViewer::unTransparent(void* node)
	{
		if (!m_pImpl)
			return;
		if (!node)
			return;
#ifdef TRACE_ALL
		std::wostringstream stream;
		stream << "unTransparent.";
		auto eid = m_sqhLogs.ExcutionStart(stream, _class_Name, "unTransparent");
#endif // TRACE_ALL
		m_pImpl->unTransparent(node);

#ifdef TRACE_ALL
		m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL
	}

	void ModelViewer::setToTransparent(const std::string& id)
	{
		if (!m_pImpl)
			return;

#ifdef TRACE_ALL
		std::wostringstream stream;
		stream << "setToTransparent.";
		stream << "id:" << id << ".";
		auto eid = m_sqhLogs.ExcutionStart(stream, _class_Name, "setToTransparent");
#endif // TRACE_ALL

		auto node = static_cast<osg::Node*>(findNodeById(id));
		if (!node)
		{
#ifdef TRACE_ALL
			m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL

			return;
		}
		m_pImpl->setToTransparent(node);

#ifdef TRACE_ALL
		m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL
	}

	void ModelViewer::setToTransparent(void* ptr)
	{
		if (!m_pImpl)
			return;

#ifdef TRACE_ALL
		std::wostringstream stream;
		stream << "setToTransparent.";
		auto eid = m_sqhLogs.ExcutionStart(stream, _class_Name, "setToTransparent");
#endif // TRACE_ALL

		auto node = static_cast<osg::Node*>(ptr);
		if (!node)
		{
#ifdef TRACE_ALL
			m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL

			return;
		}
		m_pImpl->setToTransparent(node);

#ifdef TRACE_ALL
		m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL
	}

	void ModelViewer::highlight(const std::string& id)
	{
#ifdef TRACE_ALL
		std::wostringstream stream;
		stream << "highlight.";
		stream << "id:" << id << ".";
		auto eid = m_sqhLogs.ExcutionStart(stream, _class_Name, "highlight");
#endif // TRACE_ALL

		if (m_pImpl)
			highlight(m_pImpl->findNodeById(id));

#ifdef TRACE_ALL
		m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL
	}


	long long ModelViewer::getCurrentProgressInfo()
	{
		if (!m_pImpl)
			return -1;
		return m_pImpl->getCurrentProgressInfo();

	}
	void ModelViewer::unHighlight(const std::string& id)
	{
#ifdef TRACE_ALL
		std::wostringstream stream;
		stream << "unHighlight.";
		stream << "id:" << id << ".";
		auto eid = m_sqhLogs.ExcutionStart(stream, _class_Name, "unHighlight");
#endif // TRACE_ALL

		if (m_pImpl)
			unHighlight(m_pImpl->findNodeById(id));

#ifdef TRACE_ALL
		m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL
	}

	std::string  ModelViewer::findIdByNode(void* node)
	{
		if (!m_pImpl)
			return "";

#ifdef TRACE_ALL
		std::wostringstream stream;
		stream << "findIdByNode.";
		auto eid = m_sqhLogs.ExcutionStart(stream, _class_Name, "findIdByNode");
#endif // TRACE_ALL

		auto result = m_pImpl->findIdByNode((osg::Node*)node);

#ifdef TRACE_ALL
		m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL

		return result;
	}

	void* ModelViewer::findNodeById(const std::string& id)
	{
		if (!m_pImpl)
			return NULL;

#ifdef TRACE_ALL
		std::wostringstream stream;
		stream << "findNodeById.";
		auto eid = m_sqhLogs.ExcutionStart(stream, _class_Name, "findNodeById");
#endif // TRACE_ALL
		auto result = m_pImpl->findNodeById(id);

#ifdef TRACE_ALL
		m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL

		return result;
	}

	void ModelViewer::setViewerMode(ViewerMode viewerMode)
	{
#ifdef TRACE_ALL
		std::wostringstream stream;
		stream << "setViewerMode.";
		auto eid = m_sqhLogs.ExcutionStart(stream, _class_Name, "setViewerMode");
#endif // TRACE_ALL

		if (m_pImpl)
			m_pImpl->setViewerMode(viewerMode);

#ifdef TRACE_ALL
		m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL
	}

	void ModelViewer::insertImage(double x, double y, double z, const char *imageFile)
	{
#ifdef TRACE_ALL
		std::wostringstream stream;
		stream << "insertImage.";
		auto eid = m_sqhLogs.ExcutionStart(stream, _class_Name, "insertImage");
#endif // TRACE_ALL

		if (m_pImpl)
			m_pImpl->insertImage(x, y, z, imageFile);

#ifdef TRACE_ALL
		m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL
	}

	void ModelViewer::loadAll(const char *baseDir, const char *metaFileName)
	{
#ifdef TRACE_ALL
		std::wostringstream stream;
		stream << "loadAll.";
		auto eid = m_sqhLogs.ExcutionStart(stream, _class_Name, "loadAll");
#endif // TRACE_ALL

		if (m_pImpl)
		{
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
						if (!files.empty())
							m_pImpl->loadParam(&files[0], files.size());

						files.clear();
						for (size_t i = 0; i < modelFileNames.size(); ++i)
						{
							files.push_back(modelFileNames[i].c_str());
							//m_pImpl->loadBySingleFlr(modelFileNames[i].c_str(),0.0);
						}

						std::vector<double>						dEngineeringLevelvec;
						std::vector<std::pair<YZString, std::string> >	vecresut;
						m_pImpl->loadByMultiFlr(&files[0], files.size(), dEngineeringLevelvec, vecresut);

					}

				}
			}
		}

#ifdef TRACE_ALL
		m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL
	}

	//简化模型
	void SimplifierMode(double	simplify, osg::ref_ptr<osg::Group>& reorganized)
	{
		bool smooth = false;
		if (simplify > 0.0)
		{
			osgUtil::Simplifier simple;
			simple.setSmoothing(smooth);
			osg::notify(osg::ALWAYS) << " smoothing: " << smooth << std::endl;
			simple.setSampleRatio(simplify);
			reorganized->accept(simple);
		}

	}


	void Util::reoganizeByFloor(const char**	modelFiles,
		int				modelFileCount,
		const char **	paramFiles,
		int				paramFileCount,
		const char*		outputDir,
		double			simplify,
		bool			singleFile)
	{


		ModelViewerImpl m_pImpl;
		if (paramFileCount > 0)
		{
			m_pImpl.loadParam(paramFiles, paramFileCount);
		}

		osg::ref_ptr<osg::Node> root = m_pImpl.readFBXNode(modelFiles, modelFileCount);
		osg::ref_ptr<osg::Group> reorganized = m_pImpl.reorganize();

		if (reorganized)
		{
			SimplifierMode(simplify, reorganized);
			//整体输出
			if (singleFile)
			{
				std::string strOsg(outputDir);
				//std::ostringstream os;
				//os << outputDir << ".ive";

				//os << outputDir << ".osg";

				// Optimize the model
				osgUtil::Optimizer optimizer;
				optimizer.optimize(reorganized.get());
				optimizer.reset();
				strOsg += ".ive";
				m_pImpl.writeNode(reorganized, strOsg.c_str());

				//写.osg格式用于调试
				//strOsg+=".osg";
				//m_pImpl.writeNode(reorganized, strOsg.c_str());
				return;
			}
			//分楼层
			int numChild = reorganized->getNumChildren();

			std::map<YZString, YZString> IveNameToIvePath;
			//std::map<std::string,std::string> IveNameToIvePath;
			for (int i = 0; i < numChild; ++i)
			{

				osg::Node *child = reorganized->getChild(i);
				int floor;
				std::string strOsg(outputDir);
				if (child->getUserValue("floor", floor))
				{
					std::ostringstream os;
					//std::wostringstream os
					std::string stri(".ive");
#ifdef YIZHU_BIMWORLD_USE_WCHAR
					YZString strname = boost::locale::conv::to_utf<wchar_t>(child->getName(), "utf-8");
					YZString strpath = boost::locale::conv::to_utf<wchar_t>(outputDir + child->getName() + stri, "utf-8");
					YZString stronlypath = boost::locale::conv::to_utf<wchar_t>(outputDir, "utf-8");

					//m_pImpl.writeNode(child,  boost::locale::conv::from_utf(strpath, "utf-8").c_str());
					m_pImpl.writeNode(child, (Util::ws2s(stronlypath) + Util::ws2s(strname) + stri).c_str());

#else
					YZString strname = child->getName();
					YZString strpath = outputDir + child->getName() + stri;
					m_pImpl.writeNode(child, strpath.c_str());
#endif	

					IveNameToIvePath.insert(make_pair(strname, strpath));

					//IveNameToIvePath.insert(make_pair(boost::locale::conv::from_utf(str,  "utf-8"),os.str()));
					//IveNameToIvePath.insert(make_pair(strname,os.str()));

				}
			}

			{
				std::ostringstream os;
				os << outputDir << "index.xml";

				std::ofstream osf(os.str().c_str());
				PrjConfig::writePrjConfigToXmlStream(osf, IveNameToIvePath);

			}

		}
	}



	void Util::reoganizeFbxByFloor(const char**	modelFiles,
		int				modelFileCount,
		const char **	paramFiles,
		int				paramFileCount,
		const char*		outputDir,
		double			simplify,
		bool			singleFile)
	{


		ModelViewerImpl m_pImpl;
		if (paramFileCount > 0)
		{
			m_pImpl.loadParam(paramFiles, paramFileCount);
		}

		osg::ref_ptr<osg::Node> root = m_pImpl.readFBXNode(modelFiles, modelFileCount);
		osg::ref_ptr<osg::Group> reorganized = m_pImpl.reorganizeFbx();

		if (reorganized)
		{
			SimplifierMode(simplify, reorganized);
			//整体输出
			if (singleFile)
			{
				std::string strOsg(outputDir);
				//std::ostringstream os;
				//os << outputDir << ".ive";

				//os << outputDir << ".osg";

				// Optimize the model
				osgUtil::Optimizer optimizer;
				optimizer.optimize(reorganized.get());
				optimizer.reset();
				strOsg += ".ive";
				m_pImpl.writeNode(reorganized, strOsg.c_str());


				//m_pImpl.readIveNode(strOsg.c_str(), 1);
				//写.osg格式用于调试
				//strOsg+=".osg";
				//m_pImpl.writeNode(reorganized, strOsg.c_str());
				return;
			}
			//分楼层
			int numChild = reorganized->getNumChildren();

			std::map<YZString, YZString> IveNameToIvePath;
			//std::map<std::string,std::string> IveNameToIvePath;
			for (int i = 0; i < numChild; ++i)
			{

				osg::Node *child = reorganized->getChild(i);
				int floor;
				std::string strOsg(outputDir);
				if (child->getUserValue("floor", floor))
				{
					std::ostringstream os;
					//std::wostringstream os
					std::string stri(".ive");
#ifdef YIZHU_BIMWORLD_USE_WCHAR
					YZString strname = boost::locale::conv::to_utf<wchar_t>(child->getName(), "utf-8");
					YZString strpath = boost::locale::conv::to_utf<wchar_t>(outputDir + child->getName() + stri, "utf-8");
					YZString stronlypath = boost::locale::conv::to_utf<wchar_t>(outputDir, "utf-8");

					//m_pImpl.writeNode(child,  boost::locale::conv::from_utf(strpath, "utf-8").c_str());
					m_pImpl.writeNode(child, (Util::ws2s(stronlypath) + Util::ws2s(strname) + stri).c_str());

#else
					YZString strname = child->getName();
					YZString strpath = outputDir + child->getName() + stri;
					m_pImpl.writeNode(child, strpath.c_str());
#endif	

					IveNameToIvePath.insert(make_pair(strname, strpath));

					//IveNameToIvePath.insert(make_pair(boost::locale::conv::from_utf(str,  "utf-8"),os.str()));
					//IveNameToIvePath.insert(make_pair(strname,os.str()));

				}
			}

			{
				std::ostringstream os;
				os << outputDir << "index.xml";

				std::ofstream osf(os.str().c_str());
				PrjConfig::writePrjConfigToXmlStream(osf, IveNameToIvePath);

			}

		}
	}


	void ModelViewer::frame()
	{
#ifdef TRACE_ALL
		std::wostringstream stream;
		stream << "frame.";
		auto eid = m_sqhLogs.ExcutionStart(stream, _class_Name, "frame");
#endif // TRACE_ALL

		if (m_pImpl)
			m_pImpl->frame();

#ifdef TRACE_ALL
		m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL
	}

	// <创建默认图形设备特性参数>
	void* ModelViewer::createDefaultTraits()
	{
		if (m_pImpl)
		{
			return m_pImpl->createDefaultTraits();
		}
		else
		{
			osg::notify(osg::WARN) << "create default traits failed. The m_pImpl is null. in ModelViewer::createDefaultTraits()" << std::endl;
			return NULL;
		}
	}

	// <添加跟随相机>
	void ModelViewer::addSlaveCamera(void* traits)
	{
		if (!m_pImpl)
		{
			osg::notify(osg::WARN) << "Add slave camera failed. The m_pImpl is null. in ModelViewer::addSlaveCamera()" << std::endl;
			return;
		}
		auto tra = static_cast<osg::GraphicsContext::Traits*>(traits);
		if (!tra)
		{
			osg::notify(osg::WARN) << "Add slave camera failed. The traits is invalid. in ModelViewer::addSlaveCamera()" << std::endl;
			return;
		}
		m_pImpl->addSlaveCamera(tra);
	}

	void ModelViewer::addBillboard(std::string selectedConstruct[], const void(*callback)(BillboardData*, const void*), const char* img_path_name, const void* externalData)
	{
#ifdef TRACE_ALL
		std::wostringstream stream;
		stream << "addBillboard.";
		auto eid = m_sqhLogs.ExcutionStart(stream, _class_Name, "addBillboard");
#endif // TRACE_ALL
		m_pImpl->addBillboard(selectedConstruct, callback, img_path_name, externalData);

#ifdef TRACE_ALL
		m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL
	}

	void ModelViewer::setCheckMap(std::map<bimWorld::CategoryName, bimWorld::SystemType> xmlMap)
	{
#ifdef TRACE_ALL
		std::wostringstream stream;
		stream << "setCheckMap.";
		auto eid = m_sqhLogs.ExcutionStart(stream, _class_Name, "setCheckMap");
#endif // TRACE_ALL

		m_pImpl->xmlMap = xmlMap;

#ifdef TRACE_ALL
		m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL
	}

	//void ModelViewer::setCategoryMaterial(const CategoryName& categoryName, const SystemType& systemType, const LevelName& levelName, const MaterialSetting& newSetting, bool clear)
	//{
	//	if (!m_CategoryMaterialMap)
	//		return;
	//	MaterialSetting* materailSetting = m_CategoryMaterialMap->operator[](categoryName).materialSetting;
	//	materailSetting->assign(&newSetting, clear);
	//	if (m_pImpl)
	//		m_pImpl->setCategoryMaterial(categoryName, systemType, levelName, *materailSetting);
	//}

	void ModelViewer::setCategoryMaterial(const ColorCategory& colorCategory)
	{
		if (!m_CategoryMaterialMap)
			return;

#ifdef TRACE_ALL
		std::wostringstream stream;
		stream << "setCategoryMaterial.";
		auto eid = m_sqhLogs.ExcutionStart(stream, _class_Name, "setCategoryMaterial");
#endif // TRACE_ALL

		m_CategoryMaterialMap->operator[](colorCategory.name) = colorCategory;

		if (m_pImpl)
		{
			for (auto iter = colorCategory.levels.begin(); iter != colorCategory.levels.end(); iter++)
			{
				m_pImpl->setCategoryMaterial(colorCategory.ids, *colorCategory.materialSetting);
				//m_pImpl->setCategoryMaterial(colorCategory.name, colorCategory.systemtype, *iter, colorCategory.ids, *colorCategory.materialSetting);
			}
			m_pImpl->setCategoryAndSystemTypes(colorCategory.name, colorCategory.systemtype);
		}

#ifdef TRACE_ALL
		m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL
	}

	const MaterialSetting* ModelViewer::getCategoryMaterial(const CategoryName& categoryName) const
	{
		if (!m_CategoryMaterialMap)
			return NULL;

#ifdef TRACE_ALL
		std::wostringstream stream;
		stream << "getCategoryMaterial.";
		auto eid = m_sqhLogs.ExcutionStart(stream, _class_Name, "getCategoryMaterial");
#endif // TRACE_ALL

		MaterialSetting* result = NULL;
		CategoryMaterialMap::const_iterator pos = m_CategoryMaterialMap->find(categoryName);
		if (pos != m_CategoryMaterialMap->end())
			result = pos->second.materialSetting;

#ifdef TRACE_ALL
		m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL

		return result;
	}

	bool ModelViewer::readConfigFromFile(const char *fileName)
	{
#ifdef TRACE_ALL
		std::wostringstream stream;
		stream << "readConfigFromFile.";
		auto eid = m_sqhLogs.ExcutionStart(stream, _class_Name, "readConfigFromFile");
#endif // TRACE_ALL

		std::string content;
		auto result = readConfigFromContent(content.c_str());

#ifdef TRACE_ALL
		m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL

		return result;
	}

	bool ModelViewer::readConfigFromContent(const char *content)
	{
#ifdef TRACE_ALL
		std::wostringstream stream;
		stream << "readConfigFromContent.";
		auto eid = m_sqhLogs.ExcutionStart(stream, _class_Name, "readConfigFromContent");
#endif // TRACE_ALL

		TiXmlDocument doc;
		doc.Parse(content);

#ifdef TRACE_ALL
		m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL

		if (doc.Error())
			return false;
		return true;
	}

	bool ModelViewer::saveMaterialSettingToFile(const char *fileName)
	{
		if (!m_CategoryMaterialMap)
			return false;

#ifdef TRACE_ALL
		std::wostringstream stream;
		stream << "saveMaterialSettingToFile.";
		auto eid = m_sqhLogs.ExcutionStart(stream, _class_Name, "saveMaterialSettingToFile");
#endif // TRACE_ALL

		std::ofstream os(fileName);
		auto result = Util::materialSettingToStream(*m_CategoryMaterialMap, os);

#ifdef TRACE_ALL
		m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL

		return result;
	}

	void ModelViewer::setOtherNodeToTransparent(const std::string& id)
	{
		if (!m_pImpl)
		{
			return;
		}

#ifdef TRACE_ALL
		std::wostringstream stream;
		stream << "setOtherNodeToTransparent.";
		auto eid = m_sqhLogs.ExcutionStart(stream, _class_Name, "setOtherNodeToTransparent");
#endif // TRACE_ALL

		void* node = m_pImpl->findNodeById(id);

		if (!node)
		{
#ifdef TRACE_ALL
			m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL

			return;
		}

		m_pImpl->setOtherNodeToTransparent(node);

#ifdef TRACE_ALL
		m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL

	}

	void ModelViewer::setOtherNodeToTransparent(const std::string* ids, int length)
	{
		if (!m_pImpl || !ids)
		{
			return;
		}

#ifdef TRACE_ALL
		std::wostringstream stream;
		stream << "setOtherNodeToTransparent.";
		auto eid = m_sqhLogs.ExcutionStart(stream, _class_Name, "setOtherNodeToTransparent");
#endif // TRACE_ALL

		auto nodes = new void*[length];
		for (int i = 0; i < length; i++)
		{
			nodes[i] = m_pImpl->findNodeById(ids[i]);
		}
		m_pImpl->setOtherNodeToTransparent(nodes, length);

#ifdef TRACE_ALL
		m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL

	}

	bool ModelViewer::readMaterialSettingFromFile(const char *fileName)
	{
		//// <现在不读取颜色文件了，颜色存储在所属项目数据库中。>
		//return true;

#ifdef TRACE_ALL
		std::wostringstream stream;
		stream << "readMaterialSettingFromFile.";
		auto eid = m_sqhLogs.ExcutionStart(stream, _class_Name, "readMaterialSettingFromFile");
#endif // TRACE_ALL

		std::ifstream is(fileName);
		CategoryMaterialMap newSetting;
		m_CategoryMaterialMap = new CategoryMaterialMap();
		if (Util::materialSettingFromStream(newSetting, is))
		{
			//StoreRelationship().begin();
			CategoryMaterialMap::const_iterator iterator = newSetting.begin(), end = newSetting.end();
			while (iterator != end)
			{
				setCategoryMaterial(iterator->second);
				++iterator;
			}
			//StoreRelationship().commit();

#ifdef TRACE_ALL
			m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL
			return true;
		}

#ifdef TRACE_ALL
		m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL

		return false;
	}

	int ModelViewer::loadBySingleFlr(const char *files, double dEngineeringLevel, long& outFloorName)
	{
		if (!m_pImpl)
			return -1;

#ifdef TRACE_ALL
		std::wostringstream stream;
		stream << "loadBySingleFlr.";
		auto eid = m_sqhLogs.ExcutionStart(stream, _class_Name, "loadBySingleFlr");
#endif // TRACE_ALL

		int count = m_pImpl->loadBySingleFlr(files, dEngineeringLevel, outFloorName);

#ifdef TRACE_ALL
		m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL

		return count;
	}

	void ModelViewer::unLoadAll()
	{
		if (!m_pImpl)
		{
			return;
		}

#ifdef TRACE_ALL
		std::wostringstream stream;
		stream << "unLoadAll.";
		auto eid = m_sqhLogs.ExcutionStart(stream, _class_Name, "unLoadAll");
#endif // TRACE_ALL

		m_pImpl->unLoadAll();

#ifdef TRACE_ALL
		m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL

	}

	bool  ModelViewer::unLoadBySingleFlr(long floorNode)
	{
		if (!m_pImpl)
			return false;

		auto node = (osg::Node*)(floorNode);
		if (!node)
			return false;

#ifdef TRACE_ALL
		std::wostringstream stream;
		stream << "unLoadBySingleFlr.";
		auto eid = m_sqhLogs.ExcutionStart(stream, _class_Name, "unLoadBySingleFlr");
#endif // TRACE_ALL

		auto result = m_pImpl->unLoadSingleFlr(node);

#ifdef TRACE_ALL
		m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL

		return result;
	}
	int ModelViewer::loadByMultiFlr(const char								**files,
		int										ifileSize,
		std::vector<double>&					dEngineeringLevelvec,
		std::vector<std::pair<YZString, std::string> >&	vecresut)
	{
		if (!m_pImpl)
			return -1;

#ifdef TRACE_ALL
		std::wostringstream stream;
		stream << "loadByMultiFlr.";
		auto eid = m_sqhLogs.ExcutionStart(stream, _class_Name, "loadByMultiFlr");
#endif // TRACE_ALL

		int count = m_pImpl->loadByMultiFlr(files, ifileSize, dEngineeringLevelvec, vecresut);

#ifdef TRACE_ALL
		m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL

		return count;
	}

	bool ModelViewer::CaptureScreen(unsigned char** buffer, int & width, int & height, int & channels, int x /*= 0*/, int y /*= 0*/, const char* file /*= NULL*/)
	{
		if (!m_pImpl)
			return false;

#ifdef TRACE_ALL
		std::wostringstream stream;
		stream << "CaptureScreen.";
		auto eid = m_sqhLogs.ExcutionStart(stream, _class_Name, "CaptureScreen");
#endif // TRACE_ALL

		auto result = m_pImpl->CaptureScreen(buffer, width, height, channels, x, y, file);

#ifdef TRACE_ALL
		m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL

		return result;
	}

	void ModelViewer::setKeyFrame(int framePosition)
	{
		if (!m_pImpl)
			return;

#ifdef TRACE_ALL
		std::wostringstream stream;
		stream << "setKeyFrame.";
		auto eid = m_sqhLogs.ExcutionStart(stream, _class_Name, "setKeyFrame");
#endif // TRACE_ALL

		m_pImpl->setKeyFrame(framePosition);

#ifdef TRACE_ALL
		m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL

	}

	void ModelViewer::enableAnimationEditor()
	{
		if (!m_pImpl)
			return;

#ifdef TRACE_ALL
		std::wostringstream stream;
		stream << "enableAnimationEditor.";
		auto eid = m_sqhLogs.ExcutionStart(stream, _class_Name, "enableAnimationEditor");
#endif // TRACE_ALL

		m_pImpl->enableAnimationEditor();

#ifdef TRACE_ALL
		m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL

	}

	void ModelViewer::disableAnimationEditor()
	{
		if (!m_pImpl)
			return;

#ifdef TRACE_ALL
		std::wostringstream stream;
		stream << "disableAnimationEditor.";
		auto eid = m_sqhLogs.ExcutionStart(stream, _class_Name, "disableAnimationEditor");
#endif // TRACE_ALL

		m_pImpl->disableAnimationEditor();

#ifdef TRACE_ALL
		m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL
	}

	void ModelViewer::setAnimatingNodes(void* node)
	{
		if (!m_pImpl)
			return;
		auto n = static_cast<osg::Node*>(node);
		if (!n)
			return;

#ifdef TRACE_ALL
		std::wostringstream stream;
		stream << "setAnimatingNodes.";
		auto eid = m_sqhLogs.ExcutionStart(stream, _class_Name, "setAnimatingNodes");
#endif // TRACE_ALL

		m_pImpl->setAnimatingNodes(n);

#ifdef TRACE_ALL
		m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL
	}

	void ModelViewer::setAnimatingNodes(std::vector<void*> ptrs)
	{
		if (!m_pImpl)
			return;

#ifdef TRACE_ALL
		std::wostringstream stream;
		stream << "setAnimatingNodes.";
		auto eid = m_sqhLogs.ExcutionStart(stream, _class_Name, "setAnimatingNodes");
#endif // TRACE_ALL

		std::vector<osg::Node*> nodes;
		for (auto iter = ptrs.begin(); iter != ptrs.end(); iter++)
		{
			auto node = static_cast<osg::Node*>(*iter);
			if (!node)
				continue;
			nodes.push_back(node);
		}
		m_pImpl->setAnimatingNodes(nodes);

#ifdef TRACE_ALL
		m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL
	}

	void ModelViewer::addActionToSequece(int red, int green, int blue, const std::string& id, double delayMilliseconds)
	{
		if (!m_pImpl)
			return;

#ifdef TRACE_ALL
		std::wostringstream stream;
		stream << "addActionToSequece.";
		auto eid = m_sqhLogs.ExcutionStart(stream, _class_Name, "addActionToSequece");
#endif // TRACE_ALL

		int delta = 500;
		int times = delayMilliseconds / delta;
		for (int i = 2; i < times; i += 2)
		{
			m_pImpl->addActionToSequece([=](){
				this->hide(id);
				setStateColorToConstructs(id, red, green, blue);
			}, delta);

			m_pImpl->addActionToSequece([=](){
				this->unHide(id);
			}, delta);
		}
		m_pImpl->addActionToSequece([=](){
			this->hide(id);
			this->revertColorOfNode(id);
		}, delta);
		m_pImpl->addActionToSequece([=](){
			this->unHide(id);
		}, delta);

#ifdef TRACE_ALL
		m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL
	}

	void ModelViewer::addActionToSequece(int red, int green, int blue, std::vector<std::string> ids, double delayMilliseconds)
	{
		if (!m_pImpl)
			return;

#ifdef TRACE_ALL
		std::wostringstream stream;
		stream << "addActionToSequece.";
		auto eid = m_sqhLogs.ExcutionStart(stream, _class_Name, "addActionToSequece");
#endif // TRACE_ALL

		int delta = 500;
		int times = delayMilliseconds / delta;
		for (int i = 2; i < times; i += 2)
		{
			m_pImpl->addActionToSequece([=](){
				for (auto iter = ids.begin(); iter != ids.end(); iter++)
				{
					this->hide(*iter);
					if (i == 2)
					{
						this->unTransparent(*iter);
					}
					setStateColorToConstructs(*iter, red, green, blue);

				}
			}, delta);

			m_pImpl->addActionToSequece([=](){
				for (auto iter = ids.begin(); iter != ids.end(); iter++)
				{
					this->unHide(*iter);
				}
			}, delta);
		}
		m_pImpl->addActionToSequece([=](){
			for (auto iter = ids.begin(); iter != ids.end(); iter++)
			{
				this->hide(*iter);
				this->revertColorOfNode(*iter);
				this->setToTransparent(*iter);
			}
		}, delta);
		m_pImpl->addActionToSequece([=](){
			for (auto iter = ids.begin(); iter != ids.end(); iter++)
			{
				this->unHide(*iter);
			}
		}, delta);


#ifdef TRACE_ALL
		m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL
	}

	void ModelViewer::clearActionSequece()
	{
		if (!m_pImpl)
			return;

#ifdef TRACE_ALL
		std::wostringstream stream;
		stream << "clearActionSequece.";
		auto eid = m_sqhLogs.ExcutionStart(stream, _class_Name, "clearActionSequece");
#endif // TRACE_ALL

		m_pImpl->clearActionSequece();

#ifdef TRACE_ALL
		m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL
	}

	void ModelViewer::play()
	{
		if (!m_pImpl)
			return;

#ifdef TRACE_ALL
		std::wostringstream stream;
		stream << "play.";
		auto eid = m_sqhLogs.ExcutionStart(stream, _class_Name, "play");
#endif // TRACE_ALL

		m_pImpl->play();

#ifdef TRACE_ALL
		m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL
	}

	void ModelViewer::stop()
	{
		if (!m_pImpl)
			return;

#ifdef TRACE_ALL
		std::wostringstream stream;
		stream << "stop.";
		auto eid = m_sqhLogs.ExcutionStart(stream, _class_Name, "stop");
#endif // TRACE_ALL

		m_pImpl->stop();

#ifdef TRACE_ALL
		m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL
	}

	void ModelViewer::playKeyFrame(int framePosition)
	{
		if (!m_pImpl)
			return;

#ifdef TRACE_ALL
		std::wostringstream stream;
		stream << "playKeyFrame.";
		auto eid = m_sqhLogs.ExcutionStart(stream, _class_Name, "playKeyFrame");
#endif // TRACE_ALL

		m_pImpl->playKeyFrame(framePosition);

#ifdef TRACE_ALL
		m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL
	}

	void ModelViewer::beginRecordAnimationControlPoint()
	{
		if (!m_pImpl)
			return;

#ifdef TRACE_ALL
		std::wostringstream stream;
		stream << "beginRecordAnimationControlPoint.";
		auto eid = m_sqhLogs.ExcutionStart(stream, _class_Name, "beginRecordAnimationControlPoint");
#endif // TRACE_ALL

		m_pImpl->beginRecordAnimationControlPoint();

#ifdef TRACE_ALL
		m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL
	}

	void ModelViewer::endRecordAnimationControlPoint()
	{
		if (!m_pImpl)
			return;

#ifdef TRACE_ALL
		std::wostringstream stream;
		stream << "endRecordAnimationControlPoint.";
		auto eid = m_sqhLogs.ExcutionStart(stream, _class_Name, "endRecordAnimationControlPoint");
#endif // TRACE_ALL

		m_pImpl->endRecordAnimationControlPoint();

#ifdef TRACE_ALL
		m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL
	}

	void ModelViewer::applyAnimationPath(const std::string& entityId)
	{
		if (!m_pImpl)
		{
			return;
		}

#ifdef TRACE_ALL
		std::wostringstream stream;
		stream << "applyAnimationPath.";
		auto eid = m_sqhLogs.ExcutionStart(stream, _class_Name, "applyAnimationPath");
#endif // TRACE_ALL

		auto node = static_cast<osg::Node*>(findNodeById(entityId));
		if (node)
		{
			m_pImpl->applyAnimationPath(node);
		}

#ifdef TRACE_ALL
		m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL
	}

	void ModelViewer::setSkyBox(const std::string& right, const std::string& left, const std::string& front, const std::string& back, const std::string& up, const std::string& down)
	{
		if (!m_pImpl)
			return;

#ifdef TRACE_ALL
		std::wostringstream stream;
		stream << "setSkyBox.";
		auto eid = m_sqhLogs.ExcutionStart(stream, _class_Name, "setSkyBox");
#endif // TRACE_ALL

		m_pImpl->setSkyBox(right, left, front, back, up, down);

#ifdef TRACE_ALL
		m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL
	}

	void ModelViewer::setSkyBox(const std::string& hdr)
	{
		if (!m_pImpl)
			return;

#ifdef TRACE_ALL
		std::wostringstream stream;
		stream << "setSkyBox.";
		auto eid = m_sqhLogs.ExcutionStart(stream, _class_Name, "setSkyBox");
#endif // TRACE_ALL

		m_pImpl->setSkyBox(hdr);

#ifdef TRACE_ALL
		m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL
	}

	void ModelViewer::revertBackgroundColor()
	{
		if (!m_pImpl)
		{
			return;
		}

#ifdef TRACE_ALL
		std::wostringstream stream;
		stream << "revertBackgroundColor.";
		auto eid = m_sqhLogs.ExcutionStart(stream, _class_Name, "revertBackgroundColor");
#endif // TRACE_ALL

		m_pImpl->revertBackgroundColor();

#ifdef TRACE_ALL
		m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL
	}

	bool ModelViewer::getBackgroundColor(int& red, int& green, int& blue, int& alpha)
	{
		if (!m_pImpl)
		{
			return false;
		}
		osg::Vec4f bgColor;

#ifdef TRACE_ALL
		std::wostringstream stream;
		stream << "getBackgroundColor.";
		auto eid = m_sqhLogs.ExcutionStart(stream, _class_Name, "getBackgroundColor");
#endif // TRACE_ALL

		bool result = m_pImpl->getBackgroundColor(bgColor);
		if (!result)
		{
#ifdef TRACE_ALL
			m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL
			return false;
		}
		red = (int)(bgColor.r() * 255);
		green = (int)(bgColor.g() * 255);
		blue = (int)(bgColor.b() * 255);
		alpha = (int)(bgColor.a() * 255);

#ifdef TRACE_ALL
		m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL
		return true;
	}

	void ModelViewer::setBackgroundColor(int red, int green, int blue, int alpha)
	{
		if (!m_pImpl)
		{
			return;
		}

#ifdef TRACE_ALL
		std::wostringstream stream;
		stream << "setBackgroundColor.";
		auto eid = m_sqhLogs.ExcutionStart(stream, _class_Name, "setBackgroundColor");
#endif // TRACE_ALL

		m_pImpl->setBackgroundColor(osg::Vec4f(red / 255.0f, green / 255.0f, blue / 255.0f, alpha / 255.0f));

#ifdef TRACE_ALL
		m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL
	}

	void ModelViewer::setCurrentDBPath(const std::string& path)
	{
		if (!m_pImpl)
		{
			return;
		}

#ifdef TRACE_ALL
		std::wostringstream stream;
		stream << "setCurrentDBPath.";
		auto eid = m_sqhLogs.ExcutionStart(stream, _class_Name, "setCurrentDBPath");
#endif // TRACE_ALL

		m_pImpl->setCurrentDBPath(path);

#ifdef TRACE_ALL
		m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL
	}

	bool Util::materialSettingToStream(const CategoryMaterialMap& materialSetting, std::ostream& out)
	{
		return materialSetting.writeAsXmlToStream(out);
	}

	bool Util::materialSettingFromStream(CategoryMaterialMap& materialSetting, std::istream& in)
	{
		return materialSetting.readAsXmlFromStream(in);
	}
	template<class T, class Func>
	int executeCommand(int argc, T *argv[])
	{

		//_sleep(20*1000);

		using namespace boost::program_options;
		options_description desc;
		desc.add_options()
			("parameter,p", wvalue< std::vector<std::wstring> >(),
			"list of parameter files")
			("model,m", wvalue< std::vector<std::wstring> >(),
			"list of model files")
			("output-dir,o", wvalue<std::wstring>(),
			"output directory")
			("simplify,s", value<double>(), "simplification level")
			("all-in-one,a", "indicates if all levels should go into one files");
		variables_map vm;
		store(Func(argc, argv).options(desc).run(), vm);
		//store(command_line_parsercommand_line_parser(argc, argv).
		//options(desc).positional(p).run(), vm);
		notify(vm);
		if (vm.count("model") && vm.count("output-dir"))
		{
			double simplification = 0.0;

			std::vector<const char*> models;
			std::vector<const char*> params;
			std::vector<std::wstring> w_modelFiles = vm["model"].as<std::vector<std::wstring> >();
			const std::vector<std::string>& modelFiles = Util::wVec2sVec(w_modelFiles);
			std::vector<std::wstring> w_paramFiles;
			std::vector<std::string> paramFiles;
			for (int i = 0; i < modelFiles.size(); ++i)
			{
				models.push_back(modelFiles[i].c_str());
			}
			if (vm.count("parameter"))
			{
				w_paramFiles = vm["parameter"].as<std::vector<std::wstring> >();
				paramFiles = Util::wVec2sVec(w_paramFiles);
				for (int i = 0; i < paramFiles.size(); ++i)
				{
					params.push_back(paramFiles[i].c_str());
				}
			}
			if (vm.count("simplify"))
				simplification = vm["simplify"].as<double>();

			bimWorld::Util::reoganizeFbxByFloor(&models[0],
				models.size(),
				(params.empty() ? NULL : &params[0]),
				params.size(),
				Util::ws2s(vm["output-dir"].as<std::wstring>()).c_str(),
				simplification,
				vm.count("all-in-one"));
			return 0;
		}
		return -1;
	}
	int Util::executeCommand(int argc, char *argv[])
	{
		return bimWorld::executeCommand<char, boost::program_options::command_line_parser>(argc, argv);
	}
#ifdef YIZHU_BIMWORLD_USE_WCHAR
	int Util::executeCommand(int argc, wchar_t *argv[])
	{
		return bimWorld::executeCommand<wchar_t, boost::program_options::wcommand_line_parser>(argc, argv);
	}
#endif
}
