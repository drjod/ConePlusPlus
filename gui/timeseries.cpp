#include "timeseries.h"



CTimeseries::CTimeseries()
{}



bool CTimeseries::readData( QTextStream *in)
{
    QString valueString ;
    QStringList fields;

    times.clear();
    nodeValues.clear();


    for (;;)
    {
      valueString = in->readLine();
      fields = valueString.split(" ");
      if ( fields[0].trimmed().isEmpty() ) // nothing but whitespace
        break;
      else if(fields[0] == "timeseries")
      {
        numberOfNodes = QString(fields[2]).toInt();
        valueString = in->readLine(); // line where variables mentioned
      }
      else
      {
        times.push_back( QString(fields[0]).toDouble() );
        QVector<double> timeStepNodeValues;
        for(int i=0; i < numberOfNodes; i++)
          timeStepNodeValues.push_back(QString(fields[i+1]).toDouble());
        nodeValues.push_back(timeStepNodeValues);
      }
    } // end for
    if (numberOfNodes == 0)
        return false;
    else
      return true;
}



