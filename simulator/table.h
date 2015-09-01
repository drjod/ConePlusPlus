#ifndef TABLE_H
#define TABLE_H

#include <iostream>

class CTable
{
  private:
    std::string name;
    double* times;
    double* values;
    int numberOfEntries;

  public:
    CTable(std::string, double*, double*, int);
    ~CTable(void);    
  
    // getter
    std::string getName(void);
    double* getTimes(void);
    double* getValues(void);
    int getNumberOfEntries(){ return numberOfEntries;}
  
};

#endif
