#
# Makefile for boot/x86
#

%.o: %.asm
	@echo "\t[nasm]\t$<";
	@$(NASM) $(NASMFLAGS) -o '$(TMP_BOOT_DIR)/$@' $<

%.o: %.cpp
	@echo "\t[$(CXX)]\t$<";
	@$(CXX) $(CXXFLAGS) -c $< -o '$(TMP_BOOT_DIR)/$@'

%.dat: %.asm
	@echo "\t[nasm]\t$<";
	@$(NASM) -f bin -o '$(TMP_BOOT_DIR)/$@' $<

all: libx86.a

libx86.a: \
	ThunkCode64.dat		\
	ThunkCode32.o		\
	ArchUtilsX86.o		\
	CompilerX86.o		\
	DebuggerUtilsX86.o	\
	Thunk64.o

	@echo "\t[libtool]\t$@"
	@libtool -static -o '$(TMP_BOOT_DIR)/$@'	\
	'$(TMP_BOOT_DIR)/ArchUtilsX86.o'			\
	'$(TMP_BOOT_DIR)/CompilerX86.o'				\
	'$(TMP_BOOT_DIR)/DebuggerUtilsX86.o'		\
	'$(TMP_BOOT_DIR)/Thunk64.o'

	@echo "------------------------------------------------------";

clean:
	rm -f *.o *.a *.dat *.s *.ii

