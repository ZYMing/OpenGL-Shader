//------������ɫ���м����Դ��Ϣ------
//�������Ƶ�������ɫ���У��������OpenGL��ֵ�󴫵�ƬԪ��ɫ���������Ч�ʣ����ջ���Ч������û����ô��

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