#ifndef __ICAMERA_SETTING_H__
#define __ICAMERA_SETTING_H__
#include "DataModel.h"

namespace bimWorld
{
	class YIZHU_EXPORT ICameraSettings
	{
	public:
		///////////////////////////////////////////////////////////////////////����ӿ���ز���//////////////////////////////////////////////////////////////////////////////////

		virtual bool getCameraView(CameraView& cameraView) = 0;
		
		///-------------------------------------------------------------------------------------------------
		/// <summary>	��ȡ�����ͼ�������н�ͼ. </summary>
		///
		/// <remarks>	Sqh, 2015/5/6. </remarks>
		///
		/// <param name="cameraView">	 	[in,out] ������ȡ���������ͼ. </param>
		/// <param name="strFileAllPath">	����ػ��ͼƬ�洢·���������ļ�������׺. </param>
		/// <param name="buffer">		 	[in,out] If non-null, �����ػ��ͼƬ��������. </param>
		/// <param name="width">		 	[in,out] ͼƬ�Ŀ��. </param>
		/// <param name="height">		 	[in,out] ͼƬ�ĸ߶�. </param>
		/// <param name="channels">		 	[in,out] ͼƬ��ͨ����. </param>
		///
		/// <returns>	true if it succeeds, false if it fails. </returns>
		///-------------------------------------------------------------------------------------------------

		virtual bool getCameraView(CameraView& cameraView, const char* strFileAllPath, unsigned char** buffer, int &width, int &height, int &channels) = 0;

		virtual bool getCameraPerspective(CameraPerspective& cameraPerspective) = 0;

		virtual bool getCameraSetting(CameraSetting& cameraSetting) = 0;

		virtual bool setCameraView(const CameraView& cameraView) = 0;

		virtual bool setCameraPerspective(const CameraPerspective& cameraPerspective) = 0;

		virtual bool setCameraSetting(const CameraSetting& cameraSetting) = 0;

		//////////////////////////////////////////////////////////���Ӿ���������//////////////////////////////////////////////////////////////////////////////////

		//// <����Ĭ��ͼ���豸���Բ���>
		//virtual void* createDefaultTraits() = 0;

		//// <��Ӹ������>
		//virtual void addSlaveCamera(void* traits) = 0;

	};

}
#endif // __ICAMERA_SETTING_H__