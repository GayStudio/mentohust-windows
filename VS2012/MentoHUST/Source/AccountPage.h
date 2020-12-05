#if !defined(AFX_ACCOUNTPAGE_H__87B68A37_934F_4D5D_80FF_ADBA8403B17D__INCLUDED_)
#define AFX_ACCOUNTPAGE_H__87B68A37_934F_4D5D_80FF_ADBA8403B17D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AccountPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAccountPage dialog

class CAccountPage : public CDialog
{
// Construction
public:
	CAccountPage(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAccountPage)
	enum { IDD = IDD_CFG_ACCOUNT };
	CListCtrl	m_ListCtrl;
	CIPAddressCtrl	m_IPCtrl;
	CString	m_sPassword;
	CString	m_sUsername;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAccountPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAccountPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnBnAdd();
	afx_msg void OnBnDel();
	afx_msg void OnOK();
	afx_msg void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
		
private:
	void InitListCtrl();	//初始化ListCtrl控件
	void ReadConfig();		//读取配置文件

	BOOL m_bChanged;		//是否改变

public:
	BOOL WriteConfig();			//写入配置文件
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ACCOUNTPAGE_H__87B68A37_934F_4D5D_80FF_ADBA8403B17D__INCLUDED_)
