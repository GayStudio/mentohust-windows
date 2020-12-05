// ExtraCfg.cpp : 实现文件
//Written By HustMoon

#include "stdafx.h"
#include "MentoHUST.h"
#include "MentoHUSTDlg.h"
#include "ExtraCfg.h"

// CExtraCfg 对话框

IMPLEMENT_DYNAMIC(CExtraCfg, CDialog)

CExtraCfg::CExtraCfg(CWnd* pParent /*=NULL*/)
	: CDialog(CExtraCfg::IDD, pParent)
{
	m_bCaption = FALSE;
	m_bPackage = FALSE;
	m_bMac = FALSE;
	m_bIP = FALSE;
	m_sCaption = _T("");
	m_sPath = _T("");
	m_CatchThread = NULL;
	m_iType = IDT_NULL;
}

CExtraCfg::~CExtraCfg()
{
}

void CExtraCfg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_MYCAPTION, m_bCaption);
	DDX_Check(pDX, IDC_MYPACKAGE, m_bPackage);
	DDX_Check(pDX, IDC_MYMAC, m_bMac);
	DDX_Check(pDX, IDC_MYIP, m_bIP);
	DDX_Text(pDX, IDC_EDIT_CAPTION, m_sCaption);
	DDX_Text(pDX, IDC_EDIT_PATH, m_sPath);
	DDX_Text(pDX, IDC_EDIT_MAC, m_sMac);
	DDV_MaxChars(pDX, m_sMac, 17);
	DDX_Control(pDX, IDC_IP, m_IP);
}


BEGIN_MESSAGE_MAP(CExtraCfg, CDialog)
	ON_BN_CLICKED(IDC_MYCAPTION, CExtraCfg::SetWinState)
	ON_BN_CLICKED(IDC_MYPACKAGE, CExtraCfg::SetWinState)
	ON_BN_CLICKED(IDC_MYIP, CExtraCfg::SetWinState)
	ON_BN_CLICKED(IDC_MYMAC, CExtraCfg::SetWinState)
	ON_BN_CLICKED(IDC_BN_SCAN, CExtraCfg::OnBnClickedBnScan)
	ON_BN_CLICKED(IDC_BN_CATCH, CExtraCfg::OnBnClickedBnCatch)
	ON_WM_SYSCOMMAND()
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CExtraCfg 消息处理程序

BOOL CExtraCfg::OnInitDialog()
{
	CDialog::OnInitDialog();
	// TODO:  在此添加额外的初始化
	InitData();
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CExtraCfg::SetWinState()
{
	UpdateData();
	if(m_bCaption)
		GetDlgItem(IDC_EDIT_CAPTION)->EnableWindow(TRUE);
	else
		GetDlgItem(IDC_EDIT_CAPTION)->EnableWindow(FALSE);
	if(m_bPackage)
	{
		GetDlgItem(IDC_BN_SCAN)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_PATH)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_BN_SCAN)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_PATH)->EnableWindow(FALSE);
	}
	if(m_bMac)
		GetDlgItem(IDC_EDIT_MAC)->EnableWindow(TRUE);
	else
		GetDlgItem(IDC_EDIT_MAC)->EnableWindow(FALSE);
	if(m_bIP)
		GetDlgItem(IDC_IP)->EnableWindow(TRUE);
	else
		GetDlgItem(IDC_IP)->EnableWindow(FALSE);
}

void CExtraCfg::InitData()
{
	CString Switch = AfxGetApp()->GetProfileString(_T("Parameters"), _T("FunSwitch"), _T("000"));
	m_bCaption = AfxGetApp()->GetProfileInt(_T("Parameters"), _T("CaptionFlag"), 0);
	m_sCaption = AfxGetApp()->GetProfileString(_T("Parameters"), _T("MyCaption"), _T("MentoHUST 2009Beta版"));
	m_sPath = AfxGetApp()->GetProfileString(_T("Parameters"), _T("PackagePath"), _T(""));
	m_sMac = AfxGetApp()->GetProfileString(_T("Parameters"), _T("MyMac"), _T("00:00:00:00:00:00"));
	m_IP.SetAddress(AfxGetApp()->GetProfileInt(_T("Parameters"), _T("MyIP"), 0));
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
	UpdateData(FALSE);
	SetWinState();
}

bool CExtraCfg::CheckMac()
{
	if(UpdateData())
	{
		if(m_sMac.GetLength()==17)
		{
			bool bFlag = true;
			m_sMac.MakeUpper();
			for(int i=0; i<17; i++)
			{
				if(((i+1)%3==0&&m_sMac[i]!=':')||(i+1)%3!=0&&(m_sMac[i]<'0'||m_sMac[i]>'F'||(m_sMac[i]>'9'&&m_sMac[i]<'A')))
				{
					bFlag = false;
					break;
				}
			}
			if(bFlag)
				return(true);
		}
		MessageBox(_T("MAC 格式不正确！"), _T("设置提示"), MB_OK | MB_ICONWARNING);
		GetDlgItem(IDC_EDIT_MAC)->SetFocus();
	}
	return(false);
}

bool CExtraCfg::SaveCfg()
{
	if(!CheckMac())
		return(false);
	CString Flag;
	Flag.Format(_T("%d%d%d"), m_bPackage, m_bMac, m_bIP);
	DWORD Myip;
	m_IP.GetAddress(Myip);
	if(!AfxGetApp()->WriteProfileString(_T("Parameters"), _T("FunSwitch"), Flag)
		||!AfxGetApp()->WriteProfileInt(_T("Parameters"), _T("CaptionFlag"), m_bCaption)
		||!AfxGetApp()->WriteProfileString(_T("Parameters"), _T("MyCaption"), m_sCaption)
		||!AfxGetApp()->WriteProfileString(_T("Parameters"), _T("PackagePath"), m_sPath)
		||!AfxGetApp()->WriteProfileString(_T("Parameters"), _T("MyMac"), m_sMac)
		||!AfxGetApp()->WriteProfileInt(_T("Parameters"), _T("MyIP"), Myip))
	{
		MessageBox(_T("保存设置失败！"), _T("设置提示"), MB_OK | MB_ICONWARNING );
	}
	return(true);
}

void CExtraCfg::OnBnClickedBnScan()
{
	CFileDialog dlg(TRUE,	//打开框
		_T("mhp"),	//默认扩展名
		NULL,	//默认文件名
		OFN_FILEMUSTEXIST	//只能输入已存在的文件名
		| OFN_HIDEREADONLY	//不显示“以只读方式打开”复选框
		| OFN_ENABLESIZING,	//大小可调
		_T("数据包文件(*.mhp)|*.mhp|所有文件(*.*)|*.*||"),	//文件过滤器
		NULL	//指向父窗口的指针
		);
	dlg.m_ofn.lStructSize = 88;	//88带工具栏 76无
	if(dlg.DoModal()==IDCANCEL)
		return;
	GetDlgItem(IDC_EDIT_PATH)->SetWindowText(dlg.GetPathName());
}

CString CExtraCfg::GetRuijiePath(bool reg)
{
	CString Path = _T("");
	if(reg)
	{
		HKEY hKey;
		if(RegOpenKeyEx( HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\8021x.exe"), 0, KEY_READ, &hKey)!=ERROR_SUCCESS)
			return(_T(""));
		TCHAR FileName[MAX_PATH];
		DWORD type = REG_SZ;
		DWORD cbData = MAX_PATH * sizeof(TCHAR);
		if(RegQueryValueEx(hKey, NULL, NULL, &type, (LPBYTE)(LPCTSTR)FileName, &cbData)==ERROR_SUCCESS)
			Path = FileName;
		RegCloseKey(hKey);
	}
	else
	{
		CFileDialog dlg(TRUE, _T("exe"), _T("8021x.exe"),
			OFN_FILEMUSTEXIST
			| OFN_HIDEREADONLY
			| OFN_ENABLESIZING,
			_T("锐捷客户端(8021x.exe)|8021x.exe|所有文件(*.*)|*.*||"), NULL);
		dlg.m_ofn.lStructSize = 88;
		if(dlg.DoModal()==IDCANCEL)
			return(_T(""));
		Path = dlg.GetPathName();
	}
	//Path = Path.Left(Path.ReverseFind('\\')+1) + _T("8021x.exe");
	return(Path);
}

bool CExtraCfg::SaveRuijie(CString Ruijie)
{
	CFile File(Ruijie, CFile::modeRead | CFile::shareDenyWrite | CFile::typeBinary);		
	File.Seek (0x1000,CFile::begin);
	BYTE *Buffer = new BYTE[MD5_PART_LENGTH];
	if(File.Read(Buffer, MD5_PART_LENGTH) < 0x1000L)
	{
		MessageBox(_T("读取数据失败！"), _T("操作提示"), MB_OK | MB_ICONWARNING );
		File.Close();
		delete []Buffer;
		return(false);
	}
	File.Close();
	File.Open(_T("Temp.mhp"), CFile::modeCreate | CFile::modeWrite | CFile::typeBinary);
	File.Write(Buffer, MD5_PART_LENGTH);
	File.Close();
	delete []Buffer;
	return(true);
}

void CExtraCfg::OnBnClickedBnCatch()
{
	if(m_iType != IDT_NULL)
	{
		this->SendMessage( WM_SYSCOMMAND, IDT_NULL);
		return;
	}
	CString Ruijie = GetRuijiePath(true);
	bool bExist = false;
	CFileFind finder;
	while(!bExist)
	{
		if (finder.FindFile(Ruijie))
			bExist = true;
		else if(MessageBox(_T("未找到锐捷客户端程序8021x.exe，是否手动查找？"), _T("操作提示"), MB_OKCANCEL | MB_ICONWARNING ) == IDCANCEL)
			return;
		else
			Ruijie = GetRuijiePath(false);
	}
	SetHomePath();
	if(!SaveRuijie(Ruijie)) return;
	if (this->m_CatchThread == NULL )
		this->m_CatchThread = AfxBeginThread(CatchThreadFunc, this, THREAD_PRIORITY_TIME_CRITICAL );
	this->SendMessage( WM_SYSCOMMAND, IDT_START);
	if(ShellExecute(NULL, _T("open"), Ruijie, NULL, NULL, SW_SHOWNORMAL) < (HANDLE)32)	//郁闷的断言失败，不会改啊
	{
		MessageBox(_T("无法运行锐捷客户端！"), _T("操作提示"), MB_OK | MB_ICONWARNING );
		this->SendMessage( WM_SYSCOMMAND, IDT_NULL);	
	}
}

UINT CatchThreadFunc( LPVOID pParam )
{
	CExtraCfg *CfgDlg = (CExtraCfg *)pParam;
	CMentoHUSTDlg *mainDlg = (CMentoHUSTDlg *)AfxGetApp()->GetMainWnd();
	BYTE *pRecvBuf;			//指向buf1，接收到的数据包
	BYTE *pRecvHeaderBuf;	//指向buf2，接收到的数据头

	if(!mainDlg->GetNic())
	{
		CfgDlg->SendMessage( WM_SYSCOMMAND, IDT_NULL);
		mainDlg->Report(_T("找不到网卡！"), 1,  _T("操作提示"));
		return -1;
	}
	pcap_t *pHandle = mainDlg->m_pCurrentNic->m_pHandle;

	while( CfgDlg->m_iType != IDT_SUCCESS && CfgDlg->m_iType != IDT_NULL)
	{		
		if ( pcap_next_ex(pHandle ,(pcap_pkthdr **)&pRecvHeaderBuf, (const u_char**)&pRecvBuf) != 1 )
			continue;
		if (pRecvBuf[0x0c]==0x88 && pRecvBuf[0x0d]==0x8e)		//判断是否802.1x
		{
			if(pRecvBuf[0x0f]==0x01 && CfgDlg->m_iType == IDT_START)
			{
				memcpy(CfgDlg->bStartPackage, pRecvBuf+0x29, 0x150);
				CfgDlg->m_iType = IDT_SUCCESS;
				CfgDlg->SendMessage( WM_SYSCOMMAND, IDT_SUCCESS);
			}
		}
	}
	return 0;
}

void CExtraCfg::OnSysCommand(UINT nID, LPARAM lParam)
{
	switch( nID )
	{
	case IDT_START:
		memset(this->bStartPackage, 0, 0x150);
		m_iType = IDT_START;
		GetDlgItem(IDC_BN_CATCH)->SetWindowText(_T("取消"));
		break;
	case IDT_SUCCESS:
		SavePackage();
		m_iType = IDT_NULL;
		if(m_CatchThread)
		{
			if(WaitForSingleObject(m_CatchThread->m_hThread, 3000) == WAIT_TIMEOUT)
				TerminateThread(m_CatchThread->m_hThread, 0);
			m_CatchThread = NULL;
		}
		GetDlgItem(IDC_BN_CATCH)->SetWindowText(_T("捕获"));
		break;
	case IDT_NULL:
		DeleteFile(_T("Temp.mhp"));
		m_iType = IDT_NULL;
		GetDlgItem(IDC_BN_CATCH)->SetWindowText(_T("捕获"));
		break;
	default:
		CDialog::OnSysCommand(nID, lParam);
		break;
	}
}

void CExtraCfg::OnDestroy()
{
	CDialog::OnDestroy();
	SendMessage( WM_SYSCOMMAND, IDT_NULL);
	if(m_CatchThread)
	{
		if(WaitForSingleObject(m_CatchThread->m_hThread, 2000) == WAIT_TIMEOUT)
			TerminateThread(m_CatchThread->m_hThread, 0);
		m_CatchThread = NULL;
	}
}

void CExtraCfg::SavePackage()
{
	CFileDialog dlg(FALSE, _T("mhp"), NULL, OFN_ENABLESIZING, _T("数据包文件(*.mhp)|*.mhp|所有文件(*.*)|*.*||"), NULL);
	dlg.m_ofn.lStructSize = 88;
	if(dlg.DoModal()==IDCANCEL)
	{
		SendMessage( WM_SYSCOMMAND, IDT_NULL);
		return;
	}
	CString FileName = dlg.GetFileName();
	SetHomePath();
	CFile File(_T("Temp.mhp"), CFile::modeWrite | CFile::typeBinary);		
	File.Seek (MD5_PART_LENGTH, CFile::begin);
	File.Write(bStartPackage, 0x150);
	File.Close();
	MoveFileEx(_T("Temp.mhp"), FileName, MOVEFILE_REPLACE_EXISTING);
	MessageBox(_T("相关数据已成功捕获并保存在安装目录！"), _T("操作提示"), MB_OK| MB_ICONWARNING );
}