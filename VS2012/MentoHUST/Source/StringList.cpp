// StringList.cpp : 实现文件
//Written By HustMoon

#include "stdafx.h"
#include "StringList.h"


// StringList

StringList::StringList(LPCTSTR String)
{
	m_sString = String;
	m_pNext = NULL;
}

StringList::~StringList()
{
	if (m_pNext != NULL)
		delete m_pNext;
}

void StringList::AddString(StringList *Next)
{
	if (m_pNext == NULL)
		m_pNext = Next;
	else
		m_pNext->AddString(Next);
}

CString StringList::GetString(int iIndex)
{
	if (iIndex < 0)
		return _T("");
	for (StringList *p=this; p!=NULL; p=p->m_pNext, iIndex--)
	{
		if (iIndex == 0)
			return p->m_sString;
	}
	return _T("");
}

int StringList::FindString(LPCTSTR String, BOOL bNoCase)
{
	StringList *p = this;
	int RetVal = 0;
	if (bNoCase)
	{
		for (; p!=NULL; p=p->m_pNext, RetVal++)
			if (p->m_sString.CompareNoCase(String) == 0)
				return RetVal;
	}
	else
	{
		for (; p!=NULL; p=p->m_pNext, RetVal++)
			if (p->m_sString.Compare(String) == 0)
				return RetVal;
	}
	return -1;
}

StringList *StringList::GetNext()
{
	return m_pNext;
}
