//********************************************************************
//	created:	11:11:2009   23:25
//	filename: 	FileIo.h
//	author:		tiamo
//	purpose:	file io
//********************************************************************

#pragma once

//
// open mode
//
#define IO_OPEN_MODE_NORMAL													0
#define IO_OPEN_MODE_KERNEL													1
#define IO_OPEN_MODE_RAMDISK												2

//
// opened file handle
//
typedef struct _IO_FILE_HANDLE
{
	//
	// efi file
	//
	EFI_FILE_HANDLE															EfiFileHandle;

	//
	// load file protocol
	//
	EFI_LOAD_FILE_PROTOCOL*													EfiLoadFileProtocol;

	//
	// file path
	//
	EFI_DEVICE_PATH_PROTOCOL*												EfiFilePath;

	//
	// cache buffer
	//
	UINT8*																	FileBuffer;

	//
	// offset
	//
	UINTN																	FileOffset;

	//
	// size
	//
	UINTN																	FileSize;
}IO_FILE_HANDLE;

//
// detect root
//
EFI_STATUS IoDetectRoot(EFI_HANDLE* deviceHandle, EFI_DEVICE_PATH_PROTOCOL** bootFilePath, BOOLEAN detectBoot);

//
// booting from net
//
BOOLEAN IoBootingFromNet();

//
// open file
//
EFI_STATUS IoOpenFile(CHAR8 CONST* filePathName, EFI_DEVICE_PATH_PROTOCOL* filePath, IO_FILE_HANDLE* fileHandle, UINTN openMode);

//
// set position
//
EFI_STATUS IoSetFilePosition(IO_FILE_HANDLE* fileHandle, UINT64 filePosition);

//
// get file size
//
EFI_STATUS IoGetFileSize(IO_FILE_HANDLE* fileHandle, UINT64* fileSize);

//
// get file info
//
EFI_STATUS IoGetFileInfo(IO_FILE_HANDLE* fileHandle, EFI_FILE_INFO** fileInfo);

//
// read file
//
EFI_STATUS IoReadFile(IO_FILE_HANDLE* fileHandle, VOID* readBuffer, UINTN bufferSize, UINTN* readLength, BOOLEAN directoryFile);

//
// close file
//
VOID IoCloseFile(IO_FILE_HANDLE* fileHandle);

//
// read whole file
//
EFI_STATUS IoReadWholeFile(EFI_DEVICE_PATH_PROTOCOL* bootFilePath, CHAR8 CONST* fileName, CHAR8** fileBuffer, UINTN* fileSize, BOOLEAN asTextFile);

//
// load booter
//
EFI_STATUS IoLoadBooterWithRootUUID(EFI_DEVICE_PATH_PROTOCOL* bootFilePath, CHAR8 CONST* rootUUID, EFI_HANDLE* imageHandle);