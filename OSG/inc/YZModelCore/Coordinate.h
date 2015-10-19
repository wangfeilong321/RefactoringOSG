#ifndef __COORDINATE_H__
#define __COORDINATE_H__
#include "CoordinateHelper.h"
#include <osg/ref_ptr>
#include "YZModelCoreModule.h"
#include "ICoordinate.h"

namespace bimWorld
{
	class YIZHU_EXPORT Coordinate :public YZModelCoreModule, public ICoordinate
	{
	public:
		Coordinate(YZModelCoreInternal* host);
		virtual util::CoordinateHelper* CoordinateHelper() override;
	protected:
		osg::ref_ptr<util::CoordinateHelper> m_coordHelper;
	};
}
#endif // __COORDINATE_H__