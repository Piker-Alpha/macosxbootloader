//********************************************************************
//	created:	8:11:2009   18:33
//	filename: 	Options.h
//	author:		tiamo
//	purpose:	option
//********************************************************************

#pragma once

#define BOOT_MODE_NORMAL													0x000000
#define BOOT_MODE_SAFE														0x000001
#define BOOT_MODE_VERBOSE													0x000002
#define BOOT_MODE_NET														0x000004
#define BOOT_MODE_ALT_KERNEL												0x000008
#define BOOT_MODE_SINGLE_USER												0x000010
#define BOOT_MODE_GRAPH														0x000020
#define BOOT_MODE_FIRMWARE_PASSWORD											0x000040
#define BOOT_MODE_DEBUG														0x000080

#define BOOT_MODE_BOOT_IS_NOT_ROOT											0x000100
#define BOOT_MODE_HAS_FILE_VAULT2_CONFIG									0x000200
#define BOOT_MODE_X															0x000400
#define BOOT_MODE_EFI_NVRAM_RECOVERY_BOOT_MODE								0x000800
#define BOOT_MODE_FROM_RECOVER_BOOT_DIRECTORY								0x001000
#define BOOT_MODE_HIBER_FROM_FV												0x002000
#define BOOT_MODE_ASLR														0x004000
#define BOOT_MODE_SKIP_BOARD_ID_CHECK										0x008000
#define BOOT_MODE_SKIP_PANIC_DIALOG											0x010000

//
// detect hot key
//
EFI_STATUS BlDetectHotKey();

//
// process option
//
EFI_STATUS BlProcessOptions(CHAR8 CONST* bootCommandLine, CHAR8** kernelCommandLine, EFI_DEVICE_PATH_PROTOCOL* bootDevicePath, EFI_DEVICE_PATH_PROTOCOL* bootFilePath);

//
// test boot mode
//
UINT32 BlTestBootMode(UINT32 bootMode);

//
// set boot mode
//
VOID BlSetBootMode(UINT32 setValue, UINT32 clearValue);

//
// setup kernel command line
//
CHAR8* BlSetupKernelCommandLine(CHAR8 CONST* bootOptions, CHAR8 CONST* bootArgsVariable, CHAR8 CONST* kernelFlags);