#include <vermilion.h>

#include <LoadShaders.h>
#include <vbm.h>
#include <vmath.h>

#include <iostream>

GLuint tex_prog;

enum BUFFER_IDs { ARRAYBUFFER, PIXELUNPACK, NUMBUFFERS };
enum ATTRIB_IDs { in_Position, in_Texture };
enum VAO_IDs { QUAD, NUMVAOS };
enum TEXTURE_IDs { CHECKBOARD, COLOR, NUMTEXTURES };

GLuint Buffers[NUMBUFFERS];

GLuint Vaos[NUMVAOS];

GLuint Textures[NUMTEXTURES];

void init()
{
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClearDepth(1.0);

	ShaderInfo Shaders[] = {
		{ GL_VERTEX_SHADER, "Multi_Texture.vert" },
		{ GL_FRAGMENT_SHADER, "Multi_Texture.frag" },
		{ GL_NONE, NULL }
	};

	tex_prog = LoadShaders(Shaders);

	//���ö������ݣ��������꣨ע�⣺��������ķ�Χ��
	static const GLfloat quad_data[] =
	{
		1.0f, -1.0f,
		-1.0f, -1.0f,
		-1.0f, 1.0f,
		1.0f, 1.0f,

		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f
	};				//�Ķ������������1.0����2.0�����޸��������WRAP��ʽ�ɻ�ò�ͬ��Ч��

	glGenBuffers(NUMBUFFERS, Buffers);
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[ARRAYBUFFER]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quad_data), quad_data, GL_STATIC_DRAW);

	glGenVertexArrays(NUMVAOS, Vaos);
	glBindVertexArray(Vaos[QUAD]);

	glVertexAttribPointer(in_Position, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glVertexAttribPointer(in_Texture, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(8 * sizeof(float)));

	glEnableVertexAttribArray(in_Position);
	glEnableVertexAttribArray(in_Texture);


	/*
	** ʹ�ó����е����ݼ��ؽ��������
	*/

	//���þ�̬��������
	static const GLfloat texture_data_rgb[] =
	{
		1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f
	};

	static const GLfloat texture_data_alpha[] =
	{
		0.2f, 0.2f, 0.2f, 0.2f, 0.2f, 0.2f, 0.2f, 0.2f,
		0.2f, 0.2f, 0.2f, 0.2f, 0.2f, 0.2f, 0.2f, 0.2f,
		0.2f, 0.2f, 0.2f, 0.2f, 0.2f, 0.2f, 0.2f, 0.2f,
		0.2f, 0.2f, 0.2f, 0.2f, 0.2f, 0.2f, 0.2f, 0.2f,
		0.2f, 0.2f, 0.2f, 0.2f, 0.2f, 0.2f, 0.2f, 0.2f,
		0.2f, 0.2f, 0.2f, 0.2f, 0.2f, 0.2f, 0.2f, 0.2f,
		0.2f, 0.2f, 0.2f, 0.2f, 0.2f, 0.2f, 0.2f, 0.2f,
		0.2f, 0.2f, 0.2f, 0.2f, 0.2f, 0.2f, 0.2f, 0.2f,
	};

	//����������д�뵽GL_PIXEL_UNPACK_BUFFER������
	/*glBindBuffer(GL_PIXEL_UNPACK_BUFFER, Buffers[PIXELUNPACK]);
	glBufferData(GL_PIXEL_UNPACK_BUFFER, sizeof(texture_data_rgb), texture_data_rgb, GL_STATIC_DRAW);*/

	//���ɲ������������������ݣ����ַ�����
	//ֱ����ʾ������������glTexSubImage2D���������һ������Ϊtexture_data
	//ʹ��PIXEL_UNPACK����������������glTexSubImage2D�����һ������ΪNULL
	glGenTextures(NUMTEXTURES, Textures);
	glBindTexture(GL_TEXTURE_2D, Textures[CHECKBOARD]);
	glTexStorage2D(GL_TEXTURE_2D, 4, GL_RGBA32F, 8, 8);
	//glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 8, 8, GL_RED, GL_UNSIGNED_BYTE, texture_data);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 8, 8, GL_RED, GL_FLOAT, texture_data_rgb);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 8, 8, GL_ALPHA, GL_FLOAT, texture_data_alpha);

	//�����������FILTER�˲���ʽ�� WRAP��ʽ
	static const GLint swizzles[] = { GL_RED, GL_RED, GL_RED, GL_ALPHA };
	glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzles);
	/*glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);*/
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	////����Mipmap
	glGenerateMipmap(GL_TEXTURE_2D);


	static const GLfloat tex_color_rgba[] =
	{
		1.0f, 0.0f, 0.0f, 0.5f,		0.0f, 1.0f, 0.0f, 0.5f,
		0.0f, 0.0f, 1.0f, 0.5f,		1.0f, 1.0f, 0.0f, 0.5f
	};

	glBindTexture(GL_TEXTURE_2D, Textures[COLOR]);
	glTexStorage2D(GL_TEXTURE_2D, 2, GL_RGBA32F, 2, 2);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 2, 2, GL_RGBA, GL_FLOAT, tex_color_rgba);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void DisplayFunc(void)
{
	float t = float(GetTickCount() & 0x3FFF) / float(0x3FFF);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_CULL_FACE);

	glUseProgram(tex_prog);

	glBindVertexArray(Vaos[QUAD]);

	GLint tex1_uniform_Loc = glGetUniformLocation(tex_prog, "tex1");
	GLint tex2_uniform_Loc = glGetUniformLocation(tex_prog, "tex2");
	GLint time_uniform_Loc = glGetUniformLocation(tex_prog, "time");

	glUniform1i(tex1_uniform_Loc, 0);
	glUniform1i(tex2_uniform_Loc, 1);
	glUniform1f(time_uniform_Loc, t);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Textures[CHECKBOARD]);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, Textures[COLOR]);

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	glFlush();

	glutPostRedisplay();
}

int main(int argc, char ** argv)
{
	glutInit(&argc, argv);
	glutInitWindowSize(512, 512);
	glutInitDisplayMode(GLUT_RGBA);
	glutCreateWindow("Load Texture");

	if (glewInit())
	{
		std::cerr << "Unable to initialize GLEW ... exiting" << std::endl;
		return 0;
	}

	init();

	glutDisplayFunc(DisplayFunc);
	glutMainLoop();
}