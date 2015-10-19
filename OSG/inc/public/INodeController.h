#ifndef __INODE_CONTROLLER_H__
#define __INODE_CONTROLLER_H__
#include <string>
#include <vector>
#include "config.h"

namespace bimWorld
{
	class YIZHU_EXPORT INodeController
	{
	public:
		///////////////// <������ʾ����> ///////////////////

		virtual void hideAll() = 0;

		virtual void unHideAll() = 0;

		virtual void hide(void *node) = 0;

		virtual void unHide(void *node) = 0;

		virtual void hide(const std::vector<void*>& nodes) = 0;

		virtual void unHide(const std::vector<void*>& nodes) = 0;

		virtual void hideOthers(void *node) = 0;

		virtual void unHideOthers(void *node) = 0;

		// <�л�����>
		virtual void toggleHide(void *node) = 0;

		virtual void toggleHide(const std::vector<void*>& nodes) = 0;

		virtual void setVisible(void *node, bool visible) = 0;

		// <��������>
		virtual void setVisible(const std::vector<void*>& nodes, bool visible) = 0;
		
		// <�жϹ����ɼ���>
		virtual bool isVisible(void *node) = 0;

		///////////////////////////////////////�ڵ������ز���//////////////////////////////////////

		virtual void highlight(void* node) = 0;

		virtual void unHighlight(void* node) = 0;

		virtual void transparentAll() = 0;

		virtual void unTransParentAll() = 0;

		//virtual void unTransparent(void* node) = 0;

		virtual void setToTransparent(void* node, float opacity = 0.8) = 0;

		virtual void* getModelRoot() = 0;

		virtual bool revertColorOfNode(void* node) = 0;

		virtual bool revertColorOfNodes(const std::vector<void*>& nodes) = 0;

		virtual bool revertStateColorOfConstructs(void* ptr) = 0;

		virtual void setStateColorToConstructs(void* ptr, int red, int green, int blue) = 0;
	};
}

#endif // __INODE_CONTROLLER_H__