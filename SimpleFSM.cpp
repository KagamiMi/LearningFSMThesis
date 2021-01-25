#include "SimpleFSM.h"
unsigned int SimpleFSM::idGenerator = 0;

SimpleFSM::SimpleFSM(int numStates) : numStates(numStates)
{
	id = idGenerator++;
	states.reserve(numStates);
	numInputs = pow(2, 11);
	for (int i = 0; i < pow(2, 11); i++)
	{
		std::string binary = std::bitset<11>(i).to_string();

		inputs.push_back(SimpleInput(int(binary[0] - '0'), int(binary[1] - '0'), int(binary[2] - '0'), int(binary[3] - '0'), int(binary[4] - '0'),
			int(binary[5] - '0'), int(binary[6] - '0'), int(binary[7] - '0'), int(binary[8] - '0'), int(binary[9] - '0'), int(binary[10] - '0')));
	}
}


SimpleFSM::SimpleFSM(int numStates, SimpleInput startInput): numStates(numStates), lastInput(startInput)
{
	id = idGenerator++;
	states.reserve(numStates);
	numInputs = pow(2, 11);
	for (int i = 0; i < pow(2, 11); i++)
	{
		std::string binary = std::bitset<11>(i).to_string();

		int cropped = int(binary[0]) - '0';
		inputs.push_back(SimpleInput(int(binary[0]-'0'), int(binary[1]-'0'), int(binary[2]-'0'), int(binary[3]-'0'), int(binary[4]-'0'), 
			int(binary[5]-'0'), int(binary[6]-'0'), int(binary[7]-'0'), int(binary[8]-'0'), int(binary[9]-'0'), int(binary[10]-'0')));
	}

	

}

void SimpleFSM::loadFromFile(std::string fsmFilename)
{
	std::ifstream fsmFile(fsmFilename, std::ios::in);

	std::string line;
	std::stringstream sstr;

	int state;
	std::string input;
	int nextState;
	int action;
	
	std::getline(fsmFile, line); //wczytujemy nag³ówek
	

	for (int i = 0; i < numStates; i++)
	{
		states.push_back(State<SimpleAction>(numInputs));
	}

	std::vector<std::tuple<int, SimpleAction>> transitions;
	transitions.reserve(numInputs);
	
	int lastState = 0;

	while (std::getline(fsmFile, line))
	{
		sstr.clear();
		sstr.str(line);

		sstr >> state >> input >> nextState >> action;

		if (state != lastState)
		{
			states[lastState].setTransitions(transitions);
			transitions.clear();
			transitions.reserve(numInputs);
			lastState = state;
		}
		transitions.push_back(std::tuple<int, SimpleAction>(nextState, (SimpleAction)action));
	}
	states[lastState].setTransitions(transitions);

	fsmFile.close();
}

void SimpleFSM::setStates(std::vector<State<SimpleAction>> states)
{
	this->states = states;
}

void SimpleFSM::setStates(std::uniform_int_distribution<>* distrState, std::uniform_int_distribution<>* distrAction, std::mt19937_64* genState, std::mt19937_64* genAction)
{
	for (int i = 0; i < numStates; i++)
	{
		std::vector<std::tuple<int, SimpleAction>> transitions;
		transitions.reserve(numInputs);
		for (int i = 0; i < numInputs; i++)
		{
			int nextState = (*distrState)(*genState);
			SimpleAction action = (SimpleAction)(*distrAction)(*genAction);

			transitions.push_back(std::tuple<int, SimpleAction>(nextState, action));
		}

		states.push_back(State<SimpleAction>(numInputs));
		states[i].setTransitions(transitions);
	}

	//bazuj¹c na lastInput daæ tu pocz¹tkowe currentAction - albo neutralne
	///*int index = getInputIndex(lastInput);
	//std::tuple<int, SimpleAction> tuple = states[currentState].getTransitions()[index];
	//currentState = std::get<0>(tuple);
	//currentAction = std::get<1>(tuple);*/
}

int SimpleFSM::getInputIndex(SimpleInput input)
{
	for (int i = 0; i < inputs.size(); i++)
	{
		if (inputs[i] == input)
		{
			return i;
		}
	}
	return -1;
}

void SimpleFSM::logCurrentState(std::string filename, int stepsCounter)
{
	std::ofstream log(filename, std::ios_base::app);
	//plus przed pêtl¹ by daæ nag³ówek
	log << stepsCounter << "\t";
	log << lastInput.lowHealth << lastInput.endOfPlatformOnRight << lastInput.endOfPlatformOnRight << lastInput.enemyOnLeft << lastInput.enemyOnRight <<
		lastInput.finishOnLeft << lastInput.finishOnRight << lastInput.platformAboveOnRight << lastInput.platformAboveOnLeft << lastInput.pointOnLeft <<
		lastInput.pointOnRight << "\t";
	log << currentState << "\t";
	log << (int)currentAction << "\t\n";

	log.close();
}

void SimpleFSM::updateState(SimpleInput currentInput, std::string filename, int stepsCounter)
{
	if (currentInput == lastInput)
	{
		sameInput++;
	}
	else
	{
		sameInput = 0;
	}

	//if (!(currentInput == lastInput))
	//{
	int index = getInputIndex(currentInput);
	std::tuple<int, SimpleAction> tuple = states[currentState].getTransitions()[index];
	currentState = std::get<0>(tuple);
	currentAction = std::get<1>(tuple);

	lastInput = currentInput;
		//zmiana na nowy stan, przypisanie ostatniej akcji, egzekuzja akcji
		/*printf("Input: %d%d%d%d%d%d%d%d%d%d%d \n", lastInput.lowHealth, lastInput.endOfPlatformOnRight, lastInput.endOfPlatformOnRight, lastInput.enemyOnLeft,lastInput.enemyOnRight,
			lastInput.finishOnLeft,lastInput.finishOnRight,lastInput.platformAboveOnRight,lastInput.platformAboveOnLeft,lastInput.pointOnLeft,lastInput.pointOnRight);
		printf("State: %d \n", currentState);
		printf("Action: %d \n", currentAction);*/
	//}
	//else
	//{
		//printf("no input change\n");
	//}

	//logCurrentState(filename,stepsCounter);

}

void SimpleFSM::executeAction(bool* rightButtonPressed, bool* leftButtonPressed, bool* spacePressed)
{
	switch (currentAction)
	{
	case SimpleAction::GoLeft:
		*rightButtonPressed = false;
		*leftButtonPressed = true;
		*spacePressed = false;
		break;
	case SimpleAction::GoRight:
		*rightButtonPressed = true;
		*leftButtonPressed = false;
		*spacePressed = false;
		break;
	case SimpleAction::Jump:
		*rightButtonPressed = false;
		*leftButtonPressed = false;
		*spacePressed = true;
		break;
	}
}

int SimpleFSM::getNumStates()
{
	return numStates;
}

int SimpleFSM::getNumInputs()
{
	return numInputs;
}

std::vector<SimpleInput> SimpleFSM::getInputs()
{
	return inputs;
}

std::vector<State<SimpleAction>> SimpleFSM::getStatesVector()
{
	return states;
}

void SimpleFSM::setInitialInput(SimpleInput startInput)
{
	lastInput = startInput;
}

void SimpleFSM::setFitness(float fitness)
{
	fitnessValue = fitness;
}

float SimpleFSM::getFitness() const
{
	return fitnessValue;
}

void SimpleFSM::setNewAction(SimpleAction action, int stateIndex, int inputIndex)
{
	states[stateIndex].setNewAction(action,inputIndex);
}

void SimpleFSM::setNewState(int newState, int stateIndex, int inputIndex)
{
	states[stateIndex].setNewState(newState, inputIndex);
}

//bool SimpleFSM::isSameInputOver(int overNumber)
//{
//	return sameInput >= overNumber;
//}

int SimpleFSM::getCurrentState()
{
	return currentState;
}

//void SimpleFSM::reset()
//{
//	currentState = 0;
//	sameInput = 0;
//	fitnessValue = 0;
//}

unsigned int SimpleFSM::getId()
{
	return id;
}
