//********************************************************************
//	created:	8:11:2009   16:39
//	filename: 	BootArgs.h
//	author:		tiamo
//	purpose:	boot arg
//********************************************************************

#pragma once

//
// video
//
#include <pshpack1.h>
typedef struct _BOOT_VIDEO
{
	//
	// vram base address
	//
	UINT32																	BaseAddress;

	//
	// mode,1 = graph,2 = text
	//
	UINT32																	DisplayMode;

	//
	// bytes per row
	//
	UINT32																	BytesPerRow;

	//
	// horz res
	//
	UINT32																	HorzRes;

	//
	// vert res
	//
	UINT32																	VertRes;

	//
	// color depth
	//
	UINT32																	ColorDepth;
}BOOT_VIDEO;

//
// boot arg
//
typedef struct _BOOT_ARGS
{
	//
	// revision
	//
	UINT16																	Revision;

	//
	// version
	//
	UINT16																	Version;

	//
	// efi mode
	//
	UINT8																	EfiMode;

	//
	// debug mode
	//
	UINT8																	DebugMode;

	//
	// flags
	//
	UINT16																	Flags;

	//
	// command line
	//
	CHAR8																	CommandLine[1024];

	//
	// memory map physical address < 4GB
	//
	UINT32																	MemoryMap;

	//
	// memory map size
	//
	UINT32																	MemoryMapSize;

	//
	// memory map descriptor size
	//
	UINT32																	MemoryMapDescriptorSize;

	//
	// memory map descriptor version
	//
	UINT32																	MemoryMapDescriptorVersion;

	//
	// video
	//
	BOOT_VIDEO																BootVideo;

	//
	// device tree physical address < 4GB
	//
	UINT32																	DeviceTree;

	//
	// device tree length
	//
	UINT32																	DeviceTreeLength;

	//
	// kernel start physical address < 4GB
	//
	UINT32																	KernelAddress;

	//
	// kernel size
	//
	UINT32																	KernelSize;

	//
	// efi runtime page start physical address
	//
	UINT32																	EfiRuntimeServicesPageStart;

	//
	// efi runtime page count
	//
	UINT32																	EfiRuntimeServicesPageCount;

	//
	// efi runtime page start virtual address
	//
	UINT64																	EfiRuntimeServicesVirtualPageStart;

	//
	// system table physical address
	//
	UINT32																	EfiSystemTable;

	//
	// ASLR displacement
	//
	UINT32																	ASLRDisplacement;

	//
	// performance data start
	//
	UINT32																	PerformanceDataStart;

	//
	// performance data size
	//
	UINT32																	PerformanceDataSize;

	//
	// key store data start
	//
	UINT32																	KeyStoreDataStart;

	//
	// key store data size
	//
	UINT32																	KeyStoreDataSize;

	//
	// boot mem start
	//
	UINT64																	BootMemStart;

	//
	// boot mem size
	//
	UINT64																	BootMemSize;

	//
	// physical memory size
	//
	UINT64																	PhysicalMemorySize;

	//
	// FSB frequencey
	//
	UINT64																	FSBFrequency;

	//
	// pci config space base address
	//
	UINT64																	PCIConfigSpaceBaseAddress;

	//
	// pci config space start bus number
	//
	UINT32																	PCIConfigSpaceStartBusNumber;

	//
	// pci config space end bus number
	//
	UINT32																	PCIConfigSpaceEndBusNumber;

	//
	// padding
	//
	UINT32																	Reserved3[730];
}BOOT_ARGS;
#include <poppack.h>

//
// add memory range
//
EFI_STATUS BlAddMemoryRangeNode(CHAR8 CONST* rangeName, UINT64 physicalAddress, UINT64 rangeLength);

//
// init boot args
//
EFI_STATUS BlInitializeBootArgs(EFI_DEVICE_PATH_PROTOCOL* bootDevicePath, EFI_DEVICE_PATH_PROTOCOL* bootFilePath, EFI_HANDLE kernelDeviceHandle, CHAR8 CONST* bootCommandLine, BOOT_ARGS** bootArgsP);

//
// finalize boot args
//
EFI_STATUS BlFinalizeBootArgs(BOOT_ARGS* bootArgs, CHAR8 CONST* kernelCommandLine, EFI_HANDLE bootDeviceHandle, struct _MACH_O_LOADED_INFO* loadedInfo);