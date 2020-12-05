// MentoHUST.cpp : ����Ӧ�ó��������Ϊ��
//Written By HustMoon

#include "stdafx.h"
#include "MentoHUST.h"
#include "MentoHUSTDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMentoHUSTApp

BEGIN_MESSAGE_MAP(CMentoHUSTApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


// CMentoHUSTApp ����

CMentoHUSTApp::CMentoHUSTApp()
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CHustMoonApp ����

CMentoHUSTApp theApp;

BOOL CALLBACK EnumWndProc(HWND hwnd,LPARAM lParam)	//ö�ٴ���
{
	HANDLE h = GetProp(hwnd, GUI_ID);
	if( h == (HANDLE)1)
	{
		*(HWND*)lParam = hwnd;
		return false;
	}
	return true;
}


// CMentoHUSTApp ��ʼ��

BOOL CMentoHUSTApp::InitInstance()
{
	HWND oldHWnd = NULL;
	EnumWindows(EnumWndProc,(LPARAM)&oldHWnd);	//ö���������еĴ���
	if(oldHWnd != NULL)
	{
		::SendMessage(oldHWnd, WM_COMMAND, IDSHELL_RESTORE, 0);
		return false;
	}
	
	CWinApp::InitInstance();
	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO: Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
	SetRegistryKey(_T("HustMoon Studio"));

	CMentoHUSTDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: �ڴ˷��ô����ʱ��
		//  ��ȷ�������رնԻ���Ĵ���
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: �ڴ˷��ô����ʱ��
		//  ��ȡ�������رնԻ���Ĵ���
	}

	// ���ڶԻ����ѹرգ����Խ����� FALSE �Ա��˳�Ӧ�ó���
	//  ����������Ӧ�ó������Ϣ�á�
	return FALSE;
}
