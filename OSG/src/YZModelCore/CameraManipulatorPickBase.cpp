#include "YZModelCoreInternal.h"
#include "CameraManipulatorPickBase.h"
#include "VectorExt.h"

bimWorld::CameraManipulatoerPickBase::CameraManipulatoerPickBase(YZModelCoreInternal *host, double selectionPrecision /*= 1e-6*/) :_mX(0.), _mY(0.), YZModelCoreModule(host), lineSegmentSelect(false), mode(Orbit), selectionPrecision(selectionPrecision)
{

}

bool bimWorld::CameraManipulatoerPickBase::polytopePick(const double x, const double y, osgViewer::Viewer* viewer)
{
	if (!viewer->getSceneData() || m_host == NULL)
		return false;
	using namespace std;
	double xMin = min(_mX, x), xMax = max(_mX, x);
	double yMin = min(_mY, y), yMax = max(_mY, y);
	osgUtil::PolytopeIntersector* picker =
		new osgUtil::PolytopeIntersector(osgUtil::Intersector::PROJECTION, xMin, yMin, xMax, yMax);
	picker->setIntersectionLimit(osgUtil::Intersector::LIMIT_ONE_PER_DRAWABLE);
	osgUtil::IntersectionVisitor iv(picker);

	auto camera = viewer->getCamera();
	camera->accept(iv);

	if (picker->containsIntersections())
	{
		std::vector<void*> selectedConstruct;
		osgUtil::PolytopeIntersector::Intersections::iterator begin = picker->getIntersections().begin(),
			end = picker->getIntersections().end();
		while (begin != end)
		{
			const osg::NodePath& nodePath = begin->nodePath;
			unsigned int idx = nodePath.size();
			while (idx--)
			{
				if (m_host->_ViewerData()->isTopMostNode(nodePath[idx]))
				{
					m_host->_Billboard()->triggerBillboardCallback(nodePath[idx]);
					return false;
				}
				auto node = nodePath[idx];
				if (!node)
				{
					continue;
				}
				bool visibility;
				if (node->getUserValue(NODE_VISIBILITY, visibility))
				{
					if (!visibility)
					{
						continue;
					}
				}
				if (m_host->_ViewerData()->_isComponentFunc)
				{
					if (!m_host->_ViewerData()->_isComponentFunc(node))
						continue;
				}
				if(!util::VectorExtension::contains(selectedConstruct, (void*)node))
				{
					selectedConstruct.push_back(node);
				}
				if (mode == bimWorld::MultiSelect)
				{
					//m_host->CameraManipulator()->setModelCenter(nodePath[idx]);
					m_host->_NodeControl()->highlight(nodePath[idx]);
				}
				break;
			}
			++begin;
		}
		if (!selectedConstruct.empty())
		{
			m_host->ViewerData()->SelectNodeEvent().for_each([selectedConstruct, this](decltype(m_host->ViewerData()->SelectNodeEvent()._eventType) callback){callback(selectedConstruct, mode); });
			//core_Event_Trigger(m_host->ViewerData()->SelectNodeEvent(), selectedConstruct);
			//fireSelectionEvent(m_host->_CameraManipulator()->getListeners(), selectedConstruct.data(), selectedConstruct.size());
			// TODO: to be removed
			//m_pImpl->addBillboard(selectedConstruct.data(), NULL, "E:\\WorkSpace\\OSG_FILE_PATH\\thumbtack.png");
		}
	}
	return false;
}

bool bimWorld::CameraManipulatoerPickBase::lineSegmentPick(osgViewer::View* view, const osgGA::GUIEventAdapter& ea)
{
	osgUtil::LineSegmentIntersector::Intersections intersections;

	if (view->computeIntersections(ea, intersections))
	{
		std::vector<void*> selectedConstruct;
		for (osgUtil::LineSegmentIntersector::Intersections::iterator hitr = intersections.begin(); hitr != intersections.end(); ++hitr)
		{
			if (!hitr->nodePath.empty())
			{
				unsigned int idx = hitr->nodePath.size();
				while (idx--)
				{
					auto node = hitr->nodePath[idx];
					if (node)
					{
						selectedConstruct.push_back(node);
						break;

					}
				}
			}
		}
		if (!selectedConstruct.empty())
		{
			//core_Event_Trigger(m_host->ViewerData()->SelectNodeEvent(), selectedConstruct);
			//fireSelectionEvent(m_host->_CameraManipulator()->getListeners(), selectedConstruct.data(), selectedConstruct.size());
		}
	}
	return false;
}

bool bimWorld::CameraManipulatoerPickBase::handleMouseDrag(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& /*us*/)
{
	if (!m_host || !m_host->_ViewerData()->isLoaded())
		return false;

	if (mode == bimWorld::MultiSelect)
	{
		if (selectionRange)
		{
			osg::Vec3Array* vertices = selectionRangeVertices;
			double _nX = ea.getXnormalized(), _nY = ea.getYnormalized();
			(*vertices)[1].set(_mX, _nY, 0);
			(*vertices)[2].set(_nX, _nY, 0);
			(*vertices)[3].set(_nX, _mY, 0);
			selectionRangeGeometry->dirtyDisplayList();
			selectionRangeGeometry->dirtyBound();
		}
		return true;
	}

	return false;
}

bool bimWorld::CameraManipulatoerPickBase::handleMousePush(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& /*us*/)
{
	if (!m_host || !m_host->_ViewerData()->isLoaded())
		return false;

	_mX = ea.getXnormalized(), _mY = ea.getYnormalized();
	if (mode == bimWorld::MultiSelect)
	{
		if (m_host->_ViewerData()->getWindowSpace())
		{
			osg::Geode* geode = new osg::Geode();
			osg::Vec3Array* vertices = new osg::Vec3Array(5);
			osg::Geometry* linesGeom = new osg::Geometry();
			osg::Vec4Array* colors = new osg::Vec4Array;
			(*vertices)[0].set(_mX, _mY, 0);
			(*vertices)[1].set(_mX, _mY, 0);
			(*vertices)[2].set(_mX, _mY, 0);
			(*vertices)[3].set(_mX, _mY, 0);
			(*vertices)[4].set(_mX, _mY, 0);
			linesGeom->setVertexArray(vertices);
			linesGeom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_STRIP, 0, 5));
			colors->push_back(osg::Vec4(1.0f, 1.0f, 0.0f, 1.0f));
			linesGeom->setColorArray(colors, osg::Array::BIND_OVERALL);
			geode->addDrawable(linesGeom);
			m_host->_ViewerData()->getWindowSpace()->addChild(geode);
			selectionRange = geode;
			selectionRangeVertices = vertices;
			selectionRangeGeometry = linesGeom;
		}
		return true;
	}
	//if (fixedFramerateController.hide())
	//	m_pImpl->setAbortRendering();

	return false;
}

bool bimWorld::CameraManipulatoerPickBase::handleMouseRelease(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us)
{
	if (!m_host || !m_host->_ViewerData()->isLoaded())
		return false;

	if (ea.getButton() != osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
		return false;

	osgViewer::Viewer* viewer = dynamic_cast<osgViewer::Viewer*>(&us);
	if (!viewer)
	{
		return false;
	}
	if (selectionRange)
	{
		if (m_host->_ViewerData()->getWindowSpace())
			m_host->_ViewerData()->getWindowSpace()->removeChild(selectionRange);
		selectionRange = NULL;
		selectionRangeVertices = NULL;
		selectionRangeGeometry = NULL;
	}

	switch (mode)
	{
	case bimWorld::MultiSelect:
		return polytopePick(ea.getXnormalized(), ea.getYnormalized(), viewer);
	default:
		//using namespace std;
		//if (max(fabs(ea.getYnormalized() - _mY), fabs(ea.getXnormalized() - _mX)) < selectionPrecision)
		//{
		//	_mX = ea.getXnormalized() - 0.005, _mY = ea.getYnormalized() - 0.005;
		// 旋转换右键，左键单选，所以拖动左键也可以单选，以鼠标抬起为准。
		_mX = ea.getXnormalized(), _mY = ea.getYnormalized();
		return lineSegmentSelect ? lineSegmentPick(viewer, ea) : polytopePick(ea.getXnormalized(), ea.getYnormalized(), viewer);
		//}
		return false;
	}
	return true;
}

void bimWorld::CameraManipulatoerPickBase::setMode(bimWorld::ViewerMode mode)
{
	this->mode = mode;
}

bool bimWorld::CameraManipulatoerPickBase::handleMouseMove(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& /*us*/)
{
	if (!m_host || !m_host->_ViewerData()->isLoaded())
		return false;
	_mX = ea.getXnormalized(), _mY = ea.getYnormalized();
	return false;
}
