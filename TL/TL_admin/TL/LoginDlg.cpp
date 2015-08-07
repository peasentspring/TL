// LoginDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TL.h"
#include "LoginDlg.h"
#include "afxdialogex.h"
#include "Cprofile.h"


// CLoginDlg 对话框

IMPLEMENT_DYNAMIC(CLoginDlg, CDialogEx)

CLoginDlg::CLoginDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CLoginDlg::IDD, pParent)
	, m_username(_T(""))
	, m_userpwd(_T(""))
{

}

CLoginDlg::~CLoginDlg()
{
}

void CLoginDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_INPUT_USER, m_username);
	DDX_Text(pDX, IDC_INPUT_PWD, m_userpwd);
}


BEGIN_MESSAGE_MAP(CLoginDlg, CDialogEx)
	ON_BN_CLICKED(IDC_LOGIN, &CLoginDlg::OnBnClickedLogin)
END_MESSAGE_MAP()


// CLoginDlg 消息处理程序


void CLoginDlg::OnBnClickedLogin()
{
	// TODO:  在此添加控件通知处理程序代码
	UpdateData();
	TCHAR szPath[MAX_PATH];
	GetModuleFileName(NULL, szPath, MAX_PATH);
	CString PathName(szPath);
	CString PROGRAM_PATH = PathName.Left(PathName.ReverseFind(_T('\\')) + 1) + "TL.dat";
	C_PrivProfile inifile(PROGRAM_PATH.GetBuffer());
	char ascbuf[1024];
	//MessageBox(_T(m_username));
	inifile.Get("USER", "admin", ascbuf, "");
	//MessageBox(_T(CString(ascbuf)));
	if (base64encode(m_userpwd) == CString(ascbuf))
	{
		OnOK();
	}
	//if (m_username == "admin" && m_userpwd == "123456")
	//{
	//	OnOK();
	//}
	else
	{
		MessageBox(_T("用户名或者密码错误"));
	}
}

CString CLoginDlg::base64encode(CString src)
{
	unsigned char * base64 = (unsigned char *)"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";
	int n, buflen, i, j;
	static unsigned char *dst;
	CString buf = src;
	buflen = n = buf.GetLength();
	dst = (unsigned char*)malloc(buflen / 3 * 4 + 3);
	memset(dst, 0, buflen / 3 * 4 + 3);
	for (i = 0, j = 0; i <= buflen - 3; i += 3, j += 4) {
		dst[j] = (buf[i] & 0xFC) >> 2;
		dst[j + 1] = ((buf[i] & 0x03) << 4) + ((buf[i + 1] & 0xF0) >> 4);
		dst[j + 2] = ((buf[i + 1] & 0x0F) << 2) + ((buf[i + 2] & 0xC0) >> 6);
		dst[j + 3] = buf[i + 2] & 0x3F;
	}
	if (n % 3 == 1) {
		dst[j] = (buf[i] & 0xFC) >> 2;
		dst[j + 1] = ((buf[i] & 0x03) << 4);
		dst[j + 2] = 64;
		dst[j + 3] = 64;
		j += 4;
	}
	else if (n % 3 == 2) {
		dst[j] = (buf[i] & 0xFC) >> 2;
		dst[j + 1] = ((buf[i] & 0x03) << 4) + ((buf[i + 1] & 0xF0) >> 4);
		dst[j + 2] = ((buf[i + 1] & 0x0F) << 2);
		dst[j + 3] = 64;
		j += 4;
	}
	for (i = 0; i<j; i++) /* map 6 bit value to base64 ASCII character */
		dst[i] = base64[(int)dst[i]];
	dst[j] = 0;
	return CString(dst);
}