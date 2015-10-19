//#include "FixedPathAnimation.h"
//#include "osg/Node"
//
//
//void bimWorld::Animation::FixedPathAnimation::SetLoopMode(osg::AnimationPath::LoopMode loopmode /*= osg::AnimationPath::LOOP*/)
//{
//	m_path->setLoopMode(loopmode);
//}
//
//void bimWorld::Animation::FixedPathAnimation::AddControlPoint(double time, osg::AnimationPath::ControlPoint ctrlPoint)
//{
//	m_path->insert(time, ctrlPoint);
//}
//
//void bimWorld::Animation::FixedPathAnimation::AddControlPoint(double time)
//{
//	m_path->insert(time, osg::AnimationPath::ControlPoint());
//}
//
//void bimWorld::Animation::FixedPathAnimation::AddControlPoint(double time, const osg::Vec3d& position)
//{
//	m_path->insert(time, osg::AnimationPath::ControlPoint(position));
//}
//
//void bimWorld::Animation::FixedPathAnimation::AddControlPoint(double time, const osg::Vec3d& position, const osg::Quat& rotation)
//{
//	m_path->insert(time, osg::AnimationPath::ControlPoint(position, rotation));
//}
//
//void bimWorld::Animation::FixedPathAnimation::AddControlPoint(double time, const osg::Vec3d& position, const osg::Quat& rotation, const osg::Vec3d& scale)
//{
//	m_path->insert(time, osg::AnimationPath::ControlPoint(position, rotation, scale));
//}
//
//void bimWorld::Animation::FixedPathAnimation::Apply(osg::Node* node)
//{
//	m_aniCallback->setAnimationPath(m_path);
//	node->setUpdateCallback(m_aniCallback.get());
//	hasApplied = true;
//}
//
//void bimWorld::Animation::FixedPathAnimation::Reset()
//{
//	if (!hasApplied)
//	{
//		osg::notify(osg::WARN) << "Need to apply first. in FixedPathAnimation::Reset";
//		return;
//	}
//	m_aniCallback->reset();
//}
//
//void bimWorld::Animation::FixedPathAnimation::Pause()
//{
//	if (!hasApplied)
//	{
//		osg::notify(osg::WARN) << "Need to apply first. in FixedPathAnimation::Pause";
//		return;
//	}
//	m_aniCallback->setPause(true);
//}
//
//void bimWorld::Animation::FixedPathAnimation::Continue()
//{
//	if (!hasApplied)
//	{
//		osg::notify(osg::WARN) << "Need to apply first. in FixedPathAnimation::Continue";
//		return;
//	}
//	m_aniCallback->setPause(false);
//}
//
//void bimWorld::Animation::FixedPathAnimation::SetTimeOffset(double time)
//{
//	if (!hasApplied)
//	{
//		osg::notify(osg::WARN) << "Need to apply first. in FixedPathAnimation::SetTimeOffset";
//		return;
//	}
//	m_aniCallback->setTimeOffset(time);
//}
//
//void bimWorld::Animation::FixedPathAnimation::SetTimeMultiplier(double multiple)
//{
//	if (!hasApplied)
//	{
//		osg::notify(osg::WARN) << "Need to apply first. in FixedPathAnimation::SetTimeMultiplier";
//		return;
//	}
//	m_aniCallback->setTimeMultiplier(multiple);
//}
//
//void bimWorld::Animation::FixedPathAnimation::Clear()
//{
//	m_path->clear();
//	m_createPointManip->clear();
//}
//
//void bimWorld::Animation::FixedPathAnimation::drawLines(const std::vector<osg::Vec3>& points)
//{
//	osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array(points.size(), points.data());
//	m_geom->setVertexArray(vertices);
//	m_geom->removePrimitiveSet(0, m_geom->getNumPrimitiveSets());
//	m_geom->addPrimitiveSet(new osg::DrawArrays(GL_LINE_STRIP, 0, points.size()));
//}
