//********************************************************************
//	created:	7:11:2009   1:38
//	filename: 	MiscUtils.cpp
//	author:		tiamo
//	purpose:	utils
//********************************************************************

#include "stdafx.h"

//
// global
//
STATIC UINT64 BlpMemoryCapacity												= 0;
STATIC UINT32 BlpMemoryDevices												= 0;
STATIC UINT64 BlpMemorySize													= 0;
STATIC CHAR8 BlpBoardId[64]													= {0};
#define IS_LEAP_YEAR(y)														(((y) % 4 == 0 && (y) % 100 != 0) || (y) % 400 == 0)

//
// get memory capacity
//
UINT64 BlGetMemoryCapacity()
{
	return BlpMemoryCapacity;
}

//
// get memory size
//
UINT64 BlGetMemorySize()
{
	return BlpMemorySize;
}

//
// get board id
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
// connect drivers
//
EFI_STATUS BlConnectAllController()
{
	UINTN lastHandles														= static_cast<UINTN>(-1);
	EFI_HANDLE* handleArray													= nullptr;
	EFI_STATUS status														= EFI_SUCCESS;

	__try
	{
		while(TRUE)
		{
			UINTN totalHandles												= 0;
			if(EFI_ERROR(status = EfiBootServices->LocateHandleBuffer(AllHandles, nullptr, nullptr, &totalHandles, &handleArray)))
				try_leave(NOTHING);

			if(lastHandles == totalHandles)
				try_leave(NOTHING);

			for(UINTN i = 0; i < totalHandles; i ++)
				EfiBootServices->ConnectController(handleArray[i], nullptr, nullptr, TRUE);

			EfiBootServices->FreePool(handleArray);
			handleArray														= nullptr;
			lastHandles														= totalHandles;
		}
	}
	__finally
	{
		if(handleArray)
			EfiBootServices->FreePool(handleArray);
	}

	return status;
}

//
// get base and size
//
EFI_STATUS BlGetApplicationBaseAndSize(UINT64* imageBase, UINT64* imageSize)
{
	EFI_LOADED_IMAGE_PROTOCOL* loadeImageProtocol							= nullptr;
	if(EfiBootServices->HandleProtocol(EfiImageHandle, &EfiLoadedImageProtocolGuid, reinterpret_cast<VOID**>(&loadeImageProtocol)) != EFI_SUCCESS)
		return EFI_UNSUPPORTED;

	if(imageBase)
		*imageBase															= ArchConvertPointerToAddress(loadeImageProtocol->ImageBase);

	if(imageSize)
		*imageSize															= loadeImageProtocol->ImageSize;

	return EFI_SUCCESS;
}

//
// parse device location
//
VOID BlParseDeviceLocation(CHAR8 CONST* loaderOptions, UINTN* segment, UINTN* bus, UINTN* device, UINTN* func)
{
	if(loaderOptions && loaderOptions[0])
	{
		CHAR8 CONST* temp													= strstr(loaderOptions, CHAR8_CONST_STRING("/seg="));
		if(temp && segment)
			*segment														= static_cast<UINTN>(atoi(temp + 5));

		temp																= strstr(loaderOptions, CHAR8_CONST_STRING("/bus="));
		if(temp && bus)
			*bus															= static_cast<UINTN>(atoi(temp + 5));

		temp																= strstr(loaderOptions, CHAR8_CONST_STRING("/dev="));
		if(temp && device)
			*device															= static_cast<UINTN>(atoi(temp + 5));

		temp																= strstr(loaderOptions, CHAR8_CONST_STRING("/func="));
		if(temp && func)
			*func															= static_cast<UINTN>(atoi(temp + 6));
	}
}

//
// pci get bar attribute
//
EFI_STATUS BlGetPciBarAttribute(EFI_PCI_IO_PROTOCOL* pciIoProtocol, UINT8 barIndex, UINT64* baseAddress, UINT64* barLength, BOOLEAN* isMemorySpace)
{
	VOID* pciResource														= nullptr;
	EFI_STATUS Status														= pciIoProtocol->GetBarAttributes(pciIoProtocol, barIndex, nullptr, &pciResource);

	//
	// get information
	//
	if(!EFI_ERROR(Status) && pciResource)
	{
		//
		// pci resource is an acpi descriptor
		//
		EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR* desc								= static_cast<EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR*>(pciResource);

		//
		// check it
		//
		if(desc->Desc == ACPI_ADDRESS_SPACE_DESCRIPTOR)
		{
			//
			// output
			//
			if(baseAddress)
				*baseAddress												= desc->AddrRangeMin;

			if(barLength)
				*barLength													= desc->AddrLen;

			if(isMemorySpace)
				*isMemorySpace												= desc->ResType == ACPI_ADDRESS_SPACE_TYPE_MEM;
		}
	}

	//
	// free this pointer
	//
	if(pciResource)
		EfiBootServices->FreePool(pciResource);

	return Status;
}

//
// find pci device
//
EFI_STATUS BlFindPciDevice(UINTN segment, UINTN bus, UINTN device, UINTN func, UINT8 baseClass, UINT8 subClass, UINT8 progIf, EFI_PCI_IO_PROTOCOL** outPciIoProtocol, EFI_HANDLE* outHandle)
{
	UINTN totalHandles														= 0;
	EFI_HANDLE* handleArray													= nullptr;
	EFI_STATUS status														= EFI_SUCCESS;

	__try
	{
		//
		// locate pci io protocol handle
		//
		if(EFI_ERROR(status = EfiBootServices->LocateHandleBuffer(ByProtocol, &EfiPciIoProtocolGuid, nullptr, &totalHandles, &handleArray)))
			try_leave(NOTHING);

		//
		// for each handle, check it
		//
		status																= EFI_NOT_FOUND;
		for(UINTN i = 0; i < totalHandles; i ++)
		{
			//
			// open pci io protocol
			//
			EFI_HANDLE theHandle											= handleArray[i];
			EFI_PCI_IO_PROTOCOL* pciIoProtocol								= nullptr;

			//
			// if open failed, skip it
			//
			if(EFI_ERROR(EfiBootServices->HandleProtocol(theHandle, &EfiPciIoProtocolGuid, reinterpret_cast<VOID**>(&pciIoProtocol))))
				continue;

			//
			// get location
			//
			UINTN curSeg													= static_cast<UINTN>(-1);
			UINTN curBus													= static_cast<UINTN>(-1);
			UINTN curDev													= static_cast<UINTN>(-1);
			UINTN curFunc													= static_cast<UINTN>(-1);
			if(EFI_ERROR(pciIoProtocol->GetLocation(pciIoProtocol, &curSeg, &curBus, &curDev, &curFunc)))
				continue;

			//
			// check location
			//
			if((segment != -1 && segment != curSeg) || (bus != -1 && bus != curBus) || (device != -1 && device != curDev) || (func != -1 && func != curFunc))
				continue;

			//
			// read config space
			//
			PCI_DEVICE_INDEPENDENT_REGION configHeader;
			if(EFI_ERROR(pciIoProtocol->Pci.Read(pciIoProtocol, EfiPciIoWidthUint8, 0, sizeof(configHeader), &configHeader)))
				continue;

			//
			// check base/sub/progif
			//
			if(configHeader.ClassCode[2] == baseClass && configHeader.ClassCode[1] == subClass && configHeader.ClassCode[0] == progIf)
				try_leave(if(outPciIoProtocol) *outPciIoProtocol = pciIoProtocol; if(outHandle) *outHandle = theHandle; status = EFI_SUCCESS);
		}
	}
	__finally
	{
		if(handleArray)
			EfiBootServices->FreePool(handleArray);
	}
	return status;
}

//
// start pci device
//
EFI_STATUS BlStartPciDevice(EFI_PCI_IO_PROTOCOL* pciIoProtocol, BOOLEAN decodeIo, BOOLEAN decodeMemory, BOOLEAN busMaster)
{
	UINT64 attribute														= decodeIo ? EFI_PCI_IO_ATTRIBUTE_IO : 0;
	attribute																|= (decodeMemory ? EFI_PCI_IO_ATTRIBUTE_MEMORY : 0);
	attribute																|= (busMaster ? EFI_PCI_IO_ATTRIBUTE_BUS_MASTER : 0);
	return pciIoProtocol->Attributes(pciIoProtocol, EfiPciIoAttributeOperationEnable, attribute, nullptr);
}

//
// get string from smbios table
//
UINT8* BlpGetStringFromSMBIOSTable(UINT8* startOfStringTable, UINT8 index)
{
	for(UINT8 i = 1; i < index && *startOfStringTable; i ++)
		startOfStringTable													+= strlen(reinterpret_cast<CHAR8*>(startOfStringTable)) + 1;

	STATIC UINT8 BadIndex[]													= "BadIndex";
	return *startOfStringTable ? startOfStringTable : BadIndex;
}

//
// detect memory size
//
EFI_STATUS BlDetectMemorySize()
{
	EFI_CONFIGURATION_TABLE* theTable										= EfiSystemTable->ConfigurationTable;
	for(UINTN i = 0; i < EfiSystemTable->NumberOfTableEntries; i ++, theTable ++)
	{
		if(memcmp(&theTable->VendorGuid, &EfiSmbiosTableGuid, sizeof(EfiSmbiosTableGuid)))
			continue;

		SMBIOS_TABLE_STRUCTURE* tableStructure								= static_cast<SMBIOS_TABLE_STRUCTURE*>(theTable->VendorTable);
		if(memcmp(tableStructure->AnchorString, "_SM_", sizeof(tableStructure->AnchorString)))
			break;

		UINT8* startOfTable													= ArchConvertAddressToPointer(tableStructure->TableAddress, UINT8*);
		UINT8* endOfTable													= startOfTable + tableStructure->TableLength;

		while(startOfTable + sizeof(SMBIOS_TABLE_HEADER) <= endOfTable)
		{
			SMBIOS_TABLE_HEADER* tableHeader								= reinterpret_cast<SMBIOS_TABLE_HEADER*>(startOfTable);
			if(tableHeader->Type == 16)
			{
				if(startOfTable + sizeof(SMBIOS_TABLE_TYPE16) > endOfTable)
					break;

				SMBIOS_TABLE_TYPE16* table16								= reinterpret_cast<SMBIOS_TABLE_TYPE16*>(startOfTable);
				if(table16->MaximumCapacity != 0x80000000)
				{
					BlpMemoryCapacity										+= (static_cast<UINT64>(table16->MaximumCapacity) << 10);
					BlpMemoryDevices										+= table16->NumberOfMemoryDevices;
				}
			}
			else if(tableHeader->Type == 17)
			{
				if(startOfTable + sizeof(SMBIOS_TABLE_TYPE17) > endOfTable)
					break;

				SMBIOS_TABLE_TYPE17* table17								= reinterpret_cast<SMBIOS_TABLE_TYPE17*>(startOfTable);
				if(table17->Size != 0xffff)
					BlpMemorySize											+= (static_cast<UINT64>(table17->Size) << ((table17->Size & 0x8000) ? 10 : 20));
			}
			else if(tableHeader->Type == 2)
			{
				if(startOfTable + sizeof(SMBIOS_TABLE_TYPE2) > endOfTable)
					break;

				SMBIOS_TABLE_TYPE2* table2									= reinterpret_cast<SMBIOS_TABLE_TYPE2*>(startOfTable);
				if(table2->ProductName)
				{
					UINT8* boardId											= BlpGetStringFromSMBIOSTable(startOfTable + table2->Hdr.Length, table2->ProductName);
					strncpy(BlpBoardId, reinterpret_cast<CHAR8*>(boardId), ARRAYSIZE(BlpBoardId) - 1);
				}
			}
			startOfTable													+= tableHeader->Length;

			while(startOfTable < endOfTable)
			{
				if(startOfTable[0] || startOfTable + 1 >= endOfTable || startOfTable[1])
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

#define READ_BUFFER(B, L, I, V, T)											do{if((I) >= (L)) return EFI_BAD_BUFFER_SIZE; (V) = static_cast<T>((B)[(I)]); (I) += 1;}while(0)
#define WRITE_BUFFER(B, L, I, V, T)											do{if((I) >= (L) - 1) return EFI_BUFFER_TOO_SMALL; (B)[(I)] = static_cast<T>(V); (I) += 1;}while(0)

//
// unicode to utf8
//
EFI_STATUS BlUnicodeToUtf8(CHAR16 CONST* unicodeBuffer, UINTN unicodeCharCount, CHAR8* utf8Buffer, UINTN utf8BufferLength)
{
	UINT32 j																= 0;
	utf8Buffer[utf8BufferLength - 1]										= 0;
	for(UINT32 i = 0; i < unicodeCharCount; i ++)
	{
		CHAR16 unicodeChar													= unicodeBuffer[i];
		if(unicodeChar < 0x0080)
		{
			WRITE_BUFFER(utf8Buffer, utf8BufferLength, j, unicodeChar, UINT8);
		}
		else if(unicodeChar < 0x0800)
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

	if(j < utf8BufferLength - 1)
		WRITE_BUFFER(utf8Buffer, utf8BufferLength, j, 0, UINT8);

	return EFI_SUCCESS;
}

//
// unicode to ansi
//
EFI_STATUS BlUnicodeToAnsi(CHAR16 CONST* unicodeBuffer, UINTN unicodeCharCount, CHAR8* ansiBuffer, UINTN ansiBufferLength)
{
	UINTN j																	= 0;
	ansiBuffer[ansiBufferLength - 1]										= 0;
	for(UINTN i = 0; i < unicodeCharCount; i ++)
	{
		CHAR16 unicodeChar													= unicodeBuffer[i];
		WRITE_BUFFER(ansiBuffer, ansiBufferLength, j, unicodeChar & 0xff, CHAR8);
	}

	if(j < ansiBufferLength - 1)
		WRITE_BUFFER(ansiBuffer, ansiBufferLength, j, 0, CHAR8);

	return EFI_SUCCESS;
}

//
// utf8 to unicode
//
EFI_STATUS BlUtf8ToUnicode(CHAR8 CONST* utf8Buffer, UINTN bytesCount, CHAR16* unicodeBuffer, UINTN unicodeBufferLengthInChar)
{
	UINTN i																	= 0;
	UINTN j																	= 0;
	unicodeBuffer[unicodeBufferLengthInChar - 1]							= 0;
	while(i < bytesCount)
	{
		UINT8 utf8Char1;
		UINT8 utf8Char2;
		UINT8 utf8Char3;
		READ_BUFFER(utf8Buffer, bytesCount, i, utf8Char1, UINT8);

		if(utf8Char1 < 0x80)
		{
			WRITE_BUFFER(unicodeBuffer, unicodeBufferLengthInChar, j, utf8Char1, CHAR16);
		}
		else if(utf8Char1 < 0xe0)
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

	if(j < unicodeBufferLengthInChar - 1)
		WRITE_BUFFER(unicodeBuffer, unicodeBufferLengthInChar, j, 0, CHAR16);

	return EFI_SUCCESS;
}

#undef READ_BUFFER
#undef WRITE_BUFFER

//
// build utf8 string from unicode
//
CHAR8* BlAllocateUtf8FromUnicode(CHAR16 CONST* unicodeString, UINTN unicodeCharCount)
{
	UINTN length															= unicodeCharCount == -1 ? wcslen(unicodeString) : unicodeCharCount;
	UINTN utf8BufferLength													= (length * 3 + 1) * sizeof(CHAR8);
	UINT8* utf8NameBuffer													= static_cast<UINT8*>(MmAllocatePool(utf8BufferLength));
	if(!utf8NameBuffer)
		return nullptr;

	if(!EFI_ERROR(BlUnicodeToUtf8(unicodeString, length, utf8NameBuffer, utf8BufferLength / sizeof(CHAR8))))
		return utf8NameBuffer;

	MmFreePool(utf8NameBuffer);
	return nullptr;
}

//
// build unicode string from utf8
//
CHAR16* BlAllocateUnicodeFromUtf8(CHAR8 CONST* utf8String, UINTN utf8Length)
{
	UINTN length															= (utf8Length == -1 ? strlen(utf8String) : utf8Length) + 1;
	CHAR16* unicodeBuffer													= static_cast<CHAR16*>(MmAllocatePool(length * sizeof(CHAR16)));
	if(!unicodeBuffer)
		return nullptr;

	if(!EFI_ERROR(BlUtf8ToUnicode(utf8String, utf8Length, unicodeBuffer, length)))
		return unicodeBuffer;

	MmFreePool(unicodeBuffer);
	return nullptr;
}

//
// allocate string
//
CHAR8* BlAllocateString(CHAR8 CONST* inputString)
{
	CHAR8* retValue															= static_cast<UINT8*>(MmAllocatePool(strlen(inputString) + 1));
	if(retValue)
		strcpy(retValue, inputString);

	return retValue;
}

//
// convert path sep
//
VOID BlConvertPathSeparator(CHAR8* pathName, CHAR8 fromChar, CHAR8 toChar)
{
	UINTN pathLength														= strlen(pathName);
	for(UINTN i = 0; i < pathLength; i ++)
	{
		if(pathName[i] == fromChar)
			pathName[i]														= toChar;
	}
}

//
// uuid to buffer
//
BOOLEAN BlUUIDStringToBuffer(CHAR8 CONST* uuidString, UINT8* uuidBuffer)
{
	//
	// 00112233-4455-6677-8899-aabbccddeeff
	//
	if(strlen(uuidString) != 36 || uuidString[8] != '-' || uuidString[13] != '-' || uuidString[18] != '-' || uuidString[23] != '-')
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
// compare time
//
INTN BlCompareTime(EFI_TIME* time1, EFI_TIME* time2)
{
	if(time1->Year < time2->Year)
		return -1;

	if(time1->Year > time2->Year)
		return 1;

	if(time1->Month < time2->Month)
		return -1;

	if(time1->Month > time2->Month)
		return 1;

	if(time1->Day < time2->Day)
		return -1;

	if(time1->Day > time2->Day)
		return 1;

	if(time1->Hour < time2->Hour)
		return -1;

	if(time1->Hour > time2->Hour)
		return 1;

	if(time1->Minute < time2->Minute)
		return -1;

	if(time1->Minute > time2->Minute)
		return 1;

	if(time1->Second < time2->Second)
		return -1;

	if(time1->Second > time2->Second)
		return 1;

	return 0;
}

//
// add one second
//
VOID BlAddOneSecond(EFI_TIME* theTime)
{
	theTime->Second															+= 1;
	if(theTime->Second > 59)
	{
		theTime->Second														= 0;
		theTime->Minute														+= 1;
		if(theTime->Minute > 59)
		{
			theTime->Minute													= 0;
			theTime->Hour													+= 1;
			if(theTime->Hour > 23)
			{
				theTime->Hour												= 0;
				theTime->Day												+= 1;

				UINT32 daysOfMonth											= ((1 << theTime->Month) & 0x15aa) ? 31 : 30;
				if(theTime->Month == 2)
					daysOfMonth												= IS_LEAP_YEAR(theTime->Year) ? 29 : 28;

				if(theTime->Day > daysOfMonth)
				{
					theTime->Day											= 1;
					theTime->Month											+= 1;
					if(theTime->Month > 12)
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
// efi time to unix time
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
	// check range
	//
	if(efiTime->Year < 1998 || efiTime->Year > 2099 || !efiTime->Month || efiTime->Month > 12 || !efiTime->Day || efiTime->Day > 31 || efiTime->Hour > 23 || efiTime->Minute > 59 || efiTime->Second > 59)
		return 0;

	//
	// check timezone
	//
	if(efiTime->TimeZone < -1440 || (efiTime->TimeZone > 1440 && efiTime->TimeZone != EFI_UNSPECIFIED_TIMEZONE))
		return 0;

	//
	// count year
	//
	UINT32 retValue															= 0;
	for(UINT16 year = 1970; year < efiTime->Year; year ++)
		retValue															+= cumulativeDays[IS_LEAP_YEAR(year)][13] * 60 * 60 * 24;

	//
	// count month, day, hour, minute, second
	//
	retValue																+= cumulativeDays[IS_LEAP_YEAR(efiTime->Year)][efiTime->Month] * 60 * 60 * 24;
	retValue																+= efiTime->Day > 0 ? (efiTime->Day - 1) * 60 * 60 * 24 : 0;
	retValue																+= efiTime->Hour * 60 * 60;
	retValue																+= efiTime->Minute * 60;
	retValue																+= efiTime->Second;

	//
	// efi time is reported in local time, adjust time zone (time zone is kept in minutes)
	//
	if(efiTime->TimeZone != EFI_UNSPECIFIED_TIMEZONE)
		retValue															+= efiTime->TimeZone * 60;

	return retValue;
}

//
// get current unix time
//
UINT32 BlGetCurrentUnixTime()
{
	EFI_TIME efiTime;
	EFI_TIME_CAPABILITIES timeCapabilities;
	if(EFI_ERROR(EfiRuntimeServices->GetTime(&efiTime, &timeCapabilities)))
		return 0;

	return BlEfiTimeToUnixTime(&efiTime);
}

//
// alder32
//
UINT32 BlAlder32(VOID CONST* inputBuffer, UINTN bufferLength)
{
	UINT32 lowHalf															= 1;
	UINT32 highHalf															= 0;
	UINT8 CONST* theBuffer													= static_cast<UINT8 CONST*>(inputBuffer);

	for(UINT32 i = 0; i < bufferLength; i ++)
	{
		if((i % 5000) == 0)
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
// uncompress
//
EFI_STATUS BlDecompress(VOID CONST* compressedBuffer, UINTN compressedSize, VOID* uncompressedBuffer, UINTN uncompressedBufferSize, UINTN* uncompressedSize)
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

	for(i = 0; i < N - F; i ++)
		textBuffer[i]														= ' ';

	while(TRUE)
	{
		if(((flags >>= 1) & 0x100) == 0)
		{
			if(srcBuffer < srcend)
				c															= *srcBuffer ++;
			else
				break;

			flags															= c | 0xFF00;
		}

		if(flags & 1)
		{
			if(srcBuffer < srcend)
				c															= *srcBuffer ++;
			else
				break;

			*dstBuffer ++													= c;
			textBuffer[r++]													= c;
			r																&= (N - 1);
		}
		else
		{
			if(srcBuffer < srcend)
				i															= *srcBuffer ++;
			else
				break;

			if(srcBuffer < srcend)
				j															= *srcBuffer ++;
			else
				break;
			i																|= ((j & 0xf0) << 4);
			j																=  (j & 0x0f) + THRESHOLD;

			for(INT32 k = 0; k <= j; k ++)
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