;********************************************************************
;	created:	25:10:2009   16:53
;	filename: 	All_X86.asm
;	author:		tiamo
;	purpose:	all
;********************************************************************

%macro PUBLIC_SYMBOL 1
									global %1
									%1:
%endmacro

%ifdef __APPLE__
section .data

%ifdef __DEBUG__
%define									_ThunkCode64.dat	"../../../temp/x86/Debug/boot/ThunkCode64.dat"
%else
%define									_ThunkCode64.dat	"../../../temp/x86/Release/boot/ThunkCode64.dat"
%endif

%else
section .rdata
%define									_ThunkCode64.dat	"ThunkCode64.dat"
%endif

										align 16

PUBLIC_SYMBOL ?ArchThunk64BufferStart@@3PAEA
										incbin _ThunkCode64.dat
PUBLIC_SYMBOL ?ArchThunk64BufferEnd@@3PAEA
