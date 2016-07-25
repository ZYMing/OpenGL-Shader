//------包含聚光灯的片元着色器------
#version 330 core

in vec4 vs_fs_Color;
in vec4 vs_fs_Position;
in vec3 vs_fs_Normal;

uniform vec3 Ambient;		//环境光大小
uniform vec3 LightColor;	//聚光灯大小
uniform vec3 LightPosition;	//光源位置
uniform float Shininess;	//高光光锐程度的指数值
uniform float Strength;		//用于调整光泽度的数据

uniform vec3 EyeDirection;		//视点方向
uniform float ConstantAttenuation;		//常亮衰减系数
uniform float LinearAttenuation;		//线性衰减系数
uniform float QuadraticAttenuation;		//平方衰减系数

uniform vec3 ConeDirection;		//聚光灯属性-->聚光灯方向
uniform float SpotCosCutoff;	//聚光灯的大小，余弦值
uniform float SpotExponent;		//聚光灯的衰减系数

out vec4 fs_Color;

void main(void)
{
	vec3 lightDirection = LightPosition - vec3(vs_fs_Position);
	float lightDistance = length(lightDirection);
	lightDirection = lightDirection / lightDistance;

	float attenuation = 1.0 / (ConstantAttenuation + 
				LinearAttenuation * lightDistance + 
				QuadraticAttenuation * lightDistance * lightDistance);

	//计算与聚光灯的距离，是否在聚光灯所照亮的范围内
	float spotCos = dot(lightDirection, -ConeDirection);

	if(spotCos < SpotCosCutoff)
	{
		attenuation = 0.0;
	}
	else
	{
		attenuation *= pow(spotCos, SpotExponent);
	}

	vec3 halfVector = normalize(lightDirection + EyeDirection);

	float diffuse = max(0.0, dot(lightDirection, vs_fs_Normal));
	float specular = max(0.0, dot(vs_fs_Normal, halfVector));

	if(diffuse == 0.0)
	{
		specular = 0.0;
	}
	else 
	{
		specular = pow(specular, Shininess) * Strength;
	}

	//计算散射光及反射光
	vec3 scatteredLight = Ambient + LightColor * diffuse * attenuation;
	vec3 reflectedLight = LightColor * specular * attenuation;
	vec3 rgb = min(vs_fs_Color.rgb * scatteredLight + reflectedLight, vec3(1.0));

	fs_Color = vec4(rgb, vs_fs_Color.a);
}