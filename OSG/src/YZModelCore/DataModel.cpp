#include "DataModel.h"
#include <algorithm>
//#include <boost/locale.hpp>
#include <sstream>
#include "StringExt.h"

namespace
{
#ifdef YIZHU_BIMWORLD_USE_WCHAR
	const static bimWorld::YZString floorParamName(L"LevelName");
	const static bimWorld::YZString categoryParamName(L"Category");
	const static bimWorld::YZString systemTypeParamName(L"SystemType");
#define DEFINE_MAT_NAME(name) const YZString& MaterialName::name(){\
    const static YZString color = L#name;\
    return color;\
			  }
#else
	const static bimWorld::YZString floorParamName("LevelName");
	const static bimWorld::YZString categoryParamName("Category");
	const static bimWorld::YZString systemTypeParamName("SystemType");
#define DEFINE_MAT_NAME(name) const YZString& MaterialName::name(){\
    const static YZString color = #name;\
    return color;\
			  }
#endif
#define SET_ATTRIBUTE(element, setting, name) {\
  int count = 0;\
  const double*values = setting.get(MaterialName::name(), &count);\
  if(values != NULL){\
    std::ostringstream os;\
    for(int i = 0;i < count;++ i){\
      if(i > 0)\
        os << ' ';\
      os.precision(6);\
      os << values[i];\
			    }\
    element->SetAttribute(#name, os.str().c_str());\
        }\
			}
#define SET_MATERIAL(element, setting, name){\
  const char *valueStr = element->Attribute(#name);\
  if(valueStr != NULL && *valueStr){\
    std::istringstream valueStream(valueStr);\
    double values[4]={0, 0, 0, unit};\
    if(valueStream >> values[0]){\
      valueStream >> values[1] >> values[2];\
      if(valueStream){\
        valueStream >> values[3];\
	  	  	        }\
      for(int i = 0;i < 4;++ i)\
        values[i] /= unit;\
      setting->set(MaterialName::name(), values);\
			    }\
        }\
			}
}

namespace bimWorld
{

	DEFINE_MAT_NAME(diffuse)
		DEFINE_MAT_NAME(shininess)
		DEFINE_MAT_NAME(ambient)
		DEFINE_MAT_NAME(specular)
		DEFINE_MAT_NAME(emission)


		const double* MaterialSetting::get(const YZString& name, int* count) const
	{

		const double *ret = NULL;
		int cnt = 4;
		if (name == MaterialName::diffuse())
			ret = diffuse;
		if (name == MaterialName::shininess())
		{
			ret = shininess;
			cnt = 1;
		}
		if (name == MaterialName::ambient())
			ret = ambient;
		if (name == MaterialName::specular())
			ret = specular;
		if (name == MaterialName::emission())
			ret = emission;
		if (ret != NULL && count != NULL)
			*count = cnt;
		return ret;
	}
	bool MaterialSetting::set(const YZString& name, const double *values, bool clearOnNull)
	{
		double **ret = NULL;
		int cnt = 4;
		if (name == MaterialName::diffuse())
			ret = &diffuse;
		if (name == MaterialName::shininess())
		{
			ret = &shininess;
			cnt = 1;
		}
		if (name == MaterialName::ambient())
			ret = &ambient;
		if (name == MaterialName::specular())
			ret = &specular;
		if (name == MaterialName::emission())
			ret = &emission;
		if (ret == NULL)
			return false;
		if (values == NULL)
		{
			if (clearOnNull) {
				delete *ret;
				*ret = NULL;
			}
			return true;
		}
		if (*ret == NULL)
			*ret = new double[cnt];
		std::copy(values, values + cnt, *ret);
		return true;
	}
	bool MaterialSetting::assign(const MaterialSetting* newSetting, bool clear)
	{
		set(MaterialName::diffuse(), newSetting->get(MaterialName::diffuse()), clear);
		set(MaterialName::shininess(), newSetting->get(MaterialName::shininess()), clear);
		set(MaterialName::ambient(), newSetting->get(MaterialName::ambient()), clear);
		set(MaterialName::specular(), newSetting->get(MaterialName::specular()), clear);
		set(MaterialName::emission(), newSetting->get(MaterialName::emission()), clear);
		return true;
	}
}
