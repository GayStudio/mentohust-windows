#pragma once
#include "afxwin.h"


// CUserCfg 对话框

class CUserCfg : public CDialog
{
	DECLARE_DYNAMIC(CUserCfg)

public:
	CUserCfg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CUserCfg();

// 对话框数据
	enum { IDD = IDD_USERCFG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
private:
	BOOL m_bAutoCert;		//自动认证
	BOOL m_bAutoMin;		//自动最小化
	BOOL m_bAutoRun;		//自动运行
	BOOL m_bSavePass;		//保存密码
	CComboBox m_DhcpMode;	//DHCP 方式
	BOOL m_bBandArp;		//绑定ARP
	int m_iEcho;			//响应间隔
	int m_iTimeOut;			//认证超时
	BOOL m_bStartMode;		//认证模式
	void GetCfg();			//读取配置
public:
	bool SaveCfg();			//保存配置
};
