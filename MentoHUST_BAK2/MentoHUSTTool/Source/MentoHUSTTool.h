// MentoHUSTTool.h : main header file for the MENTOHUSTTOOL application
//

#if !defined(AFX_MENTOHUSTTOOL_H__935BB182_6B3B_4DD2_8D57_49ABDFD43D54__INCLUDED_)
#define AFX_MENTOHUSTTOOL_H__935BB182_6B3B_4DD2_8D57_49ABDFD43D54__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

CString LoadString(UINT nID);		//返回资源字符串
CString GetAppPath();				//获取程序所在目录
/////////////////////////////////////////////////////////////////////////////
// CToolApp:
// See MentoHUSTTool.cpp for the implementation of this class
//

class CToolApp : public CWinApp
{
public:
	CToolApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CToolApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CToolApp)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MENTOHUSTTOOL_H__935BB182_6B3B_4DD2_8D57_49ABDFD43D54__INCLUDED_)
