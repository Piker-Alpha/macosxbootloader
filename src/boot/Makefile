#
# 
#

%.o: %.c
	@echo "\t[$(CC)]\t$<";
	@$(CC) $(CFLAGS) -c $< -o $@

%.o: %.cpp
	@echo "\t[$(CXX)]\t$<";
	@$(CXX) $(CXXFLAGS) -c $< -o $@

ifeq ("$(ARCH)", "i386")
ARCHDEP = x86/libx86.a
else
ARCHDEP = x64/libx64.a
endif

all: boot.sys

boot.sys: \
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
	@echo "\t[ld]\t$@";
	@$(LD) $(LDFLAGS) -o $@	\
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
	@echo "\t[strip]\t$@";
	@$(STRIP) $@

	@if [ ! -d $(PROJECT_DIR)/$(BINARY_DIR) ]; then \
		echo "\t[mkdir] $(BINARY_DIR)"; \
		/bin/mkdir -p $(PROJECT_DIR)/$(BINARY_DIR); \
	fi;

	@echo "\t[mtoc]\t$@ $(BINARY_DIR)/boot.efi";
	@$(MTOC) $@ $(PROJECT_DIR)/$(BINARY_DIR)/boot.efi
	@echo "\t[rm]\t$@"
	@rm -f $@
	@echo "======================================================";
	@echo "Done.";
clean:
	@rm -f *.o *.map *.sys
