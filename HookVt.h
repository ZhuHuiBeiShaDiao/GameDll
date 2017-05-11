#include <windows.h>
#include <stdio.h>
#include <string>
#include <iostream>
using  namespace std;
#pragma once
//��ת����Ľṹ
typedef struct _JMP_TABLE_ITEM
{
	UCHAR    uPushEbx;    //push ebx 0x53, ������������麯���ĵ�ַ
	UCHAR    uPushad;     //pushad 0x60
	UCHAR    uPushfd;     //pushfd 0x9C
	UCHAR    uMovEaxFirst[5];  //mov eax, 0xffffffff = B8 ffffffff
	UCHAR    uPushEaxFirst;    //push eax = ���ı��
	UCHAR    uMovEax[5];  //mov eax, 0xffffffff = B8 ffffffff
	UCHAR    uPushEax;    //push eax = ���ı��
	UCHAR    uJmp[5];     //jmp 0xfffffffe = e9...
	UCHAR    uSave;       //�����Ժ���
}JMP_TABLE_ITEM, *PJMP_TABLE_ITEM;

typedef struct _HOOK_VT_TYPE
{
	DWORD    dwObject;         //�����ַ
	DWORD    monitorFunction;  //
	PVOID	 g_pTrueVirtualTbl;//ԭ��������ַ
	PVOID	 g_pFakeVirtualTbl;//������ַ
	HANDLE   hDstThread;
	HANDLE   g_hEventUnhook ;
	int justOneTime;
	bool firstTime;
} HOOK_VT_TYPE;

class CHookVt{
public:
	CHookVt(void);
	~CHookVt(void);
	/**************************************
	*  ���ܣ�Hook���
	****************************************/
	BOOL HookVirtualTable(DWORD dwPlayerBaseAddr);

	/**************************************
	*  ���ܣ�unHOOK
	****************************************/
	VOID UnHookVirtualTable(DWORD dwPlayerBaseAddr);
public:
	
	

private:
	map<DWORD,HOOK_VT_TYPE> vtMap;
	/**************************************
	*  ���ܣ������ٵ�����
	****************************************/
	PVOID CreateFakeVirtualTable(DWORD dwPlayerBaseAddr);


	std::map<int,PVOID> addressMap;

	



};












//����4��ΪΪʵ�ֺ���
void AddLB( char szOut[]);
void MMFree(PVOID pJmpTable);
PDWORD MMalloc(DWORD dw);
//�ַ��������������
VOID   Dispatch(DWORD dwVirtualOffset);
BOOL HookVirtualTable(DWORD dwPlayerBaseAddr);
VOID   ProcessVirtualFuncCall(DWORD dwVFOffset, DWORD dwCalledAddr, DWORD dwCallerAddr);
VOID UnHookVirtualTable(DWORD dwPlayerBaseAddr);
bool HookVirtualIndex(int dwPlayerBaseAddr,int index);

DWORD _stdcall MonitorSingle();
VOID UnHookVirtualIndex(int dwPlayerBaseAddr,int index);



#define  JMP_TABLE_ITEM_SIZE sizeof(JMP_TABLE_ITEM)

//�����ٵ����
PVOID CreateFakeVirtualTable(DWORD dwPlayerBaseAddr);
//ͳ�����ĵ���
enum
{
	INIT_CALL_LOG_MEM = 1,
	UNINIT_CALL_LOG_MEM,
	OPEN_CALL_LOG,
	CLOSE_CALL_LOG
};
static BYTE   g_bVCallStatus = UNINIT_CALL_LOG_MEM;
const  DWORD  CALL_LOG_MEM_SIZE = 1024 * 4 * 2;
static PBYTE  g_pbCallLogMem  = NULL;
VOID   ProcessVirtualFuncCall(DWORD dwVFOffset, DWORD dwCalledAddr, DWORD dwCallerAddr); // ����Ψһ�Եĵ��ü�¼
VOID   InitVirtualFuncCall(); // ��ʼ���������������¼
VOID   OpenVirtualFuncCall(); // �򿪵��ü�¼���˵���¼��ĵ���
VOID   CloseVirtualFuncCall(); // �رյ��ü�¼��Ӧ��