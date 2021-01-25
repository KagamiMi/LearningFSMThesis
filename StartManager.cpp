#include "StartManager.h"

int StartManager::start(int argsNum, char* args[])
{
    if (argsNum > 1)
    {
        std::string runType = args[1];
        int argsNumber;

        if (!runType.compare("experiment"))
        {
            argsNumber = 5;
        }
        else if (!runType.compare("single_run"))
        {
            argsNumber = 6;
        }
        else
        {
            fprintf(stderr, "Wrong first argument.\n");
            return -1;
        }

        if (argsNumber == argsNum)
        {
            std::string windowVisibility = args[2];
            std::string confgFilename = args[3];
            std::string fitnessFilename = args[4];

            bool* success = new bool(false);
            FitnessValues fitnessValues = getFitnessValues(fitnessFilename, success);
            if (!(*success))
            {
                delete success;
                return -1;
            }
            delete success;

            bool *randomSeed = new bool(false);
            uint64_t *initialSeed = new uint64_t();
            SeedValues seedValues;
            success = new bool(false);
            ExperimentConfiguration expConfg = getConfiguration(confgFilename, initialSeed, randomSeed, success);
            if (!(*success))
            {
                delete success;
                delete randomSeed;
                delete initialSeed;
                return -1;
            }
            delete success;
            

            if (*randomSeed)
            {
                *initialSeed = time(0);
            }

            seedValues.initialSeed = *initialSeed;
            std::seed_seq seed{ *initialSeed };
            std::vector<std::uint64_t> seeds(7);
            seed.generate(seeds.begin(), seeds.end());
            seedValues.actionSeed = seeds[0];
            seedValues.alleleSelSeed = seeds[1];
            seedValues.mutationSeed = seeds[2];
            seedValues.mutStateOrAction = seeds[3];
            seedValues.parentsSeed = seeds[4];
            seedValues.parentsSelSeed = seeds[5];
            seedValues.stateSeed = seeds[6];

            bool visible;
            if (!windowVisibility.compare("window_invisible"))
            {
                visible = false;
            }
            else if (!windowVisibility.compare("window_visible"))
            {
                visible = true;
            }
            else
            {
                fprintf(stderr, "Wrong window visibility argument.\n");
                return -1;
            }

            GeneticManagerSimpleFSM* gmSimpleFSM = new GeneticManagerSimpleFSM(seedValues, expConfg, visible, fitnessValues);


            if (!runType.compare("experiment"))
            {
                gmSimpleFSM->runExperimentWithGenetic();
            }
            else if (!runType.compare("single_run"))
            {
                gmSimpleFSM->runOneWithGenetic((std::string)args[5]);
            }

            delete gmSimpleFSM;

            delete randomSeed;
            delete initialSeed;
            return 0;
            
        }
        else
        {
            fprintf(stderr, "Wrong number of arguments.\n");
            return -1;
        }
    }
    else
    {
        fprintf(stderr, "Missing arguments.\n");
        return -1;
    }

    return 0;
}

FitnessValues StartManager::getFitnessValues(std::string fitnessFilename, bool *success)
{
    FitnessValues fitnessValues;

    std::ifstream fitnessFileStream(fitnessFilename, std::ios::in);
    if (fitnessFileStream.is_open())
    {
        std::string line;
        std::stringstream sstr;

        while (std::getline(fitnessFileStream, line))
        {
            if (!line.compare("recordFactor"))
            {
                std::getline(fitnessFileStream, line);
                sstr.clear();
                sstr.str(line);
                sstr >> fitnessValues.recordFactor;
            }
            else if (!line.compare("enemiesFactor"))
            {
                std::getline(fitnessFileStream, line);
                sstr.clear();
                sstr.str(line);
                sstr >> fitnessValues.enemiesFactor;
            }
            else if (!line.compare("healthFactor"))
            {
                std::getline(fitnessFileStream, line);
                sstr.clear();
                sstr.str(line);
                sstr >> fitnessValues.healthFactor;
            }
            else if (!line.compare("finishedFactor"))
            {
                std::getline(fitnessFileStream, line);
                sstr.clear();
                sstr.str(line);
                sstr >> fitnessValues.finishedFactor;
            }
            else if (!line.compare("timeFactor"))
            {
                std::getline(fitnessFileStream, line);
                sstr.clear();
                sstr.str(line);
                sstr >> fitnessValues.timeFactor;
            }
            else if (!line.compare("distanceXFactor"))
            {
                std::getline(fitnessFileStream, line);
                sstr.clear();
                sstr.str(line);
                sstr >> fitnessValues.distanceXFactor;
            }
            else if (!line.compare("distanceYFactor"))
            {
                std::getline(fitnessFileStream, line);
                sstr.clear();
                sstr.str(line);
                sstr >> fitnessValues.distanceYFactor;
            }
        }

        fitnessFileStream.close();
        (*success) = true;
    }
    else
    {
        fprintf(stderr, "Wrong configuration file in arguments.\n");
        (*success) = false;
    }


    return fitnessValues;
}

ExperimentConfiguration StartManager::getConfiguration(std::string configFilename, uint64_t* initialSeed, bool *randomSeed, bool* success)
{
    std::ifstream configFileStream(configFilename, std::ios::in);
    ExperimentConfiguration expConfg;

    if (configFileStream.is_open())
    {
        std::string line;
        std::stringstream sstr;

        

        while (std::getline(configFileStream, line))
        {
            if (!line.compare("numStates"))
            {
                std::getline(configFileStream, line);
                sstr.clear();
                sstr.str(line);
                sstr >> expConfg.numStates;
            }
            else if (!line.compare("generationSize"))
            {
                std::getline(configFileStream, line);
                sstr.clear();
                sstr.str(line);
                sstr >> expConfg.generationSize;
            }
            else if (!line.compare("iterations"))
            {
                std::getline(configFileStream, line);
                sstr.clear();
                sstr.str(line);
                sstr >> expConfg.iterations;
            }
            else if (!line.compare("parentsRecombProb"))
            {
                std::getline(configFileStream, line);
                sstr.clear();
                sstr.str(line);
                sstr >> expConfg.parentsRecombProb;
            }
            else if (!line.compare("numParentsToCrossOver"))
            {
                std::getline(configFileStream, line);
                sstr.clear();
                sstr.str(line);
                sstr >> expConfg.numParentsToCrossOver;
            }
            else if (!line.compare("tournamentSize"))
            {
                std::getline(configFileStream, line);
                sstr.clear();
                sstr.str(line);
                sstr >> expConfg.tournamentSize;
            }
            else if (!line.compare("mutationProb"))
            {
                std::getline(configFileStream, line);
                sstr.clear();
                sstr.str(line);
                sstr >> expConfg.mutationProb;
            }
            else if (!line.compare("maxSteps"))
            {
                std::getline(configFileStream, line);
                sstr.clear();
                sstr.str(line);
                sstr >> expConfg.maxSteps;
            }
            else if (!line.compare("basicMoveChange"))
            {
                std::getline(configFileStream, line);
                sstr.clear();
                sstr.str(line);
                sstr >> expConfg.basicMoveChange;
            }
            else if (!line.compare("mapFileName"))
            {
                std::getline(configFileStream, line);
                sstr.clear();
                sstr.str(line);
                sstr >> expConfg.mapFilename;
            }
            else if (!line.compare("randomSeed"))
            {
                std::string check;
                std::getline(configFileStream, line);
                sstr.clear();
                sstr.str(line);
                sstr >> check;
                if (!check.compare("true"))
                {
                    *randomSeed = true;
                }
                else
                {
                    *randomSeed = false;
                }
            }
            else if (!line.compare("initialSeed"))
            {
                std::getline(configFileStream, line);
                sstr.clear();
                sstr.str(line);
                sstr >> *initialSeed;
            }
        }
        configFileStream.close();
        *success = true;
    }
    else
    {
        fprintf(stderr, "Wrong configuration file in arguments.\n");
        *success = false;
    }
    return expConfg;
}
