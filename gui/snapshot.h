#ifndef SNAPSHOT_H
#define SNAPSHOT_H

#include <QTextStream>

///////////////////////////////////////////////////////////////////////////
/// \brief The CSnapshot class
/// hosts snapshot data for specific time step
///
class CSnapshot
{
  private:
    double time;
    double *x, *y, *drawdown;
    int numberOfNodes[2]; // x, y

    // with the following arrays
    // legend is adapted for each snapshot individually (dependent on drawdown range)
    // also raster grid, scales of axes
    double drawdownRange[2]; // lower / upper limit
    double scalingFactor[2];  // to scale domain

  public:
    CSnapshot();
    ~CSnapshot();

    void config(double, int, int, int);
    bool readData(QTextStream *in, int);
    double getTime(){ return time;}
    double* getDrawdown(){return &drawdown[0];}
    int* getNumberOfNodes(){ return &numberOfNodes[0];}
    double* getScalingFactor(){return &scalingFactor[0];}
    double* getDrawdownRange(){return &drawdownRange[0];}
};


#endif
