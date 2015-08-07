
// TLView.cpp : CTLView 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "TL.h"
#endif

#include "TLDoc.h"
#include "TLView.h"
#include "Cprofile.h"
#include "ExcelFileDlg.h"
#include "excel.h"
using namespace excel9;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define BUF_SIZE_KWP        4098
char ascbuf[BUF_SIZE_KWP];

// CTLView

IMPLEMENT_DYNCREATE(CTLView, CFormView)

BEGIN_MESSAGE_MAP(CTLView, CFormView)
//	ON_BN_CLICKED(IDC_APPLICATION_BROWSE, &CTLView::OnBnClickedApplicationBrowse)
//	ON_BN_CLICKED(IDC_APPLICATION_IMPORT, &CTLView::OnBnClickedApplicationImport)
//	ON_CBN_SELCHANGE(IDC_COMBO_PRODUCT_TYPE, &CTLView::OnSelchangeComboProductType)
	ON_WM_CREATE()
	ON_BN_CLICKED(IDOK, &CTLView::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CTLView::OnBnClickedCancel)
END_MESSAGE_MAP()

// CTLView 构造/析构

CTLView::CTLView()
	: CFormView(CTLView::IDD)
	, m_baud_rate(0)
	, m_can_chan(-1)
	, m_can_hardware(_T(""))
	, m_can_recid(_T(""))
	, m_can_traid(_T(""))
	, m_check1(FALSE)
	, m_check2(FALSE)
	, m_check3(FALSE)
	, m_check4(FALSE)
	, m_check5(FALSE)
	, m_check6(FALSE)
	, m_check7(FALSE)
	, m_check8(FALSE)
	, m_check9(FALSE)
	, m_check10(FALSE)
	, m_order1(_T(""))
	, m_order2(_T(""))
	, m_order3(_T(""))
	, m_order4(_T(""))
	, m_order5(_T(""))
	, m_order6(_T(""))
	, m_order7(_T(""))
	, m_order8(_T(""))
	, m_order9(_T(""))
	, m_order10(_T(""))
	, m_access_password5(_T(""))
	, m_access_password6(_T(""))
	, m_access_password7(_T(""))
	, m_access_password8(_T(""))
	, m_product_id(_T(""))
	, m_product_type(_T(""))
	, m_checkid(_T(""))
	, m_reboot(FALSE)
{
	// TODO:  在此处添加构造代码
	TCHAR szPath[MAX_PATH];
	GetModuleFileName(NULL, szPath, MAX_PATH);
	CString PathName(szPath);
	CString PROGRAM_PATH = PathName.Left(PathName.ReverseFind(_T('\\')) + 1) + "TL.ini";
	memset(iniFilePath, '\0', sizeof(iniFilePath));
	memcpy(iniFilePath, PROGRAM_PATH.GetBuffer(), PROGRAM_PATH.GetLength());
}

CTLView::~CTLView()
{
}

void CTLView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//  DDX_Control(pDX, IDC_COMBO_PRODUCT_TYPE, m_combotype);
	//  DDX_Control(pDX, IDC_COMBO_PRODUCT_TYPE, m_combotype);
	DDX_Radio(pDX, IDC_BAUD_250K, m_baud_rate);
	DDX_CBIndex(pDX, IDC_SETUP_CAN_CHAN, m_can_chan);
	DDX_Control(pDX, IDC_SETUP_CAN_HARDWARE, m_can_hardware_ctrl);
	DDX_CBString(pDX, IDC_SETUP_CAN_HARDWARE, m_can_hardware);
	DDX_Text(pDX, IDC_SETUP_CAN_RECID, m_can_recid);
	DDX_Text(pDX, IDC_SETUP_CAN_TRAID, m_can_traid);
	DDX_Check(pDX, IDC_ORDER_CHECK_1, m_check1);
	DDX_Check(pDX, IDC_ORDER_CHECK_2, m_check2);
	DDX_Check(pDX, IDC_ORDER_CHECK_3, m_check3);
	DDX_Check(pDX, IDC_ORDER_CHECK_4, m_check4);
	DDX_Check(pDX, IDC_ORDER_CHECK_5, m_check5);
	DDX_Check(pDX, IDC_ORDER_CHECK_6, m_check6);
	DDX_Check(pDX, IDC_ORDER_CHECK_7, m_check7);
	DDX_Check(pDX, IDC_ORDER_CHECK_8, m_check8);
	DDX_Check(pDX, IDC_ORDER_CHECK_9, m_check9);
	DDX_Check(pDX, IDC_ORDER_CHECK_10, m_check10);
	DDX_Text(pDX, IDC_ORDER_1, m_order1);
	DDX_Text(pDX, IDC_ORDER_2, m_order2);
	DDX_Text(pDX, IDC_ORDER_3, m_order3);
	DDX_Text(pDX, IDC_ORDER_4, m_order4);
	DDX_Text(pDX, IDC_ORDER_5, m_order5);
	DDX_Text(pDX, IDC_ORDER_6, m_order6);
	DDX_Text(pDX, IDC_ORDER_7, m_order7);
	DDX_Text(pDX, IDC_ORDER_8, m_order8);
	DDX_Text(pDX, IDC_ORDER_9, m_order9);
	DDX_Text(pDX, IDC_ORDER_10, m_order10);
	DDX_Control(pDX, IDC_SETUP_ACCESS_PASSWORD5, m_access_password5_ctrl);
	DDX_Text(pDX, IDC_SETUP_ACCESS_PASSWORD5, m_access_password5);
	DDX_Control(pDX, IDC_SETUP_ACCESS_PASSWORD6, m_access_password6_ctrl);
	DDX_Text(pDX, IDC_SETUP_ACCESS_PASSWORD6, m_access_password6);
	DDX_Control(pDX, IDC_SETUP_ACCESS_PASSWORD7, m_access_password7_ctrl);
	DDX_Text(pDX, IDC_SETUP_ACCESS_PASSWORD7, m_access_password7);
	DDX_Control(pDX, IDC_SETUP_ACCESS_PASSWORD8, m_access_password8_ctrl);
	DDX_Text(pDX, IDC_SETUP_ACCESS_PASSWORD8, m_access_password8);
	DDX_Control(pDX, IDC_PRODUCT_INPUT, m_product_id_ctrl);
	DDX_Text(pDX, IDC_PRODUCT_INPUT, m_product_id);
	DDX_Control(pDX, IDC_PRODUCT_TYPE_INPUT, m_product_type_ctrl);
	DDX_Text(pDX, IDC_PRODUCT_TYPE_INPUT, m_product_type);
	DDX_Text(pDX, IDC_EDIT_CHECKID, m_checkid);
	DDX_Check(pDX, IDC_CHECK_REBOOT, m_reboot);
}

BOOL CTLView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO:  在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CFormView::PreCreateWindow(cs);
}

void CTLView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();
	//MessageBoxA("test");
	//设置初始值
	C_PrivProfile inifile(iniFilePath);
	//m_selhw = inifile.Get("PRODUCT", "IDSEL", 0);

	inifile.Get("PRODUCT", "ID", ascbuf, "0000000");
	m_product_id = ascbuf;

	inifile.Get("PRODUCT", "TYPE", ascbuf, "1111111");
	m_product_type = ascbuf;
	int curSel;
	curSel = inifile.Get("CAN", "HARDWARE", 0);
	m_can_hardware_ctrl.SetCurSel(curSel);
	m_can_hardware = "Kvaser";
	m_can_chan = inifile.Get("CAN", "CHANNEL", 0);
	m_baud_rate = inifile.Get("CAN", "BAUDRATESEL", 0);
	inifile.Get("CAN", "TRAID", ascbuf, "18DA1700");
	m_can_traid = ascbuf;
	inifile.Get("CAN", "RECID", ascbuf, "18DA0017");
	m_can_recid = ascbuf;
	inifile.Get("CAN", "CHECKID", ascbuf, "0CFE6CEE");
	m_checkid = ascbuf;

	inifile.Get("ACCESS", "KEY_SUPPLIER_1_SAW", ascbuf, "");
	m_access_password5 = ascbuf;
	inifile.Get("ACCESS", "KEY_SUPPLIER_2_SAW", ascbuf, "");
	m_access_password6 = ascbuf;
	inifile.Get("ACCESS", "KEY_SUPPLIER_3_SAW", ascbuf, "");
	m_access_password7 = ascbuf;
	inifile.Get("ACCESS", "KEY_SUPPLIER_4_SAW", ascbuf, "");
	m_access_password8 = ascbuf;

	m_reboot = inifile.Get("OTHER", "REBOOT", 0);
	//int OrderNum = inifile.Get("ORDER", "ORDER_NUM", 0);
	//for (int i = 0; i < OrderNum; i++)
	//{
	//	inifile.Get("ORDER", "ORDER_NAME" + i, ascbuf);
	//}
	BOOL *checklist[10] = { &m_check1, &m_check2, &m_check3, &m_check4, &m_check5, &m_check6, &m_check7, &m_check8, &m_check9, &m_check10 };
	CString *orderlist[10] = { &m_order1, &m_order2, &m_order3, &m_order4, &m_order5, &m_order6, &m_order7, &m_order8, &m_order9, &m_order10 };

	for (int i = 1; i <= 10; i++)
	{
		char tmp[20] = {};
		sprintf_s(tmp, "ORDER_NAME%d", i);
		inifile.Get("ORDER", tmp, ascbuf, "");
		*orderlist[i - 1] = ascbuf;
		if (*orderlist[i - 1] != "")
		{
			*checklist[i - 1] = TRUE;
		}
	}

	//inifile.Get("ORDER", "ORDER_NAME1", ascbuf, "");
	//m_order1 = ascbuf;
	//inifile.Get("ORDER", "ORDER_NAME2", ascbuf, "");
	//m_order2 = ascbuf;
	//inifile.Get("ORDER", "ORDER_NAME3", ascbuf, "");
	//m_order3 = ascbuf;
	//inifile.Get("ORDER", "ORDER_NAME4", ascbuf, "");
	//m_order4 = ascbuf;
	//inifile.Get("ORDER", "ORDER_NAME5", ascbuf, "");
	//m_order5 = ascbuf;
	//inifile.Get("ORDER", "ORDER_NAME6", ascbuf, "");
	//m_order6 = ascbuf;
	//inifile.Get("ORDER", "ORDER_NAME7", ascbuf, "");
	//m_order7 = ascbuf;
	//inifile.Get("ORDER", "ORDER_NAME8", ascbuf, "");
	//m_order8 = ascbuf;
	//inifile.Get("ORDER", "ORDER_NAME9", ascbuf, "");
	//m_order9 = ascbuf;
	//inifile.Get("ORDER", "ORDER_NAME10", ascbuf, "");
	//m_order10 = ascbuf;

	UpdateData(FALSE);
}


// CTLView 诊断

#ifdef _DEBUG
void CTLView::AssertValid() const
{
	CFormView::AssertValid();
}

void CTLView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CTLDoc* CTLView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CTLDoc)));
	return (CTLDoc*)m_pDocument;
}
#endif //_DEBUG


// CTLView 消息处理程序

int CTLView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFormView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	//MessageBoxA("testcreate");
	return 0;
}


void CTLView::OnBnClickedOk()
{
	// TODO:  在此添加控件通知处理程序代码

	UpdateData(TRUE);
	C_PrivProfile inifile(iniFilePath);
	m_selhw = m_can_hardware_ctrl.GetCurSel();
	strcpy_s(ascbuf, m_product_id);
	inifile.Add("PRODUCT", "ID", ascbuf);
	strcpy_s(ascbuf, m_product_type);
	inifile.Add("PRODUCT", "TYPE", ascbuf);
	strcpy_s(ascbuf, m_product_type);
	inifile.Add("CAN", "HARDWARE", m_selhw);
	inifile.Add("CAN", "CHANNEL", m_can_chan);
	strcpy_s(ascbuf, m_can_traid);
	inifile.Add("CAN", "TRAID", ascbuf);
	strcpy_s(ascbuf, m_can_recid);
	inifile.Add("CAN", "RECID", ascbuf);
	strcpy_s(ascbuf, m_checkid);
	inifile.Add("CAN", "CHECKID", ascbuf);
	inifile.Add("CAN", "BAUDRATE", m_baud_rate);
	BOOL *checklist[10] = { &m_check1, &m_check2, &m_check3, &m_check4, &m_check5, &m_check6, &m_check7, &m_check8, &m_check9, &m_check10 };
	CString *orderlist[10] = { &m_order1, &m_order2, &m_order3, &m_order4, &m_order5, &m_order6, &m_order7, &m_order8, &m_order9, &m_order10 };
	for (int i = 1; i <= 10; i++)
	{
		char tmp[20] = {};
		sprintf_s(tmp, "ORDER_NAME%d", i);
		if (*checklist[i-1])
		{
			strcpy_s(ascbuf, *orderlist[i-1]);
			inifile.Add("ORDER", tmp, ascbuf);
			//OrderNum++;
		}
		else
		{
			inifile.Add("ORDER", tmp, "");
		}
	}

	strcpy_s(ascbuf, m_access_password5);
	inifile.Add("ACCESS", "KEY_SUPPLIER_1_SAW", ascbuf);
	strcpy_s(ascbuf, m_access_password6);
	inifile.Add("ACCESS", "KEY_SUPPLIER_2_SAW", ascbuf);
	strcpy_s(ascbuf, m_access_password7);
	inifile.Add("ACCESS", "KEY_SUPPLIER_3_SAW", ascbuf);
	strcpy_s(ascbuf, m_access_password8);
	inifile.Add("ACCESS", "KEY_SUPPLIER_4_SAW", ascbuf);

	inifile.Add("OTHER", "REBOOT", m_reboot?1:0);

	//inifile.Add("TIMING", "N_AS", m_timing_n_as);
	//inifile.Add("TIMING", "N_BS", m_timing_n_bs);
	//inifile.Add("TIMING", "N_CR", m_timing_n_cr);
	//inifile.Add("TIMING", "N_STMIN", m_timing_n_stmin);
	//inifile.Add("TIMING", "BS", m_timing_bs);
	//inifile.Add("TIMING", "T_NORESP", m_timing_t_noresp);
	//inifile.Add("TIMING", "T_TP", m_timing_t_tp);
	//inifile.Add("TIMING", "T_P2", m_timing_t_p2);
	//inifile.Add("TIMING", "T_P2LONG", m_timing_t_p2long);

	//UpdateData(FALSE);
	MessageBoxA("配置保存成功");
}


void CTLView::OnBnClickedCancel()
{
	// TODO:  在此添加控件通知处理程序代码
	C_PrivProfile inifile(iniFilePath);

	inifile.Get("PRODUCT", "ID", ascbuf, "0000000");
	m_product_id = ascbuf;

	inifile.Get("PRODUCT", "TYPE", ascbuf, "1111111");
	m_product_type = ascbuf;
	int curSel;
	curSel = inifile.Get("CAN", "HARDWARE", 0);
	m_can_hardware_ctrl.SetCurSel(curSel);
	m_can_hardware = "Kvaser";
	m_can_chan = inifile.Get("CAN", "CHANNEL", 0);
	m_baud_rate = inifile.Get("CAN", "BAUDRATESEL", 0);
	inifile.Get("CAN", "TRAID", ascbuf, "18DA1700");
	m_can_traid = ascbuf;
	inifile.Get("CAN", "RECID", ascbuf, "18DA0017");
	m_can_recid = ascbuf;
	inifile.Get("CAN", "CHECKID", ascbuf, "0CFE6CEE");
	m_checkid = ascbuf;

	inifile.Get("ACCESS", "KEY_SUPPLIER_1_SAW", ascbuf, "");
	m_access_password5 = ascbuf;
	inifile.Get("ACCESS", "KEY_SUPPLIER_2_SAW", ascbuf, "");
	m_access_password6 = ascbuf;
	inifile.Get("ACCESS", "KEY_SUPPLIER_3_SAW", ascbuf, "");
	m_access_password7 = ascbuf;
	inifile.Get("ACCESS", "KEY_SUPPLIER_4_SAW", ascbuf, "");
	m_access_password8 = ascbuf;

	m_reboot = inifile.Get("OTHER", "REBOOT", 0);
	//int OrderNum = inifile.Get("ORDER", "ORDER_NUM", 0);
	//for (int i = 0; i < OrderNum; i++)
	//{
	//	inifile.Get("ORDER", "ORDER_NAME" + i, ascbuf);
	//}
	inifile.Get("ORDER", "ORDER_NAME1", ascbuf, "");
	m_order1 = ascbuf;
	inifile.Get("ORDER", "ORDER_NAME2", ascbuf, "");
	m_order2 = ascbuf;
	inifile.Get("ORDER", "ORDER_NAME3", ascbuf, "");
	m_order3 = ascbuf;
	inifile.Get("ORDER", "ORDER_NAME4", ascbuf, "");
	m_order4 = ascbuf;
	inifile.Get("ORDER", "ORDER_NAME5", ascbuf, "");
	m_order5 = ascbuf;
	inifile.Get("ORDER", "ORDER_NAME6", ascbuf, "");
	m_order6 = ascbuf;
	inifile.Get("ORDER", "ORDER_NAME7", ascbuf, "");
	m_order7 = ascbuf;
	inifile.Get("ORDER", "ORDER_NAME8", ascbuf, "");
	m_order8 = ascbuf;
	inifile.Get("ORDER", "ORDER_NAME9", ascbuf, "");
	m_order9 = ascbuf;
	inifile.Get("ORDER", "ORDER_NAME10", ascbuf, "");
	m_order10 = ascbuf;

	UpdateData(FALSE);
}

//CString GetModuleDir()
//{
//	HMODULE module = GetModuleHandle(0);
//	char pFileName[MAX_PATH];
//	GetModuleFileName(module, pFileName, MAX_PATH);
//
//	CString csFullPath(pFileName);
//	int nPos = csFullPath.ReverseFind(_T('\\'));
//	if (nPos < 0)
//		return CString("");
//	else
//		return csFullPath.Left(nPos);
//}
//
//
////获取工作路径
//CString GetWorkDir()
//{
//	char pFileName[MAX_PATH];
//	int nPos = GetCurrentDirectory(MAX_PATH, pFileName);
//
//	CString csFullPath(pFileName);
//	if (nPos < 0)
//		return CString("");
//	else
//		return csFullPath;
//}

