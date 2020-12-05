// MentoHUSTDlg.h : ͷ�ļ�
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
void SetHomePath();						//����ǰĿ¼��Ϊ��������Ŀ¼
BYTE Alog(BYTE base);					//��תȡ���㷨
UINT CertThreadFunc( LPVOID pParam );	//��֤�߳�
UINT DhcpThreadFunc( LPVOID pParam );	//DHCP�߳�
BYTE *ComputeHash(BYTE * src, int i);	//md5�㷨

// CMentoHUSTDlg �Ի���
class CMentoHUSTDlg : public CDialog
{
// ����
public:
	CMentoHUSTDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_MENTOHUST_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//LRESULT OnTaskBarCreated(WPARAM wParam, LPARAM lParam);	//�������������ؽ�ͼ��
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnClose();			//�˳���ť
	afx_msg void OnOK();			//����س��˳�
	afx_msg void OnCancel();		//���ESC�˳�
	afx_msg void OnBnOutput();		//�����ť
	afx_msg void OnBnOption();		//���ð�ť
	afx_msg void OnBnAbout();		//���ڰ�ť
	afx_msg void OnBnHelp();		//������ť
	afx_msg void OnBnClear();		//�����ť
	afx_msg void OnBnConnect();		//��֤��ť
	DECLARE_MESSAGE_MAP()

private:
	CWinThread *m_CertThread;	//��֤�߳�
	CWinThread *m_DhcpThread;	//DHCP�߳�
	NOTIFYICONDATA m_tnid;		//����ͼ��
	CListBox m_MsgList;			//����б�
	CString m_sName;			//�û���
	CString m_sPass;			//����
	CComboBox m_NicList;		//�����б�ؼ�
	NicNode *m_NicHead;			//�����б�����
	int m_iCurrentNic;			//��ǰ����
	BOOL m_bAutoCert;			//�Զ���֤
	BOOL m_bSavePass;			//��������
	BOOL m_bAutoMin;			//�Զ���С��
	BOOL m_bStartMode;			//��֤ģʽ
	int m_iDhcpMode;			//DHCP��ʽ
	int m_iEcho;				//��Ӧ���
	int m_iTimeOut;				//��֤��ʱ
	BOOL m_bPackage;			//�Ƿ��Զ����
	BOOL m_bIP;					//�Ƿ�α��IP
	BOOL m_bMac;				//�Ƿ�α��Mac
	BOOL bTimerSign;			//��ʱ���Ƿ���
	BYTE bPackage[0x3E8];		//�����͵����ݰ�
	BYTE Package[0x167];		//��ʼ���ݰ�
	int iErrorTimes;			//ʧ�ܴ���
	int iLastState;				//�ϴ�״̬

	void Blog(BYTE *circleBase, int num, BYTE *checkSum);
	void Clog(CString sPath);
	void InitInterface();			//��ʼ������
	void ChangeTrayIcon();			//�ı�����ͼ��
	void ToTray(bool hide = true);	//��С��������
	bool Minimized();				//�Ƿ���С��
	bool InitNetworkAdapter();		//��ʼ������
	TCHAR OneCode(TCHAR srcchar);
	void CodeData(CString Str1, CString Str2, CString &AStr1, CString &AStr2);
	int Decode(char *dst, char *src);
	void GetPass(CString &Name, CString &Pass);	//��ȡ���������Ϣ
	void SaveCfg();					//��������
	void GetCfg();					//��ȡ����
	void GetCertCfg();				//��ȡ��֤����
	void InitCertData();			//��ʼ����֤����
	void InitPackage(CString sPath, BOOL bStart);//��ʼ����
	void InitDefault(BOOL bStart);	//��Ĭ�����ݳ�ʼ��
	void InitHead();				//��ʼ����ͷ
	bool GetNetInfo();				//��ȡ������Ϣ
	void StartCert();				//��ʼ��֤
	int SendStartPackage();			//Start��
	int SendIdentityPackage();		//Identity��
	int SendMD5ChallengePackage();	//Md5��
	int SendEchoPackage();			//Echo��
	int SendLogoffPackage();		//Logoff��
	void SendArpPackage();			//ARP��
	void SwitchState(int IState);	//�ı�״̬
	void TimerCtrl(int timer);		//���ƶ�ʱ��
	bool SendPackage();				//���Ʒ���

public:
	NicNode *m_pCurrentNic;		//��ǰ����ָ��
	CString m_ServerMsg;		//������������Ϣ
	int m_iState;				//��֤״̬��־
	BOOL m_bBindArp;			//��ARP
	BYTE bLocalMAC[6];			//������MAC��ַ
	BYTE bDestMAC[6];			//��������MAC��ַ
	BYTE bGateMAC[6];			//����MAC
	BYTE bMyMAC[6];				//α��MAC
	BYTE bTempMAC[6];			//��ʱMAC
	UL_BYTE bIP;				//��ǰѡ���������IP��ַ
	UL_BYTE bMaskIP;			//��ǰѡ�����������������
	UL_BYTE bGateIP;			//��ǰѡ�������������
	UL_BYTE bDNS;				//��ǰѡ���������DNS
	BYTE bMD5Source[0x20];		//MD5��֤ʱ���Է��������Ĵ���
	UL_BYTE bEchoSource;		//Echoʱ������
	UL_BYTE bEchoNo;			//Echoʱ�ڶ����ֳ�ʼֵ

	void Report(CString Msg, int Type = 0, CString Title = _T("��ܰ��ʾ"));
	bool GetNic();				//ָ����ȷ����
};
