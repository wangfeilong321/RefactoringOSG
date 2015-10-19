#include "ModelCoreFactory.h"
#include "YZModelCoreInternal.h"
#ifdef _WIN32
#include <osgViewer\api\Win32\GraphicsWindowWin32>


std::shared_ptr<bimWorld::IModelCore> bimWorld::ModelCoreFactory::getModule(HWND hwnd)
{
	auto core = std::make_shared<YZModelCoreInternal>();
	core->_ViewerData()->initOSG([hwnd](osg::GraphicsContext::Traits* traits)
	{
		// Local Variable to hold window size data
		RECT rect;

		// Get the current window size
		::GetWindowRect(hwnd, &rect);
		// Init the win data Variable that holds the handle for the Window to display OSG in.
		osg::ref_ptr<osg::Referenced> windata = new osgViewer::GraphicsWindowWin32::WindowData(hwnd);
		traits->width = rect.right - rect.left;
		traits->height = rect.bottom - rect.top;
		traits->inheritedWindowData = windata;
	});
	return core;
}
#else

std::shared_ptr<bimWorld::IModelCore> bimWorld::ModelCoreFactory::getModule(int x,int y,int width,int height)
{
	auto core = std::make_shared<YZModelCoreInternal>();
	core->_ViewerData()->initOSG(x,y,width, height);
	return core;
}

#endif // _WIN32
