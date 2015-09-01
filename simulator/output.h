#ifndef OUTPUT_H
#define OUTPUT_H

#include <fstream>
#include <vector>
#include "node.h"
#include <iostream>


struct location
{
   double x[2];  
};


class COutstream
{
      
  private:
    std::string fileName;
    std::ofstream outFile;
    std::vector<location> outputNodes;
    std::vector<double> outputTimes;
    int distributionType, timingType;
  
  public:
  
    COutstream(std::string, int, int, std::vector<location>, std::vector<double>);
    ~COutstream(void);
  
    void write(double, int[], std::vector<CNode*>);   
    void writePotentially(double, int[], std::vector<CNode*>);    
  
    int getDistributionType() {return distributionType;}
    int getTimingType() {return timingType;}
    std::string getFileName() {return fileName;}
  
};

#endif
