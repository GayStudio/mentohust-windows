// MentoHUST.h : main header file for the MENTOHUST application
//

#if !defined(AFX_MENTOHUST_H__19FA2B9C_36DD_4197_BF15_730D09B35C45__INCLUDED_)
#define AFX_MENTOHUST_H__19FA2B9C_36DD_4197_BF15_730D09B35C45__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

CString LoadString(UINT nID);			//返回资源字符串
CString GetAppPath();					//获取程序所在目录
CString ConvertToUTF8(LPCTSTR strSrc);	//转换为UTF8
int EncodeRuijie(char *dst, const char *src);	//加密锐捷密码
int DecodeRuijie(char *dst, const char *src);	//解密锐捷用户名密码
/////////////////////////////////////////////////////////////////////////////
// CMentoHUSTApp:
// See MentoHUST.cpp for the implementation of this class
//

class CMentoHUSTApp : public CWinApp
{
public:
	CMentoHUSTApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMentoHUSTApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CMentoHUSTApp)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MENTOHUST_H__19FA2B9C_36DD_4197_BF15_730D09B35C45__INCLUDED_)
