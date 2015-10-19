#ifndef __CAMERA_SETTING_H__
#define __CAMERA_SETTING_H__
#include "YZModelCoreModule.h"
#include "DataModel.h"
#include "ICameraSettings.h"

namespace bimWorld
{
	class YIZHU_EXPORT CameraSettings :public YZModelCoreModule, public ICameraSettings
	{
	public:
		CameraSettings(YZModelCoreInternal* host);

		///////////////////////////////////////////////////////////////////////����ӿ���ز���//////////////////////////////////////////////////////////////////////////////////

		virtual bool getCameraView(CameraView& cameraView) override;


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

		virtual bool getCameraView(CameraView& cameraView, const char* strFileAllPath, unsigned char** buffer, int &width, int &height, int &channels) override;

		virtual bool getCameraPerspective(CameraPerspective& cameraPerspective) override;

		virtual bool getCameraSetting(CameraSetting& cameraSetting) override;

		virtual bool setCameraView(const CameraView& cameraView) override;

		virtual bool setCameraPerspective(const CameraPerspective& cameraPerspective) override;

		virtual bool setCameraSetting(const CameraSetting& cameraSetting) override;

		//////////////////////////////////////////////////////////���Ӿ���������//////////////////////////////////////////////////////////////////////////////////

		//// <����Ĭ��ͼ���豸���Բ���>
		//virtual void* createDefaultTraits() override;

		//// <��Ӹ������>
		//virtual void addSlaveCamera(void* traits) override;

	protected:
	};

}
#endif // __CAMERA_SETTING_H__