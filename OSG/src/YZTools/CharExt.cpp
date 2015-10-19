#include "CharExt.h"

using namespace util;

template<class T>
bool toValueTemplate(char srcChar, T& outInt)
{
	if (srcChar < '0' || srcChar > '9')
	{
		return false;
	}
	outInt = srcChar - '0';
	return true;
}

template<class T>
bool toValueTemplate(wchar_t srcChar, T& outInt)
{
	if (srcChar < L'0' || srcChar > L'9')
	{
		return false;
	}
	outInt = srcChar - L'0';
	return true;
}


bool CharExtension::toShort(char srcChar, short& outShort)
{
	return toValueTemplate(srcChar, outShort);
}

bool CharExtension::toInt(char srcChar, int& outInt)
{
	return toValueTemplate(srcChar, outInt);
}

bool CharExtension::toLong(char srcChar, long& outLong)
{
	return toValueTemplate(srcChar, outLong);
}


bool CharExtension::toShort(wchar_t srcChar, short& outShort)
{
	return toValueTemplate(srcChar, outShort);
}

bool CharExtension::toInt(wchar_t srcChar, int& outInt)
{
	return toValueTemplate(srcChar, outInt);
}

bool CharExtension::toLong(wchar_t srcChar, long& outLong)
{
	return toValueTemplate(srcChar, outLong);
}