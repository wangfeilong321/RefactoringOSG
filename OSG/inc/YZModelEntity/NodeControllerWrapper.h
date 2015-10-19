#ifndef __NODE_CONTROLLER_WRAPPER_H__
#define __NODE_CONTROLLER_WRAPPER_H__
#include "INodeEntityController.h"
#include "YZModelEntityModule.h"
#include "memory"

namespace bimWorld
{
	class YIZHU_EXPORT NodeControllerWrapper : public INodeEntityController, public YZModelEntityModule
	{
	public:
		NodeControllerWrapper(YZModelEntityInternal* host);

#pragma region NodeController

		virtual void hideAll() override;

		virtual void unHideAll() override;

		virtual void hide(void *node) override;

		virtual void unHide(void *node) override;

		virtual void hide(const std::vector<void*>& nodes) override;

		virtual void unHide(const std::vector<void*>& nodes) override;

		virtual void hideOthers(void *node) override;

		virtual void unHideOthers(void *node) override;

		// <�л�����>
		virtual void toggleHide(void *node) override;

		virtual void toggleHide(const std::vector<void*>& nodes) override;

		virtual void setVisible(void * node, bool visible) override;

		// <��������>
		virtual void setVisible(const std::vector<void*>& nodes, bool visible) override;

		// <�жϹ����ɼ���>
		virtual bool isVisible(void *node) override;

		////////////////////////////////////////////////////////�ڵ������ز���//////////////////////////////////////////////////////////////////////////////////

		virtual void highlight(void*node) override;

		virtual void unHighlight(void*node) override;

		virtual void transparentAll() override;

		virtual void unTransParentAll() override;

		virtual void setToTransparent(void* node, float opacity = 0.8) override;

		virtual void* getModelRoot() override;

		virtual bool revertColorOfNode(void* node) override;

		virtual bool revertColorOfNodes(const std::vector<void*>& nodes) override;

		virtual bool revertStateColorOfConstructs(void* ptr) override;

		virtual void setStateColorToConstructs(void* ptr, int red, int green, int blue) override;

#pragma endregion
	protected:
		std::unique_ptr<bimWorld::INodeController> m_controller;
	};
}
#endif // __NODE_CONTROLLER_WRAPPER_H__