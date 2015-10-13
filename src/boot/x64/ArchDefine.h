//********************************************************************
//	created:	7:11:2009   15:06
//	filename: 	ArchDefine.h
//	author:		tiamo
//	purpose:	arch define
//********************************************************************

#pragma once

#define KDP_BREAKPOINT_TYPE													UINT8
#define KDP_BREAKPOINT_VALUE												0xcc
#define DBGKD_MAXSTREAM														16
#define CONTEXT_X64															0x100000
#define CONTEXT_CONTROL														(CONTEXT_X64 | 0x01)
#define CONTEXT_INTEGER														(CONTEXT_X64 | 0x02)
#define CONTEXT_SEGMENTS													(CONTEXT_X64 | 0x04)
#define CONTEXT_FLOATING_POINT												(CONTEXT_X64 | 0x08)
#define CONTEXT_DEBUG_REGISTERS												(CONTEXT_X64 | 0x10)
#define CONTEXT_FULL														(CONTEXT_CONTROL | CONTEXT_INTEGER | CONTEXT_FLOATING_POINT)


#include "pshpack1.h"

//
// gdtr and idtr
//
typedef struct _DESCRIPTOR
{
	//
	// pading
	//
	UINT16																	Pad[3];

	//
	// limit
	//
	UINT16																	Limit;

	//
	// base
	//
	UINT64																	Base;
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

	//
	// high offset
	//
	UINT32																	HighOffset;

	//
	// reserved
	//
	UINT32																	Reserved;
}KIDTENTRY;

//
// 128bits
//
typedef struct _M128A
{
	//
	// low
	//
	UINT64																	Low;

	//
	// high
	//
	INT64																	High;
}M128A;

//
// format of data for 32-bit fxsave/fxrstor instructions.
//
typedef struct _XMM_SAVE_AREA32
{
	//
	// control word
	//
	UINT16																	ControlWord;

	//
	// status word
	//
	UINT16																	StatusWord;

	//
	// tag word
	//
	UINT8																	TagWord;

	//
	// padding
	//
	UINT8																	Reserved1;

	//
	// error opcode
	//
	UINT16																	ErrorOpcode;

	//
	// error offset
	//
	UINT32																	ErrorOffset;

	//
	// error selector, offset = 0x10
	//
	UINT16																	ErrorSelector;

	//
	// padding
	//
	UINT16																	Reserved2;

	//
	// data offset
	//
	UINT32																	DataOffset;

	//
	// data selector
	//
	UINT16																	DataSelector;

	//
	// padding
	//
	UINT16																	Reserved3;

	//
	// mxcsr
	//
	UINT32																	MxCsr;

	//
	// mxcsr mask
	//
	UINT32																	MxCsr_Mask;

	//
	// float registers, offset = 0x20
	//
	M128A																	FloatRegisters[8];

	//
	// xmm registers
	//
	M128A																	XmmRegisters[16];

	//
	// reserved
	//
	UINT8																	Reserved4[96];
}XMM_SAVE_AREA32;

typedef struct _CONTEXT
{
	//
	// P1Home
	//
	UINT64																	P1Home;

	//
	// P2Home
	//
	UINT64																	P2Home;

	//
	// P3Home
	//
	UINT64																	P3Home;

	//
	// P4Home
	//
	UINT64																	P4Home;

	//
	// P5Home
	//
	UINT64																	P5Home;

	//
	// P6Home
	//
	UINT64																	P6Home;

	//
	// control flags.
	//
	UINT32																	ContextFlags;

	//
	// MxCsr
	//
	UINT32																	MxCsr;

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
	// gs
	//
	UINT16																	SegGs;

	//
	// ss
	//
	UINT16																	SegSs;

	//
	// eflags
	//
	UINT32																	EFlags;

	//
	// dr0
	//
	UINT64																	Dr0;

	//
	// dr1
	//
	UINT64																	Dr1;

	//
	// dr2
	//
	UINT64																	Dr2;

	//
	// dr3
	//
	UINT64																	Dr3;

	//
	// dr6
	//
	UINT64																	Dr6;

	//
	// dr7
	//
	UINT64																	Dr7;

	//
	// Rax
	//
	UINT64																	Rax;

	//
	// Rcx
	//
	UINT64																	Rcx;

	//
	// Rdx
	//
	UINT64																	Rdx;

	//
	// Rbx
	//
	UINT64																	Rbx;

	//
	// Rsp
	//
	UINT64																	Rsp;

	//
	// Rbp
	//
	UINT64																	Rbp;

	//
	// Rsi
	//
	UINT64																	Rsi;

	//
	// Rdi
	//
	UINT64																	Rdi;

	//
	// R8
	//
	UINT64																	R8;

	//
	// R9
	//
	UINT64																	R9;

	//
	// R10
	//
	UINT64																	R10;

	//
	// R11
	//
	UINT64																	R11;

	//
	// R12
	//
	UINT64																	R12;

	//
	// R13
	//
	UINT64																	R13;

	//
	// R14
	//
	UINT64																	R14;

	//
	// R15
	//
	UINT64																	R15;

	//
	// rip
	//
	UINT64																	Rip;

	//
	// Floating point state offset = 0x100
	//
	union
	{
		//
		// float point state
		//
		XMM_SAVE_AREA32														FltSave;

		//
		// xmm
		//
		struct _XMM
		{
			//
			// header
			//
			M128A															Header[2];

			//
			// legacy
			//
			M128A															Legacy[8];

			//
			// xmm0
			//
			M128A															Xmm0;

			//
			// xmm1
			//
			M128A															Xmm1;

			//
			// xmm2
			//
			M128A															Xmm2;

			//
			// xmm3
			//
			M128A															Xmm3;

			//
			// xmm4
			//
			M128A															Xmm4;

			//
			// xmm5
			//
			M128A															Xmm5;

			//
			// xmm6
			//
			M128A															Xmm6;

			//
			// xmm7
			//
			M128A															Xmm7;

			//
			// xmm8
			//
			M128A															Xmm8;

			//
			// xmm9
			//
			M128A															Xmm9;

			//
			// xmm10
			//
			M128A															Xmm10;

			//
			// xmm11
			//
			M128A															Xmm11;

			//
			// xmm12
			//
			M128A															Xmm12;

			//
			// xmm13
			//
			M128A															Xmm13;

			//
			// xmm14
			//
			M128A															Xmm14;

			//
			// xmm15
			//
			M128A															Xmm15;
		}Xmm;
	};

	//
	// vector registers. offset = 0x300
	//
	M128A																	VectorRegister[26];

	//
	// vector control. offset = 0x4a0
	//
	UINT64																	VectorControl;

	//
	// debug control
	//
	UINT64																	DebugControl;

	//
	// last branch to
	//
	UINT64																	LastBranchToRip;

	//
	// last branch from
	//
	UINT64																	LastBranchFromRip;

	//
	// last exception to
	//
	UINT64																	LastExceptionToRip;

	//
	// last exception from
	//
	UINT64																	LastExceptionFromRip;
}CONTEXT;

//
// special registers. size = 0xe0
//
typedef struct _KSPECIAL_REGISTERS
{
	//
	// cr0
	//
	UINT64																	Cr0;

	//
	// cr2
	//
	UINT64																	Cr2;

	//
	// cr3
	//
	UINT64																	Cr3;

	//
	// cr4
	//
	UINT64																	Cr4;

	//
	// dr0
	//
	UINT64																	KernelDr0;

	//
	// dr1
	//
	UINT64																	KernelDr1;

	//
	// dr2
	//
	UINT64																	KernelDr2;

	//
	// dr3
	//
	UINT64																	KernelDr3;

	//
	// dr6
	//
	UINT64																	KernelDr6;

	//
	// dr7
	//
	UINT64																	KernelDr7;

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
	// MxCsr
	//
	UINT32																	MxCsr;

	//
	// debug control
	//
	UINT64																	DebugControl;

	//
	// last branch to rip
	//
	UINT64																	LastBranchToRip;

	//
	// last branch from rip
	//
	UINT64																	LastBranchFromRip;

	//
	// last exception to rip
	//
	UINT64																	LastExceptionToRip;

	//
	// last exception from rip
	//
	UINT64																	LastExceptionFromRip;

	//
	// cr8
	//
	UINT64																	Cr8;

	//
	// gs base
	//
	UINT64																	MsrGsBase;

	//
	// gs swap
	//
	UINT64																	MsrGsSwap;

	//
	// start
	//
	UINT64																	MsrStart;

	//
	// l-start
	//
	UINT64																	MsrLStart;

	//
	// c-start
	//
	UINT64																	MsrCStart;

	//
	// syscall mask
	//
	UINT64																	MsrSyscallMask;
}KSPECIAL_REGISTERS;

//
// processor state
//
typedef struct _KPROCESSOR_STATE
{
	//
	// special register
	//
	KSPECIAL_REGISTERS														SpecialRegisters;

	//
	// padding
	//
	UINT64																	Padding;

	//
	// context frame. offset = 0xe0
	//
	CONTEXT																	ContextFrame;
}KPROCESSOR_STATE;

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
	// padding
	//
	UINT32																	Reserved0;

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
	// number. offset = 0x20
	//
	INT8																	Number;

	//
	// padding
	//
	INT8																	Reserved1;

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
	// padding, offset = 0x2c
	//
	UINT8																	Reserved2[0x14];

	//
	// processor state, offset = 0x40
	//
	KPROCESSOR_STATE														ProcessorState;

	//
	// reserved
	//
	UINT8																	Reserved3[0x3530];
}KPRCB;

//
// trap frame
//
typedef struct _KTRAP_FRAME
{
	//
	// P1Home
	//
	UINT64																	P1Home;

	//
	// P2Home
	//
	UINT64																	P2Home;

	//
	// P3Home
	//
	UINT64																	P3Home;

	//
	// P4Home
	//
	UINT64																	P4Home;

	//
	// P5Home
	//
	UINT64																	P5Home;

	//
	// prev mode
	//
	UINT8																	PreviousMode;

	//
	// prev irql
	//
	UINT8																	PreviousIrql;

	//
	// fault indicator
	//
	UINT8																	FaultIndicator;

	//
	// exception active
	//
	UINT8																	ExceptionActive;

	//
	// MxCsr
	//
	UINT32																	MxCsr;

	//
	// Rax
	//
	UINT64																	Rax;

	//
	// Rcx
	//
	UINT64																	Rcx;

	//
	// Rdx
	//
	UINT64																	Rdx;

	//
	// R8
	//
	UINT64																	R8;

	//
	// R9
	//
	UINT64																	R9;

	//
	// R10
	//
	UINT64																	R10;

	//
	// R11
	//
	UINT64																	R11;

	//
	// gs
	//
	union
	{
		//
		// gs base
		//
		UINT64																GsBase;

		//
		// gs swap
		//
		UINT64																GsSwap;
	};

	//
	// xmm0
	//
	M128A																	Xmm0;

	//
	// xmm1
	//
	M128A																	Xmm1;

	//
	// xmm2
	//
	M128A																	Xmm2;

	//
	// xmm3
	//
	M128A																	Xmm3;

	//
	// xmm4
	//
	M128A																	Xmm4;

	//
	// xmm5
	//
	M128A																	Xmm5;

	union
	{
		//
		// fault address
		//
		UINT64																FaultAddress;

		//
		// context record
		//
		UINT64																ContextRecord;

		//
		// time stamp
		//
		UINT64																TimeStampCKCL;
	};

	//
	// dr0
	//
	UINT64																	Dr0;

	//
	// dr1
	//
	UINT64																	Dr1;

	//
	// dr2
	//
	UINT64																	Dr2;

	//
	// dr3
	//
	UINT64																	Dr3;

	//
	// dr6
	//
	UINT64																	Dr6;

	//
	// dr7
	//
	UINT64																	Dr7;

	union
	{
		struct _LAST_BRANCH
		{
			//
			// debug control
			//
			UINT64															DebugControl;

			//
			// last branch to rip
			//
			UINT64															LastBranchToRip;

			//
			// last branch from rip
			//
			UINT64															LastBranchFromRip;

			//
			// last exception to rip
			//
			UINT64															LastExceptionToRip;

			//
			// last exception from rip
			//
			UINT64															LastExceptionFromRip;
		}BranchInfo;

		struct _LAST_BRANCH_CONTROL
		{
			//
			// control
			//
			UINT64															LastBranchControl;

			//
			// msr
			//
			UINT64															LastBranchMsr;
		}BranchControl;
	};

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
	// gs
	//
	UINT16																	SegGs;

	//
	// trap frame
	//
	UINT64																	TrapFrame;

	//
	// Rbx
	//
	UINT64																	Rbx;

	//
	// Rdi
	//
	UINT64																	Rdi;

	//
	// Rsi
	//
	UINT64																	Rsi;

	//
	// rbp
	//
	UINT64																	Rbp;

	union
	{
		//
		// error code,if hardware did not push an error code,we push a zero on the stack
		//
		UINT64																ErrCode;

		//
		// exception frame
		//
		UINT64																ExceptionFrame;
	};

	//
	// hardware saved eip
	//
	UINT64																	Rip;

	//
	// hardware save cs
	//
	UINT16																	SegCs;

	//
	// padding
	//
	UINT8																	Reserved0;

	//
	// logging
	//
	UINT8																	Logging;

	//
	// padding
	//
	UINT32																	Reserved1;

	//
	// hardware saved eflags
	//
	UINT32																	EFlags;

	//
	// padding
	//
	UINT32																	Reserved2;

	//
	// hardware saved esp
	//
	UINT64																	HardwareRsp;

	//
	// hardware saved ss
	//
	UINT16																	HardwareSegSs;

	//
	// padding
	//
	UINT16																	Reserved3;

	//
	// patch
	//
	UINT32																	CodePatchCycle;
}KTRAP_FRAME;

//
// exception frame
//
typedef struct _KEXCEPTION_FRAME
{
	//
	// P1Home
	//
	UINT64																	P1Home;

	//
	// P2Home
	//
	UINT64																	P2Home;

	//
	// P3Home
	//
	UINT64																	P3Home;

	//
	// P4Home
	//
	UINT64																	P4Home;

	//
	// P5Home
	//
	UINT64																	P5Home;

	//
	// P6Home
	//
	UINT64																	InitialStack;

	//
	// Xmm6
	//
	M128A																	Xmm6;

	//
	// Xmm7
	//
	M128A																	Xmm7;

	//
	// Xmm8
	//
	M128A																	Xmm8;

	//
	// Xmm9
	//
	M128A																	Xmm9;

	//
	// Xmm10
	//
	M128A																	Xmm10;

	//
	// Xmm11
	//
	M128A																	Xmm11;

	//
	// Xmm12
	//
	M128A																	Xmm12;

	//
	// Xmm13
	//
	M128A																	Xmm13;

	//
	// Xmm14
	//
	M128A																	Xmm14;

	//
	// Xmm15
	//
	M128A																	Xmm15;

	//
	// trap frame
	//
	UINT64																	TrapFrame;

	//
	// callback buffer
	//
	UINT64																	CallbackBuffer;

	//
	// output buffer
	//
	UINT64																	OutputBuffer;

	//
	// output length
	//
	UINT64																	OutputLength;

	//
	// mxcsr
	//
	UINT64																	MxCsr;

	//
	// rbp
	//
	UINT64																	Rbp;

	//
	// rbx
	//
	UINT64																	Rbx;

	//
	// rdi
	//
	UINT64																	Rdi;

	//
	// rsi
	//
	UINT64																	Rsi;

	//
	// r12
	//
	UINT64																	R12;

	//
	// r13
	//
	UINT64																	R13;

	//
	// r14
	//
	UINT64																	R14;

	//
	// r15
	//
	UINT64																	R15;
}KEXCEPTION_FRAME;

//
// pcr
//
typedef struct _KPCR
{
	//
	// gdt base
	//
	VOID*																	GdtBase;

	//
	// tss base
	//
	VOID*																	TssBase;

	//
	// user rsp
	//
	UINT64																	UserRsp;

	//
	// self
	//
	struct _KPCR*															Self;

	//
	// current prcb
	//
	KPRCB*																	CurrentPrcb;

	//
	// lock array
	//
	VOID*																	LockArray;

	//
	// used self
	//
	struct _KPCR*															UsedSelf;

	//
	// idt base
	//
	KIDTENTRY*																IdtBase;

	//
	// unused
	//
	UINT64																	Unused[2];

	//
	// irql
	//
	UINT8																	Irql;

	//
	// second level cache associativity
	//
	UINT8																	SecondLevelCacheAssociativity;

	//
	// OBSOLETE number
	//
	UINT8																	ObsoleteNumber;

	//
	// padding
	//
	UINT8																	Padding0;

	//
	// unused
	//
	UINT32																	Unused0[3];

	//
	// major version
	//
	UINT16																	MajorVersion;

	//
	// minor version
	//
	UINT16																	MinorVersion;

	//
	// stall scale factor
	//
	UINT32																	StallScaleFactor;

	//
	// unused
	//
	VOID*																	Unused1[3];

	//
	// kernel reserved
	//
	UINT32																	KernelReserved[15];

	//
	// second level cache size
	//
	UINT32																	SecondLevelCacheSize;

	//
	// hal reserved
	//
	UINT32																	HalReserved[16];

	//
	// unused
	//
	UINT32																	Unused2[2];

	//
	// kd version block
	//
	VOID*																	KdVersionBlock;

	//
	// unused
	//
	VOID*																	Unused3;

	//
	// pcr algin
	//
	UINT32																	PcrAlign1[26];

	//
	// prcb
	//
	KPRCB																	Prcb;
}KPCR;

//
// control sets for supported architectures
//
typedef struct _X64_DBGKD_CONTROL_SET
{
	//
	// trace flag
	//
	UINT32																	TraceFlag;

	//
	// dr7
	//
	UINT64																	Dr7;

	//
	// symbol start
	//
	UINT64																	CurrentSymbolStart;

	//
	// symbol end
	//
	UINT64																	CurrentSymbolEnd;
}DBGKD_CONTROL_SET;

#include "poppack.h"

//
// control report
//
typedef struct _X64_DBGKD_CONTROL_REPORT
{
	//
	// kerenl dr6
	//
	UINT64																	Dr6;

	//
	// kernel dr7
	//
	UINT64																	Dr7;

	//
	// eflags
	//
	UINT32																	EFlags;

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
}DBGKD_CONTROL_REPORT;

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
VOID BOOTAPI ArchSetIdtEntry(UINT64 base, UINT32 index, UINT32 segCs, VOID* offset, UINT32 access);