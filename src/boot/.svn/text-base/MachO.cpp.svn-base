//********************************************************************
//	created:	12:11:2009   19:15
//	filename: 	MachO.cpp
//	author:		tiamo
//	purpose:	mach-o
//********************************************************************

#include "stdafx.h"

//
// define
//
#define FAT_MAGIC															0xcafebabe
#define FAT_CIGAM															0xbebafeca
#define	MH_MAGIC															0xfeedface
#define MH_MAGIC_64															0xfeedfacf
#define MH_CIGAM															0xcefaedfe
#define MH_CIGAM_64															0xcffaedfe
#define MACH_O_COMMAND_SEGMENT32											0x01
#define MACH_O_COMMAND_SYMTAB												0x02
#define	MACH_O_COMMAND_UNIX_THREAD											0x05
#define MACH_O_COMMAND_DYSYMTAB												0x0b
#define MACH_O_COMMAND_SEGMENT64											0x19

#define THREAD_STATE_FLAVOR_X86												1
#define THREAD_STATE_FLAVOR_X64												4
#define MH_PIE																0x200000
#define VM_PROT_WRITE														2

//
// fat header
//
#include <pshpack1.h>
typedef struct _FAT_HEADER
{
	//
	// magic
	//
	UINT32																	Magic;

	//
	// count
	//
	UINT32																	ArchHeadersCount;
}FAT_HEADER;

//
// fat arch header
//
typedef struct _FAT_ARCH_HEADER
{
	//
	// cpu type
	//
	UINT32																	CpuType;

	//
	// cpu sub type
	//
	UINT32																	CpuSubType;

	//
	// offset
	//
	UINT32																	OffsetInFile;

	//
	// size
	//
	UINT32																	Size;

	//
	// align
	//
	UINT32																	Align;
}FAT_ARCH_HEADER;

//
// mach header
//
typedef struct _MACH_HEADER
{
	//
	// magic
	//
	UINT32																	Magic;

	//
	// cpu type
	//
	UINT32																	CpuType;

	//
	// cpu sub type
	//
	UINT32																	CpuSubType;

	//
	// file type
	//
	UINT32																	FileType;

	//
	// cmds
	//
	UINT32																	CommandsCount;

	//
	// length of cmds
	//
	UINT32																	CommandsLength;

	//
	// flags
	//
	UINT32																	Flags;
}MACH_HEADER;

//
// mach header64
//
typedef struct _MACH_HEADER64
{
	//
	// magic
	//
	UINT32																	Magic;

	//
	// cpu type
	//
	UINT32																	CpuType;

	//
	// cpu sub type
	//
	UINT32																	CpuSubType;

	//
	// file type
	//
	UINT32																	FileType;

	//
	// cmds
	//
	UINT32																	CommandsCount;

	//
	// length of cmds
	//
	UINT32																	CommandsLength;

	//
	// flags
	//
	UINT32																	Flags;

	//
	// reserved
	//
	UINT32																	Reserved;
}MACH_HEADER64;

//
// command header
//
typedef struct _LOAD_COMMAND_HEADER
{
	//
	// type
	//
	UINT32																	CommandType;

	//
	// length
	//
	UINT32																	CommandLength;
}LOAD_COMMAND_HEADER;

//
// thread command
//
typedef struct _THREAD_COMMAND
{
	//
	// header
	//
	LOAD_COMMAND_HEADER														Header;

	//
	// thread state flover
	//
	UINT32																	ThreadStateFlavor;

	//
	// count
	//
	UINT32																	ThreadStateCount;

	//
	// thread state
	//
	union _THREAD_STATE
	{
		//
		// x86
		//
		struct _THREAD_STATE_X86
		{
			//
			// eax
			//
			UINT32															Eax;

			//
			// ebx
			//
			UINT32															Ebx;

			//
			// ecx
			//
			UINT32															Ecx;

			//
			// edx
			//
			UINT32															Edx;

			//
			// edi
			//
			UINT32															Edi;

			//
			// esi
			//
			UINT32															Esi;

			//
			// ebp
			//
			UINT32															Ebp;

			//
			// esp
			//
			UINT32															Esp;

			//
			// ss
			//
			UINT32															SegSs;

			//
			// eflags
			//
			UINT32															Eflags;

			//
			// eip
			//
			UINT32															Eip;

			//
			// cs
			//
			UINT32															SegCs;

			//
			// ds
			//
			UINT32															SegDs;

			//
			// es
			//
			UINT32															SegEs;

			//
			// fs
			//
			UINT32															SegFs;

			//
			// gs
			//
			UINT32															SegGs;
		}X86;

		//
		// x64
		//
		struct _THREAD_STATE_X64
		{
			//
			// rax
			//
			UINT64															Rax;

			//
			// rbx
			//
			UINT64															Rbx;

			//
			// rcx
			//
			UINT64															Rcx;

			//
			// rdx
			//
			UINT64															Rdx;

			//
			// rdi
			//
			UINT64															Rdi;

			//
			// rsi
			//
			UINT64															Rsi;

			//
			// rbp
			//
			UINT64															Rbp;

			//
			// rsp
			//
			UINT64															Rsp;

			//
			// r8
			//
			UINT64															R8;

			//
			// r9
			//
			UINT64															R9;

			//
			// r10
			//
			UINT64															R10;

			//
			// r11
			//
			UINT64															R11;

			//
			// r12
			//
			UINT64															R12;

			//
			// r13
			//
			UINT64															R13;

			//
			// r14
			//
			UINT64															R14;

			//
			// r15
			//
			UINT64															R15;

			//
			// rip
			//
			UINT64															Rip;

			//
			// eflags
			//
			UINT64															Eflags;

			//
			// cs
			//
			UINT64															SegCs;

			//
			// fs
			//
			UINT64															SegFs;

			//
			// gs
			//
			UINT64															SegGs;
		}X64;
	}ThreadState;
}THREAD_COMMAND;

//
// segment
//
typedef struct _SEGMENT_COMMAND
{
	//
	// header
	//
	LOAD_COMMAND_HEADER														Header;

	//
	// name
	//
	CHAR8																	Name[16];

	//
	// virtual address
	//
	UINT32																	VirtualAddress;

	//
	// virtual size
	//
	UINT32																	VirtualSize;

	//
	// file offset
	//
	UINT32																	FileOffset;

	//
	// file length
	//
	UINT32																	FileSize;
}SEGMENT_COMMAND;

//
// segment
//
typedef struct _SEGMENT_COMMAND64
{
	//
	// header
	//
	LOAD_COMMAND_HEADER														Header;

	//
	// name
	//
	CHAR8																	Name[16];

	//
	// virtual address
	//
	UINT64																	VirtualAddress;

	//
	// virtual size
	//
	UINT64																	VirtualSize;

	//
	// file offset
	//
	UINT64																	FileOffset;

	//
	// file length
	//
	UINT64																	FileSize;

	//
	// max protection
	//
	UINT32																	MaxProtection;

	//
	// initial protection
	//
	UINT32																	InitialProtection;

	//
	// section count
	//
	UINT32																	SectionCount;

	//
	// flags
	//
	UINT32																	Flags;
}SEGMENT_COMMAND64;

//
// section 64
//
typedef struct _SECTION64
{
	//
	// section name
	//
	CHAR8																	SectionName[16];

	//
	// segment name
	//
	CHAR8																	SegmentName[16];

	//
	// address
	//
	UINT64																	Address;

	//
	// size
	//
	UINT64																	Size;

	//
	// offset
	//
	UINT32																	Offset;

	//
	// align
	//
	UINT32																	Align;

	//
	// relocation offset
	//
	UINT32																	RelocationOffset;

	//
	// relocation count
	//
	UINT32																	RelocationCount;

	//
	// flags
	//
	UINT32																	Flags;

	//
	// reserved
	//
	UINT32																	Reserved[3];
}SECTION64;

//
// SYMTAB command
//
typedef struct _SYMTAB_COMMAND
{
	//
	// header
	//
	LOAD_COMMAND_HEADER														Header;

	//
	// symbol table offset
	//
	UINT32																	SymbolTableOffset;

	//
	// symbol table count
	//
	UINT32																	SymbolCount;

	//
	// string table offset
	//
	UINT32																	StringTableOffset;

	//
	// string table size
	//
	UINT32																	StringTableSize;
}SYMTAB_COMMAND;

//
// SYMTAB entry
//
typedef struct _SYMTAB_ENTRY64
{
	//
	// string table index
	//
	UINT32																	StringIndex;

	//
	// type
	//
	UINT8																	Type;

	//
	// section index
	//
	UINT8																	SectionIndex;

	//
	// description
	//
	UINT16																	Description;

	//
	// value
	//
	UINT64																	Value;
}SYMTAB_ENTRY64;

//
// DYSYMTAB command
//
typedef struct _DYSYMTAB_COMMAND
{
	//
	// header
	//
	LOAD_COMMAND_HEADER														Header;

	//
	// first local symbol
	//
	UINT32																	FirstLocalSymbol;

	//
	// local symbol count
	//
	UINT32																	LocalSymbolCount;

	//
	// first external symbol
	//
	UINT32																	FirstExternalSymbol;

	//
	// external symbol count
	//
	UINT32																	ExternalSymbolCount;

	//
	// first undefined symbol
	//
	UINT32																	FirstUndefinedSymbol;

	//
	// undefined symbol count
	//
	UINT32																	UndefinedSymbolCount;

	//
	// content data offset
	//
	UINT32																	ContentDataOffset;

	//
	// content data size
	//
	UINT32																	ContentDataSize;

	//
	// module data offset
	//
	UINT32																	ModuleDataOffset;

	//
	// module data size
	//
	UINT32																	ModuleDataSize;

	//
	// external reference table offset
	//
	UINT32																	ExternalReferenceOffset;

	//
	// external reference table size
	//
	UINT32																	ExternalReferenceCount;

	//
	// indirect table offset
	//
	UINT32																	IndirectTableOffset;

	//
	// indirect symbol count
	//
	UINT32																	IndirectSymbolCount;

	//
	// external relocation table offset
	//
	UINT32																	ExternalRelocationOffset;

	//
	// external relocation count
	//
	UINT32																	ExternalRelocationCount;

	//
	// local relocation table offset
	//
	UINT32																	LocalRelocationOffset;

	//
	// local relocation count
	//
	UINT32																	LocalRelocationCount;
}DYSYMTAB_COMMAND;

//
// relocation info
//
typedef struct _RELOCATION_INFO
{
	//
	// section offset
	//
	INT32																	SectionOffset;

	//
	// symbol table index
	//
	UINT32																	SymbolTableIndex : 24;

	//
	// pc relative
	//
	UINT32																	PCRelative : 1;

	//
	// length
	//
	UINT32																	Length : 2;

	//
	// external
	//
	UINT32																	External : 1;

	//
	// type
	//
	UINT32																	Type : 4;
}RELOCATION_INFO;
#include <poppack.h>

//
// load thin fat file info
//
EFI_STATUS MachpLoadMachOThinFatFile(IO_FILE_HANDLE* fileHandle, UINT64* offsetInFile, UINTN* dataSize)
{
	EFI_STATUS status														= EFI_SUCCESS;

	__try
	{
		//
		// seek file
		//
		if(EFI_ERROR(status = IoSetFilePosition(fileHandle, 0)))
			try_leave(NOTHING);

		//
		// read mach header
		//
		MACH_HEADER machHeader												= {0};
		UINTN readLength													= 0;
		if(EFI_ERROR(status = IoReadFile(fileHandle, &machHeader, sizeof(machHeader), &readLength, FALSE)))
			try_leave(NOTHING);

		//
		// check length
		//
		if(readLength < sizeof(machHeader))
			try_leave(status = EFI_DEVICE_ERROR);

		//
		// seek back
		//
		if(EFI_ERROR(status = IoSetFilePosition(fileHandle, 0)))
			try_leave(NOTHING);

		//
		// get file size
		//
		UINT64 fileSize														= 0;
		if(EFI_ERROR(status = IoGetFileSize(fileHandle, &fileSize)))
			try_leave(NOTHING);

		//
		// check magic
		//
		if(machHeader.Magic != MH_MAGIC_64)
			try_leave(status = EFI_NOT_FOUND);

		//
		// check cpu arch type
		//
		if(machHeader.CpuType != 0x1000007)
			try_leave(status = EFI_NOT_FOUND);

		//
		// the whole file
		//
		if(offsetInFile)
			*offsetInFile													= 0;
		if(dataSize)
			*dataSize														= static_cast<UINT32>(fileSize);
	}
	__finally
	{
	}

	return status;
}

//
// load thin fat file info
//
EFI_STATUS MachLoadThinFatFile(IO_FILE_HANDLE* fileHandle, UINT64* offsetInFile, UINTN* dataSize)
{
	EFI_STATUS status														= EFI_SUCCESS;

	__try
	{
		//
		// read fat header
		//
		FAT_HEADER fatHeader												= {0};
		UINTN readLength													= 0;
		if(EFI_ERROR(status = IoReadFile(fileHandle, &fatHeader, sizeof(fatHeader), &readLength, FALSE)))
			try_leave(NOTHING);

		//
		// check read length
		//
		if(readLength < sizeof(fatHeader))
			try_leave(status = EFI_DEVICE_ERROR);

		//
		// check fat header
		//
		BOOLEAN needSwap													= TRUE;
		if(fatHeader.Magic == FAT_MAGIC)
			needSwap														= FALSE;
		else if(fatHeader.Magic == FAT_CIGAM)
			fatHeader.ArchHeadersCount										= SWAP32(fatHeader.ArchHeadersCount);
		else
			try_leave(status = MachpLoadMachOThinFatFile(fileHandle, offsetInFile, dataSize));

		//
		// read arch headers
		//
		FAT_ARCH_HEADER x64ArchHeader										= {0};
		FAT_ARCH_HEADER curArchHeader										= {0};
		for(UINT32 i = 0; i < fatHeader.ArchHeadersCount; i ++)
		{
			//
			// read it
			//
			if(EFI_ERROR(status = IoReadFile(fileHandle, &curArchHeader, sizeof(curArchHeader), &readLength, FALSE)))
				try_leave(NOTHING);

			//
			// check length
			//
			if(readLength < sizeof(curArchHeader))
				try_leave(status = EFI_DEVICE_ERROR);

			//
			// swap
			//
			if(needSwap)
			{
				curArchHeader.CpuType										= SWAP32(curArchHeader.CpuType);
				curArchHeader.OffsetInFile									= SWAP32(curArchHeader.OffsetInFile);
				curArchHeader.Size											= SWAP32(curArchHeader.Size);
			}

			//
			// check arch
			//
			if(curArchHeader.CpuType == 0x1000007)
			{
				x64ArchHeader												= curArchHeader;
				break;
			}
		}

		//
		// not found
		//
		if(!x64ArchHeader.Size)
			try_leave(status = EFI_NOT_FOUND);

		//
		// seek file
		//
		if(x64ArchHeader.OffsetInFile)
			IoSetFilePosition(fileHandle, x64ArchHeader.OffsetInFile);

		//
		// output
		//
		if(offsetInFile)
			*offsetInFile													= x64ArchHeader.OffsetInFile;
		if(dataSize)
			*dataSize														= x64ArchHeader.Size;
	}
	__finally
	{
	}

	return status;
}

//
// get first segment64
//
SEGMENT_COMMAND64* MachpGetFirstSegment64(MACH_HEADER64* machHeader)
{
	SEGMENT_COMMAND64* segment64											= Add2Ptr(machHeader, sizeof(MACH_HEADER64), SEGMENT_COMMAND64*);
	for(UINT32 i = 0; i < machHeader->CommandsCount; i ++, segment64 = Add2Ptr(segment64, segment64->Header.CommandLength, SEGMENT_COMMAND64*))
	{
		if(segment64->Header.CommandType == MACH_O_COMMAND_SEGMENT64)
			return segment64;
	}
	return nullptr;
}

//
// get next segment64
//
SEGMENT_COMMAND64* MachpGetNextSegment64(MACH_HEADER64* machHeader, SEGMENT_COMMAND64* segment64)
{
	UINT32 i																= 0;
	LOAD_COMMAND_HEADER* loadCommand										= Add2Ptr(machHeader, sizeof(MACH_HEADER64), LOAD_COMMAND_HEADER*);
	for(; i < machHeader->CommandsCount && static_cast<VOID*>(loadCommand) != static_cast<VOID*>(segment64); i ++)
		loadCommand															= Add2Ptr(loadCommand, loadCommand->CommandLength, LOAD_COMMAND_HEADER*);

	for(i += 1, loadCommand = Add2Ptr(loadCommand, loadCommand->CommandLength, LOAD_COMMAND_HEADER*); i < machHeader->CommandsCount; i ++)
	{
		if(loadCommand->CommandType == MACH_O_COMMAND_SEGMENT64)
			return _CR(loadCommand, SEGMENT_COMMAND64, Header);

		loadCommand															= Add2Ptr(loadCommand, loadCommand->CommandLength, LOAD_COMMAND_HEADER*);
	}
	return nullptr;
}

//
// get first section64
//
SECTION64* MachpGetFirstSection64(SEGMENT_COMMAND64* segment64)
{
	return segment64 && segment64->SectionCount ? Add2Ptr(segment64, sizeof(SEGMENT_COMMAND64), SECTION64*) : nullptr;
}

//
// get next section64
//
SECTION64* MachpGetNextSection64(SEGMENT_COMMAND64* segment64, SECTION64* section64)
{
	if(!segment64 || !segment64->SectionCount)
		return nullptr;

	UINTN index																= (static_cast<UINT8*>(static_cast<VOID*>(section64)) - static_cast<UINT8*>(static_cast<VOID*>(segment64 + 1))) / sizeof(SECTION64);
	return index + 1 >= segment64->SectionCount ? nullptr : section64 + 1;
}

//
// load mach-o
//
EFI_STATUS MachLoadMachO(IO_FILE_HANDLE* fileHandle, BOOLEAN useKernelMemory, MACH_O_LOADED_INFO* loadedInfo)
{
	EFI_STATUS status														= EFI_SUCCESS;
	VOID* commandsBuffer													= 0;

	__try
	{
		//
		// get info
		//
		UINT64 machOffset													= 0;
		UINTN machLength													= 0;
		if(EFI_ERROR(status = MachLoadThinFatFile(fileHandle, &machOffset, &machLength)))
			try_leave(NOTHING);

		//
		// check length
		//
		if(!machLength)
			try_leave(status = EFI_NOT_FOUND);

		//
		// read mach header
		//
		MACH_HEADER64 machHeader											= {0};
		UINTN readLength													= 0;
		if(EFI_ERROR(status = IoReadFile(fileHandle, &machHeader, sizeof(MACH_HEADER), &readLength, FALSE)))
			try_leave(NOTHING);

		//
		// check length
		//
		if(readLength != sizeof(MACH_HEADER))
			try_leave(status = EFI_LOAD_ERROR);

		//
		// check signature
		//
		if(machHeader.Magic != MH_MAGIC_64)
			try_leave(CsPrintf(CHAR8_CONST_STRING("Booting from 32-bit kernelcache is not supported.\n")); status = EFI_LOAD_ERROR);

		//
		// read 64bit header
		//
		if(EFI_ERROR(status = IoReadFile(fileHandle, &machHeader.Reserved, sizeof(machHeader.Reserved), &readLength, FALSE)))
			try_leave(NOTHING);

		//
		// length check
		//
		if(readLength != sizeof(machHeader.Reserved))
			try_leave(status = EFI_LOAD_ERROR);

		//
		// check ASLR
		//
		if(BlTestBootMode(BOOT_MODE_ASLR) && !(machHeader.Flags & MH_PIE))
			LdrSetupASLR(FALSE, 0);

		//
		// allocate commands buffer
		//
		commandsBuffer														= MmAllocatePool(machHeader.CommandsLength);
		if(!commandsBuffer)
			try_leave(status = EFI_OUT_OF_RESOURCES);

		//
		// read commands
		//
		if(EFI_ERROR(status = IoReadFile(fileHandle, commandsBuffer, machHeader.CommandsLength, &readLength, FALSE)))
			try_leave(NOTHING);

		//
		// length check
		//
		if(readLength != machHeader.CommandsLength)
			try_leave(status = EFI_LOAD_ERROR);

		//
		// process commands
		//
		VOID* dataSegment													= nullptr;
		VOID* linkEditSegment												= nullptr;
		UINT64 linkEditSegmentOffset										= 0;
		LOAD_COMMAND_HEADER* theCommand										= static_cast<LOAD_COMMAND_HEADER*>(commandsBuffer);
		for(UINT32 i = 0; i < machHeader.CommandsCount; i ++, theCommand = Add2Ptr(theCommand, theCommand->CommandLength, LOAD_COMMAND_HEADER*))
		{
			//
			// process it
			//
			switch(theCommand->CommandType)
			{
			case MACH_O_COMMAND_UNIX_THREAD:
				{
					//
					// get rip
					//
					THREAD_COMMAND* threadCommand							= _CR(theCommand, THREAD_COMMAND, Header);
					if(threadCommand->ThreadStateFlavor != THREAD_STATE_FLAVOR_X64)
						try_leave(CsPrintf(CHAR8_CONST_STRING("Only 64-bit version of LC_UNIXTHREAD is supported.\n")); status = EFI_LOAD_ERROR);
					else 
						loadedInfo->EntryPointVirtualAddress				= threadCommand->ThreadState.X64.Rip + LdrGetASLRDisplacement();

					loadedInfo->EntryPointPhysicalAddress					= LdrStaticVirtualToPhysical(loadedInfo->EntryPointVirtualAddress);
				}
				break;

			case MACH_O_COMMAND_SYMTAB:
				{
					SYMTAB_COMMAND* symbolTableCommand						= _CR(theCommand, SYMTAB_COMMAND, Header);
					if(LdrGetASLRDisplacement())
					{
						SYMTAB_ENTRY64* symbolEntry							= Add2Ptr(linkEditSegment, symbolTableCommand->SymbolTableOffset - linkEditSegmentOffset, SYMTAB_ENTRY64*);
						for(UINT32 i = 0; i < symbolTableCommand->SymbolCount; i ++, symbolEntry ++)
						{
							if(symbolEntry->Type <= 0x1f)
								symbolEntry->Value							+= LdrGetASLRDisplacement();
						}
					}
				}
				break;

			case MACH_O_COMMAND_DYSYMTAB:
				{
					DYSYMTAB_COMMAND* dynamicSymbolTableCommand				= _CR(theCommand, DYSYMTAB_COMMAND, Header);
					if(dynamicSymbolTableCommand->LocalRelocationCount && LdrGetASLRDisplacement())
					{
						if(!dataSegment || !linkEditSegment)
							try_leave(status = EFI_LOAD_ERROR);

						RELOCATION_INFO* relocationInfo						= Add2Ptr(linkEditSegment, dynamicSymbolTableCommand->LocalRelocationOffset - linkEditSegmentOffset, RELOCATION_INFO*);
						for(UINT32 i = 0; i < dynamicSymbolTableCommand->LocalRelocationCount; i ++, relocationInfo ++)
						{
							//
							// In final linked images, there are only two valid relocation kinds:
							//		r_type=X86_64_RELOC_UNSIGNED, r_length=3, r_pcrel=0, r_extern=1, r_symbolnum=sym_index
							//			This tells dyld to add the address of a symbol to a pointer sized (8-byte)
							//			piece of data (i.e on disk the 8-byte piece of data contains the addend). The
							//			r_symbolnum contains the index into the symbol table of the target symbol.
							//
							//		r_type=X86_64_RELOC_UNSIGNED, r_length=3, r_pcrel=0, r_extern=0, r_symbolnum=0
							//			This tells dyld to adjust the pointer sized (8-byte) piece of data by the amount
							//			the containing image was loaded from its base address (e.g. slide).
							//
							if(relocationInfo->Type || relocationInfo->Length != 3 || relocationInfo->PCRelative || relocationInfo->External)
								try_leave(status = EFI_LOAD_ERROR);

							UINT64* address									= Add2Ptr(dataSegment, static_cast<INT64>(relocationInfo->SectionOffset), UINT64*);
							*address										+= LdrGetASLRDisplacement();
						}
					}
				}
				break;
				
			case MACH_O_COMMAND_SEGMENT64:
				{
					//
					// get info
					//
					SEGMENT_COMMAND64* segmentCommand64						= _CR(theCommand, SEGMENT_COMMAND64, Header);
					UINT64 segmentVirtualAddress							= segmentCommand64->VirtualAddress + LdrGetASLRDisplacement();
					UINT64 segmentVirtualSize								= segmentCommand64->VirtualSize;
					UINT64 segmentFileSize									= segmentCommand64->FileSize;
					UINT64 segmentFileOffset								= segmentCommand64->FileOffset;

					//
					// empty segment
					//
					if(!segmentVirtualSize)
						break;

					//
					// seek file
					//
					if(EFI_ERROR(status = IoSetFilePosition(fileHandle, machOffset + segmentFileOffset)))
						try_leave(NOTHING);

					//
					// allocate buffer
					//
					UINTN allocatedLength									= static_cast<UINTN>(segmentVirtualSize);
					UINT64 virtualAddress									= segmentVirtualAddress;
					UINT64 physicalAddress									= useKernelMemory ? MmAllocateKernelMemory(&allocatedLength, &virtualAddress) : MmAllocateLoaderData(&allocatedLength, &virtualAddress);
					if(!physicalAddress)
						try_leave(status = EFI_OUT_OF_RESOURCES);

					//
					// read in
					//
					UINTN fileLength										= static_cast<UINTN>(segmentVirtualSize > segmentFileSize ? segmentFileSize : segmentVirtualSize);
					if(fileLength && EFI_ERROR(status = IoReadFile(fileHandle, ArchConvertAddressToPointer(physicalAddress, VOID*), fileLength, &readLength, FALSE)))
						try_leave(NOTHING);

					//
					// zero out
					//
					if(readLength != allocatedLength)
						EfiBootServices->SetMem(Add2Ptr(physicalAddress, readLength, VOID*), allocatedLength - readLength, 0);

					//
					// first writable segment
					//
					if(!dataSegment && (segmentCommand64->InitialProtection & VM_PROT_WRITE))
						dataSegment											= ArchConvertAddressToPointer(physicalAddress, VOID*);

					//
					// link edit segment
					//
					if(!linkEditSegment && !strcmp(segmentCommand64->Name, CHAR8_CONST_STRING("__LINKEDIT")))
					{
						linkEditSegment										= ArchConvertAddressToPointer(physicalAddress, VOID*);
						linkEditSegmentOffset								= segmentFileOffset;
					}

					//
					// first __TEXT segment also contains MACH_HEADER
					//
					if(!strcmp(segmentCommand64->Name, CHAR8_CONST_STRING("__TEXT")))
					{
						//
						// save mach header
						//
						loadedInfo->ImageBasePhysicalAddress				= physicalAddress;
						loadedInfo->ImageBaseVirtualAddress					= virtualAddress;

						//
						// relocation for ASLR
						//
						if(LdrGetASLRDisplacement())
						{

							SEGMENT_COMMAND64* theSegment64					= MachpGetFirstSegment64(Add2Ptr(physicalAddress, 0, MACH_HEADER64*));
							while(theSegment64)
							{
								theSegment64->VirtualAddress				+= LdrGetASLRDisplacement();
								SECTION64* theSection64						= MachpGetFirstSection64(theSegment64);
								while(theSection64)
								{
									theSection64->Address					+= LdrGetASLRDisplacement();
									theSection64							= MachpGetNextSection64(theSegment64, theSection64);
								}
								theSegment64								= MachpGetNextSegment64(Add2Ptr(physicalAddress, 0, MACH_HEADER64*), theSegment64);
							}
						}
					}

					//
					// update min/max
					//
					if(!loadedInfo->MinVirtualAddress || virtualAddress < loadedInfo->MinVirtualAddress)
					{
						loadedInfo->MinVirtualAddress						= virtualAddress;
						loadedInfo->MinPhysicalAddress						= LdrStaticVirtualToPhysical(loadedInfo->MinVirtualAddress);
					}

					if(!loadedInfo->MaxVirtualAddress || loadedInfo->MaxVirtualAddress < virtualAddress + allocatedLength)
					{
						loadedInfo->MaxVirtualAddress						= virtualAddress + allocatedLength;
						loadedInfo->MaxPhysicalAddress						= LdrStaticVirtualToPhysical(loadedInfo->MaxVirtualAddress);
					}
				}
				break;
			}
		}

		//
		// save arch type
		//
		loadedInfo->ArchType												= machHeader.CpuType;
	}
	__finally
	{
		if(commandsBuffer)
			MmFreePool(commandsBuffer);
	}

	return status;
}

//
// get symbol virtual address by name
//
UINT64 MachFindSymbolVirtualAddressByName(MACH_O_LOADED_INFO* loadedInfo, CHAR8 CONST* symbolName)
{
	MACH_HEADER* machHeader													= ArchConvertAddressToPointer(loadedInfo->ImageBasePhysicalAddress, MACH_HEADER*);
	MACH_HEADER64* machHeader64												= ArchConvertAddressToPointer(loadedInfo->ImageBasePhysicalAddress, MACH_HEADER64*);
	BOOLEAN is64Bits														= machHeader->Magic == MH_MAGIC_64;
	LOAD_COMMAND_HEADER* commandHeader										= is64Bits ? Add2Ptr(machHeader64 + 1, 0, LOAD_COMMAND_HEADER*) : Add2Ptr(machHeader + 1, 0, LOAD_COMMAND_HEADER*);
	VOID* linkEditSegment													= nullptr;
	UINT64 linkEditSegmentOffset											= 0;
	SYMTAB_COMMAND* symbolTableCommand										= nullptr;
	for(UINT32 i = 0; i < machHeader->CommandsCount; i ++, commandHeader = Add2Ptr(commandHeader, commandHeader->CommandLength, LOAD_COMMAND_HEADER*))
	{
		if(commandHeader->CommandType == MACH_O_COMMAND_SEGMENT32 || commandHeader->CommandType == MACH_O_COMMAND_SEGMENT64)
		{
			SEGMENT_COMMAND64* segmentCommand64									= _CR(commandHeader, SEGMENT_COMMAND64, Header);
			SEGMENT_COMMAND* segmentCommand										= _CR(commandHeader, SEGMENT_COMMAND, Header);
			if(is64Bits ? strcmp(segmentCommand64->Name, CHAR8_CONST_STRING("__LINKEDIT")) : strcmp(segmentCommand->Name, CHAR8_CONST_STRING("__LINKEDIT")))
				continue;

			linkEditSegment													= ArchConvertAddressToPointer(LdrStaticVirtualToPhysical(is64Bits ? segmentCommand64->VirtualAddress : segmentCommand->VirtualAddress), VOID*);
			linkEditSegmentOffset											= is64Bits ? segmentCommand64->FileOffset : segmentCommand->FileOffset;
		}
		else if(commandHeader->CommandType == MACH_O_COMMAND_SYMTAB)
		{
			symbolTableCommand												= _CR(commandHeader, SYMTAB_COMMAND, Header);
		}
	}
	if(!linkEditSegment || !linkEditSegmentOffset || !symbolTableCommand)
		return 0;

	CHAR8 CONST* stringTable												= Add2Ptr(linkEditSegment, symbolTableCommand->StringTableOffset - linkEditSegmentOffset, CHAR8 CONST*);
	SYMTAB_ENTRY64* symbolEntry												= Add2Ptr(linkEditSegment, symbolTableCommand->SymbolTableOffset - linkEditSegmentOffset, SYMTAB_ENTRY64*);
	for(UINT32 i = 0; i < symbolTableCommand->SymbolCount; i ++, symbolEntry ++)
	{
		if(!strcmp(symbolName, stringTable + symbolEntry->StringIndex))
			return symbolEntry->Value;
	}
	return 0;
}