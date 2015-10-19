#ifndef __IMODULE_CORE_FACTORY_H__
#define __IMODULE_CORE_FACTORY_H__
#include "IModelCore.h"
#include <memory>
#include "config.h"

#ifdef _WIN32
#include <windows.h>
#endif

namespace bimWorld
{
	class YIZHU_EXPORT IModelCoreFactory
	{
	public:
#ifdef _WIN32
		virtual std::shared_ptr<IModelCore> getModule(HWND hwnd) = 0;
#else
		virtual std::shared_ptr<IModelCore> getModule(int x,int y,int width,int height) = 0;
#endif // _WIN32

	protected:
	};
}
#endif // __IMODULE_CORE_FACTORY_H__