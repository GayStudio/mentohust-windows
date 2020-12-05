// CfgDlg.cpp : ʵ���ļ�
//Written By HustMoon

#include "stdafx.h"
#include "MentoHUST.h"
#include "CfgDlg.h"

// CCfgDlg �Ի���

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


// CCfgDlg ��Ϣ�������

BOOL CCfgDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	InitTabPages();
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void CCfgDlg::InitTabPages()
{
	m_Tab.InsertItem(0, _T("һ������"));
	m_Tab.InsertItem(1, _T("�߼�����"));
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
		MessageBox(_T("���óɹ����棡"), _T("������ʾ"), MB_OK | MB_ICONWARNING);
	return(result);
}
