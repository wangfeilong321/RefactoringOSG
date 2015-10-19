#ifndef __MOUSE_MANIPULATOR_CONTAINER_H__
#define __MOUSE_MANIPULATOR_CONTAINER_H__
#include <map>
#include <string>
#include "MouseManipulatorBase.h"
#include "IMouseHandler.h"
#include <osg/Referenced>

namespace bimWorld
{
	namespace Manipulator
	{

		class YIZHU_EXPORT MouseManipulatorContainer :public osg::Referenced, IMouseHandler
		{
		public:

#pragma region IMouseHandler

			// <�������̧���¼�>
			// <�������true���ж��¼����´���>
			virtual bool handleMouseRelease(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa) override
			{
				for (auto iter = m_set.begin(); iter != m_set.end();)
				{
					auto manip = (*iter).second;
					if (!manip.valid())
					{
						m_set.erase(iter++);
						continue;
					}

					if (manip->handleMouseRelease(ea, aa))
						return true;
					++iter;
				}
				return false;
			}

			// <������갴���¼�>
			// <�������true���ж��¼����´���>
			virtual	bool handleMousePush(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa) override
			{

				for (auto iter = m_set.begin(); iter != m_set.end();)
				{
					auto manip = (*iter).second;
					if (!manip.valid())
					{
						m_set.erase(iter++);
						continue;
					}

					if (manip->handleMousePush(ea, aa))
						return true;
					++iter;
				}
				return false;
			}

			// <��������϶��¼�>
			// <�������true���ж��¼����´���>
			virtual	bool handleMouseDrag(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa) override
			{
				for (auto iter = m_set.begin(); iter != m_set.end();)
				{
					auto manip = (*iter).second;
					if (!manip.valid())
					{
						m_set.erase(iter++);
						continue;
					}

					if (manip->handleMouseDrag(ea, aa))
						return true;
					++iter;
				}
				return false;
			}

#pragma endregion

			void addOrReplaceMouseManipulator(std::string keyName, MouseManipulatorBase* manip)
			{
				m_set[keyName] = manip;
			}

			void removeMouseManipulator(std::string keyName)
			{
				m_set.erase(keyName);
			}

		protected:
			std::map<std::string, osg::ref_ptr<MouseManipulatorBase> > m_set;
		};

	}
}
#endif // __MOUSE_MANIPULATOR_CONTAINER_H__