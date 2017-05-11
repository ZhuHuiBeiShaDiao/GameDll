#include "stdafx.h"
#include "windows.h"
#include "winnt.h"



//////////////////////////////////////////////////////////////////////////
//
//  Hook mswsock.dll�������Ntdll!NtDeviceIoControlFile
//  ���������TDI Cilent�����������˷��
//  �ȶ������Σ�RING3����ײ�İ�����~
//
//////////////////////////////////////////////////////////////////////////
BOOL SuperHookParseFileFunction(
	char *ModuleName,
	char *FunctionName,
	PVOID HookFunctionProc,
	PVOID *FunctionRet,
	PVOID tableIndexAdd)
{
	//�õ�ws2_32.dll��ģ���ַ
	HMODULE hMod = LoadLibraryA(ModuleName);
	if (hMod == 0 )
	{
		return FALSE;
	}

	//�õ�DOSͷ

	PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)hMod ; 

	//���DOSͷ��Ч
	if (pDosHeader->e_magic != IMAGE_DOS_SIGNATURE)
	{
		return FALSE;
	}

	//�õ�NTͷ

	PIMAGE_NT_HEADERS pNtHeaders = (PIMAGE_NT_HEADERS)((ULONG)hMod + pDosHeader->e_lfanew);

	//���NTͷ��Ч
	if (pNtHeaders->Signature != IMAGE_NT_SIGNATURE)
	{
		return FALSE;
	}

	//������������Ŀ¼�Ƿ����
	if (pNtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress == 0 ||
		pNtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].Size == 0 )
	{
		return FALSE;
	}
	//�õ����������ָ��

	PIMAGE_IMPORT_DESCRIPTOR ImportDescriptor = (PIMAGE_IMPORT_DESCRIPTOR)((ULONG)hMod + pNtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);

	PIMAGE_THUNK_DATA ThunkData ; 

	//���ÿ��������
	while(ImportDescriptor->FirstThunk)
	{
		//�����������Ƿ�Ϊntdll.dll

		char* dllname = (char*)((ULONG)hMod + ImportDescriptor->Name);

		//������ǣ���������һ������

		if (stricmp(dllname , "ntdll.dll") !=0)
		{
			ImportDescriptor ++ ; 
			continue;
		}

		ThunkData = (PIMAGE_THUNK_DATA)((ULONG)hMod + ImportDescriptor->OriginalFirstThunk);

		int no = 1;
		while(ThunkData->u1.Function)
		{
			//��麯���Ƿ�ΪNtDeviceIoControlFile

			char* functionnameIn = (char*)((ULONG)hMod + ThunkData->u1.AddressOfData + 2);
			if (stricmp(functionnameIn , FunctionName) == 0 )
			{
				//
				//����ǣ���ô��¼ԭʼ������ַ
				//HOOK���ǵĺ�����ַ
				//
				ULONG myaddr = (ULONG)HookFunctionProc;
				ULONG btw ; 
				PDWORD lpAddr = (DWORD *)((ULONG)hMod + (DWORD)ImportDescriptor->FirstThunk) +(no-1);
				*(DWORD *)tableIndexAdd = (DWORD)lpAddr;
				*FunctionRet = (PVOID)(*(ULONG*)lpAddr) ; 
				WriteProcessMemory(GetCurrentProcess() , lpAddr , &myaddr , sizeof(ULONG), &btw );
				return TRUE; 

			}

			no++;
			ThunkData ++;
		}
		ImportDescriptor ++;
	}
	return FALSE;
}

BOOL SuperUnHookParseFileFunction(
	PVOID FunctionRet,
	PVOID tableIndexAdd)
{
	if(FunctionRet == NULL || tableIndexAdd==NULL)
	{
		return FALSE;
	}
	ULONG btw ; 
	//���������ָ��д��ȥ
	WriteProcessMemory(GetCurrentProcess() , tableIndexAdd ,
		&FunctionRet , sizeof(ULONG), &btw );
	return TRUE;
}