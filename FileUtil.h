#ifndef  _FILE_UTIL
#define  _FILE_UTIL

	//��ȡ��Ŀ¼·��
	CString GetParentDir(CString P);

	//Ŀ¼�Ƿ����
	bool FolderExists(CString s);

	//����Ŀ¼�����Զ�㴴���������ɹ������Ѵ��ڣ�����true; ���򷵻�false
	bool SuperMkDir(CString P);

	void GetExePath(char* pExePath);

	void getConfigFromIni(TCHAR *key, TCHAR *value);
#endif
