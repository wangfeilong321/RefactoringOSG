#pragma warning(disable:4244)
#include "StringExt.h"
#include "CharExt.h"
#include "MathExt.h"
#include <type_traits>
//#include "boost/locale.hpp"

using namespace util;

#pragma region Split

#ifndef __STRING_EXTENSION_SPLIT_CPP__
#define __STRING_EXTENSION_SPLIT_CPP__

template<class T>
bool splitTemplate(const T& srcStr, const T splitStr, std::vector< T >& outResult)
{
	size_t last = 0;
	size_t index = srcStr.find_first_of(splitStr, last);
	if (index == T::npos)
		return false;

	outResult.clear();

	while (index != T::npos)
	{
		outResult.push_back(srcStr.substr(last, index - last));
		last = index + 1;
		index = srcStr.find_first_of(splitStr, last);
	}

	if (index - last > 0)
	{
		outResult.push_back(srcStr.substr(last, index - last));
	}

	return true;
}

template<class T>
bool splitTemplate(const T& srcStr, const T* splitStrs, int splitLength, std::vector< T >& outResult)
{
	if (!splitStrs)
		return false;

	size_t last = 0;
	size_t min_index = T::npos;

	for (int i = 0; i < splitLength; i++)
	{
		size_t index = srcStr.find_first_of(splitStrs[i], last);
		if (index == T::npos)
			continue;
		if (min_index == T::npos || min_index < index)
			min_index = index;
	}

	outResult.clear();

	while (min_index != T::npos)
	{
		outResult.push_back(srcStr.substr(last, min_index - last));
		last = min_index + 1;

		min_index = T::npos;
		for (int i = 0; i < splitLength; i++)
		{
			size_t index = srcStr.find_first_of(splitStrs[i], last);
			if (index == T::npos)
				continue;
			if (min_index == T::npos || min_index < index)
				min_index = index;
		}
	}

	//if (min_index - last > 0)
	//{
	//	outResult.push_back(srcStr.substr(last, min_index - last));
	//}

	return true;
}

#endif // __STRING_EXTENSION_SPLIT_CPP__


bool util::StringExtension::split(const std::string& srcStr, const std::string splitStr, std::vector< std::string >& outResult)
{
	return splitTemplate(srcStr, splitStr, outResult);
	//size_t last = 0;
	//size_t index = srcStr.find_first_of(splitStr, last);
	//if (index == std::string::npos)
	//	return false;

	//while (index != std::string::npos)
	//{
	//	outResult->push_back(srcStr.substr(last, index - last));
	//	last = index + 1;
	//	index = srcStr.find_first_of(splitStr, last);
	//}
	//if (index - last > 0)
	//{
	//	outResult->push_back(srcStr.substr(last, index - last));
	//}
	//return true;
}

bool util::StringExtension::split(const std::wstring& srcStr, const std::wstring splitStr, std::vector< std::wstring >& outResult)
{
	return splitTemplate(srcStr, splitStr, outResult);
	//size_t last = 0;
	//size_t index = srcStr.find_first_of(splitStr, last);
	//if (index == std::wstring::npos)
	//	return false;

	//while (index != std::wstring::npos)
	//{
	//	outResult->push_back(srcStr.substr(last, index - last));
	//	last = index + 1;
	//	index = srcStr.find_first_of(splitStr, last);
	//}
	//if (index - last > 0)
	//{
	//	outResult->push_back(srcStr.substr(last, index - last));
	//}
	//return true;
}

bool util::StringExtension::split(const std::string& srcStr, const std::string *splitStrs, int splitLength, std::vector< std::string >& outResult)
{
	return splitTemplate(srcStr, splitStrs, splitLength, outResult);
}

bool util::StringExtension::split(const std::wstring& srcStr, const std::wstring *splitStrs, int splitLength, std::vector< std::wstring >& outResult)
{
	return splitTemplate(srcStr, splitStrs, splitLength, outResult);
}

#pragma endregion


#pragma region ParseToValue

#ifndef __STRING_EXTENSION_PARSE_TO_VALUE_CPP__
#define __STRING_EXTENSION_PARSE_TO_VALUE_CPP__

template<class T, class R>
bool parseToValueTemplate(const R& srcStr, T& outValue)
{
	outValue = 0;
	for (int i = 0; i < srcStr.size(); i++)
	{
		short tempValue = 0;
		if (util::CharExtension::toShort(srcStr[srcStr.size() - 1 - i], tempValue))
		{
			outValue += tempValue * util::MathExtension::positivePow((T)10, (T)i);
		}
		else
		{
			return false;
		}
	}
	return true;
}

#endif // __STRING_EXTENSION_PARSE_TO_VALUE_CPP__


bool util::StringExtension::parseToShort(const std::string& srcStr, short& outInt)
{
	return parseToValueTemplate(srcStr, outInt);
}

bool util::StringExtension::parseToShort(const std::wstring& srcStr, short& outInt)
{
	return parseToValueTemplate(srcStr, outInt);
}

bool util::StringExtension::parseToInt(const std::string& srcStr, int& outInt)
{
	return parseToValueTemplate(srcStr, outInt);
}

bool util::StringExtension::parseToInt(const std::wstring& srcStr, int& outInt)
{
	return parseToValueTemplate(srcStr, outInt);
}

bool util::StringExtension::parseToLong(const std::string& srcStr, long& outLong)
{
	return parseToValueTemplate(srcStr, outLong);
}

bool util::StringExtension::parseToLong(const std::wstring& srcStr, long& outLong)
{
	return parseToValueTemplate(srcStr, outLong);
}

#pragma endregion

#pragma region ParseToVector

#ifndef __STRING_EXTENSION_PARSE_TO_VECTOR_CPP__
#define __STRING_EXTENSION_PARSE_TO_VECTOR_CPP__

template <class T, class R, class K>
bool parseToVectorTemplate(const R& srcStr, std::vector<T>& outVector, const K* splitter)
{
	size_t pos = srcStr.rfind(splitter);
	auto rest = srcStr;// substr(0, srcStr.size());
	int time = 0;
	while (time++ < srcStr.size())
	{
		if (pos < 0 || pos > srcStr.size())
		{
			T value;
			if (!parseToValueTemplate(rest, value))
				return false;

			outVector.push_back(value);
			return true;
		}

		T singleValue;
		auto tempStr = rest.substr(pos + 1, rest.size());
		if (!parseToValueTemplate(tempStr, singleValue))
			return false;
		outVector.push_back(singleValue);
		rest = rest.substr(0, pos);
		pos = rest.rfind(splitter);
	}

	return true;
}

#endif // __STRING_EXTENSION_PARSE_TO_VECTOR_CPP__


bool util::StringExtension::parseToShortVector(const std::string& srcStr, std::vector<short>& outVector, const char* splitter)
{
	return parseToVectorTemplate(srcStr, outVector, splitter);
}

bool util::StringExtension::parseToShortVector(const std::wstring& srcStr, std::vector<short>& outVector, const wchar_t* splitter)
{
	return parseToVectorTemplate(srcStr, outVector, splitter);
}

bool util::StringExtension::parseToIntVector(const std::string& srcStr, std::vector<int>& outVector, const char* splitter)
{
	return parseToVectorTemplate(srcStr, outVector, splitter);
}

bool util::StringExtension::parseToIntVector(const std::wstring& srcStr, std::vector<int>& outVector, const wchar_t* splitter)
{
	return parseToVectorTemplate(srcStr, outVector, splitter);
}

bool util::StringExtension::parseToLongVector(const std::string& srcStr, std::vector<long>& outVector, const char* splitter)
{
	return parseToVectorTemplate(srcStr, outVector, splitter);
}

bool util::StringExtension::parseToLongVector(const std::wstring& srcStr, std::vector<long>& outVector, const wchar_t* splitter)
{
	return parseToVectorTemplate(srcStr, outVector, splitter);
}

// rtk
bool util::StringExtension::parseToVector(const std::string& srcStr, std::vector<std::string>& outVector, const char* splitter)
{
	size_t pos = srcStr.rfind(splitter);
	auto rest = srcStr;// substr(0, srcStr.size());
	int time = 0;
	while (time++ < srcStr.size())
	{
		if (pos < 0 || pos > srcStr.size())
		{
			outVector.push_back(rest);
			return true;
		}

		auto tempStr = rest.substr(pos + 1, rest.size());

		outVector.push_back(tempStr);
		rest = rest.substr(0, pos);
		pos = rest.rfind(splitter);
	}

	return true;

}

#pragma endregion

#pragma region ConvertWideStrAndStr

std::wstring util::StringExtension::StringToWString(const std::string& s)
{
	//std::string encoding = "utf-8";
	//return boost::locale::conv::to_utf<wchar_t>(s, encoding);
	std::wstring temp(s.length(), L' ');
	std::copy(s.begin(), s.end(), temp.begin());
	return temp;
}

std::string util::StringExtension::WStringToString(const std::wstring& s)
{
	//std::string encoding = "utf-8";
	//return boost::locale::conv::from_utf(s, encoding);
	std::string temp(s.length(), ' ');
	std::copy(s.begin(), s.end(), temp.begin());
	return temp;
}

bimWorld::YZString util::StringExtension::toYZString(const std::string& s)
{
#ifdef YIZHU_BIMWORLD_USE_WCHAR
	return util::StringExtension::StringToWString(s);
#else
	return s;
#endif
}

bimWorld::YZString util::StringExtension::toYZString(const std::wstring& s)
{
#ifdef YIZHU_BIMWORLD_USE_WCHAR
	return s;
#else
	return util::StringExtension::WStringToString(s);
#endif
}

std::string util::StringExtension::toStdString(const std::string& str)
{
	return str;
}

std::string util::StringExtension::toStdString(const std::wstring& str)
{
	return WStringToString(str);
}

#pragma endregion

