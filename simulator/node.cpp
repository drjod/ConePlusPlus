#include <vector>
#include <iostream>
#include "node.h"
#include "well.h"
#include "simulator.h"


CNode::CNode(double x[], std::vector <CWell*> wells)
{
                    
    
  for (int i=0; i < (int)wells.size(); i++)
    if (distance_squared (location, wells[i]->getLocation()) < 1.e-10 )                  
      std::cout << "WARNING - well " << i << " located at a node" << std::endl;  
                        
  location[0] = x[0];
  location[1] = x[1]; 
  
  drawdown = 0;                   
}

/*
CNode::CNode(double x[])
{                        
  location[0] = x[0];
  location[1] = x[1]; 
  
  drawdown = 0;                   
}*/

CNode::~CNode(void)
{}


double* CNode::getLocation(void)
{
   return &location[0];                   
}

double CNode::getDrawdown(void)
{
   return drawdown;                       
}

void CNode::incrementDrawdown(double value)
{
   drawdown += value;                       
}

void CNode::resetDrawdown(void)
{
   drawdown = 0;                       
}

