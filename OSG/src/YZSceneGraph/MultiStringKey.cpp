#include "MultiStringKey.h"
#include <algorithm>
#include <sstream>

#define SORT_KEY 0

// <自定义排序函数>
bool lessThan(const std::string& v1, const std::string& v2)
{
	int index = 0;
	while (v1[index] != '\0'&& v2[index] != '\0')
	{
		if (v1[index] == '\0')
		{
			return true; //  v1 < v2;
		}
		else if (v2[index] == '\0')
		{
			return false; //  v1 > v2;
		}
		else if (v1[index] != v2[index])
		{
			return v1[index] < v2[index];
		}
		else
		{
			index++;
		}
	}
	return false;
}

#pragma region Constructor

#pragma region InheritVector
bimWorld::MultiStringKey::MultiStringKey() :
base()
{

}

bimWorld::MultiStringKey::MultiStringKey(size_t size) :
base(size)
{

}
#pragma endregion


bimWorld::MultiStringKey::MultiStringKey(std::initializer_list<std::string> keys)
{
	this->operator()(keys);
}

bimWorld::MultiStringKey::MultiStringKey(const std::vector<std::string>& copy) :
base(copy)
{
#if SORT_KEY
	std::sort(this->begin(), this->end(), lessThan);
#endif // SORT_KEY

}

#pragma endregion


#pragma region Operator
bool bimWorld::MultiStringKey::operator==(const MultiStringKey& other)
{
	if (this->size() != other.size())
	{
		return false;
	}
	for (int i = 0; i < this->size(); i++)
	{
		if (this->operator[](i) != other[i])
			return false;
	}
	return true;
}

inline bimWorld::MultiStringKey bimWorld::MultiStringKey::operator()(std::initializer_list<std::string> keys)
{
	this->clear();
	base::insert(this->end(), keys.begin(), keys.end());
#if SORT_KEY
	std::sort(this->begin(), this->end(), lessThan);
#endif // SORT_KEY
	return *this;
}

void bimWorld::MultiStringKey::push_back(const std::string& str)
{
	base::push_back(str);
#if SORT_KEY
	std::sort(this->begin(), this->end(), lessThan);
#endif // SORT_KEY
}

bimWorld::MultiStringKey::iterator bimWorld::MultiStringKey::insert(const_iterator _Where, const std::string& _Val)
{
	auto iter = base::insert(_Where, _Val);
#if SORT_KEY
	std::sort(this->begin(), this->end(), lessThan);
#endif // SORT_KEY
	return iter;
}

void bimWorld::MultiStringKey::insert(const_iterator begin, const_iterator end)
{
	base::insert(this->end(), begin, end);
#if SORT_KEY
	std::sort(this->begin(), this->end(), lessThan);
#endif // SORT_KEY
}

std::string bimWorld::MultiStringKey::toString()
{
	std::stringstream stream;
    for(auto iter = (*this).begin(); iter!=(*this).end(); iter++)
	{
		stream << (*iter);
	}
	return stream.str();
}

#pragma endregion

