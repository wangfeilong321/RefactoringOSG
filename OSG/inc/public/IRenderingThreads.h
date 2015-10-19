#ifndef __IRENDERING_THREADS_H__
#define __IRENDERING_THREADS_H__
#include "Event.h"
#include <chrono>

namespace bimWorld
{
	class YIZHU_EXPORT IRenderingThreads
	{
	public:

		// <����һ����Ⱦ����>
		virtual void startRendering() = 0;

		virtual void	setIsExternalRendering(bool value) = 0;

		virtual void setNeedUpdateAndDraw(bool value, int delay = 0) = 0;

		virtual bool getNeedUpdateAndDraw() = 0;
		
#ifdef Android
		virtual bool loop(bool hasDrawn = false) = 0;
#endif // Android


		// <ˢ��>
		virtual void	updateSeveralTimes(int times = 3) = 0;

		virtual void onlyUpdateFrame() = 0;

		// <����ǰ�����¼�>
		virtual core::Event<void(std::chrono::system_clock::time_point)>& PreFrameUpdateEvent() = 0;

		// <���ƺ�����¼�>
		virtual core::Event<void(std::chrono::system_clock::time_point)>& PostFrameUpdateEvent() = 0;

		// <�޻��Ƹ����¼�>
		virtual core::Event<void(std::chrono::system_clock::time_point)>& NoneFrameUpdateEvent() = 0;

		// <�Ժ�ֱ���ṩframe����>
		// virtual void frame() = 0;
	};
}

#endif // __IRENDERING_THREADS_H__