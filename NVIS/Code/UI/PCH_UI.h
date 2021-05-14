#pragma once

#include <assert.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

// OpenMP header
#ifdef _OPENMP
#include <omp.h>
#endif

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
//
// #include <boost/property_map/property_map.hpp>
// #include <boost/graph/topology.hpp>
// #include <boost/graph/adjacency_list.hpp>
// #include <boost/random/linear_congruential.hpp>
// #include <boost/progress.hpp>
// #include <boost/graph/random_layout.hpp>
// #include <boost/graph/fruchterman_reingold.hpp>
// #include <boost/graph/kamada_kawai_spring_layout.hpp>


// Qt headers are included separately to prevent #undef'ing of OpenGL functions
#include <QAbstractItemView>
#include <QAction>
#include <QApplication>
#include <QBitmap>
#include <QBoxLayout>
#include <QButtonGroup>
#include <QCache>
#include <QCheckBox>
#include <QCloseEvent>
#include <QColorDialog>
#include <QComboBox>
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QCompleter>
#include <QCoreApplication>
#include <QDataStream>
#include <QDate>
#include <QDateEdit>
#include <QDateTime>
#include <QDebug>
#include <QDesktopWidget>
#include <QDialog>
#include <QDir>
#include <QDockWidget>
//#include <QDomDocument>
#include <QDoubleSpinBox>
#include <QDropEvent>
#include <QElapsedTimer>
#include <QEvent>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QFileOpenEvent>
#include <QFileSystemWatcher>
#include <QFlags>
#include <QFocusEvent>
#include <QFontDatabase>
#include <QFontDialog>
#include <QFontMetrics>
#include <QFrame>
#include <QGraphicsItem>
#include <QGraphicsPathItem>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneMoveEvent>
#include <QGraphicsView>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QHostAddress>
#include <QIcon>
#include <QImage>
#include <QItemDelegate>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>
#include <QLinkedList>
#include <QListIterator>
#include <QListWidget>
#include <QLocale>
#include <QMainWindow>
#include <QMap>
#include <QMargins>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QMetaClassInfo>
#include <QMetaEnum>
#include <QMetaObject>
#include <QMetaProperty>
#include <QMimeData>
#include <QMouseEvent>
#include <QMultiMap>
#include <QMutex>

#include <QOpenGLTexture>
#include <QOpenGLShaderProgram>
#include <QOpenGLWidget>
#include <QtOpenGLExtensions/QOpenGLExtensions>
#include <QOpenGLFunctions>
#include <QOpenGLFunctions_4_3_Compatibility>
#include <QOpenGLFunctions_4_1_Compatibility>
#include <QOpenGLFunctions_3_3_Compatibility>
#include <QPainter>
#include <QPen>
#include <QPixmap>
#include <QPlainTextEdit>
#include <QPluginLoader>
#include <QPrintEngine>
#include <QPrinter>
#include <QProcess>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QQueue>
#include <QRect>
#include <QRegExpValidator>
#include <QRunnable>
#include <QScreen>
#include <QScrollArea>
#include <QScrollBar>
#include <QSettings>
#include <QSharedPointer>
#include <QShowEvent>
#include <QSignalMapper>
#include <QSize>
#include <QSpinBox>
#include <QSplashScreen>
#include <QStack>
#include <QStringList>
#include <QStringListModel>
#include <QStyle>
#include <QStyleFactory>
#include <QStyleOption>
#include <QStyleOptionButton>
#include <QStyleOptionGroupBox>
#include <QStyledItemDelegate>
#include <QSurfaceFormat>
#include <QSyntaxHighlighter>
#include <QTableWidget>
#include <QTabWidget>
#include <QTcpSocket>
#include <QTextBlock>
#include <QThread>
#include <QThreadPool>
#include <QTimer>
#include <QToolBar>
#include <QToolButton>
#include <QTreeWidget>
#include <QUrlQuery>
#include <QUuid>
#include <QVBoxLayout>
#include <QValidator>
#include <QVariantAnimation>
#include <QVariantMap>
#include <QVector2D>
#include <QVector3D>
#include <QWheelEvent>
#include <QWidget>
#include <QXmlStreamReader>
#include <QtNumeric>
#include <QtTest/QTest>

#include "FTGL/ftgl.h"
#include "assimp/scene.h"
#include "pythonqt/src/PythonQt.h"

#define NEWMANIP
#include "gizmo/IGizmo.h"




#include "Windows/NodeGraphEditor/NodesEditor.h"

// Neutrino headers
#include "globals_UI.h"
#include "globals_UIProperties.h"
#include "globals_UIUtilities.h"
#include "globals_Geometry.h"
#include "globals_Render.h"
#include "globals_RenderNEO.h"
#include "globals_Core.h"
#include "globals_Physics.h"
#include "globals_GLI.h"
#include "globals.h"

#include "Utility/mathUtil.h"
#include "Utility/Math/includeMath.h"
#include "Utility/NEAABB.h"
#include "Utility/NEMocDefs.h"

// This is included last to prevent warnings
#include <qmath.h>

#include "gizmo/IGizmo.h"

struct NEUITheme
{
    QString name;
    QFont font;
    QFont fontSmall;
    QFont fontLarge;
    QFont fontBold;
    QFont fontSmallBold;
    QFont fontLargeBold;
    QColor windowColor;
    QColor fgColor;
    QColor bgColor;
    QColor lightColor;
    QColor darkColor;
    QColor highlightColor;
    QColor textColor;
    QColor highlightTextColor;
    QColor inactiveTextColor;
    QColor windowBgMixColor;
};

extern NEUITheme* UITheme;
extern QPalette UIPalette;
extern QPalette UIPaletteAlt;


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


