// ToolDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MentoHUSTTool.h"
#include "ToolDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

typedef struct _PE_HEADER_MAP
{
	DWORD signature;
	IMAGE_FILE_HEADER _head;
	IMAGE_OPTIONAL_HEADER opt_head;
	IMAGE_SECTION_HEADER section_header[8];
}PE_HEADER_MAP,*PPE_HEADER_MAP;

static void pcap_handle(u_char *user, const struct pcap_pkthdr *h, const u_char *buf);
/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	CHyperLink	m_UpdateLink;
	CButtonLink	m_ContactLink;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnContact();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
	, m_UpdateLink(LoadString(IDS_LOGO_URL))
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	DDX_Control(pDX, IDC_SC_UPDATE, m_UpdateLink);
	DDX_Control(pDX, IDC_SC_CONTACT, m_ContactLink);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	ON_BN_CLICKED(IDC_SC_CONTACT, OnContact)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CAboutDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_UpdateLink.SetTipText(LoadString(IDS_UPDATE));
	m_ContactLink.SetTipText(LoadString(IDS_CONTACT));
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CAboutDlg::OnContact() 
{
	// TODO: Add your control notification handler code here
	AfxMessageBox(LoadString(IDS_CONTACT_INFO));
	m_ContactLink.SetCapture();
}

/////////////////////////////////////////////////////////////////////////////
// CToolDlg dialog

CToolDlg::CToolDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CToolDlg::IDD, pParent)
	, m_LogoLink(LoadString(IDS_LOGO_URL))
{
	//{{AFX_DATA_INIT(CToolDlg)
	m_b8021x = TRUE;
	m_bW32N55 = TRUE;
	//}}AFX_DATA_INIT
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_iState = ID_STATE_FREE;
	m_pAdapterList = NULL;
	m_pAdapter = NULL;
	m_pCatchThread = NULL;
	m_pBuffer = NULL;
}

void CToolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CToolDlg)
	DDX_Control(pDX, IDC_SC_LOGO, m_LogoLink);
	DDX_Control(pDX, IDC_CB_ADAPTER, m_AdapterList);
	DDX_Check(pDX, IDC_CK_8021X, m_b8021x);
	DDX_Check(pDX, IDC_CK_W32N55, m_bW32N55);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CToolDlg, CDialog)
	//{{AFX_MSG_MAP(CToolDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BN_ABOUT, OnBnAbout)
	ON_BN_CLICKED(IDC_BN_HELP, OnBnHelp)
	ON_BN_CLICKED(IDC_BN_START, OnBnStart)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CToolDlg message handlers

BOOL CToolDlg::OnInitDialog()
{
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
	m_LogoLink.SetTipText(LoadString(IDS_LOGO_TIP));
	if (!InitAdapterList())
		MessageOK(LoadString(IDS_NOADAPTER));
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CToolDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		switch (nID)
		{
		case ID_STATE_START:
			m_iState = ID_STATE_START;
			m_pCatchThread = AfxBeginThread(CatchThreadFunc, this, THREAD_PRIORITY_TIME_CRITICAL, 0, CREATE_SUSPENDED);
			m_pCatchThread->m_bAutoDelete = FALSE;
			m_pCatchThread->ResumeThread();
			SetDlgItemText(IDC_BN_START, LoadString(IDS_CANCEL_CATCH));
			SetDlgItemText(IDC_SC_STATE, LoadString(IDS_STATE_START));
			m_AdapterList.EnableWindow(FALSE);
			if (ShellExecute(NULL, _T("open"), Ruijie, NULL, NULL, SW_SHOWNORMAL) < (HANDLE)32)
				MessageBox(_T("无法运行锐捷客户端，请手动运行！"), _T("温馨提示"), MB_OK | MB_ICONWARNING);
			break;
		case ID_STATE_FREE:
			StopCatch();
			break;
		case ID_STATE_SUCCESS:
			SaveData();
			StopCatch();
			break;
		default:
			CDialog::OnSysCommand(nID, lParam);
		}
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CToolDlg::OnPaint() 
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

HCURSOR CToolDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CToolDlg::MessageOK(LPCTSTR lpszText)
{
	// 温馨提示 ……
	MessageBox(lpszText, LoadString(IDS_MSG_CAPTION), MB_OK|MB_ICONWARNING);
}

void CToolDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	if (m_iState != ID_STATE_FREE)
		StopCatch();
	if (m_pAdapter != NULL)
		pcap_close(m_pAdapter);
	if (m_pAdapterList != NULL)
		delete m_pAdapterList;
}

void CToolDlg::OnOK() 
{
	OnBnStart();
}

void CToolDlg::OnBnAbout() 
{
	CAboutDlg dlg;
	dlg.DoModal();
}

void CToolDlg::OnBnHelp() 
{
	CString strFileName = GetAppPath()+LoadString(IDS_HELP_FILE);
	if (ShellExecute(NULL, _T("open"), strFileName, NULL, NULL, SW_SHOWNORMAL) < (HANDLE)32)
	{	// 在程序所在目录找不到帮助文件 Readme.txt！
		MessageOK(LoadString(IDS_HELP_ERROR));
	}
}

BOOL CToolDlg::InitAdapterList()
{
	m_AdapterList.ResetContent();
	if (m_pAdapterList != NULL)
	{
		delete m_pAdapterList;
		m_pAdapterList = NULL;
	}
	pcap_if_t *alldevs = NULL;
	char errbuf[PCAP_ERRBUF_SIZE];
	if (pcap_findalldevs(&alldevs, errbuf) == -1)
		return FALSE;
	CString strAdapterName;
	CString strDescription;
	StringList *pTemp;
	for(pcap_if_t *d=alldevs; d!=NULL; d=d->next)
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
		return FALSE;
	m_AdapterList.SetCurSel(0);
	return TRUE;
}

BOOL CToolDlg::OpenAdapter()
{
	char errbuf[PCAP_ERRBUF_SIZE];
	if (m_pAdapter != NULL)
		pcap_close(m_pAdapter);
	CString strAdapterName = m_pAdapterList->GetString(m_AdapterList.GetCurSel());
	if ((m_pAdapter = pcap_open_live(strAdapterName, 2048, 0, 500, errbuf)) == NULL)
	{
		strAdapterName.Format(LoadString(IDS_OPEN_ERROR), errbuf);
		MessageOK(strAdapterName);
		return FALSE;
	}
	strcpy(errbuf, "ether[12:2]=0x888e");
	struct bpf_program fcode;
	if (pcap_compile(m_pAdapter, &fcode, errbuf, 0, 0) == -1
		|| pcap_setfilter(m_pAdapter, &fcode) == -1)
	{
		strAdapterName.Format(LoadString(IDS_FILTER_ERROR), pcap_geterr(m_pAdapter));
		MessageOK(strAdapterName);
		return FALSE;
	}
	pcap_freecode(&fcode);
	return TRUE;
}

void CToolDlg::OnBnStart() 
{
	if (m_iState != ID_STATE_FREE)
	{
		SendMessage(WM_SYSCOMMAND, ID_STATE_FREE);
		return;
	}
	if (!CheckStart())
		return;
	SendMessage(WM_SYSCOMMAND, ID_STATE_START);
}

CString CToolDlg::GetRuijiePath(bool reg)
{
	CString Path = _T("");
	if (reg)
	{
		HKEY hKey;
		if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\8021x.exe"), 0, KEY_READ, &hKey) != ERROR_SUCCESS)
			return(_T(""));
		TCHAR FileName[MAX_PATH];
		DWORD type = REG_SZ;
		DWORD cbData = MAX_PATH * sizeof(TCHAR);
		if (RegQueryValueEx(hKey, NULL, NULL, &type, (LPBYTE)FileName, &cbData) == ERROR_SUCCESS)
			Path = FileName;
		RegCloseKey(hKey);
	}
	else
	{
		CFileDialog dlg(TRUE, _T("exe"), _T("8021x.exe"), OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_ENABLESIZING,
			_T("锐捷客户端(8021x.exe)|8021x.exe|所有文件(*.*)|*.*||"), NULL);
		dlg.m_ofn.lStructSize = 88;
		if (dlg.DoModal() == IDCANCEL)
			return(_T(""));
		Path = dlg.GetPathName();
	}
	return Path;
}

BOOL CToolDlg::CheckStart()
{
	if (!UpdateData())
		return FALSE;
	Ruijie = GetRuijiePath(true);
	CFileFind finder;
	while (!finder.FindFile(Ruijie))
	{
		if(MessageBox(_T("未找到锐捷客户端程序8021x.exe，是否手动查找？"), _T("温馨提示"), MB_OKCANCEL | MB_ICONWARNING ) == IDCANCEL)
			return FALSE;
		else
			Ruijie = GetRuijiePath(false);
	}
	BYTE *p8021x = NULL, *W32N55 = NULL;
	DWORD n8021x = 0, nW32N55 = 0;
	if (m_b8021x && (p8021x=ReadCode(Ruijie, &n8021x))==NULL)
	{
		MessageBox(_T("读取8021x.exe失败！"), _T("温馨提示"), MB_OK | MB_ICONWARNING);
		return FALSE;
	}
	Ruijie = Ruijie.Left(Ruijie.ReverseFind(_T('\\'))+1) + _T("W32N55.dll");
	if (m_bW32N55 && (W32N55=ReadCode(Ruijie, &nW32N55))==NULL)
	{
		MessageBox(_T("读取W32N55.dll失败！"), _T("温馨提示"), MB_OK | MB_ICONWARNING);
		delete []p8021x;
		return FALSE;
	}
	m_nSize = 16 + n8021x + nW32N55;
	if (m_pBuffer)
		delete []m_pBuffer;
	m_pBuffer = new BYTE[m_nSize];
	memcpy(m_pBuffer, "HUST", 4);
	*(u_int32_t *)(m_pBuffer + 4) = *(u_int32_t *)m_pBuffer ^ n8021x;
	*(u_int32_t *)(m_pBuffer + 8) = *(u_int32_t *)m_pBuffer ^ (n8021x+nW32N55);
	memcpy(m_pBuffer+12, "\0XTU", 4);
	if (m_b8021x) {
		memcpy(m_pBuffer+16, p8021x, n8021x);
		delete []p8021x;
	}
	if (m_bW32N55) {
		memcpy(m_pBuffer+16+n8021x, W32N55, nW32N55);
		delete []W32N55;
	}
	if (!OpenAdapter()) {
		delete []m_pBuffer;
		m_pBuffer = NULL;
		return FALSE;
	}
	Ruijie = Ruijie.Left(Ruijie.ReverseFind(_T('\\'))+1) + _T("RuijieSupplicant.exe");
	return TRUE;
}

BYTE *CToolDlg::ReadCode(CString file, DWORD *size) {
	BYTE *data = NULL;
	int i;
	PPE_HEADER_MAP hpe;
	
	FILE *fp = _tfopen(file, _T("rb"));
	if (fp == NULL)
		goto fileError;
	data = new BYTE[0x1000];
	if (fread(data, 0x1000, 1, fp) < 1)
		goto fileError;
	
	hpe = (PPE_HEADER_MAP)(((PIMAGE_DOS_HEADER)data)->e_lfanew + data);
	for (i=0; i<hpe->_head.NumberOfSections; i++) {
		if (hpe->section_header[i].Characteristics & (IMAGE_SCN_MEM_EXECUTE | IMAGE_SCN_CNT_CODE)) {
			fseek(fp, hpe->section_header[i].PointerToRawData, SEEK_SET);
			*size = hpe->section_header[i].SizeOfRawData;
			delete []data;
			data = new BYTE[*size];
			if (fread(data, *size, 1, fp) < 1)
				goto fileError;
			fclose(fp);
			return data;
		}
	}
	
fileError:
	if (fp != NULL)
		fclose(fp);
	if (data != NULL)
		delete []data;
	return NULL;
}

void CToolDlg::StopCatch()
{
	SetDlgItemText(IDC_SC_STATE, LoadString(IDS_STATE_STOPPING));
	pcap_breakloop(m_pAdapter);
	WaitForSingleObject(m_pCatchThread->m_hThread, INFINITE);
	delete m_pCatchThread;
	m_pCatchThread = NULL;
	delete []m_pBuffer;
	m_pBuffer = NULL;
	m_iState = ID_STATE_FREE;
	SetDlgItemText(IDC_BN_START, LoadString(IDS_START_CATCH));
	SetDlgItemText(IDC_SC_STATE, LoadString(IDS_STATE_STOP));
	m_AdapterList.EnableWindow(TRUE);
}

UINT CatchThreadFunc(LPVOID pParam)
{
	CToolDlg *dlg = (CToolDlg *)pParam;
	memset(dlg->m_btPacket, 0, 0x400);
	if (pcap_loop(dlg->m_pAdapter, -1, pcap_handle, (u_char *)pParam) == -1) /* 开始捕获数据包 */
	{
		dlg->MessageOK(LoadString(IDS_CATCH_ERROR));
		dlg->PostMessage(WM_SYSCOMMAND, ID_STATE_FREE);
		return -1;
	}
	return 0;
}

static void pcap_handle(u_char *user, const struct pcap_pkthdr *h, const u_char *buf)
{
	CToolDlg *dlg = (CToolDlg *)user;
	int &State = dlg->m_iState, start, len;
	if (buf[0x12] == 0x02)
	{
		// 16+2+buf[0x11]+23 = buf[0x11]+41
		start = buf[0x11] + 41;
		len = h->caplen - start;
		len = len > 0x200 ? 0x200 : len;
		if (buf[0x16]==0x01 && State == ID_STATE_START) {
			memcpy(dlg->m_btPacket, buf + start, len);
			State = ID_STATE_MD5;
			dlg->SetDlgItemText(IDC_SC_STATE, LoadString(IDS_STATE_MD5));
		} else if (buf[0x16]==0x04 && State==ID_STATE_MD5) {
			memcpy(dlg->m_btPacket + 0x200, buf + start, len);
			dlg->PostMessage(WM_SYSCOMMAND, ID_STATE_SUCCESS);

		}
	}
}

void CToolDlg::SaveData()
{
	m_iState = ID_STATE_SUCCESS;
	CFileDialog dlg(FALSE, _T("mpf"), NULL, OFN_ENABLESIZING, LoadString(IDS_FILE_FILTER));
	dlg.m_ofn.lStructSize = 88;
	if (dlg.DoModal() == IDCANCEL)
		return;
	try {
		CFile File(dlg.GetPathName(), CFile::modeCreate|CFile::modeWrite|CFile::typeBinary);
		File.Write(m_pBuffer, m_nSize);
		File.Write(m_btPacket, 0x400);
		File.Close();
	}
	catch (...) {
		MessageOK(LoadString(IDS_SAVE_ERROR));
		return;
	}
	MessageOK(LoadString(IDS_SAVE_SUCCESS));
}
