//********************************************************************
//	created:	7:11:2009   13:28
//	filename: 	RuntimeLib.h
//	author:		tiamo
//	purpose:	runtime lib
//********************************************************************

#pragma once

//
// define
//
#define isspace(ch)															((ch) == ' ' || (ch) == '\t' || (ch) == '\r' || (ch) == '\n')

extern "C"
{
	//
	// allocate from stack
	//
	VOID* BOOTAPI _alloca(UINTN bufferLength);
	#pragma intrinsic(_alloca)

	//
	// memcpy
	//
	VOID* memcpy(VOID* dstBuffer, VOID CONST* srcBuffer, UINTN bufferLength);

	//
	// memset
	//
	VOID* memset(VOID* dstBuffer, UINT8 setValue, UINTN bufferLength);

	//
	// memcmp
	//
	int memcmp(VOID CONST* buffer1, VOID CONST* buffer2, UINTN bufferLength);

	//
	// wcslen
	//
	UINTN wcslen(CHAR16 CONST* stringBuffer);

	//
	// strlen
	//
	UINTN strlen(CONST CHAR8* stringBuffer);

	//
	// strcmp
	//
	int strcmp(CHAR8 CONST* stringBuffer1, CHAR8 CONST* stringBuffer2);

	//
	// strncmp
	//
	INTN strnicmp(CHAR8 CONST* stringBuffer1, CHAR8 CONST* stringBuffer2, UINTN checkLength);

	//
	// strcpy
	//
	CHAR8* strcpy(CHAR8* dstBuffer, CHAR8 CONST* srcBuffer);

	//
	// strcat
	//
	CHAR8* strcat(CHAR8* dstBuffer, CHAR8 CONST* srcBuffer);

	//
	// strncpy
	//
	CHAR8* strncpy(CHAR8* dstBuffer, CHAR8 CONST* srcBuffer, UINTN maxCharCount);

	//
	// strstr
	//
	CHAR8 CONST* strstr(CHAR8 CONST* stringBuffer, CHAR8 CONST* findString);

	//
	// sprintf
	//
	UINTN snprintf(CHAR8* dstBuffer, UINTN bufferLength, CHAR8 CONST* formatString, ...);

	//
	// vsprintf
	//
	UINTN vsnprintf(CHAR8* dstBuffer, UINTN bufferLength, CHAR8 CONST* formatString, VA_LIST marker);

	//
	// atoi
	//
	INTN atoi(CHAR8 CONST* numberString);

	//
	// atoi64
	//
	INT64 atoi64(CHAR8 CONST* numberString);

	//
	// strtoul64
	//
	CHAR8 CONST* strtoul64_base16(CHAR8 CONST* numberString, UINT64* resultValue);

	//
	// upper case
	//
	CHAR8* strupr(CHAR8* stringBuffer);

	//
	// lower case
	//
	CHAR8* strlwr(CHAR8* stringBuffer);
}