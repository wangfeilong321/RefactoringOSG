#ifndef __RENDERING_THREADS_H__
#define __RENDERING_THREADS_H__

#include "RenderingThread.h"
#include "IRenderingThreads.h"

namespace bimWorld
{
	class YIZHU_EXPORT RenderingThreads : public IRenderingThreads
	{
	public:
		RenderingThreads(YZModelCoreInternal* host);

		// <开启一个渲染引擎>
		virtual void startRendering() override;

		virtual void setIsExternalRendering(bool value) override;

		virtual void setNeedUpdateAndDraw(bool value, int delay = 0) override;

		virtual bool getNeedUpdateAndDraw() override;

#ifdef Android
		virtual bool loop(bool hasDrawn = false) override;
#endif // Android


		// <刷新>
		virtual void updateSeveralTimes(int times = 3) override;

		virtual void onlyUpdateFrame() override;

		void bindAbortValue(osg::State* state);

		// <绘制前更新事件>
		core::Event<void(std::chrono::system_clock::time_point)>& PreFrameUpdateEvent();

		// <绘制后更新事件>
		core::Event<void(std::chrono::system_clock::time_point)>& PostFrameUpdateEvent();

		// <无绘制更新事件>
		core::Event<void(std::chrono::system_clock::time_point)>& NoneFrameUpdateEvent();

		void setBlockAnimation(unsigned int totalFrame, core::InstanceFunction<void(float)> pre, core::InstanceFunction<void(float)> post);

		// <以后不直接提供frame函数>
		// virtual void frame();
	protected:
		// <渲染线程>
		osg::ref_ptr<CRenderingThread> m_rendering;
	private:
		bool m_started;
	};
}

#endif // __RENDERING_THREADS_H__