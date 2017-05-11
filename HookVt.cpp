#include "stdafx.h"
#include "HookVt.h"
#include <stdarg.h>   
#include <stdio.h> 

CHookVt::CHookVt(void)
{

}

CHookVt::~CHookVt(void)
{

}
std::set<DWORD> S_OutSet;
void resetAddress(DWORD dwPlayerBaseAddr);

DWORD _stdcall Monitor(DWORD dwVirtualOffset,DWORD vtBaseAddr);
typedef DWORD (_stdcall *pMonitor)(DWORD,DWORD);
pMonitor g_pVirtualMonitor = Monitor;


VOID   Dispatch(DWORD dwVirtualOffset,DWORD vtBaseAddr);

/**************************************
*  ���ܣ��滻�������̻߳ص�
*  ������
*
*  ����ֵ��
*  �����ˣ�
*  ʱ�䣺[2014/4/27/ Administrator]
****************************************/
DWORD WINAPI ThreadProc(LPVOID lpParameter){
	try{
		Sleep(500);
		//WaitForSingleObject(g_hEventUnhook, INFINITE);
		HOOK_VT_TYPE  *vtType = (HOOK_VT_TYPE*)lpParameter;
		//�滻���
		vtType->g_pTrueVirtualTbl = (PVOID)(*(PDWORD)vtType->dwObject);	   //�����Ļ���ַ
		memcpy((PDWORD)vtType->g_pFakeVirtualTbl + 2, (PDWORD)vtType->g_pTrueVirtualTbl - 3, 12);
		OutputDbgInfo("[!] ����[!] %08x %08x %08x %08x !\n",vtType->dwObject,vtType->g_pFakeVirtualTbl,vtType->g_pTrueVirtualTbl,vtType->hDstThread);
		*( PDWORD )vtType->dwObject = (DWORD)((PDWORD)vtType->g_pFakeVirtualTbl + 5);
		OutputDbgInfo("[!] ����[!] �ص������ָ��߳�!���޸ĺ��ֵΪ��%08X\n",*( PDWORD )vtType->dwObject);
		//�ָ��߳� hDstThread
		ResumeThread(vtType->hDstThread);
	}catch( ... ){
		OutputDbgInfo("[!] ����[!] ThreadProc Exception Error!\n");
		return false;
	}
	OutputDbgInfo("[!] ����[!] ThreadProc Exit!\n");
	return true;
}

/**************************************
*  ���ܣ�����滻�������̻߳ص�
*  ������
*
*  ����ֵ��
*  �����ˣ�
*  ʱ�䣺[2014/4/27/ Administrator]
****************************************/
DWORD WINAPI ThreadProcUnHook(LPVOID lpParameter){
	HOOK_VT_TYPE  *vtType = (HOOK_VT_TYPE*)lpParameter;
	try{
		Sleep(500);
		*(PDWORD)vtType->dwObject = (DWORD)vtType->g_pTrueVirtualTbl;
		OutputDbgInfo("[!] ��ַ %08X ��map���Ƴ� ��",vtType->dwObject);
		ResumeThread(vtType->hDstThread);
	}catch( ... ){
		OutputDbgInfo("[!] ����[!] ThreadProc Exception Error!\n");
		return false;
	}
	OutputDbgInfo("[!] ����[!] ThreadProc Exit!\n");
	return true;
}

/**************************************
*  ���ܣ�Hook���
*  ������
*
*  ����ֵ��
*  �����ˣ�
*  ʱ�䣺[2014/4/27/ Administrator]
****************************************/
BOOL CHookVt::HookVirtualTable(DWORD dwPlayerBaseAddr)
{
	PVOID pFakeVTbl = NULL;
	HANDLE hNewThread = NULL;
	bool   bSuccess = false;
	PDWORD pdwFakeVtbl = NULL;
	
	if(vtMap.find(dwPlayerBaseAddr)!=vtMap.end())
	{
		OutputDbgInfo("[!] ����[!] ����%08X �Ѿ���HOOK�ˣ� \n",dwPlayerBaseAddr);
		return FALSE;
	}
	OutputDbgInfo("[!] ����[!] HookVirtualTable Enter!\n");
	HOOK_VT_TYPE vtType;
	vtType.dwObject = dwPlayerBaseAddr;
//	resetAddress(dwPlayerBaseAddr);
	try
	{
		//ͬ��HOOK��UNHOOK
        vtType.g_hEventUnhook = CreateEvent(NULL, FALSE, TRUE, NULL);
		if ( !vtType.g_hEventUnhook )
		{
			OutputDbgInfo("[!] ����[-] HookVirtualTable CreateEvent fail!");
			return FALSE;
		}
		
		//�����ٵ����
		PVOID pFakeVTbl = CreateFakeVirtualTable(dwPlayerBaseAddr);
		if ( !pFakeVTbl )
		{
			OutputDbgInfo("[!] ����[-] HookVirtualTable CreateFakeVirtualTable Error!");
			return bSuccess;
		}
		vtType.g_pFakeVirtualTbl = pFakeVTbl;
		
		pdwFakeVtbl = (PDWORD)pFakeVTbl;
		*(pdwFakeVtbl + 1) = dwPlayerBaseAddr;
		

		DWORD  dwDstThread =getThreadId();
	
		cout<<"dwDstThread2"<<hex<<(int)dwDstThread<<endl;

		//�����߳�
		vtType.hDstThread  = OpenThread(THREAD_SUSPEND_RESUME, NULL, dwDstThread);
		if ( !vtType.hDstThread)
		{
			OutputDbgInfo("[!] ����[!] ThreadProc OpenThread error!\n");
			return false;
		}
		//�����߳����滻�����
		HANDLE hNewThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadProc,&vtType, 0, NULL);
		if (!hNewThread)
		{
			OutputDbgInfo("[!] ����[-] CreateThread Error!");
			return bSuccess;
		}
		OutputDbgInfo("[!] ����[-] ��ʼ�����߳�!");
		if( (DWORD)-1 == SuspendThread(vtType.hDstThread))
		{  
			OutputDbgInfo("[!] ����[!] ThreadProc SuspendThread error!\n");
			return false;
		}
		vtMap[dwPlayerBaseAddr]=vtType;
		OutputDbgInfo("[!] ����[!] %08x %08x %08x %08x !\n",vtType.dwObject,vtType.g_pFakeVirtualTbl,vtType.g_pTrueVirtualTbl,vtType.hDstThread);
	}
	catch( ... )
	{
		OutputDbgInfo("[!] ����[!] HookVirtualTable Exception Exit!\n");
		return NULL;
	}
	
	OutputDbgInfo("[!] ����[!] HookVirtualTable Exit!\n");
	bSuccess = TRUE;
	return bSuccess;
}


/**************************************
*  ���ܣ�unHOOK
*  ������
*
*  ����ֵ��
*  �����ˣ�
*  ʱ�䣺[2014/5/17/ Administrator]
****************************************/
VOID CHookVt::UnHookVirtualTable(DWORD dwPlayerBaseAddr){
	if(vtMap.find(dwPlayerBaseAddr)==vtMap.end())
	{
		OutputDbgInfo("[!] ���ˣ���ַ %08X û����map��==��",dwPlayerBaseAddr);
		return;
	}
	//�����߳�
	HANDLE hDstThread  = OpenThread(THREAD_SUSPEND_RESUME, NULL, getThreadId());
	vtMap[dwPlayerBaseAddr].hDstThread = hDstThread;
	//�����߳����滻�����
	HANDLE hNewThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadProcUnHook,&vtMap[dwPlayerBaseAddr], 0, NULL);
	if (!hNewThread)
	{
		OutputDbgInfo("[!] ����[-] CreateThread Error!");
		return ;
	}

	OutputDbgInfo("[!] ����[-] ��ʼ�����߳�--ȡ��hook!");
	
	if( (DWORD)-1 == SuspendThread(hDstThread))
	{  
		OutputDbgInfo("[!] ����[!] ThreadProc SuspendThread error!\n");
		return ;
	}
	vtMap.erase(dwPlayerBaseAddr);
}

/**************************************
*  ���ܣ��������������ķַ�����
*  ������
*
*  ����ֵ��
*  �����ˣ�
*  ʱ�䣺[2014/4/27/ Administrator]
****************************************/
VOID  __declspec(naked) Dispatch(DWORD offsetAddr, DWORD vtBaseAddr)
{
	
	//ת��Monitor������ʵ������ʵ�ֵĹ���,����ֵΪ�������麯����ַ
    _asm
	{
		call  DWORD PTR [g_pVirtualMonitor]
	}

	//�ַ�����ʵ���ĵ��û���˵��õ���
	_asm
	{
		cmp eax, 0
		jz _NO_EXCUTE_VIRTUAL_FUC

		//���зַ�����Ӧ���麯����eax����ʵ�麯���ĵ�ַ
		mov [esp+36], eax
		popfd
		popad
        ret

		//��ִ�и��麯��,���ڲ�֪���麯���Ĳ����������ݲ�֧��
		//Ҫ֧�ֵĻ��ͱ��붯̬�����麯����������ĩβ��C3��C2������
        _NO_EXCUTE_VIRTUAL_FUC:
		popfd
		popad
		lea esp, dword ptr [esp+4]
		ret
	}
}
int justOneTime;
bool firstTime = TRUE;
std::set<DWORD> bset ;
DWORD _stdcall Monitor(DWORD dwVirtualOffset,DWORD vtBaseAddr)
{
	DWORD ebpAddr,dwCallerRetAddr;
	DWORD callAddress = *(DWORD *)(vtBaseAddr + dwVirtualOffset * 4);
	_asm
	{
		mov ebpAddr,ebp
			mov eax, [ebp+56]
		mov dwCallerRetAddr, eax
	}
	if (FilterSet.find(dwVirtualOffset*4)!=bset.end())
	{
		goto showDetail;
	}else if (IsOnly)
	{
		return callAddress;
	}

	if (firstTime)
	{
		justOneTime = GetTickCount();
		firstTime = FALSE;
	}
	if (bset.find(dwVirtualOffset)!=bset.end())
	{
		return callAddress;
	}

	if (GetTickCount()-justOneTime<5000)
	{
		bset.insert(dwVirtualOffset);
		OutputDbgInfo("[!] ����������������ţ�%04d  ",dwVirtualOffset);
		return  callAddress;
	}
	showDetail:
	DWORD param1,param2,param3,param4;
	_asm
	{
		mov eax, [ebp+60]
		mov param4, eax

			mov eax, [ebp+64]
		mov param3, eax

			mov eax, [ebp+68]
		mov param2, eax

			mov eax, [ebp+72]
		mov param1, eax
	}

	OutputDbgInfo("[!] ������offset: %04x p1: %08x p2: %08x p3: %08x p4: %08x  %08x -> %08x\n",dwVirtualOffset * 4,param1,param2,param3,param4,callAddress,dwCallerRetAddr);
	saveStatckDump(callAddress,ebpAddr);
		
	return callAddress;
}

/**************************************
*  ���ܣ��������������ļ�غ���
*  ������
*
*  ����ֵ��
*  �����ˣ�
*  ʱ�䣺[2014/4/27/ Administrator]
***************************************
DWORD _stdcall Monitor(DWORD dwVirtualOffset)
{
	DWORD dwResult = 0;
	DWORD dwCalledFuncAddr, dwCallerRetAddr,dwEbpAddr;
	DWORD param1,param2,param3,param4;
	try
	{
        dwResult = (DWORD)g_pTrueVirtualTbl + dwVirtualOffset * 4;
		dwCalledFuncAddr = *(DWORD *)dwResult;
		
		_asm
		{
			mov eax, [ebp+52]
			mov dwCallerRetAddr, eax
			mov dwEbpAddr,ebp
		}
		if(firstTime){
			justOneTime =  GetTickCount();
			firstTime=false;
		}
		if(bset.find(dwVirtualOffset)!=bset.end()){
			return dwCalledFuncAddr;
		}
		if(GetTickCount()-justOneTime<5000){//5s��֮�ڵ�ȫ������������
			bset.insert(dwVirtualOffset);
			OutputDbgInfo("[!] ƫ�ƣ�0x%04x ���õ�ַ��0x%08x �����������\n",dwVirtualOffset * 4,dwCalledFuncAddr);
			return dwCalledFuncAddr;
		}
		if ( isAll||objOffset.find(dwVirtualOffset * 4) !=objOffset.end())
		{
			OutputDbgInfo("[!] EBP��ַ��0x%08x ��\n",dwEbpAddr);
			saveStatckDump(dwEbpAddr,TRUE);
		}
		_asm
			{
				mov eax, [ebp+56]
				mov param4, eax

					mov eax, [ebp+60]
				mov param3, eax

					mov eax, [ebp+64]
				mov param2, eax

					mov eax, [ebp+68]
				mov param1, eax
			}

			OutputDbgInfo("[!] ������offset: %04x p1: %08x p2: %08x p3: %08x p4: %08x  %08x -> %08x\n",dwVirtualOffset * 4,param1,param2,param3,param4,dwCalledFuncAddr,dwCallerRetAddr);
		
	}catch( ... )
	{
		OutputDbgInfo("[!] ����[!] Monitor Exception Exit!\n");
		return dwCalledFuncAddr;
	}
	return dwCalledFuncAddr;
}
*/

/**************************************
*  ���ܣ������ٵ�����
*  ������
*
*  ����ֵ��
*  �����ˣ�
*  ʱ�䣺[2014/4/27/ Administrator]
****************************************/
PVOID  CHookVt::CreateFakeVirtualTable(DWORD dwPlayerBaseAddr)
{
	PVOID  pFakeVTbl    = NULL, pJmpTable = NULL;
	PDWORD pTrueVTbl = NULL;
	PDWORD pdwFakeVTblItem = NULL;
	PJMP_TABLE_ITEM pJmpTblItem = NULL;
    DWORD dwPlayActObj = 0, dwVtbLenth = 0;
	char  szOut[MAX_PATH];

	//�������պ�����ַָ�����
	typedef VOID (*pMonitorFn)(DWORD,DWORD);
	pMonitorFn pMonFn;
	pMonFn     = Dispatch;
    PDWORD pdwMonitorFn   = (PDWORD)pMonFn;
    
	OutputDbgInfo("[!] ����[!] CreateFakeVirtualTable Enter!\n");
	PVOID g_pFakeVirtualTbl = 0;
	try
	{

		dwPlayActObj = dwPlayerBaseAddr;
        pTrueVTbl    = (PDWORD)(*( PDWORD )dwPlayActObj);
		
		dwVtbLenth = 40000;
		sprintf(szOut, "Player Addr: 0x%.8x, True VTB:0x%.8x, VTbLen: %d\n", dwPlayActObj, pTrueVTbl, dwVtbLenth);
		AddLB(szOut);

		pJmpTable = VirtualAlloc(NULL, dwVtbLenth * JMP_TABLE_ITEM_SIZE, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
		if ( !pJmpTable )
		{
			OutputDbgInfo("[!] ����[!] CreateFakeVirtualTable MMalloc Jmp Table fail!\n");
		    return NULL;
		}
		cout<<"��ת��pJmpTable��ַ�� 0x"<<hex<<pJmpTable<<endl;
		//��������
		pFakeVTbl = VirtualAlloc(NULL, (dwVtbLenth + 1 ) * 4 + 16, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
		if ( !pFakeVTbl )
		{
			OutputDbgInfo("[!] ����[!] CreateFakeVirtualTable MMalloc Fake Virtual Table fail!\n");
			if ( pJmpTable )
			{
				MMFree( pJmpTable );
			}
		    return NULL;
		}
		cout<<"��ٱ�pFakeVTbl��ַ�� 0x"<<hex<<pFakeVTbl<<endl;
		//��ʼ�����ű�
		ZeroMemory(pJmpTable, JMP_TABLE_ITEM_SIZE * dwVtbLenth);
		ZeroMemory(pFakeVTbl, (dwVtbLenth + 1) * 4 + 16);
        
		g_pFakeVirtualTbl  = pFakeVTbl;

		//������ͷ���ֽڱ�����ת��Ļ���ַ
		*(PDWORD)pFakeVTbl = (DWORD)pJmpTable;

		pdwFakeVTblItem = (PDWORD)pFakeVTbl;
		pdwFakeVTblItem += 5;

		pJmpTblItem     = (PJMP_TABLE_ITEM)pJmpTable;
		for ( DWORD dwLoop = 0; dwLoop < dwVtbLenth; dwLoop++ )
		{
             pJmpTblItem->uPushEbx = 0x53;
			 pJmpTblItem->uPushad = 0x60;
			 pJmpTblItem->uPushfd = 0x9C;
			 pJmpTblItem->uMovEaxFirst[0]=0xB8;
			  *((DWORD *)&(pJmpTblItem->uMovEaxFirst[1])) = (DWORD)pTrueVTbl;
			 pJmpTblItem->uPushEaxFirst= 0x50;
			 pJmpTblItem->uMovEax[0] = 0xB8;
			 *((DWORD *)&(pJmpTblItem->uMovEax[1])) = dwLoop;
			 pJmpTblItem->uPushEax = 0x50;
			 pJmpTblItem->uJmp[0]  = 0xE9;
             *((DWORD *)&(pJmpTblItem->uJmp[1])) = (DWORD)(pdwMonitorFn) - ( (DWORD)(pJmpTblItem) + FIELD_OFFSET(JMP_TABLE_ITEM, uSave) );
			 

			 *pdwFakeVTblItem = (DWORD)pJmpTblItem;

			 pdwFakeVTblItem++;
			 pJmpTblItem++;
		}
	    FlushInstructionCache( GetCurrentProcess(), pJmpTable, dwVtbLenth *  JMP_TABLE_ITEM_SIZE);//�������������д�����ڴ���
		FlushInstructionCache( GetCurrentProcess(), pFakeVTbl, (dwVtbLenth + 1) * 4 );

	}
	catch( ... )
	{
       OutputDbgInfo("[!] ����[!] CreateFakeVirtualTable Exception Exit!\n");

	   if (pJmpTable)
	   {
		   MMFree(pJmpTable);
	   }

	   if (pFakeVTbl)
	   {
           MMFree(pFakeVTbl);
	   }
	   return NULL;
	}

	OutputDbgInfo("[!] ����[!] CreateFakeVirtualTable Exit!\n");
	return g_pFakeVirtualTbl;
}




//����4��ΪΪʵ�ֺ���
void AddLB( char szOut[]){};
void MMFree(PVOID pJmpTable){};
PDWORD MMalloc(DWORD dw){return NULL;};
/*
void resetAddress(DWORD dwPlayerBaseAddr){
	//bset.insert(0xa0);
	//bset.insert(0x54);
	vector<VTINFO> vec;
	vector<VTINFO>::iterator iter;
	DecVtAddress(dwPlayerBaseAddr,0xa00/4,vec);
	for (iter = vec.begin();iter!=vec.end();iter++)
	{
		if (iter->type==FUNCTION_GET||iter->type==FUNCTION_UNIMPORTENT)
		{
			bset.insert(iter->offset/4);
		}
	}
	return;

	//bset.insert(0x934/4);//����ƶ�		65e19c0  200  1b  305  9405d0->8ef39d
	bset.insert(0xa80/4);//���ڻ�ý���	a80  0  608aa  8ef546  2  93fb30->8eef70 
	bset.insert(0xa20/4);//�������		25faa5c0  c4000b0  354f0640  8  940930->116ec0b
	bset.insert(0x938/4);//�������		65e19c0   202      171      181  940780->8ef2a4
	bset.insert(0xa24/4);//�������		25faa5c0  c4000b0  354f0640  8  940960->116ec29
	bset.insert(0xa28/4);//�������		25faa5c0  c4000b0  354f0640  8  9480e0->116ecdc
	bset.insert(0xa2c/4);//�Ҽ�����		25faa5c0  c4000b0  354f0640  8  940990->116ed56
	bset.insert(0x1bc/4);//�Ҽ�����		3d1b2f80  14  95094ea1  3d1b2f80  ee6b60->b52864

	bset.insert(0x604/4);//�ı�����		381b7000  1c  147  316  ad9630->ad4a4c 
	bset.insert(0xf0/4);//�ı�����		12d26c  0  147  316  aec2d0->a9ff77 
	bset.insert(0x9c/4);//�ı�����		4ba08090  0  147  316  a76b00->b12212 
}*/

void resetAddress(DWORD dwPlayerBaseAddr){}