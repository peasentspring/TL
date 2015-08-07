// ChangePwd.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "TL.h"
#include "ChangePwd.h"
#include "LoginDlg.h"
#include "Cprofile.h"
#include "afxdialogex.h"


// CChangePwd �Ի���

IMPLEMENT_DYNAMIC(CChangePwd, CDialogEx)

CChangePwd::CChangePwd(CWnd* pParent /*=NULL*/)
	: CDialogEx(CChangePwd::IDD, pParent)
{
	m_old = _T("");
	m_new = _T("");
	m_new_sure = _T("");
	isshow = FALSE;
}

CChangePwd::~CChangePwd()
{
}

void CChangePwd::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_OLDPWD, m_old);
	DDX_Text(pDX, IDC_EDIT_NEWPWD, m_new);
	DDX_Text(pDX, IDC_EDIT_NEWPWDAG, m_new_sure);
}


BEGIN_MESSAGE_MAP(CChangePwd, CDialogEx)
	ON_BN_CLICKED(IDOK, &CChangePwd::OnBnClickedOk)
//	ON_WM_DESTROY()
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CChangePwd ��Ϣ�������

void CChangePwd::OnBnClickedOk()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CDialogEx::OnOK();
	UpdateData(TRUE);
	TCHAR szPath[MAX_PATH];
	GetModuleFileName(NULL, szPath, MAX_PATH);
	CString PathName(szPath);
	CString PROGRAM_PATH = PathName.Left(PathName.ReverseFind(_T('\\')) + 1) + "TL.dat";
	C_PrivProfile inifile(PROGRAM_PATH.GetBuffer());
	char ascbuf[1024];
	inifile.Get("USER", "admin", ascbuf, "");
	if (CLoginDlg::base64encode(m_old) != CString(ascbuf))
	{
		MessageBox("�����벻��ȷ������������!");
		m_old = "";
		UpdateData(FALSE);
		isshow = TRUE;
	}
	else if (m_new != "" && strcmp(m_new, m_new_sure) == 0)
	{
		strcpy_s(ascbuf, CLoginDlg::base64encode(m_new));
		inifile.Add("USER", "admin", ascbuf);
		MessageBox("�����޸ĳɹ�");
		isshow = FALSE;
		OnOK();
	}
	else
	{
		MessageBox("�����������벻��ȣ����������룡");
		m_new = "";
		m_new_sure = "";
		UpdateData(FALSE);
		isshow = TRUE;
	}
}


//void CChangePwd::OnDestroy()
//{
//	CDialogEx::OnDestroy();
//	isshow = FALSE;
//	// TODO:  �ڴ˴������Ϣ����������
//}


void CChangePwd::OnClose()
{
	// TODO:  �ڴ������Ϣ�����������/�����Ĭ��ֵ

	CDialogEx::OnClose();
	isshow = FALSE;
}
