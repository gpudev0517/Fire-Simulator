#pragma once

#if NEUTRINO_NEUI_BUILD
#define NEFEXP_NEUI Q_DECL_EXPORT
#define NECEXP_NEUI Q_DECL_EXPORT
#else
#define NEFEXP_NEUI Q_DECL_IMPORT
#define NECEXP_NEUI Q_DECL_IMPORT
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

