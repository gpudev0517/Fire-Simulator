#pragma once

// Standard libraries
#include <vector>
#include <set>
#include <iostream>
#include <algorithm>
#include <forward_list>
#include <cfloat>


#include <QtCore>
#include <QTcpServer>
#include <QTcpSocket>


// OpenMP header
//#ifdef _OPENMP
#include <omp.h>
//#endif

// Our headers
#include "Plugins/globals_Plugin.h"
#include "Core/globals_Core.h"
#include "Physics/globals_Physics.h"
#include "Core/globals.h"
#include "Utility/mathUtil.h"
#include "Utility/Math/includeMath.h"
#include "Utility/NEAABB.h"
#include "Utility/geometry.h"

