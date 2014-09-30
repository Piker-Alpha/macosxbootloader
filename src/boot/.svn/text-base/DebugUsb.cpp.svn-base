//********************************************************************
//	created:	5:8:2012   15:04
//	filename: 	DebugUsb.cpp
//	author:		tiamo
//	purpose:	debug over usb
//********************************************************************

#include "StdAfx.h"
#include "DebugUsb.h"

#include <pshpack1.h>

//
// global data
//
typedef struct _DEBUG_USB_GLOBAL_DATA
{
	//
	// base register
	//
	VOID*																	BaseRegister;

	//
	// operational register
	//
	VOID*																	OperationalRegister;

	//
	// debug register
	//
	VOID*																	DebugRegister;

	//
	// usb address
	//
	UINT8																	UsbAddress;

	//
	// in endpoint
	//
	UINT8																	InputEndpoint;

	//
	// out endpoint
	//
	UINT8																	OutputEndpoint;

	//
	// next recv pid
	//
	UINT8																	NextReceiveDataPid;

	//
	// next send pid
	//
	UINT8																	NextSendDataPid;

	//
	// target name length
	//
	UINT8																	NameLength;

	//
	// full packet received
	//
	BOOLEAN																	FullPacketReceived;

	//
	// need reinitialize device
	//
	BOOLEAN																	NeedReinitializeDevice;

	//
	// target name
	//
	CHAR8																	TargetName[32];

	//
	// packet header
	//
	KD_PACKET																PacketHeader;

	//
	// data buffer
	//
	UINT8																	DataBuffer[PACKET_MAX_SIZE];

	//
	// data buffer offset
	//
	UINT32																	DataBufferOffset;

	//
	// debug port number
	//
	UINT32																	DebugPortNumber;
}DEBUG_USB_GLOBAL_DATA;

//
// usb setup packet
//
typedef struct _DEBUG_USB_SETUP_PACKET
{
	//
	// request type
	//
	UINT8																	RequestType;

	//
	// request
	//
	UINT8																	Request;

	//
	// value
	//
	UINT16																	Value;

	//
	// index
	//
	UINT16																	Index;

	//
	// length
	//
	UINT16																	Length;
}DEBUG_USB_SETUP_PACKET;

#include <poppack.h>

//
// send buffer
//
typedef struct _DEBUG_USB_SEND_BUFFER
{
	//
	// buffer
	//
	VOID*																	Buffer;

	//
	// length
	//
	UINT32																	Length;
}DEBUG_USB_SEND_BUFFER;

//
// global buffer
//
DEBUG_USB_GLOBAL_DATA* BdUsbpGlobalData										= nullptr;

//
// wait
//
STATIC BOOLEAN BdUsbpWait(DEBUG_USB_GLOBAL_DATA* globalData, UINT32 millisecond)
{
	BOOLEAN retValue														= FALSE;
	__try
	{
		//
		// read FRINDEX
		//
		UINT32 temp															= ARCH_READ_REGISTER_UINT32(Add2Ptr(globalData->OperationalRegister, 0x0c, UINT32*));
		if(temp == 0xffffffff)
			try_leave(NOTHING);

		//
		// FRINDEX will be updated every 125 microseconds
		//
		UINT32 startIndex													= temp & 0x3fff;
		while(TRUE)
		{
			//
			// read USBCMD
			//
			temp															= ARCH_READ_REGISTER_UINT32(Add2Ptr(globalData->OperationalRegister, 0x00, UINT32*));

			//
			// stopped
			//
			if(!(temp & 1))
				try_leave(NOTHING);

			//
			// read FRINDEX
			//
			temp															= ARCH_READ_REGISTER_UINT32(Add2Ptr(globalData->OperationalRegister, 0x0c, UINT32*));
			if(temp == 0xffffffff)
				try_leave(NOTHING);

			//
			// XXX: overflow?
			//
			if(static_cast<UINT32>((temp & 0x3fff) - startIndex) > millisecond * 8)
				break;
		}
		retValue															= TRUE;
	}
	__finally
	{

	}
	return retValue;
}

//
// halt host controller
//
STATIC BOOLEAN BdUsbpHaltController(DEBUG_USB_GLOBAL_DATA* globalData)
{
	BOOLEAN retValue														= FALSE;
	__try
	{
		//
		// read USBSTS
		//
		UINT32 temp															= ARCH_READ_REGISTER_UINT32(Add2Ptr(globalData->OperationalRegister, 0x04, UINT32*));
		if(temp == 0xffffffff)
			try_leave(NOTHING);

		//
		// check already Halted
		//
		if(temp & 0x1000)
			try_leave(retValue = TRUE);

		//
		// read USBCMD
		//
		temp																= ARCH_READ_REGISTER_UINT32(Add2Ptr(globalData->OperationalRegister, 0x00, UINT32*));
		if(temp == 0xffffffff)
			try_leave(NOTHING);

		//
		// stop
		//
		ARCH_WRITE_REGISTER_UINT32(Add2Ptr(globalData->OperationalRegister, 0x00, UINT32*), temp & 0xfffffffe);

		//
		// max wait 200s
		//
		UINT32 endTime														= BlGetCurrentUnixTime() + 200;
		while(BlGetCurrentUnixTime() < endTime)
		{
			//
			// read USBSTS
			//
			temp															= ARCH_READ_REGISTER_UINT32(Add2Ptr(globalData->OperationalRegister, 0x04, UINT32*));
			if(temp == 0xffffffff)
				try_leave(NOTHING);

			//
			// check Halted
			//
			if(temp & 0x1000)
				try_leave(retValue = TRUE);
		}
	}
	__finally
	{

	}
	return retValue;
}

//
// initialize host controller hardware
//
STATIC BOOLEAN BdUsbpInitializeControllerHardware(DEBUG_USB_GLOBAL_DATA* globalData, EFI_PCI_IO_PROTOCOL* pciIoProtocol)
{
	BOOLEAN retValue														= FALSE;
	__try
	{
		//
		// halt host controller
		//
		if(!BdUsbpHaltController(globalData))
			try_leave(NOTHING);

		//
		// read USBCMD
		//
		UINT32 temp															= ARCH_READ_REGISTER_UINT32(Add2Ptr(globalData->OperationalRegister, 0x00, UINT32*));
		if(temp == 0xffffffff)
			try_leave(NOTHING);

		//
		// reset
		//
		ARCH_WRITE_REGISTER_UINT32(Add2Ptr(globalData->OperationalRegister, 0x00, UINT32*), temp | 2);

		//
		// wait reset done
		//
		UINT32 endTime														= BlGetCurrentUnixTime() + 200;
		while(BlGetCurrentUnixTime() < endTime)
		{
			//
			// read USBCMD
			//
			temp															= ARCH_READ_REGISTER_UINT32(Add2Ptr(globalData->OperationalRegister, 0x00, UINT32*));
			if(temp == 0xffffffff)
				try_leave(NOTHING);

			//
			// check reset complete
			//
			if(!(temp & 2))
				break;
		}

		//
		// timeout
		//
		if(temp & 2)
			try_leave(NOTHING);

		//
		// schedule enabled
		//
		if(temp & 0x30)
			try_leave(NOTHING);

		//
		// run
		//
		ARCH_WRITE_REGISTER_UINT32(Add2Ptr(globalData->OperationalRegister, 0x00, UINT32*), temp | 1);

		//
		// routing to EHCI
		//
		ARCH_WRITE_REGISTER_UINT32(Add2Ptr(globalData->OperationalRegister, 0x40, UINT32*), 1);

		//
		// wait done
		//
		endTime																= BlGetCurrentUnixTime() + 200;
		while(BlGetCurrentUnixTime() < endTime)
		{
			//
			// read USBSTS
			//
			temp															= ARCH_READ_REGISTER_UINT32(Add2Ptr(globalData->OperationalRegister, 0x04, UINT32*));
			if(temp == 0xffffffff)
				try_leave(NOTHING);

			//
			// check Halted
			//
			if(!(temp & 0x1000))
				break;
		}

		//
		// timeout
		//
		if(temp & 0x1000)
			try_leave(NOTHING);
		
		//
		// read debug port status
		//
		temp																= ARCH_READ_REGISTER_UINT32(Add2Ptr(globalData->DebugRegister, 0x00, UINT32*));
		if(temp == 0xffffffff)
			try_leave(NOTHING);

		//
		// set in-use owner
		//
		ARCH_WRITE_REGISTER_UINT32(Add2Ptr(globalData->DebugRegister, 0x00, UINT32*), temp | 0x40000400);
		if(!BdUsbpWait(globalData, 20))
			try_leave(NOTHING);

		//
		// read HCSPARAMS
		//
		temp																= ARCH_READ_REGISTER_UINT32(Add2Ptr(globalData->BaseRegister, 0x04, UINT32*));
		if(temp == 0xffffffff)
			try_leave(NOTHING);

		//
		// check port power control
		//
		if(temp & 0x10)
		{
			UINT32* port													= Add2Ptr(globalData->OperationalRegister, 0x40 + ((temp >> 20) & 0x0f) * 4, UINT32*);
			temp															= ARCH_READ_REGISTER_UINT32(port);
			ARCH_WRITE_REGISTER_UINT32(port, temp | 0x1000);
			if(!BdUsbpWait(globalData, 20))
				try_leave(NOTHING);
		}
		retValue															= TRUE;
	}
	__finally
	{

	}
	return retValue;
}

//
// locate debug port register
//
VOID* BdUsbpLocateDebugPortRegister(DEBUG_USB_GLOBAL_DATA* globalData, EFI_PCI_IO_PROTOCOL* pciIoProtocol)
{
	//
	// read status
	//
	UINT16 status															= 0;
	if(EFI_ERROR(pciIoProtocol->Pci.Read(pciIoProtocol, EfiPciIoWidthUint16, 6, sizeof(status), &status)))
		return nullptr;

	//
	// check cap ptr
	//
	if(!(status & 0x10))
		return nullptr;

	//
	// read caps offset
	//
	UINT8 capsOffset														= 0x34;
	if(EFI_ERROR(pciIoProtocol->Pci.Read(pciIoProtocol, EfiPciIoWidthUint8, capsOffset, sizeof(capsOffset), &capsOffset)))
		return nullptr;

	UINT8 pciCaps[2]														= {0};
	while(capsOffset && !(capsOffset & 0x03) && capsOffset < 0x100)
	{
		if(EFI_ERROR(pciIoProtocol->Pci.Read(pciIoProtocol, EfiPciIoWidthUint16, capsOffset, sizeof(pciCaps), pciCaps)))
			return nullptr;

		if(pciCaps[0] == 0x0a)
		{
			UINT16 offset													= 0;
			if(EFI_ERROR(pciIoProtocol->Pci.Read(pciIoProtocol, EfiPciIoWidthUint16, capsOffset + sizeof(pciCaps), sizeof(offset), &offset)))
				return nullptr;

			if((offset & 0xe000) != 0x2000)
				return nullptr;

			return Add2Ptr(globalData->BaseRegister, (offset & 0xfff), VOID*);
		}
		capsOffset															= pciCaps[1];
	}
	return nullptr;
}

//
// initialize host controller
//
STATIC BOOLEAN BdUsbpInitializeController(DEBUG_USB_GLOBAL_DATA* globalData, EFI_PCI_IO_PROTOCOL* pciIoProtocol)
{
	//
	// read CAPLENGTH
	//
	UINT32 temp																= ARCH_READ_REGISTER_UINT32(Add2Ptr(globalData->BaseRegister, 0x00, UINT32*));
	if(temp == 0xffffffff)
		return FALSE;

	//
	// save operational register, debug port register
	//
	globalData->OperationalRegister											= Add2Ptr(globalData->BaseRegister, temp & 0xff, VOID*);
	globalData->DebugRegister												= BdUsbpLocateDebugPortRegister(globalData, pciIoProtocol);
	if(!globalData->DebugRegister)
		return FALSE;

	//
	// get debug port number
	//
	temp																	= ARCH_READ_REGISTER_UINT32(Add2Ptr(globalData->BaseRegister, 0x04, UINT32*));
	if(temp == 0xffffffff)
		return FALSE;

	//
	// check debug port number
	//
	globalData->DebugPortNumber												= (temp >> 20) & 0xf;
	if(!globalData->DebugPortNumber || globalData->DebugPortNumber > (temp & 0xf))
		return FALSE;

	return BdUsbpInitializeControllerHardware(globalData, pciIoProtocol);
}

//
// control read
//
STATIC BOOLEAN BdUsbpControlRead(DEBUG_USB_GLOBAL_DATA* globalData, UINT8 usbAddress, DEBUG_USB_SETUP_PACKET const* setupPacket, VOID* dataBuffer, UINT32* dataLength)
{
	BOOLEAN retValue														= FALSE;
	__try
	{
		//
		// write data
		//
		UINT32 tempBuffer[2];
		memcpy(tempBuffer, setupPacket, sizeof(tempBuffer));

		//
		// write data buffer
		//
		ARCH_WRITE_REGISTER_UINT32(Add2Ptr(globalData->DebugRegister, 0x08, UINT32*), tempBuffer[0]);
		ARCH_WRITE_REGISTER_UINT32(Add2Ptr(globalData->DebugRegister, 0x0c, UINT32*), tempBuffer[1]);

		//
		// write PIDs (SETUP | DATA0)
		//
		ARCH_WRITE_REGISTER_UINT32(Add2Ptr(globalData->DebugRegister, 0x04, UINT32*), 0xc32d);

		//
		// write device address
		//
		ARCH_WRITE_REGISTER_UINT32(Add2Ptr(globalData->DebugRegister, 0x10, UINT32*), (usbAddress & 0x7f) << 8);

		//
		// read status
		//
		UINT32 temp															= ARCH_READ_REGISTER_UINT32(Add2Ptr(globalData->DebugRegister, 0x00, UINT32*));
		if(temp == 0xffffffff)
			try_leave(NOTHING);

		//
		// WRITE, GO, LENGTH=8, DONE#
		//
		ARCH_WRITE_REGISTER_UINT32(Add2Ptr(globalData->DebugRegister, 0x00, UINT32*), (temp & 0xfffefff8) | 0x38);

		//
		// wait DONE
		//
		while(TRUE)
		{
			//
			// read status
			//
			temp															= ARCH_READ_REGISTER_UINT32(Add2Ptr(globalData->DebugRegister, 0x00, UINT32*));
			if(temp == 0xffffffff)
				try_leave(NOTHING);

			//
			// check DONE
			//
			if(temp & 0x10000)
				break;
		}

		//
		// clear done
		//
		ARCH_WRITE_REGISTER_UINT32(Add2Ptr(globalData->DebugRegister, 0x00, UINT32*), temp);

		//
		// read back
		//
		UINT32 errorCount													= 0;
		UINT32 retry														= 0;
		UINT32 maxRetry														= 1000;
		for(; retry < maxRetry; retry ++)
		{
			//
			// too many errors
			//
			if(errorCount > 3)
				try_leave(NOTHING);

			//
			// write PIDs (IN)
			//
			ARCH_WRITE_REGISTER_UINT32(Add2Ptr(globalData->DebugRegister, 0x04, UINT32*), 0x69);

			//
			// GO, READ, DONE#
			//
			ARCH_WRITE_REGISTER_UINT32(Add2Ptr(globalData->DebugRegister, 0x00, UINT32*), (temp & 0xfffeffef) | 0x20);

			//
			// wait DONE
			//
			while(TRUE)
			{
				//
				// read status
				//
				temp														= ARCH_READ_REGISTER_UINT32(Add2Ptr(globalData->DebugRegister, 0x00, UINT32*));
				if(temp == 0xffffffff)
					try_leave(NOTHING);

				//
				// check DONE
				//
				if(temp & 0x10000)
					break;
			}

			//
			// clear done
			//
			ARCH_WRITE_REGISTER_UINT32(Add2Ptr(globalData->DebugRegister, 0x00, UINT32*), temp);

			//
			// check ERROR
			//
			if(temp & 0x40)
			{
				errorCount													+= 1;
				continue;
			}

			//
			// read PIDs
			//
			UINT32 pid														= ARCH_READ_REGISTER_UINT32(Add2Ptr(globalData->DebugRegister, 0x04, UINT32*));
			if(pid == 0xffffffff)
				try_leave(NOTHING);

			//
			// STALL
			//
			pid																= (pid >> 16) & 0xff;
			if(pid == 0x1e)
				try_leave(NOTHING);

			//
			// DATA1
			//
			if(pid == 0x4b)
				break;
		}

		//
		// timeout
		//
		if(retry == maxRetry)
			try_leave(NOTHING);

		//
		// read data length
		//
		temp																= ARCH_READ_REGISTER_UINT32(Add2Ptr(globalData->DebugRegister, 0x00, UINT32*));
		if(temp == 0xffffffff)
			try_leave(NOTHING);

		//
		// check length
		//
		UINT32 realLength													= temp & 0xf;
		if(realLength < *dataLength)
			*dataLength														= realLength;

		//
		// read data buffer
		//
		tempBuffer[0]														= ARCH_READ_REGISTER_UINT32(Add2Ptr(globalData->DebugRegister, 0x08, UINT32*));
		tempBuffer[1]														= ARCH_READ_REGISTER_UINT32(Add2Ptr(globalData->DebugRegister, 0x0c, UINT32*));
		memcpy(dataBuffer, tempBuffer, *dataLength);

		//
		// ACK
		//
		errorCount															= 0;
		retry																= 0;
		for(; retry < maxRetry; retry ++)
		{
			//
			// too many errors
			//
			if(errorCount > 3)
				try_leave(NOTHING);

			//
			// write PIDs (OUT | DATA1)
			//
			ARCH_WRITE_REGISTER_UINT32(Add2Ptr(globalData->DebugRegister, 0x04, UINT32*), 0x4be1);

			//
			// GO, WRITE, DONE#
			//
			ARCH_WRITE_REGISTER_UINT32(Add2Ptr(globalData->DebugRegister, 0x00, UINT32*), (temp & 0xfffefff0) | 0x30);

			//
			// wait DONE
			//
			while(TRUE)
			{
				//
				// read status
				//
				temp														= ARCH_READ_REGISTER_UINT32(Add2Ptr(globalData->DebugRegister, 0x00, UINT32*));
				if(temp == 0xffffffff)
					try_leave(NOTHING);

				//
				// check DONE
				//
				if(temp & 0x10000)
					break;
			}

			//
			// clear done
			//
			ARCH_WRITE_REGISTER_UINT32(Add2Ptr(globalData->DebugRegister, 0x00, UINT32*), temp);

			//
			// check ERROR
			//
			if(temp & 0x40)
			{
				errorCount													+= 1;
				continue;
			}

			//
			// read PIDs
			//
			UINT32 pid														= ARCH_READ_REGISTER_UINT32(Add2Ptr(globalData->DebugRegister, 0x04, UINT32*));
			if(pid == 0xffffffff)
				try_leave(NOTHING);

			//
			// ACK, NYET
			//
			pid																= (pid >> 16) & 0xff;
			if(pid == 0xd2 || pid == 0x96)
				try_leave(retValue = TRUE);
		}
	}
	__finally
	{

	}
	return retValue;
}

//
// control write
//
STATIC BOOLEAN BdUsbpControlWrite(DEBUG_USB_GLOBAL_DATA* globalData, UINT8 usbAddress, DEBUG_USB_SETUP_PACKET const* setupPacket, VOID* dataBuffer, UINT32 dataLength)
{
	BOOLEAN retValue														= FALSE;
	__try
	{
		//
		// write data
		//
		UINT32 tempBuffer[2];
		memcpy(tempBuffer, setupPacket, sizeof(tempBuffer));

		//
		// write data buffer
		//
		ARCH_WRITE_REGISTER_UINT32(Add2Ptr(globalData->DebugRegister, 0x08, UINT32*), tempBuffer[0]);
		ARCH_WRITE_REGISTER_UINT32(Add2Ptr(globalData->DebugRegister, 0x0c, UINT32*), tempBuffer[1]);

		//
		// write PIDs (SETUP | DATA0)
		//
		ARCH_WRITE_REGISTER_UINT32(Add2Ptr(globalData->DebugRegister, 0x04, UINT32*), 0xc32d);

		//
		// write device address
		//
		ARCH_WRITE_REGISTER_UINT32(Add2Ptr(globalData->DebugRegister, 0x10, UINT32*), (usbAddress & 0x7f) << 8);

		//
		// read status
		//
		UINT32 temp															= ARCH_READ_REGISTER_UINT32(Add2Ptr(globalData->DebugRegister, 0x00, UINT32*));
		if(temp == 0xffffffff)
			try_leave(NOTHING);

		//
		// WRITE, GO, LENGTH=8, DONE#
		//
		ARCH_WRITE_REGISTER_UINT32(Add2Ptr(globalData->DebugRegister, 0x00, UINT32*), (temp & 0xfffefff8) | 0x38);

		//
		// wait DONE
		//
		while(TRUE)
		{
			//
			// read status
			//
			temp															= ARCH_READ_REGISTER_UINT32(Add2Ptr(globalData->DebugRegister, 0x00, UINT32*));
			if(temp == 0xffffffff)
				try_leave(NOTHING);

			//
			// check DONE
			//
			if(temp & 0x10000)
				break;
		}

		//
		// clear done
		//
		ARCH_WRITE_REGISTER_UINT32(Add2Ptr(globalData->DebugRegister, 0x00, UINT32*), temp);

		//
		// write data buffer
		//
		UINT32 errorCount													= 0;
		UINT32 retry														= 0;
		UINT32 maxRetry														= 1000;
		if(dataLength)
		{
			memcpy(tempBuffer, dataBuffer, dataLength);
			for(; retry < maxRetry; retry ++)
			{
				//
				// too many errors
				//
				if(errorCount > 3)
					try_leave(NOTHING);

				//
				// write data buffer
				//
				ARCH_WRITE_REGISTER_UINT32(Add2Ptr(globalData->DebugRegister, 0x08, UINT32*), tempBuffer[0]);
				ARCH_WRITE_REGISTER_UINT32(Add2Ptr(globalData->DebugRegister, 0x0c, UINT32*), tempBuffer[1]);

				//
				// write PIDs (OUT DATA1)
				//
				ARCH_WRITE_REGISTER_UINT32(Add2Ptr(globalData->DebugRegister, 0x04, UINT32*), 0x4be1);

				//
				// GO, WRITE, DONE#
				//
				ARCH_WRITE_REGISTER_UINT32(Add2Ptr(globalData->DebugRegister, 0x00, UINT32*), (temp & 0xfffefff0) | 0x30 | (dataLength & 0xf));

				//
				// wait DONE
				//
				while(TRUE)
				{
					//
					// read status
					//
					temp													= ARCH_READ_REGISTER_UINT32(Add2Ptr(globalData->DebugRegister, 0x00, UINT32*));
					if(temp == 0xffffffff)
						try_leave(NOTHING);

					//
					// check DONE
					//
					if(temp & 0x10000)
						break;
				}

				//
				// clear done
				//
				ARCH_WRITE_REGISTER_UINT32(Add2Ptr(globalData->DebugRegister, 0x00, UINT32*), temp);

				//
				// check ERROR
				//
				if(temp & 0x40)
				{
					errorCount												+= 1;
					continue;
				}

				//
				// read PIDs
				//
				UINT32 pid													= ARCH_READ_REGISTER_UINT32(Add2Ptr(globalData->DebugRegister, 0x04, UINT32*));
				if(pid == 0xffffffff)
					try_leave(NOTHING);

				//
				// STALL
				//
				pid															= (pid >> 16) & 0xff;
				if(pid == 0x1e)
					try_leave(NOTHING);

				//
				// ACK, NYET
				//
				if(pid == 0xd2 || pid == 0x96)
					break;
			}

			//
			// timeout
			//
			if(retry == maxRetry)
				try_leave(NOTHING);
		}

		//
		// IN
		//
		errorCount															= 0;
		retry																= 0;
		for(; retry < maxRetry; retry ++)
		{
			//
			// too many errors
			//
			if(errorCount > 3)
				try_leave(NOTHING);

			//
			// write PIDs (IN)
			//
			ARCH_WRITE_REGISTER_UINT32(Add2Ptr(globalData->DebugRegister, 0x04, UINT32*), 0x69);

			//
			// GO, READ, DONE#
			//
			ARCH_WRITE_REGISTER_UINT32(Add2Ptr(globalData->DebugRegister, 0x00, UINT32*), (temp & 0xfffeffef) | 0x20);

			//
			// wait DONE
			//
			while(TRUE)
			{
				//
				// read status
				//
				temp														= ARCH_READ_REGISTER_UINT32(Add2Ptr(globalData->DebugRegister, 0x00, UINT32*));
				if(temp == 0xffffffff)
					try_leave(NOTHING);

				//
				// check DONE
				//
				if(temp & 0x10000)
					break;
			}

			//
			// clear done
			//
			ARCH_WRITE_REGISTER_UINT32(Add2Ptr(globalData->DebugRegister, 0x00, UINT32*), temp);

			//
			// check ERROR
			//
			if(temp & 0x40)
			{
				errorCount													+= 1;
				continue;
			}

			//
			// read PIDs
			//
			UINT32 pid														= ARCH_READ_REGISTER_UINT32(Add2Ptr(globalData->DebugRegister, 0x04, UINT32*));
			if(pid == 0xffffffff)
				try_leave(NOTHING);

			//
			// STALL
			//
			pid																= (pid >> 16) & 0xff;
			if(pid == 0x1e)
				try_leave(NOTHING);

			//
			// DATA1
			//
			if(pid == 0x4b)
			{
				//
				// read length
				//
				temp														= ARCH_READ_REGISTER_UINT32(Add2Ptr(globalData->DebugRegister, 0x00, UINT32*));
				if(temp == 0xffffffff)
					try_leave(NOTHING);

				//
				// check length
				//
				try_leave(retValue = (temp & 0xf) ? FALSE : TRUE);
			}
		}
	}
	__finally
	{

	}
	return retValue;
}

//
// try initialize device
//
STATIC BOOLEAN BdUsbpTrySetupDevice(DEBUG_USB_GLOBAL_DATA* globalData)
{
	//
	// DATA0
	//
	globalData->NextReceiveDataPid											= 0xc3;
	globalData->NextSendDataPid												= 0xc3;
	for(UINT32 retry = 0; retry < 3; retry ++)
	{
		//
		// get debug descriptor
		//
		DEBUG_USB_SETUP_PACKET setupPacket;
		UINT8 debugDescriptor[4]											= {0};
		setupPacket.RequestType												= 0x80;
		setupPacket.Request													= 6;
		setupPacket.Value													= 10 << 8;
		setupPacket.Index													= 0;
		setupPacket.Length													= sizeof(debugDescriptor);
		UINT32 length														= setupPacket.Length;
		if(!BdUsbpControlRead(globalData, 0, &setupPacket, debugDescriptor, &length) || length != sizeof(debugDescriptor))
			continue;

		//
		// setup endpoint, usb address
		//
		globalData->InputEndpoint											= debugDescriptor[2];
		globalData->OutputEndpoint											= debugDescriptor[3];
		globalData->UsbAddress												= 0x7f;
		setupPacket.RequestType												= 0;
		setupPacket.Request													= 5;
		setupPacket.Value													= globalData->UsbAddress;
		setupPacket.Index													= 0;
		setupPacket.Length													= 0;
		if(!BdUsbpControlWrite(globalData, 0, &setupPacket, nullptr, 0))
			return FALSE;

		//
		// set debug mode
		//
		setupPacket.RequestType												= 0;
		setupPacket.Request													= 3;
		setupPacket.Value													= 6;
		setupPacket.Index													= 0;
		setupPacket.Length													= 0;
		return BdUsbpControlWrite(globalData, globalData->UsbAddress, &setupPacket, nullptr, 0);
	}
	return FALSE;
}

//
// try reinitialize device
//
STATIC BOOLEAN BdUsbpTryInitializeDevice(DEBUG_USB_GLOBAL_DATA* globalData, BOOLEAN haltHostController, BOOLEAN* devicePresent)
{
	BOOLEAN retValue														= FALSE;
	__try
	{
		//
		// read PORTSC
		//
		UINT32 temp															= ARCH_READ_REGISTER_UINT32(Add2Ptr(globalData->OperationalRegister, 0x40 + globalData->DebugPortNumber * 4, UINT32*));
		if(temp == 0xffffffff)
			try_leave(NOTHING);

		//
		// device not present
		//
		if(!(temp & 1))
			try_leave(*devicePresent = FALSE; if(haltHostController) BdUsbpHaltController(globalData));

		//
		// port reset (it must also write a zero to Port Enable bit)
		//
		*devicePresent														= TRUE;
		ARCH_WRITE_REGISTER_UINT32(Add2Ptr(globalData->OperationalRegister, 0x40 + globalData->DebugPortNumber * 4, UINT32*), (temp & 0xfffffffb) | 0x100);

		//
		// wait reset
		//
		if(!BdUsbpWait(globalData, 20))
			try_leave(NOTHING);

		//
		// read PORTSC
		//
		temp																= ARCH_READ_REGISTER_UINT32(Add2Ptr(globalData->OperationalRegister, 0x40 + globalData->DebugPortNumber * 4, UINT32*));
		if(temp == 0xffffffff)
			try_leave(NOTHING);

		//
		// complete reset
		//
		ARCH_WRITE_REGISTER_UINT32(Add2Ptr(globalData->OperationalRegister, 0x40 + globalData->DebugPortNumber * 4, UINT32*), temp & 0xfffffeff);

		//
		// wait reset complete
		//
		if(!BdUsbpWait(globalData, 5))
			try_leave(NOTHING);

		//
		// read PORTSC
		//
		temp																= ARCH_READ_REGISTER_UINT32(Add2Ptr(globalData->OperationalRegister, 0x40 + globalData->DebugPortNumber * 4, UINT32*));
		if(temp == 0xffffffff)
			try_leave(NOTHING);

		//
		// check device present, port enabled
		//
		if((temp & 5) == 5)
		{
			//
			// read debug status
			//
			temp															= ARCH_READ_REGISTER_UINT32(Add2Ptr(globalData->DebugRegister, 0, UINT32*));
			if(temp == 0xffffffff)
				try_leave(NOTHING);

			//
			// enable debug port
			//
			ARCH_WRITE_REGISTER_UINT32(Add2Ptr(globalData->DebugRegister, 0x00, UINT32*), temp | 0x10000000);

			//
			// read PORTSC
			//
			temp															= ARCH_READ_REGISTER_UINT32(Add2Ptr(globalData->OperationalRegister, 0x40 + globalData->DebugPortNumber * 4, UINT32*));
			if(temp == 0xffffffff)
				try_leave(NOTHING);

			//
			// disable port
			//
			ARCH_WRITE_REGISTER_UINT32(Add2Ptr(globalData->OperationalRegister, 0x40 + globalData->DebugPortNumber * 4, UINT32*), temp & 0xfffffffb);
		}

		//
		// halt host controller
		//
		if(haltHostController && !BdUsbpHaltController(globalData))
			try_leave(NOTHING);

		//
		// read debug status
		//
		temp																= ARCH_READ_REGISTER_UINT32(Add2Ptr(globalData->DebugRegister, 0, UINT32*));
		if(temp == 0xffffffff)
			try_leave(NOTHING);

		//
		// check enabled
		//
		if(!(temp & 0x10000000))
			try_leave(NOTHING);

		//
		// setup device
		//
		if(!BdUsbpTrySetupDevice(globalData))
			try_leave(NOTHING);

		//
		// set in-use
		//
		retValue															= TRUE;
		temp																= ARCH_READ_REGISTER_UINT32(Add2Ptr(globalData->DebugRegister, 0, UINT32*));
		ARCH_WRITE_REGISTER_UINT32(Add2Ptr(globalData->DebugRegister, 0x00, UINT32*), temp | 0x400);
	}
	__finally
	{

	}
	return retValue;
}

//
// reinitialize device
//
STATIC BOOLEAN BdUsbpReinitializeDevice(DEBUG_USB_GLOBAL_DATA* globalData)
{
	BOOLEAN devicePresent													= FALSE;
	BOOLEAN retValue														= FALSE;
	__try
	{
		//
		// read USBCMD
		//
		UINT32 temp															= ARCH_READ_REGISTER_UINT32(Add2Ptr(globalData->OperationalRegister, 0x00, UINT32*));
		if(temp == 0xffffffff)
			try_leave(NOTHING);

		//
		// check stopped
		//
		if(temp & 1)
		{
			//
			// read PORTSC
			//
			temp															= ARCH_READ_REGISTER_UINT32(Add2Ptr(globalData->OperationalRegister, 0x40 + globalData->DebugPortNumber * 4, UINT32*));

			//
			// read status
			//
			temp															= ARCH_READ_REGISTER_UINT32(Add2Ptr(globalData->DebugRegister, 0x00, UINT32*));
			if(temp == 0xffffffff)
				try_leave(NOTHING);

			//
			// Owner = 0, Enabled = 0
			//
			temp															&= 0xafffffff;
			ARCH_WRITE_REGISTER_UINT32(Add2Ptr(globalData->DebugRegister, 0x00, UINT32*), temp);

			//
			// Owner = 1
			//
			ARCH_WRITE_REGISTER_UINT32(Add2Ptr(globalData->DebugRegister, 0x00, UINT32*), temp | 0x40000000);

			//
			// read PORTSC
			//
			temp															= ARCH_READ_REGISTER_UINT32(Add2Ptr(globalData->OperationalRegister, 0x40 + globalData->DebugPortNumber * 4, UINT32*));

			//
			// check connect status
			//
			if(!(temp & 1))
				try_leave(NOTHING);

			//
			// try initialize device
			//
			retValue														= BdUsbpTryInitializeDevice(globalData, FALSE, &devicePresent);
		}
		else
		{
			//
			// disable schedule (Asynchronous, Periodic), Run
			//
			ARCH_WRITE_REGISTER_UINT32(Add2Ptr(globalData->OperationalRegister, 0x00, UINT32*), (temp & 0xffffffcf) | 0x1);

			//
			// try initialize device
			//
			retValue														= BdUsbpTryInitializeDevice(globalData, TRUE, &devicePresent);

			//
			// restore USBCMD
			//
			ARCH_WRITE_REGISTER_UINT32(Add2Ptr(globalData->OperationalRegister, 0x00, UINT32*), temp);
		}
	}
	__finally
	{

	}
	return retValue;
}

//
// read packet once
//
STATIC UINT32 BdUsbpReadPacketOnce(DEBUG_USB_GLOBAL_DATA* globalData, VOID* dataBuffer, UINT32* dataLength)
{
	UINT32 retValue															= KDP_PACKET_RESEND;

	__try
	{
		//
		// write device address
		//
		UINT32 temp															= ((globalData->UsbAddress & 0x7f) << 8) | (globalData->InputEndpoint & 0xf);
		ARCH_WRITE_REGISTER_UINT32(Add2Ptr(globalData->DebugRegister, 0x10, UINT32*), temp);

		for(UINT32 retry = 0; retry < 3; retry ++)
		{
			//
			// write PIDs (token = IN)
			//
			ARCH_WRITE_REGISTER_UINT32(Add2Ptr(globalData->DebugRegister, 0x04, UINT32*), 0x69);

			//
			// read status
			//
			temp															= ARCH_READ_REGISTER_UINT32(Add2Ptr(globalData->DebugRegister, 0x00, UINT32*));
			if(temp == 0xffffffff)
				try_leave(NOTHING);

			//
			// Read#, GO, Done#
			//
			ARCH_WRITE_REGISTER_UINT32(Add2Ptr(globalData->DebugRegister, 0x00, UINT32*), (temp & 0xfffeffef) | 0x20);

			//
			// wait Done
			//
			UINT32 endTime													= BlGetCurrentUnixTime() + 50;
			while(BlGetCurrentUnixTime() < endTime)
			{
				//
				// read status
				//
				temp														= ARCH_READ_REGISTER_UINT32(Add2Ptr(globalData->DebugRegister, 0x00, UINT32*));
				if(temp == 0xffffffff)
					try_leave(NOTHING);

				//
				// check done
				//
				if(temp & 0x10000)
					break;
			}

			//
			// timeout, reinitialize device
			//
			if(!(temp & 0x10000))
				try_leave(CsPrintf(CHAR8_CONST_STRING("[READ] timeout\n")); BdUsbpReinitializeDevice(globalData));

			//
			// clear Done
			//
			ARCH_WRITE_REGISTER_UINT32(Add2Ptr(globalData->DebugRegister, 0x00, UINT32*), temp);

			//
			// error, retry
			//
			if(temp & 0x40)
				continue;

			//
			// read PIDs
			//
			temp															= ARCH_READ_REGISTER_UINT32(Add2Ptr(globalData->DebugRegister, 0x04, UINT32*));
			if(temp == 0xffffffff)
				try_leave(NOTHING);

			//
			// check received PID, NAK
			//
			temp															= (temp >> 16) & 0xff;
			if(temp == 0x5a)
				try_leave(retValue = KDP_PACKET_TIMEOUT);

			//
			// STALL
			//
			if(temp == 0x1e)
				try_leave(CsPrintf(CHAR8_CONST_STRING("[READ] STALL\n")));

			//
			// not the one, retry
			//
			if(temp != globalData->NextReceiveDataPid)
			{
				CsPrintf(CHAR8_CONST_STRING("[READ] invalid pid %08x, %08x, 0x08%\n"), temp, globalData->NextReceiveDataPid, retry);
				continue;
			}

			//
			// setup next PID (DATA0 <-> DATA1)
			//
			globalData->NextReceiveDataPid									= temp == 0xc3 ? 0x4b : 0xc3;

			//
			// read data length
			//
			temp															= ARCH_READ_REGISTER_UINT32(Add2Ptr(globalData->DebugRegister, 0x00, UINT32*));
			if(temp == 0xffffffff)
				try_leave(NOTHING);

			//
			// read data buffer
			//
			*dataLength														= temp & 0x0f;
			if(*dataLength)
			{
				UINT32 data0												= ARCH_READ_REGISTER_UINT32(Add2Ptr(globalData->DebugRegister, 0x08, UINT32*));
				UINT32 data1												= ARCH_READ_REGISTER_UINT32(Add2Ptr(globalData->DebugRegister, 0x0c, UINT32*));
				memcpy(dataBuffer, &data0, sizeof(data0));
				memcpy(Add2Ptr(dataBuffer, sizeof(data0), VOID*), &data1, sizeof(data1));
			}
			try_leave(retValue = KDP_PACKET_RECEIVED);
		}
		CsPrintf(CHAR8_CONST_STRING("[READ] max retry 0x%08x\n"), temp);
	}
	__finally
	{
	}
	return retValue;
}

//
// send packet once
//
STATIC UINT32 BdUsbpSendPacketOnce(DEBUG_USB_GLOBAL_DATA* globalData, VOID CONST* dataBuffer, UINT32 dataLength)
{
	UINT32 retValue															= KDP_PACKET_RESEND;
	__try
	{
		//
		// write device address
		//
		UINT32 temp															= ((globalData->UsbAddress & 0x7f) << 8) | (globalData->OutputEndpoint & 0xf);
		ARCH_WRITE_REGISTER_UINT32(Add2Ptr(globalData->DebugRegister, 0x10, UINT32*), temp);

		//
		// align
		//
		UINT32 tempBuffer[2];
		memcpy(tempBuffer, dataBuffer, sizeof(tempBuffer));
		for(UINT32 retry = 0; retry < 3; retry ++)
		{
			//
			// write data buffer
			//
			ARCH_WRITE_REGISTER_UINT32(Add2Ptr(globalData->DebugRegister, 0x08, UINT32*), tempBuffer[0]);
			ARCH_WRITE_REGISTER_UINT32(Add2Ptr(globalData->DebugRegister, 0x0c, UINT32*), tempBuffer[1]);

			//
			// write PIDs, OUT
			//
			ARCH_WRITE_REGISTER_UINT32(Add2Ptr(globalData->DebugRegister, 0x04, UINT32*), (globalData->NextSendDataPid << 8) | 0xe1);

			//
			// read status
			//
			temp															= ARCH_READ_REGISTER_UINT32(Add2Ptr(globalData->DebugRegister, 0x00, UINT32*));
			if(temp == 0xffffffff)
				try_leave(NOTHING);

			//
			// Write, Go, DataLength, Done#
			//
			ARCH_WRITE_REGISTER_UINT32(Add2Ptr(globalData->DebugRegister, 0x00, UINT32*), (temp & 0xfffefff0) | (dataLength & 0xf) | 0x30);

			//
			// wait Done
			//
			UINT32 endTime													= BlGetCurrentUnixTime() + 50;
			while(BlGetCurrentUnixTime() < endTime)
			{
				//
				// read status
				//
				temp														= ARCH_READ_REGISTER_UINT32(Add2Ptr(globalData->DebugRegister, 0x00, UINT32*));
				if(temp == 0xffffffff)
					try_leave(NOTHING);

				//
				// check done
				//
				if(temp & 0x10000)
					break;
			}

			//
			// timeout
			//
			if(!(temp & 0x10000))
				try_leave(NOTHING);

			//
			// clear Done
			//
			ARCH_WRITE_REGISTER_UINT32(Add2Ptr(globalData->DebugRegister, 0x00, UINT32*), temp);

			//
			// error, retry
			//
			if(temp & 0x40)
				continue;

			//
			// read PIDs
			//
			temp															= ARCH_READ_REGISTER_UINT32(Add2Ptr(globalData->DebugRegister, 0x04, UINT32*));
			if(temp == 0xffffffff)
				try_leave(NOTHING);

			//
			// check received PID, NAK
			//
			temp															= (temp >> 16) & 0xff;
			if(temp == 0x5a)
				try_leave(retValue = KDP_PACKET_TIMEOUT);

			//
			// STALL
			//
			if(temp == 0x1e)
				try_leave(NOTHING);

			//
			// ACK, NYET
			//
			if(temp != 0xd2 && temp != 0x96)
				continue;

			//
			// set next send pid
			//
			retValue														= KDP_PACKET_RECEIVED;
			globalData->NextSendDataPid										= globalData->NextSendDataPid == 0xc3 ? 0x4b : 0xc3;
			break;
		}
	}
	__finally
	{
	}
	return retValue;
}

//
// read packet
//
STATIC UINT32 BdUsbpReadPacket(DEBUG_USB_GLOBAL_DATA* globalData)
{
	//
	// continue read
	//
	if(!globalData->FullPacketReceived)
	{
		//
		// read to cache buffer
		//
		UINT32 dataLength													= 0;
		UINT32 result														= BdUsbpReadPacketOnce(globalData, Add2Ptr(&globalData->PacketHeader, globalData->DataBufferOffset, VOID*), &dataLength);
		if(result != KDP_PACKET_RECEIVED)
			return result;

		//
		// finished if we met a small packet
		//
		globalData->DataBufferOffset										+= dataLength;
		if(dataLength < 8)
			globalData->FullPacketReceived									= TRUE;
	}
	return KDP_PACKET_RECEIVED;
}

//
// send packet
//
STATIC UINT32 BdUsbpSendPacket(DEBUG_USB_GLOBAL_DATA* globalData, VOID CONST* dataBuffer, UINT32 dataLength)
{
	UINT32 retValue															= KDP_PACKET_TIMEOUT;
	for(UINT32 retry = 0; retry < 100000; retry ++)
	{
		//
		// retry if NOT_READY
		//
		BdUsbpReadPacket(globalData);
		retValue															= BdUsbpSendPacketOnce(globalData, dataBuffer, dataLength);
		if(retValue == KDP_PACKET_RESEND || retValue == KDP_PACKET_RECEIVED)
			break;
	}
	return retValue;
}

//
// check reinitialize device
//
STATIC BOOLEAN BdUsbpCheckReinitializeDevice(DEBUG_USB_GLOBAL_DATA* globalData)
{
	if(!globalData->NeedReinitializeDevice)
		return TRUE;
	
	BOOLEAN retValue														= BdUsbpReinitializeDevice(globalData);
	CsPrintf(CHAR8_CONST_STRING("reinitialize device %08x\n"), retValue);
	if(retValue)
		globalData->NeedReinitializeDevice									= FALSE;
	return retValue;
}

//
// send buffers
//
STATIC VOID BdUsbpSendBuffers(DEBUG_USB_GLOBAL_DATA* globalData, DEBUG_USB_SEND_BUFFER CONST* sendBuffer, UINT32 bufferCount)
{
	if(!BdUsbpCheckReinitializeDevice(globalData))
		return;

	UINT8 dataBuffer[8]														= {0};
	UINT32 dataLength														= 0;
	UINT32 result															= KDP_PACKET_RECEIVED;
	BOOLEAN sendTerminalPacket												= FALSE;
	for(UINT32 i = 0; i < bufferCount; i ++)
	{
		for(UINT32 j = 0; j < sendBuffer[i].Length; j ++)
		{
			dataBuffer[dataLength ++]										= *Add2Ptr(sendBuffer[i].Buffer, j, UINT8*);
			if(dataLength >= ARRAYSIZE(dataBuffer))
			{
				sendTerminalPacket											= TRUE;
				result														= BdUsbpSendPacket(globalData, dataBuffer, sizeof(dataBuffer));
				dataLength													= 0;
			}
		}
	}

	if(sendTerminalPacket || dataLength)
		result																= BdUsbpSendPacket(globalData, dataBuffer, dataLength);

	if(result == KDP_PACKET_RESEND)
		globalData->NeedReinitializeDevice									= TRUE;
}

//
// send control packet
//
STATIC VOID BdUsbpSendControlPacket(DEBUG_USB_GLOBAL_DATA* globalData, UINT32 packetType, UINT32 packetId)
{
	KD_PACKET packetHeader;
	DEBUG_USB_SEND_BUFFER sendBuffer;
	sendBuffer.Buffer														= &packetHeader;
	sendBuffer.Length														= sizeof(packetHeader);
	packetHeader.ByteCount													= 0;
	packetHeader.Checksum													= 0;
	packetHeader.PacketId													= packetId;
	packetHeader.PacketLeader												= CONTROL_PACKET_LEADER;
	packetHeader.PacketType													= static_cast<UINT16>(packetType);
	//CsPrintf(CHAR8_CONST_STRING("[SEND] CONTROL %08x %08x, %08x, %08x\n"), packetHeader.PacketType, packetHeader.PacketId, packetHeader.ByteCount, packetHeader.Checksum);
	BdUsbpSendBuffers(globalData, &sendBuffer, 1);
}

//
// clear and wait connect change
//
BOOLEAN BdUsbpClearAndWaitConnectChange(DEBUG_USB_GLOBAL_DATA* globalData, UINT32 portNumber)
{
	//
	// read PORTSC
	//
	UINT32 temp																= ARCH_READ_REGISTER_UINT32(Add2Ptr(globalData->OperationalRegister, 0x40 + portNumber * 4, UINT32*));
	BOOLEAN retValue														= temp != 0xffffffff;
	CsPrintf(CHAR8_CONST_STRING("waiting debug device ...(0x%08x -> "), temp);
	if(retValue)
	{
		//
		// clear connect change
		//
		ARCH_WRITE_REGISTER_UINT32(Add2Ptr(globalData->OperationalRegister, 0x40 + portNumber * 4, UINT32*), temp | 2);

		do
		{
			//
			// wait 500ms
			//
			BdUsbpWait(globalData, 500);

			//
			// read again
			//
			temp															= ARCH_READ_REGISTER_UINT32(Add2Ptr(globalData->OperationalRegister, 0x40 + portNumber * 4, UINT32*));
			if(temp == 0xffffffff)
				retValue													= FALSE;

		}while((temp & 3) != 3);
	}

	CsPrintf(CHAR8_CONST_STRING("0x%08x) [DONE]\n"), temp);
	return retValue;
}

//
// initialize device
//
STATIC BOOLEAN BdUsbpInitializeDevice(DEBUG_USB_GLOBAL_DATA* globalData, BOOLEAN waitForever)
{
	if(waitForever && !BdUsbpClearAndWaitConnectChange(globalData, globalData->DebugPortNumber))
		return FALSE;

	BOOLEAN result															= FALSE;
	BOOLEAN devicePresent													= FALSE;
	for(UINT32 retry = 0; waitForever || retry < 20; retry ++)
	{
		result																= BdUsbpTryInitializeDevice(globalData, FALSE, &devicePresent);
		if(result)
			break;

		if(!BdUsbpWait(globalData, 500))
			return FALSE;

		if(!waitForever && !devicePresent && retry > 5)
			break;
	}

	if(!BdUsbpHaltController(globalData))
		return FALSE;

	if(!result)
	{
		BdDebuggerNotPresent												= TRUE;
		globalData->NeedReinitializeDevice									= TRUE;
	}

	return TRUE;
}

//
// send packet
//
VOID BdUsbSendPacket(UINT32 packetType, STRING* messageHeader, STRING* messageData)
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
	packetHeader.PacketLeader												= PACKET_LEADER;

	//
	// set retry count
	//
	BdNumberRetries															= BdRetryCount;
	while(TRUE)
	{
		//
		// check timeout
		//
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
				(packetType == PACKET_TYPE_KD_TRACE_IO && fileIo->ApiNumber == DbgKdPrintTraceApi) ||
				(packetType == PACKET_TYPE_KD_FILE_IO && fileIo->ApiNumber == DbgKdCreateFileApi))
			{
				BdDebuggerNotPresent										= TRUE;
				BdNextPacketIdToSend										= INITIAL_PACKET_ID | SYNC_PACKET_ID;
				BdPacketIdExpected											= INITIAL_PACKET_ID;
				return;
			}
		}

		//
		// send buffer
		//
		DEBUG_USB_SEND_BUFFER sendBuffer[3];
		UINT32 bufferCount													= 2;
		packetHeader.PacketId												= BdNextPacketIdToSend;
		sendBuffer[0].Buffer												= &packetHeader;
		sendBuffer[0].Length												= sizeof(packetHeader);
		sendBuffer[1].Buffer												= messageHeader->Buffer;
		sendBuffer[1].Length												= messageHeader->Length;
		if(messageData)
		{
			bufferCount														= 3;
			sendBuffer[2].Buffer											= messageData->Buffer;
			sendBuffer[2].Length											= messageData->Length;
		}
		//CsPrintf(CHAR8_CONST_STRING("[SEND] %08x %08x, %08x, %08x\n"), packetHeader.PacketType, packetHeader.PacketId, packetHeader.ByteCount, packetHeader.Checksum);
		BdUsbpSendBuffers(BdUsbpGlobalData, sendBuffer, bufferCount);

		//
		// receive ACK
		//
		UINT32 result														= BdUsbReceivePacket(PACKET_TYPE_KD_ACKNOWLEDGE, nullptr, nullptr, nullptr);
		if(result == KDP_PACKET_TIMEOUT)
			BdNumberRetries													-= 1;

		//
		// received
		//
		if(result == KDP_PACKET_RECEIVED)
		{
			BdNextPacketIdToSend											&= ~SYNC_PACKET_ID;
			BdRetryCount													= 0x14;
			break;
		}
	}
}

//
// read debugger packet
//
STATIC UINT32 BdUsbpReadDebuggerPacket(DEBUG_USB_GLOBAL_DATA* globalData, UINT32 packetType, KD_PACKET* packetHeader, STRING* messageHeader, STRING* messageData)
{
	UINT32 result															= KDP_PACKET_RECEIVED;
	__try
	{
		//
		// check reinitialize device
		//
		if(!BdUsbpCheckReinitializeDevice(globalData))
			try_leave(result = KDP_PACKET_TIMEOUT);

		//
		// receive data
		//
		while(TRUE)
		{
			for(UINT32 retry = 0; retry < 100000 && !globalData->FullPacketReceived; retry ++)
			{
				UINT32 dataLength											= 0;
				result														= BdUsbpReadPacketOnce(globalData, Add2Ptr(&globalData->PacketHeader, globalData->DataBufferOffset, VOID*), &dataLength);
				if(result == KDP_PACKET_RESEND)
					break;

				if(result != KDP_PACKET_RECEIVED)
				{
					if(packetType == PACKET_TYPE_KD_POLL_BREAKIN && !globalData->DataBufferOffset)
						break;
				}
				else
				{
					globalData->DataBufferOffset							+= dataLength;
					if(dataLength < 8)
						globalData->FullPacketReceived						= TRUE;
					else if(globalData->DataBufferOffset == sizeof(globalData->PacketHeader) + sizeof(globalData->DataBuffer))
						globalData->DataBufferOffset						= 0;
				}
			}

			//
			// reset buffer offset
			//
			UINT32 receiveLength											= 0;
			if(globalData->FullPacketReceived)
			{
				receiveLength												= globalData->DataBufferOffset;
				globalData->DataBufferOffset								= 0;
				globalData->FullPacketReceived								= FALSE;
				result														= KDP_PACKET_RECEIVED;
			}

			//
			// check status
			//
			if(result != KDP_PACKET_RECEIVED)
				try_leave(if(result == KDP_PACKET_RESEND) globalData->NeedReinitializeDevice = TRUE);

			//
			// check NAME?
			//
			if(receiveLength == 5 && !memcmp(&globalData->PacketHeader, "NAME?", 5))
			{
				DEBUG_USB_SEND_BUFFER sendBuffer[2];
				sendBuffer[0].Buffer										= "NAME=";
				sendBuffer[0].Length										= 5;
				sendBuffer[1].Buffer										= globalData->TargetName;
				sendBuffer[1].Length										= globalData->NameLength + 1;
				BdUsbpSendBuffers(globalData, sendBuffer, ARRAYSIZE(sendBuffer));
				continue;
			}

			//
			// poll breakin
			//
			if(packetType == PACKET_TYPE_KD_POLL_BREAKIN)
				try_leave(result = receiveLength && (globalData->PacketHeader.PacketLeader & 0xff) == BREAKIN_PACKET_BYTE ? KDP_PACKET_RECEIVED : KDP_PACKET_TIMEOUT);

			//
			// check packet header length
			//
			if(receiveLength < sizeof(globalData->PacketHeader))
				try_leave(CsPrintf(CHAR8_CONST_STRING("[RECV] small length %08x\n"), receiveLength); result = KDP_PACKET_RESEND);

			//
			// copy packet header
			//
			memcpy(packetHeader, &globalData->PacketHeader, sizeof(globalData->PacketHeader));

			//
			// copy message header
			//
			if(messageHeader)
			{
				memcpy(messageHeader->Buffer, globalData->DataBuffer, messageHeader->MaximumLength);
				if(messageHeader->MaximumLength + sizeof(globalData->PacketHeader) < receiveLength && messageData)
				{
					UINT32 leftLength										= receiveLength - sizeof(globalData->PacketHeader) - messageHeader->MaximumLength;
					if(leftLength > messageData->MaximumLength)
						leftLength											= messageData->MaximumLength;
					memcpy(messageData->Buffer, globalData->DataBuffer + messageHeader->MaximumLength, leftLength);
				}
			}
			break;
		}
	}
	__finally
	{

	}
	return result;
}

//
// receive packet
//
UINT32 BdUsbReceivePacket(UINT32 packetType, STRING* messageHeader, STRING* messageData, UINT32* dataLength)
{
	KD_PACKET packetHeader;
	UINT32 result															= BdUsbpReadDebuggerPacket(BdUsbpGlobalData, packetType, &packetHeader, messageHeader, messageData);
	if(packetType == PACKET_TYPE_KD_POLL_BREAKIN)
	{
		if(result != KDP_PACKET_RECEIVED)
			return KDP_PACKET_TIMEOUT;

		BdDebuggerNotPresent												= FALSE;
		return KDP_PACKET_RECEIVED;
	}

	for(; TRUE; result = BdUsbpReadDebuggerPacket(BdUsbpGlobalData, packetType, &packetHeader, messageHeader, messageData))
	{
		if(result != KDP_PACKET_TIMEOUT)
			BdNumberRetries													= BdRetryCount;

		if(result != KDP_PACKET_RECEIVED)
		{
			if(result != KDP_PACKET_TIMEOUT)
				CsPrintf(CHAR8_CONST_STRING("[RECV] resend\n"));

			if((packetHeader.PacketLeader & 0xff) != BREAKIN_PACKET_BYTE)
				return result;

			BdControlCPending												= TRUE;
			return KDP_PACKET_RESEND;
		}

		if(packetHeader.PacketLeader == CONTROL_PACKET_LEADER)
		{
			if(packetHeader.PacketType == PACKET_TYPE_KD_RESEND)
			{
				CsPrintf(CHAR8_CONST_STRING("[RECV] control resend\n"));
				return KDP_PACKET_RESEND;
			}
			else if(packetHeader.PacketType == PACKET_TYPE_KD_ACKNOWLEDGE)
			{
				if(packetHeader.PacketId == (BdNextPacketIdToSend & ~SYNC_PACKET_ID) && packetType == PACKET_TYPE_KD_ACKNOWLEDGE)
				{
					CsPrintf(CHAR8_CONST_STRING("[RECV] ACK %08x %08x, %08x, %08x\n"), packetHeader.PacketType, packetHeader.PacketId, packetHeader.ByteCount, packetHeader.Checksum);
					BdNextPacketIdToSend									^= 1;
					return KDP_PACKET_RECEIVED;
				}
				else
				{
					CsPrintf(CHAR8_CONST_STRING("[RECV] control ack %08x, next to send %08x, wait %08x\n"), packetHeader.PacketId, BdNextPacketIdToSend, packetType);
					continue;
				}
			}
			else if(packetHeader.PacketType == PACKET_TYPE_KD_RESET)
			{
				CsPrintf(CHAR8_CONST_STRING("[RECV] control reset\n"));

				BdNextPacketIdToSend										= INITIAL_PACKET_ID;
				BdPacketIdExpected											= INITIAL_PACKET_ID;
				BdUsbpSendControlPacket(BdUsbpGlobalData, PACKET_TYPE_KD_RESET, 0);
				return KDP_PACKET_RESEND;
			}
			else
			{
				CsPrintf(CHAR8_CONST_STRING("[RECV] control unknown %08x %08x %08x\n"), packetHeader.PacketType, packetHeader.PacketId, packetHeader.ByteCount);
				continue;
			}			
		}

		if(packetType == PACKET_TYPE_KD_ACKNOWLEDGE)
		{
			if(packetHeader.PacketId == BdPacketIdExpected)
			{
				BdUsbpSendControlPacket(BdUsbpGlobalData, PACKET_TYPE_KD_RESEND, 0);
				BdNextPacketIdToSend										^= 1;
				return KDP_PACKET_RECEIVED;
			}

			BdUsbpSendControlPacket(BdUsbpGlobalData, PACKET_TYPE_KD_ACKNOWLEDGE, packetHeader.PacketId);
			continue;
		}

		if(packetHeader.ByteCount > PACKET_MAX_SIZE || packetHeader.ByteCount < messageHeader->MaximumLength)
		{
			CsPrintf(CHAR8_CONST_STRING("[RECV] invalid size %08x %08x\n"), packetHeader.ByteCount, messageHeader->MaximumLength);
			BdUsbpSendControlPacket(BdUsbpGlobalData, PACKET_TYPE_KD_RESEND, 0);
			continue;
		}

		messageHeader->Length												= messageHeader->MaximumLength;
		*dataLength															= packetHeader.ByteCount - messageHeader->Length;
		messageData->Length													= static_cast<UINT16>(*dataLength);

		if(packetHeader.PacketType != packetType)
		{
			CsPrintf(CHAR8_CONST_STRING("[RECV] not expected type %08x %08x\n"), packetType, packetHeader.PacketType);
			BdUsbpSendControlPacket(BdUsbpGlobalData, PACKET_TYPE_KD_ACKNOWLEDGE, packetHeader.PacketId);
			continue;
		}

		if(packetHeader.PacketId != INITIAL_PACKET_ID && packetHeader.PacketId != (INITIAL_PACKET_ID ^ 1))
		{
			CsPrintf(CHAR8_CONST_STRING("[RECV] invalid id %08x\n"), packetHeader.PacketId);
			BdUsbpSendControlPacket(BdUsbpGlobalData, PACKET_TYPE_KD_RESEND, 0);
			continue;
		}

		if(packetHeader.PacketId != BdPacketIdExpected)
		{
			CsPrintf(CHAR8_CONST_STRING("[RECV] not expected id %08x %08x\n"), packetHeader.PacketId, BdPacketIdExpected);
			BdUsbpSendControlPacket(BdUsbpGlobalData, PACKET_TYPE_KD_ACKNOWLEDGE, packetHeader.PacketId);
			continue;
		}

		UINT32 checksum														= BdComputeChecksum(messageHeader->Buffer, messageHeader->Length);
		checksum															+= BdComputeChecksum(messageData->Buffer, messageData->Length);
		if(checksum != packetHeader.Checksum)
		{
			CsPrintf(CHAR8_CONST_STRING("[RECV] invalid checksum %08x %08x\n"), checksum, packetHeader.Checksum);
			BdUsbpSendControlPacket(BdUsbpGlobalData, PACKET_TYPE_KD_RESEND, 0);
			continue;
		}

		//CsPrintf(CHAR8_CONST_STRING("[RECV] OK %08x %08x, %08x, %08x\n"), packetHeader.PacketType, packetHeader.PacketId, packetHeader.ByteCount, packetHeader.Checksum);
		BdUsbpSendControlPacket(BdUsbpGlobalData, PACKET_TYPE_KD_ACKNOWLEDGE, packetHeader.PacketId);
		BdPacketIdExpected													^= 1;
		return KDP_PACKET_RECEIVED;
	}
	return KDP_PACKET_RESEND;
}

//
// setup debug device
//
EFI_STATUS BdUsbConfigureDebuggerDevice(CHAR8 CONST* loaderOptions)
{
	EFI_STATUS status														= EFI_SUCCESS;

	__try
	{
		if(!loaderOptions || !loaderOptions[0])
			try_leave(NOTHING);

		//
		// check target name
		//
		CHAR8 CONST* targetName												= strstr(loaderOptions, CHAR8_CONST_STRING("/targetname="));
		if(!targetName)
			try_leave(status = EFI_INVALID_PARAMETER);

		//
		// connect wait
		//
		CHAR8 CONST* connectWait											= strstr(loaderOptions, CHAR8_CONST_STRING("/connectwait="));
		if(connectWait)
		{
			UINTN waitTime													= static_cast<UINT32>(atoi(connectWait + 13));
			EfiBootServices->Stall(1000 * 1000 * waitTime);
		}

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
		if(EFI_ERROR(status	= BlFindPciDevice(segment, bus, device, func, PCI_CLASS_SERIAL, PCI_CLASS_SERIAL_USB, 0x20, &pciIoProtocol, nullptr)))
			try_leave(NOTHING);

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
		if(barLength < 0x400 || !isMemorySpace)
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
		// allocate global data
		//
		BdUsbpGlobalData													= static_cast<DEBUG_USB_GLOBAL_DATA*>(MmAllocatePool(sizeof(DEBUG_USB_GLOBAL_DATA)));
		if(!BdUsbpGlobalData)
			try_leave(status = EFI_OUT_OF_RESOURCES);

		//
		// save target name
		//
		memset(BdUsbpGlobalData, 0, sizeof(DEBUG_USB_GLOBAL_DATA));
		BdUsbpGlobalData->BaseRegister										= ArchConvertAddressToPointer(phyAddress, VOID*);
		for(targetName += 12; BdUsbpGlobalData->NameLength < ARRAYSIZE(BdUsbpGlobalData->TargetName) - 1; ++ targetName)
		{
			if((*targetName < 'A' || *targetName > 'Z') && (*targetName < 'a' || *targetName > 'z') && (*targetName < '0' || *targetName > '9') && *targetName != '-' && *targetName != '_')
				break;

			BdUsbpGlobalData->TargetName[BdUsbpGlobalData->NameLength ++]	= *targetName >= 'a' && *targetName <= 'z' ? *targetName - 'a'  + 'A' : *targetName;
		}

		//
		// initialize controller
		//
		if(!BdUsbpInitializeController(BdUsbpGlobalData, pciIoProtocol) || !BdUsbpInitializeDevice(BdUsbpGlobalData, !!strstr(loaderOptions, CHAR8_CONST_STRING("/waitusb"))))
			try_leave(status = EFI_DEVICE_ERROR);

		//
		// set next packet id
		//
		BdNextPacketIdToSend												= INITIAL_PACKET_ID | SYNC_PACKET_ID;
		BdPacketIdExpected													= INITIAL_PACKET_ID;
		status																= EFI_SUCCESS;
	}
	__finally
	{
		if(EFI_ERROR(status) && BdUsbpGlobalData)
		{
			MmFreePool(BdUsbpGlobalData);
			BdUsbpGlobalData												= nullptr;
		}
	}

	return status;
}

//
// close debug device
//
EFI_STATUS BdUsbCloseDebuggerDevice()
{
	if(BdUsbpGlobalData)
		MmFreePool(BdUsbpGlobalData);

	BdUsbpGlobalData														= nullptr;
	
	return EFI_SUCCESS;
}