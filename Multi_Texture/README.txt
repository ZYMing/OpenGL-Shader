By Yi_M

���ܣ�
����Ŀ��ϰʵ�֡�OPENGL���ָ�ϣ��ڰ˰棩����������12-14

load_texture.cpp�ļ�ʵ����ʵ���еĳ���--���������ʹ�ã�
	��1��	��ɫ���ж��������ʹ�ã������������ͬ���ɲ�ͬ
	��2��	GLint tex1_uniform_Loc = glGetUniformLocation(tex_prog, "tex1");
			glUniform1i(tex1_uniform_Loc, 0);
	
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, Textures[CHECKBOARD]);
			��ȡuniform����������λ�ã�������ֵ�������������
	

------------------------------------------------------------------------------------
����ϰ��Ϊ����ϤOPENGL shader��ɫ����