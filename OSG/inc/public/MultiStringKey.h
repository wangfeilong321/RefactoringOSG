#ifndef __CATEGORY_TYPE_KEY_H__
#define __CATEGORY_TYPE_KEY_H__
#include <vector>
#include <string>
#include <cstdarg>
#include "config.h"

namespace bimWorld
{
	class YIZHU_EXPORT MultiStringKey : public std::vector < std::string >
	{
		typedef std::vector<std::string> base;
	public:
		// usage: CategoryTypeKey({"floor", "systemType", "category"})
		MultiStringKey();

		MultiStringKey(size_t size);

		MultiStringKey(std::initializer_list<std::string> keys);

		MultiStringKey(const std::vector<std::string>& copy);


		bool operator==(const MultiStringKey& other);

		inline MultiStringKey operator()(std::initializer_list<std::string> keys);

		std::string toString();

#pragma region override
		void push_back(const std::string& str);

		iterator insert(const_iterator _Where, const std::string& _Val);

		void insert(const_iterator begin, const_iterator end);
#pragma endregion

	};
}

#endif // __CATEGORY_TYPE_KEY_H__