//********************************************************************
//	created:	4:11:2009   10:33
//	filename: 	ArchUtils.cpp
//	author:		tiamo
//	purpose:	arch utils
//********************************************************************

#include "../StdAfx.h"
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
#ifdef _MSC_VER
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
#else
#ifdef __x86_64__
    __asm("push %%rbx\n\t"
          "push %%rdi\n\t"
          "mov %4, %%eax\n\t"
          "cpuid\n\t"
          "mov %%eax, %0\n\t"
          "mov %%ebx, %1\n\t"
          "mov %%ecx, %2\n\t"
          "mov %%edx, %3\n\t"
          "pop %%rdi\n\t"
          "pop %%rbx\n\t"
          : "=&m" (eaxValue), "=&m" (ebxValue), "=&m" (ecxValue), "=&m" (edxValue)
          : "m" (command));
#else
    __asm("push %%ebx\n\t"
          "push %%edi\n\t"
          "mov %4, %%eax\n\t"
          "cpuid\n\t"
          "mov %%eax, %0\n\t"
          "mov %%ebx, %1\n\t"
          "mov %%ecx, %2\n\t"
          "mov %%edx, %3\n\t"
          "pop %%edi\n\t"
          "pop %%ebx\n\t"
          : "=&m" (eaxValue), "=&m" (ebxValue), "=&m" (ecxValue), "=&m" (edxValue)
          : "m" (command));
#endif
#endif
}

//
// random
//
UINTN __declspec(naked) ArchHardwareRandom()
{
#ifdef _MSC_VER
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
#else
    UINTN ret;

    __asm("mov 10, %%edx\n\n"
          "LHARDWARE_RND:\n\t"
          ".byte 0x0F\n\t"
          ".byte 0xC7\n\t"
          ".byte 0xF0\n\t"
          "jnb LRETRY\n"
          "movl %%eax, %0\n"
          "jmp LEND\n\t"
          "LRETRY:\n\t"
          "dec %%edx\n\t"
          "pause\n\t"
          "jnz LHARDWARE_RND\n\t"
          "xor %%eax, %%eax\n\t"
          "mov %%eax, %0\n\n"
          "LEND:\n\t"
          : "=&m" (ret));

    return ret;
#endif
}

//
// get cpu tick
//
UINT64 __declspec(naked) ArchGetCpuTick()
{
#ifdef _MSC_VER
	__asm
	{
		lfence
		rdtsc
		lfence
		retn
	}
#else
    UINT64 ret;

    __asm("lfence\n\t"
          "rdtsc\n\t"
          "lfence\n"
          "movl %%eax, %0"
          : "=&m" (ret));

    return ret;
#endif
}

//
// transfer to kernel
//
VOID __declspec(naked) ArchStartKernel(VOID* kernelEntry, VOID* bootArgs)
{
#ifdef _MSC_VER
	__asm
	{
		cli
		mov			edx, [esp + 4]
		mov			eax, [esp + 8]
		call		edx
		retn
	}
#else
	__asm("cli\n"
	#ifdef __clang__
		  "movl %%edx, %0\n"
		  "movl %%eax, %1\n"
		  "call *(%%edx)\n"
	#else
		  "movl %0, %%edx\n"
		  "movl %1, %%eax\n"
		  "call %%edx\n"
	#endif
		  "ret\n"
		  :: "m" (kernelEntry), "m" (bootArgs));
#endif
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
#ifdef _MSC_VER
	__asm
	{
		mov		ax,cs
		movzx	eax,ax
		retn
	}
#else
    UINT32 ret;

    __asm("mov %%cs, %%ax\n"
          "movzx %%ax, %0"
          : "=&r" (ret));

    return ret;
#endif
}

//
// get idtr
//
VOID __declspec(naked) ArchGetIdtRegister(KDESCRIPTOR* idtr)
{
#ifdef _MSC_VER
	__asm
	{
		mov			eax,[esp + 4]
		sidt		fword ptr[eax + KDESCRIPTOR.Limit]
		retn
	}
#else
    __asm("mov %0, %%eax\n\t"
          "sidt 2(%%eax)\n\t"
          "ret\n\t"
          :: "m" (idtr));
#endif
}

//
// set idtr
//
VOID __declspec(naked) ArchSetIdtRegister(KDESCRIPTOR* idtr)
{
#ifdef _MSC_VER
	__asm
	{
		mov			eax,[esp + 4]
		lidt		fword ptr[eax + KDESCRIPTOR.Limit]
		retn
	}
#else
    __asm("mov %0, %%eax\n\t"
          "lidt 2(%%eax)\n\t"
          "ret\n\t"
          :: "m" (idtr));
#endif
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
#ifdef _MSC_VER
	__asm
	{
		mov			dx, [esp + 4]
		in			al, dx
		retn
	}
#else
    UINT8 ret;

    __asm("movw %1, %%dx\n"
          "inb %%dx, %%al\n\t"
          "movb %%al, %0"
          : "=&r" (ret)
          : "m" (*port));

    return ret;
#endif
}

//
// read port uint16
//
UINT16 __declspec(naked) ARCH_READ_PORT_UINT16(UINT16* port)
{
#ifdef _MSC_VER
	__asm
	{
		mov			dx, [esp + 4]
		in			ax, dx
		retn
	}
#else
    UINT16 ret;
    
    __asm("movw %1, %%dx\n"
          "inw %%dx, %%ax\n\t"
          "movw %%ax, %0"
          : "=&r" (ret)
          : "m" (*port));
    
    return ret;
#endif
}

//
// read port uint32
//
UINT32 __declspec(naked) ARCH_READ_PORT_UINT32(UINT32* port)
{
#ifdef _MSC_VER
	__asm
	{
		mov			dx,  [esp + 4]
		in			eax, dx
		retn
	}
#else
    UINT32 ret;
    
    __asm("movw %1, %%dx\n\t"
          "inl %%dx, %%eax\n\t"
          "movl %%eax, %0"
          : "=&r" (ret)
          : "m" (*port));

    return ret;
#endif
}

//
// write port uint8
//
VOID __declspec(naked) ARCH_WRITE_PORT_UINT8(UINT8* port, UINT8 value)
{
#ifdef _MSC_VER
	__asm
	{
		mov			dx, [esp + 4]
		mov			al, [esp + 8]
		out			dx, al
		retn
	}
#else
    __asm("movw %0, %%dx\n\t"
          "movb %1, %%al\n"
          "outb %%al, %%dx\n\t"
          "ret\n\t"
          :: "m" (*port), "m" (value));
#endif
}

//
// write port uint16
//
VOID __declspec(naked) ARCH_WRITE_PORT_UINT16(UINT16* port, UINT16 value)
{
#ifdef _MSC_VER
	__asm
	{
		mov			dx, [esp + 4]
		mov			ax, [esp + 8]
		out			dx, ax
		retn
	}
#else
    __asm("movw %0, %%dx\n\t"
          "movw %1, %%ax\n\t"
          "outw %%ax, %%dx\n\t"
          "ret\n\t"
          :: "m" (*port), "m" (value));
#endif
}

//
// write port uint32
//
VOID __declspec(naked) ARCH_WRITE_PORT_UINT32(UINT32* port, UINT32 value)
{
#ifdef _MSC_VER
	__asm
	{
		mov			dx, [esp + 4]
		mov			eax,[esp + 8]
		out			dx, eax
		retn
	}
#else
    __asm("movw %0, %%dx\n\t"
          "movl %1, %%eax\n\t"
          "outl %%eax, %%dx\n\t"
          "ret\n\t"
          :: "m" (*port), "m" (value));
#endif
}

//
// read register uint8
//
UINT8 __declspec(naked) ARCH_READ_REGISTER_UINT8(UINT8* port)
{
#ifdef _MSC_VER
	__asm
	{
		mov			edx, [esp + 4]
		mov			al,  [edx]
		retn
	}
#else
    UINT8 ret;

    __asm("mov %1, %%edx\n\t"
          "mov (%%edx), %%al\n\t"
          "mov %%al, %0\n\t"
          : "=&r" (ret)
          : "m" (*port));

    return ret;
#endif
}

//
// read register uint16
//
UINT16 __declspec(naked) ARCH_READ_REGISTER_UINT16(UINT16* port)
{
#ifdef _MSC_VER
	__asm
	{
		mov			edx, [esp + 4]
		mov			ax,  [edx]
		retn
	}
#else
    UINT16 ret;
    
    __asm("mov %1, %%edx\n\t"
          "mov (%%edx), %%ax\n\t"
          "mov %%ax, %0\n\t"
          : "=&r" (ret)
          : "m" (*port));
    
    return ret;
#endif
}

//
// read register uint32
//
UINT32 __declspec(naked) ARCH_READ_REGISTER_UINT32(UINT32* port)
{
#ifdef _MSC_VER
	__asm
	{
		mov			edx, [esp + 4]
		mov			eax, [edx]
		retn
	}
#else
    UINT32 ret;
    
    __asm("mov %1, %%edx\n\t"
          "mov (%%edx), %%eax\n\t"
          "mov %%eax, %0\n\t"
          : "=&r" (ret)
          : "m" (*port));
    
    return ret;
#endif
}

//
// write port uint8
//
VOID __declspec(naked) ARCH_WRITE_REGISTER_UINT8(UINT8* port, UINT8 value)
{
#ifdef _MSC_VER
	__asm
	{
		mov			edx, [esp + 4]
		mov			al,  [esp + 8]
		mov			[edx], al
		lock or		[esp + 4], edx
		retn
	}
#else
#ifndef GNU
    __asm("mov %0, %%edx\n\t"
          "mov %1, %%al\n\t"
          "mov %%al, (%%edx)\n\t"
          "lock or %0, %%edx\n\t"
          :: "m" (*port), "m" (value));
#else
    __asm("mov %0, %%edx\n\t"
          "mov %1, %%al\n\t"
          "mov %%al, (%%edx)\n\t"
          "or %0, %%edx\n\t"
          :: "m" (*port), "m" (value));
#endif
#endif
}

//
// write port uint16
//
VOID __declspec(naked) ARCH_WRITE_REGISTER_UINT16(UINT16* port, UINT16 value)
{
#ifdef _MSC_VER
	__asm
	{
		mov			edx, [esp + 4]
		mov			ax,  [esp + 8]
		mov			[edx], ax
		lock or		[esp + 4], edx
		retn
	}
#else
#ifndef GNU
    __asm("mov %0, %%edx\n\t"
          "mov %1, %%ax\n\t"
          "mov %%ax, (%%edx)\n\t"
          "lock or %0, %%edx\n\t"
          :: "m" (*port), "m" (value));
#else
    __asm("mov %0, %%edx\n\t"
          "mov %1, %%ax\n\t"
          "mov %%ax, (%%edx)\n\t"
          "or %0, %%edx\n\t"
          :: "m" (*port), "m" (value));
#endif
#endif
}

//
// write port uint32
//
VOID __declspec(naked) ARCH_WRITE_REGISTER_UINT32(UINT32* port, UINT32 value)
{
#ifdef _MSC_VER
	__asm
	{
		mov			edx, [esp + 4]
		mov			eax, [esp + 8]
		mov			[edx], eax
		lock or		[esp + 4], edx
		retn
	}
#else
#ifndef GNU
    __asm("mov %0, %%edx\n\t"
          "mov %1, %%eax\n\t"
          "mov %%eax, (%%edx)\n\t"
          "lock or %0, %%edx\n\t"
          :: "m" (*port), "m" (value));
#else
    __asm("mov %0, %%edx\n\t"
          "mov %1, %%eax\n\t"
          "mov %%eax, (%%edx)\n\t"
          "or %0, %%edx\n\t"
          :: "m" (*port), "m" (value));
#endif
#endif
}
