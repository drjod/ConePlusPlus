#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>
#include <cmath>
#include "simulator.h"

int verbosity = 1;

CSimulator::CSimulator(void)
{                                      
  outFile.open("pump.out");
}

CSimulator::~CSimulator(void)
{
  outFile.close();
}

void CSimulator::setupProblem(void)
{   
  std::ifstream inFile;
  char line[256];
  std::string line_str;
  std::stringstream str_in;
    
  inFile.open("pump.in");
  if (!inFile)
    std::cout << "ERROR - could not open file pump.in" << std::endl;
      
  for(;;)
  {
    line_str.clear();
         
    inFile.getline(line, 256);
    line_str = line;
    
    if(line_str.find("end") != std::string::npos)                  
      break;
  
    if(line_str.find("aquifer") != std::string::npos)
    {   
      inFile.getline(line, 256);
      line_str = line;                             
      str_in.str(line_str);    
        
      str_in >> aquifer.transmissivity >> aquifer.storativity;    
       
      line_str.clear();
      str_in.clear();              
    }  // aquifer
    
    if(line_str.find("well") != std::string::npos)
    {
       double location[2], pumpingRate;

       inFile.getline(line, 256);
       line_str = line; 
       str_in.str(line_str); 
       
       str_in >> location[0] >> location[1] >> pumpingRate;

       CWell *well_inst = new CWell(location, pumpingRate, nodes); 
       wells.push_back(well_inst);     
       
       str_in.clear();
    }  // well        
    
    if(line_str.find("domain") != std::string::npos)
    {
      double length[2], resolution[2];
       
      inFile.getline(line, 256);
      line_str = line;                         
      str_in.str(line_str);
   
      str_in >> length[0] >> length[1] >> resolution[0] >> resolution[1];
       
      generateGrid(length, resolution);  
       
      str_in.clear();      
    }  // domain  
        if(line_str.find("times") != std::string::npos)
    {
      int numberOfTimes;
      double timeValue;
              
      inFile.getline(line, 256);
      line_str = line;  
      str_in.str(line_str); 
      str_in >> numberOfTimes;
              
      for (int i=0; i< (int)numberOfTimes; i++)
      {     
        str_in >> timeValue;
        times.push_back(timeValue);
      }              
             
      str_in.clear();             
    }  // times             
  }   
  inFile.close();               
                         
}

void CSimulator::generateGrid(double length[], double resolution[])
{
     
  double x[2];                     
  numberOfNodes[0]= numberOfNodes[1] = 0;   // for Cone++ viewer           
  
  for(x[0] = 0; x[0] <= length[0]; x[0] += resolution[0] ) 
  {
    numberOfNodes[0]++;
    for(x[1] = 0; x[1] <= length[1]; x[1] += resolution[1] ) 
    {
      if (x[0] == 0)
        numberOfNodes[1]++;  
      CNode *node_inst = new CNode(x, wells);           
      nodes.push_back(node_inst);                   
    }
  } 

}


void CSimulator::solveProblem(void)
{
  double factors[2];  
  
  for(int i=0; i< (int)times.size(); i++)
  {     
    for(int ii=0; ii< (int)nodes.size(); ii++)   
      nodes[ii]->resetDrawdown();
      
    for(int j=0; j< (int)wells.size(); j++)     
    {
      factors[0] = wells[j]->getPumpingRate() / ( 4 * M_PI *aquifer.transmissivity);
      factors[1] = aquifer.storativity / ( 4 * aquifer.transmissivity * times[i]);
      
      for(int k=0; k < (int)nodes.size(); k++)  
      { 
         nodes[k]->incrementDrawdown(factors[0] \
          * Wu(factors[1] * distance_squared ( wells[j]->getLocation(), nodes[k]->getLocation())));
      }
    }
    writeResults(times[i]);        
  }       
}
   
void CSimulator::writeResults(double outputTime)
{

  outFile << "time " << outputTime << " - " << nodes.size() << " nodes - " << numberOfNodes[0] << " " << numberOfNodes[1]  << std::endl; 
  
  for (int i=0; i < (int)nodes.size(); i++) 
  {      
      outFile << i << " " << nodes[i]->getLocation()[0] << " " << nodes[i]->getLocation()[1] 
      << " " << nodes[i]->getDrawdown() << std::endl;   
  }
}


void CSimulator::checkInput(void)
{   
  if (verbosity > 0)
  {           
    std::cout << "Aquifer: " <<  aquifer.transmissivity << " " << aquifer.storativity << std::endl;       

    std::cout << wells.size() << " wells: ";  
         
    for (int i=0; i< (int)wells.size(); i++)     
      std::cout << wells[i]->getLocation()[0] << " " << wells[i]->getLocation()[1] << " " <<  wells[i]->getPumpingRate() << std::endl;    
       
    std::cout << times.size() << " times: ";  
    for (int i=0; i< (int)times.size(); i++)     
        std::cout << times[i] << " ";       
    std::cout << std::endl;    
  }
  
  if (verbosity > 1)
  {
    std::cout << nodes.size() << " nodes: ";  
    for (int i=0; i< (int)nodes.size(); i++)
      std::cout << "node: " << nodes[i]->getLocation()[0]  << " " << nodes[i]->getLocation()[1]  << std::endl;                       
  }          
}


double Wu(double u)
{ 
  double Wu = -0.577216 - log(u); // Euler Mascheroni constant
  double sign = -1;
  double multi_u = u;
  double denominator = 1;
  double changeInWu;
  
  if( u > 1)
      std::cout << "WARNING - u = " << u << " " << std::endl;  
  
  for(int i=1; i <= 100; i++) 
  {
    changeInWu = sign * multi_u / denominator;
    Wu += changeInWu;
    
    if (verbosity > 2)
      std::cout << i << " " << changeInWu << " ";   
    
    if( fabs(changeInWu) < 1.e-5)
      break;    
      
    multi_u *=u;        
    sign *= -1;        
    denominator *= (i + 1) * (i + 1) / i; 
  }
  
  if (verbosity > 2)
    std::cout << std::endl;
    
  return Wu;          
}
