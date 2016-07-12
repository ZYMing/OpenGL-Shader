By Yi_M

介绍：
本项目练习实现《OPENGL编程指南（第八版）》第五章例8--例12

main.cpp文件实现了实例中的程序--粒子碰撞检测运动：
	（1）	粒子运动着色器程序实现了碰撞检测算法----Ray-Triangle算法！！
	（2）！ 调用glTransformFeedbackVaryings设置要返回的信息后，需要再次连接着色器程序！！
	（3）	注意GL_TRANSFORM_FEEDBACK_BUFFER缓存与GL_ARRAY_BUFFER缓存的运用，如何将返回数据保存到缓存中！！
	（4）	调用glBindBufferBase函数设置返回信息写入的缓存
	（5）	调用glBeginTransformFeedback函数设置Transform Feedback准备记录的图元类型
	（6）	学习粒子运动程序中两个缓存的使用方法！！

transformfeedback.cpp则编写了简单的三角形程序，运用Transform Feedback实现将第一个三角形平移：
	

------------------------------------------------------------------------------------
本练习是为了熟悉OPENGL shader着色器！