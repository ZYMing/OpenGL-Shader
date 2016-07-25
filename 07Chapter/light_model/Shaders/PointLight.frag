//------包含点光源的片元着色器------

#version 330 core

in vec4 vs_fs_Color;
in vec4 vs_fs_Position;
in vec3 vs_fs_Normal;

uniform vec3 Ambient;		//环境光大小
uniform vec3 LightColor;	//点光源大小
uniform vec3 LightPosition;		//点光源位置
uniform float Shininess;		//高光光锐程度的指数值
uniform float Strength;			//用于调整光泽度的数据

uniform vec3 EyeDirection;		//视点方向
uniform float ConstantAttenuation;		//常亮衰减系数
uniform float LinearAttenuation;		//线性衰减系数
uniform float QuadraticAttenuation;		//平方衰减系数

out vec4 fs_Color;

void main(void)
{
	vec3 lightDirection = LightPosition - vec3(vs_fs_Position);
	float lightDistance = length(LightDirection);
	//计算光源方向
	lightDirection = lightDirection / lightDistance;
	//计算衰减系数
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