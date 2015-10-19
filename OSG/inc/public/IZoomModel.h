#ifndef __IZOOM_MODEL_H__
#define __IZOOM_MODEL_H__

#include <string>
#include "config.h"

namespace bimWorld
{
	// <��ͼ����>
	enum class ViewDirection
	{
		Front,
		Back,
		Left,
		Right,
		Top,
		Bottom,
	};

	class YIZHU_EXPORT IZoomModel
	{
	public:
		// <�������ڵ����>
		virtual bool zoomRoot(bool withoutAnimation = true, ViewDirection direction = ViewDirection::Front) = 0;

		//// <��������>
		//virtual bool zoomTo(const std::string& id, bool withoutAnimation = true) = 0;
		
		// <�����ڵ����>
		virtual bool zoomTo(void* node, bool withoutAnimation = true, ViewDirection direction = ViewDirection::Front) = 0;
		
		//// <�����ڵ����>
		//virtual bool zoomTo(const std::string* nodes, int length, bool withoutAnimation = true) = 0;
		
		// <�����ڵ����>
		virtual bool zoomTo(void** nodes, int length, bool withoutAnimation = true, ViewDirection direction = ViewDirection::Front) = 0;

		virtual void zoom(double dy) = 0;
	};
}

#endif // __ZOOM_MODEL_H__