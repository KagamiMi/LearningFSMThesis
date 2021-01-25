#include "GeneticManagerSimpleFSM.h"
#include <iomanip>


GeneticManagerSimpleFSM::GeneticManagerSimpleFSM(SeedValues seedValues, ExperimentConfiguration expConfg, bool windowVisibility,
    FitnessValues fitnessValues): fitnessValues(fitnessValues), genState(seedValues.stateSeed), 
distrState(0, expConfg.numStates - 1), genAction(seedValues.actionSeed), distrAction(0,numActions-1), genParents(seedValues.parentsSeed),
distrParents(0,expConfg.generationSize-1), genParentsSel(seedValues.parentsSelSeed), distrParentsSel(0,100),
genAlleleSel(seedValues.alleleSelSeed), distrAlleleSel(0,1), genMutation(seedValues.mutationSeed), distrMutation(0,1),
genMutStateOrAction(seedValues.mutStateOrAction), distrMutStateOrAction(0,1), seedValues(seedValues), expConfg(expConfg), mapPath(expConfg.mapFilename), numStates(expConfg.numStates),
windowVisibility(windowVisibility)
{
    fsms.reserve(expConfg.generationSize);
}

struct comp {
    bool operator() (SimpleFSM* fsm1, SimpleFSM* fsm2) { return fsm1->getFitness() > fsm2->getFitness(); }
} myComp;

void GeneticManagerSimpleFSM::createNewPopulation(std::vector<SimpleFSM*>* children)
{
    std::sort(fsms.begin(), fsms.end(), myComp);
    int parentsNum = fsms.size() - children->size();

    int childrenIndex = 0;

    /*for (int i = 0; i < parentsNum; i++)
    {
        fsms[i]->reset();
    }*/

    for (int i = parentsNum; i < fsms.size(); i++)
    {
        SimpleFSM* temp = fsms[i];
        fsms[i] = (*children)[childrenIndex]; //dzieci te¿ dynamicznie alokowaæ?
        delete temp;
        childrenIndex++;
    }
}

void GeneticManagerSimpleFSM::mutateChildren(std::vector<SimpleFSM*>* children, float mutProb)
{
    for (int i = 0; i < children->size(); i++) //dziecko
    {
        //std::vector<State<SimpleAction>> states = (*children)[i].getStatesVector();
        for (int j = 0; j < numStates; j++) //stan
        {

            for (int k = 0; k < numInputs; k++) //konkretna komórka
            {
                if (distrMutation(genMutation) <= mutProb) //tylko zmieniamy gdy mamy mutacjê
                {
                    if (distrMutStateOrAction(genMutStateOrAction) == 0)
                    {
                        (*children)[i]->setNewState(distrState(genState), j, k);
                        //state
                    }
                    else //mo¿e byæ tylko 1 
                    {
                        //action
                        (*children)[i]->setNewAction((SimpleAction)distrAction(genAction), j, k);
                    }
                }
            }
        }
    }
}

//numParents musi byæ parzysty
std::vector<SimpleFSM*> GeneticManagerSimpleFSM::getChildrenFromRecombination(int numParents, int numStates, int numInputs, int parentProb, std::vector<SimpleFSM*> parents)
{
    std::vector<SimpleFSM*> children;
    children.reserve(numParents);

    for (int i = 0; i < numParents; i = i + 2)
    {
        int drawn = distrParentsSel(genParentsSel); 
        if (drawn <= parentProb)
        {
            SimpleFSM* child1 = new SimpleFSM(numStates);
            SimpleFSM* child2 = new SimpleFSM(numStates);

            std::vector<State<SimpleAction>> states1;
            std::vector<State<SimpleAction>> states2;
            states1.reserve(numStates);
            states2.reserve(numStates);


            std::vector<State<SimpleAction>> parentStates1 = (*parents[i]).getStatesVector();
            std::vector<State<SimpleAction>> parentStates2 = (*parents[i + 1]).getStatesVector();


            for (int j = 0; j < numStates; j++)
            {
                states1.push_back(State<SimpleAction>(numInputs));
                states2.push_back(State<SimpleAction>(numInputs));

                std::vector<std::tuple<int, SimpleAction>> transitions1;
                std::vector<std::tuple<int, SimpleAction>> transitions2;
                transitions1.reserve(numInputs);
                transitions2.reserve(numInputs);


                std::vector<std::tuple<int, SimpleAction>> parentTransitions1 = parentStates1[j].getTransitions();
                std::vector<std::tuple<int, SimpleAction>> parentTransitions2 = parentStates2[j].getTransitions();

                for (int k = 0; k < numInputs; k++)
                {
                    int temp = distrAlleleSel(genAlleleSel); //tu mo¿emy dostaæ tylko 0 lub 1

                    if (temp == 0)
                    {
                        transitions1.push_back(parentTransitions2[k]);
                        transitions2.push_back(parentTransitions1[k]);
                    }
                    else if (temp == 1)
                    {
                        transitions1.push_back(parentTransitions1[k]);
                        transitions2.push_back(parentTransitions2[k]);
                    }
                }
                states1[j].setTransitions(transitions1);
                states2[j].setTransitions(transitions2);
            }
            child1->setStates(states1);
            child2->setStates(states2);

            children.push_back(child1);
            children.push_back(child2);
        } 
        else
        {
            children.push_back(new SimpleFSM(*parents[i])); //wa¿ne - czy tu na pewno jest robiona kopia a nie korzystamy z tego samego obiektu
            children.push_back(new SimpleFSM(*parents[i + 1]));
        }
    }
    return children;
}

bool GeneticManagerSimpleFSM::fitnessComparison(const SimpleFSM* fsm1,const SimpleFSM* fsm2)
{
    return fsm1->getFitness() < fsm2->getFitness();
}

SimpleFSM* GeneticManagerSimpleFSM::getMaxFromTournament(std::vector<SimpleFSM*>* tournamentGroup)
{
    SimpleFSM* max = (*tournamentGroup)[0];
    if (tournamentGroup->size() > 1)
    {
        for (int i = 1; i < tournamentGroup->size(); i++)
        {
            if (max->getFitness() < (*tournamentGroup)[i]->getFitness())
            {
                max = (*tournamentGroup)[i];
            }
        }
        
    }
    return max;
}

std::vector<SimpleFSM*> GeneticManagerSimpleFSM::getParentsTournamentSelection(int numParents, int tournamentSize, std::vector<SimpleFSM*> individuals)
{
    std::vector<SimpleFSM*> parents;
    parents.reserve(numParents);

    std::vector<SimpleFSM*> tournamentGroup;
    tournamentGroup.reserve(tournamentSize);

    for (int i = 0; i < numParents; i++)
    {
        for (int j = 0; j < tournamentSize; j++)
        {
            int index = distrParents(genParents);
            tournamentGroup.push_back(individuals[index]);
            //potrzebujemy tu generatora losowego
        }
        parents.push_back(getMaxFromTournament(&tournamentGroup));
        tournamentGroup.clear();
    }

    return parents;
}

float GeneticManagerSimpleFSM::getBasicFitness(Record record, int maxPoints, int maxEnemies, int maxSteps, float maxXDistance, float maxYDistance)
{
    return fitnessValues.recordFactor * ((float)record.getPoints() / (float)maxPoints)
        + fitnessValues.enemiesFactor * ((float)record.getEnemies() / (float)maxEnemies)
        + fitnessValues.healthFactor * ((float)record.getHealth() / 100.0f)
        + fitnessValues.finishedFactor * (float)record.getFinished()
        + fitnessValues.timeFactor * ((maxSteps - record.getSteps()) / maxSteps) * (float)record.getFinished()
        + fitnessValues.distanceXFactor * ((maxXDistance-record.getDistanceX())/maxXDistance)
        + fitnessValues.distanceYFactor * ((maxYDistance-record.getDistanceY())/maxYDistance);
}


void GeneticManagerSimpleFSM::runOneWithGenetic(std::string fsmFilename)
{
    fsms.push_back(new SimpleFSM(expConfg.numStates));
    fsms[0]->loadFromFile(fsmFilename);

    AgentManager* agentManager;
    OpenGLManager* openGLManager;

    bool* rightButtonPressed = new bool(false);
    bool* leftButtonPressed = new bool(false);
    bool* spacePressed = new bool(false);

    agentManager = new AgentManager(expConfg.basicMoveChange);
    openGLManager = new OpenGLManager(mapPath, expConfg.basicMoveChange, agentManager,windowVisibility);

    time_t timer;
    tm timeinfo;
    time(&timer);
    localtime_s(&timeinfo, &timer);

    std::string directory = createDirectory(timeinfo);
    std::string filename = directory + "\\seedsAndParameters.txt";
    std::string fitnessAndRecordFilename = directory + "\\fitnessAndRecord.csv";
    saveSeedsAndParameters(filename);

    openGLManager->initialization();
    float maxXDistance = openGLManager->getMapWidth();
    float maxYDistance = openGLManager->getMapHeight();

    std::vector<int> stateCounting(expConfg.numStates, 0);
    //stateCounting[fsms[i]->getCurrentState()]++;
    fsms[0]->setInitialInput(openGLManager->getCurrentInputs());
    stateCounting[0]++; //bo 0 jest zawsze stanem pocz¹tkowym

    std::string logFilename = directory + "\\logFSM.csv";
    std::string logPosFilename = directory + "\\logPosFSM.csv";

    //std::ofstream log(logFilename, std::ios_base::app);
    //log << "Input\tState\tAction\n";
    //log.close();

    float* lastTime = new float();
    *lastTime = glfwGetTime();
    int stepsCounter = 0;

    //fsms[0]->logCurrentState(filename);
    while (openGLManager->stillRunning() && agentManager->stillRunning() && stepsCounter < expConfg.maxSteps /*&& !fsms[i]->isSameInputOver(1000)*/)
    {
        fsms[0]->updateState(openGLManager->getCurrentInputs(), logFilename,stepsCounter);
        fsms[0]->executeAction(rightButtonPressed, leftButtonPressed, spacePressed);
        openGLManager->updateForLoop(rightButtonPressed, leftButtonPressed, spacePressed, logPosFilename, stepsCounter);
        stateCounting[fsms[0]->getCurrentState()]++;
        stepsCounter++;
    }
    glfwTerminate();
    
    //zapisanie wyniku
    Record record = agentManager->getRecord(stepsCounter, openGLManager->getDistanceX(), openGLManager->getDistanceY());
    fsms[0]->setFitness(getBasicFitness(record, openGLManager->getPointsNum(), openGLManager->getEnemyNum(), expConfg.maxSteps, openGLManager->getMapWidth(), openGLManager->getMapHeight()));

    saveResultsForSingle(fitnessAndRecordFilename, directory + "\\FSM.csv", directory + "\\statesUsage.csv",
        record, stateCounting);


    delete fsms[0];
    delete agentManager;
    delete openGLManager;
    delete lastTime;
    delete rightButtonPressed;
    delete leftButtonPressed;
    delete spacePressed;
}

void GeneticManagerSimpleFSM::runExperimentWithGenetic()
{
    //std::vector<SimpleFSM*> fsms;
    

    for (int i = 0; i < expConfg.generationSize; i++)
    {
        fsms.push_back(new SimpleFSM(expConfg.numStates)); //bez initialinput - trzeba potem
        fsms[i]->setStates(&distrState, &distrAction, &genState, &genAction);
    }

    AgentManager* agentManager;
    OpenGLManager* openGLManager;

    clock_t timeStart, timeEnd;
    float timeSec;

    float* lastTime = new float();

    bool* rightButtonPressed = new bool(false);
    bool* leftButtonPressed = new bool(false);
    bool* spacePressed = new bool(false);

    //agentManager = new AgentManager(expConfg.basicMoveChange);
    //openGLManager = new OpenGLManager(mapPath, expConfg.basicMoveChange, agentManager,windowVisibility);

    time_t timer;
    tm timeinfo;
    time(&timer);
    localtime_s(&timeinfo, &timer);

    std::string directory = createDirectory(timeinfo);
    std::string filename = directory+"\\seedsAndParameters.txt";
    std::string csvFilename = directory+"\\fitnessFunctionSummary.csv";
    std::string csvFilenameId = directory + "\\fitnessFunctionSummaryId.csv";
    saveResultsForExperiment(filename,csvFilename,csvFilenameId);

    for (int j = 0; j < expConfg.iterations; j++)
    {
        std::string iterationDirectory = createDirectoryForIteration(j, directory);

        
        /*int startNumber = expConfg.numParentsToCrossOver;
        if (j == 0)
        {
            startNumber = 0;
        }*/
        std::string iterationResults = iterationDirectory + "\\records.csv";
        std::ofstream records(iterationResults, std::ios_base::app);
        records << "sep= \n";
        records << "\tpoints\thealth\tenemies\tfinished\tsteps\tdistance x\tdistance y\n";
        records.close();

        std::string stateResults = iterationDirectory + "\\statesUsage.csv";
        std::ofstream states(stateResults, std::ios_base::app);    
        states << "sep= \n";
        states << "FSM\\state\t";
        for (int i = 0; i < expConfg.numStates; i++)
        {
            states << i << "\t";
        }
        states << "\n";
            
        states.close();

        for (int i = 0; i < expConfg.generationSize; i++)
        {
            if (fsms[i]->getFitness() != 0)
            {
                //int id = fsms[i]->getId();
                //std::string previousDir = directory+ "\\iteration" + std::to_string(j-1);
                //std::filesystem::directory_iterator dirIter = std::filesystem::directory_iterator(previousDir);
                //for (dirIter = std::filesystem::directory_iterator(previousDir); dirIter != std::filesystem::end(dirIter); dirIter++)
                //{
                //    std::string tempFilename = (*dirIter).path().filename().string();
                //    if ('F' == tempFilename.at(0))
                //    {
                //        int underline = tempFilename.find('_');
                //        int dot = tempFilename.find('.');
                //        std::string strIt = "";
                //        std::string strId = "";

                //        for (int i = 3; i < underline; i++)
                //        {
                //            strIt += tempFilename.at(i);
                //        }
                //        for (int i = underline + 1; i < dot; i++)
                //        {
                //            strId += tempFilename.at(i);
                //        }

                //        if (id == stoi(strId)) //ten plik
                //        {
                //            int fsmNumber = stoi(strIt);
                //            std::filesystem::copy(previousDir+"\\"+tempFilename,directory+"\\FSM"+std::to_string(i)+"_"+std::to_string(id));
                //            std::filesystem::copy(,);
                //            break;
                //        }
                //        
                //    }

                //}

                //znalezc pliki z poprzedniej iteracji
                //znaleŸæ prawid³owe id
                //stworzyæ nowe pliki 
                //skopiowaæ
                continue;
            }

            

            timeStart = clock();
            *lastTime = glfwGetTime();

            agentManager = new AgentManager(expConfg.basicMoveChange);
            openGLManager = new OpenGLManager(mapPath, expConfg.basicMoveChange, agentManager,windowVisibility);
            openGLManager->initialization();
            float maxXDistance = openGLManager->getMapWidth();
            float maxYDistance = openGLManager->getMapHeight();

            std::vector<int> stateCounting(expConfg.numStates,0);
            //stateCounting[fsms[i]->getCurrentState()]++;
            fsms[i]->setInitialInput(openGLManager->getCurrentInputs());
            stateCounting[0]++; //bo 0 jest zawsze stanem pocz¹tkowym

            std::string logFilename = iterationDirectory + "\\logFSM" + std::to_string(i) + ".csv";
            /*std::ofstream log(logFilename, std::ios_base::app);
            log << "Step\tInput\tState\tAction\n";
            log.close();*/

            std::string logPosFilename = iterationDirectory + "\\logPosFSM" + std::to_string(i) + ".csv";

            int stepsCounter = 0;

            //fsms[i]->logCurrentState(filename);
            while (openGLManager->stillRunning() && agentManager->stillRunning() && stepsCounter < expConfg.maxSteps /*&& !fsms[i]->isSameInputOver(1000)*/)
            {
                fsms[i]->updateState(openGLManager->getCurrentInputs(), logFilename, stepsCounter);
                fsms[i]->executeAction(rightButtonPressed, leftButtonPressed, spacePressed);
                openGLManager->updateForLoop(rightButtonPressed, leftButtonPressed, spacePressed, logPosFilename, stepsCounter);
                stateCounting[fsms[i]->getCurrentState()]++;
                stepsCounter++;
            }

            glfwTerminate();
            //zapisanie wyniku
            Record record = agentManager->getRecord(stepsCounter, openGLManager->getDistanceX(), openGLManager->getDistanceY());
            fsms[i]->setFitness(getBasicFitness(record, openGLManager->getPointsNum(), openGLManager->getEnemyNum(), expConfg.maxSteps,openGLManager->getMapWidth(),openGLManager->getMapHeight()));

            /*if (j == 33 && i == 35)
            {
                std::cout << "recordFactor " << fitnessValues.recordFactor << " punkty: " << record.getPoints() << " max: " << openGLManager->getPointsNum() << std::endl;
                std::cout << "enemiesFactor " << fitnessValues.enemiesFactor << " wrogowie: " << record.getEnemies() << " max: " << openGLManager->getEnemyNum() << std::endl;
                std::cout << "healthFactor " << fitnessValues.healthFactor << " zdrowie: " << record.getHealth() << std::endl;
                std::cout << "distanceXFactor " << fitnessValues.distanceXFactor << " dystans x " << record.getDistanceX() << " max: " << openGLManager->getMapWidth() << std::endl;
                std::cout << "distanceYFactor " << fitnessValues.distanceYFactor << " dystans y " << record.getDistanceY() << " max: " << openGLManager->getMapHeight() << std::endl;

            }*/

            

            saveResultsForFSM(iterationResults,iterationDirectory+"\\FSM"+std::to_string(i)+"_"+std::to_string(fsms[i]->getId())+".csv", stateResults, 
                record,i, stateCounting);

            delete openGLManager;
            delete agentManager;
        }
        saveResultsAfterIteration(csvFilename, csvFilenameId,j);

        /*if (getMaxFitnessValue() > 0.95)
        {
            return;
        }*/

        //selekcja turniejowa
        std::vector<SimpleFSM*> parents = getParentsTournamentSelection(expConfg.numParentsToCrossOver, expConfg.tournamentSize, fsms);

        //krzy¿owanie
        std::vector<SimpleFSM*> children = getChildrenFromRecombination(parents.size(), numStates, pow(2, 11), expConfg.parentsRecombProb, parents);

        //mutowanie
        mutateChildren(&children, expConfg.mutationProb);

        //tworzenie nowej populacji
        createNewPopulation(&children);
    }



    for (int i = 0; i < expConfg.generationSize; i++)
    {
        delete fsms[i];
    }
    //delete agentManager;
    //delete openGLManager;
    delete lastTime;
    delete rightButtonPressed;
    delete leftButtonPressed;
    delete spacePressed;
}

//float GeneticManagerSimpleFSM::getMaxFitnessValue()
//{
//    float maxValue = fsms[0]->getFitness();
//    for (int i = 1; i < fsms.size(); i++)
//    {
//        if (fsms[i]->getFitness() > maxValue)
//        {
//            maxValue = fsms[i]->getFitness();
//        }
//    }
//    return maxValue;
//}

std::string GeneticManagerSimpleFSM::createDirectoryForIteration(int iteration, std::string folderAbove)
{
    std::string dirName = folderAbove + "\\iteration" + std::to_string(iteration);
    int result = _mkdir(dirName.c_str());
    if (result == 0)
    {
        return dirName;
    }
    else
    {
        return folderAbove;
    }
}

std::string GeneticManagerSimpleFSM::createDirectory(tm timeinfo)
{
    std::ostringstream oss;
    oss << "FSMdata_" << std::setfill('0')
        << std::setw(4) << std::to_string(timeinfo.tm_year + 1900) << "-"
        << std::setw(2) << std::to_string(timeinfo.tm_mon + 1) << "-"
        << std::setw(2) << std::to_string(timeinfo.tm_mday) << "_"
        << std::setw(2) << std::to_string(timeinfo.tm_hour) << "-"
        << std::setw(2) << std::to_string(timeinfo.tm_min) << "-"
        << std::setw(2) << std::to_string(timeinfo.tm_sec);
        //<< ".txt";

    std::string directory =  oss.str();
    int result = _mkdir(directory.c_str());
    if (result == 0)
    {
        return directory;
    }
    else
    {
        return "";
    }
}

//std::string GeneticManagerSimpleFSM::getFilename(tm timeinfo)
//{
//    /*time_t timer;
//    tm timeinfo;
//    time(&timer);
//    localtime_s(&timeinfo, &timer);*/
//
//    std::ostringstream oss;
//    oss << "FSMdata_" << std::setfill('0')
//        << std::setw(4) << std::to_string(timeinfo.tm_year + 1900) << "-"
//        << std::setw(2) << std::to_string(timeinfo.tm_mon + 1) << "-"
//        << std::setw(2) << std::to_string(timeinfo.tm_mday) << "_"
//        << std::setw(2) << std::to_string(timeinfo.tm_hour) << "-"
//        << std::setw(2) << std::to_string(timeinfo.tm_min) /*<< "-"
//        << std::setw(2) << std::to_string(timeinfo.tm_sec) << "_"*/
//         << ".txt";
//
//    return oss.str();
//}

//std::string GeneticManagerSimpleFSM::getFilenameCSV(tm timeinfo)
//{
//    /*time_t timer;
//    tm timeinfo;
//    time(&timer);
//    localtime_s(&timeinfo, &timer);*/
//
//    std::ostringstream oss;
//    oss << "FSM_fitness_" << std::setfill('0')
//        << std::setw(4) << std::to_string(timeinfo.tm_year + 1900) << "-"
//        << std::setw(2) << std::to_string(timeinfo.tm_mon + 1) << "-"
//        << std::setw(2) << std::to_string(timeinfo.tm_mday) << "_"
//        << std::setw(2) << std::to_string(timeinfo.tm_hour) << "-"
//        << std::setw(2) << std::to_string(timeinfo.tm_min) /*<< "-"
//        << std::setw(2) << std::to_string(timeinfo.tm_sec) << "_"*/
//        << ".csv";
//
//    return oss.str();
//
//}


void GeneticManagerSimpleFSM::saveSeedsAndParameters(std::string filename)
{
    std::ofstream results(filename, std::ios_base::app);

    results << "---\nSEEDS\n---\n"
        << "Initial seed: " << seedValues.initialSeed << "\n"
        << "State seed: " << seedValues.stateSeed << "\n"
        << "Action seed: " << seedValues.actionSeed << "\n"
        << "Parents seed: " << seedValues.parentsSeed << "\n"
        << "Parents selection seed: " << seedValues.parentsSelSeed << "\n"
        << "Allele selection seed: " << seedValues.alleleSelSeed << "\n"
        << "Mutation seed: " << seedValues.mutationSeed << "\n"
        << "Mutate state or action seed: " << seedValues.mutStateOrAction << "\n"
        << "---\nPARAMETERS\n---\n"
        << "Map: " << mapPath << "\n"
        << "Number of states: " << numStates << "\n"
        << "Generation size: " << expConfg.generationSize << "\n"
        << "Number of iterations: " << expConfg.iterations << "\n"
        << "Parents recombination probability: " << expConfg.parentsRecombProb << "%\n"
        << "Number of parents selected for crossover: " << expConfg.numParentsToCrossOver << "\n"
        << "Tournament size while selecting parents: " << expConfg.tournamentSize << "\n"
        << "Basic move change: " << expConfg.basicMoveChange << "\n"
        << "Maximum FSM steps: " << expConfg.maxSteps << "\n"
        << "Childrem mutation probability: " << expConfg.mutationProb << "\n"
        << "---\nFITNESS PERCENTAGES\n---\n"
        << "Points factor: " << fitnessValues.recordFactor << "\n"
        << "Enemies factor: " << fitnessValues.enemiesFactor << "\n"
        << "Health factor: " << fitnessValues.healthFactor << "\n"
        << "Finished factor: " << fitnessValues.finishedFactor << "\n"
        << "Time factor: " << fitnessValues.timeFactor << "\n"
        << "X distance factor: " << fitnessValues.distanceXFactor << "\n"
        << "Y distance factor: " << fitnessValues.distanceYFactor << "\n";

    results.close();
}

void GeneticManagerSimpleFSM::saveResultsForExperiment(std::string fileName, std::string csvFilename, std::string csvFilenameId)
{
    saveSeedsAndParameters(fileName);

    std::ofstream resultsCsv(csvFilename, std::ios_base::app);
    std::ofstream resultsCsvId(csvFilenameId, std::ios_base::app);

    resultsCsv << "sep= \n";
    resultsCsvId << "sep= \n";

    resultsCsv << "Iteration\\FSM" << "\t";
    resultsCsvId << "Iteration\\FSM" << "\t";
    for (int i = 0; i < expConfg.generationSize; i++)
    {
        resultsCsv << i << "\t";
        resultsCsvId << i << "\t";
    }
    resultsCsv << "MAX" << "\t";
    resultsCsv << "AVERAGE" << "\t";

    resultsCsv << "\n";
    resultsCsvId << "\n";

    resultsCsv.close();
    resultsCsvId.close();
}

struct Comma final : std::numpunct<char>
{
    char do_decimal_point() const override { return ','; }
};

void GeneticManagerSimpleFSM::saveResultsAfterIteration(std::string csvFilename, std::string csvFilenameId, int iteration)
{
    std::ofstream resultsCsv(csvFilename, std::ios_base::app);
    resultsCsv.imbue(std::locale(std::locale::classic(), new Comma));

    float max;
    float average;

    average = 0.0f;
    max = fsms[0]->getFitness();
    for (int i = 0; i < expConfg.generationSize; i++)
    {
        average += fsms[i]->getFitness();
        if (max < fsms[i]->getFitness())
        {
            max = fsms[i]->getFitness();
        }
    }
    average = average / (float)expConfg.generationSize;

    resultsCsv << iteration << "\t";
    for (int i = 0; i < expConfg.generationSize; i++)
    {
        resultsCsv << fsms[i]->getFitness() << "\t";
    }
    resultsCsv << max << "\t";
    resultsCsv << average << "\t";
    resultsCsv << "\n";

    resultsCsv.close();

    std::ofstream resultsCsvId(csvFilenameId, std::ios_base::app);
    resultsCsvId.imbue(std::locale(std::locale::classic(), new Comma));

    resultsCsvId << "\t";
    for (int i = 0; i < expConfg.generationSize; i++)
    {
        resultsCsvId << fsms[i]->getId() << "\t";
    }
    resultsCsvId << "\n";

    resultsCsvId << iteration << "\t";
    for (int i = 0; i < expConfg.generationSize; i++)
    {
        resultsCsvId << fsms[i]->getFitness() << "\t";
    }
    resultsCsvId << "\n";

    resultsCsvId.close();
}

void GeneticManagerSimpleFSM::saveResultsForSingle(std::string fitnessAndRecordsFileName, std::string fsmFilename, std::string statesFilename,
    Record record, std::vector<int> stateCounting)
{
    std::ofstream fitnessAndRecords(fitnessAndRecordsFileName, std::ios_base::app);
    fitnessAndRecords.imbue(std::locale(std::locale::classic(), new Comma));

    fitnessAndRecords << "fitness\tpoints\thealth\tenemies\tfinished\tsteps\tdistance x\tdistance y\n";
    fitnessAndRecords << fsms[0]->getFitness() << "\t" << record.getPoints() << "\t" << record.getHealth() << "\t" << record.getEnemies() << "\t"
        << record.getFinished() << "\t" << record.getSteps() << "\t" << record.getDistanceX() << "\t" << record.getDistanceY() << "\n";

    fitnessAndRecords.close();

    std::ofstream fsm(fsmFilename, std::ios_base::app);

    fsm << "state\tinput\tnextState\taction\n";
    std::vector<SimpleInput> inputs = fsms[0]->getInputs();
    for (int i = 0; i < fsms[0]->getNumStates(); i++)
    {
        std::vector<std::tuple<int, SimpleAction>> transitions = fsms[0]->getStatesVector()[i].getTransitions();

        for (int j = 0; j < inputs.size(); j++)
        {
            fsm << i << "\t" << inputs[j] << "\t" << std::get<0>(transitions[j]) << "\t" << int(std::get<1>(transitions[j])) << "\n";
        }
    }
    fsm.close();

    std::ofstream states(statesFilename, std::ios_base::app);

    
    states << "FSM\\state\t";
    for (int i = 0; i < expConfg.numStates; i++)
    {
        states << i << "\t";
    }
    states << "\n";
    

    states << "\t";
    for (int i = 0; i < stateCounting.size(); i++)
    {
        states << stateCounting[i] << "\t";
    }
    states << "\n";

    states.close();
}

void GeneticManagerSimpleFSM::saveResultsForFSM(std::string recordsFileName, std::string fsmFilename, std::string statesFilename,
    Record record, int index, std::vector<int> stateCounting)
{
    std::ofstream records(recordsFileName,std::ios_base::app);
    records.imbue(std::locale(std::locale::classic(), new Comma));
    /*if (index == 0)
    {
        records << "\tpoints\thealth\tenemies\tfinished\tsteps\tdistance x\tdistance y\n";
    }*/

    records << index << "\t" << record.getPoints() << "\t" << record.getHealth() << "\t" << record.getEnemies() << "\t"
        << record.getFinished() << "\t" << record.getSteps() << "\t"   << record.getDistanceX() << "\t" << record.getDistanceY() << "\n";
    records.close();


    ///*results 
    //    << "---\nITERATION: " << iteration << "\n---\n"
    //    << "---\nINDEX: " << index << "\n---\n"
    //    << "---\nFITNESS: " << fsm->getFitness() << "\n---\n"
    //    << "---\nRECORD\n---\n"
    //    << "points\t" << record.getPoints() << "\n"
    //    << "health\t" << record.getHealth() << "\n"
    //    << "enemies\t" << record.getEnemies() << "\n"
    //    << "finished\t" << record.getFinished() << "\n"
    //    << "time\t" << record.getTime() << "\n"
    //    << "distance x\t" << record.getDistanceX() << "\n"
    //    << "distance y\t" << record.getDistanceY() << "\n"
    
    std::ofstream fsm(fsmFilename, std::ios_base::app);

    fsm << "state\tinput\tnextState\taction\n";
    std::vector<SimpleInput> inputs = fsms[index]->getInputs();
    for (int i = 0; i < fsms[index]->getNumStates(); i++)
    {
        std::vector<std::tuple<int, SimpleAction>> transitions = fsms[index]->getStatesVector()[i].getTransitions();

        for (int j = 0; j < inputs.size(); j++)
        {
            fsm << i << "\t" << inputs[j] << "\t" << std::get<0>(transitions[j]) << "\t" << int(std::get<1>(transitions[j])) << "\n";
        }
    }
    fsm.close();

    std::ofstream states(statesFilename, std::ios_base::app);

    /*if (index == 0)
    {
        states << "FSM\\state\t";
        for (int i = 0; i < expConfg.numStates; i++)
        {
            states << i << "\t";
        }
        states << "\n";
    }*/

    states << index << "\t";
    for (int i = 0; i < stateCounting.size(); i++)
    {
        states << stateCounting[i] << "\t";
    }
    states << "\n";

    states.close();
}