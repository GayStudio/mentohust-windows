// ParameterPage.cpp : implementation file
//

#include "stdafx.h"
#include "MentoHUST.h"
#include "ParameterPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CParameterPage dialog


CParameterPage::CParameterPage(CWnd* pParent /*=NULL*/)
	: CDialog(CParameterPage::IDD, pParent)
{
	//{{AFX_DATA_INIT(CParameterPage)
	m_bAutoRun = FALSE;
	m_bAutoCert = FALSE;
	m_bAutoMin = FALSE;
	m_bBandMac = FALSE;
	m_bUsePackage = FALSE;
	m_iStartMode = 1;
	m_iDhcpMode = 0;
	m_nTimeout = 3;
	m_nEchoTime = 30;
	m_sFilePath = _T("");
	m_nReconnect = 0;
	//}}AFX_DATA_INIT
}


void CParameterPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CParameterPage)
	DDX_Check(pDX, IDC_CK_AUTORUN, m_bAutoRun);
	DDX_Check(pDX, IDC_CK_AUTOCERT, m_bAutoCert);
	DDX_Check(pDX, IDC_CK_AUTOMIN, m_bAutoMin);
	DDX_Check(pDX, IDC_CK_BANDMAC, m_bBandMac);
	DDX_Check(pDX, IDC_CK_PACKAGE, m_bUsePackage);
	DDX_CBIndex(pDX, IDC_CB_STARTMODE, m_iStartMode);
	DDX_CBIndex(pDX, IDC_CB_DHCPMODE, m_iDhcpMode);
	DDX_Text(pDX, IDC_ED_TIMEOUT, m_nTimeout);
	DDV_MinMaxUInt(pDX, m_nTimeout, 0, 99);
	DDX_Text(pDX, IDC_ED_ECHOTIME, m_nEchoTime);
	DDV_MinMaxUInt(pDX, m_nEchoTime, 0, 999);
	DDX_Text(pDX, IDC_ED_PATH, m_sFilePath);
	DDX_Text(pDX, IDC_ED_RECONNECT, m_nReconnect);
	DDV_MinMaxUInt(pDX, m_nReconnect, 0, 999);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CParameterPage, CDialog)
	//{{AFX_MSG_MAP(CParameterPage)
	ON_BN_CLICKED(IDC_BN_BROWSER, OnBnBrowser)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CParameterPage message handlers


BOOL CParameterPage::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	ReadConfig();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CParameterPage::ReadConfig()
{
	CString strFileName = GetAppPath() + _T("Config.ini");
	TCHAR szCertFlag[16];
	GetPrivateProfileString(_T("Parameters"), _T("CertFlag"), _T("001110080300000"), szCertFlag, 16, strFileName);
	for (int i=0; i<15; i++)
		if (szCertFlag[i] < '0') szCertFlag[i] = '0';
	m_bAutoRun = (szCertFlag[0]-'0') % 2;
	m_bAutoCert = (szCertFlag[1]-'0') % 2;
	m_bAutoMin = (szCertFlag[2]-'0') % 2;
	m_bBandMac = (szCertFlag[3]-'0') % 2;
	m_iStartMode = (szCertFlag[4]-'0') % 2;
	m_iDhcpMode = (szCertFlag[5]-'0') % 4;
	m_nTimeout = ((szCertFlag[6]-'0')*10 + (szCertFlag[7]-'0')) % 100;
	m_nEchoTime = ((szCertFlag[8]-'0')*100 + (szCertFlag[9]-'0')*10 + (szCertFlag[10]-'0')) % 1000;
	m_nReconnect = ((szCertFlag[11]-'0')*100 + (szCertFlag[12]-'0')*10 + (szCertFlag[13]-'0')) % 1000;
	m_bUsePackage = (szCertFlag[14]-'0') % 2;
	GetPrivateProfileString(_T("Parameters"), _T("PackagePath"), _T(""), m_sFilePath.GetBuffer(MAX_PATH), MAX_PATH, strFileName);
	m_sFilePath.ReleaseBuffer();
	UpdateData(FALSE);
}

BOOL CParameterPage::WriteConfig()
{
	if (!UpdateData())
		return FALSE;
	CString strFileName = GetAppPath() + _T("Config.ini");
	CString strKeyValue;
	strKeyValue.Format(_T("%d%d%d%d%d%d%02u%03u%03u%d"), m_bAutoRun, m_bAutoCert, m_bAutoMin, m_bBandMac,
		m_iStartMode, m_iDhcpMode, m_nTimeout, m_nEchoTime, m_nReconnect, m_bUsePackage);
	if (!WritePrivateProfileString(_T("Parameters"), _T("CertFlag"), strKeyValue, strFileName))
	{	//保存设置失败！
		MessageBox(LoadString(IDS_WRITE_ERROR), LoadString(IDS_MSG_CAPTION), MB_OK|MB_ICONWARNING);
		return FALSE;
	}
	WritePrivateProfileString(_T("Parameters"), _T("PackagePath"), m_sFilePath, strFileName);
	SetAutoRun(m_bAutoRun);
	return TRUE;
}

void CParameterPage::SetAutoRun(BOOL bAutoRun)
{
	HKEY hKey;
	RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("Software\\Microsoft\\Windows\\CurrentVersion\\Run"), 0, KEY_ALL_ACCESS, &hKey);
	if (bAutoRun)
	{
		TCHAR FileName[MAX_PATH];
		GetModuleFileName(AfxGetInstanceHandle(), FileName, MAX_PATH);
		LPBYTE lpData = (LPBYTE)(LPCTSTR)FileName;
		if(RegSetValueEx(hKey, _T("MentoHUST"), 0, REG_SZ, lpData, lstrlen(FileName)*sizeof(TCHAR)) != ERROR_SUCCESS)
			//温馨提示|设为开机自动运行失败！
			MessageBox(LoadString(IDS_SETRUN_ERROR), LoadString(IDS_MSG_CAPTION), MB_OK|MB_ICONWARNING);
	}
	else
		RegDeleteValue(hKey, _T("MentoHUST"));
	RegCloseKey(hKey);
}

void CParameterPage::OnBnBrowser() 
{
	CFileDialog dlg(TRUE, NULL, NULL, OFN_FILEMUSTEXIST|OFN_HIDEREADONLY|OFN_ENABLESIZING, LoadString(IDS_PACKAGE_FILTER));
	dlg.m_ofn.lStructSize = 88;
	if (dlg.DoModal() == IDCANCEL)
		return;
	SetDlgItemText(IDC_ED_PATH, dlg.GetPathName());
}

void CParameterPage::OnCancel()
{
	::PostMessage(GetParent()->m_hWnd, WM_CLOSE, 0, 0);
	CDialog::OnCancel();
}
