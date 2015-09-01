#ifndef TIMESERIESPLOT_H
#define TIMESERIESPLOT_H


#include "timeseries.h"
#include <QWidget>
#include <qwt_plot.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_marker.h>
#include <qwt_plot_legenditem.h>


class LegendItem: public QwtPlotLegendItem
{
public:
    LegendItem()
    {
        setRenderHint( QwtPlotItem::RenderAntialiased );

        QColor color( Qt::white );

        setTextPen( color );
#if 1
        setBorderPen( color );

        QColor c( Qt::gray );
        c.setAlpha( 200 );

        setBackgroundBrush( c );
#endif
    }
};

///////////////////////////////////////////////////////////////////////////
/// \brief The TimeseriesPlot class
/// display of multiple curves (time, drawdown)
/// grid, (legend inactive)
/// zoom in and out
///
class TimeseriesPlot : public QwtPlot
{
  Q_OBJECT

  public:
    TimeseriesPlot( QWidget * = NULL );

    void update();
    bool readTimeseriesFile(QString);

  private:
    CTimeseries *timeseries;
    QwtPlotGrid *grid;
    QwtPlotCanvas *canvas;
    QwtPlotMarker *markerX, *markerY; // lines at x=0, y=0
    LegendItem *d_legendItem;
    void populate();

  public Q_SLOTS:
    void exportPlot();
};


#endif


