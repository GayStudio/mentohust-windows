// MentoHUSTDlg.cpp : 实现文件
//Written By HustMoon

#include "stdafx.h"
#include "md5.h"
#include "MentoHUST.h"
#include "MentoHUSTDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CMentoHUSTDlg 对话框

CMentoHUSTDlg::CMentoHUSTDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMentoHUSTDlg::IDD, pParent) 
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_ServerMsg = _T("");
	m_iState = IDT_DISCONNECT;
	m_sName = _T("");
	m_sPass = _T("");
	m_NicHead = NULL;
	m_pCurrentNic = NULL;
	m_iCurrentNic = 0;
	m_CertThread = NULL;
	m_DhcpThread = NULL;
}

void CMentoHUSTDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MSGLIST, m_MsgList);
	DDX_Text(pDX, IDC_EDIT_NAME, m_sName);
	DDV_MaxChars(pDX, m_sName, 65);
	DDX_Text(pDX, IDC_EDIT_PASS, m_sPass);
	DDV_MaxChars(pDX, m_sPass, 63);
	DDX_Control(pDX, IDC_NICLIST, m_NicList);
}

BEGIN_MESSAGE_MAP(CMentoHUSTDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	//ON_REGISTERED_MESSAGE(WM_TASKBARCREATED, OnTaskBarCreated)
	ON_BN_CLICKED(IDC_BN_EXIT, CMentoHUSTDlg::OnClose)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BN_OUTPUT, CMentoHUSTDlg::OnBnOutput)
	ON_BN_CLICKED(IDC_BN_OPTION, CMentoHUSTDlg::OnBnOption)
	ON_BN_CLICKED(IDC_BN_ABOUT, CMentoHUSTDlg::OnBnAbout)
	ON_BN_CLICKED(IDC_BN_HELP, CMentoHUSTDlg::OnBnHelp)
	ON_BN_CLICKED(IDC_BN_CLEAR, CMentoHUSTDlg::OnBnClear)
	ON_BN_CLICKED(IDC_BN_CONNECT, CMentoHUSTDlg::OnBnConnect)
	ON_WM_DESTROY()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CMentoHUSTDlg 消息处理程序

BOOL CMentoHUSTDlg::OnInitDialog()
{
	SetProp(m_hWnd, GUI_ID, (HANDLE)1);
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE|WS_EX_TOPMOST);
	SetHomePath();
	GetCfg();
	if(!InitNetworkAdapter() || m_NicHead == NULL)
	{
		Report(_T("找不到网卡！"), 1);
		GetDlgItem( IDC_BN_CONNECT )->EnableWindow(FALSE);
	}
	InitInterface();
	if( m_bAutoCert ) OnBnConnect();
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CMentoHUSTDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
		OnBnAbout();
	else
		switch( nID )
	{
		case SC_MINIMIZE:
			ToTray(true);
			break;
		case IDT_BIND_ARP:
			if(m_bBindArp == 2 && memcmp(bGateMAC, bTempMAC, 6)!=0)
			{
				CString strTemp;
				strTemp.Format(_T("!! ARP欺骗源: %02x:%02x:%02x:%02x:%02x:%02x"), bTempMAC[0], bTempMAC[1], bTempMAC[2], bTempMAC[3], bTempMAC[4], bTempMAC[5]);
				Report(strTemp);
				SendArpPackage();
			}
			break;
		case IDT_INIT_DATA:
			InitCertData();
			break;
		case IDT_DHCP_RENEW:
		case IDT_DISCONNECT:
		case IDT_FIND_SERVER:
		case IDT_ACK_NAME:
		case IDT_ACK_PSWD:
		case IDT_ACK_ECHO:
			SwitchState( nID );
			break;
		default:
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMentoHUSTDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CMentoHUSTDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CMentoHUSTDlg::SwitchState(int IState)
{
	m_iState = IState;
	switch( m_iState )
	{
	case IDT_DHCP_RENEW:
		TimerCtrl(0);
		if ( m_iDhcpMode == 1 ) m_iDhcpMode = 4;
		if ( m_iDhcpMode == 2 ) m_iDhcpMode = 5;
		if(m_DhcpThread == NULL)
			m_DhcpThread = AfxBeginThread(DhcpThreadFunc, this, THREAD_PRIORITY_TIME_CRITICAL);
		break;
	case IDT_FIND_SERVER:
		if(m_bPackage)
			InitPackage(AfxGetApp()->GetProfileString(_T("Parameters"), _T("PackagePath"), _T("")), true);
		else
			InitDefault(true);
		iErrorTimes = 0;
		iLastState = IDT_DISCONNECT;
		Report(_T(">> 寻找服务器..."));
		if(m_CertThread == NULL)
			m_CertThread = AfxBeginThread(CertThreadFunc, this, THREAD_PRIORITY_TIME_CRITICAL);
		TimerCtrl(m_iTimeOut);
		SendPackage();
		break;
	case IDT_ACK_NAME:
		Report(_T(">> 发送用户名...") );
		TimerCtrl(m_iTimeOut);
		SendPackage();
		break;
	case IDT_ACK_PSWD:
		Report(_T(">> 发送密码...") );
		if(m_bPackage)
			InitPackage(AfxGetApp()->GetProfileString(_T("Parameters"), _T("PackagePath"), _T("")), false);
		else
			InitDefault(false);
		TimerCtrl(m_iTimeOut);
		SendPackage();
		break;
	case IDT_ACK_ECHO:
		SendPackage();
		if(m_iDhcpMode == 1 || m_iDhcpMode == 2)
			SendMessage(WM_SYSCOMMAND, IDT_DHCP_RENEW);
		else
		{
			TimerCtrl(m_iEcho);
			if(m_bAutoMin)
				ToTray(true);
		}
		break;
	case IDT_DISCONNECT:
		m_ServerMsg.Empty();
		GetDlgItem( IDC_BN_CONNECT )->EnableWindow(FALSE);
		TimerCtrl(0);
		if(m_DhcpThread)
		{
			WaitForSingleObject(m_DhcpThread->m_hThread, INFINITE);
			m_DhcpThread = NULL;
		}
		if(m_CertThread)
		{
			WaitForSingleObject(m_CertThread->m_hThread, INFINITE);
			m_CertThread = NULL;
		}
		if(m_iDhcpMode > 2)
		{
			m_DhcpThread = AfxBeginThread(DhcpThreadFunc, this, THREAD_PRIORITY_TIME_CRITICAL);
			WaitForSingleObject(m_DhcpThread->m_hThread, INFINITE);
			m_DhcpThread = NULL;
			if( m_iDhcpMode == 4 ) m_iDhcpMode = 1;
			if( m_iDhcpMode == 5 ) m_iDhcpMode = 2;
		}
		else
			Report(_T(":: 认证失败或未认证！"));
		SendLogoffPackage();
		break;
	}
	GetDlgItem( IDC_BN_CONNECT )->EnableWindow(TRUE);
	if( Minimized() )
		ToTray(true);
	if( m_iState == IDT_DISCONNECT)
	{
		GetDlgItem( IDC_BN_CONNECT )->SetWindowText( _T("认证") );
		GetDlgItem( IDC_NICLIST )->EnableWindow(TRUE);
		GetDlgItem( IDC_EDIT_NAME )->EnableWindow(TRUE);
		GetDlgItem( IDC_EDIT_PASS )->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem( IDC_BN_CONNECT )->SetWindowText( _T("断开") );
		GetDlgItem( IDC_NICLIST )->EnableWindow(FALSE);
		GetDlgItem( IDC_EDIT_NAME )->EnableWindow(FALSE);
		GetDlgItem( IDC_EDIT_PASS )->EnableWindow(FALSE);
	}
}

void CMentoHUSTDlg::Report(CString Msg, int Type, CString Title)
{
	if(m_MsgList.GetCount() >= 100)
	{
		m_MsgList.ResetContent();
		Report(_T(":: 输出超过100, 自动清空。"), 3);
	}
	if(Type == 1)
		MessageBox(Msg, Title, MB_OK | MB_ICONWARNING);
	else if(Type == 2)
		GetDlgItem( IDC_STATE ) -> SetWindowText( Msg );
	else if(Type ==3)
		m_MsgList.AddString(Msg); 
	else
	{
		GetDlgItem( IDC_STATE ) -> SetWindowText( Msg );
		m_MsgList. AddString(Msg); 
	}
	m_MsgList.UpdateWindow();
}

void CMentoHUSTDlg::OnOK()	//解决回车退出
{
	OnBnConnect();
}

void CMentoHUSTDlg::OnCancel()	//解决ESC键退出
{
	if (MessageBox(_T("确定退出？"), _T("退出提示"), MB_OKCANCEL | MB_ICONWARNING ) == IDOK)
	{
		CDialog::OnCancel();
	}
}

void CMentoHUSTDlg::OnClose()
{
	OnCancel();
}

void CMentoHUSTDlg::OnBnOutput()
{
	RECT dlgRect, listRect;
	GetWindowRect(&dlgRect);
	m_MsgList.GetWindowRect(&listRect);
	CString WinState;
	GetDlgItem( IDC_BN_OUTPUT )->GetWindowText( WinState );
	if ( WinState == _T("输出↓"))
	{
		MoveWindow( dlgRect.left, dlgRect.top, dlgRect.right - dlgRect.left,  listRect.bottom - dlgRect.top+34, true );
		GetDlgItem( IDC_BN_OUTPUT )->SetWindowText( _T("输出↑") );
	}
	else
	{
		MoveWindow( dlgRect.left, dlgRect.top, dlgRect.right - dlgRect.left, listRect.top - dlgRect.top, true );
		GetDlgItem( IDC_BN_OUTPUT )->SetWindowText(_T("输出↓"));
	}
}

void CMentoHUSTDlg::OnBnOption()
{
	CString WinState;
	GetDlgItem( IDC_BN_OUTPUT )->GetWindowText( WinState );
	if ( WinState == _T("输出↑"))
		OnBnOutput();
	CCfgDlg dlgCfg;
	dlgCfg.DoModal();
	SetHomePath();
	if ( WinState == _T("输出↑"))
		OnBnOutput();
}

void CMentoHUSTDlg::OnBnAbout()
{
	CString WinState;
	GetDlgItem( IDC_BN_OUTPUT )->GetWindowText( WinState );
	if ( WinState == _T("输出↑"))
		OnBnOutput();
	CAboutDlg dlgAbout;
	dlgAbout.DoModal();
	if ( WinState == _T("输出↑"))
		OnBnOutput();
}

void CMentoHUSTDlg::OnBnHelp()
{
	if(ShellExecute(NULL, _T("open"), _T("MentoHUST.chm"), NULL, NULL, SW_SHOWNORMAL) < (HANDLE)32)
		Report(_T("在程序所在目录找不到帮助文件 MentoHUST.chm！"), 1);
}

void CMentoHUSTDlg::OnBnClear()
{
	m_MsgList.ResetContent();
	m_ServerMsg.Empty();
}

void CMentoHUSTDlg::InitInterface()
{
	m_tnid.cbSize = (DWORD)sizeof(NOTIFYICONDATA);	//托盘图标参数
	m_tnid.hWnd = GetSafeHwnd();
	m_tnid.uID = 0;
	m_tnid.uFlags = NIF_ICON|NIF_MESSAGE|NIF_TIP|NIF_INFO;
	m_tnid.uTimeout = 10000;
	m_tnid.uCallbackMessage = WM_COMMAND;
	RECT dlgRect, listRect;
	GetWindowRect(&dlgRect);
	m_MsgList.GetWindowRect(&listRect);
	MoveWindow( dlgRect.left, dlgRect.top, dlgRect.right - dlgRect.left, listRect.top - dlgRect.top, true );
	GetDlgItem( IDC_BN_OUTPUT )->SetWindowText(_T("输出↓"));
}

void SetHomePath(void)
{
	TCHAR Filename[MAX_PATH];
	GetModuleFileName(AfxGetInstanceHandle(), Filename, MAX_PATH);
	CString TempStr = Filename;
	TempStr = TempStr.Left(TempStr.ReverseFind('\\')+1);
	SetCurrentDirectory(TempStr);
}

void CMentoHUSTDlg::ChangeTrayIcon()
{
	lstrcpy(m_tnid.szInfo, m_ServerMsg);
	lstrcpy(m_tnid.szInfoTitle, _T("系统信息"));
	m_tnid.dwInfoFlags = NIIF_INFO;
	switch(m_iState)
	{
	case IDT_DISCONNECT:
		m_tnid.hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_FAILED));
		lstrcpy(m_tnid.szTip, _T("MentoHUST--未认证！"));
		break;
	case IDT_DHCP_RENEW:
		m_tnid.hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_CERTTING));
		lstrcpy(m_tnid.szTip, _T("MentoHUST--获取IP..."));
		break;
	case IDT_FIND_SERVER:
		m_tnid.hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_CERTTING));
		lstrcpy(m_tnid.szTip, _T("MentoHUST--寻找服务器..."));
		break;
	case IDT_ACK_NAME:
		m_tnid.hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_CERTTING));
		lstrcpy(m_tnid.szTip, _T("MentoHUST--发送用户名..."));
		break;
	case IDT_ACK_PSWD:
		m_tnid.hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_CERTTING));
		lstrcpy(m_tnid.szTip, _T("MentoHUST--发送密码..."));
		break;
	case IDT_ACK_ECHO:
		m_tnid.hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));
		lstrcpy(m_tnid.szTip, _T("MentoHUST-认证成功！"));
		break;
	}
}

bool CMentoHUSTDlg::Minimized()
{
	WINDOWPLACEMENT wndpl;
	GetWindowPlacement(&wndpl);
	if( wndpl.showCmd == SW_SHOWMINIMIZED )
		return true;
	return false;
}

void CMentoHUSTDlg::ToTray(bool hide)
{
	ChangeTrayIcon();
	if( hide )
	{
		if( Minimized() )
		{
			Shell_NotifyIcon(NIM_MODIFY, &m_tnid);	//更改托盘区图标
		}
		else
		{
			ShowWindow(SW_MINIMIZE);
			ShowWindow(SW_HIDE);
			Shell_NotifyIcon(NIM_ADD, &m_tnid);		//添加托盘区图标
		}
	}
	else
	{
		ShowWindow(SW_SHOWNORMAL);
		Shell_NotifyIcon(NIM_DELETE, &m_tnid);		//删除托盘区图标
	}
}

/*LRESULT CMentoHUSTDlg::OnTaskBarCreated(WPARAM wParam, LPARAM lParam)	//任务栏重启后重建图标
{
	if( Minimized() )
	{
		ChangeTrayIcon();
		Shell_NotifyIcon(NIM_ADD, &m_tnid);    //添加托盘区图标
	}
	return 0;
}*/

LRESULT CMentoHUSTDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if (message == WM_QUERYENDSESSION)
	{
		m_bBindArp = 0;
		CDialog::OnCancel();
	}
	else if (message == WM_TASKBARCREATED && Minimized())
	{
		ChangeTrayIcon();
		Shell_NotifyIcon(NIM_ADD, &m_tnid);    //添加托盘区图标
	}
	return CDialog::WindowProc(message, wParam, lParam);
}

BOOL CMentoHUSTDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	// TODO: 在此添加专用代码和/或调用基类
	switch ( wParam )
	{
	case IDSHELL_RESTORE:
		ToTray(false);
		break;
	case IDSHELL_CONNECT:
		OnBnConnect();
		break;
	case IDSHELL_CFG:
		{
			OnBnOption();
			ToTray(false);
			ToTray(true);
		}
		break;
	case IDSHELL_HELP:
		OnBnHelp();
		break;
	case IDM_ABOUTBOX:
		{
			OnBnAbout();
			ToTray(false);
			ToTray(true);
		}
		break;
	case IDSHELL_EXIT:
		CDialog::OnCancel();
		break;
	case 0:
		switch (lParam)
		{
		case WM_LBUTTONDBLCLK:
			ToTray( false );
			break;
		case WM_RBUTTONUP:
			LPPOINT lpoint = new tagPOINT; 
			::GetCursorPos(lpoint);//得到鼠标位置 
			CMenu menu;
			menu.CreatePopupMenu();	//声明一个弹出式菜单 
			menu.AppendMenu(MF_ENABLED, IDSHELL_RESTORE, _T("显示窗口(&S)"));
			menu.AppendMenu(MF_ENABLED, MF_SEPARATOR);
			if( m_NicHead ==NULL ) menu.AppendMenu(MF_GRAYED, IDSHELL_CONNECT, _T("认证(&C)"));
			else if( m_iState == IDT_DISCONNECT ) menu.AppendMenu(MF_ENABLED, IDSHELL_CONNECT, _T("认证(&C)"));
			else menu.AppendMenu(MF_ENABLED, IDSHELL_CONNECT, _T("断开(&D)"));
			menu.AppendMenu(MF_ENABLED, MF_SEPARATOR);
			menu.AppendMenu(MF_ENABLED, IDSHELL_CFG, _T("设置(&P)"));
			menu.AppendMenu(MF_ENABLED, MF_SEPARATOR);
			menu.AppendMenu(MF_ENABLED, IDSHELL_HELP, _T("帮助(&H)"));
			menu.AppendMenu(MF_ENABLED, IDM_ABOUTBOX, _T("关于(&A)"));
			menu.AppendMenu(MF_ENABLED, MF_SEPARATOR);
			menu.AppendMenu(MF_ENABLED, IDSHELL_EXIT, _T("退出(&E)"));
			::SetForegroundWindow(m_hWnd);
			menu.TrackPopupMenu(TPM_LEFTALIGN, lpoint->x, lpoint->y, this);	//资源回收
			menu.Detach();
			menu.DestroyMenu();
			delete lpoint;
			break;
		}
	}
	return CDialog::OnCommand(wParam, lParam);
}
void CMentoHUSTDlg::OnDestroy()
{
	CDialog::OnDestroy();
	// TODO: 在此处添加消息处理程序代码
	if(m_iState != IDT_DISCONNECT)
		SendMessage( WM_SYSCOMMAND, IDT_DISCONNECT );
	if(m_bBindArp == 2)
	{
		CString strTemp;
		strTemp.Format(_T("-d %d.%d.%d.%d"), bGateIP.bt[0], bGateIP.bt[1], bGateIP.bt[2], bGateIP.bt[3]);
		ShellExecute(NULL, _T("open"), _T("arp"), strTemp, NULL, SW_HIDE);
	}
	if ( m_NicHead != NULL )
	{
		delete m_NicHead;
		m_NicHead = NULL;
	}
	if( Minimized()) Shell_NotifyIcon(NIM_DELETE, &m_tnid);
	if(m_CertThread)
	{
		if(WaitForSingleObject(m_CertThread->m_hThread, 3000) == WAIT_TIMEOUT)
			TerminateThread(m_CertThread->m_hThread, 0);
		m_CertThread = NULL;
	}
}

bool CMentoHUSTDlg::InitNetworkAdapter(void)
{
	pcap_if_t *alldevs = NULL;
	pcap_t *adhandle = NULL;
	char errbuf[PCAP_ERRBUF_SIZE];
	NicNode *pNicTemp;

	if (pcap_findalldevs(&alldevs, errbuf) == -1)
		return false;
	for(pcap_if_t *d=alldevs; d; d=d->next)
	{
		CString strName(d->name);
		strName = strName.Right(38);
		CString strDescription(d->description);
		if ((adhandle = pcap_open_live(d->name, 65535, 0, 500, errbuf )) == NULL)
			continue;
		if ((d->flags & PCAP_IF_LOOPBACK) != 1)
		{
			pNicTemp = new NicNode(strName, adhandle);
			if (m_NicHead == NULL)
				m_NicHead = pNicTemp;
			else
				m_NicHead->AddNext(pNicTemp);
			m_NicList.AddString(strDescription);
			pNicTemp = NULL;
		}
	}
	pcap_freealldevs(alldevs);
	if (m_iCurrentNic >= m_NicList.GetCount() || m_iCurrentNic<0)
		m_iCurrentNic = 0;
	m_NicList.SetCurSel(m_iCurrentNic);
	return true;
}

BYTE Alog(BYTE base)	//简化 By HustMoon
{
	BYTE result=0;
	for(int i=0; i<8; i++)
	{
		result <<= 1;
		result |= base&0x01;
		base >>= 1;
	}
    return(~result);
}

void CMentoHUSTDlg::Blog(BYTE *circleBase, int num, BYTE *checkSum)	//简化 By HustMoon
{
    BYTE Table[]=
    {
        0x00,0x00,0x21,0x10,0x42,0x20,0x63,0x30,0x84,0x40,0xA5,0x50,0xC6,0x60,0xE7,0x70, 
        0x08,0x81,0x29,0x91,0x4A,0xA1,0x6B,0xB1,0x8C,0xC1,0xAD,0xD1,0xCE,0xE1,0xEF,0xF1, 
        0x31,0x12,0x10,0x02,0x73,0x32,0x52,0x22,0xB5,0x52,0x94,0x42,0xF7,0x72,0xD6,0x62, 
        0x39,0x93,0x18,0x83,0x7B,0xB3,0x5A,0xA3,0xBD,0xD3,0x9C,0xC3,0xFF,0xF3,0xDE,0xE3, 
        0x62,0x24,0x43,0x34,0x20,0x04,0x01,0x14,0xE6,0x64,0xC7,0x74,0xA4,0x44,0x85,0x54, 
        0x6A,0xA5,0x4B,0xB5,0x28,0x85,0x09,0x95,0xEE,0xE5,0xCF,0xF5,0xAC,0xC5,0x8D,0xD5, 
        0x53,0x36,0x72,0x26,0x11,0x16,0x30,0x06,0xD7,0x76,0xF6,0x66,0x95,0x56,0xB4,0x46, 
        0x5B,0xB7,0x7A,0xA7,0x19,0x97,0x38,0x87,0xDF,0xF7,0xFE,0xE7,0x9D,0xD7,0xBC,0xC7, 
        0xC4,0x48,0xE5,0x58,0x86,0x68,0xA7,0x78,0x40,0x08,0x61,0x18,0x02,0x28,0x23,0x38, 
        0xCC,0xC9,0xED,0xD9,0x8E,0xE9,0xAF,0xF9,0x48,0x89,0x69,0x99,0x0A,0xA9,0x2B,0xB9, 
        0xF5,0x5A,0xD4,0x4A,0xB7,0x7A,0x96,0x6A,0x71,0x1A,0x50,0x0A,0x33,0x3A,0x12,0x2A, 
        0xFD,0xDB,0xDC,0xCB,0xBF,0xFB,0x9E,0xEB,0x79,0x9B,0x58,0x8B,0x3B,0xBB,0x1A,0xAB, 
        0xA6,0x6C,0x87,0x7C,0xE4,0x4C,0xC5,0x5C,0x22,0x2C,0x03,0x3C,0x60,0x0C,0x41,0x1C, 
        0xAE,0xED,0x8F,0xFD,0xEC,0xCD,0xCD,0xDD,0x2A,0xAD,0x0B,0xBD,0x68,0x8D,0x49,0x9D, 
        0x97,0x7E,0xB6,0x6E,0xD5,0x5E,0xF4,0x4E,0x13,0x3E,0x32,0x2E,0x51,0x1E,0x70,0x0E, 
        0x9F,0xFF,0xBE,0xEF,0xDD,0xDF,0xFC,0xCF,0x1B,0xBF,0x3A,0xAF,0x59,0x9F,0x78,0x8F, 
        0x88,0x91,0xA9,0x81,0xCA,0xB1,0xEB,0xA1,0x0C,0xD1,0x2D,0xC1,0x4E,0xF1,0x6F,0xE1, 
        0x80,0x10,0xA1,0x00,0xC2,0x30,0xE3,0x20,0x04,0x50,0x25,0x40,0x46,0x70,0x67,0x60, 
        0xB9,0x83,0x98,0x93,0xFB,0xA3,0xDA,0xB3,0x3D,0xC3,0x1C,0xD3,0x7F,0xE3,0x5E,0xF3, 
        0xB1,0x02,0x90,0x12,0xF3,0x22,0xD2,0x32,0x35,0x42,0x14,0x52,0x77,0x62,0x56,0x72, 
        0xEA,0xB5,0xCB,0xA5,0xA8,0x95,0x89,0x85,0x6E,0xF5,0x4F,0xE5,0x2C,0xD5,0x0D,0xC5, 
        0xE2,0x34,0xC3,0x24,0xA0,0x14,0x81,0x04,0x66,0x74,0x47,0x64,0x24,0x54,0x05,0x44, 
        0xDB,0xA7,0xFA,0xB7,0x99,0x87,0xB8,0x97,0x5F,0xE7,0x7E,0xF7,0x1D,0xC7,0x3C,0xD7, 
        0xD3,0x26,0xF2,0x36,0x91,0x06,0xB0,0x16,0x57,0x66,0x76,0x76,0x15,0x46,0x34,0x56, 
        0x4C,0xD9,0x6D,0xC9,0x0E,0xF9,0x2F,0xE9,0xC8,0x99,0xE9,0x89,0x8A,0xB9,0xAB,0xA9, 
        0x44,0x58,0x65,0x48,0x06,0x78,0x27,0x68,0xC0,0x18,0xE1,0x08,0x82,0x38,0xA3,0x28, 
        0x7D,0xCB,0x5C,0xDB,0x3F,0xEB,0x1E,0xFB,0xF9,0x8B,0xD8,0x9B,0xBB,0xAB,0x9A,0xBB, 
        0x75,0x4A,0x54,0x5A,0x37,0x6A,0x16,0x7A,0xF1,0x0A,0xD0,0x1A,0xB3,0x2A,0x92,0x3A, 
        0x2E,0xFD,0x0F,0xED,0x6C,0xDD,0x4D,0xCD,0xAA,0xBD,0x8B,0xAD,0xE8,0x9D,0xC9,0x8D, 
        0x26,0x7C,0x07,0x6C,0x64,0x5C,0x45,0x4C,0xA2,0x3C,0x83,0x2C,0xE0,0x1C,0xC1,0x0C, 
        0x1F,0xEF,0x3E,0xFF,0x5D,0xCF,0x7C,0xDF,0x9B,0xAF,0xBA,0xBF,0xD9,0x8F,0xF8,0x9F, 
        0x17,0x6E,0x36,0x7E,0x55,0x4E,0x74,0x5E,0x93,0x2E,0xB2,0x3E,0xD1,0x0E,0xF0,0x1E
    };
	int i, index;
    for(i=0, checkSum[0] = 0, checkSum[1] = 0; i<num; i++)
    {
        index = checkSum[0] ^ circleBase[i];
        checkSum[0] = checkSum[1] ^ Table[index*2+1];
        checkSum[1] = Table[index*2];
    }
}

void CMentoHUSTDlg::Clog(CString sPath)
{
	int i, nBufferSize;
	CString Temp = _T("");
	CString Hash = _T("");
	Temp.Format(_T("客户端版本: %d.%d.%d.%d"), Package[0x3B], Package[0x3C], Package[0x3D], Package[0x3E]);
	Report(Temp, 3);
	unsigned long Version = htonl(*(ULONG *)(Package+0x3B));
	if(Version < 0x02380000)		//2.56.0.0之前
	{
		Report(_T("该版本没有客户端校验。"), 3);
		return;
	}
	else if(Version == 0x02380000)	//2.56.0.0
		nBufferSize = 0x4A00;
	else if(Version == 0x03020000)	//3.02.0.0
		nBufferSize = 0x6400;
	else if(Version == 0x03050000)	//3.05.0.0
		nBufferSize = 0x6400;
	else if(Version == 0x03160000)	//3.22.0.0
		nBufferSize = 0x8C00;
	else if(Version == 0x031E0000)	//3.30.0.0
		nBufferSize = 0x8E00;
	else							//暂时未知
	{
		nBufferSize = 0;
		Report(_T("启用智能推断功能, 但不保证完全正确。"), 3);
	}
	BYTE *Buffer = new BYTE[MD5_PART_LENGTH+0x10];
	BYTE *Table = new BYTE[144];
	memset(Table, 0, 144);
	BYTE *md5rev = bMD5Source+0x08;
	BYTE *md5Dig;
	try
	{		
		CFile File(sPath, CFile::modeRead | CFile::shareDenyWrite | CFile::typeBinary);
		File.Read(Buffer+16, MD5_PART_LENGTH);
		File.Close();
	}
	catch (CFileException *pEx)
	{
		Report(_T(">> 打开文件出错, 将使用默认包认证"), 3);
		InitDefault(false);
		pEx->Delete();
		goto clear;
	}
	if(nBufferSize == 0)
	{
		for(i=0; i<MD5_PART_LENGTH && memcmp(Buffer+16+i, Table, 16); i+=16);
		for(; i<MD5_PART_LENGTH; i+=16)
		{
			if(memcmp(Buffer+16+i, Table, 16))
			{
				nBufferSize = i/8;
				break;
			}
		}
		if(i>=MD5_PART_LENGTH)
		{
			Report(_T("暂不支持该数据包, 将使用默认包认证"), 3);
			InitDefault(false);
			goto clear;
		}
	}
	for(i=0; i<8; i++)
	{
		memcpy(Buffer+nBufferSize*i, md5rev, 16);
		md5Dig = ComputeHash(Buffer+nBufferSize*i, nBufferSize+16);
		Table[18*i] = md5rev[2*i];
		memcpy(Table+18*i+1, md5Dig, 16);
		Table[18*i+17] = md5rev[2*i+1];
	}
	md5Dig = ComputeHash(Table, 144);
	for(i=0; i<16; i++)
	{
		Temp.Format(_T("%02x"), md5Dig[i]);
		Hash = Hash + Temp;
	}
	Report(_T(" MD5校验值: ")+Hash, 3);
	for(i=0; i<32; i++)
		Package[0x4E+i] = (BYTE)Hash.GetBuffer(0)[i];
	
clear:
	delete []Buffer;
	delete []Table;
}

TCHAR CMentoHUSTDlg::OneCode(TCHAR srcchar)
{
	BYTE first = (BYTE)((USHORT)srcchar>>8);
	BYTE second = (BYTE)srcchar;
	first = ((BYTE)~Alog(first)>>1) | (first&0x80);
	second = ((BYTE)~Alog(second)>>1) | (second&0x80);
	return (TCHAR)(first<<8|second);
}

void CMentoHUSTDlg::CodeData(CString Str1, CString Str2, CString &AStr1, CString &AStr2)
{
	int i, j;
	char key[]={5, 3, 7, 4, 2, 4, 9, 3, 7, 0};
	TCHAR *str = Str1.GetBuffer(Str1.GetLength());
	Str1.ReleaseBuffer();
	for(i=0, j=0; i<Str1.GetLength(); i++, j++)
	{
#ifdef UNICODE
		str[i] += key[j];
		if(j==9) j=-1;
#endif
		str[i] = OneCode(str[i]);
	}
	AStr1 = str;
	str = Str2.GetBuffer(Str2.GetLength());
	Str2.ReleaseBuffer();
	for(i=0, j=0; i<Str2.GetLength(); i++, j++)
	{
		str[i] = OneCode(str[i]);
#ifdef UNICODE
		str[i] -= key[j];
		if(j==9) j=-1;
#endif
	}
	AStr2 = str;
}

int CMentoHUSTDlg::Decode(char *dst, char *src)
{
	char tab[] = {"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"};
	char xor[] = {"~!:?$*<(qw2e5o7i8x12c6m67s98w43d2l45we82q3iuu1z4xle23rt4oxclle34e54u6r8m"};
	unsigned long esi = 0, tmp = 0;
	unsigned i, j=0, ts = 0;
	for(i=0; src[i]!='\0'; i++)
	{
		if (src[i] == '=')
		{
			if (++ts > 2)
				return -1;
		}
		else
		{
			for(tmp=0; tab[tmp]!='\0'; tmp++)
			{
				if(tab[tmp] == src[i])
					break;
			}
			if (tab[tmp] == '\0')
				return -1;
			esi += tmp;
		}
		if(i%4 == 3)
		{
			dst[j++] = (char)(esi>>16);
			if(ts < 2)
				dst[j++] = (char)(esi>>8);
			if(ts < 1)
				dst[j++] = (char)esi;
			esi = ts = 0;
		}
		esi <<= 6;
	}
	if (i%4!=0 || strlen(xor)<j)
		return -1;
	for(i=0; i<j; i++)
		dst[i] ^= xor[i];
	dst[j] = '\0';
	return 0;
}

void CMentoHUSTDlg::GetPass(CString &Name, CString &Pass)
{
	HKEY hKey;
	wchar_t wname[90], wpass[90];
	char tmp[90], *name, *pass;
	int iSize;
	LPBYTE lpName = (LPBYTE)wname, lpPass = (LPBYTE)wpass;
	DWORD type = REG_SZ;
	DWORD cbData = 180;
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\锐捷网络\\Ruijie Supplicant"), 0, KEY_READ, &hKey) != ERROR_SUCCESS
		|| RegQueryValueEx(hKey, _T("UserName"), NULL, &type, lpName, &cbData) != ERROR_SUCCESS
		|| RegQueryValueEx(hKey, _T("Password"), NULL, &type, lpPass, &cbData) != ERROR_SUCCESS)
		return;
	RegCloseKey(hKey);
	iSize = WideCharToMultiByte(CP_ACP, 0, wname, -1, NULL, 0, NULL, NULL);
	name = new char[iSize];
	WideCharToMultiByte(CP_ACP, 0, wname, -1, name, iSize, NULL, NULL);
	iSize = WideCharToMultiByte(CP_ACP, 0, wpass, -1, NULL, 0, NULL, NULL);
	pass = new char[iSize];
	WideCharToMultiByte(CP_ACP, 0, wpass, -1, pass, iSize, NULL, NULL); 
	if (Decode(tmp, name) == 0)
		Name = tmp;
	if (Decode(tmp, pass) == 0)
		Pass = tmp;
	delete []name;
	delete []pass;
}

void CMentoHUSTDlg::SaveCfg()
{
	UpdateData();
	if((m_iCurrentNic = m_NicList.GetCurSel())==-1) m_iCurrentNic = 0;
	CString Name, Pass;
	CodeData(m_sName, m_sPass, Name, Pass);
	if( !m_bSavePass )
		Pass = _T("");
	if(!AfxGetApp()->WriteProfileInt(_T("Parameters"), _T("NicIndex"), m_iCurrentNic)
		||!AfxGetApp()->WriteProfileString(_T("Parameters"), _T("UserName"), Name)
		||!AfxGetApp()->WriteProfileString(_T("Parameters"), _T("Password"), Pass))
	{
		Report(_T("保存设置失败！"), 1, _T("设置提示"));
	}
}

void CMentoHUSTDlg::GetCfg()
{
	m_iCurrentNic = AfxGetApp()->GetProfileInt(_T("Parameters"), _T("NicIndex"), 0);
	CString Flag = AfxGetApp()->GetProfileString(_T("Parameters"), _T("CertFlag"), _T("01")),
		Name = AfxGetApp()->GetProfileString(_T("Parameters"), _T("UserName"), _T("")),
		Pass = AfxGetApp()->GetProfileString(_T("Parameters"), _T("PassWord"), _T(""));
	BOOL bCaption = AfxGetApp()->GetProfileInt(_T("Parameters"), _T("CaptionFlag"), 0);
	if(bCaption)
		SetWindowText(AfxGetApp()->GetProfileString(_T("Parameters"), _T("MyCaption"), _T("MentoHUST 2009Beta版")));
	if(Flag[0]-'0')
		m_bAutoCert = TRUE;
	else
		m_bAutoCert = FALSE;
	if(Flag[1]-'0')
		m_bSavePass = TRUE;
	else
		m_bSavePass = FALSE;
	if (Name.IsEmpty())
		GetPass(m_sName, m_sPass);
	else
		CodeData(Pass, Name, m_sPass, m_sName);
	UpdateData(FALSE);
}

bool CMentoHUSTDlg::GetNetInfo(void)
{
	bIP.ul = bMaskIP.ul = bGateIP.ul = bDNS.ul = 0;

	PIP_ADAPTER_INFO pAdapterInfo = (IP_ADAPTER_INFO *) malloc( sizeof(IP_ADAPTER_INFO) );
	
	DWORD dwRetVal;
	ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);

	if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW) 
	{
		free(pAdapterInfo);
		pAdapterInfo = (IP_ADAPTER_INFO *) malloc (ulOutBufLen);
	}

	dwRetVal = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen);
	if(dwRetVal == NO_ERROR)
	{
		for(PIP_ADAPTER_INFO pAdapter = pAdapterInfo; pAdapter; pAdapter = pAdapter->Next)
		{
			CString sAdaptername(pAdapter->AdapterName);
			if (sAdaptername.Compare(m_pCurrentNic->m_NicName) == 0)
			{
				memcpy(bLocalMAC, pAdapter->Address, 6);
				bIP.ul = inet_addr(pAdapter->IpAddressList.IpAddress.String);
				bMaskIP.ul = inet_addr(pAdapter->IpAddressList.IpMask.String);
				bGateIP.ul = inet_addr(pAdapter->GatewayList.IpAddress.String);
				break;
			}
		}
	}
	if (pAdapterInfo != NULL)
	{
		free(pAdapterInfo);
		pAdapterInfo = NULL;
	}
	if(dwRetVal == ERROR_NO_DATA)
	{
		Report(_T("初始化硬件地址失败, 请检查网络连接！"), 1);
		return false;
	}
	
	FIXED_INFO *pFixedInfo = (FIXED_INFO *) malloc( sizeof( FIXED_INFO ) );
	ulOutBufLen = sizeof( FIXED_INFO );

	if ( GetNetworkParams( pFixedInfo, &ulOutBufLen ) == ERROR_BUFFER_OVERFLOW ) 
	{
		free( pFixedInfo );
		pFixedInfo = (FIXED_INFO *) malloc( ulOutBufLen );
	}
	if(GetNetworkParams( pFixedInfo, &ulOutBufLen ) == NO_ERROR)
		bDNS.ul = inet_addr(pFixedInfo->DnsServerList.IpAddress.String);

	if (pFixedInfo != NULL)
	{
		free( pFixedInfo );
		pFixedInfo = NULL;
	}
	
	CString strTemp;
	strTemp.Format(_T("本机MAC:  %02x:%02x:%02x:%02x:%02x:%02x"), bLocalMAC[0], bLocalMAC[1], bLocalMAC[2], bLocalMAC[3], bLocalMAC[4], bLocalMAC[5]);
	this->Report(strTemp, 3);
	strTemp.Format(_T("IP地址:   %d.%d.%d.%d"), bIP.bt[0] ,bIP.bt[1] ,bIP.bt[2] ,bIP.bt[3]);
	this->Report(strTemp, 3);
	strTemp.Format(_T("子网掩码: %d.%d.%d.%d"), bMaskIP.bt[0] ,bMaskIP.bt[1] ,bMaskIP.bt[2] ,bMaskIP.bt[3]);
	this->Report(strTemp, 3);
	strTemp.Format(_T("默认网关; %d.%d.%d.%d"), bGateIP.bt[0] ,bGateIP.bt[1] ,bGateIP.bt[2] ,bGateIP.bt[3]);
	this->Report(strTemp, 3);
	strTemp.Format(_T("首选DNS:  %d.%d.%d.%d"), bDNS.bt[0] ,bDNS.bt[1] ,bDNS.bt[2] ,bDNS.bt[3]);
	this->Report(strTemp, 3);
	return true;
}

void CMentoHUSTDlg::GetCertCfg()
{
	CString Mode= AfxGetApp()->GetProfileString(_T("Parameters"), _T("CertMode"), _T("1101030031"));
	if(Mode[1]-'0')
		m_bAutoMin = TRUE;
	else
		m_bAutoMin = FALSE;
	if(Mode[2]-'0'<0 || Mode[2]-'0'>3)
		m_iDhcpMode = 0;
	else
		m_iDhcpMode = Mode[2]-'0';
	if(Mode[3]-'0')
		m_bBindArp = TRUE;
	else
		m_bBindArp = FALSE;
	m_iEcho = (Mode[4]-'0')*100 + (Mode[5]-'0')*10 +(Mode[6]-'0');
	m_iTimeOut = (Mode[7]-'0')*10 + (Mode[8]-'0');
	if(Mode[9]-'0')
		m_bStartMode = TRUE;
	else
		m_bStartMode = FALSE;

	CString Switch = AfxGetApp()->GetProfileString(_T("Parameters"), _T("FunSwitch"), _T("000"));
	if(Switch[0]-'0')
		m_bPackage = TRUE;
	else
		m_bPackage = FALSE;
	if(Switch[1]-'0')
		m_bMac = TRUE;
	else
		m_bMac = FALSE;
	if(Switch[2]-'0')
		m_bIP = TRUE;
	else
		m_bIP = FALSE;

	CString Flag = AfxGetApp()->GetProfileString(_T("Parameters"), _T("CertFlag"), _T("11"));
	if(Flag[1]-'0' != m_bSavePass)
	{
		m_bSavePass = Flag[1]-'0';
		SaveCfg();
	}
}

void CMentoHUSTDlg::InitCertData()
{
	
	if (m_iDhcpMode == 5)
	{
		Report(_T("!! 认证成功!"));
		SendMessage( WM_SYSCOMMAND, IDT_ACK_ECHO );
		return;
	}
	if( !GetNetInfo() )
	{
		SendMessage( WM_SYSCOMMAND, IDT_DISCONNECT );
		return;
	}
	if(m_bMac)
	{
		CString sMac = AfxGetApp()->GetProfileString(_T("Parameters"), _T("MyMac"), _T("00:00:00:00:00:00"));
		sMac.Remove(_T(':'));
		sMac.MakeUpper();
		if(sMac.GetLength()!=12)
			m_bMac = FALSE;
		else
		{
			TCHAR *strBuf = sMac.GetBuffer(sMac.GetLength());
			sMac.ReleaseBuffer();
			for(int i=0; i<12; i++)
			{
				if(strBuf[i]<'0'||(strBuf[i]>'9'&&strBuf[i]<'A')||strBuf[i]>'F')
				{
					m_bMac = FALSE;
					break;
				}
				else if(strBuf[i]<='9')	strBuf[i]-='0';
				else strBuf[i]-=55;
				if(i%2)
					bMyMAC[i/2] = (strBuf[i-1]<<4) + strBuf[i];
			}
		}
	}
	if(m_bIP)
		bIP.ul =  ntohl(AfxGetApp()->GetProfileInt(_T("Parameters"), _T("MyIP"), 0));
	InitHead();
	bTimerSign = FALSE;	
	SendMessage(WM_SYSCOMMAND, IDT_FIND_SERVER);
}

void CMentoHUSTDlg::InitHead()
{
	memset(Package, 0, 0x167);
	*(USHORT *)(Package+0x02) = ntohs(0x1311);
	if(m_iDhcpMode != 0)
		Package[0x04] = 0x01;
	*(ULONG *)(Package+0x05) = bIP.ul;
	*(ULONG *)(Package+0x09) = bMaskIP.ul;
	*(ULONG *)(Package+0x0D) = bGateIP.ul;
	*(ULONG *)(Package+0x11) = bDNS.ul;
	Blog(Package, 21, Package+0x15);
	for(int i=0; i<23; i++)
		Package[i] = Alog(Package[i]);
}

void CMentoHUSTDlg::InitDefault(BOOL bStart)
{
	m_bPackage = FALSE;
	*(USHORT *)(Package+0x19) = *(USHORT *)(Package+0x42) =
		*(USHORT *)(Package+0x4a) = *(USHORT *)(Package+0x72) =
		*(USHORT *)(Package+0x7E) = *(USHORT *)(Package+0x8C) = ntohs(0x1311);
	memcpy(Package+0x1B, "8021x.exe", 9);
	*(USHORT *)(Package+0x3B) = ntohs(0x021f);
	*(ULONG *)(Package+0x44) = ntohl(0x00281a28);
	*(USHORT *)(Package+0x4C) = ntohs(0x1722);
	/**(USHORT *)(Package+0x6E) = ntohs(0x1a0c);
	*(USHORT *)(Package+0x74) = ntohs(0x1806);
	if(m_iDhcpMode == 1 || m_iDhcpMode == 2)
		Package[0x79] = 0x01;
	else
		Package[0x79] = 0x00;
	*(USHORT *)(Package+0x7A) = ntohs(0x1a0e);
	*(USHORT *)(Package+0x80) = ntohs(0x2d08);
	if(m_bMac)
		memcpy(Package+0x82, bMyMAC, 6);
	else
		memcpy(Package+0x82, bLocalMAC, 6);
	*(USHORT *)(Package+0x88) = ntohs(0x1a08);
	*(USHORT *)(Package+0x8E) = ntohs(0x2f02);*/
	BYTE Start[0x20] = {
		0x92, 0x68, 0x64, 0x66, 0x92, 0x94, 0x62, 0x66, 0x91, 0x93, 0x95, 0x62, 0x93, 0x93, 0x91, 0x94,
		0x64, 0x61, 0x64, 0x64, 0x65, 0x66, 0x68, 0x94, 0x98, 0xA7, 0x61, 0x67, 0x65, 0x67, 0x9C, 0x6B};
	BYTE Md5[0x20] = {
		0x96, 0x96, 0x96, 0x96, 0x96, 0x96, 0x96, 0x96, 0x96, 0x96, 0x96, 0x96, 0x67, 0x61, 0x96, 0x96,
		0x64, 0x62, 0x67, 0x91, 0x96, 0x96, 0x96, 0x96, 0x9e, 0xc9, 0xcd, 0x96, 0x67, 0x67, 0x98, 0x96,};
	if(bStart)
		memcpy(Package+0x4E, Start, 0x20);
	else
		memcpy(Package+0x4E, Md5, 0x20);
}

void CMentoHUSTDlg::InitPackage(CString sPath, BOOL bStart)
{
	if(!bStart)
	{
		Clog(sPath);
		return;
	}
	try
	{
		CFile File(sPath, CFile::modeRead | CFile::shareDenyNone | CFile::typeBinary);
		if( File.GetLength() < MD5_PART_LENGTH+0x150L)
		{
			Report(_T(">> 所选数据包太小, 将使用默认包认证"), 3);
			File.Close();
			InitDefault(bStart);
		}
		else
		{
			File.Seek (MD5_PART_LENGTH, CFile::begin);
			File.Read(Package+0x17, 0x150);
			File.Close();
			if(m_iDhcpMode == 4 || m_iDhcpMode == 5)
				Package[0x3F] = 0x00;
			if(m_iDhcpMode == 1 || m_iDhcpMode == 2)
				Package[0x79] = 0x01;
			else
				Package[0x79] = 0x00;
			if(m_bMac)
				memcpy(Package+0x82, bMyMAC, 6);
			else
				memcpy(Package+0x82, bLocalMAC, 6);
		}
	}
	catch(CFileException *pEx)
	{
		Report(_T(">> 打开文件出错, 将使用默认包认证"), 3);
		InitDefault(bStart);
		pEx->Delete();
	}
}

bool CMentoHUSTDlg::GetNic()
{
	SaveCfg();
	m_pCurrentNic = m_NicHead;
	for(int i=0; i<m_iCurrentNic; i++)
		m_pCurrentNic = m_pCurrentNic->m_pNext;
	if(!m_pCurrentNic)	return(false);
	return(true);
}

void CMentoHUSTDlg::OnBnConnect()
{
	if(m_iState == IDT_DISCONNECT)
	{
		if(!GetNic() || m_sName.IsEmpty() || m_sPass.IsEmpty())	return;
		GetCertCfg();
		if(m_iDhcpMode == 3)
			SendMessage( WM_SYSCOMMAND, IDT_DHCP_RENEW );
		else
			SendMessage( WM_SYSCOMMAND, IDT_INIT_DATA );
	}
	else
		SendMessage( WM_SYSCOMMAND, IDT_DISCONNECT );
}

UINT DhcpThreadFunc( LPVOID pParam )
{
	CMentoHUSTDlg *mainDlg = (CMentoHUSTDlg *)pParam;
	TCHAR sysPath[MAX_PATH];
	GetSystemDirectory(sysPath, MAX_PATH);
	SHELLEXECUTEINFO ShExecInfo;
	ShExecInfo.cbSize	= sizeof(SHELLEXECUTEINFO);
	ShExecInfo.fMask	= SEE_MASK_NOCLOSEPROCESS;
	ShExecInfo.hwnd		= NULL;
	ShExecInfo.lpVerb	= NULL;
	ShExecInfo.lpFile	= _T("ipconfig.exe");
	ShExecInfo.lpDirectory = sysPath;
	ShExecInfo.nShow	= SW_HIDE;
	ShExecInfo.hInstApp =NULL;	 
	if(mainDlg->m_iState == IDT_DHCP_RENEW)
	{
		ShExecInfo.lpParameters = _T("/renew");
		ShellExecuteEx(&ShExecInfo);
		mainDlg->Report(_T(">> 正在获取IP, 请稍等……"));
		int iRnt = WaitForSingleObject(ShExecInfo.hProcess, 30000);
		if(iRnt == WAIT_TIMEOUT)
			mainDlg->Report(_T(">> 获取IP超时"), 3);
		else if(iRnt == WAIT_FAILED)
			mainDlg->Report(_T(">> 获取IP失败"), 3);
		else
			mainDlg->Report(_T(">> 获取IP结束"), 3);
		mainDlg->SendMessage( WM_SYSCOMMAND, IDT_INIT_DATA );
	}
	else
	{
		ShExecInfo.lpParameters = _T("/release");
		ShellExecuteEx(&ShExecInfo);
		mainDlg->Report(_T(">> 正在释放IP，请稍等……"));
		int iRnt = WaitForSingleObject(ShExecInfo.hProcess, 30000);
		if(iRnt == WAIT_TIMEOUT) 
			mainDlg->Report(_T(">> 释放IP超时"));
		else if(iRnt == WAIT_FAILED)
			mainDlg->Report(_T(">> 释放IP失败"));
		else
			mainDlg->Report(_T(">> 释放IP结束"));
		mainDlg->Report(_T(":: 认证失败或未认证！"));
	}
	CloseHandle(ShExecInfo.hProcess);
	return 0;
}

UINT CertThreadFunc( LPVOID pParam )
{
	CMentoHUSTDlg *mainDlg = (CMentoHUSTDlg *)pParam;
	BYTE *pRecvBuf;			//指向buf1，接收到的数据包
	BYTE *pRecvHeaderBuf;	//指向buf2，接收到的数据头
	bool bFirst = true;
	CString strTemp;
	USHORT sLen;

	pcap_t *pHandle = mainDlg->m_pCurrentNic->m_pHandle;
	while ( mainDlg->m_iState != IDT_DISCONNECT )
	{		
		if ( pcap_next_ex(pHandle ,(pcap_pkthdr **)&pRecvHeaderBuf, (const u_char**)&pRecvBuf) != 1 )
			continue;
		if (pRecvBuf[0x0c]==0x88 && pRecvBuf[0x0d]==0x8e)		//判断是否802.1x
		{
			switch(pRecvBuf[0x12])
			{
			case 0x01:					//Request，验证用户名或者密码时收到
				switch (pRecvBuf[0x16])	//判断EAP Type
				{
				case 0x01:				//Request Identity
					if ( bFirst )
					{
						memcpy(mainDlg->bDestMAC ,pRecvBuf+6, 6);
						strTemp.Format(_T("认证MAC:  %02x:%02x:%02x:%02x:%02x:%02x"), mainDlg->bDestMAC[0], mainDlg->bDestMAC[1], mainDlg->bDestMAC[2], mainDlg->bDestMAC[3], mainDlg->bDestMAC[4], mainDlg->bDestMAC[5]);
						mainDlg->Report(strTemp, 3);
						bFirst = false;
					}
					memset( mainDlg->bMD5Source, 0, 0x20 );
					memcpy( mainDlg->bMD5Source, pRecvBuf+0x10, 0x20 );
					mainDlg->SendMessage( WM_SYSCOMMAND, IDT_ACK_NAME );
					break;

				case 0x04:				//Request MD5-Challenge
					memset( mainDlg->bMD5Source, 0, 0x20 );
					memcpy( mainDlg->bMD5Source, pRecvBuf+0x10, 0x20 );
					mainDlg->SendMessage( WM_SYSCOMMAND, IDT_ACK_PSWD );
					break;
				}
				break;

			case 0x03:					//Success
				sLen = ((USHORT)pRecvBuf[0x10]<<8)|pRecvBuf[0x11];
				if (sLen != 0x94)
				{
					char *serverMsg = new char[sLen-12];
					memset(serverMsg, 0, sLen-12);
					memcpy(serverMsg, pRecvBuf+0x1e, sLen-12);
					mainDlg->m_ServerMsg = serverMsg;
					mainDlg->Report(_T("!! 认证成功: ")+mainDlg->m_ServerMsg);
					delete []serverMsg;
				}
				else
					mainDlg->Report(_T("!! 认证成功!"));
				mainDlg->bEchoSource.ul = htonl(*((ULONG *)(pRecvBuf+sLen+0x09)));
				mainDlg->bEchoSource.bt[0] = Alog(mainDlg->bEchoSource.bt[0]);
				mainDlg->bEchoSource.bt[1] = Alog(mainDlg->bEchoSource.bt[1]);
				mainDlg->bEchoSource.bt[2] = Alog(mainDlg->bEchoSource.bt[2]);
				mainDlg->bEchoSource.bt[3] = Alog(mainDlg->bEchoSource.bt[3]);
				mainDlg->SendMessage( WM_SYSCOMMAND, IDT_ACK_ECHO );
				break;

			case 0x04:					//认证失败
				sLen = ((USHORT)pRecvBuf[0x10]<<8)|pRecvBuf[0x11];
				if (sLen != 0x04)
				{
					char *serverMsg = new char[sLen-12];
					memset(serverMsg, 0, sLen-12);
					memcpy(serverMsg, pRecvBuf+0x1e, sLen-12);
					mainDlg->m_ServerMsg = serverMsg;
					mainDlg->Report(_T("!! 认证失败: ")+mainDlg->m_ServerMsg);
					delete []serverMsg;
				}
				if(mainDlg->m_iState == IDT_ACK_ECHO)
					mainDlg->SendMessage(WM_SYSCOMMAND, IDT_FIND_SERVER);
				break;
			}//end switch
		}//end if
		else if(pRecvBuf[0x0c] ==0x08 && pRecvBuf[0x0d]==0x06 )		//判断是否ARP
		{
			if(mainDlg->m_bBindArp==1 &&
				((pRecvBuf[0x15]==0x01 && memcmp(pRecvBuf+0x1c, mainDlg->bGateIP.bt, 4)==0)	//网关主动询问
				|| (pRecvBuf[0x15]==0x02 && memcmp(pRecvBuf+0x1c, mainDlg->bGateIP.bt, 4)==0 && memcmp(pRecvBuf+0x26, mainDlg->bIP.bt, 4)==0)))
			{
				memcpy(mainDlg->bGateMAC, pRecvBuf+0x16, 6);
				strTemp.Format(_T("网关MAC:  %02x:%02x:%02x:%02x:%02x:%02x"), mainDlg->bGateMAC[0], mainDlg->bGateMAC[1], mainDlg->bGateMAC[2], mainDlg->bGateMAC[3], mainDlg->bGateMAC[4], mainDlg->bGateMAC[5]);
				mainDlg->Report(strTemp, 3);
				strTemp.Format(_T("-s %d.%d.%d.%d %02x-%02x-%02x-%02x-%02x-%02x"), mainDlg->bGateIP.bt[0], mainDlg->bGateIP.bt[1], mainDlg->bGateIP.bt[2], mainDlg->bGateIP.bt[3], mainDlg->bGateMAC[0], mainDlg->bGateMAC[1], mainDlg->bGateMAC[2], mainDlg->bGateMAC[3], mainDlg->bGateMAC[4], mainDlg->bGateMAC[5]);
				ShellExecute(NULL, _T("open"), _T("arp"), strTemp, NULL, SW_HIDE);
				mainDlg->m_bBindArp = 2;
			}
			else if(pRecvBuf[0x15]==0x02 && memcmp(pRecvBuf+0x1c, mainDlg->bGateIP.bt, 4)==0 && memcmp(pRecvBuf+0x26, mainDlg->bIP.bt, 4)==0)
			{
				memcpy(mainDlg->bTempMAC, pRecvBuf+0x16, 6);
				mainDlg->SendMessage( WM_SYSCOMMAND, IDT_BIND_ARP );
			}
		}
	}//end while
	return 0;
}

bool CMentoHUSTDlg::SendPackage()
{
	if( iLastState == m_iState && m_iState != IDT_ACK_ECHO )
		iErrorTimes++;
	else
	{
		iLastState = m_iState;
		iErrorTimes = 0;
	}
	if ( iErrorTimes >= MAX_ERROR_REPEAT )
	{
		iErrorTimes = 0;
		switch( m_iState )
		{
		case IDT_FIND_SERVER:
			if( m_iDhcpMode == 4 )
			{
				Report(_T("!! 认证成功!"));
				SendMessage( WM_SYSCOMMAND, IDT_ACK_ECHO );
				return true;
			}
			Report(_T("** 找不到服务器，重启认证！"));
			break;
		case IDT_ACK_NAME:
			Report(_T("** 发送用户名超时，重启认证！"));
			break;
		case IDT_ACK_PSWD:
			Report(_T("** 发送密码超时，重启认证！"));
			break;
		}
		SendLogoffPackage();
		SendMessage( WM_SYSCOMMAND, IDT_FIND_SERVER );
		return true;
	}
	switch ( m_iState )
	{
	case IDT_FIND_SERVER:			//寻找服务器
		if ( SendStartPackage() )
			return false;
		break;
	case IDT_ACK_NAME:				//以用户名应答
		if ( SendIdentityPackage() )
			return false;
		break;
	case IDT_ACK_PSWD:				//以MD5计算得到的值应答
		if ( SendMD5ChallengePackage() )
			return false;
		break;
	case IDT_ACK_ECHO:				//运行时echo包
		if ( SendEchoPackage() )
			return false;
		if(m_bBindArp != 0 )
			SendArpPackage();
		break;
	}
	return true;
}

int CMentoHUSTDlg::SendStartPackage()
{
	BYTE StandardAddr[]	= {0x01, 0x80, 0xC2, 0x00, 0x00, 0x03};
	BYTE RuijieAddr[]	= {0x01, 0xD0, 0xF8, 0x00, 0x00, 0x03};

	memset(bPackage, 0, 0x3E8);
	if(m_iDhcpMode == 4 || m_iDhcpMode == 5)
		memcpy(bPackage, bDestMAC, 6);
	else if(m_bStartMode)
		memcpy( bPackage, RuijieAddr, 6);
	else
		memcpy( bPackage, StandardAddr, 6);
	memcpy(bPackage+0x06, bLocalMAC, 6);
	*(ULONG *)(bPackage+0x0c) = ntohl(0x888E0101);
	memcpy(bPackage+0x12, Package, 0x167);

	return pcap_sendpacket(m_pCurrentNic->m_pHandle, bPackage, 0x3E8);
}

int CMentoHUSTDlg::SendIdentityPackage()
{
	LPCTSTR name = m_sName.GetBuffer(m_sName.GetLength());
	m_sName.ReleaseBuffer();
	int iLen = WideCharToMultiByte(CP_UTF8, 0, name, -1, NULL, 0, NULL, NULL);
	char *sName = new char[iLen+1];
	WideCharToMultiByte(CP_UTF8, 0, name, -1, sName, iLen, NULL, NULL);
	iLen = strlen(sName);

	memset(bPackage, 0, 0x3E8);
	memcpy(bPackage, bDestMAC, 6);
	memcpy(bPackage+0x06, bLocalMAC, 6);
	*(ULONG *)(bPackage+0x0c) = ntohl(0x888E0100);
	*(USHORT *)(bPackage+0x14) = *(USHORT *)(bPackage+0x10) = ntohs(5 + iLen);
	bPackage[0x12] = 0x02;
	bPackage[0x13] = bMD5Source[0x03];
	bPackage[0x16] = bMD5Source[0x06];
	for(int i=0; i<iLen; i++)
		bPackage[0x17+i] = (BYTE)sName[i];
	delete []sName;
	memcpy(bPackage+0x17+iLen, Package, 0x167);
	return pcap_sendpacket( m_pCurrentNic->m_pHandle, bPackage, 0x3E8 );
}

// 计算md5摘要的函数，计算过程在头文件md5.h里
BYTE *ComputeHash(BYTE * src, int i)
{
	MD5_CTX context;
	static unsigned char digest[16];
	MD5Init(&context);
	MD5Update(&context, src, i);
	MD5Final(digest, &context);
	return digest;
}

int CMentoHUSTDlg::SendMD5ChallengePackage()
{
	BYTE md5src[80], *md5Dig;
	
	LPCTSTR name = m_sName.GetBuffer(m_sName.GetLength());
	m_sName.ReleaseBuffer();
	int iLen = WideCharToMultiByte(CP_UTF8, 0, name, -1, NULL, 0, NULL, NULL);
	char *sName = new char[iLen+1];
	WideCharToMultiByte(CP_UTF8, 0, name, -1, sName, iLen, NULL, NULL); 
	iLen = strlen(sName);
	int i;
	
	memset(bPackage, 0, 0x3E8);
	memcpy(bPackage, bDestMAC, 6);
	memcpy(bPackage+0x06, bLocalMAC, 6);
	*(ULONG *)(bPackage+0x0c) = ntohl(0x888E0100);
	*(USHORT *)(bPackage+0x14) = *(USHORT *)(bPackage+0x10) = ntohs(6+ bMD5Source[0x07] + iLen);
	bPackage[0x12] = 0x02;
	bPackage[0x13] = bMD5Source[0x03];
	bPackage[0x16] = bMD5Source[0x06];
	bPackage[0x17] = bMD5Source[0x07];

	iLen = m_sPass.GetLength();
	md5src[0] = bMD5Source[0x03];
	for( i=0; i<iLen; i++ )
		md5src[1+i] = (BYTE)(m_sPass.GetBuffer(0)[i]);
	for( i=0; i<bMD5Source[0x07]; i++ )
		md5src[1+iLen+i] = bMD5Source[0x08+i];
	md5Dig = ComputeHash( md5src, 1+iLen+bMD5Source[0x07] );
	for( i=0; i<bMD5Source[0x07]; i++ )
		bPackage[0x18+i] = md5Dig[i];
	iLen = strlen(sName);
	for( i=0 ; i<iLen; i++ )
		bPackage[0x18+bMD5Source[0x07]+i] = (BYTE)sName[i];
	delete []sName;
	memcpy(bPackage+0x18+bMD5Source[0x07]+iLen, Package, 0x167);
	bEchoNo.ul = 0x0000102b;
	
	return pcap_sendpacket( m_pCurrentNic->m_pHandle, bPackage, 0x3E8);
}

int CMentoHUSTDlg::SendEchoPackage()
{
	BYTE echoPackage[0x2D] = {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x88, 0x8e, 0x01, 0xbf,
		0x00, 0x1e, 0xff, 0xff, 0x37, 0x77, 0x7f, 0x9f, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x37, 0x77,
		0x7f, 0x9f, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x37, 0x77, 0x7f, 0x3f, 0xff};
	UL_BYTE bPart1, bPart2;
	bPart1.ul = bEchoSource.ul + bEchoNo.ul;
	bPart2.ul = bEchoNo.ul;
	bEchoNo.ul++;
	bPart1.ul = ntohl(bPart1.ul);
	bPart2.ul = ntohl(bPart2.ul);
	memcpy( echoPackage, bDestMAC, 6 );
	memcpy( echoPackage+0x06, bLocalMAC, 6 );
	echoPackage[0x18] = Alog(bPart1.bt[0]);
	echoPackage[0x19] = Alog(bPart1.bt[1]);
	echoPackage[0x1a] = Alog(bPart1.bt[2]);
	echoPackage[0x1b] = Alog(bPart1.bt[3]);
	echoPackage[0x22] = Alog(bPart2.bt[0]);
	echoPackage[0x23] = Alog(bPart2.bt[1]);
	echoPackage[0x24] = Alog(bPart2.bt[2]);
	echoPackage[0x25] = Alog(bPart2.bt[3]);

	return pcap_sendpacket( m_pCurrentNic->m_pHandle, echoPackage, 0x2D);
}

int CMentoHUSTDlg::SendLogoffPackage()
{
	memset(bPackage, 0, 0x3E8);
	memcpy(bPackage, bDestMAC, 6);
	memcpy(bPackage+0x06, bLocalMAC, 6);
	*(ULONG *)(bPackage+0x0c) = ntohl(0x888E0102);
	memcpy(bPackage+0x12, Package, 0x167);

	return pcap_sendpacket(m_pCurrentNic->m_pHandle, bPackage, 0x3E8);
}

void CMentoHUSTDlg::SendArpPackage()
{
	byte ArpPackage[0x3C] = {	//Arp包
		0xff,0xff,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x06,0x00,0x01,
		0x08,0x00,0x06,0x04,0x00};
		
		/*memcpy( ArpPackage, bLocalMAC, 6 );
		memcpy( ArpPackage+0x06, bGateMAC, 6 );
		ArpPackage[0x15]=0x02;
		memcpy( ArpPackage+0x16, bGateMAC, 6 );
		memcpy( ArpPackage+0x1c, m_NetGate.Part, 4 );
		memcpy( ArpPackage+0x20, bLocalMAC, 6 );
		memcpy( ArpPackage+0x26, m_IP.Part, 4 );
		pcap_sendpacket( m_pCurrentNIC->m_pHandle, ArpPackage, 0x3C );*/
		
	if (m_iDhcpMode ==1 || m_iDhcpMode == 2)
		return;
	if (m_bBindArp==2)
	{
		memcpy( ArpPackage, bGateMAC, 6 );
		memcpy( ArpPackage+0x06, bLocalMAC, 6 );
		ArpPackage[0x15]=0x02;
		memcpy( ArpPackage+0x16, bLocalMAC, 6 );
		memcpy( ArpPackage+0x1c, bIP.bt, 4 );
		memcpy( ArpPackage+0x20, bGateMAC, 6 );
		memcpy( ArpPackage+0x26, bGateIP.bt, 4 );
		pcap_sendpacket( m_pCurrentNic->m_pHandle, ArpPackage, 0x3C );
	}
	memset(ArpPackage, 0xFF, 6);
	memcpy( ArpPackage+0x06, bLocalMAC, 6 );
	ArpPackage[0x15]=0x01;
	memcpy( ArpPackage+0x16, bLocalMAC, 6 );
	memcpy( ArpPackage+0x1c, bIP.bt, 4 );
	memset( ArpPackage+0x20, 0, 6 );
	memcpy( ArpPackage+0x26, bGateIP.bt, 4 );
	pcap_sendpacket( m_pCurrentNic->m_pHandle, ArpPackage, 0x2A );
}

void CMentoHUSTDlg::TimerCtrl(int timer)
{
	if( bTimerSign )
		KillTimer(CERT_CYCLE_TIMER);
	if( timer == 0 )
	{
		bTimerSign = FALSE;
		return;
	}
	SetTimer(CERT_CYCLE_TIMER, 1000*timer, 0);
	bTimerSign = TRUE;
}

void CMentoHUSTDlg::OnTimer(UINT_PTR nIDEvent)
{
	switch( nIDEvent )
	{
	case CERT_CYCLE_TIMER:
		if(!SendPackage())
		{
			SendMessage( WM_SYSCOMMAND, IDT_DISCONNECT );	
			Report(_T("发送数据包失败, 请检查网络连接！\n作者：53742 49370"), 1);
			ToTray(false);
		}
		break;
	}
	CDialog::OnTimer(nIDEvent);
}