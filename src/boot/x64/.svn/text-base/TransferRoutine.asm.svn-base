;*********************************************************************
;	created:	5:10:2009   15:44
;	filename: 	TransferRoutine.asm
;	author:		tiamo
;	purpose:	transfer routine
;*********************************************************************

									default rel
									[bits 64]

%include "Common.inc"

PUBLIC_ROUTINE ?ArchTransferRoutineBegin@@YAXXZ
									cli
									lea					rax, [.32bits_code]
									mov					[.far_jmp_offset], eax
									lea					rax, [.gdt_null]
									mov					[.gdtr_offset], rax
									lgdt				[.gdtr_limit]
									mov					ax, 10h
									mov					ds, ax
									mov					es, ax
									mov					gs, ax
									mov					fs, ax
									lea					rax, [.far_jmp_offset]
									jmp					dword far [rax]

									[bits 32]
.32bits_code:
									mov					eax, cr0
									btr					eax, 31
									mov					cr0, eax
									mov					edi, ecx
									mov					ebx, edx
									mov					ecx, 0c0000080h
									rdmsr
									btr					eax, 8
									wrmsr
									jmp					.flush
.flush:
									mov					eax, ebx
									jmp					edi
									hlt
									retn

.far_jmp_offset:					dd					0
.far_jmp_selector:					dw					8
.gdtr_limit:						dw					18h
.gdtr_offset:						dq					0

.gdt_null:							dd					0, 0
.gdt_code_32:						dd					0ffffh, 0cf9e00h
.gdt_data_32:						dd					0ffffh, 0cf9200h

PUBLIC_ROUTINE ?ArchTransferRoutineEnd@@YAXXZ
									hlt
									retn
