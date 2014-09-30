//********************************************************************
//	created:	6:11:2009   22:29
//	filename: 	BootDebuggerPrivate.h
//	author:		tiamo
//	purpose:	private
//********************************************************************

#pragma once

//
// arch define
//
#include "ArchDefine.h"

//
// typedef
//
typedef INT32																NTSTATUS;
typedef BOOLEAN (*BdDebugRoutine)(struct _EXCEPTION_RECORD* exceptionRecord, struct _KEXCEPTION_FRAME* exceptionFrame, struct _KTRAP_FRAME* trapFrame);
typedef UINT32 (*BdReceivePacketRoutine)(UINT32 packetType, struct _STRING* header, struct _STRING* data, UINT32* dataLength);
typedef VOID (*BdSendPacketRoutine)(UINT32 packetType, struct _STRING* messageHeader, struct _STRING* messageData);

//
// consts
//
#define EXCEPTION_MAXIMUM_PARAMETERS										15
#define BREAKPOINT_TABLE_SIZE												32
#define PACKET_MAX_SIZE														4000
#define BREAKPOINT_BREAK													0
#define BREAKPOINT_PRINT													1
#define BREAKPOINT_PROMPT													2
#define BREAKPOINT_LOAD_SYMBOLS												3
#define BREAKPOINT_UNLOAD_SYMBOLS											4

#define INITIAL_PACKET_ID													0x80800000
#define SYNC_PACKET_ID														0x00000800

#define BREAKIN_PACKET														0x62626262
#define BREAKIN_PACKET_BYTE													0x62
#define PACKET_LEADER														0x30303030
#define PACKET_LEADER_BYTE													0x30
#define CONTROL_PACKET_LEADER												0x69696969
#define CONTROL_PACKET_LEADER_BYTE											0x69
#define PACKET_TRAILING_BYTE												0xAA

#define PACKET_TYPE_UNUSED													0
#define PACKET_TYPE_KD_STATE_CHANGE											1
#define PACKET_TYPE_KD_STATE_MANIPULATE										2
#define PACKET_TYPE_KD_DEBUG_IO												3
#define PACKET_TYPE_KD_ACKNOWLEDGE											4
#define PACKET_TYPE_KD_RESEND												5
#define PACKET_TYPE_KD_RESET												6
#define PACKET_TYPE_KD_STATE_CHANGE64										7
#define PACKET_TYPE_KD_POLL_BREAKIN											8
#define PACKET_TYPE_KD_TRACE_IO												9
#define PACKET_TYPE_KD_CONTROL_REQUEST										10
#define PACKET_TYPE_KD_FILE_IO												11
#define PACKET_TYPE_MAX														12

#define DbgKdMinimumStateChange												0x00003030
#define DbgKdExceptionStateChange											0x00003030
#define DbgKdLoadSymbolsStateChange											0x00003031
#define DbgKdCommandStringStateChange										0x00003032
#define DbgKdMaximumStateChange												0x00003033

#define REPORT_INCLUDES_SEGS												0x0001
#define REPORT_INCLUDES_CS													0x0002

#define KD_CONTINUE_ERROR													0
#define KD_CONTINUE_SUCCESS													1
#define KD_CONTINUE_PROCESSOR_RESELECTED									2
#define KD_CONTINUE_NEXT_PROCESSOR											3

#define KDP_PACKET_RECEIVED													0
#define KDP_PACKET_TIMEOUT													1
#define KDP_PACKET_RESEND													2

#define KD_BREAKPOINT_IN_USE												0x00000001
#define KD_BREAKPOINT_NEEDS_WRITE											0x00000002
#define KD_BREAKPOINT_SUSPENDED												0x00000004
#define KD_BREAKPOINT_NEEDS_REPLACE											0x00000008

#define DbgKdMinimumManipulate												0x00003130
#define DbgKdReadVirtualMemoryApi											0x00003130
#define DbgKdWriteVirtualMemoryApi											0x00003131
#define DbgKdGetContextApi													0x00003132
#define DbgKdSetContextApi													0x00003133
#define DbgKdWriteBreakPointApi												0x00003134
#define DbgKdRestoreBreakPointApi											0x00003135
#define DbgKdContinueApi													0x00003136
#define DbgKdReadControlSpaceApi											0x00003137
#define DbgKdWriteControlSpaceApi											0x00003138
#define DbgKdReadIoSpaceApi													0x00003139
#define DbgKdWriteIoSpaceApi												0x0000313A
#define DbgKdRebootApi														0x0000313B
#define DbgKdContinueApi2													0x0000313C
#define DbgKdReadPhysicalMemoryApi											0x0000313D
#define DbgKdWritePhysicalMemoryApi											0x0000313E
#define DbgKdQuerySpecialCallsApi											0x0000313F
#define DbgKdSetSpecialCallApi												0x00003140
#define DbgKdClearSpecialCallsApi											0x00003141
#define DbgKdSetInternalBreakPointApi										0x00003142
#define DbgKdGetInternalBreakPointApi										0x00003143
#define DbgKdReadIoSpaceExtendedApi											0x00003144
#define DbgKdWriteIoSpaceExtendedApi										0x00003145
#define DbgKdGetVersionApi													0x00003146
#define DbgKdWriteBreakPointExApi											0x00003147
#define DbgKdRestoreBreakPointExApi											0x00003148
#define DbgKdCauseBugCheckApi												0x00003149
#define DbgKdSwitchProcessor												0x00003150
#define DbgKdPageInApi														0x00003151
#define DbgKdReadMachineSpecificRegister									0x00003152
#define DbgKdWriteMachineSpecificRegister									0x00003153
#define OldVlm1																0x00003154
#define OldVlm2																0x00003155
#define DbgKdSearchMemoryApi												0x00003156
#define DbgKdGetBusDataApi													0x00003157
#define DbgKdSetBusDataApi													0x00003158
#define DbgKdCheckLowMemoryApi												0x00003159
#define DbgKdClearAllInternalBreakpointsApi									0x0000315A
#define DbgKdFillMemoryApi													0x0000315B
#define DbgKdQueryMemoryApi													0x0000315C
#define DbgKdSwitchPartition												0x0000315D
#define DbgKdMaximumManipulate												0x0000315E

#define DbgKdPrintStringApi													0x00003230
#define DbgKdGetStringApi													0x00003231

#define DbgKdPrintTraceApi													0x00003330

#define DbgKdCreateFileApi													0x00003430
#define DbgKdReadFileApi													0x00003431
#define DbgKdWriteFileApi													0x00003432
#define DbgKdCloseFileApi													0x00003433

#define DBGKD_64BIT_PROTOCOL_VERSION1										5
#define DBGKD_64BIT_PROTOCOL_VERSION2										6

#define DBGKD_QUERY_MEMORY_VIRTUAL											0
#define DBGKD_QUERY_MEMORY_PROCESS											0
#define DBGKD_QUERY_MEMORY_SESSION											1
#define DBGKD_QUERY_MEMORY_KERNEL											2

#define DBGKD_QUERY_MEMORY_READ												0x01
#define DBGKD_QUERY_MEMORY_WRITE											0x02
#define DBGKD_QUERY_MEMORY_EXECUTE											0x04
#define DBGKD_QUERY_MEMORY_FIXED											0x08

#define DBGKD_VERS_FLAG_MP													0x0001
#define DBGKD_VERS_FLAG_DATA												0x0002
#define DBGKD_VERS_FLAG_PTR64												0x0004
#define DBGKD_VERS_FLAG_NOMM												0x0008
#define DBGKD_VERS_FLAG_HSS													0x0010
#define DBGKD_VERS_FLAG_PARTITIONS											0x0020

#define NT_SUCCESS(S)														(((NTSTATUS)(S)) >= 0)
#define STATUS_SUCCESS														0
#define STATUS_PENDING														((NTSTATUS)0x00000103)
#define STATUS_DATATYPE_MISALIGNMENT										((NTSTATUS)0x80000002)
#define STATUS_BREAKPOINT													((NTSTATUS)0x80000003)
#define STATUS_UNSUCCESSFUL													((NTSTATUS)0xc0000001)
#define STATUS_INVALID_PARAMETER											((NTSTATUS)0xc000000d)
#define STATUS_DEVICE_NOT_CONNECTED											((NTSTATUS)0xc000009d)

#define KDP_TYPE_NONE														0
#define KDP_TYPE_COM														1
#define KDP_TYPE_1394														2
#define KDP_TYPE_USB														3

//
// list entry
//
typedef struct _LIST_ENTRY
{
	//
	// next
	//
	struct _LIST_ENTRY*														Flink;

	//
	// prev
	//
	struct _LIST_ENTRY*														Blink;
}LIST_ENTRY;

//
// ansi string
//
typedef struct _STRING
{
	//
	// length
	//
	UINT16																	Length;

	//
	// max-length
	//
	UINT16																	MaximumLength;

	//
	// buffer
	//
	CHAR8*																	Buffer;
}STRING;

//
// unicode string
//
typedef struct _UNICODE_STRING
{
	//
	// length
	//
	UINT16																	Length;

	//
	// max-length
	//
	UINT16																	MaximumLength;

	//
	// buffer
	//
	CHAR16*																	Buffer;
}UNICODE_STRING;

//
// exception record
//
typedef struct _EXCEPTION_RECORD
{
	//
	// code
	//
	UINT32																	ExceptionCode;

	//
	// flags
	//
	UINT32																	ExceptionFlags;

	//
	// record
	//
	struct _EXCEPTION_RECORD*												ExceptionRecord;

	//
	// address
	//
	VOID*																	ExceptionAddress;

	//
	// parameters count
	//
	UINT32																	NumberParameters;

	//
	// info
	//
	UINTN																	ExceptionInformation[EXCEPTION_MAXIMUM_PARAMETERS];
}EXCEPTION_RECORD;

//
// exception 64
//
typedef struct _EXCEPTION_RECORD64
{
	//
	// code
	//
	UINT32																	ExceptionCode;

	//
	// flags
	//
	UINT32																	ExceptionFlags;

	//
	// record
	//
	UINT64																	ExceptionRecord;

	//
	// address
	//
	UINT64																	ExceptionAddress;

	//
	// parameters count
	//
	UINT32																	NumberParameters;

	//
	// info
	//
	UINT64																	ExceptionInformation[EXCEPTION_MAXIMUM_PARAMETERS];
}EXCEPTION_RECORD64;

//
// loader data table entry
//
typedef struct _LDR_DATA_TABLE_ENTRY
{
	//
	// in load order link list entry
	//
	LIST_ENTRY																InLoadOrderLinks;

	//
	// in memory order link list entry
	//
	LIST_ENTRY																InMemoryOrderLinks;

	//
	// in initialization order list entry
	//
	LIST_ENTRY																InInitializationOrderLinks;

	//
	// image base
	//
	VOID*																	DllBase;

	//
	// entry point
	//
	VOID*																	EntryPoint;

	//
	// size of image
	//
	UINT32																	SizeOfImage;

	//
	// full dll name
	//
	UNICODE_STRING															FullDllName;

	//
	// base dll name
	//
	UNICODE_STRING															BaseDllName;

	//
	// flags
	//
	UINT32																	Flags;

	//
	// reference count
	//
	UINT16																	LoadCount;

	//
	// thread local storage index

	//
	UINT16																	TlsIndex;

	union
	{
		//
		// hash links
		//
		LIST_ENTRY															HashLinks;

		//
		// section and checksum
		//
		struct _SECTION_CHECKSUM
		{
			//
			// section pointer
			//
			VOID*															SectionPointer;

			//
			// checksum
			//
			UINT32															CheckSum;
		}SectionAndCheckSum;
	};

	union
	{
		//
		// time stamp
		//
		UINT32																TimeDateStamp;

		//
		// loaded imports
		//
		VOID*																LoadedImports;
	};

	//
	// entry point activation context
	//
	VOID*																	EntryPointActivationContext;

	//
	// patch information
	//
	VOID*																	PatchInformation;

	//
	// forwarder links
	//
	LIST_ENTRY																ForwarderLinks;

	//
	// service tag links
	//
	LIST_ENTRY																ServiceTagLinks;

	//
	// static links
	//
	LIST_ENTRY																StaticLinks;
}LDR_DATA_TABLE_ENTRY;

//
// KD_PACKETS are the low level data format used in KD.
// all packets begin with a packet leader, byte count, packet type.
// the sequence for accepting a packet is:
//
//  - read 4 bytes to get packet leader.
//	  if read times out (10 seconds) with a short read, or if packet leader is incorrect, then retry the read.
//
//  - next read 2 byte packet type.
//	  if read times out (10 seconds) with a short read, or if packet type is bad, then start again looking for a packet leader.
//
//  - next read 4 byte packet Id.
//	  if read times out (10 seconds) with a short read, or if packet Id is not what we expect, then ask for resend and restart again looking for a packet leader.
//
//  - next read 2 byte byte count.
//	  if read times out (10 seconds) with a short read, or if byte count is greater than PACKET_MAX_SIZE, then start again looking for a packet leader.
//
//  - next read 4 byte packet data checksum.
//
//  - The packet data immediately follows the packet.
//	  there should be ByteCount bytes following the packet header.read the packet data, if read times out (10 seconds) then start again looking for a packet leader.
//
typedef struct _KD_PACKET
{
	//
	// leader
	//
	UINT32																	PacketLeader;

	//
	// type
	//
	UINT16																	PacketType;

	//
	// byte count
	//
	UINT16																	ByteCount;

	//
	// packet id
	//
	UINT32																	PacketId;

	//
	// checksum
	//
	UINT32																	Checksum;
}KD_PACKET;

//
// exception 64
//
typedef struct _DBGKM_EXCEPTION64
{
	//
	// exception record
	//
	EXCEPTION_RECORD64														ExceptionRecord;

	//
	// first chance
	//
	UINT32																	FirstChance;
}DBGKM_EXCEPTION64;

//
// symbols info
//
typedef struct _KD_SYMBOLS_INFO
{
	//
	// base of dll
	//
	VOID*																	BaseOfDll;

	//
	// process id
	//
	UINTN																	ProcessId;

	//
	// checksum
	//
	UINT32																	CheckSum;

	//
	// size of image
	//
	UINT32																	SizeOfImage;
}KD_SYMBOLS_INFO,*PKD_SYMBOLS_INFO;

//
// load symbols
//
typedef struct _DBGKD_LOAD_SYMBOLS64
{
	//
	// path name length
	//
	UINT32																	PathNameLength;

	//
	// base of dll
	//
	UINT64																	BaseOfDll;

	//
	// process id
	//
	UINT64																	ProcessId;

	//
	// checksum
	//
	UINT32																	CheckSum;

	//
	// size of image
	//
	UINT32																	SizeOfImage;

	//
	// unload
	//
	BOOLEAN																	UnloadSymbols;
}DBGKD_LOAD_SYMBOLS64;

//
// wait state change
//
typedef struct _DBGKD_WAIT_STATE_CHANGE64
{
	//
	// new state
	//
	UINT32																	NewState;

	//
	// processor level
	//
	UINT16																	ProcessorLevel;

	//
	// processor
	//
	UINT16																	Processor;

	//
	// processor count
	//
	UINT32																	NumberProcessors;

	//
	// thread
	//
	UINT64																	Thread;

	//
	// eip
	//
	UINT64																	ProgramCounter;

	union
	{
		//
		// exception
		//
		DBGKM_EXCEPTION64													Exception;

		//
		// load symbols
		//
		DBGKD_LOAD_SYMBOLS64												LoadSymbols;
	}u;

	//
	// control report
	//
	DBGKD_CONTROL_REPORT													ControlReport;
}DBGKD_WAIT_STATE_CHANGE64;

//
// debug print string
//
typedef struct _DBGKD_PRINT_STRING
{
	//
	// length
	//
	UINT32																	LengthOfString;
}DBGKD_PRINT_STRING;

//
// debug get string
//
typedef struct _DBGKD_GET_STRING
{
	//
	// prompt string length
	//
	UINT32																	LengthOfPromptString;

	//
	// read string length
	//
	UINT32																	LengthOfStringRead;
}DBGKD_GET_STRING;

//
// debug io
//
typedef struct _DBGKD_DEBUG_IO
{
	//
	// api number
	//
	UINT32																	ApiNumber;

	//
	// processor level
	//
	UINT16																	ProcessorLevel;

	//
	// processor
	//
	UINT16																	Processor;

	//
	// param
	//
	union
	{
		//
		// debug print
		//
		DBGKD_PRINT_STRING													PrintString;

		//
		// rtl assert
		//
		DBGKD_GET_STRING													GetString;
	}u;
}DBGKD_DEBUG_IO;

//
// read memory
//
typedef struct _DBGKD_READ_MEMORY64
{
	//
	// address
	//
	UINT64																	TargetBaseAddress;

	//
	// counte
	//
	UINT32																	TransferCount;

	//
	// actual count
	//
	UINT32																	ActualBytesRead;
}DBGKD_READ_MEMORY64;

//
// write memory
//
typedef struct _DBGKD_WRITE_MEMORY64
{
	//
	// address
	//
	UINT64																	TargetBaseAddress;

	//
	// count
	//
	UINT32																	TransferCount;

	//
	// actual count
	//
	UINT32																	ActualBytesWritten;
}DBGKD_WRITE_MEMORY64;

//
// get context
//
typedef struct _DBGKD_GET_CONTEXT
{
	//
	// dummy
	//
	UINT32																	Unused;
}DBGKD_GET_CONTEXT;

//
// set context
//
typedef struct _DBGKD_SET_CONTEXT
{
	//
	// flags
	//
	UINT32																	ContextFlags;
}DBGKD_SET_CONTEXT;

//
// write breakpoint
//
typedef struct _DBGKD_WRITE_BREAKPOINT64
{
	//
	// address
	//
	UINT64																	BreakPointAddress;

	//
	// handle
	//
	UINT32																	BreakPointHandle;
}DBGKD_WRITE_BREAKPOINT64;

//
// restore breakpoint
//
typedef struct _DBGKD_RESTORE_BREAKPOINT
{
	//
	// handle
	//
	UINT32																	BreakPointHandle;
}DBGKD_RESTORE_BREAKPOINT;

//
// continue
//
typedef struct _DBGKD_CONTINUE
{
	//
	// status
	//
	NTSTATUS																ContinueStatus;
}DBGKD_CONTINUE;

//
// continue2
//
typedef struct _DBGKD_CONTINUE2
{
	//
	// status
	//
	NTSTATUS																ContinueStatus;

	//
	// control set
	//
	union
	{
		//
		// x86 control set
		//
		DBGKD_CONTROL_SET													ControlSet;

		//
		// reserved
		//
		UINT32																Reserved[7];
	};
}DBGKD_CONTINUE2;

//
// read/write io
//
typedef struct _DBGKD_READ_WRITE_IO64
{
	//
	// address
	//
	UINT64																	IoAddress;

	//
	// size
	//
	UINT32																	DataSize;

	//
	// value
	//
	UINT32																	DataValue;
} DBGKD_READ_WRITE_IO64;

//
// read/write io ex
//
typedef struct _DBGKD_READ_WRITE_IO_EXTENDED64
{
	//
	// size
	//
	UINT32																	DataSize;

	//
	// interface type
	//
	UINT32																	InterfaceType;

	//
	// bus
	//
	UINT32																	BusNumber;

	//
	// address space
	//
	UINT32																	AddressSpace;

	//
	// address
	//
	UINT64																	IoAddress;

	//
	// value
	//
	UINT32																	DataValue;
}DBGKD_READ_WRITE_IO_EXTENDED64;

//
// read/write msr
//
typedef struct _DBGKD_READ_WRITE_MSR
{
	//
	// index
	//
	UINT32																	Msr;

	//
	// data low
	//
	UINT32																	DataValueLow;

	//
	// data hi
	//
	UINT32																	DataValueHigh;
}DBGKD_READ_WRITE_MSR;

//
// query special calls
//
typedef struct _DBGKD_QUERY_SPECIAL_CALLS
{
	//
	// count
	//
	UINT32																	NumberOfSpecialCalls;
}DBGKD_QUERY_SPECIAL_CALLS;

//
// set special call
//
typedef struct _DBGKD_SET_SPECIAL_CALL64
{
	//
	// call
	//
	UINT64																	SpecialCall;
}DBGKD_SET_SPECIAL_CALL64;

//
// set internal bp
//
typedef struct _DBGKD_SET_INTERNAL_BREAKPOINT64
{
	//
	// address
	//
	UINT64																	BreakpointAddress;

	//
	// flags
	//
	UINT32																	Flags;
}DBGKD_SET_INTERNAL_BREAKPOINT64;

//
// get internal breakpoint
//
typedef struct _DBGKD_GET_INTERNAL_BREAKPOINT64
{
	//
	// address
	//
	UINT64																	BreakpointAddress;

	//
	// flags
	//
	UINT32																	Flags;

	//
	// calls
	//
	UINT32																	Calls;

	//
	// max calls per period
	//
	UINT32																	MaxCallsPerPeriod;

	//
	// min instructions
	//
	UINT32																	MinInstructions;

	//
	// max instructions
	//
	UINT32																	MaxInstructions;

	//
	// total instructions
	//
	UINT32																	TotalInstructions;
}DBGKD_GET_INTERNAL_BREAKPOINT64;

//
// get version64
//
typedef struct _DBGKD_GET_VERSION64
{
	//
	// major version
	//
	UINT16																	MajorVersion;

	//
	// minor version
	//
	UINT16																	MinorVersion;

	//
	// protocol version
	//
	UINT8																	ProtocolVersion;

	//
	// secondary version,cannot be 'A' for compat with dump header
	//
	UINT8																	KdSecondaryVersion;

	//
	// flags
	//
	UINT16																	Flags;

	//
	// machine type
	//
	UINT16																	MachineType;

	//
	// protocol command support descriptions.these allow the debugger to automatically adapt to different levels of command support in different kernels.
	// one beyond highest packet type understood, zero based.
	//
	UINT8																	MaxPacketType;

	//
	// one beyond highest state change understood, zero based.
	//
	UINT8																	MaxStateChange;

	//
	// one beyond highest state manipulate message understood, zero based.
	//
	UINT8																	MaxManipulate;

	// kind of execution environment the kernel is running in, such as a real machine or a simulator.
	// written back by the simulation if one exists.
	UINT8																	Simulation;

	//
	// pending
	//
	UINT16																	Unused[1];

	//
	// base
	//
	UINT64																	KernBase;

	//
	// loaded module list
	//
	UINT64																	PsLoadedModuleList;

	//
	// components may register a debug data block for use by debugger extensions.this is the address of the list head.
	// there will always be an entry for the debugger.
	//
	UINT64																	DebuggerDataList;
}DBGKD_GET_VERSION64;

//
// breakpoint ex
//
typedef struct _DBGKD_BREAKPOINTEX
{
	//
	// count
	//
	UINT32																	BreakPointCount;

	//
	// continue status
	//
	NTSTATUS																ContinueStatus;
}DBGKD_BREAKPOINTEX;

//
// search memory
//
typedef struct _DBGKD_SEARCH_MEMORY
{
	//
	// address
	//
	union
	{
		//
		// input
		//
		UINT64																SearchAddress;

		//
		// output
		//
		UINT64																FoundAddress;
	};

	//
	// length
	//
	UINT64																	SearchLength;

	//
	// pattern length
	//
	UINT32																	PatternLength;
}DBGKD_SEARCH_MEMORY;

//
// get set bus data
//
typedef struct _DBGKD_GET_SET_BUS_DATA
{
	//
	// bus data type
	//
	UINT32																	BusDataType;

	//
	// bus number
	//
	UINT32																	BusNumber;

	//
	// slot number
	//
	UINT32																	SlotNumber;

	//
	// offset
	//
	UINT32																	Offset;

	//
	// length
	//
	UINT32																	Length;
}DBGKD_GET_SET_BUS_DATA;

//
// fill memory
//
typedef struct _DBGKD_FILL_MEMORY
{
	//
	// address
	//
	UINT64																	Address;

	//
	// length
	//
	UINT32																	Length;

	//
	// flags
	//
	UINT16																	Flags;

	//
	// pattern length
	//
	UINT16																	PatternLength;
}DBGKD_FILL_MEMORY;

//
// query memory
//
typedef struct _DBGKD_QUERY_MEMORY
{
	//
	// address
	//
	UINT64																	Address;

	//
	// reserved
	//
	UINT64																	Reserved;

	//
	// address space
	//
	UINT32																	AddressSpace;

	//
	// flags
	//
	UINT32																	Flags;
}DBGKD_QUERY_MEMORY;

//
// switch partition
//
typedef struct _DBGKD_SWITCH_PARTITION
{
	//
	// partition
	//
	UINT32																	Partition;
}DBGKD_SWITCH_PARTITION;

//
// manipulate state
//
typedef struct _DBGKD_MANIPULATE_STATE64
{
	//
	// api number
	//
	UINT32																	ApiNumber;

	//
	// processor level
	//
	UINT16																	ProcessorLevel;

	//
	// processor
	//
	UINT16																	Processor;

	//
	// return status
	//
	NTSTATUS																ReturnStatus;

	//
	// parameters
	//
	union
	{
		//
		// read memory
		//
		DBGKD_READ_MEMORY64													ReadMemory;

		//
		// write memory
		//
		DBGKD_WRITE_MEMORY64												WriteMemory;

		//
		// get context
		//
		DBGKD_GET_CONTEXT													GetContext;

		//
		// set context
		//
		DBGKD_SET_CONTEXT													SetContext;

		//
		// write break point
		//
		DBGKD_WRITE_BREAKPOINT64											WriteBreakPoint;

		//
		// restore break point
		//
		DBGKD_RESTORE_BREAKPOINT											RestoreBreakPoint;

		//
		// continue
		//
		DBGKD_CONTINUE														Continue;

		//
		// continue2
		//
		DBGKD_CONTINUE2														Continue2;

		//
		// read write io
		//
		DBGKD_READ_WRITE_IO64												ReadWriteIo;

		//
		// read write io extened64
		//
		DBGKD_READ_WRITE_IO_EXTENDED64										ReadWriteIoExtended;

		//
		// query special calls
		//
		DBGKD_QUERY_SPECIAL_CALLS											QuerySpecialCalls;

		//
		// set special call64
		//
		DBGKD_SET_SPECIAL_CALL64											SetSpecialCall;

		//
		// set internal breakpoint64
		//
		DBGKD_SET_INTERNAL_BREAKPOINT64										SetInternalBreakpoint;

		//
		// get internal breakpoint64
		//
		DBGKD_GET_INTERNAL_BREAKPOINT64										GetInternalBreakpoint;

		//
		// get version64
		//
		DBGKD_GET_VERSION64													GetVersion64;

		//
		// breakpoint ex
		//
		DBGKD_BREAKPOINTEX													BreakPointEx;

		//
		// read write msr
		//
		DBGKD_READ_WRITE_MSR												ReadWriteMsr;

		//
		// search memory
		//
		DBGKD_SEARCH_MEMORY													SearchMemory;

		//
		// get set bus data
		//
		DBGKD_GET_SET_BUS_DATA												GetSetBusData;

		//
		// fill memory
		//
		DBGKD_FILL_MEMORY													FillMemory;

		//
		// query memory
		//
		DBGKD_QUERY_MEMORY													QueryMemory;

		//
		// switch partition
		//
		DBGKD_SWITCH_PARTITION												SwitchPartition;
	};
}DBGKD_MANIPULATE_STATE64;

//
// create remote file,unicode filename follows as additional data.
//
typedef struct _DBGKD_CREATE_FILE
{
	//
	// access,.etc,GENERIC_READ
	//
	UINT32																	DesiredAccess;

	//
	// attribute
	//
	UINT32																	FileAttributes;

	//
	// share access
	//
	UINT32																	ShareAccess;

	//
	// create disposition
	//
	UINT32																	CreateDisposition;

	//
	// create options
	//
	UINT32																	CreateOptions;

	//
	// returned file handle
	//
	UINT64																	Handle;

	//
	// file length
	//
	UINT64																	Length;
}DBGKD_CREATE_FILE;

//
// read file
// data is returned as additional data in the response.
//
typedef struct _DBGKD_READ_FILE
{
	//
	// file handle in the create packet
	//
	UINT64																	Handle;

	//
	// offset
	//
	UINT64																	Offset;

	//
	// length
	//
	UINT32																	Length;
}DBGKD_READ_FILE;

//
// write file
//
typedef struct _DBGKD_WRITE_FILE
{
	//
	// file handle
	//
	UINT64																	Handle;

	//
	// offset
	//
	UINT64																	Offset;

	//
	// length
	//
	UINT32																	Length;
}DBGKD_WRITE_FILE;

//
// close file
//
typedef struct _DBGKD_CLOSE_FILE
{
	//
	// handle
	//
	UINT64																	Handle;
}DBGKD_CLOSE_FILE;

//
// remote file io
//
typedef struct _DBGKD_FILE_IO
{
	//
	// api number
	//
	UINT32																	ApiNumber;

	//
	// result status
	//
	UINT32																	ReturnStatus;

	union
	{
		//
		// make space?
		//
		UINT64																ReserveSpace[7];

		//
		// create file
		//
		DBGKD_CREATE_FILE													CreateFile;

		//
		// read file
		//
		DBGKD_READ_FILE														ReadFile;

		//
		// write file
		//
		DBGKD_WRITE_FILE													WriteFile;

		//
		// close file
		//
		DBGKD_CLOSE_FILE													CloseFile;
	};
}DBGKD_FILE_IO;


//
// breakpoint entry
//
typedef struct _BREAKPOINT_ENTRY
{
	//
	// flags
	//
	UINT32																	Flags;

	//
	// address
	//
	UINT64																	Address;

	//
	// old content
	//
	KDP_BREAKPOINT_TYPE														Content;

}BREAKPOINT_ENTRY;

//
// report exception state change
//
VOID BdReportExceptionStateChange(EXCEPTION_RECORD* exceptionRecord, CONTEXT* contextRecord);

//
// report load symbols state change
//
VOID BdReportLoadSymbolsStateChange(STRING* moduleName, KD_SYMBOLS_INFO* symbolsInfo, BOOLEAN unloadSymbols, CONTEXT* contextRecord);

//
// print string
//
BOOLEAN BdPrintString(STRING* printString);

//
// prompt string
//
BOOLEAN BdPromptString(STRING* inputString, STRING* outputString);

//
// compute checksum
//
UINT32 BdComputeChecksum(VOID* dataBuffer, UINT32 bufferLength);

//
// copy memory
//
VOID BdCopyMemory(VOID* dstBuffer, VOID* srcBuffer, UINT32 bufferLength);

//
// move memory
//
UINT32 BdMoveMemory(VOID* dstBuffer, VOID* srcBuffer, UINT32 bufferLength);

//
// remove breakpoint in range
//
BOOLEAN BdDeleteBreakpointRange(UINT64 lowerAddress, UINT64 upperAddress);

//
// arch init
//
EFI_STATUS BdArchInitialize();

//
// destroy arch
//
EFI_STATUS BdArchDestroy();

//
// transfer physical address
//
VOID* BdTranslatePhysicalAddress(UINT64 phyAddress);

//
// read control space
//
VOID BdReadControlSpace(DBGKD_MANIPULATE_STATE64* manipulateState, STRING* additionalData, CONTEXT* contextRecord);

//
// write control space
//
VOID BdWriteControlSpace(DBGKD_MANIPULATE_STATE64* manipulateState, STRING* additionalData, CONTEXT* contextRecord);

//
// set common state
//
VOID BdSetCommonState(UINT32 newState, CONTEXT* contextRecord, DBGKD_WAIT_STATE_CHANGE64* waitStateChange);

//
// extract continuation control data from Manipulate_State message
//
VOID BdGetStateChange(DBGKD_MANIPULATE_STATE64* manipulateState, CONTEXT* contextRecord);

//
// set context state
//
VOID BdSetContextState(DBGKD_WAIT_STATE_CHANGE64* waitStateChange, CONTEXT* contextRecord);

//
// debug routine used when debugger is enabled
//
BOOLEAN BdTrap(EXCEPTION_RECORD* exceptionRecord, struct _KEXCEPTION_FRAME* exceptionFrame, KTRAP_FRAME* trapFrame);

//
// debug service
//
VOID BOOTAPI DbgService(UINTN serviceType, UINTN info1, UINTN info2, UINTN info3, UINTN info4);

//
// debug service2
//
VOID BOOTAPI DbgService(VOID* info1, VOID* info2, UINTN serviceType);

//
// load symbols
//
VOID DbgLoadImageSymbols(STRING* fileName, VOID* imageBase, UINTN processId);

//
// unload symbols
//
VOID DbgUnLoadImageSymbols(STRING* fileName, VOID* imageBase, UINTN processId);

//
// load symbols
//
VOID DbgLoadImageSymbols(UNICODE_STRING* fileName, VOID* imageBase, UINTN processId);

//
// unload symbols
//
VOID DbgUnLoadImageSymbols(UNICODE_STRING* fileName, VOID* imageBase, UINTN processId);

extern BOOLEAN																BdDebuggerNotPresent;
extern BdDebugRoutine														BdDebugTrap;
extern BdSendPacketRoutine													BdSendPacket;
extern BdReceivePacketRoutine												BdReceivePacket;
extern BOOLEAN																BdArchBlockDebuggerOperation;
extern BOOLEAN																BdControlCPressed;
extern BOOLEAN																BdControlCPending;
extern UINT32																BdNextPacketIdToSend;
extern UINT32																BdPacketIdExpected;
extern UINT32																BdNumberRetries;
extern UINT32																BdRetryCount;