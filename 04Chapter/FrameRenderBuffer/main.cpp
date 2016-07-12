#include <iostream>

#include <vgl.h>

#include <LoadShaders.h>

enum VAO_IDs{ Triangles, NumVAOs};

enum Buffer_IDs { ArrayBuffer, NumBuffers};

enum RenderBuffer_IDs{Color, NumRenderBuffers};

enum Attrib_IDs { vPosition};

GLuint Buffers[NumBuffers];

GLuint VAOs[NumVAOs];

GLuint frameBuffer;

GLuint renderBuffers[NumRenderBuffers];

const GLint numVertices = 6;

void init(void)
{
	glClearColor(0.0, 0.0, 0.0, 1.0);

	glGenVertexArrays(NumVAOs, VAOs);
	glBindVertexArray(VAOs[Triangles]);

	GLfloat vertices[numVertices][2] = {
		{ -0.90, -0.90 },
		{ 0.85, -0.90 },
		{ -0.90, 0.85 },
		{ 0.90, -0.85 },
		{ 0.90, 0.90 },
		{ -0.85, 0.90 }
	};

	glGenBuffers(NumBuffers, Buffers);
	glBindBuffer( GL_ARRAY_BUFFER, Buffers[ArrayBuffer]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//生成应用程序的帧缓存对象并绑定
	glGenFramebuffers(1, &frameBuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frameBuffer);

	//生成渲染缓存对象用作颜色信息
	//将其绑定，并分配图像数据空间
	glGenRenderbuffers(NumRenderBuffers, renderBuffers);
	glBindRenderbuffer(GL_RENDERBUFFER, renderBuffers[Color]);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA, 512, 512);

	//将渲染缓存关联到当前绑定的帧缓存对象的附件上
	glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, renderBuffers[Color]);

	ShaderInfo shaders[] = {
		{ GL_VERTEX_SHADER, "triangles.vert" },
		{ GL_FRAGMENT_SHADER, "triangles.frag" },
		{ GL_NONE, NULL }
	};

	GLuint program = LoadShaders(shaders);

	glUseProgram(program);

	glVertexAttribPointer(vPosition, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(vPosition);
}

void displayFunc(void)
{
	//准备渲染到应用程序的渲染缓存中
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frameBuffer);
	glViewport(0, 0, 512, 512);

	glClear(GL_COLOR_BUFFER_BIT);

	glBindVertexArray(VAOs[Triangles]);
	glDrawArrays(GL_TRIANGLES, 0, numVertices);

	//设置从应用程序的帧缓存附件的渲染缓存中读取，然后绘制到窗口系统的帧缓存中
	glBindFramebuffer(GL_READ_FRAMEBUFFER, frameBuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	//需要在渲染之前为每一个帧缓存重设视口
	//应用程序的帧缓存大小可能与窗口系统提供的帧缓存大小不一致
	glViewport(0, 0, 512, 512);

	glClearColor(0.0, 0.5, 0.5, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	//执行拷贝操作
	glBlitFramebuffer(0, 0, 511, 511, 0, 0, 511, 511, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	glFlush();
}

int main(int argc, char ** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA);
	glutInitWindowSize(512, 512);
	glutCreateWindow("FrameBuffer");

	if (glewInit())
	{
		std::cerr << "Unable to initalize GLEW!  Exit..." << std::endl;
		return 0;
	}

	init();
	glutDisplayFunc(displayFunc);
	glutMainLoop();
}
