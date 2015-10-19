#include "ModelViewerIOS.h"
#include "ModelViewerImpl.h"
#include "CameraManipulators.h"
#define OSG_LIBRARY_STATIC
#include <osgViewer/GraphicsWindow>
USE_GRAPICSWINDOW_IMPLEMENTATION(IOS)
#include <osgViewer/api/IOS/GraphicsWindowIOS>
#include <osgDB/FileUtils>
#include <osgGA/MultiTouchTrackballManipulator>
USE_OSGPLUGIN(ive)

  namespace{

#include <osg/Notify>
    struct SetNotifyLevel{
      SetNotifyLevel(){
        //osg::setNotifyLevel(osg::DEBUG_INFO);
      }
    }instance;
    const char *defaultVertexShader =
      "attribute vec4 position;\n\
      attribute vec3 normal;\n\
      attribute vec4 color;\n\
      \n\
      varying lowp vec4 colorVarying;\n\
      \n\
      uniform mat4 osg_modelviewprojectionmatrix;\n\
      uniform mat3 osg_normalmatrix;\n\
      void main()\n\
      {\n\
        \n\
          //don't want specicail lighting calculations,\
          //it also not working under gles2\
          //vec3 eyenormal = normalize(osg_normalmatrix * normal);\n\
          //vec3 lightposition = vec3(1.0, 1.0, 1.0);\n\
          //vec4 diffusecolor = vec4(0.4, 0.4, 1.0, 1.0);\n\
          //\n\
          //float ndotvp = max(0.5, dot(eyenormal, normalize(lightposition)));\n\
          //\n\
          //colorvarying = diffusecolor * ndotvp;\n\
          colorVarying = color;\n\
          gl_Position = osg_ModelViewProjectionMatrix * position;\n\
          \n\
      }\n\
    ";
    const char *defaultFragmentShader =
      "varying lowp vec4 colorVarying;\n\
      \n\
      void main()\n\
      {\n\
        gl_FragColor = colorVarying;\n\
      }";
    class SetupDefaultShader : public osg::NodeVisitor{
      osg::Program *program;
      osg::Program* getProgram(){
        if(program == NULL){
          program = new osg::Program;
          program->addBindAttribLocation("position", 0);
          program->addBindAttribLocation("color", 3);
          program->addBindAttribLocation("normal", 2);
          program->addShader(new osg::Shader(osg::Shader::VERTEX, defaultVertexShader));
          program->addShader(new osg::Shader(osg::Shader::FRAGMENT, defaultFragmentShader));
        }
        return program;
      }
      public:
      SetupDefaultShader()  : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN),
      program(NULL){
      }
      void setShader(osg::StateSet *stateSet){
        if(stateSet){
          osg::Program *program = getProgram();
          stateSet->setAttribute(program, osg::StateAttribute::ON);
          stateSet->setAttributeAndModes(program, osg::StateAttribute::ON);
        }
      }

      void apply(osg::Node &node){
        setShader(node.getStateSet());
        traverse(node);
      }

      void apply(osg::Geode &geode){
        setShader(geode.getStateSet());
        for(unsigned int i=0; i<geode.getNumDrawables(); ++i){
          osg::Drawable *drawable = geode.getDrawable(i);
          setShader(drawable->getStateSet());
          //drawable->setColor(osg::Vec4(1.0, 0.0, 1.0, 1.0));
        }
      }
    };
    
    class ModelViewerImplIOS : public bimWorld::ModelViewerImpl{
      UIView *view;
      bool useGLES2, fixedFrameRate;
      public:
      ModelViewerImplIOS(UIView *view, bool useGLES2 = false, bool fixedFrameRate = false)
        : view(view), useGLES2(useGLES2), fixedFrameRate(fixedFrameRate){
        }
      void setupShader(osg::Node *node){
        if(useGLES2 && node != NULL){
          setupShader(*node);
        }
      }
      void setupShader(osg::Node& node){
        NSLog(@"setting up shaders...");
        SetupDefaultShader sds;
        node.setStateSet(getSelectionRenderModel());
        osg::StateSet* stateset = node.getOrCreateStateSet();
        //stateset->setColor(osg::Vec4(0.5, 0.5, 0.5,1));
        sds.apply(node);
      }
      virtual void initOSG(){
        if(m_sceneRoot == NULL){
          m_sceneRoot = new osg::Group();
          if(useGLES2)
            setupShader(m_sceneRoot);
        }
        bimWorld::ModelViewerImpl::initOSG();
      }
      virtual void initGraphicContext(osg::GraphicsContext::Traits* traits){
        // Init the Windata Variable that holds the handle for the Window to display OSG in.
        osg::ref_ptr<osg::Referenced> windata = new osgViewer::GraphicsWindowIOS::WindowData(view);
        traits->x = view.bounds.origin.x;
        traits->y = view.bounds.origin.y;
#if TARGET_IPHONE_SIMULATOR
        traits->width = view.bounds.size.width;
        traits->height = view.bounds.size.height;
#else
        traits->width = view.bounds.size.width * 2;
        traits->height = view.bounds.size.height * 2;
#endif
        traits->inheritedWindowData = windata;
      }
      osgGA::CameraManipulator* createCameraManipulator(){
        return new bimWorld::BIMCameraManipulator(this, true, 0.02);
      }
        virtual osg::Node* readNode(const char *file){
        try{
          const char *ext = std::strrchr(file, '.');
          ext = (ext == NULL ? file : ext + 1);
          osgDB::ReaderWriter*  reader =
            osgDB::Registry::instance()->getReaderWriterForExtension(ext);
          if(reader == NULL){
            NSLog(@"cannot find reader writer for %s", ext);
            NSLog(@"read file %s failed", file);
            return NULL;
          }
          osgDB::ReaderWriter::ReadResult result =
            reader->readNode(file);
          if(!result.success()){
            NSLog(@"read file %s failed", file);
            NSLog(@"message %s", result.message().c_str());
            return NULL;
          }
          NSLog(@"read file %s succeed", file);
          // Optimize the model
          osgUtil::Optimizer optimizer;
          optimizer.optimize(result.getNode());
          optimizer.reset();
          setupShader(result.getNode());
          return result.takeNode();
        }catch(...){
          NSLog(@"exception read file %s", file);
          return NULL;
        }
      }
      virtual void loadParam(const char *file){
        std::string fileName = osgDB::findDataFile(file);
        if(fileName.empty()){
          NSLog(@"cannot read parameter file %s", file);
          return;
        }
        bimWorld::ModelViewerImpl::loadParam(fileName.c_str());
      }
      virtual osg::ref_ptr<osg::Group> reorganize(){
        osg::Group* root = bimWorld::ModelViewerImpl::reorganize();
        setupShader(root);
        return root;
      }
    };
    bimWorld::ModelViewerImpl* fromView(UIView *window,
        bool useGLES2 = false, bool fixedFrameRate = false){
      bimWorld::ModelViewerImpl* impl = new ModelViewerImplIOS(window, useGLES2, fixedFrameRate);
      impl->initOSG();
      return impl;
    }
  }
namespace bimWorld{
  ModelViewerIOS::ModelViewerIOS(UIView *window,
      bool useGLES2, bool fixedFrameRate) : ModelViewer(fromView(window, useGLES2, fixedFrameRate)){
  }
  bool ModelViewerIOS::readMaterialSettingFromFile(const char *file){
    std::string fileName = osgDB::findDataFile(file);
    if(fileName.empty()){
      NSLog(@"cannot find material file %s", file);
      return false;
    }
      return ModelViewer::readMaterialSettingFromFile(fileName.c_str());
  }
}
