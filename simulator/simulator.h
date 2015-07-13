#include <fstream>
#include <vector>
#include "well.h"
#include "node.h"


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
   
   int numberOfNodes[2]; 
    
   properties aquifer;
   
   std::ofstream outFile;

   public:            
   CSimulator(void);
   ~CSimulator(void);
  
   void setupProblem(void);
   void generateGrid(double[], double[]); 
   
   void solveProblem(void);
   void writeResults(double);
   
   void checkInput(void);
};     

double Wu(double);

inline double distance_squared (double x[], double y[])
{
  return ( x[0] - y[0] ) * ( x[0] - y[0] ) + ( x[1] - y[1] ) * ( x[1] - y[1] );     
} 
