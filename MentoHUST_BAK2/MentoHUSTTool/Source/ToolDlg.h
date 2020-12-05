// ToolDlg.h : header file
//

#if !defined(AFX_TOOLDLG_H__14DAB80D_8330_4197_B222_0E3723ACA107__INCLUDED_)
#define AFX_TOOLDLG_H__14DAB80D_8330_4197_B222_0E3723ACA107__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "HyperLink.h"
#include "StringList.h"

#define ID_STATE_FREE							(WM_USER+100)
#define ID_STATE_START							(WM_USER+101)
#define ID_STATE_MD5							(WM_USER+101)
#define ID_STATE_SUCCESS						(WM_USER+102)

UINT CatchThreadFunc(LPVOID pParam);	// 抓包线程
/////////////////////////////////////////////////////////////////////////////
// CToolDlg dialog

class CToolDlg : public CDialog
{
// Construction
public:
	CToolDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CToolDlg)
	enum { IDD = IDD_MENTOHUSTTOOL_DIALOG };
	CHyperLink	m_LogoLink;
	CComboBox	m_AdapterList;
	BOOL	m_b8021x;
	BOOL	m_bW32N55;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CToolDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CToolDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	virtual void OnOK();
	afx_msg void OnBnAbout();
	afx_msg void OnBnHelp();
	afx_msg void OnBnStart();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	StringList	*m_pAdapterList;	// 网卡列表
	CWinThread	*m_pCatchThread;	// 抓包线程
	CString Ruijie;
	BYTE	*m_pBuffer;
	DWORD	m_nSize;
	
	BOOL InitAdapterList();			// 初始化网卡列表
	BOOL OpenAdapter();				// 打开选择的网卡
	BOOL CheckStart();				// 开始前的检查
	CString GetRuijiePath(bool reg);
	BYTE *ReadCode(CString file, DWORD *size);
	void StopCatch();				// 停止抓包
	void SaveData();				// 保存结果

public:
	int			m_iState;			// 抓包状态
	pcap_t		*m_pAdapter;		// 网卡句柄
	u_char		m_btPacket[0x400];	// 抓包
	void MessageOK(LPCTSTR lpszText);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TOOLDLG_H__14DAB80D_8330_4197_B222_0E3723ACA107__INCLUDED_)
