#pragma once

#include <assert.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>


#ifdef WIN32
#include <windows.h>
#endif

#if defined __cplusplus
// Standard libraries
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <random>
#include <iomanip>
#include <forward_list>
#include <unordered_set>
#include <set>
#include <cassert>
#include <memory>
#include <stdexcept>
#include <iterator>
#include <queue>
#include <algorithm>
#include <utility>
#include <cstddef>
#include <cfloat>

extern "C"
{
//FFMpeg
#include <libavutil/avassert.h>
#include <libavutil/error.h>
#include <libavutil/channel_layout.h>
#include <libavutil/opt.h>
#include <libavutil/mathematics.h>
#include <libavutil/timestamp.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
}

//
// Need to undefine this because gcc confuses this PixelFormat with QOpenGLTexture::PixelFormat
// and causes linker errors
//
#undef PixelFormat
//


// Qt headers are included separately to prevent #undef'ing of OpenGL functions

#include <QString>
#include <QDir>
#include <QFile>
#include <QStack>
#include <QElapsedTimer>
#include <QTimer>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include <QPainter>
#include <QOffscreenSurface>
#include <QOpenGLTexture>
#include <QOpenGLWidget>
#include <QOpenGLContext>
#include <QOpenGLShaderProgram>
#include <QtOpenGLExtensions/QOpenGLExtensions>
#include <QOpenGLFunctions>
#include <QOpenGLFunctions_4_3_Compatibility>
#include <QOpenGLFunctions_4_1_Compatibility>
#include <QOpenGLFunctions_3_3_Compatibility>
#include <QGraphicsItem>
#include <QGraphicsPathItem>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneMoveEvent>
#include <QMouseEvent>
#include <QMutex>
#include <QMutexLocker>



#include "FTGL/ftgl.h"

#define NEWMANIP
#include "gizmo/GizmoTransformRotate.h"
#include "gizmo/IGizmo.h"
#include "gizmo/GizmoTransformMove.h"
#include "gizmo/GizmoTransformScale.h"




// Neutrino headers
#include "globals_GLI.h"
#include "globals_Render.h"
#include "globals_Geometry.h"
#include "globals_Core.h"
#include "globals.h"


#include "Utility/mathUtil.h"
#include "Utility/Math/includeMath.h"
#include "Utility/NEAABB.h"

// This is included last to prevent warnings
#include <qmath.h>




#include "gizmo/IGizmo.h"




#endif
