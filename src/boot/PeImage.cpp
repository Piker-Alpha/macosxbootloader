//********************************************************************
//	created:	6:11:2009   23:35
//	filename: 	PeImage.cpp
//	author:		tiamo
//	purpose:	pe image
//********************************************************************

#include "stdafx.h"

//
// get image nt header
//
EFI_IMAGE_NT_HEADERS* PeImageNtHeader(VOID* imageBase)
{
	EFI_IMAGE_DOS_HEADER* dosHeader											= static_cast<EFI_IMAGE_DOS_HEADER*>(imageBase);
	if(!dosHeader || dosHeader->e_magic != EFI_IMAGE_DOS_SIGNATURE)
		return nullptr;

	EFI_IMAGE_NT_HEADERS* ntHeader											= Add2Ptr(imageBase, dosHeader->e_lfanew, EFI_IMAGE_NT_HEADERS*);
	return ntHeader->Signature == EFI_IMAGE_NT_SIGNATURE ? ntHeader : nullptr;
}

//
// get image size
//
UINT32 PeImageGetSize(VOID* ntHeaders)
{
	EFI_IMAGE_NT_HEADERS32* ntHeaders32										= static_cast<EFI_IMAGE_NT_HEADERS32*>(ntHeaders);
	EFI_IMAGE_NT_HEADERS64* ntHeaders64										= static_cast<EFI_IMAGE_NT_HEADERS64*>(ntHeaders);
	return ntHeaders32->OptionalHeader.Magic == EFI_IMAGE_NT_OPTIONAL_HDR64_MAGIC ? ntHeaders64->OptionalHeader.SizeOfImage : ntHeaders32->OptionalHeader.SizeOfImage;
}

//
// get checksum
//
UINT32 PeImageGetChecksum(VOID* ntHeaders)
{
	EFI_IMAGE_NT_HEADERS32* ntHeaders32										= static_cast<EFI_IMAGE_NT_HEADERS32*>(ntHeaders);
	EFI_IMAGE_NT_HEADERS64* ntHeaders64										= static_cast<EFI_IMAGE_NT_HEADERS64*>(ntHeaders);
	return ntHeaders32->OptionalHeader.Magic == EFI_IMAGE_NT_OPTIONAL_HDR64_MAGIC ? ntHeaders64->OptionalHeader.CheckSum : ntHeaders32->OptionalHeader.CheckSum;
}

//
// get entry point
//
VOID* PeImageGetEntryPoint(VOID* imageBase)
{
	EFI_IMAGE_NT_HEADERS* ntHeaders											= PeImageNtHeader(imageBase);
	if(!ntHeaders)
		return nullptr;

	EFI_IMAGE_NT_HEADERS32* ntHeaders32										= static_cast<EFI_IMAGE_NT_HEADERS32*>(static_cast<VOID*>(ntHeaders));
	EFI_IMAGE_NT_HEADERS64* ntHeaders64										= static_cast<EFI_IMAGE_NT_HEADERS64*>(static_cast<VOID*>(ntHeaders));
	if(ntHeaders32->OptionalHeader.Magic == EFI_IMAGE_NT_OPTIONAL_HDR64_MAGIC)
		return Add2Ptr(imageBase, ntHeaders64->OptionalHeader.AddressOfEntryPoint, VOID*);
	return Add2Ptr(imageBase, ntHeaders32->OptionalHeader.AddressOfEntryPoint, VOID*);
}