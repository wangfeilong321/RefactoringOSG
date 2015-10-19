#ifndef __MODEL_CORE_FACTORY_H__
#define __MODEL_CORE_FACTORY_H__
#include "IModelCore.h"
#include "IModelCoreFactory.h"


namespace bimWorld
{
	class YIZHU_EXPORT ModelCoreFactory :public IModelCoreFactory
	{
	public:
#ifdef _WIN32
		std::shared_ptr<IModelCore> getModule(HWND hwnd) override;
#else
		std::shared_ptr<IModelCore> getModule(int x,int y,int width,int height) override;
#endif // _WIN32

	protected:
	private:
	};
}
#endif // __MODEL_CORE_FACTORY_H__