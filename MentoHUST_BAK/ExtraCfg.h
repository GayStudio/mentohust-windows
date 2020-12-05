#pragma once
#include "afxcmn.h"
#define IDT_START			(WM_USER+100)
#define IDT_SUCCESS			(WM_USER+101)
#define IDT_NULL			(WM_USER+102)

// CExtraCfg �Ի���
UINT CatchThreadFunc( LPVOID pParam );	//��֤�߳�
void SetHomePath();	
class CExtraCfg : public CDialog
{
	DECLARE_DYNAMIC(CExtraCfg)

public:
	CExtraCfg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CExtraCfg();

// �Ի�������
	enum { IDD = IDD_EXTRACFG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedBnScan();
	afx_msg void OnBnClickedBnCatch();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnDestroy();
	DECLARE_MESSAGE_MAP()
private:
	CWinThread *m_CatchThread;//ץ���߳�
	BOOL m_bCaption;		//�Ƿ��Զ������
	BOOL m_bPackage;		//�Ƿ��Զ����
	BOOL m_bMac;			//�Ƿ�α��MAC
	BOOL m_bIP;				//�Ƿ�α��IP
	CString m_sCaption;		//����
	CString m_sPath;		//��·��
	CString m_sMac;			//�Զ���MAC
	CIPAddressCtrl m_IP;	//IP�ؼ�

	void InitData();		//��ʼ������
	void SetWinState();		//���ÿؼ�״̬
	CString GetRuijiePath(bool reg = true);	//��ȡ���·��
	bool SaveRuijie(CString Ruijie);		//��ȡ�������
	void SavePackage();						//�������ݰ�
public:
	bool CheckMac();			//���MAC��ʽ
	bool SaveCfg();				//��������
	int m_iType;				//ץ������ Start Md5 ����
	BYTE bStartPackage[0x150];	//Start������
};
