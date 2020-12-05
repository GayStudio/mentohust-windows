// MHPCDlg.h : header file
//

#if !defined(AFX_MHPCDLG_H__353DCC44_4D61_4BA0_8D56_A5F503A71F71__INCLUDED_)
#define AFX_MHPCDLG_H__353DCC44_4D61_4BA0_8D56_A5F503A71F71__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StringList.h"

#define ID_STATE_FREE							(WM_USER+100)
#define ID_STATE_START							(WM_USER+101)
#define ID_STATE_SUCCESS						(WM_USER+102)

UINT CatchThreadFunc(LPVOID pParam);
/////////////////////////////////////////////////////////////////////////////
// CMHPCDlg dialog

class CMHPCDlg : public CDialog
{
// Construction
public:
	CMHPCDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CMHPCDlg)
	enum { IDD = IDD_MHPC_DIALOG };
	CComboBox	m_AdapterList;
	UINT	m_uDeadline;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMHPCDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CMHPCDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnDestroy();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	BOOL InitAdapterList();			//初始化网卡列表
	BOOL OpenAdapter();				//打开选择的网卡
	CString GetRuijiePath(bool reg);
	BOOL ReadRuijie(LPCTSTR Ruijie);
	void SavePackage();
	
	StringList *m_pAdapterList;
	CWinThread *m_pCatchThread;
	
public:
	BYTE *m_pBuffer;
	int m_nReadSzie;
	pcap_t	*m_pAdapter;
	int m_iState;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MHPCDLG_H__353DCC44_4D61_4BA0_8D56_A5F503A71F71__INCLUDED_)
