#
#
#

%.o: %.c
	@echo "\t[$(CC)]\t$<";
	@$(CC) $(CFLAGS) -c $< -o $(TMP_RIJNDAEL_DIR)/$@

%.o: %.asm
	@echo "\t[nasm]\t$<";
	@$(NASM) $(NASMFLAGS) $(NASMCOMPFLAGS) -o $(TMP_RIJNDAEL_DIR)/$@ $<

librijndael.a: \
	aes_modes.o	\
	aescrypt.o	\
	aeskey.o	\
	aestab.o	\
	aesxts.o

	@echo "\t[ar]\t$@";
	@$(AR) cru $(TMP_RIJNDAEL_DIR)/$@	\
	$(TMP_RIJNDAEL_DIR)/aes_modes.o	\
	$(TMP_RIJNDAEL_DIR)/aescrypt.o		\
	$(TMP_RIJNDAEL_DIR)/aeskey.o		\
	$(TMP_RIJNDAEL_DIR)/aestab.o		\
	$(TMP_RIJNDAEL_DIR)/aesxts.o

	@echo "\t[ranlib]\t$@";
	@$(RANLIB) $(TMP_RIJNDAEL_DIR)/$@
	@echo "------------------------------------------------------";

clean:
	rm -f *.a *.o *.s *.i

