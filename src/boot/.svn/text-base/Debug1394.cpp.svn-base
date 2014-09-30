//********************************************************************
//	created:	7:11:2009   2:02
//	filename: 	Debug1394.cpp
//	author:		tiamo
//	purpose:	debug over 1394
//********************************************************************

#include "stdafx.h"
#include "Debug1394.h"

//
// how does it work?
//	1.ohci controller provides us a function to read write peer's physical memory directly.
//	2.debuggee(us) setups the ohci controller to map his physical memory,and let anyone to be able to read it.
//	3.debuggee also setups a config rom to let the debugger to find detail info used to communicate with it.
//	4.debuggee uses a well-known (bus,node) number.
//	5.debugger(peer, running windbg) uses the well-known (bus,node) number as peer's address and read its config rom.
//	6.debugger decodes the config rom,and check whether it contains a debug info or not.
//	7.if the debugger found a debug info in the config rom,then it assumes that the peer can be debugged
//	8.debuggee uses two memory buffers to communicate with debugger,one for send,the other for receive
//	9.for each memory buffer there is an indicator used to present current buffer state,pending or not
//	10.debuggee also provides those two memory buffers' physical address in the config rom,then the debugger can read/write those buffers too.
//	11.if debuggee wants to send some data to debugger
//		a.the debuggee copies data to the send buffer,and set the send buffer's state to PENDING,means there is data to be read by debugger
//		b.debuggee loops and checks the send buffer's state is PENDING or not,if the state remains PENDING,the debugee will contine looping
//		c.debugger reads debuggee's send buffer's state,if the state is NOT_PENDING,which means there is no data pending,debugger will
//			wait a small time,then reads the state again,if the state is PENDING,it will set the state to NOT_PENDING,then return,otherwise continue waiting
//		d.debuggee will see the state becomes to NOT_PENDING,means that the debugger has already picked the data,send complete.
//	12.if the debugger wants to send some data to debugger
//		a.debugger first writes those data to debuggee's physical memory
//		b.debugger writes the debugee's receive buffer's state to PENDING
//		c.debugger reads the debuggee's receive buffer's state back,if it remains PENDING,it will continue reading the state and wait it to become NOT_PENDING
//		d.debuggee checks his receive buffer's state,if it is PENDING,which means that there is some data sent from the debugger,
//			it will read it,and set state to NOT_PENDING,otherwise return a timeout to his caller,
//			NOTE the debuggee will NOT loop here,it is the caller's responsibility to do the loop check work.
//
// you can see
//	debuggee simply exports his physical memory to ohci controller,and let the others to be able to read it.
//	debuggee will NOT send a 1394 request packet to ohci controller,it just read and write some physical memory directly.
//	after it sends the physical memory mapping info to the ohci controller,debuggee will NOT touch the hardware anymore.(but it will make sure the hardware is enbled)
//	debuggee and debuger use STATE to communicate to each other
//	there is NO such method to be used by data sender to notify the receiver "hey guy,there is something,read it!",it just assumes those data should be read in the later
//	but it can not be told when it will be read,so sender must do a loop to check whether the data has been read or not
//	yes,it is POLL mode,not INTERRUPT mode
//

#include <pshpack1.h>

//
// send buffer
//
typedef union _DBG1394_SEND_BUFFER
{
	struct _SEND_BUFFER
	{
		//
		// state
		//
		NTSTATUS															Status;

		//
		// packet header
		//
		KD_PACKET															PacketHeader;

		//
		// data length
		//
		UINT32																DataLength;

		//
		// data buffer
		//
		UINT8																DataBuffer[PACKET_MAX_SIZE];
	}SendBuffer;

	//
	// padding to page size
	//
	UINT8																	Padding[EFI_PAGE_SIZE];
}DBG1394_SEND_BUFFER;

//
// receive buffer
//
typedef union _DBG1394_RECEIVE_BUFFER
{
	struct _RECEIVE_BUFFER
	{
		//
		// state
		//
		NTSTATUS															Status;

		//
		// length
		//
		UINT32																TotalLength;

		//
		// packet header
		//
		KD_PACKET															PacketHeader;

		//
		// buffer
		//
		UINT8																DataBuffer[PACKET_MAX_SIZE - sizeof(KD_PACKET)];
	}ReceiveBuffer;

	//
	// padding to page size
	//
	UINT8																	Padding[EFI_PAGE_SIZE];
}DBG1394_RECEIVE_BUFFER;

//
// config info,shared between debuggee and debugger
//
typedef struct _DBG1394_DEBUG_CONFIG_INFO
{
	//
	// signature
	//
	UINT32																	Signature;

	//
	// major version
	//
	UINT16																	MajorVersion;

	//
	// minor version
	//
	UINT16																	MinorVersion;

	//
	// channel
	//
	UINT32																	Channel;

	//
	// bus present
	//
	UINT32																	BusPresent;

	//
	// send buffer physical address
	//
	UINT64																	SendBuffer;

	//
	// receive buffer physical address
	//
	UINT64																	ReceiveBuffer;
}DBG1394_DEBUG_CONFIG_INFO;

//
// config rom header
//
typedef union _IEEE1394_CONFIG_ROM_INFO
{
	struct _CRC_INFO
	{
		union
		{
			//
			// cri crc value
			//
			UINT16															CriCrcValue;

			struct _SAVED_INFO
			{
				//
				// saved info length
				//
				UINT8														CriSavedInfoLength;

				//
				// saved crc length
				//
				UINT8														CriSavedCrcLength;
			}SavedInfo;
		};

		//
		// crc length
		//
		UINT8																CriCrcLength;

		//
		// info length
		//
		UINT8																CriInfoLength;
	}CrcInfo;

	//
	// whole UINT32
	//
	UINT32																	AsULong;
}IEEE1394_CONFIG_ROM_INFO;

//
// config rom
//
typedef struct _DBG1394_DEBUG_CONFIG_ROM
{
	//
	// rom header
	//
	IEEE1394_CONFIG_ROM_INFO												Header;

	//
	// signature,'1394'
	//
	UINT32																	Singnature;

	//
	// bus option
	//
	UINT32																	BusOptions;

	//
	// guid
	//
	UINT32																	GlobalUniqueId[2];

	//
	// directory info head
	//
	IEEE1394_CONFIG_ROM_INFO												DirectoryInfoHead;

	//
	// node capabilities
	//
	UINT32																	NodeCapabilities;

	//
	// module vendor id
	//
	UINT32																	ModuleVendorId;

	//
	// extended key
	//
	UINT32																	ExtendedKey;

	//
	// debug key
	//
	UINT32																	DebugKey;

	//
	// debug value,debug config info's physical address
	//
	UINT32																	DebugConfigInfo;

	//
	// padding
	//
	UINT8																	Padding[212];
}DBG1394_DEBUG_CONFIG_ROM;

//
// debugger data,must be page algined
//
typedef struct _DBG1394_GLOBAL_DATA
{
	//
	// send buffer
	//
	DBG1394_SEND_BUFFER														SendBuffer;

	//
	// receive buffer
	//
	DBG1394_RECEIVE_BUFFER													ReceiveBuffer;

	//
	// config rom
	//
	DBG1394_DEBUG_CONFIG_ROM												DebugConfigRom;

	//
	// debug info
	//
	DBG1394_DEBUG_CONFIG_INFO												DebugConfigInfo;
}DBG1394_GLOBAL_DATA;

#include <poppack.h>

//
// ohci controller base address
//
STATIC VOID* Bd1394pControllerRegisterBase									= nullptr;

//
// global data
//
STATIC DBG1394_GLOBAL_DATA* Bd1394pGlobalData								= nullptr;

//
// physical page
//
STATIC UINT64 Bd1394pDataPhysicalAddress									= 0;

//
// stall
//
#pragma optimize("", off)
STATIC VOID Bd1394pStallExecution(UINT32 microseconds)
{
	for(UINT32 k = 0,b = 1; k < microseconds; k ++)
	{
		for(UINT32 i = 1; i < 100000; i++)
			b																= b * (i >> k);
	}
}
#pragma optimize("", on)

//
// crc16
//
STATIC UINT32 Bd1394pCrc16(UINT32 inputData, UINT32 inputCheck)
{
	UINT32 nextValue														= inputCheck;
	for(INT32 shiftCount = 28; shiftCount >= 0; shiftCount -= 4)
	{
		UINT32 sum															= ((nextValue >> 12) ^ (inputData >> shiftCount)) & 0xf;
		nextValue															= (nextValue << 4) ^ (sum << 12) ^ (sum << 5) ^ (sum);
	}

	return nextValue & 0xFFFF;
}

//
// cacl crc
//
STATIC UINT16 Bd1394pCalculateCrc(VOID* inputBuffer, UINT32 bytesCount)
{
	UINT32 temp																= 0;
	UINT32* quadlet															= static_cast<UINT32*>(inputBuffer);

	for(UINT32 index = 0; index < bytesCount; index++)
		temp																= Bd1394pCrc16(quadlet[index], temp);

	return static_cast<UINT16>(temp);
}

//
// get physical address
//
STATIC UINT64 Bd1394pGetPhysicalAddress(VOID* virtualAddress)
{
	UINT64 physicalAddress													= 0;
	if(MmTranslateVirtualAddress(virtualAddress, &physicalAddress))
		return physicalAddress;

	return 0;
}

//
// read phy register
//
STATIC BOOLEAN Bd1394pReadPhyRegister(VOID* baseRegister, UINT8 index, UINT8* value)
{
	//
	// write phy register index to phy control
	//
	UINT32 phyControl														= ((index & 0xf) | 0x80) << 8;
	ARCH_WRITE_REGISTER_UINT32(Add2Ptr(baseRegister, 0xec, UINT32*), phyControl);

	for(UINT32 i = 0; i < 400000; i ++)
	{
		//
		// check read done
		//
		UINT32 temp															= ARCH_READ_REGISTER_UINT32(Add2Ptr(baseRegister, 0xec, UINT32*));
		if(temp & 0x80000000)
		{
			*value															= static_cast<UINT8>((temp >> 0x10) & 0xff);
			return TRUE;
		}
	}

	return FALSE;
}

//
// write phy register
//
STATIC BOOLEAN Bd1394pWritePhyRegister(VOID* baseRegister, UINT8 index, UINT8 value)
{
	//
	// write phy register index to phy control
	//
	UINT32 phyControl														= (((index & 0xf) | 0x40) << 8) | value;
	ARCH_WRITE_REGISTER_UINT32(Add2Ptr(baseRegister, 0xec, UINT32*), phyControl);

	for(UINT32 i = 0; i < 400000; i ++)
	{
		//
		// check write done
		//
		if(!(ARCH_READ_REGISTER_UINT32(Add2Ptr(baseRegister, 0xec, UINT32*)) & 0x00004000))
			return TRUE;
	}

	return FALSE;
}

//
// initialize ohci controller
//
STATIC BOOLEAN Bd1394pInitializeController(UINT32 channel)
{
	//
	// setup debug info
	//
	Bd1394pGlobalData->DebugConfigInfo.Signature							= 0xbabababa;
	Bd1394pGlobalData->DebugConfigInfo.BusPresent							= FALSE;
	Bd1394pGlobalData->DebugConfigInfo.Channel								= (channel < 0x100) ? channel : 0;
	Bd1394pGlobalData->DebugConfigInfo.MajorVersion							= 1;
	Bd1394pGlobalData->DebugConfigInfo.MinorVersion							= 0;
	Bd1394pGlobalData->DebugConfigInfo.ReceiveBuffer						= Bd1394pGetPhysicalAddress(&Bd1394pGlobalData->ReceiveBuffer);
	Bd1394pGlobalData->DebugConfigInfo.SendBuffer							= Bd1394pGetPhysicalAddress(&Bd1394pGlobalData->SendBuffer);

	//
	// read version, must be 1
	//
	UINT32 temp																= ARCH_READ_REGISTER_UINT32(static_cast<UINT32*>(Bd1394pControllerRegisterBase));
	if((temp & 0x00ff0000) != 0x00010000)
		return FALSE;

	//
	// do a soft reset
	//
	ARCH_WRITE_REGISTER_UINT32(Add2Ptr(Bd1394pControllerRegisterBase, 0x50, UINT32*), 0x10000);

	//
	// wait soft reset complete
	//
	for(UINT32 i = 0; i < 1000; i ++)
	{
		temp																= ARCH_READ_REGISTER_UINT32(Add2Ptr(Bd1394pControllerRegisterBase, 0x50, UINT32*));
		Bd1394pStallExecution(1);
		if(temp & 0x10000)
			continue;

		break;
	}

	//
	// soft reset timeout
	//
	if(temp & 0x10000)
		return FALSE;

	//
	// enable link <-> phy communication
	//
	ARCH_WRITE_REGISTER_UINT32(Add2Ptr(Bd1394pControllerRegisterBase, 0x50, UINT32*), 0x80000);
	Bd1394pStallExecution(20);

	//
	// end / recv data in little-endian
	//
	ARCH_WRITE_REGISTER_UINT32(Add2Ptr(Bd1394pControllerRegisterBase, 0x54, UINT32*), 0x40000000);

	//
	// enable post-write
	//
	ARCH_WRITE_REGISTER_UINT32(Add2Ptr(Bd1394pControllerRegisterBase, 0x50, UINT32*), 0x40000);

	//
	// cycle Master | cycle Enable
	//
	ARCH_WRITE_REGISTER_UINT32(Add2Ptr(Bd1394pControllerRegisterBase, 0xe4, UINT32*), 0x300600);
	ARCH_WRITE_REGISTER_UINT32(Add2Ptr(Bd1394pControllerRegisterBase, 0xe0, UINT32*), 0x300000);

	//
	// busNumber 0x3ff,NodeNumber 0
	//
	ARCH_WRITE_REGISTER_UINT32(Add2Ptr(Bd1394pControllerRegisterBase, 0xe8, UINT32*),0xffc0);

	//
	// set singnatuer
	//
	Bd1394pGlobalData->DebugConfigRom.Singnature							= '1394';

	//
	// set bus options
	//
	temp																	= ARCH_READ_REGISTER_UINT32(Add2Ptr(Bd1394pControllerRegisterBase, 0x20, UINT32*));
	temp																	= SWAP32(temp);
	temp																	&= 0x7ffff3f;
	temp																	|= 0x40;
	Bd1394pGlobalData->DebugConfigRom.BusOptions							= SWAP32(temp);

	//
	// read global unique id
	//
	Bd1394pGlobalData->DebugConfigRom.GlobalUniqueId[0]						= ARCH_READ_REGISTER_UINT32(Add2Ptr(Bd1394pControllerRegisterBase, 0x24, UINT32*));
	Bd1394pGlobalData->DebugConfigRom.GlobalUniqueId[1]						= ARCH_READ_REGISTER_UINT32(Add2Ptr(Bd1394pControllerRegisterBase, 0x28, UINT32*));

	//
	// setup config rom
	//
	IEEE1394_CONFIG_ROM_INFO Info;
	Info.AsULong															= 0;
	Info.CrcInfo.CriCrcLength												= 4;
	Info.CrcInfo.CriInfoLength												= 4;
	Info.CrcInfo.CriCrcValue												= Bd1394pCalculateCrc(&Bd1394pGlobalData->DebugConfigRom.Singnature, 4);
	Bd1394pGlobalData->DebugConfigRom.Header								= Info;
	Bd1394pGlobalData->DebugConfigRom.NodeCapabilities						= 0xc083000c;
	Bd1394pGlobalData->DebugConfigRom.ModuleVendorId						= 0xf2500003;
	Bd1394pGlobalData->DebugConfigRom.ExtendedKey							= 0xf250001c;
	Bd1394pGlobalData->DebugConfigRom.DebugKey								= 0x0200001d;
	temp																	= (0xffffff & static_cast<UINT32>(Bd1394pGetPhysicalAddress(&Bd1394pGlobalData->DebugConfigInfo))) | 0x1e000000;
	Bd1394pGlobalData->DebugConfigRom.DebugConfigInfo						= SWAP32(temp);
	Info.AsULong															= 0;
	Info.CrcInfo.CriCrcLength												= 5;
	Info.CrcInfo.CriCrcValue												= Bd1394pCalculateCrc(&Bd1394pGlobalData->DebugConfigRom.NodeCapabilities, Info.CrcInfo.CriCrcLength);
	Info.AsULong															= SWAP32(Info.AsULong);
	Bd1394pGlobalData->DebugConfigRom.DirectoryInfoHead						= Info;

	//
	// write the new config rom head
	//
	ARCH_WRITE_REGISTER_UINT32(Add2Ptr(Bd1394pControllerRegisterBase, 0x18, UINT32*), Bd1394pGlobalData->DebugConfigRom.Header.AsULong);
	ARCH_WRITE_REGISTER_UINT32(Add2Ptr(Bd1394pControllerRegisterBase, 0x1c, UINT32*), Bd1394pGlobalData->DebugConfigRom.Singnature);
	ARCH_WRITE_REGISTER_UINT32(Add2Ptr(Bd1394pControllerRegisterBase, 0x20, UINT32*), Bd1394pGlobalData->DebugConfigRom.BusOptions);
	ARCH_WRITE_REGISTER_UINT32(Add2Ptr(Bd1394pControllerRegisterBase, 0x24, UINT32*), Bd1394pGlobalData->DebugConfigRom.GlobalUniqueId[0]);
	ARCH_WRITE_REGISTER_UINT32(Add2Ptr(Bd1394pControllerRegisterBase, 0x28, UINT32*), Bd1394pGlobalData->DebugConfigRom.GlobalUniqueId[1]);

	//
	// write config rom address
	//
	ARCH_WRITE_REGISTER_UINT32(Add2Ptr(Bd1394pControllerRegisterBase, 0x34, UINT32*), static_cast<UINT32>(Bd1394pGetPhysicalAddress(&Bd1394pGlobalData->DebugConfigRom)));

	//
	// mask all interrupt
	//
	ARCH_WRITE_REGISTER_UINT32(Add2Ptr(Bd1394pControllerRegisterBase, 0x8c, UINT32*), 0xffffffff);

	//
	// enable link
	//
	ARCH_WRITE_REGISTER_UINT32(Add2Ptr(Bd1394pControllerRegisterBase, 0x50, UINT32*), 0x20000);
	Bd1394pStallExecution(1000);

	//
	// accept asyn request from all the nodes
	//
	ARCH_WRITE_REGISTER_UINT32(Add2Ptr(Bd1394pControllerRegisterBase, 0x108, UINT32*), 0xffffffff);
	ARCH_WRITE_REGISTER_UINT32(Add2Ptr(Bd1394pControllerRegisterBase, 0x100, UINT32*), 0xffffffff);

	//
	// accept asyn physical request from all the nodes
	//
	ARCH_WRITE_REGISTER_UINT32(Add2Ptr(Bd1394pControllerRegisterBase, 0x110, UINT32*), 0xffffffff);
	ARCH_WRITE_REGISTER_UINT32(Add2Ptr(Bd1394pControllerRegisterBase, 0x118, UINT32*), 0xffffffff);

	//
	// hard reset on the bus
	//
	UINT8 phyTemp;
	if(!Bd1394pReadPhyRegister(Bd1394pControllerRegisterBase, 1, &phyTemp))
		return FALSE;

	phyTemp																	|= 0x40;
	Bd1394pWritePhyRegister(Bd1394pControllerRegisterBase, 1, phyTemp);
	Bd1394pStallExecution(2000);

	return TRUE;
}

//
// check rom
//
STATIC VOID Bd1394pCheckRom(DBG1394_GLOBAL_DATA* globalData)
{
	//
	// get current rom address
	//
	UINT32 currentRom														= ARCH_READ_REGISTER_UINT32(Add2Ptr(Bd1394pControllerRegisterBase, 0x34, UINT32*));

	//
	// get our rom address
	//
	UINT32 dbgRom															= static_cast<UINT32>(Bd1394pGetPhysicalAddress(&globalData->DebugConfigRom));

	//
	// reinitialize controller
	//
	if(currentRom != dbgRom)
		Bd1394pInitializeController(globalData->DebugConfigInfo.Channel);
}

//
// enable physical access
//
STATIC VOID Bd1394pEnablePhysicalAccess(DBG1394_GLOBAL_DATA* globalData)
{
	//
	// check config rom
	//
	Bd1394pCheckRom(globalData);

	//
	// link enabled
	//
	UINT32 temp																= ARCH_READ_REGISTER_UINT32(Add2Ptr(Bd1394pControllerRegisterBase, 0x50, UINT32*));
	if(!(temp & 0x20000))
		return;

	//
	// link <-> PHY communication permitted
	//
	if(!(temp & 0x80000))
		return;

	//
	// soft resetting
	//
	if(temp & 0x10000)
		return;

	//
	// clear bus reset interrupt
	//
	if(ARCH_READ_REGISTER_UINT32(Add2Ptr(Bd1394pControllerRegisterBase, 0x80, UINT32*)) & 0x20000)
		ARCH_WRITE_REGISTER_UINT32(Add2Ptr(Bd1394pControllerRegisterBase, 0x84, UINT32*), 0x20000);

	//
	// allow asyn request from any node
	//
	ARCH_WRITE_REGISTER_UINT32(Add2Ptr(Bd1394pControllerRegisterBase, 0x100, UINT32*), 0xffffffff);
	ARCH_WRITE_REGISTER_UINT32(Add2Ptr(Bd1394pControllerRegisterBase, 0x108, UINT32*), 0xffffffff);

	//
	// allow asyn phy request from any node
	//
	ARCH_WRITE_REGISTER_UINT32(Add2Ptr(Bd1394pControllerRegisterBase, 0x110, UINT32*), 0xffffffff);
	ARCH_WRITE_REGISTER_UINT32(Add2Ptr(Bd1394pControllerRegisterBase, 0x118, UINT32*), 0xffffffff);
}

//
// read packet
//
STATIC UINT32 Bd1394pReadPacket(DBG1394_GLOBAL_DATA* globalData, KD_PACKET* packetHeader, STRING* messageHeader, STRING* messageData, BOOLEAN waitPacket)
{
	for(UINT32 i = 0; i < 512000; i ++)
	{
		//
		// enable physical access
		//
		Bd1394pEnablePhysicalAccess(globalData);

		//
		// check receive buffer state
		//
		if(globalData->ReceiveBuffer.ReceiveBuffer.Status == STATUS_PENDING)
		{
			//
			// some data pending, debugger present
			//
			BdDebuggerNotPresent											= FALSE;

			//
			// read packet header
			//
			memcpy(packetHeader, &globalData->ReceiveBuffer.ReceiveBuffer.PacketHeader, sizeof(KD_PACKET));

			//
			// packet header is invalid
			//
			if(globalData->ReceiveBuffer.ReceiveBuffer.TotalLength < sizeof(KD_PACKET))
			{
				//
				// finish
				//
				globalData->ReceiveBuffer.ReceiveBuffer.Status				= STATUS_SUCCESS;

				return KDP_PACKET_RESEND;
			}

			if(messageHeader)
			{
				//
				// copy message header
				//
				memcpy(messageHeader->Buffer, globalData->ReceiveBuffer.ReceiveBuffer.DataBuffer, messageHeader->MaximumLength);

				//
				// check data buffer
				//
				UINT32 copyLength											= globalData->ReceiveBuffer.ReceiveBuffer.TotalLength - sizeof(KD_PACKET) - messageHeader->MaximumLength;
				if(globalData->ReceiveBuffer.ReceiveBuffer.TotalLength > sizeof(KD_PACKET) + messageHeader->MaximumLength && messageData)
					memcpy(messageData->Buffer, globalData->ReceiveBuffer.ReceiveBuffer.DataBuffer + messageHeader->MaximumLength, copyLength);
			}

			//
			// finish
			//
			globalData->ReceiveBuffer.ReceiveBuffer.Status					= STATUS_SUCCESS;

			return KDP_PACKET_RECEIVED;
		}

		//
		// caller did not want to wait
		//
		if(!waitPacket)
			return KDP_PACKET_RESEND;
	}

	//
	// timeout
	//
	return KDP_PACKET_TIMEOUT;
}

//
// send control packet
//
STATIC VOID Bd1394pSendControlPacket(UINT32 packetType, UINT32 packetId)
{
	//
	// zero send buffer out
	//
	memset(&Bd1394pGlobalData->SendBuffer, 0, sizeof(Bd1394pGlobalData->SendBuffer));

	//
	// control packet does not have a data buffer
	//
	Bd1394pGlobalData->SendBuffer.SendBuffer.DataLength						= 0;

	//
	// setup packet header, the other fields are already zeroed out by RtlZeroMemory, and PacketId is always set to zero in ms's original KD1394.dll
	//
	KD_PACKET* packetHeader													= &Bd1394pGlobalData->SendBuffer.SendBuffer.PacketHeader;
	packetHeader->PacketLeader												= CONTROL_PACKET_LEADER;
	packetHeader->PacketType												= static_cast<UINT16>(packetType);

	//
	// 'send' it without wait
	//
	Bd1394pGlobalData->SendBuffer.SendBuffer.Status							= STATUS_PENDING;
}

//
// receive a packet
//
UINT32 Bd1394ReceivePacket(UINT32 packetType, STRING* messageHeader, STRING* messageData, UINT32* dataLength)
{
	//
	// enable physical access
	//
	Bd1394pEnablePhysicalAccess(Bd1394pGlobalData);

	//
	// special case for poll breakin (boot loader will never use this)
	//
	if(packetType == PACKET_TYPE_KD_POLL_BREAKIN)
	{
		//
		// have a pending packet?
		//
		if(Bd1394pGlobalData->ReceiveBuffer.ReceiveBuffer.Status != STATUS_PENDING)
			return KDP_PACKET_TIMEOUT;

		//
		// breakin?
		//
		if(static_cast<UINT8>(Bd1394pGlobalData->ReceiveBuffer.ReceiveBuffer.PacketHeader.PacketLeader) != BREAKIN_PACKET_BYTE)
			return KDP_PACKET_TIMEOUT;

		//
		// yes, got it
		//
		BdDebuggerNotPresent												= FALSE;
		Bd1394pGlobalData->ReceiveBuffer.ReceiveBuffer.Status				= STATUS_SUCCESS;

		return KDP_PACKET_RECEIVED;
	}

	//
	// read the packet
	//
	KD_PACKET packetHeader;
	for( ; TRUE ; Bd1394pSendControlPacket(PACKET_TYPE_KD_RESEND, 0))
	{
		UINT32 ret															= Bd1394pReadPacket(Bd1394pGlobalData, &packetHeader, messageHeader, messageData, TRUE);

		//
		// received something, it has high possibility that kernel debugger is alive, so reset counter
		//
		if(ret != KDP_PACKET_TIMEOUT)
		{
			BdNumberRetries													= BdRetryCount;
			BdDebuggerNotPresent											= FALSE;
		}

		//
		// failed to read
		//
		if(ret != KDP_PACKET_RECEIVED)
		{
			//
			// check breakin
			//
			if(static_cast<UINT8>(packetHeader.PacketLeader) == BREAKIN_PACKET_BYTE)
			{
				BdControlCPending											= TRUE;
				ret															= KDP_PACKET_RESEND;
			}

			return ret;
		}

		//
		// if the packet we received is a resend request, we return true and let caller resend the packet.
		//
		if(packetHeader.PacketLeader == CONTROL_PACKET_LEADER && packetHeader.PacketType == PACKET_TYPE_KD_RESEND)
			return KDP_PACKET_RESEND;

		//
		// check length, if length is invalid, send a resend control packet and loop again
		//
		if(packetHeader.ByteCount > PACKET_MAX_SIZE || packetHeader.ByteCount < messageHeader->MaximumLength)
			continue;

		//
		// setup length
		//
		*dataLength															= packetHeader.ByteCount - messageHeader->MaximumLength;
		messageHeader->Length												= messageHeader->MaximumLength;
		messageData->Length													= static_cast<UINT16>(*dataLength);

		//
		// not the one we are waiting for
		//
		if(packetHeader.PacketType != packetType)
			continue;

		//
		// calc checksum, and compare it
		//
		UINT32 checksum														= BdComputeChecksum(messageHeader->Buffer, messageHeader->Length);
		checksum															+= BdComputeChecksum(messageData->Buffer, messageData->Length);

		if(checksum == packetHeader.Checksum)
			return KDP_PACKET_RECEIVED;
	}

	return KDP_PACKET_TIMEOUT;
}

//
// send packet
//
VOID Bd1394SendPacket(UINT32 packetType, STRING* messageHeader, STRING* messageData)
{
	//
	// calc checksum
	//
	KD_PACKET packetHeader;
	if(messageData)
	{
		packetHeader.Checksum												= BdComputeChecksum(messageData->Buffer, messageData->Length);
		packetHeader.ByteCount												= messageData->Length;
	}
	else
	{
		packetHeader.Checksum												= 0;
		packetHeader.ByteCount												= 0;
	}

	//
	// setup packet header
	//
	packetHeader.Checksum													+= BdComputeChecksum(messageHeader->Buffer, messageHeader->Length);
	packetHeader.PacketType													= static_cast<UINT16>(packetType);
	packetHeader.ByteCount													+= messageHeader->Length;
	packetHeader.PacketId													= BdNextPacketIdToSend;
	packetHeader.PacketLeader												= PACKET_LEADER;
	BdNextPacketIdToSend													+= 1;

	//
	// set retry count
	//
	BdNumberRetries															= BdRetryCount;

	//
	// setup send buffer
	//
	DBG1394_SEND_BUFFER* sendBuffer											= &Bd1394pGlobalData->SendBuffer;
	memcpy(&sendBuffer->SendBuffer.PacketHeader, &packetHeader, sizeof(KD_PACKET));

	memcpy(sendBuffer->SendBuffer.DataBuffer, messageHeader->Buffer, messageHeader->Length);
	sendBuffer->SendBuffer.DataLength										= messageHeader->Length;

	if(messageData)
	{
		memcpy(sendBuffer->SendBuffer.DataBuffer + messageHeader->Length, messageData->Buffer, messageData->Length);
		sendBuffer->SendBuffer.DataLength									+= messageData->Length;
	}

	//
	// and 'send' it
	//
	sendBuffer->SendBuffer.Status											= STATUS_PENDING;

	BOOLEAN exceptionPacketTimeouted										= FALSE;
	DBG1394_RECEIVE_BUFFER* receiveBuffer									= &Bd1394pGlobalData->ReceiveBuffer;
	while(TRUE)
	{
		if(BdNumberRetries == 0)
		{
			DBGKD_WAIT_STATE_CHANGE64* stateChange							= static_cast<DBGKD_WAIT_STATE_CHANGE64*>(static_cast<VOID*>(messageHeader->Buffer));
			DBGKD_DEBUG_IO* debugIo											= static_cast<DBGKD_DEBUG_IO*>(static_cast<VOID*>(messageHeader->Buffer));
			DBGKD_FILE_IO* fileIo											= static_cast<DBGKD_FILE_IO*>(static_cast<VOID*>(messageHeader->Buffer));

			//
			// if the packet is not for reporting exception, we give up and declare debugger not present.
			//
			if( (packetType == PACKET_TYPE_KD_DEBUG_IO && debugIo->ApiNumber == DbgKdPrintStringApi) ||
				(packetType == PACKET_TYPE_KD_STATE_CHANGE64 && stateChange->NewState == DbgKdLoadSymbolsStateChange) ||
				(packetType == PACKET_TYPE_KD_FILE_IO && fileIo->ApiNumber == DbgKdCreateFileApi) ||
				(packetType == PACKET_TYPE_KD_TRACE_IO && fileIo->ApiNumber == DbgKdPrintTraceApi))
			{
				BdDebuggerNotPresent										= TRUE;
				sendBuffer->SendBuffer.Status								= STATUS_SUCCESS;
				return;
			}

			if(packetType != PACKET_TYPE_KD_DEBUG_IO && packetType != PACKET_TYPE_KD_STATE_CHANGE64 && packetType != PACKET_TYPE_KD_FILE_IO)
				exceptionPacketTimeouted									= TRUE;
		}

		BOOLEAN done														= FALSE;

		for(UINT32 i = 0; i < 512000; i ++)
		{
			//
			// enable physical access
			//
			Bd1394pEnablePhysicalAccess(Bd1394pGlobalData);

			//
			// debugger read this packet? or debugger gave us a new command?
			//
			if(sendBuffer->SendBuffer.Status != STATUS_PENDING || (receiveBuffer->ReceiveBuffer.Status == STATUS_PENDING && !exceptionPacketTimeouted))
			{
				done														= TRUE;
				break;
			}
		}

		//
		// finished
		//
		if(done)
			break;

		//
		// retry
		//
		BdNumberRetries														-= 1;
	}

	//
	// since we are able to talk to debugger, the retrycount is set to maximum value.
	//
	BdRetryCount															= 0x14;
	BdDebuggerNotPresent													= FALSE;
}

//
// setup debug device
//
EFI_STATUS Bd1394ConfigureDebuggerDevice(CHAR8 CONST* loaderOptions)

{
	EFI_STATUS status														= EFI_SUCCESS;

	__try
	{
		if(!loaderOptions || !loaderOptions[0])
			try_leave(NOTHING);

		//
		// get channel
		//
		UINT32 channel														= 12;
		CHAR8 CONST* channleParam											= strstr(loaderOptions, CHAR8_CONST_STRING("/channel="));
		if(!channleParam)
			try_leave(status = EFI_INVALID_PARAMETER);
		channel																= static_cast<UINT32>(atoi(channleParam + 9));

		//
		// connect wait
		//
		CHAR8 CONST* connectWait											= strstr(loaderOptions, CHAR8_CONST_STRING("/connectwait="));
		if(connectWait)
			EfiBootServices->Stall(1000 * 1000 * static_cast<UINT32>(atoi(connectWait + 13)));

		//
		// connect all
		//
		if(strstr(loaderOptions, CHAR8_CONST_STRING("/connectall")) && EFI_ERROR(status = BlConnectAllController()))
			try_leave(NOTHING);

		//
		// parse location
		//
		UINTN segment														= static_cast<UINTN>(-1);
		UINTN bus															= static_cast<UINTN>(-1);
		UINTN device														= static_cast<UINTN>(-1);
		UINTN func															= static_cast<UINTN>(-1);
		BlParseDeviceLocation(loaderOptions, &segment, &bus, &device, &func);

		//
		// find pci device
		//
		EFI_PCI_IO_PROTOCOL* pciIoProtocol									= nullptr;
		EFI_HANDLE controllerHandle											= nullptr;
		if(EFI_ERROR(status = BlFindPciDevice(segment, bus, device, func, PCI_CLASS_SERIAL, PCI_CLASS_SERIAL_FIREWIRE, 0x10, &pciIoProtocol, &controllerHandle)))
			try_leave(NOTHING);

		//
		// disconnect from efi
		//
		EFI_GUID firewireProtocolGuid										= {0x67708aa8, 0x2079, 0x4e4f, 0xb1, 0x58, 0xb1, 0x5b, 0x1f, 0x6a, 0x6c, 0x92};
		VOID* protocol														= nullptr;
		status																= EfiBootServices->HandleProtocol(controllerHandle, &firewireProtocolGuid, &protocol);
		CsPrintf(CHAR8_CONST_STRING("handle protocol %08x, %08x\n"), status, protocol);
		EfiBootServices->DisconnectController(controllerHandle, nullptr, nullptr);
		status																= EfiBootServices->HandleProtocol(controllerHandle, &firewireProtocolGuid, &protocol);
		CsPrintf(CHAR8_CONST_STRING("handle protocol %08x, %08x\n"), status, protocol);

		//
		// get bar attribute
		//
		UINT64 phyAddress													= 0;
		UINT64 barLength													= 0;
		BOOLEAN isMemorySpace												= FALSE;
		if(EFI_ERROR(status = BlGetPciBarAttribute(pciIoProtocol, 0, &phyAddress, &barLength, &isMemorySpace)))
			try_leave(NOTHING);

		//
		// check bar length
		//
		if(barLength < 0x800 || !isMemorySpace)
			try_leave(status = EFI_NOT_FOUND);

		//
		// start device
		//
		if(EFI_ERROR(status = BlStartPciDevice(pciIoProtocol, FALSE, TRUE, TRUE)))
			try_leave(NOTHING);

		//
		// 4GB check
		//
		if(phyAddress >= 4 * 1024 * 1024 * 1024ULL)
			try_leave(status = EFI_NOT_FOUND);

		//
		// allocate physical page
		//
		Bd1394pControllerRegisterBase										= ArchConvertAddressToPointer(phyAddress, VOID*);
		Bd1394pDataPhysicalAddress											= 1 * 1024 * 1024 - 1;
		if(!MmAllocatePages(AllocateMaxAddress, EfiReservedMemoryType, EFI_SIZE_TO_PAGES(sizeof(DBG1394_GLOBAL_DATA)), &Bd1394pDataPhysicalAddress))
			try_leave(Bd1394pDataPhysicalAddress = 0;status = EFI_OUT_OF_RESOURCES);

		//
		// zero it out
		//
		Bd1394pGlobalData													= ArchConvertAddressToPointer(Bd1394pDataPhysicalAddress, DBG1394_GLOBAL_DATA*);
		memset(Bd1394pGlobalData, 0, sizeof(DBG1394_GLOBAL_DATA));

		//
		// initialize controller
		//
		if(!Bd1394pInitializeController(channel))
			try_leave(status = EFI_DEVICE_ERROR);

		//
		// set next packet id
		//
		BdNextPacketIdToSend												= 0;
		status																= EFI_SUCCESS;
	}
	__finally
	{
		//
		// clean up
		//
		if(EFI_ERROR(status))
		{
			//
			// free
			//
			if(Bd1394pDataPhysicalAddress)
				MmFreePages(Bd1394pDataPhysicalAddress);

			Bd1394pGlobalData												= nullptr;
			Bd1394pDataPhysicalAddress										= 0;
			Bd1394pControllerRegisterBase									= nullptr;
		}
	}
	return status;
}

//
// close debug device
//
EFI_STATUS Bd1394CloseDebuggerDevice()
{
	if(Bd1394pDataPhysicalAddress)
		MmFreePages(Bd1394pDataPhysicalAddress);

	Bd1394pGlobalData														= nullptr;
	Bd1394pDataPhysicalAddress												= 0;
	Bd1394pControllerRegisterBase											= nullptr;

	return EFI_SUCCESS;
}