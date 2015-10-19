#ifndef __I_MOUSE_HANDLER_H__
#define __I_MOUSE_HANDLER_H__
#include <osgGA/GUIEventAdapter>
#include <osgGA/GUIActionAdapter>
#include "config.h"

namespace bimWorld
{
	namespace Manipulator
	{
		class YIZHU_EXPORT IMouseHandler
		{
		public:
			IMouseHandler()
			{
			}

			// <�������̧���¼�>
			// <�������true���ж��¼����´���>
			virtual bool handleMouseRelease(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa) = 0;

			// <������갴���¼�>
			// <�������true���ж��¼����´���>
			virtual	bool handleMousePush(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa) = 0;

			// <��������϶��¼�>
			// <�������true���ж��¼����´���>
			virtual	bool handleMouseDrag(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa) = 0;
		};
	}
}

#endif // __I_MOUSE_HANDLER_H__