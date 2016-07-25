//------包含多光源与多材料性质的顶点着色器------

#version 330 core

in vec4 VertexColor;
in vec4 VertexPosition;
in vec3 VertexNormal;

uniform mat4 MVPMatrix;
uniform mat4 MVMatrix;
uniform mat3 NormalMatrix;

out vec4 vs_fs_Color;
out vec4 vs_fs_Position;
out vec3 vs_fs_Normal;

void main(void)
{
	vs_fs_Color = VertexColor;
	
	vs_fs_Normal = normalize(NormalMatrix * VertexNormal);

	vs_fs_Position = MVMatrix * VertexPosition;
	gl_Position = MVPMatrix * VertexPosition;
}