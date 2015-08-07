
// TLView.h : CTLView 类的接口
//

#if !defined(AFX_TLVIEW_H__5452FFC5_B232_11D2_99ED_00104B168C9F__INCLUDED_)
#define AFX_TLVIEW_H__5452FFC5_B232_11D2_99ED_00104B168C9F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// TLView form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

//#include "resource.h"
//#include "afxwin.h"

enum KWP_STATE
{
	KWP_IDLE,
	WAIT_FOR_CONF,
	WAIT_FOR_RESP,
	WAIT_FOR_RESP_COMPL,
	WAIT_FOR_BUF_UNLOCK
};

enum USDT_STATE
{
	USDT_IDLE,
	WAIT_FOR_FC,
	WAIT_FOR_STMIN,
	RECEIVE_CF
};

enum USDT_PENDING
{
	PEND_NONE,
	PEND_CONF,
	PEND_FF,
	PEND_BUF
};

enum USDT_ERROR
{
	ERR_NONE,
	ERR_TIMEOUT_AS,
	ERR_TIMEOUT_AR,
	ERR_TIMEOUT_BS,
	ERR_TIMEOUT_CR,
	ERR_WRONG_SN,
	ERR_UNEXP_PDU
};

enum PROCESS_STATE
{
   PROCESS_NONE,
   PROCESS_LOGIN,
   PROCESS_PTLOGIN
};

enum LOGIN_STATE
{
	LOGIN_IDLE,
	START_DIAGNOSTIC_SESSION,
	SECURITY_ACCESS_1,
	SECURITY_ACCESS_2,
	STOP_DIAGNOSTIC_SESSION
};
enum PTLOGIN_STATE
{
	STATE1,
	STATE2,
	STATE3
};

enum TRANSMIT_STATE
{
	ON_IDLE,
	ON_LOGOUT,
	ON_LOGIN,
	ON_PTLOGIN,
	ON_CMD_1,
	ON_CMD_2,
	ON_CMD_3,
	ON_CMD_4,
	ON_CMD_5,
	ON_CMD_6,
	ON_CMD_7,
	ON_CMD_8,
	ON_CMD_9,
	ON_CMD_10,
	ON_CMD_RESET
};

enum SEND_CMD_STATE
{
	SEND_WAIT,
	SEND_TRUE,
	SEND_FALSE
};

enum DOWNLOAD_RESULT
{
	RESULT_NONE,
	RESULT_SUCC,
	RESULT_FALSE
};

#define BUF_SIZE_KWP        256
#define BAUD_1M        (-1)
#define BAUD_500K      (-2)
#define BAUD_250K      (-3)
#define BAUD_125K      (-4)
#define BAUD_100K      (-5)
#define BAUD_62K       (-6)
#define BAUD_50K       (-7)

typedef unsigned long U32;

/** Seed/key data, 64 bits. */
typedef union {
   struct {
      U32 high; ///< Upper 32 bits of seed or key
      U32 low;  ///< Lower 32 bits of seed or key
   } part;
   U32 data[2]; ///< 64 bits seed or key
} keyDataT;

class CTLView : public CFormView
{
protected: // 仅从序列化创建
	CTLView();
	DECLARE_DYNCREATE(CTLView)

public:
	enum{ IDD = IDD_TL_FORM };

// 特性
public:
	CTLDoc* GetDocument() const;
	//zwj
	CString m_excel_file;

private:
	HWND hWnd;

	BOOL b_tmoutmm0=FALSE, b_tmoutmm1=FALSE, b_tmout0=FALSE, b_tmout1=FALSE, can_enabled=FALSE;
	BOOL b_tripmm0=FALSE, b_actmm0=FALSE, b_tripmm1=FALSE, b_actmm1=FALSE;
	BOOL b_loggedin=FALSE;
	BYTE trabuf[256], recbuf[256];

	UINT gwID0, gwID1;
	int cs;
	int n_as = 1000, n_bs = 1000, n_cr = 1000, n_stmin = 0, tra_bs = 5;
	int t_noresp = 7000, t_tp = 4000, t_p2 = 25, t_p2long = 5000;

	char tbuf[100];

	//={0x9A355006, 0xEC9D3E06, 0x584A8306, 0x943F9B06};


	BYTE can_trabuf[8], can_recbuf[8], *trabufp, *recbufp;
	int tra_len, rec_len, rec_lencur, tseq, rseq, bs, bscur, BScur, stmin;
	int can_tralen;


	BYTE kwp_buf[BUF_SIZE_KWP];
	int kwp_len;
	BYTE tp[2];
	//={0x3e, 0x00};		// TesterPresent


	BOOL pt_code = FALSE;

	UINT can_reclen;

	enum KWP_STATE kwp_state=KWP_IDLE;
	enum USDT_STATE usdt_state=USDT_IDLE;
	enum USDT_PENDING usdt_pending=PEND_NONE;
	enum USDT_ERROR usdt_error=ERR_NONE;
	enum PROCESS_STATE process_state=PROCESS_NONE;
	enum LOGIN_STATE login_state=LOGIN_IDLE;
	enum PTLOGIN_STATE ptlogin_state=STATE1;

	BOOL m_Logout_Flag = FALSE;
	BOOL m_Login_Flag = FALSE;
	BOOL m_PTLogin_Flag = FALSE;
	enum SEND_CMD_STATE m_Send_Cmd_State = SEND_FALSE;
	BOOL m_OnDownloadState = FALSE;
	BOOL m_ConnectState = FALSE;
	BOOL m_DownloadFalse = FALSE;
	enum DOWNLOAD_RESULT m_DownloadResult = RESULT_NONE;
	BOOL m_DownloadPataClick = FALSE;
	BOOL m_CheckConnect = FALSE;

	UINT m_SendCmdTimeoutCnt = 0;

	UINT checkTimerID = 3;
	UINT checkConetState = 4;
	HICON m_hIcon_green;
	HICON m_hIcon_red;
	HICON m_hIcon_gray;
	UINT paravalue[10];
	//char *iniFilePath = "C:\\mycode\\TL\\TL\\TL\\Debug\\TL.ini";
	char iniFilePath[1024];
	BOOL hasImportPara;
	SYSTEMTIME st;
	char logbuf[13000];
	
public:
	int baud_rate_kvaser = BAUD_250K;
	ULONG TraID=0x18da1700;
	ULONG RecID=0x18da0017;
	ULONG CheckID = 0x0CFE6CEE;

	UINT can_channel = 0;
	UINT can_hardware = 0;

	//Cmd
	BYTE m_CmdData1[8];
	//= {0x07, 0x01, 0x0A, 0x08, 0x00, 0x00, 0x00, 0x00};
	BYTE m_CmdData2[8];
	//= {0x07, 0x01, 0x0A, 0x0C, 0x00, 0x00, 0x00, 0x00};
	BYTE m_CmdData3[8];
	//= {0x07, 0x01, 0x0A, 0x08, 0x00, 0x00, 0x00, 0x00};
	BYTE m_CmdData4[8];
	//= {0x07, 0x01, 0x0A, 0x0C, 0x00, 0x00, 0x00, 0x00};
	BYTE m_CmdData5[8];
	//= {0x07, 0x01, 0x0A, 0x08, 0x00, 0x00, 0x00, 0x00};
	BYTE m_CmdData6[8];
	//= {0x07, 0x01, 0x0A, 0x0C, 0x00, 0x00, 0x00, 0x00};
	BYTE m_CmdData7[8];
	//= {0x07, 0x01, 0x0A, 0x08, 0x00, 0x00, 0x00, 0x00};
	BYTE m_CmdData8[8];
	//= {0x07, 0x01, 0x0A, 0x0C, 0x00, 0x00, 0x00, 0x00};
	BYTE m_CmdData9[8];
	//= {0x07, 0x01, 0x0A, 0x08, 0x00, 0x00, 0x00, 0x00};
	BYTE m_CmdData10[8];
	//={0x60, 0x01, 0x08, 0x43, 0xA0, 0x0F, 0x00, 0x00};

	//Cmd is valid set this true
	BOOL m_CmdDataCheck1 = FALSE;
	BOOL m_CmdDataCheck2 = FALSE;
	BOOL m_CmdDataCheck3 = FALSE;
	BOOL m_CmdDataCheck4 = FALSE;
	BOOL m_CmdDataCheck5 = FALSE;
	BOOL m_CmdDataCheck6 = FALSE;
	BOOL m_CmdDataCheck7 = FALSE;
	BOOL m_CmdDataCheck8 = FALSE;
	BOOL m_CmdDataCheck9 = FALSE;
	BOOL m_CmdDataCheck10 = FALSE;
	BOOL m_CmdResetCheck = TRUE;
	U32 key_supplier[4];
	
// Operations
//public:
//	BOOL OnToolTipNotify(UINT id, NMHDR *pNMHDR, LRESULT *pResult);


// Overrides
//	public:
//	virtual void OnInitialUpdate();
//	protected:
//	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
private:
	void SetMMTimer(int tn, int timeout);
	void KillMMTimer(int tn);
	BOOL InitCan(void);
	void CanError(int stat, char *b);
	BOOL kwpc_sendMsg(BYTE *buf, int len);
	BOOL usdt_dataReq(BYTE *buf,int len);
	int PutCanFrame(void);
	BOOL GetCanFrame(void);
	void LogDisp(int ind, BYTE *b, int l);
	void ProcessMessage(void);
	void CalculateKey(const keyDataT *seed, keyDataT *key);
	void Encrypt(const U32 data_in[2], U32 data_out[2], const U32 key[4]);
	void RespError(char *title, char *service);
	
	BOOL KWP_State_Process(void);
	void USDT_State_Process(void);
	void Login_State_Process(void);
	void PTLogin_State_Process(void);
	void ProgCancel(void);
	BOOL OnStateLogout(void);
	BOOL OnStateLogin(void);
	BOOL OnStatePTLogin(void);
	BOOL OnStateResetTarget(void);
	BOOL OnStateCmdData(BYTE *pData, BYTE data_length);
	void Download_Process(void);

public:
	void ExitCan(void);
	BOOL HWinit(void);
	void Execute_Processes(void);
	BOOL Check_Equipment_State(void);
	enum DOWNLOAD_RESULT Check_DownloadResult(void);
	void SetDownloadClick(void);

// 操作
public:

// 重写
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual void OnInitialUpdate(); // 构造后第一次调用

// 实现
public:
	virtual ~CTLView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedApplicationBrowse();
	afx_msg void OnBnClickedApplicationImport();
//	CComboBox m_combotype;
	afx_msg void OnSelchangeComboProductType();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	CComboBox m_combotype;
	afx_msg void OnBnClickedButtonDownload();
	CStatic m_hicon_green;
	CStatic m_hicon_red;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	BOOL m_resetcheck;
	CString m_product_id;
	CString m_product_type;
	CRichEditCtrl m_log;
};

#ifndef _DEBUG  // TLView.cpp 中的调试版本
inline CTLDoc* CTLView::GetDocument() const
   { return reinterpret_cast<CTLDoc*>(m_pDocument); }
#endif

#endif
#include "afxwin.h"
#include "afxcmn.h"
