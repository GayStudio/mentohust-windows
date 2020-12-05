// MentoHUSTDlg.h : header file
//

#if !defined(AFX_MENTOHUSTDLG_H__19F6D5BF_21C1_4D7F_9717_417107227C55__INCLUDED_)
#define AFX_MENTOHUSTDLG_H__19F6D5BF_21C1_4D7F_9717_417107227C55__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "HyperLink.h"
#include "Process.h"
#include "StringList.h"

#define SHOW_TIMER			(WM_USER+101)
#define SEND_TIMER			(WM_USER+102)
#define TEST_TIMER			(WM_USER+103)
#define MEM_TIMER			(WM_USER+104)
#define END_CERT			(WM_USER+105)

const LPCTSTR GUI_ID = _T("MentoHUST-8BDAA526-CBE4-4797-8D3E-E63C8D362894");
const UINT WM_TASKBARCREATED = ::RegisterWindowMessage(_T("TaskbarCreated"));
/////////////////////////////////////////////////////////////////////////////
// CMentoHUSTDlg dialog

class CMentoHUSTDlg : public CDialog
{
// Construction
public:
	CMentoHUSTDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CMentoHUSTDlg)
	enum { IDD = IDD_MENTOHUST_DIALOG };
	CHyperLink	m_linkLogo;
	CComboBox	m_AdapterList;
	CComboBox	m_AccountList;
	CString	m_sStateArea;
	CString	m_sStateBoard;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMentoHUSTDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CMentoHUSTDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBnAbout();
	afx_msg void OnBnClear();
	afx_msg void OnBnHelp();
	afx_msg void OnBnOption();
	afx_msg void OnBnOutput();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	int ShowDlg(CDialog *dlg);	//��ʾ�����ڡ��͡����á��Ի���
	void MyInitDialog();		//��ʼ���Ի���
	void InitInterface();		//��ʼ������
	void ReadConfig();			//��ȡ����
	void InitTrayIcon();		//��ʼ������ͼ��
	void InitAccountList();		//��ʼ���˺��б�
	void InitAdapterList();		//��ʼ�������б�
	BOOL ReadRuijieAccount(CString &strUsername);	//��ȡ���ע����˺�
	BOOL GetAccount();			//��ȡ�û�������IP
	BOOL GetAdapter();			//��ѡ�������
	void SaveSelect();			//����ѡ����˺ź�����
	void ToTray(int nType);		//��С��������
	
	int		m_iBigHeight;		//�����Ի���߶�
	int		m_iSmallHeight;		//���ز��ֺ�߶�
	BOOL	m_bChanged;			//�����Ƿ�ı�
	BOOL	m_bAutoCert;		//���к��Զ���֤��
	CProcess m_Process;			//��֤����
	NOTIFYICONDATA	m_tnid;		//����ͼ��
	StringList	*m_pAccountList;//�˺��б�
	StringList	*m_pAdapterList;//�����б�

public:
	int MsgBox(LPCTSTR lpszText, UINT nType = MB_OK|MB_ICONWARNING);	//��Ϣ��ʾ
	void Output(LPCTSTR Msg, int Type = 0);	//���״̬
	void ChangeTrayIcon();		//�ı�����ͼ��

	CString m_sServerMsg;	//��������Ϣ
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MENTOHUSTDLG_H__19F6D5BF_21C1_4D7F_9717_417107227C55__INCLUDED_)
