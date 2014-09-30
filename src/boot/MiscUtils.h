//********************************************************************
//	created:	7:11:2009   1:36
//	filename: 	MiscUtils.h
//	author:		tiamo
//	purpose:	utils
//********************************************************************

#pragma once

//
// get memory capacity
//
UINT64 BlGetMemoryCapacity();

//
// get memory size
//
UINT64 BlGetMemorySize();

//
// get board id
//
CHAR8* BlGetBoardId();

//
// connect drivers
//
EFI_STATUS BlConnectAllController();

//
// get base and size
//
EFI_STATUS BlGetApplicationBaseAndSize(UINT64* imageBase, UINT64* imageSize);

//
// parse device location
//
VOID BlParseDeviceLocation(CHAR8 CONST* loaderOptions, UINTN* segment, UINTN* bus, UINTN* device, UINTN* func);

//
// pci get bar attribute
//
EFI_STATUS BlGetPciBarAttribute(EFI_PCI_IO_PROTOCOL* pciIoProtocol, UINT8 barIndex, UINT64* baseAddress, UINT64* barLength, BOOLEAN* isMemorySpace);

//
// find pci device
//
EFI_STATUS BlFindPciDevice(UINTN segment, UINTN bus, UINTN device, UINTN func, UINT8 baseClass, UINT8 subClass, UINT8 progIf, EFI_PCI_IO_PROTOCOL** outPciIoProtocol, EFI_HANDLE* outHandle);

//
// start pci device
//
EFI_STATUS BlStartPciDevice(EFI_PCI_IO_PROTOCOL* pciIoProtocol, BOOLEAN decodeIo, BOOLEAN decodeMemory, BOOLEAN busMaster);

//
// detect memory size
//
EFI_STATUS BlDetectMemorySize();

//
// unicode to utf8
//
EFI_STATUS BlUnicodeToUtf8(CHAR16 CONST* unicodeBuffer, UINTN unicodeCharCount, CHAR8* utf8Buffer, UINTN utf8BufferLength);

//
// unicode to ansi
//
EFI_STATUS BlUnicodeToAnsi(CHAR16 CONST* unicodeBuffer, UINTN unicodeCharCount, CHAR8* ansiBuffer, UINTN ansiBufferLength);

//
// utf8 to unicode
//
EFI_STATUS BlUtf8ToUnicode(CHAR8 CONST* utf8Buffer, UINTN bytesCount, CHAR16* unicodeBuffer, UINTN unicodeBufferLengthInChar);

//
// build utf8 string from unicode
//
CHAR8* BlAllocateUtf8FromUnicode(CHAR16 CONST* unicodeString, UINTN unicodeCharCount);

//
// build unicode string from utf8
//
CHAR16* BlAllocateUnicodeFromUtf8(CHAR8 CONST* utf8String, UINTN utf8Length);

//
// allocate string
//
CHAR8* BlAllocateString(CHAR8 CONST* inputString);

//
// convert path sep
//
VOID BlConvertPathSeparator(CHAR8* pathName, CHAR8 fromChar, CHAR8 toChar);

//
// uuid to buffer
//
BOOLEAN BlUUIDStringToBuffer(CHAR8 CONST* uuidString, UINT8* uuidBuffer);

//
// compare time
//
INTN BlCompareTime(EFI_TIME* time1, EFI_TIME* time2);

//
// add one second
//
VOID BlAddOneSecond(EFI_TIME* theTime);

//
// efi time to unix time
//
UINT32 BlEfiTimeToUnixTime(EFI_TIME CONST* efiTime);

//
// get current unix time
//
UINT32 BlGetCurrentUnixTime();

//
// alder32
//
UINT32 BlAlder32(VOID CONST* inputBuffer, UINTN bufferLength);

//
// uncompress
//
EFI_STATUS BlDecompress(VOID CONST* compressedBuffer, UINTN compressedSize, VOID* uncompressedBuffer, UINTN uncompressedBufferSize, UINTN* uncompressedSize);