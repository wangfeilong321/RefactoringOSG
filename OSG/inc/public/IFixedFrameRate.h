#ifndef __IFIXED_FRAME_RATE_H__
#define __IFIXED_FRAME_RATE_H__

#include "config.h"
#include "DataModel.h"
namespace bimWorld
{
	class YIZHU_EXPORT IFixedFrameRate
	{
	public:

		virtual void cancelFixedFramerate() = 0;

		virtual void startFixedFramerate() = 0;

	};

}
#endif // __IFIXED_FRAME_RATE_H__