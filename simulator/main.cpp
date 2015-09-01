#include "simulator.h"

int main(int argc, char *argv[])
{
    CSimulator *simulator = new CSimulator();
    
    if( simulator->setupProblem() == true   // error in input file if one is false
        && simulator->checkInput() == true )
      simulator->solveProblem();
    
    delete simulator;
    
    //system("PAUSE");
    return EXIT_SUCCESS;
}
