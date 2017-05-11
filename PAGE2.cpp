// PAGE2.cpp : implementation file
//

#include "stdafx.h"
#include "DLL.h"
#include "PAGE2.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPAGE2 dialog


CPAGE2::CPAGE2(CWnd* pParent /*=NULL*/)
	: CDialog(CPAGE2::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPAGE2)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CPAGE2::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPAGE2)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_LIST_NET_REPORT, m_DataListCtrl);
}


BEGIN_MESSAGE_MAP(CPAGE2, CDialog)
	//{{AFX_MSG_MAP(CPAGE2)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_ADD, &CPAGE2::OnBnClickedButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR, &CPAGE2::OnBnClickedButtonClear)
	ON_BN_CLICKED(IDC_BUTTON_TEST_MAL, &CPAGE2::OnBnClickedButtonTestMal)
	ON_BN_CLICKED(IDC_BUTTON_MALLOC, &CPAGE2::OnBnClickedButtonMalloc)
	ON_BN_CLICKED(IDC_BUTTON_UNMALLOC, &CPAGE2::OnBnClickedButtonUnmalloc)
END_MESSAGE_MAP()

BOOL CPAGE2::OnInitDialog()
{
	CDialog::OnInitDialog();

	//�б���ʼ��
	DWORD dwStyle = (DWORD)m_DataListCtrl.SendMessage(LVM_GETEXTENDEDLISTVIEWSTYLE,0,0);
	dwStyle |= LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_HEADERDRAGDROP;
	m_DataListCtrl.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE,0,(LPARAM)dwStyle);

	m_DataListCtrl.InsertColumn(0, _T("���"), LVCFMT_LEFT, 40);
	m_DataListCtrl.InsertColumn(1, _T("ƫ��"), LVCFMT_LEFT, 80);

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}
set<DWORD> objOffset;//Ҫ��ע���麯��ƫ��

BOOL isAll = FALSE;

void CPAGE2::OnBnClickedButtonAdd()
{
	// IDC_EDIT_CODE
	try
	{
		CString strBuf;
		GetDlgItemText(IDC_EDIT_FILTER,strBuf);
		DWORD dwObject = stol(strBuf.GetBuffer(0),FALSE,16);  
		if (dwObject>=0)
		{
			objOffset.insert(dwObject);

			int size = m_DataListCtrl.GetItemCount();
			strBuf.Format(L"%02d",size+1);
			m_DataListCtrl.InsertItem(size,strBuf);//������
			strBuf.Format(L"%04x",dwObject);
			m_DataListCtrl.SetItemText(size,1,strBuf);//���ø��еĲ�ͬ�е���ʾ�ַ�
		}
	}catch (...){
		OutputDbgInfo("[!] ����ָ����Χִ���쳣:%d�\\r\n",GetLastError());
	}
}


void CPAGE2::OnBnClickedButtonClear()
{
	objOffset.clear();
	m_DataListCtrl.DeleteAllItems();
}


void CPAGE2::OnBnClickedButtonTestMal()
{
	OutputDbgInfo("[!] ������ַ%08x",&CPAGE2::OnBnClickedButtonTestMal);

	malloc(sizeof(char));
	malloc(sizeof(short));
	malloc(sizeof(int));
	malloc(sizeof(double));
	malloc(sizeof(DWORD));
	malloc(16);
	malloc(1601);
	malloc(16002);
	malloc(16000);
}


void CPAGE2::OnBnClickedButtonMalloc()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}


void CPAGE2::OnBnClickedButtonUnmalloc()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}
