
#include "StartManager.h"

#include "vld.h"

using namespace glm;
    
int main(int argc, char* argv[])
{
    StartManager *startManager = new StartManager();
    int result = startManager->start(argc,argv);
    delete startManager;
    return result;
}

