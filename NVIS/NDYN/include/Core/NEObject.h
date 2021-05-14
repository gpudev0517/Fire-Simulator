#pragma once

#include "Utility/NEMocDefs.h"

/*! \brief NEObject is the most basic object class providing common functionality for every node in Neutrino.
 *
    All instances that store properties and that should be saved and loaded should be inherited from NEObject.
    The class provides subsystems for:
    \list
        \li On-Screen-Display arrangement for the nodes.
        \li Property grouping/subgrouping/tweaking helper functions.
        \li Manages GUI visibility of the nodes.
        \li Static node behavior.
    \todo Eliminate GUI related stuff from here.
*/

NE_FWD_DECL_3(class, QtVariantProperty, NE_PRIVATE(NEObject), QtPropertyBase)
struct NEOsdMsg;

/*! \brief stores dynamic property specific metadata
*/
struct NEDynaPropMetaData
{
    int type;
    QString visName;
    QString path;
    QString helpText;
};

/*! \brief Stores property specific metadata
*/
struct NEPropMetaData
{
//    bool frozen;
//    bool muted;
    bool locked;
    bool hidden;
};

class IObjectControllerPrivate
{
public:
    virtual void addDynamicProperty(const QString& name, const QVariant& val, const NEDynaPropMetaData& dm, const NEPropMetaData& dn) = 0;
    virtual void removeDynamicProperty(const QString& name) = 0;
};


class NECEXP_CORE NEObject : public QObject
{
    Q_OBJECT
    NE_CLASS("Node Information")
    NE_DECL_PRIVATE(NEObject)

public:
    enum NEOsdMsgPos { Top, TopRight, Right, BottomRight, Bottom, BottomLeft, Left,	TopLeft	};

    NE_PROP_RW(NEOsdMsgPos, OsdMsgPos, "OSD Location", "Location of the displayed statistics on the OpenGL display.", true,)
    NE_PROP_RW(bool, Verbose, "Verbose", "Display additional statistics", true,)

    Q_ENUMS(NEOsdMsgPos)

 public:
    //! In the constructor, initilize the object so that it is visible in the scene
    NEObject();
    virtual ~NEObject();
    //! Object initialization function, should be called from all inheriting classes.
    virtual uint init(uint initMode = NE::kInitOnCreate);
    //! Handles various object events
    bool event(QEvent* e);
    //! Returns a pointer to the list of actions stored with this instance.
    const QList<QObject*>* actions();
    //! Returns/Sets whether the object should be hidden from the UI
    bool isHidden() const;
    //! Sets whether the object should be hidden from the UI
    void setHidden(bool val);
    //! Returns whether the object is static (non-deletable)
    bool isStatic() const;
    //! Sets whether the object is static (non-deletable)
    void setStatic(bool val);
    //! The start rank of the object OSD
    uint osdStartRank() const;
    //! Sets the start rank of the object OSD
    void setOsdStartRank(uint val);
    //! Returns whether the node is in a verbose state
    void setParent(QObject* val);

    const QMap<QString, NEDynaPropMetaData>& DynaProMD();     /**< Returns Map of metadata for dynamic properties. */
    const QMap<QString, NEPropMetaData>& ProMD();             /**< Returns Map of metadata for all property types */

    //! Set the object controller
    void setObjectController( IObjectControllerPrivate* objController );
    //! This can be used to tweak property appearances and behaviors, when overridden, all parent implementations should also be called in bottom to top order
    virtual void propertyTweak(QtPropertyBase* p);

public slots:
    //! This is for adding dynamic properties to the objects of NENode and derivatives
    uint addProperty(QString name, const QVariant& value, QString visibleName = QString(), QString path = QString(), QString help = QString());
    //! This is for removing dynamic properties from the objects of NENode and derivatives
    uint removeProperty(QString name);
    //! This is used to lock the passed property, such that editing through the ui wont be possible
    uint lockProperty(QString name, bool val);
    //! This is used to hide properties from the UI
    uint hideProperty(QString name, bool val);
    //! Returns whether the property is hidden from the ui
    bool propertyHidden(QString name);
    //! Returns whether the property is locked (i.e. not modifyable throught the ui)
    bool propertyLocked(QString name);
    //! Resets all properties to their default values
    virtual void resetProperties();

signals:
    void registerAction(QString actionName, QObject* &action);
    void addOSD(uint rank, const NEOsdMsg& msg);
    void addOSD(uint rank, const NEOsdMsg& msg, const NEObject* object);
    void removeOSD(uint rank, NEOsdMsgPos position);
    void removeOSD(uint rank, const NEObject* object);
    void logToFileOSD(QString scenePath, const double elapsedTime);
    void moveBufferOSD( const NEObject::NEOsdMsgPos oldPos, const NEObject::NEOsdMsgPos newPos, const NEObject* object );

protected:
    //! Adds an empty action and returns a pointer to it for editing
    //QObject* addAction(QString actionName, QString shortCut = "", QObject* parent = NULL);
    //! Adds the passed action to the object
    void addAction(QObject* action);
    //! Called when a node is added as the child of this node
    virtual void childAdded(QObject* child);
    //! Called when a node is removed from a parent
    virtual void childRemoved(QObject* child);
    //! Initializes right click actions for the object
    virtual void initializeGUIActions();

    //! Dummy read functions for grouping properties
    static const PropertyGroupStart groupStart();
    static const PropertyGroupEnd groupEnd();
    //! Dummy read functions for sub-grouping properties
    static const SubPropertyStart subStart();
    static const SubPropertyEnd subEnd();

    NE_FRIENDS_3(class, NEObjectController, NE_PRIVATE(NEObjectController), NE_PRIVATE(NEBase))
};

Q_DECLARE_INTERFACE(NEObject, "NEObject")
