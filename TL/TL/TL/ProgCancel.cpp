// ProgCancel.cpp : implementation file

/*
** $Id: ProgCancel.cpp 4 2013-10-16 08:25:28Z STONERIDGE\mima $
*/

#include "stdafx.h"
#include "ProgCancel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CProgCancel dialog

CProgCancel::CProgCancel(CWnd* pParent /*=NULL*/)
	: CDialog(CProgCancel::IDD, pParent)
{
	//{{AFX_DATA_INIT(CProgCancel)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	ASSERT(pParent != NULL);
	m_pParent = pParent;
	m_nID = CProgCancel::IDD;
}

void CProgCancel::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProgCancel)
	DDX_Control(pDX, IDC_PROG_PROGRESS, m_progress);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CProgCancel, CDialog)
	//{{AFX_MSG_MAP(CProgCancel)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDCANCEL, &CProgCancel::OnBnClickedCancel)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_PROG_PROGRESS, &CProgCancel::OnNMCustomdrawProgProgress)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProgCancel message handlers

BOOL CProgCancel::Create(void) 
{
	return CDialog::Create(m_nID,m_pParent);
}

BOOL CProgCancel::PreCreateWindow(CREATESTRUCT& cs) 
{
	m_progress.SetRange(0,100);
	m_progress.SetPos(0);
	
	return CDialog::PreCreateWindow(cs);
}

void CProgCancel::PostNcDestroy() 
{
	delete this;
}

void CProgCancel::OnCancel() 
{
//	((CMainForm*)m_pParent)->ProgCancel();
	DestroyWindow();
}


void CProgCancel::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	//((CMainForm*)m_pParent)->m_data_length_button.EnableWindow(TRUE);
	//((CMainForm*)m_pParent)->m_radio_32_byte_data_length.EnableWindow(((CMainForm*)m_pParent)->m_data_length_control);
	//((CMainForm*)m_pParent)->m_radio_128_byte_data_length.EnableWindow(((CMainForm*)m_pParent)->m_data_length_control);
	//((CMainForm*)m_pParent)->m_radio_256_byte_data_length.EnableWindow(((CMainForm*)m_pParent)->m_data_length_control);
	//((CMainForm*)m_pParent)->m_radio_2048_byte_data_length.EnableWindow(((CMainForm*)m_pParent)->m_data_length_control);
	OnCancel();
}

void CProgCancel::OnNMCustomdrawProgProgress(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}
