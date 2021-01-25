#include "State.h"

template<typename T>
State<T>::State(int numInputs): numTransitions(numInputs)
{
	transitions.reserve(numInputs);
}

template<typename T>
void State<T>::setTransitions(std::vector<std::tuple<int, T>> transitions)
{
	this->transitions = transitions;
}

//template<typename T>
//void State<T>::setAccessible(bool accessible)
//{
//	this->accessible = accessible;
//}

template<typename T>
std::vector<std::tuple<int, T>> State<T>::getTransitions()
{
	return transitions;
}

template<typename T>
void State<T>::setNewAction(T action, int inputIndex)
{
	std::get<1>(transitions[inputIndex]) = action;
}

template<typename T>
void State<T>::setNewState(int newState, int inputIndex)
{
	std::get<0>(transitions[inputIndex]) = newState;
}

template State<Action>::State(int numInputs);
template void State<Action>::setTransitions(std::vector<std::tuple<int, Action>> transitions);
//template void State<Action>::setAccessible(bool accessible);
template std::vector<std::tuple<int, Action>> State<Action>::getTransitions();
template void State<Action>::setNewAction(Action action, int inputIndex);
template void State<Action>::setNewState(int newState, int inputIndex);

template State<SimpleAction>::State(int numInputs);
template void State<SimpleAction>::setTransitions(std::vector<std::tuple<int, SimpleAction>> transitions);
//template void State<SimpleAction>::setAccessible(bool accessible);
template std::vector<std::tuple<int, SimpleAction>> State<SimpleAction>::getTransitions();
template void State<SimpleAction>::setNewAction(SimpleAction action, int inputIndex);
template void State<SimpleAction>::setNewState(int newState, int inputIndex);