// MentoHUST.cpp : 定义应用程序的类行为。
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


// CMentoHUSTApp 构造

CMentoHUSTApp::CMentoHUSTApp()
{
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 CHustMoonApp 对象

CMentoHUSTApp theApp;

BOOL CALLBACK EnumWndProc(HWND hwnd,LPARAM lParam)	//枚举窗口
{
	HANDLE h = GetProp(hwnd, GUI_ID);
	if( h == (HANDLE)1)
	{
		*(HWND*)lParam = hwnd;
		return false;
	}
	return true;
}


// CMentoHUSTApp 初始化

BOOL CMentoHUSTApp::InitInstance()
{
	HWND oldHWnd = NULL;
	EnumWindows(EnumWndProc,(LPARAM)&oldHWnd);	//枚举所有运行的窗口
	if(oldHWnd != NULL)
	{
		::SendMessage(oldHWnd, WM_COMMAND, IDSHELL_RESTORE, 0);
		return false;
	}
	
	CWinApp::InitInstance();
	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("HustMoon Studio"));

	CMentoHUSTDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: 在此放置处理何时用
		//  “确定”来关闭对话框的代码
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: 在此放置处理何时用
		//  “取消”来关闭对话框的代码
	}

	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	//  而不是启动应用程序的消息泵。
	return FALSE;
}
