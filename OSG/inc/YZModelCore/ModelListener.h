#ifndef __MODEL_LISTENER_H__
#define __MODEL_LISTENER_H__
#include "DataModel.h"

namespace bimWorld
{
	class YIZHU_EXPORT ModelListener
	{
	public:

		////////////////////////////////////////////////////////�û������¼�//////////////////////////////////////////////////////////////////////////////////
		virtual bool						registerModelListener(ModelViewerListener *listener);

		virtual bool						removeModelListener(ModelViewerListener *listener);

		static bool							registerDefaultModelListener(ModelViewerListener *listener);

		static bool							removeDefaultModelListener(ModelViewerListener *listener);

	protected:
	};
}

#endif // __MODEL_LISTENER_H__