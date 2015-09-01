#ifndef SPECTROGRAMDATA_H
#define SPECTROGRAMDATA_H

#include <qwt_plot.h>
#include <qwt_plot_spectrogram.h>
#include "snapshot.h"
#include <QVector>
#include <QMessageBox>

class SnapshotPlot;

///////////////////////////////////////////////////////////////////////////
/// \brief The SpectrogramData class - gate to drawdown values
///
class SpectrogramData: public QwtRasterData
{

  private:
    CSnapshot *snapshot; // handle

  public:
    SpectrogramData();

    void setSnapshot(CSnapshot*);
    void setIntervals(void);

   //  provides drawdown value at points on raster grid
   virtual double value( double x, double y ) const
    {  // drawdown value
         int i = (int)x + (int)y * snapshot->getNumberOfNodes()[0];
         return snapshot->getDrawdown()[i];
    }
};


#endif


