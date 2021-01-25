#pragma once

#include <GL\glew.h>
#include <GLFW\glfw3.h>

#include <glm\glm.hpp>
#include <list>
#include <vector>
#include "Rectangle.h"

template<typename T>
struct ElementData
{
	T element;
	std::vector<float> vertsData;
	GLuint vaoId;
	GLuint vertexBuffer;
};
