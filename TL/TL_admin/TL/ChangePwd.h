#pragma once


// CChangePwd 对话框

class CChangePwd : public CDialogEx
{
	DECLARE_DYNAMIC(CChangePwd)

public:
	CChangePwd(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CChangePwd();

// 对话框数据
	enum { IDD = IDD_USER_CHANGE };
	CString	m_old;
	CString	m_new;
	CString	m_new_sure;
	BOOL isshow;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
//	afx_msg void OnDestroy();
	afx_msg void OnClose();
};
