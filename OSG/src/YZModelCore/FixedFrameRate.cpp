#include "FixedFrameRate.h"
#include "YZModelCoreInternal.h"

bimWorld::FixedFrameRate::FixedFrameRate(bimWorld::YZModelCoreInternal* host) :
bimWorld::YZModelCoreModule(host)
{

}

void bimWorld::FixedFrameRate::setupFrameRateController(osg::Node *root)
{
#if _WIN32||IOS_USING_FIXED_FRAMERATE
	if (root == NULL)
	{
		fixedFramerateController.clearNodeStats();
		return;
	}
	bimWorld::CollectNodeStats cns;
	cns.osg::NodeVisitor::apply(*root);
	fixedFramerateController.setNodeStats(cns.nodeStats);
#endif
}

void bimWorld::FixedFrameRate::cancelFixedFramerate()
{
#if USING_FIXED_FRAMERATE
	if (fixedFramerateController.isValid())
		fixedFramerateController.unHide();
#endif
}

void bimWorld::FixedFrameRate::startFixedFramerate()
{
#if USING_FIXED_FRAMERATE
	if (fixedFramerateController.isValid())
		fixedFramerateController.hide();
#endif
}

