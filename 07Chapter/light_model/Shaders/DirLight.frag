//------仅包含方向光的片元着色器------

#version 330 core

in vec4 vs_fs_Color;
in vec3 vs_fs_Normal;

uniform vec3 Ambient = (0.2, 0.2, 0.2);		//环境光大小
uniform vec3 LightColor;	//方向光大小
uniform vec3 LightDirection;	//光照方向
uniform vec3 HalfVector;		//光泽度的表面方向
uniform float Shininess = 20.0f;		//高光尖锐程度的指数值
uniform float Strength = 10.0f;			//用于调整光泽度的数据

out vec4 fs_Color;

void main(void)
{
	float diffuse = max(0.0, dot(vs_fs_Normal, LightDirection));
	float specular = max(0.0, dot(vs_fs_Normal, HalfVector));

	//如果方向光找不到，那么镜面反射也是不可能的
	if(diffuse == 0.0)
	{
		specular = 0.0;
	}
	else
	{
		specular = pow(specular, Shininess);
	}

	vec3 scatteredLight = Ambient + diffuse * LightColor;		//计算散射光
	vec3 reflectedLight = specular * LightColor * Strength;		//计算反射光

	vec3 rgb = min(Color.rgb * scatteredLight + reflectedLight, vec3(1.0));
	fs_Color = vec4(rgb, vs_fs_Color.a);
}