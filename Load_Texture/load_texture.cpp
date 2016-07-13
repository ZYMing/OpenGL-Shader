#include <vermilion.h>

#include <LoadShaders.h>
#include <vbm.h>
#include <vmath.h>

#include <iostream>

GLuint tex_prog;

enum BUFFER_IDs {ARRAYBUFFER, PIXELUNPACK, NUMBUFFERS};
enum ATTRIB_IDs {in_Position, in_Texture};
enum VAO_IDs {QUAD, NUMVAOS};
enum TEXTURE_IDs {CHECKBOARD, IMAGE, NUMTEXTURES};

GLuint Buffers[NUMBUFFERS];

GLuint Vaos[NUMVAOS];

GLuint Textures[NUMTEXTURES];

void init()
{
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClearDepth(1.0);

	ShaderInfo Shaders[] = {
		{ GL_VERTEX_SHADER, "load_texture.vert" },
		{ GL_FRAGMENT_SHADER, "load_texture.frag" },
		{ GL_NONE, NULL}
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
		2.0f, 0.0f,
		2.0f, 2.0f,
		0.0f, 2.0f
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
	** �������ݽ������������ַ�ʽ��ʹ�ó����е����ݣ�ʹ�ô����е�ͼ������
	** ʹ�ó����е�����ʱglTexSubImage2D������data���������ֽ���
	*/

	/*
	** ʹ�ó����е����ݼ��ؽ��������
	*/
	//{
	//	//���þ�̬��������
	//	static const unsigned char texture_data[] =
	//	{
	//		0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00,
	//		0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF,
	//		0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00,
	//		0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF,
	//		0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00,
	//		0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF,
	//		0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00,
	//		0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF
	//	};

	//	static const unsigned char texture_data_alpha[] =
	//	{
	//		0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F,
	//		0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F,
	//		0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F,
	//		0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F,
	//		0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F,
	//		0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F,
	//		0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F,
	//		0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F,
	//	};

	//	//����������д�뵽GL_PIXEL_UNPACK_BUFFER������
	//	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, Buffers[PIXELUNPACK]);
	//	glBufferData(GL_PIXEL_UNPACK_BUFFER, sizeof(texture_data), texture_data, GL_STATIC_DRAW);

	//	//���ɲ������������������ݣ����ַ�����
	//	//ֱ����ʾ������������glTexSubImage2D���������һ������Ϊtexture_data
	//	//ʹ��PIXEL_UNPACK����������������glTexSubImage2D�����һ������ΪNULL
	//	glGenTextures(NUMTEXTURES, Textures);
	//	glBindTexture(GL_TEXTURE_2D, Textures[CHECKBOARD]);
	//	glTexStorage2D(GL_TEXTURE_2D, 4, GL_RGBA8, 8, 8);
	//	//glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 8, 8, GL_RED, GL_UNSIGNED_BYTE, texture_data);
	//	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 8, 8, GL_RED, GL_UNSIGNED_BYTE, NULL);
	//	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 8, 8, GL_ALPHA, GL_UNSIGNED_BYTE, texture_data_alpha);

	//	//�����������FILTER�˲���ʽ�� WRAP��ʽ
	//	static const GLint swizzles[] = { GL_RED, GL_RED, GL_RED, GL_ALPHA };
	//	glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzles);
	//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	//	//����Mipmap
	//	glGenerateMipmap(GL_TEXTURE_2D);
	//}

	/*
	** ʹ�ô����е����ݼ��ؽ��������
	*/
	{
		vglImageData Image;

		Textures[IMAGE] = vglLoadTexture("../../media/test3.dds", 0, &Image);

		glTexParameteri(Image.target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

		vglUnloadImage(&Image);
	}
}

void DisplayFunc(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_CULL_FACE);

	glUseProgram(tex_prog);

	glBindVertexArray(Vaos[QUAD]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	glFlush();
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