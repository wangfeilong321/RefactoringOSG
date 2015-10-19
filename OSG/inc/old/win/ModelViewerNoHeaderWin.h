#ifndef __MODEL_VIEWER_WIN_H__
#define __MODEL_VIEWER_WIN_H__
#include "../ModelViewer.h"

namespace bimWorld
{
  class YIZHU_EXPORT ModelViewerNoHeaderWin : public ModelViewer
  {
		ModelViewerImpl* fromHandle(void* hWnd);
    public:
		ModelViewerNoHeaderWin(void* hWnd) : ModelViewer(fromHandle(hWnd))
		{

		}

		static void renderControlBackground(ModelViewer* viewer, void* handle);
  };
}
#endif // __MODEL_VIEWER_WIN_H__
