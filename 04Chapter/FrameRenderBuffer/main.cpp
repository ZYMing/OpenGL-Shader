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

	//����Ӧ�ó����֡������󲢰�
	glGenFramebuffers(1, &frameBuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frameBuffer);

	//������Ⱦ�������������ɫ��Ϣ
	//����󶨣�������ͼ�����ݿռ�
	glGenRenderbuffers(NumRenderBuffers, renderBuffers);
	glBindRenderbuffer(GL_RENDERBUFFER, renderBuffers[Color]);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA, 512, 512);

	//����Ⱦ�����������ǰ�󶨵�֡�������ĸ�����
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
	//׼����Ⱦ��Ӧ�ó������Ⱦ������
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frameBuffer);
	glViewport(0, 0, 512, 512);

	glClear(GL_COLOR_BUFFER_BIT);

	glBindVertexArray(VAOs[Triangles]);
	glDrawArrays(GL_TRIANGLES, 0, numVertices);

	//���ô�Ӧ�ó����֡���渽������Ⱦ�����ж�ȡ��Ȼ����Ƶ�����ϵͳ��֡������
	glBindFramebuffer(GL_READ_FRAMEBUFFER, frameBuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	//��Ҫ����Ⱦ֮ǰΪÿһ��֡���������ӿ�
	//Ӧ�ó����֡�����С�����봰��ϵͳ�ṩ��֡�����С��һ��
	glViewport(0, 0, 512, 512);

	glClearColor(0.0, 0.5, 0.5, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	//ִ�п�������
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
