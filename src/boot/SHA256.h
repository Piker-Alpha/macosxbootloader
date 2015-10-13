//********************************************************************
//	created:	20:9:2012   18:17
//	filename: 	SHA256.h
//	author:		tiamo	
//	purpose:	sha256
//********************************************************************

#pragma once

//
// context
//
typedef struct _SHA256_CONTEXT
{
	//
	// length
	//
	UINT32																	TotalLength[2];

	//
	// state
	//
	UINT32																	State[8];

	//
	// buffer
	//
	UINT8																	Buffer[64];
}SHA256_CONTEXT;

//
// init
//
VOID SHA256_Init(SHA256_CONTEXT* sha256Context);

//
// update
//
VOID SHA256_Update(VOID CONST* dataBuffer, UINTN dataLength, SHA256_CONTEXT* sha256Context);

//
// final
//
VOID SHA256_Final(UINT8* resultBuffer, SHA256_CONTEXT* sha256Context);

//
// sha256 buffer
//
VOID SHA256(VOID CONST* dataBuffer, UINTN dataLength, UINT8* resultBuffer);
