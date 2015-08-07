
// TLView.cpp : CTLView 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include <mmsystem.h>
#include <process.h>
#include "timing.h"
#include "canlib.h"
#include "Cprofile.h"
#include "ProgCancel.h"
#include "TLDoc.h"
#include "TLView.h"
#include "ExcelFileDlg.h"
#include "excel.h"
using namespace excel9;
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

ULONG getULong(void);
int getByte(int);
int atoh(char);
void getDate(void);
int HexToBin(char *name, BYTE *cmd);
// CTLView

IMPLEMENT_DYNCREATE(CTLView, CFormView)

BEGIN_MESSAGE_MAP(CTLView, CFormView)
	ON_BN_CLICKED(IDC_APPLICATION_BROWSE, &CTLView::OnBnClickedApplicationBrowse)
	ON_BN_CLICKED(IDC_APPLICATION_IMPORT, &CTLView::OnBnClickedApplicationImport)
	ON_CBN_SELCHANGE(IDC_COMBO_PRODUCT_TYPE, &CTLView::OnSelchangeComboProductType)
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_BUTTON_DOWNLOAD, &CTLView::OnBnClickedButtonDownload)
	ON_WM_TIMER()
END_MESSAGE_MAP()

extern CTLView *pForm;

CProgCancel *m_cancel=NULL;
TIMECAPS tcaps;
MMRESULT gwIDmm0, gwIDmm1;
canHandle hCAN = 0;
char ascbuf[4096];
// CTLView 构造/析构


CTLView::CTLView()
	: CFormView(CTLView::IDD)
	, m_resetcheck(FALSE)
	, m_product_type(_T(""))
{
	// TODO:  在此处添加构造代码
	TCHAR szPath[MAX_PATH];
	GetModuleFileName(NULL, szPath, MAX_PATH);
	CString PathName(szPath);
	CString PROGRAM_PATH = PathName.Left(PathName.ReverseFind(_T('\\')) + 1) + "TL.ini";
	memset(iniFilePath, '\0', sizeof(iniFilePath));
	memcpy(iniFilePath, PROGRAM_PATH.GetBuffer(), PROGRAM_PATH.GetLength());

	hasImportPara = FALSE;

	char tempKey[10];
	uint i;
	U32  tempKeyUL;

	C_PrivProfile inifile(iniFilePath);
	inifile.Get("PRODUCT", "ID", ascbuf, "0000000");
	m_product_id = ascbuf;
	inifile.Get("PRODUCT", "TYPE", ascbuf, "1111111");
	m_product_type = ascbuf; 
	//SetDlgItemText(IDC_COMBO_PRODUCT_TYPE, m_product_type);
	can_hardware = inifile.Get("CAN", "HARDWARE", 0);
	can_channel = inifile.Get("CAN", "CHANNEL", 0);
	inifile.Get("CAN", "TRAID", ascbuf, "18DA1700");
	TraID = getULong();
	inifile.Get("CAN", "RECID", ascbuf, "18DA0017");
	RecID = getULong();
	inifile.Get("CAN", "CHECKID", ascbuf, "0CFE6CEE");
	CheckID = getULong();

	switch (inifile.Get("CAN", "BAUDRATE", 0))
	{
	case 0:
		baud_rate_kvaser = BAUD_250K;
		break;
	case 1:
		baud_rate_kvaser = BAUD_500K;
		break;
	case 2:
		baud_rate_kvaser = BAUD_1M;
		break;
	default:
		baud_rate_kvaser = BAUD_250K;
		break;
	}

	inifile.Get("ACCESS", "KEY_SUPPLIER_1_SAW", tempKey, "");
	for (i = 0; i<strlen(tempKey); i++)
		tempKeyUL = (tempKeyUL << 4) | atoh(tempKey[i]);
	key_supplier[0] = tempKeyUL;
	inifile.Get("ACCESS", "KEY_SUPPLIER_2_SAW", tempKey, "");
	for (i = 0; i<strlen(tempKey); i++)
		tempKeyUL = (tempKeyUL << 4) | atoh(tempKey[i]);
	key_supplier[1] = tempKeyUL;
	inifile.Get("ACCESS", "KEY_SUPPLIER_3_SAW", tempKey, "");
	for (i = 0; i<strlen(tempKey); i++)
		tempKeyUL = (tempKeyUL << 4) | atoh(tempKey[i]);
	key_supplier[2] = tempKeyUL;
	inifile.Get("ACCESS", "KEY_SUPPLIER_4_SAW", tempKey, "");
	for (i = 0; i<strlen(tempKey); i++)
		tempKeyUL = (tempKeyUL << 4) | atoh(tempKey[i]);
	key_supplier[3] = tempKeyUL;

	m_resetcheck = inifile.Get("OTHER", "REBOOT", 0);

	BOOL *downchecklist[10] = { &m_CmdDataCheck1, &m_CmdDataCheck2, &m_CmdDataCheck3, &m_CmdDataCheck4, &m_CmdDataCheck5,
		&m_CmdDataCheck6, &m_CmdDataCheck7, &m_CmdDataCheck8, &m_CmdDataCheck9, &m_CmdDataCheck10, };
	BYTE *allcmd[10] = { m_CmdData1, m_CmdData2, m_CmdData3, m_CmdData4, m_CmdData5,
		m_CmdData6, m_CmdData7, m_CmdData8, m_CmdData9, m_CmdData10 };

	for (int i = 1; i <= 10; i++)
	{
		char tmp[20] = {};
		sprintf_s(tmp, "ORDER_NAME%d", i);
		inifile.Get("ORDER", tmp, ascbuf, "");
		if (strlen(ascbuf)!= 0) {
			*downchecklist[i-1] = TRUE;
			int ret;
			if (ret = HexToBin(ascbuf, allcmd[i - 1]))
			{
				if (ret == 1)
					MessageBox("Non-hex character in string", "Command", MB_ICONHAND | MB_OK);

				if (ret == 2)
					MessageBox("Odd number of characters in string", "Command", MB_ICONHAND | MB_OK);
			}
		}
	}
}

CTLView::~CTLView()
{

}

void CTLView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_PRODUCT_TYPE, m_combotype);
	DDX_Control(pDX, IDI_ICON_GREEN, m_hicon_green);
	DDX_Control(pDX, IDI_ICON_RED, m_hicon_red);
	DDX_Check(pDX, IDC_RESETCHECK1, m_resetcheck);
	DDX_Text(pDX, IDC_STATIC_PRODUCT_TYPE, m_product_type);
	DDX_Control(pDX, IDC_RICHEDIT, m_log);
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

	pForm = this;

	m_combotype.SetCurSel(0);
	if (!HWinit())
	{
		MessageBoxA(_T("初始化can总线失败"));
	}
	SetTimer(checkConetState, 500, NULL);
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
void CTLView::OnBnClickedApplicationBrowse()
{
	// TODO:  在此添加控件通知处理程序代码
	CExcelFileDlg dlg;
	UpdateData(TRUE);
	if (dlg.DoModal() == IDOK)
	{
		m_excel_file = dlg.GetPathName();
		UpdateData(FALSE);
	}
	SetDlgItemText(IDC_APPLICATION_FILE, m_excel_file);
}
void CTLView::OnBnClickedApplicationImport()
{
	// TODO:  在此添加控件通知处理程序代码
	int start = m_excel_file.ReverseFind('\\');
	CString tmp = CString(m_excel_file.Mid(start + 1, m_product_id.GetLength()));
	if (tmp != m_product_id)
	{
		MessageBoxA(_T("参数文件版本不正确"));
		return;
	}
	hasImportPara = TRUE;
	//要操作表，必须先逐步获取Workbooks->Workbook->Worksheets->Worksheet->Range

	_Application app;  //先创建一个_Application类，用_Application来创建一个Excel应用程序接口
	Workbooks books;   //工作薄集合
	_Workbook book;    //工作薄
	Worksheets sheets; //工作表集合
	_Worksheet sheet;  //工作表
	Range range;       //单元格区域对象

	/*    COleVariant类为VARIANT数据类型的包装，
	在自动化程序中，通常都使用ARIANT数据类型进行参数传递。
	故下列程序中，函数参数都是通过COleVariant类来转换了的。
	*/

	//covOptional 可选参数的VARIANT类型
	COleVariant covOptional((long)DISP_E_PARAMNOTFOUND, VT_ERROR);

	if (!app.CreateDispatch("Excel.Application"))
	{
		this->MessageBox(_T("无法创建Excel应用！"));
		return;
	}

	CFileFind filefind;
	if (!filefind.FindFile(m_excel_file))
	{
		AfxMessageBox(_T("没有找到模版文档，请其查找"));
		return;
	}
	LPDISPATCH lpDisp; 	//接口指针
	//获取工作薄集合
	books = app.GetWorkbooks();
	VARIANT readonly;
	readonly.vt = VT_INT;
	readonly.intVal = 1;
	lpDisp = books.Open(m_excel_file,
		covOptional, readonly, covOptional, covOptional,
		covOptional, covOptional, covOptional, covOptional,
		covOptional, covOptional, covOptional, covOptional,
		covOptional, covOptional);					//与的不同，是个参数的，直接在后面加了两个covOptional成功了
	book.AttachDispatch(lpDisp);
	//显示Excel表格，并设置状态为用户可控制
	app.SetVisible(FALSE);
	//获取工作表集合
	sheets = book.GetSheets();
	//获取第一个工作表
	sheet = sheets.GetItem(COleVariant((short)1));

	int ID[10] = { IDC_PARA_NAME1, IDC_PARA_NAME2, IDC_PARA_NAME3, IDC_PARA_NAME4, IDC_PARA_NAME5, IDC_PARA_NAME6,
		IDC_PARA_NAME7, IDC_PARA_NAME8, IDC_PARA_NAME9, IDC_PARA_NAME10 };
	int VALUE[10] = { IDC_PARA_VALUE1, IDC_PARA_VALUE2, IDC_PARA_VALUE3, IDC_PARA_VALUE4, IDC_PARA_VALUE5, IDC_PARA_VALUE6,
		IDC_PARA_VALUE7, IDC_PARA_VALUE8, IDC_PARA_VALUE9, IDC_PARA_VALUE10 };
	//选择工作表中A1:A1单元格区域
	for (int i = 0; i < 10; i++)
	{
		CString A;
		A.Format("A%d", i+1);
		CString B;
		B.Format("B%d", i+1);
		CString C;
		C.Format("C%d", i + 1);
		range = sheet.GetRange(COleVariant(A), COleVariant(A));
		COleVariant rValue;
		rValue = COleVariant(range.GetValue2());
		rValue.ChangeType(VT_BSTR);
		SetDlgItemText(ID[i], CString(rValue.bstrVal));
		range = sheet.GetRange(COleVariant(B), COleVariant(B));
		rValue = COleVariant(range.GetValue2());
		rValue.ChangeType(VT_BSTR);
		SetDlgItemText(VALUE[i], CString(rValue.bstrVal));

		range = sheet.GetRange(COleVariant(C), COleVariant(C));
		rValue = COleVariant(range.GetValue2());
		rValue.ChangeType(VT_UINT);
		paravalue[i] = rValue.uintVal;
	}
	range.ReleaseDispatch();
	sheet.ReleaseDispatch();
	sheets.ReleaseDispatch();
	book.ReleaseDispatch();
	books.ReleaseDispatch();
	app.Quit();
	app.ReleaseDispatch();
	app = NULL;

	BOOL downchecklist[10] = { m_CmdDataCheck1, m_CmdDataCheck2, m_CmdDataCheck3, m_CmdDataCheck4, m_CmdDataCheck5,
		m_CmdDataCheck6, m_CmdDataCheck7, m_CmdDataCheck8, m_CmdDataCheck9, m_CmdDataCheck10, };
	BYTE *allcmd[10] = { m_CmdData1, m_CmdData2, m_CmdData3, m_CmdData4, m_CmdData5,
		m_CmdData6, m_CmdData7, m_CmdData8, m_CmdData9, m_CmdData10 };

	for (int i = 0; i < 10; i++)
	{
		if (downchecklist[i])
		{
			BYTE *tmp = (BYTE*)&paravalue[i];
			allcmd[i][4] = BYTE(tmp[0]);
			allcmd[i][5] = BYTE(tmp[1]);
			allcmd[i][6] = BYTE(tmp[2]);
			allcmd[i][7] = BYTE(tmp[3]);
			//memcpy((void *)(&allcmd[i][4]), (void *)&paravalue[i], 4);
		}
	}
}
void CTLView::OnSelchangeComboProductType()
{
	// TODO:  在此添加控件通知处理程序代码
	int comboType;
	comboType = m_combotype.GetCurSel();

	return;
}


int CTLView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFormView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	return 0;
}

void CTLView::OnBnClickedButtonDownload()
{
	// TODO:  在此添加控件通知处理程序代码
	UpdateData();
	if (!hasImportPara && !m_resetcheck)
	{
		MessageBoxA(_T("请先导入参数"));
		return;
	}
	m_CmdResetCheck = m_resetcheck;
	SetTimer(checkTimerID, 1000, NULL);

	if (!Check_Equipment_State())
	{
		MessageBoxA(_T("请连接设备"));
	}
	else
	{
		SetDownloadClick();
	}
}

//void CALLBACK ActiveCheck(HWND hwnd, UINT nMsg, UINT nTimerid, Dowr)

#if 0
void CTLView::OnInitialUpdate() 
{
   BOOL err=FALSE;

   pForm = this;
   CFormView::OnInitialUpdate();

   EnableToolTips(TRUE);

   if (!(hWnd=GetSafeHwnd()))
      err = TRUE;

   else if (timeGetDevCaps(&tcaps,sizeof(tcaps)) != TIMERR_NOERROR)
      err = TRUE;

   else if (timeBeginPeriod(tcaps.wPeriodMin) != TIMERR_NOERROR)  
      err = TRUE;

   if (err)
      MessageBox("Could not initialize timing system",
		 "Multimedia timing",MB_ICONHAND|MB_OK);

//   GetSetup();
	
   if (InitCan())
   {
      can_enabled = TRUE;
//      m_controlstatus = "CAN initialized";
   }

   else
   {
      MessageBox("CAN card not installed","Initializing",MB_ICONHAND|MB_OK);
//      m_controlstatus = "CAN not activated";
   }

   UpdateData(FALSE);

   // KWP is our default protocol
//   m_radio_kwp_ctrl.SetCheck(BST_CHECKED);
   //m_radio_1mb_flash_boot_ctrl.SetCheck(BST_CHECKED);
//   m_radio_2mb_flash_boot_ctrl.SetCheck(BST_CHECKED);

//   m_radio_32_byte_data_length.EnableWindow(FALSE);
//   m_radio_128_byte_data_length.EnableWindow(FALSE);
//   m_radio_256_byte_data_length.EnableWindow(FALSE);
//   m_radio_2048_byte_data_length.EnableWindow(FALSE);

}

BOOL CTLView::OnToolTipNotify(UINT id, NMHDR *pNMHDR, LRESULT *pResult)
{
   TOOLTIPTEXT *pTTT = (TOOLTIPTEXT *)pNMHDR;
   UINT nID = pNMHDR->idFrom;

   if (pTTT->uFlags & TTF_IDISHWND)
   {
      nID = ::GetDlgCtrlID((HWND)nID);

      if (nID)
      {
         pTTT->lpszText = MAKEINTRESOURCE(nID);
         pTTT->hinst = AfxGetResourceHandle();
         return TRUE;
      }
   }

   return FALSE;
}
#endif

/* ***********************
***********************
**                   **
**  API              **
**                   **
***********************
*********************** */

/*
  Standard Windows timers. These timers have rather coarse
  resolution, in the order of 50 ms.

  Right now, two standard timers are implemented.

  Timer 0:
  set by: gwID0 = SetTimer(1,<timeout>,0);
  killed by: KillTimer(gwID0);
  tripping indicated by: b_tmout0

  Timer 1:
  set by: gwID1 = SetTimer(2,<timeout>,0);
  killed by: KillTimer(gwID1);
  tripping indicated by: b_tmout1

  <timeout> is in ms.

  After either timer has tripped, make sure to reset the
  corresponding b_tmoutx flag.

  Timer0 is currently used as a general lack of response timeout,
  configurable in the Setup dialog as Time Noresp.

  Timer1 is used to generate periodic Tester Present messages.
*/

/*

	
Use SetMMTimer() and KillMMTimer() to activate and deactivate.

Two multimedia timers are currently implemented, tripping
being indicated by b_tmoutmm0 and b_tmoutmm1, respectively.
*/

/*
  Multimedia timers. These have better resolution than
  standard timers (down to 1 ms) but are more CPU intensive
  and require timing.dll (or equivalent).

  Two multimedia timers are currently used.

  MMtimer 0:
  set by:					SetMMTimer(0,<timeout>;
  killed by:				KillMMTimer(0);
  tripping indicated by:	b_tmoutmm0

  MMtimer 1:
  set by:					SetMMTimer(1,<timeout>);
  killed by:				KillMMTimer(1);
  tripping indicated by:	b_tmoutmm1

  <timeout> is in ms.

  After either timer has tripped, make sure to reset the
  corresponding b_tmoutmmx flag.

  MMtimer0 is used for KWP2000 timing, MMtimer 1 for USDT timing.
*/
void CTLView::SetMMTimer(int tn, int timeout)
{
   if (tn)
   {	
      if (b_actmm1 && (!b_tripmm1))
         timeKillEvent(gwIDmm1);

      b_actmm1 = FALSE;
      b_tripmm1 = FALSE;

      if (timeout)
      {
         if (!(gwIDmm1=timeSetEvent(timeout,tcaps.wPeriodMin,TimerCallBack,
                                    (DWORD_PTR)hWnd,TIME_ONESHOT)))
//Disable by liangyuan            MessageBox("failed to set timer 1",
//Disable by liangyuan                       "Multimedia timing error",MB_ICONHAND|MB_OK);
			RespError(NULL, NULL);

         b_actmm1 = TRUE;
         b_tmoutmm1 = FALSE;
      }

      else
         b_tmoutmm1 = TRUE;
   }

   else	
   {
      if (b_actmm0 && (!b_tripmm0))
         timeKillEvent(gwIDmm0);

      b_actmm0 = FALSE;
      b_tripmm0 = FALSE;

      if (timeout)
      {
         if (!(gwIDmm0=timeSetEvent(timeout,tcaps.wPeriodMin,TimerCallBack,
                                    (DWORD_PTR)hWnd,TIME_ONESHOT)))
//Disable by liangyuan            MessageBox("failed to set timer 0",
//Disable by liangyuan                       "Multimedia timing error",MB_ICONHAND|MB_OK);
			RespError(NULL, NULL);

         b_actmm0 = TRUE;
         b_tmoutmm0 = FALSE;
      }

      else
         b_tmoutmm0 = TRUE;
   }
}

void CTLView::KillMMTimer(int tn)
{
   if (tn)
   {
      timeKillEvent(gwIDmm1);
      b_actmm1 = FALSE;
      b_tmoutmm1 = FALSE;
   }

   else
   {
      timeKillEvent(gwIDmm0);
      b_actmm0 = FALSE;
      b_tmoutmm0 = FALSE;
   }
}

/*
  ProcessMessage() is called whenever there is an
  incoming KWP2000 message pending. It is responsible
  for executing the appropriate state machine.

  The incoming message is in recbuf, with length rec_len.
*/

void CTLView::ProcessMessage(void)
{
   usdt_pending = PEND_NONE;
	
   switch (process_state)
   {
   case PROCESS_NONE:
      if (recbuf[0] == 0x7e)		// TesterPresent response
      {
         KillTimer(gwID0);
         b_tmout0 = FALSE;
      }

      break;
#if 0
   case PROCESS_RESET:
      Reset_Target_State_Process();
      break;
#endif
   case PROCESS_LOGIN:
      Login_State_Process();
      break;

#if 0
   case PROCESS_FLASH:
      Flash_State_Process();
      break;

   case PROCESS_EE:
      EE_State_Process();
      break;

   case PROCESS_EE_IMG_READ:
      EEImg_Read_State_Process();
      break;

   case PROCESS_EE_IMG_WRITE:
      EEImg_Write_State_Process();
      break;

   case PROCESS_SCRIPT:
      Script_State_Process();
      break;
#endif

   case PROCESS_PTLOGIN:
      PTLogin_State_Process();
      break;

   default:
      break;
   }
}

/*
  kwpc_sendMsg() is is used to transmit a KWP2000 message.
  The outgoing message should be placed in buf, with length len.
*/

BOOL CTLView::kwpc_sendMsg(BYTE *buf, int len)	// SEND_KWP_MSG
{
   if ((kwp_state != KWP_IDLE) && (buf[0] != tp[0]))
      return FALSE;

   memcpy(kwp_buf,buf,len);
   kwp_len = len;

   if (!usdt_dataReq(kwp_buf,kwp_len))
      return FALSE;

   if (b_loggedin)
      gwID1 = SetTimer(2,t_tp,0);

   LogDisp(0,kwp_buf,kwp_len);

   if (buf[0] != tp[0])
      kwp_state = WAIT_FOR_CONF;
	
   return TRUE;
}

BOOL CTLView::InitCan(void)
{
   switch (can_hardware)
   {
   case 0:			// Kvaser
      canInitializeLibrary();
			
      if ((hCAN=canOpenChannel(can_channel,canWANT_EXCLUSIVE)) < 0)
      {
         CanError(hCAN,"canOpenChannel");
         return FALSE;
      }

      if ((cs=canSetBusParams(hCAN,baud_rate_kvaser,0,0,0,0,0)) != canOK)
      {
         CanError(cs,"canSetBusParams");
         return FALSE;
      }

      if ((cs=canSetBusOutputControl(hCAN,canDRIVER_NORMAL)) != canOK)
      {
         CanError(cs,"canSetBusOutputControl");
         return FALSE;
      }

      if ((cs=canBusOn(hCAN)) != canOK)
      {
         CanError(cs,"canBusOn");
         return FALSE;
      }

      break;

   default:
      return FALSE;
   }
	
   return TRUE;
}

void CTLView::ExitCan(void)
{
   if (can_enabled)
   {
      switch (can_hardware)
      {
      case 0:			// Kvaser
         canBusOff(hCAN);
         canClose(hCAN);
         break;

      default:
         break;
      }

      can_enabled = FALSE;
   }
}

void CTLView::CanError(int stat, char *b)
{
   	
   tbuf[0] = 0;
	
   switch (can_hardware)
   {
   case 0:			// Kvaser
      canGetErrorText((canStatus)stat,tbuf,sizeof(tbuf));
      break;

   default:
      break;
   }

//Disable by liangyuan   MessageBox(tbuf,b,MB_ICONHAND|MB_OK);
//		RespError(NULL, NULL);
}

// kwp state machine
BOOL CTLView::KWP_State_Process(void)
{
   switch (kwp_state)
   {
   case KWP_IDLE:
	  if (usdt_state == USDT_IDLE)
	  {
		 switch (usdt_pending)
		 {
		 case PEND_BUF:
			LogDisp(1,recbuf,rec_len);
//			CommError("Unsolicited message received");
			break;

		 case PEND_FF:
//			CommError("Unexpected FF received");
			break;

		 case PEND_CONF:
//			CommError("Unexpected confirmation received");
			break;

		 default:
			break;
		 }

		 usdt_pending = PEND_NONE;
	  }

	  break;

   case WAIT_FOR_CONF:
	  if (usdt_pending == PEND_CONF)		// DATA_CONF_OK
	  {
		 SetMMTimer(0,t_p2);
		 kwp_state = WAIT_FOR_RESP;
		 usdt_pending = PEND_NONE;
	  }

	  else if (usdt_pending != PEND_NONE)
	  {
//		 CommError("Unexpected FF or message received");
		 usdt_pending = PEND_NONE;
	  }

	  else if (usdt_error == ERR_TIMEOUT_AS)
	  {
//		 CommError("USDT error ERR_TIMEOUT_AS");
		 usdt_state = USDT_IDLE;
		 usdt_error = ERR_NONE;
		 usdt_dataReq(kwp_buf,kwp_len); 	// retry
	  }

	  else if (usdt_error != ERR_NONE)
	  {
//		 sprintf_s(ascbuf,"USDT error %d",usdt_error);
//		 CommError(ascbuf);
		 usdt_state = USDT_IDLE;
		 kwp_state = KWP_IDLE;
		 usdt_error = ERR_NONE;
	  }

	  break;

   case WAIT_FOR_RESP:
	  if (b_tmoutmm0)						// P2_TIMEOUT
	  {
//		 CommError("P2 timeout");
		 kwp_state = KWP_IDLE;
		 break;
	  }

	  if (usdt_error != ERR_NONE)			// OTHER_ERROR
	  {
//		 sprintf_s(ascbuf,"USDT error %d",usdt_error);
//		 CommError(ascbuf);
		 usdt_state = USDT_IDLE;
		 kwp_state = KWP_IDLE;
	  }

	  if (usdt_pending == PEND_FF)		// FF_IND
	  {
		 KillMMTimer(0);
		 kwp_state = WAIT_FOR_RESP_COMPL; 
	  }
				
	  else if (usdt_pending == PEND_BUF)	// DATA_IND_OK
	  {
		 LogDisp(1,recbuf,rec_len);
		 usdt_pending = PEND_NONE;			
		 usdt_error = ERR_NONE; 				

		 if ((recbuf[0] == 0x7f) && (recbuf[2] == 0x78))		// RC78_MSG
		 {
			SetMMTimer(0,t_p2long);
					
			if (process_state != PROCESS_NONE)
			   gwID0 = SetTimer(1,t_noresp+t_p2long,0);

		m_Send_Cmd_State = SEND_FALSE;	//Added by michael
			return FALSE;
		 }

		 else							// SERVICE_MSG
		 {
			KillMMTimer(0);
			kwp_state = KWP_IDLE;
			ProcessMessage();		// incoming message in buffer
			m_Send_Cmd_State = SEND_TRUE; //Added by michael
			return TRUE;
		 }
	  }

	  else if (usdt_pending != PEND_NONE)		// OTHER_ERROR
	  {
	  }
//		 CommError("Unexpected confirmation received");

	  usdt_pending = PEND_NONE; 		
	  usdt_error = ERR_NONE;
	  break;

   case WAIT_FOR_RESP_COMPL:
	  if (usdt_pending == PEND_BUF)
	  {
		 LogDisp(1,recbuf,rec_len);
		 usdt_pending = PEND_NONE;			
		 usdt_error = ERR_NONE;
				
		 if ((recbuf[0] == 0x7f) && (recbuf[2] == 0x78))		// RC78_MSG
		 {
			SetMMTimer(0,t_p2long);
			gwID0 = SetTimer(1,t_noresp+t_p2long,0);
		m_Send_Cmd_State = SEND_FALSE;	//Added by michael
			return FALSE;
		 }
				
		 else								// DATA_IND_OK
		 {
			KillMMTimer(0);
			kwp_state = KWP_IDLE;
			ProcessMessage();			// incoming message in buffer
			m_Send_Cmd_State = SEND_TRUE; //Added by michael
			return TRUE;
		 }
	  }

	  else if (usdt_pending != PEND_NONE)
	  		;
//		 CommError("Unexpected FF or confirmation received");

	  else if ((usdt_error == ERR_TIMEOUT_AR) || (usdt_error == ERR_TIMEOUT_CR))
		 // DATA_IND_NOK
	  {
		 SetMMTimer(0,t_p2long);			// wait long
		 kwp_state = WAIT_FOR_RESP;
	  }

	  else if (usdt_error != ERR_NONE)	// OTHER_ERROR
	  {
//		 sprintf_s(ascbuf,"USDT error %d",usdt_error);
//		 CommError(ascbuf);
		 usdt_state = USDT_IDLE;
		 kwp_state = KWP_IDLE;
	  }

	  usdt_pending = PEND_NONE; 		
	  usdt_error = ERR_NONE;
	  break;

   default:
	  break;
   }

   return FALSE;
}


//void CTLView::CommError(char *text)
//{
//}

BOOL CTLView::usdt_dataReq(BYTE *buf, int len)
{
   BOOL b_single=FALSE; 
   int css;

   if (usdt_state != USDT_IDLE)
      return FALSE;

   usdt_error = ERR_NONE;
   usdt_pending = PEND_NONE;

   // If sending PT code login command
   if((buf[0]==0x01) && (buf[1]==0x01) && (buf[2]==0x20) &&(buf[3]==0x64) &&(buf[4]==0x04) &&(buf[5]==0x05) &&(buf[6]==0xFB) &&(buf[7]==0x11) )
   {
      pt_code = TRUE;
      b_loggedin = FALSE;
//      m_radio_pt_ctrl.SetCheck(BST_CHECKED);
//      m_radio_kwp_ctrl.SetCheck(BST_UNCHECKED);
   }

   if (pt_code)
   {
      b_single = TRUE;
      can_tralen = 8;
      memcpy(&can_trabuf[0],buf,len);

      // If sending PT code logout command
      if((buf[0]==0x01) && (buf[1]==0x02) && (buf[2]==0x19) &&(buf[3]==0x67) &&(buf[4]==0x09) &&(buf[5]==0x08) &&(buf[6]==0xFF) &&(buf[7]==0x40) )
      {
         pt_code = FALSE;
         b_loggedin = TRUE;
//         m_radio_pt_ctrl.SetCheck(BST_UNCHECKED);
//         m_radio_kwp_ctrl.SetCheck(BST_CHECKED);
      }
   }
   else
   {
      if (len < 8)
      {
         b_single = TRUE;
         can_trabuf[0] = len;				// SEND_SF_MSG
         can_tralen = len + 1;
         memcpy(&can_trabuf[1],buf,len);
      }

      else
      {
         tseq = 1;
         memcpy(trabuf,buf,len);
         can_trabuf[0] = 0x10 | (len >> 8);	// SEND_MSG
         can_trabuf[1] = len & 0xff;
         memcpy(&can_trabuf[2],buf,6);
         tra_len = len - 6;
         can_tralen = 8;
         memcpy(trabuf,&buf[6],tra_len);
         trabufp = trabuf;
      }
   }


   // SendSFFrame, SendFFFrame
   if ((css=PutCanFrame()) == canERR_TIMEOUT)	// SEND_NOK
   {
      usdt_error = ERR_TIMEOUT_AS;
      return FALSE;
   }

   if (css != canOK)
      return FALSE;

   if (b_single)							// SEND_OK
   {
      if (buf[0] == tp[0])
         return TRUE;

      usdt_pending = PEND_CONF;
   }

   else									// SEND_OK
   {
      SetMMTimer(1,n_bs);
      usdt_state = WAIT_FOR_FC;
   }

   return TRUE;
}

int CTLView::PutCanFrame(void)
{
   if (!can_enabled)
      return 0;

   switch (can_hardware)
   {
   case 0:			// Kvaser
      if ((cs=canWrite(hCAN,TraID,can_trabuf,can_tralen,canMSG_EXT)) != canOK)
      {
         CanError(cs,"canWrite");
         return cs;
      }

      if (((cs=canWriteSync(hCAN,n_as)) != canOK) && (cs != canERR_TIMEOUT))
         CanError(cs,"canWriteSync");

      break;

   default:
      cs = 0;
      break;
   }

   return cs;
}

BOOL CTLView::GetCanFrame(void)
{
   if (!can_enabled)
      return 0;

	if (FALSE == m_OnDownloadState)
		{
		//if (cs = canRead(hCAN, NULL, NULL, NULL, NULL, NULL) == canOK)
	   if ((cs = canReadSpecific(hCAN, CheckID, NULL, NULL, NULL, NULL)) == canOK)
			{
			m_ConnectState = TRUE;
			//m_CheckConnect = TRUE;
			}
		else
			{
			if (cs != canERR_NOMSG)
			{
				CanError(cs, "canRead");
			}
			//m_CheckConnect = FALSE;
			}
		}
	
   switch (can_hardware)
   {
   case 0:			// Kvaser
      if ((cs=canReadSpecificSkip(hCAN,RecID,can_recbuf,&can_reclen,NULL,NULL)) == canOK)
         return TRUE;

      if (cs != canERR_NOMSG)
         CanError(cs,"canReadSpecificSkip");

      break;

   default:
      break;
   }

   return FALSE;
}

// usdt state machine
void CTLView::USDT_State_Process(void)
{
   int css;

   if (usdt_error != ERR_NONE)
   {
//      sprintf_s(ascbuf,"USDT error %s",usdt_errtext[usdt_error-1]);
//      CommError(ascbuf);
      usdt_error = ERR_NONE;
   }

   switch (usdt_state)
   {
   case USDT_IDLE:
      if (usdt_pending != PEND_NONE)
         break;				// wait for kwp process to get buffer

      if (GetCanFrame())								// USDT_MSG_RECV
      {
         // checkRecvdFrame
         if(pt_code)
         {
            rec_len = 8;
            memcpy(recbuf,&can_recbuf[0],rec_len);
            usdt_pending = PEND_BUF;
         }
         else if (!(can_recbuf[0] & 0xf0))				// SF_RECV
         {
            // storeData
            rec_len = can_recbuf[0] & 0xf;
            memcpy(recbuf,&can_recbuf[1],rec_len);
            usdt_pending = PEND_BUF;
         }

         else if ((can_recbuf[0] & 0xf0) == 0x10)	// FF_RECV
         {
            // storeData
            rec_len = ((can_recbuf[0] & 0xf) << 8) | can_recbuf[1];
            recbufp = recbuf;
            memcpy(recbufp,&can_recbuf[2],6);
            recbufp += 6;
            rec_lencur = rec_len - 6;
            can_trabuf[0] = 0x30;					// SendFCFrame
            can_trabuf[1] = tra_bs;
            BScur = tra_bs;
            can_trabuf[2] = n_stmin;
            can_tralen = 3;

            if ((css=PutCanFrame()) == canERR_TIMEOUT)	// SEND_NOK
               usdt_error = ERR_TIMEOUT_AR;

            else if (css == canOK)					// SEND_OK
            {
               SetMMTimer(1,n_cr);
               rseq = 1;
               usdt_state = RECEIVE_CF;
            }

            usdt_pending = PEND_FF;					// FF_INDICATION
         }
      }

      break;

   case WAIT_FOR_FC:
      if (b_tmoutmm1)									// TIMEOUT
      {
         usdt_error = ERR_TIMEOUT_BS;
         usdt_state = USDT_IDLE;
      }

      else
      {
         if (GetCanFrame())							// USDT_MSG_RECV
         {
            // checkRecvdFrame
            if (can_recbuf[0] == 0x31)				// FC_WT_RECV
               SetMMTimer(1,n_bs);

            else if (can_recbuf[0] == 0x30)			// FC_CTS_RECV
            {
               bs = can_recbuf[1];
               bscur = bs;
               stmin = can_recbuf[2];
               SetMMTimer(0,5000);
               SetMMTimer(1,stmin);
               usdt_state = WAIT_FOR_STMIN;
            }
         }
      }

      break;

   case WAIT_FOR_STMIN:
      if (b_tmoutmm1)									// TIMEOUT
      {
         // sendCFFrame
         can_trabuf[0] = 0x20 | (tseq++ & 0xf);
         memcpy(&can_trabuf[1],trabufp,7);
         trabufp += 7;
         can_tralen = min(tra_len,7);
         tra_len -= can_tralen;
         can_tralen++;
         bscur--;

         if (PutCanFrame() != canOK)					// SEND_NOK
         {
            usdt_error = ERR_TIMEOUT_AS;
            usdt_state = USDT_IDLE;
         }

         else										// SEND_OK
         {
            // checkDL
            if (!tra_len)							// NO_BYTES_REMAIN
            {
               usdt_pending = PEND_CONF;
               usdt_state = USDT_IDLE;
            }

            else									// BYTES_REMAIN
            {
               // checkNofFrames
               if ((!bs) || bscur)					// NOT_BS_FRAMES
                  SetMMTimer(1,stmin);
						
               else								// BS_FRAMES
               {
                  SetMMTimer(1,n_bs);
                  usdt_state = WAIT_FOR_FC;
               }
            }
         }
      }

      break;

   case RECEIVE_CF:
      if (b_tmoutmm1)									// TIMEOUT
      {
         usdt_error = ERR_TIMEOUT_CR;
         usdt_state = USDT_IDLE;
      }
			
      else
      {
         if (GetCanFrame())							// USDT_MSG_RECV
         {
            // checkRecvdFrame
            if ((can_recbuf[0] & 0xf0) == 0x10)		// FF_RECV
            {
               // storeData
               rec_len = ((can_recbuf[0] & 0xf) << 8) | can_recbuf[1];
               recbufp = recbuf;
               memcpy(recbufp,&can_recbuf[2],6);
               recbufp += 6;
               rec_lencur = rec_len - 6;
               usdt_error = ERR_UNEXP_PDU;
               can_trabuf[0] = 0x30;				// SendFCFrame
               can_trabuf[1] = tra_bs;
               BScur = tra_bs;
               can_trabuf[2] = n_stmin;
               can_tralen = 3;

               if ((css=PutCanFrame()) == canOK)	// SEND_OK
               {
                  SetMMTimer(1,n_cr);
                  rseq = 1;
               }

               else								// SEND_NOK
               {
                  usdt_error = ERR_TIMEOUT_AR;
                  usdt_state = USDT_IDLE;						
               }
            }

            else if (!(can_recbuf[0] & 0xf0))		// SF_RECV
            {
               // storeData
               rec_len = can_recbuf[0] & 0xf;
               memcpy(recbuf,&can_recbuf[1],rec_len);
               usdt_error = ERR_UNEXP_PDU;
               usdt_state = USDT_IDLE;
               KillMMTimer(1);
               usdt_pending = PEND_BUF;
            }

            else if ((can_recbuf[0] & 0xf0) == 0x20)	// CF_RECV
            {
               // checkSN
               if ((can_recbuf[0] & 0xf) != (rseq++ & 0xf))	// SN_NOK
               {
                  usdt_error = ERR_WRONG_SN;
                  usdt_state = USDT_IDLE;
               }

               else									// SN_OK
               {
                  // storeData
                  memcpy(recbufp,&can_recbuf[1],7);
                  recbufp += 7;
                  rec_lencur -= 7;

                  // checkDL
                  if (rec_lencur <= 0)				// NO_BYTES_REMAIN
                  {
                     usdt_error = ERR_NONE;
                     usdt_pending = PEND_BUF;
                     usdt_state = USDT_IDLE;
                     KillMMTimer(1);
                  }

                  else								// BYTES_REMAIN
                  {
                     if (tra_bs)
                     {
                        // checkNofFrames
                        if (!(--BScur))				// BS_FRAMES
                        {
                           can_trabuf[0] = 0x30;		// SendFCFrame
                           can_trabuf[1] = tra_bs;
                           BScur = tra_bs;
                           can_trabuf[2] = n_stmin;
                           can_tralen = 3;

                           if ((css=PutCanFrame()) == canERR_TIMEOUT)
                              // SEND_NOK
                           {
                              usdt_error = ERR_TIMEOUT_AR;
                              usdt_state = USDT_IDLE;									
                           }

                           else if (css == canOK)	// SEND_OK
                              SetMMTimer(1,n_cr);
                        }
                     }
                  }
               }
            }
         }
      }

      break;

   default:
      break;
   }
}

// login state machine
void CTLView::Login_State_Process(void)
{
	BYTE buf[10];
	//CSetup setup;
	keyDataT seed, key;

	switch (login_state)
	{
	case START_DIAGNOSTIC_SESSION:
	  if ((recbuf[0] == 0x50) && (recbuf[1] == 0x86)) // Peter 0x85
		{
		buf[0] = 0x27;
		buf[1] = 0x03;
		kwpc_sendMsg(buf,2);
		login_state = SECURITY_ACCESS_1;
		}
	  else
	  	{
		RespError("Login","Start Diagnostic Session");
	  	}

	  break;

	case SECURITY_ACCESS_1:
	if ((recbuf[0] == 0x67) && (recbuf[1] == 0x03)) 
	{
		seed.part.high = (recbuf[5]<<24) + (recbuf[4]<<16) + (recbuf[3]<<8) + (recbuf[2]);
		seed.part.low  = (recbuf[9]<<24) + (recbuf[8]<<16) + (recbuf[7]<<8) + (recbuf[6]);

		CalculateKey(&seed,&key);
		buf[2] = (BYTE)(key.part.high & 0xFF);
		buf[3] = (BYTE)((key.part.high>> 8) & 0xFF);
		buf[4] = (BYTE)((key.part.high>>16) & 0xFF);
		buf[5] = (BYTE)((key.part.high>>24) & 0xFF);
		buf[6] = (BYTE)(key.part.low & 0xFF);
		buf[7] = (BYTE)((key.part.low>> 8) & 0xFF);
		buf[8] = (BYTE)((key.part.low>>16) & 0xFF);
		buf[9] = (BYTE)((key.part.low>>24) & 0xFF);

		buf[0] = 0x27;
		buf[1] = 0x04;
		kwpc_sendMsg(buf,10);
		login_state = SECURITY_ACCESS_2;
	}

	else
		 RespError("Login","Security Access (1)");

	break;


	case SECURITY_ACCESS_2:
		if ((recbuf[0] == 0x67) && (recbuf[1] == 0x04) && (recbuf[2] == 0x34)) 
		{
			m_Login_Flag = TRUE;	//Logout sucessfully!!!!!
			b_loggedin = TRUE;
			KillTimer(gwID0);
			b_tmout0 = FALSE;
			gwID1 = SetTimer(2,t_tp,0);
			process_state = PROCESS_NONE;
		}

		else
		{
			RespError("Login","Security Access (2)");
		}
	   break;

	case STOP_DIAGNOSTIC_SESSION:
		if ((recbuf[0] == 0x50) && (recbuf[1] == 0x81))
		{
			m_Logout_Flag = TRUE;	//Logout sucessfully!!!!!
			b_loggedin = FALSE;
			KillTimer(gwID0);
			b_tmout0 = FALSE;
			KillTimer(gwID1);
			b_tmout1 = FALSE;
			process_state = PROCESS_NONE;
		}
		else
		{
			RespError("Logout","Stop Diagnostic Session");
		}
		break;

	default:
		break;
   }
}


void CTLView::RespError(char *title, char *service)
{
#if 1
   //int i;
   //char bb[5];

   KillTimer(gwID0);
   b_tmout0 = FALSE;
   process_state = PROCESS_NONE;
//   strcpy_s(ascbuf,"Neg. response on ");
//   strcat_s(ascbuf,service);
//   strcat_s(ascbuf,": ");

//   for (i=0; i<rec_len; i++)
//   {
//      sprintf_s(bb,"%02X ",recbuf[i]);
//      strcat_s(ascbuf,bb);
//   }

   if (m_cancel)
   {
      m_cancel->DestroyWindow();
      m_cancel = NULL;
   }

	m_DownloadFalse = TRUE;
	m_DownloadResult = RESULT_FALSE;
	
//   MessageBox(ascbuf,title,MB_ICONHAND |MB_OK);
//   m_log.EnableWindow(TRUE);

//   m_data_length_button.EnableWindow(TRUE);
//   m_radio_32_byte_data_length.EnableWindow(m_data_length_control);
//   m_radio_128_byte_data_length.EnableWindow(m_data_length_control);
//   m_radio_256_byte_data_length.EnableWindow(m_data_length_control);
//   m_radio_2048_byte_data_length.EnableWindow(m_data_length_control);

//   if(m_data_length_control == TRUE && (m_radio_32_byte_data_length.GetCheck() == TRUE) )
//   {
//	   data_length = BYTES_32;
//   }
	   

//   boot_sequence = 0;
#endif
}

void CTLView::Execute_Processes(void)
{
	Download_Process();
	
//   if (b_progcancel)
//      CancelOperation();
	
   if (KWP_State_Process())
      return;

   USDT_State_Process();

   if (b_tmout0)		// t_noresp
   {
      b_tmout0 = FALSE;
      process_state = PROCESS_NONE;
      usdt_state = USDT_IDLE;
      kwp_state = KWP_IDLE;
//Disable by liangyuan      MessageBox("Communication Timeout","Communication",MB_ICONHAND |MB_OK);
		RespError(NULL, NULL);
   }

   if (b_tmout1)		// t_tp, TesterPresent
   {
      if (b_loggedin)
         kwpc_sendMsg(tp,2);

      else
         KillTimer(gwID1);

      b_tmout1 = FALSE;
   }
}

void CTLView::LogDisp(int ind, BYTE *b, int l)
{
	//static BYTE test_buff[16];
	//memcpy(test_buff, b, l);
	//UpdateData(TRUE);

	int i, j, k = 0;
	char bb[5];

	UpdateData(TRUE);

	//if (!m_enablelog)
	//	return;

	i = m_log.GetLineCount() - 1;
	j = m_log.LineIndex(i);

	if (j)
		j += m_log.LineLength(i) + 2;

	m_log.SetSel(j, j);

	//ZeroMemory(&charf, sizeof(charf));
	//charf.cbSize = sizeof(charf);
	//charf.dwEffects = 0;

	//if (ind)
	//	charf.crTextColor = RGB(0, 127, 0);		// rec

	//else
	//	charf.crTextColor = RGB(255, 0, 0);		// tra

	//charf.dwMask = CFM_COLOR;
	//m_log.SetSelectionCharFormat(charf);

	GetLocalTime(&st);
	sprintf_s(logbuf, "%02d:%02d:%02d.%03d\t", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);

	for (i = 0; i<l; i++)
	{
		sprintf_s(bb, "%02X ", b[i]);
		strcat_s(logbuf, bb);
		k++;
		if (k>35)
		{
			strcat_s(logbuf, "\r\n\t\t");
			k = 0;
		}
	}

	strcat_s(logbuf, "\r\n");
	m_log.ReplaceSel(logbuf);

}


/*************************************************************************************/
/** Encrypts the seed using the given encryption method.
 *************************************************************************************/
void CTLView::CalculateKey(const keyDataT *seed,   /**< [in] 64 bits seed to encrypt. */
							       keyDataT *key     /**< [out] 64 bits key (encrypted seed). */
							 )
{
   const U32 *secret = NULL;

      secret = key_supplier;

   if (secret != NULL)
   {
      Encrypt(seed->data, key->data, secret);
   }
}


/*************************************************************************************/
/** Encrypts the given data using the XTEA algorithm.
 *************************************************************************************/
void CTLView::Encrypt(const U32 data_in[2],  /**< 64-bit data block to encrypt:
                                                       - data_in[0] = upper 32 bits,
                                                       - data_in[1] = lower 32 bits. */
                                   U32 data_out[2], /**< [out] 64-bit encrypted output:
                                                       - data_out[0] = upper 32 bits,
                                                       - data_out[1] = lower 32 bits. */
                             const U32 key[4]       /**< 128-bit key to use for encryption. */
)
{
   const U32  CRYPT_DELTA      = 0x9E3779B9uL; // Magic constant, to avoid round symmetry attacks
   const BYTE NUMBER_OF_ROUNDS = 32u;          // Number of rounds
   U32   sum;
   U32   data_low;
   U32   data_high;
   BYTE  i;

   sum       = 0u;
   data_high = data_in[0];
   data_low  = data_in[1];
   for (i = 0u; i < NUMBER_OF_ROUNDS; i++)
   {
      data_high += (((data_low << 4u) ^ (data_low >> 5u)) + data_low) ^ (sum + key[(sum & 3u)]);
      sum       += CRYPT_DELTA; // Add "key schedule constant"
      data_low  += (((data_high << 4u) ^ (data_high >> 5u) ) + data_high) ^ (sum + key[((sum >> 11u) & 3u)]);
   }
   data_out[0] = data_high;
   data_out[1] = data_low;
}

/*
  ee_ident governs reading and writing of EEprom parameters.
  lsw of each entry is the parameter common or local identifier.
  b16 indicates common identifier.
  b17 means "don't write this parameter".
  b18 means "write fingerprint" if in writing sequence.
  (lsw + b16 holds identifier, timestamp assumed on identifier + 1).
  b19 means "compute checksum" if in writing sequence. lsb holds checkarea code.
  -1 indicates end of table.

  EXAMPLE:
  long ee_ident[]=
  {
// CF_Parameters
0x50098,		// write CF_Parameters fingerprint

// Cluster common EcuIdentification area
0x10087, 0x10091, 0x1008a, 0x10092, 0x10093, 0x10094, 0x10095, 0x10097,
0x1008b, 0x10090,

0x80004,		// compute checksum

// Cluster common configuration area
0x10100,

0x80006,		// compute checksum

// Cluster local configuration area
0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f,
0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57,
0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f,
0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67,
0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d,

0x80007,		// compute checksum

//Cluster local volatile area, DON'T WRITE
0x20080, 0x20081, 0x20082, 0x20083, 0x20084, 0x20085, 0x20086, 0x20087,
0x20088, 0x20089, 0x2008a, 0x2008b, 0x2008c, 0x2008d, 0x2008e, 0x2008f,
0x20090, 0x20091, 0x20092, 0x20093, 0x20094, 0x20095, 0x20096, 0x20097,
0x20098, 0x20099, 0x2009a, 0x2009b, 0x2009c, 0x2009d, 0x2009e, 0x2009f,
0x200a0, 0x200a1, 0x200a2, 0x200a3, 0x200a4, 0x200a5, 0x200a6, 0x200a7,
0x200a8, 0x200a9, 0x200aa, 0x200ab, 0x200ac, 0x200ad, 0x200ae, 0x200af,
0x200b0, 0x200b1, 0x200b2, 0x200b3, 0x200b4,


// Cluster checksum local area, DON'T WRITE
0x200c8, 0x200c9, 0x200ca, 0x200cb, 0x200cc, 0x200cd,

0x50072,		// write CF_Master fingerprint

// Password local area
0xc3, 0xc4,

0x80008,		// compute checksum

// Stoneridge local calibration and production area
0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5,

0x80009,		// compute checksum

// Calculated values relocation area, DON'T WRITE
0x200e0, 0x200e1, 0x200e2, 0x200e3, 0x200e4, 0x200e5, 0x200e6,

// the Fingerprint area itself, DON'T WRITE
0x30098, 0x30099, 0x30070, 0x30071, 0x30072, 0x30073, 0x30074, 0x30075,
0x30076, 0x30077,

-1
};
*/
void CTLView::ProgCancel(void)
{
//   b_progcancel = TRUE;
//   m_cancel = NULL;
}


void CTLView::PTLogin_State_Process(void)
{
   BYTE buf[8];
   //CSetup setup;
   switch (ptlogin_state)
   {
	  case STATE1:
		 if (recbuf[0]!=0x7f)
		 {
			buf[0] = 0x01;
			buf[1] = 0x01;
			buf[2] = 0x20;
			buf[3] = 0x64;
			buf[4] = 0x04;
			buf[5] = 0x05;
			buf[6] = 0xFB;
			buf[7] = 0x11;
			kwpc_sendMsg(buf,8);
			ptlogin_state = STATE2;
		 }
		 else
		 {
			RespError("PTLogin","PTLogin error");
			KillTimer(gwID0);
			process_state=PROCESS_NONE;
		 }
		 break;

	  case STATE2:
		  if ((recbuf[0] == 0x81) && (recbuf[1] == 0x01))
		 {
			buf[0] = 0x10;
			buf[1] = 0x01;
			buf[2] = 0x40;
			buf[3] = 0x00;
			buf[4] = 0x00;
			buf[5] = 0xFF;
			buf[6] = 0xFF;
			buf[7] = 0xFF;
			kwpc_sendMsg(buf,8);
			ptlogin_state = STATE3;
		 }
		 else
			RespError("PTLogin","PTLogin error");
		 break;

	  case STATE3:
		  if ((recbuf[0] == 0x90) && (recbuf[1] == 0x01))
		 {
			KillTimer(gwID0);
			b_tmout0 = FALSE;
			ptlogin_state = STATE1;
			process_state=PROCESS_NONE;
			m_PTLogin_Flag = TRUE;
		 }
		 else
			RespError("PTLogin","STATE2");
		 break;


	  default:
		 break;
   }
}

BOOL CTLView::HWinit(void)
{
	BOOL err, ret;
	err = FALSE;
	ret = FALSE;
	if (!(hWnd=GetSafeHwnd()))	//Is need????
	   err = TRUE;
	
	else if (timeGetDevCaps(&tcaps,sizeof(tcaps)) != TIMERR_NOERROR)
	   err = TRUE;
	
	else if (timeBeginPeriod(tcaps.wPeriodMin) != TIMERR_NOERROR)  
	   err = TRUE;
	
	if (err)
		{
		ret = FALSE;
		}
	  
	if (InitCan())
		{
		can_enabled = TRUE;
		ret = TRUE;
		}
	else
		{
		ret = FALSE;
		}
	
	return ret;
}

BOOL CTLView::Check_Equipment_State(void)
{
	static UINT test_timeout_cnt = 0;
	//static BOOL ret = FALSE;

	if (test_timeout_cnt++ >= 2)
	{
		test_timeout_cnt = 0;

		if (FALSE == can_enabled)
		{
			m_CheckConnect = FALSE;
		}

		if (FALSE == m_OnDownloadState)
		{
			if (m_ConnectState == TRUE)
			{
				m_ConnectState = FALSE;
				m_CheckConnect = TRUE;
			}
			else
			{
				m_CheckConnect = FALSE;
			}
		}
		else
		{
			//Is downloading
			m_CheckConnect = FALSE;
		}
	}
	return m_CheckConnect;
}

//*

/**/
enum DOWNLOAD_RESULT CTLView::Check_DownloadResult(void)
{
	enum DOWNLOAD_RESULT ret;

	ret = m_DownloadResult;

	if (RESULT_NONE != m_DownloadResult)
		{
			m_DownloadResult = RESULT_NONE;
		}

	return ret;
}

void CTLView::SetDownloadClick(void)
{
	m_DownloadPataClick = TRUE;
}

BOOL CTLView::OnStateLogout() 
{
   BYTE buf[2] = {0x10, 0x81};
   BOOL ret = FALSE;
   
   pt_code = FALSE;

   if (process_state == PROCESS_NONE)
   {
      gwID0 = SetTimer(1,t_noresp,0);
      process_state = PROCESS_LOGIN;
      ret = kwpc_sendMsg(buf,2);
      login_state = STOP_DIAGNOSTIC_SESSION;
   }	

   return ret;
}

BOOL CTLView::OnStateLogin() 
{
	BYTE buf[2] = {0x10, 0x86};
	BOOL ret = FALSE;
	
	m_Login_Flag = FALSE;

	if (process_state == PROCESS_NONE)
	{
		buf[0] = 0x10;
		buf[1] = 0x86;

		gwID0 = SetTimer(1,t_noresp,0);
		process_state = PROCESS_LOGIN;
		ret = kwpc_sendMsg(buf,2);
		login_state = START_DIAGNOSTIC_SESSION;
	}
	
	return ret;
}
BOOL CTLView::OnStatePTLogin()
{
//Is a bug????   BYTE buf[7] = {0x31, 0xFA, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
	BYTE buf[8] = {0x31, 0xFA, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00};
   BOOL ret = FALSE;

   pt_code = FALSE;
   if (process_state == PROCESS_NONE)
   {
      gwID0 = SetTimer(1,t_noresp,0);
      process_state = PROCESS_PTLOGIN;
      ret = kwpc_sendMsg(buf,8);
      ptlogin_state = STATE1;
   }

   return ret;
}

BOOL CTLView::OnStateCmdData(BYTE *pData, BYTE data_length)
{
	BOOL ret = FALSE;
	
	if ((data_length > 8) || (NULL == pData))
		{
		}
	else
		{
		ret = kwpc_sendMsg(pData, data_length);
		}

	return ret;
}

BOOL CTLView::OnStateResetTarget(void)
{
	BYTE buf[8] = {0x01, 0x02, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
	BOOL ret = FALSE;

	ret = kwpc_sendMsg(buf,8);
	m_Logout_Flag = FALSE;
	m_Login_Flag = FALSE;
	m_PTLogin_Flag = FALSE;
	process_state = PROCESS_NONE;

	return ret;
}

void CTLView::Download_Process(void)
{
	static enum TRANSMIT_STATE Transmt_state = ON_IDLE;
	BOOL ret = FALSE;
	static BYTE resend_count = 0;

	switch (Transmt_state)
		{
		case ON_IDLE:
			if (TRUE == m_DownloadPataClick)
				{
				if (TRUE == m_CheckConnect)
					{
					resend_count = 0;
					m_SendCmdTimeoutCnt = 0;
					m_OnDownloadState = TRUE;
					m_Logout_Flag = FALSE;
					m_Login_Flag = FALSE;
					m_PTLogin_Flag = FALSE;
					m_Send_Cmd_State = SEND_WAIT;
					m_DownloadFalse = FALSE;
					m_DownloadResult = RESULT_NONE;

					ret = OnStateLogout();
					if (ret)
						{
						m_DownloadPataClick = FALSE;
						Transmt_state = ON_LOGOUT;
						}
					else
						{
						if (resend_count++ >= 5)
						{
							resend_count = 0;
							m_DownloadPataClick = FALSE;
							m_DownloadResult = RESULT_FALSE;
						}
						}
					}
				else
					{
					m_DownloadPataClick = FALSE;
					m_DownloadResult = RESULT_FALSE;
					}
				}
			break;

		case ON_LOGOUT:
			if (TRUE == m_Logout_Flag)
				{
				ret = OnStateLogin();
				if (ret)
					{
					Transmt_state = ON_LOGIN;
					}
				}
			else
				{
				if (TRUE == m_DownloadFalse)
					{
					//return error
					Transmt_state = ON_IDLE;
					}
				}
			break;

		case ON_LOGIN:
			if (TRUE == m_Login_Flag)
				{
				ret = OnStatePTLogin();
				if (ret)
					{
					Transmt_state = ON_PTLOGIN;
					}
				}
			else
				{
				if (TRUE == m_DownloadFalse)
					{
					//return error
					Transmt_state = ON_IDLE;
					}
				}
			break;

		case ON_PTLOGIN:
			if (TRUE == m_PTLogin_Flag)
				{
				Transmt_state = ON_CMD_1;
				}
			else
				{
				if (TRUE == m_DownloadFalse)
					{
					//return error
					Transmt_state = ON_IDLE;
					}
				}
			break;

		case ON_CMD_1:
			if (TRUE == m_CmdDataCheck1)
				{
				if (SEND_TRUE == m_Send_Cmd_State)
					{
					ret = OnStateCmdData(m_CmdData1, sizeof(m_CmdData1));
					if (ret)
						{
						Transmt_state = ON_CMD_2;
						m_Send_Cmd_State = SEND_WAIT;
						}
					}
				else
					{
					if ((SEND_FALSE == m_Send_Cmd_State) || (m_SendCmdTimeoutCnt++ >= 10000))
						{
						//return error
						m_SendCmdTimeoutCnt = 0;
						Transmt_state = ON_IDLE;
						m_DownloadResult = RESULT_FALSE;
						}
					}
				break;
				}
			else
				{
				Transmt_state = ON_CMD_2;
				}
			break;

		case ON_CMD_2:
			if (TRUE == m_CmdDataCheck2)
				{
				if (SEND_TRUE == m_Send_Cmd_State)
					{
					ret = OnStateCmdData(m_CmdData2, sizeof(m_CmdData2));
					if (ret)
						{
						Transmt_state = ON_CMD_3;
						m_Send_Cmd_State = SEND_WAIT;
						}
					}
				else
					{
					if ((SEND_FALSE == m_Send_Cmd_State) || (m_SendCmdTimeoutCnt++ >= 10000))
						{
						//return error
						m_SendCmdTimeoutCnt = 0;
						Transmt_state = ON_IDLE;
						m_DownloadResult = RESULT_FALSE;
						}
					}
				break;
				}
			else
				{
				Transmt_state = ON_CMD_3;
				}

		case ON_CMD_3:
			if (TRUE == m_CmdDataCheck3)
				{
				if (SEND_TRUE == m_Send_Cmd_State)
					{
					ret = OnStateCmdData(m_CmdData3, sizeof(m_CmdData3));
					if (ret)
						{
						Transmt_state = ON_CMD_4;
						m_Send_Cmd_State = SEND_WAIT;
						}
					}
				else
					{
					if ((SEND_FALSE == m_Send_Cmd_State) || (m_SendCmdTimeoutCnt++ >= 10000))
						{
						//return error
						m_SendCmdTimeoutCnt = 0;
						Transmt_state = ON_IDLE;
						m_DownloadResult = RESULT_FALSE;
						}
					}
				break;
				}
			else
				{
				Transmt_state = ON_CMD_4;
				}

		case ON_CMD_4:
			if (TRUE == m_CmdDataCheck4)
				{
				if (SEND_TRUE == m_Send_Cmd_State)
					{
					ret = OnStateCmdData(m_CmdData4, sizeof(m_CmdData4));
					if (ret)
						{
						Transmt_state = ON_CMD_5;
						m_Send_Cmd_State = SEND_WAIT;
						}
					}
				else
					{
					if ((SEND_FALSE == m_Send_Cmd_State) || (m_SendCmdTimeoutCnt++ >= 10000))
						{
						//return error
						m_SendCmdTimeoutCnt = 0;
						Transmt_state = ON_IDLE;
						m_DownloadResult = RESULT_FALSE;
						}
					}
				break;
				}
			else
				{
				Transmt_state = ON_CMD_5;
				}

		case ON_CMD_5:
			if (TRUE == m_CmdDataCheck5)
				{
				if (SEND_TRUE == m_Send_Cmd_State)
					{
					ret = OnStateCmdData(m_CmdData5, sizeof(m_CmdData5));
					if (ret)
						{
						Transmt_state = ON_CMD_6;
						m_Send_Cmd_State = SEND_WAIT;
						}
					}
				else
					{
					if ((SEND_FALSE == m_Send_Cmd_State) || (m_SendCmdTimeoutCnt++ >= 10000))
						{
						//return error
						m_SendCmdTimeoutCnt = 0;
						Transmt_state = ON_IDLE;
						m_DownloadResult = RESULT_FALSE;
						}
					}
				break;
				}
			else
				{
				Transmt_state = ON_CMD_6;
				}

		case ON_CMD_6:
			if (TRUE == m_CmdDataCheck6)
				{
				if (SEND_TRUE == m_Send_Cmd_State)
					{
					ret = OnStateCmdData(m_CmdData6, sizeof(m_CmdData6));
					if (ret)
						{
						Transmt_state = ON_CMD_7;
						m_Send_Cmd_State = SEND_WAIT;
						}
					}
				else
					{
					if ((SEND_FALSE == m_Send_Cmd_State) || (m_SendCmdTimeoutCnt++ >= 10000))
						{
						//return error
						m_SendCmdTimeoutCnt = 0;
						Transmt_state = ON_IDLE;
						m_DownloadResult = RESULT_FALSE;
						}
					}
				break;
				}
			else
				{
				Transmt_state = ON_CMD_7;
				}

		case ON_CMD_7:
			if (TRUE == m_CmdDataCheck7)
				{
				if (SEND_TRUE == m_Send_Cmd_State)
					{
					ret = OnStateCmdData(m_CmdData7, sizeof(m_CmdData7));
					if (ret)
						{
						Transmt_state = ON_CMD_8;
						m_Send_Cmd_State = SEND_WAIT;
						}
					}
				else
					{
					if ((SEND_FALSE == m_Send_Cmd_State) || (m_SendCmdTimeoutCnt++ >= 10000))
						{
						//return error
						m_SendCmdTimeoutCnt = 0;
						Transmt_state = ON_IDLE;
						m_DownloadResult = RESULT_FALSE;
						}
					}
				break;
				}
			else
				{
				Transmt_state = ON_CMD_8;
				}

		case ON_CMD_8:
			if (TRUE == m_CmdDataCheck8)
				{
				if (SEND_TRUE == m_Send_Cmd_State)
					{
					ret = OnStateCmdData(m_CmdData8, sizeof(m_CmdData8));
					if (ret)
						{
						Transmt_state = ON_CMD_9;
						m_Send_Cmd_State = SEND_WAIT;
						}
					}
				else
					{
					if ((SEND_FALSE == m_Send_Cmd_State) || (m_SendCmdTimeoutCnt++ >= 10000))
						{
						//return error
						m_SendCmdTimeoutCnt = 0;
						Transmt_state = ON_IDLE;
						m_DownloadResult = RESULT_FALSE;
						}
					}
				break;
				}
			else
				{
				Transmt_state = ON_CMD_9;
				}

		case ON_CMD_9:
			if (TRUE == m_CmdDataCheck9)
				{
				if (SEND_TRUE == m_Send_Cmd_State)
					{
					ret = OnStateCmdData(m_CmdData9, sizeof(m_CmdData9));
					if (ret)
						{
						Transmt_state = ON_CMD_10;
						m_Send_Cmd_State = SEND_WAIT;
						}
					}
				else
					{
					if ((SEND_FALSE == m_Send_Cmd_State) || (m_SendCmdTimeoutCnt++ >= 10000))
						{
						//return error
						m_SendCmdTimeoutCnt = 0;
						Transmt_state = ON_IDLE;
						m_DownloadResult = RESULT_FALSE;
						}
					}
				break;
				}
			else
				{
				Transmt_state = ON_CMD_10;
				}

		case ON_CMD_10:
			if (TRUE == m_CmdDataCheck10)
				{
				if (SEND_TRUE == m_Send_Cmd_State)
					{
					ret = OnStateCmdData(m_CmdData10, sizeof(m_CmdData10));
					if (ret)
						{
						Transmt_state = ON_CMD_RESET;
						m_Send_Cmd_State = SEND_WAIT;
						}
					}
				else
					{
					if ((SEND_FALSE == m_Send_Cmd_State) || (m_SendCmdTimeoutCnt++ >= 10000))
						{
						//return error
						m_SendCmdTimeoutCnt = 0;
						Transmt_state = ON_IDLE;
						m_DownloadResult = RESULT_FALSE;
						}
					}
				break;
				}
			else
				{
				Transmt_state = ON_CMD_RESET;
				}

		case ON_CMD_RESET:
			if (TRUE == m_CmdResetCheck)
				{
				if (SEND_TRUE == m_Send_Cmd_State || TRUE == m_PTLogin_Flag)
					{
					ret = OnStateResetTarget();
					if (ret)
						{
						m_OnDownloadState = FALSE;
						m_Send_Cmd_State = SEND_WAIT;
						m_DownloadResult = RESULT_SUCC;
						// download parameter is successfully!!!
						Transmt_state = ON_IDLE;
						}
					}
				else
					{
					if ((SEND_FALSE == m_Send_Cmd_State) || (m_SendCmdTimeoutCnt++ >= 10000))
						{
						//return error
						m_SendCmdTimeoutCnt = 0;
						Transmt_state = ON_IDLE;
						m_OnDownloadState = FALSE;
						m_DownloadResult = RESULT_FALSE;
						}
					}
				}
			else
				{
				m_SendCmdTimeoutCnt = 0;
				m_OnDownloadState = FALSE;
				m_Logout_Flag = FALSE;
				m_Login_Flag = FALSE;
				m_PTLogin_Flag = FALSE;
				m_Send_Cmd_State = SEND_WAIT;
				m_DownloadFalse = FALSE;
				m_DownloadResult = RESULT_SUCC;

				// download parameter is successfully!!!
				Transmt_state = ON_IDLE;
				}
			break;

		default:
			//return error
			Transmt_state = ON_IDLE;
			break;
		}
}

ULONG getULong(void)
{
	int i;
	ULONG ul = 0;

	for (i = 0; i<8; i += 2)
		ul = (ul << 8) | getByte(i);

	return ul;
}

int getByte(int n)
{
	int i;

	i = atoh(ascbuf[n]) << 4;
	i += atoh(ascbuf[n + 1]);
	return i;
}

int atoh(char bb)
{
	char btemp;

	btemp = bb;

	if (btemp >= 'a')
		btemp -= 0x20;

	btemp -= '0';

	if (btemp > 9)
		btemp -= 7;

	return btemp;
}

//void getDate(void)
//{
//	CTime theTime = CTime::GetCurrentTime();
//
//	sprintf_s(datebuf, "%02d%02d%02d",
//		theTime.GetYear() % 100, theTime.GetMonth(), theTime.GetDay());
//}

int HexToBin(char *name, BYTE *cmd)
{
	char c;
	int i, j = 0;

	for (i = 0; i<(int)strlen(name); i++)
	{
		c = name[i];

		if (c == ' ')
			continue;

		if (c >= 'a')
			c -= 0x20;

		if ((c >= '0') && (c <= 'F'))
			ascbuf[j++] = c;

		else
			return 1;
	}

	ascbuf[j] = 0;
	j = strlen(ascbuf);

	if (j % 2)
		return 2;

	j /= 2;

	for (i = 0; i<j; i++)
		cmd[i] = getByte(2 * i);

	//kwpc_sendMsg(databuf, j);
	return 0;
}

void CTLView::OnTimer(UINT_PTR nIDEvent)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	   if (nIDEvent == gwID0)
	   {
	      KillTimer(gwID0);
	//      m_log.EnableWindow(TRUE);
	
	      if (m_cancel)
	      {
	         m_cancel->DestroyWindow();
	         m_cancel = NULL;
	      }
	
	//      CancelOperation();
	      b_tmout0 = TRUE;
	   }
	
	   else if (nIDEvent == gwID1)
	      b_tmout1 = TRUE;
	   else if (nIDEvent == checkTimerID)
	   {
		   DOWNLOAD_RESULT tmp = Check_DownloadResult();
		   switch (tmp)
		   {
		   case 1:
			   MessageBoxA(_T("下载成功"));
			   KillTimer(checkTimerID);
			   break;
		   case 2:
			   MessageBoxA(_T("下载失败"));
			   KillTimer(checkTimerID);
			   break;
		   }
	   }
	   else if (nIDEvent == checkConetState)
	   {
		   m_hIcon_green = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICON_GREEN));
		   m_hIcon_red = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICON_RED));
		   m_hIcon_gray = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICON_GRAY));
		   if (Check_Equipment_State())
		   {
			   m_hicon_green.SetIcon(m_hIcon_green);
			   m_hicon_red.SetIcon(m_hIcon_gray);
		   }
		   else
		   {
			   m_hicon_green.SetIcon(m_hIcon_gray);
			   m_hicon_red.SetIcon(m_hIcon_red);
		   }
	   }
	CFormView::OnTimer(nIDEvent);
}
