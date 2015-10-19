#ifndef MODEL_VIEWER_WIN_H
#define MODEL_VIEWER_WIN_H
#include "../ModelViewer.h"
#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

namespace bimWorld{
  class ModelViewerIOS : public ModelViewer{
    public:
      ModelViewerIOS(UIView *view,
          bool isGLES2 = false, bool fixedFrameRate = false);
      
      bool readMaterialSettingFromFile(const char *fileName);
  };
}
#endif
