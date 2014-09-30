//********************************************************************
//	created:	8:11:2009   6:38
//	filename: 	DevicePath.h
//	author:		tiamo
//	purpose:	device path
//********************************************************************

#pragma once

//
// get device path size
//
UINTN DevPathGetSize(EFI_DEVICE_PATH_PROTOCOL* devicePath);

//
// get node count
//
UINTN DevPathGetNodeCount(EFI_DEVICE_PATH_PROTOCOL* devicePath);

//
// get node
//
EFI_DEVICE_PATH_PROTOCOL* DevPathGetNode(EFI_DEVICE_PATH_PROTOCOL* devicePath, UINT8 type, UINT8 subType);

//
// copy device path
//
EFI_DEVICE_PATH_PROTOCOL* DevPathDuplicate(EFI_DEVICE_PATH_PROTOCOL* devicePath);

//
// append device path
//
EFI_DEVICE_PATH_PROTOCOL* DevPathAppendDevicePath(EFI_DEVICE_PATH_PROTOCOL* devicePath, EFI_DEVICE_PATH_PROTOCOL* appendPath);

//
// append file path
//
EFI_DEVICE_PATH_PROTOCOL* DevPathAppendFilePath(EFI_DEVICE_PATH_PROTOCOL* devicePath, CHAR16 CONST* fileName);

//
// append file path
//
EFI_DEVICE_PATH_PROTOCOL* DevPathAppendLastComponent(EFI_DEVICE_PATH_PROTOCOL* devicePath, CHAR8 CONST* fileName, BOOLEAN replaceLastComponent);

//
// extract file path name
//
CHAR8* DevPathExtractFilePathName(EFI_DEVICE_PATH_PROTOCOL* devicePath, BOOLEAN fullPath);

//
// check net
//
BOOLEAN DevPathHasMacAddressNode(EFI_DEVICE_PATH_PROTOCOL* devicePath);

//
// get device path protocol
//
EFI_DEVICE_PATH_PROTOCOL* DevPathGetDevicePathProtocol(EFI_HANDLE theHandle);

//
// get partition number
//
UINT32 DevPathGetPartitionNumber(EFI_DEVICE_PATH_PROTOCOL* devicePath);

//
// get partition handle by number
//
EFI_HANDLE DevPathGetPartitionHandleByNumber(EFI_DEVICE_PATH_PROTOCOL* referencePath, UINT32 number);