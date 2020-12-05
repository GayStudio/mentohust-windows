// MHPCDlg.cpp : implementation file
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
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMHPCDlg dialog

CMHPCDlg::CMHPCDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMHPCDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMHPCDlg)
	m_uDeadline = 0;
	//}}AFX_DATA_INIT
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pAdapterList = NULL;
	m_pAdapter = NULL;
	m_pBuffer = NULL;
	m_pCatchThread = NULL;
	m_iState = ID_STATE_FREE;
}

void CMHPCDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMHPCDlg)
	DDX_Control(pDX, IDC_CB_ADAPTER, m_AdapterList);
	DDX_Text(pDX, IDC_ED_DEADLINE, m_uDeadline);
	DDV_MinMaxUInt(pDX, m_uDeadline, 20090601, 20150601);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CMHPCDlg, CDialog)
	//{{AFX_MSG_MAP(CMHPCDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMHPCDlg message handlers

BOOL CMHPCDlg::OnInitDialog()
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
	if (!InitAdapterList())
		MessageBox(_T("找不到网卡！"), _T("温馨提示"), MB_OK | MB_ICONWARNING);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CMHPCDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CString strRuijie;
		switch (nID)
		{
		case ID_STATE_START:
			m_iState = ID_STATE_START;
			SetDlgItemText(IDOK, _T("取消"));
			m_pCatchThread = AfxBeginThread(CatchThreadFunc, this, THREAD_PRIORITY_TIME_CRITICAL);
			strRuijie = (LPCTSTR)lParam;
			strRuijie = strRuijie.Left(strRuijie.ReverseFind(_T('\\')))+_T("\\RuijieSupplicant.exe");
			if(ShellExecute(NULL, _T("open"), strRuijie, NULL, NULL, SW_SHOWNORMAL) < (HANDLE)32)
				MessageBox(_T("无法运行锐捷客户端，请手动运行！"), _T("温馨提示"), MB_OK | MB_ICONWARNING);
			break;
		case ID_STATE_SUCCESS:
			SavePackage();
		case ID_STATE_FREE:
			m_iState = ID_STATE_FREE;
			if (m_pBuffer != NULL)
			{
				delete []m_pBuffer;
				m_pBuffer = NULL;
			}
			if (m_pCatchThread != NULL)
			{
				if (WaitForSingleObject(m_pCatchThread->m_hThread, 1000) == WAIT_TIMEOUT)
					TerminateThread(m_pCatchThread->m_hThread, -1);
				m_pCatchThread = NULL;
			}
			SetDlgItemText(IDOK, _T("开始"));
			break;
		default:
			CDialog::OnSysCommand(nID, lParam);
		}
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMHPCDlg::OnPaint() 
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

HCURSOR CMHPCDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}


void CMHPCDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	SendMessage(WM_SYSCOMMAND, ID_STATE_FREE, 0);
	if (m_pAdapterList != NULL)
		delete m_pAdapterList;
	if (m_pAdapter != NULL)
		pcap_close(m_pAdapter);
}

BOOL CMHPCDlg::InitAdapterList()
{
	pcap_if_t *alldevs = NULL;
	char errbuf[PCAP_ERRBUF_SIZE];
	if (pcap_findalldevs(&alldevs, errbuf) == -1)
		return FALSE;
	CString strAdapterName, strDescription;
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

BOOL CMHPCDlg::OpenAdapter()
{
	char errbuf[PCAP_ERRBUF_SIZE];
	CString strAdapterName = m_pAdapterList->GetString(m_AdapterList.GetCurSel());
	m_pAdapter = pcap_open_live(strAdapterName, 65535, 0, 500, errbuf);
	if (m_pAdapter == NULL)
		return FALSE;
	return TRUE;
}

CString CMHPCDlg::GetRuijiePath(bool reg)
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

void CMHPCDlg::OnOK() 
{
	if(m_iState != ID_STATE_FREE)
	{
		SendMessage(WM_SYSCOMMAND, ID_STATE_FREE);
		return;
	}
	if (!UpdateData())
		return;
	if (!OpenAdapter())
	{
		MessageBox(_T("打开网卡失败！"), _T("温馨提示"), MB_OK | MB_ICONWARNING);
		return;
	}
	CString Ruijie = GetRuijiePath(true);
	CFileFind finder;
	while (!finder.FindFile(Ruijie))
	{
		if(MessageBox(_T("未找到锐捷客户端程序8021x.exe，是否手动查找？"), _T("温馨提示"), MB_OKCANCEL | MB_ICONWARNING ) == IDCANCEL)
			return;
		else
			Ruijie = GetRuijiePath(false);
	}
	if(!ReadRuijie(Ruijie))
	{
		MessageBox(_T("文件操作失败！"), _T("温馨提示"), MB_OK | MB_ICONWARNING);
		return;
	}
	SendMessage(WM_SYSCOMMAND, ID_STATE_START, (LPARAM)(LPCTSTR)Ruijie);
}

BOOL CMHPCDlg::ReadRuijie(LPCTSTR Ruijie)
{
	HANDLE hFile = CreateFile(Ruijie, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, 0);
	if (hFile == INVALID_HANDLE_VALUE)
		return FALSE;
	HANDLE hMapping = CreateFileMapping(hFile, 0, PAGE_READONLY|SEC_COMMIT, 0, 0, 0);
	CloseHandle(hFile);
	if (hMapping == NULL)
		return FALSE;
	void *basepointer = MapViewOfFile(hMapping, FILE_MAP_READ, 0, 0, 0);
	if (basepointer == NULL)
	{
		CloseHandle(hMapping);
		return FALSE;
	}
	IMAGE_DOS_HEADER *pDosHeader = (IMAGE_DOS_HEADER *)basepointer;
	IMAGE_NT_HEADERS *pNtHeaders = (IMAGE_NT_HEADERS *)((char *)pDosHeader + pDosHeader->e_lfanew);
	IMAGE_FILE_HEADER *pFileHeader = (IMAGE_FILE_HEADER *)((char *)pNtHeaders + sizeof(IMAGE_NT_SIGNATURE));
	IMAGE_SECTION_HEADER *pSectionHeader = (IMAGE_SECTION_HEADER *)((char *)pNtHeaders + sizeof(IMAGE_NT_HEADERS));
	int nSectionCount = pFileHeader->NumberOfSections;
	char szSectionName[9];
	int i;
	for (i=0; i<nSectionCount; i++)
	{
		memcpy(szSectionName, pSectionHeader->Name, IMAGE_SIZEOF_SHORT_NAME);
		szSectionName[8] = '\0';
		if (strcmp(szSectionName, ".text") == 0)
			break;
		pSectionHeader++;
	}
	if (i == nSectionCount)
	{
		CloseHandle(hMapping);
		return FALSE;
	}
	m_nReadSzie = pSectionHeader->Misc.VirtualSize;
	m_pBuffer = new BYTE[m_nReadSzie + 0x210];
	memcpy(m_pBuffer+0x10, (BYTE *)basepointer+pSectionHeader->PointerToRawData, m_nReadSzie);
	CloseHandle(hMapping);
	*(u_int32_t *)(m_pBuffer + 4) = m_uDeadline;
	*(u_int32_t *)(m_pBuffer + 8) = m_nReadSzie;
	*(u_int32_t *)(m_pBuffer + 12) = pSectionHeader->SizeOfRawData;
	srand((unsigned)time(0));
	for (i=0; i<4; i++)
	{
		m_pBuffer[i] = rand() % 256;
		m_pBuffer[i+4] ^= m_pBuffer[i];
		m_pBuffer[i+8] ^= m_pBuffer[i];
		m_pBuffer[i+12] ^= m_pBuffer[i];
	}
	m_nReadSzie += 16;
	for (i=16; i<m_nReadSzie; i+=16)
	{
		for (int j=0; j<16; j++)
			m_pBuffer[i+j] ^= m_pBuffer[j];
	}
	return TRUE;
}

UINT CatchThreadFunc(LPVOID pParam)
{
	CMHPCDlg *mainDlg = (CMHPCDlg *)pParam;
	BYTE *pRecvBuf;			//指向buf1，接收到的数据包
	BYTE *pRecvHeaderBuf;	//指向buf2，接收到的数据头
	while (mainDlg->m_iState != ID_STATE_FREE)
	{		
		if (pcap_next_ex(mainDlg->m_pAdapter ,(pcap_pkthdr **)&pRecvHeaderBuf, (const u_char**)&pRecvBuf) != 1 )
			continue;
		if (pRecvBuf[0x0c]==0x88 && pRecvBuf[0x0d]==0x8e)
		{
			if(pRecvBuf[0x0f]==0x01 && mainDlg->m_iState==ID_STATE_START)
			{
				memcpy(pRecvBuf+0x94, "MACMAC", 6);
				memset(pRecvBuf+0xb3, 0, 16);
				memset(pRecvBuf+0xcb, 0, 16);
				memcpy(mainDlg->m_pBuffer + mainDlg->m_nReadSzie, pRecvBuf+0x29, 0x200);
				mainDlg->m_nReadSzie += 0x200;
				mainDlg->m_iState = ID_STATE_FREE;
				mainDlg->PostMessage(WM_SYSCOMMAND, ID_STATE_SUCCESS);
			}
		}
	}
	return 0;
}

void CMHPCDlg::SavePackage()
{
	CFileDialog dlg(FALSE, _T("mpf"), NULL, OFN_ENABLESIZING, _T("MentoHUST 数据包文件(*.mpf)|*.mpf|所有文件(*.*)|*.*||"), NULL);
	dlg.m_ofn.lStructSize = 88;
	if (dlg.DoModal() == IDCANCEL)
	{
		SendMessage(WM_SYSCOMMAND, ID_STATE_FREE);
		return;
	}
	try
	{
		CFile File(dlg.GetPathName(), CFile::modeCreate|CFile::modeWrite|CFile::typeBinary);
		File.Write(m_pBuffer, m_nReadSzie);
		File.Close();
	}catch(...)
	{
		MessageBox(_T("保存数据失败！"), _T("温馨提示"), MB_OK| MB_ICONWARNING);
		return;
	}
	MessageBox(_T("相关数据已成功保存！"), _T("温馨提示"), MB_OK| MB_ICONWARNING );
}