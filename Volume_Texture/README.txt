By Yi_M

介绍：
本项目练习实现《OPENGL编程指南（第八版）》第六章例15-16

ch06_volume_texture.cpp文件实现了实例中的程序--体数据纹理的渲染：
	（1）	渲染体的最简单方法是渲染穿过体的平面，把平面作为每个顶点有3维纹理坐标的四边形
	（2）	实现中的顶点着色器将二维纹理坐标变换到三维纹理坐标，通过OpenGL插值，应用于片元着色器中
	（3）	其余过程与纹理数据的加载相同
	

------------------------------------------------------------------------------------
本练习是为了熟悉OPENGL shader着色器！