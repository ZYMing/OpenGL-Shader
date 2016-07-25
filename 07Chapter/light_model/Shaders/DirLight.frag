//------������������ƬԪ��ɫ��------

#version 330 core

in vec4 vs_fs_Color;
in vec3 vs_fs_Normal;

uniform vec3 Ambient = (0.2, 0.2, 0.2);		//�������С
uniform vec3 LightColor;	//������С
uniform vec3 LightDirection;	//���շ���
uniform vec3 HalfVector;		//����ȵı��淽��
uniform float Shininess = 20.0f;		//�߹����̶ȵ�ָ��ֵ
uniform float Strength = 10.0f;			//���ڵ�������ȵ�����

out vec4 fs_Color;

void main(void)
{
	float diffuse = max(0.0, dot(vs_fs_Normal, LightDirection));
	float specular = max(0.0, dot(vs_fs_Normal, HalfVector));

	//���������Ҳ�������ô���淴��Ҳ�ǲ����ܵ�
	if(diffuse == 0.0)
	{
		specular = 0.0;
	}
	else
	{
		specular = pow(specular, Shininess);
	}

	vec3 scatteredLight = Ambient + diffuse * LightColor;		//����ɢ���
	vec3 reflectedLight = specular * LightColor * Strength;		//���㷴���

	vec3 rgb = min(Color.rgb * scatteredLight + reflectedLight, vec3(1.0));
	fs_Color = vec4(rgb, vs_fs_Color.a);
}