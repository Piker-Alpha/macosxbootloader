//********************************************************************
//	created:	4:11:2009   10:34
//	filename: 	ArchUtils.h
//	author:		tiamo
//	purpose:	arch utils
//********************************************************************

#pragma once

//
// initialize phase 0
//
EFI_STATUS ArchInitialize0();

//
// initialize phase 1
//
EFI_STATUS ArchInitialize1();

//
// check 64bit cpu
//
EFI_STATUS ArchCheck64BitCpu();

//
// sweep instruction cache
//
VOID BOOTAPI ArchSweepIcacheRange(VOID* startAddress, UINT32 bytesCount);

//
// transfer to kernel
//
VOID BOOTAPI ArchStartKernel(VOID* kernelEntry, VOID* bootArgs);

//
// setup thunk code
//
VOID BOOTAPI ArchSetupThunkCode0(UINT64 thunkOffset, struct _MACH_O_LOADED_INFO* loadedInfo);

//
// setup thunk code
//
VOID BOOTAPI ArchSetupThunkCode1(UINT64* efiSystemTablePhysicalAddress, UINT64 thunkOffset);

//
// cpu id
//
VOID BOOTAPI ArchCpuId(UINT32 command, UINT32* eaxValue, UINT32* ebxValue, UINT32* ecxValue, UINT32* edxValue);

//
// get cpu tick
//
UINT64 BOOTAPI ArchGetCpuTick();

//
// random
//
UINTN BOOTAPI ArchHardwareRandom();

//
// read port uint8
//
UINT8 BOOTAPI ARCH_READ_PORT_UINT8(UINT8* port);

//
// read port uint16
//
UINT16 BOOTAPI ARCH_READ_PORT_UINT16(UINT16* port);

//
// read port uint32
//
UINT32 BOOTAPI ARCH_READ_PORT_UINT32(UINT32* port);

//
// write port uint8
//
VOID BOOTAPI ARCH_WRITE_PORT_UINT8(UINT8* port, UINT8 value);

//
// write port uint16
//
VOID BOOTAPI ARCH_WRITE_PORT_UINT16(UINT16* port, UINT16 value);

//
// write port uint32
//
VOID BOOTAPI ARCH_WRITE_PORT_UINT32(UINT32* port, UINT32 value);

//
// read register uint8
//
UINT8 BOOTAPI ARCH_READ_REGISTER_UINT8(UINT8* port);

//
// read register uint16
//
UINT16 BOOTAPI ARCH_READ_REGISTER_UINT16(UINT16* port);

//
// read register uint32
//
UINT32 BOOTAPI ARCH_READ_REGISTER_UINT32(UINT32* port);

//
// write register uint8
//
VOID BOOTAPI ARCH_WRITE_REGISTER_UINT8(UINT8* port, UINT8 value);

//
// write register uint16
//
VOID BOOTAPI ARCH_WRITE_REGISTER_UINT16(UINT16* port, UINT16 value);

//
// write register uint32
//
VOID BOOTAPI ARCH_WRITE_REGISTER_UINT32(UINT32* port, UINT32 value);