//********************************************************************
//	created:	4:11:2009   17:19
//	filename: 	Compiler.cpp
//	author:		tiamo
//	purpose:	compiler stub
//********************************************************************

#include "stdafx.h"

//
// 64bit right shift
//
extern "C" int __declspec(naked) _aullshr()
{
	__asm
	{
		cmp			cl, 64
		jae			RETSIGN

		cmp			cl, 32
		jae			MORE32
		shrd		eax, edx, cl
		sar			edx, cl
		ret

	MORE32:
		mov			eax, edx
		sar			edx, 31
		and			cl, 31
		sar			eax, cl
		ret

	RETSIGN:
		sar			edx, 31
		mov			eax, edx
		ret
	}
}

//
// 64bit div
//
extern "C" void __declspec(naked) _alldiv()
{
	__asm
	{
		push		edi
		push		esi
		push		ebx

		xor			edi, edi

		mov			eax, [esp + 20]
		or			eax, eax
		jge			L1

		inc			edi
		mov			edx, [esp + 16]
		neg			eax
		neg			edx
		sbb			eax,0
		mov			[esp + 20], eax
		mov			[esp + 16], edx
	L1:
		mov			eax, [esp + 28]
		or			eax, eax
		jge			L2
		inc			edi
		mov			edx, [esp + 24]
		neg			eax
		neg			edx
		sbb			eax, 0
		mov			[esp + 28], eax
		mov			[esp + 24], edx
	L2:
		or			eax, eax
		jnz			L3
		mov			ecx, [esp + 24]
		mov			eax, [esp + 20]
		xor			edx, edx
		div			ecx
		mov			ebx, eax
		mov			eax, [esp + 16]
		div			ecx
		mov			edx, ebx
		jmp			L4

	L3:
		mov			ebx, eax
		mov			ecx, [esp + 24]
		mov			edx, [esp + 20]
		mov			eax, [esp + 16]
	L5:
		shr			ebx, 1
		rcr			ecx, 1
		shr			edx, 1
		rcr			eax, 1
		or			ebx, ebx
		jnz			L5
		div			ecx
		mov			esi, eax

		mul			dword ptr [esp + 28]
		mov			ecx, eax
		mov			eax, [esp + 24]
		mul			esi
		add			edx, ecx
		jc			L6

		cmp			edx, [esp + 20]
		ja			L6
		jb			L7
		cmp			eax, [esp + 16]
		jbe			L7
	L6:
		dec			esi
	L7:
		xor			edx, edx
		mov			eax, esi

	L4:
		dec			edi
		jnz			L8
		neg			edx
		neg			eax
		sbb			edx, 0

	L8:
		pop			ebx
		pop			esi
		pop			edi

		ret			16
	}
}

//
// ULONG64 div
//
extern "C" void __declspec(naked) _aulldiv()
{
	__asm
	{
		push		ebx
		push		esi

		mov			eax, [esp + 24]
		or			eax, eax
		jnz			L1

		mov			ecx, [esp + 20]
		mov			eax, [esp + 16]
		xor			edx, edx
		div			ecx
		mov			ebx, eax
		mov			eax, [esp + 12]
		div			ecx
		mov			edx, ebx
		jmp			L2

	L1:
		mov			ecx, eax
		mov			ebx, [esp + 20]
		mov			edx, [esp + 14]
		mov			eax, [esp + 12]

	L3:
		shr			ecx, 1
		rcr			ebx, 1
		shr			edx, 1
		rcr			eax, 1
		or			ecx, ecx
		jnz			L3
		div			ebx
		mov			esi, eax

		mul			dword ptr [esp + 24]
		mov			ecx, eax
		mov			eax, [esp + 20]
		mul			esi
		add			edx, ecx
		jc			L4

		cmp			edx, [esp + 16]
		ja			L4
		jb			L5
		cmp			eax, [esp + 12]
		jbe			L5
	L4:
		dec			esi
	L5:
		xor			edx, edx
		mov			eax, esi

	L2:

		pop			esi
		pop			ebx

		ret			16
	}
}

//
// ULONG rem
//
extern "C" void __declspec(naked) _aullrem()
{
	__asm
	{
		push		ebx

		mov			eax, [esp + 16][4]
		or			eax, eax
		jnz			L1

		mov			ecx, [esp + 16]
		mov			eax, [esp + 8][4]
		xor			edx, edx
		div			ecx
		mov			eax, [esp + 8]
		div			ecx
		mov			eax, edx
		xor			edx, edx
		jmp			L2
	L1:
		mov			ecx, eax
		mov			ebx, [esp + 16]
		mov			edx, [esp + 8][4]
		mov			eax, [esp + 8]
	L3:
		shr			ecx, 1
		rcr			ebx, 1
		shr			edx, 1
		rcr			eax, 1
		or			ecx, ecx
		jnz			L3

		div			ebx
		mov			ecx, eax
		mul			dword ptr [esp + 16][4]
		xchg		ecx, eax
		mul			dword ptr [esp + 16]
		add			edx, ecx
		jc			L4

		cmp			edx, [esp + 8][4]
		ja			L4
		jb			L5

		cmp			eax, [esp + 8]
		jbe			L5
	L4:
		sub			eax, [esp + 16]
		sbb			edx, [esp + 16][4]
	L5:
		sub			eax, [esp + 8]
		sbb			edx, [esp + 8][4]
		neg			edx
		neg			eax
		sbb			edx, 0
	L2:
		pop			ebx
		ret			16
	}
}

//
// 64bits shift left
//
extern "C" void __declspec(naked) _allshl()
{
	__asm
	{
		cmp			cl, 64
		jae			RETZERO

		cmp			cl, 32
		jae			MORE32
		shld		edx, eax, cl
		shl			eax, cl
		ret

	MORE32:
		mov			edx, eax
		xor			eax, eax
		and			cl, 31
		shl			edx, cl
		ret

	RETZERO:
		xor			eax, eax
		xor			edx, edx
		ret
	}
}

//
// LONG64 rem
//
extern "C" void __declspec(naked) _allrem()
{
	__asm
	{
		push		ebx
		push		edi
		xor			edi, edi

		mov			eax, [esp + 12][4]
		or			eax, eax
		jge			L1
		inc			edi
		mov			edx, [esp + 12]
		neg			eax
		neg			edx
		sbb			eax, 0
		mov			[esp + 12][4], eax
		mov			[esp + 12], edx

	L1:
		mov			eax, [esp + 20][4]
		or			eax, eax
		jge			L2
		mov			edx, [esp + 20]
		neg			eax
		neg			edx
		sbb			eax, 0
		mov			[esp + 20][4], eax
		mov			[esp + 20], edx

	L2:
		or			eax, eax
		jnz			L3
		mov			ecx, [esp + 20]
		mov			eax, [esp + 12][4]
		xor			edx, edx
		div			ecx
		mov			eax, [esp + 12]
		div			ecx
		mov			eax, edx
		xor			edx, edx
		dec			edi
		jns			L4
		jmp			L8

	L3:
		mov			ebx, eax
		mov			ecx, [esp + 20]
		mov			edx, [esp + 12][4]
		mov			eax, [esp + 12]
	L5:
		shr			ebx, 1
		rcr			ecx, 1
		shr			edx, 1
		rcr			eax, 1
		or			ebx, ebx
		jnz			L5

		div			ecx
		mov			ecx, eax
		mul			dword ptr [esp + 20][4]
		xchg		ecx, eax
		mul			dword ptr [esp + 20]
		add			edx, ecx
		jc			L6

		cmp			edx, [esp + 12][4]
		ja			L6
		jb			L7
		cmp			eax, [esp + 12]
		jbe			L7

	L6:
		sub			eax, [esp + 20]
		sbb			edx, [esp + 20][4]

	L7:
		sub			eax, [esp + 12]
		sbb			edx, [esp + 12][4]
		dec			edi
		jns			L8
	L4:
		neg			edx
		neg			eax
		sbb			edx, 0

	L8:
		pop			edi
		pop			ebx

		ret			16
	}
}

//
// LONG64 mul
//
extern "C" void __declspec(naked) _allmul()
{
	__asm
	{
		mov			eax, [esp + 4][4]
		mov			ecx, [esp + 12][4]
		or			ecx, eax
		mov			ecx, [esp + 12]
		jnz			HARD

		mov			eax, [esp + 4]
		mul			ecx
		ret			16

	HARD:
		push		ebx
		mul			ecx
		mov			ebx, eax
		mov			eax, [esp + 8]
		mul			dword ptr [esp + 16][4]
		add			ebx, eax
		mov			eax, [esp + 8]
		mul			ecx
		add			edx, ebx
		pop			ebx

		ret			16
	}
}

//
// LONG64 shr
//
extern "C" void	__declspec(naked) _allshr()
{
	__asm
	{
		cmp			cl, 64
		jae			RETSIGN

		cmp			cl, 32
		jae			MORE32
		shrd		eax, edx, cl
		sar			edx, cl
		ret

	MORE32:
		mov			eax, edx
		sar			edx, 31
		and			cl, 31
		sar			eax, cl
		ret

	RETSIGN:
		sar			edx, 31
		mov			eax, edx
		ret
	}
}

extern "C" VOID __declspec(naked) _aulldvrm()
{
	__asm
	{
		push		esi

        mov			eax, [esp + 16][4]
        or			eax, eax
        jnz			L1
        mov			ecx, [esp + 16]
        mov			eax, [esp + 8][4]
        xor			edx, edx
        div			ecx
        mov			ebx, eax
        mov			eax, [esp + 8]
        div			ecx
        mov			esi, eax

        mov			eax, ebx
        mul			dword ptr [esp + 16]
        mov			ecx, eax
        mov			eax, esi
        mul			dword ptr [esp + 16]
        add			edx, ecx
        jmp			L2
	L1:
        mov			ecx, eax
        mov			ebx, [esp + 16]
        mov			edx, [esp + 8][4]
        mov			eax, [esp + 8]
	L3:
        shr			ecx, 1
        rcr			ebx, 1
        shr			edx, 1
        rcr			eax, 1
        or			ecx, ecx
        jnz			L3
        div			ebx
        mov			esi, eax

        mul			dword ptr [esp + 16][4]
        mov			ecx, eax
        mov			eax, [esp + 16]
        mul			esi
        add			edx, ecx
        jc			L4

        cmp			edx, [esp + 8][4]
        ja			L4
        jb			L5
        cmp			eax, [esp + 8]
        jbe			L5
	L4:
        dec			esi
        sub			eax, [esp + 16]
        sbb			edx, [esp + 16][4]
	L5:
        xor			ebx, ebx

	L2:
        sub			eax, [esp + 8]
        sbb			edx, [esp + 8][4]
        neg			edx
        neg			eax
        sbb			edx, 0

        mov			ecx, edx
        mov			edx, ebx
        mov			ebx, ecx
        mov			ecx, eax
        mov			eax, esi

        pop			esi
        ret			16
	}
}

extern "C" VOID __declspec(naked) _alldvrm()
{
	__asm
	{
		push		edi
        push		esi
        push		ebp

        xor			edi, edi
        xor			ebp, ebp

        mov			eax, [esp + 16][4]
        or			eax, eax
        jge			L1
        inc			edi
        inc			ebp
        mov			edx, [esp + 16]
        neg			eax
        neg			edx
        sbb			eax, 0
        mov			[esp + 16][4], eax
        mov			[esp + 16], edx
	L1:
        mov			eax, [esp + 24][4]
        or			eax, eax
        jge			L2
        inc			edi
        mov			edx, [esp + 24]
        neg			eax
        neg			edx
        sbb			eax, 0
        mov			[esp + 24][4], eax
        mov			[esp + 24], edx
	L2:
        or			eax, eax
        jnz			L3
        mov			ecx, [esp + 24]
        mov			eax, [esp + 16][4]
        xor			edx, edx
        div			ecx
        mov			ebx, eax
        mov			eax, [esp + 16]
        div			ecx
        mov			esi, eax

        mov			eax, ebx
        mul			dword ptr [esp + 24]
        mov			ecx, eax
        mov			eax, esi
        mul			dword ptr [esp + 24]
        add			edx, ecx
        jmp			L4

	L3:
        mov			ebx, eax
        mov			ecx, [esp + 24]
        mov			edx, [esp + 16][4]
        mov			eax, [esp + 16]
	L5:
        shr			ebx, 1
        rcr			ecx, 1
        shr			edx, 1
        rcr			eax, 1
        or			ebx, ebx
        jnz			L5
        div			ecx
        mov			esi, eax

        mul			dword ptr [esp + 24][4]
        mov			ecx, eax
        mov			eax, [esp + 24]
        mul			esi
        add			edx, ecx
        jc			L6

        cmp			edx, [esp + 16][4]
        ja			L6
        jb			L7
        cmp			eax, [esp + 16]
        jbe			L7
	L6:
        dec			esi
        sub			eax, [esp + 24]
        sbb			edx, [esp + 24][4]
	L7:
        xor			ebx, ebx

	L4:
        sub			eax, [esp + 16]
        sbb			edx, [esp + 16][4]

		dec			ebp
        jns			L9
        neg			edx
        neg			eax
        sbb			edx, 0

	L9:
        mov			ecx, edx
        mov			edx, ebx
        mov			ebx, ecx
        mov			ecx, eax
        mov			eax, esi

		dec			edi
        jnz			L8
        neg			edx
        neg			eax
        sbb			edx, 0

	L8:
        pop			ebp
        pop			esi
        pop			edi

        ret			16
	}
}

extern "C" VOID __declspec(naked) _chkstk()
{
	__asm
	{
		cmp			eax, 1000h
		jnb			BIG_STACK

		neg			eax
		add			eax, esp
		add			eax, 4
		test		[eax], eax
		xchg		eax, esp
		mov			eax, [eax]
		push		eax
		retn

	BIG_STACK:
		push		ecx
		lea			ecx, [esp+8]

	TOUCH_STACK:
		sub			ecx, 1000h
		sub			eax, 1000h
		test		[ecx], eax
		cmp			eax, 1000h
		jnb			TOUCH_STACK

		sub			ecx, eax
		mov			eax, esp
		test		[ecx], eax
		mov			esp, ecx
		mov			ecx, [eax]
		mov			eax, [eax+4]
		push		eax
		retn
	}
}

extern "C" VOID __declspec(naked) _alloca_probe_16()
{
	__asm
	{
		jmp			_chkstk
	}
}

extern "C" int _except_handler3()
{
	return 0;
}

extern "C" VOID __declspec(naked) _SEH_epilog()
{
	__asm
	{
		pop			ecx
		pop			edi
		pop			esi
		pop			ebx
		leave
		push		ecx
		retn
	}
}

extern "C" VOID __declspec(naked) _SEH_prolog()
{
	__asm
	{
		push		0
		xor			eax,eax
		push		eax
		mov			eax, [esp + 0x10]
		mov			[esp + 0x10], ebp
		lea			ebp, [esp + 0x10]
		sub			esp, eax
		push		ebx
		push		esi
		push		edi
		mov			eax, [ebp - 8]
		mov			[ebp - 18h], esp
		push		eax
		mov			eax, [ebp - 4]
		mov			dword ptr [ebp - 4], 0ffffffffh
		mov			[ebp - 8], eax
		lea			eax, [ebp - 10h]
		retn
	}
}