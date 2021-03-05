#include "FDYNVariantFactory.h"
#include "FDYNVariantManager.h"

#include "Windows/PropertyManager/fileedit.h"
#include "UIProperties/Windows/PropertyManager/NEColorRampEditor.h"
#include "Utility/NEColorRamp.h"

FDYNVariantFactory::~FDYNVariantFactory()
{
	QList<QWidget*> editors = theEditorToProperty.keys();
	QListIterator<QWidget*> it(editors);
	while(it.hasNext())
		delete it.next();
}

void FDYNVariantFactory::connectPropertyManager(QtVariantPropertyManager* manager)
{
	connect(manager, SIGNAL(valueChanged(QtProperty*, const QVariant&)),
			this, SLOT(slotPropertyChanged(QtProperty*, const QVariant&)));
	connect(manager, SIGNAL(attributeChanged(QtProperty*, const QString&, const QVariant&)),
			this, SLOT(slotPropertyAttributeChanged(QtProperty*, const QString&, const QVariant&)));
	QtVariantEditorFactory::connectPropertyManager(manager);
}

QWidget* FDYNVariantFactory::createEditor(QtVariantPropertyManager* manager,
										QtProperty* property, QWidget* parent)
{
    if(manager->propertyType(property) == FDYNVariantManager::filePathTypeId())
	{
		//std::cout<< "Filedit created"<<std::endl;
		FileEdit* editor = new FileEdit(parent);
		editor->setFilePath(manager->value(property).toString());
		editor->setFilter(manager->attributeValue(property, QLatin1String("filter")).toString());
		editor->setType((FilePath::DialogType)manager->attributeValue(property, QLatin1String("type")).toInt());
		theCreatedEditors[property].append(editor);
		theEditorToProperty[editor] = property;

		connect(editor, SIGNAL(filePathChanged(const QString&)),
				this, SLOT(slotSetValue(const QString&)));
		connect(editor, SIGNAL(destroyed(QObject*)),
				this, SLOT(slotEditorDestroyed(QObject*)));
		return editor;
	}
    if(manager->propertyType(property) == FDYNVariantManager::colorRampTypeId())
    {
        //std::cout<< "NEColorRampEditor created"<<std::endl;
        NEColorRampEditor* editor = new NEColorRampEditor(parent, Qt::Horizontal);
        NEColorRamp colorRamp = manager->value(property).value<NEColorRamp>();
        editor->setRamp(colorRamp.getColorRamp());
        editor->setSlideUpdate(true);
//        editor->setMappingTextVisualize(true);
        editor->setMappingTextColor(Qt::blue);
        editor->setMappingTextAccuracy(2);

        theCreatedEditors[property].append(editor);
        theEditorToProperty[editor] = property;

        connect(editor, SIGNAL(rampChanged()),
                this, SLOT(slotRampChanged()));
        connect(editor, SIGNAL(destroyed(QObject*)),
                this, SLOT(slotEditorDestroyed(QObject*)));
        return editor;
    }


	return QtVariantEditorFactory::createEditor(manager, property, parent);
}

void FDYNVariantFactory::disconnectPropertyManager(QtVariantPropertyManager* manager)
{
	disconnect(manager, SIGNAL(valueChanged(QtProperty*, const QVariant&)),
			   this, SLOT(slotPropertyChanged(QtProperty*, const QVariant&)));
	disconnect(manager, SIGNAL(attributeChanged(QtProperty*, const QString&, const QVariant&)),
			   this, SLOT(slotPropertyAttributeChanged(QtProperty*, const QString&, const QVariant&)));
	QtVariantEditorFactory::disconnectPropertyManager(manager);
}

void FDYNVariantFactory::slotPropertyChanged(QtProperty* property,
		const QVariant& value)
{
	if(!theCreatedEditors.contains(property))
		return;

	QList<QWidget*> editors = theCreatedEditors[property];
	QListIterator<QWidget*> itEditor(editors);
	while(itEditor.hasNext())
	{
		QWidget* w = itEditor.next();
		if(w != NULL)
		{
            QString str = value.toString();
            QPixmap pm = value.value<QPixmap>();
            if (str == "" && pm.isNull())
				return;
            FileEdit* pFileEdit = static_cast< FileEdit* >(w);
            if (str != "" && pFileEdit)
			{
                pFileEdit->blockSignals(true);
                pFileEdit->setFilePath(str);
                pFileEdit->blockSignals(false);
            }

            NEColorRampEditor* pColorRampEdit = static_cast< NEColorRampEditor* >(w);
            if (!pm.isNull() && pColorRampEdit)
            {
                pColorRampEdit->blockSignals(true);
                pColorRampEdit->blockSignals(false);
            }
		}
	}
}

void FDYNVariantFactory::slotPropertyAttributeChanged(QtProperty* property, const QString& attribute, const QVariant& value)
{
	if(!theCreatedEditors.contains(property))
		return;

	if(attribute == QLatin1String("filter"))
	{
		QList< QWidget* > editors = (theCreatedEditors.value(property));
		QListIterator< QWidget* > itEditor(editors);
		while(itEditor.hasNext())
		{
			FileEdit* ed = static_cast< FileEdit* >(itEditor.next());
			ed->setFilter(value.toString());
		}
	}
	else if(attribute == QLatin1String("type"))
	{
		QList< QWidget* > editors = (theCreatedEditors.value(property));
		QListIterator< QWidget* > itEditor(editors);
		while(itEditor.hasNext())
		{
			FileEdit* ed = static_cast< FileEdit* >(itEditor.next());
			ed->setType((FilePath::DialogType) value.toInt());
		}
	}
}

void FDYNVariantFactory::slotSetValue(const QString& value)
{
	QObject* object = sender();
	QMap<QWidget*, QtProperty*>::ConstIterator itEditor = theEditorToProperty.constBegin();
	while(itEditor != theEditorToProperty.constEnd())
	{
		if(itEditor.key() == object)
		{
			QtProperty* property = itEditor.value();
			QtVariantPropertyManager* manager = propertyManager(property);
			if(!manager)
				return;
			manager->setValue(property, value);
			return;
		}
		itEditor++;
	}
}

void FDYNVariantFactory::slotRampChanged()
{
    QObject* object = sender();
    QMap<QWidget*, QtProperty*>::ConstIterator itEditor = theEditorToProperty.constBegin();
    while(itEditor != theEditorToProperty.constEnd())
    {
        if(itEditor.key() == object)
        {
            QtProperty* property = itEditor.value();
            QtVariantPropertyManager* manager = propertyManager(property);
            if(!manager)
                return;

            NEColorRampEditor* colorRampEditor = reinterpret_cast<NEColorRampEditor*>(object);
            NEColorRamp colorRamp;
            colorRamp.setColorRamp(colorRampEditor->getRamp());
            manager->setValue(property, colorRamp);
            return;
        }
        itEditor++;
    }
}

void FDYNVariantFactory::slotEditorDestroyed(QObject* object)
{
	QMap<QWidget*, QtProperty*>::Iterator itEditor = theEditorToProperty.begin();
	while(itEditor != theEditorToProperty.end())
	{

		if(itEditor.key() == object)
		{
//			FileEdit* editor = static_cast< FileEdit* >(itEditor.key());
            QWidget* editor = static_cast< QWidget* >(itEditor.key());
            QtProperty* property = itEditor.value();

			theEditorToProperty.remove(editor);
			theCreatedEditors[property].removeAll(editor);
			if(theCreatedEditors[property].isEmpty())
				theCreatedEditors.remove(property);
			//std::cout<< "Filedit destroyed "<<theCreatedEditors[property].size()<<" "<<theEditorToProperty.size()<<std::endl;
			return;
		}
		itEditor++;
	}
}

