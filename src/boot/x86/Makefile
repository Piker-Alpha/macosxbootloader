#
#
#

%.o: %.asm
	@echo "\t[nasm]\t$<";
	@$(NASM) $(NASMFLAGS) -o $@ $<

%.o: %.cpp
	@echo "\t[$(CXX)]\t$<";
	@$(CXX) $(CXXFLAGS) -c $< -o $@

%.dat: %.asm
	@echo "\t[nasm]\t$<";
	@$(NASM) -f bin -o $@ $<

all: libx86.a

libx86.a: \
	ThunkCode64.dat		\
	ThunkCode32.o		\
	ArchUtilsX86.o		\
	CompilerX86.o		\
	Thunk64.o		\
	DebuggerUtilsX86.o

	@echo "\t[ar]\t$@";
	@$(AR) cru $@		\
	ThunkCode32.o		\
	ArchUtilsX86.o		\
	CompilerX86.o		\
	Thunk64.o		\
	DebuggerUtilsX86.o

	@echo "\t[ranlib]\t$@";
	@$(RANLIB) $@
	@echo "------------------------------------------------------";

clean:
	rm -f *.dat *.o *.a

