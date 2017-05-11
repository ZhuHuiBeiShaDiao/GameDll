// Test.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#define LEN 1024

vector<DLLINFO> g_dllInfoVec;//ȫ�ֵ�ģ����Ϣ

//���浽�ļ�
void saveFile(string fileFullPath,string str){
	ofstream out(fileFullPath,ios::out | ios::app); //ofstream out("out.txt",ios::out | ios::app);  
	//out<<str<<endl;
	out<<str;
	out.close();
}


//���ļ��ж�ȡ�ļ�
void readFile(string fileFullPath,vector<string>& vec){

	ifstream inFile;
	inFile.open(fileFullPath,ios::in);

	if(!inFile)
	{
		cout<<"�ļ������ڣ� "<<fileFullPath<<endl;
		return;
	}

	char buffer[256];
	string message;
	while(!inFile.eof())
	{
		inFile.getline(buffer,256,'\n');//getline(char *,int,char) ��ʾ�����ַ��ﵽ256�����������оͽ���
		message = buffer;
	//	cout<<message<<endl;
		vec.push_back(message);
	}
	inFile.close();
}

vector<string>::iterator iter;
map<DWORD,DWORD>::iterator mapIter;
void getAddress(string str,map<DWORD,DWORD>& addressMap){


	vector<string> vecLine;

	readFile(str,vecLine);

	cout<<"��õĶ�ջ�������� "<<vecLine.size()<<endl;

	string tempAddress;

	for (iter = vecLine.begin();iter!=vecLine.end();iter++)
	{
		try
		{
			tempAddress = *iter;
			if(tempAddress.length()>15)
			{
				DWORD value = stol(tempAddress.substr(11,8),FALSE,16);
				DWORD key = stol(tempAddress.substr(0,8),FALSE,16);
				addressMap[key]=value;
			}
		}catch (...){
			OutputDbgInfo("[!] ����addressMap�����쳣:%d�\\r\n",GetLastError());
		}
	}

}

// ������ȵݹ����Ŀ¼�����е��ļ�
// BOOL  DirectoryList(LPCSTR Path,vector<string>& fileVec)
// {
// 	WIN32_FIND_DATA FindData;
// 	HANDLE hError;
// 	int FileCount = 0;
// 	char FilePathName[LEN];
// 	// ����·��
// 	char FullPathName[LEN];
// 	strcpy(FilePathName, Path);
// 	strcat(FilePathName, "\\*.*");
// 	hError = FindFirstFile(FilePathName, &FindData);
// 	if (hError == INVALID_HANDLE_VALUE)
// 	{
// 		printf("����ʧ��!");
// 		return 0;
// 	}
// 	while(::FindNextFile(hError, &FindData))
// 	{
// 		// ����.��..
// 		if (strcmp(FindData.cFileName, ".") == 0 
// 			|| strcmp(FindData.cFileName, "..") == 0 )
// 		{
// 			continue;
// 		}
// 		// ��������·��
// 		wsprintf(FullPathName, "%s\\%s", Path,FindData.cFileName);
// 		FileCount++;
// 		// ����������ļ�
// 		printf("\n%d  %s  ", FileCount, FullPathName);
// 
// 		fileVec.push_back(FullPathName);
// 
// 		/*if (FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
// 		{
// 			printf("<Dir>");
// 			
// 			DirectoryList(FullPathName);
// 		}*/
// 	}
// 	return 0;
// }

/**************************************
*  ���ܣ���֤��ַ�Ƿ�ɷ���
*  ������
*
*  ����ֵ��
*  �����ˣ�
*  ʱ�䣺[2014/4/27/ Administrator]
****************************************/
bool checkAddress(DWORD address){   
	if(!IsBadReadPtr((PVOID)address,4)){	//��o?��?DЧ?���ب�?ַ?�践��|����?0
		return true;
	}else{
		return false;
	}
} 

/**************************************
*  ���ܣ����Ҫ������߳�
*  ������
*
*  ����ֵ��
*  �����ˣ�
*  ʱ�䣺[2014/4/27/ Administrator]
****************************************/
DWORD getThreadId(){
	//DWORD  dwDstThread =0;
	//	HWND gameh=FindWindow(NULL,"QQ����");
	//if(gameh==0){
	//cout<<"û�ҵ�	"<<endl;
	//}
	//dwDstThread=::GetWindowThreadProcessId(gameh,NULL);
	// 
	int temp = 0;
	_asm
	{

		mov eax,dword ptr fs:[0x24]
		mov temp,eax

	}
	OutputDbgInfo("[!] ����[!] getThreadId%d!\n",temp);
	return temp;
}



/**************************************
*  ���ܣ����������Ϣ
*  ������
****************************************/
void OutputDbgInfo(LPCSTR format, ...)
{
	va_list arglist;
	char buffer[1024];
	va_start (arglist,format);
	vsprintf_s(buffer, format, arglist);
	va_end (arglist);
	strcat_s(buffer, "\n");
	OutputDebugStringA(buffer);
}


/**************************************
*  ���ܣ����������
*  ������
****************************************/
DWORD GetSpecial(IN unsigned char* code,IN DWORD length,IN DWORD begin,IN DWORD scope)
{
	try
	{
		unsigned char *p ;
		p=(unsigned char *)begin;
		for (DWORD i = 0; i < scope; i++,p++){
			for(DWORD j=0;j<length;j++){
				if(code[j]==0xcc){
					continue;
				}
				if (!checkAddress(DWORD(p-length+j)))
				{
					continue;//��ַ���ɷ���
				}
				if(*(p-length+j)!=code[j]){
					break; //��һ�������ϵľ�ָ����һ��
				}
				if(j==length-1){
					return  (DWORD)(p-length);//��ʼ����ַ
				}
			}
		}
	}catch (...){
		OutputDbgInfo("[!] ����xxxxxxxxִ���쳣:%d�\\r\n",GetLastError());
	}
	return 0;
}

/**************************************
*  ���ܣ����������
*  ������
****************************************/
VOID GetSpecial_temp(IN unsigned char* code,IN DWORD length,IN DWORD begin,IN DWORD end,OUT vector<DWORD>& vec)
{
	try
	{
		unsigned char *p ;
		p=(unsigned char *)begin;
		DWORD scope = end - begin;
		for (DWORD i = 0; i < scope; i++,p++){
			for(DWORD j=0;j<length;j++){
				if(code[j]==0xcc){
					continue;
				}
				if(*(p-length+j)!=code[j]){
					break; //��һ�������ϵľ�ָ����һ��
				}

				if(j==length-1){
					vec.push_back((DWORD)(p-length));
				}
			}
		}
	}catch (...){
		OutputDbgInfo("[!] ����xxxxxxxxִ���쳣:%d�\\r\n",GetLastError());
	}
}

_declspec(naked) VOID VirtualProtectDetail()
{
	_asm
	{
		mov     edi, edi
		push    ebp
		mov     ebp, esp
		push    esi
		mov     esi, 0x7C92d6ee  //7C8012C4 >7C92D6EE  ntdll.ZwProtectVirtualMemory
		mov     eax,0x7c801a6d
		jmp     eax
	}
}

#define WIN7

BOOL myVirtualProtect(DWORD p,DWORD lengthCode,int status,DWORD* lpflOldProtect)
{

	#ifdef  WIN7
		return VirtualProtect((LPVOID)p, lengthCode, status, lpflOldProtect);
	#endif

	int temp = 0;
	_asm
	{
		push    lpflOldProtect
			push    PAGE_EXECUTE_READWRITE
			push    lengthCode
			push    p
			push    -1
			call    VirtualProtectDetail
			mov temp,eax
	}
	if(temp){
		return TRUE;
	}else
	{
		return FALSE;
	}
}

/**************************************
*  ���ܣ����ַд��
*  ������
****************************************/
VOID writeMemoryStatic(LPVOID p,unsigned char replace[],int length){	 
	DWORD   lpflOldProtect;
	//�ı��д״̬
	if(myVirtualProtect((DWORD)p,length,PAGE_EXECUTE_READWRITE,&lpflOldProtect)){
		CopyMemory((void *)p,replace,length);
		//�ָ���д״̬
		myVirtualProtect((DWORD)p,length,lpflOldProtect,&lpflOldProtect);
		OutputDbgInfo("[!] ��ַ0x%x �޸ĳɹ�!!!�\\r\n",(DWORD)p);
	}else
	{
		OutputDbgInfo("[!] ��ַ0x%x ��д״̬ʧ��!!! error: %d�\\r\n",(DWORD)p,GetLastError());
	}
}

VOID getAddress()
{
	OutputDbgInfo("[!] myVirtualProtect��ַ0x%x \r\n",myVirtualProtect);

}

/**************************************
*  ���ܣ��ı��ַ��д״̬
*  ������
****************************************/
BOOL changAddressStatusStatic(DWORD address,int length){	 
	DWORD   lpflOldProtect;
	//�ı��д״̬
	if(myVirtualProtect(address,length,PAGE_EXECUTE_READWRITE,&lpflOldProtect)){
		OutputDbgInfo("[!] ��ַ0x%x ��д״̬�޸ĳɹ�!!!�\\r\n",address);
		return TRUE;
	}
	return FALSE;
}

/**************************************
*  ���ܣ������ڶ�������麯�����ã�Ŀǰֻ������dword����
*  ������
****************************************/
DWORD  virtualCall(DWORD objBase,DWORD offSet_param, ...)
{
	//OutputDbgInfo("[!] �����ַ %0x  ƫ�� %08x \n",objBase,offSet_param);  
	if (!checkAddress(objBase))
	{
		OutputDbgInfo("[!] �����ַ����ȷ %0x\n",objBase);  
		return 0;
	}
	DWORD temp[10]={};
	va_list arg_ptr;  
	DWORD nArgValue = objBase;  
	DWORD nArgCount = 0;  
	va_start(arg_ptr,objBase);  
	do  
	{  
		
		//OutputDbgInfo("[!] the %d the arg:%08x\n",nArgCount,nArgValue);  
		temp[nArgCount]=nArgValue;
		nArgValue=va_arg(arg_ptr,DWORD);  
		nArgCount++;
		if (nArgCount>10)
		{
			break;
		}
		
	}while(nArgValue != VIRTUALCALL_END);  

	if (nArgCount<2||offSet_param>0x10000)
	{
		return 0;
	}
	for (int i=nArgCount-1;i>=2;i--)//�ӵ�������ʼ�ǲ���
	{
	//	OutputDbgInfo("[!] ����Ĳ�����������Ϊ %0x\n",temp[i]);  
		nArgValue = temp[i];
		_asm
		{
			push nArgValue
		}
	}
	try
	{
		_asm
		{
			mov ecx,objBase
			mov eax,[ecx]//�麯�����ַ
			add eax,offSet_param//�麯��ƫ��
			call [eax]   //�麯����ĳƫ�Ƴ���Ӧ�õĺ�����ַ
			mov nArgCount,eax
		}
	}catch (...){
		OutputDbgInfo("[!] ����xxxxxxxxִ���쳣:%d�\\r\n",GetLastError());
	}
	return nArgCount;
}

//�ַ����ָ��
std::vector<std::string> split22(std::string str,std::string pattern)
{
	std::string::size_type pos;
	std::vector<std::string> result;
	str+=pattern;//��չ�ַ����Է������
	int size=str.size();

	for(int i=0; i<size; i++)
	{
		pos=str.find(pattern,i);
		if(pos<size)
		{
			std::string s=str.substr(i,pos-i);
			result.push_back(s);
			i=pos+pattern.size()-1;
		}
	}
	return result;
}

/**************************************
*  ���ܣ���õ��������ĵ�ַ
*  ������
*
*  ����ֵ��
*  �����ˣ�
*  ʱ�䣺[2014/6/4/ Administrator]
****************************************/
DWORD getFunctionAddress(char *dllName,char* functionName)
{
	HANDLE handle =  GetModuleHandleA(dllName);
	return (DWORD)GetProcAddress((HMODULE)handle,functionName);
}
BOOL checkAddressMy(DWORD address)
{
	if (address>0x100000&&address<0x80000000)
	{
		return TRUE;
	}
	return FALSE;
}

void GetAddressMoreInfo(DWORD ebpAddr, char *info,BOOL recodeParams)
{
	vector<DLLINFO>::iterator iter;
	DLLINFO dllInfo;
	DWORD returnAddr = *(DWORD *)(ebpAddr + 4);	//���ص�ַ
	BOOL findModule = FALSE;		//�Ƿ��ҵ�
	BOOL isVTCall = FALSE;
	for(iter=g_dllInfoVec.begin();iter!=g_dllInfoVec.end();iter++)
	{
		dllInfo = *iter;
		if (returnAddr<=dllInfo.modelEnd && returnAddr>=dllInfo.modelBase)
		{
			findModule = TRUE;
			break;
		}
	}

	if (checkAddress(returnAddr))
	{
		//��ͨcalle8���麯��call FF
		isVTCall = (*((UCHAR *)returnAddr - 5) != 0xe8); 
	}

	if (!findModule)
	{
		dllInfo.modelBase = returnAddr;
		dllInfo.dllName = "====";
	}

	if (recodeParams)
	{
		char temp[100];
		if (isVTCall)
		{
			sprintf(temp,"%s","%08X[%06X |%16s]===[%08X, %08X, %08X, %08X] <VT>\r\n");
			
		}else
		{
			sprintf(temp,"%s","%08X[%06X |%16s]===[%08X, %08X, %08X, %08X]\r\n");
		}
		sprintf(info,temp, 
				returnAddr, 
				returnAddr - dllInfo.modelBase, 
				dllInfo.dllName.c_str(),
				*(DWORD *)(ebpAddr + 8),
				*(DWORD *)(ebpAddr + 0xc),
				*(DWORD *)(ebpAddr + 0x10),
				*(DWORD *)(ebpAddr + 0x14)
				);
	}else
	{
		sprintf(info,"%08X[%06X | %s]->", returnAddr, 
			returnAddr - dllInfo.modelBase, dllInfo.dllName.c_str());
	}

}



/**************************************
*  ���ܣ�hook��dump����ջ
*  ������
*
*  ����ֵ��
*  �����ˣ�
*  ʱ�䣺[2014/6/5/ Administrator]
****************************************/
void saveStatckDumpWithPath(string path,DWORD callAddress,DWORD ebp,char *info,BOOL recodeParams)
{
	DWORD nowEbp = ebp;
	DWORD index = 0;
	DWORD value = 0;
	char stackInfo[5000];
	int  count = 0;
	try
	{
		if (info!=NULL)
		{
			if (strlen(info)>20)
			{
				info[19]='\0';
			}
			sprintf(stackInfo,"\r\n%20s >>> \r\n", info);
		}
		while (checkAddress(ebp) && count<=50)//ֻҪջ�׵�ַ��Ч��һֱ����ȥ
		{
			CHAR  ansii_str[100];
			ZeroMemory(ansii_str,100);
			GetAddressMoreInfo(ebp, ansii_str, recodeParams);
			strcat_s(stackInfo, ansii_str);

			ebp = *(DWORD *)ebp;
			count++;
		}

		OutputDbgInfo("[!] ��ջ���� ���ϴ�С:%d  \r\n", count);

		if (callAddress)
		{
			CHAR  ansii_str[50];
			ZeroMemory(ansii_str, 50);
			sprintf(ansii_str,"%08X(VT) -> END\r\n",callAddress);
			strcat_s(stackInfo,ansii_str);
		}

		saveFile(path,stackInfo);
	}catch (...){
		OutputDbgInfo("[!] ����xxxxxxxxִ���쳣:%d \r\n",GetLastError());
	}
}
/**************************************
*  ���ܣ�hook��dump����ջ
*  ������
*
*  ����ֵ��
*  �����ˣ�
*  ʱ�䣺[2014/6/5/ Administrator]
****************************************/
void saveStatckDump(DWORD callAddress,DWORD ebp)
{
	saveStatckDumpWithPath("c:\\stack.txt",callAddress,ebp,"",FALSE);

}

/**************************************
*  ���ܣ�hook��dump����ջ�������麯����
*  ������
*
*  ����ֵ��
*  �����ˣ�
*  ʱ�䣺[2014/6/5/ Administrator]
****************************************/
void saveStatckDump(DWORD ebp, BOOL recodeParams)
{
	saveStatckDumpWithPath("c:\\stack.txt",0,ebp,"",recodeParams);
}

/**************************************
*  ���ܣ�hook��dump����ջ�������麯����
*  ������
*
*  ����ֵ��
*  �����ˣ�
*  ʱ�䣺[2014/6/5/ Administrator]
****************************************/
void saveStatckDump(DWORD ebp)
{
	saveStatckDump(0,ebp);
}

void saveStatckDump(DWORD ebp,char *info)
{
	saveStatckDumpWithPath("c:\\stack.txt",0,ebp,info,FALSE);
}
/**************************************
*  ���ܣ���Ϸ�ﶼ��unicode�ģ�תΪchar*
*  ������
*
*  ����ֵ��
*  �����ˣ�
*  ʱ�䣺[2014/6/11/ Administrator]
****************************************/
char*  myWideToChar(DWORD wideAddr){
	char*  tmpPT="";
	if (checkAddress(wideAddr))
	{
		int len = ::WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)wideAddr, -1, 0, 0, NULL, NULL);
		tmpPT= new char[len+1];
		::WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)wideAddr, -1, tmpPT, len, NULL, NULL);
		tmpPT[len] = 0;
	}else{
		OutputDbgInfo("[!] ���ֽ�תchar*ʧ�ܣ���ַ��%08X ���ɷ��ʣ�",wideAddr);
	}
	return tmpPT;
}

/**************************************
*  ���ܣ����ĳһPE�ļ���ĳһ���εĴ�С����.text��
*  ������filePath �ļ�ȫ·��   dataType ��������
*
*  ����ֵ��
*  �����ˣ�
*  ʱ�䣺[2014/6/21/ Administrator]
****************************************/
PEHEAD getPeHeadSize(const char * filePath,const char * dataType)
{
	DWORD returnSize = 0;
	PEHEAD item ;
	item.virtualOffset=0;
	item.virtualSize=0;
	IMAGE_DOS_HEADER DosHeader = {0};
	IMAGE_FILE_HEADER FileHeader = {0};
	HANDLE  hFile = INVALID_HANDLE_VALUE;
	DWORD  dwReadLen = 0;
	WORD  NumOfSec = 0; //���α����
	IMAGE_SECTION_HEADER *pSecHeader = NULL;
	//���ļ�
	hFile = CreateFileA(filePath,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		OutputDbgInfo("�޷����ļ�������ʧ�ܣ� GetLastError: %d",GetLastError());
		return item;
	}

	//��ȡ����
	if (!ReadFile(hFile,&DosHeader,sizeof(DosHeader),&dwReadLen,NULL))
	{
		OutputDbgInfo("�޷���ȡ�ļ�������ʧ�ܣ� GetLastError: %d",GetLastError());
		return  item;
	}
	SetFilePointer(hFile,DosHeader.e_lfanew + sizeof(IMAGE_NT_SIGNATURE),NULL,FILE_BEGIN);
	ReadFile(hFile,&FileHeader,sizeof(FileHeader),&dwReadLen,NULL);
	//�ó����θ���
	NumOfSec = FileHeader.NumberOfSections;
	//���������ڴ�
	DWORD SectionSize = NumOfSec * IMAGE_SIZEOF_SECTION_HEADER;
	char *pSecBuff = new char[SectionSize + 1];
	//�ƶ�ָ�뵽���α�
	SetFilePointer(hFile,FileHeader.SizeOfOptionalHeader,NULL,FILE_CURRENT);
	ReadFile(hFile,pSecBuff,SectionSize,&dwReadLen,NULL);

	//��ȡ��ϣ��رվ��
	CloseHandle(hFile);

	int iIndexItem = 0;
	TCHAR tzBuff[10] = {0};
	char szSecName[IMAGE_SIZEOF_SHORT_NAME] = {0};
	//�ڴ�����ָ��ת��
	pSecHeader = (PIMAGE_SECTION_HEADER)pSecBuff;
	for (int i = 0; i < NumOfSec; i++)
	{
		memcpy(szSecName,pSecHeader->Name,8);
		if (strcmp(dataType,(const char *)pSecHeader->Name)==0)
		{
			item.virtualSize =  pSecHeader->Misc.VirtualSize;
			item.virtualOffset = pSecHeader->VirtualAddress;
			break;
		}
		//ָ�����
		pSecHeader++;
	}
	//�ͷ��ڴ�
	if (pSecBuff)
	{
		delete []pSecBuff;
	}
	return item;
}

/**************************************
*  ���ܣ���õ�ǰ���̵�����ģ��
*  ������
*
*  ����ֵ��
*  �����ˣ�
*  ʱ�䣺[2014/6/21/ Administrator]
****************************************/
void getProcessModules(OUT map<DWORD, string> &dllMap)
{
	PLDR_DATA_TABLE_ENTRY pLdrDataTable = NULL;
	__asm
	{
		mov eax, fs:[30h]       // eax==PEB
		mov eax, [eax + 0ch]    // eax==Ldr-->PEB_LDR_DATA
		// + 08h InLoadOrderModuleList-->LIST_ENTRY
		// + 10h InMemoryOrderModuleList-->LIST_ENTRY
		// + 14h InInitializationOrderModuleList-->LIST_ENTRY
		mov eax, [eax + 10h]    // InMemoryOrderModuleList
		mov pLdrDataTable, eax 
	}
	PLDR_DATA_TABLE_ENTRY pNextLdrTable = pLdrDataTable;
	// ����ȫ��ģ��
	while ( ((PLIST_ENTRY)pLdrDataTable)->Flink != (PLIST_ENTRY)pNextLdrTable )
	{

		PWSTR pwstr = pNextLdrTable->FullDllName.Buffer;
		size_t len = wcslen(pwstr) + 1;
		size_t converted = 0;
		char *CStr;
		CStr=(char*)malloc(len*sizeof(char));
		wcstombs_s(&converted, CStr, len, pwstr, _TRUNCATE);

		string str(CStr);
		DWORD key = (DWORD)pNextLdrTable->DllBase;
		dllMap[key]=str;
		pNextLdrTable = (PLDR_DATA_TABLE_ENTRY)(PLIST_ENTRY(pNextLdrTable))->Blink;
	}

}
/**************************************
*  ���ܣ������Ҫģ��Ĵ���ε�ַ
*  ������
*
*  ����ֵ��
*  �����ˣ�
*  ʱ�䣺[2014/6/21/ Administrator]
****************************************/
void getModuleDetailInfo(vector<DLLINFO> &vec)
{
	PLDR_DATA_TABLE_ENTRY pLdrDataTable = NULL;
	__asm
	{
		mov eax, fs:[30h]       // eax==PEB
		mov eax, [eax + 0ch]    // eax==Ldr-->PEB_LDR_DATA
		// + 08h InLoadOrderModuleList-->LIST_ENTRY
		// + 10h InMemoryOrderModuleList-->LIST_ENTRY
		// + 14h InInitializationOrderModuleList-->LIST_ENTRY
		mov eax, [eax + 10h]    // InMemoryOrderModuleList
		mov pLdrDataTable, eax 
	}
	PLDR_DATA_TABLE_ENTRY pNextLdrTable = pLdrDataTable;
	DLLINFO dllInfo;
	// ����ȫ��ģ��
	while ( ((PLIST_ENTRY)pLdrDataTable)->Flink != (PLIST_ENTRY)pNextLdrTable )
	{

		PWSTR pwstr = pNextLdrTable->FullDllName.Buffer;
		size_t len = wcslen(pwstr) + 1;
		size_t converted = 0;
		char *CStr;
		CStr=(char*)malloc(len*sizeof(char));
		wcstombs_s(&converted, CStr, len, pwstr, _TRUNCATE);
		string str(CStr);

		DWORD key = (DWORD)pNextLdrTable->DllBase;
		dllInfo.modelBase = (DWORD)pNextLdrTable->DllBase;
		dllInfo.modelEnd = (DWORD)pNextLdrTable->Reserved3[1] + (DWORD)pNextLdrTable->DllBase;
		dllInfo.fullPath=str;
		dllInfo.dllName = str.substr(str.find_last_of("\\")+1,str.length());
		vec.push_back(dllInfo);
		pNextLdrTable = (PLDR_DATA_TABLE_ENTRY)(PLIST_ENTRY(pNextLdrTable))->Blink;
	}

}
/**************************************
*  ���ܣ������Ҫģ��Ĵ���ε�ַ
*  ������
*
*  ����ֵ��
*  �����ˣ�
*  ʱ�䣺[2014/6/21/ Administrator]
****************************************/
void getModuleDetailInfo222(vector<DLLINFO> &vec)
{
	map<DWORD,string> dllMap;
	getProcessModules(dllMap);
	DLLINFO dllInfo;
	map<DWORD,string>::iterator iterMap;
	string exePath;

	OutputDbgInfo("[!] ���ģ����Ϣ  %d",dllMap.size());
	for (iterMap=dllMap.begin();iterMap!=dllMap.end();iterMap++)
	{
		DWORD key = iterMap->first;
		string value = iterMap->second;
		if(strstr(value.c_str(),".exe"))
		{
			OutputDbgInfo("[!] EntryPoint: %08X FullDllName: %s\r\n", key, value.c_str());
			exePath = value;
		}
	}
// 	if (exePath.length()<20)
// 	{
// 		OutputDbgInfo("[!] û���ҵ�\r\n");
// 		return;
// 	}
	//string first20 = exePath.substr(0,12);
	for (iterMap=dllMap.begin();iterMap!=dllMap.end();iterMap++)
	{
		DWORD key = iterMap->first;
		string value = iterMap->second;
		//OutputDbgInfo("[!] ģ��·����%s\r\n",  value.c_str());
	//	if (value.find(first20)!=value.npos)   //����ж���Ѱ����ģ��
	//	{
			PEHEAD item = getPeHeadSize(value.c_str(),".text");
			if (item.virtualSize>0&&item.virtualOffset>0)
			{
				dllInfo.fullPath=value;
				dllInfo.dllName = value.substr(value.find_last_of("\\")+1,value.length());
				dllInfo.textStart=item.virtualOffset+key;
				dllInfo.textEnd = item.virtualOffset+key+item.virtualSize;
				vec.push_back(dllInfo);
			}
	//	}
	}
}



//�����ֽ�char*ת��Ϊ���ֽ�wchar_t*  
wchar_t* AnsiToUnicode( const char* szStr )  
{  
	int nLen = MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, szStr, -1, NULL, 0 );  
	if (nLen == 0)  
	{  
		return NULL;  
	}  
	wchar_t* pResult = new wchar_t[nLen];  
	MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, szStr, -1, pResult, nLen );  
	return pResult;  
}  

//�����ֽ�wchar_t*ת��Ϊ���ֽ�char*  
char* UnicodeToAnsi( const wchar_t* szStr )  
{  
	int nLen = WideCharToMultiByte( CP_ACP, 0, szStr, -1, NULL, 0, NULL, NULL );  
	if (nLen == 0)  
	{  
		return NULL;  
	}  
	char* pResult = new char[nLen];  
	WideCharToMultiByte( CP_ACP, 0, szStr, -1, pResult, nLen, NULL, NULL );  
	return pResult;  
}  


DWORD ConverToHex(CString str, int jinzhi)
{
	DWORD dwObject = 0;
	try
	{
		dwObject = stol(str.GetBuffer(0),FALSE,jinzhi);  
	}
	catch (...)
	{
		OutputDbgInfo("[!] %s ת��Ϊ %d ���Ƴ����쳣������\r\n",
			str.GetBuffer(str.GetLength()), jinzhi);
	}
	return dwObject;
}

extern SENDHOOKFILTER g_sHookFilter;
void PrintBuffer(BOOL isTcp,PVOID buffer, int length)
{
	DWORD ebpAddr = 0;

	char temp[800];
	if (isTcp)
	{
		sprintf(temp, "[!] <TCP> �����Ϣ %02x : ", length);
	}else
	{
		sprintf(temp, "[!] <UDP> �����Ϣ %02x : ", length);
	}
	
	//���������
	//�������������Ϣ��������DbgView�鿴�������UI��������SendMessage��ʽ~  
	char t[500];
	for (int i=0;i<length/2;i=i++)
	{
		
		sprintf(t, "%04x ", *((unsigned short*)buffer+i));
		strcat(temp,t);
		if (i>100)
		{
			strcat(temp,"*******");
			break;
		}
	}
	OutputDebugStringA(temp);

	if ((isTcp&&g_sHookFilter.bWatchTCPToStack)//tcp���Ҽ��udp
		||(!isTcp&&g_sHookFilter.bWatchUDPToStack))//udp���Ҽ��udp
	{
		_asm{
			mov ebpAddr, ebp
		}
		saveStatckDump(ebpAddr,true);
	}

}