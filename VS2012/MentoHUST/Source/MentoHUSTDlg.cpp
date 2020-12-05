// MentoHUSTDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MentoHUST.h"
#include "MentoHUSTDlg.h"
#include "ConfigDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define MAX_OUTPUT_LINES						500

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	CButtonLink	m_LinkContact;
	CHyperLink	m_LinkUpdate;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	afx_msg void OnScContact();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
	, m_LinkUpdate(LoadString(IDS_LINKURL))
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	DDX_Control(pDX, IDC_SC_CONTRACT, m_LinkContact);
	DDX_Control(pDX, IDC_SC_CHECKNEW, m_LinkUpdate);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	ON_BN_CLICKED(IDC_SC_CONTRACT, OnScContact)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CAboutDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_LinkContact.SetTipText(LoadString(IDS_CONTACT_TIP));
	m_LinkUpdate.SetTipText(LoadString(IDS_UPDATE_TIP));
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CAboutDlg::OnScContact() 
{
	MessageBox(LoadString(IDS_CONTACT_TEXT), LoadString(IDS_CONTACT_TIP), MB_OK|MB_ICONWARNING);
	m_LinkContact.SetCapture();
}

/////////////////////////////////////////////////////////////////////////////
// CMentoHUSTDlg dialog

CMentoHUSTDlg::CMentoHUSTDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMentoHUSTDlg::IDD, pParent)
	, m_sStateArea(LoadString(IDS_WELCOME))
	, m_sStateBoard(LoadString(IDS_WELCOME))
{
	//{{AFX_DATA_INIT(CMentoHUSTDlg)
	//}}AFX_DATA_INIT
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_sServerMsg = _T("");
	m_bChanged = FALSE;
	m_pAccountList = NULL;
	m_pAdapterList = NULL;
}

void CMentoHUSTDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMentoHUSTDlg)
	DDX_Control(pDX, IDC_SC_LOGO, m_linkLogo);
	DDX_Control(pDX, IDC_CB_ADAPTER, m_AdapterList);
	DDX_Control(pDX, IDC_CB_ACCOUNT, m_AccountList);
	DDX_Text(pDX, IDC_ED_OUTPUT, m_sStateArea);
	DDX_Text(pDX, IDC_SC_STATE, m_sStateBoard);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CMentoHUSTDlg, CDialog)
	//{{AFX_MSG_MAP(CMentoHUSTDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BN_ABOUT, OnBnAbout)
	ON_BN_CLICKED(IDC_BN_CLEAR, OnBnClear)
	ON_BN_CLICKED(IDC_BN_HELP, OnBnHelp)
	ON_BN_CLICKED(IDC_BN_OPTION, OnBnOption)
	ON_BN_CLICKED(IDC_BN_OUTPUT, OnBnOutput)
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_SC_LOGO, OnScLogo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMentoHUSTDlg message handlers

BOOL CMentoHUSTDlg::OnInitDialog()
{
	SetProp(m_hWnd, GUI_ID, (HANDLE)1);

	CDialog::OnInitDialog();
	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	MyInitDialog();
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CMentoHUSTDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
		OnBnAbout();
	else
	{
		switch (nID)
		{
		case SC_MINIMIZE:
			ToTray(1);
			break;
		case END_CERT:
			m_Process.endCert();
			break;
		default:
			CDialog::OnSysCommand(nID, lParam);
		}
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMentoHUSTDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

HCURSOR CMentoHUSTDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

int CMentoHUSTDlg::MsgBox(LPCTSTR lpszText, UINT nType)
{
	return MessageBox(lpszText, LoadString(IDS_MSG_CAPTION), nType);
}

int CMentoHUSTDlg::ShowDlg(CDialog *dlg)
{
	int iRetVal;
	CRect dlgRect;
	GetWindowRect(&dlgRect);
	if (dlgRect.Height() >= m_iBigHeight)
	{
		OnBnOutput();
		iRetVal = dlg->DoModal();
		OnBnOutput();
	}
	else
		iRetVal = dlg->DoModal();
	return iRetVal;
}

void CMentoHUSTDlg::OnBnAbout() 
{
	CAboutDlg dlg;
	ShowDlg(&dlg);
}

void CMentoHUSTDlg::OnBnClear() 
{
	m_sServerMsg.Empty();
	m_sStateArea.LoadString(IDS_WELCOME);	//:: 欢迎使用 MentoHUST!
	SetDlgItemText(IDC_ED_OUTPUT, m_sStateArea);
}

void CMentoHUSTDlg::Output(LPCTSTR Msg, int Type)
{
	CEdit *StateEdit = (CEdit *)GetDlgItem(IDC_ED_OUTPUT);
	if (StateEdit->GetLineCount() > MAX_OUTPUT_LINES)
		m_sStateArea.Format(LoadString(IDS_MSGCLEAR), MAX_OUTPUT_LINES);	//:: 输出超过 %d, 自动清空。
	switch (Type)
	{
	case 1:
		SetDlgItemText(IDC_SC_STATE, Msg);
		return;
	case 2:
		m_sStateArea = m_sStateArea + _T("\r\n") + Msg;
		break;
	default:
		SetDlgItemText(IDC_SC_STATE, Msg);
		m_sStateArea = m_sStateArea + _T("\r\n") + Msg;
	}
	StateEdit->SetWindowText(m_sStateArea);
	StateEdit->SetSel(-2, -1);
}

void CMentoHUSTDlg::OnBnHelp()
{
	CString strFileName = GetAppPath() + _T("Readme.txt");
	if (ShellExecute(NULL, _T("open"), strFileName, NULL, NULL, SW_SHOWNORMAL) < (HANDLE)32)
		MsgBox(LoadString(IDS_HELPERROR));	//在程序所在目录找不到帮助文件 Readme.txt！
}

void CMentoHUSTDlg::OnBnOption() 
{
	CConfigDlg dlg;
	if (IDOK == ShowDlg(&dlg))
	{
		if (m_Process.m_state == IDT_DISCONNECT)
		{
			ReadConfig();
			InitAccountList();
		}
		else
			m_bChanged = TRUE;
	}
}

void CMentoHUSTDlg::OnBnOutput() 
{
	CRect dlgRect;
	GetWindowRect(&dlgRect);
	if (dlgRect.Height() >= m_iBigHeight)
	{
		dlgRect.bottom = dlgRect.top + m_iSmallHeight;
		SetDlgItemText(IDC_BN_OUTPUT, LoadString(IDS_OUTPUT_D));	//输出↑->输出↓
	}
	else
	{
		dlgRect.bottom = dlgRect.top + m_iBigHeight;
		SetDlgItemText(IDC_BN_OUTPUT, LoadString(IDS_OUTPUT_U));	//输出↓->输出↑
	}
	MoveWindow(&dlgRect);
}

void CMentoHUSTDlg::OnCancel() 
{
	if (MsgBox(LoadString(IDS_EXIT_CONFIRM), MB_OKCANCEL|MB_ICONWARNING) == IDOK)
		CDialog::OnCancel();
}

void CMentoHUSTDlg::MyInitDialog()
{
	InitInterface();	//初始化界面
	ReadConfig();		//读取配置
	InitAccountList();	//初始化账号列表
	InitAdapterList();	//初始化网卡列表
	if (m_pAdapterList == NULL)		//找不到网卡
		MsgBox(LoadString(IDS_FIND_NONIC));
	else if (m_bAutoCert)	//运行后自动认证
		OnOK();
	InitTrayIcon();		//初始化托盘图标
}

void CMentoHUSTDlg::InitInterface()
{
	CRect dlgRect, msgRect;
	GetWindowRect(&dlgRect);
	GetDlgItem(IDC_ED_OUTPUT)->GetWindowRect(&msgRect);
	m_iBigHeight = dlgRect.Height();
	m_iSmallHeight = msgRect.top - dlgRect.top;
	OnBnOutput();
	
	CButtonLink::SetLinkCursor(AfxGetApp()->LoadCursor(IDC_CR_HAND));
	CString strFileName = GetAppPath() + _T("logo.bmp");
	HBITMAP hLogoBmp = (HBITMAP)LoadImage(AfxGetInstanceHandle(), strFileName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE|LR_CREATEDIBSECTION);
	if (hLogoBmp != NULL)
		m_linkLogo.SetBitmap(hLogoBmp);
	
	strFileName = GetAppPath() + _T("Config.ini");
	CString strKeyValue;
	GetPrivateProfileString(_T("Parameters"), _T("Caption"), _T(""), strKeyValue.GetBuffer(100), 100, strFileName);
	strKeyValue.ReleaseBuffer();
	if (!strKeyValue.IsEmpty())
		SetWindowText(strKeyValue);
	GetPrivateProfileString(_T("Parameters"), _T("LinkURL"), LoadString(IDS_LINKURL), strKeyValue.GetBuffer(100), 100, strFileName);
	strKeyValue.ReleaseBuffer();
	m_linkLogo.SetURL(strKeyValue);
	GetPrivateProfileString(_T("Parameters"), _T("LinkTip"), LoadString(IDS_UPDATE_TIP), strKeyValue.GetBuffer(100), 100, strFileName);
	strKeyValue.ReleaseBuffer();
	m_linkLogo.SetTipText(strKeyValue);
}

void CMentoHUSTDlg::ReadConfig()
{
	CString strFileName = GetAppPath() + _T("Config.ini");
	TCHAR szCertFlag[16];
	GetPrivateProfileString(_T("Parameters"), _T("CertFlag"), _T("001110080200000"), szCertFlag, 16, strFileName);
	int i;
	for (i=0; i<15; i++)
		if (szCertFlag[i] < '0') szCertFlag[i] = '0';
	m_bAutoCert = (szCertFlag[1]-'0') % 2;
	m_Process.m_autoMin = (szCertFlag[2]-'0') % 2;
	m_Process.m_bandMac = (szCertFlag[3]-'0') % 2;
	m_Process.m_startMode = (szCertFlag[4]-'0') % 2;
	m_Process.m_dhcpMode = (szCertFlag[5]-'0') % 4;
	m_Process.m_timeout = (((szCertFlag[6]-'0')*10 + (szCertFlag[7]-'0')) % 100) * 1000;
	m_Process.m_echoTime = (((szCertFlag[8]-'0')*100 + (szCertFlag[9]-'0')*10 + (szCertFlag[10]-'0')) % 1000) * 1000;
	m_Process.m_autoReconnect = (((szCertFlag[11]-'0')*100 + (szCertFlag[12]-'0')*10 + (szCertFlag[13]-'0')) % 1000) * 60000;
	m_Process.m_usePackage = (szCertFlag[14]-'0') % 2;
	GetPrivateProfileString(_T("Parameters"), _T("PackagePath"), _T(""), m_Process.m_package, MAX_PATH, strFileName);
	GetPrivateProfileString(_T("Parameters"), _T("ExtraFlag"), _T("15021"), szCertFlag, 6, strFileName);
	for (i=0; i<5; i++)
		if (szCertFlag[i] < '0') szCertFlag[i] = '0';
	m_Process.m_restart = (((szCertFlag[0]-'0')*10 + (szCertFlag[1]-'0')) % 100) * 1000;
	unsigned memTime = (((szCertFlag[2]-'0')*10 + (szCertFlag[3]-'0')) % 100) * 30000;
	if (memTime != 0)
		SetTimer(MEM_TIMER, memTime, NULL);
	else
		KillTimer(MEM_TIMER);
	if (szCertFlag[4] - '0')
		SetWindowPos(&CWnd::wndTopMost, 0,0,0,0, SWP_NOMOVE|SWP_NOSIZE);
	else
		SetWindowPos(&CWnd::wndNoTopMost, 0,0,0,0, SWP_NOMOVE|SWP_NOSIZE);
	m_Process.m_bCernet = GetPrivateProfileInt(_T("Parameters"), _T("ClientType"), 0, strFileName) ? 1 : 0;
}

void CMentoHUSTDlg::InitTrayIcon()
{
	m_tnid.cbSize = (DWORD)sizeof(NOTIFYICONDATA);
	m_tnid.hWnd = GetSafeHwnd();
	m_tnid.uID = 0;
	m_tnid.uFlags = NIF_ICON|NIF_MESSAGE|NIF_TIP|NIF_INFO;
	m_tnid.uTimeout = 10000;
	m_tnid.uCallbackMessage = WM_COMMAND;
	m_tnid.dwInfoFlags = NIIF_INFO;
	m_tnid.hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_FAILED));
	lstrcpy(m_tnid.szInfoTitle, LoadString(IDS_TRAY_CAPTION));
	lstrcpy(m_tnid.szInfo, m_sServerMsg);
	lstrcpy(m_tnid.szTip, LoadString(IDS_TRAY_DISCONNECT));
	Shell_NotifyIcon(NIM_ADD, &m_tnid);
}

void CMentoHUSTDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	if (m_Process.m_state != IDT_DISCONNECT)
		//m_Process.endCert();//不用管直接退出
	Shell_NotifyIcon(NIM_DELETE, &m_tnid);
	if (m_pAccountList != NULL)
		delete m_pAccountList;
	if (m_pAdapterList != NULL)
		delete m_pAdapterList;
}

void CMentoHUSTDlg::InitAccountList()
{
	if (m_pAccountList != NULL)
	{
		delete m_pAccountList;
		m_pAccountList = NULL;
		m_AccountList.ResetContent();
	}
	CString strFileName = GetAppPath() + _T("Config.ini");
	int AccountCount = GetPrivateProfileInt(_T("Parameters"), _T("AccountCount"), 0, strFileName);
	CString strSectionName, strKeyValue;
	StringList *pTemp;
	for (int i=0; i<AccountCount; i++)
	{
		strSectionName.Format(_T("Account%d"), i);
		GetPrivateProfileString(strSectionName, _T("Username"), _T(""), strKeyValue.GetBuffer(70), 70, strFileName);
		strKeyValue.ReleaseBuffer();
		if (strKeyValue.IsEmpty())
			continue;
		pTemp = new StringList(strSectionName);
		if (m_pAccountList == NULL)
			m_pAccountList = pTemp;
		else
			m_pAccountList->AddString(pTemp);
		m_AccountList.AddString(strKeyValue);
	}
	if (m_pAccountList == NULL)
	{
		if (ReadRuijieAccount(strKeyValue))
		{
			m_pAccountList = new StringList(_T("Account0"));
			m_AccountList.AddString(strKeyValue);
			m_AccountList.SetCurSel(0);
		}
		return;
	}
	GetPrivateProfileString(_T("Parameters"), _T("DefaultAccount"), _T(""), strKeyValue.GetBuffer(20), 20, strFileName);
	strKeyValue.ReleaseBuffer();
	int nIndex = m_pAccountList->FindString(strKeyValue);
	if (nIndex == -1)
		nIndex = 0;
	m_AccountList.SetCurSel(nIndex);
}

void CMentoHUSTDlg::InitAdapterList()
{
	if (m_pAdapterList != NULL)
	{
		delete m_pAdapterList;
		m_pAdapterList = NULL;
		m_AdapterList.ResetContent();
	}
	pcap_if_t *alldevs = NULL;
	char errbuf[PCAP_ERRBUF_SIZE];
	if (pcap_findalldevs(&alldevs, errbuf) == -1)
		return;
	CString strAdapterName;
	CString strDescription;
	StringList *pTemp;
	for (pcap_if_t *d=alldevs; d!=NULL; d=d->next)
	{
		strAdapterName = d->name;
		strDescription = d->description;
		if (!(d->flags & PCAP_IF_LOOPBACK))
		{
			pTemp = new StringList(strAdapterName);
			if (m_pAdapterList == NULL)
				m_pAdapterList = pTemp;
			else
				m_pAdapterList->AddString(pTemp);
			m_AdapterList.AddString(strDescription);
		}
	}
	pcap_freealldevs(alldevs);
	if (m_pAdapterList == NULL)
		return;
	CString strFileName = GetAppPath() + _T("Config.ini");
	GetPrivateProfileString(_T("Parameters"), _T("DefaultAdapter"), _T(""), strAdapterName.GetBuffer(60), 60, strFileName);
	strAdapterName.ReleaseBuffer();
	int nIndex = m_pAdapterList->FindString(strAdapterName);
	if (nIndex == -1)
		nIndex = 0;
	m_AdapterList.SetCurSel(nIndex);
}

BOOL CMentoHUSTDlg::ReadRuijieAccount(CString &strUsername)
{
	HKEY hKey;
	TCHAR szUsername[70], szPassword[70];
	DWORD cbUsername = sizeof(szUsername), cbPassword = sizeof(szPassword);
	DWORD type = REG_SZ;
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, LoadString(IDS_RUIJIE_REG), 0, KEY_READ, &hKey) != ERROR_SUCCESS)
		return FALSE;
	if (RegQueryValueEx(hKey, _T("UserName"), NULL, &type, (LPBYTE)szUsername, &cbUsername) != ERROR_SUCCESS
		|| RegQueryValueEx(hKey, _T("Password"), NULL, &type, (LPBYTE)szPassword, &cbPassword) != ERROR_SUCCESS)
	{
		RegCloseKey(hKey);
		return FALSE;
	}
	RegCloseKey(hKey);
	
	strUsername.Empty();
	CString strPassword = szPassword;
#ifdef UNICODE
	char szTemp[70];
	wcstombs(szTemp, szUsername, 70);
	const char *szUser = szTemp;
#else
	const char *szUser = szUsername;
#endif
	char buf[90];
	if (DecodeRuijie(buf, szUser))
		strUsername = buf;
	if (strUsername.IsEmpty() || strPassword.IsEmpty())
		return FALSE;
	CString strFileName = GetAppPath() + _T("Config.ini");
	WritePrivateProfileString(_T("Parameters"), _T("AccountCount"), _T("1"), strFileName);
	WritePrivateProfileString(_T("Parameters"), _T("DefaultAccount"), _T("Account0"), strFileName);
	WritePrivateProfileString(_T("Account0"), _T("Username"), strUsername, strFileName);
	WritePrivateProfileString(_T("Account0"), _T("Password"), strPassword, strFileName);
	return TRUE;
}

BOOL CMentoHUSTDlg::GetAccount()
{
	if (m_pAccountList == NULL)
		return FALSE;
	CString strFileName = GetAppPath() + _T("Config.ini");
	CString strSectionName, strUsername, strPassword, strIP;
	strSectionName = m_pAccountList->GetString(m_AccountList.GetCurSel());
	GetDlgItemText(IDC_CB_ACCOUNT, strUsername);
	GetPrivateProfileString(strSectionName, _T("Password"), _T(""), strPassword.GetBuffer(70), 70, strFileName);
	strPassword.ReleaseBuffer();
	if (strPassword.IsEmpty())
		return FALSE;
#ifdef UNICODE
	char szTemp[70];
	wcstombs(szTemp, strPassword, 70);
	const char *szPass = szTemp;
#else
	const char *szPass = strPassword;
#endif
	char buf[90];
	if (DecodeRuijie(buf, szPass))
		strPassword = buf;
	else
		return FALSE;
	strncpy(m_Process.m_userName, (LPCSTR)(LPCTSTR)ConvertToUTF8(strUsername), sizeof(m_Process.m_userName));
	strncpy(m_Process.m_password, (LPCSTR)(LPCTSTR)ConvertToUTF8(strPassword), sizeof(m_Process.m_password));
	GetPrivateProfileString(strSectionName, _T("IP"), _T("0.0.0.0"), strIP.GetBuffer(20), 20, strFileName);
	strIP.ReleaseBuffer();
#ifdef UNICODE
	wcstombs(szTemp, strIP, 70);
	m_Process.m_ip = inet_addr(szTemp);
#else
	m_Process.m_ip = inet_addr(strIP);
#endif
	return TRUE;
}

BOOL CMentoHUSTDlg::GetAdapter()
{
	if (m_pAdapterList == NULL)
		return FALSE;
	CString strAdapterName = m_pAdapterList->GetString(m_AdapterList.GetCurSel());
	if (strAdapterName.GetLength() < 38)
		return FALSE;
#ifdef UNICODE
	wcstombs(m_Process.m_nic, strAdapterName, 60);
#else
	strncpy(m_Process.m_nic, strAdapterName, 60);
#endif
	return TRUE;
}

void CMentoHUSTDlg::SaveSelect()
{
	CString strFileName = GetAppPath() + _T("Config.ini");
	CString strKeyValue;
	strKeyValue = m_pAccountList->GetString(m_AccountList.GetCurSel());
	WritePrivateProfileString(_T("Parameters"), _T("DefaultAccount"), strKeyValue, strFileName);
	strKeyValue = m_pAdapterList->GetString(m_AdapterList.GetCurSel());
	WritePrivateProfileString(_T("Parameters"), _T("DefaultAdapter"), strKeyValue, strFileName);
}

void CMentoHUSTDlg::ChangeTrayIcon()
{
	lstrcpy(m_tnid.szInfo, m_sServerMsg);
	switch (m_Process.m_state)
	{
	case IDT_DISCONNECT:
		m_tnid.hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_FAILED));
		lstrcpy(m_tnid.szTip, LoadString(IDS_TRAY_DISCONNECT));
		break;
	case IDT_START:
		m_tnid.hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_CERTTING));
		lstrcpy(m_tnid.szTip, LoadString(IDS_TRAY_START));
		break;
	case IDT_IDENTITY:
		m_tnid.hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_CERTTING));
		lstrcpy(m_tnid.szTip, LoadString(IDS_TRAY_USERNAME));
		break;
	case IDT_CHALLENGE:
		m_tnid.hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_CERTTING));
		lstrcpy(m_tnid.szTip, LoadString(IDS_TRAY_PASSWORD));
		break;
	case IDT_ECHO:
		m_tnid.hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));
		lstrcpy(m_tnid.szTip, LoadString(IDS_TRAY_SUCCESS));
		break;
	case IDT_WAITECHO:
		m_tnid.hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));
		lstrcpy(m_tnid.szTip, LoadString(IDS_TRAY_SUCCESS));
		break;
	case IDT_DHCP:
		m_tnid.hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_CERTTING));
		lstrcpy(m_tnid.szTip, LoadString(IDS_TRAY_DHCP));
		break;
	default:
		break;
	}
	Shell_NotifyIcon(NIM_ADD, &m_tnid);
	Shell_NotifyIcon(NIM_MODIFY, &m_tnid);
}

void CMentoHUSTDlg::ToTray(int nType)
{
	if (nType == 0)
		ShowWindow(SW_SHOWNORMAL);
	else if (nType == 1)
	{
		ShowWindow(SW_MINIMIZE);
		ShowWindow(SW_HIDE);
	}
	else
	{
		WINDOWPLACEMENT wndpl;
		GetWindowPlacement(&wndpl);
		if (wndpl.showCmd == SW_SHOWMINIMIZED)
			ToTray(0);
		else
			ToTray(1);
	}
}

BOOL CMentoHUSTDlg::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	switch (wParam)
	{
	case ID_MENU_RESTORE:
		ToTray(2);
		break;
	case ID_MENU_CONNECT:
		OnOK();
		break;
	case ID_MENU_CONFIG:
		OnBnOption();
		break;
	case ID_MENU_HELP:
		OnBnHelp();
		break;
	case ID_MENU_ABOUT:
		OnBnAbout();
		break;
	case ID_MENU_EXIT:
		CDialog::OnCancel();
		break;
	case 0:
		switch (lParam)
		{
		case WM_LBUTTONDOWN:
			SetTimer(SHOW_TIMER, 300, NULL);
			break;
		case WM_LBUTTONDBLCLK:
			KillTimer(SHOW_TIMER);
			ToTray(2);
			break;
		case WM_RBUTTONUP:
			CMenu menu;
			VERIFY(menu.LoadMenu(IDR_TRAY_MENU));
			CMenu *pPopup = menu.GetSubMenu(0);
			ASSERT(pPopup != NULL);
			if (m_Process.m_state != IDT_DISCONNECT)
				pPopup->ModifyMenu(ID_MENU_CONNECT, MF_BYCOMMAND|MF_STRING, ID_MENU_CONNECT, LoadString(IDS_DISCONNECT));
			POINT pt;   
			GetCursorPos(&pt);
			::SetForegroundWindow(m_hWnd);
			pPopup->TrackPopupMenu(TPM_LEFTALIGN, pt.x, pt.y, this);
			menu.Detach();
			menu.DestroyMenu();
			break;
		}
	}
	return CDialog::OnCommand(wParam, lParam);
}

LRESULT CMentoHUSTDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if (message == WM_QUERYENDSESSION)
		CDialog::OnCancel();
	else if (message == WM_TASKBARCREATED)
		Shell_NotifyIcon(NIM_ADD, &m_tnid);
	return CDialog::WindowProc(message, wParam, lParam);
}

void CMentoHUSTDlg::OnTimer(UINT nIDEvent) 
{
	switch (nIDEvent)
	{
	case SHOW_TIMER:
		PostMessage(WM_COMMAND, 0, WM_LBUTTONDBLCLK);
		break;
	case SEND_TIMER:
		if (m_Process.switchState(m_Process.m_state) == -1)
		{
			m_Process.endCert();
			MsgBox(LoadString(IDS_SEND_ERROR));
			ShowWindow(SW_SHOWNORMAL);
		}
		break;
	case TEST_TIMER:
		m_Process.sendTestPacket();
		break;
	case MEM_TIMER:
		WINDOWPLACEMENT wndpl;
		GetWindowPlacement(&wndpl);
		if (wndpl.showCmd == SW_SHOWMINIMIZED && (m_Process.m_state==IDT_ECHO || \
			m_Process.m_state==IDT_WAITECHO || m_Process.m_state==IDT_DISCONNECT))
			SetProcessWorkingSetSize(GetCurrentProcess(), -1, -1);
		break;
	}
	CDialog::OnTimer(nIDEvent);
}

void CMentoHUSTDlg::OnOK() 
{
	if (m_Process.m_state == IDT_DISCONNECT)
	{
		if (!GetAdapter())
		{
			MsgBox(LoadString(IDS_GET_ERROR));
			return;
		}
		SaveSelect();
		m_Process.startCert();
	}
	else
	{
		m_Process.endCert();
		if (m_bChanged)
		{
			ReadConfig();
			InitAccountList();
			m_bChanged = FALSE;
		}
	}
}


void CMentoHUSTDlg::OnScLogo() 
{
	// TODO: Add your control notification handler code here
	
}
