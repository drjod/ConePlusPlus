#include "timeseriesPlot.h"
#include <qwt_plot_panner.h>
#include <qwt_plot_magnifier.h>
#include <qwt_plot_curve.h>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <qwt_plot_renderer.h>


TimeseriesPlot::TimeseriesPlot( QWidget *parent) :
    QwtPlot( parent )
{
  timeseries = new CTimeseries();

  // axes
  setAxisTitle( xBottom, "Time [s]" );
  setAxisTitle( yLeft, "Drawdown [m]" );

   // canvas
  canvas = new QwtPlotCanvas();
  canvas->setLineWidth( 1 );
  canvas->setFrameStyle( QFrame::Box | QFrame::Plain );
  canvas->setBorderRadius( 15 );
  setCanvas( canvas );

  // panning with the left mouse button
  ( void ) new QwtPlotPanner( canvas );

  // zoom in/out with the wheel
  ( void ) new QwtPlotMagnifier( canvas );
}

void TimeseriesPlot::populate()
{
  markerX = new QwtPlotMarker();
  markerX->setLineStyle( QwtPlotMarker::VLine );
  markerX->setXValue( 0.0 );
  markerX->attach( this );

  markerY = new QwtPlotMarker();
  markerY->setLineStyle( QwtPlotMarker::HLine );
  markerY->setYValue( 0.0 );
  markerY->attach( this );

  grid = new QwtPlotGrid;
  grid->enableXMin( true );
  grid->setMajorPen( Qt::gray, 0, Qt::DotLine );
  grid->setMinorPen( Qt::darkGray, 0, Qt::DotLine );
  grid->attach( this );

  const Qt::GlobalColor colors[] =
  {
      Qt::red,
      Qt::blue,
      Qt::darkCyan,
      Qt::darkMagenta,
      Qt::darkYellow
  };

  const int numColors = sizeof( colors ) / sizeof( colors[0] );
  for(int j = 0; j <  timeseries->getNumberOfNodes(); j++ )
  {
    const Qt::GlobalColor color = colors[ j % numColors ];

    QwtPlotCurve *curve = new QwtPlotCurve( "node " + QString::number(j));
    QPolygonF points;
    for (int i = 0; i < (int)timeseries->getTimes().size();i++ )
    {
       points << QPointF( timeseries->getTimes()[i], timeseries->getNodeValues()[i][j] );
    }
    curve->setPen( color, 2.0);
    curve->setSamples( points );
    curve->attach( this );
  }
  //legend->update();
  d_legendItem = new LegendItem();
  d_legendItem->attach( this );
}

void TimeseriesPlot::exportPlot()
{
    QwtPlotRenderer renderer;
    renderer.exportTo( this, "timeseries.pdf" );
}

void TimeseriesPlot::update()
{
    detachItems(); // remove old curves - because user may revisit viewPage
    populate();
    replot();
}

bool TimeseriesPlot::readTimeseriesFile(QString fileName)
{
  QFile file(fileName);
  QTextStream in (&file);

  if(!file.open(QIODevice::ReadOnly))
  {
    QMessageBox::information(0,"info","No timeseries data file");
    return false;
  }
  else
  {
    if( timeseries->readData(&in) == false )
        return false;
  } //end file open
  file.close();
  return true;
}

