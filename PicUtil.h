
#ifndef  _PIC_UTIL
#define  _PIC_UTIL

class CPicUtil
{
public:
	CPicUtil();
	~CPicUtil();

	/************************************************************************
	HWND hwnd---��ͼ�Ĵ��ھ��(0Ϊ��ǰ��Ļ)
	int left---����ڴ��ھ����߾�
	int top---����� ���ھ�����߾�
	int width---��ͼ���(��ȸ߶�ͬʱΪ0��ȡȫ��)
	int height---��ͼ�߶�
	CString path---�ļ�����·��                                                         	************************************************************************/
	bool CPicUtil::GetScreenBmp(HWND hwnd,char *savePath,int left,int top,int width,int height);

};



#endif
