#ifndef __MODEL_VIEWER_WIN_H__
#define __MODEL_VIEWER_WIN_H__
#include "../ModelViewer.h"
#include <Windows.h>

namespace bimWorld
{
  class YIZHU_EXPORT ModelViewerWin : public ModelViewer
  {
		ModelViewerImpl* fromHandle(HWND hWnd);
    public:
		ModelViewerWin(HWND hWnd) : ModelViewer(fromHandle(hWnd))
		{

		}
  };
}
#endif // __MODEL_VIEWER_WIN_H__
