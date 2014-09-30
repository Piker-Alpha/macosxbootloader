//********************************************************************
//	created:	21:8:2012   22:43
//	filename: 	AppleDeviceProperty.h
//	author:		tiamo
//	purpose:	apple device property
//********************************************************************

#ifndef _APPLE_DEVICE_PROPERTY_H_
#define _APPLE_DEVICE_PROPERTY_H_

#define APPLE_DEVICE_PROPERTY_PROTOCOL_GUID									{0x91bd12fe, 0xf6c3, 0x44fb, 0xa5, 0xb7, 0x51, 0x22, 0xab, 0x30, 0x3a, 0xe0}

EFI_FORWARD_DECLARATION(APPLE_DEVICE_PROPERTY_PROTOCOL);

typedef EFI_STATUS (EFIAPI* GET_DEVICE_PROPERTY)(IN APPLE_DEVICE_PROPERTY_PROTOCOL* This, OUT VOID* PropertiesBuffer, IN OUT UINTN *BufferSize);

typedef struct _APPLE_DEVICE_PROPERTY_PROTOCOL
{
	UINT64																	Signature;
	UINTN																	Unknown[3];
	GET_DEVICE_PROPERTY														GetDeviceProperty;
}APPLE_DEVICE_PROPERTY_PROTOCOL;

extern EFI_GUID gAppleDevicePropertyProtocolGuid;

#endif
