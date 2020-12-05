#pragma once
#include "pcap.h"

// NicNode

class NicNode
{
public:
	NicNode(CString m_NicName, pcap_t *m_pHandle);
	virtual ~NicNode();
	void AddNext(NicNode *NextNic);
	pcap_t *m_pHandle;
	CString m_NicName;
	NicNode *m_pNext;
};
