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

StallPosID StallPosArr::pop()
{
	if (!m_head)
	{
		if (StallInfo.IsInitialized())
		{
			Serial.println("Popped an empty array!");
			return 0;
		}
	}
	StallPosID pos = m_tail->Pos;
	if (m_count == 1)
	{
		delete m_head;
		m_head = m_tail = nullptr;
		m_count--;
		return pos;
	}
	StallPosArrNode* node = m_head;
	while (node->Next->Next != nullptr)
	{
		node = node->Next;
	}
	delete node->Next;
	node->Next = nullptr;
	m_tail = node;
	m_count--;
	return pos;
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
		if (item.Pos == id)
		{
			Serial.print("Already stalling: ");
			Serial.println(id);
			return;
		}
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

void _stallDelay(uint32_t ms, const StallPosID& id)
{
	uint32_t end = millis() + ms;
	while (millis() < end)
	{
		StallInfo.Run(id);
		delay(1);
	}
}
