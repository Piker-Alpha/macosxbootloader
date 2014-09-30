//********************************************************************
//	created:	6:11:2009   21:34
//	filename: 	BootDebugger.h
//	author:		tiamo
//	purpose:	boot debugger
//********************************************************************

#pragma once

//
// initialize boot debugger
//
EFI_STATUS BdInitialize(CHAR8 CONST* loaderOptions);

//
// debugger is enabled
//
BOOLEAN BdDebuggerEnabled();

//
// poll break in
//
BOOLEAN BdPollBreakIn();

//
// poll connection
//
VOID BdPollConnection();

//
// dbg breakpoint
//
VOID BOOTAPI DbgBreakPoint();

//
// dbg print
//
UINT32 DbgPrint(CHAR8 CONST* printFormat, ...);

//
// destroy debugger
//
EFI_STATUS BdFinalize();