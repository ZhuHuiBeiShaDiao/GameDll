#pragma once
class CLuaAPI
{
public:
	CLuaAPI(void);
	~CLuaAPI(void);
	/**************************************
	LeftClick 1
	RightClick 1
	LeftDoubleClick 1
	LeftDown 1
	LeftUp 1
	RightDown 1
	RightUp 1
	****************************************/
	VOID MoveTo(DWORD clientX,DWORD clientY);

	/**************************************
	*  ���ܣ�������
	****************************************/
	VOID LeftClick(DWORD clientX,DWORD clientY);

	/**************************************
	*  ���ܣ��Ҽ����
	****************************************/
	VOID RightClick(DWORD clientX,DWORD clientY);

	/**************************************
	*  ���ܣ�������
	****************************************/
	VOID LeftDown(DWORD clientX,DWORD clientY);

	/**************************************
	*  ���ܣ��������
	****************************************/
	VOID LeftUp(DWORD clientX,DWORD clientY);

	/**************************************
	*  ���ܣ��Ҽ�����
	****************************************/
	VOID RightDown(DWORD clientX,DWORD clientY);

	/**************************************
	*  ���ܣ��Ҽ�����
	****************************************/
	VOID RightUp(DWORD clientX,DWORD clientY);
};

