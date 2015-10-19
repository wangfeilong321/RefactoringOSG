#ifndef __DATA_MODEL_H__
#define __DATA_MODEL_H__
#include <vector>
#include <map>
#include <set>
#include <iostream>
#include "config.h"

namespace bimWorld
{
	struct YIZHU_EXPORT MaterialName
	{
		static const YZString& diffuse();
		static const YZString& shininess();
		static const YZString& ambient();
		static const YZString& specular();
		static const YZString& emission();
	};
	class YIZHU_EXPORT MaterialSetting
	{
		double *diffuse, *ambient, *specular, *emission, *shininess;
	public:
		MaterialSetting()
		{
			diffuse = ambient = specular = emission = shininess = NULL;
		}
		MaterialSetting(const MaterialSetting& newSetting) {
			diffuse = ambient = specular = emission = shininess = NULL;
			assign(&newSetting, true);
		}
		MaterialSetting& operator=(const MaterialSetting& newSetting)
		{
			if (this != &newSetting)
				assign(&newSetting, true);
			return *this;
		}
		const double *get(const YZString& name, int* count = NULL) const;
		bool set(const YZString& name, const double *values, bool clearOnNull = true);
		bool assign(const MaterialSetting* newSetting, bool clear = false);
		bool isEmpty()
		{
			return diffuse != NULL && ambient != NULL && specular != NULL &&
				emission != NULL && shininess != NULL;
		}
		~MaterialSetting()
		{
			delete diffuse;
			delete ambient;
			delete specular;
			delete emission;
			delete shininess;
		}
	};

	typedef YZString				CategoryName;	//������
	typedef YZString				SystemType;		//ϵͳ����
	typedef YZString				LevelName;		//¥����
	class YIZHU_EXPORT ColorCategory
	{
	public:
		CategoryName name;
		SystemType systemtype;
		std::vector<LevelName> levels;
        MaterialSetting* materialSetting;
		std::vector<std::string> ids;

	public:
        ColorCategory():materialSetting(new MaterialSetting())
		{
			name = CategoryName();
			systemtype = SystemType();
			levels = std::vector<LevelName>();
		}

		ColorCategory(const ColorCategory& newValue) {
			assign(newValue);
		}

		ColorCategory& operator=(const ColorCategory& newSetting)
		{
			if (this != &newSetting)
				assign(newSetting);
			return *this;
		}
		~ColorCategory()
		{
            delete materialSetting;
		}
        void assign(const ColorCategory& newValue) {
            name = newValue.name;
			systemtype = newValue.systemtype;
			levels = newValue.levels;
            materialSetting = new MaterialSetting(*newValue.materialSetting);
			ids = newValue.ids;
        }
	};

	struct CameraView
	{
		double position[3];
		double target[3];
		double upVector[3];
		double matrix[16];
	};
	//͸���Ӿ������
	struct CameraPerspective
	{
		double fov;
		double aspectRatio;
		double zNear;
		double zFar;
	};

	struct CameraSetting
	{
		CameraView view;
		CameraPerspective perspective;
	};

	enum ViewerMode
	{
		MultiSelect,//��ѡģʽ 
		Orbit,		//��ת��ѡ
	};

	class ModelViewerListener
	{
	public:
		virtual void onModelLoad()
		{
		}
		virtual void onSelect(void* ptrs[], int n)
		{
		}
		virtual void onModelUnload()
		{
		}
		virtual void clearSelection()
		{
		}
		virtual ~ModelViewerListener()
		{
		}
	};

#pragma region BillboardData

	struct ArrayInfo
	{
		void* data;
		int length;
	};

#define KEY_CONSTRUCT_IDS "construct_ids"

	class BillboardData
	{
	public:

		void getConstructIds(std::string*& ids, int& length)
		{
			auto c_ids = static_cast<ArrayInfo*>(getProperty(KEY_CONSTRUCT_IDS));
			ids = static_cast<std::string*>(c_ids->data);
			length = c_ids->length;
		}

		void setConstructIds(std::string ids[])
		{
			int length = 0;
			if (ids)
				length = sizeof(ids) / sizeof(ids[0]);
			auto info = new ArrayInfo();
			info->data = ids;
			info->length = length;
			setProperty(KEY_CONSTRUCT_IDS, info);
		}

		void* getProperty(const char* key)
		{
			auto iter = m_map.find(key);
			if (iter == m_map.end())
				return NULL;
			else
				return iter->second;
		}

		void setProperty(const char* key, void* data)
		{
			m_map.insert(std::pair<const char*, void*>(key, data));
		}

	protected:
		std::map<const char*, void*> m_map;
	};

#pragma endregion // BillboardData


}

#endif // __DATA_MODEL_H__
