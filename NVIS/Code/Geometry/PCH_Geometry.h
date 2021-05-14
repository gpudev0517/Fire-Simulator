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

#include <boost/graph/adjacency_list.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/math/special_functions/round.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/type_traits/is_integral.hpp>
#include <boost/type_traits/is_floating_point.hpp>
#include <boost/math/special_functions/round.hpp>

// Qt headers are included separately to prevent #undef'ing of OpenGL functions
#include <QXmlStreamReader>
#include <QColor>
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
#include <QLibrary>
#include <QPluginLoader>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QSize>



#undef foreach
#include <openvdb/openvdb.h>


// Neutrino headers
#include "globals_Geometry.h"
#include "globals_Core.h"
#include "globals.h"
#include "Utility/mathUtil.h"
#include "Utility/Math/includeMath.h"
#include "Utility/NEAABB.h"
#include "Utility/NEMocDefs.h"

// This is included last to prevent warnings
#include <qmath.h>

#ifdef OPENCL_AVAILABLE
#include "CL/opencl.h"
#include "CL/cl.hpp"
#endif

#include <tbb/parallel_for.h>
#include <tbb/mutex.h>
#include <tbb/parallel_reduce.h>
#include <tbb/enumerable_thread_specific.h>
#include <tbb/blocked_range.h>

#endif

