//********************************************************************
//	created:	6:11:2009   21:39
//	filename: 	ArchDefine.h
//	author:		tiamo
//	purpose:	arch define
//********************************************************************

#pragma once

//
// consts
//
#define KDP_BREAKPOINT_TYPE													UINT8
#define KDP_BREAKPOINT_VALUE												0xcc
#define DBGKD_MAXSTREAM														16
#define SIZE_OF_80387_REGISTERS												80
#define CONTEXT_i386														0x00010000
#define CONTEXT_i486														0x00010000
#define CONTEXT_CONTROL														(CONTEXT_i386 | 0x00000001L)
#define CONTEXT_INTEGER														(CONTEXT_i386 | 0x00000002L)
#define CONTEXT_SEGMENTS													(CONTEXT_i386 | 0x00000004L)
#define CONTEXT_FLOATING_POINT												(CONTEXT_i386 | 0x00000008L)
#define CONTEXT_DEBUG_REGISTERS												(CONTEXT_i386 | 0x00000010L)
#define CONTEXT_FULL														(CONTEXT_CONTROL | CONTEXT_INTEGER | CONTEXT_SEGMENTS)
#define MAXIMUM_SUPPORTED_EXTENSION											512

//
// byte pack
//
#include <pshpack1.h>

//
// gdtr and idtr
//
typedef struct _DESCRIPTOR
{
	//
	// pading
	//
	UINT16																	Pad;

	//
	// limit
	//
	UINT16																	Limit;

	//
	// base
	//
	UINT32																	Base;
}KDESCRIPTOR;

//
// idt entry
//
typedef struct _KIDTENTRY
{
	//
	// offset
	//
	UINT16																	Offset;

	//
	// selector
	//
	UINT16																	Selector;

	//
	// access
	//
	UINT16																	Access;

	//
	// extended offset
	//
	UINT16																	ExtendedOffset;
}KIDTENTRY;

//
// special register
//
typedef struct _KSPECIAL_REGISTERS
{
	//
	// cr0
	//
	UINT32																	Cr0;

	//
	// cr2
	//
	UINT32																	Cr2;

	//
	// cr3
	//
	UINT32																	Cr3;

	//
	// cr4
	//
	UINT32																	Cr4;

	//
	// kernel dr0
	//
	UINT32																	KernelDr0;

	//
	// kernel dr1
	//
	UINT32																	KernelDr1;

	//
	// kernel dr2
	//
	UINT32																	KernelDr2;

	//
	// kernel dr3
	//
	UINT32																	KernelDr3;

	//
	// kernel dr6
	//
	UINT32																	KernelDr6;

	//
	// kernel dr7
	//
	UINT32																	KernelDr7;

	//
	// gdtr
	//
	KDESCRIPTOR																Gdtr;

	//
	// idtr
	//
	KDESCRIPTOR																Idtr;

	//
	// tr
	//
	UINT16																	Tr;

	//
	// ldtr
	//
	UINT16																	Ldtr;

	//
	// padding
	//
	UINT32																	Reserved[6];
}KSPECIAL_REGISTERS;

//
// floating save area
//
typedef struct _FLOATING_SAVE_AREA
{
	//
	// control word
	//
	UINT32																	ControlWord;

	//
	// status word
	//
	UINT32																	StatusWord;

	//
	// tag word
	//
	UINT32																	TagWord;

	//
	// error offset
	//
	UINT32																	ErrorOffset;

	//
	// error selector
	//
	UINT32																	ErrorSelector;

	//
	// data offset
	//
	UINT32																	DataOffset;

	//
	// data selector
	//
	UINT32																	DataSelector;

	//
	// registers
	//
	UINT8																	RegisterArea[SIZE_OF_80387_REGISTERS];

	//
	// npx state
	//
	UINT32																	Cr0NpxState;
}FLOATING_SAVE_AREA;

//
// context
//
typedef struct _CONTEXT
{
	//
	// flags
	//
	UINT32																	ContextFlags;

	//
	// dr0
	//
	UINT32																	Dr0;

	//
	// dr1
	//
	UINT32																	Dr1;

	//
	// dr2
	//
	UINT32																	Dr2;

	//
	// dr3
	//
	UINT32																	Dr3;

	//
	// dr6
	//
	UINT32																	Dr6;

	//
	// dr7
	//
	UINT32																	Dr7;

	//
	// floating save
	//
	FLOATING_SAVE_AREA														FloatSave;

	//
	// gs
	//
	UINT32																	SegGs;

	//
	// fs
	//
	UINT32																	SegFs;

	//
	// es
	//
	UINT32																	SegEs;

	//
	// ds
	//
	UINT32																	SegDs;

	//
	// edi
	//
	UINT32																	Edi;

	//
	// esi
	//
	UINT32																	Esi;

	//
	// ebx
	//
	UINT32																	Ebx;

	//
	// edx
	//
	UINT32																	Edx;

	//
	// ecx
	//
	UINT32																	Ecx;

	//
	// eax
	//
	UINT32																	Eax;

	//
	// ebp
	//
	UINT32																	Ebp;

	//
	// eip
	//
	UINT32																	Eip;

	//
	// cs
	//
	UINT32																	SegCs;              // MUST BE SANITIZED

	//
	// eflags
	//
	UINT32																	EFlags;             // MUST BE SANITIZED

	//
	// esp
	//
	UINT32																	Esp;

	//
	// ss
	//
	UINT32																	SegSs;

	//
	// extended registers
	//
	UINT8																	ExtendedRegisters[MAXIMUM_SUPPORTED_EXTENSION];
}CONTEXT;

//
// processor state
//
typedef struct _KPROCESSOR_STATE
{
	//
	// context frame
	//
	CONTEXT																	ContextFrame;

	//
	// special register
	//
	KSPECIAL_REGISTERS														SpecialRegisters;
}KPROCESSOR_STATE;

//
// trap frame
//
typedef struct _KTRAP_FRAME
{
	//
	// following 4 values are only used and defined for DBG systems,
	// but are always allocated to make switching from DBG to non-DBG and back quicker.
	// they are not DEVL because they have a non-0 performance impact.
	//

	//
	// copy of User EBP set up so KB will work.
	//
	UINT32																	DbgEbp;

	//
	// EIP of caller to system call, again, for KB.
	//
	UINT32																	DbgEip;

	//
	// marker to show no args here.
	//
	UINT32																	DbgArgMark;

	//
	// Pointer to the actual args
	//
	UINT32																	DbgArgPointer;

	//
	// temporary values used when frames are edited.
	// any code that want's ESP must materialize it, since it is not stored in the frame for kernel mode callers.
	// and code that sets ESP in a KERNEL mode frame, must put the new value in TempEsp,
	// make sure that TempSegCs holds the real SegCs value, and put a special marker value into SegCs.
	//
	UINT32																	TempSegCs;

	//
	// temp esp
	//
	UINT32																	TempEsp;

	//
	//  debug registers 0
	//
	UINT32																	Dr0;

	//
	//  debug registers 1
	//
	UINT32																	Dr1;

	//
	//  debug registers 2
	//
	UINT32																	Dr2;

	//
	//  debug registers 3
	//
	UINT32																	Dr3;

	//
	//  debug registers 6
	//
	UINT32																	Dr6;

	//
	//  debug registers 7
	//
	UINT32																	Dr7;

	//
	//  gs
	//
	UINT32																	SegGs;

	//
	// es
	//
	UINT32																	SegEs;

	//
	// ds
	//
	UINT32																	SegDs;

	//
	// edx
	//
	UINT32																	Edx;

	//
	// ecx
	//
	UINT32																	Ecx;

	//
	// eax
	//
	UINT32																	Eax;

	//
	// nesting state, not part of context record
	//
	UINT32																	PreviousPreviousMode;

	//
	// exception list
	// trash if caller was user mode.
	// saved exception list if caller was kernel mode or we're in an interrupt.
	//
	VOID*																	ExceptionList;

	//
	// fS is TIB/PCR pointer, is here to make save sequence easy
	//
	UINT32																	SegFs;

	//
	//  edi
	//
	UINT32																	Edi;

	//
	// esi
	//
	UINT32																	Esi;

	//
	// ebx
	//
	UINT32																	Ebx;

	//
	// ebp
	//
	UINT32																	Ebp;

	//
	// error code,if hardware did not push an error code,we push a zero on the stack
	//
	UINT32																	ErrCode;

	//
	// hardware saved eip
	//
	UINT32																	Eip;

	//
	// hardware save cs
	//
	UINT32																	SegCs;

	//
	// hardware saved eflags
	//
	UINT32																	EFlags;

	//
	// WARNING - segSS:esp are only here for stack that involve a ring transition.
	//
	UINT32																	HardwareEsp;

	//
	// hardware saved ss
	//
	UINT32																	HardwareSegSs;

	//
	// v86 mode es
	//
	UINT32																	V86Es;

	//
	// v86 mode ds
	//
	UINT32																	V86Ds;

	//
	// v86 mode fs
	//
	UINT32																	V86Fs;

	//
	// v86 mode gs
	//
	UINT32																	V86Gs;
}KTRAP_FRAME;

//
// processor control block (PRCB)
//
typedef struct _KPRCB
{
	//
	// minor version
	//
	UINT16																	MinorVersion;

	//
	// major version
	//
	UINT16																	MajorVersion;

	//
	// current thread
	//
	VOID*																	CurrentThread;

	//
	// next thread
	//
	VOID*																	NextThread;

	//
	// idle thread
	//
	VOID*																	IdleThread;

	//
	// number
	//
	INT8																	Number;

	//
	// padding
	//
	INT8																	Reserved;

	//
	// build type
	//
	UINT16																	BuildType;

	//
	// set member
	//
	UINT32																	SetMember;

	//
	// cpu type
	//
	INT8																	CpuType;

	//
	// cpu id
	//
	INT8																	CpuID;

	//
	// cpu step
	//
	UINT16																	CpuStep;

	//
	// processor state
	//
	KPROCESSOR_STATE														ProcessorState;

	//
	// reserved
	//
	UINT8																	Reserved1[0xb84];
}KPRCB;

//
// pcr
//
typedef struct _KPCR
{
	//
	// exception list
	//
	VOID*																	UsedExceptionList;

	//
	// stack base
	//
	VOID*																	UsedStackBase;

	//
	// pref global group mask
	//
	VOID*																	PrefGlobalGroupMask;

	//
	// Tss copy
	//
	VOID*																	TssCopy;

	//
	// context switches
	//
	UINT32																	ContextSwitches;

	//
	// set member copy
	//
	UINT32																	SetMemberCopy;

	//
	// used self
	//
	struct _KPCR*															UsedSelf;

	//
	// self pcr
	//
	struct _KPCR*															SelfPcr;

	//
	// prcb
	//
	KPRCB*																	Prcb;

	//
	// irql
	//
	UINT32																	Irql;

	//
	// irr
	//
	UINT32																	Irr;

	//
	// irr active
	//
	UINT32																	IrrActive;

	//
	// IDR
	//
	UINT32																	Idr;

	//
	// kd version
	//
	VOID*																	KdVersionBlock;

	//
	// idt
	//
	VOID*																	IdtBase;

	//
	// gdt
	//
	VOID*																	GdtBase;

	//
	// tss
	//
	VOID*																	TssBase;

	//
	// major version
	//
	UINT16																	MajorVersion;

	//
	// minor version
	//
	UINT16																	MinorVersion;

	//
	// set member
	//
	UINT32																	SetMember;

	//
	// stall scale factor
	//
	UINT32																	StallScaleFactor;

	//
	// unused
	//
	UINT8																	SpareUnused;

	//
	// number
	//
	UINT8																	Number;

	//
	// spare
	//
	UINT8																	Spare0;

	//
	// second level cache associativity
	//
	UINT8																	SecondLevelCacheAssociativity;

	//
	// vdm alert
	//
	UINT32																	VdmAlert;

	//
	// kernel reserved
	//
	UINT32																	KernelReserved[14];

	//
	// second level cache size
	//
	UINT32																	SecondLevelCacheSize;

	//
	// hal reserved
	//
	UINT32																	HalReserved[16];

	//
	// interrupt mode
	//
	UINT32																	InterruptMode;

	//
	// spare
	//
	UINT32																	Spare1;

	//
	// kernel reserved
	//
	UINT32																	KernelReserved2[17];

	//
	// prcb
	//
	KPRCB																	PrcbData;
}KPCR;

//
// restore pack
//
#include <poppack.h>

//
// control report
//
typedef struct _X86_DBGKD_CONTROL_REPORT
{
	//
	// kerenl dr6
	//
	UINT32																	Dr6;

	//
	// kernel dr7
	//
	UINT32																	Dr7;

	//
	// instruction count
	//
	UINT16																	InstructionCount;

	//
	// report flags
	//
	UINT16																	ReportFlags;

	//
	// instruction stream
	//
	UINT8																	InstructionStream[DBGKD_MAXSTREAM];

	//
	// cs
	//
	UINT16																	SegCs;

	//
	// ds
	//
	UINT16																	SegDs;

	//
	// es
	//
	UINT16																	SegEs;

	//
	// fs
	//
	UINT16																	SegFs;

	//
	// eflags
	//
	UINT32																	EFlags;

	//
	// pading
	//
	UINT32																	Padding[2];
}DBGKD_CONTROL_REPORT;

//
// control sets for supported architectures
//
typedef struct _X86_DBGKD_CONTROL_SET
{
	//
	// trace flag
	//
	UINT32																	TraceFlag;

	//
	// dr7
	//
	UINT32																	Dr7;

	//
	// symbol start
	//
	UINT32																	CurrentSymbolStart;

	//
	// symbol end
	//
	UINT32																	CurrentSymbolEnd;
}DBGKD_CONTROL_SET;

//
// get cs
//
UINT32 BOOTAPI ArchGetSegCs();

//
// get gdtr
//
VOID BOOTAPI ArchGetGdtRegister(KDESCRIPTOR* gdtr);

//
// set gdtr
//
VOID BOOTAPI ArchSetGdtRegister(KDESCRIPTOR* gdtr);

//
// get idtr
//
VOID BOOTAPI ArchGetIdtRegister(KDESCRIPTOR* idtr);

//
// set idtr
//
VOID BOOTAPI ArchSetIdtRegister(KDESCRIPTOR* idtr);

//
// set idt entry
//
VOID ArchSetIdtEntry(UINT32 base, UINT32 index, UINT32 segCs, VOID* offset,UINT32 access);