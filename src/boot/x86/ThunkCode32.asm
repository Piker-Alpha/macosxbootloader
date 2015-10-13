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

section .rdata
									align				16
PUBLIC_SYMBOL ?ArchThunk64BufferStart@@3PAEA
									incbin				"ThunkCode64.dat"
PUBLIC_SYMBOL ?ArchThunk64BufferEnd@@3PAEA
