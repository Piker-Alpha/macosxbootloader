global __aullshr
__aullshr:
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

global __alldiv
__alldiv:
%ifdef ARCH64
		push		rdi
		push		rsi
		push		rbx
%else
		push		edi
		push		esi
		push		ebx
%endif

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

		mul			dword [esp + 28]
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
%ifdef ARCH64
		pop			rbx
		pop			rsi
		pop			rdi
%else
		pop			ebx
		pop			esi
		pop			edi
%endif

		ret			16

global __aulldiv
__aulldiv:
%ifdef ARCH64
		push		rbx
		push		rsi
%else
		push		ebx
		push		esi
%endif

		mov			eax, [esp + 24]
		or			eax, eax
		jnz			LA

		mov			ecx, [esp + 20]
		mov			eax, [esp + 16]
		xor			edx, edx
		div			ecx
		mov			ebx, eax
		mov			eax, [esp + 12]
		div			ecx
		mov			edx, ebx
		jmp			LB

	LA:
		mov			ecx, eax
		mov			ebx, [esp + 20]
		mov			edx, [esp + 14]
		mov			eax, [esp + 12]

	LC:
		shr			ecx, 1
		rcr			ebx, 1
		shr			edx, 1
		rcr			eax, 1
		or			ecx, ecx
		jnz			LC
		div			ebx
		mov			esi, eax

		mul			dword [esp + 24]
		mov			ecx, eax
		mov			eax, [esp + 20]
		mul			esi
		add			edx, ecx
		jc			LD

		cmp			edx, [esp + 16]
		ja			LD
		jb			LE
		cmp			eax, [esp + 12]
		jbe			LE
	LD:
		dec			esi
	LE:
		xor			edx, edx
		mov			eax, esi

	LB:

%ifdef ARCH64
		pop			rsi
		pop			rbx
%else
		pop			esi
		pop			ebx
%endif

		ret			16

global __aullrem
__aullrem:
%ifdef ARCH64
		push		rbx
%else
		push		ebx
%endif

		mov			eax, [esp + 20]
		or			eax, eax
		jnz			L2A

		mov			ecx, [esp + 16]
		mov			eax, [esp + 12]
		xor			edx, edx
		div			ecx
		mov			eax, [esp + 8]
		div			ecx
		mov			eax, edx
		xor			edx, edx
		jmp			L2B
	L1A:
		mov			ecx, eax
		mov			ebx, [esp + 16]
		mov			edx, [esp + 12]
		mov			eax, [esp + 8]
	L1C:
		shr			ecx, 1
		rcr			ebx, 1
		shr			edx, 1
		rcr			eax, 1
		or			ecx, ecx
		jnz			L1C

		div			ebx
		mov			ecx, eax
		mul			dword [esp + 20]
		xchg		ecx, eax
		mul			dword [esp + 16]
		add			edx, ecx
		jc			L1D

		cmp			edx, [esp + 12]
		ja			L1D
		jb			L1E

		cmp			eax, [esp + 8]
		jbe			L1E
	L1D:
		sub			eax, [esp + 16]
		sbb			edx, [esp + 20]
	L1E:
		sub			eax, [esp + 8]
		sbb			edx, [esp + 12]
		neg			edx
		neg			eax
		sbb			edx, 0
	L1B:
%ifdef ARCH64
		pop			rbx
%else
		pop			ebx
%endif
		ret			16

global __allshl
		cmp			cl, 64
		jae			RETZEROA

		cmp			cl, 32
		jae			MORE32A
		shld		edx, eax, cl
		shl			eax, cl
		ret

	MORE32A:
		mov			edx, eax
		xor			eax, eax
		and			cl, 31
		shl			edx, cl
		ret

	RETZEROA:
		xor			eax, eax
		xor			edx, edx
		ret

global __allrem
__allrem:
%ifdef ARCH64
		push		rbx
		push		rdi
%else
		push		ebx
		push		edi
%endif
		xor			edi, edi

		mov			eax, [esp + 16]
		or			eax, eax
		jge			L2A
		inc			edi
		mov			edx, [esp + 12]
		neg			eax
		neg			edx
		sbb			eax, 0
		mov			[esp + 16], eax
		mov			[esp + 12], edx

	L2A:
		mov			eax, [esp + 24]
		or			eax, eax
		jge			L2B
		mov			edx, [esp + 20]
		neg			eax
		neg			edx
		sbb			eax, 0
		mov			[esp + 24], eax
		mov			[esp + 20], edx

	L2B:
		or			eax, eax
		jnz			L2C
		mov			ecx, [esp + 20]
		mov			eax, [esp + 16]
		xor			edx, edx
		div			ecx
		mov			eax, [esp + 12]
		div			ecx
		mov			eax, edx
		xor			edx, edx
		dec			edi
		jns			L2D
		jmp			L8

	L2C:
		mov			ebx, eax
		mov			ecx, [esp + 20]
		mov			edx, [esp + 16]
		mov			eax, [esp + 12]
	L2E:
		shr			ebx, 1
		rcr			ecx, 1
		shr			edx, 1
		rcr			eax, 1
		or			ebx, ebx
		jnz			L2E

		div			ecx
		mov			ecx, eax
		mul			dword [esp + 24]
		xchg		ecx, eax
		mul			dword [esp + 20]
		add			edx, ecx
		jc			L2F

		cmp			edx, [esp + 16]
		ja			L2F
		jb			L2G
		cmp			eax, [esp + 12]
		jbe			L2G

	L2F:
		sub			eax, [esp + 20]
		sbb			edx, [esp + 24]

	L2G:
		sub			eax, [esp + 12]
		sbb			edx, [esp + 16]
		dec			edi
		jns			L2H
	L2D:
		neg			edx
		neg			eax
		sbb			edx, 0

	L2H:
%ifdef ARCH64
		pop			rdi
		pop			rbx
%else
		pop			edi
		pop			ebx
%endif

		ret			16

global __aullmul
__aullmul:
		mov			eax, [esp + 8]
		mov			ecx, [esp + 16]
		or			ecx, eax
		mov			ecx, [esp + 12]
		jnz			HARD

		mov			eax, [esp + 4]
		mul			ecx
		ret			16

	HARD:
%ifdef ARCH64
		push		rbx
%else
		push		ebx
%endif
		mul			ecx
		mov			ebx, eax
		mov			eax, [esp + 8]
		mul			dword [esp + 20]
		add			ebx, eax
		mov			eax, [esp + 8]
		mul			ecx
		add			edx, ebx
%ifdef ARCH64
		pop			rbx
%else
		pop			ebx
%endif

		ret			16

global __allshl
__allshr:
		cmp			cl, 64
		jae			RETSIGNB

		cmp			cl, 32
		jae			MORE32B
		shrd		eax, edx, cl
		sar			edx, cl
		ret

	MORE32B:
		mov			eax, edx
		sar			edx, 31
		and			cl, 31
		sar			eax, cl
		ret

	RETSIGNB:
		sar			edx, 31
		mov			eax, edx
		ret

global __aulldvrm
__aulldvrm:
%ifdef ARCH64
	push		rsi
%else
	push		esi
%endif

        mov			eax, [esp + 20]
        or			eax, eax
        jnz			L3A
        mov			ecx, [esp + 16]
        mov			eax, [esp + 12]
        xor			edx, edx
        div			ecx
        mov			ebx, eax
        mov			eax, [esp + 8]
        div			ecx
        mov			esi, eax

        mov			eax, ebx
        mul			dword [esp + 16]
        mov			ecx, eax
        mov			eax, esi
        mul			dword [esp + 16]
        add			edx, ecx
        jmp			L3B
	L3A:
        mov			ecx, eax
        mov			ebx, [esp + 16]
        mov			edx, [esp + 12]
        mov			eax, [esp + 8]
	L3C:
        shr			ecx, 1
        rcr			ebx, 1
        shr			edx, 1
        rcr			eax, 1
        or			ecx, ecx
        jnz			L3C
        div			ebx
        mov			esi, eax

        mul			dword [esp + 20]
        mov			ecx, eax
        mov			eax, [esp + 16]
        mul			esi
        add			edx, ecx
        jc			L3D

        cmp			edx, [esp + 12]
        ja			L3D
        jb			L3E
        cmp			eax, [esp + 8]
        jbe			L3E
	L3D:
        dec			esi
        sub			eax, [esp + 16]
        sbb			edx, [esp + 20]
	L3E:
        xor			ebx, ebx

	L3B:
        sub			eax, [esp + 8]
        sbb			edx, [esp + 12]
        neg			edx
        neg			eax
        sbb			edx, 0

        mov			ecx, edx
        mov			edx, ebx
        mov			ebx, ecx
        mov			ecx, eax
        mov			eax, esi

%ifdef ARCH64
	pop			rsi
%else
        pop			esi
%endif

        ret			16

global __alldvrm
__alldvrm:
%ifdef ARCH64
	push		rdi
        push		rsi
        push		rbp
%else
	push		edi
        push		esi
        push		ebp
%endif

        xor			edi, edi
        xor			ebp, ebp

        mov			eax, [esp + 20]
        or			eax, eax
        jge			L4A
        inc			edi
        inc			ebp
        mov			edx, [esp + 16]
        neg			eax
        neg			edx
        sbb			eax, 0
        mov			[esp + 20], eax
        mov			[esp + 16], edx
	L4A:
        mov			eax, [esp + 28]
        or			eax, eax
        jge			L4B
        inc			edi
        mov			edx, [esp + 24]
        neg			eax
        neg			edx
        sbb			eax, 0
        mov			[esp + 28], eax
        mov			[esp + 24], edx
	L4B:
        or			eax, eax
        jnz			L4C
        mov			ecx, [esp + 24]
        mov			eax, [esp + 20]
        xor			edx, edx
        div			ecx
        mov			ebx, eax
        mov			eax, [esp + 16]
        div			ecx
        mov			esi, eax

        mov			eax, ebx
        mul			dword [esp + 24]
        mov			ecx, eax
        mov			eax, esi
        mul			dword [esp + 24]
        add			edx, ecx
        jmp			L4D

	L4C:
        mov			ebx, eax
        mov			ecx, [esp + 24]
        mov			edx, [esp + 20]
        mov			eax, [esp + 16]
	L4E:
        shr			ebx, 1
        rcr			ecx, 1
        shr			edx, 1
        rcr			eax, 1
        or			ebx, ebx
        jnz			L4E
        div			ecx
        mov			esi, eax

        mul			dword [esp + 28]
        mov			ecx, eax
        mov			eax, [esp + 24]
        mul			esi
        add			edx, ecx
        jc			L4F

        cmp			edx, [esp + 20]
        ja			L4F
        jb			L4G
        cmp			eax, [esp + 16]
        jbe			L4G
	L4F:
        dec			esi
        sub			eax, [esp + 24]
        sbb			edx, [esp + 28]
	L4G:
        xor			ebx, ebx

	L4D:
        sub			eax, [esp + 16]
        sbb			edx, [esp + 20]

		dec			ebp
        jns			L4I
        neg			edx
        neg			eax
        sbb			edx, 0

	L4I:
        mov			ecx, edx
        mov			edx, ebx
        mov			ebx, ecx
        mov			ecx, eax
        mov			eax, esi

		dec			edi
        jnz			L4H
        neg			edx
        neg			eax
        sbb			edx, 0

	L4H:
%ifdef ARCH64
        pop			rbp
        pop			rsi
        pop			rdi
%else
        pop			ebp
        pop			esi
        pop			edi
%endif

        ret			16

global __chkstk
__chkstk:
		cmp			eax, 1000h
		jnb			BIG_STACK

		neg			eax
		add			eax, esp
		add			eax, 4
		test		[eax], eax
		xchg		eax, esp
		mov			eax, [eax]
%ifdef ARCH64
		push		rax
%else
		push		eax
%endif
		retn

	BIG_STACK:
%ifdef ARCH64
		push		rcx
%else
		push		ecx
%endif
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
%ifdef ARCH64
		push		rax
%else
		push		eax
%endif
		retn

global __alloca_probe_16
__alloca_probe_16:
		jmp			__chkstk

global __except_handler3
__except_handler3:
		xor eax, eax
		retn

global __SEH_epilog
__SEH_epilog:
%ifdef ARCH64
		pop			rcx
		pop			rdi
		pop			rsi
		pop			rbx
%else
		pop			ecx
		pop			edi
		pop			esi
		pop			ebx
%endif
		leave
%ifdef ARCH64
		push		rcx
%else
		push		ecx
%endif
		retn


global __SEH_prolog
__SEH_prolog:
		push		0
		xor			eax,eax
%ifdef ARCH64
		push		rax
%else
		push		eax
%endif
		mov			eax, [esp + 0x10]
		mov			[esp + 0x10], ebp
		lea			ebp, [esp + 0x10]
		sub			esp, eax
%ifdef ARCH64
		push		rbx
		push		rsi
		push		rdi
%else
		push		ebx
		push		esi
		push		edi
%endif
		mov			eax, [ebp - 8]
		mov			[ebp - 18h], esp
%ifdef ARCH64
		push		rax
%else
		push		eax
%endif
		mov			eax, [ebp - 4]
		mov			dword [ebp - 4], 0ffffffffh
		mov			[ebp - 8], eax
		lea			eax, [ebp - 10h]
		retn
