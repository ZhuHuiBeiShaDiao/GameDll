//////////exediy 8763232  2014��7��6��
#pragma once


class HookSystemCall
{
public:
	HookSystemCall(void);
	~HookSystemCall(void);
	
	//��ʼ��
	BOOL Initialization(BOOL (__stdcall * myHookRet)(DWORD index));

	//���ú���GetDllFuncAddr�Ա��÷����
	int GetSysCallIndex( PCHAR FuncName );

	// ��װ����
	BOOL InstallHook(PCHAR FunName,			//NTAPI
					 DWORD pHookFunc,		//HOOK�ĺ���
					 DWORD * pOrigFunc);	//���صĺ���

	// Ex��װ����
	BOOL InstallHookEx(DWORD dwIndex,		//NTAPI
						WORD dwRetIndex,	//���ض�ջƽ��
						DWORD pHookFunc,		//HOOK�ĺ���
						DWORD * pOrigFunc);	//���صĺ���
	// ж��HOOK
	BOOL UnHook(PCHAR FunName);

	// ж��ϵͳ����
	BOOL UnSystemHook();

};

