#pragma once
#include "afxcmn.h"
#include "UserCfg.h"
#include "ExtraCfg.h"
// CCfgDlg 对话框

class CCfgDlg : public CDialog
{
	DECLARE_DYNAMIC(CCfgDlg)

public:
	CCfgDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CCfgDlg();

// 对话框数据
	enum { IDD = IDD_CFG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
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
