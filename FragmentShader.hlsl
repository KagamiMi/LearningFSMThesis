#version 330 core

out vec3 color;

uniform vec3 obj_color;

void main()
{
	color = obj_color;
}