#include "table.h"


CTable::CTable(std::string _name, double* _times, double* _values, int _numberOfEntries)
{    
  times = new double[_numberOfEntries];
  values = new double[_numberOfEntries];
  
  name  = _name;
  times = _times;
  values = _values;
  numberOfEntries = _numberOfEntries;
}

CTable::~CTable(void)
{}

std::string CTable::getName(void)
{
  return name;       
}

double* CTable::getTimes(void)
{
  return times;       
}


double* CTable::getValues(void)
{
  return values;       
}
