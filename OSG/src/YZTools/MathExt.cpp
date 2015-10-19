#include "MathExt.h"

using namespace util;

template<class T>
static T positivePowTemplate(T base, T exponent)
{
	T result = 1;
	for (T i = 0; i < exponent; i++)
	{
		result *= base;
	}
	return result;
}

short MathExtension::positivePow(short base, short exponent)
{
	return positivePowTemplate(base, exponent);
}

int MathExtension::positivePow(int base, int exponent)
{
	return positivePowTemplate(base, exponent);
}

long MathExtension::positivePow(long base, long exponent)
{
	return positivePowTemplate(base, exponent);
}
