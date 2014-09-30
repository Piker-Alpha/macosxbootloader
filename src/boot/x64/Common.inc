
struc _KDESCRIPTOR
	.Padding						resw			3
	.Limit							resw			1
	.Base							resq			1
endstruc

struc _KSPECIAL_REGISTERS
	.Cr0:							resq			1
	.Cr2:							resq			1
	.Cr3:							resq			1
	.Cr4:							resq			1
	.KernelDr0:						resq			1
	.KernelDr1:						resq			1
	.KernelDr2:						resq			1
	.KernelDr3:						resq			1
	.KernelDr6:						resq			1
	.KernelDr7:						resq			1
	.GdtrPadding					resw			3
	.GdtrLimit						resw			1
	.GdtrBase						resq			1
	.IdtrPadding					resw			3
	.IdtrLimit						resw			1
	.IdtrBase						resq			1
	.Tr:							resw			1
	.Ldtr:							resw			1
	.MxCsr							resd			1
	.DebugControl:					resq			1
	.LastBranchToRip:				resq			1
	.LastBranchFromRip:				resq			1
	.LastExceptionToRip:			resq			1
	.LastExceptionFromRip:			resq			1
	.Cr8:							resq			1
	.MsrGsBase:						resq			1
	.MsrGsSwap:						resq			1
	.MsrStart:						resq			1
	.MsrLStart:						resq			1
	.MsrCStart:						resq			1
	.MsrSyscallMask.				resq			1
endstruc

struc _KTRAP_FRAME
	.P1Home							resq			1
	.P2Home							resq			1
	.P3Home							resq			1
	.P4Home							resq			1
	.P5Home							resq			1
	.PreviousMode					resb			1
	.PreviousIrql					resb			1
	.FaultIndicator					resb			1
	.ExceptionActive				resb			1
	.MxCsr							resd			1
	.Rax:							resq			1
	.Rcx:							resq			1
	.Rdx:							resq			1
	.R8: 							resq			1
	.R9: 							resq			1
	.R10:	 						resq			1
	.R11:	 						resq			1
	.GsBase:
	.GsSwap:						resq			1
	.Xmm0:							resq			2
	.Xmm1:							resq			2
	.Xmm2:							resq			2
	.Xmm3:							resq			2
	.Xmm4:							resq			2
	.Xmm5:							resq			2
	.FaultAddress:
	.ContextRecord:
	.TimeStampCKCL:					resq			1
	.Dr0:							resq			1
	.Dr1:							resq			1
	.Dr2:							resq			1
	.Dr3:							resq			1
	.Dr6:							resq			1
	.Dr7:							resq			1
	.DebugControl:
	.LastBranchControl				resq			1
	.LastBranchToRip:
	.LastBranchMsr:					resq			1
	.LastBranchFromRip:				resq			1
	.LastExceptionToRip:			resq			1
	.LastExceptionFromRip:			resq			1
	.SegDs:							resw			1
	.SegEs:							resw			1
	.SegFs:							resw			1
	.SegGs:							resw			1
	.TrapFrame:						resq			1
	.Rbx:							resq			1
	.Rdi:							resq			1
	.Rsi:							resq			1
	.Rbp:							resq			1
	.ErrCode:
	.ExceptionFrame:				resq			1
	.Rip:							resq			1
	.SegCs:							resq			1
	.EFlags:						resq			1
	.HardwareRsp:					resq			1
	.HardwareSs:					resq			1
endstruc

struc _KEXCEPTION_FRAME
	.P1Home							resq			1
	.P2Home							resq			1
	.P3Home							resq			1
	.P4Home							resq			1
	.P5Home							resq			1
	.InitialStack					resq			1
	.Xmm6:							resq			2
	.Xmm7:							resq			2
	.Xmm8:							resq			2
	.Xmm9:							resq			2
	.Xmm10:							resq			2
	.Xmm11:							resq			2
	.Xmm12:							resq			2
	.Xmm13:							resq			2
	.Xmm14:							resq			2
	.Xmm15:							resq			2
	.TrapFrame:						resq			1
	.CallbackBuffer:				resq			1
	.OutputBuffer:					resq			1
	.OutputLength:					resq			1
	.MxCsr:							resq			1
	.Rbp:							resq			1
	.Rbx:							resq			1
	.Rdi:							resq			1
	.Rsi:							resq			1
	.R12:							resq			1
	.R13:							resq			1
	.R14:							resq			1
	.R15:							resq			1
endstruc

struc _EXCEPTION_RECORD64
	.ExceptionCode:					resd			1
	.ExceptionFlags:				resd			1
	.ExceptionRecord:				resq			1
	.ExceptionAddress:				resq			1
	.NumberParameters:				resd			1
	.Padding						resd			1
	.ExceptionInformation0:			resq			1
	.ExceptionInformation1:			resq			1
	.ExceptionInformation2:			resq			1
	.ExceptionInformations:			resq			12
endstruc

%macro PUBLIC_ROUTINE							1
								global %1
								%1:
%endmacro
