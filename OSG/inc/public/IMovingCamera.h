#ifndef __IMOVING_CAMERA_H__
#define __IMOVING_CAMERA_H__

namespace bimWorld
{
	class YIZHU_EXPORT IMovingCamera
	{
	public:
		virtual void moveHorizontal(double distance) = 0;

		virtual void moveVertical(double  distance) = 0;

		virtual void moveUp(double distance) = 0;

		virtual void moveRight(double distance) = 0;

		virtual void translation(double startx, double starty, double endx, double endy) = 0;

	protected:
	};
}
#endif // __IMOVING_CAMERA_H__