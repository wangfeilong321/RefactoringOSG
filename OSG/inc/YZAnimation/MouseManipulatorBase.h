#ifndef __MOUSE_MANIPULATOR_BASE_H__
#define __MOUSE_MANIPULATOR_BASE_H__
#include <osg/Referenced>
#include <osgGA/GUIEventAdapter>
#include <osgGA/GUIActionAdapter>
#include "config.h"
#include "IMouseHandler.h"

namespace bimWorld
{
	namespace Manipulator
	{
		class YIZHU_EXPORT MouseManipulatorBase :public osg::Referenced, public IMouseHandler
		{
		public:
			MouseManipulatorBase()
			{
				m_isEnabled = false;
				m_isManipulating = false;
			}

			// <�������̧���¼�>
			// <�������true���ж��¼����´���>
			virtual bool handleMouseRelease(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa) override
			{
				m_isManipulating = false;
				return dealMouseRelease(ea, aa);
			}

			// <������갴���¼�>
			// <�������true���ж��¼����´���>
			virtual bool handleMousePush(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa) override
			{
				if (!m_isEnabled)
					return false;

				m_isManipulating = true;
				// handle

				return dealMousePush(ea, aa);
			}

			// <��������϶��¼�>
			// <�������true���ж��¼����´���>
			virtual bool handleMouseDrag(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa) override
			{
				if (!m_isEnabled)
					return false;

				m_isManipulating = true;
				// handle

				return dealMouseDrag(ea, aa);
			}

			bool IsEnabled()
			{
				return m_isEnabled;
			}

			virtual void Enable()
			{
				m_isEnabled = true;
			}

			virtual void Disable()
			{
				m_isEnabled = false;
			}

		protected:
			bool m_isEnabled;
			bool m_isManipulating;

			// <������갴���¼�>
			// <�������true���ж��¼����´���>
			virtual bool dealMousePush(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa) = 0;

			// <�������̧���¼�>
			// <�������true���ж��¼����´���>
			virtual bool dealMouseRelease(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa) = 0;

			// <��������϶��¼�>
			// <�������true���ж��¼����´���>
			virtual bool dealMouseDrag(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa) = 0;
		};
	}
}

#endif // __MOUSE_MANIPULATOR_BASE_H__