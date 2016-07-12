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
	/*创建粒子模拟系统的粒子运动着色器*/

	//创建着色器程序
	update_prog = glCreateProgram();

	//第一个三角形的顶点着色器代码
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

	//第一个三角形的片元着色器代码
	//设置颜色为蓝色
	static const char white_fs[] =
		"#version 330\n"
		"\n"
		"out vec4 color;\n"
		"\n"
		"void main(void)\n"
		"{\n"
		"    color = vec4(0.0, 0.0, 1.0, 1.0);\n"
		"}\n";

	//添加着色器代码到粒子运动着色器程序中
	//创建着色器对象--添加着色器代码--编译着色器--关联着色器到程序--删除着色器对象
	vglAttachShaderSource(update_prog, GL_VERTEX_SHADER, update_vs_source);
	vglAttachShaderSource(update_prog, GL_FRAGMENT_SHADER, white_fs);

	//设置Transform FeedBack要记录的变量名
	static const char * varyings[] = {
		"out_position"
	};

	//调用glTransformFeedbackVaryings函数进行设置
	glTransformFeedbackVaryings(update_prog, 1, varyings, GL_INTERLEAVED_ATTRIBS);
	//调用glTransformFeedbackVaryings后重新链接程序
	glLinkProgram(update_prog);
	glUseProgram(update_prog);


	/*创建第二个三角形的着色器*/

	//创建着色器程序
	render_prog = glCreateProgram();

	//第二个三角形的顶点着色器
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

	//第二个三角形的片元着色器
	static const char blue_fs[] =
		"#version 330\n"
		"\n"
		"out vec4 color;\n"
		"\n"
		"void main(void)\n"
		"{\n"
		"    color = vec4(1.0, 0.0, 0.0, 1.0);\n"
		"}\n";

	//添加着色器代码到几何运动着色器程序中
	//创建着色器对象--添加着色器代码--编译着色器--关联着色器到程序--删除着色器对象
	vglAttachShaderSource(render_prog, GL_VERTEX_SHADER, render_vs);
	vglAttachShaderSource(render_prog, GL_FRAGMENT_SHADER, blue_fs);

	//调用glTransformFeedbackVaryings后重新链接着色器程序
	glLinkProgram(render_prog);
	glUseProgram(render_prog);


	//生成顶点数组与缓存对象
	glGenVertexArrays(2, vao);
	glGenBuffers(2, vbo);

	/* 绑定Transform Feedback缓存到生成的缓存对象上并为其分配存储空间设置为GL_DYNAMIC_COPY
	*  第一个缓存中存储第一个三角形的位置坐标，第二个缓存则保存第一个三角形着色器中返回的信息
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


	//调用第一个三角形的着色器程序
	glUseProgram(update_prog);

	//绑定顶点属性，画出第一个三角形
	glBindVertexArray(vao[0]);

	//绑定Transform Feedback 缓存，将返回的顶点信息保存到第二个缓存中
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, vbo[1]);

	//启用Transform Feedback，GL_TRIANLES为准备绘制的图元类型
	glBeginTransformFeedback(GL_TRIANGLES);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glEndTransformFeedback();

	//调用第二个三角形着色器程序，为其uniform变量设置数据值
	glUseProgram(render_prog);
	
	//绑定顶点属性，画出第二个三角形
	glBindVertexArray(vao[1]);

	//设置uniform变量，平移第二个三角形
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