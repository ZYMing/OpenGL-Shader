//-------���������������ƬԪ��ɫ��------

#version 330 core

in vec4 vs_fs_Color;	//������ɫ������ƬԪ��ɫ������ɫֵ

uniform vec4 Ambient;	//������Ĺ��մ�С

out vec4 fs_Color;

void main(void)
{
	vec4 scatteredLight = Ambient;
	fs_Color = min(vs_fs_Color * scatteredLight, vec4(1.0));

	//vec3 scatteredLight = vec3(Ambient);
	//vec3 rgb = min(vs_fs_Color.rgb * scatteredLight, vec3(1.0));
	//fs_Color = vec4(rgb, vs_fs_Color.a);
}