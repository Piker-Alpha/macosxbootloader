#
# 
#

%.o: %.c
	@echo "\t[$(CC)]\t$@";
	@$(CC) $(CFLAGS) -c $< -o $@

%.o: %.cpp
	@echo "\t[$(CXX)]\t$@";
	@$(CXX) $(CXXFLAGS) -c $< -o $@

ifeq ("$(ARCH)", "i386")
ARCHDEP = x86/libx86.a
else
ARCHDEP = x64/libx64.a
endif

all: boot.efi

boot.efi: \
	Base64.o		\
	BootArgs.o		\
	BootDebugger.o		\
	Config.o		\
	Console.o		\
	Crc32.o			\
	Debug1394.o		\
	DebugUsb.o		\
	DevicePath.o		\
	DeviceTree.o		\
	FileIo.o		\
	FileVault.o		\
	GuidDefine.o		\
	Hibernate.o		\
	LoadDrivers.o		\
	LoadKernel.o		\
	MachO.o			\
	Main.o			\
	MD5.o			\
	Memory.o		\
	MemoryMap.o		\
	MiscUtils.o		\
	NetBoot.o		\
	Options.o		\
	PanicDialog.o		\
	PeImage.o		\
	PlatformExpert.o	\
	RuntimeLib.o 		\
	SHA256.o		\
	StdAfx.o		\
	AcpiUtils.o		\
	$(ARCHDEP)		\
	../rijndael/librijndael.a


	@echo "------------------------------------------------------";
	@echo "\t[ld]\t$@.tmp";
	@$(LD) $(LDFLAGS) -o	\
	$@.tmp			\
	Base64.o		\
	BootArgs.o		\
	BootDebugger.o		\
	Config.o		\
	Console.o		\
	Crc32.o			\
	Debug1394.o		\
	DebugUsb.o		\
	DevicePath.o		\
	DeviceTree.o		\
	FileIo.o		\
	FileVault.o		\
	GuidDefine.o		\
	Hibernate.o		\
	LoadDrivers.o		\
	LoadKernel.o		\
	MachO.o			\
	Main.o			\
	MD5.o			\
	Memory.o		\
	MemoryMap.o		\
	MiscUtils.o		\
	NetBoot.o		\
	Options.o		\
	PanicDialog.o		\
	PeImage.o		\
	PlatformExpert.o	\
	RuntimeLib.o		\
	SHA256.o		\
	AcpiUtils.o		\
	StdAfx.o		\
	$(ARCHDEP)		\
	../rijndael/librijndael.a

	@echo "------------------------------------------------------";
	@echo "\t[strip]\t$@.tmp";
	@$(STRIP) $@.tmp
	@echo "\t[mtoc]\t$@.tmp";
	@$(MTOC) $@.tmp $@
	@rm -f $@.tmp
	@echo "\t[rm]\t$@.tmp"
	@echo "======================================================";

clean:
	@rm -f *.o *.efi *.map
