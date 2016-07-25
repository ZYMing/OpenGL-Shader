//------包含多个光源与多个材质的片元着色器------

#version 330 core

struct LightProperties{
	bool isEnabled;			//是否启用
	bool isLocal;			//是否是点光源或聚光灯
	bool isSpot;			//是否是聚光灯
	vec3 ambient;			//环境光大小
	vec3 color;				//光源大小
	vec3 position;			//方向光时，为光源方向，点光源或聚光灯时为光源位置
	vec3 halfVector
	vec3 coneDirection;
	float spotCosCutoff;
	float spotExponent;
	float constantAttenuation;
	float linearAttenuation;
	float quadraticAttenuation;
}

const int MaxLight = 10;
uniform LightProperties Lights[MaxLight];

struct MaterialProperties{
	vec3 emission;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
	float strength;
}

const int MaxMaterials = 14;
uniform MaterialProperties Materials[MaxMaterial];
flat in int MaterIndex;

in vec4 vs_fs_Color;
in vec4 vs_fs_Position;
in vec3 vs_fs_Normal;

uniform float Shininess;
uniform float Strength;
uniform vec3 EyeDirection;

out vec4 fs_Color;

void main(void)
{
	vec3 scatteredLight = vec3(0.0);
	vec3 reflectedLight = vec3(0.0);

	for(int light = 0; light < MaxLight; ++light)
	{
		if(!Lights[light].isEnabled) {	continue; }

		vec3 halfVector;
		vec3 lightDirection = Lights[light].position;
		float attenuation = 1.0;

		if(Lights[light].isLocal)
		{
			lightDirection = lightDirection - vec3(vs_fs_Position);
			float lightDistance = length(lightDirection);
			lightDirection = lightDirection / lightDistance;

			attenuation = 1.0 / (Lights[light].constantAttenuation + 
							Lights[light].linearAttenuation * lightDistance + 
							Lights[light].quadraticAttenuation * lightDistance * lightDistance);

			if(Lights[light].isSpot)
			{
				float spotCos = dot(lightDirection, - Lights[light].coneDirection);
				if(spotCos < Lights[light].spotCosCutoff)
				{
					attenuation = 0.0;
				}
				else
				{
					attenuation *= pow(spotCos, Lights[light].spotExponent);
				}
			}

			halfVector = normalize(lightDirection + EyeDirection);
		}
		else
		{
			halfVector = Lights[light].halfVector;
		}

		float diffuse = max(0.0, dot(vs_fs_Normal, lightDirection));
		float specular = max(0.0, dot(vs_fs_Normal, halfVector));

		if(diffuse == 0.0)
		{
			specular = 0.0;
		}
		else
		{
			specular = pow(specular, Materials[MaterIndex].shininess) * Materials[MaterIndex].strength;
		}

		scatteredLight += Lights[light].ambient * Materials[MaterIndex].ambient * attenuation + 
							Lights[light].color * Materials[MaterIndex].diffuse * diffuse * attenuation;
		reflectedLight += Lights[light].color * Materials[MaterIndex].specular * specular * attenuation;
	}

	vec3 rgb = min(Materials[MaterIndex].emission + vs_fs_Color.rgb * scatteredLight + reflectedLight, vec3(1.0));

	fs_Color = vec4(rgb, vs_fs_Color.a);
}