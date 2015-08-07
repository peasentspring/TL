
// TLView.h : CTLView 类的接口
//

#pragma once

#include "resource.h"
#include "afxwin.h"


class CTLView : public CFormView
{
protected: // 仅从序列化创建
	CTLView();
	DECLARE_DYNCREATE(CTLView)

	int m_selhw;

public:
	enum{ IDD = IDD_TL_FORM };

// 特性
public:
	CTLDoc* GetDocument() const;
	//zwj
//	CString m_excel_file;

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
//	afx_msg void OnBnClickedApplicationBrowse();
//	afx_msg void OnBnClickedApplicationImport();
//	CComboBox m_combotype;
//	afx_msg void OnSelchangeComboProductType();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
//	CComboBox m_combotype;
	afx_msg void OnBnClickedOk();
//	int m_baud_rate;
	int m_baud_rate;
	int m_can_chan;
	CComboBox m_can_hardware_ctrl;
	CString m_can_hardware;
	CString m_can_recid;
	CString m_can_traid;
	BOOL m_check1;
	BOOL m_check2;
	BOOL m_check3;
	BOOL m_check4;
	BOOL m_check5;
	BOOL m_check6;
	BOOL m_check7;
	BOOL m_check8;
	BOOL m_check9;
	BOOL m_check10;
	CString m_order1;
	CString m_order2;
	CString m_order3;
	CString m_order4;
	CString m_order5;
	CString m_order6;
	CString m_order7;
	CString m_order8;
	CString m_order9;
	CString m_order10;
	CEdit m_access_password5_ctrl;
	CString m_access_password5;
	CEdit m_access_password6_ctrl;
	CString m_access_password6;
	CEdit m_access_password7_ctrl;
	CString m_access_password7;
	CEdit m_access_password8_ctrl;
	CString m_access_password8;
	CEdit m_product_id_ctrl;
	CString m_product_id;
	CEdit m_product_type_ctrl;
	CString m_product_type;
	afx_msg void OnBnClickedCancel();

private:
	BOOL *checklist[10];
	CString *orderlist[10];
//	char iniFilePath = "C:\\TL.ini";
	char iniFilePath[1024];
public:
	CString m_checkid;
	BOOL m_reboot;
};

#ifndef _DEBUG  // TLView.cpp 中的调试版本
inline CTLDoc* CTLView::GetDocument() const
   { return reinterpret_cast<CTLDoc*>(m_pDocument); }
#endif

