#include "PointerSerializer.h"
#include "PointerAccess.h"
#include <sstream>

namespace std
{
	template<typename T>
	std::string to_string(T value)
	{
		std::ostringstream os;
		os << value;
		return os.str();
	}
}

std::string PointerSerializer::Serialize(void* ptr)
{
	if (!ptr)
		return "";
	return std::to_string((long)ptr);
}

//template <class T>
//T* PointerSerializer::Deserialize(int ptr)
void* PointerSerializer::Deserialize(long ptr)
{
	void * parsePtr = nullptr;
	if (ptr <= 0)
		return parsePtr;
	//auto ptrSize = sizeof(T);
	unsigned int ptrSize = 1;
	parsePtr = (void *)ptr;
	void* cParsePtr = parsePtr;

	if (0 == IsBadWritePtr(cParsePtr, ptrSize) && 0 == IsBadReadPtr(parsePtr, ptrSize))
	{
		return parsePtr;
	}
	return nullptr;
}
