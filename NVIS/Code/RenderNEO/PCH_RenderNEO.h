#pragma once

#include <assert.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <float.h>
#include <fcntl.h>      /* for O_BINARY */
#include <sys/types.h>
#include <sys/stat.h>

#if _WIN32
#include <io.h>         /* for setmode() */
#include <direct.h>
#endif

// OpenMP header
#ifdef _OPENMP
#include <omp.h>
#endif

#if defined __cplusplus
// Standard libraries
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <list>
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

// Qt headers are included separately to prevent #undef'ing of OpenGL functions
#include <QXmlStreamReader>
#include <QDebug>
#include <QDataStream>
#include <QThread>
#include <QElapsedTimer>
#include <QTimer>
#include <QFile>
#include <QDir>
#include <QSettings>
#include <QMetaProperty>
#include <QProcess>
#include <QStack>
#include <QMap>
#include <QString>
#include <QEvent>
#include <QLibrary>
#include <QPluginLoader>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QValidator>
#include <QDateTime>
#include <QPainter>
#include <QMutex>
#include <QMutexLocker>
#include <QOpenGLTexture>
#include <QOpenGLShaderProgram>
#include <QOpenGLShader>
#include <QOffscreenSurface>
#include <QOpenGLContext>
#include <QtOpenGLExtensions/QOpenGLExtensions>
#include <QOpenGLFunctions>
#include <QOpenGLFunctions_4_3_Compatibility>
#include <QOpenGLFunctions_4_1_Compatibility>
#include <QOpenGLFunctions_3_3_Compatibility>



#include "FTGL/ftgl.h"

// Neutrino headers
#include "globals_Core.h"
#include "globals.h"

#include "globals_Geometry.h"
#include "globals_Physics.h"
#include "globals_PostProcessor.h"
#include "globals_Render.h"
#include "globals_RenderNEO.h"



#include "Utility/mathUtil.h"
#include "Utility/Math/includeMath.h"
#include "Utility/NEAABB.h"
#include "Utility/NEMocDefs.h"

// This is included last to prevent warnings
#include <qmath.h>

#include "glm/glm.hpp"
#include <glm/gtc/type_ptr.hpp>

#ifdef OPENCL_AVAILABLE
#include "CL/opencl.h"
#include "CL/cl.hpp"
#endif


#if USE_EXT_GFSDK_SSAO
// HBAO+
#include "GFSDK_SSAO_GL/GFSDK_SSAO.h"
#endif

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


#endif
