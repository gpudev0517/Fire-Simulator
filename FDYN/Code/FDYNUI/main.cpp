#include <QGuiApplication>
#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlEngine>
#include <QQmlContext>
#include <QtQml>
#include <QFile>

#include <QMainWindow>
#include <QDockWidget>
#include <QQuickWidget>
#include <QQuickView>
#include <QQuickItem>
#include <QObject>

#include "TreeModel.h"
#include "customtype.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QApplication app(argc, argv);

    QMainWindow mainWindow;
    qmlRegisterType<CustomType>("FProperty", 1, 0, "CustomType");

    // add menu
    QQuickWidget *qWidget = new QQuickWidget(QUrl("qrc:/FOpenGL.qml"), &mainWindow);
    qWidget->setMinimumWidth(600);
    qWidget->setMinimumHeight(300);

    mainWindow.setCentralWidget(qWidget);
    mainWindow.setStyleSheet("background-color: #0e0c0f");

    auto dockMenu = new QDockWidget("", &mainWindow);
    dockMenu->setFloating(false);
    dockMenu->setAllowedAreas(Qt::TopDockWidgetArea);

    auto view = new QQuickWidget(dockMenu);
    view->setResizeMode(QQuickWidget::SizeRootObjectToView);
    view->setMinimumHeight(80);
    view->setSource(QUrl("qrc:/MenuZone.qml"));

    dockMenu->setWidget(view);
    dockMenu->setTitleBarWidget(new QWidget());
    dockMenu->setFeatures(QDockWidget::NoDockWidgetFeatures);
    mainWindow.addDockWidget(Qt::TopDockWidgetArea, dockMenu);

    /*
    auto dockZone = new QDockWidget("ZONE", &mainWindow);

    view = new QQuickWidget(dockZone);
    view->setResizeMode(QQuickWidget::SizeRootObjectToView);
    view->setSource(QUrl("qrc:/ZoneManager.qml"));
    dockZone->setWidget(view);

    mainWindow.addDockWidget(Qt::LeftDockWidgetArea, dockZone);
    */

    TreeModel model;
    // add from json
    model.loadDataFromJson(":/Data.json");

    // add dyanmically
    TreeNode *rootNode = model.getRootNode();
    QJsonObject obj1
    {
        {"name", "Jack Dev"},
        {"status", 0},
        {"icon", "ring.png"},
        {"buo" , 0.5},
        {"fuel" , 0.5},
        {"damp" , 0.5},
        {"uncomp" , true},
        {"quality" , 0.5},
        {"solver" , "Navier-Stokes"},
        {"turb" , 0.5}
    };

    QJsonObject obj2
    {
        {"name", "Open GL"},
        {"status", 1},
        {"icon", "ring.png"},
        {"buo" , 0.8},
        {"fuel" , 0.8},
        {"damp" , 0.8},
        {"uncomp" , false},
        {"quality" , 0.8},
        {"solver" , "Navier-Stokes"},
        {"turb" , 0.8}
    };

    QJsonArray objArray;
    objArray.append(obj1);
    objArray.append(obj2);
    model.addArrayToTree(rootNode, objArray);

    auto dockScenario = new QDockWidget("Scenario", &mainWindow);

    view = new QQuickWidget(dockScenario);
    view->setResizeMode(QQuickWidget::SizeRootObjectToView);
    view->rootContext()->setContextProperty("tModel", &model);
    //view->setSource(QUrl("qrc:/ScenarioManager.qml"));
    view->setSource(QUrl("qrc:/ScenaZone.qml"));
    dockScenario->setWidget(view);
//    dockScenario->setTitleBarWidget(new QWidget());

    mainWindow.addDockWidget(Qt::LeftDockWidgetArea, dockScenario);
//    mainWindow.tabifyDockWidget(dockZone, dockScenario);
    QObject *treeview = view->rootObject()->findChild<QObject*>("treeview");

    auto dockSettings = new QDockWidget("SETTINGS", &mainWindow);

    view = new QQuickWidget(dockSettings);
    view->setResizeMode(QQuickWidget::SizeRootObjectToView);
    view->setSource(QUrl("qrc:/PropertyEditor.qml"));

    dockSettings->setWidget(view);
//    dockSettings->setTitleBarWidget(new QWidget());

    mainWindow.addDockWidget(Qt::LeftDockWidgetArea, dockSettings);

    QObject *propertyObj = view->rootObject()->findChild<QObject*>("property_editor");
    auto dockWidget = new QDockWidget("FRANX", &mainWindow);
    //dockWidget->setTitleBarWidget();

    view = new QQuickWidget(dockWidget);
    view->setResizeMode(QQuickWidget::SizeRootObjectToView);
    view->setSource(QUrl("qrc:/FranxZone.qml"));

    dockWidget->setWidget(view);
//    dockWidget->setTitleBarWidget(new QWidget());

    mainWindow.addDockWidget(Qt::RightDockWidgetArea, dockWidget);

    auto dockMsg = new QDockWidget("MESSAGE", &mainWindow);

    view = new QQuickWidget(dockMsg);
    view->setResizeMode(QQuickWidget::SizeRootObjectToView);
    view->setSource(QUrl("qrc:/MessageZone.qml"));

    dockMsg->setWidget(view);
//    dockMsg->setTitleBarWidget(new QWidget());

    mainWindow.addDockWidget(Qt::RightDockWidgetArea, dockMsg);

    mainWindow.setDockOptions(QMainWindow::AllowTabbedDocks | QMainWindow::AllowNestedDocks);
    mainWindow.setDockNestingEnabled(true);
    mainWindow.showMaximized();

    /*
    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("tModel", &model);
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;
    */

//    QObject::connect(treeview, SIGNAL(sigTreeItemSelected(QVariant)), &model, SLOT(slotItemSelected(QVariant)));
    QObject::connect(treeview, SIGNAL(sigTreeItemSelected(QVariant)), propertyObj, SIGNAL(sigPropertyItemChanged(QVariant)));
    return app.exec();
}
