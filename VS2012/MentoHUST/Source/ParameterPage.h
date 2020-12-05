#if !defined(AFX_PARAMETERPAGE_H__0D8356C4_5D37_438E_A112_14704C9BC23B__INCLUDED_)
#define AFX_PARAMETERPAGE_H__0D8356C4_5D37_438E_A112_14704C9BC23B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ParameterPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CParameterPage dialog

class CParameterPage : public CDialog
{
// Construction
public:
	CParameterPage(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CParameterPage)
	enum { IDD = IDD_CFG_PARAMETER };
	BOOL	m_bAutoRun;
	BOOL	m_bAutoCert;
	BOOL	m_bAutoMin;
	BOOL	m_bBandMac;
	BOOL	m_bUsePackage;
	int		m_iStartMode;
	int		m_iDhcpMode;
	UINT	m_nTimeout;
	UINT	m_nEchoTime;
	CString	m_sFilePath;
	UINT	m_nReconnect;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CParameterPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CParameterPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnBnBrowser();
	afx_msg void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	void SetAutoRun(BOOL bAutoRun);		//开机自动运行？
	void ReadConfig();		//读取配置
		
public:
	BOOL WriteConfig();		//写入配置文件
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PARAMETERPAGE_H__0D8356C4_5D37_438E_A112_14704C9BC23B__INCLUDED_)
