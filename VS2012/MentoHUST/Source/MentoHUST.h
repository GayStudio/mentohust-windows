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

CString LoadString(UINT nID);			//������Դ�ַ���
CString GetAppPath();					//��ȡ��������Ŀ¼
CString ConvertToUTF8(LPCTSTR strSrc);	//ת��ΪUTF8
int EncodeRuijie(char *dst, const char *src);	//�����������
int DecodeRuijie(char *dst, const char *src);	//��������û�������
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
