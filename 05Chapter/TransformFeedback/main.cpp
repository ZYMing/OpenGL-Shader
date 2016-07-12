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
	/*创建粒子模拟系统的粒子运动着色器*/

	//创建着色器程序
	update_prog = glCreateProgram();

	//粒子运动的顶点着色器代码
	//碰撞检测算法----Ray-Triangle算法
	//碰撞弹回算法----向量运算
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

	//粒子运动的片元着色器代码
	//设置颜色为白色
	static const char white_fs[] =
		"#version 410\n"
		"\n"
		"layout (location = 0) out vec4 color;\n"
		"\n"
		"void main(void)\n"
		"{\n"
		"    color = vec4(1.0);\n"
		"}\n";

	//添加着色器代码到粒子运动着色器程序中
	//创建着色器对象--添加着色器代码--编译着色器--关联着色器到程序--删除着色器对象
	vglAttachShaderSource(update_prog, GL_VERTEX_SHADER, update_vs_source);
	vglAttachShaderSource(update_prog, GL_FRAGMENT_SHADER, white_fs);

	//设置Transform FeedBack要记录的变量名
	static const char * varyings[] = {
		"position_out", "velocity_out"
	};

	//调用glTransformFeedbackVaryings函数进行设置
	glTransformFeedbackVaryings(update_prog, 2, varyings, GL_INTERLEAVED_ATTRIBS);
	//调用glTransformFeedbackVaryings后重新链接程序
	glLinkProgram(update_prog);
	glUseProgram(update_prog);

	//获取着色器中uniform变量的位置
	model_matrix_loc = glGetUniformLocation(update_prog, "model_matrix");
	projection_matrix_loc = glGetUniformLocation(update_prog, "projection_matrix");
	triangle_count_loc = glGetUniformLocation(update_prog, "triangle_count");
	time_step_loc = glGetUniformLocation(update_prog, "time_step");

	/*创建粒子模拟系统的几何运动着色器*/

	//创建着色器程序
	render_prog = glCreateProgram();

	//几何运动程序的顶点着色器
	//返回世界坐标系中的顶点给Feedback用于粒子运动碰撞检测
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

	//几何运动程序的片元着色器
	//注意法向量的运算
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

	//添加着色器代码到几何运动着色器程序中
	//创建着色器对象--添加着色器代码--编译着色器--关联着色器到程序--删除着色器对象
	vglAttachShaderSource(render_prog, GL_VERTEX_SHADER, render_vs);
	vglAttachShaderSource(render_prog, GL_FRAGMENT_SHADER, blue_fs);

	//设置要记录的顶点着色器返回的变量名
	static const char *varying2[] = {
		"world_space_position"
	};
	//调用glTransformFeedbackVaryings函数进行设置
	glTransformFeedbackVaryings(render_prog, 1, varying2, GL_INTERLEAVED_ATTRIBS);

	//调用glTransformFeedbackVaryings后重新链接着色器程序
	glLinkProgram(render_prog);
	glUseProgram(render_prog);

	//获取uniform变量位置
	render_model_matrix_loc = glGetUniformLocation(render_prog, "model_matrix");
	render_projection_matrix_loc = glGetUniformLocation(render_prog, "projection_matrix");

	//生成顶点数组与缓存对象
	glGenVertexArrays(2, vao);
	glGenBuffers(2, vbo);

	/* 绑定Transform Feedback缓存到生成的缓存对象上并为其分配存储空间设置为GL_DYNAMIC_COPY
	 * 两个缓存交换使用：当使用第一个缓存的数据时，将Transform Feedback返回的数据写入第二个缓存供后续使用
	 * 先为第一个缓存（即vbo[0]）初始化粒子数据（位置与速度），第二个缓存的数据将在后面绘制的时候从Transform Feedback中返回
	 * 每一个缓存需要再次绑定到GL_ARRAY_BUFFER上，并配合绑定顶点数组对象，方便后续着色器输入数据使用
	 *		即，每次着色器的输入数据都是依靠顶点数组，绑定相应的顶点数组，利用对应的输入
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

	/* 生成几何体缓存对象，存储几何体的顶点数据（即其世界空间的位置坐标）
	 * 生成缓存与纹理，并将其绑定到一起。
	 * 分配存储空间时设置其类型为GL_DYNAMIC_COPY，可以动态使用，方便Transform Feedback返回其数据
	*/
	glGenBuffers(1, &geometry_vbo);
	glGenTextures(1, &geometry_tex);
	glBindBuffer(GL_TEXTURE_BUFFER, geometry_vbo);
	glBufferData(GL_TEXTURE_BUFFER, 1024 * 1024 * sizeof(vmath::vec4), NULL, GL_DYNAMIC_COPY);
	glBindTexture(GL_TEXTURE_BUFFER, geometry_tex);
	glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, geometry_vbo);
	/* 生成顶点数组，并绑定
	 * 绑定缓存到GL_ARRAY_BUFFER
	 * ---貌似Transform Feedback返回的数据都需要绑定缓存到GL_ARRAY_BUFFER（·_·）
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

	//设置投影矩阵与模型矩阵
	vmath::mat4 projection_matrix(vmath::frustum(-1.0f, 1.0f, -aspect, aspect, 1.0f, 5000.0f) * vmath::translate(0.0f, 0.0f, -100.0f));
	vmath::mat4 model_matrix(vmath::scale(0.3f) *
		vmath::rotate(t * 360.0f, 0.0f, 1.0f, 0.0f) *
		vmath::rotate(t * 360.0f * 3.0f, 0.0f, 0.0f, 1.0f));

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	//调用几何体运动着色器程序，并为其uniform变量设置数据值
	glUseProgram(render_prog);
	glUniformMatrix4fv(render_model_matrix_loc, 1, GL_FALSE, model_matrix);
	glUniformMatrix4fv(render_projection_matrix_loc, 1, GL_FALSE, projection_matrix);

	//---这句话不知道能起到什么作用，毕竟在Render函数中有重新绑定了顶点数组（·_·）
	glBindVertexArray(render_vao);

	//将geometry_vbo缓存对象绑定到GL_TRANSFORM_FEEDBACK_BUFFER
	//这样Transform Feedback返回数据可以写入缓存对象中
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, geometry_vbo);

	//启用Transform Feedback，GL_TRIANLES为准备绘制的图元类型
	glBeginTransformFeedback(GL_TRIANGLES);
	object.Render();
	glEndTransformFeedback();

	//调用粒子运动着色器程序，为其uniform变量设置数据值
	glUseProgram(update_prog);
	model_matrix = vmath::mat4::identity();
	glUniformMatrix4fv(model_matrix_loc, 1, GL_FALSE, model_matrix);
	glUniformMatrix4fv(projection_matrix_loc, 1, GL_FALSE, projection_matrix);
	glUniform1i(triangle_count_loc, object.GetVertexCount() / 3);

	if (t > q)
	{
		//设置粒子运动步长
		glUniform1f(time_step_loc, (t - q) * 2000.0f);
	}

	q = t;

	/*如果当前绘制帧数为偶数：
		  绑定顶点数组vao[0]，即运用vao[0]中的数据作为输入数据
		  绑定Transform Feedback的缓存对象为vbo[1]，即将返回的数据存入其中，供下次绘制使用
	  如果当前绘制帧数为奇数：操作与偶数相反
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

	//启用Transform Feedback，绘制并记录其顶点着色器返回的数据
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