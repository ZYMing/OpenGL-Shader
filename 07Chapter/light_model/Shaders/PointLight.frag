//------�������Դ��ƬԪ��ɫ��------

#version 330 core

in vec4 vs_fs_Color;
in vec4 vs_fs_Position;
in vec3 vs_fs_Normal;

uniform vec3 Ambient;		//�������С
uniform vec3 LightColor;	//���Դ��С
uniform vec3 LightPosition;		//���Դλ��
uniform float Shininess;		//�߹����̶ȵ�ָ��ֵ
uniform float Strength;			//���ڵ�������ȵ�����

uniform vec3 EyeDirection;		//�ӵ㷽��
uniform float ConstantAttenuation;		//����˥��ϵ��
uniform float LinearAttenuation;		//����˥��ϵ��
uniform float QuadraticAttenuation;		//ƽ��˥��ϵ��

out vec4 fs_Color;

void main(void)
{
	vec3 lightDirection = LightPosition - vec3(vs_fs_Position);
	float lightDistance = length(LightDirection);
	//�����Դ����
	lightDirection = lightDirection / lightDistance;
	//����˥��ϵ��
	float attenuation = 1.0 / (ConstantAttenuation + 
					LinearAttenuation * lightDistance +
					QuadraticAttenuation * lightDistance * lightDistance);

	vec3 haltVector = normalize(lightDirection + EyeDirection);
	
	float diffuse = max(0.0, dot(vs_fs_Normal, lightDirection));
	float specular = max(0.0, dot(vs_fs_Normal, halfVector));

	if(diffuse == 0.0)
	{
		specular = 0.0;
	}
	else
	{
		specular = pow(specular, Shininess) * Strength;
	}

	vec3 scatteredLight = Ambient + LightColor * diffuse * attenuation;
	vec3 reflectedLight = LightColor * specular * attenuation;

	vec3 rgb = min(vs_fs_Color.rgb * scatteredLight + reflectedLight, vec3(1.0));
	fs_Color = vec4(rgb, vs_fs_Color.a);
}