//********************************************************************
//	created:	21:8:2012   22:43
//	filename: 	AppleNetBoot.h
//	author:		tiamo
//	purpose:	apple net boot
//********************************************************************

#ifndef _APPLE_NET_BOOT_H_
#define _APPLE_NET_BOOT_H_

#define APPLE_NET_BOOT_PROTOCOL_GUID										{0x78ee99fb, 0x6a5e, 0x4186, 0x97, 0xde, 0xcd, 0x0a, 0xba, 0x34, 0x5a, 0x74}

EFI_FORWARD_DECLARATION(APPLE_NET_BOOT_PROTOCOL);

typedef EFI_STATUS (EFIAPI* GET_DHCP_RESPONSE)(APPLE_NET_BOOT_PROTOCOL* This, UINTN* BufferSize, VOID* DataBuffer);
typedef EFI_STATUS (EFIAPI* GET_BSDP_RESPONSE)(APPLE_NET_BOOT_PROTOCOL* This, UINTN* BufferSize, VOID* DataBuffer);

typedef struct _APPLE_NET_BOOT_PROTOCOL
{
	GET_DHCP_RESPONSE														GetDhcpResponse;
	GET_BSDP_RESPONSE														GetBsdpResponse;
}APPLE_NET_BOOT_PROTOCOL;

extern EFI_GUID gAppleNetBootProtocolGuid;

#endif
