// MentoHUST.h : PROJECT_NAME 应用程序的主头文件
//Written By HustMoon

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号

#define MD5_PART_LENGTH		0x70000
// CMentoHUSTApp:
// 有关此类的实现，请参阅 MentoHUST.cpp
//

class CMentoHUSTApp : public CWinApp
{
public:
	CMentoHUSTApp();

// 重写
	public:
	virtual BOOL InitInstance();

// 实现

	DECLARE_MESSAGE_MAP()
};

extern CMentoHUSTApp theApp;