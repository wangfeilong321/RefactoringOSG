#include "Animations.h"

std::shared_ptr<bimWorld::Animation::IKeyFrameAnimation> bimWorld::Animations::KeyFrameAnimation()
{
	if (!m_keyFrameAnimation)
	{
		m_keyFrameAnimation = std::make_shared<bimWorld::Animation::KeyFrameAnimation>();
	}
	return m_keyFrameAnimation;
}

std::shared_ptr<bimWorld::Animation::ICameraAnimationPath> bimWorld::Animations::CameraPathAnimation()
{
	if (!m_cameraPathAnimation)
	{
		m_cameraPathAnimation = std::make_shared<bimWorld::Animation::CameraAnimationPath>();
	}
	return m_cameraPathAnimation;
}
