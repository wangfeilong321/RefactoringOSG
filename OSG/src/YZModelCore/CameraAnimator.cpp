#include "CameraAnimator.h"
#include "YZModelCoreInternal.h"

using namespace bimWorld;


bimWorld::CameraAnimator::CameraAnimator(YZModelCoreInternal* host) :
m_isAnimating(false),
m_isFinished(false),
m_index(0),
m_validCode(CAMERA_ANIMATOR_VALIDATION_MASK),
YZModelCoreModule(host)
{

}

void CameraAnimator::addCompleteCallback(std::function<void()> func)
{
	if (!isValid())
		return;
	m_completeCallback.push_back(func);
}

void CameraAnimator::addUpdateCallback(std::function<void(const CameraAnimator*)> func)
{
	if (!isValid())
		return;
	m_updateCallback.push_back(func);
}

bimWorld::CameraVector* CameraAnimator::getCameraViewStruct()
{
	if (!isValid())
		return NULL;

	if (!m_isAnimating || m_isFinished)
		return NULL;
	if (!hasAnimation() || m_index < 0)
		return NULL;
	if (m_index >= m_path.size())
	{
		finish();
		return NULL;
	}
	for (auto iter = m_updateCallback.begin(); iter != m_updateCallback.end(); iter++)
	{
		(*iter)(this);
	}
	auto cw = &m_path.at(m_index++);

	return cw;
}


void CameraAnimator::finish()
{
	if (!isValid())
		return;

	for (auto iter = m_completeCallback.begin(); iter < m_completeCallback.end(); iter++)
	{
		(*iter)();
	}
	m_isAnimating = false;
	m_isFinished = true;
	m_completeCallback.clear();
	m_updateCallback.clear();
}