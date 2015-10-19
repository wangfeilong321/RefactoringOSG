#ifndef __MATH_EXTENSION_H__
#define __MATH_EXTENSION_H__
#include "config.h"

namespace util
{

	class YIZHU_EXPORT MathExtension
	{
	public:



#pragma region PositivePow

		static short positivePow(short base, short exponent);

		static int positivePow(int base, int exponent);

		static long positivePow(long base, long exponent);

#pragma endregion

	};
}
#endif // __MATH_EXTENSION_H__