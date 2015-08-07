// ProgCancel.h

/*
** $Id: ProgCancel.h 4 2013-10-16 08:25:28Z STONERIDGE\mima $
*/

#if !defined(AFX_PROGCANCEL_H__2F05D0DA_5AA4_4469_A307_141D18AD167D__INCLUDED_)
#define AFX_PROGCANCEL_H__2F05D0DA_5AA4_4469_A307_141D18AD167D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ProgCancel.h : header file
//
#include "resource.h"
#include "afxwin.h"
/////////////////////////////////////////////////////////////////////////////
// CProgCancel dialog

class CProgCancel : public CDialog
{
// Construction
public:
	CProgCancel(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CProgCancel)
	enum { IDD = IDD_PROGCANCEL };
	CProgressCtrl	m_progress;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProgCancel)
	public:
	virtual BOOL Create(void);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
protected:
	CWnd* m_pParent;
	int m_nID;

	// Generated message map functions
	//{{AFX_MSG(CProgCancel)
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCancel();
public:
	afx_msg void OnNMCustomdrawProgProgress(NMHDR *pNMHDR, LRESULT *pResult);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROGCANCEL_H__2F05D0DA_5AA4_4469_A307_141D18AD167D__INCLUDED_)
