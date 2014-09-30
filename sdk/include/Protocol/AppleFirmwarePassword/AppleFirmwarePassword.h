//********************************************************************
//	created:	26:8:2012   17:08
//	filename: 	AppleFirmwarePassword.h
//	author:		tiamo
//	purpose:	apple firmware password
//********************************************************************

#ifndef _APPLE_FIRMWARE_PASSWORD_H_
#define _APPLE_FIRMWARE_PASSWORD_H_

#define APPLE_FIRMWARE_PASSWORD_PROTOCOL_GUID								{0x8ffeeb3a, 0x4c98, 0x4630, 0x80, 0x3f, 0x74, 0x0f, 0x95, 0x67, 0x09, 0x1d}

EFI_FORWARD_DECLARATION(APPLE_FIRMWARE_PASSWORD_PROTOCOL);

typedef EFI_STATUS (EFIAPI* CHECK)(IN APPLE_FIRMWARE_PASSWORD_PROTOCOL* This, OUT UINTN* CheckValue);

typedef struct _APPLE_FIRMWARE_PASSWORD_PROTOCOL
{
	UINT64																	Signature;
	UINTN																	Unknown[3];
	CHECK																	Check;
}APPLE_FIRMWARE_PASSWORD_PROTOCOL;

extern EFI_GUID gAppleFirmwarePasswordProtocolGuid;

#endif
