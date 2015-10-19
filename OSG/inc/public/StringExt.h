#ifndef __STRING_EXTENSION_H__
#define __STRING_EXTENSION_H__

#include <vector>
#include <string>
//#include <math.h>
#include <algorithm>
#include "config.h"

namespace util
{

	class YIZHU_EXPORT StringExtension
	{
	public:

#pragma region Split

		static bool split(const std::string& srcStr, const std::string splitStr, std::vector< std::string >& outResult);

		static bool split(const std::wstring& srcStr, const std::wstring splitStr, std::vector< std::wstring >& outResult);

		static bool split(const std::string& srcStr, const std::string *splitStrs, int splitLength, std::vector< std::string >& outResult);

		static bool split(const std::wstring& srcStr, const std::wstring *splitStrs, int splitLength, std::vector< std::wstring >& outResult);

#pragma endregion

#pragma region ParseToValue

#pragma region std::string

		static bool parseToShort(const std::string& srcStr, short& outInt);

		static bool parseToInt(const std::string& srcStr, int& outInt);

		static bool parseToLong(const std::string& srcStr, long& outLong);

#pragma endregion


#pragma region std::wstring

		static bool parseToShort(const std::wstring& srcStr, short& outInt);

		static bool parseToInt(const std::wstring& srcStr, int& outInt);

		static bool parseToLong(const std::wstring& srcStr, long& outLong);

#pragma endregion

#pragma endregion

#pragma region ParseToVector

#pragma region std::string

		static bool parseToShortVector(const std::string& srcStr, std::vector<short>& outVector, const char* splitter);

		static bool parseToIntVector(const std::string& srcStr, std::vector<int>& outVector, const char* splitter);

		static bool parseToLongVector(const std::string& srcStr, std::vector<long>& outVector, const char* splitter);

		static bool parseToVector(const std::string& idsStr, std::vector<std::string>& idset, const char* splitter);

#pragma endregion

#pragma region std::wstring

		static bool parseToShortVector(const std::wstring& srcStr, std::vector<short>& outVector, const wchar_t* splitter);

		static bool parseToIntVector(const std::wstring& srcStr, std::vector<int>& outVector, const wchar_t* splitter);

		static bool parseToLongVector(const std::wstring& srcStr, std::vector<long>& outVector, const wchar_t* splitter);
#pragma endregion

#pragma endregion

#pragma region ConvertWideStrAndStr

		// Prototype for conversion functions
		static std::wstring StringToWString(const std::string& str);

		static std::string WStringToString(const std::wstring& str);

		static bimWorld::YZString toYZString(const std::string& str);
		
		static bimWorld::YZString toYZString(const std::wstring& str);

		static std::string toStdString(const std::string& str);

		static std::string toStdString(const std::wstring& str);
#pragma endregion


	protected:
	};

}
#endif // __STRING_EXTENSION_H__