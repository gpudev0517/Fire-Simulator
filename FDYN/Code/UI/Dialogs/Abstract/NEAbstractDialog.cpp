#include "Dialogs/Abstract/NEAbstractDialog.h"

NEAbstractDialog::NEAbstractDialog(QWidget* parent, uint DialogType): QDialog(parent)
{
    switch(DialogType)
    {
    case 1:
    {
        m_MainLayout = new QVBoxLayout(this);

        //setup main grid layout
        m_MainContentLayout = new QGridLayout(this);
        m_MainContentLayout->setColumnMinimumWidth(1,20);
        m_MainContentLayout->setColumnStretch(3,1);
        m_MainContentLayout->setRowMinimumHeight(1,10);
        m_MainContentLayout->setRowMinimumHeight(3,10);
        m_MainContentLayout->setRowStretch(5,1);
        m_MainLayout->insertLayout(0,m_MainContentLayout);

        //setup buttons and button layout
        m_SubmitButton = new QPushButton("OK");
        m_CancelButton = new QPushButton("Cancel");
        m_ButtonsLayout = new QHBoxLayout(this);
        m_ButtonsLayout->addWidget(m_SubmitButton);
        m_ButtonsLayout->addWidget(m_CancelButton);
        connect(m_SubmitButton, SIGNAL(clicked()), this, SLOT(accept()));
        connect(m_CancelButton, SIGNAL(clicked()), this, SLOT(reject()));
        m_MainLayout->addLayout(m_ButtonsLayout);
    }
        break;
    }
    m_SignalMapper = new QSignalMapper(this);
}

NEAbstractDialog::~NEAbstractDialog()
{
}

QWidget* NEAbstractDialog::getWidget(int index)
{
    QMap<int,QWidget*>::const_iterator i = m_Widgets.find(index);
    if(i != m_Widgets.end() && i.key() == index)
        return i.value();
    return NULL;
}

int NEAbstractDialog::getPropertyIndex(QWidget* widget)
{
    QMap<int,QWidget*>::const_iterator i;
    for(i = m_Widgets.begin(); i!=m_Widgets.end();i++)
    {
        if(*i==widget)
            return i.key();
    }
    return -1;
}

QGridLayout* NEAbstractDialog::groupBoxLayout(QString text)
{
    QMap<QString, QGridLayout*>::const_iterator i = m_Layouts.find(text);
    if (i != m_Layouts.end() && i.key() == text)
    {
        return i.value();
    }
    return NULL;
}

QGridLayout* NEAbstractDialog::addGroupBox(QString text, QVBoxLayout* pLayout, QWidget* parent)
{
    QGridLayout* layout = new QGridLayout(parent);
    QGroupBox* groupBox = new QGroupBox(text);
    groupBox->setLayout(layout);
    pLayout->addWidget(groupBox);
    m_Layouts.insert(text,layout);
    return layout;
}

QComboBox* NEAbstractDialog::addComboBox(const QStringList& itemList, int row, int column, QGridLayout* layout)
{
    QComboBox* comboBox = new QComboBox(this);
    QStringListIterator iterator(itemList);
    while (iterator.hasNext())
    {
        QString text = iterator.next();
        comboBox->addItem(tr(text.toLatin1()));
    }
    layout->addWidget(comboBox,row,column);// Qt::AlignLeft | Qt::AlignVCenter);

    return comboBox;
}

QComboBox* NEAbstractDialog::addComboBox(const QMetaEnum& metaEnum, int index, int row, int column, QGridLayout* layout)
{
    QComboBox* comboBox = new QComboBox(this);

    for(int i = 0; i < metaEnum.keyCount(); ++i)
    {
        const char* name = metaEnum.key(i);
        int value = metaEnum.keyToValue(name);
        QString text(name);
        comboBox->addItem(tr(text.toLatin1()), QVariant(value));
    }

    connect(comboBox, SIGNAL(currentIndexChanged(int)), m_SignalMapper, SLOT(map()));
    m_SignalMapper->setMapping(comboBox, index);
    connect(m_SignalMapper, SIGNAL(mapped(int)), this, SIGNAL(comboBoxIndexChanged(int)), Qt::UniqueConnection);

    layout->addWidget(comboBox,row,column);
    m_Widgets.insert(index,comboBox);
    return comboBox;
}



//void NEAbstractDialog::createGroupBoxForLayout(QVBoxLayout* layout)
//{
//	QMap<QString, QGridLayout*>::iterator i;
//	for (i = m_Layouts.begin(); i != m_Layouts.end();++i) 
//	{		
//		QGroupBox* groupBox = new QGroupBox(i.key());
//		groupBox->setLayout(i.value());
//		layout->addWidget(groupBox);
//	}
//
//}

void NEAbstractDialog::addLabel(QString name ,int row,  int column, QGridLayout* layout)
{
    QLabel *text = new QLabel(name,this);
    layout->addWidget(text,row,column);
}

void NEAbstractDialog::addLineEdit(int index ,int row, int column, QGridLayout* layout)
{
    //QValidator *validator = new QIntValidator(100, 999, this);
    QLineEdit *edit = new QLineEdit(this);
    layout->addWidget(edit,row,column);
    m_Widgets.insert(index,edit);
}

void NEAbstractDialog::addCheckBox(int index ,int row, int column, QGridLayout* layout)
{
    QCheckBox *cb = new QCheckBox(this);
    layout->addWidget(cb,row,column);
    m_Widgets.insert(index,cb);
}

void NEAbstractDialog::addSpinBox(int index , int row, int column, QGridLayout* layout, QIntValidator* validator)
{	
    QSpinBox *spinner = new QSpinBox(this);
    if(validator)
    {
        spinner->setMinimum(validator->bottom());
        spinner->setMaximum(validator->top());
    }
    else
        spinner->setMaximum(INT_MAX);
    layout->addWidget(spinner,row,column);
    m_Widgets.insert(index,spinner);
}

void NEAbstractDialog::addDoubleSpinBox(int index ,int row,  int column, QGridLayout* layout, QDoubleValidator* validator)
{
    QDoubleSpinBox *spinner = new QDoubleSpinBox(this);
    if(validator)
    {
        spinner->setMinimum(validator->bottom());
        spinner->setMaximum(validator->top());
    }
    spinner->setSingleStep(0.1);
    spinner->setDecimals(3);
    layout->addWidget(spinner,row,column);
    m_Widgets.insert(index,spinner);
}

void NEAbstractDialog::addVector4D(int index ,int row,  int column, QGridLayout* layout,QDoubleValidator* validator)
{
}

void NEAbstractDialog::addVector3D(int index ,int row,  int column, QGridLayout* layout,QDoubleValidator* validator)
{
}

/*!
        Reads all Q_PROPERTIES of the metaObject and creates widgets for it. Offset can be used to exclude properties of inherited classes.
*/
void NEAbstractDialog::addPropertyWidgets(const QObject *object, int row, int column, QGridLayout* layout, int offset)
{
    if(object == NULL)
        return;
    const QMetaObject *metaObject = object->metaObject();
    int count = metaObject->propertyCount();
    //int j = metaObject->propertyOffset();
    for (int i=offset; i<count; ++i)
    {
        //get i-th property
        QMetaProperty tmpProperty = metaObject->property(i);
        QMetaProperty* metaProperty = &tmpProperty;
        QString name = tr(metaProperty->name());

        if(metaProperty->isEnumType())
        {
            QMetaEnum enumProp = metaProperty->enumerator();
            addLabel(name,row,0,layout);
            addComboBox(enumProp,i,row++,1,layout);
        }
        else
        {
            //get the type of the property, if not user type it is a qt type
            int type = metaProperty->userType();

            switch(type)
            {
            case QVariant::Bool:
                addLabel(name,row,0,layout);
                addCheckBox(i,row++,1,layout);
                break;
            case QVariant::Int:
            {
                //QIntValidator* validator = (QIntValidator*)specificSettings->validator(i);
                addLabel(name,row,0,layout);
                addSpinBox(i,row++,1,layout);
            }
                break;
            case QVariant::UInt:
            {
                //QIntValidator* validator = (QIntValidator*)specificSettings->validator(i);
                addLabel(name,row,0,layout);
                addSpinBox(i,row++,1,layout);
            }
                break;
            case QVariant::Double:
            {
                //QDoubleValidator* validator = (QDoubleValidator*)specificSettings->validator(i);
                addLabel(name,row,0,layout);
                addDoubleSpinBox(i,row++,1,layout);
            }
                break;
            }
        }
        QVariant value = metaProperty->read(object);
        setValueOfWidget(i,value);
    }
}

void NEAbstractDialog::clearLayout(QGridLayout* layout)
{
    QLayoutItem *wItem = layout->takeAt(0);
    while (wItem != 0)
    {
        QWidget* widget = wItem->widget();
        layout->removeItem(wItem);
        delete widget;
        wItem = layout->takeAt(0);
    }
}


QVariant NEAbstractDialog::valueOfWidget(int index, uint type)
{
    return -1;
}

void NEAbstractDialog::setValueOfWidget(int index, QVariant value)
{
    QWidget* widget = getWidget(index);

    if(widget==NULL)
        return;

    if(QComboBox* comboBox = qobject_cast<QComboBox*>(widget))
    {
        int id = comboBox->findData(value);
        comboBox->setCurrentIndex(id);
    }
    else
    {
        switch(value.type())
        {
        case QVariant::Bool:
        {
            bool state = value.toBool();
            if(state)
                static_cast<QCheckBox*>(widget)->setCheckState(Qt::Checked);
            else
                static_cast<QCheckBox*>(widget)->setCheckState(Qt::Unchecked);
        }
            break;
        case QVariant::Int:
            static_cast<QSpinBox*>(widget)->setValue(value.toInt());
            break;
        case QVariant::UInt:
            static_cast<QSpinBox*>(widget)->setValue(value.toUInt());
            break;
        case QVariant::Double:
            static_cast<QDoubleSpinBox*>(widget)->setValue(value.toDouble());
            break;
        default:
            break;
        }
    }
}

void NEAbstractDialog::writeAllProperties(QObject *object, QGridLayout* layout, int offset)
{
    if(object == NULL)
        return;
    const QMetaObject *metaObject = object->metaObject();
    int count = metaObject->propertyCount();
    for (int i=offset; i<count; ++i)
    {
        QMetaProperty tmpProperty = metaObject->property(i);
        QMetaProperty* metaProperty = &tmpProperty;
        const char* mpName = metaProperty->name();
        printf("QMetaProperty::name: %s",  mpName);
        QWidget* widget = getWidget(i);
        if(widget==NULL)
            continue;

        if(metaProperty->isEnumType())
        {
            int value = static_cast<QComboBox*>(widget)->currentData().toInt();
            metaProperty->write(object,value);
        }
        else
        {
            switch(metaProperty->type())
            {
            case QVariant::Bool:
            {
                bool state = static_cast<QCheckBox*>(widget)->checkState();
                metaProperty->write(object,state);
            }
                break;
            case QVariant::Int:
            {
                int value = static_cast<QSpinBox*>(widget)->value();
                metaProperty->write(object,value);
            }
                break;
            case QVariant::UInt:
            {
                uint value = (uint) static_cast<QSpinBox*>(widget)->value();
                metaProperty->write(object,value);
            }
                break;
            case QVariant::Double:
            {
                double value = static_cast<QDoubleSpinBox*>(widget)->value();
                metaProperty->write(object,value);
            }
                break;
            default:
                break;
            }
        }
    }
}
