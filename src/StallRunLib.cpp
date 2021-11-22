/*
 Name:		StallRunLib.cpp
 Created:	11/21/2021 7:07:02 PM
 Author:	civel
 Editor:	http://www.visualmicro.com
*/

#include "StallRunLib.h"
#include <algorithm>

size_t StallPosArr::size()
{
	return m_count;
}

StallPosArrNode* StallPosArr::begin()
{
	return m_head;
}

StallPosArrNode* StallPosArr::end()
{
	return m_tail;
}

void StallPosArr::push(const StallPosID& pos)
{
	if (m_count <= 0)
	{
		m_tail = m_head = new StallPosArrNode(pos);
	}
	else
	{
		m_tail->Next = new StallPosArrNode(pos);
		m_tail = m_tail->Next;
	}
	m_count++;
}

const StallPosID& StallPosArr::pop()
{
	const StallPosID& pos = m_tail->Pos;
	StallPosArrNode* node = m_head;
	while (node->Next != m_tail)
	{
		node = node->Next;
	}
	delete m_tail;
	m_tail = node;
	node->Next = nullptr;
	m_count--;
}

StallPosID StallInfoClass::GetNextStallPosID()
{
	return m_funcID++;
}

bool StallInfoClass::IsInitialized()
{
	return m_isInitialized;
}

void StallInfoClass::InitDone()
{
	m_isInitialized = true;
}

void StallInfoClass::Run(const StallPosID& id)
{
	for (const auto& item : m_arr)
	{
		if (item.Pos == id) return;
	}
	m_arr.push(id);
	InStall();
	m_arr.pop();
}

StallInfoClass StallInfo{};

StallPosArrNode::StallPosArrNode(StallPosID pos)
{
	Pos = pos;
}
