#pragma once

#include "Windows/PropertyManager/qtpropertybrowser.h"
#include "Windows/PropertyManager/qtvariantproperty.h"

class QObject;
class IObjectControllerPrivate;
class NEObject;
struct NEDynaPropMetaData;
struct NEPropMetaData;

class NEObjectControllerPrivate : public IObjectControllerPrivate
{
	NEObjectController* q_ptr;
	Q_DECLARE_PUBLIC(NEObjectController)
public:
	NEObjectControllerPrivate();

	~NEObjectControllerPrivate();

	//! Adds the static properties of the object
	void addObjectStaticProperties(const QMetaObject* metaObject, QObject* object);
	//! Adds the dynamic properties of the object, called after scene loading
	void addObjectDynamicProperties(NEObject* obj);
	//! Adds a dynamic property, called when dynamic properties are added online
	void addDynamicProperty(const QString& name, const QVariant& val, const NEDynaPropMetaData& dm, const NEPropMetaData& dn);
	void createDynamicProperty(const QString& name, const QVariant& val, const NEDynaPropMetaData& dm, const NEPropMetaData& dn);
	void createDynamicPropertiesGroup();
	void removeDynamicProperty(const QString& name);

	void updateClassProperties(const QMetaObject* metaObject, bool recursive, QObject* obj, NEObject* n);
	void updateDynamicProperties(NEObject* n);

	void slotValueChanged(QtProperty* property, const QVariant& value);
    void valueChanged(QObject* object, QtProperty* property, const QVariant& value);
	int enumToInt(const QMetaEnum& metaEnum, int enumValue) const;
	int intToEnum(const QMetaEnum& metaEnum, int intValue) const;
	int flagToInt(const QMetaEnum& metaEnum, int flagValue) const;
	int intToFlag(const QMetaEnum& metaEnum, int intValue) const;
	bool isSubValue(int value, int subValue) const;
	bool isPowerOf2(int value) const;
	void applyFilter();
	int applyPropertiesFilter(const QList<QtBrowserItem *> &items);


	void saveExpandedState(QXmlStreamWriter* writer);
	void loadExpandedState(QXmlStreamReader* reader, const QMetaObject* metaObject, bool recursive, QObject* obj);

	//! Pointer to the controlled object
	QObject* m_object;
	//! Convenience pointer if object is also an NEObject
	NEObject* m_neobject;
	//! Map from the class to the created QtProperty
	QMap<const QMetaObject*, QtProperty*> m_classToProperty;
	//! Map from the QtProperty to the class
	QMap<QtProperty*, const QMetaObject*> m_propertyToClass;
	//! Map from property to metaproperty index
	QMap<QtProperty*, int>     m_propertyToIndex;
	//! Map from class to index to property
	QMap<const QMetaObject*, QMap<int, QtVariantProperty*> > m_classToIndexToProperty;
	//! Map of expanded properties
	QMap<QtBrowserItem*, QtBrowserItem*> m_expandedProperties;
	//! List of top-level properties, used for deinitialization
	QList<QtProperty*>         m_topLevelProperties = QList<QtProperty*>();
	//! Maps property indices to group properties
	QMap<int, QtProperty*> m_IndexToGroupProperty;
	//! A list of all property names, used to avoid adding duplicate properties
	QStringList m_PropertyNames;
	//! Group for the dynamic properties
	QtProperty* m_DynProps;
	//! Map from dynamic property names to property objects
	QMap<QString, QtVariantProperty*> m_DynPropNameToProperty;

	QtAbstractPropertyBrowser*    m_browser = 0;
	QtVariantPropertyManager* m_manager = 0;
	QtVariantPropertyManager* m_readOnlyManager = 0;
	QtVariantEditorFactory* m_factory = 0;
	QString m_filterPattern;


	void updateAndExpandClassProperties(const QMetaObject* metaObject, bool recursive, QObject* obj, NEObject*n, QMap<QString, bool>& propExpMap);
};
