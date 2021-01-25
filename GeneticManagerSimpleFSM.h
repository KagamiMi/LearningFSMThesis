#pragma once

#include <string>
#include <sstream>
#include <random>
#include <direct.h>
#include <locale>
#include <filesystem>

#include "SimpleFSM.h"
#include "AgentManager.h"
#include "OpenGLManager.h"
#include "SeedValues.h"
#include "ExperimentConfiguration.h"
#include "FitnessValues.h"

class GeneticManagerSimpleFSM
{

    std::vector<SimpleFSM*> fsms;
	std::string mapPath;
    bool windowVisibility;

    SeedValues seedValues;
    ExperimentConfiguration expConfg;
    FitnessValues fitnessValues;

    //float epsilon = 0.001f;
    //genetyczne
    /*int generationSize = 20;
    
    int iterations = 1000;


    float stateSeed = 78;
    float actionSeed = 112;
    float parentsSeed = 100;
    float parentsSelSeed = 99;
    float alleleSelSeed = 103;
    float mutationSeed = 345;
    float mutStateOrAction = 12;

    int parentsRecombProb = 100;*/

    //fsm
    int numActions = 3;
    int numStates; /*= 10;*/
    int numInputs = pow(2, 11);
    //float speedFactor = 5.0;
    //float maxTimeSec = 30.0f;
    
    //distributions and generations
    std::mt19937_64 genState;
    std::uniform_int_distribution<int> distrState;

    std::mt19937_64 genAction;
    std::uniform_int_distribution<int> distrAction;

    std::mt19937_64 genParents;
    std::uniform_int_distribution<int> distrParents;

    std::mt19937_64 genParentsSel;
    std::uniform_int_distribution<int> distrParentsSel;

    std::mt19937_64 genAlleleSel;
    std::uniform_int_distribution<int> distrAlleleSel;

    std::mt19937_64 genMutation;
    std::uniform_real_distribution<float> distrMutation;

    std::mt19937_64 genMutStateOrAction;
    std::uniform_int_distribution<int> distrMutStateOrAction;

private:
    void mutateChildren(std::vector<SimpleFSM*>* children, float mutProb);
	float getBasicFitness(Record record, int maxPoints, int maxEnemies, int maxSteps, float maxXDistance, float maxYDistance);
	std::vector<SimpleFSM*> getParentsTournamentSelection(int numParents, int tournamentSize, std::vector<SimpleFSM*> individuals);
	bool fitnessComparison(const SimpleFSM* fsm1,const SimpleFSM* fsm2);
	std::vector<SimpleFSM*> getChildrenFromRecombination(int numParents, int numStates, int numInputs, int parentProb, std::vector<SimpleFSM*> parents);
    void createNewPopulation(std::vector<SimpleFSM*>* children);
    SimpleFSM* getMaxFromTournament(std::vector<SimpleFSM*>* tournamentGroup);
    //std::string getFilename(tm timeinfo);
    void saveResultsForExperiment(std::string fileName, std::string csvFilename, std::string csvFilenameId);
    void saveResultsForFSM(std::string resultsFileName, std::string fsmFilename, std::string statesFilename,
        Record record, int index,std::vector<int> stateCounting);
    //std::string getFilenameCSV(tm timeinfo);
    void saveResultsAfterIteration(std::string csvFilename, std::string csvFilenameId, int iteration);
    std::string createDirectory(tm timeinfo);
    std::string createDirectoryForIteration(int iteration, std::string folderAbove);
    //float getMaxFitnessValue();
    void saveSeedsAndParameters(std::string filename);
    void saveResultsForSingle(std::string fitnessAndRecordsFileName, std::string fsmFilename, std::string statesFilename,
        Record record, std::vector<int> stateCounting);
public:
    GeneticManagerSimpleFSM(SeedValues seedValues, ExperimentConfiguration expConfg, bool windowVisibility, FitnessValues fitnessValues);
	void runExperimentWithGenetic();
    void runOneWithGenetic(std::string fsmFilename);
};

