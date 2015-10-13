//********************************************************************
//	created:	5:11:2009   13:45
//	filename: 	ArchUtils.cpp
//	author:		tiamo
//	purpose:	arch utils
//********************************************************************

#include "stdafx.h"
#include "ArchDefine.h"

//
// global
//
typedef VOID (BOOTAPI *ArchTransferRoutine)(VOID* kernelEntry, VOID* bootArgs);
ArchTransferRoutine	ArchpTransferRoutine									= nullptr;

//
// init phase 0
//
EFI_STATUS ArchInitialize0()
{
	return EFI_SUCCESS;
}

//
// init phase 1
//
EFI_STATUS ArchInitialize1()
{
	extern VOID ArchTransferRoutineBegin();
	extern VOID ArchTransferRoutineEnd();
	UINTN bytesCount														= ArchConvertPointerToAddress(&ArchTransferRoutineEnd) - ArchConvertPointerToAddress(&ArchTransferRoutineBegin);
	UINT64 physicalAddress													= 4 * 1024 * 1024 * 1024ULL - 1;
	VOID* transferRoutineBuffer												= MmAllocatePages(AllocateMaxAddress, EfiLoaderCode, EFI_SIZE_TO_PAGES(bytesCount), &physicalAddress);
	if(!transferRoutineBuffer)
		return EFI_OUT_OF_RESOURCES;

	memcpy(transferRoutineBuffer, &ArchTransferRoutineBegin, bytesCount);
	ArchpTransferRoutine													= reinterpret_cast<ArchTransferRoutine>(transferRoutineBuffer);
	return EFI_SUCCESS;
}

//
// check 64bit cpu
//
EFI_STATUS ArchCheck64BitCpu()
{
	return EFI_SUCCESS;
}

//
// set idt entry
//
VOID ArchSetIdtEntry(UINT64 base, UINT32 index, UINT32 segCs, VOID* offset,UINT32 access)
{
	KIDTENTRY* idtEntry														= Add2Ptr(base, index * sizeof(KIDTENTRY), KIDTENTRY*);
	idtEntry->Selector														= static_cast<UINT16>(segCs);
	idtEntry->Access														= static_cast<UINT16>(access);
	idtEntry->Offset														= static_cast<UINT16>(ArchConvertPointerToAddress(offset) & 0xffff);
	idtEntry->ExtendedOffset												= static_cast<UINT16>(ArchConvertPointerToAddress(offset) >> 16);
	idtEntry->HighOffset													= static_cast<UINT32>(ArchConvertPointerToAddress(offset) >> 32);
	idtEntry->Reserved														= 0;
}

//
// sweep instruction cache
//
VOID ArchSweepIcacheRange(VOID* startAddress, UINT32 bytesCount)
{
}

//
// transfer to kernel
//
VOID ArchStartKernel(VOID* kernelEntry, VOID* bootArgs)
{
	ArchpTransferRoutine(kernelEntry, bootArgs);
}

//
// setup thunk code
//
VOID ArchSetupThunkCode0(UINT64 thunkOffset, struct _MACH_O_LOADED_INFO* loadedInfo)
{

}

//
// setup thunk code
//
VOID ArchSetupThunkCode1(UINT64* efiSystemTablePhysicalAddress, UINT64 thunkOffset)
{
}