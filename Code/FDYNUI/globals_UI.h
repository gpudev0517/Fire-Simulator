#pragma once

#if FDYN_FDYNUI_BUILD
#define NEFEXP_FDYNUI Q_DECL_EXPORT
#define NECEXP_FDYNUI Q_DECL_EXPORT
#else
#define NEFEXP_FDYNUI Q_DECL_IMPORT
#define NECEXP_FDYNUI Q_DECL_IMPORT
#endif

/*!
    For knowing from where a command has been issued
*/
enum NECommandIssuer
{
    kSceneInspector,
    kConsole,
    kGLWidget,
    kNodeGraphEditor
};

