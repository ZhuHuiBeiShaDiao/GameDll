#ifndef _HOOK_CALLBACK_HEADER_
#define _HOOK_CALLBACK_HEADER_

/**************************************
*  ���ܣ�Hook Systemcall �Ĺ��˺��� 
*  ������
*
*  ����ֵ��
*  ʱ�䣺[2015/1/31 14:04 Administrator]
****************************************/
BOOL _stdcall HookSystemCallBack(DWORD index);

void initCallIndex();

DWORD _stdcall HookCheckCallBack();



#endif