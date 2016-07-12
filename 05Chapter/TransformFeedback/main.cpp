#include <vutils.h>
#include <vmath.h>
#include <vbm.h>

#include <iostream>

float aspect;

GLuint update_prog;
GLuint model_matrix_loc;
GLuint projection_matrix_loc;
GLuint triangle_count_loc;
GLuint time_step_loc;

GLuint render_prog;
GLuint render_model_matrix_loc;
GLuint render_projection_matrix_loc;

GLuint vbo[2];
GLuint geometry_vbo;
GLuint vao[2];
GLuint render_vao;

GLuint geometry_tex;

VBObject object;

const int point_count = 5000;
static unsigned int seed = 0x13371337;

static inline float random_float()
{
	float res;
	unsigned int tmp;

	seed *= 16807;

	tmp = seed ^ (seed >> 4) ^ (seed << 15);

	*((unsigned int *)&res) = (tmp >> 9) | 0x3F800000;

	return (res - 1.0f);
}

static vmath::vec3 random_vector(float minmag = 0.0f, float maxmag = 1.0f)
{
	vmath::vec3 randomvec(random_float() * 2.0f - 1.0f, random_float() * 2.0f - 1.0f, random_float() * 2.0f - 1.0f);
	randomvec = normalize(randomvec);
	randomvec *= (random_float() * (maxmag - minmag) + minmag);

	return randomvec;
}

void init(void)
{
	/*��������ģ��ϵͳ�������˶���ɫ��*/

	//������ɫ������
	update_prog = glCreateProgram();

	//�����˶��Ķ�����ɫ������
	//��ײ����㷨----Ray-Triangle�㷨
	//��ײ�����㷨----��������
	static const char update_vs_source[] =
		"#version 410\n"
		"\n"
		"uniform mat4 model_matrix;\n"
		"uniform mat4 projection_matrix;\n"
		"uniform int triangle_count;\n"
		"\n"
		"layout (location = 0) in vec4 position;\n"
		"layout (location = 1) in vec3 velocity;\n"
		"\n"
		"out vec4 position_out;\n"
		"out vec3 velocity_out;\n"
		"\n"
		"uniform samplerBuffer geometry_tbo;\n"
		"uniform float time_step = 0.02;\n"
		"\n"
		"bool intersect(vec3 origin, vec3 direction, vec3 v0, vec3 v1, vec3 v2, out vec3 point)\n"
		"{\n"
		"    vec3 u, v, n;\n"
		"    vec3 w0, w;\n"
		"    float r, a, b;\n"
		"\n"
		"    u = (v1 - v0);\n"
		"    v = (v2 - v0);\n"
		"    n = cross(u, v);\n"
		// "    if (length(n) < 0.1)\n"
		// "        return false;\n"
		"\n"
		"    w0 = origin - v0;\n"
		"    a = -dot(n, w0);\n"
		"    b = dot(n, direction);\n"
		//"    if (abs(b) < 0.1)\n"
		//"        return false;\n"
		"\n"
		"    r = a / b;\n"
		"    if (r < 0.0 || r > 1.0)\n"
		"        return false;\n"
		"\n"
		"    point = origin + r * direction;\n"
		"\n"
		"    float uu, uv, vv, wu, wv, D;\n"
		"\n"
		"    uu = dot(u, u);\n"
		"    uv = dot(u, v);\n"
		"    vv = dot(v, v);\n"
		"    w = point - v0;\n"
		"    wu = dot(w, u);\n"
		"    wv = dot(w, v);\n"
		"    D = uv * uv - uu * vv;\n"
		"\n"
		"    float s, t;\n"
		"\n"
		"    s = (uv * wv - vv * wu) / D;\n"
		"    if (s < 0.0 || s > 1.0)\n"
		"        return false;\n"
		"    t = (uv * wu - uu * wv) / D;\n"
		"    if (t < 0.0 || (s + t) > 1.0)\n"
		"        return false;\n"
		"\n"
		"    return true;\n"
		"}\n"
		"\n"
		"vec3 reflect_vector(vec3 v, vec3 n)\n"
		"{\n"
		"    return v - 2.0 * dot(v, n) * n;\n"
		"}\n"
		"\n"
		"void main(void)\n"
		"{\n"
		"    vec3 accelleration = vec3(0.0, -0.3, 0.0);\n"
		"    vec3 new_velocity = velocity + accelleration * time_step;\n"
		"    vec4 new_position = position + vec4(new_velocity * time_step, 0.0);\n"
		"    vec3 v0, v1, v2;\n"
		"    vec3 point;\n"
		"    int i;\n"
		"    for (i = 0; i < triangle_count; i++)\n"
		"    {\n"
		"        v0 = texelFetch(geometry_tbo, i * 3).xyz;\n"
		"        v1 = texelFetch(geometry_tbo, i * 3 + 1).xyz;\n"
		"        v2 = texelFetch(geometry_tbo, i * 3 + 2).xyz;\n"
		"        if (intersect(position.xyz, position.xyz - new_position.xyz, v0, v1, v2, point))\n"
		"        {\n"
		"            vec3 n = normalize(cross(v1 - v0, v2 - v0));\n"
		"            new_position = vec4(point + reflect_vector(new_position.xyz - point, n), 1.0);\n"
		"            new_velocity = 0.8 * reflect_vector(new_velocity, n);\n"
		"        }\n"
		"    }\n"
		"    if (new_position.y < -40.0)\n"
		"    {\n"
		"        new_position = vec4(-new_position.x * 0.3, position.y + 80.0, 0.0, 1.0);\n"
		"        new_velocity *= vec3(0.2, 0.1, -0.3);\n"
		"    }\n"
		"    velocity_out = new_velocity * 0.9999;\n"
		"    position_out = new_position;\n"
		"    gl_Position = projection_matrix * (model_matrix * position);\n"
		"}\n";

	//�����˶���ƬԪ��ɫ������
	//������ɫΪ��ɫ
	static const char white_fs[] =
		"#version 410\n"
		"\n"
		"layout (location = 0) out vec4 color;\n"
		"\n"
		"void main(void)\n"
		"{\n"
		"    color = vec4(1.0);\n"
		"}\n";

	//�����ɫ�����뵽�����˶���ɫ��������
	//������ɫ������--�����ɫ������--������ɫ��--������ɫ��������--ɾ����ɫ������
	vglAttachShaderSource(update_prog, GL_VERTEX_SHADER, update_vs_source);
	vglAttachShaderSource(update_prog, GL_FRAGMENT_SHADER, white_fs);

	//����Transform FeedBackҪ��¼�ı�����
	static const char * varyings[] = {
		"position_out", "velocity_out"
	};

	//����glTransformFeedbackVaryings������������
	glTransformFeedbackVaryings(update_prog, 2, varyings, GL_INTERLEAVED_ATTRIBS);
	//����glTransformFeedbackVaryings���������ӳ���
	glLinkProgram(update_prog);
	glUseProgram(update_prog);

	//��ȡ��ɫ����uniform������λ��
	model_matrix_loc = glGetUniformLocation(update_prog, "model_matrix");
	projection_matrix_loc = glGetUniformLocation(update_prog, "projection_matrix");
	triangle_count_loc = glGetUniformLocation(update_prog, "triangle_count");
	time_step_loc = glGetUniformLocation(update_prog, "time_step");

	/*��������ģ��ϵͳ�ļ����˶���ɫ��*/

	//������ɫ������
	render_prog = glCreateProgram();

	//�����˶�����Ķ�����ɫ��
	//������������ϵ�еĶ����Feedback���������˶���ײ���
	static const char render_vs[] =
		"#version 410\n"
		"\n"
		"uniform mat4 model_matrix;\n"
		"uniform mat4 projection_matrix;\n"
		"\n"
		"layout (location = 0) in vec4 position;\n"
		"layout (location = 1) in vec3 normal;\n"
		"\n"
		"out vec4 world_space_position;\n"
		"\n"
		"out vec3 vs_fs_normal;\n"
		"\n"
		"void main(void)\n"
		"{\n"
		"    vec4 pos = (model_matrix * (position * vec4(1.0, 1.0, 1.0, 1.0)));\n"
		"    world_space_position = pos;\n"
		"    vs_fs_normal = normalize((model_matrix * vec4(normal, 0.0)).xyz);\n"
		"    gl_Position = projection_matrix * pos;\n"
		"}\n";

	//�����˶������ƬԪ��ɫ��
	//ע�ⷨ����������
	static const char blue_fs[] =
		"#version 410\n"
		"\n"
		"layout (location = 0) out vec4 color;\n"
		"\n"
		"in vec3 vs_fs_normal;\n"
		"\n"
		"void main(void)\n"
		"{\n"
		"    color = vec4(0.0, 0.2, 0.0, 1.0) + vec4(0.2, 0.5, 0.4, 1.0) * abs(vs_fs_normal.z) + vec4(0.8, 0.9, 0.7, 1.0) * pow(abs(vs_fs_normal.z), 70.0);\n"
		"}\n";

	//�����ɫ�����뵽�����˶���ɫ��������
	//������ɫ������--�����ɫ������--������ɫ��--������ɫ��������--ɾ����ɫ������
	vglAttachShaderSource(render_prog, GL_VERTEX_SHADER, render_vs);
	vglAttachShaderSource(render_prog, GL_FRAGMENT_SHADER, blue_fs);

	//����Ҫ��¼�Ķ�����ɫ�����صı�����
	static const char *varying2[] = {
		"world_space_position"
	};
	//����glTransformFeedbackVaryings������������
	glTransformFeedbackVaryings(render_prog, 1, varying2, GL_INTERLEAVED_ATTRIBS);

	//����glTransformFeedbackVaryings������������ɫ������
	glLinkProgram(render_prog);
	glUseProgram(render_prog);

	//��ȡuniform����λ��
	render_model_matrix_loc = glGetUniformLocation(render_prog, "model_matrix");
	render_projection_matrix_loc = glGetUniformLocation(render_prog, "projection_matrix");

	//���ɶ��������뻺�����
	glGenVertexArrays(2, vao);
	glGenBuffers(2, vbo);

	/* ��Transform Feedback���浽���ɵĻ�������ϲ�Ϊ�����洢�ռ�����ΪGL_DYNAMIC_COPY
	 * �������潻��ʹ�ã���ʹ�õ�һ�����������ʱ����Transform Feedback���ص�����д��ڶ������湩����ʹ��
	 * ��Ϊ��һ�����棨��vbo[0]����ʼ���������ݣ�λ�����ٶȣ����ڶ�����������ݽ��ں�����Ƶ�ʱ���Transform Feedback�з���
	 * ÿһ��������Ҫ�ٴΰ󶨵�GL_ARRAY_BUFFER�ϣ�����ϰ󶨶���������󣬷��������ɫ����������ʹ��
	 *		����ÿ����ɫ�����������ݶ��������������飬����Ӧ�Ķ������飬���ö�Ӧ������
	 */
	for (int i = 0; i < 2; i++)
	{
		glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, vbo[i]);
		glBufferData(GL_TRANSFORM_FEEDBACK_BUFFER, point_count * (sizeof(vmath::vec4) + sizeof(vmath::vec3)), NULL, GL_DYNAMIC_COPY);
		if (i == 0)
		{
			struct buffer_t {
				vmath::vec4 position;
				vmath::vec3 velocity;
			} *buffer = (buffer_t *)glMapBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, GL_WRITE_ONLY);

			for (int j = 0; j < point_count; j++)
			{
				buffer[j].velocity = random_vector();
				buffer[j].position = vmath::vec4(buffer[j].velocity + vmath::vec3(-0.5f, 40.0f, 0.0f), 1.0f);
				buffer[j].velocity = vmath::vec3(buffer[j].velocity[0], buffer[j].velocity[1] * 0.3f, buffer[j].velocity[2] * 0.3f);
			}

			glUnmapBuffer(GL_TRANSFORM_FEEDBACK_BUFFER);
		}

		glBindVertexArray(vao[i]);
		glBindBuffer(GL_ARRAY_BUFFER, vbo[i]);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(vmath::vec4) + sizeof(vmath::vec3), NULL);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vmath::vec4) + sizeof(vmath::vec3), (GLvoid *)sizeof(vmath::vec4));
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
	}

	/* ���ɼ����建����󣬴洢������Ķ������ݣ���������ռ��λ�����꣩
	 * ���ɻ���������������󶨵�һ��
	 * ����洢�ռ�ʱ����������ΪGL_DYNAMIC_COPY�����Զ�̬ʹ�ã�����Transform Feedback����������
	*/
	glGenBuffers(1, &geometry_vbo);
	glGenTextures(1, &geometry_tex);
	glBindBuffer(GL_TEXTURE_BUFFER, geometry_vbo);
	glBufferData(GL_TEXTURE_BUFFER, 1024 * 1024 * sizeof(vmath::vec4), NULL, GL_DYNAMIC_COPY);
	glBindTexture(GL_TEXTURE_BUFFER, geometry_tex);
	glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, geometry_vbo);
	/* ���ɶ������飬����
	 * �󶨻��浽GL_ARRAY_BUFFER
	 * ---ò��Transform Feedback���ص����ݶ���Ҫ�󶨻��浽GL_ARRAY_BUFFER����_����
	 */
	glGenVertexArrays(1, &render_vao);
	glBindVertexArray(render_vao);
	glBindBuffer(GL_ARRAY_BUFFER, geometry_vbo);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearDepth(1.0f);

	object.LoadFromVBM("../../media/armadillo_low.vbm", 0, 1, 2);
	
}

static inline int min(int a, int b)
{
	return a < b ? a : b;
}

void displayFunc(void)
{
	static int frame_count = 0;
	float t = float(GetTickCount() & 0x3FFFF) / float(0x3FFFF);
	static float q = 0.0f;
	static const vmath::vec3 X(1.0f, 0.0f, 0.0f);
	static const vmath::vec3 Y(0.0f, 1.0f, 0.0f);
	static const vmath::vec3 Z(0.0f, 0.0f, 1.0f);

	//����ͶӰ������ģ�;���
	vmath::mat4 projection_matrix(vmath::frustum(-1.0f, 1.0f, -aspect, aspect, 1.0f, 5000.0f) * vmath::translate(0.0f, 0.0f, -100.0f));
	vmath::mat4 model_matrix(vmath::scale(0.3f) *
		vmath::rotate(t * 360.0f, 0.0f, 1.0f, 0.0f) *
		vmath::rotate(t * 360.0f * 3.0f, 0.0f, 0.0f, 1.0f));

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	//���ü������˶���ɫ�����򣬲�Ϊ��uniform������������ֵ
	glUseProgram(render_prog);
	glUniformMatrix4fv(render_model_matrix_loc, 1, GL_FALSE, model_matrix);
	glUniformMatrix4fv(render_projection_matrix_loc, 1, GL_FALSE, projection_matrix);

	//---��仰��֪������ʲô���ã��Ͼ���Render�����������°��˶������飨��_����
	glBindVertexArray(render_vao);

	//��geometry_vbo�������󶨵�GL_TRANSFORM_FEEDBACK_BUFFER
	//����Transform Feedback�������ݿ���д�뻺�������
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, geometry_vbo);

	//����Transform Feedback��GL_TRIANLESΪ׼�����Ƶ�ͼԪ����
	glBeginTransformFeedback(GL_TRIANGLES);
	object.Render();
	glEndTransformFeedback();

	//���������˶���ɫ������Ϊ��uniform������������ֵ
	glUseProgram(update_prog);
	model_matrix = vmath::mat4::identity();
	glUniformMatrix4fv(model_matrix_loc, 1, GL_FALSE, model_matrix);
	glUniformMatrix4fv(projection_matrix_loc, 1, GL_FALSE, projection_matrix);
	glUniform1i(triangle_count_loc, object.GetVertexCount() / 3);

	if (t > q)
	{
		//���������˶�����
		glUniform1f(time_step_loc, (t - q) * 2000.0f);
	}

	q = t;

	/*�����ǰ����֡��Ϊż����
		  �󶨶�������vao[0]��������vao[0]�е�������Ϊ��������
		  ��Transform Feedback�Ļ������Ϊvbo[1]���������ص����ݴ������У����´λ���ʹ��
	  �����ǰ����֡��Ϊ������������ż���෴
	*/
	if ((frame_count & 1) != 0)
	{
		glBindVertexArray(vao[1]);
		glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, vbo[0]);
	}
	else
	{
		glBindVertexArray(vao[0]);
		glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, vbo[1]);
	}

	//����Transform Feedback�����Ʋ���¼�䶥����ɫ�����ص�����
	glBeginTransformFeedback(GL_POINTS);
	glDrawArrays(GL_POINTS, 0, min(point_count, (frame_count >> 3)));
	glEndTransformFeedback();

	glBindVertexArray(0);

	frame_count++;
	glutSwapBuffers();
	glutPostRedisplay();
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