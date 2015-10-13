//********************************************************************
//	created:	4:11:2009   10:41
//	filename: 	Console.h
//	author:		tiamo
//	purpose:	console
//********************************************************************

#pragma once

//
// initialize
//
EFI_STATUS CsInitialize();

//
// connect device
//
EFI_STATUS CsConnectDevice(BOOLEAN connectAll, BOOLEAN connectDisplay);

//
// set text mode
//
EFI_STATUS CsSetConsoleMode(BOOLEAN textMode, BOOLEAN force);

//
// initialize graph mode
//
EFI_STATUS CsInitializeGraphMode();

//
// draw boot image
//
EFI_STATUS CsDrawBootImage(BOOLEAN normalLogo);

//
// draw panic image
//
EFI_STATUS CsDrawPanicImage();

//
// initialize boot video
//
EFI_STATUS CsInitializeBootVideo(struct _BOOT_VIDEO* bootVideo);

//
// setup device tree
//
EFI_STATUS CsSetupDeviceTree(struct _BOOT_ARGS* bootArgs);

//
// clear screen
//
VOID CsClearScreen();

//
// console finalize
//
EFI_STATUS CsFinalize();

//
// print string
//
VOID CsPrintf(CHAR8 CONST* printForamt, ...);

//
// draw preview
//
VOID CsDrawPreview(HIBERNATE_PREVIEW* previewBuffer, UINT32 imageIndex, UINT8 progressSaveUnder[HIBERNATE_PROGRESS_COUNT][HIBERNATE_PROGRESS_SAVE_UNDER_SIZE], BOOLEAN colorMode, BOOLEAN fromFV2, INT32* gfxRestoreStatus);

//
// update progress
//
VOID CsUpdateProgress(UINT8 progressSaveUnder[HIBERNATE_PROGRESS_COUNT][HIBERNATE_PROGRESS_SAVE_UNDER_SIZE], UINTN prevBlob, UINTN currentBlob);