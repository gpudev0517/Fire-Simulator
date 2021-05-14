#include "Managers/NEGUIManager.h"
#include "Base/NESceneManager.h"
#include "Base/NEBase.h"
#include "NEPropertyEditorWindow.h"
#include "ui_NEPropertyEditorWindow.h"
#include "Windows/PropertyManager/NEObjectController.h"
#include "Widgets/Dock/NEDockWidget.h"

QObject* NEPropertyEditorWindow::m_pPrevObject = 0;

NEPropertyEditorWindow::NEPropertyEditorWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::NEPropertyEditorWindow)
{
    ui->setupUi(this);
    // Set validator for the property name
    QRegExp rx("[A-Za-z_][A-Za-z_0-9]*");
    m_PropNameValidator = new QRegExpValidator(rx);
    m_FreeValidator = new QRegExpValidator();
    //ui->propNameLineEdit->setValidator(m_pRXValidator);
    comboLineEdit = new QLineEdit(ui->comboBoxOthers);
    comboLineEdit->setClearButtonEnabled(true);
    othersCBStrs.resize(4);
    for(int i=0; i<othersCBStrs.size(); i++)
        othersCBStrs[i] = QString();

    m_LineEditAnim = new QPropertyAnimation(comboLineEdit, "placeholderText", comboLineEdit);
    m_LineEditAnim->setDuration(2000);

    ui->filterLabel->setVisible(false);
    ui->filterLineEdit->setVisible(false);
    ui->comboBoxOthers->setVisible(false);
    ui->comboBoxPropType->setVisible(false);
    ui->addPropButton->setVisible(false);

    connect(ui->actionLoad_preset, &QAction::triggered, this, &NEPropertyEditorWindow::loadPreset);
    connect(ui->actionSave_preset, &QAction::triggered, this, &NEPropertyEditorWindow::savePreset);
    connect(ui->addPropButton, &QPushButton::clicked, this, &NEPropertyEditorWindow::addProperty);
    connect(ui->comboBoxOthers, static_cast<void (QComboBox::*)(int)>(&QComboBox::activated), this, &NEPropertyEditorWindow::othersComboBoxActivated);
    connect(ui->comboBoxOthers, &QComboBox::editTextChanged, this, &NEPropertyEditorWindow::othersComboBoxEditTextChanged);
    connect(ui->filterLineEdit, &QLineEdit::textChanged, this, &NEPropertyEditorWindow::onFilterTextChanged);

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
}

NEPropertyEditorWindow::~NEPropertyEditorWindow()
{
    delete ui;
    delete m_PropNameValidator;
    delete m_FreeValidator;
}

void NEPropertyEditorWindow::showObjectProperty(QObject* object)
{
    NEObject* n;

    if(!object)
    {
        static_cast<NEDockWidget*>(parentWidget())->setTitleText("Property Editor");
        ui->actionLoad_preset->setDisabled(true);
        ui->actionSave_preset->setDisabled(true);
        ui->actionRefresh->setDisabled(true);
        ui->actionReset_properties->setDisabled(true);
        ui->actionFilter_properties->setDisabled(true);
        ui->filterLineEdit->setDisabled(true);
    }
    else
    {
        static_cast<NEDockWidget*>(parentWidget())->setTitleText("Property Editor - " + object->objectName());
        ui->actionLoad_preset->setEnabled(true);
        ui->actionSave_preset->setEnabled(true);
        ui->actionRefresh->setEnabled(true);
        ui->actionReset_properties->setEnabled(true);
        ui->actionFilter_properties->setEnabled(true);
        ui->filterLineEdit->setEnabled(true);
    }

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
}

QObject*NEPropertyEditorWindow::currentObject()
{
    return ui->objController->object();
}

void NEPropertyEditorWindow::showPrevObjProperty()
{
    showObjectProperty(m_pPrevObject);
}

void NEPropertyEditorWindow::init()
{
    QObject::connect(ui->actionNew_editor, &QAction::triggered, &NEGUI, &NEGUIManager::createNewPropertyEditor);
    QObject::connect(ui->actionLoad_selected_object, &QAction::triggered, this, &NEPropertyEditorWindow::loadSelectedObject);
    QObject::connect(ui->objController, &NEObjectController::showNodePropertyExpression, &NEGUI, &NEGUIManager::showNodePropertyExpression );
    QObject::connect(ui->objController, &NEObjectController::pushUserEvent, &NEGUI, &NEGUIManager::pushUserEvent );
    QObject::connect(ui->objController, &NEObjectController::updateGLWindows, &NEGUI, &NEGUIManager::updateGLWindows );


    showObjectProperty(currentObject());
}

void NEPropertyEditorWindow::updateProperties()
{
    if(ui->objController->object())
        ui->objController->updateProperties();
}

void NEPropertyEditorWindow::removeObject( QObject* object )
{
    ui->objController->removeObject(object);
}

void NEPropertyEditorWindow::loadSelectedObject()
{
    NEObject* o = NESCENE.selectedObject();
    showObjectProperty(o);
    NEGUI.hideObjectFromOtherPropEditors(o, this);
}

void NEPropertyEditorWindow::loadPreset()
{
    QString path = QFileDialog::getOpenFileName(this,
                                                tr("Open Preset"), "./Presets", tr("Neutrino Preset Files (*.nepreset)"));

    NEBASE.loadObjectPreset(currentObject(), path);
    ui->objController->updateProperties();

}

void NEPropertyEditorWindow::savePreset()
{
    QString path = QFileDialog::getSaveFileName(this,
                                                tr("Save Preset"), "./Presets/"+ QString(currentObject()->metaObject()->className()) + ".nepreset", tr("Neutrino Preset Files (*.nepreset)"));

    NEBASE.saveObjectPreset(currentObject(), path);
}

void NEPropertyEditorWindow::othersComboBoxActivated(int index)
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

void NEPropertyEditorWindow::othersComboBoxEditTextChanged(const QString& str)
{
    if(str.isEmpty() || str[0] !='[')
        othersCBStrs[ui->comboBoxOthers->currentIndex()] = str;
}

void NEPropertyEditorWindow::addProperty()
{
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
}

void NEPropertyEditorWindow::onFilterTextChanged(const QString& s)
{
    ui->objController->applyFilter(s);
    //    if(ui->filterLineEdit->text().length() == 0)
    //        return;
    //    ui->objController->setObject(ui->objController->object());
    f= false;

    //    ui->objController->treeWidget()->expandAll();
    //    setChildsVisibility(ui->objController->treeWidget()->invisibleRootItem(), ui->filterLineEdit->text());

    //    ui->objController->treeWidget()->update();

}

void NEPropertyEditorWindow::setChildsVisibility(QTreeWidgetItem* item, QString exp)
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
    if(item->parent() == 0 || item->parent() == ui->objController->treeWidget()->invisibleRootItem())
    {
        f = false;
    }
}

