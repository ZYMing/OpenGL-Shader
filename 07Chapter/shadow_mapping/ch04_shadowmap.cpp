/* $URL$
   $Rev$
   $Author$
   $Date$
   $Id$
 */

#include "vapp.h"
#include "vutils.h"

#include "vmath.h"

#include "vbm.h"
#include "LoadShaders.h"

#include <stdio.h>

using namespace vmath;

#define FRUSTUM_DEPTH       800.0f
#define DEPTH_TEXTURE_SIZE  2048

BEGIN_APP_DECLARATION(ShadowMapExample)
    // Override functions from base class
    virtual void Initialize(const char * title);
    virtual void Display(bool auto_redraw);
    virtual void Finalize(void);
    virtual void Reshape(int width, int height);

    // Member variables
    float aspect;

    // Program to render from the light's position
	//�ӹ���λ�ý�����Ⱦ����ɫ������
    GLuint render_light_prog;
    struct
    {
        GLint model_view_projection_matrix;
    } render_light_uniforms;

    // FBO to render depth with
	//FBO֡�������������Ⱦ�����Ϣ
    GLuint  depth_fbo;
    GLuint  depth_texture;

    // Program to render the scene from the viewer's position
	//�ӹ۲����ӽ���Ⱦ��������ɫ������
    GLuint render_scene_prog;
    struct
    {
        GLint model_matrix;
        GLint view_matrix;
        GLint projection_matrix;
        GLint shadow_matrix;
        GLint light_position;
        GLint material_ambient;
        GLint material_diffuse;
        GLint material_specular;
        GLint material_specular_power;
    } render_scene_uniforms;

    // Ground plane
	//��ƽ��
    GLuint  ground_vbo;
    GLuint  ground_vao;

    VBObject object;

    GLint current_width;
    GLint current_height;

    void DrawScene(bool depth_only = false);
END_APP_DECLARATION()

DEFINE_APP(ShadowMapExample, "Shadow Mapping Example")

#define INSTANCE_COUNT 100

void ShadowMapExample::Initialize(const char * title)
{
    base::Initialize(title);

    // Create the program for rendering the scene from the light's POV.
    glCreateProgram();

	//������ɫ������������Ӱ
    ShaderInfo light_shaders[] =
    {
        { GL_VERTEX_SHADER, "shadowmap_shadow.vs.glsl" },
        { GL_FRAGMENT_SHADER, "shadowmap_shadow.fs.glsl" },
        { GL_NONE }
    };

    render_light_prog = LoadShaders(light_shaders);

    // Get the location of the projetion_matrix uniform
    render_light_uniforms.model_view_projection_matrix = glGetUniformLocation(render_light_prog, "model_view_projection_matrix");

    // Create the program for rendering the scene from the viewer's position
	//������ɫ��������Ⱦ����������ʾ
    ShaderInfo scene_shaders[] =
    {
        { GL_VERTEX_SHADER, "shadowmap_scene.vs.glsl" },
        { GL_FRAGMENT_SHADER, "shadowmap_scene.fs.glsl" },
        { GL_NONE }
    };

    render_scene_prog = LoadShaders(scene_shaders);

    // Get the locations of all the uniforms in the program
    render_scene_uniforms.model_matrix = glGetUniformLocation(render_scene_prog, "model_matrix");
    render_scene_uniforms.view_matrix = glGetUniformLocation(render_scene_prog, "view_matrix");
    render_scene_uniforms.projection_matrix = glGetUniformLocation(render_scene_prog, "projection_matrix");
    render_scene_uniforms.shadow_matrix = glGetUniformLocation(render_scene_prog, "shadow_matrix");
    render_scene_uniforms.light_position = glGetUniformLocation(render_scene_prog, "light_position");
    render_scene_uniforms.material_ambient = glGetUniformLocation(render_scene_prog, "material_ambient");
    render_scene_uniforms.material_diffuse = glGetUniformLocation(render_scene_prog, "material_diffuse");
    render_scene_uniforms.material_specular = glGetUniformLocation(render_scene_prog, "material_specular");
    render_scene_uniforms.material_specular_power = glGetUniformLocation(render_scene_prog, "material_specular_power");

    // Set the depth texture uniform to unit 0
	//ע��uniform�����ĸ�ֵʱ�������õ��ǵ�ǰ�󶨵���ɫ������
    glUseProgram(render_scene_prog);
	GLint render_sence_uniforms_texture = glGetUniformLocation(render_scene_prog, "depth_texture");
	glUniform1i(render_sence_uniforms_texture, 0);

    // Create a depth texture
	//�������������
    glGenTextures(1, &depth_texture);
    glBindTexture(GL_TEXTURE_2D, depth_texture);
	//Ϊ�������������ݿռ䣬ע�������ڲ���ʽΪGL_DEPTH_COMPONENT32�������С����Ϊ��֡�����С��ͬ
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, DEPTH_TEXTURE_SIZE, DEPTH_TEXTURE_SIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	//���������˲�ģʽΪ�����˲�
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//��������Ƚ�ģʽ
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	//���ñ߽��ȡģʽ
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Create FBO to render depth into
	//����֡�������������Ӱ����
    glGenFramebuffers(1, &depth_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, depth_fbo);

	//������ͼ��0���������Ϊ֡���渽����FBO
	//ע�⵱�����ڲ���ʽ����ΪGL_DEPTH_COMPONENT32ʱ��attachment����������ΪGL_DEPTH_ATTACHMENT
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depth_texture, 0);
	//��ֹ��Ⱦ��ɫ����Ϊû��������ɫ������
    glDrawBuffer(GL_NONE);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Upload geometry for the ground plane
    static const float ground_vertices[] =
    {
        -500.0f, -50.0f, -500.0f, 1.0f,
        -500.0f, -50.0f,  500.0f, 1.0f,
         500.0f, -50.0f,  500.0f, 1.0f,
         500.0f, -50.0f, -500.0f, 1.0f,
    };

    static const float ground_normals[] =
    {
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f
    };

	//��ƽ����������
    glGenVertexArrays(1, &ground_vao);
    glGenBuffers(1, &ground_vbo);
    glBindVertexArray(ground_vao);
    glBindBuffer(GL_ARRAY_BUFFER, ground_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(ground_vertices) + sizeof(ground_normals), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(ground_vertices), ground_vertices);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(ground_vertices), sizeof(ground_normals), ground_normals);

    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, NULL);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (const GLvoid *)sizeof(ground_vertices));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    // Load the object
    object.LoadFromVBM("armadillo_low.vbm", 0, 1, 2);
}

void ShadowMapExample::Display(bool auto_redraw)
{
    float t = float(GetTickCount() & 0xFFFF) / float(0xFFFF);
    static float q = 0.0f;
    static const vec3 X(1.0f, 0.0f, 0.0f);
    static const vec3 Y(0.0f, 1.0f, 0.0f);
    static const vec3 Z(0.0f, 0.0f, 1.0f);

	//��ʱ��仯�Ĺ�Դλ��
    vec3 light_position = vec3(sinf(t * 6.0f * 3.141592f) * 300.0f, 200.0f, cosf(t * 4.0f * 3.141592f) * 100.0f + 250.0f);

    // Setup
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    // Matrices for rendering the scene
	//���ó������õľ���--ģ�;����ӽǾ���ͶӰ������Ӱƫ�ƾ���
    mat4 scene_model_matrix = rotate(t * 720.0f, Y);
    mat4 scene_view_matrix = translate(0.0f, 0.0f, -300.0f);
    mat4 scene_projection_matrix = frustum(-1.0f, 1.0f, -aspect, aspect, 1.0f, FRUSTUM_DEPTH);
    const mat4 scale_bias_matrix = mat4(vec4(0.5f, 0.0f, 0.0f, 0.0f),
                                        vec4(0.0f, 0.5f, 0.0f, 0.0f),
                                        vec4(0.0f, 0.0f, 0.5f, 0.0f),
                                        vec4(0.5f, 0.5f, 0.5f, 1.0f));

    // Matrices used when rendering from the light's position
	//�ӹ�Դλ����Ⱦ������Ӱ���õľ���
    mat4 light_view_matrix = lookat(light_position, vec3(0.0f), Y);
    mat4 light_projection_matrix(frustum(-1.0f, 1.0f, -1.0f, 1.0f, 1.0f, FRUSTUM_DEPTH));

    // Now we render from the light's position into the depth buffer.
    // Select the appropriate program
	//�ӹ�Դλ����Ⱦ����Ȼ��棬����ѡ����ɫ�����򣬲����þ������
    glUseProgram(render_light_prog);
    glUniformMatrix4fv(render_light_uniforms.model_view_projection_matrix, 1, GL_FALSE, light_projection_matrix * light_view_matrix * scene_model_matrix);

    // Bind the 'depth only' FBO and set the viewport to the size of the depth texture
	//��FBO��ʹ����Ⱦ�����Ϣ����Ȼ��棬�������ӿڴ�СΪ�������Ĵ�С
    glBindFramebuffer(GL_FRAMEBUFFER, depth_fbo);
    glViewport(0, 0, DEPTH_TEXTURE_SIZE, DEPTH_TEXTURE_SIZE);

    // Clear
    glClearDepth(1.0f);
    glClear(GL_DEPTH_BUFFER_BIT);

    // Enable polygon offset to resolve depth-fighting isuses
	//�򿪶����ƫ�ƣ��Ա���������ݵ�z-fighting����
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(2.0f, 4.0f);
    // Draw from the light's point of view
	//�ӹ�Դλ�ý��л���
    DrawScene(true);
    glDisable(GL_POLYGON_OFFSET_FILL);

    // Restore the default framebuffer and field of view
	//���°󶨵�Ĭ��֡���棬�������ӿڴ�С
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, current_width, current_height);

    // Now render from the viewer's position
	//���ó�����ɫ������
    glUseProgram(render_scene_prog);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    // Setup all the matrices
	//���ó�����ɫ�������uniform����
    glUniformMatrix4fv(render_scene_uniforms.model_matrix, 1, GL_FALSE, scene_model_matrix);
    glUniformMatrix4fv(render_scene_uniforms.view_matrix, 1, GL_FALSE, scene_view_matrix);
    glUniformMatrix4fv(render_scene_uniforms.projection_matrix, 1, GL_FALSE, scene_projection_matrix);
    glUniformMatrix4fv(render_scene_uniforms.shadow_matrix, 1, GL_FALSE, scale_bias_matrix * light_projection_matrix * light_view_matrix);
    glUniform3fv(render_scene_uniforms.light_position, 1, light_position);

    // Bind the depth texture
	//������Ӷ�������Ӱӳ��
    glBindTexture(GL_TEXTURE_2D, depth_texture);
    glGenerateMipmap(GL_TEXTURE_2D);

    // Draw
    DrawScene(false);

    // Done
    base::Display();
}

void ShadowMapExample::DrawScene(bool depth_only)
{
    // Set material properties for the object
    if (!depth_only)
    {
		//����ģ�͵Ĳ�������
        glUniform3fv(render_scene_uniforms.material_ambient, 1, vec3(0.3f, 0.2f, 0.4f));
        glUniform3fv(render_scene_uniforms.material_diffuse, 1, vec3(0.5f, 0.4f, 1.0f));
        glUniform3fv(render_scene_uniforms.material_specular, 1, vec3(1.0f, 1.0f, 1.0f));
        glUniform1f(render_scene_uniforms.material_specular_power, 25.0f);
    }

    // Draw the object
    object.Render();

    // Set material properties for the ground
    if (!depth_only)
    {
		//���õ�ƽ�ߵĲ�������
        glUniform3fv(render_scene_uniforms.material_ambient, 1, vec3(0.2f, 0.2f, 0.2f));
        glUniform3fv(render_scene_uniforms.material_diffuse, 1, vec3(0.1f, 0.5f, 0.1f));
        glUniform3fv(render_scene_uniforms.material_specular, 1, vec3(0.1f, 0.1f, 0.1f));
        glUniform1f(render_scene_uniforms.material_specular_power, 3.0f);
    }

    // Draw the ground
    glBindVertexArray(ground_vao);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glBindVertexArray(0);
}

void ShadowMapExample::Finalize(void)
{
    glUseProgram(0);
    glDeleteProgram(render_light_prog);
    glDeleteProgram(render_scene_prog);
    glDeleteBuffers(1, &ground_vbo);
    glDeleteVertexArrays(1, &ground_vao);
}

void ShadowMapExample::Reshape(int width, int height)
{
    current_width = width;
    current_height = height;

    aspect = float(height) / float(width);
}
