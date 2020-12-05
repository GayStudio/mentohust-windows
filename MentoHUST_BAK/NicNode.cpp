// NicNode.cpp : 实现文件
//Written By HustMoon

#include "stdafx.h"
#include "NicNode.h"


// NicNode

NicNode::NicNode(CString m_NicName, pcap_t *m_pHandle)
{
	this->m_pHandle = m_pHandle;
	this->m_NicName = m_NicName;
	this->m_pNext = NULL;
}

NicNode::~NicNode()
{
	if(this->m_pNext)
	{
		pcap_close(this->m_pHandle);
		delete this->m_pNext;
	}
}

void NicNode::AddNext(NicNode *NextNic)
{
	if(m_pNext == NULL)
		m_pNext = NextNic;
	else
		m_pNext->AddNext(NextNic);
}