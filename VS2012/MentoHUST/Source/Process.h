#if !defined(AFX_PROCESS_H__60218E04_5D72_48A8_9425_A25ACB0E4F2B__INCLUDED_)
#define AFX_PROCESS_H__60218E04_5D72_48A8_9425_A25ACB0E4F2B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
unsigned long _beginthreadex( void *security, unsigned stack_size, unsigned ( __stdcall *start_address )( void * ), void *arglist, unsigned initflag, unsigned *thrdaddr );
void _endthreadex(unsigned status);
#include "pcap.h"
// Process.h : header file
//
#define IDT_DISCONNECT			0	/* �Ͽ�״̬ */
#define IDT_START				1	/* Ѱ�ҷ����� */
#define IDT_IDENTITY			2	/* �����û��� */
#define IDT_CHALLENGE			3	/* �������� */
#define IDT_ECHO				4	/* ���Ͷ�ʱ��Ӧ */
#define IDT_DHCP				5	/* ����IP */
#define IDT_WAITECHO			6	/* �ȴ�Echo�������� */

class CProcess
{
public:
	CProcess();
	virtual ~CProcess();
	void startCert();	/* ��ʼ��֤ */
	void endCert();		/* ֹͣ��֤ */
	int switchState(int type);	/* �ı�״̬ */
	int sendTestPacket();
	void getEchoKey(const u_char *buf);
	void getEchoKeyFormEcho(const u_char *buf);
	void restart();
	
	volatile int m_state;		/* ��֤״̬ */
	char m_userName[66], m_password[64];	/* �û��������� */
	u_char m_servMAC[6], m_gateMAC[6];
	u_char m_localMAC[6];
	u_int32_t m_ip, m_rip, m_gateway;		/* IP */
	char m_nic[60];		/* ������ */
	unsigned m_timeout, m_echoTime, m_autoReconnect, m_restart;		//�������
	unsigned m_startMode, m_dhcpMode;		/* �鲥��ʽ��DHCP��ʽ */
	BOOL m_bandMac;		/* ������MAC�� */
	BOOL m_usePackage;	/* ʹ�����ݰ� */
	BOOL m_autoMin;	/* ʹ�����ݰ� */
	TCHAR m_package[MAX_PATH];	/* ���ݰ� */
	pcap_t *m_hPcap;	/* pcap��� */
	const u_char *m_pcapBuf;
	BOOL m_bCernet;
	u_int32_t m_echoKey, m_echoNo;

private:
	static const u_char STANDARD_ADDR[];
	static const u_char RUIJIE_ADDR[];

	u_int32_t m_mask, m_dns;		/* ���������ַ */
	u_char m_sendPacket[1000];
	u_char *m_dataBuf;
	unsigned m_dataSize;
	unsigned m_readSize, m_checkSize;
	
	unsigned m_sendCount;
	CWinThread *m_pCertThread;	//��֤�߳�
	
	BOOL getAddress();	/* ��ȡ�����ַ */
	BOOL openAdapter();	/* ������ */
	void getSize();		/* ��ȡ��С */
	u_char encode(u_char base);
	void checkSum();
	void fillHeader();
	int setVendor(u_char type, const u_char *value, int length);
	int readPacket(int type);
	void fillStartPacket();
	void fillMd5Packet(const u_char *md5Seed);
	int checkRuijie(const u_char *md5Seed);
	int checkRuijie2(const u_char *md5Seed);
	void fillEchoPacket();
	u_char *checkPass(u_char id, const u_char *md5Seed);
	void fillEtherAddr(u_int32_t protocol);
	int sendStartPacket();
	int sendIdentityPacket();
	int sendChallengePacket();
	int sendEchoPacket();
	int sendLogoffPacket();
	void sendArpPacket();
	int getNewIP();
	int waitEchoPacket();
};

#endif // !defined(AFX_PROCESS_H__60218E04_5D72_48A8_9425_A25ACB0E4F2B__INCLUDED_)
