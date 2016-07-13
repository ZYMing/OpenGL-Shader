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

	//设置顶点数据，纹理坐标（注意：纹理坐标的范围）
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
	};				//改动纹理坐标大于1.0（如2.0），修改纹理参数WRAP方式可获得不同的效果

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
	** 加载数据进纹理对象的两种方式：使用程序中的数据，使用磁盘中的图像数据
	** 使用程序中的数据时glTexSubImage2D函数的data参数有两种解释
	*/

	/*
	** 使用程序中的数据加载进纹理对象
	*/
	//{
	//	//设置静态纹理数据
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

	//	//将纹理数据写入到GL_PIXEL_UNPACK_BUFFER缓存中
	//	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, Buffers[PIXELUNPACK]);
	//	glBufferData(GL_PIXEL_UNPACK_BUFFER, sizeof(texture_data), texture_data, GL_STATIC_DRAW);

	//	//生成并绑定纹理，设置纹理数据（两种方法）
	//	//直接显示设置纹理数据glTexSubImage2D函数的最后一个参数为texture_data
	//	//使用PIXEL_UNPACK缓存设置纹理数据glTexSubImage2D的最后一个参数为NULL
	//	glGenTextures(NUMTEXTURES, Textures);
	//	glBindTexture(GL_TEXTURE_2D, Textures[CHECKBOARD]);
	//	glTexStorage2D(GL_TEXTURE_2D, 4, GL_RGBA8, 8, 8);
	//	//glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 8, 8, GL_RED, GL_UNSIGNED_BYTE, texture_data);
	//	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 8, 8, GL_RED, GL_UNSIGNED_BYTE, NULL);
	//	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 8, 8, GL_ALPHA, GL_UNSIGNED_BYTE, texture_data_alpha);

	//	//设置纹理参数FILTER滤波方式， WRAP方式
	//	static const GLint swizzles[] = { GL_RED, GL_RED, GL_RED, GL_ALPHA };
	//	glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzles);
	//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	//	//生成Mipmap
	//	glGenerateMipmap(GL_TEXTURE_2D);
	//}

	/*
	** 使用磁盘中的数据加载进纹理对象
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