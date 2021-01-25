#pragma once
#include <stdio.h>

#include <GL\glew.h>
#include <GLFW\glfw3.h>

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <filesystem>
#include <ctime>
#include <iomanip>
#include <algorithm>

#include "OpenGLManager.h"
#include "FSM.h"
#include "SimpleFSM.h"
#include "GeneticManagerSimpleFSM.h"

class StartManager
{
public:
	int start(int argsNum, char* args[]);
private:
	FitnessValues getFitnessValues(std::string fitnessFilename, bool *success);
	ExperimentConfiguration getConfiguration(std::string configFilename, uint64_t* initialSeed, bool* randomSeed, bool *success);
};

