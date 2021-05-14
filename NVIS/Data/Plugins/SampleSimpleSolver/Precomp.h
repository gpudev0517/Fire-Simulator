#pragma once

// Standard libraries
#include <vector>
#include <memory>

// For accessing all OpenGL functions
#include "GL/glew.h"

// Qt headers are included separately to prevent #undef'ing of OpenGL functions
#include <QtCore>
#include <QAction>

// OpenMP header
//#ifdef _OPENMP
#include <omp.h>
//#endif

// Our headers
#include "Utility/globals.h"
#include "Utility/mathUtil.h"
#include "Utility/Math/includeMath.h"
#include "Utility/NEAABB.h"


