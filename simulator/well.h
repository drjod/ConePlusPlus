#ifndef WELL_H
#define WELL_H

#include <vector>

class CNode;

class CWell
{
  private:
  double location[2];
  double pumpingRate;   
  
  public:
  CWell(double[], double, std::vector <CNode*>);
  ~CWell(void);    
  
  // getter
  double getPumpingRate(void);
  double* getLocation(void); 
};

#endif
