#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <fstream>
#include <vector>
#include "well.h"
#include "node.h"
#include "table.h"
#include "output.h"


struct properties
{
      double transmissivity;
      double storativity;  
};

class CSimulator
{          
   private:                           
   std::vector<CWell*> wells;
   std::vector<CNode*> nodes;
   std::vector<double> times;
   std::vector<CTable*> tables;
   std::vector<COutstream*> outstreams;
    
   properties aquifer;
   
   int numberOfNodes[2];
   
   public:            
   CSimulator(void);
   ~CSimulator(void);
  
   bool setupProblem(void);
   bool generateGrid(double[], double[]); 
   
   void solveProblem(void);
   void writeResults(double);
   
   bool checkInput(void);
   double getPumpingRate(double, int);
};     

double Wu(double);

inline double distance_squared (double x[], double y[])
{
  return ( x[0] - y[0] ) * ( x[0] - y[0] ) + ( x[1] - y[1] ) * ( x[1] - y[1] );     
} 

bool checkDivisibility( double, double);

#endif
