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

		// <����һ����Ⱦ����>
		virtual void startRendering() override;

		virtual void setIsExternalRendering(bool value) override;

		virtual void setNeedUpdateAndDraw(bool value, int delay = 0) override;

		virtual bool getNeedUpdateAndDraw() override;

#ifdef Android
		virtual bool loop(bool hasDrawn = false) override;
#endif // Android


		// <ˢ��>
		virtual void updateSeveralTimes(int times = 3) override;

		virtual void onlyUpdateFrame() override;

		void bindAbortValue(osg::State* state);

		// <����ǰ�����¼�>
		core::Event<void(std::chrono::system_clock::time_point)>& PreFrameUpdateEvent();

		// <���ƺ�����¼�>
		core::Event<void(std::chrono::system_clock::time_point)>& PostFrameUpdateEvent();

		// <�޻��Ƹ����¼�>
		core::Event<void(std::chrono::system_clock::time_point)>& NoneFrameUpdateEvent();

		void setBlockAnimation(unsigned int totalFrame, core::InstanceFunction<void(float)> pre, core::InstanceFunction<void(float)> post);

		// <�Ժ�ֱ���ṩframe����>
		// virtual void frame();
	protected:
		// <��Ⱦ�߳�>
		osg::ref_ptr<CRenderingThread> m_rendering;
	private:
		bool m_started;
	};
}

#endif // __RENDERING_THREADS_H__