#ifndef WELL_H
#define WELL_H

#include <vector>
#include <iostream>

class CNode;

class CWell
{
  private:
  double location[2];
  double pumpingRate; 
  std::string tableName;  
  
  public:
  CWell(double[], double, std::vector <CNode*>, std::string);
  ~CWell(void);    
  
  // getter
  double getPumpingRate(void);
  double* getLocation(void); 
  std::string getTableName(void);
};

#endif
