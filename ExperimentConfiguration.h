#pragma once

struct ExperimentConfiguration
{
	int numStates;
	int generationSize;
	int iterations;

	int parentsRecombProb;
	int numParentsToCrossOver;
	int tournamentSize;
	
	float mutationProb;
	int maxSteps;
	float basicMoveChange;

	std::string mapFilename;
};