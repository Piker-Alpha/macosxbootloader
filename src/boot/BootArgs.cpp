//********************************************************************
//	created:	8:11:2009   16:38
//	filename: 	BootArgs.cpp
//	author:		tiamo
//	purpose:	boot arg
//********************************************************************

#include "stdafx.h"

//
// ram dmg extent info
//
#include <pshpack1.h>
typedef struct _RAM_DMG_EXTENT_INFO
{
	//
	// start
	//
	UINT64																	Start;

	//
	// length
	//
	UINT64																	Length;
}RAM_DMG_EXTENT_INFO;

//
// ram dmg header
//
typedef struct _RAM_DMG_HEADER
{
	//
	// signature
	//
	UINT64																	Signature;

	//
	// version
	//
	UINT32																	Version;

	//
	// extent count
	//
	UINT32																	ExtentCount;

	//
	// extent
	//
	RAM_DMG_EXTENT_INFO														ExtentInfo[0xfe];

	//
	// padding
	//
	UINT64																	Reserved;

	//
	// signature 2
	//
	UINT64																	Signature2;
}RAM_DMG_HEADER;
#include <poppack.h>

//
// global
//
STATIC DEVICE_TREE_NODE* BlpMemoryMapNode									= nullptr;

//
// root UUID from hfs volume header
//
STATIC CHAR8 CONST* BlpRootUUIDFromDisk(EFI_HANDLE deviceHandle, CHAR8* uuidBuffer, UINTN uuidBufferLength)
{
	CHAR8 CONST* rootUUID													= nullptr;

	__try
	{
		//
		// get block io protocol
		//
		EFI_BLOCK_IO_PROTOCOL* blockIoProtocol								= nullptr;
		if(EFI_ERROR(EfiBootServices->HandleProtocol(deviceHandle, &EfiBlockIoProtocolGuid, reinterpret_cast<VOID**>(&blockIoProtocol))))
			try_leave(NOTHING);

		//
		// get disk io protocol
		//
		EFI_DISK_IO_PROTOCOL* diskIoProtocol								= nullptr;
		if(EFI_ERROR(EfiBootServices->HandleProtocol(deviceHandle, &EfiDiskIoProtocolGuid, reinterpret_cast<VOID**>(&diskIoProtocol))))
			try_leave(NOTHING);

		//
		// read volume header
		//
		STATIC UINT8 volumeHeader[0x200]									= {0};
		if(EFI_ERROR(diskIoProtocol->ReadDisk(diskIoProtocol, blockIoProtocol->Media->MediaId, 1024, sizeof(volumeHeader), volumeHeader)))
			try_leave(NOTHING);

		//
		// hfs+ volume
		//
		if(volumeHeader[0] == 'B' && volumeHeader[1] == 'D' && volumeHeader[0x7c] == 'H' && volumeHeader[0x7d] == '+')
		{
			UINT64 volumeHeaderOffset										= ((volumeHeader[0x1d] + (volumeHeader[0x1c] << 8)) << 9) + 1024;
			UINT16 startBlock												= volumeHeader[0x7f] + (volumeHeader[0x7e] << 8);
			UINT32 blockSize												= volumeHeader[0x17] + (volumeHeader[0x16] << 8) + (volumeHeader[0x15] << 16) + (volumeHeader[0x14] << 24);
			volumeHeaderOffset												+= startBlock * blockSize;

			if(EFI_ERROR(diskIoProtocol->ReadDisk(diskIoProtocol, blockIoProtocol->Media->MediaId, volumeHeaderOffset, sizeof(volumeHeader), volumeHeader)))
				try_leave(NOTHING);
		}

		//
		// check hfs+ volume header
		//
		UINT8 volumeId[8]													= {0};
		if(volumeHeader[0] == 'H' && (volumeHeader[1] == '+' || volumeHeader[1] == 'X'))
			memcpy(volumeId, volumeHeader + 0x68, sizeof(volumeId));
		else
			try_leave(NOTHING);

		//
		// just like AppleFileSystemDriver
		//
		UINT8 md5Result[0x10]												= {0};
		UINT8 prefixBuffer[0x10]											= {0xb3, 0xe2, 0x0f, 0x39, 0xf2, 0x92, 0x11, 0xd6, 0x97, 0xa4, 0x00, 0x30, 0x65, 0x43, 0xec, 0xac};
		MD5_CONTEXT md5Context;
		MD5Init(&md5Context);
		MD5Update(&md5Context, prefixBuffer, sizeof(prefixBuffer));
		MD5Update(&md5Context, volumeId, sizeof(volumeId));
		MD5Final(md5Result, &md5Context);

		//
		// this UUID has been made version 3 style (i.e. via namespace)
		// see "-uuid-urn-" IETF draft (which otherwise copies byte for byte)
		//
		CHAR8* dstBuffer													= uuidBuffer;
		md5Result[6]														= 0x30 | (md5Result[6] & 0x0f);
		md5Result[8]														= 0x80 | (md5Result[8] & 0x3f);
		UINTN formatBase													= 0;
		UINT8 uuidFormat[]													= {4, 2, 2, 2, 6};
		for(UINTN formatIndex = 0; formatIndex < ARRAYSIZE(uuidFormat); formatIndex ++)
		{
			UINTN i;
			for(i = 0; i < uuidFormat[formatIndex]; i++)
			{
				UINT8 byteValue												= md5Result[formatBase + i];
				CHAR8 nibValue												= byteValue >> 4;

				*dstBuffer													= nibValue + '0';
				if(*dstBuffer > '9')
					*dstBuffer												= (nibValue - 9 + ('A' - 1));

				dstBuffer													+= 1;

				nibValue													= byteValue & 0xf;
				*dstBuffer													= nibValue + '0';
				if(*dstBuffer > '9')
					*dstBuffer												= (nibValue - 9 + ('A' - 1));

				dstBuffer													+= 1;
			}

			formatBase														+= i;
			if(formatIndex < ARRAYSIZE(uuidFormat) - 1)
			{
				*dstBuffer													= '-';
				dstBuffer													+= 1;
			}
			else
			{
				*dstBuffer													= 0;
			}
		}

		rootUUID															= uuidBuffer;
	}
	__finally
	{
	}

	return rootUUID;
}

//
// root uuid from device path
//
STATIC CHAR8 CONST* BlpRootUUIDFromDevicePath(EFI_HANDLE deviceHandle, CHAR8* uuidBuffer, UINTN uuidBufferLength)
{
	EFI_DEVICE_PATH_PROTOCOL* devicePath									= nullptr;
	if(EFI_ERROR(EfiBootServices->HandleProtocol(deviceHandle, &EfiDevicePathProtocolGuid, reinterpret_cast<VOID**>(&devicePath))))
		return nullptr;

	HARDDRIVE_DEVICE_PATH* hardDriverDevicePath								= _CR(DevPathGetNode(devicePath, MEDIA_DEVICE_PATH, MEDIA_HARDDRIVE_DP), HARDDRIVE_DEVICE_PATH, Header);
	if(!hardDriverDevicePath || hardDriverDevicePath->MBRType != MBR_TYPE_EFI_PARTITION_TABLE_HEADER || hardDriverDevicePath->SignatureType != SIGNATURE_TYPE_GUID)
		return nullptr;

	EFI_GUID g;
	memcpy(&g, hardDriverDevicePath->Signature, sizeof(g));
	CHAR8 CONST* formatString												= CHAR8_CONST_STRING("%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X");
	snprintf(uuidBuffer, uuidBufferLength, formatString, g.Data1, g.Data2, g.Data3, g.Data4[0], g.Data4[1], g.Data4[2], g.Data4[3],g.Data4[4],g.Data4[5],g.Data4[6],g.Data4[7]);

	return uuidBuffer;
}

//
// add ram dmg property
//
STATIC VOID BlpAddRamDmgProperty(DEVICE_TREE_NODE* chosenNode, EFI_DEVICE_PATH_PROTOCOL* bootDevicePath)
{
	__try
	{
		//
		// get mem map device path
		//
		MEMMAP_DEVICE_PATH* memMapDevicePath								= nullptr;
		MEMMAP_DEVICE_PATH* ramDmgDevicePath								= nullptr;
		UINT64* ramDmgSize													= nullptr;
		while(!EfiIsDevicePathEnd(bootDevicePath))
		{
			if(EfiDevicePathType(bootDevicePath) == HARDWARE_DEVICE_PATH && bootDevicePath->SubType == HW_MEMMAP_DP)
			{
				memMapDevicePath											= _CR(bootDevicePath, MEMMAP_DEVICE_PATH, Header);
			}
			else if(EfiDevicePathType(bootDevicePath) == MESSAGING_DEVICE_PATH && bootDevicePath->SubType == MSG_VENDOR_DP)
			{
				VENDOR_DEVICE_PATH* vendorDevicePath						= _CR(bootDevicePath, VENDOR_DEVICE_PATH, Header);
				if(!memcmp(&vendorDevicePath->Guid, &AppleRamDmgDevicePathGuid, sizeof(EFI_GUID)))
				{
					ramDmgSize												= reinterpret_cast<UINT64*>(vendorDevicePath + 1);
					ramDmgDevicePath										= memMapDevicePath;
					break;
				}
			}
			bootDevicePath													= EfiNextDevicePathNode(bootDevicePath);
		}
		if(!ramDmgDevicePath)
			try_leave(NOTHING);

		//
		// check length
		//
		if(ramDmgDevicePath->EndingAddress + 1 - ramDmgDevicePath->StartingAddress < sizeof(RAM_DMG_HEADER))
			try_leave(NOTHING);

		//
		// check header
		//
		RAM_DMG_HEADER* ramDmgHeader										= ArchConvertAddressToPointer(ramDmgDevicePath->StartingAddress, RAM_DMG_HEADER*);
		if(ramDmgHeader->Signature != ramDmgHeader->Signature2 || ramDmgHeader->Signature != 0x544E5458444D4152ULL || ramDmgHeader->Version != 0x10000)
			try_leave(NOTHING);

		//
		// check size
		//
		if(ramDmgHeader->ExtentCount > ARRAYSIZE(ramDmgHeader->ExtentInfo) || !*ramDmgSize)
			try_leave(NOTHING);

		DevTreeAddProperty(chosenNode, CHAR8_CONST_STRING("boot-ramdmg-extents"), ramDmgHeader->ExtentInfo, ramDmgHeader->ExtentCount * sizeof(RAM_DMG_EXTENT_INFO), FALSE);
		DevTreeAddProperty(chosenNode, CHAR8_CONST_STRING("boot-ramdmg-size"), ramDmgSize, sizeof(UINT64), FALSE);
	}
	__finally
	{

	}
}

//
// add memory range
//
EFI_STATUS BlAddMemoryRangeNode(CHAR8 CONST* rangeName, UINT64 physicalAddress, UINT64 rangeLength)
{
	UINT32 propertyBuffer[2]												= {static_cast<UINT32>(physicalAddress), static_cast<UINT32>(rangeLength)};
	return DevTreeAddProperty(BlpMemoryMapNode, rangeName, propertyBuffer, sizeof(propertyBuffer), TRUE);
}

//
// init boot args
//
EFI_STATUS BlInitializeBootArgs(EFI_DEVICE_PATH_PROTOCOL* bootDevicePath, EFI_DEVICE_PATH_PROTOCOL* bootFilePath, EFI_HANDLE bootDeviceHandle, CHAR8 CONST* kernelCommandLine, BOOT_ARGS** bootArgsP)
{
	EFI_STATUS status														= EFI_SUCCESS;
	*bootArgsP																= nullptr;
	UINT64 virtualAddress													= 0;
	UINT64 physicalAddress													= 0;

	__try
	{
		//
		// detect acpi nvs memory
		//
		AcpiDetectNVSMemory();

		//
		// allocate memory
		//
		UINTN bufferLength													= sizeof(BOOT_ARGS);
		physicalAddress														= MmAllocateKernelMemory(&bufferLength, &virtualAddress);
		if(!physicalAddress)
			try_leave(status = EFI_OUT_OF_RESOURCES);

		//
		// fill common fileds
		//
		BOOT_ARGS* bootArgs													= ArchConvertAddressToPointer(physicalAddress, BOOT_ARGS*);
		memset(bootArgs, 0, sizeof(BOOT_ARGS));
		bootArgs->Revision													= 0;
		bootArgs->Version													= 2;
		bootArgs->EfiMode													= ArchNeedEFI64Mode() ? 64 : 32;
		bootArgs->DebugMode													= 0;
		bootArgs->Flags														= 1;	// kBootArgsFlagRebootOnPanic
		bootArgs->PhysicalMemorySize										= BlGetMemorySize();
		bootArgs->ASLRDisplacement											= static_cast<UINT32>(LdrGetASLRDisplacement());

		//
		// read fsb frequency
		//
		DEVICE_TREE_NODE* platformNode										= DevTreeFindNode(CHAR8_CONST_STRING("/efi/platform"), FALSE);
		if(platformNode)
		{
			UINT32 length													= 0;
			VOID CONST* fsbFrequency										= DevTreeGetProperty(platformNode, CHAR8_CONST_STRING("FSBFrequency"), &length);
			if(length >= 8)
				bootArgs->FSBFrequency										= *static_cast<UINT64 CONST*>(fsbFrequency);
			else if(length >= 4)
				bootArgs->FSBFrequency										= *static_cast<UINT32 CONST*>(fsbFrequency);
		}

		//
		// get pci config space info
		//
		AcpiGetPciConfigSpaceInfo(&bootArgs->PCIConfigSpaceBaseAddress, &bootArgs->PCIConfigSpaceStartBusNumber, &bootArgs->PCIConfigSpaceEndBusNumber);

		//
		// get root node
		//
		DEVICE_TREE_NODE* rootNode											= DevTreeFindNode(CHAR8_CONST_STRING("/"), FALSE);
		if(!rootNode)
			try_leave(status = EFI_OUT_OF_RESOURCES);

		//
		// compatible = ACPI
		//
		if(EFI_ERROR(status = DevTreeAddProperty(rootNode, CHAR8_CONST_STRING("compatible"), "ACPI", 5, FALSE)))
			try_leave(NOTHING);

		//
		// model = ACPI
		//
		if(EFI_ERROR(status = DevTreeAddProperty(rootNode, CHAR8_CONST_STRING("model"), "ACPI", 5, FALSE)))
			try_leave(NOTHING);

		//
		// get chosen node
		//
		DEVICE_TREE_NODE* chosenNode										= DevTreeFindNode(CHAR8_CONST_STRING("/chosen"), TRUE);
		if(!chosenNode)
			try_leave(status = EFI_OUT_OF_RESOURCES);

		//
		// get memory map node
		//
		BlpMemoryMapNode													= DevTreeFindNode(CHAR8_CONST_STRING("/chosen/memory-map"), TRUE);
		if(!BlpMemoryMapNode)
			try_leave(status = EFI_OUT_OF_RESOURCES);

		//
		// get boot guid
		//
		if(!CmGetStringValueForKeyAndCommandLine(kernelCommandLine, CHAR8_CONST_STRING("rd"), nullptr, FALSE))
		{
			//
			// get root match
			//
			CHAR8 CONST* rootMatchDict										= CmSerializeValueForKey(CHAR8_CONST_STRING("Root Match"), nullptr);
			if(!rootMatchDict)
			{
				//
				// root match dict not found,try root UUID
				//
				CHAR8 uuidBuffer[0x41]										= {0};
				CHAR8 const* rootUUID										= CmGetStringValueForKey(nullptr, CHAR8_CONST_STRING("Root UUID"), nullptr);
				if(!rootUUID)
				{
					//
					// check net boot
					//
					rootMatchDict											= NetGetRootMatchDict(bootDevicePath);
					if(rootMatchDict)
					{
						//
						// set net-boot flags
						//
						DevTreeAddProperty(rootNode, CHAR8_CONST_STRING("net-boot"), nullptr, 0, FALSE);
					}
					else
					{
						//
						// build uuid by md5(hfs volume header)
						//
						rootUUID											= BlpRootUUIDFromDisk(bootDeviceHandle, uuidBuffer, ARRAYSIZE(uuidBuffer) - 1);

						//
						// extract uuid from device path
						//
						if(!rootUUID)
							rootUUID										= BlpRootUUIDFromDevicePath(bootDeviceHandle, uuidBuffer, ARRAYSIZE(uuidBuffer) - 1);
					}
				}

				//
				// add root uuid
				//
				if(rootUUID)
					DevTreeAddProperty(chosenNode, CHAR8_CONST_STRING("boot-uuid"), rootUUID, static_cast<UINT32>(strlen(rootUUID) + 1) * sizeof(CHAR8), TRUE);
			}

			//
			// add root match
			//
			if(rootMatchDict)
			{
				DevTreeAddProperty(chosenNode, CHAR8_CONST_STRING("root-matching"), rootMatchDict, static_cast<UINT32>(strlen(rootMatchDict) + 1) * sizeof(CHAR8), TRUE);
				MmFreePool(const_cast<CHAR8*>(rootMatchDict));
			}
		}

		//
		// add kernel file name chosen node
		//
		CHAR8 CONST* bootFileName											= LdrGetKernelPathName();
		if(!bootFileName)
			bootFileName													= LdrGetKernelCachePathName();
		if(EFI_ERROR(status = DevTreeAddProperty(chosenNode, CHAR8_CONST_STRING("boot-file"), bootFileName, static_cast<UINT32>(strlen(bootFileName) + 1) * sizeof(CHAR8), FALSE)))
			try_leave(NOTHING);

		//
		// add boot device path
		//
		if(EFI_ERROR(status = DevTreeAddProperty(chosenNode, CHAR8_CONST_STRING("boot-device-path"), bootDevicePath, static_cast<UINT32>(DevPathGetSize(bootDevicePath)), FALSE)))
			try_leave(NOTHING);

		//
		// add boot file path
		//
		if(EFI_ERROR(status = DevTreeAddProperty(chosenNode, CHAR8_CONST_STRING("boot-file-path"), bootFilePath, static_cast<UINT32>(DevPathGetSize(bootFilePath)), FALSE)))
			try_leave(NOTHING);

		//
		// add ram dmg info
		//
		BlpAddRamDmgProperty(chosenNode, bootDevicePath);

		//
		// output
		//
		*bootArgsP															= bootArgs;
	}
	__finally
	{
		if(EFI_ERROR(status))
			MmFreeKernelMemory(virtualAddress, physicalAddress);
	}

	return status;
}

//
// finalize boot args
//
EFI_STATUS BlFinalizeBootArgs(BOOT_ARGS* bootArgs, CHAR8 CONST* kernelCommandLine, EFI_HANDLE bootDeviceHandle, MACH_O_LOADED_INFO* loadedInfo)
{
	EFI_STATUS status														= EFI_SUCCESS;

	__try
	{
		//
		// save command line
		//
		strncpy(bootArgs->CommandLine, kernelCommandLine, ARRAYSIZE(bootArgs->CommandLine) - 1);

		//
		// get chosen node
		//
		DEVICE_TREE_NODE* chosenNode										= DevTreeFindNode(CHAR8_CONST_STRING("/chosen"), TRUE);
		if(!chosenNode)
			try_leave(status = EFI_OUT_OF_RESOURCES);

		//
		// add boot-args
		//
		if(EFI_ERROR(status = DevTreeAddProperty(chosenNode, CHAR8_CONST_STRING("boot-args"), bootArgs->CommandLine, static_cast<UINT32>(strlen(bootArgs->CommandLine) + 1) * sizeof(CHAR8), FALSE)))
			try_leave(NOTHING);

		//
		// net
		//
		if(EFI_ERROR(status = NetSetupDeviceTree(bootDeviceHandle)))
			try_leave(NOTHING);

		//
		// pe
		//
		if(EFI_ERROR(status = PeSetupDeviceTree()))
			try_leave(NOTHING);

		//
		// FileVault2, key store
		//
		if(BlTestBootMode(BOOT_MODE_HAS_FILE_VAULT2_CONFIG))
		{
			UINT64 keyStorePhysicalAddress									= 0;
			UINTN keyStoreDataSize											= 0;
			if(EFI_ERROR(status = FvSetupDeviceTree(&keyStorePhysicalAddress, &keyStoreDataSize, TRUE)))
				try_leave(NOTHING);

			bootArgs->KeyStoreDataStart										= static_cast<UINT32>(keyStorePhysicalAddress);
			bootArgs->KeyStoreDataSize										= static_cast<UINT32>(keyStoreDataSize);
		}
			

		//
		// console device tree
		//
		if(EFI_ERROR(status = CsSetupDeviceTree(bootArgs)))
			try_leave(NOTHING);

		//
		// get device tree size
		//
		UINT32 deviceTreeSize												= 0;
		DevTreeFlatten(nullptr, &deviceTreeSize);

		//
		// allocate buffer
		//
		UINT64 virtualAddress												= 0;
		UINTN bufferLength													= deviceTreeSize;
		UINT64 physicalAddress												= MmAllocateKernelMemory(&bufferLength, &virtualAddress);
		if(!physicalAddress)
			try_leave(status = EFI_OUT_OF_RESOURCES);

		//
		// flatten it
		//
		VOID* flattenBuffer													= ArchConvertAddressToPointer(physicalAddress, VOID*);
		if(EFI_ERROR(status = DevTreeFlatten(&flattenBuffer, &deviceTreeSize)))
			try_leave(NOTHING);

		//
		// save it
		//
		bootArgs->DeviceTree												= static_cast<UINT32>(physicalAddress);
		bootArgs->DeviceTreeLength											= deviceTreeSize;

		//
		// free device tree
		//
		DevTreeFinalize();

		//
		// get memory map
		//
		UINTN memoryMapSize													= 0;
		EFI_MEMORY_DESCRIPTOR* memoryMap									= nullptr;
		UINTN memoryMapKey													= 0;
		UINTN descriptorSize												= 0;
		UINT32 descriptorVersion											= 0;
		if(EFI_ERROR(status = MmGetMemoryMap(&memoryMapSize, &memoryMap, &memoryMapKey, &descriptorSize, &descriptorVersion)))
			try_leave(NOTHING);

		//
		// get runtime memory info
		//
		UINT64 runtimeMemoryPages											= 0;
		UINTN runtimeMemoryDescriptors										= MmGetRuntimeMemoryInfo(memoryMap, memoryMapSize, descriptorSize, &runtimeMemoryPages);

		//
		// free memory map
		//
		MmFreePool(memoryMap);

		//
		// allocate kernel memory for runtime area
		//
		runtimeMemoryPages													+= 1;
		runtimeMemoryDescriptors											+= 1;
		UINT64 runtimeServicesVirtualAddress								= 0;
		bufferLength														= static_cast<UINTN>(runtimeMemoryPages << EFI_PAGE_SHIFT);
		UINT64 runtimeServicesPhysicalAddress								= MmAllocateKernelMemory(&bufferLength, &runtimeServicesVirtualAddress);
		if(!runtimeServicesPhysicalAddress)
			try_leave(status = EFI_OUT_OF_RESOURCES);

		//
		// allocate memory map
		//
		memoryMapSize														+= runtimeMemoryDescriptors * descriptorSize + 512;
		UINT64 memoryMapVirtualAddress										= 0;
		UINT64 memoryMapPhysicalAddress										= MmAllocateKernelMemory(&memoryMapSize, &memoryMapVirtualAddress);
		if(!memoryMapPhysicalAddress)
			try_leave(status = EFI_OUT_OF_RESOURCES);

		//
		// get memory map
		//
		for(UINTN i = 0; i < 5; i ++)
		{
			UINTN currentSize												= 0;
			status															= EfiBootServices->GetMemoryMap(&currentSize, 0, &memoryMapKey, &descriptorSize, &descriptorVersion);
			if(status != EFI_BUFFER_TOO_SMALL)
				try_leave(NOTHING);

			if(currentSize > memoryMapSize)
			{
				MmFreeKernelMemory(memoryMapVirtualAddress, memoryMapPhysicalAddress);
				currentSize													+= (runtimeMemoryDescriptors + 2) * descriptorSize + 512;
				memoryMapPhysicalAddress									= MmAllocateKernelMemory(&currentSize, &memoryMapVirtualAddress);
				memoryMapSize												= currentSize;
			}

			memoryMap														= ArchConvertAddressToPointer(memoryMapPhysicalAddress, EFI_MEMORY_DESCRIPTOR*);
			status															= EfiBootServices->GetMemoryMap(&currentSize, memoryMap, &memoryMapKey, &descriptorSize, &descriptorVersion);
			if(!EFI_ERROR(status))
			{
				memoryMapSize												= currentSize;
				break;
			}
		}

		//
		// unable to get memory map
		//
		if(EFI_ERROR(status))
			try_leave(NOTHING);

		//
		// save it
		//
		bootArgs->MemoryMap													= static_cast<UINT32>(memoryMapPhysicalAddress);
		bootArgs->MemoryMapDescriptorSize									= static_cast<UINT32>(descriptorSize);
		bootArgs->MemoryMapDescriptorVersion								= static_cast<UINT32>(descriptorVersion);
		bootArgs->MemoryMapSize												= static_cast<UINT32>(memoryMapSize);

		//
		// exit boot services
		//
		if(EFI_ERROR(status = EfiBootServices->ExitBootServices(EfiImageHandle, memoryMapKey)))
			try_leave(NOTHING);

		//
		// adjust memory map
		//
		memoryMapSize														= AcpiAdjustMemoryMap(memoryMap, memoryMapSize, descriptorSize);
		bootArgs->MemoryMapSize												= static_cast<UINT32>(memoryMapSize);

		//
		// sort memory map
		//
		MmSortMemoryMap(memoryMap, memoryMapSize, descriptorSize);

		//
		// convert pointer
		//
		UINT64 efiSystemTablePhysicalAddress								= ArchConvertPointerToAddress(EfiSystemTable);
		MmConvertPointers(memoryMap, &memoryMapSize, descriptorSize, descriptorVersion, runtimeServicesPhysicalAddress, runtimeMemoryPages, runtimeServicesVirtualAddress, &efiSystemTablePhysicalAddress, TRUE, loadedInfo);

		//
		// sort memory map
		//
		MmSortMemoryMap(memoryMap, memoryMapSize, descriptorSize);

		//
		// save efi services
		//
		bootArgs->MemoryMapSize												= static_cast<UINT32>(memoryMapSize);
		bootArgs->EfiSystemTable											= static_cast<UINT32>(efiSystemTablePhysicalAddress);
		bootArgs->EfiRuntimeServicesPageCount								= static_cast<UINT32>(runtimeMemoryPages);
		bootArgs->EfiRuntimeServicesPageStart								= static_cast<UINT32>(runtimeServicesPhysicalAddress >> EFI_PAGE_SHIFT);
		bootArgs->EfiRuntimeServicesVirtualPageStart						= runtimeServicesVirtualAddress >> EFI_PAGE_SHIFT;

		//
		// save kernel range
		//
		UINT64 kernelBegin													= 0;
		UINT64 kernelEnd													= 0;
		MmGetKernelPhysicalRange(&kernelBegin, &kernelEnd);
		bootArgs->KernelAddress												= static_cast<UINT32>(kernelBegin);
		bootArgs->KernelSize												= static_cast<UINT32>(kernelEnd - kernelBegin);

		//
		// clear serives pointer
		//
		EfiBootServices														= nullptr;
	}
	__finally
	{
	}

	return status;
}