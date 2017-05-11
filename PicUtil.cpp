#include "stdafx.h"
#include "PicUtil.h"

#include "FileUtil.h"

CPicUtil::CPicUtil()
{

}
CPicUtil::~CPicUtil()
{

}
/************************************************************************
	HWND hwnd---��ͼ�Ĵ��ھ��(0Ϊ��ǰ��Ļ)
	int left---����ڴ��ھ����߾�
	int top---����� ���ھ�����߾�
	int width---��ͼ���(��ȸ߶�ͬʱΪ0��ȡȫ��)
	int height---��ͼ�߶�
	CString path---�ļ�����·��                                                         	************************************************************************/
bool CPicUtil::GetScreenBmp(HWND hwnd,char *savePath,int left,int top,int width,int height)
{
	if(!hwnd || NULL == savePath)
	{
		return false;
	}

	CDC *pDC;//��ĻDC
	pDC = CDC::FromHandle(::GetDC(hwnd));//��ȡ��ĻDC(0Ϊȫ���������Ϊ����)

	int BitPerPixel = pDC->GetDeviceCaps(BITSPIXEL);//�����ɫģʽ
	if(width==0 && height==0)//Ĭ�Ͽ�Ⱥ͸߶�Ϊȫ��
	{
		width = pDC->GetDeviceCaps(HORZRES); //����ͼ����ȫ��
		height= pDC->GetDeviceCaps(VERTRES); //����ͼ��߶�ȫ��
	}


	CDC memDC;//�ڴ�DC
	memDC.CreateCompatibleDC(pDC);

	CBitmap memBitmap, *oldmemBitmap;//��������Ļ���ݵ�bitmap
	memBitmap.CreateCompatibleBitmap(pDC, width, height);

	oldmemBitmap = memDC.SelectObject(&memBitmap);//��memBitmapѡ���ڴ�DC
	//memDC.BitBlt(0, 0, Width, Height, pDC, 0, 0, SRCCOPY);//������Ļͼ���ڴ�DC
	memDC.BitBlt(0, 0, width, height, pDC, left,top , SRCCOPY);//ͼ���ȸ߶Ⱥͽ�ȡλ��


	//���´��뱣��memDC�е�λͼ���ļ�
	BITMAP bmp;
	memBitmap.GetBitmap(&bmp);//���λͼ��Ϣ

	//���·��
	CHAR dllPath[MAX_PATH] = {0};
	GetExePath(dllPath);
	strcat(dllPath,"\\CheckCode\\");

	//����Ŀ¼
	CString str(dllPath);
	bool isOk = SuperMkDir(str);

	char temp[50];
	sprintf(temp,"QQhx-%d.jpg",GetTickCount());
	strcat(dllPath,temp);

	memcpy(savePath, dllPath, MAX_PATH-1);


	FILE *fp = fopen(savePath, "w+b");//ͼƬ����·���ͷ�ʽ

	BITMAPINFOHEADER bih = {0};//λͼ��Ϣͷ
	bih.biBitCount = bmp.bmBitsPixel;//ÿ�������ֽڴ�С
	bih.biCompression = BI_RGB;
	bih.biHeight = bmp.bmHeight;//�߶�
	bih.biPlanes = 1;
	bih.biSize = sizeof(BITMAPINFOHEADER);
	bih.biSizeImage = bmp.bmWidthBytes * bmp.bmHeight;//ͼ�����ݴ�С
	bih.biWidth = bmp.bmWidth;//���

	BITMAPFILEHEADER bfh = {0};//λͼ�ļ�ͷ
	bfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);//��λͼ���ݵ�ƫ����
	bfh.bfSize = bfh.bfOffBits + bmp.bmWidthBytes * bmp.bmHeight;//�ļ��ܵĴ�С
	bfh.bfType = (WORD)0x4d42;

	fwrite(&bfh, 1, sizeof(BITMAPFILEHEADER), fp);//д��λͼ�ļ�ͷ
	fwrite(&bih, 1, sizeof(BITMAPINFOHEADER), fp);//д��λͼ��Ϣͷ
	byte * p = new byte[bmp.bmWidthBytes * bmp.bmHeight];//�����ڴ汣��λͼ����
	GetDIBits(memDC.m_hDC, (HBITMAP) memBitmap.m_hObject, 0, height, p, 
		(LPBITMAPINFO) &bih, DIB_RGB_COLORS);//��ȡλͼ����
	fwrite(p, 1, bmp.bmWidthBytes * bmp.bmHeight, fp);//д��λͼ����
	delete [] p;
	fclose(fp);
	memDC.SelectObject(oldmemBitmap);
	return true;
}