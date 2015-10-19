#include "ZoomEntityModel.h"
#include "YZModelEntityInternal.h"

bimWorld::ZoomEntityModel::ZoomEntityModel(YZModelEntityInternal* host) :
ZoomModelWrapper(host)
{

}

bool bimWorld::ZoomEntityModel::zoomTo(const std::string& id, bool withoutAnimation /*= true*/)
{
	return m_controller->zoomTo(m_host->ControlByData()->findNodeById(id), withoutAnimation);
}

bool bimWorld::ZoomEntityModel::zoomTo(const std::vector<std::string>& ids, bool withoutAnimation /*= true*/, ViewDirection direction)
{
	if (ids.size() < 1)
		return false;
	auto length = ids.size();
#pragma region Calculate Zoom Parameter

	CameraPerspective pers;
	bool got = m_host->_modelCore->ViewerData()->getCameraPerspective(pers);
	if (!got)
	{
		osg::notify(osg::FATAL) << "Get camera perspective failed, in zoomTo(void* nodes[] ... ) at ModelViewerImpl." << std::endl;
		return false;
	}
	float radius = 0;
	osg::Vec3 center;
	osg::Node* node;
	float centerCalcus = 0; // <计算center时，计算了几次。除以此参数以获得中点。>

	osg::Vec3 minPos;
	osg::Vec3 maxPos(0, 0, 0);
	bool isFirstSetMin = true;

	for (size_t i = 0; i < length; i++)
	{
		auto n = static_cast<osg::Node*>(m_host->ControlByData()->findNodeById(ids[i]));
		if (!n)
		{
			osg::notify(osg::FATAL) << "The sub node is null, but we continued. In ModelViewerImpl::zoomTo(void* nodes[] ... )." << std::endl;
			continue;
		}

#if defined(USING_BOUNDING_SPHERE) // defined(OSG_GLES1_AVAILABLE) || defined(OSG_GLES2_AVAILABLE)
		//auto transMatrix = n->getWorldMatrices()[0];
		auto sphere = n->getBound();
		double temp_radius = sphere.radius();
		//osg::Vec3 world = pos * worldMatrix;
		auto temp_center = sphere.center();
#else
		// TODO: to be implemented.
		assert(false);
#endif

		auto tmaxX = temp_center.x() + temp_radius;
		auto tmaxY = temp_center.y() + temp_radius;
		auto tmaxZ = temp_center.z() + temp_radius;

		auto tminX = temp_center.x() - temp_radius;
		auto tminY = temp_center.y() - temp_radius;
		auto tminZ = temp_center.z() - temp_radius;

		tmaxX = (maxPos.x() < tmaxX) ? tmaxX : maxPos.x();
		tmaxY = (maxPos.y() < tmaxY) ? tmaxY : maxPos.y();
		tmaxZ = (maxPos.z() < tmaxZ) ? tmaxZ : maxPos.z();
		if (!isFirstSetMin)
		{
			tminX = (minPos.x() > tminX) ? tminX : minPos.x();
			tminY = (minPos.y() > tminY) ? tminY : minPos.y();
			tminZ = (minPos.z() > tminZ) ? tminZ : minPos.z();
		}
		maxPos.set(tmaxX, tmaxY, tmaxZ);
		minPos.set(tminX, tminY, tminZ);

		center += temp_center * temp_radius;
		centerCalcus += temp_radius;
	}
	if (centerCalcus == 0)
	{
		centerCalcus = 1;
		osg::notify(osg::FATAL) << "The center calcus is zero. It is invalid. in ModelViewerImpl::zoomTo(void* nodes[] ... )." << std::endl;
	}
	center /= centerCalcus;

#pragma region Calcu Radius

	for (size_t i = 0; i < length; i++)
	{
		osg::Node *n = static_cast<osg::Node*>(m_host->ControlByData()->findNodeById(ids[i]));
		if (!n)
		{
			osg::notify(osg::FATAL) << "The sub node is null, but we continued. In ModelViewerImpl::zoomTo(void* nodes[] ... )." << std::endl;
			continue;
		}

#if defined(USING_BOUNDING_SPHERE)
		auto sphere = n->getBound();
		auto temp_center = sphere.center();
		double temp_radius = sphere.radius();
		//temp_radius = (center - temp_center).length() + temp_radius;
#else
		// TODO: to be implemented.
		assert(false);
#endif

		if (temp_radius > radius)
		{
			node = n;
			//radius = temp_radius;
		}
	}

	radius = (maxPos - minPos).length() / 2;

#pragma endregion


	auto tx = radius / std::tan(pers.fov / 2.0 / 180.0 * 3.14159265358979323846);

	osg::Matrix rotation = osg::Matrix::rotate(osg::DegreesToRadians(90.0), osg::Vec3d(1, 0, 0));
	osg::Vec3d position;
	osg::Vec3d up;
	switch (direction)
	{
	case ViewDirection::Front:
		up = osg::Vec3d(0, 1, 0);
		position = osg::Vec3d(0, 0, 0 + tx);
		break;
	case ViewDirection::Back:
		up = osg::Vec3d(0, 1, 0);
		position = osg::Vec3d(0, 0, 0 - tx);
		break;
	case ViewDirection::Left:
		up = osg::Vec3d(0, 1, 0);
		position = osg::Vec3d(0 - tx, 0, 0);
		break;
	case ViewDirection::Right:
		up = osg::Vec3d(0, 1, 0);
		position = osg::Vec3d(0 + tx, 0, 0);
		break;
	case ViewDirection::Top:
		up = osg::Vec3d(0, 0, -1);
		position = osg::Vec3d(0, 0 + tx, 0);
		break;
	case ViewDirection::Bottom:
		up = osg::Vec3d(0, 0, 1);
		position = osg::Vec3d(0, 0 - tx, 0);
		break;
	default:
		break;
	}

	up = rotation.preMult(up);
	position = rotation.preMult(position);
	position += rotation.preMult(center);

	osg::Node* n = node;
	do
	{
		osg::MatrixTransform *mt = ((n == node) ? NULL : dynamic_cast<osg::MatrixTransform*>(n));
		if (mt != NULL) {
			center = mt->getMatrix().preMult(center);
			position = mt->getMatrix().preMult(position);
		}
	} while (n->getNumParents() == 1 && (n = n->getParent(0)));

#pragma endregion

	if (withoutAnimation)
	{
		m_host->_modelCore->_CameraManipulator()->getCameraManipulator()->setHomePosition(position, center, up, false);
		m_host->_modelCore->_ViewerData()->ModelViewer()->home();
		//m_zoomtimes++;
	}
	else
	{
		osg::Vec3d frontVector = position - center;
		frontVector.normalize();
		osg::Vec3d newposition = center + frontVector*radius*2.5;
		m_host->_modelCore->_CameraManipulator()->CameraAnimation()->play(newposition, center, up);

	}

	// we need refresh at last.
	m_host->_modelCore->RenderingThreads()->updateSeveralTimes(1);
	return true;
}
