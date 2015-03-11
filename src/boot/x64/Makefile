#
# Makefile bot boot/x64
#

%.o: %.asm
	@echo "\t[nasm]\t$@<";
	@$(NASM) $(NASMFLAGS) -o '$(TMP_BOOT_DIR)/$@' $<

%.o: %.cpp
	@echo "\t[$(CXX)]\t$<";
	@$(CXX) $(CXXFLAGS) -o '$(TMP_BOOT_DIR)/$@' -c $<

all: libx64.a

libx64.a: \
	DebugTrap.o			\
	CompilerX64.o		\
	ArchUtilsX64.o		\
	DebuggerUtilsX64.o	\
	PortRegisterIo.o	\
	TransferRoutine.o

	@echo "\t[libtool]\t$@"
	@libtool -static -o '$(TMP_BOOT_DIR)/$@'	\
	'$(TMP_BOOT_DIR)/DebugTrap.o'				\
	'$(TMP_BOOT_DIR)/CompilerX64.o'				\
	'$(TMP_BOOT_DIR)/ArchUtilsX64.o'			\
	'$(TMP_BOOT_DIR)/DebuggerUtilsX64.o'		\
	'$(TMP_BOOT_DIR)/PortRegisterIo.o'			\
	'$(TMP_BOOT_DIR)/TransferRoutine.o'

	@echo "------------------------------------------------------";

clean:
	rm -f *.o *.a *.s *.ii
