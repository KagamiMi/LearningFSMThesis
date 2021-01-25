#include "FSM.h"

FSM::FSM(int numStates, int healthLimit, Input input): numStates(numStates), healthLimit(healthLimit), lastInput(input)
{
	states.reserve(numStates);

	inputs.push_back(Input(0, 0, 0, 0));
	inputs.push_back(Input(0, 0, 0, 1));
	inputs.push_back(Input(0, 0, 1, 0));
	inputs.push_back(Input(0, 0, 1, 1));
	inputs.push_back(Input(0, 1, 0, 0));
	inputs.push_back(Input(0, 1, 0, 1));
	inputs.push_back(Input(0, 1, 1, 0));
	inputs.push_back(Input(0, 1, 1, 1));
	inputs.push_back(Input(1, 0, 0, 0));
	inputs.push_back(Input(1, 0, 0, 1));
	inputs.push_back(Input(1, 0, 1, 0));
	inputs.push_back(Input(1, 0, 1, 1));
	inputs.push_back(Input(1, 1, 0, 0));
	inputs.push_back(Input(1, 1, 0, 1));
	inputs.push_back(Input(1, 1, 1, 0));
	inputs.push_back(Input(1, 1, 1, 1));
	
}

void FSM::setStates(std::uniform_int_distribution<> *distrState, std::uniform_int_distribution<> *distrAction, std::mt19937_64 *genState, std::mt19937_64 *genAction)
{
	for (int i = 0; i < numStates; i++)
	{
		std::vector<std::tuple<int, Action>> transitions;
		for (int i = 0; i < numInputs; i++)
		{
			int nextState = (*distrState)(*genState);
			Action action = (Action)(*distrAction)(*genAction);
			
			if (!inputs[i].getEnemyVisible() && action == Action::AttackEnemy)
			{
				while (action == Action::AttackEnemy)
				{
					action = (Action)(*distrAction)(*genAction);
				}
				if (!inputs[i].getPointVisible() && action == Action::CollectPoint)
				{
					while (action == Action::AttackEnemy || action == Action::CollectPoint)
					{
						action = (Action)(*distrAction)(*genAction);
					}
				}
			}

			if (!inputs[i].getPointVisible() && action == Action::CollectPoint)
			{
				while (action == Action::CollectPoint)
				{
					action = (Action)(*distrAction)(*genAction);
				}
				if (!inputs[i].getEnemyVisible() && action == Action::AttackEnemy)
				{
					while (action == Action::AttackEnemy || action == Action::CollectPoint)
					{
						action = (Action)(*distrAction)(*genAction);
					}
				}
			}

			transitions.push_back(std::tuple<int, Action>(nextState, action));
		}

		states.push_back(State<Action>(numInputs));
		states[i].setTransitions(transitions);
	}

	//bazuj¹c na lastInput daæ tu pocz¹tkowe currentAction - albo neutralne
	int index = getInputIndex(lastInput);
	std::tuple<int, Action> tuple = states[currentState].getTransitions()[index];
	currentState = std::get<0>(tuple);
	currentAction = std::get<1>(tuple);
}

int FSM::getNumStates()
{
	return numStates;
}

int FSM::getHealthLimit()
{
	return healthLimit;
}

std::vector<State<Action>> FSM::getStatesVector()
{
	return states;
}

std::vector<Input> FSM::getInputs()
{
	return inputs;
}

void FSM::updateState(Input currentInput)
{
	if (!(currentInput == lastInput))
	{
		int index = getInputIndex(currentInput);
		std::tuple<int, Action> tuple = states[currentState].getTransitions()[index];
		currentState = std::get<0>(tuple);
		currentAction = std::get<1>(tuple);

		lastInput = currentInput;
		//zmiana na nowy stan, przypisanie ostatniej akcji, egzekuzja akcji
		printf("Input: %d%d%d%d \n", lastInput.getOverHealthLimit(),lastInput.getEnemyVisible(),lastInput.getPointVisible(),lastInput.getPlatformVisible());
		printf("State: %d \n", currentState);
		printf("Action: %d \n", currentAction);
	}
	else
	{
		//printf("no input change\n");
	}
}

int FSM::getInputIndex(Input input)
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

void FSM::executeAction(bool* rightButtonPressed, bool* leftButtonPressed, bool* spacePressed, 
	EnemyRectangle* nearestEnemy, Rectangle* nearestPoint, std::vector<std::list<Rectangle>::iterator> visiblePlatformsIter, 
	Rectangle* agent, bool isJumping)
{
	switch (currentAction)
	{
	case Action::AttackEnemy:
		attackEnemy(rightButtonPressed,leftButtonPressed,spacePressed, nearestEnemy, agent, isJumping);
		break;
	case Action::AvoidEnemy:
		avoidEnemy(rightButtonPressed, leftButtonPressed, spacePressed);
		break;
	case Action::CollectPoint:
		collectPoint(rightButtonPressed, leftButtonPressed, spacePressed);
		break;
	case Action::ContinueToFinish:
		continueToFinish(rightButtonPressed, leftButtonPressed, spacePressed);
		break;
	}
}

void FSM::attackEnemy(bool* rightButtonPressed, bool* leftButtonPressed, bool* spacePressed, EnemyRectangle* nearestEnemy, Rectangle* agent, bool isJumping)
{
	//refki: 
	//najbli¿szy wróg
	//zasiêg skoku
	//czy ju¿ skaczemy czy nie

	if (isJumping == false)
	{
		*spacePressed = true;
	}
	else
	{
		*spacePressed = false;
	}

	if (agent->getYCenter() < nearestEnemy->getYCenter())
	{
		//znajdŸ drogê do góry
	}
	else
	{
		if (agent->getXCenter() < nearestEnemy->getXCenter())
		{
			*rightButtonPressed = true;
			*leftButtonPressed = false;
		}
		else if (agent->getXCenter() > nearestEnemy->getXCenter())
		{
			*rightButtonPressed = false;
			*leftButtonPressed = true;
		}
		else
		{
			*rightButtonPressed = false;
			*leftButtonPressed = false;
		}
	}

	*rightButtonPressed = true;
	*leftButtonPressed = false;
	*spacePressed = false;
}

void FSM::avoidEnemy(bool* rightButtonPressed, bool* leftButtonPressed, bool* spacePressed)
{
	*rightButtonPressed = false;
	*leftButtonPressed = false;
	*spacePressed = false;
}

void FSM::collectPoint(bool* rightButtonPressed, bool* leftButtonPressed, bool* spacePressed)
{
	*rightButtonPressed = true;
	*leftButtonPressed = false;
	*spacePressed = false;
}

void FSM::continueToFinish(bool* rightButtonPressed, bool* leftButtonPressed, bool* spacePressed)
{
	*rightButtonPressed = false;
	*leftButtonPressed = false;
	*spacePressed = true;
}


