By Yi_M

���ܣ�
����Ŀ��ϰʵ�֡�OPENGL���ָ�ϣ��ڰ˰棩��������---��ʵ����Ⱦ������---������9-12����

��vbm�ļ��ж�ȡobject��������ԣ�����������λ�ã��������ȣ��󶨵�ARRAY_BUFFER��
����color��ɫ��model_matrixģ�;���󶨵�ARRAY_BUFFER��Ӧ�Ļ��������
color��model_matrix������������Ϊʵ�����ԣ����У�����ע�����Ķ����������ã�����

��Ҫ������
	1. ���ƺ�����glDrawElementsInstanced, glDrawArraysInstanced��ʵ��������
	2. ��ɫ���༭��main������
	3. �������Ժ�����glVertexAttribPointer��glEnableVertexAttribArray������ glVertexAttribDivisor�������Էָ��
	4. ����ӳ�亯����glMapBufferӳ�仺�溯����glUnMapBuffer���ӳ�亯��

------------------------------------------------------------------------------------
����ϰ��Ϊ����ϤOPENGL shader��ɫ����