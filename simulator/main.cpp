#include "simulator.h"

int main(int argc, char *argv[])
{
    CSimulator *simulator = new CSimulator();
    
    simulator->setupProblem();
    simulator->checkInput();
    simulator->solveProblem();
    
    delete simulator;
    
    system("PAUSE");
    return EXIT_SUCCESS;
}
