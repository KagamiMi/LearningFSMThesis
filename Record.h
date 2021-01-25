#pragma once
class Record
{
private:
	int points;
	int health;
	int enemies;
	bool finished;
	int steps;
	float distanceX;
	float distanceY;
public:
	Record(int points, int health, int enemies, bool finished, int steps, float distanceX, float distanceY);
	int getPoints();
	int getHealth();
	int getEnemies();
	bool getFinished();
	float getSteps();
	float getDistanceX();
	float getDistanceY();
};

