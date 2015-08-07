// ExcelFileDlg.h
//
/////////////////////////////////////////////////////////////////////////////



#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

class CExcelFileDlg : public CFileDialog {
	public:
		DECLARE_DYNAMIC(CExcelFileDlg) CExcelFileDlg();
		virtual BOOL OnInitDialog();
};
 