//********************************************************************
//	created:	12:9:2012   23:10
//	filename: 	PanicDialog.cpp
//	author:		tiamo
//	purpose:	panic dialog
//********************************************************************

#include "StdAfx.h"
#include "PanicDialog.h"

#include <pshpack1.h>
typedef struct _PANIC_INFO_LOG
{
	//
	// signature 0x1234
	//
	UINT32																	Signature;

	//
	// write index
	//
	UINT32																	WriteIndex;

	//
	// checksum CRC32
	//
	UINT32																	CheckSum;

	//
	// reboot time
	//
	EFI_TIME																RebootTime[5];
}PANIC_INFO_LOG;
#include <poppack.h>

//
// panic info name
//
STATIC CHAR16 BlpPanicInfoName[19]											= {L'A', L'A', L'P', L'L', L',', L'P', L'a', L'n', L'i', L'c', L'I', L'n', L'f', L'o', L'0', L'0', L'0', L'0'};

//
// detect panic loop
//
STATIC BOOLEAN BlpDetectPanicLoop(UINT8 maxPanicInfoIndex, UINTN totalInfoLength)
{
	BOOLEAN retValue														= FALSE;
	UINT8* panicInfoBuffer													= nullptr;

	__try
	{
		UINT8* panicInfoBuffer												= static_cast<UINT8*>(MmAllocatePool(totalInfoLength));
		if(!panicInfoBuffer)
			try_leave(NOTHING);

		//
		// read all info
		//
		UINTN leftLength													= totalInfoLength;
		VOID* readBuffer													= panicInfoBuffer;
		UINTN dataSize														= leftLength;
		for(UINT8 i = 0; i <= maxPanicInfoIndex && leftLength; i ++)
		{
			BlpPanicInfoName[17]											= static_cast<CHAR16>(i < 10 ? L'0' + i : L'A' + i - 10);
			if(EFI_ERROR(EfiRuntimeServices->GetVariable(BlpPanicInfoName, &AppleNVRAMVariableGuid, nullptr, &dataSize, readBuffer)))
				try_leave(NOTHING);

			leftLength														-= dataSize;
			readBuffer														= Add2Ptr(readBuffer, dataSize, VOID*);
			dataSize														= leftLength;
		}

		//
		// compute crc32 value
		//
		UINT32 checkSum														= BlCrc32(0, panicInfoBuffer, totalInfoLength);

		//
		// get current time
		//
		EFI_TIME nowEfiTime													= {0};
		EfiRuntimeServices->GetTime(&nowEfiTime, nullptr);

		//
		// read AAPL,PanicInfoLog
		//
		PANIC_INFO_LOG panicInfoLog											= {0};
		dataSize															= sizeof(panicInfoLog);
		EFI_STATUS status													= EfiRuntimeServices->GetVariable(CHAR16_STRING(L"AAPL,PanicInfoLog"), &AppleNVRAMVariableGuid, nullptr, &dataSize, &panicInfoLog);
		if(status == EFI_NOT_FOUND)
		{
			memset(&panicInfoLog, 0, sizeof(panicInfoLog));
			panicInfoLog.Signature											= 0x1234;
			panicInfoLog.RebootTime[0]										= nowEfiTime;
			panicInfoLog.WriteIndex											= 0;
			panicInfoLog.CheckSum											= checkSum;
			EfiRuntimeServices->SetVariable(CHAR16_STRING(L"AAPL,PanicInfoLog"), &AppleNVRAMVariableGuid, EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE, sizeof(panicInfoLog), &panicInfoLog);
			try_leave(NOTHING);
		}
		else if(EFI_ERROR(status) || panicInfoLog.Signature != 0x1234)
		{
			EfiRuntimeServices->SetVariable(CHAR16_STRING(L"AAPL,PanicInfoLog"), &AppleNVRAMVariableGuid, EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE, 0, nullptr);
			try_leave(NOTHING);
		}

		//
		// compute max delta time
		//
		UINT32 deltaTime													= 0;
		UINT32 nowTime														= BlEfiTimeToUnixTime(&nowEfiTime);
		for(UINT8 i = 0; i < ARRAYSIZE(panicInfoLog.RebootTime); i ++)
		{
			UINT32 rebootTime												= BlEfiTimeToUnixTime(panicInfoLog.RebootTime + i);
			if(rebootTime >= nowTime)
				continue;

			rebootTime														= nowTime - rebootTime;
			if(rebootTime > deltaTime)
				deltaTime													= rebootTime;
		}

		//
		// update panic info log
		//
		UINT32 oldCheckSum													= panicInfoLog.CheckSum;
		panicInfoLog.CheckSum												= checkSum;
		panicInfoLog.WriteIndex												= (panicInfoLog.WriteIndex + 1) % ARRAYSIZE(panicInfoLog.RebootTime);
		panicInfoLog.RebootTime[panicInfoLog.WriteIndex]					= nowEfiTime;
		if(EFI_ERROR(EfiRuntimeServices->SetVariable(CHAR16_STRING(L"AAPL,PanicInfoLog"), &AppleNVRAMVariableGuid, EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE, sizeof(panicInfoLog), &panicInfoLog)))
			try_leave(NOTHING);

		//
		// check the same
		//
		if(oldCheckSum == checkSum)
			try_leave(retValue = TRUE);

		//
		// count 180 seconds
		//
		if(!deltaTime || deltaTime >= 180)
			try_leave(NOTHING);

		//
		// clear old record
		//
		EfiRuntimeServices->SetVariable(CHAR16_STRING(L"AAPL,PanicInfoLog"), &AppleNVRAMVariableGuid, EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE, 0, nullptr);

		//
		// setup display
		//
		CsConnectDevice(FALSE, FALSE);
		if(!BlTestBootMode(BOOT_MODE_VERBOSE))
		{
			CsInitializeGraphMode();
			CsDrawBootImage(FALSE);
		}
		else
		{
			CsSetConsoleMode(TRUE, FALSE);
		}

		//
		// shutdown
		//
		CsPrintf(CHAR8_CONST_STRING("***************************************************************\n"));
		CsPrintf(CHAR8_CONST_STRING("Panic loop detected (%d in %3d seconds).  System will shutdown.\n"), 5, deltaTime);
		CsPrintf(CHAR8_CONST_STRING("Try booting into the Recovery HD volume by holding Command + R.\n"));
		CsPrintf(CHAR8_CONST_STRING("***************************************************************\n"));
		EfiBootServices->Stall(30 * 1000 * 1000);
		EfiRuntimeServices->ResetSystem(EfiResetShutdown, EFI_SUCCESS, 0, nullptr);
	}
	__finally
	{
		if(panicInfoBuffer)
			MmFreePool(panicInfoBuffer);
	}

	return retValue;
}

//
// show panic dialog
//
VOID BlShowPanicDialog(CHAR8** kernelCommandLine)
{
	__try
	{
		//
		// read AAPL,PanicInfo0000
		//
		UINTN dataSize														= 0;
		BlpPanicInfoName[17]												= L'0';
		if(EfiRuntimeServices->GetVariable(BlpPanicInfoName, &AppleNVRAMVariableGuid, nullptr, &dataSize, nullptr) != EFI_BUFFER_TOO_SMALL || !dataSize)
			try_leave(NOTHING);

		//
		// read Panic flags ?
		//
		UINT8 systemVolume													= 0;
		dataSize															= sizeof(systemVolume);
		if(EFI_ERROR(EfiRuntimeServices->GetVariable(CHAR16_STRING("SystemAudioVolume"), &AppleNVRAMVariableGuid, nullptr, &dataSize, &systemVolume)) || !(systemVolume & 0x80))
			try_leave(NOTHING);

		//
		// get all info length
		//
		UINTN totalLength													= 0;
		UINT8 maxIndex														= 0;
		EFI_STATUS status													= EFI_SUCCESS;
		for(UINT8 i = 0; i < 16; i ++)
		{
			BlpPanicInfoName[17]											= static_cast<CHAR16>(i < 10 ? L'0' + i : L'A' + i - 10);
			dataSize														= 0;
			status															= EfiRuntimeServices->GetVariable(BlpPanicInfoName, &AppleNVRAMVariableGuid, nullptr, &dataSize, nullptr);
			if(status == EFI_BUFFER_TOO_SMALL)
				totalLength													+= dataSize;
			else if(EFI_ERROR(status))
				break;

			maxIndex														= i;
		}

		//
		// detect panic loop
		//
		if(!EFI_ERROR(status) && totalLength && BlpDetectPanicLoop(maxIndex, totalLength))
			try_leave(NOTHING);

		CsPrintf(CHAR8_CONST_STRING("\n"));
		CsPrintf(CHAR8_CONST_STRING("***************************************************************\n"));
		CsPrintf(CHAR8_CONST_STRING("This system was automatically rebooted after panic\n"));
		CsPrintf(CHAR8_CONST_STRING("***************************************************************\n"));

		//
		// show panic image
		//
		if(!BlTestBootMode(BOOT_MODE_VERBOSE))
		{
			//
			// draw image
			//
			CsConnectDevice(FALSE, FALSE);
			CsInitializeGraphMode();
			CsDrawPanicImage();

			//
			// wait mouse/key/timer
			//
			UINTN eventIndex												= 0;
			EFI_EVENT waitEvent[3]											= {nullptr, EfiSystemTable->ConIn->WaitForKey, nullptr};
			if(!EFI_ERROR(EfiBootServices->CreateEvent(EFI_EVENT_TIMER, 0, nullptr, nullptr, waitEvent)))
			{
				EfiBootServices->SetTimer(waitEvent[0], TimerRelative, 100 * 1000 * 1000);

				EFI_SIMPLE_POINTER_PROTOCOL* simplePointerProtocol			= nullptr;
				if(!EFI_ERROR(EfiBootServices->LocateProtocol(&EfiSimplePointerProtocolGuid, nullptr, reinterpret_cast<VOID**>(&simplePointerProtocol))))
				{
					waitEvent[2]											= simplePointerProtocol->WaitForInput;
					while(TRUE)
					{
						if(EFI_ERROR(EfiBootServices->WaitForEvent(3, waitEvent, &eventIndex)) || eventIndex != 2)
							break;

						EFI_SIMPLE_POINTER_STATE state						= {0};
						if(EFI_ERROR(simplePointerProtocol->GetState(simplePointerProtocol, &state)) || state.LeftButton || state.RightButton)
							break;
					}
				}
				else
				{
					EfiBootServices->WaitForEvent(2, waitEvent, &eventIndex);
				}
				EfiBootServices->CloseEvent(waitEvent[0]);
			}

			CsClearScreen();
			EfiBootServices->Stall(1000 * 1000);
		}
		else
		{
			CsSetConsoleMode(TRUE, FALSE);
			EfiBootServices->Stall(5 * 1000 * 1000);
		}

		//
		// reinitialize kernel command line
		//
		BlDetectHotKey();
		if(*kernelCommandLine)
		{
			CHAR8* oldCommandLine											= *kernelCommandLine;
			*kernelCommandLine												= BlSetupKernelCommandLine(oldCommandLine, nullptr, nullptr);
			MmFreePool(oldCommandLine);
		}
	}
	__finally
	{

	}
}
