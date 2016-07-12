#include <vgl.h>
#include <LoadShaders.h>
#include <vmath.h>

#include <iostream>

enum VAO_IDs {Triangles, Num_VAOs};

enum BUFFER_IDs {ArrayBuffer, ElementBuffer, Num_Buffers};

enum Attribute_IDs {vPosition, vColor};

GLuint VAOs[Num_VAOs];

GLuint Buffers[Num_Buffers];

GLuint program;

void init(void)
{
	// 8 corners of a cube, side length 2, centered on the origin
	static const GLfloat cube_positions[] =
	{
		-1.0f, -1.0f, -1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, -1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, -1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, -1.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f
	};

	// Color for each vertex
	static const GLfloat cube_colors[] =
	{
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f, 1.0f,
		0.5f, 0.5f, 0.5f, 1.0f
	};

	// Indices for the triangle strips
	static const GLushort cube_indices[] =
	{
		0, 1, 2, 3, 6, 7, 4, 5,         // First strip
		0xFFFF,                         // <<-- This is the restart index
		2, 6, 0, 4, 1, 5, 3, 7          // Second strip
	};

	glGenVertexArrays(Num_VAOs, VAOs);
	glBindVertexArray(VAOs[Triangles]);

	glGenBuffers(Num_Buffers, Buffers);
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[ArrayBuffer]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_positions)+sizeof(cube_colors), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(cube_positions), cube_positions);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(cube_positions), sizeof(cube_colors), cube_colors);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Buffers[ElementBuffer]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);

	ShaderInfo shader[] = {
		{ GL_VERTEX_SHADER, "primitive_restart.vert" },
		{ GL_FRAGMENT_SHADER, "primitive_restart.frag" },
		{ GL_NONE, NULL }
	};

	program = LoadShaders(shader);
	glUseProgram(program);

	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(cube_positions)));
	glEnableVertexAttribArray(vPosition);
	glEnableVertexAttribArray(vColor);

	glClearColor(0.0, 0.0, 0.0, 1.0);

}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(program);

	GLint model_matrix_loc = glGetUniformLocation(program, "model_matrix");
	GLint projection_matrix_loc = glGetUniformLocation(program, "projection_matrix");

	float t = float(GetTickCount() & 0x1FFF) / float(0x1FFF);
	static float q = 0.0f;
	float aspect = 1.0f;
	static const vmath::vec3 X(1.0f, 0.0f, 0.0f);
	static const vmath::vec3 Y(0.0f, 1.0f, 0.0f);
	static const vmath::vec3 Z(0.0f, 0.0f, 1.0f);

	vmath::mat4 model_matrix(vmath::translate(0.0f, 0.0f, -5.0f) * vmath::rotate(t * 360.0f, Y) * vmath::rotate(t * 720.0f, Z));
	vmath::mat4 projection_matrix(vmath::frustum(-1.0f, 1.0f, -aspect, aspect, 1.0f, 500.0f));

	glUniformMatrix4fv(model_matrix_loc, 1, GL_FALSE, model_matrix);
	glUniformMatrix4fv(projection_matrix_loc, 1, GL_FALSE, projection_matrix);

	glBindVertexArray(VAOs[Triangles]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Buffers[ElementBuffer]);

#if USE_PRIMITIVE_RESTART
	glEnable(GL_PRIMITIVE_RESTART);
	glPrimitiveRestartIndex(0xFFFF);
	glDrawElements(GL_TRIANGLE_STRIP, 17, GL_UNSIGNED_SHORT, NULL);

#else 
	glDrawElements(GL_TRIANGLE_STRIP, 8, GL_UNSIGNED_SHORT, NULL);
	glDrawElements(GL_TRIANGLE_STRIP, 8, GL_UNSIGNED_SHORT, BUFFER_OFFSET(9 * sizeof(GLushort)));
#endif

	glFlush();
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA);
	glutInitWindowSize(512, 512);
	glutCreateWindow("Primitive Restart");

	if (glewInit())
	{
		std::cerr << "Unable to initialize GLEW... Exiting!" << std::endl;
		return 0;
	}

	init();

	glutDisplayFunc(display);

	glutMainLoop();
}