By Yi_M

介绍：
本项目练习实现《OPENGL编程指南（第八版）》第六章例12-14

load_texture.cpp文件实现了实例中的程序--多重纹理的使用：
	（1）	着色器中多重纹理的使用，纹理坐标可相同，可不同
	（2）	GLint tex1_uniform_Loc = glGetUniformLocation(tex_prog, "tex1");
			glUniform1i(tex1_uniform_Loc, 0);
	
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, Textures[CHECKBOARD]);
			获取uniform采样变量的位置，设置其值，激活纹理对象。
	

------------------------------------------------------------------------------------
本练习是为了熟悉OPENGL shader着色器！