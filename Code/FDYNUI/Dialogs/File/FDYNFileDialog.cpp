#include "FDYNFileDialog.h"

FDYNFileWidget::FDYNFileWidget(QWidget * parent) : QFileDialog(parent, Qt::Widget)
{
    setViewMode(QFileDialog::List);
    setWindowFlags(Qt::Widget | Qt::FramelessWindowHint);
    setModal(false);
    setSizeGripEnabled(false);

    QList<QPushButton *> btn = findChildren<QPushButton *>();
    QListIterator<QPushButton *> iterator(btn);
    while (iterator.hasNext())
        iterator.next()->setVisible(false);

//    setMinimumSize(QSize(480, 320));
    setMinimumSize(QSize(700, 500));
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
}

FDYNFileWidget::~FDYNFileWidget()
{

}

void FDYNFileWidget::showEvent(QShowEvent* ev)
{
    QList<QPushButton *> buttons = findChildren<QPushButton *>();
    QListIterator<QPushButton*> i(buttons);
    while (i.hasNext())
        i.next()->setVisible(false);

    ev->accept();
}

FDYNFileDialog::FDYNFileDialog(QWidget *parent, QString wDir, QString caption, QString filter, DialogMode mode, QFileDialog::FileMode flags) : QDialog(parent)
{
    setStyle( QStyleFactory::create( "Fusion" ) );

    setModal(true);
    setWindowTitle(caption);
    setSizeGripEnabled(true);

    vboxLayout = new QVBoxLayout(this);
    vboxLayout->setSpacing(5);
    vboxLayout->setMargin(10);
    hboxLayout = new QHBoxLayout;
    hboxLayout->setSpacing(5);
    hboxLayout->setMargin(0);
    fileDialog = new FDYNFileWidget(this);
    fileDialog->setNameFilter(filter);
    fileDialog->selectNameFilter(filter);
    fileDialog->setDirectory(wDir);
    fileDialog->setFileMode(flags);

    if (mode == SAVE || mode == COLORRAMP_SAVE)
        fileDialog->setAcceptMode(QFileDialog::AcceptSave);
    else
        fileDialog->setAcceptMode(QFileDialog::AcceptOpen);

    hboxLayout->addWidget(fileDialog);

    vboxLayout1 = new QVBoxLayout;
    vboxLayout1->setSpacing(10);
    vboxLayout1->setMargin(0);
    vboxLayout1->setContentsMargins(0, 37, 0, 0);
    vboxLayout1->setAlignment(Qt::AlignTop);

    hboxLayout0 = new QHBoxLayout;
    hboxLayout0->setSpacing(5);
    hboxLayout0->setMargin(0);


    if (mode == OPEN || mode == PLOT3D)
    {
        vboxLayout1->addLayout(hboxLayout0);
    }
    else
    {

    }

    //useExistingCache = new QCheckBox(this);
    //useExistingCache->setFixedWidth(200);
    //useExistingCache->setCheckState(Qt::Unchecked);
    //useExistingCache->setText(tr("Use Existing Cache"));

    //copyCache = new QCheckBox(this);
    //copyCache->setFixedWidth(160);
    //copyCache->setCheckState(Qt::Unchecked);
    //copyCache->setText(tr("Copy Cache"));

    if (mode == SAVE)
    {
        //vboxLayout1->addWidget(useExistingCache);
        //vboxLayout1->addWidget(copyCache);
    } else
    {
        //useExistingCache->hide();
        //copyCache->hide();
    }

    scaleLabel = new QLabel(this);
    scaleLabel->setFixedWidth(160);
    scaleLabel->setText(tr("Scale:"));
    scaleLabel->hide();

    QRegExp rx("\\d{1,3}[.]\\d{1,4}");
    QRegExpValidator *validator = new QRegExpValidator(rx, 0);

    hboxLayout1 = new QHBoxLayout;
    hboxLayout1->setSpacing(3);
    hboxLayout1->setMargin(0);

    xLabel = new QLabel(this);
    xLabel->setFixedWidth(8);
    xLabel->setText(tr("X:"));

    xScaleEdit = new QLineEdit(this);
    xScaleEdit->setText("1.0000");
    xScaleEdit->setFixedWidth(50);
    xScaleEdit->setValidator(validator);

    yLabel = new QLabel(this);
    yLabel->setFixedWidth(8);
    yLabel->setText(tr("Y:"));

    yScaleEdit = new QLineEdit(this);
    yScaleEdit->setText("1.0000");
    yScaleEdit->setFixedWidth(50);
    yScaleEdit->setValidator(validator);

    zLabel = new QLabel(this);
    zLabel->setFixedWidth(8);
    zLabel->setText(tr("Z:"));

    zScaleEdit = new QLineEdit(this);
    zScaleEdit->setText("1.0000");
    zScaleEdit->setFixedWidth(50);
    zScaleEdit->setValidator(validator);

    hboxLayout1->addWidget(xLabel);
    hboxLayout1->addWidget(xScaleEdit);
    hboxLayout1->addWidget(yLabel);
    hboxLayout1->addWidget(yScaleEdit);
    hboxLayout1->addWidget(zLabel);
    hboxLayout1->addWidget(zScaleEdit);

    xLabel->hide();
    xScaleEdit->hide();
    yLabel->hide();
    yScaleEdit->hide();
    zLabel->hide();
    zScaleEdit->hide();

    swapXY = new QCheckBox(this);
    swapXY->setFixedWidth(160);
    swapXY->setCheckState(Qt::Unchecked);
    swapXY->setText(tr("Swap X/Y"));
    swapXY->setHidden( true );


    twoSideLighting = new QCheckBox(this);
    twoSideLighting->setFixedWidth(200);
    twoSideLighting->setCheckState(Qt::Checked);
    twoSideLighting->setText(tr("Two Side Lighting"));

    solidWireframe = new QCheckBox(this);
    solidWireframe->setFixedWidth(200);
    solidWireframe->setCheckState(Qt::Unchecked);
    solidWireframe->setText(tr("Solid Wire frame"));

    /*
    active = new QCheckBox(this);
    active->setFixedWidth(200);
    active->setCheckState(Qt::Unchecked);
    active->setText(tr("Active"));
    */

    /*
    meshesAsOneObject = new QCheckBox(this);
    meshesAsOneObject->setFixedWidth(180);
    meshesAsOneObject->setCheckState(Qt::Unchecked);
    meshesAsOneObject->setText(tr("All Meshes As One Object"));
    */


    swapXY->hide();
    twoSideLighting->hide();
    solidWireframe->hide();
    //active->hide();


    if (mode == IMPORT)
    {
        //vboxLayout1->addWidget(meshesAsOneObject);
    }
    else{
        //meshesAsOneObject->hide();
    }

    /*
    uniformScaleLabel = new QLabel(this);
    uniformScaleLabel->setText(tr("Uniform Scale:"));
    uniformScaleLabel->setFixedWidth(150);
    uniformScaleEdit = new QLineEdit(this);
    uniformScaleEdit->setFixedWidth(75);
    uniformScaleEdit->setValidator(validator);
    uniformScaleEdit->setText("1.00");

    hboxLayout3 = new QHBoxLayout;
    hboxLayout3->addWidget(uniformScaleLabel);
    hboxLayout3->addWidget(uniformScaleEdit);

    reload = new QCheckBox(this);
    reload->setFixedWidth(160);
    reload->setCheckState(Qt::Unchecked);
    reload->setText(tr("Reload"));



    swapAxis = new QCheckBox(this);
    swapAxis->setFixedWidth(160);
    swapAxis->setCheckState(Qt::Unchecked);
    swapAxis->setText(tr("Swap Y/Z Axis"));
    */


    /*
    vboxLayout1->addLayout(hboxLayout3);
    reload->hide();
    swapAxis->hide();
*/
    QSpacerItem *spacerItem = new QSpacerItem(2, 2, QSizePolicy::Minimum, QSizePolicy::Expanding);
    vboxLayout1->addItem(spacerItem);

    hboxLayout->addLayout(vboxLayout1);
    vboxLayout->addLayout(hboxLayout);

    hboxLayout2 = new QHBoxLayout;
    hboxLayout2->setSpacing(5);
    hboxLayout2->setContentsMargins(9, 0, 0, 9);

    //dataDir = new QLabel(this);
    //dataDir->setText(tr("Path of Data:"));

    //dataDirEdit = new QLineEdit(this);

    //refButton = new QPushButton(this);
    //refButton->setText(tr("..."));
    //refButton->setFixedWidth(30);

    QSpacerItem *spacerItem1 = new QSpacerItem(600, 2, QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);

    if (mode == IMPORT || mode == REFER || mode == COLORRAMP_SAVE || mode == COLORRAMP_OPEN)
    {
        //dataDir->hide();
        //dataDirEdit->hide();
        //refButton->hide();
        vboxLayout1->addLayout(hboxLayout2);
    }
    else
    {
        //hboxLayout2->addWidget(dataDir);
        //hboxLayout2->addWidget(dataDirEdit);
        //hboxLayout2->addWidget(refButton);
        hboxLayout2->addItem(spacerItem1);
        vboxLayout->addLayout(hboxLayout2);
    }

    okButton = new QPushButton(this);
    if (mode == SAVE || mode == COLORRAMP_SAVE)
        okButton->setText(tr("&Save"));
    else
        okButton->setText(tr("&OK"));
    okButton->setDefault(true);
    hboxLayout2->addWidget(okButton);
    cancelButton = new QPushButton(tr("&Cancel"), this);
    cancelButton->setAutoDefault(false);
    hboxLayout2->addWidget(cancelButton);

    QSpacerItem *spacerItem2 = new QSpacerItem(15, 2, QSizePolicy::Fixed, QSizePolicy::Minimum);
    hboxLayout2->addItem(spacerItem2);

    //QObject::connect(refButton, SIGNAL(clicked()), this, SLOT(setDataPath()));
    QObject::connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
    QObject::connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

    QObject::connect(fileDialog, SIGNAL(filesSelected(const QStringList &)), this, SLOT(accept()));
    QObject::connect(fileDialog, SIGNAL(accepted()), this, SLOT(accept()));
    QObject::connect(fileDialog, SIGNAL(rejected()), this, SLOT(reject()));

    resize(minimumSizeHint());
}

FDYNFileDialog::~FDYNFileDialog()
{

}


bool FDYNFileDialog::isSwapXY()
{
    return swapXY->checkState() == Qt::Unchecked ? false : true;
}


bool FDYNFileDialog::isTwoSideLighting()
{
    return twoSideLighting->checkState() == Qt::Unchecked ? false : true;
}

bool FDYNFileDialog::isSolidWireframe()
{
    return solidWireframe->checkState() == Qt::Unchecked ? false : true;
}

bool FDYNFileDialog::isActive()
{
    return active->checkState() == Qt::Unchecked ? false : true;
}

bool FDYNFileDialog::isMeshesAsOneOBject()
{
    return meshesAsOneObject->checkState() == Qt::Unchecked ? false : true;
}


float FDYNFileDialog::uniformScale()
{
    QString strScale = uniformScaleEdit->text();
    if (strScale.isEmpty())
        return 0.00f;
    else
        return strScale.toFloat();
}

bool FDYNFileDialog::isReload()
{
    return reload->checkState() == Qt::Unchecked ? false : true;
}


bool FDYNFileDialog::isSwapAxis()
{
    return swapAxis->checkState() == Qt::Unchecked ? false : true;
}

float FDYNFileDialog::xScale()
{
    QString strScale = xScaleEdit->text();
    if (strScale.isEmpty())
        return 0.00f;
    else
        return strScale.toFloat();
}

float FDYNFileDialog::yScale()
{
    QString strScale = yScaleEdit->text();
    if (strScale.isEmpty())
        return 0.00f;
    else
        return strScale.toFloat();
}

float FDYNFileDialog::zScale()
{
    QString strScale = zScaleEdit->text();
    if (strScale.isEmpty())
        return 0.00f;
    else
        return strScale.toFloat();
}

QString FDYNFileDialog::dataPath()
{
    return "";
    //return dataDirEdit->text();
}

void FDYNFileDialog::setDataPath()
{
    //QString dataPath = QFileDialog::getExistingDirectory(this, tr("Select Data Directory"));
    //if (!dataPath.isEmpty())
    //    dataDirEdit->setText(dataPath);
}

void FDYNFileDialog::accept()
{
    if (selectedFile().isEmpty())
        return;

    QDialog::accept();
}

QString FDYNFileDialog::selectedFile()
{
    QStringList selectFiles = fileDialog->selectedFiles();
    if (!selectFiles.isEmpty())
        return QDir::fromNativeSeparators(selectFiles[0]);

    return QString();
}

QStringList FDYNFileDialog::selectedFiles()
{
    return fileDialog->selectedFiles();
}

void FDYNFileDialog::addWidgets(QWidget *widgets)
{
    vboxLayout->addWidget(widgets);
}

void FDYNFileDialog::showEvent(QShowEvent* e)
{
    // remove buttons of QFileDialog.
    QList<QPushButton *> buttons = fileDialog->findChildren<QPushButton *>();
    QListIterator<QPushButton*> i(buttons);
    while (i.hasNext())
        i.next()->setVisible(false);

    if (fileDialog) fileDialog->setSizeGripEnabled(false);

    QDialog::showEvent(e);
}
