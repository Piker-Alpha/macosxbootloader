#
# 
#

%.o: %.c
	@echo "\t[$(CC)]\t$<";
	@$(CC) $(CFLAGS) -c $< -o $(TMP_BOOT_DIR)/$@

%.o: %.cpp
	@echo "\t[$(CXX)]\t$<";
	@$(CXX) $(CXXFLAGS) -c $< -o $(TMP_BOOT_DIR)/$@

ifeq ("$(ARCH)", "i386")
ARCHDEP = libx86.a
else
ARCHDEP = libx64.a
endif

all: boot.sys

boot.sys: \
	Base64.o					\
	BootArgs.o					\
	BootDebugger.o				\
	Config.o					\
	Console.o					\
	Crc32.o						\
	Debug1394.o					\
	DebugUsb.o					\
	DevicePath.o				\
	DeviceTree.o				\
	FileIo.o					\
	FileVault.o					\
	GuidDefine.o				\
	Hibernate.o					\
	LoadDrivers.o				\
	LoadKernel.o				\
	MachO.o						\
	Main.o						\
	MD5.o						\
	Memory.o					\
	MemoryMap.o					\
	MiscUtils.o					\
	NetBoot.o					\
	Options.o					\
	PanicDialog.o				\
	PeImage.o					\
	PlatformExpert.o			\
	RuntimeLib.o				\
	SHA256.o					\
	StdAfx.o					\
	AcpiUtils.o					\
	$(TMP_BOOT_DIR)/$(ARCHDEP)	\
	$(TMP_RIJNDAEL_DIR)/librijndael.a

	@echo "------------------------------------------------------";
	@echo "\t[ld]\t$@";

	@$(LD) $(LDFLAGS) -o \
	$(TMP_BOOT_DIR)/$@					\
	$(TMP_BOOT_DIR)/Base64.o			\
	$(TMP_BOOT_DIR)/BootArgs.o			\
	$(TMP_BOOT_DIR)/BootDebugger.o		\
	$(TMP_BOOT_DIR)/Config.o			\
	$(TMP_BOOT_DIR)/Console.o			\
	$(TMP_BOOT_DIR)/Crc32.o				\
	$(TMP_BOOT_DIR)/Debug1394.o			\
	$(TMP_BOOT_DIR)/DebugUsb.o			\
	$(TMP_BOOT_DIR)/DevicePath.o		\
	$(TMP_BOOT_DIR)/DeviceTree.o		\
	$(TMP_BOOT_DIR)/FileIo.o			\
	$(TMP_BOOT_DIR)/FileVault.o			\
	$(TMP_BOOT_DIR)/GuidDefine.o		\
	$(TMP_BOOT_DIR)/Hibernate.o			\
	$(TMP_BOOT_DIR)/LoadDrivers.o		\
	$(TMP_BOOT_DIR)/LoadKernel.o		\
	$(TMP_BOOT_DIR)/MachO.o				\
	$(TMP_BOOT_DIR)/Main.o				\
	$(TMP_BOOT_DIR)/MD5.o				\
	$(TMP_BOOT_DIR)/Memory.o			\
	$(TMP_BOOT_DIR)/MemoryMap.o			\
	$(TMP_BOOT_DIR)/MiscUtils.o			\
	$(TMP_BOOT_DIR)/NetBoot.o			\
	$(TMP_BOOT_DIR)/Options.o			\
	$(TMP_BOOT_DIR)/PanicDialog.o		\
	$(TMP_BOOT_DIR)/PeImage.o			\
	$(TMP_BOOT_DIR)/PlatformExpert.o	\
	$(TMP_BOOT_DIR)/RuntimeLib.o		\
	$(TMP_BOOT_DIR)/SHA256.o			\
	$(TMP_BOOT_DIR)/AcpiUtils.o			\
	$(TMP_BOOT_DIR)/StdAfx.o			\
	$(TMP_BOOT_DIR)/$(ARCHDEP)			\
	$(TMP_DIR)/rijndael/librijndael.a

	@echo "------------------------------------------------------";

ifeq ("$(DEBUG)", "0")
	@echo "\t[strip]\t$@";
	@$(STRIP) $(TMP_BOOT_DIR)/$@
endif

	@echo "\t[mtoc]\ttemp/$(ARCHDIR)/$(BUILD_TARGET_TYPE)/$@ $(BINARY_DIR)/boot.efi"
	@$(MTOC) $(TMP_BOOT_DIR)/$@ $(PROJECT_DIR)/$(BINARY_DIR)/boot.efi

	@echo "======================================================";
	@echo "Done.";

clean:
	@rm -f *.o *.map *.sys *.ii *.s
