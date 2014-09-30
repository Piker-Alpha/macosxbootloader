;*********************************************************************
;	created:	6:11:2009   17:22
;	filename: 	DebugTrap.asm
;	author:		tiamo
;	purpose:	debug trap
;*********************************************************************

									default rel
									[bits 64]

%include "Common.inc"

%macro TRAP_FRAME_ENTER				1

								%if %1 <> 0
									sub					rsp, 8
								%endif

									push				rbp
									sub					rsp, _KTRAP_FRAME.Rbp
									mov					rbp, rsp
									mov					byte [rbp + _KTRAP_FRAME.ExceptionActive], 1
									mov					[rbp + _KTRAP_FRAME.Rax], rax
									mov					[rbp + _KTRAP_FRAME.Rcx], rcx
									mov					[rbp + _KTRAP_FRAME.Rdx], rdx
									mov					[rbp + _KTRAP_FRAME.R8 ], r8
									mov					[rbp + _KTRAP_FRAME.R9 ], r9
									mov					[rbp + _KTRAP_FRAME.R10], r10
									mov					[rbp + _KTRAP_FRAME.R11], r11
									cld

%endmacro

extern ?BdTrap@@YAEPEAU_EXCEPTION_RECORD@@PEAU_KEXCEPTION_FRAME@@PEAU_KTRAP_FRAME@@@Z

BdDispatch:
									sub					rsp, _EXCEPTION_RECORD64_size + _KEXCEPTION_FRAME_size + 8

									mov					rax, rsp
									mov					[rax + _KEXCEPTION_FRAME.Rbx], rbx
									mov					[rax + _KEXCEPTION_FRAME.Rdi], rdi
									mov					[rax + _KEXCEPTION_FRAME.Rsi], rsi
									mov					[rax + _KEXCEPTION_FRAME.R12], r12
									mov					[rax + _KEXCEPTION_FRAME.R13], r13
									mov					[rax + _KEXCEPTION_FRAME.R14], r14
									mov					[rax + _KEXCEPTION_FRAME.R15], r15

									lea					rax, [rsp + _KEXCEPTION_FRAME_size]

									mov					[rax + _EXCEPTION_RECORD64.ExceptionCode], ecx
									xor					rcx, rcx
									mov					[rax + _EXCEPTION_RECORD64.ExceptionFlags], ecx
									mov					[rax + _EXCEPTION_RECORD64.ExceptionRecord], rcx
									mov					[rax + _EXCEPTION_RECORD64.ExceptionAddress], r8
									mov					[rax + _EXCEPTION_RECORD64.NumberParameters], edx
									mov					[rax + _EXCEPTION_RECORD64.ExceptionInformation0], r9
									mov					[rax + _EXCEPTION_RECORD64.ExceptionInformation1], r10
									mov					[rax + _EXCEPTION_RECORD64.ExceptionInformation2], r11

									mov					r8 , rbp
									mov					rdx, rsp
									mov					rcx, rax
									call				?BdTrap@@YAEPEAU_EXCEPTION_RECORD@@PEAU_KEXCEPTION_FRAME@@PEAU_KTRAP_FRAME@@@Z

									mov					rcx, rsp
									mov					rbx, [rcx + _KEXCEPTION_FRAME.Rbx]
									mov					rdi, [rcx + _KEXCEPTION_FRAME.Rdi]
									mov					rsi, [rcx + _KEXCEPTION_FRAME.Rsi]
									mov					r12, [rcx + _KEXCEPTION_FRAME.R12]
									mov					r13, [rcx + _KEXCEPTION_FRAME.R13]
									mov					r14, [rcx + _KEXCEPTION_FRAME.R14]
									mov					r15, [rcx + _KEXCEPTION_FRAME.R15]

									mov					rsp, rbp

									mov					r11, [rbp + _KTRAP_FRAME.R11]
									mov					r10, [rbp + _KTRAP_FRAME.R10]
									mov					r9,  [rbp + _KTRAP_FRAME.R9 ]
									mov					r8,  [rbp + _KTRAP_FRAME.R8 ]
									mov					rcx, [rbp + _KTRAP_FRAME.Rcx]
									mov					rdx, [rbp + _KTRAP_FRAME.Rdx]
									mov					rax, [rbp + _KTRAP_FRAME.Rax]
									mov					rbp, [rbp + _KTRAP_FRAME.Rbp]
									add					rsp, _KTRAP_FRAME.Rip
									iretq

									align				16
PUBLIC_ROUTINE ?BdTrap01@@YAXXZ
									TRAP_FRAME_ENTER	1

									and					dword [rbp + _KTRAP_FRAME.EFlags], 0fffffeffh
									mov					ecx, 80000004h
									xor					edx, edx
									mov					r8, [rbp + _KTRAP_FRAME.Rip]
									call				BdDispatch

.loop_forever:
									jmp					.loop_forever

									align				16
PUBLIC_ROUTINE ?BdTrap03@@YAXXZ
									TRAP_FRAME_ENTER	1

									dec					qword [rbp + _KTRAP_FRAME.Rip]
									mov					ecx, 80000003h
									mov					edx, 1
									mov					r8,  [rbp + _KTRAP_FRAME.Rip]
									dec					r8
									xor					r9d, r9d
									call				BdDispatch

.loop_forever:
									jmp					.loop_forever

									align				16
PUBLIC_ROUTINE ?BdTrap0d@@YAXXZ
									TRAP_FRAME_ENTER	0

									mov					ecx, 0c0000005h
									mov					edx, 2
									mov					r8,  [rbp + _KTRAP_FRAME.Rip]
									mov					r9d, [rbp + _KTRAP_FRAME.ErrCode]
									and					r9d, 0ffffh
									xor					r10, r10
									call				BdDispatch

.loop_forever:
									jmp					.loop_forever

									align				16
PUBLIC_ROUTINE ?BdTrap0e@@YAXXZ
									TRAP_FRAME_ENTER	0

									mov					eax, [rbp + _KTRAP_FRAME.ErrCode]
									mov					r10, cr2
									shr					eax, 1
									mov					[rbp + 0d0h], r10
									and					eax, 9
									mov					ecx, 0c0000005h
									mov					[rbp + 2ah], al
									mov					edx, 2
									mov					r9d, [rbp + _KTRAP_FRAME.ErrCode]
									mov					r8,  [rbp + _KTRAP_FRAME.Rip]
									and					r9d, 2
									call				BdDispatch

.loop_forever:
									jmp					.loop_forever

									align				16
PUBLIC_ROUTINE ?BdTrap2d@@YAXXZ
									TRAP_FRAME_ENTER	1

									mov					ecx, 80000003h
									mov					edx, 3
									mov					r8, [rbp + _KTRAP_FRAME.Rip]
									mov					r9,  [rbp + _KTRAP_FRAME.Rax]
									mov					r10, [rbp + _KTRAP_FRAME.Rcx]
									mov					r11, [rbp + _KTRAP_FRAME.Rdx]
									call				BdDispatch

.loop_forever:
									jmp					.loop_forever

PUBLIC_ROUTINE ?DbgBreakPoint@@YAXXZ
									int3
									retn

PUBLIC_ROUTINE ?DbgService@@YAX_K0000@Z
									mov					rax, rcx
									mov					rcx, rdx
									mov					rdx, r8
									mov					r8, r9
									mov					r9, [rsp + 28h]
									int					2dh
									int3
									retn

PUBLIC_ROUTINE ?DbgService@@YAXPEAX0_K@Z
									mov					rax, r8
									int					2dh
									int3
									retn

PUBLIC_ROUTINE ?BdpSaveProcessorControlState@@YAXPEAU_KSPECIAL_REGISTERS@@@Z
									mov					rax, cr0
									mov					[rcx + _KSPECIAL_REGISTERS.Cr0], rax
									mov					rax, cr2
									mov					[rcx + _KSPECIAL_REGISTERS.Cr2], rax
									mov					rax, cr3
									mov					[rcx + _KSPECIAL_REGISTERS.Cr3], rax
									mov					rax, cr4
									mov					[rcx + _KSPECIAL_REGISTERS.Cr4], rax
									mov					rax, cr8
									mov					[rcx + _KSPECIAL_REGISTERS.Cr8], rax
									sgdt				[rcx + _KSPECIAL_REGISTERS.GdtrLimit]
									sidt				[rcx + _KSPECIAL_REGISTERS.IdtrLimit]
									str					[rcx + _KSPECIAL_REGISTERS.Tr]
									sldt				[rcx + _KSPECIAL_REGISTERS.Ldtr]
									mov					rax, dr0
									mov					rdx, dr1
									mov					[rcx + _KSPECIAL_REGISTERS.KernelDr0], rax
									mov					[rcx + _KSPECIAL_REGISTERS.KernelDr1], rdx
									mov					rax, dr2
									mov					rdx, dr3
									mov					[rcx + _KSPECIAL_REGISTERS.KernelDr2], rax
									mov					[rcx + _KSPECIAL_REGISTERS.KernelDr3], rdx
									mov					rax, dr6
									mov					rdx, dr7
									mov					[rcx + _KSPECIAL_REGISTERS.KernelDr6], rax
									mov					[rcx + _KSPECIAL_REGISTERS.KernelDr7], rdx
									xor					eax, eax
									mov					dr7, rax
									retn

PUBLIC_ROUTINE ?BdpRestoreProcessorControlState@@YAXPEAU_KSPECIAL_REGISTERS@@@Z
									mov					rax, [rcx + _KSPECIAL_REGISTERS.Cr0]
									mov					cr0, rax
									mov					rax, [rcx + _KSPECIAL_REGISTERS.Cr3]
									mov					cr3, rax
									mov					rax, [rcx + _KSPECIAL_REGISTERS.Cr4]
									mov					cr4, rax
									mov					rax, [rcx + _KSPECIAL_REGISTERS.Cr8]
									mov					cr8, rax
									lgdt				[rcx + _KSPECIAL_REGISTERS.GdtrLimit]
									lidt				[rcx + _KSPECIAL_REGISTERS.IdtrLimit]
									xor					eax, eax
									lldt				ax
									xor					edx, edx
									mov					dr7, rdx
									mov					rax, [rcx + _KSPECIAL_REGISTERS.KernelDr0]
									mov					rdx, [rcx + _KSPECIAL_REGISTERS.KernelDr1]
									mov					dr0, rax
									mov					dr1, rdx
									mov					rax, [rcx + _KSPECIAL_REGISTERS.KernelDr2]
									mov					rdx, [rcx + _KSPECIAL_REGISTERS.KernelDr3]
									mov					dr2, rax
									mov					dr3, rdx
									mov					rdx, [rcx + _KSPECIAL_REGISTERS.KernelDr7]
									xor					eax, eax
									mov					dr6, rax
									mov					dr7, rdx
									retn