By Yi_M

介绍：
本项目练习实现《OPENGL编程指南（第八版）》第十章--几何着色器的Transform Feedback
 
 ch10_draw_xfb.cpp文件实现了--几何着色器与TransForm Feedback的简单运用：
		（1） sort_vs_source 与sort_gs_source 为顶点着色器与几何着色器，用于集合排序
					其中，几何着色器用到了多重数据流，stream0 与stream1，并将数据分开分别发送到两个流中
		（2） 调用glEnable(GL_RASTERIZER_DISCARD)关闭光栅化，避免多余的工作
		（3） TransForm Feedback的运用，获取几何着色器返回的数据，调用glDrawTransformFeedbackStream函数再次绘制
	

------------------------------------------------------------------------------------
本练习是为了熟悉OPENGL shader着色器！