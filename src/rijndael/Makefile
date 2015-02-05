
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.asm
	$(NASM) $(NASMFLAGS) $(NASMCOMPFLAGS) -o $@ $<

librijndael.a: aes_modes.o aescrypt.o aeskey.o aestab.o aesxts.o
	$(AR) cru $@ aes_modes.o aescrypt.o aeskey.o aestab.o aesxts.o
	$(RANLIB) $@

clean:
	rm -f *.a *.o

