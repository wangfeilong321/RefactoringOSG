#include "ModelViewerWin.h"
#include "../ModelViewerImpl.h"
#include <osgViewer\api\Win32\GraphicsWindowWin32>
namespace{
  class ModelViewerImplWin : public bimWorld::ModelViewerImpl{
    HWND m_hWnd;
    public:
    ModelViewerImplWin(HWND hWnd) : m_hWnd(hWnd){
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
  ModelViewerImpl* ModelViewerWin::fromHandle(HWND hWnd)
  {
    ModelViewerImpl* m_pImpl = new ModelViewerImplWin(hWnd);
    m_pImpl->initOSG();
    return m_pImpl;
  }
}
