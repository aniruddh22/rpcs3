#pragma once
#include "SPUThread.h"
#include "Emu/Event.h"

__forceinline static u32 GetRawSPURegAddrByNum(int num, int offset)
{
	return RAW_SPU_OFFSET * num + RAW_SPU_BASE_ADDR + RAW_SPU_PROB_OFFSET + offset;
}

__forceinline static u32 GetRawSPURegAddrById(int id, int offset)
{
	return GetRawSPURegAddrByNum(Emu.GetCPU().GetThreadNumById(CPU_THREAD_RAW_SPU, id), offset);
}

class RawSPUThread
	: public SPUThread
	, public MemoryBlock
{
	u32 m_index;

public:
	RawSPUThread(u32 index, CPUThreadType type = CPU_THREAD_RAW_SPU);
	virtual ~RawSPUThread();

	virtual bool Read32(const u64 addr, u32* value) override;

	virtual bool Write32(const u64 addr, const u32 value) override;

public:
	virtual void InitRegs();
	u32 GetIndex() const;

private:
	virtual void Task();
};

SPUThread& GetCurrentSPUThread();