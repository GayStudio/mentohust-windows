#pragma once
#include "afxcmn.h"
#include "UserCfg.h"
#include "ExtraCfg.h"
// CCfgDlg �Ի���

class CCfgDlg : public CDialog
{
	DECLARE_DYNAMIC(CCfgDlg)

public:
	CCfgDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CCfgDlg();

// �Ի�������
	enum { IDD = IDD_CFG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();
	afx_msg void OnTcnSelchangeTab(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnSave();
	afx_msg void OnOK();
	DECLARE_MESSAGE_MAP()
private:
	CTabCtrl m_Tab;
	CUserCfg cfgPage1;
	CExtraCfg cfgPage2;
	void InitTabPages();
	bool SaveCfg();
};
