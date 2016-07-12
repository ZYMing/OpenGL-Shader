By Yi_M

介绍：
本项目练习实现《OPENGL编程指南（第八版）》第四章---帧缓存对象---例4.11代码

生成应用程序的帧缓存对象并将其绑定，添加渲染缓存对象作为其颜色附件
最后从应用程序的帧缓存中赋值渲染结果至窗口系统的帧缓存中

主要包括：
	1. 绘制函数：glDrawArray函数；
	2. 帧缓存函数：glGenFrameBuffer, glBindFrameBuffer函数
	3. 渲染缓存函数：glGenRenderBuffer, glBindRenderBuffer函数和glRenderbufferStorage函数为渲染缓存分配图像数据空间
					 glFramebufferRenderbuffer函数将渲染缓存关联到当前绑定的帧缓存对象的附件上。

------------------------------------------------------------------------------------
本练习是为了熟悉OPENGL shader着色器！