#include "output.h"
#include <iostream>
#include <vector>
#include "node.h"
#include <cmath>
#include "simulator.h"
  
COutstream::COutstream(std::string _fileName, int _distributionType, int _timingType, std::vector<location> _outputNodes, std::vector<double> _outputTimes)
{ 
                                   
  fileName.assign(_fileName);  // seet  fileName
  
  outFile.open(fileName.c_str());
  if (!outFile)
    std::cout << "ERROR - could not open file " << fileName << std::endl;
  
  if ( _distributionType == 0 || _distributionType == 1 )          
    distributionType = _distributionType;
  else
    std::cout << "ERROR in output constructor for " << fileName << " - distribution type not supported" << std::endl;
    
  if ( _timingType == 0 || _timingType == 1 )    
    timingType = _timingType;  
  else                      
    std::cout << "ERROR in output constructor for " << fileName << " - timing type not supported" << std::endl;
    
  outputNodes.clear();
  for (int i=0; i < _outputNodes.size(); i++)
    outputNodes.push_back(_outputNodes[i]);  
  
  outputTimes.clear(); 
  for (int i=0; i < _outputTimes.size(); i++)
    outputTimes.push_back(_outputTimes[i]); 
  
  if (distributionType == 1) // time series 
  {
    outFile << "timeseries - " << outputNodes.size() << " nodes" << std::endl;
    outFile << "time";
    for(int i=0; i < (int) outputNodes.size(); i++ )
      outFile << " - " << i << " (" << outputNodes[i].x[0]<< "," << outputNodes[i].x[1] << ")";    
    outFile << std::endl;    
  } 
  else
  {
    outFile << "snapshot nodeNumber x y drawdown" << std::endl;
  }
}
  
  
COutstream::~COutstream(void)
{
  outFile.close();
}

void COutstream::writePotentially(double currentTime, int numberOfNodes[], std::vector<CNode*> domainNodes)
{
  if (timingType == 0) // all times
  {
    write(currentTime, numberOfNodes, domainNodes);
  }
  else if (timingType == 1) // selected times
  {
    for(int i=0; i < (int)outputTimes.size(); i++)
    {
      if(fabs(outputTimes[i] - currentTime) < 1.e-10)
      {
         write(currentTime, numberOfNodes, domainNodes);       
      }
    }
  }
}
  
void COutstream::write(double currentTime, int numberOfNodes[], std::vector<CNode*> domainNodes)
{
  
  double distanceSquared, distanceSquared_min;
  int outputNode;
  
  if(distributionType == 0) // domain - snapshot
  {
    outFile << "time " << currentTime << " - " ;            
    outFile << domainNodes.size() << " nodes - " << numberOfNodes[0] << " " << numberOfNodes[1]  << std::endl; 
    for (int i=0; i < (int)domainNodes.size(); i++) 
    {      
      outFile << i << " " <<  domainNodes[i]->getLocation()[0] << " " << domainNodes[i]->getLocation()[1] << " " << domainNodes[i]->getDrawdown() << std::endl;   
    }
  }
  else if (distributionType == 1)  // nodes selected - time series 
  {
    outFile << currentTime;
    for (int i=0; i < (int)outputNodes.size(); i++) 
    {
      distanceSquared_min = 1e100;  
      // take closest domain node
      for (int j=0; j < (int)domainNodes.size(); j++)  
      {
        distanceSquared = distance_squared (domainNodes[j]->getLocation(), outputNodes[i].x);
        if(distanceSquared < distanceSquared_min) 
        {
          distanceSquared_min = distanceSquared;
          outputNode = j;
        }
      } 
      outFile << " " << domainNodes[outputNode]->getDrawdown();   
    }
    outFile << std::endl;
  }    
}

