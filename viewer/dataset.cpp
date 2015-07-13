#include <QMessageBox>
#include <QVector>
#include <QFile>
#include <QTextStream>
#include "dataset.h"

extern double drawdownRange[2];
extern double scalingFactor[2];

CSnapshot::CSnapshot(double _time , int nnodes, int numberOfNodesX, int numberOfNodesY)
{
  if( numberOfNodesX * numberOfNodesY != nnodes )
    QMessageBox::information(0,"info","Error in pump.out header - number of nodes incoherent");

  time =_time;
  x = new double[nnodes];
  y = new double[nnodes];
  drawdown = new double[nnodes];

  numberOfNodes[0] = numberOfNodesX;
  numberOfNodes[1] = numberOfNodesY;
}

CSnapshot::~CSnapshot()
{
  delete [] x;
  delete [] y;
  delete [] drawdown;
}

double CSnapshot::getTime(void)
{
  return time;
}

double CSnapshot::getDrawdown(int ndx)
{
  return drawdown[ndx];
}

double CSnapshot::getNumberOfNodes(int ndx)
{
  return numberOfNodes[ndx];
}

void CSnapshot::readSnapshotData(QTextStream *in, int numberOfLines)
{
  QString valueString;
  QStringList  fields;


  for (int i = 0; i < numberOfLines; i++)
  {
    valueString = in->readLine();
    fields = valueString.split(" ");

    if(i == 1)
        scalingFactor[1] =  QString(fields[2]).toDouble();
    if(i == numberOfNodes[1])
        scalingFactor[0] =  QString(fields[1]).toDouble();

    x[QString(fields[0]).toInt()] = QString(fields[1]).toDouble();
    y[QString(fields[0]).toInt()] = QString(fields[2]).toDouble();
    drawdown[QString(fields[0]).toInt()] = QString(fields[3]).toDouble();

    if(QString(fields[3]).toDouble() < drawdownRange[0] )
      drawdownRange[0] = QString(fields[3]).toDouble();
    if(QString(fields[3]).toDouble() > drawdownRange[1] )
      drawdownRange[1] = QString(fields[3]).toDouble();

  }
}

void readData(QString fileName)
{
  QFile file(fileName);
  if(!file.open(QIODevice::ReadOnly))
    QMessageBox::information(0,"info","No file pump.out in parent directory");

  QTextStream in (&file);
  QString valueString ;
  QStringList fields;

  for (;;)
  {
    valueString = in.readLine();
    fields = valueString.split(" ");
    if ( fields[0].trimmed().isEmpty() ) // nothing but whitespace
      break;
    else
    {
      if(fields[0] == "time")
      {
        //times.append(QString(fields[1]).toInt())
        CSnapshot *data = new CSnapshot(QString(fields[1]).toDouble(),  QString(fields[3]).toInt(),  QString(fields[6]).toInt(),  QString(fields[7]).toInt());
        data->readSnapshotData(&in, QString(fields[3]).toInt());
        snapshots.append(data);
      }
    }
  } // for
  file.close();
}



