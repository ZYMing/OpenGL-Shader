By Yi_M

介绍：
本项目练习实现《OPENGL编程指南（第八版）》第七章例15-21

ch04——shadowmap.cpp文件实现了实例中的程序--阴影映射：
	（1）！	首先需要从光源位置生成阴影信息存储到纹理中：
				a, 纹理内部格式设置为深度信息类型，即GL_DEPTH_COMPONENT32， 视口大小为帧缓存大小
				b, 设置纹理比较模式为GL_COMPARE_REF_TO_TEXTURE，比较函数为GL_LEQUAL
				c, 调用glFramebufferTexture函数将纹理对象关联到FBO帧缓存对象，渲染深度信息到纹理空间中
				d, 从光源位置进行绘制，并打开多边形偏移 glEnable(GL_POLYGON_OFFSET_FILL);    glPolygonOffset(2.0f, 4.0f);，防止z-fighting问题
	（2）	进行场景渲染时，注意重新绑定默认帧缓存
	（3）！	场景渲染着色器的实现，调用了textureProj函数，阴影矩阵的设置使得阴影坐标有些许偏移，
			那些比记录的深度值还远的片元对于光源来说将是不可见的
	

------------------------------------------------------------------------------------
本练习是为了熟悉OPENGL shader着色器！