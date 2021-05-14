#pragma once


// Standard libraries
// Standard libraries
#include <math.h>
#include <float.h>
#include <stdlib.h>
#include <iostream>
#include <map>
#include <unordered_map>
#include <set>
#include <stack>
#include <unordered_set>
#include <vector>
#include <list>
#include <forward_list>
#include <memory>
#include <string>
#include <algorithm>
#include <fstream>
#include <limits>
#include <bitset>
#include <deque>
#include <random>
#include <iomanip>
#include <cstdlib>
#include <sstream>
#include <stdlib.h>
#include <assert.h>


/*
#ifdef WIN32
#include <vld/vld.h>
#endif
*/


// Qt headers are included separately to prevent #undef'ing of OpenGL functions
#include <QXmlStreamReader>
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QDebug>
#include <QApplication>
#include <QDataStream>
#include <QTcpServer>
#include <QColor>
#include <QOpenGLContext>


// OpenMP header
#ifdef _OPENMP
#include <omp.h>
#endif


// Neutrino headers
#include "globals_UI.h"
#include "globals_Physics.h"
#include "globals_Render.h"
#include "globals_RenderNEO.h"
#include "globals_Core.h"
#include "globals.h"
#include "globals_Geometry.h"
#include "Utility/mathUtil.h"
#include "Utility/Math/includeMath.h"
#include "Utility/NEAABB.h"




// This is included last to prevent warnings
//#include <qmath.h>


#ifdef OPENCL_AVAILABLE
#include "CL/opencl.h"
#include "CL/cl.hpp"
#endif
