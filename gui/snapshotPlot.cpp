#include <qprinter.h>
#include <qprintdialog.h>
#include <qnumeric.h>
#include <qwt_color_map.h>
#include <qwt_plot_spectrogram.h>
#include <qwt_scale_widget.h>
#include <qwt_plot_layout.h>
#include <qwt_plot_renderer.h>
#include "snapshotPlot.h"
#include "snapshot.h"
#include <QMessageBox>
#include <QVector>
#include <QFile>
#include <QTextStream>

///////////////////////////////////////////////////////////////////////////
/// \brief The MyScaleDraw class - to scale axes of 2D domain
///
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


class MyZoomer: public QwtPlotZoomer
{
  public:
    MyZoomer( QWidget *canvas ):
        QwtPlotZoomer( canvas )
    {
        setTrackerMode( AlwaysOn );
    }

    virtual QwtText trackerTextF( const QPointF &pos ) const
    {
        QColor bg( Qt::white );
        bg.setAlpha( 200 );

        QwtText text = QwtPlotZoomer::trackerTextF( pos );
        text.setBackgroundBrush( QBrush( bg ) );
        return text;
    }
};

class LinearColorMapRGB: public QwtLinearColorMap
{
  public:
    LinearColorMapRGB():
        QwtLinearColorMap( Qt::darkCyan, Qt::red, QwtColorMap::RGB )
    {
        addColorStop( 0.1, Qt::cyan );
        addColorStop( 0.3, Qt::green );
        addColorStop( 0.5, Qt::yellow );
    }
};

class HueColorMap: public QwtColorMap
{
  public:

    HueColorMap():
        d_hue1(0),
        d_hue2(359),
        d_saturation(150),
        d_value(200)
    {
        updateTable();
    }

    virtual QRgb rgb( const QwtInterval &interval, double value ) const
    {
        if ( qIsNaN(value) )
            return 0u;

        const double width = interval.width();
        if ( width <= 0 )
            return 0u;

        if ( value <= interval.minValue() )
            return d_rgbMin;

        if ( value >= interval.maxValue() )
            return d_rgbMax;

        const double ratio = ( value - interval.minValue() ) / width;
        int hue = d_hue1 + qRound( ratio * ( d_hue2 - d_hue1 ) );

        if ( hue >= 360 )
        {
            hue -= 360;

            if ( hue >= 360 )
                hue = hue % 360;
        }

        return d_rgbTable[hue];
    }

    virtual unsigned char colorIndex( const QwtInterval &, double ) const
    {
        // we don't support indexed colors
        return 0;
    }

  private:
    void updateTable()
    {
        for ( int i = 0; i < 360; i++ )
            d_rgbTable[i] = QColor::fromHsv( i, d_saturation, d_value ).rgb();

        d_rgbMin = d_rgbTable[ d_hue1 % 360 ];
        d_rgbMax = d_rgbTable[ d_hue2 % 360 ];
    }

    int d_hue1, d_hue2, d_saturation, d_value;
    QRgb d_rgbMin, d_rgbMax, d_rgbTable[360];
};

class AlphaColorMap: public QwtAlphaColorMap
{
  public:
    AlphaColorMap()
    {
        //setColor( QColor("DarkSalmon") );
        setColor( QColor("SteelBlue") );
    }
};

SnapshotPlot::SnapshotPlot( QWidget *parent ):
    QwtPlot( parent ),
    d_alpha(255)
{
    selectedTimeStep = 0;
    spectrogramData = new SpectrogramData();

    spectrogram = new QwtPlotSpectrogram();
    spectrogram->setRenderThreadCount( 0 ); // use system specific thread count
    spectrogram->setCachePolicy( QwtPlotRasterItem::PaintCache );
    spectrogram->attach( this );

    // LeftButton for the zooming
    // MidButton for the panning
    // RightButton: zoom out by 1
    // Ctrl+RighButton: zoom out to full size
    zoomer = new MyZoomer( canvas() );
    zoomer->setMousePattern( QwtEventPattern::MouseSelect2,
        Qt::RightButton, Qt::ControlModifier );
    zoomer->setMousePattern( QwtEventPattern::MouseSelect3,
        Qt::RightButton );

    panner = new QwtPlotPanner( canvas() );
    panner->setAxisEnabled( QwtPlot::yRight, false);
    panner->setMouseButton( Qt::MidButton );

    // Avoid jumping when labels with more/less digits
    // appear/disappear when scrolling vertically
    const QFontMetrics fm( axisWidget( QwtPlot::yLeft )->font() );
    scaleDraw = axisScaleDraw( QwtPlot::yLeft );
    scaleDraw->setMinimumExtent( fm.width( "100.00" ) );

    const QColor c( Qt::darkBlue );
    zoomer->setRubberBandPen( c );
    zoomer->setTrackerPen( c );

    replot();
}

void SnapshotPlot::showContour( bool on )
{
    spectrogram->setDisplayMode( QwtPlotSpectrogram::ContourMode, on );
    replot();
}

void SnapshotPlot::showSpectrogram( bool on )
{
    spectrogram->setDisplayMode( QwtPlotSpectrogram::ImageMode, on );
    spectrogram->setDefaultContourPen(
        on ? QPen( Qt::black, 0 ) : QPen( Qt::NoPen ) );

    replot();
}

void SnapshotPlot::setColorMap( int type )
{
    currentColorMap = type;

    QwtScaleWidget *axis = axisWidget( QwtPlot::yRight );
    const QwtInterval zInterval = spectrogram->data()->interval( Qt::ZAxis );

    d_mapType = type;

    int alpha = d_alpha;
    switch( type )
    {
        case SnapshotPlot::HueMap:
        {
            spectrogram->setColorMap( new HueColorMap() );
            axis->setColorMap( zInterval, new HueColorMap() );
            break;
        }
        case SnapshotPlot::AlphaMap:
        {
            alpha = 255;
            spectrogram->setColorMap( new AlphaColorMap() );
            axis->setColorMap( zInterval, new AlphaColorMap() );
            break;
        }
        case SnapshotPlot::RGBMap:
        {
            spectrogram->setColorMap( new LinearColorMapRGB() );
            axis->setColorMap( zInterval, new LinearColorMapRGB() );
            break;
        }
        default:
            QMessageBox::information(0,"info","ERROR - Color map not supported.");
            break;
    }
    spectrogram->setAlpha( alpha );

    replot();
}

void SnapshotPlot::setAlpha( int alpha )
{
    // setting an alpha value doesn't make sense in combination
    // with a color map interpolating the alpha value

    d_alpha = alpha;
    if ( d_mapType != SnapshotPlot::AlphaMap )
    {
        spectrogram->setAlpha( alpha );
        replot();
    }
}

void SnapshotPlot::setTimeStep( int number )
{
    if(number != -1)  // QComboBox::clear() produces signal with value -1
    {
      selectedTimeStep = number;
      update();
    }
}

void SnapshotPlot::exportPlot()
{
    QwtPlotRenderer renderer;
    renderer.exportTo( this, "snapshot.pdf" );
}


bool SnapshotPlot::readSnapshotFile(QString fileName)
{
  QFile file(fileName);
  if(!file.open(QIODevice::ReadOnly))
  {
    QMessageBox::information(0,"info","No snapshot data file");
    return false;
  }
  else
  {
    QTextStream in (&file);
    QString valueString ;
    QStringList fields;
    snapshots.clear();

    for (;;)
    {
      valueString = in.readLine();
      fields = valueString.split(" ");
      if ( fields[0].trimmed().isEmpty() ) // nothing but whitespace
        break;
      else
      {
        if(fields[0] == "time")
        {                      // output time step
          CSnapshot *snapshot = new CSnapshot();
          //               time                           nnodes                      numberOfNodesX           numberOfNodesY
          snapshot->config(QString(fields[1]).toDouble(), QString(fields[3]).toInt(), QString(fields[6]).toInt(),  QString(fields[7]).toInt());
          if (snapshot->readData(&in, QString(fields[3]).toInt())  == false)
              return false;
          snapshots.push_back(snapshot);
        }
      }
    } // end for
    file.close();
    spectrogramData->setSnapshot(this->snapshots[selectedTimeStep]);
  } // end file open

  if (snapshots.size() == 0)
    return false;
  else
    return true;
}

void SnapshotPlot::update()
{
    spectrogramData->setSnapshot(snapshots[selectedTimeStep]);
    spectrogramData->setIntervals();
    spectrogram->setData( spectrogramData);

    QList<double> contourLevels;
    for ( double level = 0.5; level < snapshots[selectedTimeStep]->getDrawdownRange()[1]; level += 1.0 )
        contourLevels += level;
    spectrogram->setContourLevels( contourLevels );

    spectrogram->attach( this );

    zInterval = spectrogram->data()->interval( Qt::ZAxis );

    // A color bar on the right axis
    rightAxis = axisWidget( QwtPlot::yRight );
    rightAxis->setTitle( "Drawdown" );
    rightAxis->setColorBarEnabled( true );

    setAxisScale( QwtPlot::yRight, zInterval.minValue(), zInterval.maxValue() );
    enableAxis( QwtPlot::yRight );

    // scale 2D domain
    setAxisScaleDraw( QwtPlot::xBottom, new MyScaleDraw(snapshots[selectedTimeStep]->getScalingFactor()[0]));
    setAxisScaleDraw( QwtPlot::yLeft, new MyScaleDraw(snapshots[selectedTimeStep]->getScalingFactor()[1]));

    plotLayout()->setAlignCanvasToScales( true );

    setColorMap( currentColorMap );

    replot();
}

void SnapshotPlot::initialize()
{
    setColorMap( SnapshotPlot::RGBMap );
    zoomer->setZoomBase();
}
