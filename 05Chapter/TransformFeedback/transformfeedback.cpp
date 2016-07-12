#include <vutils.h>
#include <vmath.h>
#include <vbm.h>

#include <iostream>

float aspect;

GLuint update_prog;

GLuint render_prog;

GLuint vbo[2];
GLuint vao[2];

const int point_count = 3;

void init(void)
{
	/*��������ģ��ϵͳ�������˶���ɫ��*/

	//������ɫ������
	update_prog = glCreateProgram();

	//��һ�������εĶ�����ɫ������
	static const char update_vs_source[] =
		"#version 330\n"
		"\n"
		"in vec4 position;\n"
		"\n"
		"out vec4 out_position;\n"
		"\n"
		"void main()\n"
		"{"
		"	gl_Position = position;\n"
		"	out_position = position;\n"
		"}"
		"\n";

	//��һ�������ε�ƬԪ��ɫ������
	//������ɫΪ��ɫ
	static const char white_fs[] =
		"#version 330\n"
		"\n"
		"out vec4 color;\n"
		"\n"
		"void main(void)\n"
		"{\n"
		"    color = vec4(0.0, 0.0, 1.0, 1.0);\n"
		"}\n";

	//�����ɫ�����뵽�����˶���ɫ��������
	//������ɫ������--�����ɫ������--������ɫ��--������ɫ��������--ɾ����ɫ������
	vglAttachShaderSource(update_prog, GL_VERTEX_SHADER, update_vs_source);
	vglAttachShaderSource(update_prog, GL_FRAGMENT_SHADER, white_fs);

	//����Transform FeedBackҪ��¼�ı�����
	static const char * varyings[] = {
		"out_position"
	};

	//����glTransformFeedbackVaryings������������
	glTransformFeedbackVaryings(update_prog, 1, varyings, GL_INTERLEAVED_ATTRIBS);
	//����glTransformFeedbackVaryings���������ӳ���
	glLinkProgram(update_prog);
	glUseProgram(update_prog);


	/*�����ڶ��������ε���ɫ��*/

	//������ɫ������
	render_prog = glCreateProgram();

	//�ڶ��������εĶ�����ɫ��
	static const char render_vs[] =
		"#version 330\n"
		"\n"
		"in vec4 position;\n"
		"\n"
		"uniform mat4 model_matrix;\n"
		"\n"
		"void main(void)\n"
		"{\n"
		"    gl_Position = model_matrix * position;\n"
		"}\n";

	//�ڶ��������ε�ƬԪ��ɫ��
	static const char blue_fs[] =
		"#version 330\n"
		"\n"
		"out vec4 color;\n"
		"\n"
		"void main(void)\n"
		"{\n"
		"    color = vec4(1.0, 0.0, 0.0, 1.0);\n"
		"}\n";

	//�����ɫ�����뵽�����˶���ɫ��������
	//������ɫ������--�����ɫ������--������ɫ��--������ɫ��������--ɾ����ɫ������
	vglAttachShaderSource(render_prog, GL_VERTEX_SHADER, render_vs);
	vglAttachShaderSource(render_prog, GL_FRAGMENT_SHADER, blue_fs);

	//����glTransformFeedbackVaryings������������ɫ������
	glLinkProgram(render_prog);
	glUseProgram(render_prog);


	//���ɶ��������뻺�����
	glGenVertexArrays(2, vao);
	glGenBuffers(2, vbo);

	/* ��Transform Feedback���浽���ɵĻ�������ϲ�Ϊ�����洢�ռ�����ΪGL_DYNAMIC_COPY
	*  ��һ�������д洢��һ�������ε�λ�����꣬�ڶ��������򱣴��һ����������ɫ���з��ص���Ϣ
	*/
	for (int i = 0; i < 2; i++)
	{
		glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, vbo[i]);
		glBufferData(GL_TRANSFORM_FEEDBACK_BUFFER, point_count * (sizeof(vmath::vec4)), NULL, GL_DYNAMIC_COPY);
		if (i == 0)
		{
			vmath::vec4 *buffer = (vmath::vec4 *)glMapBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, GL_WRITE_ONLY);

			buffer[0] = vmath::vec4(-0.90, -0.45, 0.0, 1.0);
			buffer[1] = vmath::vec4(-0.90, 0.45, 0.0, 1.0);
			buffer[2] = vmath::vec4(0.0, -0.45, 0.0, 1.0);

			glUnmapBuffer(GL_TRANSFORM_FEEDBACK_BUFFER);
		}

		glBindVertexArray(vao[i]);
		glBindBuffer(GL_ARRAY_BUFFER, vbo[i]);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(0);
	}

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearDepth(1.0f);

}

static inline int min(int a, int b)
{
	return a < b ? a : b;
}

void displayFunc(void)
{
	glClear(GL_COLOR_BUFFER_BIT);


	//���õ�һ�������ε���ɫ������
	glUseProgram(update_prog);

	//�󶨶������ԣ�������һ��������
	glBindVertexArray(vao[0]);

	//��Transform Feedback ���棬�����صĶ�����Ϣ���浽�ڶ���������
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, vbo[1]);

	//����Transform Feedback��GL_TRIANLESΪ׼�����Ƶ�ͼԪ����
	glBeginTransformFeedback(GL_TRIANGLES);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glEndTransformFeedback();

	//���õڶ�����������ɫ������Ϊ��uniform������������ֵ
	glUseProgram(render_prog);
	
	//�󶨶������ԣ������ڶ���������
	glBindVertexArray(vao[1]);

	//����uniform������ƽ�Ƶڶ���������
	int modelmatrixLoc = glGetUniformLocation(render_prog, "model_matrix");
	vmath::mat4 modelmatrix(vmath::translate(0.90f, 0.0f, 0.0f));
	glUniformMatrix4fv(modelmatrixLoc, 1, GL_FALSE, modelmatrix);

	glDrawArrays(GL_TRIANGLES, 0, 3);

	glFlush();
}

void reshapeFunc(int width, int height)
{
	glViewport(0, 0, width, height);
	aspect = float(height) / float(width);
}

int main(int argc, char ** argv)
{
	glutInit(&argc, argv);
	glutInitWindowSize(512, 512);
	glutInitDisplayMode(GLUT_RGBA);
	glutCreateWindow("Transform Feedback");

	if (glewInit())
	{
		std::cerr << "Unable to initialize GLEW! Exit..." << std::endl;
		return 0;
	}

	init();
	glutDisplayFunc(displayFunc);
	glutReshapeFunc(reshapeFunc);

	glutMainLoop();
}