// MentoHUSTDlg.h : 头文件
//Written By HustMoon

#pragma once
#include "afxwin.h"
#include "NicNode.h"
#include "CfgDlg.h"
#define GUI_ID _T("MentoHUST-EEAB45AC-A7DE-4e85-8B82-F238E9BD8512")
#define IDSHELL_RESTORE				(WM_USER+100)
#define IDSHELL_CONNECT				(WM_USER+101)
#define IDSHELL_EXIT				(WM_USER+102)
#define IDSHELL_HELP				(WM_USER+103)
#define IDSHELL_CFG					(WM_USER+104)

#define IDT_DISCONNECT				(WM_USER+200)
#define IDT_FIND_SERVER				(WM_USER+201)
#define IDT_ACK_NAME				(WM_USER+202)
#define IDT_ACK_PSWD				(WM_USER+203)
#define	IDT_ACK_ECHO				(WM_USER+204)
#define IDT_BIND_ARP				(WM_USER+205)
#define	IDT_DHCP_RENEW				(WM_USER+206)
#define IDT_INIT_DATA				(WM_USER+207)
#define CERT_CYCLE_TIMER			(WM_USER+208)

#define MAX_ERROR_REPEAT			3

union UL_BYTE
{
	ULONG  ul;
	BYTE  bt[4];
};
const UINT WM_TASKBARCREATED = ::RegisterWindowMessage(_T("TaskbarCreated"));
void SetHomePath();						//将当前目录设为程序所在目录
BYTE Alog(BYTE base);					//逆转取反算法
UINT CertThreadFunc( LPVOID pParam );	//认证线程
UINT DhcpThreadFunc( LPVOID pParam );	//DHCP线程
BYTE *ComputeHash(BYTE * src, int i);	//md5算法

// CMentoHUSTDlg 对话框
class CMentoHUSTDlg : public CDialog
{
// 构造
public:
	CMentoHUSTDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_MENTOHUST_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//LRESULT OnTaskBarCreated(WPARAM wParam, LPARAM lParam);	//任务栏重启后重建图标
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnClose();			//退出按钮
	afx_msg void OnOK();			//解决回车退出
	afx_msg void OnCancel();		//解决ESC退出
	afx_msg void OnBnOutput();		//输出按钮
	afx_msg void OnBnOption();		//设置按钮
	afx_msg void OnBnAbout();		//关于按钮
	afx_msg void OnBnHelp();		//帮助按钮
	afx_msg void OnBnClear();		//清除按钮
	afx_msg void OnBnConnect();		//认证按钮
	DECLARE_MESSAGE_MAP()

private:
	CWinThread *m_CertThread;	//认证线程
	CWinThread *m_DhcpThread;	//DHCP线程
	NOTIFYICONDATA m_tnid;		//托盘图标
	CListBox m_MsgList;			//输出列表
	CString m_sName;			//用户名
	CString m_sPass;			//密码
	CComboBox m_NicList;		//网卡列表控件
	NicNode *m_NicHead;			//网卡列表链表
	int m_iCurrentNic;			//当前网卡
	BOOL m_bAutoCert;			//自动认证
	BOOL m_bSavePass;			//保存密码
	BOOL m_bAutoMin;			//自动最小化
	BOOL m_bStartMode;			//认证模式
	int m_iDhcpMode;			//DHCP方式
	int m_iEcho;				//响应间隔
	int m_iTimeOut;				//认证超时
	BOOL m_bPackage;			//是否自定义包
	BOOL m_bIP;					//是否伪造IP
	BOOL m_bMac;				//是否伪造Mac
	BOOL bTimerSign;			//定时器是否开启
	BYTE bPackage[0x3E8];		//待发送的数据包
	BYTE Package[0x167];		//开始数据包
	int iErrorTimes;			//失败次数
	int iLastState;				//上次状态

	void Blog(BYTE *circleBase, int num, BYTE *checkSum);
	void Clog(CString sPath);
	void InitInterface();			//初始化界面
	void ChangeTrayIcon();			//改变托盘图标
	void ToTray(bool hide = true);	//最小化到托盘
	bool Minimized();				//是否最小化
	bool InitNetworkAdapter();		//初始化网卡
	TCHAR OneCode(TCHAR srcchar);
	void CodeData(CString Str1, CString Str2, CString &AStr1, CString &AStr2);
	int Decode(char *dst, char *src);
	void GetPass(CString &Name, CString &Pass);	//读取锐捷密码信息
	void SaveCfg();					//保存配置
	void GetCfg();					//读取配置
	void GetCertCfg();				//读取认证配置
	void InitCertData();			//初始化认证数据
	void InitPackage(CString sPath, BOOL bStart);//初始化包
	void InitDefault(BOOL bStart);	//以默认数据初始化
	void InitHead();				//初始化包头
	bool GetNetInfo();				//获取网卡信息
	void StartCert();				//开始认证
	int SendStartPackage();			//Start包
	int SendIdentityPackage();		//Identity包
	int SendMD5ChallengePackage();	//Md5包
	int SendEchoPackage();			//Echo包
	int SendLogoffPackage();		//Logoff包
	void SendArpPackage();			//ARP包
	void SwitchState(int IState);	//改变状态
	void TimerCtrl(int timer);		//控制定时器
	bool SendPackage();				//控制发包

public:
	NicNode *m_pCurrentNic;		//当前网卡指针
	CString m_ServerMsg;		//服务器返回信息
	int m_iState;				//认证状态标志
	BOOL m_bBindArp;			//绑定ARP
	BYTE bLocalMAC[6];			//本机的MAC地址
	BYTE bDestMAC[6];			//服务器的MAC地址
	BYTE bGateMAC[6];			//网关MAC
	BYTE bMyMAC[6];				//伪造MAC
	BYTE bTempMAC[6];			//临时MAC
	UL_BYTE bIP;				//当前选择的网卡的IP地址
	UL_BYTE bMaskIP;			//当前选择的网卡的子网掩码
	UL_BYTE bGateIP;			//当前选择的网卡的网关
	UL_BYTE bDNS;				//当前选择的网卡的DNS
	BYTE bMD5Source[0x20];		//MD5验证时，对方发过来的代码
	UL_BYTE bEchoSource;		//Echo时的种子
	UL_BYTE bEchoNo;			//Echo时第二部分初始值

	void Report(CString Msg, int Type = 0, CString Title = _T("温馨提示"));
	bool GetNic();				//指向正确网卡
};
