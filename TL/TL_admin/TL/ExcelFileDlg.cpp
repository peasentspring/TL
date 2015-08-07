// ScriptFileDlg.cpp

/*
** $Id: ScriptFileDlg.cpp 4 2013-10-16 08:25:28Z STONERIDGE\mima $
*/

#include "stdafx.h"
#include "ExcelFileDlg.h"
#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////
// CExcelFileDlg
//
IMPLEMENT_DYNAMIC(CExcelFileDlg, CFileDialog)

CExcelFileDlg::CExcelFileDlg() : CFileDialog(TRUE,_T(".xls"),NULL,OFN_HIDEREADONLY,
 _T("Excel File (*.xls)|*.xls|All Files (*.*)|*.*||"))
{
    m_ofn.lpstrTitle = _T("Select Excel File");
}

BOOL CExcelFileDlg::OnInitDialog()
{
	if (m_ofn.Flags & OFN_EXPLORER)
		GetParent()->SetDlgItemText(IDOK,_T("Select"));
	
	else
		SetDlgItemText(IDOK,_T("Select"));

	return CFileDialog::OnInitDialog();
}

 
