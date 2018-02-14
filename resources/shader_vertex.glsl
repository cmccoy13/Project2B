#version 330 core
layout(location = 0) in vec3 vertPos;

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;

out vec3 tempCol;

void main()
{
	vec3 tempPos = vertPos;
	gl_Position = P * V * M * vec4(tempPos, 1.0);
	tempCol = tempPos;
}
