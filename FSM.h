#pragma once
#include <list>
#include <vector>
#include <tuple>
#include <random>
#include <algorithm>
#include "State.h"
#include "Input.h"
#include "EnemyRectangle.h"
#include "Rectangle.h"

class FSM
{
private:
	int numStates;
	int numInputs = 16;
	std::vector<State<Action>> states;
	std::vector<Input> inputs;
	int healthLimit; //te¿ do mutacji
	int currentState = 0;
	Action currentAction;
	Input lastInput;

public:
	FSM(int numStates, int healthLimit, Input input);
	void setStates(std::uniform_int_distribution<> *distrState, std::uniform_int_distribution<> *distrAction, std::mt19937_64 *genState, std::mt19937_64 *genAction);
	int getNumStates();
	int getHealthLimit();
	std::vector<State<Action>> getStatesVector();
	std::vector<Input> getInputs();
	void updateState(Input currentInput);
	void executeAction(bool* rightButtonPressed, bool* leftButtonPressed, bool* spacePressed, 
		EnemyRectangle* nearestEnemy, Rectangle* nearestPoint, std::vector<std::list<Rectangle>::iterator> visiblePlatformsIter, Rectangle *agent, bool isJumping);
private:
	void attackEnemy(bool* rightButtonPressed, bool* leftButtonPressed, bool* spacePressed, EnemyRectangle* nearestEnemy, Rectangle* agent, bool isJumping);
	void avoidEnemy(bool* rightButtonPressed, bool* leftButtonPressed, bool* spacePressed);
	void collectPoint(bool* rightButtonPressed, bool* leftButtonPressed, bool* spacePressed);
	void continueToFinish(bool* rightButtonPressed, bool* leftButtonPressed, bool* spacePressed);
	int getInputIndex(Input input);
};

