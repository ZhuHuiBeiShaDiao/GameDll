
#ifndef  _HWND_UTIL
#define  _HWND_UTIL

class CHwndUtil
{
public:
	CHwndUtil();
	~CHwndUtil();

	bool CHwndUtil::GetHwndByPid(DWORD ProcessId, HWND* hWnd, int* iLength);
	HWND CHwndUtil::GetHwndInjectByIni();//ע���þ����ʽ
	HWND CHwndUtil::GetHwndUnInjectByIni();//δע���þ����ʽ
};
#endif
