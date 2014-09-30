//********************************************************************
//	created:	8:11:2009   18:33
//	filename: 	Options.cpp
//	author:		tiamo
//	purpose:	process option
//********************************************************************

#include "stdafx.h"

//
// global
//
STATIC UINT32 BlpBootMode													= BOOT_MODE_NORMAL | BOOT_MODE_VERBOSE | BOOT_MODE_SKIP_BOARD_ID_CHECK;
STATIC UINT32 BlpForceCpuArchType											= CPU_ARCH_NONE;
STATIC BOOLEAN BlpPasswordUIEfiRun											= FALSE;

//
// extract options
//
STATIC CHAR8 CONST* BlpExtractOptions(CHAR8 CONST* commandLine)
{
	while(isspace(*commandLine))
		commandLine															+= 1;

	CHAR8 CONST* retValue													= commandLine;
	CHAR8 c																	= *commandLine;

	if((c < 'a' || c > 'z') && c != '/' && c != '\\' && (c < 'A' || c > 'Z'))
		return retValue;

	while(*commandLine && *commandLine != '=' && !isspace(*commandLine))
		commandLine															+= 1;

	if(*commandLine == '=')
		return retValue;

	while(isspace(*commandLine))
		commandLine															+= 1;

	return commandLine;
}

//
// check temporary boot
//
STATIC BOOLEAN BlpIsTemporaryBoot()
{
	UINT32 attribute														= 0;
	UINT32 value															= 0;
	UINTN dataSize															= sizeof(value);
	if(!EFI_ERROR(EfiRuntimeServices->GetVariable(CHAR16_STRING(L"PickerEntryReason"), &AppleFirmwareVariableGuid, &attribute, &dataSize, &value)) && !(attribute & EFI_VARIABLE_NON_VOLATILE))
		return TRUE;

	attribute																= 0;
	dataSize																= sizeof(value);
	if(!EFI_ERROR(EfiRuntimeServices->GetVariable(CHAR16_STRING(L"BootCurrent"), &AppleFirmwareVariableGuid, &attribute, &dataSize, &value)) && !(attribute & EFI_VARIABLE_NON_VOLATILE) && !value)
		return TRUE;

	return FALSE;
}

//
// run PasswordUI.efi
//
STATIC EFI_STATUS BlpRunPasswordUIEfi()
{
	EFI_STATUS status														= EFI_SUCCESS;
	UINTN handleCount														= 0;
	EFI_HANDLE* handleArray													= nullptr;

	__try
	{
		if(!BlTestBootMode(BOOT_MODE_EFI_NVRAM_RECOVERY_BOOT_MODE) || BlpIsTemporaryBoot() || BlpPasswordUIEfiRun)
			try_leave(NOTHING);

		CsConnectDevice(TRUE, FALSE);
		EfiBootServices->LocateHandleBuffer(ByProtocol, &EfiFirmwareVolumeProtocolGuid, nullptr, &handleCount, &handleArray);
		for(UINTN i = 0; i < handleCount; i ++)
		{
			EFI_HANDLE theHandle											= handleArray[i];
			EFI_FIRMWARE_VOLUME_PROTOCOL* firwareVolumeProtocol				= nullptr;
			if(EFI_ERROR(EfiBootServices->HandleProtocol(theHandle, &EfiFirmwareVolumeDispatchProtocolGuid, reinterpret_cast<VOID**>(&firwareVolumeProtocol))))
				continue;
			if(EFI_ERROR(EfiBootServices->HandleProtocol(theHandle, &EfiFirmwareVolumeProtocolGuid, reinterpret_cast<VOID**>(&firwareVolumeProtocol))))
				continue;

			UINTN fileSize													= 0;
			EFI_FV_FILETYPE fileType										= 0;
			EFI_FV_FILE_ATTRIBUTES fileAttribute							= 0;
			UINT32 authStatus												= 0;
			if(EFI_ERROR(firwareVolumeProtocol->ReadFile(firwareVolumeProtocol, &ApplePasswordUIEfiFileNameGuid, nullptr, &fileSize, &fileType, &fileAttribute, &authStatus)))
				continue;

			EFI_DEVICE_PATH_PROTOCOL* devPath								= DevPathGetDevicePathProtocol(theHandle);
			if(devPath)
			{
				UINT8 buffer[sizeof(MEDIA_FW_VOL_FILEPATH_DEVICE_PATH) + END_DEVICE_PATH_LENGTH];
				MEDIA_FW_VOL_FILEPATH_DEVICE_PATH* fvFileDevPathNode		= reinterpret_cast<MEDIA_FW_VOL_FILEPATH_DEVICE_PATH*>(buffer);
				fvFileDevPathNode->Header.Type								= MEDIA_DEVICE_PATH;
				fvFileDevPathNode->Header.SubType							= MEDIA_FV_FILEPATH_DP;
				memcpy(&fvFileDevPathNode->NameGuid, &ApplePasswordUIEfiFileNameGuid, sizeof(ApplePasswordUIEfiFileNameGuid));
				SetDevicePathNodeLength(&fvFileDevPathNode->Header, sizeof(MEDIA_FW_VOL_FILEPATH_DEVICE_PATH));
				EFI_DEVICE_PATH_PROTOCOL* endOfPath							= NextDevicePathNode(&fvFileDevPathNode->Header);
				SetDevicePathEndNode(endOfPath);
				EFI_DEVICE_PATH_PROTOCOL* fileDevPath						= DevPathAppendDevicePath(devPath, &fvFileDevPathNode->Header);

				EFI_HANDLE imageHandle										= nullptr;
				if(!EFI_ERROR(EfiBootServices->LoadImage(FALSE, EfiImageHandle, fileDevPath, nullptr, 0, &imageHandle)))
				{
					if(!EFI_ERROR(EfiBootServices->StartImage(imageHandle, nullptr, nullptr)))
						try_leave(BlpPasswordUIEfiRun = TRUE);
				}
			}
			break;
		}
		BlSetBootMode(0, BOOT_MODE_EFI_NVRAM_RECOVERY_BOOT_MODE);
	}
	__finally
	{
		if(handleArray)
			MmFreePool(handleArray);
	}

	return status;
}

//
// read kernel flags
//
STATIC EFI_STATUS BlpReadKernelFlags(EFI_DEVICE_PATH_PROTOCOL* bootFilePath, CHAR8 CONST* fileName, CHAR8 CONST** kernelFlags)
{
	EFI_STATUS status														= EFI_SUCCESS;
	CHAR8* fileBuffer														= nullptr;

	__try
	{
		//
		// read config file
		//
		if(EFI_ERROR(status	= IoReadWholeFile(bootFilePath, fileName, &fileBuffer, nullptr, TRUE)))
			try_leave(NOTHING);

		//
		// parse file
		//
		if(EFI_ERROR(status = CmParseXmlFile(fileBuffer, nullptr)))
			try_leave(NOTHING);

		*kernelFlags														= CmGetStringValueForKey(nullptr, CHAR8_CONST_STRING("Kernel Flags"), nullptr);
	}
	__finally
	{
		if(fileBuffer)
			MmFreePool(fileBuffer);
	}

	return status;
}

//
// load config file
//
STATIC CHAR8 CONST* BlpLoadConfigFile(CHAR8 CONST* bootOptions, EFI_DEVICE_PATH_PROTOCOL* bootFilePath)
{
	CHAR8 CONST* retValue													= nullptr;
	EFI_DEVICE_PATH_PROTOCOL* bootPlistDevPath								= nullptr;
	CHAR8* bootPlistPathName												= nullptr;

	__try
	{
		//
		// try com.apple.Boot.plist in current directory
		//
		bootPlistDevPath													= DevPathAppendLastComponent(bootFilePath, CHAR8_CONST_STRING("com.apple.Boot.plist"), TRUE);
		if(bootPlistDevPath)
		{
			bootPlistPathName												= DevPathExtractFilePathName(bootPlistDevPath, TRUE);
			if(bootPlistPathName && !EFI_ERROR(BlpReadKernelFlags(bootFilePath, bootPlistPathName, &retValue)))
				try_leave(NOTHING);
		}

		//
		// read config from command line
		//
		STATIC CHAR8 fileName[1024]											= {0};
		UINTN valueLength													= 0;
		CHAR8 CONST* configFileName											= CmGetStringValueForKeyAndCommandLine(bootOptions, CHAR8_CONST_STRING("config"), &valueLength, FALSE);
		if(!configFileName)
		{
			//
			// then default file
			//
			configFileName													= CHAR8_CONST_STRING("com.apple.Boot");
			valueLength														= 14;
		}

		//
		// build full path name
		//
		if(!IoBootingFromNet() && configFileName[0] != '/' && configFileName[0] != '\\')
			strcpy(fileName, CHAR8_CONST_STRING("Library\\Preferences\\SystemConfiguration\\"));

		UINTN length														= strlen(fileName);
		memcpy(fileName + length, configFileName, valueLength);
		fileName[length + valueLength]										= 0;
		strcat(fileName, CHAR8_CONST_STRING(".plist"));
		BlpReadKernelFlags(bootFilePath, fileName, &retValue);
	}
	__finally
	{
		if(bootPlistPathName)
			MmFreePool(bootPlistPathName);
		if(bootPlistDevPath)
			MmFreePool(bootPlistDevPath);
	}

	return retValue;
}

//
// read device path variable
//
STATIC EFI_DEVICE_PATH_PROTOCOL* BlpReadDevicePathVariable(CHAR16 CONST* variableName, BOOLEAN macAddressNode)
{
	EFI_DEVICE_PATH_PROTOCOL* devicePath									= nullptr;
	EFI_DEVICE_PATH_PROTOCOL* retValue										= nullptr;

	__try
	{
		UINTN variableLength												= 0;
		if(EfiRuntimeServices->GetVariable(const_cast<CHAR16*>(variableName), &AppleNVRAMVariableGuid, nullptr, &variableLength, nullptr) != EFI_BUFFER_TOO_SMALL)
			try_leave(NOTHING);

		devicePath															= static_cast<EFI_DEVICE_PATH_PROTOCOL*>(MmAllocatePool(variableLength));
		if(!devicePath)
			try_leave(NOTHING);

		if(EFI_ERROR(EfiRuntimeServices->GetVariable(const_cast<CHAR16*>(variableName), &AppleNVRAMVariableGuid, nullptr, &variableLength, devicePath)))
			try_leave(NOTHING);

		if(macAddressNode != DevPathHasMacAddressNode(devicePath))
			try_leave(NOTHING);

		retValue															= devicePath;
		devicePath															= nullptr;
	}
	__finally
	{
		if(devicePath)
			MmFreePool(devicePath);
	}

	return retValue;
}

//
// setup path from variable
//
STATIC VOID BlpSetupPathFromVariable(EFI_DEVICE_PATH_PROTOCOL** filePath, CHAR8** pathName, CHAR16 CONST* variableName)
{
	EFI_DEVICE_PATH_PROTOCOL* devicePath									= BlpReadDevicePathVariable(variableName, BlTestBootMode(BOOT_MODE_NET) ? TRUE : FALSE);
	*filePath																= devicePath;
	if(devicePath && pathName)
		*pathName															= DevPathExtractFilePathName(devicePath, FALSE);
}

//
// setup path from command line
//
STATIC EFI_STATUS BlpSetupPathFromCommandLine(EFI_DEVICE_PATH_PROTOCOL** filePath, CHAR8** pathName, CHAR8 CONST* kernelCommandLine, CHAR8 CONST* keyName, EFI_DEVICE_PATH_PROTOCOL* bootFilePath, BOOLEAN* keyFound)
{
	UINTN valueLength														= 0;
	CHAR8 CONST* theValue													= CmGetStringValueForKeyAndCommandLine(kernelCommandLine, keyName, &valueLength, TRUE);
	if(theValue && valueLength)
	{
		//
		// free prev one
		//
		if(*pathName)
			MmFreePool(*pathName);

		//
		// allocate and copy path name
		//
		*pathName															= static_cast<CHAR8*>(MmAllocatePool(valueLength + 1));
		if(!*pathName)
			return EFI_OUT_OF_RESOURCES;

		memcpy(*pathName, theValue, valueLength);
		(*pathName)[valueLength]											= 0;
		BlConvertPathSeparator(*pathName, '/', '\\');

		//
		// build device path
		//
		if(IoBootingFromNet() && filePath)
		{
			if(*filePath)
				MmFreePool(*filePath);

			*filePath														= DevPathAppendLastComponent(bootFilePath, *pathName, TRUE);
			if(!*filePath)
				return EFI_OUT_OF_RESOURCES;
		}

		if(keyFound)
			*keyFound														= TRUE;
	}
	else if(keyFound)
	{
		*keyFound															= FALSE;
	}
	return EFI_SUCCESS;
}

//
// get key and value
//
STATIC CHAR8 CONST* BlpGetKeyAndValue(CHAR8 CONST* inputBuffer, CHAR8 CONST** keyBuffer, UINTN* totalLength)
{
	*totalLength															= 0;
	while(isspace(*inputBuffer))
		inputBuffer															+= 1;

	*keyBuffer																= inputBuffer;
	while(*inputBuffer && !isspace(*inputBuffer))
	{
		inputBuffer															+= 1;
		*totalLength														+= 1;
	}
	return inputBuffer;
}

//
// copy args
//
STATIC VOID BlpCopyArgs(CHAR8* dstBuffer, CHAR8 CONST* srcBuffer)
{
	while(srcBuffer && *srcBuffer)
	{
		CHAR8 CONST* keyBuffer												= nullptr;
		UINTN totalLength													= 0;
		srcBuffer															= BlpGetKeyAndValue(srcBuffer, &keyBuffer, &totalLength);
		if(!totalLength)
			continue;

		if(CmGetStringValueForKeyAndCommandLine(dstBuffer, keyBuffer, nullptr, FALSE))
			continue;

		strcat(dstBuffer, CHAR8_CONST_STRING(" "));
		UINTN dstLength														= strlen(dstBuffer);
		memcpy(dstBuffer + dstLength, keyBuffer, totalLength);
		dstBuffer[dstLength + totalLength]									= 0;
	}
}

//
// setup kernel command line
//
CHAR8* BlSetupKernelCommandLine(CHAR8 CONST* bootOptions, CHAR8 CONST* bootArgsVariable, CHAR8 CONST* kernelFlags)
{
	UINTN totalLength														= 0x80;
	totalLength																+= bootOptions ? strlen(bootOptions) : 0;
	totalLength																+= bootArgsVariable ? strlen(bootArgsVariable) : 0;
	totalLength																+= kernelFlags ? strlen(kernelFlags) : 0;

	//
	// allocate kernel command line
	//
	CHAR8* retValue															= static_cast<CHAR8*>(MmAllocatePool(totalLength));
	if(retValue)
	{
		retValue[0]															= 0;
		BlpCopyArgs(retValue, bootOptions);
		BlpCopyArgs(retValue, bootArgsVariable);
		BlpCopyArgs(retValue, kernelFlags);

		if(BlTestBootMode(BOOT_MODE_SAFE))
			BlpCopyArgs(retValue, CHAR8_CONST_STRING("-x"));
		if(BlTestBootMode(BOOT_MODE_SINGLE_USER))
			BlpCopyArgs(retValue, CHAR8_CONST_STRING("-s"));
		if(BlTestBootMode(BOOT_MODE_VERBOSE))
			BlpCopyArgs(retValue, CHAR8_CONST_STRING("-v"));
		if(BlTestBootMode(BOOT_MODE_NET))
			BlpCopyArgs(retValue, CHAR8_CONST_STRING("srv=1"));
	}
	return retValue;
}

//
// detect hot key
//
EFI_STATUS BlDetectHotKey()
{
	EFI_STATUS status														= EFI_SUCCESS;

	__try
	{
		//
		// locate firmware password protocol
		//
		APPLE_FIRMWARE_PASSWORD_PROTOCOL* fwPwdProtocol						= nullptr;
		if(!EFI_ERROR(EfiBootServices->LocateProtocol(&AppleFirmwarePasswordProtocolGuid, nullptr, reinterpret_cast<VOID**>(&fwPwdProtocol))))
		{
			UINTN checkResult												= FALSE;
			fwPwdProtocol->Check(fwPwdProtocol, &checkResult);
			if(checkResult)
				BlSetBootMode(BOOT_MODE_FIRMWARE_PASSWORD, 0);
		}

		//
		// skip hiber
		//
		if(BlTestBootMode(BOOT_MODE_HIBER_FROM_FV))
			try_leave(NOTHING);

		//
		// locate key press protocol
		//
		APPLE_KEY_STATE_PROTOCOL* keyStateProtocol							= nullptr;
		if(EFI_ERROR(status = EfiBootServices->LocateProtocol(&AppleKeyStateProtocolGuid, 0, reinterpret_cast<VOID**>(&keyStateProtocol))))
			try_leave(NOTHING);

		//
		// read state
		//
		UINT16 modifyFlags													= 0;
		CHAR16 pressedKeys[32]												= {0};
		UINTN statesCount													= ARRAYSIZE(pressedKeys);
		if(EFI_ERROR(status = keyStateProtocol->ReadKeyState(keyStateProtocol, &modifyFlags, &statesCount, pressedKeys)))
			try_leave(NOTHING);

		//
		// check keys
		//
		BOOLEAN pressedV													= FALSE;	// al
		BOOLEAN pressedR													= FALSE;	// cl
		BOOLEAN pressedC													= FALSE;	// r8b
		BOOLEAN pressedMinus												= FALSE;	// r9b
		BOOLEAN pressedX													= FALSE;	// r11b
		BOOLEAN pressedS													= FALSE;	// r14b
		BOOLEAN pressedCommand												= (modifyFlags & (APPLE_KEY_STATE_MODIFY_LEFT_COMMAND | APPLE_KEY_STATE_MODIFY_RIGHT_COMMAND)) ? TRUE : FALSE; // !dl
		BOOLEAN pressedShift												= (modifyFlags & (APPLE_KEY_STATE_MODIFY_LEFT_SHIFT | APPLE_KEY_STATE_MODIFY_RIGHT_SHIFT)) ? TRUE : FALSE;
		BOOLEAN pressedOption												= (modifyFlags & (APPLE_KEY_STATE_MODIFY_LEFT_OPTION | APPLE_KEY_STATE_MODIFY_RIGHT_OPTION)) ? TRUE : FALSE;
		BOOLEAN pressedControl												= (modifyFlags & (APPLE_KEY_STATE_MODIFY_LEFT_CONTROL | APPLE_KEY_STATE_MODIFY_RIGHT_CONTROL)) ? TRUE : FALSE;
		
		for(UINTN i = 0; i < statesCount; i ++)
		{
			switch(pressedKeys[i])
			{
			case APPLE_KEY_STATE_C:
				pressedC													= TRUE;
				break;

			case APPLE_KEY_STATE_MINUS:
				pressedMinus												= TRUE;
				break;

			case APPLE_KEY_STATE_R:
				pressedR													= TRUE;
				break;

			case APPLE_KEY_STATE_S:
				pressedS													= TRUE;
				break;

			case APPLE_KEY_STATE_V:
				pressedV													= TRUE;
				break;

			case APPLE_KEY_STATE_X:
				pressedX													= TRUE;
				break;
			}
		}

		//
		// Command, R = recovery
		//
		if(pressedR && pressedCommand)
			BlSetBootMode(BOOT_MODE_EFI_NVRAM_RECOVERY_BOOT_MODE, 0);

		//
		// run PasswordUI.efi
		//
		if(BlTestBootMode(BOOT_MODE_FIRMWARE_PASSWORD))
			try_leave(BlpRunPasswordUIEfi());

		//
		// SHIFT
		//
		if(pressedShift && !pressedCommand && !pressedControl && !pressedOption)
		{
			BlSetBootMode(BOOT_MODE_SAFE, 0);
			LdrSetupASLR(FALSE, 0);
			try_leave(NOTHING);
		}

		if(pressedCommand)
		{
			if(pressedV)
				BlSetBootMode(BOOT_MODE_VERBOSE, 0);

			if(pressedS)
			{
				if(pressedMinus)
					LdrSetupASLR(FALSE, 0);
				else
					BlSetBootMode(BOOT_MODE_SINGLE_USER | BOOT_MODE_VERBOSE, 0);
			}

			if(pressedX)
				BlSetBootMode(BOOT_MODE_X, 0);

			if(pressedC && pressedMinus)
				BlSetBootMode(BOOT_MODE_SKIP_BOARD_ID_CHECK, 0);
		}
	}
	__finally
	{
	}

	return status;
}

//
// process option
//
EFI_STATUS BlProcessOptions(CHAR8 CONST* bootCommandLine, CHAR8** kernelCommandLine, EFI_DEVICE_PATH_PROTOCOL* bootDevicePath, EFI_DEVICE_PATH_PROTOCOL* bootFilePath)
{
	EFI_STATUS status														= EFI_SUCCESS;
	CHAR8* bootArgsVariable													= nullptr;
	CHAR8* bootOptions														= nullptr;
	CHAR8 CONST* kernelFlags												= nullptr;
	CHAR8* kernelCachePathName												= nullptr;
	CHAR8* kernelPathName													= nullptr;
	CHAR8* ramDiskPathName													= nullptr;
	EFI_DEVICE_PATH_PROTOCOL* kernelCacheFilePath							= nullptr;
	EFI_DEVICE_PATH_PROTOCOL* kernelFilePath								= nullptr;
	EFI_DEVICE_PATH_PROTOCOL* ramDiskFilePath								= nullptr;
	BOOLEAN kernelCacheOverride												= FALSE;

	__try
	{
		//
		// extract kernel name and build boot options
		//
		bootCommandLine														= BlpExtractOptions(bootCommandLine);
		bootOptions															= static_cast<CHAR8*>(MmAllocatePool(strlen(bootCommandLine) + 1));
		if(!bootOptions)
			try_leave(status = EFI_OUT_OF_RESOURCES);
		strcpy(bootOptions, bootCommandLine);

		//
		// check safe mode
		//
		UINTN valueLength													= 0;
		if(CmGetStringValueForKeyAndCommandLine(bootOptions, CHAR8_CONST_STRING("-x"), &valueLength, FALSE))
			BlSetBootMode(BOOT_MODE_SAFE, 0);

		//
		// check net boot device path
		//
		if(DevPathHasMacAddressNode(bootDevicePath))
			BlSetBootMode(BOOT_MODE_NET, 0);

		//
		// check recovery mode
		//
		if(!BlTestBootMode(BOOT_MODE_NET | BOOT_MODE_HIBER_FROM_FV))
		{
			UINT8 dataBuffer[10]											= {0};
			UINTN dataSize													= sizeof(dataBuffer);
			UINT32 attribute												= 0;
			status															= EfiRuntimeServices->GetVariable(CHAR16_STRING(L"recovery-boot-mode"), &AppleNVRAMVariableGuid, &attribute, &dataSize, dataBuffer);
			if(!EFI_ERROR(status) || status == EFI_BUFFER_TOO_SMALL)
				BlSetBootMode(BOOT_MODE_EFI_NVRAM_RECOVERY_BOOT_MODE, 0);

			status															= EFI_SUCCESS;
		}

		//
		// read boot-args
		//
		if(!BlTestBootMode(BOOT_MODE_SAFE))
		{
			UINTN dataSize													= 0;
			UINT32 attribute												= 0;
			if(EfiRuntimeServices->GetVariable(CHAR16_STRING(L"boot-args"), &AppleNVRAMVariableGuid, &attribute, &dataSize, nullptr) == EFI_BUFFER_TOO_SMALL)
			{
				bootArgsVariable											= static_cast<CHAR8*>(MmAllocatePool(dataSize + sizeof(CHAR8)));
				if(bootArgsVariable)
				{
					if(!EFI_ERROR(EfiRuntimeServices->GetVariable(CHAR16_STRING(L"boot-args"), &AppleNVRAMVariableGuid, &attribute, &dataSize, bootArgsVariable)))
						bootArgsVariable[dataSize]							= 0;
					else
						MmFreePool(bootArgsVariable), bootArgsVariable = nullptr;
				}
			}
		}

		//
		// load kernel cache device path
		//
		if(!BlTestBootMode(BOOT_MODE_SAFE))
			BlpSetupPathFromVariable(&kernelCacheFilePath, &kernelCachePathName, CHAR16_CONST_STRING(L"efi-boot-kernelcache-data"));

		//
		// load kernel device path
		//
		if(!BlTestBootMode(BOOT_MODE_SAFE) && !kernelCacheFilePath)
			BlpSetupPathFromVariable(&kernelFilePath, &kernelPathName, CHAR16_CONST_STRING(L"efi-boot-file-data"));

		//
		// setup default kernel cache name
		//
		if(BlTestBootMode(BOOT_MODE_NET) && !kernelCacheFilePath && !kernelFilePath)
		{
			kernelCachePathName												= BlAllocateString(CHAR8_CONST_STRING("x86_64\\kernelcache"));
			kernelCacheFilePath												= DevPathAppendLastComponent(bootFilePath, kernelCachePathName, TRUE);
		}

		//
		// setup override flags
		//
		if(BlTestBootMode(BOOT_MODE_NET) && kernelCachePathName)
			kernelCacheOverride												= TRUE;

		//
		// load config file
		//
		if(!BlTestBootMode(BOOT_MODE_SAFE) || BlTestBootMode(BOOT_MODE_BOOT_IS_NOT_ROOT))
			kernelFlags														= BlpLoadConfigFile(bootOptions, bootFilePath);

		//
		// setup kernel command line
		//
		*kernelCommandLine													= BlSetupKernelCommandLine(bootOptions, bootArgsVariable, kernelFlags);

		//
		// check kernel
		//
		BOOLEAN keyFound													= FALSE;
		if(!EFI_ERROR(BlpSetupPathFromCommandLine(&kernelFilePath, &kernelPathName, *kernelCommandLine, CHAR8_CONST_STRING("Kernel"), bootFilePath, &keyFound)) && keyFound)
		{
			if(strcmp(kernelPathName[0] == '/' || kernelPathName[0] == '\\' ? kernelPathName + 1 : kernelPathName, CHAR8_CONST_STRING("mach_kernel")))
				BlSetBootMode(BOOT_MODE_ALT_KERNEL, 0);
		}

		//
		// check kernel cache
		//
		if(!EFI_ERROR(BlpSetupPathFromCommandLine(&kernelCacheFilePath, &kernelCachePathName, *kernelCommandLine, CHAR8_CONST_STRING("Kernel Cache"), bootFilePath, &keyFound)) && keyFound)
			kernelCacheOverride												= TRUE;

		//
		// check ramdisk
		//
		BlpSetupPathFromCommandLine(&ramDiskFilePath, &ramDiskPathName, *kernelCommandLine, CHAR8_CONST_STRING("RAM Disk"), bootFilePath, nullptr);

		//
		// verbose mode
		//
		if(CmGetStringValueForKeyAndCommandLine(*kernelCommandLine, CHAR8_CONST_STRING("-v"), &valueLength, FALSE))
			BlSetBootMode(BOOT_MODE_VERBOSE, 0);

		//
		// safe mode
		//
		if(CmGetStringValueForKeyAndCommandLine(*kernelCommandLine, CHAR8_CONST_STRING("-x"), &valueLength, FALSE))
			BlSetBootMode(BOOT_MODE_SAFE, 0);

		//
		// single user mode
		//
		if(CmGetStringValueForKeyAndCommandLine(*kernelCommandLine, CHAR8_CONST_STRING("-s"), &valueLength, FALSE))
			BlSetBootMode(BOOT_MODE_SINGLE_USER | BOOT_MODE_VERBOSE, 0);

		//
		// compact check
		//
		if(CmGetStringValueForKeyAndCommandLine(*kernelCommandLine, CHAR8_CONST_STRING("-no_compat_check"), &valueLength, FALSE))
			BlSetBootMode(BOOT_MODE_SKIP_BOARD_ID_CHECK, 0);

		//
		// show panic dialog
		//
		if(CmGetStringValueForKeyAndCommandLine(*kernelCommandLine, CHAR8_CONST_STRING("-no_panic_dialog"), &valueLength, FALSE))
			BlSetBootMode(BOOT_MODE_SKIP_PANIC_DIALOG, 0);

		//
		// debug
		//
		if(CmGetStringValueForKeyAndCommandLine(*kernelCommandLine, CHAR8_CONST_STRING("-debug"), &valueLength, FALSE))
			BlSetBootMode(BOOT_MODE_DEBUG, 0);

		//
		// disable ASLR for safe mode
		//
		if(BlTestBootMode(BOOT_MODE_SAFE) && BlTestBootMode(BOOT_MODE_ASLR))
			LdrSetupASLR(FALSE, 0);

		//
		// check slide
		//
		if(BlTestBootMode(BOOT_MODE_ASLR))
		{
			CHAR8 CONST* slideString										= CmGetStringValueForKeyAndCommandLine(*kernelCommandLine, CHAR8_CONST_STRING("slide"), &valueLength, TRUE);
			if(slideString)
			{
				INTN slideValue												= valueLength ? atoi(slideString) : 0;
				if(slideValue >= 0 && slideValue <= 0xff)
					LdrSetupASLR(!valueLength || slideValue, static_cast<UINT8>(slideValue));
			}
		}

		//
		// setup default value
		//
		if(!IoBootingFromNet())
		{
			if(!kernelPathName)
				kernelPathName												= BlAllocateString(CHAR8_CONST_STRING("mach_kernel"));
		}

		//
		// save those
		//
		LdrSetupKernelCachePath(kernelCacheFilePath, kernelCachePathName, kernelCacheOverride);
		LdrSetupKernelPath(kernelFilePath, kernelPathName);
		LdrSetupRamDiskPath(ramDiskFilePath, ramDiskPathName);
	}
	__finally
	{
		if(bootOptions)
			MmFreePool(bootOptions);

		if(bootArgsVariable)
			MmFreePool(bootArgsVariable);
	}

	return status;
}

//
// test boot mode
//
UINT32 BlTestBootMode(UINT32 bootMode)
{
	return BlpBootMode & bootMode;
}

//
// set boot mode
//
VOID BlSetBootMode(UINT32 setValue, UINT32 clearValue)
{
	BlpBootMode																|= setValue;
	BlpBootMode																&= ~clearValue;
}