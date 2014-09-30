									[bits 64]
									default 			rel

%define	KERNEL_BASE					0ffffff8000000000h
%define	CS_KERNEL32					50h
%define CS_KERNEL64					08h
%define DS_KERNEL32					10h

struc _EFI_TIME
	.Year:							resw				1
	.Month:							resb				1
	.Day:							resb				1
	.Hour:							resb				1
	.Minute:						resb				1
	.Second:						resb				1
	.Pad1:							resb				1
	.Nanosecond:					resd				1
	.TimeZone:						resw				1
	.Daylight:						resb				1
	.Pad2							resb				1
endstruc

struc _EFI_TIME_CAPS
	.Resolution:					resd				1
	.Accuracy:						resd				1
	.SetsToZero:					resb				1
endstruc

struc _EFI_GUID
	.Data0:							resd				1
	.Data1:							resw				1
	.Data2:							resw				1
	.Data3:							resb				8
endstruc

struc _GLOBAL_DATA
	.KernelArg0:					resq				1
	.KernelArg1:					resq				1
	.KernelArg2:					resq				1
	.KernelArg3:					resq				1
	.KernelArg4:					resq				1
	.KernelArg5:					resq				1
	.ArgLength0:					resd				1
	.ArgLength1:					resd				1
	.ArgLength2:					resd				1
	.ArgLength3:					resd				1
	.ArgLength4:					resd				1
	.ArgLength5:					resd				1
	.EfiArg0:						resd				1
	.EfiArg1:						resd				1
	.EfiArg2:						resd				1
	.EfiArg3:						resd				1
	.EfiArg4:						resd				1
	.EfiArg5:						resd				1
	.RetValue:						resq				1
	.ParamInfo:						resq				1
	.ParamCount:					resd				1
	.EfiRoutine:					resd				1
	.StackTop:						resq				1
	.KernelRSP:						resq				1
	.KernelCR3:						resq				1
endstruc

struc _PARAM_INFO
	.Input:							resb				1
	.Output:						resb				1
	.LengthType:					resb				1
	.Length:						resb				1
endstruc

struc _ROUTINE_INFO
	.Count:							resd				1
	.ParamInfo:						resb				_PARAM_INFO_size * 6
endstruc

efi_runtime_services:
.signature:							dq					0
.revision							dd					0
.header_size:						dd					0
.crc32:								dd					0
.reserved:							dd					0
.get_time:							dd					get_time_thunk - $$
.set_time:							dd					set_time_thunk - $$
.get_wakeup_time:					dd					get_wakeup_time_thunk - $$
.set_wakeup_time:					dd					set_wakeup_time_thunk - $$
.set_virtual_address_map:			dd					unsupported_thunk - $$
.convert_pointer:					dd					unsupported_thunk - $$
.get_variable:						dd					get_variable_thunk - $$
.get_next_variable_name:			dd					get_next_variable_name_thunk - $$
.set_variable:						dd					set_variable_thunk - $$
.get_next_high_monotonic_count:		dd					get_next_high_monotonic_count_thunk - $$
.reset_system:						dd					reset_system_thunk - $$
.report_status_code:				dd					noop_thunk - $$

EfiCR3:								dq					0
ScratchBufferTop:					dq					0
KernelIdlePML4						dq					0ffffff80008c0ac8h

									align				4
get_time_data:
									dd					2
									db					0, 1, 0, _EFI_TIME_size
									db					0, 1, 0, _EFI_TIME_CAPS_size

									align				4
set_time_data:
									dd					1
									db					1, 0, 0, _EFI_TIME_size

									align				4
get_wakeup_time_data:
									dd					3
									db					0, 1, 0, 1
									db					0, 1, 0, 1
									db					0, 1, 0, _EFI_TIME_size

									align				4
set_wakeup_time_data:
									dd					2
									db					0, 0, 0, 0
									db					1, 0, 0, _EFI_TIME_size

									align				4
get_variable_data:
									dd					5
									db					1, 0, 1, 0
									db					1, 0, 0, _EFI_GUID_size
									db					0, 1, 0, 4
									db					1, 1, 0, 4
									db					0, 1, 3, 3

									align				4
get_next_variable_name_data:
									dd					3
									db					1, 1, 0, 4
									db					1, 1, 3, 0
									db					1, 1, 0, _EFI_GUID_size

									align				4
set_variable_data:
									dd					5
									db					1, 0, 1, 0
									db					1, 0, 0, _EFI_GUID_size
									db					0, 0, 0, 0
									db					0, 0, 0, 0
									db					1, 0, 2, 3

									align				4
get_next_high_monotonic_count_data:
									dd					1
									db					0, 1, 0, 4

									align				4
reset_system_data:
									dd					4
									db					0, 0, 0, 0
									db					0, 0, 3, 0
									db					0, 0, 0, 0
									db					1, 0, 2, 2

									align				16
CommonThunk:
									push				rbp
									mov					rbp, rsp
									push				r15
									push				r14
									push				r13
									push				rdi
									pushf
									mov					r15, [ScratchBufferTop]
									mov					rdi, 0fffffffffffffff0h
									sub					r15, _GLOBAL_DATA_size
									mov					r13, 8000000000000003h
									and					r15, rdi
									mov					[r15 + _GLOBAL_DATA.KernelArg0], rcx
									mov					[r15 + _GLOBAL_DATA.KernelArg1], rdx
									mov					[r15 + _GLOBAL_DATA.KernelArg2], r8
									mov					[r15 + _GLOBAL_DATA.KernelArg3], r9
									mov					rcx, [rbp + 30h]
									mov					rdx, [rbp + 38h]
									mov					[r15 + _GLOBAL_DATA.KernelArg4], rcx
									mov					[r15 + _GLOBAL_DATA.KernelArg5], rdx
									mov					[r15 + _GLOBAL_DATA.ParamCount], r10d
									mov					[r15 + _GLOBAL_DATA.ParamInfo], r11
									mov					[r15 + _GLOBAL_DATA.EfiRoutine], eax
									mov					[r15 + _GLOBAL_DATA.StackTop], r15
									mov					[r15 + _GLOBAL_DATA.RetValue], r13
									mov					r14, r11
									xor					r13d, r13d

.check_input_count:
									cmp					r13d, [r15 + _GLOBAL_DATA.ParamCount]
									jnb					.build_efi_stack

									mov					edi, [r14 + r13 * 4]
									mov					rdx, [r15 + _GLOBAL_DATA.KernelArg0 + r13 * 8]
									test				di, di
									jz					.value_input

									test				rdx, rdx
									jz					.write_value_input

									mov					ecx, edi
									shr					ecx, 16
									cmp					cl, 0
									jz					.fixed_length_input

									dec					cl
									jz					.wstring_length_input

									movzx				eax, ch
									mov					rax, [r15 + _GLOBAL_DATA.KernelArg0 + rax * 8]
									dec					cl
									jz					.align_input_length
									test				rax, rax
									cmovnz				rax, [rax]
									jmp					.align_input_length

.fixed_length_input:
									movzx				eax, ch
									jmp					.align_input_length

.wstring_length_input:
									call				string_buffer_size_in_rdx

.align_input_length:
									mov					r8d, eax
									mov					[r15 + _GLOBAL_DATA.ArgLength0 + r13 * 4], eax
									add					eax, 0fh
									and					eax, 0fffffff0h
									sub					[r15 + _GLOBAL_DATA.StackTop], rax
									mov					rcx, [r15 + _GLOBAL_DATA.StackTop]
									mov					[r15 + _GLOBAL_DATA.EfiArg0 + r13 * 4], ecx

									test				dil, dil
									jz					.next_input

									call				memcpy
									jmp					.next_input

.value_input:
									test				edi, 00ff0000h
									jz					.write_value_input
									test				rdx, rdx
									jns					.write_value_input
									or					edx, 80000000h

.write_value_input:
									mov					[r15 + _GLOBAL_DATA.EfiArg0 + r13 * 4], edx
									mov					dword [r15 + _GLOBAL_DATA.ArgLength0 + r13 * 4], 0
.next_input:
									inc					r13d
									jmp					.check_input_count

.build_efi_stack:
									mov					rcx, [r15 + _GLOBAL_DATA.StackTop]
									lea					r8, [r15 + _GLOBAL_DATA.EfiArg0 + r13 * 4]
									test				r13d, r13d

.build_efi_stack_check_left:
									jz					.prepare_efi_call
									sub					r8, 4
									sub					rcx, 4
									mov					eax, [r8]
									dec					r13d
									mov					[rcx], eax
									jmp					.build_efi_stack_check_left

.prepare_efi_call:
									cli
									mfence
									clts

									mov					r10, [KernelIdlePML4]
									lea					rax, [.enter_compatibility_mode]
									mov					r8, [r10]
									mov					edx, [r15 + _GLOBAL_DATA.EfiRoutine]
									mov					r9, [r8 + 511 * 8]
									mov					r10, cr3
									mov					r8, [EfiCR3]
									mov					edi, eax
									mov					[r8], r9
									mov					[r15 + _GLOBAL_DATA.KernelCR3], r10
									mov					[r8 + 511 * 8], r9
									mov					eax, r8d
									mov					[r15 + _GLOBAL_DATA.KernelRSP], rsp
									mov					cr3, rax
									jmp					.flush_after_efi_cr3

.flush_after_efi_cr3:
									push				CS_KERNEL32
									push				rdi
								o64	retf

									[bits 32]
.enter_compatibility_mode:
									mov					ax, DS_KERNEL32
									mov					ds, ax
									mov					es, ax
									mov					ss, ax
									mov					esp, ecx
									call				edx

									lea					ecx, [edi + .enter_64_bit_mode - .enter_compatibility_mode]
									push				CS_KERNEL64
									push				ecx
									retf

									[bits 64]
.enter_64_bit_mode:
									lea					rcx, [.enter_ub_space]
									mov					rdx, KERNEL_BASE
									or					rcx, rdx
									jmp					rcx

.enter_ub_space:
									mov					ds, r13w
									mov					es, r13w
									mov					ss, r13w
									mov					rsp, [r15 + _GLOBAL_DATA.KernelRSP]

									mov					edx, 80000000h
									btr					eax, 31
									cmovnc				edx, r13d
									mov					[r15 + _GLOBAL_DATA.RetValue + 0], eax
									mov					[r15 + _GLOBAL_DATA.RetValue + 4], edx

.restore_kernel_cr3:
									mov					rcx, [r15 + _GLOBAL_DATA.KernelCR3]
									mov					cr3, rcx
									jmp					.check_output_count

.check_output_count:
									cmp					r13d, [r15 + _GLOBAL_DATA.ParamCount]
									jnb					.finished

									cmp					byte [r14 + r13 * 4 + 1], 0
									jz					.next_output

									mov					rcx, [r15 + _GLOBAL_DATA.KernelArg0 + r13 * 8]
									mov					rax, KERNEL_BASE
									mov					edx, [r15 + _GLOBAL_DATA.EfiArg0 + r13 * 4]
									mov					r8d, [r15 + _GLOBAL_DATA.ArgLength0 + r13 * 4]
									or					rdx, rax
									call				memcpy

.next_output:
									inc					r13d
									jmp					.check_output_count

.finished:
									mov					rax, [r15 + _GLOBAL_DATA.RetValue]
									popf
									pop					rdi
									pop					r13
									pop					r14
									pop					r15
									pop					rbp
									retn

string_buffer_size_in_rdx:
									mov					r8, rdx
									xor					eax, eax
.next:
									mov					r9w, [r8]
									add					r8, 2
									add					eax, 2
									test				r9w, r9w
									jnz					.next

									retn

memcpy:
									push				rdi
									push				rsi
									mov					rdi, rcx
									mov					rsi, rdx
									mov					ecx, r8d
									shr					ecx, 2
									cld
								rep	movsd
									mov					ecx, r8d
									and					ecx, 3
								rep movsb
									pop					rsi
									pop					rdi
									retn

%macro BUILD_THUNK_CODE	1
							%1_thunk:
									mov					eax, [efi_runtime_services.%1]
									lea					r11, [%1_data]
									mov					r10d, [r11 + _ROUTINE_INFO.Count]
									lea					r11, [r11 + _ROUTINE_INFO.ParamInfo]
									jmp					CommonThunk

%endmacro

BUILD_THUNK_CODE					get_time
BUILD_THUNK_CODE					set_time
BUILD_THUNK_CODE					get_wakeup_time
BUILD_THUNK_CODE					set_wakeup_time
BUILD_THUNK_CODE					get_variable
BUILD_THUNK_CODE					get_next_variable_name
BUILD_THUNK_CODE					set_variable
BUILD_THUNK_CODE					get_next_high_monotonic_count
BUILD_THUNK_CODE					reset_system

unsupported_thunk:
									mov					rax, 8000000000000003h
									retn

noop_thunk:
									xor					rax, rax
									retn