#version 330 core
out vec3 color;

in vec4 gl_FragCoord;
in vec3 tempCol;

void main()
{
	color = tempCol;
}
