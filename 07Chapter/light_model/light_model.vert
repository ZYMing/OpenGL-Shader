#version 430 core

in vec4 position;
in vec3 normal;

uniform mat4 model_matrix;
uniform mat4 proj_matrix;

out vec3 vs_worldpos;
out vec3 vs_normal;

void main()
{
	vec4 position = proj_matrix * model_matrix * position;
	gl_Position = position;
	vs_worldpos = position.xyz;
	vs_normal = mat3(model_matrix) * normal;
}