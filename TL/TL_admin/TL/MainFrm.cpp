
// MainFrm.cpp : CMainFrame ���ʵ��
//

#include "stdafx.h"
#include "TL.h"

#include "MainFrm.h"
#include "ChangePwd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_COMMAND(ID_USER_CHANGE, &CMainFrame::OnUserChange)
	ON_UPDATE_COMMAND_UI(ID_USER_CHANGE, &CMainFrame::OnUpdateUserChange)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // ״̬��ָʾ��
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame ����/����

CMainFrame::CMainFrame()
{
	// TODO:  �ڴ���ӳ�Ա��ʼ������
	m_bItemUserChange = TRUE;

}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("δ�ܴ���״̬��\n");
		return -1;      // δ�ܴ���
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO:  �ڴ˴�ͨ���޸�
	//  CREATESTRUCT cs ���޸Ĵ��������ʽ

	cs.style = WS_OVERLAPPED | WS_CAPTION | FWS_ADDTOTITLE
		 | WS_THICKFRAME | WS_MINIMIZEBOX | WS_SYSMENU;
	cs.style &= ~WS_THICKFRAME;

	return TRUE;
}

// CMainFrame ���

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}
#endif //_DEBUG


// CMainFrame ��Ϣ�������
void CMainFrame::OnUserChange()
{
	// TODO: Add your command handler code here

	// 	if(!m_pUserChange->Create(IDD_USERCHANGE_FORM,this))return;
	// 	m_pUserChange->ShowWindow( SW_SHOW );
	// 	m_pUserChange->SetDlgItemInt(IDD_USERCHANGE_FORM,TRUE);
	m_bItemUserChange = FALSE;
	CChangePwd dlg;
showdlg:
	dlg.DoModal();
	if (dlg.isshow)
	{
		goto showdlg;
	}
	m_bItemUserChange = TRUE;

}

void CMainFrame::OnUpdateUserChange(CCmdUI* pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(m_bItemUserChange);
}

