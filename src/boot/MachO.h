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
	// image base physical address
	//
	UINT64																	ImageBasePhysicalAddress;

	//
	// image base virtual address
	//
	UINT64																	ImageBaseVirtualAddress;

	//
	// min physical address
	//
	UINT64																	MinPhysicalAddress;

	//
	// max physical address
	//
	UINT64																	MaxPhysicalAddress;

	//
	// entry point physical address
	//
	UINT64																	EntryPointPhysicalAddress;

	//
	// min virtual address
	//
	UINT64																	MinVirtualAddress;

	//
	// max virtual address
	//
	UINT64																	MaxVirtualAddress;

	//
	// entry point virtual address
	//
	UINT64																	EntryPointVirtualAddress;

	//
	// arch type
	//
	UINT32																	ArchType;
}MACH_O_LOADED_INFO;

//
// get thin fat info
//
EFI_STATUS MachLoadThinFatFile(IO_FILE_HANDLE* fileHandle, UINT64* offsetInFile, UINTN* dataSize);

//
// load mach-o
//
EFI_STATUS MachLoadMachO(IO_FILE_HANDLE* fileHandle, BOOLEAN useKernelMemory, MACH_O_LOADED_INFO* loadedInfo);

//
// get symbol virtual address by name
//
UINT64 MachFindSymbolVirtualAddressByName(MACH_O_LOADED_INFO* loadedInfo, CHAR8 CONST* symbolName);