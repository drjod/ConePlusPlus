#ifndef TIMESERIES_H
#define TIMESERIES_H

#include <QVector>
#include <QTextStream>

///////////////////////////////////////////////////////////////////////////
/// \brief The CTimeseries class
/// just input right now
///
class CTimeseries
{
  public:
    CTimeseries();

    bool readData(QTextStream *in);
    QVector <double> getTimes() {return times;}
    QVector <QVector <double> >  getNodeValues() {return nodeValues;}
    int getNumberOfNodes() {return numberOfNodes;}

  private:
    QVector <double> times;
    QVector <QVector <double> > nodeValues;
    int numberOfNodes;
};


#endif
