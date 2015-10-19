#ifndef YZTOOLS_GLOBAL_H
#define YZTOOLS_GLOBAL_H

#include <QtCore/qglobal.h>

#ifdef YZTOOLS_LIB
# define YZTOOLS_EXPORT Q_DECL_EXPORT
#else
# define YZTOOLS_EXPORT Q_DECL_IMPORT
#endif

#endif // YZTOOLS_GLOBAL_H
