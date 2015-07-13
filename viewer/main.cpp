#include <qapplication.h>
#include <qmainwindow.h>
#include <qtoolbar.h>
#include <qtoolbutton.h>
#include <qcombobox.h>
#include <qslider.h>
#include <qlabel.h>
#include <qcheckbox.h>
#include "plot.h"
#include "dataset.h"
#include "qwt_color_map.h"
#include <qwt_scale_draw.h>

double scalingFactor[2];

class MyScaleDraw: public QwtScaleDraw
{
  private:

    double scalingFactor;

  public:

    MyScaleDraw( double _scalingFactor)
    {
        setTickLength( QwtScaleDiv::MajorTick, 10 );
        setTickLength( QwtScaleDiv::MinorTick, 2 );
        setTickLength( QwtScaleDiv::MediumTick, 0 );

        setLabelRotation( 0 );
        setLabelAlignment( Qt::AlignLeft | Qt::AlignVCenter );

        setSpacing( 10 );
        scalingFactor = _scalingFactor;
    }

    virtual QwtText label( double value ) const
    {
        QwtText h=QwtText(QString::number(value*scalingFactor));
        return h;
    }
};

class MainWindow: public QMainWindow
{
public:
    MainWindow( QWidget * = NULL );

private:
    Plot *d_plot;
};

MainWindow::MainWindow( QWidget *parent ):
    QMainWindow( parent )
{
    d_plot = new Plot( this );

    setCentralWidget( d_plot );

    d_plot->setAxisScaleDraw( QwtPlot::xBottom, new MyScaleDraw(scalingFactor[0]) );
    d_plot->setAxisScaleDraw( QwtPlot::yLeft, new MyScaleDraw(scalingFactor[1]) );

    QToolBar *toolBar = new QToolBar( this );

/*
#ifndef QT_NO_PRINTER
    QToolButton *btnPrint = new QToolButton( toolBar );
    btnPrint->setText( "Print" );
    btnPrint->setToolButtonStyle( Qt::ToolButtonTextUnderIcon );
    toolBar->addWidget( btnPrint );
    connect( btnPrint, SIGNAL( clicked() ),
        d_plot, SLOT( printPlot() ) );

    toolBar->addSeparator();
#endif
*/
    toolBar->addWidget( new QLabel("Color Map " ) );
    QComboBox *mapBox = new QComboBox( toolBar );
    mapBox->addItem( "RGB" );
    //mapBox->addItem( "Indexed Colors" );
    mapBox->addItem( "Hue" );
    mapBox->addItem( "Alpha" );
    mapBox->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
    toolBar->addWidget( mapBox );
    connect( mapBox, SIGNAL( currentIndexChanged( int ) ),
             d_plot, SLOT( setColorMap( int ) ) );

    toolBar->addWidget( new QLabel("Pumping time" ) );
    QComboBox *timeBox = new QComboBox( toolBar );
    for(int i=0; i < snapshots.size(); i++)
      timeBox->addItem( QString::number(snapshots[i]->getTime()) );

    timeBox->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
    toolBar->addWidget( timeBox );
    connect( timeBox, SIGNAL( currentIndexChanged( int ) ),
             d_plot, SLOT( setTimeStep( int ) ) );


    toolBar->addWidget( new QLabel( " Opacity " ) );
    QSlider *slider = new QSlider( Qt::Horizontal );
    slider->setRange( 0, 255 );
    slider->setValue( 255 );
    connect( slider, SIGNAL( valueChanged( int ) ), 
        d_plot, SLOT( setAlpha( int ) ) );

    toolBar->addWidget( slider );
    toolBar->addWidget( new QLabel("   " ) );

    QCheckBox *btnSpectrogram = new QCheckBox( "Spectrogram", toolBar );
    toolBar->addWidget( btnSpectrogram );
    connect( btnSpectrogram, SIGNAL( toggled( bool ) ),
        d_plot, SLOT( showSpectrogram( bool ) ) );

    QCheckBox *btnContour = new QCheckBox( "Contour", toolBar );
    toolBar->addWidget( btnContour );
    connect( btnContour, SIGNAL( toggled( bool ) ),
        d_plot, SLOT( showContour( bool ) ) );

    addToolBar( toolBar );

    btnSpectrogram->setChecked( true );
    btnContour->setChecked( false );

}

int main( int argc, char **argv  )
{
    readData("..\\pump.out");

    QApplication a( argc, argv );
    a.setStyle( "Windows" );

    MainWindow mainWindow;
    mainWindow.resize( 1000, 100 );
    mainWindow.show();

    return a.exec();
}


