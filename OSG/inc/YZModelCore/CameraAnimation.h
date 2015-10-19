#ifndef __CAMERA_ANIMATION_H__
#define __CAMERA_ANIMATION_H__
#include "YZModelCoreModule.h"
#include "CameraAnimator.h"
#include <osg/ref_ptr>
#include <osg/Vec3d>

namespace bimWorld
{
	class YIZHU_EXPORT CameraAnimation : public YZModelCoreModule
	{
	public:
		CameraAnimation(YZModelCoreInternal* host);

		void play(osg::Vec3d pos, osg::Vec3d center, osg::Vec3d up);

	//internal:
		std::vector<bimWorld::CameraVector> createAnimationPath(osg::Vec3d pos, osg::Vec3d center, osg::Vec3d up, float delta_time = 50);
		
	protected:
		// <绘制：true，更新：false。在执行 Camera 动画之前，在绘制阶段还是在仅更新阶段。>
		bool origin_draw;
		osg::ref_ptr<CameraAnimator> m_cameraAnimator;
	};
}

#endif // __CAMERA_ANIMATION_H__