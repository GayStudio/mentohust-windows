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
#define IDT_DISCONNECT			0	/* 断开状态 */
#define IDT_START				1	/* 寻找服务器 */
#define IDT_IDENTITY			2	/* 发送用户名 */
#define IDT_CHALLENGE			3	/* 发送密码 */
#define IDT_ECHO				4	/* 发送定时响应 */
#define IDT_DHCP				5	/* 更新IP */
#define IDT_WAITECHO			6	/* 等待Echo（赛尔） */

class CProcess
{
public:
	CProcess();
	virtual ~CProcess();
	void startCert();	/* 开始认证 */
	void endCert();		/* 停止认证 */
	int switchState(int type);	/* 改变状态 */
	int sendTestPacket();
	void getEchoKey(const u_char *buf);
	void getEchoKeyFormEcho(const u_char *buf);
	void restart();
	
	volatile int m_state;		/* 认证状态 */
	char m_userName[66], m_password[64];	/* 用户名、密码 */
	u_char m_servMAC[6], m_gateMAC[6];
	u_char m_localMAC[6];
	u_int32_t m_ip, m_rip, m_gateway;		/* IP */
	char m_nic[60];		/* 网卡名 */
	unsigned m_timeout, m_echoTime, m_autoReconnect, m_restart;		//重连间隔
	unsigned m_startMode, m_dhcpMode;		/* 组播方式、DHCP方式 */
	BOOL m_bandMac;		/* 绑定网关MAC？ */
	BOOL m_usePackage;	/* 使用数据包 */
	BOOL m_autoMin;	/* 使用数据包 */
	TCHAR m_package[MAX_PATH];	/* 数据包 */
	pcap_t *m_hPcap;	/* pcap句柄 */
	const u_char *m_pcapBuf;
	BOOL m_bCernet;
	u_int32_t m_echoKey, m_echoNo;

private:
	static const u_char STANDARD_ADDR[];
	static const u_char RUIJIE_ADDR[];

	u_int32_t m_mask, m_dns;		/* 各种网络地址 */
	u_char m_sendPacket[1000];
	u_char *m_dataBuf;
	unsigned m_dataSize;
	unsigned m_readSize, m_checkSize;
	
	unsigned m_sendCount;
	CWinThread *m_pCertThread;	//认证线程
	
	BOOL getAddress();	/* 获取网络地址 */
	BOOL openAdapter();	/* 打开网卡 */
	void getSize();		/* 获取大小 */
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
