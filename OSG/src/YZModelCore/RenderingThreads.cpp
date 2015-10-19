#include "RenderingThreads.h"
#include "YZModelCoreInternal.h"

bimWorld::RenderingThreads::RenderingThreads(YZModelCoreInternal * host)
{
	m_rendering = new CRenderingThread(host);
	m_started = false;
}

void bimWorld::RenderingThreads::startRendering()
{
	if (!m_rendering)
	{
		return;
	}
	m_rendering->start();
	m_started = true;
}

void bimWorld::RenderingThreads::setIsExternalRendering(bool value)
{
	if (!m_rendering || !m_started)
	{
		return;
	}
	m_rendering->setIsExternalRendering(value);
}

void bimWorld::RenderingThreads::updateSeveralTimes(int times)
{
	if (!m_rendering || !m_started)
	{
		return;
	}
	m_rendering->updateSeveralTimes(times);
}

void bimWorld::RenderingThreads::setNeedUpdateAndDraw(bool value, int delay)
{
	if (!m_rendering || !m_started)
	{
		return;
	}
	m_rendering->setNeedUpdateAndDraw(value, delay);
}

bool bimWorld::RenderingThreads::getNeedUpdateAndDraw()
{
	if (!m_rendering || !m_started)
	{
		return false;
	}
	return m_rendering->getNeedUpdateAndDraw();
}

void bimWorld::RenderingThreads::onlyUpdateFrame()
{
	if (!m_rendering || !m_started)
	{
		return;
	}
	m_rendering->onlyUpdateFrame();
}

void bimWorld::RenderingThreads::bindAbortValue(osg::State* state)
{
	if (!m_rendering || !m_started)
	{
		return;
	}
	m_rendering->bindAbortValue(state);
}

core::Event<void(std::chrono::system_clock::time_point)>& bimWorld::RenderingThreads::PreFrameUpdateEvent()
{
	if (!m_rendering || !m_started)
	{
		core::Event<void(std::chrono::system_clock::time_point)> empty;
		return empty;
	}
	return m_rendering->PreFrameUpdateEvent();
}

core::Event<void(std::chrono::system_clock::time_point)>& bimWorld::RenderingThreads::PostFrameUpdateEvent()
{
	if (!m_rendering || !m_started)
	{
		core::Event<void(std::chrono::system_clock::time_point)> empty;
		return empty;
	}
	return m_rendering->PostFrameUpdateEvent();
}

core::Event<void(std::chrono::system_clock::time_point)>& bimWorld::RenderingThreads::NoneFrameUpdateEvent()
{
	if (!m_rendering || !m_started)
	{
		core::Event<void(std::chrono::system_clock::time_point)> empty;
		return empty;
	}
	return m_rendering->NoneFrameUpdateEvent();
}

void bimWorld::RenderingThreads::setBlockAnimation(unsigned int totalFrame, core::InstanceFunction<void(float)> pre, core::InstanceFunction<void(float)> post)
{
	if (!m_rendering || !m_started)
	{
		return;
	}
	m_rendering->setBlockAnimation(totalFrame, pre, post);
}


#ifdef Android

bool bimWorld::RenderingThreads::loop(bool hasDrawn)
{
	if (!m_rendering || !m_started)
	{
		return false;
	}
	m_rendering->loop(hasDrawn);
}

#endif // Android
