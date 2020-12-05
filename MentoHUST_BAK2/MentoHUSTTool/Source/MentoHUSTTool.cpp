// MentoHUSTTool.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "MentoHUSTTool.h"
#include "ToolDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CToolApp

BEGIN_MESSAGE_MAP(CToolApp, CWinApp)
	//{{AFX_MSG_MAP(CToolApp)
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CToolApp construction

CToolApp::CToolApp()
{
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CToolApp object

CToolApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CToolApp initialization

BOOL CToolApp::InitInstance()
{
	// Standard initialization

	CToolDlg dlg;
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

CString LoadString(UINT nID)
{
	CString strTemp;
	strTemp.LoadString(nID);
	return strTemp;
}

CString GetAppPath()
{
	CString strPath;
	GetModuleFileName(AfxGetInstanceHandle(), strPath.GetBuffer(MAX_PATH), MAX_PATH);
	strPath.ReleaseBuffer();
	return strPath.Left(strPath.ReverseFind('\\')+1);
}
