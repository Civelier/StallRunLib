/*
 Name:		StallRunLib.h
 Created:	11/21/2021 7:07:02 PM
 Author:	civel
 Editor:	http://www.visualmicro.com
*/

#ifndef _StallRunLib_h
#define _StallRunLib_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

using StallPosID = uint32_t;

#define StallRun()\
{\
	static StallPosID _stallpos = StallInfo.GetNextStallPosID();\
	StallInfo.Run(_stallpos);\
}

struct StallPosArrNode
{
	StallPosID Pos;
	StallPosArrNode* Next{ nullptr };
	StallPosArrNode(StallPosID pos);
};

class StallPosArr
{
private:
	StallPosArrNode* m_head;
	StallPosArrNode* m_tail;
	size_t m_count;
public:
	size_t size();
	StallPosArrNode* begin();
	StallPosArrNode* end();
	void push(const StallPosID& pos);
	StallPosID pop();
};

void InStall();

class StallInfoClass
{
private:
	bool m_isInitialized{ false };
	StallPosID m_funcID{ 0 };
	StallPosArr m_arr{};
public:
	StallPosID GetNextStallPosID();
	bool IsInitialized();
	void InitDone();
	void Run(const StallPosID& id);
};

extern StallInfoClass StallInfo;

void _stallDelay(uint32_t ms, const StallPosID& id);

#define stallDelay(ms) {static StallPosID _stallpos = StallInfo.GetNextStallPosID(); _stallDelay(ms, _stallpos);}

#endif

