#include "NEChartsVisWindow.h"
#include "ui_NEChartsVisWindow.h"
#include <Base/NEBase.h>
#include <Base/NESceneManager.h>
#include <Managers/NEGUIManager.h>

NEChartsVisWindow::NEChartsVisWindow( QWidget *parent ): QMainWindow( parent ), ui( new Ui::NEChartsVisWindow )
{
    ui->setupUi( this );

    connect( ui->actionSaveAs, &QAction::triggered, this, &NEChartsVisWindow::saveChartAs );
    connect( ui->actionSave_Anim_As, &QAction::triggered, this, &NEChartsVisWindow::saveAnimChartAs );
    connect( ui->actionClear,  &QAction::triggered, this, &NEChartsVisWindow::clear );

    QLinearGradient plotGradient;
    plotGradient.setStart( 0, 0 );
    plotGradient.setFinalStop( 0, 200 );
    //plotGradient.setColorAt( 0, QColor( 0XFF, 0xE1, 170 ) );
    //plotGradient.setColorAt( 1, QColor( 100, 100, 100 ) );
    plotGradient.setColorAt( 0, QColor( 0x73, 0xB2, 0xFF ) );
    //plotGradient.setColorAt( 1, QColor( 0X77, 0x00, 0xB2 ) );
    //plotGradient.setColorAt( 0, QColor( 200, 200, 200 ) );
    plotGradient.setColorAt( 1, QColor( 200, 200, 200 ) );
    ui->customPlot->setBackground( plotGradient );

    ui->customPlot->setWindowTitle( "Measurements" );
    setWindowTitle( "Plots: " );

    ui->customPlot->clearItems();




    ui->customPlot->legend->setVisible(true);
    ui->customPlot->clearPlottables();
    ui->customPlot->clearGraphs();
    ui->customPlot->plotLayout()->clear();
    ui->customPlot->replot();
}

NEChartsVisWindow::~NEChartsVisWindow()
{
    clearPlots();
    delete ui;
}

uint NEChartsVisWindow::saveAnimChartAs()
{

    QFileDialog dialogFile( this );
    dialogFile.setAcceptDrops( true );
    dialogFile.setFileMode( QFileDialog::AnyFile );
    dialogFile.setViewMode( QFileDialog::Detail );
    dialogFile.setAcceptMode( QFileDialog::AcceptSave );

    QString saveAsStr = dialogFile.getSaveFileName( reinterpret_cast< QMainWindow* >( NEGUI.mainWindow()),
                                                    "Save Anim Chart Prefix",
                                                    ".",
                                                    "PNG Sequence Prefix(*)"
                                                    );
    QFileInfo sfInfo = QFileInfo( saveAsStr );

    QDir sfDir = sfInfo.dir();
    QString sfBaseName = sfInfo.baseName();

    uint currentFrame = NESCENE.CurrentFrame();

    for ( int i = currentFrame; i < NESCENE.FrameInfo().lastFrame(); ++i )
    {
        NESCENE.incrementFrame();
        NEGUI.updatePropertyEditor();
        uint currentFrame = NESCENE.CurrentFrame();
        QString paddedFrame = QString("%1").arg( currentFrame, 6, 10, QChar('0'));

        QString saveFileName = sfDir.path() + "/" + sfBaseName + "." + paddedFrame + "." + "png";
        if( !saveFileName.isEmpty() )
        {
            QFile file( saveFileName );
            if( !file.open( QIODevice::WriteOnly ) )
            {
                Log() << "Cannot save Image" << saveAsStr; EndLog( NE::kLogError );
                return NE::kReturnCannotSaveFile;
            }

            ui->customPlot->savePng( saveFileName );
        }

    }
    return 1;
}


uint NEChartsVisWindow::saveChartAs()
{
    QString saveAsStr = QFileDialog::getSaveFileName( reinterpret_cast< QMainWindow* >( NEGUI.mainWindow()),
                                                      "Save Chart As", ".", "PNG (*.png)");
    if( !saveAsStr.isEmpty() )
    {
        QFile file( saveAsStr );
        if( !file.open( QIODevice::WriteOnly ) )
        {
            Log() << "Cannot save Image" << saveAsStr; EndLog( NE::kLogError );
            return NE::kReturnCannotSaveFile;
        }

        ui->customPlot->savePng( saveAsStr );
        return NE::kReturnSuccess;
    }
    return 0;
}

uint NEChartsVisWindow::clear()
{
    clearPlots();
    return 0;
}

//
// Clears the data from the plot
//
void NEChartsVisWindow::clearPlot( QString chartName )
{
    if( m_graphMap1.contains( chartName ) )
    {
        QCPGraph* g = m_graphMap1.find( chartName ).value();
        g->clearData();
        ui->customPlot->clearGraphs();
        ui->customPlot->replot();
    }
}

//
// Clears all data from all plots
//

void NEChartsVisWindow::clearPlots()
{
    QMap< QString, QCPGraph* >::iterator itr = m_graphMap1.begin();
    for( ; itr != m_graphMap1.end(); ++itr )
    {
        itr.value()->clearData();
    }
    ui->customPlot->clearGraphs();
    ui->customPlot->replot();
}


//
// Remove the plot itself
//

void NEChartsVisWindow::removePlot( QString chartName )
{
    m_graphMap1.remove( chartName );

    if( ui->customPlot )
    {
        ui->customPlot->clearItems();
        ui->customPlot->clearPlottables();
        ui->customPlot->clearGraphs();
        ui->customPlot->plotLayout()->clear();

        QMap< QString, QCPGraph *>::iterator itr = m_graphMap1.begin();
        QVector< QString > graphNames;
        for( ; itr != m_graphMap1.end(); ++itr )
        {
            graphNames.push_back( itr.key() );
        }
        m_graphIndexMap.clear();
        m_graphMap1.clear();
        for( int i = 0; i < graphNames.size(); ++i )
        {
            setupPlot( graphNames[i] );
        }
        graphNames.clear();

    }
}

//
// Create all the plottables
//


void NEChartsVisWindow::setupPlot( QString chartName )
{
    QCPGraph* graph1 = NULL;

    if( m_graphMap1.contains( chartName ) )
    {
        graph1 = m_graphMap1.find( chartName ).value();
        ui->customPlot->removeGraph( graph1 );
        m_graphMap1.remove( chartName );
    }

    QCPAxisRect *axisRect = NULL;

    if( m_graphMap1.size() == 0 )
    {
        ui->customPlot->clearItems();
        ui->customPlot->clearPlottables();
        ui->customPlot->clearGraphs();
        ui->customPlot->plotLayout()->clear();
    }


    axisRect = new QCPAxisRect( ui->customPlot, true );
    axisRect->setupFullAxesBox( true );
    graph1 =  ui->customPlot->addGraph( axisRect->axis( QCPAxis::atBottom ), axisRect->axis( QCPAxis::atLeft ) );

    QCPAxis* xAxis  = axisRect->axis( QCPAxis::atBottom );
    QCPAxis* yAxis  = axisRect->axis( QCPAxis::atLeft );

    QCPAxis* yAxis2 = axisRect->axis( QCPAxis::atRight );
    QCPAxis* xAxis2 = axisRect->axis( QCPAxis::atTop );
    xAxis2->setVisible( true );
    yAxis2->setVisible( true );
    xAxis2->setTickLabels( false );
    yAxis2->setTickLabels( false );


    connect( xAxis, SIGNAL( rangeChanged( QCPRange ) ), xAxis2, SLOT( setRange( QCPRange ) ) );
    connect( yAxis, SIGNAL( rangeChanged( QCPRange ) ), yAxis2, SLOT( setRange( QCPRange ) ) );

    //axisRect->setupFullAxesBox( true );

    axisRect->axis( QCPAxis::atRight, 0 )->setTickLabels( true );

    unsigned int numGraphs = m_graphMap1.size();
    {
        ui->customPlot->plotLayout()->addElement( numGraphs, 0, axisRect );
        m_graphIndexMap[ chartName ] = numGraphs;
    }

    m_graphMap1[ chartName ] = graph1;

    //
    // Make left and bottom axes always transfer their ranges to right and top axes:
    //

    //
    // Allow user to drag axis ranges with mouse, zoom with mouse wheel and select graphs by clicking:
    //
    ui->customPlot->setInteractions(  QCP::iSelectLegend  | QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables );

    if( ui->customPlot->legend )
        ui->customPlot->legend->setVisible(true);

    ui->customPlot->replot();
}

void NEChartsVisWindow::setPlotType( QString plotType )
{

}

//
// Update all plots
//
void NEChartsVisWindow::updatePlots()
{
    ui->customPlot->replot();
}


//
// Update the specific data for the plot
//
void NEChartsVisWindow::updatePlot( QString chartName, QVector<double>& x0, QVector<double>& y0 )
{

    QCPGraph* graph1 = NULL;


    if( m_graphMap1.contains( chartName ) )
    {
        graph1 = m_graphMap1.find( chartName ).value();
    }
    else
    {
        std::cout << "Cant find plot - setting up " << std::endl;
        setupPlot( chartName );
    }


    //
    // Pass data points to graphs
    //
    graph1->setData( x0, y0 );
    graph1->setPen( QPen( Qt::red ) );
    graph1->setBrush( QBrush(QColor( 0 ,0 , 0xFF,230 ) ) );
    graph1->setScatterStyle( QCPScatterStyle( QCPScatterStyle::ssDot, QPen( Qt::darkBlue, 1.0), QBrush( Qt::darkYellow ), 5 ) );

    graph1->rescaleAxes();

}





void NEChartsVisWindow::screenShot( QString& fileName )
{
    QPixmap pm = QPixmap::grabWidget( this );
    pm.save( fileName );
}


