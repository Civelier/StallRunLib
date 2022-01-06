/*
 Name:		StallRunLib.h
 Created:	11/21/2021 7:07:02 PM
 Author:	civel
 Editor:	http://www.visualmicro.com
*/

#ifndef _StallRunLib_h
#define _StallRunLib_h

#include <inttypes.h>
//#if defined(ARDUINO) && ARDUINO >= 100
//	#include "arduino.h"
//#else
//	#include "WProgram.h"
//#endif

using StallPosID = uint32_t;


#ifdef STALL_RUN
#pragma message "Stall run has been enabled"

// Call this when stalling to update key systems
#define StallRun()\
{\
	static StallPosID _stallpos = StallInfo.GetNextStallPosID();\
	StallInfo.Run(_stallpos);\
}

#define StallRunLowPriority(ms)\
{\
	static StallPosID _stallpos = StallInfo.GetNextStallPosID();\
	StallInfo.LowPriorityRun(_stallpos, ms);\
}
#elif defined(NO_STALL_RUN)
#pragma message "Stall run has been disabled"

// Call this when stalling to update key systems
#define StallRun()

#define StallRunLowPriority(ms)

#else
#pragma message "Stall run has been disabled, to enable add a config define called 'STALL_RUN' using visual micro"

#define StallRun()

#define StallRunLowPriority(ms)
#endif



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
	uint32_t m_count;
public:
	uint32_t size();
	StallPosArrNode* begin();
	StallPosArrNode* end();
	void push(const StallPosID& pos);
	StallPosID pop();
};

// Implement this function to run code during a stall time. (protected from recusivity)
void InStall();
// Implement this function to run code without recursive prevention.
void ForcedRunStall();


class StallInfoClass
{
private:
	bool m_isInitialized{ false };
	StallPosID m_funcID{ 0 };
	StallPosArr m_arr{};
	bool m_lowPriority{ false };
	uint32_t m_lastRun{ 0 };
public:
	StallPosID GetNextStallPosID();
	bool IsInitialized();
	void InitDone();
	bool IsLowPriority();
	void Run(const StallPosID& id);
	void LowPriorityRun(const StallPosID& id, uint32_t refreshMS);
};

extern StallInfoClass StallInfo;

void _stallDelay(uint32_t ms, const StallPosID& id);

// Safe delay for stalling time
#define stallDelay(ms) {static StallPosID _stallpos = StallInfo.GetNextStallPosID(); _stallDelay(ms, _stallpos);}

#endif

