#
#
#

%.o: %.c
	@echo "\t[$(CC)]\t$<";
	@$(CC) $(CFLAGS) -c $< -o $@

%.o: %.asm
	@echo "\t[nasm]\t$<";
	@$(NASM) $(NASMFLAGS) $(NASMCOMPFLAGS) -o $@ $<

librijndael.a: \
	aes_modes.o	\
	aescrypt.o	\
	aeskey.o	\
	aestab.o	\
	aesxts.o

	@echo "\t[ar]\t$@";
	@$(AR) cru $@ aes_modes.o aescrypt.o aeskey.o aestab.o aesxts.o
	@echo "\t[ranlib]\t$@";
	@$(RANLIB) $@
	@echo "------------------------------------------------------";

clean:
	rm -f *.a *.o

