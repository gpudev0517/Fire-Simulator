#pragma once


#include "NENode.h"
/*!
 * \brief The NEUserEventType enum
 * Enumeration for different Neutrino user event types
 */

enum NEUserEventType
{
    kUserEventProperyChange,
    kUserEventObjectCreation,
    kUserEventObjectDeletion,
    kUserEventPropertyConnection,
    kUserEventPropertyDisconnection,
    kUserEventNodeConnection,
    kUserEventNodeDisconnection,
    kUserEventPropertyCallbackAdd,
    kUserEventPropertyCallbackRemove,
    kUserEventPropertyUpdateExprAdd,
    kUserEventPropertyUpdateExprRemove,
    kUserEventKeyframeAdd,
    kUserEventKeyframeRemove,
    kUserEventGrouping,
    kUserEventUngrouping
};


struct NECEXP_CORE NEUserEvent
{
public:
    NEUserEvent(NEUserEventType e);
    virtual ~NEUserEvent() {}
    NEUserEventType type = kUserEventProperyChange;


    virtual NEUserEvent*  undo(NEUserEvent* event) = 0;
    virtual NEUserEvent*  redo(NEUserEvent* event) = 0;

};

struct NECEXP_CORE NEUserEventProperyChange : public NEUserEvent
{
    NEUserEventProperyChange() : NEUserEvent(kUserEventProperyChange)
    {

    }

    NEUserEvent* undo(NEUserEvent* event);
    NEUserEvent* redo(NEUserEvent* event);

    QVariant data;
    QObject* obj;
    QMetaProperty metaProp;
};

struct NEUserEventCreation  : public NEUserEvent
{
    NEUserEventCreation() : NEUserEvent(kUserEventObjectCreation)
    {

    }
    NEUserEvent* undo(NEUserEvent* event);
    NEUserEvent* redo(NEUserEvent* event);

    QString nodeName;
    QString nodeType;
    NEObject* parent = 0;

    QMap<uint, NENode::NEConn> m_Conns;                         /**< Connections to other NENodes. */
    QMap<uint, NENode::NEConn> m_SrcConns;                      /**< Connections from other NENodes. */
    QMap<QString, QMap<NENode*, NENode::NEPropConn>> m_PropConns;  /**< Property connections to other nodes. */
    QMap<QString, NENode::NESrcPropConn> m_SrcPropConns;            /**< Property connections from other nodes. */
    QMap<uint, double> m_Keyframes;                     /**< Keeps keyframes and their corresponding elapsed times. */
    QMap<QString, QMap<uint, QVariant>> m_KeyedData;    /**< Keeps a map of the property names to the map of; frames to the values. */
    QMap<QString, QString> m_PropUpdateExprs;           /**< This keeps a map of per-property python expressions (called per update). */
    QMap<int, QString> m_PropCbExprs;       /**< This keeps a map of per-property python expression (called when a property changes). */
};

struct NEUserEventDeletion  : public NEUserEvent
{
    NEUserEventDeletion() : NEUserEvent(kUserEventObjectDeletion)
    {

    }

    NEUserEvent* undo(NEUserEvent* event);
    NEUserEvent* redo(NEUserEvent* event);

    QString nodeName;
    QString nodeType;
    //NEObject* parent = 0;
    QString parentName;

    QMap<uint, NENode::NEConn> m_Conns;                         /**< Connections to other NENodes. */
    QMap<uint, NENode::NEConn> m_SrcConns;                      /**< Connections from other NENodes. */
    QMap<QString, QMap<NENode*, NENode::NEPropConn>> m_PropConns;  /**< Property connections to other nodes. */
    QMap<QString, NENode::NESrcPropConn> m_SrcPropConns;            /**< Property connections from other nodes. */
    QMap<uint, double> m_Keyframes;                     /**< Keeps keyframes and their corresponding elapsed times. */
    QMap<QString, QMap<uint, QVariant>> m_KeyedData;    /**< Keeps a map of the property names to the map of; frames to the values. */
    QMap<QString, QString> m_PropUpdateExprs;           /**< This keeps a map of per-property python expressions (called per update). */
    QMap<int, QString> m_PropCbExprs;         /**< This keeps a map of per-property python expression (called when a property changes). */
    QMap<QString, QVariant> m_MetaProperties; /**< Meta Property value list. */
};


struct NEUserEventPropertyConnection : public NEUserEvent
{
    NEUserEventPropertyConnection();
    NEUserEvent* undo(NEUserEvent* event);
    NEUserEvent* redo(NEUserEvent* event);

    NENode* node = 0;
    NENode::NEPropConn conn;
};



struct NEUserEventPropertyDisconnection : public NEUserEvent
{
    NEUserEventPropertyDisconnection();
    NEUserEvent* undo(NEUserEvent* event);
    NEUserEvent* redo(NEUserEvent* event);

    NENode* node = 0;
    NENode::NEPropConn conn;
};


struct NEUserEventNodeConnection : public NEUserEvent
{
    NEUserEventNodeConnection();
    NEUserEvent* undo(NEUserEvent* event);
    NEUserEvent* redo(NEUserEvent* event);

    NENode* node = 0;
    NENode::NEConn conn;
};

struct NEUserEventNodeDisconnection : public NEUserEvent
{
    NEUserEventNodeDisconnection();
    NEUserEvent* undo(NEUserEvent* event);
    NEUserEvent* redo(NEUserEvent* event);

    NENode* node = 0;
    NENode::NEConn conn;
};

struct NEUserEventPropCallbackAdd : public NEUserEvent
{
    NEUserEventPropCallbackAdd();
    NEUserEvent* undo(NEUserEvent* event);
    NEUserEvent* redo(NEUserEvent* event);

    NENode* obj = 0;
    QString propName;
    QString cb;
};

struct NEUserEventPropCallbackRemove : public NEUserEvent
{
    NEUserEventPropCallbackRemove();
    NEUserEvent* undo(NEUserEvent* event);
    NEUserEvent* redo(NEUserEvent* event);

    NENode* obj = 0;
    QString propName;
    QString cb;
};



struct NEUserEventPropUpdateExpressionAdd : public NEUserEvent
{
    NEUserEventPropUpdateExpressionAdd();
    NEUserEvent* undo(NEUserEvent* event);
    NEUserEvent* redo(NEUserEvent* event);

    NENode* obj = 0;
    QString propName;
    QString ux;
};


struct NEUserEventPropUpdateExpressionRemove : public NEUserEvent
{
    NEUserEventPropUpdateExpressionRemove();
    NEUserEvent* undo(NEUserEvent* event);
    NEUserEvent* redo(NEUserEvent* event);

    NENode* obj = 0;
    QString propName;
    QString ux;
};


struct NEUserEventKeyframeAdd : public NEUserEvent
{
    NEUserEventKeyframeAdd();
    NEUserEvent* undo(NEUserEvent* event);
    NEUserEvent* redo(NEUserEvent* event);

    NENode* obj = 0;
    QString propName;
    QVariant data;
    uint frame;
    NEConstants::NECurveType curveType;
    NECurve* curve = 0;
};


struct NEUserEventKeyframeRemove : public NEUserEvent
{
    NEUserEventKeyframeRemove();
    NEUserEvent* undo(NEUserEvent* event);
    NEUserEvent* redo(NEUserEvent* event);

    NENode* obj = 0;
    QString propName;
    QVariant data;
    uint frame;
    NEConstants::NECurveType curveType;
    NECurve* curve = 0;
};

struct NEUserEventGroup : public NEUserEvent
{
    NEUserEventGroup();
    NEUserEvent* undo(NEUserEvent* event);
    NEUserEvent* redo(NEUserEvent* event);

    QList<NENode*> objs;
    NENode *groupNode;
};

struct NEUserEventUngroup : public NEUserEvent
{
    NEUserEventUngroup();
    NEUserEvent* undo(NEUserEvent* event);
    NEUserEvent* redo(NEUserEvent* event);

    QList<NENode*> objs;
    NENode *groupNode;

};



