//********************************************************************
//	created:	4:11:2009   10:33
//	filename: 	ArchUtils.cpp
//	author:		tiamo
//	purpose:	arch utils
//********************************************************************

#include "stdafx.h"
#include "ArchDefine.h"

//
// init phase 0
//
EFI_STATUS ArchInitialize0()
{
	BlSetBootMode(BOOT_MODE_SKIP_BOARD_ID_CHECK, 0);
	return EFI_SUCCESS;
}

//
// check 64bit cpu
//
EFI_STATUS ArchCheck64BitCpu()
{
	UINT32 eaxValue															= 0;
	UINT32 ebxValue															= 0;
	UINT32 ecxValue															= 0;
	UINT32 edxValue															= 0;
	ArchCpuId(0x80000000, &eaxValue, &ebxValue, &ecxValue, &edxValue);
	if(eaxValue < 0x80000001)
		return EFI_UNSUPPORTED;

	ArchCpuId(0x80000001, &eaxValue, &ebxValue, &ecxValue, &edxValue);
	return edxValue & 0x20000000 ? EFI_SUCCESS : EFI_UNSUPPORTED;
}

//
// cpu id
//
VOID __declspec(naked) ArchCpuId(UINT32 command, UINT32* eaxValue, UINT32* ebxValue, UINT32* ecxValue, UINT32* edxValue)
{
	__asm
	{
		push		ebx
		push		edi
		mov			eax, [esp + 0x0c]
		cpuid
		mov			edi, [esp + 0x10]
		mov			[edi], eax
		mov			edi, [esp + 0x14]
		mov			[edi], ebx
		mov			edi, [esp + 0x18]
		mov			[edi], ecx
		mov			edi, [esp + 0x1c]
		mov			[edi], edx
		pop			edi
		pop			ebx
		retn
	}
}

//
// random
//
UINTN __declspec(naked) ArchHardwareRandom()
{
	__asm
	{
		mov			edx, 10

	HARDWARE_RND:
		__emit		0x0f
		__emit		0xc7
		__emit		0xf0
		jnb			RETRY

		retn

	RETRY:
		dec			edx
		pause
		jnz			HARDWARE_RND

		xor			eax, eax
		retn
	}
}

//
// get cpu tick
//
UINT64 __declspec(naked) ArchGetCpuTick()
{
	__asm
	{
		lfence
		rdtsc
		lfence
		retn
	}
}

//
// transfer to kernel
//
VOID __declspec(naked) ArchStartKernel(VOID* kernelEntry, VOID* bootArgs)
{
	__asm
	{
		cli
		mov			edx, [esp + 4]
		mov			eax, [esp + 8]
		call		edx
		retn
	}
}

//
// translate address
//
BOOLEAN MmTranslateVirtualAddress(VOID* virtualAddress, UINT64* physicalAddress)
{
	if(physicalAddress)
		*physicalAddress													= ArchConvertPointerToAddress(virtualAddress);

	return TRUE;
}

//
// get cs
//
UINT32 __declspec(naked) ArchGetSegCs()
{
	__asm
	{
		mov		ax,cs
		movzx	eax,ax
		retn
	}
}

//
// get idtr
//
VOID __declspec(naked) ArchGetIdtRegister(KDESCRIPTOR* idtr)
{
	__asm
	{
		mov			eax,[esp + 4]
		sidt		fword ptr[eax + KDESCRIPTOR.Limit]
		retn
	}
}

//
// set idtr
//
VOID __declspec(naked) ArchSetIdtRegister(KDESCRIPTOR* idtr)
{
	__asm
	{
		mov			eax,[esp + 4]
		lidt		fword ptr[eax + KDESCRIPTOR.Limit]
		retn
	}
}

//
// set idt entry
//
VOID ArchSetIdtEntry(UINT32 base, UINT32 index, UINT32 segCs, VOID* offset,UINT32 access)
{
	KIDTENTRY* idtEntry														= Add2Ptr(base, index * sizeof(KIDTENTRY), KIDTENTRY*);
	idtEntry->Selector														= static_cast<UINT16>(segCs);
	idtEntry->Access														= static_cast<UINT16>(access);
	idtEntry->Offset														= static_cast<UINT16>(ArchConvertPointerToAddress(offset) & 0xffff);
	idtEntry->ExtendedOffset												= static_cast<UINT16>(ArchConvertPointerToAddress(offset) >> 16);
}

//
// sweep instruction cache
//
VOID ArchSweepIcacheRange(VOID* startAddress, UINT32 bytesCount)
{
}

//
// read port uint8
//
UINT8 __declspec(naked) ARCH_READ_PORT_UINT8(UINT8* port)
{
	__asm
	{
		mov			dx, [esp + 4]
		in			al, dx
		retn
	}
}

//
// read port uint16
//
UINT16 __declspec(naked) ARCH_READ_PORT_UINT16(UINT16* port)
{
	__asm
	{
		mov			dx, [esp + 4]
		in			ax, dx
		retn
	}
}

//
// read port uint32
//
UINT32 __declspec(naked) ARCH_READ_PORT_UINT32(UINT32* port)
{
	__asm
	{
		mov			dx,  [esp + 4]
		in			eax, dx
		retn
	}
}

//
// write port uint8
//
VOID __declspec(naked) ARCH_WRITE_PORT_UINT8(UINT8* port, UINT8 value)
{
	__asm
	{
		mov			dx, [esp + 4]
		mov			al, [esp + 8]
		out			dx, al
		retn
	}
}

//
// write port uint16
//
VOID __declspec(naked) ARCH_WRITE_PORT_UINT16(UINT16* port, UINT16 value)
{
	__asm
	{
		mov			dx, [esp + 4]
		mov			ax, [esp + 8]
		out			dx, ax
		retn
	}
}

//
// write port uint32
//
VOID __declspec(naked) ARCH_WRITE_PORT_UINT32(UINT32* port, UINT32 value)
{
	__asm
	{
		mov			dx, [esp + 4]
		mov			eax,[esp + 8]
		out			dx, eax
		retn
	}
}

//
// read register uint8
//
UINT8 __declspec(naked) ARCH_READ_REGISTER_UINT8(UINT8* port)
{
	__asm
	{
		mov			edx, [esp + 4]
		mov			al,  [edx]
		retn
	}
}

//
// read register uint16
//
UINT16 __declspec(naked) ARCH_READ_REGISTER_UINT16(UINT16* port)
{
	__asm
	{
		mov			edx, [esp + 4]
		mov			ax,  [edx]
		retn
	}
}

//
// read register uint32
//
UINT32 __declspec(naked) ARCH_READ_REGISTER_UINT32(UINT32* port)
{
	__asm
	{
		mov			edx, [esp + 4]
		mov			eax, [edx]
		retn
	}
}

//
// write port uint8
//
VOID __declspec(naked) ARCH_WRITE_REGISTER_UINT8(UINT8* port, UINT8 value)
{
	__asm
	{
		mov			edx, [esp + 4]
		mov			al,  [esp + 8]
		mov			[edx], al
		lock or		[esp + 4], edx
		retn
	}
}

//
// write port uint16
//
VOID __declspec(naked) ARCH_WRITE_REGISTER_UINT16(UINT16* port, UINT16 value)
{
	__asm
	{
		mov			edx, [esp + 4]
		mov			ax,  [esp + 8]
		mov			[edx], ax
		lock or		[esp + 4], edx
		retn
	}
}

//
// write port uint32
//
VOID __declspec(naked) ARCH_WRITE_REGISTER_UINT32(UINT32* port, UINT32 value)
{
	__asm
	{
		mov			edx, [esp + 4]
		mov			eax, [esp + 8]
		mov			[edx], eax
		lock or		[esp + 4], edx
		retn
	}
}