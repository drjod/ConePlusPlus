#ifndef NODE_H
#define NODE_H

#include <vector>

class CWell;

class CNode
{    
  private:      
  double location[2];
  double drawdown;  
 
  public:            
  CNode(double[], std::vector <CWell*>);
  ~CNode(void);
  
  double* getLocation(void);
  
  double getDrawdown(void);
  
  void incrementDrawdown(double);
  
  void resetDrawdown(void);
  
};

#endif
