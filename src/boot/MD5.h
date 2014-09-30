//********************************************************************
//	created:	8:11:2009   20:04
//	filename: 	MD5.h
//	author:		tiamo
//	purpose:	md5
//********************************************************************

#pragma once

//
// context
//
typedef struct _MD5_CONTEXT
{
	//
	// state
	//
	UINT32																	State[4];

	//
	// count
	//
	UINT32																	Count[2];

	//
	// input buffer
	//
	UINT8																	InputBuffer[64];
}MD5_CONTEXT;

//
// init
//
VOID MD5Init(MD5_CONTEXT* md5Context);

//
// update
//
VOID MD5Update(MD5_CONTEXT* md5Context, VOID CONST* byteBuffer, UINT32 bufferLength);

//
// finish
//
VOID MD5Final(UINT8* md5Result, MD5_CONTEXT* md5Context);