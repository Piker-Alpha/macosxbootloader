//********************************************************************
//	created:	8:11:2009   6:39
//	filename: 	DevicePath.cpp
//	author:		tiamo
//	purpose:	device path
//********************************************************************

#include "stdafx.h"

//
// get device path size
//
UINTN DevPathGetSize(EFI_DEVICE_PATH_PROTOCOL* devicePath)
{
	if(!devicePath)
		return 0;

	EFI_DEVICE_PATH_PROTOCOL* start											= devicePath;
	while(!EfiIsDevicePathEnd(devicePath))
		devicePath															= EfiNextDevicePathNode(devicePath);

	return ArchConvertPointerToAddress(devicePath) - ArchConvertPointerToAddress(start) + sizeof (EFI_DEVICE_PATH_PROTOCOL);
}

//
// get node count
//
UINTN DevPathGetNodeCount(EFI_DEVICE_PATH_PROTOCOL* devicePath)
{
	if(!devicePath)
		return 0;

	UINTN retValue															= 0;
	for(; !EfiIsDevicePathEnd(devicePath); retValue += 1)
		devicePath															= EfiNextDevicePathNode(devicePath);
	return retValue;
}

//
// get node
//
EFI_DEVICE_PATH_PROTOCOL* DevPathGetNode(EFI_DEVICE_PATH_PROTOCOL* devicePath, UINT8 type, UINT8 subType)
{
	while(!EfiIsDevicePathEnd(devicePath))
	{
		if(EfiDevicePathType(devicePath) == type && devicePath->SubType == subType)
			return devicePath;

		devicePath															= EfiNextDevicePathNode(devicePath);
	}
	return nullptr;
}

//
// copy device path
//
EFI_DEVICE_PATH_PROTOCOL* DevPathDuplicate(EFI_DEVICE_PATH_PROTOCOL* devicePath)
{
	if(!devicePath)
		return nullptr;

	UINTN size																= DevPathGetSize(devicePath);
	if(!size)
		return nullptr;

	EFI_DEVICE_PATH_PROTOCOL* newDevicePath									= static_cast<EFI_DEVICE_PATH_PROTOCOL*>(MmAllocatePool(size));
	if(!newDevicePath)
		return nullptr;

	memcpy(newDevicePath, devicePath, size);
	return newDevicePath;
}

//
// append device path
//
EFI_DEVICE_PATH_PROTOCOL* DevPathAppendDevicePath(EFI_DEVICE_PATH_PROTOCOL* devicePath, EFI_DEVICE_PATH_PROTOCOL* appendPath)
{
	if(!devicePath)
		return DevPathDuplicate(appendPath);

	if(!appendPath)
		return DevPathDuplicate(devicePath);

	UINTN size1																= DevPathGetSize(devicePath) - END_DEVICE_PATH_LENGTH;
	UINTN size2																= DevPathGetSize(appendPath);
	UINTN size																= size1 + size2;

	EFI_DEVICE_PATH_PROTOCOL* newDevicePath									= static_cast<EFI_DEVICE_PATH_PROTOCOL*>(MmAllocatePool(size));
	if(!newDevicePath)
		return nullptr;

	memcpy(newDevicePath, devicePath, size1);
	memcpy(Add2Ptr(newDevicePath, size1, VOID*), appendPath, size2);
	return newDevicePath;
}

//
// append file path
//
EFI_DEVICE_PATH_PROTOCOL* DevPathAppendFilePath(EFI_DEVICE_PATH_PROTOCOL* devicePath, CHAR16 CONST* fileName)
{
	if(!devicePath || !fileName || !fileName[0])
		return nullptr;

	UINTN devicePathSize													= DevPathGetSize(devicePath);
	if(!devicePathSize)
		return nullptr;

	UINTN size																= (wcslen(fileName) + 1) * sizeof(CHAR16);
	EFI_DEVICE_PATH_PROTOCOL* filePath										= static_cast<EFI_DEVICE_PATH_PROTOCOL*>(MmAllocatePool(size + devicePathSize + SIZE_OF_FILEPATH_DEVICE_PATH));
	if(!filePath)
		return nullptr;

	devicePathSize															-= END_DEVICE_PATH_LENGTH;
	memcpy(filePath, devicePath, devicePathSize);
	FILEPATH_DEVICE_PATH* filePathNode										= Add2Ptr(filePath, devicePathSize, FILEPATH_DEVICE_PATH*);
	filePathNode->Header.Type												= MEDIA_DEVICE_PATH;
	filePathNode->Header.SubType											= MEDIA_FILEPATH_DP;
	SetDevicePathNodeLength(&filePathNode->Header, size + SIZE_OF_FILEPATH_DEVICE_PATH);
	memcpy(filePathNode->PathName, fileName, size);
	EFI_DEVICE_PATH_PROTOCOL* endOfPath										= NextDevicePathNode(&filePathNode->Header);
	SetDevicePathEndNode(endOfPath);
	return filePath;
}

//
// append file path
//
EFI_DEVICE_PATH_PROTOCOL* DevPathAppendLastComponent(EFI_DEVICE_PATH_PROTOCOL* devicePath, CHAR8 CONST* fileName, BOOLEAN replaceLastComponent)
{
	UINTN fileNameLength													= strlen(fileName);
	UINTN length															= DevPathGetSize(devicePath) + (fileNameLength + 1) * sizeof(CHAR16);
	EFI_DEVICE_PATH_PROTOCOL* newDevicePath									= static_cast<EFI_DEVICE_PATH_PROTOCOL*>(MmAllocatePool(length));
	if(!newDevicePath)
		return nullptr;

	EFI_DEVICE_PATH_PROTOCOL* dstDevicePath									= newDevicePath;
	EFI_DEVICE_PATH_PROTOCOL* srcDevicePath									= devicePath;
	FILEPATH_DEVICE_PATH* srcFilePath										= nullptr;
	FILEPATH_DEVICE_PATH* dstFilePath										= nullptr;

	while(!EfiIsDevicePathEnd(srcDevicePath))
	{
		if(EfiDevicePathType(srcDevicePath) == MEDIA_DEVICE_PATH && srcDevicePath->SubType == MEDIA_FILEPATH_DP)
		{
			srcFilePath														= _CR(srcDevicePath, FILEPATH_DEVICE_PATH, Header);
			dstFilePath														= _CR(dstDevicePath, FILEPATH_DEVICE_PATH, Header);
		}

		memcpy(dstDevicePath, srcDevicePath, DevicePathNodeLength(srcDevicePath));
		srcDevicePath														= EfiNextDevicePathNode(srcDevicePath);
		dstDevicePath														= EfiNextDevicePathNode(dstDevicePath);
	}

	if(!srcFilePath || !dstFilePath)
	{
		MmFreePool(newDevicePath);
		return nullptr;
	}

	dstFilePath->Header.Type												= MEDIA_DEVICE_PATH;
	dstFilePath->Header.SubType												= MEDIA_FILEPATH_DP;
	UINTN writePosition														= 0;

	if(replaceLastComponent)
	{
		CHAR16* pathName													= srcFilePath->PathName;
		CHAR16* lastDirectory												= nullptr;
		UINTN pathLength													= (DevicePathNodeLength(&srcFilePath->Header) - SIZE_OF_FILEPATH_DEVICE_PATH) / sizeof(CHAR16);
		for(UINTN i = 0; i < pathLength && pathName[i]; i ++)
		{
			if(pathName[i] == L'\\' || pathName[i] == L'/')
				lastDirectory												= pathName + i;
		}

		if(lastDirectory)
			lastDirectory													+= 1;
		else
			lastDirectory													= pathName;

		writePosition														= lastDirectory - pathName;
	}
	else
	{
		writePosition														= (DevicePathNodeLength(&srcFilePath->Header) - SIZE_OF_FILEPATH_DEVICE_PATH) / sizeof(CHAR16);
		while(!srcFilePath->PathName[writePosition - 1] && writePosition)
			writePosition													-= 1;
	}

	UINTN usedLength														= ArchConvertPointerToAddress(dstFilePath->PathName + writePosition) - ArchConvertPointerToAddress(newDevicePath);
	BlUtf8ToUnicode(fileName, fileNameLength, dstFilePath->PathName + writePosition, (length - usedLength - END_DEVICE_PATH_LENGTH) / sizeof(CHAR16));
	UINTN nodeLength														= SIZE_OF_FILEPATH_DEVICE_PATH + (wcslen(dstFilePath->PathName) + 1) * sizeof(CHAR16);
	SetDevicePathNodeLength(&dstFilePath->Header, nodeLength);
	EFI_DEVICE_PATH_PROTOCOL* endOfPath										= NextDevicePathNode(&dstFilePath->Header);
	SetDevicePathEndNode(endOfPath);
	return newDevicePath;
}

//
// get utf8 name
//
CHAR8* DevPathExtractFilePathName(EFI_DEVICE_PATH_PROTOCOL* devicePath, BOOLEAN fullPath)
{
	FILEPATH_DEVICE_PATH* startFilePath										= nullptr;
	FILEPATH_DEVICE_PATH* endFilePath										= nullptr;
	UINTN totalLength														= 0;
	while(!EfiIsDevicePathEnd(devicePath))
	{
		if(devicePath->Type == MEDIA_DEVICE_PATH && devicePath->SubType == MEDIA_FILEPATH_DP)
		{
			endFilePath														= _CR(devicePath, FILEPATH_DEVICE_PATH, Header);
			totalLength														+= DevicePathNodeLength(devicePath) - SIZE_OF_FILEPATH_DEVICE_PATH + sizeof(CHAR16);
			if(!startFilePath)
				startFilePath												= endFilePath;
		}
		else if(startFilePath)
		{
			break;
		}

		devicePath															= EfiNextDevicePathNode(devicePath);
	}

	if(!startFilePath)
		return nullptr;

	CHAR16* pathName														= nullptr;
	CHAR16* allocatedPathName												= nullptr;
	UINTN pathLength														= 0;
	if(startFilePath != endFilePath)
	{
		allocatedPathName													= static_cast<CHAR16*>(MmAllocatePool(totalLength));
		if(!allocatedPathName)
			return nullptr;

		pathName															= allocatedPathName;
		while(TRUE)
		{
			UINTN length													= (DevicePathNodeLength(&startFilePath->Header) - SIZE_OF_FILEPATH_DEVICE_PATH) / sizeof(CHAR16);
			for(UINTN i = 0; i < length && startFilePath->PathName[i]; i ++, pathLength ++)
				pathName[pathLength]										= startFilePath->PathName[i];

			if(startFilePath == endFilePath)
				break;

			startFilePath													= _CR(EfiNextDevicePathNode(&startFilePath->Header), FILEPATH_DEVICE_PATH, Header);
		}
	}
	else
	{
		pathName															= startFilePath->PathName;
		pathLength															= (DevicePathNodeLength(&startFilePath->Header) - SIZE_OF_FILEPATH_DEVICE_PATH) / sizeof(CHAR16);
		while(!pathName[pathLength - 1] && pathLength)
			pathLength														-= 1;
	}

	CHAR16* lastDirectory													= nullptr;
	for(UINTN i = 0; i < pathLength && pathName[i]; i ++)
	{
		if(pathName[i] == L'\\' || pathName[i] == L'/')
			lastDirectory													= pathName + i;
	}

	if(lastDirectory && !fullPath)
		lastDirectory														+= 1;
	else
		lastDirectory														= pathName;

	CHAR8* retValue															= BlAllocateUtf8FromUnicode(lastDirectory, pathName + pathLength - lastDirectory);
	if(retValue)
		BlConvertPathSeparator(retValue, '/', '\\');
	if(allocatedPathName)
		MmFreePool(allocatedPathName);
	return retValue;
}

//
// check net
//
BOOLEAN DevPathHasMacAddressNode(EFI_DEVICE_PATH_PROTOCOL* devicePath)
{
	while(!EfiIsDevicePathEnd(devicePath))
	{
		if(EfiDevicePathType(devicePath) == MESSAGING_DEVICE_PATH && devicePath->SubType == MSG_MAC_ADDR_DP)
			return TRUE;

		devicePath															= EfiNextDevicePathNode(devicePath);
	}
	return FALSE;
}

//
// get device path protocol
//
EFI_DEVICE_PATH_PROTOCOL* DevPathGetDevicePathProtocol(EFI_HANDLE theHandle)
{
	EFI_DEVICE_PATH_PROTOCOL* retValue										= nullptr;
	EfiBootServices->HandleProtocol(theHandle, &EfiDevicePathProtocolGuid, reinterpret_cast<VOID**>(&retValue));
	return retValue;
}

//
// get partition number
//
UINT32 DevPathGetPartitionNumber(EFI_DEVICE_PATH_PROTOCOL* devicePath)
{
	while(!EfiIsDevicePathEnd(devicePath))
	{
		if(EfiDevicePathType(devicePath) == MEDIA_DEVICE_PATH && devicePath->SubType == MEDIA_HARDDRIVE_DP)
		{
			HARDDRIVE_DEVICE_PATH* hardDriveDevicePath						= _CR(devicePath, HARDDRIVE_DEVICE_PATH, Header);
			if(hardDriveDevicePath->SignatureType == SIGNATURE_TYPE_GUID && hardDriveDevicePath->MBRType == MBR_TYPE_EFI_PARTITION_TABLE_HEADER)
				return hardDriveDevicePath->PartitionNumber;
		}

		devicePath															= EfiNextDevicePathNode(devicePath);
	}
	return static_cast<UINT32>(-1);
}

//
// get partition handle by number
//
EFI_HANDLE DevPathGetPartitionHandleByNumber(EFI_DEVICE_PATH_PROTOCOL* referencePath, UINT32 number)
{
	UINTN nodeCount															= DevPathGetNodeCount(referencePath);
	if(!nodeCount)
		return nullptr;

	UINTN count																= 0;
	EFI_HANDLE* handleArray													= nullptr;
	if(EFI_ERROR(EfiBootServices->LocateHandleBuffer(ByProtocol, &EfiBlockIoProtocolGuid, nullptr, &count, &handleArray)))
		return nullptr;

	EFI_HANDLE retValue														= nullptr;
	for(UINTN i = 0; i < count; i ++)
	{
		EFI_HANDLE theHandle												= handleArray[i];
		if(!theHandle)
			continue;

		EFI_DEVICE_PATH_PROTOCOL* devicePath								= DevPathGetDevicePathProtocol(theHandle);
		if(!devicePath)
			continue;

		if(DevPathGetNodeCount(devicePath) != nodeCount)
			continue;

		EFI_DEVICE_PATH_PROTOCOL* pathA										= referencePath;
		EFI_DEVICE_PATH_PROTOCOL* pathB										= devicePath;
		BOOLEAN checkResult													= TRUE;
		for(UINTN i = 0; i < nodeCount && !EfiIsDevicePathEnd(pathA) && !EfiIsDevicePathEnd(pathB); i ++)
		{
			if(EfiDevicePathNodeLength(pathA) != EfiDevicePathNodeLength(pathB))
			{
				checkResult													= FALSE;
				break;
			}

		#if 0
			if(pathA->Type == MESSAGING_DEVICE_PATH && pathA->SubType == MSG_VENDOR_DP && !memcmp(pathA + 1, &EfiDevicePathMessagingSASGuid, sizeof(EFI_GUID)))
			{
				SAS_DEVICE_PATH* sasA										= _CR(pathA, SAS_DEVICE_PATH, Header);
				SAS_DEVICE_PATH* sasB										= _CR(pathB, SAS_DEVICE_PATH, Header);
				if(sasA->SasAddress != sasB->SasAddress || sasA->Lun != sasB->Lun || sasA->RelativeTargetPort != sasB->RelativeTargetPort)
				{
					checkResult												= FALSE;
					break;
				}
			}
		#endif

			if(memcmp(pathA, pathB, EfiDevicePathNodeLength(pathA)))
			{
				checkResult													= FALSE;
				break;
			}
		}
		if(!checkResult)
			continue;

		if(DevPathGetPartitionNumber(devicePath) != number)
			continue;

		retValue															= theHandle;
		break;
	}

	MmFreePool(handleArray);
	return retValue;
}
