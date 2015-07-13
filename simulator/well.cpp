#include <iostream>
#include <vector>
#include "well.h"
#include "node.h"
#include "simulator.h"


CWell::CWell(double x[], double rate, std::vector<CNode*> nodes)
{
  for (int i=0; i < (int)nodes.size(); i++)
    if (distance_squared (location, nodes[i]->getLocation()) < 1.e-10 )                  
      std::cout << "WARNING - node " << i << " located at a well" << std::endl; 
      
  location[0] = x[0];
  location[1] = x[1];

  pumpingRate = rate;
}

/*
CWell::CWell(double x[], double rate)
{    
  location[0] = x[0];
  location[1] = x[1]; 

  pumpingRate = rate;
}*/

CWell::~CWell(void)
{}

double CWell::getPumpingRate(void)
{
  return pumpingRate;       
}

double* CWell::getLocation(void)
{
  return &location[0];        
}


