// MHPC.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "MHPC.h"
#include "MHPCDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMHPCApp

BEGIN_MESSAGE_MAP(CMHPCApp, CWinApp)
	//{{AFX_MSG_MAP(CMHPCApp)
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMHPCApp construction

CMHPCApp::CMHPCApp()
{
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CMHPCApp object

CMHPCApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CMHPCApp initialization

BOOL CMHPCApp::InitInstance()
{
	// Standard initialization

	CMHPCDlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
	}
	else if (nResponse == IDCANCEL)
	{
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
