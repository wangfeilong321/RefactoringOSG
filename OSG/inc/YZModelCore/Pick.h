#ifndef __PICK_H__
#define __PICK_H__
//#include <osg/StateSet>
//#include <osg/ref_ptr>
#include "YZModelCoreModule.h"
#include "DataModel.h"
namespace bimWorld
{
	typedef std::vector<ModelViewerListener*> ListenerSet;

	class YIZHU_EXPORT Pick
	{
	public:
	protected:
		// <Ê°È¡×´Ì¬>
		//osg::ref_ptr<osg::StateSet> m_selectionRenderMode;

		// <ÊÓÍ¼¼àÌýÆ÷¼¯ºÏ>
		ListenerSet m_modelViewerListeners;
	private:
	};
}
#endif // __PICK_H__