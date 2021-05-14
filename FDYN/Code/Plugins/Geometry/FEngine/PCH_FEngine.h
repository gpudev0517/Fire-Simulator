#pragma once

// Standard libraries
#include <vector>
#include <memory>
#include <iostream>
#include <forward_list>
#include <string>
#include <iostream>



// Qt includes
#include <QCoreApplication>
#include <QLibrary>
#include <QDebug>

#include <QtCore>
#include <QColor>
#include <QPair>
#include <QDataStream>
#include <QProcess>
#include <QStandardPaths>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

// Rendering
#include <QOpenGLContext>
#include <QOffscreenSurface>
#include <QOpenGLTexture>
#include <QOpenGLShaderProgram>
#include <QOpenGLShader>
#include <QtOpenGLExtensions/QOpenGLExtensions>
#include <QOpenGLFunctions>
#include <QOpenGLFunctions_4_3_Compatibility>
#include <QOpenGLFunctions_4_1_Compatibility>
#include <QOpenGLFunctions_3_3_Compatibility>



// Our headers
#include "globals_Plugin.h"
#include "globals_Geometry.h"
#include "globals_Core.h"
#include "globals_Render.h"
#include "globals.h"
#include "Utility/mathUtil.h"
#include "Utility/Math/includeMath.h"
#include "Utility/NEAABB.h"

typedef bool  (*f_NewProject)(char*);
typedef bool  (*f_OpenProject)(char*, bool);
typedef bool  (*f_ImportPlantProjects)( char *, char *, char *);
typedef bool  (*f_SaveProjectAs)( char *);
typedef bool  (*f_SetTempWorkLoc)( char *);
typedef char* (*f_GetTempWorkLoc)();
typedef bool  (*f_SaveTempWork)();
typedef bool  (*f_ProjectHasRecover)( char * );

typedef char *(*f_GetZonesFunc)(char *);
typedef char *(*f_GetScenariosFunc)(char *);
typedef char *(*f_GetZonesForScenarioFunc)(int);
typedef char *(*f_GetScenariosForZoneFunc)(int);

typedef char *(*f_GetFranxZoneTreeFunc)(int);
typedef char *(*f_GetPlantZoneTreeFunc)(int);
typedef char *(*f_GetPlantScenarioTreeFunc)(int);

typedef char *(*f_GetFranxScenarioTreeFunc)(int);
typedef char *(*f_GetPlantScenarioTreeFunc)(int);
typedef char *(*f_UpdateItemFunc)(char *, int);

typedef int   (*f_NewModelItemFunc)( char *, int, int );
typedef int   (*f_SaveModelItemFunc)(char *, int);
typedef char *(*f_SaveModelItemsFunc)(char *, int);

typedef int   (*f_NewPhysicalOnlyItemFunc)(char *, int);
typedef char *(*f_DeleteItemFunc)(char *);
typedef int   (*f_ItemReferenceCnt)(char *);
typedef char *(*f_RemoveItem)( char *, int );

typedef char *(*f_AddItemToScenarioFunc)( char *, int, int  );
typedef char *(*f_RemoveScenarioItemFunc)( char *, int, int );
typedef char *(*f_RemoveItemFromAllScenariosFunc)( char * );

typedef char *(*f_SaveScenarioAsFunc)( char *, char * );
typedef int   (*f_NewScenarioFunc)( char *, int );
typedef char *(*f_DeleteScenarioFunc)( char * );

typedef char *(*f_GetFRANXTreeFunc)( int, int );
typedef char *(*f_ExcludeFRANXItemFunc)( char *, int, int );
typedef char *(*f_IncludeFRANXItemFunc)( char *, int, int );
typedef char *(*f_GetBaseFRANXModelLoc)();


typedef char* (*f_GetFranxMasterLoc)();
typedef bool  (*f_GetFranxMasterChanged)();
typedef bool  (*f_SetFranxMasterLoc)( char *);

typedef bool  (*f_RunFranxScenario)(int, char *, char *);
typedef char* (*f_RunFranxScenarios)(char *);


typedef char* (*f_GetDefaultMaterialProps)(bool);

typedef char* (*f_GetMaterialProps)();
typedef char* (*f_SaveMaterialProps)(char *);
typedef int   (*f_SaveSingleMaterialProps)(char *);
typedef char* (*f_DeleteMaterialProps)(char *);

typedef char* (*f_GetFireMaterialProps)();
typedef char* (*f_SaveFireMaterialProps)(char *);
typedef int   (*f_SaveSingleFireMaterialProps)(char *);
typedef char* (*f_DeleteFireMaterialProps)(char *);

typedef char* (*f_GetFailureCriteriaProps)();
typedef char* (*f_SaveFailureCriteriaProps)(char *);
typedef int   (*f_SaveSingleFailureCriteriaProps)(char *);
typedef char* (*f_DeleteFailureCriteriaProps)(char *);

typedef char* (*f_GetCriteriaProps)();
typedef char* (*f_SaveCriteriaProps)(char *);
typedef int   (*f_SaveSingleCriteriaProps)(char *);
typedef char* (*f_DeleteCriteriaProps)(char *);

typedef char* (*f_GetSimInfoProps)();
typedef int   (*f_SaveSimInfoProps)( char * );

typedef char* (*f_GetAlarmProps)();
typedef char* (*f_SaveAlarmProps)(char *);
typedef int   (*f_SaveSingleAlarmProps)(char *);
typedef char* (*f_DeleteAlarmProps)(char *);

typedef char* (*f_GetScenarioFailureOutput)(int);

typedef int   (*f_RunNewCFASTSimulation)(char *, int, char *, char *);
typedef int   (*f_RunExistingCFASTSimulation)(char *, int, char *);

typedef char *(*f_TestGetJSONFunc)();

typedef char *(*f_SaveSimulationItems)( char *, int );
typedef int   (*f_SaveSingleSimulationItem)( char *, int );
typedef char *(*f_GetSimulationItemsInZone)(int);
typedef char *(*f_GetSimulationItemsInZoneForType)(int , char * );



typedef  void (*f_SetCFASTLoc)(char *);
typedef  char *(*f_GetCFASTLoc)();

typedef  char *(*f_GetCableSpecsList)(int, int, int, int, int );
typedef  int   (*f_AddCableSpec)(char *);
typedef  char *(*f_GetCableSpecOptions)();


typedef char *(*f_GetUnknownItemsLocName)();
typedef void  (*f_SetUnknownItemsLocName)( char *);

typedef bool  (*f_SetUIProjectSetting)(char *, char * );
typedef char *(*f_GetUIProjectSetting)(char *);

typedef bool (*f_SetUIAppSetting)(char *, char *);
typedef char *(*f_GetUIAppSetting)(char *name);
