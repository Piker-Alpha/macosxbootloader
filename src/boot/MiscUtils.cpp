//********************************************************************
//	created:	7:11:2009   1:38
//	filename: 	MiscUtils.cpp
//	author:		tiamo
//	purpose:	utils
//********************************************************************

#include "StdAfx.h"

//
// global
//
STATIC UINT64 BlpMemoryCapacity												= 0;
STATIC UINT32 BlpMemoryDevices												= 0;
STATIC UINT64 BlpMemorySize													= 0;
STATIC CHAR8 BlpBoardId[64]													= {0};
EFI_GUID BlpSmbiosUuid														= {0};

#define IS_LEAP_YEAR(y)														(((y) % 4 == 0 && (y) % 100 != 0) || (y) % 400 == 0)

//
// Get memory capacity.
//
UINT64 BlGetMemoryCapacity()
{
	return BlpMemoryCapacity;
}

//
// Get memory size.
//
UINT64 BlGetMemorySize()
{
	return BlpMemorySize;
}

//
// Get board-id.
//
CHAR8* BlGetBoardId()
{
	UINT32 eaxValue															= 0;
	UINT32 ebxValue															= 0;
	UINT32 ecxValue															= 0;
	UINT32 edxValue															= 0;
	ArchCpuId(1, &eaxValue, &ebxValue, &ecxValue, &edxValue);

	return ecxValue & 0x80000000 ? CHAR8_STRING("VMM") : BlpBoardId;
}

//
// Get system-id.
//
EFI_GUID BlGetSmbiosUuid()
{
	return BlpSmbiosUuid;
}

//
// Connect drivers.
//
EFI_STATUS BlConnectAllController()
{
	UINTN lastHandles														= static_cast<UINTN>(-1);
	EFI_HANDLE* handleArray													= nullptr;
	EFI_STATUS status														= EFI_SUCCESS;

	__try
	{
		while (TRUE)
		{
			UINTN totalHandles												= 0;

			if (EFI_ERROR(status = EfiBootServices->LocateHandleBuffer(AllHandles, nullptr, nullptr, &totalHandles, &handleArray)))
				try_leave(NOTHING);

			if (lastHandles == totalHandles)
				try_leave(NOTHING);

			for (UINTN i = 0; i < totalHandles; i ++)
				EfiBootServices->ConnectController(handleArray[i], nullptr, nullptr, TRUE);

			EfiBootServices->FreePool(handleArray);
			handleArray														= nullptr;
			lastHandles														= totalHandles;
		}
	}
	__finally
	{
		if (handleArray)
			EfiBootServices->FreePool(handleArray);
	}

	return status;
}

//
// Get base and size.
//
EFI_STATUS BlGetApplicationBaseAndSize(UINT64* imageBase, UINT64* imageSize)
{
	EFI_LOADED_IMAGE_PROTOCOL* loadeImageProtocol							= nullptr;

	if (EfiBootServices->HandleProtocol(EfiImageHandle, &EfiLoadedImageProtocolGuid, reinterpret_cast<VOID**>(&loadeImageProtocol)) != EFI_SUCCESS)
		return EFI_UNSUPPORTED;

	if (imageBase)
		*imageBase															= ArchConvertPointerToAddress(loadeImageProtocol->ImageBase);

	if (imageSize)
		*imageSize															= loadeImageProtocol->ImageSize;

	return EFI_SUCCESS;
}

//
// Parse device location.
//
VOID BlParseDeviceLocation(CHAR8 CONST* loaderOptions, UINTN* segment, UINTN* bus, UINTN* device, UINTN* func)
{
	if (loaderOptions && loaderOptions[0])
	{
		CHAR8 CONST* temp													= strstr(loaderOptions, CHAR8_CONST_STRING("/seg="));

		if (temp && segment)
			*segment														= static_cast<UINTN>(atoi(temp + 5));

		temp																= strstr(loaderOptions, CHAR8_CONST_STRING("/bus="));

		if (temp && bus)
			*bus															= static_cast<UINTN>(atoi(temp + 5));

		temp																= strstr(loaderOptions, CHAR8_CONST_STRING("/dev="));

		if (temp && device)
			*device															= static_cast<UINTN>(atoi(temp + 5));

		temp																= strstr(loaderOptions, CHAR8_CONST_STRING("/func="));

		if (temp && func)
			*func															= static_cast<UINTN>(atoi(temp + 6));
	}
}

//
// PCI get bar attribute.
//
EFI_STATUS BlGetPciBarAttribute(EFI_PCI_IO_PROTOCOL* pciIoProtocol, UINT8 barIndex, UINT64* baseAddress, UINT64* barLength, BOOLEAN* isMemorySpace)
{
	VOID* pciResource														= nullptr;
	EFI_STATUS Status														= pciIoProtocol->GetBarAttributes(pciIoProtocol, barIndex, nullptr, &pciResource);

	//
	// Get information.
	//
	if (!EFI_ERROR(Status) && pciResource)
	{
		//
		// PCI resource is an ACPI descriptor.
		//
		EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR* desc								= static_cast<EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR*>(pciResource);

		//
		// Check it.
		//
		if (desc->Desc == ACPI_ADDRESS_SPACE_DESCRIPTOR)
		{
			//
			// Output.
			//
			if (baseAddress)
				*baseAddress												= desc->AddrRangeMin;

			if (barLength)
				*barLength													= desc->AddrLen;

			if (isMemorySpace)
				*isMemorySpace												= desc->ResType == ACPI_ADDRESS_SPACE_TYPE_MEM;
		}
	}

	//
	// Free this pointer.
	//
	if (pciResource)
		EfiBootServices->FreePool(pciResource);

	return Status;
}

//
// Find PCI device.
//
EFI_STATUS BlFindPciDevice(UINTN segment, UINTN bus, UINTN device, UINTN func, UINT8 baseClass, UINT8 subClass, UINT8 progIf, EFI_PCI_IO_PROTOCOL** outPciIoProtocol, EFI_HANDLE* outHandle)
{
	UINTN totalHandles														= 0;
	EFI_HANDLE* handleArray													= nullptr;
	EFI_STATUS status														= EFI_SUCCESS;

	__try
	{
		//
		// Locate PCI io protocol handle.
		//
		if (EFI_ERROR(status = EfiBootServices->LocateHandleBuffer(ByProtocol, &EfiPciIoProtocolGuid, nullptr, &totalHandles, &handleArray)))
			try_leave(NOTHING);

		//
		// For each handle, check it.
		//
		status																= EFI_NOT_FOUND;

		for (UINTN i = 0; i < totalHandles; i ++)
		{
			//
			// Open PCI io protocol.
			//
			EFI_HANDLE theHandle											= handleArray[i];
			EFI_PCI_IO_PROTOCOL* pciIoProtocol								= nullptr;

			//
			// If open failed, skip it.
			//
			if (EFI_ERROR(EfiBootServices->HandleProtocol(theHandle, &EfiPciIoProtocolGuid, reinterpret_cast<VOID**>(&pciIoProtocol))))
				continue;

			//
			// Get location.
			//
			UINTN curSeg													= static_cast<UINTN>(-1);
			UINTN curBus													= static_cast<UINTN>(-1);
			UINTN curDev													= static_cast<UINTN>(-1);
			UINTN curFunc													= static_cast<UINTN>(-1);

			if (EFI_ERROR(pciIoProtocol->GetLocation(pciIoProtocol, &curSeg, &curBus, &curDev, &curFunc)))
				continue;

			//
			// Check location.
			//
			if ((segment != -1 && segment != curSeg) || (bus != -1 && bus != curBus) || (device != -1 && device != curDev) || (func != -1 && func != curFunc))
				continue;

			//
			// Read config space.
			//
			PCI_DEVICE_INDEPENDENT_REGION configHeader;

			if (EFI_ERROR(pciIoProtocol->Pci.Read(pciIoProtocol, EfiPciIoWidthUint8, 0, sizeof(configHeader), &configHeader)))
				continue;

			//
			// Check base/sub/progif.
			//
			if (configHeader.ClassCode[2] == baseClass && configHeader.ClassCode[1] == subClass && configHeader.ClassCode[0] == progIf)
				try_leave(if (outPciIoProtocol) *outPciIoProtocol = pciIoProtocol; if (outHandle) *outHandle = theHandle; status = EFI_SUCCESS);
		}
	}
	__finally
	{
		if (handleArray)
			EfiBootServices->FreePool(handleArray);
	}
	return status;
}

//
// Start PCI device.
//
EFI_STATUS BlStartPciDevice(EFI_PCI_IO_PROTOCOL* pciIoProtocol, BOOLEAN decodeIo, BOOLEAN decodeMemory, BOOLEAN busMaster)
{
	UINT64 attribute														= decodeIo ? EFI_PCI_IO_ATTRIBUTE_IO : 0;
	attribute																|= (decodeMemory ? EFI_PCI_IO_ATTRIBUTE_MEMORY : 0);
	attribute																|= (busMaster ? EFI_PCI_IO_ATTRIBUTE_BUS_MASTER : 0);

	return pciIoProtocol->Attributes(pciIoProtocol, EfiPciIoAttributeOperationEnable, attribute, nullptr);
}

//
// Get string from SMBIOS table.
//
UINT8* BlpGetStringFromSMBIOSTable(UINT8* startOfStringTable, UINT8 index)
{
	for (UINT8 i = 1; i < index && *startOfStringTable; i ++)
		startOfStringTable													+= strlen(reinterpret_cast<CHAR8*>(startOfStringTable)) + 1;

	STATIC UINT8 BadIndex[]                                                 = "BadIndex";

	return *startOfStringTable ? startOfStringTable : (UINT8 *)BadIndex;
}

//
// Detect memory size.
//
EFI_STATUS BlDetectMemorySize()
{
	EFI_CONFIGURATION_TABLE* theTable										= EfiSystemTable->ConfigurationTable;

	for (UINTN i = 0; i < EfiSystemTable->NumberOfTableEntries; i ++, theTable ++)
	{
		if (memcmp(&theTable->VendorGuid, &EfiSmbiosTableGuid, sizeof(EfiSmbiosTableGuid)))
			continue;

		SMBIOS_ENTRY_POINT_STRUCTURE* tableStructure						= static_cast<SMBIOS_ENTRY_POINT_STRUCTURE*>(theTable->VendorTable);

		if (memcmp(tableStructure->AnchorString, "_SM_", sizeof(tableStructure->AnchorString)))
			break;

		UINT8* startOfTable													= ArchConvertAddressToPointer(tableStructure->DMI.TableAddress, UINT8*);
		UINT8* endOfTable													= startOfTable + tableStructure->DMI.TableLength;

		while (startOfTable + sizeof(SMBIOS_TABLE_HEADER) <= endOfTable)
		{
			SMBIOS_TABLE_HEADER* tableHeader								= reinterpret_cast<SMBIOS_TABLE_HEADER*>(startOfTable);

			if (tableHeader->Type == 16) // Physical Memory Array.
			{
				if (startOfTable + sizeof(SMBIOS_TABLE_TYPE16) > endOfTable)
					break;

				SMBIOS_TABLE_TYPE16* table16								= reinterpret_cast<SMBIOS_TABLE_TYPE16*>(startOfTable);

				// Capacity present?
				if (table16->MaximumCapacity != 0x80000000)
				{
					// Maximum memory capacity in kilobytes.
					BlpMemoryCapacity										+= (static_cast<UINT64>(table16->MaximumCapacity) << 10);
					BlpMemoryDevices										+= table16->NumberOfMemoryDevices;
				}
				else
				{
					// Use Maximum memory capacity.
				}
			}
			else if (tableHeader->Type == 17) // Memory Device.
			{
				if (startOfTable + sizeof(SMBIOS_TABLE_TYPE17) > endOfTable)
					break;

				SMBIOS_TABLE_TYPE17* table17								= reinterpret_cast<SMBIOS_TABLE_TYPE17*>(startOfTable);

				if (table17->Size != 0xffff)
				{
					// The granularity depends on bit-15 (0x8000). If set the value is given in kilobyte units otherwise in megabytes units.
					BlpMemorySize											+= (static_cast<UINT64>(table17->Size) << ((table17->Size & 0x8000) ? 10 : 20));
				}
				else if (table17->Size > 0)
				{
					// Use Extended Size field for 32/64/128 GB modules.
					BlpMemorySize											+= (static_cast<UINT64>(table17->ExtendedSize) << 10);
				}
			}
			else if (tableHeader->Type == 1) // System Information.
			{
				if (startOfTable + sizeof(SMBIOS_TABLE_TYPE1) > endOfTable)
					break;
				
				SMBIOS_TABLE_TYPE1* table1									= reinterpret_cast<SMBIOS_TABLE_TYPE1*>(startOfTable);

				if (!isEfiNullGuid(&table1->Uuid))
				{
					//
					// Copy SMBIOS UUID into BlpSystemId.
					//
					memcpy((VOID*)&BlpSmbiosUuid, (VOID*)&table1->Uuid, 16);
				}
			}
			else if (tableHeader->Type == 2) // Baseboard (or Module) Information.
			{
				if (startOfTable + sizeof(SMBIOS_TABLE_TYPE2) > endOfTable)
					break;
				
				SMBIOS_TABLE_TYPE2* table2									= reinterpret_cast<SMBIOS_TABLE_TYPE2*>(startOfTable);

				if (table2->ProductName)
				{
					UINT8* boardId											= BlpGetStringFromSMBIOSTable(startOfTable + table2->Hdr.Length, table2->ProductName);
					strncpy(BlpBoardId, reinterpret_cast<CHAR8*>(boardId), ARRAYSIZE(BlpBoardId) - 1);
				}
			}

			startOfTable													+= tableHeader->Length;

			while (startOfTable < endOfTable)
			{
				if (startOfTable[0] || startOfTable + 1 >= endOfTable || startOfTable[1])
				{
					startOfTable											+= 1;
				}
				else
				{
					startOfTable											+= 2;
					break;
				}
			}
		}
	}

	return EFI_SUCCESS;
}

#define READ_BUFFER(B, L, I, V, T)											do{if ((I) >= (L)) return EFI_BAD_BUFFER_SIZE; (V) = static_cast<T>((B)[(I)]); (I) += 1;}while (0)
#define WRITE_BUFFER(B, L, I, V, T)											do{if ((I) >= (L) - 1) return EFI_BUFFER_TOO_SMALL; (B)[(I)] = static_cast<T>(V); (I) += 1;}while (0)

//
// Unicode to UTF8.
//
EFI_STATUS BlUnicodeToUtf8(CHAR16 CONST* unicodeBuffer, UINTN unicodeCharCount, CHAR8* utf8Buffer, UINTN utf8BufferLength)
{
	UINT32 j																= 0;
	utf8Buffer[utf8BufferLength - 1]										= 0;
	for (UINT32 i = 0; i < unicodeCharCount; i ++)
	{
		CHAR16 unicodeChar													= unicodeBuffer[i];
		if (unicodeChar < 0x0080)
		{
			WRITE_BUFFER(utf8Buffer, utf8BufferLength, j, unicodeChar, UINT8);
		}
		else if (unicodeChar < 0x0800)
		{
			WRITE_BUFFER(utf8Buffer, utf8BufferLength, j, ((unicodeChar >>  6) & 0x0f) | 0xc0, UINT8);
			WRITE_BUFFER(utf8Buffer, utf8BufferLength, j, ((unicodeChar >>  0) & 0x3f) | 0x80, UINT8);
		}
		else
		{
			WRITE_BUFFER(utf8Buffer, utf8BufferLength, j, ((unicodeChar >> 12) & 0x0f) | 0xe0, UINT8);
			WRITE_BUFFER(utf8Buffer, utf8BufferLength, j, ((unicodeChar >>  6) & 0x3f) | 0x80, UINT8);
			WRITE_BUFFER(utf8Buffer, utf8BufferLength, j, ((unicodeChar >>  0) & 0x3f) | 0x80, UINT8);
		}
	}

	if (j < utf8BufferLength - 1)
		WRITE_BUFFER(utf8Buffer, utf8BufferLength, j, 0, UINT8);

	return EFI_SUCCESS;
}

//
// unicode to ANSI.
//
EFI_STATUS BlUnicodeToAnsi(CHAR16 CONST* unicodeBuffer, UINTN unicodeCharCount, CHAR8* ansiBuffer, UINTN ansiBufferLength)
{
	UINTN j																	= 0;
	ansiBuffer[ansiBufferLength - 1]										= 0;
	for (UINTN i = 0; i < unicodeCharCount; i ++)
	{
		CHAR16 unicodeChar													= unicodeBuffer[i];
		WRITE_BUFFER(ansiBuffer, ansiBufferLength, j, unicodeChar & 0xff, CHAR8);
	}

	if (j < ansiBufferLength - 1)
		WRITE_BUFFER(ansiBuffer, ansiBufferLength, j, 0, CHAR8);

	return EFI_SUCCESS;
}

//
// UTF8 to unicode.
//
EFI_STATUS BlUtf8ToUnicode(CHAR8 CONST* utf8Buffer, UINTN bytesCount, CHAR16* unicodeBuffer, UINTN unicodeBufferLengthInChar)
{
	UINTN i																	= 0;
	UINTN j																	= 0;
	unicodeBuffer[unicodeBufferLengthInChar - 1]							= 0;
	while (i < bytesCount)
	{
		UINT8 utf8Char1;
		UINT8 utf8Char2;
		UINT8 utf8Char3;
		READ_BUFFER(utf8Buffer, bytesCount, i, utf8Char1, UINT8);

		if (utf8Char1 < 0x80)
		{
			WRITE_BUFFER(unicodeBuffer, unicodeBufferLengthInChar, j, utf8Char1, CHAR16);
		}
		else if (utf8Char1 < 0xe0)
		{
			READ_BUFFER(utf8Buffer, bytesCount, i, utf8Char2, UINT8);

			UINT16 unicodeChar												= (static_cast<UINT16>(utf8Char1 & 0x0f) << 6) | (utf8Char2 & 0x3f);
			WRITE_BUFFER(unicodeBuffer, unicodeBufferLengthInChar, j, unicodeChar, CHAR16);
		}
		else
		{
			READ_BUFFER(utf8Buffer, bytesCount, i, utf8Char2, UINT8);
			READ_BUFFER(utf8Buffer, bytesCount, i, utf8Char3, UINT8);

			UINT16 unicodeChar												= (static_cast<UINT16>(utf8Char1 & 0x0f) << 12) | (static_cast<UINT16>(utf8Char2 & 0x3f) << 6) | static_cast<UINT16>(utf8Char3 & 0x3f);
			WRITE_BUFFER(unicodeBuffer, unicodeBufferLengthInChar, j, unicodeChar, CHAR16);
		}
	}

	if (j < unicodeBufferLengthInChar - 1)
		WRITE_BUFFER(unicodeBuffer, unicodeBufferLengthInChar, j, 0, CHAR16);

	return EFI_SUCCESS;
}

#undef READ_BUFFER
#undef WRITE_BUFFER

//
// Build UTF8 string from unicode.
//
CHAR8* BlAllocateUtf8FromUnicode(CHAR16 CONST* unicodeString, UINTN unicodeCharCount)
{
	UINTN length															= unicodeCharCount == -1 ? wcslen(unicodeString) : unicodeCharCount;
	UINTN utf8BufferLength													= (length * 3 + 1) * sizeof(CHAR8);
	UINT8* utf8NameBuffer													= static_cast<UINT8*>(MmAllocatePool(utf8BufferLength));

	if (!utf8NameBuffer)
		return nullptr;

	if (!EFI_ERROR(BlUnicodeToUtf8(unicodeString, length, utf8NameBuffer, utf8BufferLength / sizeof(CHAR8))))
		return utf8NameBuffer;

	MmFreePool(utf8NameBuffer);

	return nullptr;
}

//
// Build unicode string from UTF8.
//
CHAR16* BlAllocateUnicodeFromUtf8(CHAR8 CONST* utf8String, UINTN utf8Length)
{
	UINTN length															= (utf8Length == -1 ? strlen(utf8String) : utf8Length) + 1;
	CHAR16* unicodeBuffer													= static_cast<CHAR16*>(MmAllocatePool(length * sizeof(CHAR16)));

	if (!unicodeBuffer)
		return nullptr;

	if (!EFI_ERROR(BlUtf8ToUnicode(utf8String, utf8Length, unicodeBuffer, length)))
		return unicodeBuffer;

	MmFreePool(unicodeBuffer);

	return nullptr;
}

//
// Allocate string.
//
CHAR8* BlAllocateString(CHAR8 CONST* inputString)
{
	CHAR8* retValue															= static_cast<UINT8*>(MmAllocatePool(strlen(inputString) + 1));

	if (retValue)
		strcpy(retValue, inputString);

	return retValue;
}

//
// Convert path separator.
//
VOID BlConvertPathSeparator(CHAR8* pathName, CHAR8 fromChar, CHAR8 toChar)
{
	UINTN pathLength														= strlen(pathName);

	for (UINTN i = 0; i < pathLength; i ++)
	{
		if (pathName[i] == fromChar)
			pathName[i]														= toChar;
	}
}

//
// UUID to buffer.
//
BOOLEAN BlUUIDStringToBuffer(CHAR8 CONST* uuidString, UINT8* uuidBuffer)
{
	//
	// 00112233-4455-6677-8899-aabbccddeeff
	//
	if (strlen(uuidString) != 36 || uuidString[8] != '-' || uuidString[13] != '-' || uuidString[18] != '-' || uuidString[23] != '-')
		return FALSE;

#define CHAR_TO_NIBBLE(c)													((c) >= '0' && (c) <= '9' ? (c) - '0' : ((c) >= 'a' && (c) <= 'f' ? (c) - 'a' + 10 : ((c) >= 'A' && (c) <= 'F' ? (c) - 'A' + 10 : 0)))
	uuidBuffer[ 0]															= (CHAR_TO_NIBBLE(uuidString[ 0]) << 4) + CHAR_TO_NIBBLE(uuidString[ 1]);
	uuidBuffer[ 1]															= (CHAR_TO_NIBBLE(uuidString[ 2]) << 4) + CHAR_TO_NIBBLE(uuidString[ 3]);
	uuidBuffer[ 2]															= (CHAR_TO_NIBBLE(uuidString[ 4]) << 4) + CHAR_TO_NIBBLE(uuidString[ 5]);
	uuidBuffer[ 3]															= (CHAR_TO_NIBBLE(uuidString[ 6]) << 4) + CHAR_TO_NIBBLE(uuidString[ 7]);
	uuidBuffer[ 4]															= (CHAR_TO_NIBBLE(uuidString[ 9]) << 4) + CHAR_TO_NIBBLE(uuidString[10]);
	uuidBuffer[ 5]															= (CHAR_TO_NIBBLE(uuidString[11]) << 4) + CHAR_TO_NIBBLE(uuidString[12]);
	uuidBuffer[ 6]															= (CHAR_TO_NIBBLE(uuidString[14]) << 4) + CHAR_TO_NIBBLE(uuidString[15]);
	uuidBuffer[ 7]															= (CHAR_TO_NIBBLE(uuidString[16]) << 4) + CHAR_TO_NIBBLE(uuidString[17]);
	uuidBuffer[ 8]															= (CHAR_TO_NIBBLE(uuidString[19]) << 4) + CHAR_TO_NIBBLE(uuidString[20]);
	uuidBuffer[ 9]															= (CHAR_TO_NIBBLE(uuidString[21]) << 4) + CHAR_TO_NIBBLE(uuidString[22]);
	uuidBuffer[10]															= (CHAR_TO_NIBBLE(uuidString[24]) << 4) + CHAR_TO_NIBBLE(uuidString[25]);
	uuidBuffer[11]															= (CHAR_TO_NIBBLE(uuidString[26]) << 4) + CHAR_TO_NIBBLE(uuidString[27]);
	uuidBuffer[12]															= (CHAR_TO_NIBBLE(uuidString[28]) << 4) + CHAR_TO_NIBBLE(uuidString[29]);
	uuidBuffer[13]															= (CHAR_TO_NIBBLE(uuidString[30]) << 4) + CHAR_TO_NIBBLE(uuidString[31]);
	uuidBuffer[14]															= (CHAR_TO_NIBBLE(uuidString[32]) << 4) + CHAR_TO_NIBBLE(uuidString[33]);
	uuidBuffer[15]															= (CHAR_TO_NIBBLE(uuidString[34]) << 4) + CHAR_TO_NIBBLE(uuidString[35]);
#undef CHAR_TO_NIBBLE

	return TRUE;
}

//
// Compare time.
//
INTN BlCompareTime(EFI_TIME* time1, EFI_TIME* time2)
{
	if (time1->Year < time2->Year)
		return -1;

	if (time1->Year > time2->Year)
		return 1;

	if (time1->Month < time2->Month)
		return -1;

	if (time1->Month > time2->Month)
		return 1;

	if (time1->Day < time2->Day)
		return -1;

	if (time1->Day > time2->Day)
		return 1;

	if (time1->Hour < time2->Hour)
		return -1;

	if (time1->Hour > time2->Hour)
		return 1;

	if (time1->Minute < time2->Minute)
		return -1;

	if (time1->Minute > time2->Minute)
		return 1;

	if (time1->Second < time2->Second)
		return -1;

	if (time1->Second > time2->Second)
		return 1;

	return 0;
}

//
// Add one second.
//
VOID BlAddOneSecond(EFI_TIME* theTime)
{
	theTime->Second															+= 1;

	if (theTime->Second > 59)
	{
		theTime->Second														= 0;
		theTime->Minute														+= 1;

		if (theTime->Minute > 59)
		{
			theTime->Minute													= 0;
			theTime->Hour													+= 1;

			if (theTime->Hour > 23)
			{
				theTime->Hour												= 0;
				theTime->Day												+= 1;

				UINT32 daysOfMonth											= ((1 << theTime->Month) & 0x15aa) ? 31 : 30;

				if (theTime->Month == 2)
					daysOfMonth												= IS_LEAP_YEAR(theTime->Year) ? 29 : 28;

				if (theTime->Day > daysOfMonth)
				{
					theTime->Day											= 1;
					theTime->Month											+= 1;

					if (theTime->Month > 12)
					{
						theTime->Month										= 1;
						theTime->Year										+= 1;
					}
				}
			}
		}
	}
}

//
// EFI time to UNIX time.
//
UINT32 BlEfiTimeToUnixTime(EFI_TIME CONST* efiTime)
{
	STATIC UINT32 cumulativeDays[2][14] =
	{
		{
			0, 0,
			31,
			31 + 28,
			31 + 28 + 31,
			31 + 28 + 31 + 30,
			31 + 28 + 31 + 30 + 31,
			31 + 28 + 31 + 30 + 31 + 30,
			31 + 28 + 31 + 30 + 31 + 30 + 31,
			31 + 28 + 31 + 30 + 31 + 30 + 31 + 31,
			31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30,
			31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31,
			31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31 + 30,
			31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31 + 30 + 31,
		},

		{
			0, 0,
			31,
			31 + 29,
			31 + 29 + 31,
			31 + 29 + 31 + 30,
			31 + 29 + 31 + 30 + 31,
			31 + 29 + 31 + 30 + 31 + 30,
			31 + 29 + 31 + 30 + 31 + 30 + 31,
			31 + 29 + 31 + 30 + 31 + 30 + 31 + 31,
			31 + 29 + 31 + 30 + 31 + 30 + 31 + 31 + 30,
			31 + 29 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31,
			31 + 29 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31 + 30,
			31 + 29 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31 + 30 + 31,
		},
	};

	//
	// Check range.
	//
	if (efiTime->Year < 1998 || efiTime->Year > 2099 || !efiTime->Month || efiTime->Month > 12 || !efiTime->Day || efiTime->Day > 31 || efiTime->Hour > 23 || efiTime->Minute > 59 || efiTime->Second > 59)
		return 0;

	//
	// Check timezone.
	//
	if (efiTime->TimeZone < -1440 || (efiTime->TimeZone > 1440 && efiTime->TimeZone != EFI_UNSPECIFIED_TIMEZONE))
		return 0;

	//
	// Count year.
	//
	UINT32 retValue															= 0;

	for (UINT16 year = 1970; year < efiTime->Year; year ++)
		retValue															+= cumulativeDays[IS_LEAP_YEAR(year)][13] * 60 * 60 * 24;

	//
	// Count month, day, hour, minute, second.
	//
	retValue																+= cumulativeDays[IS_LEAP_YEAR(efiTime->Year)][efiTime->Month] * 60 * 60 * 24;
	retValue																+= efiTime->Day > 0 ? (efiTime->Day - 1) * 60 * 60 * 24 : 0;
	retValue																+= efiTime->Hour * 60 * 60;
	retValue																+= efiTime->Minute * 60;
	retValue																+= efiTime->Second;

	//
	// EFI time is reported in local time, adjust time zone (time zone is kept in minutes).
	//
	if (efiTime->TimeZone != EFI_UNSPECIFIED_TIMEZONE)
		retValue															+= efiTime->TimeZone * 60;

	return retValue;
}

//
// Get current UNIX time.
//
UINT32 BlGetCurrentUnixTime()
{
	EFI_TIME efiTime;
	EFI_TIME_CAPABILITIES timeCapabilities;

	if (EFI_ERROR(EfiRuntimeServices->GetTime(&efiTime, &timeCapabilities)))
		return 0;

	return BlEfiTimeToUnixTime(&efiTime);
}

//
// Returns adler32.
//
UINT32 BlAdler32(VOID CONST* inputBuffer, UINTN bufferLength)
{
	UINT32 lowHalf															= 1;
	UINT32 highHalf															= 0;
	UINT8 CONST* theBuffer													= static_cast<UINT8 CONST*>(inputBuffer);

	for (UINT32 i = 0; i < bufferLength; i ++)
	{
		if ((i % 5000) == 0)
		{
			lowHalf															%= 65521;
			highHalf														%= 65521;
		}

		lowHalf																+= theBuffer[i];
		highHalf															+= lowHalf;
	}

	lowHalf																	%= 65521;
	highHalf																%= 65521;

	return (highHalf << 16) | lowHalf;
}

//
// Uncompress LZSS.
//
EFI_STATUS BlDecompressLZSS(VOID CONST* compressedBuffer, UINTN compressedSize, VOID* uncompressedBuffer, UINTN uncompressedBufferSize, UINTN* uncompressedSize)
{
	#define N																4096
	#define F																18
	#define THRESHOLD														2
	#define NIL																N

	UINT8 textBuffer[N + F - 1]												= {0};
	UINT8* dstBuffer														= static_cast<UINT8*>(uncompressedBuffer);
	UINT8* dstStart															= dstBuffer;
	UINT8 CONST* srcBuffer													= static_cast<UINT8 CONST*>(compressedBuffer);
	UINT8 CONST* srcend														= srcBuffer + compressedSize;
	INT32 i																	= 0;
	INT32 j																	= 0;
	UINT8 c																	= 0;
	INT32 r																	= N - F;
	UINT32 flags															= 0;

	for (i = 0; i < N - F; i ++)
		textBuffer[i]														= ' ';

	while (TRUE)
	{
		if (((flags >>= 1) & 0x100) == 0)
		{
			if (srcBuffer < srcend)
				c															= *srcBuffer ++;
			else
				break;

			flags															= c | 0xFF00;
		}

		if (flags & 1)
		{
			if (srcBuffer < srcend)
				c															= *srcBuffer ++;
			else
				break;

			*dstBuffer ++													= c;
			textBuffer[r++]													= c;
			r																&= (N - 1);
		}
		else
		{
			if (srcBuffer < srcend)
				i															= *srcBuffer ++;
			else
				break;

			if (srcBuffer < srcend)
				j															= *srcBuffer ++;
			else
				break;
			i																|= ((j & 0xf0) << 4);
			j																=  (j & 0x0f) + THRESHOLD;

			for (INT32 k = 0; k <= j; k ++)
			{
				c															= textBuffer[(i + k) & (N - 1)];
				*dstBuffer ++												= c;
				textBuffer[r++]												= c;
				r															&= (N - 1);
			}
		}
	}

	*uncompressedSize														= dstBuffer - dstStart;

	return EFI_SUCCESS;
}

#if (TARGET_OS >= YOSEMITE)
//
// Uncompress LZVN.
//
EFI_STATUS BlDecompressLZVN(VOID CONST* compressedBuffer, UINTN aCompressedSize, VOID* uncompressedBuffer, UINTN uncompressedBufferSize, UINTN* uncompressedSize)
{
	const UINT64 decompBuffer	= (const UINT64)uncompressedBuffer;
	
	UINT64 length				= 0;												// xor	%rax,%rax
	UINT64 decompressedSize		= (UINT64)uncompressedBufferSize;
	UINT64 compressedSize		= (UINT64)aCompressedSize;
	UINT64 compBuffer			= (UINT64)compressedBuffer;
	UINT64 compBufferPointer	= 0;

	UINT64 caseTableIndex		= 0;
	UINT64 byteCount			= 0;
	UINT64 currentLength		= 0;												// xor	%r12,%r12
	UINT64 negativeOffset		= 0;
	UINT64 address				= 0;												// ((UINT64)compBuffer + compBufferPointer)

	UINT8 jmpTo					= CASE_TABLE;

	CHAR8 byte_data				= 0;

	// Jump table developed by 'MinusZwei'
	static short caseTable[ 256 ] =
	{
		1,  1,  1,  1,    1,  1,  2,  3,    1,  1,  1,  1,    1,  1,  4,  3,
		1,  1,  1,  1,    1,  1,  4,  3,    1,  1,  1,  1,    1,  1,  5,  3,
		1,  1,  1,  1,    1,  1,  5,  3,    1,  1,  1,  1,    1,  1,  5,  3,
		1,  1,  1,  1,    1,  1,  5,  3,    1,  1,  1,  1,    1,  1,  5,  3,
		1,  1,  1,  1,    1,  1,  0,  3,    1,  1,  1,  1,    1,  1,  0,  3,
		1,  1,  1,  1,    1,  1,  0,  3,    1,  1,  1,  1,    1,  1,  0,  3,
		1,  1,  1,  1,    1,  1,  0,  3,    1,  1,  1,  1,    1,  1,  0,  3,
		5,  5,  5,  5,    5,  5,  5,  5,    5,  5,  5,  5,    5,  5,  5,  5,
		1,  1,  1,  1,    1,  1,  0,  3,    1,  1,  1,  1,    1,  1,  0,  3,
		1,  1,  1,  1,    1,  1,  0,  3,    1,  1,  1,  1,    1,  1,  0,  3,
		6,  6,  6,  6,    6,  6,  6,  6,    6,  6,  6,  6,    6,  6,  6,  6,
		6,  6,  6,  6,    6,  6,  6,  6,    6,  6,  6,  6,    6,  6,  6,  6,
		1,  1,  1,  1,    1,  1,  0,  3,    1,  1,  1,  1,    1,  1,  0,  3,
		5,  5,  5,  5,    5,  5,  5,  5,    5,  5,  5,  5,    5,  5,  5,  5,
		7,  8,  8,  8,    8,  8,  8,  8,    8,  8,  8,  8,    8,  8,  8,  8,
		9, 10, 10, 10,   10, 10, 10, 10,   10, 10, 10, 10,   10, 10, 10, 10
	};
	
	decompressedSize -= 8;															// sub	$0x8,%rsi
	
	if (decompressedSize < 8)														// jb	Llzvn_exit
	{
		return EFI_LOAD_ERROR;
	}
	
	compressedSize = (compBuffer + compressedSize - 8);								// lea	-0x8(%rdx,%rcx,1),%rcx
	
	if (compBuffer > compressedSize)												// cmp	%rcx,%rdx
	{
		return EFI_LOAD_ERROR;														// ja	Llzvn_exit
	}
	
	compBufferPointer = *(UINT64 *)compBuffer;										// mov	(%rdx),%r8
	caseTableIndex = (compBufferPointer & 255);										// movzbq	(%rdx),%r9
	
	do																				// jmpq	*(%rbx,%r9,8)
	{
		switch (jmpTo)																// our jump table
		{
			case CASE_TABLE: /******************************************************/
				
				switch (caseTable[(UINT8)caseTableIndex])
				{
					case 0:
						caseTableIndex >>= 6;										// shr	$0x6,%r9
						compBuffer = (compBuffer + caseTableIndex + 1);				// lea	0x1(%rdx,%r9,1),%rdx
						
						if (compBuffer > compressedSize)							// cmp	%rcx,%rdx
						{
							return EFI_LOAD_ERROR;									// ja	Llzvn_exit
						}
						
						byteCount = 56;												// mov	$0x38,%r10
						byteCount &= compBufferPointer;								// and	%r8,%r10
						compBufferPointer >>= 8;									// shr	$0x8,%r8
						byteCount >>= 3;											// shr	$0x3,%r10
						byteCount += 3;												// add	$0x3,%r10
						
						jmpTo = LZVN_10;											// jmp	Llzvn_l10
						break;
						
					case 1:
						caseTableIndex >>= 6;										// shr	$0x6,%r9
						compBuffer = (compBuffer + caseTableIndex + 2);				// lea	0x2(%rdx,%r9,1),%rdx
						
						if (compBuffer > compressedSize)							// cmp	%rcx,%rdx
						{
							return EFI_LOAD_ERROR;									// ja	Llzvn_exit
						}
						
						negativeOffset = compBufferPointer;							// mov	%r8,%r12
						negativeOffset = OSSwapInt64(negativeOffset);				// bswap	%r12
						byteCount = negativeOffset;									// mov	%r12,%r10
						negativeOffset <<= 5;										// shl	$0x5,%r12
						byteCount <<= 2;											// shl	$0x2,%r10
						negativeOffset >>= 53;										// shr	$0x35,%r12
						byteCount >>= 61;											// shr	$0x3d,%r10
						compBufferPointer >>= 16;									// shr	$0x10,%r8
						byteCount += 3;												// add	$0x3,%r10
						
						jmpTo = LZVN_10;											// jmp	Llzvn_l10
						break;
						
					case 2:
						*uncompressedSize = (length & EFI_MAX_ADDRESS);				// Maximum legal IA-32 length

						return EFI_SUCCESS;
					
					case 3:
						caseTableIndex >>= 6;										// shr	$0x6,%r9
						compBuffer = (compBuffer + caseTableIndex + 3);				// lea	0x3(%rdx,%r9,1),%rdx
						
						if (compBuffer > compressedSize)							// cmp	%rcx,%rdx
						{
							return EFI_LOAD_ERROR;									// ja	Llzvn_exit
						}
						
						byteCount = 56;												// mov	$0x38,%r10
						negativeOffset = 65535;										// mov	$0xffff,%r12
						byteCount &= compBufferPointer;								// and	%r8,%r10
						compBufferPointer >>= 8;									// shr	$0x8,%r8
						byteCount >>= 3;											// shr	$0x3,%r10
						negativeOffset &= compBufferPointer;						// and	%r8,%r12
						compBufferPointer >>= 16;									// shr	$0x10,%r8
						byteCount += 3;												// add	$0x3,%r10
						
						jmpTo = LZVN_10;											// jmp	Llzvn_l10
						break;
						
					case 4:
						compBuffer += 1;											// add	$0x1,%rdx
						
						if (compBuffer > compressedSize)							// cmp	%rcx,%rdx
						{
							return EFI_LOAD_ERROR;									// ja	Llzvn_exit
						}
						
						compBufferPointer = *(UINT64 *)compBuffer;					// mov	(%rdx),%r8
						caseTableIndex = (compBufferPointer & 255);					// movzbq (%rdx),%r9
						
						jmpTo = CASE_TABLE;											// continue;
						break;														// jmpq	*(%rbx,%r9,8)
						
					case 5:
						return EFI_LOAD_ERROR;										// Llzvn_table5;
						
					case 6:
						caseTableIndex >>= 3;										// shr	$0x3,%r9
						caseTableIndex &= 3;										// and	$0x3,%r9
						compBuffer = (compBuffer + caseTableIndex + 3);				// lea	0x3(%rdx,%r9,1),%rdx
						
						if (compBuffer > compressedSize)							// cmp	%rcx,%rdx
						{
							return EFI_LOAD_ERROR;									// ja	Llzvn_exit
						}
						
						byteCount = compBufferPointer;								// mov	%r8,%r10
						byteCount &= 775;											// and	$0x307,%r10
						compBufferPointer >>= 10;									// shr	$0xa,%r8
						negativeOffset = (byteCount & 255);							// movzbq %r10b,%r12
						byteCount >>= 8;											// shr	$0x8,%r10
						negativeOffset <<= 2;										// shl	$0x2,%r12
						byteCount |= negativeOffset;								// or	%r12,%r10
						negativeOffset = 16383;										// mov	$0x3fff,%r12
						byteCount += 3;												// add	$0x3,%r10
						negativeOffset &= compBufferPointer;						// and	%r8,%r12
						compBufferPointer >>= 14;									// shr	$0xe,%r8
						
						jmpTo = LZVN_10;											// jmp	Llzvn_l10
						break;
					
					case 7:
						compBufferPointer >>= 8;									// shr	$0x8,%r8
						compBufferPointer &= 255;									// and	$0xff,%r8
						compBufferPointer += 16;									// add	$0x10,%r8
						compBuffer = (compBuffer + compBufferPointer + 2);			// lea	0x2(%rdx,%r8,1),%rdx
						
						jmpTo = LZVN_0;												// jmp	Llzvn_l0
						break;
						
					case 8:
						compBufferPointer &= 15;									// and	$0xf,%r8
						compBuffer = (compBuffer + compBufferPointer + 1);			// lea	0x1(%rdx,%r8,1),%rdx
						
						jmpTo = LZVN_0;												// jmp	Llzvn_l0
						break;
						
					case 9:
						compBuffer += 2;											// add	$0x2,%rdx
						
						if (compBuffer > compressedSize)							// cmp	%rcx,%rdx
						{
							return EFI_LOAD_ERROR;									// ja	Llzvn_exit
						}
						
						// Up most significant byte (count) by 16 (0x10/16 - 0x10f/271).
						byteCount = compBufferPointer;								// mov	%r8,%r10
						byteCount >>= 8;											// shr	$0x8,%r10
						byteCount &= 255;											// and	$0xff,%r10
						byteCount += 16;											// add	$0x10,%r10
						
						jmpTo = LZVN_11;											// jmp	Llzvn_l11
						break;
						
					case 10:
						compBuffer += 1;											// add	$0x1,%rdx
						
						if (compBuffer > compressedSize)							// cmp	%rcx,%rdx
						{
							return EFI_LOAD_ERROR;									// ja	Llzvn_exit
						}
						
						byteCount = compBufferPointer;								// mov	%r8,%r10
						byteCount &= 15;											// and	$0xf,%r10
						
						jmpTo = LZVN_11;											// jmp	Llzvn_l11
						break;
						
					default:return EFI_LOAD_ERROR;
						
				}																	// switch (caseTable[caseTableIndex])
				
				break;
				
			case LZVN_0: /**********************************************************/
				
				if (compBuffer > compressedSize)									// cmp	%rcx,%rdx
				{
					return EFI_LOAD_ERROR;											// ja	Llzvn_exit
				}
				
				currentLength = (length + compBufferPointer);						// lea	(%rax,%r8,1),%r11
				compBufferPointer = (0 - compBufferPointer);						// neg	%r8
				
				if (currentLength > decompressedSize)								// cmp	%rsi,%r11
				{
					jmpTo = LZVN_2;													// ja	Llzvn_l2
					break;
				}
				
				currentLength = (decompBuffer + currentLength);						// lea	(%rdi,%r11,1),%r11
				
			case LZVN_1: /**********************************************************/
				
				do																	// Llzvn_l1:
				{
					caseTableIndex = *(UINT64 *)((UINT64)compBuffer + compBufferPointer);
					*(UINT64 *)((UINT64)currentLength + compBufferPointer) = caseTableIndex;
					
					compBufferPointer += 8;											// add	$0x8,%r8
					
				} while ((0xFFFFFFFFFFFFFFFF - (compBufferPointer - 8)) >= 8);		// jae	Llzvn_l1
				
				length = currentLength;												// mov	%r11,%rax
				length -= decompBuffer;												// sub	%rdi,%rax
				
				compBufferPointer = *(UINT64 *)compBuffer;							// mov	(%rdx),%r8
				caseTableIndex = (compBufferPointer & 255);							// movzbq (%rdx),%r9
				
				jmpTo = CASE_TABLE;
				break;																// jmpq	*(%rbx,%r9,8)
				
			case LZVN_2: /**********************************************************/
				
				currentLength = (decompressedSize + 8);								// lea	0x8(%rsi),%r11
				
			case LZVN_3: /***********************************************************/
				
				do																	// Llzvn_l3:
				{
					address = (compBuffer + compBufferPointer);						// movzbq (%rdx,%r8,1),%r9
					caseTableIndex = *((UINT64 *)address);
					caseTableIndex &= 255;
					
					address = (decompBuffer + length);								// mov	%r9b,(%rdi,%rax,1)
					byte_data = (CHAR8)caseTableIndex;
					memcpy((void *)address, &byte_data, sizeof(byte_data));
					
					length += 1;													// add	$0x1,%rax
					
					if (currentLength == length)									// cmp	%rax,%r11
					{
						*uncompressedSize = (length & EFI_MAX_ADDRESS);				// Maximum legal IA-32 length

						return EFI_SUCCESS;											// je	Llzvn_exit2
					}
					
					compBufferPointer += 1;											// add	$0x1,%r8
					
				} while ((int64_t)compBufferPointer != 0);							// jne	Llzvn_l3
				
				compBufferPointer = *(UINT64 *)compBuffer;							// mov	(%rdx),%r8
				caseTableIndex = (compBufferPointer & 255);							// movzbq	(%rdx),%r9
				
				jmpTo = CASE_TABLE;
				break;																// jmpq	*(%rbx,%r9,8)
				
			case LZVN_4: /**********************************************************/
				
				currentLength = (decompressedSize + 8);								// lea	0x8(%rsi),%r11
				
			case LZVN_9: /**********************************************************/
				
				do																	// Llzvn_l9:
				{
					address = (decompBuffer + compBufferPointer);					// movzbq (%rdi,%r8,1),%r9
					byte_data = *((CHAR8 *)address);
					caseTableIndex = byte_data;
					caseTableIndex &= 255;
					compBufferPointer += 1;											// add	$0x1,%r8
					
					address = (decompBuffer + length);								// mov	%r9,(%rdi,%rax,1)
					byte_data = (CHAR8)caseTableIndex;
					memcpy((void *)address, &byte_data, sizeof(byte_data));
					
					length += 1;													// add	$0x1,%rax
					
					if (length == currentLength)									// cmp	%rax,%r11
					{
						*uncompressedSize = (length & EFI_MAX_ADDRESS);				// Maximum legal IA-32 length

						return EFI_SUCCESS;											// je	Llzvn_exit2
					}
					
					byteCount -= 1;													// sub	$0x1,%r10
					
				} while (byteCount);												// jne	Llzvn_l9
				
				compBufferPointer = *(UINT64 *)compBuffer;							// mov	(%rdx),%r8
				caseTableIndex = (compBufferPointer & 255);							// movzbq	(%rdx),%r9
				
				jmpTo = CASE_TABLE;
				break;																// jmpq	*(%rbx,%r9,8)
				
			case LZVN_5: /**********************************************************/
				
				do
				{
					address = (decompBuffer + compBufferPointer);					// mov	(%rdi,%r8,1),%r9
					caseTableIndex = *((UINT64 *)address);
					compBufferPointer += 8;											// add	$0x8,%r8
					
					address = (decompBuffer + length);								// mov	%r9,(%rdi,%rax,1)
					memcpy((void *)address, &caseTableIndex, sizeof(caseTableIndex));
					
					length += 8;													// add	$0x8,%rax
					byteCount -= 8;													// sub	$0x8,%r10
					
				} while ((byteCount + 8) > 8);										// ja	Llzvn_l5
				
				length += byteCount;												// add	%r10,%rax
				compBufferPointer = *(UINT64 *)compBuffer;							// mov	(%rdx),%r8
				caseTableIndex = (compBufferPointer & 255);							// movzbq	(%rdx),%r9
				
				jmpTo = CASE_TABLE;
				break;																// jmpq	*(%rbx,%r9,8)
				
			case LZVN_10: /*********************************************************/
				
				currentLength = (length + caseTableIndex);							// lea	(%rax,%r9,1),%r11
				currentLength += byteCount;											// add	%r10,%r11
				
				if (currentLength < decompressedSize)								// cmp	%rsi,%r11 (block_end: jae	Llzvn_l8)
				{
					address = decompBuffer + length;								// mov	%r8,(%rdi,%rax,1)
					memcpy((void *)address, &compBufferPointer, sizeof(compBufferPointer));
					
					length += caseTableIndex;										// add	%r9,%rax
					compBufferPointer = length;										// mov	%rax,%r8
					
					if (compBufferPointer < negativeOffset)							// jb	Llzvn_exit
					{
						return EFI_LOAD_ERROR;
					}
					
					compBufferPointer -= negativeOffset;							// sub	%r12,%r8
					
					if (negativeOffset < 8)											// cmp	$0x8,%r12
					{
						jmpTo = LZVN_4;												// jb	Llzvn_l4
						break;
					}
					
					jmpTo = LZVN_5;													// jmpq	*(%rbx,%r9,8)
					break;
				}
				
			case LZVN_8: /**********************************************************/
				
				if (caseTableIndex == 0)											// test	%r9,%r9
				{
					jmpTo = LZVN_7;													// jmpq	*(%rbx,%r9,8)
					break;
				}
				
				currentLength = (decompressedSize + 8);								// lea	0x8(%rsi),%r11
				
			case LZVN_6: /**********************************************************/
				
				do
				{
					address = (decompBuffer + length);								// mov	%r8b,(%rdi,%rax,1)
					byte_data = (CHAR8)(compBufferPointer & 255);
					memcpy((void *)address, &byte_data, sizeof(byte_data));
					length += 1;													// add	$0x1,%rax
					
					if (length == currentLength)									// cmp	%rax,%r11
					{
						*uncompressedSize = (length & EFI_MAX_ADDRESS);				// Maximum legal IA-32 length

						return EFI_SUCCESS;											// je	Llzvn_exit2
					}
					
					compBufferPointer >>= 8;										// shr	$0x8,%r8
					caseTableIndex -= 1;											// sub	$0x1,%r9
					
				} while (caseTableIndex != 1);										// jne	Llzvn_l6
				
			case LZVN_7: /**********************************************************/
				
				compBufferPointer = length;											// mov	%rax,%r8
				compBufferPointer -= negativeOffset;								// sub	%r12,%r8
				
				if (compBufferPointer < negativeOffset)								// jb	Llzvn_exit
				{
					return EFI_LOAD_ERROR;
				}
				
				jmpTo = LZVN_4;
				break;																// jmpq	*(%rbx,%r9,8)
				
			case LZVN_11: /*********************************************************/
				
				compBufferPointer = length;											// mov	%rax,%r8
				compBufferPointer -= negativeOffset;								// sub	%r12,%r8
				currentLength = (length + byteCount);								// lea	(%rax,%r10,1),%r11
				
				if (currentLength < decompressedSize)								// cmp	%rsi,%r11
				{
					if (negativeOffset >= 8)										// cmp	$0x8,%r12
					{
						jmpTo = LZVN_5;												// jae	Llzvn_l5
						break;
					}
				}
				
				jmpTo = LZVN_4;														// jmp	Llzvn_l4
				break;
		}																			// switch (jmpq)
		
	} while (1);
	
	return EFI_LOAD_ERROR;
}
#endif // #if (TARGET_OS => YOSEMITE)
