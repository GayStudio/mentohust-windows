// MentoHUST.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "MentoHUST.h"
#include "MentoHUSTDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static const unsigned char base64Tab[] = {"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"};
static const char xorRuijie[] = {"~!:?$*<(qw2e5o7i8x12c6m67s98w43d2l45we82q3iuu1z4xle23rt4oxclle34e54u6r8m"};

static BOOL CALLBACK EnumWndProc(HWND hwnd,LPARAM lParam);
/////////////////////////////////////////////////////////////////////////////
// CMentoHUSTApp

BEGIN_MESSAGE_MAP(CMentoHUSTApp, CWinApp)
	//{{AFX_MSG_MAP(CMentoHUSTApp)
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMentoHUSTApp construction

CMentoHUSTApp::CMentoHUSTApp()
{
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CMentoHUSTApp object

CMentoHUSTApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CMentoHUSTApp initialization

BOOL CMentoHUSTApp::InitInstance()
{
	HWND oldHWnd = NULL;
	EnumWindows(EnumWndProc,(LPARAM)&oldHWnd);	//枚举所有运行的窗口
	if (oldHWnd != NULL)
	{
		::SendMessage(oldHWnd, WM_COMMAND, ID_MENU_RESTORE, 0);
		return FALSE;
	}

	CMentoHUSTDlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
	}
	else if (nResponse == IDCANCEL)
	{
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

static BOOL CALLBACK EnumWndProc(HWND hwnd,LPARAM lParam)	//枚举窗口
{
	HANDLE h = GetProp(hwnd, GUI_ID);
	if (h == (HANDLE)1)
	{
		*(HWND *)lParam = hwnd;
		return FALSE;
	}
	return TRUE;
}

CString LoadString(UINT nID)
{
	CString strTemp;
	strTemp.LoadString(nID);
	return strTemp;
}

CString GetAppPath()
{
	CString strPath;
	GetModuleFileName(AfxGetInstanceHandle(), strPath.GetBuffer(MAX_PATH), MAX_PATH);
	strPath.ReleaseBuffer();
	return strPath.Left(strPath.ReverseFind('\\')+1);
}

CString ConvertToUTF8(LPCTSTR strSrc)
{
	CString strUTF8;
	int len;

#ifndef UNICODE
	CString strTemp;
	len = MultiByteToWideChar(CP_ACP, 0, strSrc, -1, NULL, 0);
	LPWSTR szTemp = (LPWSTR)strTemp.GetBuffer(len * 2 + 2);
	MultiByteToWideChar(CP_ACP, 0, strSrc, -1, szTemp, len);
	strTemp.ReleaseBuffer();
	strSrc = strTemp;
#endif
	len = WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)strSrc, -1, NULL, 0, NULL, NULL);
	LPSTR strDst = (LPSTR)strUTF8.GetBuffer(len+ 1);
	WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)strSrc, -1, strDst, len, NULL, NULL);
	strUTF8.ReleaseBuffer();
	return strUTF8;
}

int EncodeRuijie(char *dst, const char *osrc)
{
    unsigned char in[3], buf[70];
	unsigned char *src = buf;
	int sz = strlen(osrc);
    int i, len;
	if ((int)strlen(xorRuijie) < sz)
		return 0;
	for(i=0; i<sz; i++)
		src[i] = osrc[i] ^ xorRuijie[i];
    while (sz > 0)
    {
        for (len=0, i=0; i<3; i++, sz--)
        {
			if (sz > 0)
			{
				len++;
				in[i] = src[i];
            }
			else
				in[i] = 0;
        }
        src += 3;
        if (len)
        {
			dst[0] = base64Tab[ in[0] >> 2 ];
			dst[1] = base64Tab[ ((in[0] & 0x03) << 4) | ((in[1] & 0xf0) >> 4) ];
			dst[2] = len > 1 ? base64Tab[ ((in[1] & 0x0f) << 2) | ((in[2] & 0xc0) >> 6) ] : '=';
			dst[3] = len > 2 ? base64Tab[ in[2] & 0x3f ] : '=';
            dst += 4;
        }
    }
    *dst = '\0';
	return 1;
}

int DecodeRuijie(char *dst, const char *src)
{
	UINT32 esi = 0, idx = 0;
	int i=0, j=0, equal=0;
	for(; src[i]!='\0'; i++)
	{
		if (src[i] == '=')
		{
			if (++equal > 2)
				return 0;
		}
		else
		{
			for(idx=0; base64Tab[idx]!='\0'; idx++)
			{
				if(base64Tab[idx] == src[i])
					break;
			}
			if (idx == 64)
				return 0;
			esi += idx;
		}
		if(i%4 == 3)
		{
			dst[j++] = (char)(esi>>16);
			if(equal < 2)
				dst[j++] = (char)(esi>>8);
			if(equal < 1)
				dst[j++] = (char)esi;
			esi = 0;
			equal = 0;
		}
		esi <<= 6;
	}
	if (i%4!=0 || (int)strlen(xorRuijie)<j)
		return 0;
	for(i=0; i<j; i++)
		dst[i] ^= xorRuijie[i];
	dst[j] = '\0';
	return 1;
}
