#ifndef __ICOORDINATE_H__
#define __ICOORDINATE_H__
#include "CoordinateHelper.h"
namespace bimWorld
{
	class YIZHU_EXPORT ICoordinate
	{
	public:
		virtual util::CoordinateHelper* CoordinateHelper() = 0;
	};
}
#endif // __ICOORDINATE_H__