#ifndef __MAP_EXT_H__
#define __MAP_EXT_H__

#include <map>
#include "config.h"

namespace util
{
	class YIZHU_EXPORT MapExtension
	{
	public:
		template< class K, class V >
		static bool containsKey(const std::map<K, V>& This, K value)
		{
			for (auto iter = This.begin(); iter != This.end(); iter++)
			{
				if (iter->first == value)
					return true;
			}
			return false;
		}
	protected:
	};
}

#endif // __MAP_EXT_H__