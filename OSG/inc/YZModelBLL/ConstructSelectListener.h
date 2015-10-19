#ifndef __CONSTRUCT_SELECT_LISTENER_H__
#define __CONSTRUCT_SELECT_LISTENER_H__
#include "DataModel.h"

namespace bimWorld
{
	class ModelViewer;

	class YIZHU_EXPORT ConstructSelectListener : public ModelViewerListener
	{
	public:
		explicit ConstructSelectListener(ModelViewer* pModelView = NULL, bool needZoom = true);

		virtual void onModelLoad();

		virtual void onSelect(const std::string ids[], int n);

		virtual void onModelUnload();

		virtual void clearSelection() override;

		virtual ~ConstructSelectListener()
		{
		}

		std::vector<void*> getCurSelect();

		virtual void setCurrentSelect(std::vector<void*> newSelection);

		//void focusAndSelect(void* newSelection);

	protected:
		std::vector<void*>		currentSelected;
		ModelViewer* modelViewer;
		bool m_needZoom;
#ifdef TRACE_ALL
		YZLogs m_sqhLogs;
#endif
	};
}
#endif // !__CONSTRUCT_SELECT_LISTENER_H__
