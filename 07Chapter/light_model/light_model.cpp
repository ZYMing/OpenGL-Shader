#include <vgl.h>
#include <LoadShaders.h>
#include <vbm.h>
#include <vmath.h>

#include <iostream>

GLuint light_prog;

GLint model_mat_loc;
GLint proj_mat_loc;
GLint color_ambient_loc;
GLint color_diffuse_loc;
GLint color_specular_loc;
GLint shininess_loc;
GLint light_pos_loc;

VBObject object;

void Init(void)
{
	ShaderInfo shaders[] = {
		{ GL_VERTEX_SHADER, "light_model.vert" },
		{ GL_FRAGMENT_SHADER, "light_model.frag" },
		{ GL_NONE, NULL}
	};

	light_prog = LoadShaders(shaders);

	model_mat_loc = glGetUniformLocation(light_prog, "model_matrix");
	proj_mat_loc = glGetUniformLocation(light_prog, "proj_matrix");
	color_ambient_loc = glGetUniformLocation(light_prog, "color_ambient");
	color_diffuse_loc = glGetUniformLocation(light_prog, "color_diffuse");
	color_specular_loc = glGetUniformLocation(light_prog, "color_specular");
	shininess_loc = glGetUniformLocation(light_prog, "shininess");
	light_pos_loc = glGetUniformLocation(light_prog, "light_position");

	object.LoadFromVBM("../../media/unit_torus.vbm", 0, 1, 2);
}

void DisplayFunc(void)
{
	float time = float(GetTickCount() & 0xFFFF) / float(0xFFFF);

	vmath::mat4 model_matrix = vmath::translate(0.0f, 0.0f, -4.0f) *
		vmath::rotate(987.0f * time * 3.141592f, vmath::vec3(0.0f, 0.0f, 1.0f)) *
		vmath::rotate(1234.0f *time * 3.141592f, vmath::vec3(1.0f, 0.0f, 0.0f));
	vmath::mat4 proj_matrix = vmath::perspective(60.0f, 1.333, 0.1f, 1000.0f);

	glUseProgram(light_prog);

	glUniformMatrix4fv(model_mat_loc, 1, GL_FALSE, model_matrix);
	glUniformMatrix4fv(proj_mat_loc, 1, GL_FALSE, proj_matrix);
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	object.Render();

	glutPostRedisplay();
}

int main(int argc, char ** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA);
	glutInitWindowSize(512, 512);
	glutCreateWindow("Light_Model");

	if (glewInit())
	{
		std::cerr << "Unable to initilize GLEW!\n";
		return 0;
	}

	Init();

	glutDisplayFunc(DisplayFunc);
	glutMainLoop();
}