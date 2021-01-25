#pragma once

#include "Record.h"
#include "Rectangle.h"
#include <iostream>

class AgentManager
{
private:
	//podstawowe dane agenta
	int points = 0;
	int health = 100;
	bool finished = false;
	int enemiesDefeated = 0;

	//float speed = 4.0f;
	//float jumpSpeed = 10.0f;
	//float immunityTime = 2.0f;

	//zmienne do poruszania

	float xChange = 0.0f;

	float yChange = 0.0f;

	bool isJumping = false;
	float jumpDistance = 5.0f;
	float currentJumpDistance = 0.0f;

	bool isImmune = false;
	int immunityMaxSteps;
	int passedImmunitySteps = 0;
public:
	AgentManager(float basicMoveChange);
	bool stillRunning();
	int getHealth();
	Record getRecord(int steps, float distanceX, float distanceY);
	float getXChange();
	float getYChange();
	void addToXChange(float change);
	//float addToXChangeTime(float deltaTime);
	void addToYChange(float change);
	//float addToYChangeTime(float deltaTime);
	void cleanXYChange();
	void addPoints(int pointsToAdd);
	int getPoints();
	void updateImmunity();
	void enemyDefeat();
	bool getIsImmune();
	void enemyAttack(int enemyDamage);
	void setFinished(bool finished);
	float handleJumping(float moveChange,bool top, bool bottom);
	float startJumping(float moveChange, bool top, bool bottom,bool*spacePressed);
	bool getIsJumping();
};

