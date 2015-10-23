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
            for(auto iter = drawable->getParents().begin(); iter!= drawable->getParents().end(); iter++)
			{
				if (!allParentsAreVisible(*iter))
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
            for(auto iter = parents.begin(); iter!=parents.end(); iter++)
			{
				if ((*iter)->getNodeMask() == 0x0)
				{
					return false;
				}
				if (!allParentsAreVisible(*iter))
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
            for(auto iter = parents.begin(); iter!=parents.end(); iter++)
			{
                auto parent = *iter;
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