#include "Coordinate.h"
#include "YZModelCoreInternal.h"

bimWorld::Coordinate::Coordinate(YZModelCoreInternal * host) :
	YZModelCoreModule(host)
{
}

util::CoordinateHelper * bimWorld::Coordinate::CoordinateHelper()
{
	if (!m_coordHelper)
	{
		m_coordHelper = new util::CoordinateHelper(m_host->_ViewerData()->ModelViewer()->getCamera());
	}
	return m_coordHelper;
}
