#include <QMessageBox>
#include <QVector>
#include <QFile>
#include <QTextStream>
#include "snapshot.h"


CSnapshot::CSnapshot()
{
    numberOfNodes[0]=0;
    numberOfNodes[1]=0;

    drawdownRange[0]=1e10;
    drawdownRange[1]=0;
}

CSnapshot::~CSnapshot()
{
  delete [] x;
  delete [] y;
  delete [] drawdown;
}

bool CSnapshot::readData(QTextStream *in, int numberOfLines)
{
  QString valueString;
  QStringList fields;

  for (int i = 0; i < numberOfLines; i++)
  {
    valueString = in->readLine();
    fields = valueString.split(" ");

    if(i == 0)  // use first line (lower left edge point,
    {           // (which is in the centre of first cell) to get size of a cell
      scalingFactor[0] =  2 * QString(fields[1]).toDouble();
      scalingFactor[1] =  2 * QString(fields[2]).toDouble();
    }

    x[QString(fields[0]).toInt()] = QString(fields[1]).toDouble();
    y[QString(fields[0]).toInt()] = QString(fields[2]).toDouble();
    drawdown[QString(fields[0]).toInt()] = QString(fields[3]).toDouble();

    if(QString(fields[3]).toDouble() < drawdownRange[0] ) // determine drawdown range
      drawdownRange[0] = QString(fields[3]).toDouble();
    if(QString(fields[3]).toDouble() > drawdownRange[1] )
      drawdownRange[1] = QString(fields[3]).toDouble();

  }
  if(drawdownRange[1] <1.)
      drawdownRange[1] = .01; // set range in plot for time 0

  return true;
}


void CSnapshot::config(double _time , int nnodes, int numberOfNodesX, int numberOfNodesY)
{
  if( numberOfNodesX * numberOfNodesY != nnodes )
    QMessageBox::information(0,"info","Error in header - number of nodes incoherent");

  time =_time;
  x = new double[nnodes];
  y = new double[nnodes];
  drawdown = new double[nnodes];

  numberOfNodes[0] = numberOfNodesX;
  numberOfNodes[1] = numberOfNodesY;
}


