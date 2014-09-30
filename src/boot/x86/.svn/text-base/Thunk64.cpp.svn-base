//********************************************************************
//	created:	15:11:2009   15:50
//	filename: 	Thunk64.cpp
//	author:		tiamo
//	purpose:	64bits thunk
//********************************************************************

#include "stdafx.h"

//
// 64 bits configuration table
//
typedef struct EFI_CONFIGURATION_TABLE64
{
	EFI_GUID																VendorGuid;
	UINT64																	VendorTable;
}EFI_CONFIGURATION_TABLE64;

//
// 64bits system table
//
typedef struct _EFI_SYSTEM_TABLE64
{
	EFI_TABLE_HEADER														Hdr;
	UINT64																	FirmwareVendor;
	UINT32																	FirmwareRevision;
	UINT32																	Padding;
	UINT64																	ConsoleInHandle;
	UINT64																	ConIn;
	UINT64																	ConsoleOutHandle;
	UINT64																	ConOut;
	UINT64																	StandardErrorHandle;
	UINT64																	StdErr;
	UINT64																	RuntimeServices;
	UINT64																	BootServices;
	UINT64																	NumberOfTableEntries;
	UINT64																	ConfigurationTable;
}EFI_SYSTEM_TABLE64;

//
// 64bits runtime services
//
typedef struct _EFI_RUNTIME_SERVICES64
{
	EFI_TABLE_HEADER														Hdr;

	UINT64																	GetTime;
	UINT64																	SetTime;
	UINT64																	GetWakeupTime;
	UINT64																	SetWakeupTime;
	UINT64																	SetVirtualAddressMap;
	UINT64																	ConvertPointer;
	UINT64																	GetVariable;
	UINT64																	GetNextVariableName;
	UINT64																	SetVariable;
	UINT64																	GetNextHighMonotonicCount;
	UINT64																	ResetSystem;
	UINT64																	ReportStatusCode;
}EFI_RUNTIME_SERVICES64;

//
// thunk buffer
//
STATIC VOID* ArchpThunkCodeStart											= nullptr;
STATIC VOID* ArchpThunkDataStart											= nullptr;
STATIC UINT32 ArchpThunkDataSize											= 0;
STATIC EFI_EVENT ArchpVirtualAddressChangedEvent							= nullptr;
STATIC UINT64 ArchpKernelIdlePML4											= 0;

//
// virtual address changed
//
STATIC VOID EFIAPI ArchpVirtualAddressChanged(EFI_EVENT theEvent, VOID* theContext)
{
#ifndef EFI_INTERNAL_POINTER
	#define EFI_INTERNAL_POINTER 0x00000004
#endif

	EfiRuntimeServices->ConvertPointer(EFI_INTERNAL_POINTER, &ArchpThunkCodeStart);
	EfiRuntimeServices->ConvertPointer(EFI_INTERNAL_POINTER, &ArchpThunkDataStart);
}

//
// setup thunk code
//
VOID ArchSetupThunkCode0(UINT64 thunkOffset, MACH_O_LOADED_INFO* loadedInfo)
{
	if(loadedInfo)
	{
		ArchpKernelIdlePML4													= MachFindSymbolVirtualAddressByName(loadedInfo, CHAR8_CONST_STRING("_IdlePML4")); //0x8c0ac8 + thunkOffset;
		EfiRuntimeServices->SetVariable(CHAR16_STRING(L"IdlePML4"), &AppleNVRAMVariableGuid, EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS, sizeof(UINT64), &ArchpKernelIdlePML4);
	}
	else
	{
		UINTN dataSize														= sizeof(UINT64);
		EfiRuntimeServices->GetVariable(CHAR16_STRING(L"IdlePML4"), &AppleNVRAMVariableGuid, nullptr, &dataSize, &ArchpKernelIdlePML4);
	}
}

//
// setup thunk code
//
VOID ArchSetupThunkCode1(UINT64* efiSystemTablePhysicalAddress, UINT64 thunkOffset)
{
	//
	// setup pointers
	//
	EFI_SYSTEM_TABLE* systemTable32											= ArchConvertAddressToPointer(*efiSystemTablePhysicalAddress, EFI_SYSTEM_TABLE*);
	EFI_RUNTIME_SERVICES* runtimeServices32									= systemTable32->RuntimeServices;
	EFI_RUNTIME_SERVICES* runtimeServicesInThunk							= static_cast<EFI_RUNTIME_SERVICES*>(ArchpThunkCodeStart);
	UINT32 efiTableSize														= sizeof(EFI_SYSTEM_TABLE64) + sizeof(EFI_RUNTIME_SERVICES64);
	efiTableSize															= (efiTableSize + systemTable32->NumberOfTableEntries * sizeof(EFI_CONFIGURATION_TABLE64) + 15) & ~15;
	UINT64* efiMapL4														= Add2Ptr(ArchpThunkDataStart, ArchpThunkDataSize - EFI_PAGE_SIZE, UINT64*);
	EFI_SYSTEM_TABLE64* systemTable64										= Add2Ptr(efiMapL4, -static_cast<INTN>(efiTableSize), EFI_SYSTEM_TABLE64*);
	EFI_RUNTIME_SERVICES64* runtimeServices64								= Add2Ptr(systemTable64, sizeof(EFI_SYSTEM_TABLE64), EFI_RUNTIME_SERVICES64*);
	EFI_CONFIGURATION_TABLE64* configurationTable64							= Add2Ptr(runtimeServices64, sizeof(EFI_RUNTIME_SERVICES64), EFI_CONFIGURATION_TABLE64*);

	//
	// setup system table64
	//
	systemTable64->Hdr.Signature											= EFI_SYSTEM_TABLE_SIGNATURE;
	systemTable64->Hdr.Revision												= EFI_SYSTEM_TABLE_REVISION;
	systemTable64->Hdr.Reserved												= 0;
	systemTable64->Hdr.HeaderSize											= sizeof(EFI_SYSTEM_TABLE64);
	systemTable64->FirmwareVendor											= ArchConvertPointerToAddress(systemTable32->FirmwareVendor) + thunkOffset;
	systemTable64->FirmwareRevision											= systemTable32->FirmwareRevision;
	systemTable64->Padding													= 0;
	systemTable64->ConsoleInHandle											= 0;
	systemTable64->ConIn													= 0;
	systemTable64->ConsoleOutHandle											= 0;
	systemTable64->ConOut													= 0;
	systemTable64->StandardErrorHandle										= 0;
	systemTable64->StdErr													= 0;
	systemTable64->RuntimeServices											= ArchConvertPointerToAddress(runtimeServices64) + thunkOffset;
	systemTable64->BootServices												= 0;
	systemTable64->NumberOfTableEntries										= systemTable32->NumberOfTableEntries;
	systemTable64->ConfigurationTable										= ArchConvertPointerToAddress(configurationTable64) + thunkOffset;

	//
	// setup configuration tables
	//
	EFI_CONFIGURATION_TABLE* configurationTable32							= systemTable32->ConfigurationTable;
	for(UINTN i = 0; i < systemTable32->NumberOfTableEntries; i ++, configurationTable32 ++, configurationTable64 ++)
	{
		configurationTable64->VendorGuid									= configurationTable32->VendorGuid;
		configurationTable64->VendorTable									= ArchConvertPointerToAddress(configurationTable32->VendorTable) + thunkOffset;
	}

	//
	// setup runtime service64
	//
	UINT64 adjustOffset														= thunkOffset + ArchConvertPointerToAddress(ArchpThunkCodeStart);
	runtimeServices64->SetVirtualAddressMap									= 0;
	runtimeServices64->ConvertPointer										= 0;
	runtimeServices64->GetTime												= adjustOffset + ArchConvertPointerToAddress(runtimeServicesInThunk->GetTime);
	runtimeServices64->SetTime												= adjustOffset + ArchConvertPointerToAddress(runtimeServicesInThunk->SetTime);
	runtimeServices64->GetWakeupTime										= adjustOffset + ArchConvertPointerToAddress(runtimeServicesInThunk->GetWakeupTime);
	runtimeServices64->SetWakeupTime										= adjustOffset + ArchConvertPointerToAddress(runtimeServicesInThunk->SetWakeupTime);
	runtimeServices64->GetVariable											= adjustOffset + ArchConvertPointerToAddress(runtimeServicesInThunk->GetVariable);
	runtimeServices64->GetNextVariableName									= adjustOffset + ArchConvertPointerToAddress(runtimeServicesInThunk->GetNextVariableName);
	runtimeServices64->SetVariable											= adjustOffset + ArchConvertPointerToAddress(runtimeServicesInThunk->SetVariable);
	runtimeServices64->GetNextHighMonotonicCount							= adjustOffset + ArchConvertPointerToAddress(runtimeServicesInThunk->GetNextHighMonotonicCount);
	runtimeServices64->ResetSystem											= adjustOffset + ArchConvertPointerToAddress(runtimeServicesInThunk->ResetSystem);
	runtimeServices64->ReportStatusCode										= adjustOffset + ArchConvertPointerToAddress(runtimeServicesInThunk->ReportStatusCode);
	runtimeServices64->Hdr.Signature										= EFI_RUNTIME_SERVICES_SIGNATURE;
	runtimeServices64->Hdr.Revision											= EFI_RUNTIME_SERVICES_REVISION;
	runtimeServices64->Hdr.Reserved											= 0;
	runtimeServices64->Hdr.HeaderSize										= sizeof(EFI_RUNTIME_SERVICES64);
	runtimeServices64->Hdr.CRC32											= 0;
	runtimeServices64->Hdr.CRC32											= BlCrc32(0, runtimeServices64, runtimeServices64->Hdr.HeaderSize);

	//
	// setup runtime services in thunk
	//
	memcpy(runtimeServicesInThunk, runtimeServices32, sizeof(EFI_RUNTIME_SERVICES));

	//
	// setup efi page table
	//
	UINT64* efiCR3InThunk													= Add2Ptr(ArchpThunkCodeStart, sizeof(EFI_RUNTIME_SERVICES), UINT64*);
	*efiCR3InThunk															= ArchConvertPointerToAddress(efiMapL4) + thunkOffset;
	memset(efiMapL4, 0, EFI_PAGE_SIZE);

	//
	// save scratch address in thunk
	//
	UINT64* scratchAddressInThunk											= Add2Ptr(efiCR3InThunk, sizeof(UINT64), UINT64*);
	*scratchAddressInThunk													= ArchConvertPointerToAddress(systemTable64) + thunkOffset;

	//
	// save kernel idlePML4
	//
	UINT64* kernelIdlePML4													= Add2Ptr(scratchAddressInThunk, sizeof(UINT64), UINT64*);
	*kernelIdlePML4															= ArchpKernelIdlePML4;

	//
	// new system table
	//
	*efiSystemTablePhysicalAddress											= ArchConvertPointerToAddress(systemTable64);
}

//
// init phase 1
//
EFI_STATUS ArchInitialize1()
{
	//
	// allocate thunk code pages
	//
	extern UINT8* ArchThunk64BufferStart;
	extern UINT8* ArchThunk64BufferEnd;
	UINTN thunkCodeSize														= ArchConvertPointerToAddress(&ArchThunk64BufferEnd) - ArchConvertPointerToAddress(&ArchThunk64BufferStart);
	UINT64 physicalAddress													= 4 * 1024 * 1024 * 1024ULL - 1;
	ArchpThunkCodeStart														= MmAllocatePages(AllocateMaxAddress, EfiRuntimeServicesCode, EFI_SIZE_TO_PAGES(thunkCodeSize), &physicalAddress);
	if(!ArchpThunkCodeStart)
		return EFI_OUT_OF_RESOURCES;
	memcpy(ArchpThunkCodeStart, &ArchThunk64BufferStart, thunkCodeSize);

	//
	// allocate thunk data pages
	//
	physicalAddress															= 4 * 1024 * 1024 * 1024ULL - 1;
	UINT32 efiTableSize														= sizeof(EFI_SYSTEM_TABLE64) + sizeof(EFI_RUNTIME_SERVICES64);
	efiTableSize															+= EfiSystemTable->NumberOfTableEntries * sizeof(EFI_CONFIGURATION_TABLE64) * 2;
	ArchpThunkDataSize														= efiTableSize + 64 * 1024 + EFI_PAGE_SIZE;
	UINT32 pageCount														= EFI_SIZE_TO_PAGES(ArchpThunkDataSize);
	ArchpThunkDataSize														= EFI_PAGES_TO_SIZE(pageCount);
	ArchpThunkDataStart														= MmAllocatePages(AllocateMaxAddress, EfiRuntimeServicesData, pageCount, &physicalAddress);
	if(!ArchpThunkDataStart)
		return EFI_OUT_OF_RESOURCES;

	//
	// register notification
	//
	return EfiBootServices->CreateEvent(EFI_EVENT_SIGNAL_VIRTUAL_ADDRESS_CHANGE, EFI_TPL_NOTIFY, &ArchpVirtualAddressChanged, nullptr, &ArchpVirtualAddressChangedEvent);
}