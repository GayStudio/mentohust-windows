// CfgDlg.cpp : 实现文件
//Written By HustMoon

#include "stdafx.h"
#include "MentoHUST.h"
#include "CfgDlg.h"

// CCfgDlg 对话框

IMPLEMENT_DYNAMIC(CCfgDlg, CDialog)

CCfgDlg::CCfgDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCfgDlg::IDD, pParent)
{
}

CCfgDlg::~CCfgDlg()
{
}

void CCfgDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB, m_Tab);
}


BEGIN_MESSAGE_MAP(CCfgDlg, CDialog)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB, CCfgDlg::OnTcnSelchangeTab)
	ON_BN_CLICKED(IDC_SAVE, CCfgDlg::OnBnSave)
END_MESSAGE_MAP()


// CCfgDlg 消息处理程序

BOOL CCfgDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	InitTabPages();
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CCfgDlg::InitTabPages()
{
	m_Tab.InsertItem(0, _T("一般设置"));
	m_Tab.InsertItem(1, _T("高级设置"));
	cfgPage1.Create(IDD_USERCFG, GetDlgItem(IDC_TAB));
	cfgPage2.Create(IDD_EXTRACFG, GetDlgItem(IDC_TAB));
	CRect rs;
	m_Tab.GetClientRect(&rs);
	rs.top += 20;
	rs.left+=1;
	rs.right-=1;
	rs.bottom-=2;
	cfgPage1.MoveWindow(&rs);
	cfgPage2.MoveWindow(&rs);
	cfgPage1.ShowWindow(TRUE);
	cfgPage2.ShowWindow(FALSE);
	m_Tab.SetCurSel(0);
}

void CCfgDlg::OnTcnSelchangeTab(NMHDR *pNMHDR, LRESULT *pResult)
{
	switch ( m_Tab.GetCurSel() )
	{
	case 0:
		if(!cfgPage2.CheckMac())
		{
			m_Tab.SetCurSel(1);
			break;
		}
		cfgPage1.ShowWindow(TRUE);
		cfgPage2.ShowWindow(FALSE);
		break;
	case 1:
		if(!cfgPage1.UpdateData())
		{
			m_Tab.SetCurSel(0);
			break;
		}
		cfgPage1.ShowWindow(FALSE);
		cfgPage2.ShowWindow(TRUE);
		break;
	}
	*pResult = 0;
}

void CCfgDlg::OnOK()
{
	if(SaveCfg())
		CDialog::OnOK();
}
void CCfgDlg::OnBnSave()
{
	SaveCfg();
}

bool CCfgDlg::SaveCfg()
{
	bool result;
	if(result = cfgPage1.SaveCfg() && cfgPage2.SaveCfg())
		MessageBox(_T("设置成功保存！"), _T("设置提示"), MB_OK | MB_ICONWARNING);
	return(result);
}
