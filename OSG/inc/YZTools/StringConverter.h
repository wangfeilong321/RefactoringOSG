#ifndef __STRING_CONVERTER_H__
#define __STRING_CONVERTER_H__

#include <config.h>
#include <QtCore/QString>

namespace util
{
	class StringConverter
	{
	public:
		static QString toQString(const std::string& text)
		{
			return QString::fromStdString(text);
		}

		static QString toQString(const std::wstring& text)
		{
			return QString::fromStdWString(text);
		}

		static bimWorld::YZString toYZString(const QString& text)
		{
#ifdef YIZHU_BIMWORLD_USE_WCHAR
			return text.toStdWString();
#else
			return text.toStdString();
#endif
		}
	
	protected:
	private:
	};
}

#endif // __STRING_CONVERTER_H__