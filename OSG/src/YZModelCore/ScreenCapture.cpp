#include "ScreenCapture.h"
#include "YZModelCoreInternal.h"
#include "ScreenCaptureHandler.h"

bimWorld::ScreenCapture::ScreenCapture(YZModelCoreInternal* host) :
YZModelCoreModule(host)
{

}

bool bimWorld::ScreenCapture::CaptureScreen(unsigned char** buffer, int & width, int & height, int & channels, int x, int y, const char* file)
{
#if _WIN32 && TEST_CAPTURE_SCREEN
	osg::Image* shot = new osg::Image();
	shot->allocateImage(width, height, 24, GL_RGB, GL_UNSIGNED_BYTE);
	camera->attach(osg::Camera::COLOR_BUFFER, shot);
	osgDB::writeImageFile(*shot, "image_file.png");
	// <若要获得屏幕截图，必需利用Viewer渲染一个帧(frame)>
	return true;
#endif

	std::mutex _lock;
	_lock.lock();
	m_host->_RenderingThreads()->setIsExternalRendering(true);

	auto originalThreadingModel = m_host->_ViewerData()->ModelViewer()->getThreadingModel();

	m_host->_ViewerData()->ModelViewer()->setThreadingModel(osgViewer::Viewer::SingleThreaded);



	//        m_mViewer->frame();

	auto camera = m_host->_ViewerData()->ModelViewer()->getCamera();

	if (width == 0 || height == 0)
	{
		width = camera->getViewport()->width();
		height = camera->getViewport()->height();
	}
	int length;

	//        OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_mutex);

	std::cerr << "W : " << width << "H : " << height;
	auto gc = camera->getGraphicsContext();
	if (!gc)
		return false;

	if (!gc->getTraits())
		return false;

	if (!gc->isCurrent())
		gc->makeCurrent();

	GLenum pixelFormat;

	if (gc->getTraits()->alpha)
		pixelFormat = GL_RGBA;
	else
		pixelFormat = GL_RGB;

#if defined(OSG_GLES1_AVAILABLE) || defined(OSG_GLES2_AVAILABLE)
	if (pixelFormat == GL_RGB)
	{
		GLint value = 0;
#ifndef GL_IMPLEMENTATION_COLOR_READ_FORMAT
#define GL_IMPLEMENTATION_COLOR_READ_FORMAT 0x8B9B
#endif
		glGetIntegerv(GL_IMPLEMENTATION_COLOR_READ_FORMAT, &value);
		if (value != GL_RGB ||
			value != GL_UNSIGNED_BYTE)
		{
			pixelFormat = GL_RGBA;//always supported
		}
	}
#endif

	// set channels.
	if (pixelFormat == GL_RGBA)
		channels = 4;
	else if (pixelFormat == GL_RGB)
		channels = 3;
	else
	{
		osg::notify(osg::FATAL) << "The pixel format is not usual, may be get failed." << std::endl;
		return false;
	}

	// Align the border, 8 bytes divisible
	auto alignWidth = width;
	auto remainder = alignWidth * channels % 8;
	if (remainder > 0)
		alignWidth += (8 - remainder);

	length = (alignWidth)* (height)* (channels);

	auto buf = new unsigned char[length];

	osg::ref_ptr<ScreenCaptureHandler> handler = new ScreenCaptureHandler(file, &buf, width, height, channels, pixelFormat, x, y);

	m_host->_ViewerData()->ModelViewer()->getCamera()->setFinalDrawCallback(handler.get());
	m_host->_ViewerData()->ModelViewer()->renderingTraversals();
	m_host->_ViewerData()->ModelViewer()->getCamera()->setFinalDrawCallback(NULL);

	//		handler->capture(m_mViewer->getCamera());

	m_host->_ViewerData()->ModelViewer()->setThreadingModel(originalThreadingModel);

	m_host->_RenderingThreads()->setIsExternalRendering(false);

	// judge whether it is a black picture.
	for (int i = 0; i < width*height*channels; i++)
	{
		if ((int)(buf)[i] != 0)
		{
			if (channels == 4 && ((i + 1) % 4 == 0))
			{
				continue;
			}
			else
			{
				*buffer = buf;
				_lock.unlock();
				return true;
			}
		}
	}

	*buffer = buf;
	_lock.unlock();
	return false;
}
