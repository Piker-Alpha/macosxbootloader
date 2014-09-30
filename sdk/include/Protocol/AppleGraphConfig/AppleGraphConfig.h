//********************************************************************
//	created:	28:8:2012   18:15
//	filename: 	AppleGraphConfig.h
//	author:		tiamo
//	purpose:	graph config
//********************************************************************

#ifndef _APPLE_GRAPH_CONFIG_H_
#define _APPLE_GRAPH_CONFIG_H_

#define APPLE_GRAPH_CONFIG_PROTOCOL_GUID									{0x8ece08d8, 0xa6d4, 0x430b, 0xa7, 0xb0, 0x2d, 0xf3, 0x18, 0xe7, 0x88, 0x4a}

EFI_FORWARD_DECLARATION(APPLE_GRAPH_CONFIG_PROTOCOL);

typedef EFI_STATUS (EFIAPI* RESTORE_CONFIG)(APPLE_GRAPH_CONFIG_PROTOCOL* This, UINT32 Param1, UINT32 Param2, VOID* Param3, VOID* Param4, VOID* Param5);

typedef struct _APPLE_GRAPH_CONFIG_PROTOCOL
{
	UINTN																	Unknown0;
	RESTORE_CONFIG															RestoreConfig;
}APPLE_GRAPH_CONFIG_PROTOCOL;

extern EFI_GUID gAppleGraphConfigProtocolGuid;

#endif
