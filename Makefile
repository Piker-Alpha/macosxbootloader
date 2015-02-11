#
#
#

export PROJECT_DIR = $(PWD)

AR = ar
CC = gcc
CXX = g++
LD = ld
MTOC = $(PROJECT_DIR)/tools/mtoc -subsystem UEFI_APPLICATION -align 0x20
NASM = $(PROJECT_DIR)/tools/nasm
RANLIB = ranlib
STRIP = strip

#
# Was a target arch specified?
#
ifeq ("$(ARCH)", "")
#
# No. Select i386 as our default target.
#
ARCH=i386
#
#
#
endif

#
# Compile a 64-bit only version of boot.efi?
#
ifeq ("$(ARCH)", "x86_64")
#
# Yes.
#
ARCHDIR = x64
ARCHFLAGS = -arch x86_64
ARCHLDFLAGS = -u ?EfiMain@@YA_KPEAXPEAU_EFI_SYSTEM_TABLE@@@Z -e ?EfiMain@@YA_KPEAXPEAU_EFI_SYSTEM_TABLE@@@Z
ARCHCFLAGS = -target x86_64-pc-win32-macho -funsigned-char -fno-ms-extensions -fno-stack-protector -fno-builtin -fshort-wchar -mno-implicit-float -msoft-float -mms-bitfields -ftrap-function=undefined_behavior_has_been_optimized_away_by_clang -D__x86_64__=1

NASMFLAGS = -f macho64 -D__APPLE__
NASMCOMPFLAGS = -Daes_encrypt=_aes_encrypt -Daes_decrypt=_aes_decrypt
#
# End of x86_64 target setup.
#
else
#
# No. Compile a i386 only version of boot.efi
#
ARCHDIR = x86
ARCHFLAGS = -arch i386
ARCHLDFLAGS = -u __Z7EfiMainPvP17_EFI_SYSTEM_TABLE -e __Z7EfiMainPvP17_EFI_SYSTEM_TABLE -read_only_relocs suppress
ARCHCFLAGS = -funsigned-char -fno-ms-extensions -fno-stack-protector -fno-builtin -fshort-wchar -mno-implicit-float -mms-bitfields -ftrap-function=undefined_behavior_has_been_optimized_away_by_clang -Duint_8t=unsigned\ char -Duint_16t=unsigned\ short -Duint_32t=unsigned\ int -Duint_64t=unsigned\ long\ long -DBRG_UI8=1 -DBRG_UI16=1 -DBRG_UI32=1 -DBRG_UI64=1 -D__i386__=1

NASMFLAGS = -f macho -D__APPLE__ -D__ARCH32__
NASMCOMPFLAGS =
#
# End of i386 target setup.
#
endif

#
# Set to 0 by default. Use 1 to include debug info.
#
DEBUG = 1

ifdef DEBUG
#
# Include debug info.
#
BUILD_TARGET_TYPE  = Debug
DEBUGFLAGS = -gfull -fstandalone-debug -O0 -DDEBUG_LDRP_CALL_CSPRINTF
else
#
# Debug disabled. Do not include debug info.
#
BUILD_TARGET_YPE = Release
DEBUGFLAGS = -g0 -fno-standalone-debug -OZ
endif

#
# Export bin directory
#
export BINARY_DIR = bin/$(ARCHDIR)/$(BUILD_TARGET_TYPE)

INCLUDES = -I /usr/include -I $(PROJECT_DIR)/sdk/include -I $(PROJECT_DIR)/sdk/include/$(ARCHDIR) -I $(PROJECT_DIR)/src/include
WFLAGS = -Wall -Werror -Wno-unknown-pragmas

CFLAGS = "$(ARCHFLAGS) $(DEBUGFLAGS) $(WFLAGS) -fborland-extensions $(ARCHCFLAGS) -fpie -std=gnu11 -DEFI_SPECIFICATION_VERSION=0x0001000a -DTIANO_RELEASE_VERSION=1 $(INCLUDES) -D_MSC_EXTENSIONS=1 -fno-exceptions"

CXXFLAGS = "$(ARCHFLAGS) $(DEBUGFLAGS) $(WFLAGS) -fborland-extensions $(ARCHCFLAGS) -fpie -DEFI_SPECIFICATION_VERSION=0x0001000a -DTIANO_RELEASE_VERSION=1 $(INCLUDES) -D_MSC_EXTENSIONS=1 -fno-exceptions -std=gnu++11"

LDFLAGS = "$(ARCHFLAGS) -preload -segalign 0x20 $(ARCHLDFLAGS) -pie -all_load -dead_strip -image_base 0x240 -compatibility_version 1.0 -current_version 2.1 -flat_namespace -print_statistics -map boot.map -sectalign __TEXT __text 0x20  -sectalign __TEXT __eh_frame  0x20 -sectalign __TEXT __ustring 0x20 -sectalign __TEXT __const 0x20 -sectalign __TEXT __ustring 0x20 -sectalign __DATA __data 0x20  -sectalign __DATA __bss 0x20  -sectalign __DATA __common 0x20 -final_output boot.sys"


all: rijndael $(ARCHDIR) boot

rijndael:
	@clear
	@echo "Compiling boot.efi (ARCH=$(ARCH))";
	@echo "======================================================";

	@make --directory=src/rijndael ARCH="$(ARCH)" NASM="$(NASM)" NASMFLAGS="$(NASMFLAGS)" CC="$(CC)" CFLAGS=$(CFLAGS) AR="$(AR)" RANLIB="$(RANLIB)" NASMCOMPFLAGS="$(NASMCOMPFLAGS)"

x64:	
	@make --directory=src/boot/x64 CXX="$(CXX)" CXXFLAGS=$(CXXFLAGS) NASM="$(NASM)" NASMFLAGS="$(NASMFLAGS)" AR="$(AR)" RANLIB="$(RANLIB)"

x86:	
	@make --directory=src/boot/x86 CXX="$(CXX)" CXXFLAGS=$(CXXFLAGS) NASM="$(NASM)" NASMFLAGS="$(NASMFLAGS)" AR="$(AR)" RANLIB="$(RANLIB)"

boot:	
	@make --directory=src/boot/ CC="$(CC)" CFLAGS=$(CFLAGS) CXX="$(CXX)" ARCH=$(ARCH) CXXFLAGS=$(CXXFLAGS) LD="$(LD)" LDFLAGS=$(LDFLAGS) STRIP="$(STRIP)" MTOC="$(MTOC)"

clean:
	@clear

	@if [ -d $(PROJECT_DIR)/out.log ]; then \
		echo "\t[rm] out.log"; \
		rm $(PROJECT_DIR)/out.log; \
	fi;

	@if [ -d $(PROJECT_DIR)/$(BINARY_DIR) ]; then \
		echo "\t[rm] $(BINARY_DIR)"; \
		rm -R $(PROJECT_DIR)/bin; \
	fi;

	@cd $(PROJECT_DIR)/src/rijndael && make clean
	@cd $(PROJECT_DIR)/src/boot && make clean
	@cd $(PROJECT_DIR)/src/boot/x86 && make clean
	@cd $(PROJECT_DIR)/src/boot/x64 && make clean
