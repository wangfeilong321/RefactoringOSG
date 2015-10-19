#include "RenderingThread.h"
#include "CameraManipulators.h"
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif
#include "YZModelCoreInternal.h"
#include <thread>

namespace bimWorld
{
	CRenderingThread::CRenderingThread(YZModelCoreInternal* ptr)
		: OpenThreads::Thread(), YZModelCoreModule(ptr), /*m_done(false), */m_currentBlockFrame(0), m_totalBlockFrame(0), m_hasBlockAnimation(false){
		m_needProcess = false;
		m_delayProcess = 0;
		m_delayPauseProcess = 10;
		m_isExternalRendering = false;
		m_isRendering = false;
	}

	CRenderingThread::~CRenderingThread()
	{
		//m_done = true;
		m_host->_ViewerData()->setDone(true);

		while (isRunning())
			OpenThreads::Thread::YieldCurrentThread();
	}

	void CRenderingThread::setNeedUpdateAndDraw(bool value, int delay)
	{
		OpenThreads::ScopedLock<OpenThreads::Mutex> lock(m_mutex);

		m_needProcess = value;
		if (m_needProcess)
		{
			m_delayProcess = delay;
			m_delayPauseProcess = 0;
		}
		else
		{
			// 至少再渲染一帧，确保固定帧率取消后更新一帧。
			if (delay == 0)
				delay = 1;
			m_delayPauseProcess = delay;
			m_delayProcess = 0;
		}

	}

	bool CRenderingThread::getNeedUpdateAndDraw()
	{
		return m_needProcess;
	}

	void CRenderingThread::setIsExternalRendering(bool value)
	{
		OpenThreads::ScopedLock<OpenThreads::Mutex> lock(m_mutex);

		m_isExternalRendering = value;

		lock.~ScopedLock();

		int sleeptime = 0;
		while (m_isRendering && sleeptime < 700) {
			sleeptime++;
#ifdef _WIN32
			Sleep(1);
#else
			usleep(1000);
#endif
		}
		if (sleeptime > 0)
			osg::notify(osg::DEBUG_INFO) << "sleep time: " << sleeptime << " milliseconds." << std::endl;
	}

	void CRenderingThread::updateSeveralTimes(int times)
	{
		if (m_needProcess)
			return;
		// 在非绘制阶段调用本函数，执行之后，
		// 会进入 m_delayPauseProcess 减一操作，所以绘制次数需要加一
		if (m_delayPauseProcess < times)
			m_delayPauseProcess = times + 1;
	}

	void CRenderingThread::run()
	{
		if (!m_host)
		{
			//m_done = true;
			return;
		}

#ifndef Android
		bool hasDrawn = false;

		osgViewer::ViewerBase* viewer = m_host->_ViewerData()->ModelViewer();
		do
		{
			auto timer = std::thread([](){
				std::this_thread::sleep_for(std::chrono::microseconds(33));
			});
			auto draw = std::thread([this, &hasDrawn](){
				hasDrawn = loop(hasDrawn);
			});
			draw.join();
			//m_babortRendering = true;
			//auto start = clock();
			timer.join();
			//auto end = clock();
			//double delay = (double)(end - start)/* / CLOCKS_PER_SEC*/;
			//if (delay>10)
			//{
			//	std::cerr << "delay Run time: " << delay << "s" << std::endl;
			//}
		} while (!testCancel() && !viewer->done() && !m_host->ViewerData()->isDone());
#endif // !Android

	}

	void CRenderingThread::frame(bool isRenderingThread)
	{
		OpenThreads::ScopedLock<OpenThreads::Mutex> lock(m_mutex);
		if (!isRenderingThread)
		{
			setIsExternalRendering(true);
		}

		m_babortRendering = false;
		m_host->_ViewerData()->ModelViewer()->frame();

		if (!isRenderingThread)
			setIsExternalRendering(false);
	}

	core::Event<void(std::chrono::system_clock::time_point)>& CRenderingThread::PreFrameUpdateEvent()
	{
		return m_preFrameUpdateEvent;
	}

	core::Event<void(std::chrono::system_clock::time_point)>& CRenderingThread::PostFrameUpdateEvent()
	{
		return m_postFrameUpdateEvent;
	}

	core::Event<void(std::chrono::system_clock::time_point)>& CRenderingThread::NoneFrameUpdateEvent()
	{
		return m_noneFrameUpdateEvent;
	}

	void CRenderingThread::onlyUpdateFrame()
	{
		if (!m_host)
			return;
		m_host->_ViewerData()->ModelViewer()->advance();
		m_host->_ViewerData()->ModelViewer()->eventTraversal();
		m_host->_ViewerData()->ModelViewer()->updateTraversal();
		//m_host->Animation()->update();
		osgViewer::ViewerBase* viewer = m_host->_ViewerData()->ModelViewer();
		core_Event_Trigger(m_noneFrameUpdateEvent, std::chrono::system_clock::now());
	}

	void CRenderingThread::bindAbortValue(osg::State* state)
	{
		state->setAbortRenderingPtr(&m_babortRendering);
	}

	void CRenderingThread::setBlockAnimation(unsigned int totalFrame, core::InstanceFunction<void(float)> pre, core::InstanceFunction<void(float)> post)
	{
		if (m_hasBlockAnimation)
		{
			assert(false);
			return;
		}
		m_hasBlockAnimation = true;
		m_totalBlockFrame = totalFrame;
		m_currentBlockFrame = 1;
		m_preBlockUpdateEvent += pre;
		m_postBlockUpdateEvent += post;

		std::function<void(float)> func = [this, pre, post](float){
			if (m_currentBlockFrame > m_totalBlockFrame)
			{
				auto preEv = core::FunctionKey(pre.getInstance(), pre.getFuncName());
				m_preBlockUpdateEvent -= preEv;
				auto postEv = core::FunctionKey(post.getInstance(), post.getFuncName());
				m_postBlockUpdateEvent -= postEv;
				auto postEv2 = core::FunctionKey(this, "removeBlockAnimation");
				m_postBlockUpdateEvent -= postEv2;
				m_hasBlockAnimation = false;
				m_currentBlockFrame = 0;
				m_totalBlockFrame = 0;
			}
		};
		core::InstanceFunction<void(float)> insfunc(func, this, "removeBlockAnimation");
		m_postBlockUpdateEvent += insfunc;
	}

	bool CRenderingThread::loop(bool hasDrawn/* = false*/)
	{
		if (testCancel() || m_host->ViewerData()->isDone())
			return false;

		osgViewer::ViewerBase* viewer = m_host->_ViewerData()->ModelViewer();

		if (viewer->done())
		{
			return false;
		}

		//double startTime = viewer->elapsedTime();

		if (m_isExternalRendering)
		{
			YieldCurrentThread();
			//#ifdef _WIN32
			//			Sleep(33);
			//#else
			//			usleep(33000);
			//#endif
		}
		else if (m_hasBlockAnimation)
		{
			OpenThreads::ScopedLock<OpenThreads::Mutex> lock(m_mutex);
			m_isRendering = true;
			core_Event_Trigger(m_preBlockUpdateEvent, m_currentBlockFrame);
			m_babortRendering = false;
			frame(true);
			//m_host->_ViewerData()->ModelViewer()->renderingTraversals();
			++m_currentBlockFrame;
			//core_Event_Trigger(m_postBlockUpdateEvent, ++m_currentBlockFrame);
			m_isRendering = false;
			lock.~ScopedLock();
			hasDrawn = true;
		}
		else if ((m_needProcess && m_delayProcess == 0) || (!m_needProcess && m_delayPauseProcess > 1))
		{
			OpenThreads::ScopedLock<OpenThreads::Mutex> lock(m_mutex);
			m_isRendering = true;
			core_Event_Trigger(m_preFrameUpdateEvent, std::chrono::system_clock::now());
			frame(true);
			core_Event_Trigger(m_postFrameUpdateEvent, std::chrono::system_clock::now());
			m_isRendering = false;
			lock.~ScopedLock();
			hasDrawn = true;
		}
		else if (!m_needProcess && m_delayPauseProcess == 1)
		{
			OpenThreads::ScopedLock<OpenThreads::Mutex> lock(m_mutex);
			auto viewer = m_host->_ViewerData()->ModelViewer();
			if (viewer && m_host && m_host->_ViewerData()->isLoaded())
			{
				m_host->_FixedFrameRate()->cancelFixedFramerate();
			}
			m_isRendering = true;
			core_Event_Trigger(m_preFrameUpdateEvent, std::chrono::system_clock::now());
			frame(true);
			core_Event_Trigger(m_postFrameUpdateEvent, std::chrono::system_clock::now());
			m_isRendering = false;
			lock.~ScopedLock();
			hasDrawn = true;
		}
		else
		{
			OpenThreads::ScopedLock<OpenThreads::Mutex> lock(m_mutex);
			onlyUpdateFrame();
			lock.~ScopedLock();
			YieldCurrentThread();
		}

		//double endTime = viewer->elapsedTime();

		OpenThreads::ScopedLock<OpenThreads::Mutex> lock(m_mutex);
		if (m_delayProcess > 0)
			m_delayProcess--;
		if (m_delayPauseProcess > 0)
			m_delayPauseProcess--;

		//if (!m_hasBlockAnimation)
		//{
		//	onlyUpdateFrame();
		//}
		return hasDrawn;
	}

}