#include "NEObjectController.h"
#include "qtvariantproperty.h"
#include "NEVariantFactory.h"
#include "NEVariantManager.h"


#include "qtgroupboxpropertybrowser.h"
#include "qttreepropertybrowser.h"
#include "qtpropertybrowser.h"

#include "Base/NEBase.h"
#include "Utility/NEUserEvent.h"

//#include "Managers/NEGUIManager.h"
//#include "Managers/NEGUIManager_p.h"

#include "Base/NESceneManager.h"

#include "PropertyManager/NEPropertySettings.h"
#include "Utility/FilePath.h"
#include "NENode.h"
#include "Base/NENodeFactory.h"
#include "NEObject_p.h"
#include "NEObjectController_p.h"

QMap<QObject*, NEObjectControllerPrivate*> NEObjectController::editMap;
QMap<QString, QMap<QString, bool>> NEObjectController::expandedMap;

int NEObjectControllerPrivate::enumToInt(const QMetaEnum& metaEnum, int enumValue) const
{
	QMap<int, int> valueMap; // dont show multiple enum values which have the same values
	int pos = 0;
	for(int i = 0; i < metaEnum.keyCount(); i++)
	{
		int value = metaEnum.value(i);
		if(!valueMap.contains(value))
		{
			if(value == enumValue)
				return pos;
			valueMap[value] = pos++;
		}
	}
	return -1;
}

int NEObjectControllerPrivate::intToEnum(const QMetaEnum& metaEnum, int intValue) const
{
	QMap<int, bool> valueMap; // dont show multiple enum values which have the same values
	QList<int> values;
	for(int i = 0; i < metaEnum.keyCount(); i++)
	{
		int value = metaEnum.value(i);
		if(!valueMap.contains(value))
		{
			valueMap[value] = true;
			values.append(value);
		}
	}
	if(intValue >= values.count())
		return -1;
	return values.at(intValue);
}

bool NEObjectControllerPrivate::isSubValue(int value, int subValue) const
{
	if(value == subValue)
		return true;
	int i = 0;
	while(subValue)
	{
		if(!(value & (1 << i)))
		{
			if(subValue & 1)
				return false;
		}
		i++;
		subValue = subValue >> 1;
	}
	return true;
}

bool NEObjectControllerPrivate::isPowerOf2(int value) const
{
	while(value)
	{
		if(value & 1)
		{
			return value == 1;
		}
		value = value >> 1;
	}
	return false;
}

void NEObjectControllerPrivate::applyFilter()
{
	const QList<QtBrowserItem*> items = m_browser->topLevelItems();

	QListIterator<QtBrowserItem*> itTopLevel(items);
	while(itTopLevel.hasNext())
	{
		QtBrowserItem* item = itTopLevel.next();
		((QtTreePropertyBrowser*)m_browser)->setItemVisible(item, applyPropertiesFilter(item->children()));
	}
}

int NEObjectControllerPrivate::applyPropertiesFilter(const QList<QtBrowserItem*>& items)
{
	int showCount = 0;
	const bool matchAll = m_filterPattern.isEmpty();
	QListIterator<QtBrowserItem*> itProperty(items);
	while(itProperty.hasNext())
	{
		QtBrowserItem* propertyItem = itProperty.next();
		QtProperty* property = propertyItem->property();
		int pi = m_propertyToIndex[property];
		QMetaProperty mp = m_object->metaObject()->property(pi);
		if(!mp.isDesignable())
			continue;

		const QString propertyName = property->propertyName();
		const bool showProperty = matchAll || propertyName.contains(m_filterPattern, Qt::CaseInsensitive);
        ((QtTreePropertyBrowser*)m_browser)->setItemVisible(propertyItem, showProperty);
		if(showProperty)
			showCount++;
	}
	return showCount;
}

void NEObjectControllerPrivate::saveExpandedState(QXmlStreamWriter* writer)
{
	foreach(QtBrowserItem* item, m_expandedProperties)
	{
		writer->writeStartElement(item->property()->propertyName());
		writer->writeEndElement();
	}
}

void NEObjectControllerPrivate::loadExpandedState(QXmlStreamReader* reader, const QMetaObject* metaObject, bool recursive, QObject* obj)
{

}



int NEObjectControllerPrivate::flagToInt(const QMetaEnum& metaEnum, int flagValue) const
{
	if(!flagValue)
		return 0;
	int intValue = 0;
	QMap<int, int> valueMap; // dont show multiple enum values which have the same values
	int pos = 0;
	for(int i = 0; i < metaEnum.keyCount(); i++)
	{
		int value = metaEnum.value(i);
		if(!valueMap.contains(value) && isPowerOf2(value))
		{
			if(isSubValue(flagValue, value))
				intValue |= (1 << pos);
			valueMap[value] = pos++;
		}
	}
	return intValue;
}

int NEObjectControllerPrivate::intToFlag(const QMetaEnum& metaEnum, int intValue) const
{
	QMap<int, bool> valueMap; // dont show multiple enum values which have the same values
	QList<int> values;
	for(int i = 0; i < metaEnum.keyCount(); i++)
	{
		int value = metaEnum.value(i);
		if(!valueMap.contains(value) && isPowerOf2(value))
		{
			valueMap[value] = true;
			values.append(value);
		}
	}
	int flagValue = 0;
	int temp = intValue;
	int i = 0;
	while(temp)
	{
		if(i >= values.count())
			return -1;
		if(temp & 1)
			flagValue |= values.at(i);
		i++;
		temp = temp >> 1;
	}
	return flagValue;
}

void NEObjectControllerPrivate::updateClassProperties(const QMetaObject* metaObject, bool recursive, QObject* obj, NEObject* n)
{
	if(!metaObject)
		return;

	if(recursive)
    {
        if( !metaObject->superClass() )
            return;
		updateClassProperties(metaObject->superClass(), recursive, obj, n);
    }

	QtProperty* classProperty = m_classToProperty.value(metaObject);
	if(!classProperty)
		return;

	for(int idx = metaObject->propertyOffset(); idx < metaObject->propertyCount(); idx++)
	{
		const QMetaProperty& metaProperty = metaObject->property(idx);
		uint userType = metaProperty.userType();

		if(metaProperty.isReadable())
		{
			if(m_classToIndexToProperty.contains(metaObject) && m_classToIndexToProperty[metaObject].contains(idx))
			{
				QtVariantProperty* subProperty = m_classToIndexToProperty[metaObject][idx];
				if(n)
					subProperty->setEnabled(!n->P->m_ProMD.value(subProperty->propertyName()).locked);

				subProperty->oldProp = subProperty->value();

				if(metaProperty.isEnumType())
				{
					if(metaProperty.isFlagType())
						subProperty->setValue(flagToInt(metaProperty.enumerator(), metaProperty.read(m_object).toInt()));
					else
						subProperty->setValue(enumToInt(metaProperty.enumerator(), metaProperty.read(m_object).toInt()));
				}
				else
				{
                    if(userType == getNEVarIds().filePathId)
						subProperty->setValue(metaProperty.read(m_object).value<FilePath>().path());
                    else if(userType == getNEVarIds().colorRampId)
                        subProperty->setValue(metaProperty.read(m_object).value<NEColorRamp>());
                    else
						subProperty->setValue(metaProperty.read(m_object));
				}
				// Hide non-designable and user hidden properties
				if(metaProperty.isDesignable(obj))
				{
					QList<QtBrowserItem*> items = m_browser->items(subProperty);
					((QtTreePropertyBrowser*)m_browser)->setItemVisible(items.last(), true && (n) ? (!n->P->m_ProMD.value(subProperty->propertyName()).hidden) : true);
				}
				else
				{
					QList<QtBrowserItem*> items = m_browser->items(subProperty);
					((QtTreePropertyBrowser*)m_browser)->setItemVisible(items.last(), false && (n) ? (n->P->m_ProMD.value(subProperty->propertyName()).hidden) : false);
				}
			}
            else if(metaProperty.userType() == getNEVarIds().propertyGroupStartId)
			{
				if(metaProperty.isDesignable(obj))
				{
					QtProperty* prop = m_IndexToGroupProperty[metaProperty.propertyIndex()];
					QList<QtBrowserItem*> items = m_browser->items(prop);
					((QtTreePropertyBrowser*)m_browser)->setItemVisible(items.last(), true);
				}
				else
				{
					QtProperty* prop = m_IndexToGroupProperty[metaProperty.propertyIndex()];
					QList<QtBrowserItem*> items = m_browser->items(prop);
					((QtTreePropertyBrowser*)m_browser)->setItemVisible(items.last(), false);
				}
			}
		}
	}
}

void NEObjectControllerPrivate::updateDynamicProperties(NEObject* n)
{
	for(auto i = n->P->m_DynaProMD.begin(); i != n->P->m_DynaProMD.end(); i++)
	{
		auto v = m_DynPropNameToProperty.find(i.key());

		if(v != m_DynPropNameToProperty.end())
		{
			QtVariantProperty* subProperty = *v;
			subProperty->setEnabled(!n->P->m_ProMD.value(subProperty->propertyName()).locked);
			subProperty->oldProp = subProperty->value();
			subProperty->setValue(n->property(i.key().toUtf8()));
		}
	}
}

void NEObjectControllerPrivate::updateAndExpandClassProperties(const QMetaObject* metaObject, bool recursive, QObject* obj, NEObject* n, QMap<QString, bool>& propExpMap)
{
	if(!metaObject)
		return;

	if(recursive)
		updateAndExpandClassProperties(metaObject->superClass(), recursive, obj, n, propExpMap);

	QtProperty* classProperty = m_classToProperty.value(metaObject);
	if(!classProperty)
		return;

	for(int idx = metaObject->propertyOffset(); idx < metaObject->propertyCount(); idx++)
	{
		const QMetaProperty& metaProperty = metaObject->property(idx);
		uint userType = metaProperty.userType();

		if(metaProperty.isReadable())
		{
			if(m_classToIndexToProperty.contains(metaObject) && m_classToIndexToProperty[metaObject].contains(idx))
			{
				QtVariantProperty* subProperty = m_classToIndexToProperty[metaObject][idx];
				if(n)
					subProperty->setEnabled(!n->P->m_ProMD.value(subProperty->propertyName()).locked);

				subProperty->oldProp = subProperty->value();

				if(metaProperty.isEnumType())
				{
					if(metaProperty.isFlagType())
						subProperty->setValue(flagToInt(metaProperty.enumerator(), metaProperty.read(m_object).toInt()));
					else
						subProperty->setValue(enumToInt(metaProperty.enumerator(), metaProperty.read(m_object).toInt()));
				}
				else
				{
                    if(userType == getNEVarIds().filePathId)
						subProperty->setValue(metaProperty.read(m_object).value<FilePath>().path());
                    else if(userType == getNEVarIds().colorRampId)
                        subProperty->setValue(metaProperty.read(m_object).value<NEColorRamp>());
                    else
						subProperty->setValue(metaProperty.read(m_object));
				}
				// Hide non-designable and user hidden properties
				if(metaProperty.isDesignable(obj))
				{
					QList<QtBrowserItem*> items = m_browser->items(subProperty);
					((QtTreePropertyBrowser*)m_browser)->setItemVisible(items.last(), true && (n) ? (!n->P->m_ProMD.value(subProperty->propertyName()).hidden) : true);
				}
				else
				{
					QList<QtBrowserItem*> items = m_browser->items(subProperty);
                    ((QtTreePropertyBrowser*)m_browser)->setItemVisible(items.last(), false && (n) ? (n->P->m_ProMD.value(subProperty->propertyName()).hidden) : false);
				}

				// Check if property has been expanded
				QMap<QString, bool>::iterator i = propExpMap.find(metaProperty.name());
				if(i != propExpMap.end())
				{
					QList<QtBrowserItem*> items = m_browser->items(subProperty);
					QtBrowserItem* item = items.last();
					// Update the expanded map
					m_expandedProperties[item] = item;
					((QtTreePropertyBrowser*)m_browser)->setExpanded(item, true);
				}
			}
            else if(metaProperty.userType() == getNEVarIds().propertyGroupStartId)
			{
				if(metaProperty.isDesignable(obj))
				{
					QtProperty* prop = m_IndexToGroupProperty[metaProperty.propertyIndex()];
					QList<QtBrowserItem*> items = m_browser->items(prop);
					((QtTreePropertyBrowser*)m_browser)->setItemVisible(items.last(), true);
				}
				else
				{
					QtProperty* prop = m_IndexToGroupProperty[metaProperty.propertyIndex()];
					QList<QtBrowserItem*> items = m_browser->items(prop);
					((QtTreePropertyBrowser*)m_browser)->setItemVisible(items.last(), false);
				}

				// Check if property has been expanded
				QMap<QString, bool>::iterator i = propExpMap.find(metaProperty.name());
				if(i != propExpMap.end())
				{
					QtProperty* prop = m_IndexToGroupProperty[metaProperty.propertyIndex()];
					QList<QtBrowserItem*> items = m_browser->items(prop);
					QtBrowserItem* item = items.last();
					// Update the expanded map
					m_expandedProperties[item] = item;
					((QtTreePropertyBrowser*)m_browser)->setExpanded(item, true);
				}
			}
		}
	}
}


void NEObjectControllerPrivate::addObjectStaticProperties(const QMetaObject* metaObject, QObject* object)
{
	if(!metaObject)
		return;

	QtProperty* classProperty = m_classToProperty.value(metaObject);

	if(metaObject->propertyOffset() < metaObject->propertyCount())
	{
		if(!classProperty) // If the class property is not found in the map, create the properties
		{
			QString className = QLatin1String(metaObject->className());
			// Hides the qobject from the property editor
			if(className[0] == 'Q')
				return;

			// Read the user friendly class name if available
			if(metaObject->classInfoCount())
			{
				int infoIndex = metaObject->indexOfClassInfo("ClassName");
				if(infoIndex != -1 && infoIndex >= metaObject->classInfoOffset())
				{
					const QMetaClassInfo& nameInfo = metaObject->classInfo(infoIndex);
					className = nameInfo.value();
				}
			}
			// Add the class name as a group property to the property editor
			classProperty = m_manager->addProperty(QtVariantPropertyManager::groupTypeId(), className);
			// Bookkeep the property and the class
			m_classToProperty[metaObject] = classProperty;
			m_propertyToClass[classProperty] = metaObject;
			// This stack is used for grouping properties
			QStack<QtProperty*> propGroupStack;
			propGroupStack.push(classProperty);
			// This stack is used for sub-grouping properties
			QStack<QtProperty*> propStack;
			// Iterate over the current classes properties
			for(int idx = metaObject->propertyOffset(); idx < metaObject->propertyCount(); idx++)
			{
				bool added = true;	// Will the subproperty be added ?

				const QMetaProperty& metaProperty = metaObject->property(idx);
				uint type = metaProperty.userType();
				QtVariantProperty* subProperty = 0;
				// If the property is a group starter type
                if(type == getNEVarIds().propertyGroupStartId)
				{
					// Add the property to the read-only property manager
					QtProperty* propertyGroup = m_readOnlyManager->addProperty(QtVariantPropertyManager::groupTypeId(), metaProperty.name());
					int visibleNameIndex = metaObject->indexOfClassInfo((QString("name-") + metaProperty.name()).toUtf8());
					if(visibleNameIndex != -1)
					{
						const QMetaClassInfo& nameInfo = metaObject->classInfo(visibleNameIndex);
						QString visibleName = nameInfo.value();
						propertyGroup->setVisiblePropertyName(visibleName);
					}
					int infoIndex = metaObject->indexOfClassInfo((QString("help-") + metaProperty.name()).toUtf8());
					if(infoIndex != -1)
					{
						const QMetaClassInfo& helpInfo = metaObject->classInfo(infoIndex);
						QString helpText = QString(helpInfo.value()) + QString(" [property: ") + metaProperty.name() + QString("]");
						propertyGroup->setStatusTip(helpText);
						propertyGroup->setToolTip(helpText);
					}

					propGroupStack.top()->addSubProperty(propertyGroup);
					propGroupStack.push(propertyGroup);

					m_IndexToGroupProperty[idx] = propertyGroup;

					continue;
				}
                else if(type == getNEVarIds().propertyGroupEndId)
				{
					propGroupStack.pop();
					continue;
				}
                else if(type == getNEVarIds().subPropertyStartId)
				{
					propGroupStack.push(propStack.top());
					continue;
				}
                else if(type == getNEVarIds().subPropertyEndId)
				{
					propGroupStack.pop();
					continue;
				}
				// Inserts non-readable property
				if(!metaProperty.isReadable())
				{
					subProperty = m_readOnlyManager->addProperty(QVariant::String, QLatin1String(metaProperty.name()));
					subProperty->setValue(QLatin1String("< Non Readable >"));
				}
//				//Skip adding the property if it was already added
//				else if(m_PropertyNames.indexOf(metaProperty.name()) > -1)
//				{
//					added = false;
//				}
				else if(metaProperty.isEnumType())
				{
					if(metaProperty.isFlagType())
					{
						subProperty = m_manager->addProperty(QtVariantPropertyManager::flagTypeId(), QLatin1String(metaProperty.name()));
						QMetaEnum metaEnum = metaProperty.enumerator();

						QMap<int, bool> valueMap;
						QStringList flagNames;
						for(int i = 0; i < metaEnum.keyCount(); i++)
						{
							int value = metaEnum.value(i);
							if(!valueMap.contains(value) && isPowerOf2(value))
							{
								valueMap[value] = true;
								flagNames.append(QLatin1String(metaEnum.key(i)));
							}
							subProperty->setAttribute(QLatin1String("flagNames"), flagNames);
							subProperty->setValue(flagToInt(metaEnum, metaProperty.read(m_object).toInt()));
						}
					}
					else
					{
						subProperty = m_manager->addProperty(QtVariantPropertyManager::enumTypeId(), QLatin1String(metaProperty.name()));

						QMetaEnum metaEnum = metaProperty.enumerator();
						QMetaObject* scc = const_cast<QMetaObject*>(metaObject);
						QMap<int, bool> valueMap; // dont show multiple enum values which have the same values
						QStringList enumNames;

						while(scc)
						{
							int ei = scc->indexOfEnumerator(metaEnum.name());
							if(ei == -1)
							{
								scc = const_cast<QMetaObject*>(scc->superClass());
								continue;
							}

							QMetaEnum metaEnum2 = scc->enumerator(ei);

							for(int i = 0; i < metaEnum2.keyCount(); i++)
							{
								int value = metaEnum2.value(i);
								if(!valueMap.contains(value))
								{
									valueMap[value] = true;
									enumNames.append(splitFromCapitals(QLatin1String(metaEnum2.key(i))));
								}
							}
							scc = const_cast<QMetaObject*>(scc->superClass());
						}
						subProperty->setAttribute(QLatin1String("enumNames"), enumNames);
						subProperty->setValue(enumToInt(metaEnum, metaProperty.read(m_object).toInt()));
					}
				}
				else if(m_manager->isPropertyTypeSupported(type))
				{
					if(!metaProperty.isWritable())	// If the property is not writable, add it to the read-only manager
						subProperty = m_readOnlyManager->addProperty(type, QLatin1String(metaProperty.name()));
					else							// Otherwise add it to the regular writable property manager
						subProperty = m_manager->addProperty(type, QLatin1String(metaProperty.name()));

                    if(type == getNEVarIds().filePathId) //  Do additional stuff for filepath properties
					{
						FilePath tmpPath = metaProperty.read(m_object).value<FilePath>();
						subProperty->setValue(tmpPath.path());
						subProperty->setAttribute("filter", tmpPath.filter());
						subProperty->setAttribute("type", tmpPath.dialogType());
					}
                    else if(type == getNEVarIds().colorRampId)
                        subProperty->setValue(metaProperty.read(m_object).value<NEColorRamp>());
                    else	// Simply setValue for the remaining properties
					{
						subProperty->setValue(metaProperty.read(m_object));
					}
				}
				else	// Specify unknown property types in the read-only manager only
				{
					subProperty = m_readOnlyManager->addProperty(QVariant::String, QLatin1String(metaProperty.name()));
					subProperty->setValue(QLatin1String("< Unknown Type >"));
					subProperty->setEnabled(false);
				}
				// If the property was added
				if(added)
				{
					// Set the qobject for the property
					subProperty->setQObject(m_object);
					// If the qobject is an NEObject, we can tweak the property
//					NEObject* neObject = qobject_cast<NEObject*>(m_object);
					if(m_neobject)
					{
						m_neobject->propertyTweak(subProperty);
					}
					// Append the property name to the property names list
					m_PropertyNames.append(subProperty->propertyName());

					propStack.push(subProperty);
					propGroupStack.top()->addSubProperty(subProperty);

					// Bookkeeep
					m_propertyToIndex[subProperty] = idx;
					m_classToIndexToProperty[metaObject][idx] = subProperty;

					if(metaObject->classInfoCount())
					{
						int infoIndex = metaObject->indexOfClassInfo((QString("help-") + metaProperty.name()).toUtf8());
						if(infoIndex != -1)
						{
							const QMetaClassInfo& helpInfo = metaObject->classInfo(infoIndex);
							QString helpText = QString(helpInfo.value()) + QString(" [property: ") + metaProperty.name() + QString("]");
							subProperty->setStatusTip(helpText);
							subProperty->setToolTip(helpText);
						}

						int visibleNameIndex = metaObject->indexOfClassInfo((QString("name-") + metaProperty.name()).toUtf8());
						if(visibleNameIndex != -1)
						{
							const QMetaClassInfo& nameInfo = metaObject->classInfo(visibleNameIndex);
							QString visibleName = nameInfo.value();
							subProperty->setVisiblePropertyName(visibleName);
						}
					}
				}
			}
		}

		// A list of top-level properties is kept for de-initialization
		m_topLevelProperties.append(classProperty);
		// Adds all class properties at once to the browser by adding the parent class property
		m_browser->addProperty(classProperty);
	}
	// Recursively add the properties of the superclasses
	addObjectStaticProperties(metaObject->superClass(), object);
}

void NEObjectControllerPrivate::addObjectDynamicProperties(NEObject* obj)
{
	// The object doesn't have any dynamic property, return
	if(obj->P->m_DynaProMD.isEmpty())
		return;

	createDynamicPropertiesGroup();

	// Iterate through the dynamic properties metadata map
	for(auto it = obj->P->m_DynaProMD.begin(); it != obj->P->m_DynaProMD.end(); ++it)
	{
		const NEDynaPropMetaData& dm = it.value();
		const QString& n = it.key();
		const NEPropMetaData& pm = obj->P->m_ProMD.value(n);
		createDynamicProperty(n, obj->property(n.toUtf8()), dm, pm);
	}
}

void NEObjectControllerPrivate::addDynamicProperty(const QString& name, const QVariant& val, const NEDynaPropMetaData& dm, const NEPropMetaData& dn)
{
	createDynamicPropertiesGroup();
	createDynamicProperty(name, val, dm, dn);
}

void NEObjectControllerPrivate::createDynamicProperty(const QString& name, const QVariant& val, const NEDynaPropMetaData& dm, const NEPropMetaData& dn)
{
	QtVariantProperty* p = m_manager->addProperty(dm.type, name);
	if(p)
	{
		if(dm.visName.size()) p->setVisiblePropertyName(dm.visName);
		if(dm.helpText.size()) p->setToolTip(dm.helpText);
		// If the property has a path, construct the path
		if(dm.path.size())
		{
			// Split the path
			QStringList sl = dm.path.split('/');
			// Start pointing to the root groupd
			QtProperty* prop = m_DynProps;

			foreach(const QString& s, sl)
			{
				QtProperty* p = m_manager->addProperty(QtVariantPropertyManager::groupTypeId(), s);
				prop->addSubProperty(p);
				// Update the pointer
				prop = p;
			}
			prop->addSubProperty(p);
		}
		else
			m_DynProps->addSubProperty(p);

		m_DynPropNameToProperty[name] = p;
		p->setValue(val);
		p->setQObject(m_object);
		p->setEnabled(!dn.locked);
	}
}

void NEObjectControllerPrivate::createDynamicPropertiesGroup()
{
	// Create the dynamic properties group if not already created
	if(!m_DynProps)
	{
		m_DynProps = m_manager->addProperty(QtVariantPropertyManager::groupTypeId(), "UserProperties");
		m_DynProps->setVisiblePropertyName("User Properties");
		m_browser->addProperty(m_DynProps);
	}\
}

void NEObjectControllerPrivate::removeDynamicProperty(const QString& name)
{
	auto it = m_DynPropNameToProperty.find(name);
	if(it != m_DynPropNameToProperty.end())
	{
		m_browser->removeProperty(*it); // Deletes the property from the browser
		delete *it;	// Deletes the qt variant property
		m_DynPropNameToProperty.erase(it);	// Erase from bookkeeping
	}
}

void NEObjectControllerPrivate::slotValueChanged(QtProperty* property, const QVariant& value)
{
    if(NESCENE.selectedObjects()->size() != 0)
    {
        if(NENode* node = qobject_cast<NENode*>(m_object))
        {
            QString name = node->factory().nodeName();
            QMap<NEObject*, NEObject*>* pObjects = NESCENE.selectedObjects();
            foreach( NEObject* object, *pObjects)
                if(NENode* nd = qobject_cast<NENode*>(object))
                    if(nd->factory().nodeName() == name)
                        valueChanged(object, property, value);

            return;
        }
    }
    valueChanged(m_object, property, value);
}

void NEObjectControllerPrivate::valueChanged(QObject *object, QtProperty *property, const QVariant &value)
{
    if(m_propertyToIndex.contains(property))	// We have a static property
    {
        int idx = m_propertyToIndex.value(property);

        const QMetaObject* metaObject = object->metaObject();
        const QMetaProperty& metaProperty = metaObject->property(idx);
        uint userType = metaProperty.userType();

        if(UndoRedoEnabled())
        {
            NEUserEventProperyChange* event = new NEUserEventProperyChange();
//			event.type = kUserEventProperyChange;
            event->data = property->oldProp;/*metaProperty.read(m_object)*/;
            event->obj = object;
            event->metaProp = metaProperty;
            emit q_ptr->pushUserEvent((NEUserEvent *)event);
           /// NEGUI.pushUserEvent((NEUserEvent*)event);
        }

        if(metaProperty.isEnumType())
        {
            if(metaProperty.isFlagType())
                metaProperty.write(object, intToFlag(metaProperty.enumerator(), value.toInt()));
            else
                metaProperty.write(object, intToEnum(metaProperty.enumerator(), value.toInt()));
        }
        else if(userType)
        {
            if(userType == getNEVarIds().filePathId)
            {
                metaProperty.write(object, FilePath(value.toString()));
                //Log() << "FilePath"; EndLog(NE::kLogInfo);
            }
            else if(userType == getNEVarIds().colorRampId)
            {
                metaProperty.write(object, value.value<NEColorRamp>());
                //Log() << "ColorRamp"; EndLog(NE::kLogInfo);
            }
            else if(QString(metaProperty.name()) == "objectName")
                ((NENode*)(object))->setObjectName(value.toString());
            else
            {
                metaProperty.write(object, value);
            }
        }

        if(userType && userType == getNEVarIds().neBoolId)
        {

        }
        else
        {
            updateClassProperties(metaObject, true, object, m_neobject);
        }
        emit q_ptr->updateGLWindows();
        //NEGUI.updateSceneInspector();
        //NEGUI.updateSystemInspector();
        //NEGUI.updateGLWindows();
    }
    else if(m_DynPropNameToProperty.size())
    {
        auto it = m_DynPropNameToProperty.find(property->propertyName());
        if(it != m_DynPropNameToProperty.end())
            object->setProperty(property->propertyName().toUtf8(), value);
    }
}

NEObjectControllerPrivate::NEObjectControllerPrivate()
{
	m_DynProps = 0;
	m_object = 0;
	m_neobject = 0;
}

NEObjectControllerPrivate::~NEObjectControllerPrivate()
{
//	QListIterator<QtProperty*> it(m_topLevelProperties);
	if(!m_topLevelProperties.isEmpty())
		for(auto it = m_topLevelProperties.begin(); it != m_topLevelProperties.end(); ++it)
		{
			m_browser->removeProperty(*it);

		}

	delete m_browser;

	delete m_manager;
	delete m_readOnlyManager;
	delete m_factory;
}


///////////////////

NEObjectController::NEObjectController(QWidget *parent)
	: QWidget(parent)
{

	layout = new QVBoxLayout(this);
	layout->setMargin(0);
	d_ptr = 0;
}

NEObjectController::~NEObjectController()
{
    if( d_ptr )
        delete d_ptr;
}

void NEObjectController::setObject(QObject* object)
{
	if(object == 0)
	{
		if(d_ptr == 0)
		{
			return;
		}
		else
		{
			// Don't remove if the old browser is assigned to another property editor
			if(d_ptr->m_browser->parentWidget() == layout->parentWidget())
			{
				d_ptr->m_browser->hide();
				layout->removeWidget(d_ptr->m_browser);
			}
			return;
		}
	}


	QMap<QObject*, NEObjectControllerPrivate*>::iterator iter = editMap.find(object);
	if(iter == editMap.end())
	{
		QtTreePropertyBrowser* browser = new QtTreePropertyBrowser(this);
		browser->setHeaderVisible(false);
		browser->setResizeMode(QtTreePropertyBrowser::ResizeToContents);
		browser->setAlternatingRowColors(false);
		browser->setRootIsDecorated(true);

		if(d_ptr)
		{
			// Don't remove if the browser is assigned to another property editor

			if(d_ptr->m_browser->parentWidget() == layout->parentWidget())
			{
				d_ptr->m_browser->hide();
				layout->removeWidget(d_ptr->m_browser);
			}

		}
		layout->addWidget(browser);
		browser->show();

        d_ptr = new NEObjectControllerPrivate;

		d_ptr->q_ptr = this;
		d_ptr->m_object = object;

		//! Assign the pointer to the object controller private part to the object
		NEObject* n = 0;
		if(n = qobject_cast<NEObject*>(object))
		{
            n->P->m_pObjCont = d_ptr;
			d_ptr->m_neobject = n;
		}

		if(!d_ptr->m_object)
			return;

		d_ptr->m_browser = browser;

		d_ptr->m_readOnlyManager = new NEVariantManager(this);
		d_ptr->m_manager = new NEVariantManager(this);
		d_ptr->m_factory = new NEVariantFactory(this);
		d_ptr->m_browser->setFactoryForManager(d_ptr->m_manager, d_ptr->m_factory);
		connect(d_ptr->m_manager, SIGNAL(valueChanged(QtProperty*, const QVariant&)),
				this, SLOT(slotValueChanged(QtProperty*, const QVariant&)));

		editMap[object] = d_ptr;
		d_ptr->m_manager->blockSignals(true);
		// Generate the staticproperties
		d_ptr->addObjectStaticProperties(d_ptr->m_object->metaObject(), static_cast<NEObject*>(object));
		d_ptr->m_manager->blockSignals(false);
		if(n)
			d_ptr->addObjectDynamicProperties(n);
		// Check if any properties have been previously expanded
		QMap<QString, QMap<QString, bool>>::iterator i = expandedMap.find(object->objectName());
		// For nodes with expanded states, update and expand the properties
		if(i != expandedMap.end())
		{
			d_ptr->updateAndExpandClassProperties(d_ptr->m_object->metaObject(), true, object, n, *i);
			// Remove the object from the map after expanding
			expandedMap.remove(object->objectName());
		}
		// Otherwise, only update the properties
		else
			d_ptr->updateClassProperties(d_ptr->m_object->metaObject(), true, object, n);

		// Connect expanded and collapsed signals of the tree property browser to here
		connect(browser, &QtTreePropertyBrowser::collapsed, this, &NEObjectController::itemCollapsed);
		connect(browser, &QtTreePropertyBrowser::expanded, this, &NEObjectController::itemExpanded);
	}
	else
	{
		if(d_ptr)
		{
			// Don't remove if the browser is assigned to another property editor
			if(d_ptr->m_browser->parentWidget() == layout->parentWidget())
			{
				d_ptr->m_browser->hide();
				layout->removeWidget(d_ptr->m_browser);
			}
		}
		layout->addWidget(iter.value()->m_browser);
		iter.value()->m_browser->show();
		d_ptr = iter.value();
		d_ptr->updateClassProperties(d_ptr->m_object->metaObject(), true, object, d_ptr->m_neobject);
	}
}

QObject* NEObjectController::object() const
{
	if(d_ptr)
		return d_ptr->m_object;
	else
		return 0;
}

//QTreeWidget* treeWidget();

QTreeWidget* NEObjectController::treeWidget()
{
	return ((QtTreePropertyBrowser*)d_ptr->m_browser)->treeWidget();
}

void NEObjectController::applyFilter(const QString& s)
{
	d_ptr->m_filterPattern = s;
	d_ptr->applyFilter();
}

void NEObjectController::updateProperties()
{
    if( !d_ptr )
        return;

    if( !d_ptr->m_object )
		return;

    d_ptr->updateClassProperties(d_ptr->m_object->metaObject(), true, d_ptr->m_object, d_ptr->m_neobject);
	// Update dynamic properties only for NEObject and derivatives
    if(d_ptr->m_neobject)
        d_ptr->updateDynamicProperties(d_ptr->m_neobject);
}

void NEObjectController::clearPropertiesMap()
{
	editMap.clear();
}

void NEObjectController::saveUIStates(QXmlStreamWriter* writer)
{
	writer->writeStartElement("expandedprops");
	// Iterate over all cached object controllers
	foreach(NEObjectControllerPrivate* ocpr, editMap)
	{
		if(ocpr->m_expandedProperties.size())
		{
			writer->writeStartElement(ocpr->m_object->objectName());
			ocpr->saveExpandedState(writer);
			writer->writeEndElement();
		}
	}
	// Save the states of the non-expanded nodes
	for(QMap<QString, QMap<QString, bool>>::iterator i = expandedMap.begin(); i != expandedMap.end(); ++i)
	{
		writer->writeStartElement(i.key());
		for(QMap<QString, bool>::iterator j = i->begin(); j != i->end(); ++j)
		{
			writer->writeStartElement(j.key());
			writer->writeEndElement();
		}
		writer->writeEndElement();
	}
	writer->writeEndElement();
}

void NEObjectController::loadUIStates(QXmlStreamReader* reader)
{
	QString nodeName;
	QString propName;

	while(reader->readNextStartElement())
	{
		// Read the expanded states of the properties
		if(reader->name() == "expandedprops")
		{
			// Read node names
			while(reader->readNext() != QXmlStreamReader::Invalid)
			{
				if(reader->tokenType() == QXmlStreamReader::EndElement && reader->name() == "expandedprops")
					break;
				else if(reader->tokenType() != QXmlStreamReader::StartElement)
					continue;

				nodeName = reader->name().toString();
				// Read expanded properties
				while(reader->readNext() != QXmlStreamReader::Invalid)
				{
					if(reader->tokenType() == QXmlStreamReader::EndElement && reader->name() == nodeName)
						break;
					else if(reader->tokenType() != QXmlStreamReader::StartElement)
						continue;

					propName = reader->name().toString();
					expandedMap[nodeName][propName] = true;
				}
				if(reader->hasError())
				{
					Log() << "Error parsing scene file: " << reader->errorString();
					EndLog(NE::kLogWarning);
				}
			}
			if(reader->hasError())
			{
				Log() << "Error parsing scene file: " << reader->errorString();
				EndLog(NE::kLogWarning);
			}
		}
		else
		{
			reader->skipCurrentElement();
			Log() << "Unknown section " << reader->name() << " in the scene file.";
			EndLog(NE::kLogWarning);
		}
	}
}

void NEObjectController::removeObject(QObject* object)
{
	QMap<QObject*, NEObjectControllerPrivate*>::iterator iter = editMap.find(object);
	if(iter == editMap.end())
		return;

	if(object == d_ptr->m_object)
		d_ptr = 0;

	delete iter.value();
	editMap.erase(iter);
}

void NEObjectController::itemCollapsed(QtBrowserItem* item)
{
	if(d_ptr->m_expandedProperties.contains(item))
		d_ptr->m_expandedProperties.remove(item);
}


void NEObjectController::itemExpanded(QtBrowserItem* item)
{
	d_ptr->m_expandedProperties[item] = item;
}



#include "moc_NEObjectController.cpp"
