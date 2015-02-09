#
#
#

%.o: %.asm
	@echo "\t[nasm]\t$@<";
	@$(NASM) $(NASMFLAGS) -o $@ $<

%.o: %.cpp
	@echo "\t[$(CXX)]\t$<";
	@$(CXX) $(CXXFLAGS) -o $@ -c $<

all: libx64.a

libx64.a: \
	DebugTrap.o		\
	CompilerX64.o		\
	ArchUtilsX64.o		\
	DebuggerUtilsX64.o	\
	PortRegisterIo.o	\
	TransferRoutine.o

	@$(AR) cru $@		\
	DebugTrap.o		\
	CompilerX64.o		\
	ArchUtilsX64.o		\
	DebuggerUtilsX64.o	\
	PortRegisterIo.o	\
	TransferRoutine.o

	@echo "\t[ranlib]\t$@";
	@$(RANLIB) $@
	@echo "------------------------------------------------------";

clean:
	rm -f *.a *.o
