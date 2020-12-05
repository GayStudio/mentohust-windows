// MentoHUSTDlg.h : header file
//

#if !defined(AFX_MENTOHUSTDLG_H__19F6D5BF_21C1_4D7F_9717_417107227C55__INCLUDED_)
#define AFX_MENTOHUSTDLG_H__19F6D5BF_21C1_4D7F_9717_417107227C55__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "HyperLink.h"
#include "Process.h"
#include "StringList.h"

#define SHOW_TIMER			(WM_USER+101)
#define SEND_TIMER			(WM_USER+102)
#define TEST_TIMER			(WM_USER+103)
#define MEM_TIMER			(WM_USER+104)
#define END_CERT			(WM_USER+105)

const LPCTSTR GUI_ID = _T("MentoHUST-8BDAA526-CBE4-4797-8D3E-E63C8D362894");
const UINT WM_TASKBARCREATED = ::RegisterWindowMessage(_T("TaskbarCreated"));
/////////////////////////////////////////////////////////////////////////////
// CMentoHUSTDlg dialog

class CMentoHUSTDlg : public CDialog
{
// Construction
public:
	CMentoHUSTDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CMentoHUSTDlg)
	enum { IDD = IDD_MENTOHUST_DIALOG };
	CHyperLink	m_linkLogo;
	CComboBox	m_AdapterList;
	CComboBox	m_AccountList;
	CString	m_sStateArea;
	CString	m_sStateBoard;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMentoHUSTDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CMentoHUSTDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBnAbout();
	afx_msg void OnBnClear();
	afx_msg void OnBnHelp();
	afx_msg void OnBnOption();
	afx_msg void OnBnOutput();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	int ShowDlg(CDialog *dlg);	//显示“关于”和“设置”对话框
	void MyInitDialog();		//初始化对话框
	void InitInterface();		//初始化界面
	void ReadConfig();			//读取配置
	void InitTrayIcon();		//初始化托盘图标
	void InitAccountList();		//初始化账号列表
	void InitAdapterList();		//初始化网卡列表
	BOOL ReadRuijieAccount(CString &strUsername);	//读取锐捷注册表账号
	BOOL GetAccount();			//获取用户名密码IP
	BOOL GetAdapter();			//打开选择的网卡
	void SaveSelect();			//保存选择的账号和网卡
	void ToTray(int nType);		//最小化到托盘
	
	int		m_iBigHeight;		//完整对话框高度
	int		m_iSmallHeight;		//隐藏部分后高度
	BOOL	m_bChanged;			//参数是否改变
	BOOL	m_bAutoCert;		//运行后自动认证？
	CProcess m_Process;			//认证过程
	NOTIFYICONDATA	m_tnid;		//托盘图标
	StringList	*m_pAccountList;//账号列表
	StringList	*m_pAdapterList;//网卡列表

public:
	int MsgBox(LPCTSTR lpszText, UINT nType = MB_OK|MB_ICONWARNING);	//消息提示
	void Output(LPCTSTR Msg, int Type = 0);	//输出状态
	void ChangeTrayIcon();		//改变托盘图标

	CString m_sServerMsg;	//服务器消息
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MENTOHUSTDLG_H__19F6D5BF_21C1_4D7F_9717_417107227C55__INCLUDED_)
