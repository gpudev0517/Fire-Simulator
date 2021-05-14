#include "FDYNSimulateScenarioDialog.h"
#include "ui_FDYNSimulateScenarioDialog.h"

#include "globals_Plugin.h"
#include "Geometry/FEngine/FCFAST.h"


FDYNSimulateScenarioDialog::FDYNSimulateScenarioDialog(QWidget *parent, NENode* simengine) :
    QDialog(parent),
    ui(new Ui::FDYNSimulateScenarioDialog)
{
    m_simEngine = simengine;
    ui->setupUi(this);
    setStyle( QStyleFactory::create( "Fusion" ) );

    connect(ui->btnRun, SIGNAL(clicked()), this, SLOT(accept()));
    connect(ui->btnCancel, SIGNAL(clicked()), this, SLOT(reject()));
    connect(ui->lineSimTime, SIGNAL(editingFinished()), this, SLOT(editedLineEdit()));

    QString STYLE_COLOR_BACKGROUND = "#343235";
    QString STYLE_COLOR_FOREGROUND = "white";
    QString STYLE_COLOR_TRANSPARENT = "transparent";
    QString STYLE_COLOR_DOCK_TITLE_BACKGROUND = "#282629";

    QString styleSheet =
        "QWidget{"
            "background: " + STYLE_COLOR_BACKGROUND + ";}"

        "QTabBar::tab{"
            "background: " + STYLE_COLOR_TRANSPARENT + ";"
            "color: " + STYLE_COLOR_FOREGROUND + ";"
            "height: 30px;"
            "width:  80px;"
            "font: 8pt 'Segoe UI';"
            "font-weight:bold;"
            "border-top-left-radius: 10px;"
            "border-top-right-radius: 10px;}"

        "QTabBar::tab:selected {"
            "background: " + STYLE_COLOR_DOCK_TITLE_BACKGROUND + ";}"
        "QTabWidget::tab-bar {"
            "bottom: 2px;}";
    setStyleSheet(styleSheet);

    ui->tabWidget->setCurrentIndex(0);

    setDefaultLineEdit();

}

FDYNSimulateScenarioDialog::~FDYNSimulateScenarioDialog()
{

}

QString FDYNSimulateScenarioDialog::ScenarioName()
{
    return( ui->lineName->text() );
}

void FDYNSimulateScenarioDialog::SetScenarioName( QString scnName )
{
    ui->lineName->setText( scnName );
}

QString FDYNSimulateScenarioDialog::Desc()
{
    return( ui->lineDescription->text() );
}

void FDYNSimulateScenarioDialog::editedLineEdit()
{
    FCFAST* cfastengine = dynamic_cast< FCFAST *>( m_simEngine );

    QLineEdit *line = qobject_cast<QLineEdit *>(sender());
    QRegExp rx("[+-]?([0-9]*[.])?[0-9]+");

    if(line == ui->lineSimTime)
    {
        QString unit("s");

        int pos = 0;
        if( (pos = rx.indexIn(line->text(), pos)) != -1 )
            m_SimTime = rx.cap(0).toFloat();
        else
            m_SimTime = 0.0;

        if( cfastengine )
        {
            cfastengine->SetSimTime( m_SimTime );
            m_SimTime = cfastengine->SimTime();
        }

        line->setText(QString::number(m_SimTime) + QChar::Space + unit);
    }
    else if(line == ui->lineIntTemperature)
    {
        QString unit("°C");

        int pos = 0;
        if( (pos = rx.indexIn(line->text(), pos)) != -1 )
            m_IntTemperature = rx.cap(0).toFloat();
        else
            m_IntTemperature = 0.0;

        if( cfastengine )
            cfastengine->SetIntTemp( m_IntTemperature );

        line->setText(QString::number(m_IntTemperature) + QChar::Space + unit);
    }
    else if(line == ui->lineHumidity)
    {
        QString unit("%");

        int pos = 0;
        if( (pos = rx.indexIn(line->text(), pos)) != -1 )
            m_Humidity = rx.cap(0).toFloat();
        else
            m_Humidity = 0.0;

        if( cfastengine )
            cfastengine->SetHumidity( m_Humidity );


        line->setText(QString::number(m_Humidity) + QChar::Space + unit);
    }
    else if(line == ui->lineExtTemperature)
    {
        QString unit("°C");

        int pos = 0;
        if( (pos = rx.indexIn(line->text(), pos)) != -1 )
            m_ExtTemperature = rx.cap(0).toFloat();
        else
            m_ExtTemperature = 0.0;

        if( cfastengine )
            cfastengine->SetExtTemp( m_ExtTemperature );

        line->setText(QString::number(m_ExtTemperature) + QChar::Space + unit);
    }
    else if(line == ui->linePressure)
    {
        QString unit("Pa");

        int pos = 0;
        if( (pos = rx.indexIn(line->text(), pos)) != -1 )
            m_Pressure = rx.cap(0).toFloat();
        else
            m_Pressure = 0.0;

        if( cfastengine )
            cfastengine->SetPressure( m_Pressure );

        line->setText(QString::number(m_Pressure) + QChar::Space + unit);
    }
    else if(line == ui->lineMaxTimeStep )
    {
        int pos = 0;
        if( (pos = rx.indexIn(line->text(), pos)) != -1 )
            m_MaxTimeStep = rx.cap(0).toFloat();
        else
            m_MaxTimeStep = 0.0;

        if( cfastengine )
            cfastengine->SetMaxTimeStep( m_MaxTimeStep );

        line->setText(QString::number(m_MaxTimeStep) );
    }
    else if(line == ui->lineOutputInterval )
    {
        int pos = 0;
        if( (pos = rx.indexIn(line->text(), pos)) != -1 )
            m_OutputInterval = rx.cap(0).toFloat();
        else
            m_OutputInterval = 1.0;

        if( cfastengine )
        {
            cfastengine->SetTextOutputInt( m_OutputInterval );
            cfastengine->SetSmokeViewOutputInt( m_OutputInterval );
            cfastengine->SetSpreadSheetOutputInt( m_OutputInterval );
        }

        line->setText(QString::number(m_OutputInterval) );
    }


}

void FDYNSimulateScenarioDialog::setDefaultLineEdit()
{
    m_SimTime = 3600;
    m_IntTemperature = 20;
    m_Humidity = 50;
    m_ExtTemperature = 20;
    m_Pressure = 101325;
    m_OutputInterval = 1.0;

    m_MaxTimeStep = 0.1;
    m_LowerOxyLimit = 0.15;

    FCFAST* cfastengine = qobject_cast< FCFAST *>( m_simEngine );
    if( cfastengine )
    {
        m_SimTime = cfastengine->SimTime();
        m_IntTemperature = cfastengine->IntTemp();
        m_Humidity = cfastengine->Humidity();
        m_ExtTemperature = cfastengine->ExtTemp();
        m_Pressure = cfastengine->Pressure();
        m_LowerOxyLimit = cfastengine->LowerO2Limit();
        m_MaxTimeStep = cfastengine->MaxTimeStep();
        m_OutputInterval = cfastengine->TextOutputInt();
    }



    QString unit("s");
    ui->lineSimTime->setText(QString::number(m_SimTime) + QChar::Space + unit);

    unit = QString("°C");
    ui->lineIntTemperature->setText(QString::number(m_IntTemperature) + QChar::Space + unit);

    unit = QString("%");
    ui->lineHumidity->setText(QString::number(m_Humidity) + QChar::Space + unit);

    unit = QString("°C");
    ui->lineExtTemperature->setText(QString::number(m_ExtTemperature) + QChar::Space + unit);

    unit = QString("Pa");
    ui->linePressure->setText(QString::number(m_Pressure) + QChar::Space + unit);

    ui->lineMaxTimeStep->setText(QString::number(m_MaxTimeStep));
    ui->lineLowerOxyLimit->setText(QString::number(m_LowerOxyLimit));

    unit = QString("s");
    ui->lineOutputInterval->setText(QString::number(m_OutputInterval));


}
