//********************************************************************
//	created:	20:9:2012   21:17
//	filename: 	Base64.cpp
//	author:		tiamo
//	purpose:	base64
//********************************************************************

#include "StdAfx.h"

//
// decode
//
UINTN Base64Decode(CHAR8 CONST* inputString, UINTN inputLength, VOID* outputBuffer, UINTN* outputLength)
{
	STATIC CHAR8 table64[]													= "|$$$}rstuvwxyz{$$$$$$$>?@ABCDEFGHIJKLMNOPQRSTUVW$$$$$$XYZ[\\]^_`abcdefghijklmnopq";
	UINTN retLength															= 0;

	while(inputLength && *inputString)
	{
		CHAR8 iBuf[4]														= {0};
		CHAR8 oBuf[3]														= {0};
		UINTN inputParts													= 0;
		for(UINTN i = 0; i < 4; ) 
		{
			iBuf[i]															= 0;
			CHAR8 v															= inputLength ? *inputString : 0;
			if(!v)
				break;

			inputString														+= 1;
			inputLength														-= 1;
			if(v == '\n' || v == '\r' || v == '\t' || v == ' ')
				continue;
			
			inputParts														+= 1;
			v																= v < 43 || v > 122 ? 0 : table64[v - 43];
			if(v)
				v															= v == '$' ? 0 : v - 61;
			iBuf[i++]														= v ? v - 1 : 0;
		}
		if(!inputParts)
			break;

		oBuf[0]																= (iBuf[0] << 2 | iBuf[1] >> 4);
		oBuf[1]																= (iBuf[1] << 4 | iBuf[2] >> 2);
		oBuf[2]																= (((iBuf[2] << 6) & 0xc0) | iBuf[3]);

		if(*outputLength < retLength + inputParts - 1)
			break;

		memcpy(Add2Ptr(outputBuffer, retLength, VOID*), oBuf, inputParts - 1);
		retLength															+= inputParts - 1;
	}

	*outputLength															= retLength;
	return inputLength;
}

