//********************************************************************
//	created:	4:11:2009   10:04
//	filename: 	boot.cpp
//	author:		tiamo
//	purpose:	main
//********************************************************************

#include "StdAfx.h"
#include "DebugUsb.h"

//
// read debug options
//
STATIC EFI_STATUS BlpReadDebugOptions(CHAR8** debugOptions)
{
	*debugOptions															= nullptr;
	UINTN variableSize														= 0;
	EFI_STATUS status														= EfiRuntimeServices->GetVariable(CHAR16_STRING(L"boot-windbg-args"), &AppleNVRAMVariableGuid, nullptr, &variableSize, nullptr);
	if(status != EFI_BUFFER_TOO_SMALL)
		return status;

	CHAR8* variableBuffer													= static_cast<CHAR8*>(MmAllocatePool(variableSize + sizeof(CHAR8)));
	if(!variableBuffer)
		return EFI_OUT_OF_RESOURCES;

	variableBuffer[variableSize]											= 0;
	status																	= EfiRuntimeServices->GetVariable(CHAR16_STRING(L"boot-windbg-args"), &AppleNVRAMVariableGuid, nullptr, &variableSize, variableBuffer);
	if(!EFI_ERROR(status))
		*debugOptions														= variableBuffer;
	else
		MmFreePool(variableBuffer);

	return status;
}

//
// setup rom variable
//
STATIC EFI_STATUS BlpSetupRomVariable()
{
	__try
	{
		//
		// ROM = [0xffffff01, 0xffffff07)
		//
		UINT32 attribute													= EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS;
		UINT8 romBuffer[6]													= {0};
		UINTN dataSize														= sizeof(romBuffer);
		if(EFI_ERROR(EfiRuntimeServices->GetVariable(CHAR16_STRING(L"ROM"), &AppleFirmwareVariableGuid, nullptr, &dataSize, romBuffer)))
			EfiRuntimeServices->SetVariable(CHAR16_STRING(L"ROM"), &AppleFirmwareVariableGuid, attribute, sizeof(romBuffer), ArchConvertAddressToPointer(0xffffff01, VOID*));

		//
		// check MLB
		//
		UINT8 mlbBuffer[0x80]												= {0};
		dataSize															= sizeof(mlbBuffer);
		if(!EFI_ERROR(EfiRuntimeServices->GetVariable(CHAR16_STRING(L"MLB"), &AppleFirmwareVariableGuid, nullptr, &dataSize, mlbBuffer)))
			try_leave(NOTHING);

		//
		// search [0xffffff08, 0xffffff50)
		//
		UINT8 tempBuffer[0x48];
		memset(tempBuffer, 0xff, sizeof(tempBuffer));
		EfiBootServices->CopyMem(tempBuffer, ArchConvertAddressToPointer(0xffffff08, VOID*), sizeof(tempBuffer));
		for(UINTN i = 0; i < 4; i ++)
		{
			if(tempBuffer[i * 0x12] == 0xff)
			{
				if(i)
				{
					dataSize												= 0;
					UINT8* mlbBuffer										= tempBuffer + i * 0x12 - 0x12;
					while(mlbBuffer[dataSize] != ' ')
						dataSize											+= 1;

					EfiRuntimeServices->SetVariable(CHAR16_STRING(L"MLB"), &AppleFirmwareVariableGuid, attribute, dataSize, mlbBuffer);
				}
				break;
			}
		}
	}
	__finally
	{

	}

	return EFI_SUCCESS;
}

//
// check board id
//
STATIC EFI_STATUS BlpCheckBoardId(CHAR8 CONST* boardId, EFI_DEVICE_PATH_PROTOCOL* bootFilePath)
{
	EFI_STATUS status														= EFI_SUCCESS;
	EFI_DEVICE_PATH_PROTOCOL* fileDevPath									= nullptr;
	CHAR8* fileName															= nullptr;
	CHAR8* fileBuffer														= nullptr;
	XML_TAG* rootTag														= nullptr;

	__try
	{
		//
		// VMM ok
		//
		if(!strnicmp(boardId, CHAR8_CONST_STRING("VMM"), 3))
			try_leave(NOTHING);

		//
		// check current directory
		//
		fileDevPath															= DevPathAppendLastComponent(bootFilePath, CHAR8_CONST_STRING("PlatformSupport.plist"), TRUE);
		if(fileDevPath)
		{
			fileName														= DevPathExtractFilePathName(fileDevPath, TRUE);
			if(fileName)
				status														= IoReadWholeFile(bootFilePath, fileName, &fileBuffer, nullptr, TRUE);
			else
				status														= EFI_NOT_FOUND;
		}
		else
		{
			//
			// check root directory
			//
			status															= IoReadWholeFile(bootFilePath, CHAR8_CONST_STRING("PlatformSupport.plist"), &fileBuffer, nullptr, TRUE);
		}

		//
		// check default file
		//
		if(EFI_ERROR(status) && EFI_ERROR(status = IoReadWholeFile(bootFilePath, CHAR8_CONST_STRING("System\\Library\\CoreServices\\PlatformSupport.plist"), &fileBuffer, nullptr, TRUE)))
			try_leave(status = EFI_SUCCESS);

		//
		// parse the file
		//
		if(EFI_ERROR(status = CmParseXmlFile(fileBuffer, &rootTag)))
			try_leave(NOTHING);

		//
		// get tag value
		//
		XML_TAG* supportedIds												= CmGetTagValueForKey(rootTag, CHAR8_CONST_STRING("SupportedBoardIds"));
		UINTN count															= CmGetListTagElementsCount(supportedIds);
		for(UINTN i = 0; i < count; i ++)
		{
			XML_TAG* supportedId											= CmGetListTagElementByIndex(supportedIds, i);
			if(!supportedId || supportedId->Type != XML_TAG_STRING)
				continue;

			if(!strcmp(supportedId->StringValue, boardId))
				try_leave(NOTHING);
		}
		status																= EFI_UNSUPPORTED;
	}
	__finally
	{
		if(fileDevPath)
			MmFreePool(fileDevPath);
		if(fileName)
			MmFreePool(fileName);
		if(fileBuffer)
			MmFreePool(fileBuffer);
		if(rootTag)
			CmFreeTag(rootTag);
	}

	return status;
}

//
// run recovery booter
//
STATIC EFI_STATUS BlpRunRecoveryEfi(EFI_DEVICE_PATH_PROTOCOL* bootDevicePath, EFI_DEVICE_PATH_PROTOCOL* bootFilePath)
{
	EFI_STATUS status														= EFI_SUCCESS;
	EFI_DEVICE_PATH_PROTOCOL* recoveryFilePath								= nullptr;

	__try
	{
		//
		// get current partition number
		//
		UINT32 partitionNumber												= DevPathGetPartitionNumber(bootDevicePath);
		if(partitionNumber == -1)
			try_leave(status = EFI_NOT_FOUND);

		//
		// root partition is followed by recovery partition
		//
		if(!BlTestBootMode(BOOT_MODE_BOOT_IS_NOT_ROOT))
			partitionNumber													+= 1;

		//
		// get recovery partition handle
		//
		EFI_HANDLE recoveryPartitionHandle									= DevPathGetPartitionHandleByNumber(bootDevicePath, partitionNumber);
		if(!recoveryPartitionHandle)
			try_leave(status = EFI_NOT_FOUND);

		//
		// get recovery partition device path
		//
		EFI_DEVICE_PATH_PROTOCOL* recoveryPartitionDevicePath				= DevPathGetDevicePathProtocol(recoveryPartitionHandle);
		if(!recoveryPartitionDevicePath)
			try_leave(status = EFI_NOT_FOUND);

		//
		// get recovery file path
		//
		recoveryFilePath													= DevPathAppendFilePath(recoveryPartitionDevicePath, CHAR16_CONST_STRING(L"\\com.apple.recovery.boot\\boot.efi"));
		if(!recoveryFilePath)
			try_leave(status = EFI_NOT_FOUND);

		//
		// load image
		//
		EFI_HANDLE imageHandle												= nullptr;
		if(EFI_ERROR(status = EfiBootServices->LoadImage(FALSE, EfiImageHandle, recoveryFilePath, nullptr, 0, &imageHandle)))
		{
			if(!BlTestBootMode(BOOT_MODE_BOOT_IS_NOT_ROOT))
				try_leave(NOTHING);

			//
			// get root UUID
			//
			CHAR8 CONST* rootUUID											= CmGetStringValueForKey(nullptr, CHAR8_CONST_STRING("Root UUID"), nullptr);
			if(!rootUUID)
				try_leave(status = EFI_NOT_FOUND);

			//
			// load booter
			//
			if(EFI_ERROR(status = IoLoadBooterWithRootUUID(bootFilePath, rootUUID, &imageHandle)))
				try_leave(NOTHING);
		}

		//
		// start it
		//
		status																= EfiBootServices->StartImage(imageHandle, nullptr, nullptr);
	}
	__finally
	{
		if(recoveryFilePath)
			MmFreePool(recoveryFilePath);
	}

	return status;
}

//
// run apple boot
//
STATIC EFI_STATUS BlpRunAppleBoot(CHAR8 CONST* bootFileName)
{
	EFI_STATUS status														= EFI_SUCCESS;
	EFI_HANDLE* handleArray													= nullptr;
	CHAR16* fileName														= nullptr;

	__try
	{
		//
		// convert file name
		//
		fileName															= BlAllocateUnicodeFromUtf8(bootFileName, strlen(bootFileName));
		if(!fileName)
			try_leave(status = EFI_OUT_OF_RESOURCES);

		//
		// locate file system protocol
		//
		UINTN totalHandles													= 0;
		if(EFI_ERROR(status = EfiBootServices->LocateHandleBuffer(ByProtocol, &EfiSimpleFileSystemProtocolGuid, nullptr, &totalHandles, &handleArray)))
			try_leave(NOTHING);

		for(UINTN i = 0; i < totalHandles; i ++)
		{
			EFI_HANDLE theHandle											= handleArray[i];
			if(!theHandle)
				continue;

			//
			// get file system protocol
			//
			EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* fileSystemProtocol				= nullptr;
			if(EFI_ERROR(EfiBootServices->HandleProtocol(theHandle, &EfiSimpleFileSystemProtocolGuid, reinterpret_cast<VOID**>(&fileSystemProtocol))))
				continue;

			//
			// open root directory
			//
			EFI_FILE_HANDLE rootFile										= nullptr;
			if(EFI_ERROR(fileSystemProtocol->OpenVolume(fileSystemProtocol, &rootFile)))
				continue;

			//
			// open boot.efi
			//
			EFI_FILE_HANDLE bootFile										= nullptr;
			EFI_STATUS openStatus											= rootFile->Open(rootFile, &bootFile, fileName, EFI_FILE_MODE_READ, EFI_FILE_READ_ONLY);
			rootFile->Close(rootFile);
			if(EFI_ERROR(openStatus))
				continue;

			//
			// close it
			//
			bootFile->Close(bootFile);

			//
			// get device path
			//
			EFI_DEVICE_PATH_PROTOCOL* rootDevicePath						= nullptr;
			if(EFI_ERROR(status = EfiBootServices->HandleProtocol(theHandle, &EfiDevicePathProtocolGuid, reinterpret_cast<VOID**>(&rootDevicePath))))
				try_leave(NOTHING);

			//
			// load it
			//
			EFI_DEVICE_PATH_PROTOCOL* bootFilePath							= DevPathAppendFilePath(rootDevicePath, fileName);
			EFI_HANDLE imageHandle											= nullptr;
			if(EFI_ERROR(status = EfiBootServices->LoadImage(TRUE, EfiImageHandle, bootFilePath, nullptr, 0, &imageHandle)))
				try_leave(NOTHING);

			//
			// free file path
			//
			MmFreePool(bootFilePath);

			//
			// get loaded image protocol
			//
			EFI_LOADED_IMAGE_PROTOCOL* loadedImage							= nullptr;
			if(EFI_ERROR(status = EfiBootServices->HandleProtocol(imageHandle, &EfiLoadedImageProtocolGuid, reinterpret_cast<VOID**>(&loadedImage))))
				try_leave(EfiBootServices->UnloadImage(imageHandle));

			//
			// run it
			//
			UINTN exitDataSize												= 0;
			try_leave(status = EfiBootServices->StartImage(imageHandle, &exitDataSize, nullptr));
		}
	}
	__finally
	{
		if(fileName)
			MmFreePool(fileName);
	}

	return status;
}

//
// main
//
EFI_STATUS EFIAPI EfiMain(EFI_HANDLE imageHandle, EFI_SYSTEM_TABLE* systemTable)
{
	EFI_STATUS status														= EFI_SUCCESS;

	__try
	{
		//
		// save handle
		//
		EfiImageHandle														= imageHandle;
		EfiSystemTable														= systemTable;
		EfiBootServices														= systemTable->BootServices;
		EfiRuntimeServices													= systemTable->RuntimeServices;

		//
		// stop watchdog timer
		//
		EfiBootServices->SetWatchdogTimer(0, 0, 0, nullptr);

		//
		// memory initialize
		//
		if(EFI_ERROR(status = MmInitialize()))
			try_leave(NOTHING);

		//
		// init arch phase 0
		//
		if(EFI_ERROR(status = ArchInitialize0()))
			try_leave(NOTHING);

		//
		// get debug options
		//
		CHAR8* debugOptions													= nullptr;
		BlpReadDebugOptions(&debugOptions);

		//
		// init boot debugger
		//
		//debugOptions														= CHAR8_STRING("/debug=1394 /channel=12 /break /connectall /runapple=/System/Library/CoreServices/boot.apple");
		//debugOptions														= CHAR8_STRING("/debug=1394 /channel=12 /break /connectall /connectwait=5");
		if(EFI_ERROR(status = BdInitialize(debugOptions)))
			try_leave(NOTHING);

		//
		// run apple's boot.efi
		//
		CHAR8 CONST* appleBootFileName										= debugOptions ? strstr(debugOptions, CHAR8_CONST_STRING("/runapple=")) : nullptr;
		if(appleBootFileName)
			try_leave(status = BlpRunAppleBoot(appleBootFileName + 10));

		//
		// init arch parse 1
		//
		if(EFI_ERROR(status = ArchInitialize1()))
			try_leave(NOTHING);

		//
		// initialize console
		//
		if(EFI_ERROR(status = CsInitialize()))
			try_leave(NOTHING);

		//
		// fix rom variable
		//
		if(EFI_ERROR(status = BlpSetupRomVariable()))
			try_leave(NOTHING);

		//
		// initialize device tree
		//
		if(EFI_ERROR(status = DevTreeInitialize()))
			try_leave(NOTHING);

		//
		// init platform expert
		//
		if(EFI_ERROR(status = PeInitialize()))
			try_leave(NOTHING);

		//
		// detect memory size
		//
		if(EFI_ERROR(status = BlDetectMemorySize()))
			try_leave(NOTHING);

		//
		// check hibernate
		//
		UINT8 coreStorageVolumeKeyIdent[16]									= {0};
		BOOLEAN resumeFromCoreStorage										= HbStartResumeFromHibernate(coreStorageVolumeKeyIdent);
		if(resumeFromCoreStorage)
			BlSetBootMode(BOOT_MODE_HIBER_FROM_FV, 0);

		//
		// enable ASLR
		//
		LdrSetupASLR(TRUE, 0);

		//
		// detect hot key
		//
		if(EFI_ERROR(status = BlDetectHotKey()))
			try_leave(NOTHING);

		//
		// get loaded image protocol
		//
		EFI_LOADED_IMAGE_PROTOCOL* loadedBootImage							= nullptr;
		if(EFI_ERROR(status = EfiBootServices->HandleProtocol(EfiImageHandle, &EfiLoadedImageProtocolGuid, reinterpret_cast<VOID**>(&loadedBootImage))))
			try_leave(NOTHING);

		//
		// allocate buffer
		//
		UINTN loaderOptionsSize												= (loadedBootImage->LoadOptionsSize / sizeof(CHAR16) + 1) * sizeof(CHAR8);
		CHAR8* loaderOptions												= static_cast<CHAR8*>(MmAllocatePool(loaderOptionsSize));
		if(!loaderOptions)
			try_leave(status = EFI_OUT_OF_RESOURCES);

		//
		// unicode -> utf8
		//
		if(EFI_ERROR(status = BlUnicodeToUtf8(static_cast<CHAR16*>(loadedBootImage->LoadOptions), loadedBootImage->LoadOptionsSize / sizeof(CHAR16), loaderOptions, loaderOptionsSize / sizeof(CHAR8))))
			try_leave(NOTHING);

		//
		// detect root device
		//
		EFI_HANDLE bootDeviceHandle											= loadedBootImage->DeviceHandle;
		EFI_DEVICE_PATH_PROTOCOL* bootFilePath								= loadedBootImage->FilePath;
		if(EFI_ERROR(status = IoDetectRoot(&bootDeviceHandle, &bootFilePath, debugOptions && strstr(debugOptions, CHAR8_CONST_STRING("/detectboot")))))
			try_leave(NOTHING);

		//
		// get boot device path
		//
		EFI_DEVICE_PATH_PROTOCOL* bootDevicePath							= DevPathGetDevicePathProtocol(bootDeviceHandle);
		if(!bootDevicePath)
			try_leave(status = EFI_DEVICE_ERROR);

		//
		// process option
		//
		CHAR8* kernelCommandLine											= nullptr;
		if(EFI_ERROR(status = BlProcessOptions(loaderOptions, &kernelCommandLine, bootDevicePath, bootFilePath)))
			try_leave(NOTHING);

		//
		// check 64-bit cpu
		//
		if(EFI_ERROR(status = ArchCheck64BitCpu()))
			try_leave(NOTHING);

		//
		// compact check
		//
		if(!BlTestBootMode(BOOT_MODE_SKIP_BOARD_ID_CHECK) && EFI_ERROR(status = BlpCheckBoardId(BlGetBoardId(), bootFilePath)))
			try_leave(NOTHING);

		//
		// check recovery
		//
		CHAR8* filePath														= DevPathExtractFilePathName(bootFilePath, TRUE);
		if(filePath)
		{
			if(strstr(filePath, CHAR8_CONST_STRING("com.apple.recovery.boot")))
				BlSetBootMode(BOOT_MODE_FROM_RECOVER_BOOT_DIRECTORY, BOOT_MODE_EFI_NVRAM_RECOVERY_BOOT_MODE | BOOT_MODE_BOOT_IS_NOT_ROOT);

			MmFreePool(filePath);
		}

		//
		// show panic dialog
		//
		if(!BlTestBootMode(BOOT_MODE_SKIP_PANIC_DIALOG | BOOT_MODE_FROM_RECOVER_BOOT_DIRECTORY | BOOT_MODE_HIBER_FROM_FV | BOOT_MODE_SAFE))
			BlShowPanicDialog(&kernelCommandLine);

		//
		// run recovery.efi
		//
		if(BlTestBootMode(BOOT_MODE_EFI_NVRAM_RECOVERY_BOOT_MODE))
			BlpRunRecoveryEfi(bootDevicePath, bootFilePath);

		//
		// check FileVault2
		//
		if(BlTestBootMode(BOOT_MODE_BOOT_IS_NOT_ROOT))
			FvLookupUnlockCoreVolumeKey(bootDevicePath, resumeFromCoreStorage);

		//
		// restore gragh config
		//
		if(!BlTestBootMode(BOOT_MODE_HAS_FILE_VAULT2_CONFIG))
			CsConnectDevice(FALSE, FALSE);

		//
		// setup console mode
		//
		if(BlTestBootMode(BOOT_MODE_VERBOSE))
		{
			CsSetConsoleMode(TRUE, FALSE);
		}
		else
		{
			if(!EFI_ERROR(CsInitializeGraphMode()))
				CsDrawBootImage(TRUE);
		}

		//
		// continue hibernate
		//
		if(resumeFromCoreStorage)
		{
			UINT8 coreStorageVolumeKey[16]									= {0};
			if(FvFindCoreVolumeKey(coreStorageVolumeKeyIdent, coreStorageVolumeKey, sizeof(coreStorageVolumeKey)))
				HbContinueResumeFromHibernate(coreStorageVolumeKey, sizeof(coreStorageVolumeKey));
		}

		//
		// load kernel cache
		//
		MACH_O_LOADED_INFO kernelInfo										= {0};
		status																= LdrLoadKernelCache(&kernelInfo, bootDevicePath);
		BOOLEAN usingKernelCache											= !EFI_ERROR(status);
		if(!usingKernelCache && LdrGetKernelCacheOverride())
			try_leave(NOTHING);

		//
		// load kernel
		//
		if(!usingKernelCache && EFI_ERROR(status = LdrLoadKernel(&kernelInfo)))
			try_leave(NOTHING);

		//
		// initialize boot args
		//
		BOOT_ARGS* bootArgs													= nullptr;
		if(EFI_ERROR(status = BlInitializeBootArgs(bootDevicePath, bootFilePath, bootDeviceHandle, kernelCommandLine, &bootArgs)))
			try_leave(NOTHING);

		//
		// load driver
		//
		if(!usingKernelCache && EFI_ERROR(status = LdrLoadDrivers()))
			try_leave(NOTHING);

		//
		// load ramdisk
		//
		LdrLoadRamDisk();

		//
		// console finalize
		//
		CsFinalize();

		//
		// finish boot args
		//
		if(EFI_ERROR(status = BlFinalizeBootArgs(bootArgs, kernelCommandLine, bootDeviceHandle, &kernelInfo)))
			try_leave(NOTHING);

		//
		// stop debugger
		//
		BdFinalize();

		//
		// start kernel
		//
		ArchStartKernel(ArchConvertAddressToPointer(kernelInfo.EntryPointPhysicalAddress, VOID*), bootArgs);
	}
	__finally
	{
	}

	return status;
}