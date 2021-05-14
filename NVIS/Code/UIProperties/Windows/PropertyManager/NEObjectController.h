#pragma once

class NEObjectControllerPrivate;
class QtTreePropertyBrowser;
class QtBrowserItem;
class NEObject;

#include "NENode.h"
#include "Utility/NEUserEvent.h"


class NECEXP_NEUIPROPERTIES NEObjectController : public QWidget
{
    Q_OBJECT
public:
    NEObjectController(QWidget *parent = 0);
    virtual ~NEObjectController();

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

    QTreeWidget* treeWidget();
    void applyFilter(const QString& s);

signals:

    void showNodePropertyExpression(const NENode* val, const QString P);
    void pushUserEvent( NEUserEvent *event );
    void updateGLWindows();

public slots:
    void itemCollapsed(QtBrowserItem* item);
    void itemExpanded(QtBrowserItem* item);


private:
    //! Pointer to the currently shown object
    NEObjectControllerPrivate *d_ptr;
    //! Layout where the property browser is shown inside
    QVBoxLayout *layout;
    //! Map of all cached objects
    static QMap<QObject*, NEObjectControllerPrivate*> editMap;

    //! Map to keep the expanded nodes the first time when as scene is loaded
    //! Maps object name to the map of property name
    static QMap<QString, QMap<QString, bool>> expandedMap;

    Q_DECLARE_PRIVATE(NEObjectController)
    Q_DISABLE_COPY(NEObjectController)
    Q_PRIVATE_SLOT(d_func(), void slotValueChanged(QtProperty *, const QVariant &))
};





