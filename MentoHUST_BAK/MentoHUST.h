// MentoHUST.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//Written By HustMoon

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������

#define MD5_PART_LENGTH		0x70000
// CMentoHUSTApp:
// �йش����ʵ�֣������ MentoHUST.cpp
//

class CMentoHUSTApp : public CWinApp
{
public:
	CMentoHUSTApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CMentoHUSTApp theApp;