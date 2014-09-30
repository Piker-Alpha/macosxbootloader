//********************************************************************
//	created:	6:11:2009   20:18
//	filename: 	DebuggerUtils.cpp
//	author:		tiamo
//	purpose:	debugger routine
//********************************************************************

#include "stdafx.h"
#include "BootDebuggerPrivate.h"

KPCR* BdPcr																	= nullptr;
KPRCB* BdPrcb																= nullptr;
UINT64 BdPcrPhysicalAddress													= 0;

//
// debug breakpoint
//
VOID __declspec(naked) BOOTAPI DbgBreakPoint()
{
	__asm
	{
		int		3
		retn
	}
}

//
// debug service
//
VOID __declspec(naked) BOOTAPI DbgService(UINTN serviceType, UINTN info1, UINTN info2, UINTN info3, UINTN info4)
{
	__asm
	{
		push		ebp
		mov			ebp, esp
		push		ecx
		push		ebx
		push		edi
		mov			eax, [ebp + 0x08]
		mov			ecx, [ebp + 0x0c]
		mov			edx, [ebp + 0x10]
		mov			ebx, [ebp + 0x14]
		mov			edi, [ebp + 0x18]
		int			0x2d
		int			3
		pop			edi
		pop			ebx
		leave
		retn
	}
}

//
// debug service
//
VOID __declspec(naked) BOOTAPI DbgService(VOID* info1, VOID* info2, UINTN serviceType)
{
	__asm
	{
		push		ebp
		mov			ebp, esp
		mov			eax, [ebp + 0x10]
		mov			ecx, [ebp + 0x08]
		mov			edx, [ebp + 0x0c]
		int			0x2d
		int			3
		leave
		retn
	}
}

//
// return from exception handler
//
STATIC VOID __declspec(naked) BdpTrapExit()
{
	__asm
	{
		lea			esp, [ebp+30h]
		pop			gs
		pop			es
		pop			ds
		pop			edx
		pop			ecx
		pop			eax
		add			esp, 8													// skip exception list and previous previous mode
		pop			fs
		pop			edi
		pop			esi
		pop			ebx
		pop			ebp
		add			esp, 4													// skip error code
		iretd
	}
}

//
// common dispatch
//
STATIC VOID __declspec(naked) BdpTrapDispatch()
{
	__asm
	{
		sub			esp, size EXCEPTION_RECORD								// sizeof(EXCEPTION_RECORD) = 0x50
		mov			[esp + EXCEPTION_RECORD.ExceptionCode], eax				// ExceptionCode
		xor			eax, eax
		mov			[esp + EXCEPTION_RECORD.ExceptionFlags], eax			// ExceptionFlags
		mov			[esp + EXCEPTION_RECORD.ExceptionRecord], eax			// ExceptionRecord
		mov			[esp + EXCEPTION_RECORD.ExceptionAddress], ebx			// ExceptionAddress
		mov			[esp + EXCEPTION_RECORD.NumberParameters], ecx			// NumberParameters
		mov			[esp + EXCEPTION_RECORD.ExceptionInformation], edx		// ExceptionInformation0
		mov			[esp + EXCEPTION_RECORD.ExceptionInformation + 4], edi	// ExceptionInformation1
		mov			[esp + EXCEPTION_RECORD.ExceptionInformation + 8], esi	// ExceptionInformation2
		mov			eax, dr0
		mov			[ebp + KTRAP_FRAME.Dr0], eax							// dr0
		mov			eax, dr1
		mov			[ebp + KTRAP_FRAME.Dr1], eax							// dr1
		mov			eax, dr2
		mov			[ebp + KTRAP_FRAME.Dr2], eax							// dr2
		mov			eax, dr3
		mov			[ebp + KTRAP_FRAME.Dr3], eax							// dr3
		mov			eax, dr6
		mov			[ebp + KTRAP_FRAME.Dr6], eax							// dr6
		mov			eax, dr7
		mov			[ebp + KTRAP_FRAME.Dr7], eax							// dr7
		mov			ax, ss
		mov			[ebp + KTRAP_FRAME.TempSegCs], eax						// TempSegCs
		mov			[ebp + KTRAP_FRAME.TempEsp], ebp						// TempEsp
		add			dword ptr [ebp + KTRAP_FRAME.TempEsp], 0x74				// 0x74 = FIELD_OFFSET(KTRAP_FRAME,HardwareEsp)
		mov			ecx, esp
		push		ebp														// trap frame
		push		0														// kernel mode
		push		ecx														// exception record
		call		BdDebugTrap
		add			esp, size EXCEPTION_RECORD + 0x0c
		retn
	}
}

//
// single step exception
//
VOID __declspec(naked) BdTrap01()
{
	__asm
	{
		push		0														// dummy error code
		push		ebp
		push		ebx
		push		esi
		push		edi
		push		fs
		push		0ffffffffh												// ExceptionList
		push		0ffffffffh												// PreviousPreviousMode
		push		eax
		push		ecx
		push		edx
		push		ds
		push		es
		push		gs
		sub			esp, 30h												// KTRAP_FRAME.SegGs
		mov			ebp, esp
		cld
		and			dword ptr [ebp + KTRAP_FRAME.EFlags], 0fffffeffh		// clear single step flag
		mov			eax, 80000004h											// EXCEPTION_SINGLE_STEP
		mov			ebx, [ebp+KTRAP_FRAME.Eip]								// exception address = Eip
		xor			ecx, ecx												// param count = 0
		call		BdpTrapDispatch
		jmp			BdpTrapExit
	}
}

//
// breakpoint exception
//
VOID __declspec(naked) BdTrap03()
{
	__asm
	{
		push		0
		push		ebp
		push		ebx
		push		esi
		push		edi
		push		fs
		push		0ffffffffh
		push		0ffffffffh
		push		eax
		push		ecx
		push		edx
		push		ds
		push		es
		push		gs
		sub			esp, 30h
		mov			ebp, esp
		cld
		dec			dword ptr [ebp + KTRAP_FRAME.Eip]						// point to breakpoint instruction
		mov			eax, 80000003h											// EXCEPTION_BREAKPOINT
		mov			ebx, [ebp + KTRAP_FRAME.Eip]							// exception address = Eip
		mov			ecx,0													// param count
		xor			edx, edx
		call		BdpTrapDispatch
		jmp			BdpTrapExit
	}
}

//
// general protection
//
VOID __declspec(naked) BdTrap0d()
{
	__asm
	{
		push		ebp
		push		ebx
		push		esi
		push		edi
		push		fs
		push		0ffffffffh
		push		0ffffffffh
		push		eax
		push		ecx
		push		edx
		push		ds
		push		es
		push		gs
		sub			esp, 30h
		mov			ebp, esp
		cld
loop_forever:
		mov			eax, 0c0000005h											// EXCEPTION_ACCESS_VIOLATION
		mov			ebx, [ebp + KTRAP_FRAME.Eip]							// exception address = eip
		mov			ecx, 1													// param count = 1
		mov			edx, [ebp + KTRAP_FRAME.ErrCode]						// hardware error code
		and			edx, 0ffffh
		call		BdpTrapDispatch
		jmp			loop_forever
	}
}

//
// page fault
//
VOID __declspec(naked) BdTrap0e()
{
	__asm
	{
		push		ebp
		push		ebx
		push		esi
		push		edi
		push		fs
		push		0ffffffffh
		push		0ffffffffh
		push		eax
		push		ecx
		push		edx
		push		ds
		push		es
		push		gs
		sub			esp, 30h
		mov			ebp, esp
		cld
loop_forever:
		mov			eax, 0c0000005h											// EXCEPTION_ACCESS_VIOLATION
		mov			ebx, [ebp + KTRAP_FRAME.Eip]							// exception address = eip
		mov			ecx, 3													// param count = 3
		mov			edx, [ebp + KTRAP_FRAME.ErrCode]						// hardware error code
		and			edx, 2													// read or write
		mov			edi, cr2												// reference memory location
		xor			esi,esi
		call		BdpTrapDispatch
		jmp			loop_forever
	}
}

//
// debug service
//
VOID __declspec(naked) BdTrap2d()
{
	__asm
	{
		push		0
		push		ebp
		push		ebx
		push		esi
		push		edi
		push		fs
		push		0ffffffffh
		push		0ffffffffh
		push		eax
		push		ecx
		push		edx
		push		ds
		push		es
		push		gs
		sub			esp, 30h
		mov			ebp, esp
		cld
		mov			eax, 80000003h											// EXCEPTION_BREAKPOINT
		mov			ebx, [ebp + KTRAP_FRAME.Eip]							// exception address = eip
		mov			ecx, 3													// param count = 3
		xor			edx, edx
		mov			edx, [ebp + KTRAP_FRAME.Eax]							// edx = eax = debug service type
		mov			edi, [ebp + KTRAP_FRAME.Ecx]							// edi = ecx = debug service param1
		mov			esi, [ebp + KTRAP_FRAME.Edx]							// esi = edx = debug service param2
		call		BdpTrapDispatch
		jmp			BdpTrapExit
	}
}

//
// save processor context
//
STATIC VOID __declspec(naked) BOOTAPI BdpSaveProcessorControlState(KPROCESSOR_STATE* processorState)
{
	__asm
	{
		mov			edx, [esp + 4]
		xor			ecx, ecx
		mov			eax, cr0
		mov			[edx + KPROCESSOR_STATE.SpecialRegisters.Cr0], eax
		mov			eax, cr2
		mov			[edx + KPROCESSOR_STATE.SpecialRegisters.Cr2], eax
		mov			eax, cr3
		mov			[edx + KPROCESSOR_STATE.SpecialRegisters.Cr3], eax
		mov			[edx + KPROCESSOR_STATE.SpecialRegisters.Cr4], ecx
		mov			eax, dr0
		mov			[edx + KPROCESSOR_STATE.SpecialRegisters.KernelDr0], eax
		mov			eax, dr1
		mov			[edx + KPROCESSOR_STATE.SpecialRegisters.KernelDr1], eax
		mov			eax, dr2
		mov			[edx + KPROCESSOR_STATE.SpecialRegisters.KernelDr2], eax
		mov			eax, dr3
		mov			[edx + KPROCESSOR_STATE.SpecialRegisters.KernelDr3], eax
		mov			eax, dr6
		mov			[edx + KPROCESSOR_STATE.SpecialRegisters.KernelDr6], eax
		mov			eax, dr7
		mov			dr7, ecx
		mov			[edx + KPROCESSOR_STATE.SpecialRegisters.KernelDr7], eax
		sgdt		fword ptr [edx + KPROCESSOR_STATE.SpecialRegisters.Gdtr.Limit]
		sidt		fword ptr [edx + KPROCESSOR_STATE.SpecialRegisters.Idtr.Limit]
		str			word ptr  [edx + KPROCESSOR_STATE.SpecialRegisters.Tr]
		sldt		word ptr  [edx + KPROCESSOR_STATE.SpecialRegisters.Ldtr]
		retn
	}
}

//
// save trapframe
//
STATIC VOID BdpSaveKframe(KTRAP_FRAME* trapFrame, CONTEXT* contextRecord)
{
	contextRecord->Ebp														= trapFrame->Ebp;
	contextRecord->Eip														= trapFrame->Eip;
	contextRecord->SegCs													= trapFrame->SegCs;
	contextRecord->EFlags													= trapFrame->EFlags;
	contextRecord->Esp														= trapFrame->TempEsp;
	contextRecord->SegSs													= trapFrame->TempSegCs;
	contextRecord->SegDs													= trapFrame->SegDs;
	contextRecord->SegEs													= trapFrame->SegEs;
	contextRecord->SegFs													= trapFrame->SegFs;
	contextRecord->SegGs													= trapFrame->SegGs;
	contextRecord->Eax														= trapFrame->Eax;
	contextRecord->Ebx														= trapFrame->Ebx;
	contextRecord->Ecx														= trapFrame->Ecx;
	contextRecord->Edx														= trapFrame->Edx;
	contextRecord->Edi														= trapFrame->Edi;
	contextRecord->Esi														= trapFrame->Esi;
	contextRecord->Dr0														= trapFrame->Dr0;
	contextRecord->Dr1														= trapFrame->Dr1;
	contextRecord->Dr2														= trapFrame->Dr2;
	contextRecord->Dr3														= trapFrame->Dr3;
	contextRecord->Dr6														= trapFrame->Dr6;
	contextRecord->Dr7														= trapFrame->Dr7;

	BdpSaveProcessorControlState(&BdPrcb->ProcessorState);
}

//
// restore processor context
//
STATIC VOID __declspec(naked) BOOTAPI BdpRestoreProcessorControlState(KPROCESSOR_STATE* processorState)
{
	__asm
	{
		mov			edx, [esp + 4]
		mov			eax, [edx + KPROCESSOR_STATE.SpecialRegisters.Cr0]
		mov			cr0, eax
		mov			eax, [edx + KPROCESSOR_STATE.SpecialRegisters.Cr2]
		mov			cr2, eax
		mov			eax, [edx + KPROCESSOR_STATE.SpecialRegisters.Cr3]
		mov			cr3, eax
		mov			eax, [edx + KPROCESSOR_STATE.SpecialRegisters.KernelDr0]
		mov			dr0, eax
		mov			eax, [edx + KPROCESSOR_STATE.SpecialRegisters.KernelDr1]
		mov			dr1, eax
		mov			eax, [edx + KPROCESSOR_STATE.SpecialRegisters.KernelDr2]
		mov			dr2, eax
		mov			eax, [edx + KPROCESSOR_STATE.SpecialRegisters.KernelDr3]
		mov			dr3, eax
		mov			eax, [edx + KPROCESSOR_STATE.SpecialRegisters.KernelDr6]
		mov			dr6, eax
		mov			eax, [edx + KPROCESSOR_STATE.SpecialRegisters.KernelDr7]
		mov			dr7, eax
		lgdt		fword ptr [edx + KPROCESSOR_STATE.SpecialRegisters.Gdtr.Limit]
		lidt		fword ptr [edx + KPROCESSOR_STATE.SpecialRegisters.Idtr.Limit]
		lldt		word ptr  [edx + KPROCESSOR_STATE.SpecialRegisters.Ldtr]
		retn
	}
}

//
// restore trap frame
//
STATIC VOID BdpRestoreKframe(KTRAP_FRAME* trapFrame, CONTEXT* contextRecord)
{
	trapFrame->Ebp															= contextRecord->Ebp;
	trapFrame->Eip															= contextRecord->Eip;
	trapFrame->SegCs														= contextRecord->SegCs;
	trapFrame->EFlags														= contextRecord->EFlags;
	trapFrame->SegDs														= contextRecord->SegDs;
	trapFrame->SegEs														= contextRecord->SegEs;
	trapFrame->SegFs														= contextRecord->SegFs;
	trapFrame->SegGs														= contextRecord->SegGs;
	trapFrame->Edi															= contextRecord->Edi;
	trapFrame->Esi															= contextRecord->Esi;
	trapFrame->Eax															= contextRecord->Eax;
	trapFrame->Ebx															= contextRecord->Ebx;
	trapFrame->Ecx															= contextRecord->Ecx;
	trapFrame->Edx															= contextRecord->Edx;

	BdpRestoreProcessorControlState(&BdPrcb->ProcessorState);
}

#pragma optimize("",off)

//
// debug routine used when debugger is enabled
//
BOOLEAN BdTrap(EXCEPTION_RECORD* exceptionRecord, struct _KEXCEPTION_FRAME* exceptionFrame, KTRAP_FRAME* trapFrame)
{
	BdPrcb->ProcessorState.ContextFrame.ContextFlags						= CONTEXT_FULL | CONTEXT_DEBUG_REGISTERS;

	if(exceptionRecord->ExceptionCode != STATUS_BREAKPOINT || exceptionRecord->ExceptionInformation[0] == BREAKPOINT_BREAK)
	{
		BdpSaveKframe(trapFrame, &BdPrcb->ProcessorState.ContextFrame);
		BdReportExceptionStateChange(exceptionRecord, &BdPrcb->ProcessorState.ContextFrame);
		BdpRestoreKframe(trapFrame, &BdPrcb->ProcessorState.ContextFrame);

		BdControlCPressed													= FALSE;

		return TRUE;
	}

	UINT32 debugServiceType													= exceptionRecord->ExceptionInformation[0];
	switch(debugServiceType)
	{
	case BREAKPOINT_PRINT:
		{
			STRING printString;
			printString.Length												= static_cast<UINT16>(exceptionRecord->ExceptionInformation[2]);
			printString.Buffer												= ArchConvertAddressToPointer(exceptionRecord->ExceptionInformation[1], CHAR8*);
			if(BdDebuggerNotPresent)
				trapFrame->Eax												= static_cast<UINT32>(STATUS_DEVICE_NOT_CONNECTED);
			else
				trapFrame->Eax												= BdPrintString(&printString) ? STATUS_BREAKPOINT : STATUS_SUCCESS;

			trapFrame->Eip													+= sizeof(KDP_BREAKPOINT_TYPE);
		}
		break;

	case BREAKPOINT_PROMPT:
		{
			STRING inputString;
			inputString.Length												= static_cast<UINT16>(exceptionRecord->ExceptionInformation[2]);
			inputString.Buffer												= ArchConvertAddressToPointer(exceptionRecord->ExceptionInformation[1], CHAR8*);

			STRING outputString;
			outputString.MaximumLength										= static_cast<UINT16>(trapFrame->Edi);
			outputString.Buffer												= ArchConvertAddressToPointer(trapFrame->Ebx, CHAR8*);

			while(BdPromptString(&inputString, &outputString)) {}

			trapFrame->Eax													= outputString.Length;
			trapFrame->Eip													+= sizeof(KDP_BREAKPOINT_TYPE);
		}
		break;

	case BREAKPOINT_LOAD_SYMBOLS:
	case BREAKPOINT_UNLOAD_SYMBOLS:
		{
			BdpSaveKframe(trapFrame, &BdPrcb->ProcessorState.ContextFrame);

			UINT32 savedContextEip											= BdPrcb->ProcessorState.ContextFrame.Eip;

			if(!BdDebuggerNotPresent)
			{
				STRING* moduleName											= ArchConvertAddressToPointer(exceptionRecord->ExceptionInformation[1], STRING*);
				KD_SYMBOLS_INFO* symbolsInfo								= ArchConvertAddressToPointer(exceptionRecord->ExceptionInformation[2], KD_SYMBOLS_INFO*);
				BOOLEAN unloadSymbols										= debugServiceType == BREAKPOINT_UNLOAD_SYMBOLS;

				BdReportLoadSymbolsStateChange(moduleName, symbolsInfo, unloadSymbols, &BdPrcb->ProcessorState.ContextFrame);
			}

			if(savedContextEip == BdPrcb->ProcessorState.ContextFrame.Eip)
				BdPrcb->ProcessorState.ContextFrame.Eip					+= sizeof(KDP_BREAKPOINT_TYPE);

			BdpRestoreKframe(trapFrame, &BdPrcb->ProcessorState.ContextFrame);
		}
		break;

	default:
		return FALSE;
		break;
	}

	return TRUE;
}

#pragma optimize("",on)

//
// extract continuation control data from Manipulate_State message
//
VOID BdGetStateChange(DBGKD_MANIPULATE_STATE64* manipulateState, CONTEXT* contextRecord)
{
	if(!NT_SUCCESS(manipulateState->Continue2.ContinueStatus))
		return;

	//
	// the debugger is doing a continue, and it makes sense to apply control changes.
	//
	if(manipulateState->Continue2.ControlSet.TraceFlag == 1)
		contextRecord->EFlags												|= 0x100;
	else
		contextRecord->EFlags												&= ~0x100;

	BdPrcb->ProcessorState.SpecialRegisters.KernelDr7						= manipulateState->Continue2.ControlSet.Dr7;
	BdPrcb->ProcessorState.SpecialRegisters.KernelDr6						= 0;
}

//
// set context state
//
VOID BdSetContextState(DBGKD_WAIT_STATE_CHANGE64* waitStateChange, CONTEXT* contextRecord)
{
	waitStateChange->ControlReport.Dr6										= BdPrcb->ProcessorState.SpecialRegisters.KernelDr6;
	waitStateChange->ControlReport.Dr7										= BdPrcb->ProcessorState.SpecialRegisters.KernelDr7;
	waitStateChange->ControlReport.EFlags									= contextRecord->EFlags;
	waitStateChange->ControlReport.SegCs									= static_cast<UINT16>(contextRecord->SegCs);
	waitStateChange->ControlReport.SegDs									= static_cast<UINT16>(contextRecord->SegDs);
	waitStateChange->ControlReport.SegEs									= static_cast<UINT16>(contextRecord->SegEs);
	waitStateChange->ControlReport.SegFs									= static_cast<UINT16>(contextRecord->SegFs);
	waitStateChange->ControlReport.ReportFlags								= REPORT_INCLUDES_SEGS;
}

//
// read control space
//
VOID BdReadControlSpace(DBGKD_MANIPULATE_STATE64* manipulateState, STRING* additionalData, CONTEXT* contextRecord)
{
	STRING messageHeader;
	DBGKD_READ_MEMORY64* readMemory											= &manipulateState->ReadMemory;
	messageHeader.Length													= sizeof(DBGKD_MANIPULATE_STATE64);
	messageHeader.Buffer													= static_cast<CHAR8*>(static_cast<VOID*>(manipulateState));
	UINT32 readLength														= readMemory->TransferCount;
	if(readMemory->TransferCount > PACKET_MAX_SIZE - sizeof(DBGKD_MANIPULATE_STATE64))
		readLength															= PACKET_MAX_SIZE - sizeof(DBGKD_MANIPULATE_STATE64);

	if(static_cast<UINT32>(readMemory->TargetBaseAddress) + readLength <= sizeof(KPROCESSOR_STATE))
	{
		VOID* readAddress													= Add2Ptr(&BdPrcb->ProcessorState.ContextFrame.ContextFlags, static_cast<UINT32>(readMemory->TargetBaseAddress), VOID*);
		readLength															= BdMoveMemory(additionalData->Buffer, readAddress, readLength);
		additionalData->Length												= static_cast<UINT16>(readLength);
		manipulateState->ReturnStatus										= STATUS_SUCCESS;
		readMemory->ActualBytesRead											= readLength;
	}
	else
	{
		additionalData->Length												= 0;
		manipulateState->ReturnStatus										= STATUS_UNSUCCESSFUL;
		readMemory->ActualBytesRead											= 0;
	}

	BdSendPacket(PACKET_TYPE_KD_STATE_MANIPULATE, &messageHeader, additionalData);
}

//
// write control space
//
VOID BdWriteControlSpace(DBGKD_MANIPULATE_STATE64* manipulateState, STRING* additionalData, CONTEXT* contextRecord)
{
	STRING messageHeader;
	DBGKD_WRITE_MEMORY64* writeMemory										= &manipulateState->WriteMemory;
	UINT32 writeLength														= writeMemory->TransferCount;
	messageHeader.Length													= sizeof(DBGKD_MANIPULATE_STATE64);
	messageHeader.Buffer													= static_cast<CHAR8*>(static_cast<VOID*>(manipulateState));
	if(writeLength >= additionalData->Length)
		writeLength															= additionalData->Length;

	if(static_cast<UINT32>(writeMemory->TargetBaseAddress) + writeLength <= sizeof(KPROCESSOR_STATE))
	{
		VOID* writeAddress													= Add2Ptr(&BdPrcb->ProcessorState.ContextFrame.ContextFlags, static_cast<UINT32>(writeMemory->TargetBaseAddress), VOID*);
		writeLength															= BdMoveMemory(writeAddress, additionalData->Buffer, writeLength);
		manipulateState->ReturnStatus										= STATUS_SUCCESS;
		writeMemory->ActualBytesWritten										= writeLength;
	}
	else
	{
		manipulateState->ReturnStatus										= STATUS_UNSUCCESSFUL;
		writeMemory->ActualBytesWritten										= 0;
	}

	BdSendPacket(PACKET_TYPE_KD_STATE_MANIPULATE, &messageHeader, nullptr);
}

//
// set common state
//
VOID BdSetCommonState(UINT32 newState, CONTEXT* contextRecord, DBGKD_WAIT_STATE_CHANGE64* waitStateChange)
{
	//
	// sign extend
	//
	waitStateChange->ProgramCounter											= static_cast<UINT64>(static_cast<INT32>(contextRecord->Eip));
	waitStateChange->NewState												= newState;
	waitStateChange->NumberProcessors										= 1;
	waitStateChange->Thread													= 0;
	waitStateChange->Processor												= 0;
	waitStateChange->ProcessorLevel											= 0;

	memset(&waitStateChange->ControlReport, 0, sizeof(waitStateChange->ControlReport));

	//
	// copy instructions
	//
	VOID* dstBuffer															= waitStateChange->ControlReport.InstructionStream;
	VOID* srcBuffer															= ArchConvertAddressToPointer(contextRecord->Eip, VOID*);
	waitStateChange->ControlReport.InstructionCount							= static_cast<UINT16>(BdMoveMemory(dstBuffer, srcBuffer, DBGKD_MAXSTREAM));

	//
	// delete breakpoint in this range
	// there were any breakpoints cleared, recopy the area without them
	//
	if(BdDeleteBreakpointRange(contextRecord->Eip, waitStateChange->ControlReport.InstructionCount + contextRecord->Eip - 1))
		BdMoveMemory(waitStateChange->ControlReport.InstructionStream, ArchConvertAddressToPointer(contextRecord->Eip, VOID*), waitStateChange->ControlReport.InstructionCount);
}

//
// transfer physical address
//
VOID* BdTranslatePhysicalAddress(UINT64 phyAddress)
{
	return ArchConvertAddressToPointer(phyAddress, VOID*);
}

//
// initialize arch
//
EFI_STATUS BdArchInitialize()
{
	BdPcrPhysicalAddress													= 4 * 1024 * 1024 * 1024ULL - 1;
	BdPcr																	= static_cast<KPCR*>(MmAllocatePages(AllocateMaxAddress, EfiBootServicesData, EFI_SIZE_TO_PAGES(sizeof(KPCR)), &BdPcrPhysicalAddress));
	if(!BdPcr)
	{
		BdPcrPhysicalAddress												= 0;
		return EFI_OUT_OF_RESOURCES;
	}
	BdPrcb																	= &BdPcr->PrcbData;

	KDESCRIPTOR idtr;
	ArchGetIdtRegister(&idtr);

	UINT32 segCs															= ArchGetSegCs();
	ArchSetIdtEntry(idtr.Base, 0x01, segCs, &BdTrap01, 0x8e00);
	ArchSetIdtEntry(idtr.Base, 0x03, segCs, &BdTrap03, 0x8e00);
	ArchSetIdtEntry(idtr.Base, 0x0d, segCs, &BdTrap0d, 0x8e00);
	ArchSetIdtEntry(idtr.Base, 0x0e, segCs, &BdTrap0e, 0x8e00);
	ArchSetIdtEntry(idtr.Base, 0x2d, segCs, &BdTrap2d, 0x8e00);
	ArchSetIdtRegister(&idtr);

	BdArchBlockDebuggerOperation											= FALSE;
	return EFI_SUCCESS;
}

//
// destroy arch
//
EFI_STATUS BdArchDestroy()
{
	if(BdPcrPhysicalAddress)
		MmFreePages(BdPcrPhysicalAddress);

	BdPcrPhysicalAddress													= 0;
	BdArchBlockDebuggerOperation											= TRUE;
	return EFI_SUCCESS;
}