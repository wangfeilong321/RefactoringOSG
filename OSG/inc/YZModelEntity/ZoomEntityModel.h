#ifndef __ZOOM_ENTITY_MODEL_H__
#define __ZOOM_ENTITY_MODEL_H__
#include "ZoomModelWrapper.h"
#include "IZoomEntityModel.h"
#include "YZModelEntityModule.h"

namespace bimWorld
{
	class ZoomEntityModel:public ZoomModelWrapper
	{
	public:
		ZoomEntityModel(YZModelEntityInternal* host);

		// <��������>
		virtual bool zoomTo(const std::string& id, bool withoutAnimation = true) override;

		// <�����ڵ����>
		virtual bool zoomTo(const std::vector<std::string>& nodes, bool withoutAnimation = true, ViewDirection direction = ViewDirection::Front) override;

	protected:
	};
}
#endif // __ZOOM_ENTITY_MODEL_H__