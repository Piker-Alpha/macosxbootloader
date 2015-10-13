//********************************************************************
//	created:	11:11:2009   23:24
//	filename: 	FileIo.cpp
//	author:		tiamo
//	purpose:	file io
//********************************************************************

#include "stdafx.h"

//
// booting from net
//
STATIC EFI_LOAD_FILE_PROTOCOL* IopLoadFileProtocol							= nullptr;
STATIC EFI_FILE_HANDLE IopRootFile											= nullptr;

//
// find boot device
//
STATIC EFI_STATUS IopFindBootDevice(EFI_HANDLE* bootDeviceHandle, EFI_DEVICE_PATH_PROTOCOL** bootFilePath)
{
	STATIC CHAR16* checkFileName[] = 
	{
		CHAR16_STRING(L"\\OS X Install Data"),
		CHAR16_STRING(L"\\mach_kernel"),
		CHAR16_STRING(L"\\com.apple.boot.R"),
		CHAR16_STRING(L"\\com.apple.boot.P"),
		CHAR16_STRING(L"\\com.apple.boot.S"),
		CHAR16_STRING(L"\\com.apple.recovery.boot"),
	};

	STATIC CHAR16* booterName[] = 
	{
		CHAR16_STRING(L"\\OS X Install Data\\boot.efi"),
		CHAR16_STRING(L"\\System\\Library\\CoreServices\\boot.efi"),
		CHAR16_STRING(L"\\System\\Library\\CoreServices\\boot.efi"),
		CHAR16_STRING(L"\\System\\Library\\CoreServices\\boot.efi"),
		CHAR16_STRING(L"\\System\\Library\\CoreServices\\boot.efi"),
		CHAR16_STRING(L"\\com.apple.recovery.boot\\boot.efi"),
	};

	STATIC UINT8 bootFilePathBuffer[256]									= {0};
	EFI_HANDLE foundHandle[ARRAYSIZE(checkFileName)]						= {0};
	EFI_HANDLE* handleArray													= nullptr;
	EFI_STATUS status														= EFI_NOT_FOUND;

	__try
	{
		//
		// search all block device
		//
		UINTN handleCount													= 0;
		EfiBootServices->LocateHandleBuffer(ByProtocol, &EfiSimpleFileSystemProtocolGuid, nullptr, &handleCount, &handleArray);
		for(UINTN i = 0; i < handleCount; i ++)
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
			// check file exist
			//
			for(UINTN j = 0; j < ARRAYSIZE(checkFileName); j ++)
			{
				EFI_FILE_HANDLE checkFile									= nullptr;
				BOOLEAN fileExist											= !EFI_ERROR(rootFile->Open(rootFile, &checkFile, checkFileName[j], EFI_FILE_MODE_READ, 0));
				if(!fileExist)
					continue;

				foundHandle[j]												= theHandle;
				checkFile->Close(checkFile);
			}
			rootFile->Close(rootFile);
		}
	}
	__finally
	{
		if(handleArray)
			MmFreePool(handleArray);

		for(UINTN i = 0; i < ARRAYSIZE(foundHandle); i ++)
		{
			if(!foundHandle[i])
				continue;

			FILEPATH_DEVICE_PATH* filePath									= reinterpret_cast<FILEPATH_DEVICE_PATH*>(bootFilePathBuffer);
			*bootFilePath													= &filePath->Header;
			filePath->Header.Type											= MEDIA_DEVICE_PATH;
			filePath->Header.SubType										= MEDIA_FILEPATH_DP;
			status															= EFI_SUCCESS;
			*bootDeviceHandle												= foundHandle[i];
			UINTN size														= (wcslen(booterName[i]) + 1) * sizeof(CHAR16);
			SetDevicePathNodeLength(&filePath->Header, size + SIZE_OF_FILEPATH_DEVICE_PATH);
			EFI_DEVICE_PATH_PROTOCOL* endOfPath								= NextDevicePathNode(&filePath->Header);
			memcpy(filePath->PathName, booterName[i], size);
			SetDevicePathEndNode(endOfPath);
			break;
		}
	}

	return status;
}

//
// check PRS
//
STATIC EFI_STATUS IopCheckRPS(EFI_FILE_HANDLE rootFile, EFI_FILE_HANDLE* realRootFile)
{
	EFI_STATUS retValue														= EFI_SUCCESS;
	*realRootFile															= rootFile;

	EFI_FILE_HANDLE fileR													= nullptr;
	EFI_STATUS startR														= rootFile->Open(rootFile, &fileR, CHAR16_STRING(L"com.apple.boot.R"), EFI_FILE_MODE_READ, 0);

	EFI_FILE_HANDLE fileP													= nullptr;
	EFI_STATUS startP														= rootFile->Open(rootFile, &fileP, CHAR16_STRING(L"com.apple.boot.P"), EFI_FILE_MODE_READ, 0);

	EFI_FILE_HANDLE fileS													= nullptr;
	EFI_STATUS startS														= rootFile->Open(rootFile, &fileS, CHAR16_STRING(L"com.apple.boot.S"), EFI_FILE_MODE_READ, 0);

	if(!EFI_ERROR(startR) && !EFI_ERROR(startP) && !EFI_ERROR(startS))
		*realRootFile														= fileR;
	else if(!EFI_ERROR(startR) && !EFI_ERROR(startP))
		*realRootFile														= fileP;
	else if(!EFI_ERROR(startR) && !EFI_ERROR(startS))
		*realRootFile														= fileR;
	else if(!EFI_ERROR(startS) && !EFI_ERROR(startP))
		*realRootFile														= fileS;
	else if(!EFI_ERROR(startR))
		*realRootFile														= fileR;
	else if(!EFI_ERROR(startP))
		*realRootFile														= fileP;
	else if(!EFI_ERROR(startS))
		*realRootFile														= fileS;
	else
		retValue															= EFI_NOT_FOUND;

	if(!EFI_ERROR(startR) && *realRootFile != fileR)
		fileR->Close(fileR);
	if(!EFI_ERROR(startP) && *realRootFile != fileP)
		fileP->Close(fileP);
	if(!EFI_ERROR(startS) && *realRootFile != fileS)
		fileS->Close(fileS);

	return retValue;
}

//
// detect root
//
STATIC EFI_STATUS IopDetectRoot(EFI_HANDLE deviceHandle, EFI_DEVICE_PATH_PROTOCOL* bootFilePath, BOOLEAN allowBootDirectory)
{
	EFI_STATUS status														= EFI_SUCCESS;
	EFI_FILE_HANDLE kernelFile												= nullptr;
	EFI_FILE_HANDLE realRootFile											= nullptr;
	CHAR8* bootFullPath														= nullptr;
	CHAR16* bootFullPath16													= nullptr;
	IopRootFile																= nullptr;
	IopLoadFileProtocol														= nullptr;

	__try
	{
		//
		// check simple file system protocol or load file protocol
		//
		EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* fileSystemProtocol					= nullptr;
		if(EFI_ERROR(status = EfiBootServices->HandleProtocol(deviceHandle, &EfiSimpleFileSystemProtocolGuid, reinterpret_cast<VOID**>(&fileSystemProtocol))))
			try_leave(status = EfiBootServices->HandleProtocol(deviceHandle, &EfiLoadFileProtocolGuid, reinterpret_cast<VOID**>(&IopLoadFileProtocol)));

		//
		// open root directory
		//
		if(EFI_ERROR(status = fileSystemProtocol->OpenVolume(fileSystemProtocol, &IopRootFile)))
			try_leave(NOTHING);

		//
		// check kernel in root directory
		//
		if(!EFI_ERROR(status = IopRootFile->Open(IopRootFile, &kernelFile, CHAR16_STRING(L"mach_kernel"), EFI_FILE_MODE_READ, 0)))
			try_leave(NOTHING);

		//
		// detect RPS
		//
		if(!EFI_ERROR(status = IopCheckRPS(IopRootFile, &realRootFile)))
		{
			DEVICE_TREE_NODE* chosenNode									= DevTreeFindNode(CHAR8_CONST_STRING("/chosen"), TRUE);
			if(chosenNode)
				DevTreeAddProperty(chosenNode, CHAR8_CONST_STRING("bootroot-active"), nullptr, 0, FALSE);

			try_leave(BlSetBootMode(BOOT_MODE_BOOT_IS_NOT_ROOT, 0));
		}

		//
		// check boot directory
		//
		if(!allowBootDirectory)
			try_leave(status = EFI_NOT_FOUND);

		//
		// get booter's full path
		//
		status																= EFI_SUCCESS;
		bootFullPath														= DevPathExtractFilePathName(bootFilePath, TRUE);
		if(!bootFullPath || (bootFullPath[0] != '/' && bootFullPath[0] != '\\') || strlen(bootFullPath) == 1)
			try_leave(NOTHING);

		//
		// get current directory
		//
		CHAR8* lastComponent												= nullptr;
		for(UINTN i = 1; bootFullPath[i]; i ++)
		{
			if(bootFullPath[i] == '\\' || bootFullPath[i] == '/')
				lastComponent												= bootFullPath + i;
		}
		if(lastComponent)
			*lastComponent													= 0;
		else
			try_leave(NOTHING);

		//
		// open it
		//
		bootFullPath16														= BlAllocateUnicodeFromUtf8(bootFullPath, lastComponent - bootFullPath);
		if(!bootFullPath16)
			try_leave(status = EFI_OUT_OF_RESOURCES);
		if(EFI_ERROR(status = IopRootFile->Open(IopRootFile, &realRootFile, bootFullPath16, EFI_FILE_MODE_READ, 0)))
			try_leave(realRootFile = nullptr);

		//
		// check EncryptedRoot.plist.wipekey
		//
		if(!EFI_ERROR(realRootFile->Open(realRootFile, &kernelFile, CHAR16_STRING(L"EncryptedRoot.plist.wipekey"), EFI_FILE_MODE_READ, 0)))
			BlSetBootMode(BOOT_MODE_BOOT_IS_NOT_ROOT, 0);
		else
			realRootFile->Close(realRootFile), realRootFile = nullptr;
	}
	__finally
	{
		if(bootFullPath)
			MmFreePool(bootFullPath);

		if(bootFullPath16)
			MmFreePool(bootFullPath16);

		if(kernelFile)
			kernelFile->Close(kernelFile);

		if(realRootFile && realRootFile != IopRootFile)
			IopRootFile->Close(IopRootFile), IopRootFile = realRootFile;

		if(IopRootFile && EFI_ERROR(status))
			IopRootFile->Close(IopRootFile), IopRootFile = nullptr;
	}

	return status;
}

//
// load booter with root uuid
//
STATIC EFI_STATUS IopLoadBooterWithRootUUID(EFI_DEVICE_PATH_PROTOCOL* bootFilePath, EFI_HANDLE theHandle, CHAR8 CONST* rootUUID, EFI_HANDLE* imageHandle)
{
	EFI_STATUS status														= EFI_SUCCESS;
	EFI_FILE_HANDLE savedRootFile											= IopRootFile;
	EFI_DEVICE_PATH_PROTOCOL* recoveryFilePath								= nullptr;
	CHAR8* fileBuffer														= nullptr;
	XML_TAG* rootTag														= nullptr;
	EFI_FILE_HANDLE rootFile												= nullptr;
	*imageHandle															= nullptr;
	IopRootFile																= nullptr;

	__try
	{
		//
		// get device path
		//
		EFI_DEVICE_PATH_PROTOCOL* devicePath								= DevPathGetDevicePathProtocol(theHandle);
		if(!devicePath)
			try_leave(status = EFI_NOT_FOUND);

		//
		// build recovery file path
		//
		recoveryFilePath													= DevPathAppendFilePath(devicePath, CHAR16_CONST_STRING(L"\\com.apple.recovery.boot\\boot.efi"));
		if(!recoveryFilePath)
			try_leave(status = EFI_NOT_FOUND);

		//
		// check file exist
		//
		if(EFI_ERROR(status = EfiBootServices->LoadImage(FALSE, EfiImageHandle, recoveryFilePath, nullptr, 0, imageHandle)))
			try_leave(NOTHING);

		//
		// get file system protocol
		//
		EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* fileSystemProtocol					= nullptr;
		if(EFI_ERROR(status = EfiBootServices->HandleProtocol(theHandle, &EfiSimpleFileSystemProtocolGuid, reinterpret_cast<VOID**>(&fileSystemProtocol))))
			try_leave(NOTHING);

		//
		// open root directory
		//
		if(EFI_ERROR(status = fileSystemProtocol->OpenVolume(fileSystemProtocol, &rootFile)))
			try_leave(NOTHING);

		//
		// detect real root
		//
		if(EFI_ERROR(status = IopCheckRPS(rootFile, &IopRootFile)))
			try_leave(NOTHING);

		//
		// load boot file
		//
		if(EFI_ERROR(status = IoReadWholeFile(bootFilePath, CHAR8_CONST_STRING("Library\\Preferences\\SystemConfiguration\\com.apple.Boot.plist"), &fileBuffer, nullptr, TRUE)))
			try_leave(NOTHING);

		//
		// parse it
		//
		if(EFI_ERROR(status = CmParseXmlFile(fileBuffer, &rootTag)))
			try_leave(NOTHING);

		//
		// read root UUID
		//
		CHAR8 CONST* theRootUUID											= CmGetStringValueForKey(rootTag, CHAR8_CONST_STRING("Root UUID"), nullptr);
		if(!theRootUUID)
			try_leave(status = EFI_NOT_FOUND);

		//
		// check is the same
		//
		status																= strcmp(theRootUUID, rootUUID) ? EFI_NOT_FOUND : EFI_SUCCESS;
	}
	__finally
	{
		if(recoveryFilePath)
			MmFreePool(recoveryFilePath);
		if(fileBuffer)
			MmFreePool(fileBuffer);
		if(rootTag)
			CmFreeTag(rootTag);

		if(EFI_ERROR(status) && *imageHandle)
			EfiBootServices->UnloadImage(*imageHandle);

		if(IopRootFile != rootFile)
			IopRootFile->Close(IopRootFile);

		if(rootFile)
			rootFile->Close(rootFile);

		IopRootFile															= savedRootFile;
	}

	return status;
}

//
// detect root
//
EFI_STATUS IoDetectRoot(EFI_HANDLE* deviceHandle, EFI_DEVICE_PATH_PROTOCOL** bootFilePath, BOOLEAN detectBoot)
{
	if(detectBoot && (!EFI_ERROR(IopDetectRoot(*deviceHandle, *bootFilePath, FALSE)) || !EFI_ERROR(IopFindBootDevice(deviceHandle, bootFilePath))))
		return EFI_SUCCESS;

	return IopDetectRoot(*deviceHandle, *bootFilePath, TRUE);
}

//
// load booter
//
EFI_STATUS IoLoadBooterWithRootUUID(EFI_DEVICE_PATH_PROTOCOL* bootFilePath, CHAR8 CONST* rootUUID, EFI_HANDLE* imageHandle)
{
	EFI_STATUS status														= EFI_SUCCESS;
	EFI_HANDLE* handleArray													= nullptr;

	__try
	{
		//
		// search all block device
		//
		CsConnectDevice(TRUE, FALSE);
		UINTN handleCount													= 0;
		EfiBootServices->LocateHandleBuffer(ByProtocol, &EfiBlockIoProtocolGuid, nullptr, &handleCount, &handleArray);
		for(UINTN i = 0; i < handleCount; i ++)
		{
			EFI_HANDLE theHandle											= handleArray[i];
			if(!theHandle)
				continue;

			if(!EFI_ERROR(status = IopLoadBooterWithRootUUID(bootFilePath, theHandle, rootUUID, imageHandle)))
				try_leave(NOTHING);
		}
		status																= EFI_NOT_FOUND;
	}
	__finally
	{
		if(handleArray)
			MmFreePool(handleArray);
	}

	return status;
}

//
// booting from net
//
BOOLEAN IoBootingFromNet()
{
	return !IopRootFile && IopLoadFileProtocol;
}

//
// open file
//
EFI_STATUS IoOpenFile(CHAR8 CONST* filePathName, EFI_DEVICE_PATH_PROTOCOL* filePath, IO_FILE_HANDLE* fileHandle, UINTN openMode)
{
	EFI_STATUS status														= EFI_SUCCESS;

	__try
	{
		memset(fileHandle, 0, sizeof(IO_FILE_HANDLE));
		if(IopLoadFileProtocol)
		{
			if(filePath)
			{
				fileHandle->EfiLoadFileProtocol								= IopLoadFileProtocol;
				fileHandle->EfiFilePath										= DevPathDuplicate(filePath);
				try_leave(status = fileHandle->EfiFilePath ? EFI_SUCCESS : EFI_OUT_OF_RESOURCES);
			}

			if(!filePathName || !filePathName[0])
				try_leave(status = EFI_NOT_FOUND);

			if(openMode == IO_OPEN_MODE_RAMDISK)
				try_leave(status = EFI_UNSUPPORTED);
		}
		else if(filePath)
		{
			if(openMode == IO_OPEN_MODE_KERNEL)
				try_leave(status = EFI_UNSUPPORTED);
		}
		else if((!filePathName || !filePathName[0] || !IopRootFile))
		{
			try_leave(status = EFI_NOT_FOUND);
		}

		STATIC CHAR16 unicodeFilePathName[1024]								= {0};
		UINTN nameLength													= filePathName ? strlen(filePathName) : 0;
		BlUtf8ToUnicode(filePathName, nameLength, unicodeFilePathName, ARRAYSIZE(unicodeFilePathName) - 1);

		if(IopLoadFileProtocol)
		{
			UINTN filePathNodeSize											= (nameLength + 1) * sizeof(CHAR16) + SIZE_OF_FILEPATH_DEVICE_PATH;
			FILEPATH_DEVICE_PATH* fileDevicePath							= static_cast<FILEPATH_DEVICE_PATH*>(MmAllocatePool(filePathNodeSize + END_DEVICE_PATH_LENGTH));
			if(!fileDevicePath)
				try_leave(status = EFI_OUT_OF_RESOURCES);

			fileDevicePath->Header.Type										= MEDIA_DEVICE_PATH;
			fileDevicePath->Header.SubType									= MEDIA_FILEPATH_DP;
			SetDevicePathNodeLength(&fileDevicePath->Header, filePathNodeSize);
			memcpy(fileDevicePath->PathName, unicodeFilePathName, (nameLength + 1) * sizeof(CHAR16));

			EFI_DEVICE_PATH_PROTOCOL* endNode								= EfiNextDevicePathNode(&fileDevicePath->Header);
			SetDevicePathEndNode(endNode);

			fileHandle->EfiLoadFileProtocol									= IopLoadFileProtocol;
			fileHandle->EfiFilePath											= &fileDevicePath->Header;
		}
		else
		{
			status															= IopRootFile->Open(IopRootFile, &fileHandle->EfiFileHandle, unicodeFilePathName, EFI_FILE_MODE_READ, 0);
		}
	}
	__finally
	{
	}

	return status;
}

//
// set position
//
EFI_STATUS IoSetFilePosition(IO_FILE_HANDLE* fileHandle, UINT64 filePosition)
{
	if(fileHandle->EfiFileHandle)
		return fileHandle->EfiFileHandle->SetPosition(fileHandle->EfiFileHandle, filePosition);

	fileHandle->FileOffset													= static_cast<UINTN>(filePosition);
	return EFI_SUCCESS;
}

//
// get file size
//
EFI_STATUS IoGetFileSize(IO_FILE_HANDLE* fileHandle, UINT64* fileSize)
{
	*fileSize																= 0;

	if(fileHandle->EfiLoadFileProtocol)
	{
		if(!fileHandle->FileSize)
		{
			EFI_STATUS status												= fileHandle->EfiLoadFileProtocol->LoadFile(fileHandle->EfiLoadFileProtocol, fileHandle->EfiFilePath, FALSE, &fileHandle->FileSize, nullptr);
			if(status != EFI_BUFFER_TOO_SMALL)
				return status;
		}

		*fileSize															= fileHandle->FileSize;
		return EFI_SUCCESS;
	}

	if(fileHandle->FileBuffer)
	{
		*fileSize															= fileHandle->FileSize;
		return EFI_SUCCESS;
	}

	if(fileHandle->EfiFileHandle)
	{
		EFI_FILE_INFO* fileInfo												= nullptr;
		EFI_STATUS status													= IoGetFileInfo(fileHandle, &fileInfo);
		if(EFI_ERROR(status) || !fileInfo)
			return status;

		*fileSize															= fileInfo->FileSize;
		MmFreePool(fileInfo);
		return EFI_SUCCESS;
	}

	return EFI_INVALID_PARAMETER;
}

//
// get file info
//
EFI_STATUS IoGetFileInfo(IO_FILE_HANDLE* fileHandle, EFI_FILE_INFO** fileInfo)
{
	*fileInfo																= nullptr;
	if(!fileHandle->EfiFileHandle)
		return EFI_INVALID_PARAMETER;

	UINTN infoSize															= SIZE_OF_EFI_FILE_INFO + sizeof(CHAR16) * 64;
	EFI_FILE_INFO* infoBuffer												= static_cast<EFI_FILE_INFO*>(MmAllocatePool(infoSize));
	if(!infoBuffer)
		return EFI_OUT_OF_RESOURCES;

	EFI_STATUS status														= fileHandle->EfiFileHandle->GetInfo(fileHandle->EfiFileHandle, &EfiFileInfoGuid, &infoSize, infoBuffer);
	if(status == EFI_BUFFER_TOO_SMALL)
	{
		MmFreePool(infoBuffer);
		infoBuffer															= static_cast<EFI_FILE_INFO*>(MmAllocatePool(infoSize));
		status																= fileHandle->EfiFileHandle->GetInfo(fileHandle->EfiFileHandle, &EfiFileInfoGuid, &infoSize, infoBuffer);
	}

	if(EFI_ERROR(status))
		MmFreePool(infoBuffer);
	else
		*fileInfo															= infoBuffer;

	return status;
}

//
// read file
//
EFI_STATUS IoReadFile(IO_FILE_HANDLE* fileHandle, VOID* readBuffer, UINTN bufferSize, UINTN* readLength, BOOLEAN directoryFile)
{
	EFI_STATUS status														= EFI_SUCCESS;

	__try
	{
		*readLength															= 0;

		if(fileHandle->EfiLoadFileProtocol)
		{
			UINT64 fileSize													= 0;
			if(EFI_ERROR(status = IoGetFileSize(fileHandle, &fileSize)))
				try_leave(NOTHING);

			if(!fileHandle->FileBuffer && !fileHandle->FileOffset && bufferSize == fileSize)
				try_leave(*readLength = bufferSize; status = fileHandle->EfiLoadFileProtocol->LoadFile(fileHandle->EfiLoadFileProtocol, fileHandle->EfiFilePath, FALSE, readLength, readBuffer));

			if(fileSize <= fileHandle->FileOffset)
				try_leave(NOTHING);

			fileHandle->FileSize											= static_cast<UINTN>(fileSize);

			if(!fileHandle->FileBuffer)
			{
				fileHandle->FileBuffer										= static_cast<UINT8*>(MmAllocatePool(static_cast<UINTN>(fileSize)));
				if(!fileHandle->FileBuffer)
					try_leave(status = EFI_OUT_OF_RESOURCES);

				UINTN readLength											= static_cast<UINTN>(fileSize);
				status														= fileHandle->EfiLoadFileProtocol->LoadFile(fileHandle->EfiLoadFileProtocol, fileHandle->EfiFilePath, FALSE, &readLength, fileHandle->FileBuffer);
				if(EFI_ERROR(status))
					try_leave(NOTHING);

				if(readLength != fileHandle->FileSize)
					try_leave(MmFreePool(fileHandle->FileBuffer); fileHandle->FileBuffer = nullptr; status = EFI_DEVICE_ERROR);
			}
		}

		if(fileHandle->FileBuffer)
		{
			UINTN copyLength												= fileHandle->FileOffset >= fileHandle->FileSize ? 0 : fileHandle->FileSize - fileHandle->FileOffset;
			if(copyLength > bufferSize)
				copyLength													= bufferSize;

			memcpy(readBuffer, fileHandle->FileBuffer + fileHandle->FileOffset, copyLength);
			fileHandle->FileOffset											+= copyLength;
			*readLength														= copyLength;
			try_leave(NOTHING);
		}

		if(!fileHandle->EfiFileHandle)
			try_leave(status = EFI_INVALID_PARAMETER);

		UINT8* curBuffer													= static_cast<UINT8*>(readBuffer);
		while(bufferSize)
		{
			UINTN lengthThisRun												= bufferSize > 1024 * 1024 ? 1024 * 1024 : bufferSize;
			if(EFI_ERROR(status = fileHandle->EfiFileHandle->Read(fileHandle->EfiFileHandle, &lengthThisRun, curBuffer)) || !lengthThisRun)
				try_leave(NOTHING);

			bufferSize														-= lengthThisRun;
			curBuffer														+= lengthThisRun;
			*readLength														+= lengthThisRun;

			if(directoryFile)
				break;
		}
	}
	__finally
	{
	}

	return status;
}

//
// close file
//
VOID IoCloseFile(IO_FILE_HANDLE* fileHandle)
{
	if(fileHandle->FileBuffer)
		MmFreePool(fileHandle->FileBuffer);

	if(fileHandle->EfiFilePath)
		MmFreePool(fileHandle->EfiFilePath);

	if(fileHandle->EfiFileHandle)
		fileHandle->EfiFileHandle->Close(fileHandle->EfiFileHandle);

	memset(fileHandle, 0, sizeof(IO_FILE_HANDLE));
}

//
// read whole file
//
EFI_STATUS IoReadWholeFile(EFI_DEVICE_PATH_PROTOCOL* bootFilePath, CHAR8 CONST* fileName, CHAR8** fileBuffer, UINTN* fileSize, BOOLEAN asTextFile)
{
	EFI_STATUS status														= EFI_SUCCESS;
	*fileBuffer																= nullptr;
	IO_FILE_HANDLE fileHandle												= {0};
	EFI_DEVICE_PATH_PROTOCOL* filePath										= nullptr;

	__try
	{
		//
		// build file path
		//
		if(IoBootingFromNet() && bootFilePath)
			filePath														= DevPathAppendLastComponent(bootFilePath, fileName, TRUE);

		//
		// open file
		//
		if(EFI_ERROR(status = IoOpenFile(fileName, filePath, &fileHandle, IO_OPEN_MODE_NORMAL)))
			try_leave(NOTHING);

		//
		// get file size
		//
		UINT64 localFileSize												= 0;
		if(EFI_ERROR(status = IoGetFileSize(&fileHandle, &localFileSize)))
			try_leave(NOTHING);

		//
		// allocate buffer
		//
		UINTN totalSize														= static_cast<UINTN>(localFileSize) + (asTextFile ? sizeof(CHAR8) : 0);
		*fileBuffer															= static_cast<CHAR8*>(MmAllocatePool(totalSize));
		if(!*fileBuffer)
			try_leave(status = EFI_OUT_OF_RESOURCES);

		//
		// read file
		//
		UINTN readLength													= 0;
		if(EFI_ERROR(status = IoReadFile(&fileHandle, *fileBuffer, static_cast<UINTN>(localFileSize), &readLength, FALSE)))
			try_leave(NOTHING);

		//
		// append NULL
		//
		if(asTextFile)
			(*fileBuffer)[readLength / sizeof(CHAR8)]						= 0;

		if(fileSize)
			*fileSize														= static_cast<UINTN>(localFileSize);
	}
	__finally
	{
		IoCloseFile(&fileHandle);

		if(filePath)
			MmFreePool(filePath);

		if(EFI_ERROR(status))
		{
			if(*fileBuffer)
				MmFreePool(*fileBuffer);

			*fileBuffer														= nullptr;
			if(fileSize)
				*fileSize													= 0;
		}
	}

	return status;
}