By Yi_M

介绍：
本项目练习实现《OPENGL编程指南（第八版）》第六章例19-22

ch06_cube_map.cpp文件实现了实例中的程序--立方体纹理映射--天空盒：
	（1）	初始化立方体映射纹理，绑定纹理目标为GL_TEXTURE_CUBE_MAP，并分别为6个面分配纹理空间，设置数据
	（2）	skybox_shader_vs与skybox_shader_fs为简单天空盒着色器，注意纹理坐标与gl_Position的计算
	（3）	object_shader_vs与object_shader_fs为环境映射着色器，
				将物体的顶点位置与法向量传入着色器，通过矩阵计算传出表面法线与空间位置到片元着色器
				片元着色器沿法线反射观察空间位置调用着色器函数reflect计算纹理坐标，完成环境映射
	（4）	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS)的调用完成无缝立方体映射滤波，避免立方体映射相邻边之间的缝
	

------------------------------------------------------------------------------------
本练习是为了熟悉OPENGL shader着色器！