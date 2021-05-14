#include <QQuickWidget>
#include <QQuickItem>

#include "Managers/FDYNGUIManager.h"
#include "Base/NESceneManager.h"
#include "Base/NEBase.h"
#include "FDYNPropertyEditorWindow.h"
#ifdef OLD_PROPERTY
#include "ui_FDYNPropertyEditorWindow.h"
#endif
#include "Windows/PropertyManager/FDYNObjectController.h"
#include "Widgets/Dock/FDYNDockWidget.h"

QObject* FDYNPropertyEditorWindow::m_pPrevObject = 0;

FDYNPropertyEditorWindow::FDYNPropertyEditorWindow(QWidget *parent) :
    QMainWindow(parent)
#ifdef OLD_PROPERTY
    , ui(new Ui::FDYNPropertyEditorWindow)
#endif
{
#ifndef OLD_PROPERTY
    FDYNDockWidget* dw = qobject_cast< FDYNDockWidget *>( parent );
    m_objController = new FDYNObjectController( dw );
    setCentralWidget(m_objController);
    //dw->setView( m_objController );


    //QObject::connect(treeview, SIGNAL(sigTreeItemSelected(QVariant)), propertyObj, SIGNAL(sigPropertyItemChanged(QVariant)));
#else
    ui->setupUi(this);
#endif

    this->menuBar()->hide();
    // Set validator for the property name
    QRegExp rx("[A-Za-z_][A-Za-z_0-9]*");
    m_PropNameValidator = new QRegExpValidator(rx);
    m_FreeValidator = new QRegExpValidator();
    //ui->propNameLineEdit->setValidator(m_pRXValidator);

    othersCBStrs.resize(4);
    for(int i=0; i<othersCBStrs.size(); i++)
        othersCBStrs[i] = QString();

    this->setMinimumSize(200, 300);

#ifdef OLD_PROPERTY
    comboLineEdit = new QLineEdit(ui->comboBoxOthers);
    comboLineEdit->setClearButtonEnabled(true);
    ui->filterLabel->setVisible(false);
    ui->filterLineEdit->setVisible(false);
    ui->comboBoxOthers->setVisible(false);
    ui->comboBoxPropType->setVisible(false);
    ui->addPropButton->setVisible(false);

    connect(ui->actionLoad_preset, &QAction::triggered, this, &FDYNPropertyEditorWindow::loadPreset);
    connect(ui->actionSave_preset, &QAction::triggered, this, &FDYNPropertyEditorWindow::savePreset);
    connect(ui->addPropButton, &QPushButton::clicked, this, &FDYNPropertyEditorWindow::addProperty);
    connect(ui->comboBoxOthers, static_cast<void (QComboBox::*)(int)>(&QComboBox::activated), this, &FDYNPropertyEditorWindow::othersComboBoxActivated);
    connect(ui->comboBoxOthers, &QComboBox::editTextChanged, this, &FDYNPropertyEditorWindow::othersComboBoxEditTextChanged);
    connect(ui->filterLineEdit, &QLineEdit::textChanged, this, &FDYNPropertyEditorWindow::onFilterTextChanged);

    // Add dynamic property types to add property combo box
    QComboBox* cb = ui->comboBoxPropType;
    cb->addItem("bool", bool());
    cb->addItem("float", float());
    cb->addItem("double", double());
    cb->addItem("int", int());
    cb->addItem("uint", uint());
    cb->addItem("eulerf", eulerf());
    cb->addItem("quatf", quatf());
    cb->addItem("vec2f", vec2f());
    cb->addItem("vec3i", vec3i());
    cb->addItem("vec3f", vec3f());
    cb->addItem("vec3d", vec3d());
    cb->addItem("vec4f", vec4f());
    ui->comboBoxPropType->setCurrentIndex(0);;

    ui->comboBoxOthers->setLineEdit(comboLineEdit);
    comboLineEdit->setPlaceholderText("[Property name]");
    ui->comboBoxOthers->addItem(QString("[Property name]"));
    ui->comboBoxOthers->addItem(QString("[Visible name]"));
    ui->comboBoxOthers->addItem(QString("[Property path]"));
    ui->comboBoxOthers->addItem(QString("[Help text]"));
    ui->comboBoxOthers->setCurrentIndex(0);
    comboLineEdit->setText(QString());
#endif
}

FDYNPropertyEditorWindow::~FDYNPropertyEditorWindow()
{
#ifdef OLD_PROPERTY
    delete ui;
    delete m_PropNameValidator;
    delete m_FreeValidator;
#endif
}

void FDYNPropertyEditorWindow::showObjectProperty(QObject* object)
{
    NEObject* n;

    if(!object)
    {
        qobject_cast<FDYNDockWidget*>(parentWidget())->setTitleText("SETTINGS");
#ifdef OLD_PROPERTY
        ui->actionLoad_preset->setDisabled(true);
        ui->actionSave_preset->setDisabled(true);
        ui->actionRefresh->setDisabled(true);
        ui->actionReset_properties->setDisabled(true);
        ui->actionFilter_properties->setDisabled(true);
        ui->filterLineEdit->setDisabled(true);
#endif
    }
    else
    {
        static_cast<FDYNDockWidget*>(parentWidget())->setTitleText("SETTINGS - " + object->objectName());
#ifdef OLD_PROPERTY
        ui->actionLoad_preset->setEnabled(true);
        ui->actionSave_preset->setEnabled(true);
        ui->actionRefresh->setEnabled(true);
        ui->actionReset_properties->setEnabled(true);
        ui->actionFilter_properties->setEnabled(true);
        ui->filterLineEdit->setEnabled(true);
#endif
    }

#ifdef OLD_PROPERTY
    if(!(n = qobject_cast<NEObject*>(object)))
    {
        ui->actionAdd_property->setDisabled(true);
        ui->addPropButton->setDisabled(true);
        ui->comboBoxOthers->setDisabled(true);
    }
    // Properties can only be added to NEObject derivatives
    else
    {
        ui->actionAdd_property->setEnabled(true);
        ui->addPropButton->setEnabled(true);
        ui->comboBoxOthers->setEnabled(true);
    }

    m_pPrevObject = ui->objController->object();
    ui->objController->setObject(object);
#else
    m_pPrevObject = m_objController->object();
    m_objController->setObject(object);
#endif

}

QObject*FDYNPropertyEditorWindow::currentObject()
{
#ifdef OLD_PROPERTY
    return ui->objController->object();
#else
    return m_objController->object();
#endif

}

void FDYNPropertyEditorWindow::showPrevObjProperty()
{
    showObjectProperty(m_pPrevObject);
}

void FDYNPropertyEditorWindow::init()
{
#ifdef OLD_PROPERTY
    QObject::connect(ui->actionNew_editor, &QAction::triggered, &FDYNGUI, &FDYNGUIManager::createNewPropertyEditor);
    QObject::connect(ui->actionLoad_selected_object, &QAction::triggered, this, &FDYNPropertyEditorWindow::loadSelectedObject);
    showObjectProperty(currentObject());
#endif
}

void FDYNPropertyEditorWindow::updateProperties()
{
#if OLD_PROPERTY
    if(ui->objController->object())
        ui->objController->updateProperties();
#else
    if(m_objController->object())
        m_objController->updateProperties();
#endif

}

void FDYNPropertyEditorWindow::removeObject( QObject* object )
{
#if OLD_PROPERTY
    ui->objController->removeObject(object);
#else
    m_objController->removeObject(object);
#endif
}

void FDYNPropertyEditorWindow::loadSelectedObject()
{
    NEObject* o = NESCENE.selectedObject();
    showObjectProperty(o);
#if OLD_PROPERTY
    FDYNGUI.hideObjectFromOtherPropEditors(o, this);
#endif
}

void FDYNPropertyEditorWindow::loadPreset()
{
    QString path = QFileDialog::getOpenFileName(this,
                                                tr("Open Preset"), "./Presets", tr("FDYN Preset Files (*.nepreset)"));

    NEBASE.loadObjectPreset(currentObject(), path);
#if OLD_PROPERTY
    ui->objController->updateProperties();
#else
    m_objController->updateProperties();
#endif

}

void FDYNPropertyEditorWindow::savePreset()
{
    QString path = QFileDialog::getSaveFileName(this,
                                                tr("Save Preset"), "./Presets/"+ QString(currentObject()->metaObject()->className()) + ".nepreset", tr("FDYN Preset Files (*.nepreset)"));

    NEBASE.saveObjectPreset(currentObject(), path);
}

void FDYNPropertyEditorWindow::othersComboBoxActivated(int index)
{
    switch(index)
    {
    case 0:
        comboLineEdit->setPlaceholderText("[Property name]");
        comboLineEdit->setValidator(m_PropNameValidator);
        break;
    case 1:
        comboLineEdit->setPlaceholderText("[Visible name]");
        comboLineEdit->setValidator(m_FreeValidator);
        break;
    case 2:
        comboLineEdit->setPlaceholderText("[Property path]");
        comboLineEdit->setValidator(m_FreeValidator);
        break;
    case 3:
        comboLineEdit->setPlaceholderText("[Help text]");
        comboLineEdit->setValidator(m_FreeValidator);
        break;
    }

    comboLineEdit->setText(othersCBStrs[index]);
}

void FDYNPropertyEditorWindow::othersComboBoxEditTextChanged(const QString& str)
{
#ifdef OLD_PROPERTY
    if(str.isEmpty() || str[0] !='[')
        othersCBStrs[ui->comboBoxOthers->currentIndex()] = str;
#endif
}

void FDYNPropertyEditorWindow::addProperty()
{
#if OLD_PROPERTY
    NEObject* n;
    // Properties can only be added to NEObject derivatives
    if(!(n = qobject_cast<NEObject*>(currentObject())))
        return;
    if(othersCBStrs[0].size())
    {
        uint r = n->addProperty(othersCBStrs[0], ui->comboBoxPropType->currentData(), othersCBStrs[1], othersCBStrs[2], othersCBStrs[3]);
        if(r == NE::kReturnSuccess)
        {
            comboLineEdit->setText("");
            othersCBStrs[0] = QString("");
            m_LineEditAnim->setStartValue(QString("Property added!"));
            m_LineEditAnim->setEndValue(comboLineEdit->placeholderText());
            m_LineEditAnim->start();

        }
        else
        {
            comboLineEdit->setText("");
            othersCBStrs[0] = QString("");
            m_LineEditAnim->setStartValue(QString("Property already exists!"));
            m_LineEditAnim->setEndValue(comboLineEdit->placeholderText());
            m_LineEditAnim->start();
        }
    }
    else
    {
        m_LineEditAnim->setStartValue(QString("Enter a property name!"));
        m_LineEditAnim->setEndValue(comboLineEdit->placeholderText());
        m_LineEditAnim->start();
    }
#endif
}

void FDYNPropertyEditorWindow::onFilterTextChanged(const QString& s)
{

#ifdef OLD_PROPERTY
    ui->objController->applyFilter(s);
#else
    m_objController->applyFilter(s);
#endif
    //    if(ui->filterLineEdit->text().length() == 0)
    //        return;
    //    ui->objController->setObject(ui->objController->object());
    f= false;

    //    ui->objController->treeWidget()->expandAll();
    //    setChildsVisibility(ui->objController->treeWidget()->invisibleRootItem(), ui->filterLineEdit->text());

    //    ui->objController->treeWidget()->update();

}
#ifdef OLD_PROPERTY
void FDYNPropertyEditorWindow::setChildsVisibility(QTreeWidgetItem* item, QString exp)
#else
void FDYNPropertyEditorWindow::setChildsVisibility(FDYNPropertyItem* item, QString exp)
#endif
{
    if(!item)
        return;

    if(item->childCount() == 0)
    {
        if(item->text(0).contains(exp, Qt::CaseInsensitive))
        {
            item->setHidden(false);
            f = true;
        }
        else
        {

            item->setHidden(true);
        }
    }
    else
    {

        for(int i = 0; i < item->childCount(); ++i)
        {
            setChildsVisibility(item->child(i), exp);
        }

        if(item->text(0).contains(exp, Qt::CaseInsensitive))
        {
            item->setHidden(false);
        }
        else
        {
            if(!f)
                item->setHidden(true);
        }
    }
#ifdef OLD_PROPERTY
    if(item->parent() == 0 || item->parent() == ui->objController->treeWidget()->invisibleRootItem())
#else
    if(item->parent() == 0 || item->parent() == m_objController->treeWidget()->invisibleRootItem())
#endif
    {
        f = false;
    }
}

