#pragma once
#include <QQuickWidget>
#include <QMutexLocker>
#include <QMutex>
#include "Utilities/FDYNPropertyWidget.h"

class FDYNObjectControllerPrivate;
class FDYNQtTreePropertyBrowser;
class QtBrowserItem;
class NEObject;

class FDYNObjectController : public QWidget
{
    Q_OBJECT
public:
    FDYNObjectController(QWidget *parent = 0);
    virtual ~FDYNObjectController();

    //! Sets the passed object to the controller
    void setObject(QObject *object);
    //! Revoves the objects data from the controller
    void removeObject(QObject* object);
    //! Updates the properties of the pointed object
    void updateProperties();
    //! Clears the map of all cached objects
    static void clearPropertiesMap();
    //! Saves the UI states of the object controllers
    static void saveUIStates(QXmlStreamWriter *writer);
    //! Loads the UI states of the object controllers
    static void loadUIStates(QXmlStreamReader *reader);
    //! Pointer to the controlled object
    QObject *object() const;

#ifdef OLD_PROPERTY
    QTreeWidget* treeWidget();
#else
    FDYNPropertyWidget* treeWidget();
#endif
    void applyFilter(const QString& s);

public slots:
    void itemCollapsed(QtBrowserItem* item);
    void itemExpanded(QtBrowserItem* item);
    void propertyChanged(QString propertyName);

private:
    //! Pointer to the currently shown object
    FDYNObjectControllerPrivate *d_ptr;
    //! Layout where the property browser is shown inside
    QVBoxLayout *layout;
    //! Map of all cached objects
    static QMap<QObject*, FDYNObjectControllerPrivate*> editMap;

    //! Map to keep the expanded nodes the first time when as scene is loaded
    //! Maps object name to the map of property name
    static QMap<QString, QMap<QString, bool>> expandedMap;

    static QMutex m_mutex;

    Q_DECLARE_PRIVATE(FDYNObjectController)
    Q_DISABLE_COPY(FDYNObjectController)
    Q_PRIVATE_SLOT(d_func(), void slotValueChanged(QtProperty *, const QVariant &))
};





