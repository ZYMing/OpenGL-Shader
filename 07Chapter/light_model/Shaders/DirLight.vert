//------仅包含方向光的顶点着色器------

#version 330 core

in vec4 VertexColor;
in vec4 VertexPosition;
in vec3 VertexNormal;

uniform mat4 MVPMatrix;
uniform mat3 NormalMatrix;

out vec4 vs_fs_Color;
out vec3 vs_fs_Normal;

void main(void)
{
	vs_fs_Color = VertexColor;

	vs_fs_Normal = normalize(NormalMatrix * VertexNormal);

	gl_Position = MVPMatrix * VertexPosition;
}