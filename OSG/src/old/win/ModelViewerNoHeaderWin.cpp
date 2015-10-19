#include "ModelViewerNoHeaderWin.h"
#include "../ModelViewerImpl.h"
#include <osgViewer\api\Win32\GraphicsWindowWin32>
#include <Windows.h>

namespace{
  class ModelViewerImplWin : public bimWorld::ModelViewerImpl{
    HWND m_hWnd;
    public:
    ModelViewerImplWin(void* hWnd) : m_hWnd((HWND)hWnd){
   }
    virtual void initGraphicContext(osg::GraphicsContext::Traits* traits){
      // Local Variable to hold window size data
      RECT rect;

      // Get the current window size
      ::GetWindowRect(m_hWnd, &rect);
      // Init the Windata Variable that holds the handle for the Window to display OSG in.
      osg::ref_ptr<osg::Referenced> windata = new osgViewer::GraphicsWindowWin32::WindowData(m_hWnd);
      traits->width = rect.right - rect.left;
      traits->height = rect.bottom - rect.top;
      traits->inheritedWindowData = windata;
    }
  };
}
namespace bimWorld
{
	ModelViewerImpl* ModelViewerNoHeaderWin::fromHandle(void* hWnd)
  {
    ModelViewerImpl* m_pImpl = new ModelViewerImplWin((HWND)hWnd);
    m_pImpl->initOSG();
    return m_pImpl;
  }
	void ModelViewerNoHeaderWin::renderControlBackground(ModelViewer* viewer, void* handle)
	{
		HWND hWnd = (HWND)handle;
		osg::GraphicsContext::Traits* traits = static_cast<osg::GraphicsContext::Traits*>(viewer->createDefaultTraits());
		// Local Variable to hold window size data
		RECT rect;

		// Get the current window size
		::GetWindowRect(hWnd, &rect);
		// Init the Win data Variable that holds the handle for the Window to display OSG in.
		osg::ref_ptr<osg::Referenced> windata = new osgViewer::GraphicsWindowWin32::WindowData(hWnd);
		traits->width = rect.right - rect.left;
		traits->height = rect.bottom - rect.top;
		traits->inheritedWindowData = windata;
		viewer->addSlaveCamera(traits);
	}

}
