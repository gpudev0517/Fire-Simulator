#pragma once

#include <assert.h>
#include <stdio.h>
#include <time.h>
#include <cmath>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <float.h>
#include <fcntl.h>      /* for O_BINARY */
#include <sys/types.h>
#include <sys/stat.h>

#ifdef _WIN32
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

// #include <boost/graph/adjacency_list.hpp>
// #include <boost/algorithm/string/join.hpp>
// #include <boost/math/special_functions/round.hpp>
// #include <boost/utility/enable_if.hpp>
// #include <boost/type_traits/is_integral.hpp>
// #include <boost/type_traits/is_floating_point.hpp>
// #include <boost/math/special_functions/round.hpp>
// #include <boost/multi_array.hpp>

// Qt headers are included separately to prevent #undef'ing of OpenGL functions
#include <QXmlStreamReader>
#include <QDebug>
#include <QAction>
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
#include <QMutex>
#include <QMutexLocker>

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

#undef foreach
#include <openvdb/openvdb.h>
#include <openvdb/tools/LevelSetFilter.h>
#include <openvdb/tools/LevelSetRebuild.h>
#include <openvdb/tools/SignedFloodFill.h>
#include <openvdb/util/Util.h>
#include <openvdb/tools/ParticlesToLevelSet.h>
#include <openvdb/tree/Tree.h>
#include <openvdb/tools/VolumeToMesh.h>
#include <openvdb/tools/Composite.h>
#include <openvdb/tools/MeshToVolume.h>
#include <openvdb/tools/LevelSetUtil.h>
#include <openvdb/tools/Prune.h>
#include <openvdb/tools/Morphology.h>       // for deactivate()
#include <openvdb/util/NullInterrupter.h>
#include <openvdb/tree/ValueAccessor.h>
#include <openvdb/tools/Interpolation.h>
#include <openvdb/tree/LeafManager.h>
#include <openvdb/tools/PointScatter.h>
#include <openvdb/tools/GridTransformer.h>  // for resampleToMatch()
#include <openvdb/math/Coord.h>

/*
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkFloatArray.h>
#include <vtkCellArray.h>
#include <vtkPointData.h>
#include <vtkXMLPolyDataWriter.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkPolyDataMapper.h>
#include <vtkStructuredGridWriter.h>
#include <vtkXMLStructuredGridWriter.h>
#include <vtkUnstructuredGrid.h>
#include <vtkXMLUnstructuredGridWriter.h>
#include <vtkStructuredGrid.h>
*/


#include "FTGL/ftgl.h"

// Neutrino headers


#include "globals_Core.h"
#include "globals_PostProcessor.h"
#include "globals_Geometry.h"
#include "globals_Render.h"
#include "globals.h"

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

#include <tbb/parallel_for.h>
#include <tbb/mutex.h>
#include <tbb/parallel_reduce.h>
#include <tbb/enumerable_thread_specific.h>
#include <tbb/blocked_range.h>

#if USE_EXT_GFSDK_SSAO
// HBAO+
#include "GFSDK_SSAO_GL/GFSDK_SSAO.h"
#endif

extern "C"
{
//FFMpeg
#include <libavcodec/avcodec.h>
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
