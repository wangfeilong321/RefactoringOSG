#include "ConstructSelectListener.h"
#include "ModelViewer.h"

bimWorld::ConstructSelectListener::ConstructSelectListener(ModelViewer* pModelView /*= NULL*/, bool needZoom /*= true*/) :
m_needZoom(needZoom)
#ifdef TRACE_ALL
, m_sqhLogs("sqh")
#endif
{
	currentSelected.clear();
	modelViewer = pModelView;
	if (pModelView) {
		//pModelView->->registerModelListener(this);
	}
}

void bimWorld::ConstructSelectListener::onModelLoad()
{
	currentSelected.clear();
}

void bimWorld::ConstructSelectListener::onSelect(const std::string ids[], int n)
{
#ifdef TRACE_ALL
	std::wostringstream stream;
	stream << "onSelect.";
	auto eid = m_sqhLogs.ExcutionStart(stream, "ConstructSelectListener", "onSelect");
#endif // TRACE_ALL

	std::vector<void*> nodes;

	if (!modelViewer || n <= 0)
		setCurrentSelect(nodes);

	// <Orbit 模式下只能单选>
	if (modelViewer->CameraManipulator()->getCameraManipulatorMode() == Orbit)
		n = 1;

	for (int i = 0; i < n; i++)
	{
		nodes.push_back(modelViewer->NodeControl()->findNodeById(ids[i]));
	}
	setCurrentSelect(nodes);

#ifdef TRACE_ALL
	m_sqhLogs.ExcutionEnd(eid);
#endif // TRACE_ALL
}

void bimWorld::ConstructSelectListener::onModelUnload()
{
	currentSelected.clear();
}

void bimWorld::ConstructSelectListener::clearSelection()
{
	if (!modelViewer)
		return;

	// cancel last highlighted nodes.
	for (int i = 0; i < currentSelected.size(); i++)
	{
		modelViewer->NodeControl()->unHighlight(currentSelected[i]);
	}
	currentSelected.clear();
}

std::vector<void*> bimWorld::ConstructSelectListener::getCurSelect()
{
	return currentSelected;
}

void bimWorld::ConstructSelectListener::setCurrentSelect(std::vector<void*> newSelection)
{
	if (!modelViewer)
		return;

	// if same return;
	if (newSelection.size() == currentSelected.size())
	{
		for (int i = 0; i < newSelection.size(); i++)
		{
			bool has = false;
			for (int j = 0; j < currentSelected.size(); j++)
			{
				if (newSelection[i] == currentSelected[j])
				{
					has = true;
					break;
				}
			}
			if (!has)
				break;
		}
	}

	if (m_needZoom)
	{
		if (modelViewer->CameraManipulator()->getCameraManipulatorMode() == Orbit)
			modelViewer->ZoomModel()->zoomTo(newSelection[0]);
		else
			modelViewer->ZoomModel()->zoomTo(newSelection.data());
	}
	// cancel last highlighted nodes.
	clearSelection();

	// highlight current nodes.
	for (int i = 0; i < newSelection.size(); i++)
	{
		modelViewer->NodeControl()->highlight(newSelection[i]);
	}

	currentSelected = newSelection;
}

//void bimWorld::ConstructSelectListener::focusAndSelect(void* newSelection)
//{
//	if (modelViewer && m_needZoom)
//	{
//		modelViewer->ZoomModel()->zoomTo(newSelection);
//	}
//	std::vector<void*> nodes;
//	nodes.push_back(newSelection);
//	setCurrentSelect(nodes);
//}
