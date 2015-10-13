//********************************************************************
//	created:	7:11:2009   2:01
//	filename: 	Debug1394.h
//	author:		tiamo
//	purpose:	debug over 1394
//********************************************************************

#pragma once

#include "BootDebuggerPrivate.h"

//
// setup debug device
//
EFI_STATUS Bd1394ConfigureDebuggerDevice(CHAR8 CONST* loaderOptions);

//
// send packet
//
VOID Bd1394SendPacket(UINT32 packetType, STRING* messageHeader, STRING* messageData);

//
// receive packet
//
UINT32 Bd1394ReceivePacket(UINT32 packetType, STRING* messageHeader, STRING* messageData, UINT32* dataLength);

//
// close debug device
//
EFI_STATUS Bd1394CloseDebuggerDevice();