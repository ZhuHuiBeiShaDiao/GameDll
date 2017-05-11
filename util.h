

typedef struct _INLINE_HOOK
{
	DWORD FunctionAddress;
	PVOID FunctionCallBack;
	PVOID InLineCommonHookZone;
	int InLineCommonLength;
}INLINEHOOKINFO, *PINLINEHOOKINFO;


typedef struct _DLLINFO
{
	string fullPath;
	string dllName;
	DWORD  textStart;
	DWORD  textEnd;
	DWORD  modelBase;
	DWORD  modelEnd;
}DLLINFO;
typedef struct _PEHEAD
{
	
	DWORD  virtualSize;
	DWORD  virtualOffset;
}PEHEAD;
/**************************************
*  ���ܣ����浽�ļ�
****************************************/
void saveFile(string fileFullPath,string str);

/**************************************
*  ���ܣ����ļ��ж�ȡ
****************************************/
void readFile(string fileFullPath,vector<string>& vec);

/**************************************
*  ���ܣ��Ӷ�ջ�ļ��л�ȡ��ַ
****************************************/
void getAddress(string str,map<DWORD,DWORD>& addressMap);

/**************************************
*  ���ܣ�������ȵݹ����Ŀ¼�����е��ļ�
****************************************/
//BOOL  DirectoryList(LPCSTR Path,vector<string>& fileVec);

/**************************************
*  ���ܣ��������
****************************************/
void OutputDbgInfo(LPCSTR format, ...);

/**************************************
*  ���ܣ���֤��ַ�Ƿ�ɷ���
****************************************/
bool checkAddress(DWORD address);

/**************************************
*  ���ܣ����Ҫ������߳�
****************************************/
DWORD getThreadId();

/**************************************
*  ���ܣ����������Ϣ
****************************************/
void OutputDbgInfo(LPCSTR format, ...);

/**************************************
*  ���ܣ����������
****************************************/
DWORD GetSpecial(IN unsigned char* code,IN DWORD length,IN DWORD begin,IN DWORD scope);

/**************************************
*  ���ܣ����ַд��
****************************************/
VOID writeMemoryStatic(LPVOID p,unsigned char replace[],int length);


VOID getAddress();

/**************************************
*  ���ܣ��ı��ַ��д״̬
*  ������
****************************************/
BOOL changAddressStatusStatic(DWORD address,int length);

/**************************************
*  ���ܣ������ڶ�������麯������
****************************************/
DWORD  virtualCall(DWORD objBase,DWORD offSet, ...);

/**************************************
*  ���ܣ���õ��������ĵ�ַ
****************************************/
DWORD getFunctionAddress(char *dllName,char* functionName);

/**************************************
*  ���ܣ�hook��dump����ջ
****************************************/
void saveStatckDump(DWORD callAddress,DWORD ebp);

/**************************************
*  ���ܣ�hook��dump����ջ�������麯����
****************************************/
void saveStatckDump(DWORD ebp);
/**************************************
*  ���ܣ�hook��dump����ջ������Լ�����ʾ��Ϣ
****************************************/
void saveStatckDump(DWORD ebp,char *info);
/**************************************
*  ���ܣ�hook��dump����ջ�������麯����
****************************************/
void saveStatckDumpWithPath(string path,DWORD callAddress,DWORD ebp,char *info, BOOL recodeParams);

void saveStatckDump(DWORD ebp, BOOL recodeParams);//��Ӽ�¼����

/**************************************
*  ���ܣ���Ϸ�ﶼ��unicode�ģ�תΪchar*
****************************************/
char*  myWideToChar(DWORD wideAddr);

/**************************************
*  ���ܣ����ĳһPE�ļ���ĳһ���εĴ�С����.text��
****************************************/
PEHEAD getPeHeadSize(const char * filePath,const char * dataType);

/**************************************
*  ���ܣ���õ�ǰ���̵�����ģ��
****************************************/
void getProcessModules(OUT map<DWORD, string> &dllMap);

/**************************************
*  ���ܣ������Ҫģ��Ĵ���ε�ַ
****************************************/
void getModuleDetailInfo(vector<DLLINFO> &vec);


//�����ֽ�char*ת��Ϊ���ֽ�wchar_t*  
wchar_t* AnsiToUnicode( const char* szStr )  ;

//�����ֽ�wchar_t*ת��Ϊ���ֽ�char*  
char* UnicodeToAnsi( const wchar_t* szStr )  ;

/**************************************
*  ���ܣ�cstringתΪָ������ ���������쳣�����������һ������
*  ������
*
*  ����ֵ��
*  ʱ�䣺[2015/1/11 9:07 Administrator]
****************************************/
DWORD ConverToHex(CString str, int jinzhi);

/**************************************
*  ���ܣ�		char *ת����short���������ʾ�� ��Ҫ���ַ�����ƴ�Ӽ���ʽ�� 
*  ������		buffer ������ַ���	length ����
*
*  ����ֵ��	void
*  ʱ�䣺[2015/1/1 22:10 Administrator]
****************************************/
void PrintBuffer(BOOL isTcp,PVOID buffer, int length);