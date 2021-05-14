#include "NEShelfItemEditor.h"
#include "ui_NEShelfItemEditor.h"
#include "Widgets/ShelfBar/NEShelfBar.h"
#include "Windows/Main/NEMainWindow.h"
#include "Managers/NEGUIManager.h"
#include "NEPreferencesDialog.h"

NEShelfItemEditor::NEShelfItemEditor(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NEShelfItemEditor)
{
    ui->setupUi(this);
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &NEShelfItemEditor::acceptDialog);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &NEShelfItemEditor::rejectDialog);
    connect(ui->pickIconButton, &QPushButton::clicked, this, &NEShelfItemEditor::pickIcon);

}

NEShelfItemEditor::~NEShelfItemEditor()
{
    delete ui;
}

void NEShelfItemEditor::populateEditor(QString name, QString help, QString script, QString icon, QTreeWidgetItem* twi)
{
    m_CurrentTreeWidgetItem = twi;
    m_LastIconPath = icon;
    ui->helpEdit->clear();
    ui->scriptEdit->clear();

    ui->nameEdit->setText(name);
    ui->helpEdit->appendPlainText(help);
    ui->scriptEdit->appendPlainText(script);
    QIcon ic;
    ic.addFile(icon);
    ui->pickIconButton->setIcon(ic);
}

void NEShelfItemEditor::acceptDialog()
{
    m_CurrentTreeWidgetItem->setText(0, ui->nameEdit->text());
    m_CurrentTreeWidgetItem->setIcon(3, ui->pickIconButton->icon());
    m_CurrentTreeWidgetItem->setText(3, m_LastIconPath);
    m_CurrentTreeWidgetItem->setText(1, ui->helpEdit->toPlainText());
    m_CurrentTreeWidgetItem->setText(2, ui->scriptEdit->toPlainText());
}

void NEShelfItemEditor::rejectDialog()
{

}

void NEShelfItemEditor::pickIcon()
{
    QString s = QFileDialog::getOpenFileName(this, "Pick an icon for the shelf item", ".", "*.*");
    m_LastIconPath = s;
    QIcon icon;
    icon.addFile(s);
    ui->pickIconButton->setIcon(icon);

}
