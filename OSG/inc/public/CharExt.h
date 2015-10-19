#ifndef __CHAR_EXTENSION_H__
#define __CHAR_EXTENSION_H__
#include "config.h"

namespace util
{

	class YIZHU_EXPORT CharExtension
	{

	public:

#pragma region ToValue

#pragma region char

		static bool toShort(char srcChar, short& outShort);

		static bool toInt(char srcChar, int& outInt);

		static bool toLong(char srcChar, long& outLong);

#pragma endregion

#pragma region wchar_t

		static bool toShort(wchar_t srcChar, short& outShort);

		static bool toInt(wchar_t srcChar, int& outInt);

		static bool toLong(wchar_t srcChar, long& outLong);

#pragma endregion


#pragma endregion

	};
}
#endif // __CHAR_EXTENSION_H__