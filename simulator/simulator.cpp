#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>
#include <cmath>
#include "simulator.h"

int verbosity = 1;  // 0 - 4

CSimulator::CSimulator(void)
{
 // numberOfNodesX = numberOfNodesY = 0;
}

CSimulator::~CSimulator(void)
{}

bool CSimulator::setupProblem(void)
{   
  std::ifstream inFile;
  char line[256];
  std::string line_str;
  std::stringstream str_in;
    
  inFile.open("pump.in");
  if (!inFile)
  {
    std::cout << "ERROR - could not open file pump.in" << std::endl;
    return false;
  }   
  for(;;)
  {
    line_str.clear();
         
    inFile.getline(line, 256);
    line_str = line;
    
    if(line_str.find("#end") != std::string::npos)                  
      break;
  
    if(line_str.find("#aquifer") != std::string::npos)
    {   
      inFile.getline(line, 256);
      line_str = line;                             
      str_in.str(line_str);    
        
      str_in >> aquifer.transmissivity >> aquifer.storativity;    
       
      line_str.clear();
      str_in.clear();              
    }  // aquifer
    
    if(line_str.find("#well") != std::string::npos)
    {
       double location[2], pumpingRate;
       int tableNumber = -1;
       std::string tableName;       
       inFile.getline(line, 256);
       line_str = line; 
       str_in.str(line_str); 
       
       str_in >> location[0] >> location[1] >> pumpingRate >> tableName; // line_str modified
       
       CWell *well_inst = new CWell(location, pumpingRate, nodes, tableName); 
       wells.push_back(well_inst);     
       
       str_in.clear();
    }  // well        
    
    if(line_str.find("#domain") != std::string::npos)
    {
      double length[2], resolution[2];
       
      inFile.getline(line, 256);
      line_str = line;                         
      str_in.str(line_str);
   
      str_in >> length[0] >> length[1] >> resolution[0] >> resolution[1];
       
      if (generateGrid(length, resolution) == false)
        return false;  
       
      str_in.clear();      
    }  // domain  
    
    if(line_str.find("#timing") != std::string::npos)
    {
      int numberOfTimes;
      double timeValue;
                  
      if(line_str.find("values") != std::string::npos) 
      {
             
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
      } // values
      
      if(line_str.find("intervals") != std::string::npos) 
      {       
        double end;
        int numberOfIntervals;
        
        inFile.getline(line, 256);
        line_str = line;                    
        str_in.str(line_str); 
        str_in >> numberOfIntervals >> end;
              
        for (int i=1; i<= (int)numberOfIntervals; i++)
        {     
          timeValue = i * end / numberOfIntervals;
          times.push_back(timeValue);
           
        }               
             
        str_in.clear();            
      } // intervals
    }  // times     
    if(line_str.find("#initialization") != std::string::npos)
    {
      double value;
      inFile.getline(line, 256);
      line_str = line;                    
      str_in.str(line_str); 
      
      for (int i=0; i < (int)nodes.size(); i++)
      {
        str_in >> value;
        nodes[i]->incrementDrawdown(value);   
      }                              
      str_in.clear();       
    } // initialization    
    if(line_str.find("#table") != std::string::npos)
    {
      std::string tableName; 
      int size;
      
      inFile.getline(line, 256);
      line_str = line;                    
      str_in.str(line_str); 
      str_in >> tableName;
      str_in.clear();  
      
      inFile.getline(line, 256);
      line_str = line;                    
      str_in.str(line_str); 
      str_in >> size;
      str_in.clear();  

      double* times = new double[size];
      double* values = new double[size];
      
      for (int i=0; i < size; i++)
      {
        inFile.getline(line, 256);
        line_str = line;                    
        str_in.str(line_str);
        
        str_in >> times[i] >> values[i];
        str_in.clear();  
      }
      CTable *table_inst = new CTable(tableName, times, values, size);
      tables.push_back(table_inst);
    } // table
    if(line_str.find("#output") != std::string::npos)
    {
       int outputDistributionType, outputTimingType, size, instance;
       std::vector<double> outputTimes;
       std::string outputFileName;
       std::vector<location> outputNodes;
       
       inFile.getline(line, 256);
       line_str = line;  
       str_in.str(line_str); 
       str_in >> outputFileName;
       
       line_str.clear();
       str_in.clear();
       
       inFile.getline(line, 256);
       line_str = line;  
       
       
       if(line_str.find("domain") != std::string::npos)
       {
         outputDistributionType = 0;  
       } 
       else if(line_str.find("selected") != std::string::npos && line_str.find("nodes") != std::string::npos)
       {
         line_str.clear();
         location location_inst;  
         outputNodes.clear();
         outputDistributionType = 1;
         inFile.getline(line, 256);
         line_str = line;                    
         str_in.str(line_str); 
         str_in >> size;

         for (int i=0; i < size; i++)
         {
           str_in.clear();
           inFile.getline(line, 256);
           line_str = line;                    
           str_in.str(line_str);
           str_in >> location_inst.x[0] >> location_inst.x[1] ;
          
           outputNodes.push_back(location_inst); 
         }
         str_in.clear();
                     
       }
       else
       {
         std::cout << "ERROR - outstream node type not supported" << std::endl; 
         return false;    
       }
       
       line_str.clear();
       
       inFile.getline(line, 256);
       line_str = line;  
       
       if(line_str.find("times") != std::string::npos)
       {
         if(line_str.find("all") != std::string::npos)
         {                   
           outputTimingType = 0;  
         } 
         else if(line_str.find("selected") != std::string::npos)
         {
           line_str.clear();
           outputTimingType = 1;
         
           inFile.getline(line, 256);
           line_str = line;                    
           str_in.str(line_str); 
           str_in >> size;
      
           for (int i=0; i < size; i++)
           {
             str_in >> instance;
             outputTimes.push_back(instance); 
           }
           str_in.clear();
         }              
       }
       else
       {
         std::cout << "ERROR - outstream time type not supported" << std::endl; 
         return false;    
       }
       //std::cout << "pass" <<outputTimes.size()<< std::endl;
       COutstream *outstream_inst = new COutstream(outputFileName, outputDistributionType, outputTimingType, outputNodes, outputTimes);
       outstreams.push_back(outstream_inst);            
    }  // outstream 
    
  }  // for 
  inFile.close();  
  return true;                                
}

bool CSimulator::generateGrid(double length[], double resolution[])
{
  double x[2];                    
  numberOfNodes[0]= numberOfNodes[1] = 0;
       
  if ( checkDivisibility(length[0], resolution[0]) == false     // type double ensured by GUI
         || checkDivisibility(length[1], resolution[1]) == false )
  {
    std::cout << "ERROR - domain length divided by resolution is not a whole number";     
    return false; 
  }
  else
  {                
    for(x[1] = 0.5 * resolution[1]; x[1] <= length[1]; x[1] += resolution[1] )
    {
      numberOfNodes[1]++;
      for(x[0] = 0.5 * resolution[0]; x[0] <= length[0]; x[0] += resolution[0] )
      {
        if (x[1] == 0.5 * resolution[1])
           numberOfNodes[0]++;     
        CNode *node_inst = new CNode(x, wells);          
        nodes.push_back(node_inst);                  
      }
    }
  }
  return true;
}


void CSimulator::solveProblem(void)
{     
  double factors[2], oldTime = 0, u;  
  
  writeResults(0.); // write IC
  
  if (verbosity > 0)
      std::cout << "  Calculating" << std::endl;
      
  for(int i=0; i< (int)times.size(); i++)
  {     
    if (verbosity > 0)
      std::cout << "    time step " <<  i << " - " << times[i] << std::endl;
    for(int j=0; j< (int)wells.size(); j++)     
    {
      if (verbosity > 3)
      {
        std::cout << "      well " << j << " - node - u - Wu updates" <<  std::endl;
      }
      factors[0] = getPumpingRate(times[i], j) / ( 4 * M_PI *aquifer.transmissivity);
      factors[1] = aquifer.storativity / ( 4 * aquifer.transmissivity * (times[i]- oldTime));
      
      for(int k=0; k < (int)nodes.size(); k++)  
      { 
        u = factors[1] * distance_squared ( wells[j]->getLocation(), nodes[k]->getLocation());
        if (verbosity > 3)
        {
          std::cout << "        " << k << "  " << u << "    ";
        }
        nodes[k]->incrementDrawdown(factors[0] * Wu(u));
      }
    }
    writeResults(times[i]);     
    oldTime = times[i];   
  }       
  
  std::cout << "+++++++++++++++++++++++++++++++++++++++++++++"  << std::endl;  

}
   
void CSimulator::writeResults(double outstreamTime)
{
  for (int i=0; i < (int)outstreams.size(); i++)
  {
    outstreams[i]->writePotentially(outstreamTime, numberOfNodes, nodes);
  }
}


bool CSimulator::checkInput(void)
{   
  if (verbosity > 0)
  {           
    std::cout << "  Cone++ simulation"  << std::endl; 
    std::cout << "+++++++++++++++++++++++++++++++++++++++++++++"  << std::endl; 
    std::cout << "  aquifer - transmissivity " <<  aquifer.transmissivity << " - storativity " << aquifer.storativity << std::endl;       

    std::cout << "  " << wells.size() << " wells" << std::endl;         
    for (int i=0; i< (int)wells.size(); i++) 
    {    
      std::cout << "    (x,y) "  << wells[i]->getLocation()[0] << " " << wells[i]->getLocation()[1] << " - rate " <<  wells[i]->getPumpingRate(); 
      std::cout << " - table " <<  wells[i]->getTableName() << std::endl;    
    }   
    std::cout << "  " << times.size() << " simulation times" << std::endl;  
    for (int i=0; i< (int)times.size(); i++)     
        std::cout << "    "  << times[i] << " ";       
    std::cout << std::endl;    
    
    std::cout << "  " << tables.size() << " tables - ";  
    for (int i=0; i<(int)tables.size(); i++)
    {
      std::cout << tables[i]->getName() << " "; 
    }
    std::cout << std::endl;  
    
    if (verbosity > 1)
    {
      for (int i=0; i<(int)tables.size(); i++)
      {
        std::cout << "    content " <<  tables[i]->getName() << std::endl; 
        for (int j=0; j<(int)times.size(); j++)     
          std::cout << "      " <<  tables[i]->getTimes()[j] << " " << tables[i]->getValues()[j] << std::endl;          
      }
    }  
    
    std::cout << "  " <<  nodes.size() << " nodes " << std::endl;   
    if (verbosity > 2)
    {
      for (int i=0; i< (int)nodes.size(); i++)
        std::cout << "    " <<  "node: " << nodes[i]->getLocation()[0]  << " " << nodes[i]->getLocation()[1]  << " " << nodes[i]->getDrawdown()  << std::endl; 
    }      
    
    std::cout << "  " <<  outstreams.size() << " outputs" << std::endl;  
    for(int i=0; i< outstreams.size(); i++)
    {
      if(outstreams[i]->getDistributionType() == 0)
        std::cout << "    snaphot file " << outstreams[i]->getFileName(); 
      else if(outstreams[i]->getDistributionType() == 1)
        std::cout << "    time series file " << outstreams[i]->getFileName();
      else  
      {
        std::cout << "ERROR - distribution type not supported" << std::endl;
        return false;
      } 
      if(outstreams[i]->getTimingType() == 0)
        std::cout << " for all times" << std::endl; 
      else if(outstreams[i]->getTimingType() == 1) 
      {
        std::cout << " for selected times" << std::endl;  
      }
      else  
      {
        std::cout << "ERROR - timing type not supported" << std::endl; 
        return false;
      }
    }
    std::cout << "+++++++++++++++++++++++++++++++++++++++++++++"  << std::endl;  
  }  
  // exceptions
  if( aquifer.storativity <= 0 ||  aquifer.transmissivity <= 0)  // type double ensured by GUI 
  {
        std::cout << "ERROR - aquifer parameter must be positive" << std::endl;              
        return false;
  }
  
  return true;
}

double CSimulator::getPumpingRate(double currentTime, int wellNumber)
{
  double rate;
  int tableNdx = -1;
  int timeNdx = -1;
  
  rate = wells[wellNumber]->getPumpingRate();
  
  if ( wells[wellNumber]->getTableName() != "No")
  {
    for(int i=0; i < tables.size();i++)
    {
      if(wells[wellNumber]->getTableName() == tables[i]->getName() )
        tableNdx = i;
    }
    
    if ( tableNdx == -1)
      std::cout << "WARNING - Table " << wells[wellNumber]->getTableName() << " not found - Pumping rate not modified"  << std::endl; 
    else
    {
      for(int j=0; j < tables[tableNdx]->getNumberOfEntries();j++){
        if (tables[tableNdx]->getTimes()[j] == currentTime )
          timeNdx = j;
      }
      if( timeNdx == -1)
        std::cout << "WARNING - Table " + wells[wellNumber]->getTableName() << " has no entry for time " << currentTime << " - Pumping rate not modified"  << std::endl; 
      else
        rate *= tables[tableNdx]->getValues()[timeNdx];
    }
  }
  return rate;
}

double Wu(double u)
{ 
  double Wu = -0.577216 - log(u); // Euler Mascheroni constant
  double sign = -1;
  double multi_u = u;
  double denominator = 1;
  double changeInWu;
  
  if( u > 1 && verbosity > 0)
      std::cout << "WARNING - u = " << u << std::endl;  
  
  for(int i=1; i <= 100; i++) 
  {
    changeInWu = sign * multi_u / denominator;
    Wu += changeInWu;
    
    if (verbosity > 3)
      std::cout << "  " << changeInWu << " ";   
    
    if( fabs(changeInWu) < 1.e-5)
      break;    
      
    multi_u *=u;        
    sign *= -1;        
    denominator *= (i + 1) * (i + 1) / i; 
  }
  
  if (verbosity > 3)
    std::cout << std::endl;
    
  return Wu;          
}

/////////////////////////////////////////////////////////////////////////////
/// checkDivisibility - if one number can be evenly divided by another
/// parameter:
/// nominator
/// denominator
/// return true if one number (nominator) divided by other number (denominator) is
///  (approximatelly) a whole number
///
bool checkDivisibility( double nominator, double denominator)
{
  double test = nominator / denominator;

  if (fabs(trunc(test) - test) < 1e-10)
    return true;

  return false;
}
