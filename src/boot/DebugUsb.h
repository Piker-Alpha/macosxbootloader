//********************************************************************
//	created:	5:8:2012   15:03
//	filename: 	DebugUsb.h
//	author:		tiamo
//	purpose:	debug over usb
//********************************************************************

#pragma once

#include "BootDebuggerPrivate.h"

//
// setup debug device
//
EFI_STATUS BdUsbConfigureDebuggerDevice(CHAR8 CONST* loaderOptions);

//
// send packet
//
VOID BdUsbSendPacket(UINT32 packetType, STRING* messageHeader, STRING* messageData);

//
// receive packet
//
UINT32 BdUsbReceivePacket(UINT32 packetType, STRING* messageHeader, STRING* messageData, UINT32* dataLength);

//
// close debug device
//
EFI_STATUS BdUsbCloseDebuggerDevice();
