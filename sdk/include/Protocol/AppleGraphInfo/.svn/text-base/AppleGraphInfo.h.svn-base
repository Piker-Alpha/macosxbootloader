//********************************************************************
//	created:	28:8:2012   18:40
//	filename: 	AppleGraphInfo.h
//	author:		tiamo
//	purpose:	graph info
//********************************************************************

#ifndef _APPLE_GRAPH_INFO_H_
#define _APPLE_GRAPH_INFO_H_

#define APPLE_GRAPH_INFO_PROTOCOL_GUID										{0xe316e100, 0x0751, 0x4c49, 0x90, 0x56, 0x48, 0x6c, 0x7e, 0x47, 0x29, 0x03}

EFI_FORWARD_DECLARATION(APPLE_GRAPH_INFO_PROTOCOL);

typedef EFI_STATUS (EFIAPI* GET_INFO)(APPLE_GRAPH_INFO_PROTOCOL* This, UINT64* BaseAddress, UINT64* FrameBufferSize, UINT32* ByterPerRow, UINT32* Width, UINT32* Height, UINT32* ColorDepth);

typedef struct _APPLE_GRAPH_INFO_PROTOCOL
{
	GET_INFO																GetInfo;
}APPLE_GRAPH_INFO_PROTOCOL;

extern EFI_GUID gAppleGraphInfoProtocolGuid;

#endif
