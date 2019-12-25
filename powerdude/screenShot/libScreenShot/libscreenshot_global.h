#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(LIBSCREENSHOT_LIB)
#  define LIBSCREENSHOT_EXPORT Q_DECL_EXPORT
# else
#  define LIBSCREENSHOT_EXPORT Q_DECL_IMPORT
# endif
#else
# define LIBSCREENSHOT_EXPORT
#endif
