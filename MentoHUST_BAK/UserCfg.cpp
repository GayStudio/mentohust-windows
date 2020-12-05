// UserCfg.cpp : 实现文件
//Written By HustMoon

#include "stdafx.h"
#include "MentoHUST.h"
#include "MentoHUSTDlg.h"
#include "UserCfg.h"


// CUserCfg 对话框

IMPLEMENT_DYNAMIC(CUserCfg, CDialog)

CUserCfg::CUserCfg(CWnd* pParent /*=NULL*/)
	: CDialog(CUserCfg::IDD, pParent)
{
	m_bAutoCert = FALSE;
	m_bAutoMin = FALSE;
	m_bAutoRun = FALSE;
	m_bSavePass = FALSE;
	m_bBandArp = FALSE;
	m_iEcho = 30;
	m_iTimeOut = 3;
	m_bStartMode = FALSE;
}

CUserCfg::~CUserCfg()
{
}

void CUserCfg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_AUTOCERT, m_bAutoCert);
	DDX_Check(pDX, IDC_AUTOMIN, m_bAutoMin);
	DDX_Check(pDX, IDC_AUTORUN, m_bAutoRun);
	DDX_Control(pDX, IDC_DHCP, m_DhcpMode);
	DDX_Text(pDX, IDC_EDIT_ECHO, m_iEcho);
	DDV_MinMaxInt(pDX, m_iEcho, 1, 999);
	DDX_Text(pDX, IDC_EDIT_TIMEOUT, m_iTimeOut);
	DDV_MinMaxInt(pDX, m_iTimeOut, 1, 99);
	DDX_Radio(pDX, IDC_NORMAL, m_bStartMode);
	DDX_Check(pDX, IDC_SAVEPASS, m_bSavePass);
	DDX_Check(pDX, IDC_ARP_BIND, m_bBandArp);
}


BEGIN_MESSAGE_MAP(CUserCfg, CDialog)
END_MESSAGE_MAP()


// CUserCfg 消息处理程序

BOOL CUserCfg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	m_DhcpMode.AddString(_T("不使用"));
	m_DhcpMode.AddString(_T("二次认证"));
	m_DhcpMode.AddString(_T("认证后"));
	m_DhcpMode.AddString(_T("认证前"));
	m_DhcpMode.SetCurSel(0);
	GetCfg();
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CUserCfg::GetCfg()
{
	CString Mode= AfxGetApp()->GetProfileString(_T("Parameters"), _T("CertMode"), _T("0101030031")),
		Flag = AfxGetApp()->GetProfileString(_T("Parameters"), _T("CertFlag"), _T("01"));
	if(Mode[0]-'0')
		m_bAutoRun = TRUE;
	else
		m_bAutoRun = FALSE;
	if(Mode[1]-'0')
		m_bAutoMin = TRUE;
	else
		m_bAutoMin = FALSE;
	if(Mode[2]-'0'<0 || Mode[2]-'0'>3)
		m_DhcpMode.SetCurSel(0);
	else
		m_DhcpMode.SetCurSel(Mode[2]-'0');
	if(Mode[3]-'0')
		m_bBandArp = TRUE;
	else
		m_bBandArp = FALSE;
	m_iEcho = (Mode[4]-'0')*100 + (Mode[5]-'0')*10 +(Mode[6]-'0');
	m_iTimeOut = (Mode[7]-'0')*10 + (Mode[8]-'0');
	if(Mode[9]-'0')
		m_bStartMode = TRUE;
	else
		m_bStartMode = FALSE;
	if(Flag[0]-'0')
		m_bAutoCert = TRUE;
	else
		m_bAutoCert = FALSE;
	if(Flag[1]-'0')
		m_bSavePass = TRUE;
	else
		m_bSavePass = FALSE;
	UpdateData(FALSE);
}

bool CUserCfg::SaveCfg()
{
	if(UpdateData())
	{
		CString Mode, Flag;
		Mode.Format(_T("%d%d%d%d%03d%02d%d"),
			m_bAutoRun, m_bAutoMin, m_DhcpMode.GetCurSel(), m_bBandArp, m_iEcho, m_iTimeOut, m_bStartMode);
		Flag.Format(_T("%d%d"), m_bAutoCert, m_bSavePass);
		if(!m_bSavePass)
			AfxGetApp()->WriteProfileString(_T("Parameters"), _T("PassWord"), _T(""));
		if(!AfxGetApp()->WriteProfileString(_T("Parameters"), _T("CertMode"), Mode)
			||!AfxGetApp()->WriteProfileString(_T("Parameters"), _T("CertFlag"), Flag))
		{
			MessageBox(_T("保存设置失败！"), _T("设置提示"), MB_OK | MB_ICONWARNING );
		}
		HKEY hKey;
		RegOpenKeyEx( HKEY_LOCAL_MACHINE, _T("Software\\Microsoft\\Windows\\CurrentVersion\\Run"), 0, KEY_ALL_ACCESS, &hKey);
		if(m_bAutoRun)
		{
			TCHAR FileName[MAX_PATH];
			memset(FileName, 0, MAX_PATH*sizeof(TCHAR));
			GetModuleFileName(AfxGetInstanceHandle(), FileName+1, MAX_PATH-2);
			FileName[0] = FileName[lstrlen(FileName+1)+1] = '"';
			LPBYTE lpData = (LPBYTE)(LPCTSTR)FileName;
			if( RegSetValueEx(hKey, _T("MentoHUST"), 0, REG_SZ, lpData, lstrlen(FileName)*sizeof(TCHAR)) != ERROR_SUCCESS )
				MessageBox(_T("设为开机自动运行失败！"), _T("设置提示"), MB_OK | MB_ICONWARNING );
		}
		else
			RegDeleteValue(hKey, _T("MentoHUST"));
		RegCloseKey(hKey);
		return(true);
	}
	return(false);
}