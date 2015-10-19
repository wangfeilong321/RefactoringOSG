#ifndef __VECTOR_EXT_H__
#define __VECTOR_EXT_H__

#include <vector>
#include "config.h"

namespace util
{
	class YIZHU_EXPORT VectorExtension
	{
	public:
		template< class T >
		static bool contains(const std::vector<T>& This, T value)
		{
			for (auto iter = This.begin(); iter != This.end(); iter++)
			{
				if (*iter == value)
					return true;
			}
			return false;
		}
	protected:
	};
}

#endif // __VECTOR_EXT_H__