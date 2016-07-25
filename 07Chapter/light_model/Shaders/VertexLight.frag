//------光源信息在顶点着色器中计算的片元着色器------
#version 330 core

in vec4 vs_fs_Color;
in vec3 vs_fs_Normal;
in vec3 LightDirection;
in vec3 HalfVector;
in float Attenuation;

uniform vec3 Ambient;
uniform vec3 LightColor;
uniform float Shininess;
uniform float Strength;

out vec4 fs_Color;

void main(void)
{
	float diffuse = max(0.0, dot(vs_fs_Normal, LightDirection));
	float specular = max(0.0, dot(vs_fs_Normal, HalfVector));

	if(diffuse == 0.0)
	{
		specular = 0.0;
	}
	else
	{
		specular = pow(specular, Shininess) * Strength;
	}

	vec3 scatteredLight = Ambient + LightColor * diffuse * Attenuation;
	vec3 reflectedLight = LightColor * specular * Attenuation;
	vec3 rgb = min(vs_fs_Color * scatteredLight + reflectedLight, vec3(1.0));

	fs_Color = vec4(rgb, vs_fs_Color.a);
}