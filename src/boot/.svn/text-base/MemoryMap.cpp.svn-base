//********************************************************************
//	created:	8:11:2009   23:05
//	filename: 	MemMap.cpp
//	author:		tiamo
//	purpose:	memory map
//********************************************************************

#include "stdafx.h"

//
// get memory map
//
EFI_STATUS MmGetMemoryMap(UINTN* memoryMapSize, EFI_MEMORY_DESCRIPTOR** memoryMap, UINTN* memoryMapKey, UINTN* descriptorSize, UINT32* descriptorVersion)
{
	EFI_STATUS status														= EFI_SUCCESS;
	EFI_MEMORY_DESCRIPTOR* allocatedMap										= nullptr;
	UINTN allocatedSize														= 0;

	//
	// reties
	//
	for(UINTN i = 0; i < 5; i ++)
	{
		//
		// get size
		//
		UINTN currentSize													= 0;
		status																= EfiBootServices->GetMemoryMap(&currentSize, nullptr, memoryMapKey, descriptorSize, descriptorVersion);
		if(status != EFI_BUFFER_TOO_SMALL)
			return status;

		//
		// reallocate buffer
		//
		if(currentSize > allocatedSize)
		{
			if(allocatedMap)
				MmFreePool(allocatedMap);

			allocatedSize													= currentSize + 512;
			currentSize														= allocatedSize;
			allocatedMap													= static_cast<EFI_MEMORY_DESCRIPTOR*>(MmAllocatePool(allocatedSize));
			if(!allocatedMap)
				return EFI_OUT_OF_RESOURCES;
		}

		//
		// get map
		//
		status																= EfiBootServices->GetMemoryMap(&currentSize, allocatedMap, memoryMapKey, descriptorSize, descriptorVersion);
		if(!EFI_ERROR(status))
		{
			*memoryMap														= allocatedMap;
			*memoryMapSize													= currentSize;
			break;
		}
	}

	return status;
}

//
// get runtime memory info
//
UINTN MmGetRuntimeMemoryInfo(EFI_MEMORY_DESCRIPTOR* memoryMap, UINTN memoryMapSize, UINTN descriptorSize, UINT64* totalPages)
{
	UINTN totalDescriptors													= 0;
	*totalPages																= 0;
	for(UINTN i = 0; i < memoryMapSize; i += descriptorSize, memoryMap = NextMemoryDescriptor(memoryMap, descriptorSize))
	{
		if(!(memoryMap->Attribute & EFI_MEMORY_RUNTIME))
			continue;

		totalDescriptors													+= 1;
		*totalPages															+= memoryMap->NumberOfPages;
	}

	return totalDescriptors;
}

//
// remove non runtime descriptors
//
EFI_STATUS MmRemoveNonRuntimeDescriptors(EFI_MEMORY_DESCRIPTOR* memoryMap, UINTN* memoryMapSize, UINTN descriptorSize)
{
	UINTN newSize															= 0;
	EFI_MEMORY_DESCRIPTOR* writePosition									= memoryMap;
	for(UINTN i = 0; i < *memoryMapSize; i += descriptorSize, memoryMap = NextMemoryDescriptor(memoryMap, descriptorSize))
	{
		if(memoryMap->Type == EfiReservedMemoryType || !(memoryMap->Attribute & EFI_MEMORY_RUNTIME))
			continue;

		if(writePosition != memoryMap)
			*writePosition													= *memoryMap;

		writePosition														= NextMemoryDescriptor(writePosition, descriptorSize);
		newSize																+= descriptorSize;
	}

	*memoryMapSize															= newSize;
	return EFI_SUCCESS;
}

//
// sort
//
VOID MmSortMemoryMap(EFI_MEMORY_DESCRIPTOR* memoryMap, UINTN memoryMapSize, UINTN descriptorSize)
{
	EFI_MEMORY_DESCRIPTOR* localDescriptor									= static_cast<EFI_MEMORY_DESCRIPTOR*>(_alloca(descriptorSize));
	EFI_MEMORY_DESCRIPTOR* curDescriptor									= NextMemoryDescriptor(memoryMap, descriptorSize);
	for(UINTN i = descriptorSize; i < memoryMapSize; i += descriptorSize, curDescriptor = NextMemoryDescriptor(curDescriptor, descriptorSize))
	{
		EFI_MEMORY_DESCRIPTOR* testDescriptor								= memoryMap;
		for(UINTN j = 0; j < i; j += descriptorSize, testDescriptor = NextMemoryDescriptor(testDescriptor, descriptorSize))
		{
			if(testDescriptor->PhysicalStart <= curDescriptor->PhysicalStart)
				continue;

			memcpy(localDescriptor, curDescriptor, descriptorSize);

			EFI_MEMORY_DESCRIPTOR* prevDescriptor							= NextMemoryDescriptor(curDescriptor, -static_cast<INTN>(descriptorSize));
			EFI_MEMORY_DESCRIPTOR* thisDescriptor							= curDescriptor;
			for(UINTN k = i - descriptorSize; static_cast<INTN>(k - j) >= 0; k -= descriptorSize, thisDescriptor = prevDescriptor, prevDescriptor = NextMemoryDescriptor(prevDescriptor, -static_cast<INTN>(descriptorSize)))
				memcpy(thisDescriptor, prevDescriptor, descriptorSize);

			memcpy(testDescriptor, localDescriptor, descriptorSize);
			break;
		}
	}
}

//
// create sub range
//
VOID MmpCreateSubRegion(EFI_MEMORY_DESCRIPTOR* memoryMap, UINTN* memoryMapSize, UINTN descriptorSize, UINT32 descriptorVersion, EFI_MEMORY_DESCRIPTOR** createdDescriptor, UINT64 newPhysicalAddress, UINT64 pagesCount)
{
	EFI_MEMORY_DESCRIPTOR* curDescriptor									= memoryMap;
	UINT64 length															= pagesCount << EFI_PAGE_SHIFT;
	for(UINTN i = 0; i < *memoryMapSize; i += descriptorSize, curDescriptor = NextMemoryDescriptor(curDescriptor, descriptorSize))
	{
		if(curDescriptor->PhysicalStart > newPhysicalAddress || newPhysicalAddress >= curDescriptor->PhysicalStart + (curDescriptor->NumberOfPages << EFI_PAGE_SHIFT))
			continue;

		UINT64 curPagesCount												= curDescriptor->NumberOfPages;
		EFI_MEMORY_DESCRIPTOR* newDescriptor								= Add2Ptr(memoryMap, *memoryMapSize, EFI_MEMORY_DESCRIPTOR*);

		if(newPhysicalAddress > curDescriptor->PhysicalStart)
		{
			UINT64 newPagesCount											= EFI_SIZE_TO_PAGES(newPhysicalAddress - curDescriptor->PhysicalStart);
			memcpy(newDescriptor, curDescriptor, descriptorSize);
			newDescriptor->NumberOfPages									= newPagesCount;
			UINT64 length													= newPagesCount << EFI_PAGE_SHIFT;
			curDescriptor->PhysicalStart									+= length;
			curDescriptor->NumberOfPages									-= newPagesCount;
			curPagesCount													-= newPagesCount;
			if(curDescriptor->VirtualStart)
				curDescriptor->VirtualStart									+= length;

			newDescriptor													= NextMemoryDescriptor(newDescriptor, descriptorSize);
			*memoryMapSize													+= descriptorSize;
		}

		if(curPagesCount > pagesCount)
		{
			memcpy(newDescriptor, curDescriptor, descriptorSize);
			curDescriptor->NumberOfPages									= pagesCount;
			newDescriptor->NumberOfPages									= curPagesCount - pagesCount;
			newDescriptor->PhysicalStart									+= length;
			if(newDescriptor->VirtualStart)
				newDescriptor->VirtualStart									+= length;

			*memoryMapSize													+= descriptorSize;
		}

		*createdDescriptor													= curDescriptor;
		break;
	}
}

//
// convert pointers
//
EFI_STATUS MmConvertPointers(EFI_MEMORY_DESCRIPTOR* memoryMap, UINTN* memoryMapSize, UINTN descSize, UINT32 descVersion, UINT64 rtPhysical, UINT64 runtimePages, UINT64 rtVirtual, UINT64* efiSysTablePhy, BOOLEAN createRegion, MACH_O_LOADED_INFO* loadedInfo)
{
	EFI_MEMORY_DESCRIPTOR* curDescriptor									= memoryMap;
	UINT64 curVirtualAddress												= rtVirtual;
	UINT64 runtimeServicesPhyAddrEnd										= rtPhysical + (runtimePages << EFI_PAGE_SHIFT);
	UINT64 mappedRuntimePages												= 0;
	BOOLEAN setupThunkCode													= curVirtualAddress > static_cast<UINTN>(-1);
	UINT64 thunkOffset														= setupThunkCode ? curVirtualAddress - LdrStaticVirtualToPhysical(curVirtualAddress) : 0;

	//
	// setup virtual address
	//
	for(UINTN i = 0; i < *memoryMapSize; i += descSize, curDescriptor = NextMemoryDescriptor(curDescriptor, descSize))
	{
		if(curDescriptor->Type == EfiReservedMemoryType)
			curDescriptor->Attribute										&= ~EFI_MEMORY_RUNTIME;

		if(!(curDescriptor->Attribute & EFI_MEMORY_RUNTIME))
			continue;

		if(LdrStaticVirtualToPhysical(curVirtualAddress) < rtPhysical)
			return EFI_NO_MAPPING;

		UINT64 curVirtualAddressEnd											= curVirtualAddress + (curDescriptor->NumberOfPages << EFI_PAGE_SHIFT);
		if(LdrStaticVirtualToPhysical(curVirtualAddressEnd) > runtimeServicesPhyAddrEnd)
			return EFI_NO_MAPPING;

		mappedRuntimePages													+= curDescriptor->NumberOfPages;

		curDescriptor->VirtualStart											= setupThunkCode ? LdrStaticVirtualToPhysical(curVirtualAddress) : curVirtualAddress;
		curVirtualAddress													= curVirtualAddressEnd;
	}

	//
	// setup thunk code before adjust memory map
	//
	if(setupThunkCode)
		ArchSetupThunkCode0(thunkOffset, loadedInfo);

	//
	// remap runtime services
	//
	if(mappedRuntimePages)
	{
		//
		// convert efi pointers
		//
		if(EFI_ERROR(EfiRuntimeServices->SetVirtualAddressMap(*memoryMapSize, descSize, descVersion, memoryMap)))
			return EFI_NO_MAPPING;

		if(setupThunkCode)
		{
			//
			// adjust virtual address
			//
			curDescriptor													= memoryMap;
			for(UINTN i = 0; i < *memoryMapSize; i += descSize, curDescriptor = NextMemoryDescriptor(curDescriptor, descSize))
			{
				if(!(curDescriptor->Attribute & EFI_MEMORY_RUNTIME))
					continue;

				curDescriptor->VirtualStart									+= thunkOffset;
			}
		}
	}
	else
	{
		EfiSystemTable->RuntimeServices										= nullptr;
	}

	//
	// move memory
	//
	curDescriptor															= memoryMap;
	for(UINTN i = 0; i < *memoryMapSize; i += descSize, curDescriptor = NextMemoryDescriptor(curDescriptor, descSize))
	{
		if(curDescriptor->Type != EfiRuntimeServicesCode && curDescriptor->Type != EfiRuntimeServicesData)
			continue;

		UINT64 kernelPhysical												= LdrStaticVirtualToPhysical(curDescriptor->VirtualStart);
		if(kernelPhysical == curDescriptor->PhysicalStart)
			continue;

		UINTN bufferLength													= static_cast<UINTN>(curDescriptor->NumberOfPages << EFI_PAGE_SHIFT);
		memcpy(ArchConvertAddressToPointer(kernelPhysical, VOID*), ArchConvertAddressToPointer(curDescriptor->PhysicalStart, VOID*), bufferLength);
		memset(ArchConvertAddressToPointer(curDescriptor->PhysicalStart, VOID*), 0, bufferLength);
	}

	//
	// relocate system table
	//
	curDescriptor															= memoryMap;
	for(UINTN i = 0; i < *memoryMapSize; i += descSize, curDescriptor = NextMemoryDescriptor(curDescriptor, descSize))
	{
		if(curDescriptor->Type != EfiRuntimeServicesCode && curDescriptor->Type != EfiRuntimeServicesData)
			continue;

		if(ArchConvertPointerToAddress(EfiSystemTable) < curDescriptor->PhysicalStart || ArchConvertPointerToAddress(EfiSystemTable) >= curDescriptor->PhysicalStart + (curDescriptor->NumberOfPages << EFI_PAGE_SHIFT))
			continue;

		UINT64 newAddress													= ArchConvertPointerToAddress(EfiSystemTable);
		newAddress															+= (curDescriptor->VirtualStart - curDescriptor->PhysicalStart);
		EfiSystemTable														= ArchConvertAddressToPointer(LdrStaticVirtualToPhysical(newAddress), EFI_SYSTEM_TABLE*);
		*efiSysTablePhy														= LdrStaticVirtualToPhysical(newAddress);
		EfiRuntimeServices													= EfiSystemTable->RuntimeServices;
		break;
	}

	//
	// relocate configuration tables pointer
	//
	curDescriptor															= memoryMap;
	for(UINTN i = 0; i < *memoryMapSize; i += descSize, curDescriptor = NextMemoryDescriptor(curDescriptor, descSize))
	{
		if(curDescriptor->Type != EfiRuntimeServicesCode && curDescriptor->Type != EfiRuntimeServicesData)
			continue;

		UINT64 tableAddress													= ArchConvertPointerToAddress(EfiSystemTable->ConfigurationTable);
		if(tableAddress < curDescriptor->PhysicalStart || tableAddress >= curDescriptor->PhysicalStart + (curDescriptor->NumberOfPages << EFI_PAGE_SHIFT))
			continue;

		tableAddress														+= (curDescriptor->VirtualStart - curDescriptor->PhysicalStart);
		if(setupThunkCode)
			tableAddress													= LdrStaticVirtualToPhysical(tableAddress);

		EfiSystemTable->ConfigurationTable									= ArchConvertAddressToPointer(tableAddress, EFI_CONFIGURATION_TABLE*);
	}

	//
	// relocate configuration tables
	//
	curDescriptor															= memoryMap;
	for(UINTN i = 0; i < *memoryMapSize; i += descSize, curDescriptor = NextMemoryDescriptor(curDescriptor, descSize))
	{
		if(curDescriptor->Type != EfiRuntimeServicesCode && curDescriptor->Type != EfiRuntimeServicesData)
			continue;

		EFI_CONFIGURATION_TABLE* configurationTable							= ArchConvertAddressToPointer(LdrStaticVirtualToPhysical(ArchConvertPointerToAddress(EfiSystemTable->ConfigurationTable)), EFI_CONFIGURATION_TABLE*);
		for(UINTN j = 0; j < EfiSystemTable->NumberOfTableEntries; j ++, configurationTable ++)
		{
			UINT64 tableAddress												= ArchConvertPointerToAddress(configurationTable->VendorTable);
			if(tableAddress < curDescriptor->PhysicalStart || tableAddress >= curDescriptor->PhysicalStart + (curDescriptor->NumberOfPages << EFI_PAGE_SHIFT))
				continue;

			tableAddress													+= (curDescriptor->VirtualStart - curDescriptor->PhysicalStart);
			if(setupThunkCode)
				tableAddress												= LdrStaticVirtualToPhysical(tableAddress);

			configurationTable->VendorTable									= ArchConvertAddressToPointer(tableAddress, VOID*);
		}
	}

	//
	// setup thunk code
	//
	if(setupThunkCode)
		ArchSetupThunkCode1(efiSysTablePhy, thunkOffset);

	//
	// fix physical address
	//
	curDescriptor															= memoryMap;
	for(UINTN i = 0; i < *memoryMapSize; i += descSize, curDescriptor = NextMemoryDescriptor(curDescriptor, descSize))
	{
		if(curDescriptor->Type != EfiRuntimeServicesCode && curDescriptor->Type != EfiRuntimeServicesData)
			continue;

		UINT64 kernelPhysical												= LdrStaticVirtualToPhysical(curDescriptor->VirtualStart);
		if(kernelPhysical == curDescriptor->PhysicalStart)
			continue;

		if(createRegion)
		{
			EFI_MEMORY_DESCRIPTOR savedDescriptor							= *curDescriptor;
			EFI_MEMORY_DESCRIPTOR* newDescriptor							= 0;
			curDescriptor->VirtualStart										= 0;
			curDescriptor->Attribute										&= ~EFI_MEMORY_RUNTIME;
			curDescriptor->Type												= EfiConventionalMemory;
			MmpCreateSubRegion(memoryMap, memoryMapSize, descSize, descVersion, &newDescriptor, LdrStaticVirtualToPhysical(savedDescriptor.VirtualStart), curDescriptor->NumberOfPages);
			newDescriptor->Attribute										|= EFI_MEMORY_RUNTIME;
			newDescriptor->VirtualStart										= savedDescriptor.VirtualStart;
			newDescriptor->Type												= savedDescriptor.Type;
		}
		else
		{
			curDescriptor->PhysicalStart									= kernelPhysical;
		}
	}

	//
	// fix crc
	//
	EFI_TABLE_HEADER* tableHeader											= ArchConvertAddressToPointer(*efiSysTablePhy, EFI_TABLE_HEADER*);
	tableHeader->CRC32														= 0;
	tableHeader->CRC32														= BlCrc32(0, tableHeader, tableHeader->HeaderSize);

	return EFI_SUCCESS;
}