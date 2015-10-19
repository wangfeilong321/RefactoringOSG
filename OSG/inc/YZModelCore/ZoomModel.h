#ifndef __ZOOM_MODEL_H__
#define __ZOOM_MODEL_H__

#include <string>
#include "YZModelCoreModule.h"
#include "IZoomModel.h"

namespace bimWorld
{
	class YIZHU_EXPORT ZoomModel :public YZModelCoreModule, public IZoomModel
	{
	public:
		ZoomModel(YZModelCoreInternal* host);

		/////////////////////////�ڵ�zoom/////////////////////////////////

		//// <��������>
		//virtual bool zoomTo(const std::string& id, bool withoutAnimation = true) override;
		// <�����ڵ����>
		virtual bool zoomTo(void* node, bool withoutAnimation = true, ViewDirection direction = ViewDirection::Front) override;
		//// <�����ڵ����>
		//virtual bool zoomTo(const std::string* nodes, int length, bool withoutAnimation = true) override;
		// <�����ڵ����>
		virtual bool zoomTo(void** nodes, int length, bool withoutAnimation = true, ViewDirection direction = ViewDirection::Front) override;
		
		// <�������ڵ����>
		virtual bool zoomRoot(bool withoutAnimation = true, ViewDirection direction = ViewDirection::Front) override;

		virtual void zoom(double dy) override;
	protected:
	};
}

#endif // __ZOOM_MODEL_H__