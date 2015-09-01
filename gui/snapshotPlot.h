#ifndef SNAPSHOTPLOT_H
#define SNAPSHOTPLOT_H

#include <qwt_plot.h>
#include <qwt_plot_spectrogram.h>
#include <qwt_scale_draw.h>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_panner.h>
#include "snapshot.h"
#include "spectrogramData.h"
#include <QVector>
#include <QMessageBox>

///////////////////////////////////////////////////////////////////////////
/// \brief The SnapshotPlot class - colored 2D drawdown plots
/// optionally with contour lines
///
class SnapshotPlot: public QwtPlot
{
    Q_OBJECT

  public:
    SnapshotPlot(QWidget * = NULL );
    bool readSnapshotFile(QString);
    void update();
    void initialize();
    QVector<CSnapshot*> getSnapshots(){return snapshots;}

    enum ColorMap
    {
        RGBMap,
        HueMap,
        AlphaMap
    };

  public Q_SLOTS:
    void showContour( bool on );
    void showSpectrogram( bool on );
    void setColorMap( int );
    void setAlpha( int );
    void setTimeStep ( int );
    void exportPlot();

  private:
    QVector<CSnapshot*> snapshots; // one for each output time step
    double selectedTimeStep;       // in view combo box
    double drawdownRange[2];       // of selectedTimeStep
    int currentColorMap; // to switch between time steps

    QwtPlotSpectrogram *spectrogram;
    SpectrogramData *spectrogramData;

    QwtInterval zInterval;
    QwtScaleWidget *rightAxis;

    QwtPlotZoomer* zoomer; // with mouse buttons
    QwtPlotPanner *panner;
    QwtScaleDraw *scaleDraw;

    int d_mapType;  // coloring
    int d_alpha;
};

#endif


