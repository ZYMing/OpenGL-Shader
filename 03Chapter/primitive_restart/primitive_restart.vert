#version 330 core

in vec4 vPosition;

in vec4 vColor;

out vec4 Color;

uniform mat4 model_matrix;
uniform mat4 projection_matrix;

void main()
{
	gl_Position = projection_matrix * model_matrix * vPosition;
	Color = vColor;
}