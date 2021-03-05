#include "Managers/FDYNGUIManager.h"
#include "Dialogs/File/FDYNFileDialog.h"
#include "fileedit.h"

FileEdit::FileEdit(QWidget *parent)
    : QWidget(parent), theType(FilePath::DialogType::kOpenFileType)
{
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setMargin(0);
    layout->setSpacing(0);
    theLineEdit = new QLineEdit(this);
    theLineEdit->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred));
    theLineEdit->setReadOnly(false);
    QToolButton *button = new QToolButton(this);
    button->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred));
    button->setText(QLatin1String("..."));
    layout->addWidget(theLineEdit);
    layout->addWidget(button);
    setFocusProxy(theLineEdit);
    setFocusPolicy(Qt::StrongFocus);
    setAttribute(Qt::WA_InputMethodEnabled);
    connect(theLineEdit, SIGNAL(textEdited(const QString &)),
            this, SIGNAL(filePathChanged(const QString &)));
    connect(button, SIGNAL(clicked()),
            this, SLOT(buttonClicked()));
}

void FileEdit::buttonClicked()
{
    QString strFilePath = filePath();
    QString fileName;
    QSettings settings("Fire Dynamics", "FRI3D");
    if( strFilePath.isNull() || strFilePath.isEmpty() )
    {
        strFilePath = settings.value( "lastOpenImportPath" ).toByteArray();
    } else
    {
        fileName = QFileInfo(strFilePath).baseName();
        if( !fileName.isEmpty() )
        {
            int nPos = strFilePath.lastIndexOf( fileName );
            if( nPos != -1 )
                strFilePath = strFilePath.left( nPos );
        }
    }
    switch( theType )
    {
    case FilePath::DialogType::kOpenFileType:
//        filePath = QFileDialog::getOpenFileName( reinterpret_cast< QMainWindow* >( NEGUI.mainWindow()),
//                                                 tr("Choose a file"),
//                                                 theLineEdit->text(), theFilter);
        {
           FDYNFileDialog dlg( reinterpret_cast< QMainWindow* >( FDYNGUI.mainWindow()),
                              strFilePath, tr("Choose a file"), theFilter);
            if( dlg.exec() != QDialog::Accepted )
                return;
            strFilePath = dlg.selectedFile();
        }
        break;
    case FilePath::DialogType::kRefeFileType:
        {
            FDYNFileDialog dlg( reinterpret_cast< QMainWindow* >(FDYNGUI.mainWindow()),
                              strFilePath, tr("Reference"), theFilter, FDYNFileDialog::REFER, QFileDialog::ExistingFiles);
            if( dlg.exec() != QDialog::Accepted )
                return;
            strFilePath = dlg.selectedFile();
            settings.setValue( "lastOpenImportPath", QFileInfo( strFilePath ).path() );
        }
        break;
    case FilePath::DialogType::kImpoFileType:
        {
            FDYNFileDialog dlg( reinterpret_cast< QMainWindow* >( FDYNGUI.mainWindow()),
                              strFilePath, tr("Import"), theFilter, FDYNFileDialog::IMPORT, QFileDialog::ExistingFiles);
            if( dlg.exec() != QDialog::Accepted )
                return;
            strFilePath = dlg.selectedFile();
            settings.setValue( "lastOpenImportPath", QFileInfo( strFilePath ).path() );
        }
        break;
    case FilePath::DialogType::kPLOT3DFileType:
        {
            FDYNFileDialog dlg( reinterpret_cast< QMainWindow* >( FDYNGUI.mainWindow()),
                              strFilePath, tr("PLOT 3D Files"), theFilter, FDYNFileDialog::PLOT3D,
                                QFileDialog::ExistingFiles);
            if( dlg.exec() != QDialog::Accepted )
                return;
            strFilePath = dlg.selectedFile();
            settings.setValue( "lastOpenImportPath", QFileInfo( strFilePath ).path() );
        }
        break;
    case FilePath::DialogType::kSaveFileType:
//        filePath = QFileDialog::getSaveFileName( reinterpret_cast< QMainWindow* >( NEGUI.mainWindow()),
//                                                 tr("Choose a file"),
//                                                 theLineEdit->text(), theFilter);
        {
            FDYNFileDialog dlg( reinterpret_cast< QMainWindow* >(FDYNGUI.mainWindow()),
                              strFilePath, tr("Choose a file"), theFilter, FDYNFileDialog::SAVE, QFileDialog::AnyFile);
            if( dlg.exec() != QDialog::Accepted )
                return;
            strFilePath = dlg.selectedFile();
        }
        break;
    case FilePath::DialogType::kOpenPathType:
        strFilePath = QFileDialog::getExistingDirectory( reinterpret_cast< QMainWindow* >( FDYNGUI.mainWindow()),
                                                      tr("Choose a folder"),
                                                      strFilePath);
        break;
    }

    if (strFilePath.isNull())
        return;
    theLineEdit->setText(strFilePath);
    emit filePathChanged(strFilePath);
}

void FileEdit::focusInEvent(QFocusEvent *e)
{
    theLineEdit->event(e);
    if (e->reason() == Qt::TabFocusReason || e->reason() == Qt::BacktabFocusReason) {
        theLineEdit->selectAll();
    }
    QWidget::focusInEvent(e);
}

void FileEdit::focusOutEvent(QFocusEvent *e)
{
    theLineEdit->event(e);
    QWidget::focusOutEvent(e);
}

void FileEdit::keyPressEvent(QKeyEvent *e)
{
    theLineEdit->event(e);
}

void FileEdit::keyReleaseEvent(QKeyEvent *e)
{
    theLineEdit->event(e);
}
