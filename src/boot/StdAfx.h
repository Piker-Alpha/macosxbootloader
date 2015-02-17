//********************************************************************
//	created:	4:11:2009   10:03
//	filename: 	StdAfx.h
//	author:		tiamo
//	purpose:	stdafx
//********************************************************************

#pragma once

#ifdef __APPLE__
  #define __leave
  #define __try if (1)
  #define __except(x) if (0 && (x))
  #define __finally if (1)
  #define _alloca(size)    MmAllocatePool(size)

  #define CHAR16_STRING(S)													static_cast<CHAR16*>((VOID*)(S))
  #define CHAR8_STRING(S)													static_cast<CHAR8*>((VOID*)(S))

  #ifndef _INT8_T
    //
    // Let _int8_t.h know that we don't want it to define int8_t for us.
    //
    #define _INT8_T  1
  #endif
#else
  #define CHAR16_STRING(S)													static_cast<CHAR16*>(static_cast<VOID*>(S))
  #define CHAR8_STRING(S)													static_cast<CHAR8*>(static_cast<VOID*>(S))
#endif

#define NOTHING
#define BOOTAPI																__cdecl
#define CHAR8_CONST_STRING(S)												static_cast<CHAR8 CONST*>(static_cast<VOID CONST*>(S))
#define CHAR16_CONST_STRING(S)												static_cast<CHAR16 CONST*>(static_cast<VOID CONST*>(S))


#define try_leave(S)														do{S;__leave;}while(0)
#define ARRAYSIZE(A)														(sizeof((A)) / sizeof((A)[0]))
#define ArchConvertAddressToPointer(P,T)									((T)((UINTN)(P)))
#define ArchConvertPointerToAddress(A)										((UINTN)(A))
#define ArchNeedEFI64Mode()													(MmGetKernelVirtualStart() > static_cast<UINT32>(-1) || sizeof(UINTN) == sizeof(UINT64))
#define LdrStaticVirtualToPhysical(V)										((V) & (1 * 1024 * 1024 * 1024 - 1))
#define Add2Ptr(P, O, T)													ArchConvertAddressToPointer(ArchConvertPointerToAddress(P) + (O), T)
#define PAGE_ALIGN(A)														((A) & ~EFI_PAGE_MASK)
#define BYTE_OFFSET(A)														((UINTN)(A) & EFI_PAGE_MASK)

#define SWAP32(V)															((((UINT32)(V) & 0xff) << 24) | (((UINT32)(V) & 0xff00) << 8) | (((UINT32)(V) & 0xff0000) >> 8) |  (((UINT32)(V) & 0xff000000) >> 24))
#define SWAP_BE32_TO_HOST													SWAP32

//
// Use 1 for grey Apple logo/panic dialog.
//
#define LEGACY_GREY_SUPPORT													0

#include "EfiCommon.h"
#include "EfiApi.h"
#include "EfiImage.h"
#include "EfiDevicePath.h"
#include "IndustryStandard/Acpi.h"
#include "IndustryStandard/pci.h"
#include "IndustryStandard/SmBios.h"

#include "GuidDefine.h"
#include "RuntimeLib.h"
#include "ArchUtils.h"
#include "Memory.h"
#include "MiscUtils.h"
#include "AcpiUtils.h"
#include "PeImage.h"
#include "BootDebugger.h"
#include "Base64.h"
#include "Crc32.h"
#include "MD5.h"
#include "SHA256.h"
#include "DeviceTree.h"
#include "DevicePath.h"
#include "Config.h"
#include "FileIo.h"
#include "MachO.h"
#include "PlatformExpert.h"
#include "NetBoot.h"
#include "Hibernate.h"
#include "Console.h"
#include "Options.h"
#include "LoadKernel.h"
#include "LoadDrivers.h"
#include "BootArgs.h"
#include "MemoryMap.h"
#include "PanicDialog.h"
#include "FileVault.h"