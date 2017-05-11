#include "stdafx.h"
#include "HookFunctions.h"
#include "HookSystemCall.h"
#include "Afd.h"
/**************************************
	DisapterMessageW  Hook
****************************************/
extern int g_FilterMessageType;
extern int g_FilterSendType;

typedef LONG (WINAPI *MESSAGECALL)( const MSG *pMsg );
BOOL isDisapterMessageHooked = FALSE;
PVOID DisapterMessageWRet;
int DisapterMessageWPatchCodeLength;
BOOL bool_ret = FALSE;

//�м̺���
__declspec(naked) LONG DisapterMessageWHookZone(...)
{
	_asm
	{
		_emit 0x90;
		_emit 0x90;
		_emit 0x90;
		_emit 0x90;
		_emit 0x90;
		_emit 0x90;
		_emit 0x90;
		_emit 0x90;
		jmp [DisapterMessageWRet];

	}
}

//�Լ��߼��ĺ���
LONG NTAPI NewFreeLibrary(
	MSG* pmsg
	)
{
	MESSAGECALL messageCall;
	if(pmsg->message == g_FilterMessageType)
	{
		OutputDbgInfo("[!] HWAND: %08X  TYPE: %04X CODE: %04X  LPARAM: %08X TIME: %08X X: 0x%04d Y: %04d \r\n",
			pmsg->hwnd, 
			pmsg->message,
			pmsg->wParam,
			pmsg->lParam, 
			pmsg->time, 
			pmsg->pt.x,
			pmsg->pt.y);
		//pmsg->lParam = 0;
		//pmsg->wParam = 0;
	}

	messageCall = (MESSAGECALL)DisapterMessageWHookZone;
	return messageCall(pmsg);
}

//hook����
BOOL HookDispatchMessageW()
{
	if (isDisapterMessageHooked)
	{
		OutputDebugStringA("[!] DispatchMessageW have been hooked!!!\r\n");
		return FALSE;
	}
	int temp = (DWORD)DisapterMessageWHookZone 
		+ *(DWORD *)((char *)DisapterMessageWHookZone + 1) + 5;
	BOOL ret = HookFunctionInHeader(
		"user32.dll",
		"DispatchMessageW",
		NewFreeLibrary,
		(PVOID)temp,
		&DisapterMessageWPatchCodeLength,
		&DisapterMessageWRet);
	if (ret)
	{
		OutputDebugStringA("[!] HookDispatchMessageW success\r\n");
		isDisapterMessageHooked = TRUE;
	}else
	{
		OutputDebugStringA("[!] HookDispatchMessageW failed\r\n");
	}
	return ret;
}

//unhook����
BOOL UnHookDispatchMessageW()
{
	if (!isDisapterMessageHooked)
	{
		OutputDebugStringA("[!] DispatchMessageW have not been hooked!!!\r\n");
		return FALSE;
	}
	int temp = (DWORD)DisapterMessageWHookZone 
		+ *(DWORD *)((char *)DisapterMessageWHookZone + 1) + 5;
	BOOL ret = UnHookFunctionInHeader(
		"user32.dll",
		"DispatchMessageW",
		(PVOID)temp,
		DisapterMessageWPatchCodeLength);
	if (ret)
	{
		OutputDebugStringA("[!] UnHookDispatchMessageW success\r\n");
		isDisapterMessageHooked = FALSE;
	}else
	{
		OutputDebugStringA("[!] UnHookDispatchMessageW failed\r\n");
	}
	return ret;
}

/**************************************
	NtDeviceIoControl  Hook  �շ���
****************************************/

PVOID pNtDeviceIoControl  = NULL ; 
PVOID pNtDeviceIoControlRet;
PVOID pTableIndexAddr	  = 0;
BOOL  isNtDeviceIoControlHooked = FALSE;
typedef NTSTATUS (WINAPI *TrueNtDeviceIoControl)(
	HANDLE FileHandle,
	HANDLE Event OPTIONAL,
	PVOID ApcRoutine OPTIONAL,
	PVOID ApcContext OPTIONAL,
	PVOID IoStatusBlock,
	ULONG IoControlCode,
	PVOID InputBuffer OPTIONAL,
	ULONG InputBufferLength,
	PVOID OutputBuffer OPTIONAL,
	ULONG OutputBufferLength);

TrueNtDeviceIoControl pTrueNtDeviceIoControl;

typedef LONG NTSTATUS;

SENDHOOKFILTER g_sHookFilter;

//���ݽ�������ж��Ƿ���ϼ������
BOOL IsFilterOk(ULONG Len, DWORD params, COMPARESULT compareType)
{
	if (compareType==COMPARE_EQUIL && Len != params)
	{
		return FALSE;
	}
	if (compareType==COMPARE_LESS && Len >= params)
	{
		return FALSE;
	}
	if (compareType==COMPARE_BIG && Len <= params)
	{
		return FALSE;
	}
	
	return TRUE;
}


//�Ƿ���udp��ȫ�ֱ���

NTSTATUS __stdcall NewNtDeviceIoControlFile(
	HANDLE FileHandle,
	HANDLE Event OPTIONAL,
	PVOID ApcRoutine OPTIONAL,
	PVOID ApcContext OPTIONAL,
	PVOID IoStatusBlock,
	ULONG IoControlCode,
	PVOID InputBuffer OPTIONAL,
	ULONG InputBufferLength,
	PVOID OutputBuffer OPTIONAL,
	ULONG OutputBufferLength
	)
{
	//�ȵ���ԭʼ����
	NTSTATUS stat = pTrueNtDeviceIoControl(
		FileHandle,
		Event OPTIONAL,
		ApcRoutine OPTIONAL,
		ApcContext OPTIONAL,
		IoStatusBlock,
		IoControlCode,
		InputBuffer OPTIONAL,
		InputBufferLength,
		OutputBuffer OPTIONAL,
		OutputBufferLength);

	//���ԭʼ����ʧ���ˣ�����RECV�����ݣ�
	if (!NT_SUCCESS(stat))
	{
		return stat ; 
	}

	PVOID Buffer = NULL; 
	ULONG Len = 0;
	switch(IoControlCode)
	{
	case IOCTL_AFD_SEND:
		if (g_sHookFilter.bWatchTCP && 
			(g_sHookFilter.eSendType == TYPE_SEND || g_sHookFilter.eSendType == TYPE_ALL))
		{
			PAFD_SEND_INFO AfdInfo = (PAFD_SEND_INFO)InputBuffer ; 
			Buffer = AfdInfo->BufferArray->buf ; 
			Len = AfdInfo->BufferArray->len;

			if (!IsFilterOk(Len,g_sHookFilter.wTCPMinLen,g_sHookFilter.eTCPCompare))
			{
				break;
			}
			OutputDbgInfo("[!] <TCP> SendPacket����(%d)\r\n", AfdInfo->BufferCount);
			PrintBuffer(true, Buffer, Len);
		}
		break;
	case IOCTL_AFD_RECV:
		if (g_sHookFilter.bWatchTCP 
			&&(g_sHookFilter.eSendType == TYPE_RECEIVE || g_sHookFilter.eSendType == TYPE_ALL))
		{
			PAFD_RECV_INFO AfdInfo = (PAFD_RECV_INFO)InputBuffer ; 
			Buffer = AfdInfo->BufferArray->buf ; 
			Len = AfdInfo->BufferArray->len;
			if (!IsFilterOk(Len,g_sHookFilter.wTCPMinLen,g_sHookFilter.eTCPCompare))
			{
				break;
			}
			OutputDbgInfo("[!] <TCP> SendPacket����(%d)\r\n", AfdInfo->BufferCount);
			PrintBuffer(true, Buffer, Len);
		}
		break;
	case IOCTL_AFD_RECV_DATAGRAM:
		if (g_sHookFilter.bWatchUDP 
			&&(g_sHookFilter.eSendType == TYPE_RECEIVE || g_sHookFilter.eSendType == TYPE_ALL))
		{
			PAFD_RECV_INFO_UDP UdpSendInfo = (PAFD_RECV_INFO_UDP)InputBuffer ; 
			Buffer = UdpSendInfo->BufferArray->buf ; 
			Len = UdpSendInfo->BufferArray->len;
			if (!IsFilterOk(UdpSendInfo->BufferCount,g_sHookFilter.wUDPMinLen,
				g_sHookFilter.eUDPCompare))
			{
				break;
			}
			OutputDbgInfo("[!] <UDP> RECVPacket����(%d)\r\n", UdpSendInfo->BufferCount);
			PrintBuffer(false, Buffer, Len);
		}
		break;
	case IOCTL_AFD_SEND_DATAGRAM:
		if (g_sHookFilter.bWatchUDP 
			&&(g_sHookFilter.eSendType == TYPE_SEND || g_sHookFilter.eSendType == TYPE_ALL))
		{
			PAFD_SEND_INFO_UDP UdpSendInfo = (PAFD_SEND_INFO_UDP)InputBuffer ; 
			
			//udpЭ�飬q���ܼ��������29��  �������Ϊ3��
			if (UdpSendInfo->BufferCount<3 || UdpSendInfo->BufferArray[2].len!=29)
			{
				//break;
			}
			Buffer = UdpSendInfo->BufferArray->buf ; 
			Len = UdpSendInfo->BufferArray->len;
			OutputDbgInfo("%d  %d" ,UdpSendInfo->BufferCount,g_sHookFilter.wUDPMinLen);
			if (!IsFilterOk(UdpSendInfo->BufferCount,g_sHookFilter.wUDPMinLen,
				g_sHookFilter.eUDPCompare))
			{
				break;
			}
			OutputDbgInfo("[!] <UDP> SendPacket����(%d)\r\n", UdpSendInfo->BufferCount);
			for (int i = 0; i<UdpSendInfo->BufferCount; i++)
			{
				OutputDbgInfo("[!] <UDP> ��%d�����ȣ�(%d)\r\n", i, UdpSendInfo->BufferArray[i].len);
			}

			PrintBuffer(false, Buffer, Len);
		}
		break;
	//default:
		//OutputDbgInfo("[!] IOCODE = %08X", IoControlCode);
	}
	return stat ; 
}

//unhook����
BOOL HookNtDeviceIoControl()
{
	if (isNtDeviceIoControlHooked)
	{
		OutputDebugStringA("[!] NtDeviceIoControl have been hooked!!!\r\n");
		return FALSE;
	}
	BOOL ret = SuperHookParseFileFunction(
		"mswsock.dll",
		"NtDeviceIoControlFile",
		NewNtDeviceIoControlFile,
		//����ָ��  ��ֵΪ��ȷ�ĺ�����ַ
		(PVOID *)&pTrueNtDeviceIoControl,
		&pTableIndexAddr
		);
	if (ret)
	{
		OutputDebugStringA("[!] NtDeviceIoControl hook success !!!\r\n");
		isNtDeviceIoControlHooked = TRUE;
	}else
	{
		OutputDebugStringA("[!] NtDeviceIoControl hook failed !!!\r\n");
	}
	return TRUE;
}

//unhook����
BOOL UnHookNtDeviceIoControl()
{
	if (!isNtDeviceIoControlHooked)
	{
		OutputDebugStringA("[!] NtDeviceIoControl have not been hooked!!!\r\n");
		return FALSE;
	}
	BOOL ret = SuperUnHookParseFileFunction(
		(PVOID )pTrueNtDeviceIoControl, pTableIndexAddr
		);
	if (ret)
	{
		OutputDebugStringA("[!] NtDeviceIoControl unhook success !!!\r\n");
		isNtDeviceIoControlHooked = FALSE;
	}else
	{
		OutputDebugStringA("[!] NtDeviceIoControl unhook failed !!!\r\n");
	}
	return TRUE;
}

HookSystemCall hookSystem;

/**************************************
	SystemCall  Hook
****************************************/
BOOL HookSystemCallEx(BOOL (__stdcall * myHookRet)(DWORD index))
{
	return hookSystem.Initialization(myHookRet);
}

BOOL UnHookSystemCallEx()
{
	return hookSystem.UnSystemHook();
}

void testtest()
{

}

map<DWORD, PINLINEHOOKINFO> g_mHookInfo;

/**************************************
	datadetail  Hook
****************************************/




//hook����
BOOL InLineHookCommon(DWORD InFunctionAddress, PVOID FunctionCallBack)
{
	if(g_mHookInfo.find(InFunctionAddress)!=g_mHookInfo.end())
	{
		OutputDbgInfo("[!] ���� %08X �Ѿ���HOOK�ˣ� \n",InFunctionAddress);
		return FALSE;
	}
	int CodeLength;
	PVOID FunctionRet;

	byte * HookZone = (byte *)malloc(48*sizeof(byte));

	RtlFillMemory(HookZone, 48*sizeof(byte), 0x90);//��nop���

	*(byte *)(HookZone+15)=0x60;//pushad
	*(byte *)(HookZone+16)=0x9c;//pushfd
	*(byte *)(HookZone+17)=0xe8;//call

	*(byte *)(HookZone+22)=0x9D;//popfd
	*(byte *)(HookZone+23)=0x61;//popad
	*(byte *)(HookZone+24)=0xe9;//jmp

	BOOL ret = HookFunctionInAddress(
		(PBYTE)InFunctionAddress,
		HookZone,
		&CodeLength,
		&FunctionRet);

	if (ret)
	{
		OutputDbgInfo("[!] [%08X]InLineHookCommon success\r\n", InFunctionAddress);

		*(DWORD *)(HookZone+18)=(DWORD)FunctionCallBack-((DWORD)(HookZone+17)+5);//call ��ַ
		*(DWORD *)(HookZone+25)=(DWORD)FunctionRet - ((DWORD)(HookZone+24)+5);//jmp ��ַ


		PINLINEHOOKINFO pInfo = (PINLINEHOOKINFO)malloc(sizeof(INLINEHOOKINFO));
		pInfo->FunctionAddress = InFunctionAddress;//��ַ
		pInfo->InLineCommonLength = CodeLength;//����
		pInfo->InLineCommonHookZone = HookZone;
		g_mHookInfo[InFunctionAddress] = pInfo;
	}else
	{
		OutputDebugStringA("[!] InLineHookCommon failed\r\n");
	}
	return ret;
}

//unhook����
BOOL UnInLineHookCommon(DWORD inFunctionAddress)
{
	if(g_mHookInfo.find(inFunctionAddress)==g_mHookInfo.end())
	{
		OutputDbgInfo("[!] ���� %08X ��û�б�HOOK�ˣ� \n",inFunctionAddress);
		return FALSE;
	}
	PINLINEHOOKINFO pInfo = g_mHookInfo[inFunctionAddress];

	BOOL ret = UnHookFunctionInAddress(
		(PBYTE)pInfo->FunctionAddress,
		 pInfo->InLineCommonHookZone ,
		 pInfo->InLineCommonLength);

	if (ret)
	{
		OutputDbgInfo("[!] [%08X]UnInLineHookCommon success\r\n", inFunctionAddress);
		if(pInfo->InLineCommonHookZone)
		{
			free(pInfo->InLineCommonHookZone);
			pInfo->InLineCommonHookZone = NULL;
		}
		if (pInfo)
		{
			free(pInfo);
			pInfo = NULL;
		}
		g_mHookInfo.erase(inFunctionAddress);
	}else
	{
		OutputDebugStringA("[!] UnInLineHookCommon failed\r\n");
	}
	return ret;
}


/**
eax   [ebp+0x28]
ecx   [ebp+0x24]
edx   [ebp+0x20]
ebx   [ebp+0x1c]
esp   [ebp+0x18]
ebp   [ebp+0x14]
esi   [ebp+0x10]
edi   [ebp+0xc]

ELF   [ebp+0x8]
*/
REGISTERFILTER registerFilter;
//�Լ��߼��ĺ���
LONG NTAPI InLineCommoLibrary()
{
	DWORD ebpAddress;
	_asm{
		mov ebpAddress, ebp
	}
	char info[MAX_PATH]="[!]";
	if (registerFilter.bWatchEAX)
	{
		char temp[50];
		sprintf_s(temp, 50, " EAX=%08X ", *(DWORD *)(ebpAddress + 0X28));
		strcat_s(info, MAX_PATH, temp);
	}
	if (registerFilter.bWatchECX)
	{
		char temp[50];
		sprintf_s(temp, 50, " ECX=%08X ", *(DWORD *)(ebpAddress + 0X24));
		strcat_s(info, MAX_PATH, temp);
	}
	if (registerFilter.bWatchEDX)
	{
		char temp[50];
		sprintf_s(temp, 50, " EDX=%08X ", *(DWORD *)(ebpAddress + 0X20));
		strcat_s(info, MAX_PATH, temp);
	}
	if (registerFilter.bWatchEBX)
	{
		char temp[50];
		sprintf_s(temp, 50, " EBX=%08X ", *(DWORD *)(ebpAddress + 0X1C));
		strcat_s(info, MAX_PATH, temp);
	}
	if (registerFilter.bWatchESI)
	{
		char temp[50];
		sprintf_s(temp, 50, " ESI=%08X ", *(DWORD *)(ebpAddress + 0X10));
		strcat_s(info, MAX_PATH, temp);
	}
	if (registerFilter.bWatchEDI)
	{
		char temp[50];
		sprintf_s(temp, 50, " EDI=%08X ", *(DWORD *)(ebpAddress + 0Xc));
		strcat_s(info, MAX_PATH, temp);
	}
	if (strlen(info) > 5)
	{
		OutputDbgInfo("%s", info);
	}
	if (registerFilter.bWatchPARAMS)
	{
		DWORD oldEbp = *(DWORD *)(ebpAddress + 0X14);
		OutputDbgInfo("[!]������Ϣ [%08X, %08X, %08X, %08X]\r\n",
			*(DWORD *)(oldEbp + 8),
			*(DWORD *)(oldEbp + 0xc),
			*(DWORD *)(oldEbp + 0x10),
			*(DWORD *)(oldEbp + 0x14)
			);
	}

	saveStatckDump(ebpAddress, TRUE);
	return 1;
}

DWORD HookWithSpecialCode(char *dllName,unsigned char data_chars[], 
	int length, CEdit *editInfo, DWORD scope)
{
	if (!scope)
	{
		scope = 0x20000;
	}
	HMODULE module = GetModuleHandleA(dllName);
	CHAR buffer[100];
	if (!module)
	{
		if(editInfo!=NULL){
			sprintf(buffer, "û���ҵ�ָ��ģ��[%s]", dllName);
			editInfo->SetWindowTextW(AnsiToUnicode(buffer));
		}
		return 0;
	}
	// �����������ȡ��ַ
	DWORD FunctionAddress = GetSpecial(data_chars, length, (DWORD)module, scope);

	OutputDbgInfo("FunctionAddress = %08X", FunctionAddress);
	if (!checkAddress(FunctionAddress))
	{
		if(editInfo!=NULL){
			sprintf(buffer, "�����������ҵ��ĵ�ַ[%08x]��Ч��", FunctionAddress);
			editInfo->SetWindowTextW(AnsiToUnicode(buffer));
		}
		return 0;
	}
	InLineHookCommon(FunctionAddress, InLineCommoLibrary);
	if(editInfo!=NULL){
		sprintf(buffer, "hook ����[%08X] success��", FunctionAddress);
		editInfo->SetWindowTextW(AnsiToUnicode(buffer));
	}
	return FunctionAddress;
}
DWORD HookWithSpecialCode(char *dllName,unsigned char data_chars[], 
	int length, CEdit *editInfo)
{
	return HookWithSpecialCode(dllName,data_chars, length, editInfo, 0x20000);
}

//�����������ַ���"8B 95 F0 FB FF FF 8B 02 50 8B 8D F0 FB FF FF" ������������hook
DWORD HookWithString(char *dllName, char *info, CEdit *editInfo, DWORD scope)
{
	if (info==NULL||editInfo==NULL)
	{
		return 0;
	}
	int length = strlen(info);
	if (length > 80)
	{
		editInfo->SetWindowTextW(_T("������Ҳ�����̫���˰ɡ���"));
		return 0;
	}
	if (length < 10)
	{
		editInfo->SetWindowTextW(_T("������Ҳ�����̫���˰ɡ���"));
		return 0;
	}


	char buffer[80] = {0};
	int index = 0;
	while (*info!='\0')
	{
		if (*info!=' ')
		{
			buffer[index]=*info;
			index++;
		}
		info++;
	}

	length = strlen(buffer);
	if (length%2 == 1)
	{
		editInfo->SetWindowTextW(_T("һ��Ҫ��ż������ĸ���С���"));
		return 0;
	}

	unsigned char com_chars[80]  = {0};

	for (int i=0;i<length/2;i++)
	{
		char hexStr[3] = {0};
		hexStr[0] = buffer[2*i];
		hexStr[1] = buffer[2*i+1];;
		com_chars[i] = ConverToHex(hexStr,16);
	}
	OutputDbgInfo("[!] HOOK��Ϣ����%08X|%s�� (%d:%s)", 
		scope, dllName, length/2, buffer);
	//�������������hook��
	HookWithSpecialCode(
		dllName, 
		com_chars, 
		length/2, 
		editInfo, 
		scope);
}