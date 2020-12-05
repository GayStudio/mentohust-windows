#pragma once
#include "afxcmn.h"
#define IDT_START			(WM_USER+100)
#define IDT_SUCCESS			(WM_USER+101)
#define IDT_NULL			(WM_USER+102)

// CExtraCfg 对话框
UINT CatchThreadFunc( LPVOID pParam );	//认证线程
void SetHomePath();	
class CExtraCfg : public CDialog
{
	DECLARE_DYNAMIC(CExtraCfg)

public:
	CExtraCfg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CExtraCfg();

// 对话框数据
	enum { IDD = IDD_EXTRACFG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedBnScan();
	afx_msg void OnBnClickedBnCatch();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnDestroy();
	DECLARE_MESSAGE_MAP()
private:
	CWinThread *m_CatchThread;//抓包线程
	BOOL m_bCaption;		//是否自定义标题
	BOOL m_bPackage;		//是否自定义包
	BOOL m_bMac;			//是否伪造MAC
	BOOL m_bIP;				//是否伪造IP
	CString m_sCaption;		//标题
	CString m_sPath;		//包路径
	CString m_sMac;			//自定义MAC
	CIPAddressCtrl m_IP;	//IP控件

	void InitData();		//初始化数据
	void SetWinState();		//设置控件状态
	CString GetRuijiePath(bool reg = true);	//获取锐捷路径
	bool SaveRuijie(CString Ruijie);		//提取锐捷数据
	void SavePackage();						//保存数据包
public:
	bool CheckMac();			//检查MAC格式
	bool SaveCfg();				//保存配置
	int m_iType;				//抓包类型 Start Md5 结束
	BYTE bStartPackage[0x150];	//Start包数据
};
