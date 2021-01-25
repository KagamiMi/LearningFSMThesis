#pragma once

#include <vector>
#include <string>
#include <sstream>
#include <bitset>
#include <fstream>
#include "State.h"
#include "SimpleInput.h"
#include "SimpleAction.h"

class SimpleFSM
{
private:
	static unsigned int idGenerator;
	unsigned int id;
	int numStates;
	int numInputs;
	std::vector<State<SimpleAction>> states;
	std::vector<SimpleInput> inputs;
	int currentState = 0;
	SimpleAction currentAction;
	SimpleInput lastInput;
	float fitnessValue = 0.0f;
	int sameInput = 0;

public:
	SimpleFSM(int numStates);
	SimpleFSM(int numStates, SimpleInput startInput);
	void setStates(std::uniform_int_distribution<>* distrState, std::uniform_int_distribution<>* distrAction, std::mt19937_64* genState, std::mt19937_64* genAction);
	void setStates(std::vector<State<SimpleAction>> states);
	int getInputIndex(SimpleInput input);
	void updateState(SimpleInput currentInput, std::string filename, int stepsCounter);
	void executeAction(bool* rightButtonPressed, bool* leftButtonPressed, bool* spacePressed);
	int getNumStates();
	int getNumInputs();
	std::vector<SimpleInput> getInputs();
	std::vector<State<SimpleAction>> getStatesVector();
	void setInitialInput(SimpleInput startInput);
	void setFitness(float fitness);
	float getFitness() const;
	void setNewAction(SimpleAction action, int stateIndex, int inputIndex);
	void setNewState(int newState, int stateIndex, int inputIndex);
	//bool isSameInputOver(int overNumber);
	int getCurrentState();
	//void reset();
	unsigned int getId();
	void loadFromFile(std::string fsmFilename);
	void logCurrentState(std::string filename, int stepsCounter);
};

