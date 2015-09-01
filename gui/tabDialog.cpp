
#include <QtWidgets>
#include "tabdialog.h"

///////////////////////////////////////////////////////////////////////////////////////
/// \brief TabDialog::TabDialog - hosts snapshots andd timeseries
/// \param parent
///
TabDialog::TabDialog(QWidget *parent)
   : QDialog(parent)
{
  tabWidget = new QTabWidget;
  snapshotTab = new SnapshotTab();
  timeseriesTab = new TimeseriesTab();
  tabWidget->addTab(snapshotTab, "Screenshot");
  tabWidget->addTab(timeseriesTab, "Time series");

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addWidget(tabWidget);

  setLayout(mainLayout);
}

SnapshotTab::SnapshotTab(QWidget *parent)
    : QWidget(parent)
{   
    snapshotPlot = new SnapshotPlot( this );
    toolBar = new QToolBar( this );

    mapBox = new QComboBox( this );
    mapBox->addItem( "RGB" );
    mapBox->addItem( "Hue" );
    mapBox->addItem( "Alpha" );
    mapBox->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );

    timeBox = new QComboBox( this );
    timeBox->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );

    slider = new QSlider( Qt::Horizontal );
    slider->setRange( 0, 255 );
    slider->setValue( 255 );

    exportButton = new QPushButton( "Export", toolBar );
    contourButton = new QCheckBox( "Contour", toolBar );
    contourButton->setChecked( false );

    toolBar->addWidget( new QLabel("Color Map " ) );
    toolBar->addWidget( mapBox );
    toolBar->addWidget( new QLabel(" Time" ) );
    toolBar->addWidget( timeBox );
    toolBar->addWidget( new QLabel( " Opacity " ) );
    toolBar->addWidget( slider );
    toolBar->addWidget( new QLabel("   " ) );
    toolBar->addWidget( contourButton );
    toolBar->addWidget( exportButton );

    QObject::connect( mapBox, SIGNAL( currentIndexChanged( int ) ),
             snapshotPlot, SLOT( setColorMap( int ) ) );
    QObject::connect( timeBox, SIGNAL( currentIndexChanged( int ) ),
             snapshotPlot, SLOT( setTimeStep( int ) ) );
    QObject::connect( slider, SIGNAL( valueChanged( int ) ),
             snapshotPlot, SLOT( setAlpha( int ) ) );
    QObject::connect( contourButton, SIGNAL( toggled( bool ) ),
             snapshotPlot, SLOT( showContour( bool ) ) );
    QObject::connect( exportButton, SIGNAL( clicked() ),
             snapshotPlot, SLOT( exportPlot() ) );

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(toolBar);
    mainLayout->addWidget(snapshotPlot);
    setLayout(mainLayout);
}

TimeseriesTab::TimeseriesTab(QWidget *parent)
    : QWidget(parent)
{
    timeseriesPlot = new TimeseriesPlot( this );

    toolBar = new QToolBar( this );
    exportButton = new QPushButton( toolBar );
    exportButton->setText( "Export" );
    toolBar->addWidget( exportButton );

    QObject::connect( exportButton, SIGNAL( clicked() ), timeseriesPlot, SLOT( exportPlot() ) );

    QVBoxLayout *mainLayout = new QVBoxLayout( this );;
    mainLayout->setContentsMargins( 0, 0, 0, 0 );
    mainLayout->addWidget(toolBar);
    mainLayout->addWidget(timeseriesPlot);
    setLayout(mainLayout);
}




void SnapshotTab::update()
{
  timeBox->clear();
  for(int i=0; i < getSnapshotPlot()->getSnapshots().size(); i++)
    timeBox->addItem( QString::number(getSnapshotPlot()->getSnapshots()[i]->getTime()));

  getSnapshotPlot()->setTimeStep(0);  // 0 certainly exists
  getSnapshotPlot()->update();
}



