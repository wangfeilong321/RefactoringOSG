#ifndef __C_RENDERING_THREAD_H__
#define __C_RENDERING_THREAD_H__

#include <osg/Material>
#include <osg/StateSet>
#include <osgUtil/Optimizer>
#include <osg/Group>
#include <osg/BlendFunc>
#include <map>
#include "YZModelCoreModule.h"
#include "Event.h"
#include <atomic>
#include <chrono>

namespace bimWorld
{
	class CRenderingThread :
		public osg::Referenced, public OpenThreads::Thread, public YZModelCoreModule
	{
#ifdef Android
	public:
#else
	protected:
#endif
		// <渲染循环>
		bool loop(bool hasDrawn = false);

	public:
		CRenderingThread(YZModelCoreInternal* host);

		virtual ~CRenderingThread();

		virtual void run();

		//
		virtual void frame(bool isRenderingThread);
		
		void setNeedUpdateAndDraw(bool value, int delay = 0);

		bool getNeedUpdateAndDraw();
        
        void setIsExternalRendering(bool value);

		// <刷新>
		void updateSeveralTimes(int times = 3);

		// <绘制前更新事件>
		core::Event<void(std::chrono::system_clock::time_point)>& PreFrameUpdateEvent();

		// <绘制后更新事件>
		core::Event<void(std::chrono::system_clock::time_point)>& PostFrameUpdateEvent();

		// <无绘制更新事件>
		core::Event<void(std::chrono::system_clock::time_point)>& NoneFrameUpdateEvent();

		void onlyUpdateFrame();

		void bindAbortValue(osg::State* state);

		void setBlockAnimation(unsigned int totalFrame, core::InstanceFunction<void(float)> pre, core::InstanceFunction<void(float)> post);

	protected:
		// <绘制前更新事件>
		core::Event<void(std::chrono::system_clock::time_point)> m_preFrameUpdateEvent;

		// <绘制后更新事件>
		core::Event<void(std::chrono::system_clock::time_point)> m_postFrameUpdateEvent;

		// <无绘制更新事件>
		core::Event<void(std::chrono::system_clock::time_point)> m_noneFrameUpdateEvent;

		// <阻塞动画绘制前更新事件>
		core::Event<void(float)> m_preBlockUpdateEvent;

		// <阻塞动画绘制后更新事件>
		core::Event<void(float)> m_postBlockUpdateEvent;

		// <处理更新计时到时间事件>
		core::Event<void(float)> m_handleUpdateTickEvent;

		//volatile bool m_done;

		// whether needs to process ( update & draw).
		std::atomic_bool m_needProcess;
		
		// process later. process after m_delayProcessProcess time.
		std::atomic_int m_delayProcess;

		// pause later. pause after m_delayPauseProcess time.
		std::atomic_int m_delayPauseProcess;
        
        // is rendering outside rendering thread.
        std::atomic_bool m_isExternalRendering;
        
        // is rendering.
		std::atomic_bool m_isRendering;

		OpenThreads::Mutex m_mutex;

		// <是否取消渲染>
		bool m_babortRendering;
		bool m_hasBlockAnimation;
		// begin at 1
		unsigned int m_currentBlockFrame;
		unsigned int m_totalBlockFrame;

		double m_lastLoopTime;
		double m_lastDrawPeriod;
	};

}
#endif // __C_RENDERING_THREAD_H__