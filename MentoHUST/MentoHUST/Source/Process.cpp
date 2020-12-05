// Process.cpp : implementation file
//

#include "stdafx.h"
#include "MentoHUST.h"
#include "Process.h"
#include "MentoHUSTDlg.h"
#include "md5.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#include "iphlpapi.h"

#define MAX_SEND_COUNT				3
static CMentoHUSTDlg *mainDlg = NULL;
static UINT CertThreadFunc(LPVOID pParam);
static void pcap_handle(u_char *user, const struct pcap_pkthdr *h, const u_char *buf);
static void printMsg(const u_char *buf, unsigned bufLen);
static void showMsg(const u_char *buf);	/* 显示服务器提示信息 */
/////////////////////////////////////////////////////////////////////////////
// CProcess

const u_char CProcess::STANDARD_ADDR[] = {0x01,0x80,0xC2,0x00,0x00,0x03};
const u_char CProcess::RUIJIE_ADDR[] = {0x01,0xD0,0xF8,0x00,0x00,0x03};

CProcess::CProcess()
{
	m_state = IDT_DISCONNECT;
	m_hPcap = NULL;
	m_dataBuf = NULL;
	m_dataSize = 0;
	m_pCertThread = NULL;
}

CProcess::~CProcess()
{
	if (m_hPcap != NULL)
		pcap_close(m_hPcap);
	if (m_dataBuf != NULL)
		free(m_dataBuf);
}

void CProcess::startCert()
{
	if (mainDlg == NULL)
		mainDlg = (CMentoHUSTDlg *)AfxGetApp()->m_pMainWnd;
	if (!getAddress() || !openAdapter())
		return;
	getSize();
	fillHeader();
	m_pCertThread = AfxBeginThread(CertThreadFunc, this, THREAD_PRIORITY_TIME_CRITICAL, CREATE_SUSPENDED);
	m_pCertThread->m_bAutoDelete = FALSE;
	m_pCertThread->ResumeThread();
	mainDlg->SetDlgItemText(IDOK, LoadString(IDS_BN_DISCONNECT));
	mainDlg->GetDlgItem(IDC_CB_ACCOUNT)->EnableWindow(FALSE);
	mainDlg->GetDlgItem(IDC_CB_ADAPTER)->EnableWindow(FALSE);
	return;
}

BOOL CProcess::getAddress()
{
	PIP_ADAPTER_INFO pAdapterInfo = (PIP_ADAPTER_INFO)malloc(sizeof(IP_ADAPTER_INFO));
	ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);
	if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW) 
	{
		free(pAdapterInfo);
		pAdapterInfo = (PIP_ADAPTER_INFO)malloc(ulOutBufLen);
	}
	BOOL bFound = FALSE;
	if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == NO_ERROR)
	{
		const char *nic = m_nic + (strlen(m_nic) - 38);
		for (PIP_ADAPTER_INFO pAdapter=pAdapterInfo; pAdapter; pAdapter=pAdapter->Next)
		{
			if (strcmp(nic, pAdapter->AdapterName) == 0)
			{
				memcpy(m_localMAC, pAdapter->Address, 6);
				m_rip = inet_addr(pAdapter->IpAddressList.IpAddress.String);
				m_mask = inet_addr(pAdapter->IpAddressList.IpMask.String);
				m_gateway = inet_addr(pAdapter->GatewayList.IpAddress.String);
				bFound = TRUE;
				break;
			}
		}
	}
	free(pAdapterInfo);
	if (!bFound)
	{
		mainDlg->MsgBox(LoadString(IDS_ADDRESS_ERROR));
		return FALSE;
	}

	PFIXED_INFO pFixedInfo = (PFIXED_INFO)malloc(sizeof(FIXED_INFO));
	ulOutBufLen = sizeof(FIXED_INFO);
	if (GetNetworkParams(pFixedInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW) 
	{
		free(pFixedInfo);
		pFixedInfo = (PFIXED_INFO)malloc(ulOutBufLen);
	}
	if (GetNetworkParams(pFixedInfo, &ulOutBufLen) == NO_ERROR)
		m_dns = inet_addr(pFixedInfo->DnsServerList.IpAddress.String);
	free(pFixedInfo);
	
	CString strTemp;
	strTemp.Format(LoadString(IDS_LOCALMAC), m_localMAC[0], m_localMAC[1], m_localMAC[2], m_localMAC[3], m_localMAC[4], m_localMAC[5]);
	mainDlg->Output(strTemp, 2);
	u_char *ip = (u_char *)&m_rip;
	strTemp.Format(LoadString(IDS_LOCALIP), ip[0], ip[1], ip[2], ip[3]);
	mainDlg->Output(strTemp, 2);
	if (m_dhcpMode==0 && m_ip!=0 && m_ip!=m_rip)
	{
		ip = (u_char *)&m_ip;
		strTemp.Format(LoadString(IDS_BANDIP), ip[0], ip[1], ip[2], ip[3]);
		mainDlg->Output(strTemp, 2);
	}
	else
		m_ip = m_rip;
	ip = (u_char *)&m_mask;
	strTemp.Format(LoadString(IDS_LOCALMASK), ip[0], ip[1], ip[2], ip[3]);
	mainDlg->Output(strTemp, 2);
	ip = (u_char *)&m_gateway;
	strTemp.Format(LoadString(IDS_LOCALGATE), ip[0], ip[1], ip[2], ip[3]);
	mainDlg->Output(strTemp, 2);
	ip = (u_char *)&m_dns;
	strTemp.Format(LoadString(IDS_LOCALDNS), ip[0], ip[1], ip[2], ip[3]);
	mainDlg->Output(strTemp, 2);
	return TRUE;
}

BOOL CProcess::openAdapter()
{
	if (m_hPcap != NULL)
		pcap_close(m_hPcap);
	char buf[PCAP_ERRBUF_SIZE];
	if ((m_hPcap = pcap_open_live(m_nic, 65535, 1, 500, buf)) == NULL)
	{
		mainDlg->MsgBox(LoadString(IDS_OPEN_ERROR));
		return FALSE;
	}
	struct bpf_program fcode;
	char mac[20];
	sprintf(mac, "%02x:%02x:%02x:%02x:%02x:%02x", m_localMAC[0], m_localMAC[1], \
		m_localMAC[2], m_localMAC[3], m_localMAC[4], m_localMAC[5]);
		sprintf(buf, "(ether proto 0x888e and (ether dst %s or ether dst 01:80:c2:00:00:03) \
			and not ether src %s) or ether proto 0x0806", mac, mac);
	if (pcap_compile(m_hPcap, &fcode, buf, 1, 0xffffff) == -1
		|| pcap_setfilter(m_hPcap, &fcode) == -1)
	{
		pcap_close(m_hPcap);
		mainDlg->MsgBox(LoadString(IDS_FILTER_ERROR));
		return FALSE;
	}
	pcap_freecode(&fcode);
	return TRUE;
}

void CProcess::getSize()
{
	FILE *fp;
	if (m_dataBuf != NULL)
		free(m_dataBuf);
	if (!m_usePackage || m_package[0]== _T('\0'))
		goto fileError;
	u_char buf[16];
	if ((fp = _tfopen(m_package, _T("rb"))) == NULL)
		goto fileError;
	if (fread(buf, 16, 1, fp) < 1)
	{
		fclose(fp);
		goto fileError;
	}
/*	m_readSize = *(u_int32_t *)buf ^ *(u_int32_t *)(buf + 4);
	SYSTEMTIME stLocal;
	::GetLocalTime(&stLocal);
	m_checkSize = stLocal.wYear*10000 + stLocal.wMonth*100 + stLocal.wDay;
	if (m_readSize < m_checkSize)
		goto fileError;*/
	m_readSize = (*(u_int32_t *)buf ^ *(u_int32_t *)(buf + 8)) + 16;
	m_checkSize = (*(u_int32_t *)buf ^ *(u_int32_t *)(buf + 12)) + 16;
	fseek(fp, 0, SEEK_END);
	m_dataSize = ftell(fp);
	fclose(fp);
	if (m_checkSize<m_readSize || m_dataSize<m_readSize)
		goto fileError;
	m_dataSize = (m_dataSize - m_readSize) / 2 + 0x17;
	if (m_dataSize < 0x80)
		goto fileError;
	m_dataBuf = (u_char *)malloc(m_dataSize);
	return;

fileError:
	if (m_usePackage)
	{
		m_usePackage = FALSE;
		mainDlg->Output(LoadString(IDS_PACKAGE_ERROR), 2);
	}
	m_dataSize = 0x80;
	m_dataBuf = (u_char *)malloc(m_dataSize);
}

static UINT CertThreadFunc(LPVOID pParam)
{
	CProcess *proc = (CProcess *)pParam;
	if (proc->m_dhcpMode == 3)	  /* 认证前DHCP */
		proc->switchState(IDT_DHCP);
	else
		proc->switchState(IDT_START);
	if (-1 == pcap_loop(proc->m_hPcap, -1, pcap_handle, (u_char *)pParam)) /* 开始捕获数据包 */
	{
		mainDlg->MsgBox(LoadString(IDS_CATCH_ERROR));
		mainDlg->ShowWindow(SW_SHOWNORMAL);
		mainDlg->PostMessage(WM_SYSCOMMAND, END_CERT, 0);
		return -1;
	}
	return  0;
}

static void pcap_handle(u_char *user, const struct pcap_pkthdr *h, const u_char *buf)
{
	CProcess *proc = (CProcess *)user;
	CString strTemp;
	if (buf[0x0c]==0x88 && buf[0x0d]==0x8e)
	{
		if (memcmp(proc->m_servMAC, buf+6, 6)!=0 && proc->m_startMode>=2)	/* 服务器MAC地址不符 */
			return;
		proc->m_pcapBuf = buf;
		if (buf[0x0F]==0x00 && buf[0x12]==0x01 && buf[0x16]==0x01)	/* 验证用户名 */
		{
			if (proc->m_startMode < 2)
			{
				memcpy(proc->m_servMAC, buf+6, 6);
				strTemp.Format(LoadString(IDS_SERVMAC), buf[6], buf[7], buf[8], buf[9], buf[10], buf[11]);
				mainDlg->Output(strTemp, 2);
				proc->m_startMode += 2;
			}
			mainDlg->m_sServerMsg.Empty();
			if (!proc->m_bCernet && memcmp(buf+0x17, "User name", 9)==0)
			{
				proc->m_bCernet = TRUE;
				CString strFileName = GetAppPath() + _T("Config.ini");
				WritePrivateProfileString(_T("Parameters"), _T("ClientType"), _T("1"), strFileName);
			}
			proc->switchState(IDT_IDENTITY);
		}
		else if (buf[0x0F]==0x00 && buf[0x12]==0x01 && buf[0x16]==0x04)	/* 验证密码 */
			proc->switchState(IDT_CHALLENGE);
		else if (buf[0x0F]==0x00 && buf[0x12]==0x03)	/* 认证成功 */
		{
			mainDlg->Output(LoadString(IDS_STATE_SUCCESS));
			if (proc->m_autoMin)
				mainDlg->PostMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0);
			if (!proc->m_bCernet)
			{
				proc->getEchoKey(buf);
				printMsg(buf, h->caplen);
			}
			if (proc->m_dhcpMode==1 || proc->m_dhcpMode==2)
				proc->switchState(IDT_DHCP);
			else if (proc->m_bCernet)
				proc->switchState(IDT_WAITECHO);
			else
				proc->switchState(IDT_ECHO);
		}
		else if (buf[0x0F]==0x00 && buf[0x12]==0x01 && buf[0x16]==0x02)	/* 显示提示信息 */
			showMsg(buf);
		else if (buf[0x0F] == 0x05)	/* 响应在线 */
		{
			mainDlg->m_sServerMsg.Empty();
			proc->switchState(IDT_ECHO);
		}
		else if (buf[0x0F]==0x00 && buf[0x12]==0x04)  /* 认证失败或被踢下线 */
		{
			if (proc->m_state==IDT_WAITECHO || proc->m_state==IDT_ECHO)
			{
				mainDlg->Output(LoadString(IDS_CERT_DOWN));
				proc->switchState(IDT_START);
			}
			else if (buf[0x1b]!=0 || proc->m_bCernet)
			{
				mainDlg->Output(LoadString(IDS_CERT_FAILED));
				if (!proc->m_bCernet)
					printMsg(buf, h->caplen);
				proc->restart();
			}
			else
				proc->switchState(IDT_START);
		}
		mainDlg->ChangeTrayIcon();
	}
	else if (buf[0x0c] ==0x08 && buf[0x0d]==0x06 && proc->m_bandMac!=0)
	{
		if (*(u_int32_t *)(buf+0x1c) == proc->m_gateway)
		{
			if (proc->m_bandMac == 1)
			{
				memcpy(proc->m_gateMAC, buf+0x16, 6);
				strTemp.Format(LoadString(IDS_GATEMAC), buf[0x16], buf[0x17], buf[0x18], buf[0x19], buf[0x1a], buf[0x1b]);
				mainDlg->Output(strTemp, 2);
				u_char *ip = (u_char *)&proc->m_gateway;
				strTemp.Format(_T("-s %d.%d.%d.%d %02x-%02x-%02x-%02x-%02x-%02x"), ip[0], ip[1], ip[2], ip[3],
					buf[0x16], buf[0x17], buf[0x18], buf[0x19], buf[0x1a], buf[0x1b]);
				ShellExecute(NULL, _T("open"), _T("arp"), strTemp, NULL, SW_HIDE);
				proc->m_bandMac = 2;
			}
			else if (buf[0x15]==0x02 && *(u_int32_t *)(buf+0x26)==proc->m_rip)
			{
				if (memcmp(proc->m_gateMAC, buf+0x16, 6) != 0)
				{
					mainDlg->m_sServerMsg.Format(LoadString(IDS_ARP_ALERT),
						buf[0x16], buf[0x17], buf[0x18], buf[0x19], buf[0x1a], buf[0x1b]);
					mainDlg->Output(mainDlg->m_sServerMsg);
					mainDlg->ChangeTrayIcon();
				}
			}
		}
	}
}

void CProcess::restart()
{
	mainDlg->KillTimer(SEND_TIMER);
	if (m_startMode >= 2) m_startMode -= 2;
	CString strTemp;
	strTemp.Format(LoadString(IDS_RESTART), m_restart/1000);
	mainDlg->Output(strTemp);
	m_state = IDT_START;
	m_sendCount = -1;
	mainDlg->SetTimer(SEND_TIMER, m_restart, NULL);
}

static void printMsg(const u_char *buf, unsigned bufLen)
{
	char *serverMsg;
	unsigned length = buf[0x1b];
	CString strTemp;
	if (length > 0)
	{
		for (serverMsg=(char *)(buf+0x1c); *serverMsg==' '||*serverMsg=='\r'||*serverMsg=='\n'; serverMsg++,length--);	/* 跳过开头的换行符 */
		mainDlg->m_sServerMsg = serverMsg;
		if (!mainDlg->m_sServerMsg.IsEmpty())
			mainDlg->Output(LoadString(IDS_SERVMSG1) + mainDlg->m_sServerMsg);
	}
	if ((length=0x1c+buf[0x1b]+0x69+39) < bufLen)
	{
		serverMsg=(char *)(buf+length);
		if ((unsigned)(buf[length-1]-2) > bufLen-length)
			length = bufLen - length;
		else
			length = buf[length-1]-2;
		for (; *serverMsg==' '||*serverMsg=='\r'||*serverMsg=='\n'; serverMsg++,length--);
		char msg[257];
		strncpy(msg, serverMsg, length);
		msg[length] = '\0';
		mainDlg->m_sServerMsg = msg;
		if (!mainDlg->m_sServerMsg.IsEmpty())
			mainDlg->Output(LoadString(IDS_SERVMSG2) + mainDlg->m_sServerMsg);
	}
}

void CProcess::endCert()
{
	mainDlg->GetDlgItem(IDOK)->EnableWindow(FALSE);
	switchState(IDT_DISCONNECT);
	pcap_breakloop(m_hPcap);
	WaitForSingleObject(m_pCertThread->m_hThread, INFINITE);
	delete m_pCertThread;
	m_pCertThread = NULL;
	if (m_bandMac == 2) m_bandMac = 1;
	if (m_dhcpMode >= 4) m_dhcpMode -= 3;
	if (m_startMode >= 2) m_startMode -= 2;
	mainDlg->Output(LoadString(IDS_STATE_DISCONNECT));
	mainDlg->SetDlgItemText(IDOK, LoadString(IDS_BN_CONNECT));
	mainDlg->GetDlgItem(IDC_CB_ACCOUNT)->EnableWindow(TRUE);
	mainDlg->GetDlgItem(IDC_CB_ADAPTER)->EnableWindow(TRUE);
	mainDlg->GetDlgItem(IDOK)->EnableWindow(TRUE);
	mainDlg->m_sServerMsg.Empty();
	mainDlg->ChangeTrayIcon();
}

u_char CProcess::encode(u_char base)	/* 算法，将一个字节的8位颠倒并取反 */
{
	u_char result = 0;
	int i;
	for (i=0; i<8; i++)
	{
		result <<= 1;
		result |= base&0x01;
		base >>= 1;
	}
	return(~result);
}

void CProcess::checkSum()	/* 算法，计算两个字节的checksum */
{
	u_char table[] =
	{
		0x00,0x00,0x21,0x10,0x42,0x20,0x63,0x30,0x84,0x40,0xA5,0x50,0xC6,0x60,0xE7,0x70,
		0x08,0x81,0x29,0x91,0x4A,0xA1,0x6B,0xB1,0x8C,0xC1,0xAD,0xD1,0xCE,0xE1,0xEF,0xF1,
		0x31,0x12,0x10,0x02,0x73,0x32,0x52,0x22,0xB5,0x52,0x94,0x42,0xF7,0x72,0xD6,0x62,
		0x39,0x93,0x18,0x83,0x7B,0xB3,0x5A,0xA3,0xBD,0xD3,0x9C,0xC3,0xFF,0xF3,0xDE,0xE3,
		0x62,0x24,0x43,0x34,0x20,0x04,0x01,0x14,0xE6,0x64,0xC7,0x74,0xA4,0x44,0x85,0x54,
		0x6A,0xA5,0x4B,0xB5,0x28,0x85,0x09,0x95,0xEE,0xE5,0xCF,0xF5,0xAC,0xC5,0x8D,0xD5,
		0x53,0x36,0x72,0x26,0x11,0x16,0x30,0x06,0xD7,0x76,0xF6,0x66,0x95,0x56,0xB4,0x46,
		0x5B,0xB7,0x7A,0xA7,0x19,0x97,0x38,0x87,0xDF,0xF7,0xFE,0xE7,0x9D,0xD7,0xBC,0xC7,
		0xC4,0x48,0xE5,0x58,0x86,0x68,0xA7,0x78,0x40,0x08,0x61,0x18,0x02,0x28,0x23,0x38,
		0xCC,0xC9,0xED,0xD9,0x8E,0xE9,0xAF,0xF9,0x48,0x89,0x69,0x99,0x0A,0xA9,0x2B,0xB9,
		0xF5,0x5A,0xD4,0x4A,0xB7,0x7A,0x96,0x6A,0x71,0x1A,0x50,0x0A,0x33,0x3A,0x12,0x2A,
		0xFD,0xDB,0xDC,0xCB,0xBF,0xFB,0x9E,0xEB,0x79,0x9B,0x58,0x8B,0x3B,0xBB,0x1A,0xAB,
		0xA6,0x6C,0x87,0x7C,0xE4,0x4C,0xC5,0x5C,0x22,0x2C,0x03,0x3C,0x60,0x0C,0x41,0x1C,
		0xAE,0xED,0x8F,0xFD,0xEC,0xCD,0xCD,0xDD,0x2A,0xAD,0x0B,0xBD,0x68,0x8D,0x49,0x9D,
		0x97,0x7E,0xB6,0x6E,0xD5,0x5E,0xF4,0x4E,0x13,0x3E,0x32,0x2E,0x51,0x1E,0x70,0x0E,
		0x9F,0xFF,0xBE,0xEF,0xDD,0xDF,0xFC,0xCF,0x1B,0xBF,0x3A,0xAF,0x59,0x9F,0x78,0x8F,
		0x88,0x91,0xA9,0x81,0xCA,0xB1,0xEB,0xA1,0x0C,0xD1,0x2D,0xC1,0x4E,0xF1,0x6F,0xE1,
		0x80,0x10,0xA1,0x00,0xC2,0x30,0xE3,0x20,0x04,0x50,0x25,0x40,0x46,0x70,0x67,0x60,
		0xB9,0x83,0x98,0x93,0xFB,0xA3,0xDA,0xB3,0x3D,0xC3,0x1C,0xD3,0x7F,0xE3,0x5E,0xF3,
		0xB1,0x02,0x90,0x12,0xF3,0x22,0xD2,0x32,0x35,0x42,0x14,0x52,0x77,0x62,0x56,0x72,
		0xEA,0xB5,0xCB,0xA5,0xA8,0x95,0x89,0x85,0x6E,0xF5,0x4F,0xE5,0x2C,0xD5,0x0D,0xC5,
		0xE2,0x34,0xC3,0x24,0xA0,0x14,0x81,0x04,0x66,0x74,0x47,0x64,0x24,0x54,0x05,0x44,
		0xDB,0xA7,0xFA,0xB7,0x99,0x87,0xB8,0x97,0x5F,0xE7,0x7E,0xF7,0x1D,0xC7,0x3C,0xD7,
		0xD3,0x26,0xF2,0x36,0x91,0x06,0xB0,0x16,0x57,0x66,0x76,0x76,0x15,0x46,0x34,0x56,
		0x4C,0xD9,0x6D,0xC9,0x0E,0xF9,0x2F,0xE9,0xC8,0x99,0xE9,0x89,0x8A,0xB9,0xAB,0xA9,
		0x44,0x58,0x65,0x48,0x06,0x78,0x27,0x68,0xC0,0x18,0xE1,0x08,0x82,0x38,0xA3,0x28,
		0x7D,0xCB,0x5C,0xDB,0x3F,0xEB,0x1E,0xFB,0xF9,0x8B,0xD8,0x9B,0xBB,0xAB,0x9A,0xBB,
		0x75,0x4A,0x54,0x5A,0x37,0x6A,0x16,0x7A,0xF1,0x0A,0xD0,0x1A,0xB3,0x2A,0x92,0x3A,
		0x2E,0xFD,0x0F,0xED,0x6C,0xDD,0x4D,0xCD,0xAA,0xBD,0x8B,0xAD,0xE8,0x9D,0xC9,0x8D,
		0x26,0x7C,0x07,0x6C,0x64,0x5C,0x45,0x4C,0xA2,0x3C,0x83,0x2C,0xE0,0x1C,0xC1,0x0C,
		0x1F,0xEF,0x3E,0xFF,0x5D,0xCF,0x7C,0xDF,0x9B,0xAF,0xBA,0xBF,0xD9,0x8F,0xF8,0x9F,
		0x17,0x6E,0x36,0x7E,0x55,0x4E,0x74,0x5E,0x93,0x2E,0xB2,0x3E,0xD1,0x0E,0xF0,0x1E
	};
	u_char *checkSum = m_dataBuf + 0x15;
	int i, index;
	for (i=0; i<0x15; i++)
	{
		index = checkSum[0] ^ m_dataBuf[i];
		checkSum[0] = checkSum[1] ^ table[index*2+1];
		checkSum[1] = table[index*2];
	}
	for (i=0; i<0x17; i++)
		m_dataBuf[i] = encode(m_dataBuf[i]);
}

void CProcess::fillHeader()
{
	memset(m_dataBuf, 0, m_dataSize);
	m_dataBuf[0x02] = 0x13;
	m_dataBuf[0x03] = 0x11;
	if (m_dhcpMode != 0)
		m_dataBuf[0x04] = 0x01;		/* DHCP位，使用1、不使用0 */
	memcpy(m_dataBuf+0x05, &m_ip, 4);
	memcpy(m_dataBuf+0x09, &m_mask, 4);
	memcpy(m_dataBuf+0x0D, &m_gateway, 4);
	memcpy(m_dataBuf+0x11, &m_dns, 4);
	checkSum();
}

int CProcess::setVendor(u_char type, const u_char *value, int length)
{
	u_char *p = m_dataBuf + 0x46, *end = m_dataBuf + m_dataSize - length - 8;
	while (p <= end)
	{
		if (*p == 0x1a)
			p += 2;
		if (p[4] == type)
		{
			memcpy(p+4+p[5]-length, value, length);
			return 0;
		}
		p += p[5] + 4;
	}
	return -1;
}

int CProcess::readPacket(int type)
{
	u_char dhcp[] = {0x00};
	FILE *fp = _tfopen(m_package, _T("rb"));
	if (fp == NULL)
		goto fileError;
	type %= 2;
	fseek(fp, m_readSize+(m_dataSize-0x17)*type, SEEK_SET);
	if (fread(m_dataBuf+0x17, m_dataSize-0x17, 1, fp) < 1)
	{
		fclose(fp);
		goto fileError;
	}
	fclose(fp);
	if (m_dhcpMode == 1)
		dhcp[0] = 0x01;
	setVendor(0x18, dhcp, 1);
	setVendor(0x2D, m_localMAC, 6);
	return 0;

fileError:
	m_usePackage = FALSE;
	mainDlg->Output(LoadString(IDS_PACKAGE_ERROR), 2);
	return -1;
}

void CProcess::fillStartPacket()
{
	if (!m_usePackage)	/* 不使用数据包？ */
	{
		u_char packet[] =
		{
			0x00,0x00,0x13,0x11,0x38,0x30,0x32,0x31,0x78,0x2e,0x65,0x78,0x65,0x00,0x00,0x00,
			0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
			0x00,0x00,0x00,0x00,0x01,0x00,0x01,0x00,0x00,0x00,0x00,0x13,0x11,0x00,0x28,0x1a,
			0x28,0x00,0x00,0x13,0x11,0x17,0x22,0x91,0x66,0x64,0x93,0x67,0x60,0x65,0x62,0x62,
			0x94,0x61,0x69,0x67,0x63,0x91,0x93,0x92,0x68,0x66,0x93,0x91,0x66,0x95,0x65,0xaa,
			0xdc,0x64,0x98,0x96,0x6a,0x9d,0x66,0x00,0x00,0x13,0x11,0x18,0x06,0x02,0x00,0x00
		};
		memcpy(m_dataBuf+0x17, packet, sizeof(packet));
		if (m_dhcpMode == 1)
			m_dataBuf[0x77] = 0x01;
		else
			m_dataBuf[0x77] = 0x00;
	}
	else if (readPacket(0) == -1)
		fillStartPacket();
}

void CProcess::fillMd5Packet(const u_char *md5Seed)
{
	if (!m_usePackage)	/* 不使用数据包？ */
	{
		/* xrgsu的Md5包与Start包有一个字节的差异，若以其他版本为基础，可进一步区别对待 */
		fillStartPacket();
		m_dataBuf[0x3F] = 0x04;
	}
	else if (readPacket(1) == -1 || checkRuijie(md5Seed) == -1)
			fillMd5Packet(md5Seed);
	m_echoNo = 0x0000102B;	/* 初始化echoNo */
}

int CProcess::checkRuijie(const u_char *md5Seed)	/* 算法，填充锐捷客户端MD5校验部分 */
{
	FILE *fp = NULL;
	u_char *ruijie;
	char md5[33];
	u_char table[144], *md5Dig;
	unsigned i, j;
	CString strTemp;
	strTemp.Format(LoadString(IDS_RUIJIE_VERSION), m_dataBuf[0x3B], m_dataBuf[0x3C],\
		m_dataBuf[0x3D]?_T("Linux"):_T("Windows"), m_dataBuf[0x3E]);
	mainDlg->Output(strTemp, 2);
	if (m_readSize<0x810 || ntohs(*(u_int16_t *)(m_dataBuf+0x3B)) < 0x0238)		/* 2.56之前 */
	{
		mainDlg->Output(LoadString(IDS_NO_CLIENTCHECK), 2);
		return 0;
	}
	if ((fp = _tfopen(m_package, _T("rb"))) == NULL)
		goto fileError;
	ruijie = (u_char *)malloc(m_checkSize);
	if (fread(ruijie, m_readSize, 1, fp) < 1)
	{
		fclose(fp);
		free(ruijie);
		goto fileError;
	}
	fclose(fp);
	for (i=16; i<m_readSize; i+=16)	/* 还原数据 */
	{
		for (j=0; j<16; j++)
			ruijie[i+j] ^= ruijie[j];
	}
	memset(ruijie+m_readSize, 0, m_checkSize-m_readSize);
	j = (m_checkSize - 16) / 8;	/* 数据量现在清楚了，下面用Soar的算法开始计算相应MD5值 */
	for (i=0; i<8; i++)	
	{
		memcpy(ruijie + j * i, md5Seed, 16);
		md5Dig = ComputeHash(ruijie + j * i, j + 16);
		table[18*i] = md5Seed[2*i];
		memcpy(table+18*i+1, md5Dig, 16);
		table[18*i+17] = md5Seed[2*i+1];
	}
	free(ruijie);
	md5Dig = ComputeHash(table, 144);
	for (i=0; i<16; i++)
		sprintf(md5+2*i,"%02x", md5Dig[i]);
	strTemp = md5;
	mainDlg->Output(LoadString(IDS_MD5_STRING)+strTemp, 2);
	setVendor(0x17, (u_char *)md5, 32);
	return 0;

fileError:
	m_usePackage = FALSE;
	mainDlg->Output(LoadString(IDS_PACKAGE_ERROR), 2);
	return -1;
}

void CProcess::fillEchoPacket()
{
	int i;
	u_char bt1[4], bt2[4];
	*(u_int32_t *)bt1 = htonl(m_echoKey + m_echoNo);
	*(u_int32_t *)bt2 = htonl(m_echoNo);
	m_echoNo++;
	for (i=0; i<4; i++)
	{
		m_sendPacket[0x18+i] = encode(bt1[i]);
		m_sendPacket[0x22+i] = encode(bt2[i]);
	}
}

void CProcess::getEchoKey(const u_char *buf)
{
	int i, offset = 0x1c + buf[0x1b] + 0x69 + 24;
	m_echoKey = ntohl(*(u_int32_t *)(buf + offset));
	u_char *base = (u_char *)(&m_echoKey);
	for (i=0; i<4; i++)
		base[i] = encode(base[i]);
}

u_char *CProcess::checkPass(u_char id, const u_char *md5Seed)
{
	u_char md5Src[80];
	int passLen = strlen(m_password);
	md5Src[0] = id;
	memcpy(md5Src+1, m_password, passLen);
	memcpy(md5Src+1+passLen, md5Seed, 16);
	return ComputeHash(md5Src, passLen+17);
}

int CProcess::switchState(int type)
{
	if (m_state == type) /* 跟上次是同一状态？ */
		m_sendCount++;
	else
	{
		m_state = type;
		m_sendCount = 0;
	}
	if (m_autoReconnect!=0 && m_state!=IDT_ECHO)
		mainDlg->KillTimer(TEST_TIMER);
	if (m_sendCount>=MAX_SEND_COUNT && type!=IDT_ECHO)  /* 超时太多次？ */
	{
		switch (type)
		{
		case IDT_START:
			if (m_dhcpMode == 4)
			{
				mainDlg->Output(LoadString(IDS_STATE_SUCCESS));
				return switchState(IDT_ECHO);
			}
			mainDlg->Output(LoadString(IDS_START_ERROR));
			break;
		case IDT_IDENTITY:
			mainDlg->Output(LoadString(IDS_USER_ERROR));
			break;
		case IDT_CHALLENGE:
			mainDlg->Output(LoadString(IDS_PASS_ERROR));
			break;
		case IDT_WAITECHO:
			mainDlg->Output(LoadString(IDS_WAIT_ERROR));
			return switchState(IDT_ECHO);
		}
		restart();
		return 0;
	}
	switch (type)
	{
	case IDT_DHCP:
		return getNewIP();
	case IDT_START:
		return sendStartPacket();
	case IDT_IDENTITY:
		return sendIdentityPacket();
	case IDT_CHALLENGE:
		return sendChallengePacket();
	case IDT_WAITECHO:
		return waitEchoPacket();
	case IDT_ECHO:
		if (m_bandMac != 0)
			sendArpPacket();
		return sendEchoPacket();
	case IDT_DISCONNECT:
		return sendLogoffPacket();
	}
	return 0;
}

void CProcess::fillEtherAddr(u_int32_t protocol)
{
	memset(m_sendPacket, 0, 0x3E8);
	memcpy(m_sendPacket, m_servMAC, 6);
	memcpy(m_sendPacket+0x06, m_localMAC, 6);
	*(u_int32_t *)(m_sendPacket+0x0C) = htonl(protocol);
}

int CProcess::sendTestPacket()
{
	u_char testPacket[0x3E8] = {0};
	if (m_startMode % 2 == 0)
		memcpy(testPacket, STANDARD_ADDR, 6);
	else
		memcpy(testPacket, RUIJIE_ADDR, 6);
	memcpy(testPacket+0x06, m_localMAC, 6);
	*(u_int32_t *)(testPacket+0x0C) = htonl(0x888E0101);
	memcpy(testPacket+0x12, m_dataBuf, m_dataSize);
	return pcap_sendpacket(m_hPcap, testPacket, 0x3E8);
}

int CProcess::sendStartPacket()
{
	if (m_bCernet)
	{
		if (m_sendCount == 0)
		{
			mainDlg->Output(LoadString(IDS_STATE_START));
			memcpy(m_sendPacket, STANDARD_ADDR, 6);
			memcpy(m_sendPacket+0x06, m_localMAC, 6);
			*(u_int16_t *)(m_sendPacket+0x0C) = htons(0x888E);
			*(u_int16_t *)(m_sendPacket+0x0E) = htons(0x0101);
			*(u_int16_t *)(m_sendPacket+0x10) = 0;
			memset(m_sendPacket+0x12, 0xa5, 42);
			mainDlg->SetTimer(SEND_TIMER, m_timeout, NULL);
		}
		return pcap_sendpacket(m_hPcap, m_sendPacket, 60);
	}
	if (m_sendCount == 0)
	{
		if (m_startMode == 0)
			memcpy(m_servMAC, STANDARD_ADDR, 6);
		else if (m_startMode == 1)
			memcpy(m_servMAC, RUIJIE_ADDR, 6);
		mainDlg->Output(LoadString(IDS_STATE_START));
		fillStartPacket();
		fillEtherAddr(0x888E0101);
		memcpy(m_sendPacket+0x12, m_dataBuf, m_dataSize);
		mainDlg->SetTimer(SEND_TIMER, m_timeout, NULL);
	}
	return pcap_sendpacket(m_hPcap, m_sendPacket, 0x3E8);
}

int CProcess::sendIdentityPacket()
{
	if (m_bCernet)
	{
		int nameLen = strlen(m_userName);
		if (m_sendCount == 0)
		{
			mainDlg->Output(LoadString(IDS_STATE_USERNAME));
			*(u_int16_t *)(m_sendPacket+0x0E) = htons(0x0100);
			*(u_int16_t *)(m_sendPacket+0x10) = *(u_int16_t *)(m_sendPacket+0x14) = htons(nameLen+30);
			m_sendPacket[0x12] = 0x02;
			m_sendPacket[0x16] = 0x01;
			m_sendPacket[0x17] = 0x01;
			memcpy(m_sendPacket+0x18, &m_ip, 4);
			memcpy(m_sendPacket+0x1C, &m_mask, 4);
			memcpy(m_sendPacket+0x20, &m_gateway, 4);
			/* memcpy(m_sendPacket+0x24, &m_dns, 4); */
			memset(m_sendPacket+0x24, 0xff, 4);
			memcpy(m_sendPacket+0x28, "03.02.05", 8);
			memcpy(m_sendPacket+0x30, m_userName, nameLen);
			mainDlg->SetTimer(SEND_TIMER, m_timeout, NULL);
		}
		m_sendPacket[0x13] = m_pcapBuf[0x13];
		return pcap_sendpacket(m_hPcap, m_sendPacket, nameLen+48);
	}
	if (m_sendCount == 0)
	{
		mainDlg->Output(LoadString(IDS_STATE_USERNAME));
		fillEtherAddr(0x888E0100);
		int nameLen = strlen(m_userName);
		*(u_int16_t *)(m_sendPacket+0x14) = *(u_int16_t *)(m_sendPacket+0x10) = htons(nameLen+5);
		m_sendPacket[0x12] = 2;
		m_sendPacket[0x13] = m_pcapBuf[0x13];
		m_sendPacket[0x16] = 1;
		memcpy(m_sendPacket+0x17, m_userName, nameLen);
		memcpy(m_sendPacket+0x17+nameLen, m_dataBuf, m_dataSize);
		mainDlg->SetTimer(SEND_TIMER, m_timeout, NULL);
	}
	return pcap_sendpacket(m_hPcap, m_sendPacket, 0x3E8);
}

int CProcess::sendChallengePacket()
{
	if (m_bCernet)
	{
		int nameLen = strlen(m_userName);
		if (m_sendCount == 0)
		{
			int md5Len = 0;
			u_char md5Src[50];
			*(u_int16_t *)(m_sendPacket+0x0E) = htons(0x0100);
			*(u_int16_t *)(m_sendPacket+0x10) = *(u_int16_t *)(m_sendPacket+0x14) = htons(nameLen+22);
			m_sendPacket[0x12] = 0x02;
			m_sendPacket[0x13] = m_pcapBuf[0x13];
			m_sendPacket[0x16] = 0x04;
			m_sendPacket[0x17] = 16;
			md5Src[md5Len++] = m_pcapBuf[0x13];
			memcpy(md5Src+md5Len, m_password, strlen(m_password));
			md5Len += strlen(m_password);
			memcpy(md5Src+md5Len, "xxghlmxhzb", 10);
			md5Len += 10;
			memcpy(md5Src+md5Len, m_pcapBuf+0x18, m_pcapBuf[0x17]);
			md5Len += m_pcapBuf[0x17];
			memcpy(m_sendPacket+0x18, ComputeHash(md5Src, md5Len), 16);
			memcpy(m_sendPacket+0x28, m_userName, nameLen);
			mainDlg->SetTimer(SEND_TIMER, m_timeout, NULL);
		}
		return pcap_sendpacket(m_hPcap, m_sendPacket, nameLen+40);
	}
	if (m_sendCount == 0)
	{
		mainDlg->Output(LoadString(IDS_STATE_PASSWORD));
		fillMd5Packet(m_pcapBuf + 0x18);
		fillEtherAddr(0x888E0100);
		int nameLen = strlen(m_userName);
		*(u_int16_t *)(m_sendPacket+0x14) = *(u_int16_t *)(m_sendPacket+0x10) = htons(nameLen+22);
		m_sendPacket[0x12] = 2;
		m_sendPacket[0x13] = m_pcapBuf[0x13];
		m_sendPacket[0x16] = 4;
		m_sendPacket[0x17] = 16;
		memcpy(m_sendPacket+0x18, checkPass(m_pcapBuf[0x13],m_pcapBuf+0x18), 16);
		memcpy(m_sendPacket+0x28, m_userName, nameLen);
		memcpy(m_sendPacket+0x28+nameLen, m_dataBuf, m_dataSize);
		mainDlg->SetTimer(SEND_TIMER, m_timeout, NULL);
	}
	return pcap_sendpacket(m_hPcap, m_sendPacket, 0x3E8);
}

int CProcess::sendEchoPacket()
{
	if (m_bCernet)
	{
		*(u_int16_t *)(m_sendPacket+0x0E) = htons(0x0106);
		*(u_int16_t *)(m_sendPacket+0x10) = 0;
		memset(m_sendPacket+0x12, 0xa5, 42);
		switchState(IDT_WAITECHO);	/* 继续等待 */
		return pcap_sendpacket(m_hPcap, m_sendPacket, 60);
	}
	if (m_sendCount == 0)
	{
		u_char echo[] =
		{
			0x00,0x1E,0xFF,0xFF,0x37,0x77,0x7F,0x9F,0xFF,0xFF,0xD9,0x13,0xFF,0xFF,0x37,0x77,
			0x7F,0x9F,0xFF,0xFF,0xF7,0x2B,0xFF,0xFF,0x37,0x77,0x7F,0x3F,0xFF
		};
		fillStartPacket();
		fillEtherAddr(0x888E01BF);
		memcpy(m_sendPacket+0x10, echo, sizeof(echo));
		mainDlg->SetTimer(SEND_TIMER, m_echoTime, NULL);
		if (m_autoReconnect != 0)
			mainDlg->SetTimer(TEST_TIMER, m_autoReconnect, NULL);
	}
	fillEchoPacket();
	return pcap_sendpacket(m_hPcap, m_sendPacket, 0x2D);
}

int CProcess::sendLogoffPacket()
{
	if (m_bCernet)
	{
		mainDlg->KillTimer(SEND_TIMER);
		*(u_int16_t *)(m_sendPacket+0x0E) = htons(0x0102);
		*(u_int16_t *)(m_sendPacket+0x10) = 0;
		memset(m_sendPacket+0x12, 0xa5, 42);
		return pcap_sendpacket(m_hPcap, m_sendPacket, 60);
	}
	mainDlg->KillTimer(SEND_TIMER);
	fillEtherAddr(0x888E0102);
	memcpy(m_sendPacket+0x12, m_dataBuf, m_dataSize);
	return pcap_sendpacket(m_hPcap, m_sendPacket, 0x3E8);
}

void CProcess::sendArpPacket()
{
	u_char arpPacket[0x3C] = {	//Arp包
		0xff,0xff,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x06,0x00,0x01,
		0x08,0x00,0x06,0x04,0x00};
		
		if (m_dhcpMode == 1 || m_dhcpMode == 2)
			return;
		if (m_bandMac == 2)
		{
			memcpy(arpPacket, m_gateMAC, 6 );
			memcpy(arpPacket+0x06, m_localMAC, 6 );
			arpPacket[0x15]=0x02;
			memcpy(arpPacket+0x16, m_localMAC, 6 );
			memcpy(arpPacket+0x1c, &m_rip, 4 );
			memcpy(arpPacket+0x20, m_gateMAC, 6 );
			memcpy(arpPacket+0x26, &m_gateway, 4 );
			pcap_sendpacket(m_hPcap, arpPacket, 0x3C );
		}
		memset(arpPacket, 0xFF, 6);
		memcpy(arpPacket+0x06, m_localMAC, 6 );
		arpPacket[0x15]=0x01;
		memcpy(arpPacket+0x16, m_localMAC, 6 );
		memcpy(arpPacket+0x1c, &m_rip, 4 );
		memset(arpPacket+0x20, 0, 6 );
		memcpy(arpPacket+0x26, &m_gateway, 4 );
		pcap_sendpacket(m_hPcap, arpPacket, 0x2A );
}

int CProcess::getNewIP()
{
	mainDlg->KillTimer(SEND_TIMER);
	TCHAR sysPath[MAX_PATH];
	GetSystemDirectory(sysPath, MAX_PATH);
	SHELLEXECUTEINFO ShExecInfo;
	ShExecInfo.cbSize	= sizeof(SHELLEXECUTEINFO);
	ShExecInfo.fMask	= SEE_MASK_NOCLOSEPROCESS;
	ShExecInfo.hwnd		= NULL;
	ShExecInfo.lpVerb	= NULL;
	ShExecInfo.lpFile	= _T("ipconfig.exe");
	ShExecInfo.lpDirectory	= sysPath;
	ShExecInfo.nShow	= SW_HIDE;
	ShExecInfo.hInstApp = NULL;
	ShExecInfo.lpParameters = _T("/renew");
	mainDlg->Output(LoadString(IDS_DHCP_START), 2);
	ShellExecuteEx(&ShExecInfo);
	WaitForSingleObject(ShExecInfo.hProcess, 10000);
	mainDlg->Output(LoadString(IDS_DHCP_END), 2);
	m_dhcpMode += 3; /* 标记为已获取，123变为456，5不需再认证*/
	if (!getAddress())
		endCert();
	fillHeader();
	if (m_dhcpMode == 5)
		return switchState(IDT_ECHO);
	return switchState(IDT_START);
}

int CProcess::waitEchoPacket()
{
	if (m_sendCount == 0)
		mainDlg->SetTimer(SEND_TIMER, m_echoTime, NULL);
	return 0;
}

static void showMsg(const u_char *buf)
{
	int length = ntohs(*(u_int16_t *)(buf+0x14)) - 5;
	if (length <= 0)
		return;
	char msg[513];
	memcpy(msg, buf+0x17, length);
	msg[length] = '\0';
	mainDlg->m_sServerMsg = msg;
	if (!mainDlg->m_sServerMsg.IsEmpty())
		mainDlg->Output(LoadString(IDS_SERVMSG1) + mainDlg->m_sServerMsg);
}
