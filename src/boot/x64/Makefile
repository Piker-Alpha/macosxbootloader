
%.o: %.asm
	$(NASM) $(NASMFLAGS) -o $@ $<

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -o $@ -c $<

all: libx64.a

libx64.a: DebugTrap.o CompilerX64.o ArchUtilsX64.o DebuggerUtilsX64.o PortRegisterIo.o TransferRoutine.o

	$(AR) cru $@ DebugTrap.o CompilerX64.o ArchUtilsX64.o DebuggerUtilsX64.o PortRegisterIo.o TransferRoutine.o
	$(RANLIB) $@

clean:
	rm -f *.a *.o
