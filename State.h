#pragma once
#include <list>
#include <tuple>
#include <vector>
#include <random>
#include "Action.h"
#include "SimpleAction.h"


template<typename T>
class State
{
private:
	int numTransitions;
	//bool accessible;
	std::vector<std::tuple<int, T>> transitions;

public:
	State(int numInputs);
	void setTransitions(std::vector<std::tuple<int, T>> transitions);
	//void setAccessible(bool accessible);
	std::vector<std::tuple<int, T>> getTransitions();
	void setNewAction(T action, int inputIndex);
	void setNewState(int newState, int inputIndex);
};

