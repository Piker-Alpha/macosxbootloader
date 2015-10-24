//********************************************************************
//	created:	12:11:2009   1:41
//	filename: 	MachO.h
//	author:		tiamo
//	purpose:	mach-o
//********************************************************************

#pragma once

//
// loaded mach-o info
//
typedef struct _MACH_O_LOADED_INFO
{
	//
	// Image base physical address.
	//
	UINT64																	ImageBasePhysicalAddress;

	//
	// __TEXT segment size.
	//
	UINT64																	TextSegmentFileSize;

	//
	// Image base virtual address.
	//
	UINT64																	ImageBaseVirtualAddress;
	
	//
	// Miniman physical address.
	//
	UINT64																	MinPhysicalAddress;

	//
	// Maximum physical address.
	//
	UINT64																	MaxPhysicalAddress;

	//
	// Entry point physical address.
	//
	UINT64																	EntryPointPhysicalAddress;

	//
	// Minimum virtual address.
	//
	UINT64																	MinVirtualAddress;

	//
	// Max virtual address.
	//
	UINT64																	MaxVirtualAddress;

	//
	// Entry point virtual address.
	//
	UINT64																	EntryPointVirtualAddress;

	//
	// Arch type.
	//
	UINT32																	ArchType;
	
	//
	//
	//
	UINT64																	IdlePML4VirtualAddress;
}MACH_O_LOADED_INFO;

//
// get thin fat info
//
EFI_STATUS MachLoadThinFatFile(IO_FILE_HANDLE* fileHandle, UINT64* offsetInFile, UINTN* dataSize);

//
// load mach-o
//
EFI_STATUS MachLoadMachO(IO_FILE_HANDLE* fileHandle, MACH_O_LOADED_INFO* loadedInfo);

//
// get symbol virtual address by name
//
UINT64 MachFindSymbolVirtualAddressByName(MACH_O_LOADED_INFO* loadedInfo, CHAR8 CONST* symbolName);