By Yi_M

介绍：
本项目练习实现《OPENGL编程指南（第八版）》第三章---多实例渲染简单样例---书中例9-12代码

从vbm文件中读取object的相关属性，包括：顶点位置，法向量等，绑定到ARRAY_BUFFER中
设置color颜色和model_matrix模型矩阵绑定到ARRAY_BUFFER相应的缓存对象中
color和model_matrix顶点属性设置为实例属性，其中，！！注意矩阵的顶点属性设置！！！

主要包括：
	1. 绘制函数：glDrawElementsInstanced, glDrawArraysInstanced多实例函数；
	2. 着色器编辑在main函数里
	3. 顶点属性函数：glVertexAttribPointer，glEnableVertexAttribArray函数和 glVertexAttribDivisor顶点属性分割函数
	4. 缓存映射函数：glMapBuffer映射缓存函数和glUnMapBuffer解除映射函数

------------------------------------------------------------------------------------
本练习是为了熟悉OPENGL shader着色器！