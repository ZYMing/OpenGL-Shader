//-------仅仅包含环境光的顶点着色器------
#version 330 core

in vec4 VertexColor;
in vec4 VertexPosition;

uniform mat4 MVPMatrix;

out vec4 vs_fs_Color;

void main(void)
{
	vs_fs_Color = VertexColor;
	gl_Position = MVPMatrix * VertexPosition;
}