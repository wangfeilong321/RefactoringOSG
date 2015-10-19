#ifndef __CATEGORY_QUERY_KEY_H__
#define __CATEGORY_QUERY_KEY_H__
#include <map>
#include <vector>
#include "MultiStringKey.h"
#include "config.h"

namespace bimWorld
{
	class YIZHU_EXPORT CategoryQueryKey
	{
	public:
		// usage: CategoryQueryKey key({ "floor", "category" }, { "2ndF", "pipeline" });
		CategoryQueryKey(const MultiStringKey& types, const MultiStringKey& keys);

		// <��ȡ��ѯ�����ͼ�>
		const MultiStringKey getTypes() const;
		
		// <��ȡ��ѯ�ļ�>
		const MultiStringKey getKeys() const;
	protected:
		MultiStringKey _types;
		MultiStringKey _keys;
	};
}
#endif // __CATEGORY_QUERY_KEY_H__