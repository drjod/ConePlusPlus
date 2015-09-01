#include "spectrogramData.h"


SpectrogramData::SpectrogramData()
{}

void SpectrogramData::setSnapshot(CSnapshot* _snapshot)
{snapshot = _snapshot;}

void SpectrogramData::setIntervals(void)
{  
  setInterval( Qt::XAxis, QwtInterval( 0, snapshot->getNumberOfNodes()[0] ) );
  setInterval( Qt::YAxis, QwtInterval( 0, snapshot->getNumberOfNodes()[1] ) );
  setInterval( Qt::ZAxis, QwtInterval( (int)snapshot->getDrawdownRange()[0],
                                       (int)snapshot->getDrawdownRange()[1]));
}


