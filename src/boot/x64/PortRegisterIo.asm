;*********************************************************************
;	created:	6:10:2009   16:24
;	filename: 	PortRegisterIo.asm
;	author:		tiamo
;	purpose:	io
;*********************************************************************

									default rel
									[bits 64]

%include "Common.inc"

PUBLIC_ROUTINE ?ARCH_READ_PORT_UINT8@@YAEPEAE@Z
									mov					dx, cx
									in					al, dx
									retn

PUBLIC_ROUTINE ?ARCH_READ_PORT_UINT16@@YAGPEAG@Z
									mov					dx, cx
									in					ax, dx
									retn

PUBLIC_ROUTINE ?ARCH_READ_PORT_UINT32@@YAIPEAI@Z
									mov					dx, cx
									in					eax, dx
									retn

PUBLIC_ROUTINE ?ARCH_WRITE_PORT_UINT8@@YAXPEAEE@Z
									mov					al, dl
									mov					dx, cx
									out					dx, al
									retn

PUBLIC_ROUTINE ?ARCH_WRITE_PORT_UINT16@@YAXPEAGG@Z
									mov					ax, dx
									mov					dx, cx
									out					dx, ax
									retn

PUBLIC_ROUTINE ?ARCH_WRITE_PORT_UINT32@@YAXPEAII@Z
									mov					eax, edx
									mov					dx, cx
									out					dx, eax
									retn

PUBLIC_ROUTINE ?ARCH_READ_REGISTER_UINT8@@YAEPEAE@Z
									mov					al, [rcx]
									retn

PUBLIC_ROUTINE ?ARCH_READ_REGISTER_UINT16@@YAGPEAG@Z
									mov					ax, [rcx]
									retn

PUBLIC_ROUTINE ?ARCH_READ_REGISTER_UINT32@@YAIPEAI@Z
									mov					eax, [rcx]
									retn

PUBLIC_ROUTINE ?ARCH_WRITE_REGISTER_UINT8@@YAXPEAEE@Z
									xor					eax, eax
									mov					[rcx], dl
							lock	or					[rsp], eax
									retn

PUBLIC_ROUTINE ?ARCH_WRITE_REGISTER_UINT16@@YAXPEAGG@Z
									xor					eax, eax
									mov					[rcx], dx
							lock	or					[rsp], eax
									retn

PUBLIC_ROUTINE ?ARCH_WRITE_REGISTER_UINT32@@YAXPEAII@Z
									xor					eax, eax
									mov					[rcx], rdx
							lock	or					[rsp], eax
									retn