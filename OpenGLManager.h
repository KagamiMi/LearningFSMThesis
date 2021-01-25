#pragma once

#include <stdio.h>
#include <stdlib.h>


#include <GL\glew.h>
#include <GLFW\glfw3.h>

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <list>
#include <algorithm>
#include <ctime>
#include "Rectangle.h"
#include "EnemyRectangle.h"
#include "Record.h"
#include "Input.h"
#include "ElementData.h";
#include "AgentManager.h"
#include "SimpleInput.h"

using namespace glm;
class OpenGLManager
{
private:

	AgentManager* agentManager;

	//zmienne szybkoœciowe
	//float speedFactor;

	float basicMoveChange = 0.5f;
	float basicMoveChangeEnemy = basicMoveChange/2.0f;

	//float gravity = 5.0f;
	
	//float enemySpeed = 2.0f;

	//zwi¹zane z wrogiem
	int enemyDeathPoints = 5;
	//int enemyDamage = 10;

	float globalXChange = 0.0f;
	float globalYChange = 0.0f;


	//mapa
	std::string mapPath;

	//dane do renderowania opengl
	GLFWwindow* window;
	float width = 800;
	float height = 500;
	float worldSizeHalfWidth;
	float worldSizeHalfHeight;

	float mapHeight;
	float mapWidth;

	int pointsNum;
	int enemyNum;

	ElementData<Rectangle> agent;
	/*Rectangle agent;
	GLuint agentVaoId;
	GLuint agentVertexBuffer;
	std::vector<float> agentData;*/

	ElementData<std::list<Rectangle>> platforms;
	/*GLuint platformsVaoId;
	GLuint platformsVertexBuffer;
	std::list<Rectangle> platforms;
	std::vector<float> platformsData;*/

	ElementData<std::list<Rectangle>> collectables;
	/*std::list<Rectangle> collectables;
	std::vector<float> collectablesData;
	GLuint collectablesVaoId;
	GLuint collectablesVertexBuffer;*/

	ElementData<std::list<EnemyRectangle>> enemies;
	/*std::vector<float> enemiesData;
	std::list<EnemyRectangle> enemies;
	GLuint enemiesVaoId;
	GLuint enemiesVertexBuffer;*/

	ElementData<Rectangle> finish;
	//GLuint finishVaoId;
	//GLuint finishVertexBuffer;
	//Rectangle finish;
	//std::vector<float> finishData;

	glm::mat4 Projection, View, Model = glm::mat4(1);


	GLuint MatrixID;
	GLuint colorUniform;
	GLuint programID;

	bool windowVisible;

public:
	OpenGLManager(std::string mapPath, float basicMoveChange, AgentManager* agentManager, bool windowVisible);
	int initialization();
	bool stillRunning();
	//void updateForLoop(bool* rightButtonPressed, bool* leftButtonPressed, bool* spacePressed, Rectangle*agent);
	void updateForLoop(bool* rightButtonPressed, bool* leftButtonPressed, bool* spacePressed, std::string logPosFilename, int stepsCounter);
	//Input getCurrentInputs(int healthLimit, EnemyRectangle* nearestEnemy, Rectangle* nearestPoint, std::vector<std::list<Rectangle>::iterator> visiblePlatformsIter);
	SimpleInput getCurrentInputs();
	int getPointsNum();
	int getEnemyNum();
	float getDistanceX();
	float getDistanceY();
	float getMapHeight();
	float getMapWidth();
private:
	void loadMap();
	void loadPlatforms();
	void loadAgent();
	void loadFinish();
	void loadCollectables();
	void loadEnemies();
	void updateAgent();
	void updateCollectables();
	void updateEnemies();
	GLuint loadShaders(const char* vertex_file_path, const char* fragment_file_path);
	glm::mat4 getModelMatrix(bool* rightButtonPressed, bool* leftButtonPressed, bool* spacePressed, std::string logPositionFile, int step);
	glm::mat4 getModelMatrix(bool* rightButtonPressed, bool* leftButtonPressed, bool* spacePressed);
	void draw();
	//EnemyRectangle *getNearestEnemy(std::vector<std::list<EnemyRectangle>::iterator> visibleEnemies, std::vector<std::list<Rectangle>::iterator> visiblePlatforms);
	//Rectangle* getNearestPoint(std::vector<std::list<Rectangle>::iterator> visiblePoints);
};

