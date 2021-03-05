#include "NESceneManagementDialog.h"
#include "ui_NESceneManagementDialog.h"
#include "Managers/NEGUIManager.h"
#include "Windows/Main/NEMainWindow.h"
#include "Core/Base/NEBase.h"

NESceneManagementDialog::NESceneManagementDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NESceneManagementDialog)
{
    ui->setupUi(this);

    connect(ui->buttonCreateScene, SIGNAL(clicked()), this, SLOT(accept()));
    connect(ui->buttonLoadScene, SIGNAL(clicked()), this, SLOT(accept()));
    connect(ui->buttonCreateScene, SIGNAL(clicked()), NEGUI.mainWindow(), SLOT(newScene()));
    connect(ui->buttonLoadScene, SIGNAL(clicked()), NEGUI.mainWindow(), SLOT(open()));
    connect(this, SIGNAL(rejected()), NEGUI.mainWindow(), SLOT(close()));

    QString arch("arch-unknown");
    QString bits("bit-unknown");
    QString compile("version-unknown");
    QString openmp("");
#ifdef _DEBUG
    compile = QString("Debug");
#else
    compile = QString("Release");
#endif

#ifdef _OPENMP
    openmp = QString("OpenMP");
#endif

#ifdef NE_BUILD_64
    bits = QString("64-bit");
#else
    bits = QString("32-bit");
#endif

#if defined(NE_FOR_WINDOWS)
    arch = QString("Windows");
#elif defined(NE_FOR_LINUX)
    arch = QString("Linux");
#elif defined(NE_FOR_MAC)
    arch = QString("Mac");
#endif


    setWindowTitle(tr("Neutrino v0.%6 - (%1-%2-%3)").arg(arch, bits, compile, openmp));
}

NESceneManagementDialog::~NESceneManagementDialog()
{
    delete ui;
}
