#pragma once
#include <QQuickWidget>

class FDYNGUIManager;

class FDYNCADModelingTool : public QQuickWidget
{
    Q_OBJECT

public:
    friend class FDYNGUIManager;
    friend class FDYNGUIManagerPrivate;
    FDYNCADModelingTool(QWidget* parent = 0);
    ~FDYNCADModelingTool();

public slots:

private:
    void init();

};

class CursorPosProvider : public QObject
{
    Q_OBJECT
public:
    explicit CursorPosProvider(QObject *parent = nullptr);

    virtual ~CursorPosProvider() = default;

    Q_INVOKABLE QPointF cursorPos();

};
