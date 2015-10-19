
#include "Point.h"
#include "Rect.h"
#include "DraggerManipulator.h"
#include "DragSelectionManipulator.h"
#include "MouseManipulatorBase.h"
#include "MouseManipulatorContainer.h"
#include "AnimationEditor.h"
#include "AnimationChain.h"
#include "FixedPathAnimation.h"

using namespace bimWorld::Manipulator;
using namespace bimWorld::GraphicsStructure;
using namespace bimWorld::Animation;

void test()
{
	Point p;
	Rect rect;
	DraggerManipulator* dragger = new DraggerManipulator();
	DragSelectionManipulator* selection = new DragSelectionManipulator();
	MouseManipulatorContainer* container = new MouseManipulatorContainer();
	AnimationChain* chain = new AnimationChain();
	AnimationEditor* editor = new AnimationEditor();
	FixedPathAnimation* fixedAnim = new FixedPathAnimation();
	fixedAnim->Continue();
}