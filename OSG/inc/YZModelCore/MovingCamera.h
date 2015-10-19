#ifndef __MOVING_CAMERA_H__
#define __MOVING_CAMERA_H__
#include "YZModelCoreModule.h"
#include "IMovingCamera.h"

namespace bimWorld
{
	class YIZHU_EXPORT MovingCamera :public YZModelCoreModule, public IMovingCamera
	{
	public:

		MovingCamera(YZModelCoreInternal* host);

		virtual void moveHorizontal(double distance) override;

		virtual void moveVertical(double  distance) override;

		virtual void moveUp(double distance) override;

		virtual void moveRight(double distance) override;

		virtual void translation(double startx, double starty, double endx, double endy) override;

	protected:
	private:
	};

}
#endif // __MOVING_CAMERA_H__