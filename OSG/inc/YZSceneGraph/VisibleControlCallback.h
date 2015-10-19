#ifndef __VISIBLE_CONTROL_CALLBACK_H__
#define __VISIBLE_CONTROL_CALLBACK_H__
#include <osg/NodeCallback>

namespace bimWorld
{
	struct DrawableCullCallback : public osg::Drawable::CullCallback
	{
		/** do customized cull code.*/
		virtual bool cull(osg::NodeVisitor*, osg::Drawable* drawable, osg::State* /*state*/) const
		{
			for each (auto parent in drawable->getParents())
			{
				if (!allParentsAreVisible(parent))
				{
					return true;
				}
			}
			//std::cout << "Drawable cull callback " << drawable << std::endl;
			return false;
		}
		bool allParentsAreVisible(osg::Node* child) const
		{
			if (!child)
			{
				return false;
			}
			auto parents = child->getParents();
			for each (auto parent in parents)
			{
				if (parent->getNodeMask() == 0x0)
				{
					return false;
				}
				if (!allParentsAreVisible(parent))
				{
					return false;
				}
			}
			return true;
		}

	};
	class VisibleControlCallback :public osg::NodeCallback
	{
	public:
		VisibleControlCallback()
		{

		}
		virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
		{
			if (allParentsAreVisible(node))
			{
				std::mutex _mutex;
				_mutex.lock();
				node->setUserValue(NODE_VISIBILITY, true);
				_mutex.unlock();
				traverse(node, nv);
			}
			else
			{
				std::mutex _mutex;
				_mutex.lock();
				node->setUserValue(NODE_VISIBILITY, false);
				_mutex.unlock();
			}
		}

		bool allParentsAreVisible(osg::Node* child)
		{
			if (!child)
			{
				return false;
			}
			auto parents = child->getParents();
			for each (auto parent in parents)
			{
				if (parent->getNodeMask() == 0x0)
				{
					return false;
				}
				if (!allParentsAreVisible(parent))
				{
					return false;
				}
			}
			return true;
		}


	};

}

#endif // __VISIBLE_CONTROL_CALLBACK_H__