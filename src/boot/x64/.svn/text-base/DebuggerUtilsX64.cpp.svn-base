//********************************************************************
//	created:	7:11:2009   16:00
//	filename: 	DebuggerUtils.cpp
//	author:		tiamo
//	purpose:	debugger utils
//********************************************************************

#include "stdafx.h"
#include "BootDebuggerPrivate.h"

KPCR* BdPcr																	= nullptr;
KPRCB* BdPrcb																= nullptr;
UINT64 BdPcrPhysicalAddress													= 0;

//
// translate physical address
//
VOID* BdTranslatePhysicalAddress(UINT64 physicalAddress)
{
	return ArchConvertAddressToPointer(physicalAddress, VOID*);
}

//
// set common state
//
VOID BdSetCommonState(UINT32 newState, CONTEXT* contextRecord, DBGKD_WAIT_STATE_CHANGE64* waitStateChange)
{
	//
	// sign extend
	//
	waitStateChange->ProgramCounter											= contextRecord->Rip;
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
	VOID* srcBuffer															= ArchConvertAddressToPointer(contextRecord->Rip, VOID*);
	waitStateChange->ControlReport.InstructionCount							= static_cast<UINT16>(BdMoveMemory(dstBuffer, srcBuffer, DBGKD_MAXSTREAM));

	//
	// delete breakpoint in this range
	// there were any breakpoints cleared, recopy the area without them
	//
	if(BdDeleteBreakpointRange(contextRecord->Rip, waitStateChange->ControlReport.InstructionCount + contextRecord->Rip - 1))
		BdMoveMemory(waitStateChange->ControlReport.InstructionStream, ArchConvertAddressToPointer(contextRecord->Rip, VOID*), waitStateChange->ControlReport.InstructionCount);
}

//
// set context state
//
VOID BdSetContextState(DBGKD_WAIT_STATE_CHANGE64* waitStateChange, CONTEXT* contextRecord)
{
	waitStateChange->ControlReport.Dr6										= BdPrcb->ProcessorState.SpecialRegisters.KernelDr6;
	waitStateChange->ControlReport.Dr7										= BdPrcb->ProcessorState.SpecialRegisters.KernelDr7;
	waitStateChange->ControlReport.EFlags									= contextRecord->EFlags;
	waitStateChange->ControlReport.SegCs									= contextRecord->SegCs;
	waitStateChange->ControlReport.SegDs									= contextRecord->SegDs;
	waitStateChange->ControlReport.SegEs									= contextRecord->SegEs;
	waitStateChange->ControlReport.SegFs									= contextRecord->SegFs;
	waitStateChange->ControlReport.ReportFlags								= REPORT_INCLUDES_SEGS;
}

//
// get state change
//
VOID BdGetStateChange(DBGKD_MANIPULATE_STATE64* manipulateState, CONTEXT* contextRecord)
{
	//
	// the debugger is doing a continue, and it makes sense to apply control changes.
	//
	if(manipulateState->Continue2.ControlSet.TraceFlag)
		contextRecord->EFlags												|= 0x100;
	else
		contextRecord->EFlags												&= ~0x100;

	BdPrcb->ProcessorState.SpecialRegisters.KernelDr7						= manipulateState->Continue2.ControlSet.Dr7;
	BdPrcb->ProcessorState.SpecialRegisters.KernelDr6						= 0;
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

	switch(readMemory->TargetBaseAddress)
	{
	case 0:
		readLength															= sizeof(BdPcr->GdtBase);
		BdCopyMemory(additionalData->Buffer, &BdPcr->GdtBase, readLength);
		additionalData->Length												= static_cast<UINT16>(readLength);
		manipulateState->ReturnStatus										= STATUS_SUCCESS;
		readMemory->ActualBytesRead											= readLength;
		break;

	case 1:
		readLength															= EFI_FIELD_OFFSET(KPRCB, CurrentThread);
		BdCopyMemory(additionalData->Buffer, BdPcr, readLength);
		additionalData->Length												= static_cast<UINT16>(readLength);
		manipulateState->ReturnStatus										= STATUS_SUCCESS;
		readMemory->ActualBytesRead											= readLength;
		break;

	case 2:
		readLength															= BdMoveMemory(additionalData->Buffer, &BdPrcb->ProcessorState.SpecialRegisters, sizeof(KSPECIAL_REGISTERS));
		additionalData->Length												= static_cast<UINT16>(readLength);
		manipulateState->ReturnStatus										= STATUS_SUCCESS;
		readMemory->ActualBytesRead											= readLength;
		break;

	default:
		manipulateState->ReturnStatus										= STATUS_UNSUCCESSFUL;
		readMemory->ActualBytesRead											= 0;
		additionalData->Length												= 0;
		break;
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
	messageHeader.Length													= sizeof(DBGKD_MANIPULATE_STATE64);
	messageHeader.Buffer													= static_cast<CHAR8*>(static_cast<VOID*>(manipulateState));
	UINT32 writeLength														= writeMemory->TransferCount;
	if(writeLength >= additionalData->Length)
		writeLength															= additionalData->Length;

	switch(writeMemory->TargetBaseAddress)
	{
	case 2:
		if(writeLength < sizeof(KSPECIAL_REGISTERS))
			writeLength														= sizeof(KSPECIAL_REGISTERS);

		writeLength															= BdMoveMemory(&BdPrcb->ProcessorState.SpecialRegisters, additionalData->Buffer, writeLength);
		manipulateState->ReturnStatus										= STATUS_SUCCESS;
		writeMemory->ActualBytesWritten										= writeLength;
		break;

	default:
		manipulateState->ReturnStatus										= STATUS_UNSUCCESSFUL;
		writeMemory->ActualBytesWritten										= 0;
		break;
	}

	BdSendPacket(PACKET_TYPE_KD_STATE_MANIPULATE, &messageHeader, nullptr);
}

//
// save trapframe
//
STATIC VOID BdpSaveKframe(KTRAP_FRAME* trapFrame, KEXCEPTION_FRAME* exceptionFrame, CONTEXT* contextRecord)
{
	contextRecord->Rbp														= trapFrame->Rbp;
	contextRecord->Rip														= trapFrame->Rip;
	contextRecord->SegCs													= trapFrame->SegCs;
	contextRecord->EFlags													= trapFrame->EFlags;
	contextRecord->Rsp														= trapFrame->HardwareRsp;
	contextRecord->Rax														= trapFrame->Rax;
	contextRecord->Rcx														= trapFrame->Rcx;
	contextRecord->Rdx														= trapFrame->Rdx;
	contextRecord->R8														= trapFrame->R8;
	contextRecord->R9														= trapFrame->R9;
	contextRecord->R10														= trapFrame->R10;
	contextRecord->R11														= trapFrame->R11;

	contextRecord->Rbx														= exceptionFrame->Rbx;
	contextRecord->Rdi														= exceptionFrame->Rdi;
	contextRecord->Rsi														= exceptionFrame->Rsi;
	contextRecord->R12														= exceptionFrame->R12;
	contextRecord->R13														= exceptionFrame->R13;
	contextRecord->R14														= exceptionFrame->R14;
	contextRecord->R15														= exceptionFrame->R15;

	VOID BOOTAPI BdpSaveProcessorControlState(KSPECIAL_REGISTERS* specialRegisters);
	BdpSaveProcessorControlState(&BdPrcb->ProcessorState.SpecialRegisters);
}

//
// restore trapframe
//
STATIC VOID BdpRestoreKframe(KTRAP_FRAME* trapFrame, KEXCEPTION_FRAME* exceptionFrame, CONTEXT* contextRecord)
{
	trapFrame->Rbp															= contextRecord->Rbp;
	trapFrame->Rip															= contextRecord->Rip;
	trapFrame->SegCs														= contextRecord->SegCs;
	trapFrame->EFlags														= contextRecord->EFlags;
	trapFrame->Rax															= contextRecord->Rax;
	trapFrame->Rcx															= contextRecord->Rcx;
	trapFrame->Rdx															= contextRecord->Rdx;
	trapFrame->R8															= contextRecord->R8;
	trapFrame->R9															= contextRecord->R9;
	trapFrame->R10															= contextRecord->R10;
	trapFrame->R11															= contextRecord->R11;

	exceptionFrame->Rbx														= contextRecord->Rbx;
	exceptionFrame->Rdi														= contextRecord->Rdi;
	exceptionFrame->Rsi														= contextRecord->Rsi;
	exceptionFrame->R12														= contextRecord->R12;
	exceptionFrame->R13														= contextRecord->R13;
	exceptionFrame->R14														= contextRecord->R14;
	exceptionFrame->R15														= contextRecord->R15;

	VOID BOOTAPI BdpRestoreProcessorControlState(KSPECIAL_REGISTERS* specialRegisters);
	BdpRestoreProcessorControlState(&BdPrcb->ProcessorState.SpecialRegisters);
}

//
// trap handler
//
#pragma optimize("",off)
BOOLEAN BdTrap(EXCEPTION_RECORD* exceptionRecord, KEXCEPTION_FRAME* exceptionFrame, KTRAP_FRAME* trapFrame)
{
	BdPrcb->ProcessorState.ContextFrame.ContextFlags						= CONTEXT_FULL | CONTEXT_DEBUG_REGISTERS;
	if(exceptionRecord->ExceptionCode != STATUS_BREAKPOINT || exceptionRecord->ExceptionInformation[0] == BREAKPOINT_BREAK)
	{
		BdpSaveKframe(trapFrame, exceptionFrame, &BdPrcb->ProcessorState.ContextFrame);
		BdReportExceptionStateChange(exceptionRecord, &BdPrcb->ProcessorState.ContextFrame);
		BdpRestoreKframe(trapFrame, exceptionFrame, &BdPrcb->ProcessorState.ContextFrame);

		BdControlCPressed													= FALSE;

		return TRUE;
	}

	UINTN debugServiceType													= exceptionRecord->ExceptionInformation[0];
	switch(debugServiceType)
	{
	case BREAKPOINT_PRINT:
		{
			STRING printString;
			printString.Length												= static_cast<UINT16>(exceptionRecord->ExceptionInformation[2]);
			printString.Buffer												= ArchConvertAddressToPointer(exceptionRecord->ExceptionInformation[1], CHAR8*);
			if(BdDebuggerNotPresent)
				trapFrame->Rax												= static_cast<UINT64>(static_cast<INT64>(STATUS_DEVICE_NOT_CONNECTED));
			else
				trapFrame->Rax												= static_cast<UINT64>(static_cast<UINT64>(BdPrintString(&printString) ? STATUS_BREAKPOINT : STATUS_SUCCESS));

			trapFrame->Rip													+= sizeof(KDP_BREAKPOINT_TYPE);
		}
		break;

	case BREAKPOINT_PROMPT:
		{
			STRING inputString;
			inputString.Length												= static_cast<UINT16>(exceptionRecord->ExceptionInformation[2]);
			inputString.Buffer												= ArchConvertAddressToPointer(exceptionRecord->ExceptionInformation[1], CHAR8*);

			STRING outputString;
			outputString.MaximumLength										= static_cast<UINT16>(trapFrame->R9);
			outputString.Buffer												= ArchConvertAddressToPointer(trapFrame->R8, CHAR8*);

			while(BdPromptString(&inputString, &outputString))
			{
				NOTHING;
			}

			trapFrame->Rax													= outputString.Length;

			trapFrame->Rip													+= sizeof(KDP_BREAKPOINT_TYPE);
		}
		break;

	case BREAKPOINT_LOAD_SYMBOLS:
	case BREAKPOINT_UNLOAD_SYMBOLS:
		{
			BdpSaveKframe(trapFrame, exceptionFrame, &BdPrcb->ProcessorState.ContextFrame);

			UINT64 savedContextRip											= BdPrcb->ProcessorState.ContextFrame.Rip;

			if(!BdDebuggerNotPresent)
			{
				STRING* moduleName											= ArchConvertAddressToPointer(exceptionRecord->ExceptionInformation[1], STRING*);
				KD_SYMBOLS_INFO* symbolsInfo								= ArchConvertAddressToPointer(exceptionRecord->ExceptionInformation[2], KD_SYMBOLS_INFO*);
				BOOLEAN unloadSymbols										= debugServiceType == BREAKPOINT_UNLOAD_SYMBOLS;

				BdReportLoadSymbolsStateChange(moduleName, symbolsInfo, unloadSymbols, &BdPrcb->ProcessorState.ContextFrame);
			}

			if(savedContextRip == BdPrcb->ProcessorState.ContextFrame.Rip)
				BdPrcb->ProcessorState.ContextFrame.Rip						+= sizeof(KDP_BREAKPOINT_TYPE);

			BdpRestoreKframe(trapFrame, exceptionFrame, &BdPrcb->ProcessorState.ContextFrame);
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
// initialize
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
	BdPrcb																	= &BdPcr->Prcb;

	KDESCRIPTOR idtr;
	ArchGetIdtRegister(&idtr);

	extern VOID BdTrap01();
	extern VOID BdTrap03();
	extern VOID BdTrap0d();
	extern VOID BdTrap0e();
	extern VOID BdTrap2d();
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
// destroy
//
EFI_STATUS BdArchDestroy()
{
	if(BdPcrPhysicalAddress)
		MmFreePages(BdPcrPhysicalAddress);

	BdPcrPhysicalAddress													= 0;
	BdArchBlockDebuggerOperation											= TRUE;
	return EFI_SUCCESS;
}