#ifndef __IBILLBOARD_H__
#define __IBILLBOARD_H__

#include "config.h"
#include "DataModel.h"
namespace bimWorld
{
	class YIZHU_EXPORT IBillboard
	{
	public:
		///-------------------------------------------------------------------------------------------------
		/// <summary>	ÃÌº”π„∏Ê≈∆. </summary>
		///
		/// <remarks>	Sqh, 2015/5/6. </remarks>
		///
		/// <param name="selectedConstruct">	The selected construct. </param>
		/// <param name="callback">				[in,out] If non-null, the callback. </param>
		/// <param name="img_path_name">		Full pathname of the image file. </param>
		/// <param name="externalData">			Information describing the external. </param>
		///-------------------------------------------------------------------------------------------------
		virtual int addBillboard(void* selectedConstruct[], const void(*callback)(BillboardData*, const void*), const char* img_path_name = NULL, const void* externalData = NULL) = 0;

		virtual bool removeBillboard(int id) = 0;
	};

}
#endif // __IBILLBOARD_H__