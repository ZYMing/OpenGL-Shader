//------顶点着色器中计算光源信息------
//将计算移到顶点着色器中，结果经过OpenGL插值后传到片元着色器，提高了效率，最终绘制效果可能没有那么好

#version 330 core

in vec4 VertexColor;
in vec4 VertexPosition;
in vec3 VertexNormal;

uniform mat4 MVPMatrix;
uniform mat3 NormalMatrix;

uniform vec3 LightPosition;
uniform vec3 EyeDirection;
uniform float ConstantAttenuation;
uniform float LinearAttenuation;
uniform float QuadraticAttenuation;

out vec4 vs_fs_Color;
out vec3 vs_fs_Normal;
out vec3 LightDirection;
out vec3 HalfVector;
out float Attenuation;

void main(void)
{
	vs_fs_Color = VertexColor;
	vs_fs_Normal = normalize(NormalMatrix * VertexNormal);

	LightDirection = LightPosition - vec3(VertexPosition);
	float lightDistance = length(LightDirection);
	LightDirection = LightDirection / lightDistance;

	Attenuation = 1.0 / (ConstantAttenuation + 
				LinearAttenuation * lightDistance + 
				QuadraticAttenuation * lightDistance * lightDistance);

	HalfVector = normalize(LightDirection + EyeDirection);
	
	gl_Position = MVPMatrix * VertexPosition;
}