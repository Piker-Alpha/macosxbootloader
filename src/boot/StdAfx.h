//********************************************************************
//	created:	4:11:2009   10:03
//	filename: 	StdAfx.h
//	author:		tiamo
//	purpose:	stdafx
//********************************************************************

#pragma once

#define DEBUG_LDRP_CALL_CSPRINTF											0
#define DEBUG_NVRAM_CALL_CSPRINTF											0
#define DEBUG_KERNEL_PATCHER												1

#define OS_LEGACY															6
#define YOSEMITE															10
#define EL_CAPITAN															11

#define TARGET_OS															EL_CAPITAN

#if (TARGET_OS >= YOSEMITE)
	#define LEGACY_GREY_SUPPORT												1
#else
	#define LEGACY_GREY_SUPPORT												0
#endif

#define LOAD_EXECUTABLE_TARGET_UINT64										0x487074db8548c389ULL
#define LOAD_EXECUTABLE_PATCH_UINT64										0x4812ebdb8548c389ULL

#define READ_STARTUP_EXTENSIONS_TARGET_UINT64								0xe805eb00000025ebULL
#define READ_STARTUP_EXTENSIONS_PATCH_UINT64								0xe8909000000025ebULL

#define DO_REPLACE_BOARD_ID													1

#if DO_REPLACE_BOARD_ID
	#define MACPRO_31														"Mac-F42C88C8"
	#define MACBOOKPRO_31													"Mac-F4238BC8"

	#define DEBUG_BOARD_ID_CSPRINTF											0
#endif



#define NOTHING
#define BOOTAPI																__cdecl
#define CHAR8_CONST_STRING(S)												static_cast<CHAR8 CONST*>(static_cast<VOID CONST*>(S))
#define CHAR16_CONST_STRING(S)												static_cast<CHAR16 CONST*>(static_cast<VOID CONST*>(S))
#define CHAR8_STRING(S)														static_cast<CHAR8*>(static_cast<VOID*>(S))
#define CHAR16_STRING(S)													static_cast<CHAR16*>(static_cast<VOID*>(S))
#define try_leave(S)														do{S;__leave;}while(0)
#define ARRAYSIZE(A)														(sizeof((A)) / sizeof((A)[0]))
#define ArchConvertAddressToPointer(P,T)									((T)((UINTN)(P)))
#define ArchConvertPointerToAddress(A)										((UINTN)(A))
#define ArchNeedEFI64Mode()													(MmGetKernelVirtualStart() > static_cast<UINT32>(-1) || sizeof(UINTN) == sizeof(UINT64))
#define LdrStaticVirtualToPhysical(V)										((V) & (1 * 1024 * 1024 * 1024 - 1))
#define Add2Ptr(P, O, T)													ArchConvertAddressToPointer(ArchConvertPointerToAddress(P) + (O), T)
#define PAGE_ALIGN(A)														((A) & ~EFI_PAGE_MASK)
#define BYTE_OFFSET(A)														((UINT32)(A) & EFI_PAGE_MASK)

#define SWAP32(V)															((((UINT32)(V) & 0xff) << 24) | (((UINT32)(V) & 0xff00) << 8) | (((UINT32)(V) & 0xff0000) >> 8) |  (((UINT32)(V) & 0xff000000) >> 24))
#define SWAP_BE32_TO_HOST													SWAP32

#if (TARGET_OS == EL_CAPITAN)
#ifndef kBootArgsFlagCSRActiveConfig
	#define kBootArgsFlagCSRActiveConfig	(1 << 3)	// 8
#endif

#ifndef kBootArgsFlagCSRConfigMode
	#define kBootArgsFlagCSRConfigMode		(1 << 4)	// 16
#endif

#ifndef kBootArgsFlagCSRBoot
	#define kBootArgsFlagCSRBoot			(1 << 5)	// 32
#endif

#ifndef CSR_VALID_FLAGS
	/* Rootless configuration flags */
	#define CSR_ALLOW_UNTRUSTED_KEXTS		(1 << 0)	// 1
	#define CSR_ALLOW_UNRESTRICTED_FS		(1 << 1)	// 2
	#define CSR_ALLOW_TASK_FOR_PID			(1 << 2)	// 4
	#define CSR_ALLOW_KERNEL_DEBUGGER		(1 << 3)	// 8
	#define CSR_ALLOW_APPLE_INTERNAL		(1 << 4)	// 16
	#define CSR_ALLOW_UNRESTRICTED_DTRACE	(1 << 5)	// 32
	#define CSR_ALLOW_UNRESTRICTED_NVRAM	(1 << 6)	// 64
	#define CSR_ALLOW_DEVICE_CONFIGURATION	(1 << 7)	// 128

	#define CSR_VALID_FLAGS (CSR_ALLOW_UNTRUSTED_KEXTS | \
			CSR_ALLOW_UNRESTRICTED_FS | \
			CSR_ALLOW_TASK_FOR_PID | \
			CSR_ALLOW_KERNEL_DEBUGGER | \
			CSR_ALLOW_APPLE_INTERNAL | \
			CSR_ALLOW_UNRESTRICTED_DTRACE | \
			CSR_ALLOW_UNRESTRICTED_NVRAM | \
			CSR_ALLOW_DEVICE_CONFIGURATION)
#endif
#endif // #if (TARGET_OS == YOSMITE)

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