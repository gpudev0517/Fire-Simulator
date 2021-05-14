#ifndef GIZMO_GLOBAL
#define GIZMO_GLOBAL

#ifdef WIN32
#if defined(GIZMO_LIBRARY)
#  define GIZMOSHARED_EXPORT __declspec(dllexport)
#else
#  define GIZMOSHARED_EXPORT __declspec(dllimport)
#endif
#else
#   define GIZMOSHARED_EXPORT
#endif

#endif // QTCSV_GLOBAL_H


