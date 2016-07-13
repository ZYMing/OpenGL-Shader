By Yi_M

介绍：
本项目练习实现《OPENGL编程指南（第八版）》第六章例1-3，例7-9，例11

load_texture.cpp文件实现了实例中的程序--纹理数据的加载：
	（1）	设置纹理数据主要有两种方法：从程序中的数据加载进纹理对象、读取磁盘中的数据加载进纹理对象；
	（2）！	从程序中的数据加载进纹理对象：a. 调用了glGenTextures、 glBindTexture、 glTexStorage2D、 glTexSubImage2D函数
										  b. 调用了glTexParameteriv、 glTexParameteri函数设置了纹理参数（包括纹理数据的组织方式SWIZZLE，WRAP模式的不同效果，FILTER滤波的不同选择）
										  c. glTexSubImage2D函数的data参数有两种不同的解释，分别为内存指针与缓存对象的偏移
	（3）	读取磁盘中的数据加载进纹理对象：调用随书提供的函数库中包含的vglLoadTexture函数与vglImageData数据结构；
	（4）！	着色器中纹理坐标的传递和uniform sampler2D变量的使用，着色器函数texture的调用；
	

------------------------------------------------------------------------------------
本练习是为了熟悉OPENGL shader着色器！