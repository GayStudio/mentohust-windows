// MHPC.h : main header file for the MHPC application
//

#if !defined(AFX_MHPC_H__3DE35DFF_1AA8_4264_B9EC_96BAE0AECAA4__INCLUDED_)
#define AFX_MHPC_H__3DE35DFF_1AA8_4264_B9EC_96BAE0AECAA4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CMHPCApp:
// See MHPC.cpp for the implementation of this class
//

class CMHPCApp : public CWinApp
{
public:
	CMHPCApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMHPCApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CMHPCApp)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MHPC_H__3DE35DFF_1AA8_4264_B9EC_96BAE0AECAA4__INCLUDED_)
