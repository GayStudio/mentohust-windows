#pragma once
#include "afxwin.h"


// CUserCfg �Ի���

class CUserCfg : public CDialog
{
	DECLARE_DYNAMIC(CUserCfg)

public:
	CUserCfg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CUserCfg();

// �Ի�������
	enum { IDD = IDD_USERCFG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
private:
	BOOL m_bAutoCert;		//�Զ���֤
	BOOL m_bAutoMin;		//�Զ���С��
	BOOL m_bAutoRun;		//�Զ�����
	BOOL m_bSavePass;		//��������
	CComboBox m_DhcpMode;	//DHCP ��ʽ
	BOOL m_bBandArp;		//��ARP
	int m_iEcho;			//��Ӧ���
	int m_iTimeOut;			//��֤��ʱ
	BOOL m_bStartMode;		//��֤ģʽ
	void GetCfg();			//��ȡ����
public:
	bool SaveCfg();			//��������
};
