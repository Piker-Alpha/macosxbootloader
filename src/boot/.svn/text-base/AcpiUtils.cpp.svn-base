//********************************************************************
//	created:	6:11:2009   15:29
//	filename: 	AcpiUtils.cpp
//	author:		tiamo
//	purpose:	acpi utils
//********************************************************************

#include "stdafx.h"

//
// global
//
STATIC UINT64 AcpipNVSMemoryAddress											= 0;

//
// get acpi table
//
VOID CONST* AcpipGetTable(UINT32 signature)
{
	EFI_CONFIGURATION_TABLE* theTable										= EfiSystemTable->ConfigurationTable;
	EFI_ACPI_2_0_ROOT_SYSTEM_DESCRIPTION_POINTER CONST* acpiRsdp			= nullptr;
	for(UINTN i = 0; i < EfiSystemTable->NumberOfTableEntries; i ++, theTable ++)
	{
		if(!memcmp(&theTable->VendorGuid, &EfiAcpi20TableGuid, sizeof(EfiAcpi20TableGuid)))
		{
			acpiRsdp														= static_cast<EFI_ACPI_2_0_ROOT_SYSTEM_DESCRIPTION_POINTER CONST*>(theTable->VendorTable);
			break;
		}

		if(!memcmp(&theTable->VendorGuid, &EfiAcpiTableGuid, sizeof(EfiAcpiTableGuid)))
			acpiRsdp														= static_cast<EFI_ACPI_2_0_ROOT_SYSTEM_DESCRIPTION_POINTER CONST*>(theTable->VendorTable);
	}

	if(!acpiRsdp)
		return nullptr;

	EFI_ACPI_DESCRIPTION_HEADER* acpiRsdt									= nullptr;
	UINT32 tableCount														= 0;
	UINT32 itemSize															= 0;
	if(acpiRsdp->Revision == 0)
	{
		acpiRsdt															= ArchConvertAddressToPointer(acpiRsdp->RsdtAddress, EFI_ACPI_DESCRIPTION_HEADER*);
		itemSize															= sizeof(UINT32);
		if(acpiRsdt)
			tableCount														= (acpiRsdt->Length - sizeof(EFI_ACPI_DESCRIPTION_HEADER)) / sizeof(UINT32);
	}
	else if(acpiRsdp->Revision >= EFI_ACPI_2_0_REVISION)
	{
		itemSize															= sizeof(UINT64);
		acpiRsdt															= ArchConvertAddressToPointer(acpiRsdp->XsdtAddress, EFI_ACPI_DESCRIPTION_HEADER*);
		if(acpiRsdt)
			tableCount														= (acpiRsdt->Length - sizeof(EFI_ACPI_DESCRIPTION_HEADER)) / sizeof(UINT64);
	}

	VOID* tableArray														= acpiRsdt + 1;
	for(UINT32 i = 0; i < tableCount; i ++, tableArray = Add2Ptr(tableArray, itemSize, VOID*))
	{
		UINT64 tableAddress													= itemSize == sizeof(UINT64) ? *static_cast<UINT64*>(tableArray) : *static_cast<UINT32*>(tableArray);
		EFI_ACPI_DESCRIPTION_HEADER* theTable								= ArchConvertAddressToPointer(tableAddress, EFI_ACPI_DESCRIPTION_HEADER*);
		if(theTable->Signature == signature)
			return theTable;
	}

	return nullptr;
}

//
// get fadt
//
VOID CONST* AcpiGetFixedAcpiDescriptionTable()
{
	return AcpipGetTable(EFI_ACPI_1_0_FIXED_ACPI_DESCRIPTION_TABLE_SIGNATURE);
}

//
// get pci config space info
//
EFI_STATUS AcpiGetPciConfigSpaceInfo(UINT64* baseAddress, UINT32* startBus, UINT32* endBus)
{
	EFI_ACPI_DESCRIPTION_HEADER CONST* header								= static_cast<EFI_ACPI_DESCRIPTION_HEADER CONST*>(AcpipGetTable(0x4746434D));
	if(!header || header->Revision != 1)
		return EFI_NOT_FOUND;

	*baseAddress															= *Add2Ptr(header, 0x2c, UINT64*);
	*startBus																= *Add2Ptr(header, 0x36, UINT8*);
	*endBus																	= *Add2Ptr(header, 0x37, UINT8*);
	return EFI_SUCCESS;
}

//
// get machine signature
//
EFI_STATUS AcpiGetMachineSignature(UINT32* machineSignature)
{
	EFI_ACPI_3_0_FIXED_ACPI_DESCRIPTION_TABLE CONST* acpiFadt				= static_cast<EFI_ACPI_3_0_FIXED_ACPI_DESCRIPTION_TABLE CONST*>(AcpiGetFixedAcpiDescriptionTable());
	if(!acpiFadt)
		return EFI_UNSUPPORTED;

	//
	// get firmware control
	//
	UINT64 firmwareControlAddress											= acpiFadt->FirmwareCtrl;
	if(acpiFadt->Header.Revision >= EFI_ACPI_3_0_REVISION && acpiFadt->XFirmwareCtrl)
		firmwareControlAddress												= acpiFadt->XFirmwareCtrl;

	//
	// add hardware signature
	//
	EFI_ACPI_1_0_FIRMWARE_ACPI_CONTROL_STRUCTURE CONST* fwControl			= ArchConvertAddressToPointer(firmwareControlAddress, EFI_ACPI_1_0_FIRMWARE_ACPI_CONTROL_STRUCTURE CONST*);
	if(!fwControl)
		return EFI_UNSUPPORTED;

	*machineSignature														= fwControl->HardwareSignature;
	return EFI_SUCCESS;
}

//
// detect acpi nvs memory
//
VOID AcpiDetectNVSMemory()
{
	//
	// get FirmwareFeatures/FirmwareFeaturesMask
	//
	UINT32 firmwareFeatures													= 0;
	UINTN dataLength														= sizeof(firmwareFeatures);
	if(!EFI_ERROR(EfiRuntimeServices->GetVariable(CHAR16_STRING(L"FirmwareFeatures"), &AppleFirmwareVariableGuid, 0, &dataLength, &firmwareFeatures)))
	{
		UINT32 firmwareFeaturesMask											= 0;
		dataLength															= sizeof(firmwareFeaturesMask);
		if(!EFI_ERROR(EfiRuntimeServices->GetVariable(CHAR16_STRING(L"FirmwareFeaturesMask"), &AppleFirmwareVariableGuid, 0, &dataLength, &firmwareFeaturesMask)))
		{
			firmwareFeatures												&= firmwareFeaturesMask;
			if(!(firmwareFeatures & 0x80000000))
			{
				VOID* acpiGlobalVariable									= nullptr;
				dataLength													= sizeof(acpiGlobalVariable);
				if(!EFI_ERROR(EfiRuntimeServices->GetVariable(CHAR16_STRING(L"AcpiGlobalVariable"), &AppleAcpiVariableGuid, 0, &dataLength, &acpiGlobalVariable)))
				{
					if(acpiGlobalVariable)
						AcpipNVSMemoryAddress								= *Add2Ptr(acpiGlobalVariable, 0x18, UINT64*);
				}
			}
		}
	}
}

//
// adjust memory map for acpi nvs memory
//
UINTN AcpiAdjustMemoryMap(EFI_MEMORY_DESCRIPTOR* memoryMap, UINTN memoryMapSize, UINTN descriptorSize)
{
	if(!AcpipNVSMemoryAddress)
		return memoryMapSize;

	//
	// end of memory map
	//
	EFI_MEMORY_DESCRIPTOR* endOfMemoryMap									= Add2Ptr(memoryMap, memoryMapSize, EFI_MEMORY_DESCRIPTOR*);
	for(UINTN i = 0; i < memoryMapSize; i += descriptorSize, memoryMap = NextMemoryDescriptor(memoryMap, descriptorSize))
	{
		//
		// bounds check
		//
		UINT64 physicalBegin												= memoryMap->PhysicalStart;
		UINT64 physicalLength												= memoryMap->NumberOfPages << EFI_PAGE_SHIFT;
		UINT64 physicalEnd													= physicalLength + physicalBegin;
		STATIC UINT32 allowType												= 0x9e;
		if(AcpipNVSMemoryAddress >= physicalBegin && AcpipNVSMemoryAddress < physicalEnd && (allowType & (1 << memoryMap->Type)))
		{
			UINT64 alignedAcpiAddress										= PAGE_ALIGN(AcpipNVSMemoryAddress);

			if(alignedAcpiAddress < physicalBegin + 16 * 1024)
			{
				//
				// calc size
				//
				UINT64 pagesCount											= EFI_SIZE_TO_PAGES(AcpipNVSMemoryAddress - physicalBegin + 16 * 1024);

				//
				// build new descriptor
				//
				endOfMemoryMap->Attribute									= memoryMap->Attribute;
				endOfMemoryMap->Type										= memoryMap->Type;
				endOfMemoryMap->NumberOfPages								= memoryMap->NumberOfPages - pagesCount;
				endOfMemoryMap->PhysicalStart								= (pagesCount << EFI_PAGE_SHIFT) + physicalBegin;
				endOfMemoryMap->VirtualStart								= 0;

				//
				// adjust
				//
				memoryMap->Type												= EfiACPIMemoryNVS;
				memoryMap->NumberOfPages									= pagesCount;
			}
			else if(alignedAcpiAddress >= physicalEnd - 32 * 1024)
			{
				//
				// calc size
				//
				UINT64 pagesCount											= EFI_SIZE_TO_PAGES(physicalEnd - alignedAcpiAddress);

				//
				// build new descriptor
				//
				endOfMemoryMap->Attribute									= memoryMap->Attribute;
				endOfMemoryMap->Type										= EfiACPIMemoryNVS;
				endOfMemoryMap->NumberOfPages								= pagesCount;
				endOfMemoryMap->PhysicalStart								= alignedAcpiAddress;
				endOfMemoryMap->VirtualStart								= 0;

				//
				// adjust
				//
				memoryMap->NumberOfPages									-= pagesCount;
			}
			else
			{
				//
				// calc size
				//
				UINT64 pagesCount											= EFI_SIZE_TO_PAGES(BYTE_OFFSET(AcpipNVSMemoryAddress) + 16 * 1024);

				//
				// build new descriptor
				//
				endOfMemoryMap->Attribute									= memoryMap->Attribute;
				endOfMemoryMap->Type										= EfiACPIMemoryNVS;
				endOfMemoryMap->NumberOfPages								= pagesCount;
				endOfMemoryMap->PhysicalStart								= alignedAcpiAddress;
				endOfMemoryMap->VirtualStart								= 0;

				memoryMapSize												+= descriptorSize;
				EFI_MEMORY_DESCRIPTOR* endOfMemoryMap2						= NextMemoryDescriptor(endOfMemoryMap, descriptorSize);
				endOfMemoryMap2->Attribute									= memoryMap->Attribute;
				endOfMemoryMap2->Type										= memoryMap->Type;
				endOfMemoryMap2->PhysicalStart								= alignedAcpiAddress + (pagesCount << EFI_PAGE_SHIFT);
				endOfMemoryMap2->VirtualStart								= 0;
				endOfMemoryMap2->NumberOfPages								= memoryMap->NumberOfPages - EFI_SIZE_TO_PAGES(endOfMemoryMap2->PhysicalStart - physicalBegin);

				//
				// adjust
				//
				memoryMap->NumberOfPages									= EFI_SIZE_TO_PAGES(endOfMemoryMap->PhysicalStart - physicalBegin);
			}

			memoryMapSize													+= descriptorSize;
			break;
		}
	}

	return memoryMapSize;
}