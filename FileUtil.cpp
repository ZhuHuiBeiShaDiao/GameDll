#include "stdafx.h"
#include "FileUtil.h"


//��ȡ��Ŀ¼·��
CString GetParentDir(CString P)
{
	return P.Left(P.ReverseFind('//'));
} 

//Ŀ¼�Ƿ����
bool FolderExists(CString s)
{
	DWORD attr;   
	attr   =   GetFileAttributes(s);  
	return   (attr   !=   (DWORD)(-1) )  && 
		( attr   &   FILE_ATTRIBUTE_DIRECTORY);   
}

//����Ŀ¼�����Զ�㴴���������ɹ������Ѵ��ڣ�����true; ���򷵻�false
bool SuperMkDir(CString P)
{
	int len=P.GetLength();
	if ( len <2 ) return false;  //����ַ�������С��2��������Ч��·����ʽ��

	if('//'==P[len-1])
	{
		P=P.Left(len-1);
		len=P.GetLength();
	}// ɾ��ĩβ��"/"
	if (  len <=0 ) return false;

	if (len <=3) //�������<=3����ô�ض��Ǹ�Ŀ¼��ʽ
	{
		if (FolderExists(P))return true;//�����Ŀ¼����,����true;
		else return false;              //��������ڣ���Ŀ¼���޷������ģ�����ʧ�ܣ�
	}

	if (FolderExists(P))return true;//���Ŀ¼����,����true;

	// �ֿ���Ŀ¼�ͱ���Ŀ¼����
	CString Parent;
	Parent=P.Left(P.ReverseFind('\\') );

	if(Parent.GetLength()<=0)return false; //Ŀ¼���ƴ���

	bool Ret=SuperMkDir(Parent);  //�ݹ鴴����Ŀ¼

	if(Ret)      // ��Ŀ¼����,ֱ�Ӵ���Ŀ¼
	{
		SECURITY_ATTRIBUTES sa;
		sa.nLength=sizeof(SECURITY_ATTRIBUTES);
		sa.lpSecurityDescriptor=NULL;
		sa.bInheritHandle=0;
		Ret=(CreateDirectory(P,&sa)==TRUE);
		return Ret;
	}
	else
		return false;
}

/************************************************************************/
/* ��õ�ǰģ��Ŀ¼                                                        */
/************************************************************************/
void GetExePath(char* pExePath) 
{     
	HMODULE handle = GetModuleHandleA("001.dll");
	int  pathlen = GetModuleFileNameA(handle, pExePath, MAX_PATH);    
	while(1)     
	{         
		if(pExePath[pathlen--]=='\\')             
			break;     
	}     
	pExePath[++pathlen] = 0;     
}   


void getConfigFromIni(TCHAR *key, TCHAR *value)
{

	//��������ļ����õ�ini�ļ�����
	char iniPath[MAX_PATH] = {0};
	GetExePath(iniPath);
	strcat(iniPath,"\\");
	strcat(iniPath,"config.ini");
	TCHAR *utfPath = AnsiToUnicode(iniPath);
	
	GetPrivateProfileString(
		L"base", // ָ����� Section ���Ƶ��ַ�����ַ
		key, // ָ����� Key ���Ƶ��ַ�����ַ
		L"xxxx", // ��� Key ֵû���ҵ����򷵻�ȱʡ���ַ����ĵ�ַ
		value, // �����ַ����Ļ�������ַ
		MAX_PATH, // �������ĳ���
		utfPath // ini �ļ����ļ���
		);
	//AfxMessageBox(utfPath);
	//AfxMessageBox(value);
}