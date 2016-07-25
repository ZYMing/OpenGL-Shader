//-------仅仅包含环境光的片元着色器------

#version 330 core

in vec4 vs_fs_Color;	//顶点着色器传到片元着色器的颜色值

uniform vec4 Ambient;	//环境光的光照大小

out vec4 fs_Color;

void main(void)
{
	vec4 scatteredLight = Ambient;
	fs_Color = min(vs_fs_Color * scatteredLight, vec4(1.0));

	//vec3 scatteredLight = vec3(Ambient);
	//vec3 rgb = min(vs_fs_Color.rgb * scatteredLight, vec3(1.0));
	//fs_Color = vec4(rgb, vs_fs_Color.a);
}