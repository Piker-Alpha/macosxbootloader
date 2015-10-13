//********************************************************************
//	created:	4:11:2009   11:47
//	filename: 	memory.h
//	author:		tiamo
//	purpose:	memory
//********************************************************************

#pragma once

//
// initialize
//
EFI_STATUS MmInitialize();

//
// finalize
//
VOID MmFinalize();

//
// translate address
//
BOOLEAN BOOTAPI MmTranslateVirtualAddress(VOID* virtualAddress, UINT64* physicalAddress);

//
// allocate pool
//
VOID* MmAllocatePool(UINTN bufferLength);

//
// free
//
VOID MmFreePool(VOID* freeBuffer);

//
// allocate page
//
VOID* MmAllocatePages(EFI_ALLOCATE_TYPE allocateType, EFI_MEMORY_TYPE memoryType, UINTN pagesCount, UINT64* physicalAddress);

//
// free pages
//
VOID MmFreePages(UINT64 phyAddress);

//
// allocate kernel memory
//
UINT64 MmAllocateKernelMemory(UINTN* bufferLength, UINT64* virtualAddress);

//
// free kernel memory
//
VOID MmFreeKernelMemory(UINT64 virtualAddress, UINT64 physicalAddress);

//
// allocate loader data memory
//
UINT64 MmAllocateLoaderData(UINTN* bufferLength, UINT64* virtualAddress);

//
// free loader data memory
//
VOID MmFreeLoaderData(UINT64 virtualAddress, UINT64 physicalAddress);

//
// get kernel memory range
//
VOID MmGetKernelPhysicalRange(UINT64* lowerAddress, UINT64* upperAddress);

//
// get kernel virtual start
//
UINT64 MmGetKernelVirtualStart();