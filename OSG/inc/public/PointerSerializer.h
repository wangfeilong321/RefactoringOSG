#include "config.h"
#include <string>

class YIZHU_EXPORT PointerSerializer
{
public:
	static std::string Serialize(void* ptr);
	//template <class T>
	//static T* Deserialize(int ptr, size_t size);
	static void* Deserialize(int ptr);
protected:
private:
};
