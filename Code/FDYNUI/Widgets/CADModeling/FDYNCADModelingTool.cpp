#include <QQuickView>
#include <QtQuickControls2>

#include "FDYNCADModelingTool.h"
//#include "FDYNTimelineWidget.h"
#include "Managers/FDYNGUIManager.h"
#include "Base/NESceneManager.h"
#include "Spatial/NESpatial.h"

CursorPosProvider mousePosProvider;

FDYNCADModelingTool::FDYNCADModelingTool( QWidget* parent /*= 0*/ ) : QQuickWidget(parent)
{

    rootContext()->setContextProperty("mousePosition", &mousePosProvider );

    setSource(QUrl("qrc:/FCADModelingTool.qml"));
    setResizeMode(QQuickWidget::SizeRootObjectToView);

}

FDYNCADModelingTool::~FDYNCADModelingTool()
{

}

void FDYNCADModelingTool::init()
{
}


CursorPosProvider::CursorPosProvider(QObject *parent):  QObject(parent)
{
}

QPointF CursorPosProvider::cursorPos()
{
    return QCursor::pos();
}
