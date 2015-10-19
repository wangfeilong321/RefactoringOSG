#include "AnimationFactory.h"
#include "Animations.h"

std::shared_ptr<bimWorld::IAnimations> bimWorld::AniamtionFactory::getModule()
{
	return std::make_shared<bimWorld::Animations>();
}
