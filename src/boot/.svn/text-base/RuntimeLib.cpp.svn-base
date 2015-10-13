//********************************************************************
//	created:	7:11:2009   13:28
//	filename: 	RuntimeLib.cpp
//	author:		tiamo
//	purpose:	runtime lib
//********************************************************************

#include "stdafx.h"

//
// memcpy
//
#pragma function(memcpy)
VOID* memcpy(VOID* dstBuffer, VOID CONST* srcBuffer, UINTN bufferLength)
{
	UINT8* byteDstBuffer													= static_cast<UINT8*>(dstBuffer);
	UINT8 CONST* byteSrcBuffer												= static_cast<UINT8 CONST*>(srcBuffer);
	for(UINTN i = 0; i < bufferLength; i ++)
		byteDstBuffer[i]													= byteSrcBuffer[i];
	return dstBuffer;
}

//
// memset
//
#pragma function(memset)
VOID* memset(VOID* dstBuffer, UINT8 setValue, UINTN bufferLength)
{
	UINT8* byteDstBuffer													= static_cast<UINT8*>(dstBuffer);
	for(UINTN i = 0; i < bufferLength; i ++)
		byteDstBuffer[i]													= setValue;
	return dstBuffer;
}

//
// memcmp
//
#pragma function(memcmp)
int memcmp(VOID CONST* buffer1, VOID CONST* buffer2, UINTN bufferLength)
{
	UINT8 CONST* byteBuffer1												= static_cast<UINT8 CONST*>(buffer1);
	UINT8 CONST* byteBuffer2												= static_cast<UINT8 CONST*>(buffer2);
	for(UINTN i = 0; i < bufferLength; i ++)
	{
		if(byteBuffer1[i] < byteBuffer2[i])
			return -1;

		if(byteBuffer1[i] > byteBuffer2[i])
			return 1;
	}
	return 0;
}

//
// wcslen
//
#pragma function(wcslen)
UINTN wcslen(CHAR16 CONST* stringBuffer)
{
	UINTN length															= 0;
	while(*stringBuffer ++)
		length																+= 1;
	return length;
}

//
// strlen
//
#pragma function(strlen)
UINTN strlen(CHAR8 CONST* stringBuffer)
{
	UINTN length															= 0;
	while(*stringBuffer ++)
		length																+= 1;
	return length;
}

//
// strcmp
//
#pragma function(strcmp)
int strcmp(CHAR8 CONST* stringBuffer1, CHAR8 CONST* stringBuffer2)
{
	while(*stringBuffer1 && *stringBuffer2)
	{
		int v																= *stringBuffer1 - *stringBuffer2;
		if(v)
			return v;

		stringBuffer1														+= 1;
		stringBuffer2														+= 1;
	}
	return *stringBuffer1 - *stringBuffer2;
}

//
// strcat
//
#pragma function(strcat)
CHAR8* strcat(CHAR8* dstBuffer, CHAR8 CONST* srcBuffer)
{
	UINTN i																	= 0;
	while(dstBuffer[i])
		i																	+= 1;
	for(; *srcBuffer; i ++)
		dstBuffer[i]														= *srcBuffer ++;
	dstBuffer[i]															= 0;
	return dstBuffer;
}

//
// strcpy
//
#pragma function(strcpy)
CHAR8* strcpy(CHAR8* dstBuffer, CHAR8 CONST* srcBuffer)
{
	UINTN i																	= 0;
	for(i = 0; *srcBuffer; i ++)
		dstBuffer[i]														= *srcBuffer ++;
	dstBuffer[i]															= 0;
	return dstBuffer;
}

//
// strncmp
//
INTN strnicmp(CHAR8 CONST* stringBuffer1, CHAR8 CONST* stringBuffer2, UINTN checkLength)
{
	while(*stringBuffer1 && *stringBuffer2 && checkLength)
	{
		INTN v																= (*stringBuffer1 | 0x20) - (*stringBuffer2 | 0x20);
		if(v)
			return v;

		checkLength															-= 1;
		stringBuffer1														+= 1;
		stringBuffer2														+= 1;
	}
	return checkLength ? (*stringBuffer1 | 0x20) - (*stringBuffer2 | 0x20) : 0;
}

//
// strncpy
//
CHAR8* strncpy(CHAR8* dstBuffer, CHAR8 CONST* srcBuffer, UINTN maxCharCount)
{
	UINTN i																	= 0;
	for(i = 0; *srcBuffer && i < maxCharCount; i ++)
		dstBuffer[i]														= *srcBuffer ++;
	if(i < maxCharCount)
		dstBuffer[i]														= 0;
	return dstBuffer;
}

//
// strstr
//
CHAR8 CONST* strstr(CHAR8 CONST* stringBuffer, CHAR8 CONST* findString)
{
	if(!findString[0])
		return stringBuffer;

	CHAR8 CONST* cp															= stringBuffer;
	while(*cp)
	{
		CHAR8 CONST* s1														= cp;
		CHAR8 CONST* s2														= findString;
		while( *s1 && *s2 && *s1 == *s2)
		{
			s1																+= 1;
			s2																+= 1;
		}
		if(!*s2)
			return cp;
		cp																	+= 1;
	}
	return nullptr;
}

//
// sprintf
//
UINTN snprintf(CHAR8* dstBuffer, UINTN bufferLength, CHAR8 CONST* formatString, ...)
{
	VA_LIST list;
	VA_START(list, formatString);
	INTN ret																= vsnprintf(dstBuffer, bufferLength, formatString, list);
	VA_END(list);
	return ret;
}

//
// atoi
//
INTN atoi(CHAR8 CONST* numberString)
{
	CHAR8 ch																= 0;

	do
	{
		ch																	= *numberString ++;
	}while(isspace(ch));

	CHAR8 sign																= ch;
	if(ch == '+' || ch == '-')
		ch																	= *numberString ++;

	INTN value																= 0;
	while(ch >= '0' && ch <= '9')
	{
		value																= value * 10 + ch - '0';
		ch																	= *numberString ++;
	}
	return sign == '-' ? -value : value;
}

//
// atoi
//
INT64 atoi64(CHAR8 CONST* numberString)
{
	CHAR8 ch																= 0;

	do
	{
		ch																	= *numberString ++;
	}while(isspace(ch));

	CHAR8 sign																= ch;
	if(ch == '+' || ch == '-')
		ch																	= *numberString ++;

	INT64 value																= 0;
	while(ch >= '0' && ch <= '9')
	{
		value																= value * 10 + ch - '0';
		ch																	= *numberString ++;
	}
	return sign == '-' ? -value : value;
}

//
// strtoul64
//
CHAR8 CONST* strtoul64_base16(CHAR8 CONST* numberString, UINT64* resultValue)
{
	UINT64 number															= 0;
	CHAR8 c																	= *numberString ++;

	while(isspace(c))
		c																	= *numberString ++;

	if(c == '0' && (*numberString == 'x' || *numberString == 'X'))
	{
		numberString														+= 1;
		c																	= *numberString ++;
	}

	while(TRUE)
	{
		UINTN digval														= 0;
		if(c >= '0' && c <= '9')
			digval = c - '0';
		else if(c >= 'a' && c <= 'f')
			digval = c - 'a' + 10;
		else if(c >= 'A' && c <= 'F')
			digval = c - 'A' + 10;
		else
			break;

		number																= number * 16 + digval;
		c																	= *numberString ++;
	}

	*resultValue															= number;
	return numberString - 1;
}

//
// lower case
//
CHAR8* strlwr(CHAR8* stringBuffer)
{
	for(CHAR8* cp = stringBuffer; *cp; cp ++)
	{
		if('A' <= *cp && *cp <= 'Z')
			*cp																+= 'a' - 'A';
	}
	return stringBuffer;
}

//
// upper case
//
CHAR8* strupr(CHAR8* stringBuffer)
{
	for(CHAR8* cp = stringBuffer; *cp; cp ++)
	{
		if('a' <= *cp && *cp <= 'z')
			*cp																-= 'a' - 'A';
	}
	return stringBuffer;
}

//
// vsprintf flags
//
#define LEFT_JUSTIFY														0x01
#define PREFIX_SIGN															0x02
#define PREFIX_BLANK														0x04
#define COMMA_TYPE															0x08
#define LONG_TYPE															0x10
#define PREFIX_ZERO															0x20
#define LOWER_CASE															0x40

STATIC CHAR8 CONST* _get_flags_and_width(CHAR8 CONST* formatString, UINTN* flags, UINTN* width, VA_LIST* marker)
{
	*flags																	= 0;
	*width																	= 0;
	for(BOOLEAN done = FALSE; !done; )
	{
		formatString														+= 1;

		switch(*formatString)
		{
		case '-':
			*flags															|= LEFT_JUSTIFY;
			break;

		case '+':
			*flags															|= PREFIX_SIGN;
			break;

		case ' ':
			*flags															|= PREFIX_BLANK;
			break;

		case ',':
			*flags															|= COMMA_TYPE;
			break;

		case 'L':
		case 'l':
			*flags															|= LONG_TYPE;
			break;

		case '*':
			*width															= VA_ARG(*marker, UINTN);
			break;

		case '0':
			*flags															|= PREFIX_ZERO;
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			*width															= 0;
			do
			{
				*width														= (*width * 10) + *formatString - '0';
				formatString												+= 1;
			}while(*formatString >= '0'  &&  *formatString <= '9');

			formatString													-= 1;
			break;

		default:
			done															= TRUE;
			break;
		}
	}
	return formatString;
}

STATIC UINTN _value_to_mhex_string(CHAR8* dstBuffer, UINT64 value, UINTN flags, UINTN width)
{
	CHAR8 tempBuffer[320]													= {0};
	CHAR8* tempString														= tempBuffer;
	CHAR8 prefix															= 0;
	CHAR8* buffer															= dstBuffer;
	UINTN count																= 0;
	UINTN index;

	do
	{
		if(flags & LOWER_CASE)
			*tempString														= "0123456789abcdef"[value & 0x0f];
		else
			*tempString														= "0123456789ABCDEF"[value & 0x0f];

		tempString															+= 1;
		value																>>= 4;
		count																+= 1;
	}while(value);

	if(flags & PREFIX_ZERO)
		prefix																= '0';
	else if(!(flags & LEFT_JUSTIFY))
		prefix																= ' ';

	for(index = count; index < width; index++, tempString ++)
		*tempString															= prefix;

	if(width > 0 && static_cast<UINTN>(tempString - tempBuffer) > width)
		tempString															= tempBuffer + width;

	index																	= 0;
	while(tempString != tempBuffer)
	{
		tempString															-= 1;
		*buffer																= *tempString;
		buffer																+= 1;
		index																+= 1;
	}

	*buffer																	= 0;
	return index;
}

//
// value to string
//
STATIC UINTN _value_to_string(CHAR8* dstBuffer, INT64 value, UINTN flags, UINTN width)
{
	CHAR8 tempBuffer[320]													= {0};
	CHAR8* tempString														= tempBuffer;
	CHAR8* buffer															= dstBuffer;
	UINTN count																= 0;
	UINTN numberCount														= 0;
	BOOLEAN negative														= TRUE;

	if(value < 0)
		value																= -value;
	else
		negative															= FALSE;

	do
	{
		UINTN remainder														= static_cast<UINT64>(value) % 10;
		value																= static_cast<INT64>(static_cast<UINT64>(value) / 10);

		*tempString															= static_cast<CHAR8>(remainder + '0');
		tempString															+= 1;
		count																+= 1;
		numberCount															+= 1;

		if((flags & COMMA_TYPE) == COMMA_TYPE)
		{
			if(numberCount % 3 == 0 && value != 0)
			{
				*tempString													= ',';
				tempString													+= 1;
				count														+= 1;
			}
		}
	}while(value != 0);

	if(negative)
	{
		*buffer																= '-';
		buffer																+= 1;
		count																+= 1;
	}


	if(width > 0 && static_cast<UINTN>(tempString - tempBuffer) > width)
		tempString															= tempBuffer + width;

	UINTN index																= 0;
	while(tempString != tempBuffer)
	{
		tempString															-= 1;
		*buffer																= *tempString;
		buffer																+= 1;
	}

	*buffer																	= 0;
	return index;
}

//
// vsprintf
//
UINTN vsnprintf(CHAR8* dstBuffer, UINTN bufferLength, CHAR8 CONST* formatString, VA_LIST marker)
{
	CHAR8 tempBuffer[320]													= {0};
	CHAR8* asciiString														= nullptr;
	CHAR16* unicodeString													= nullptr;
	UINTN flags																= 0;
	UINTN width																= 0;
	UINTN count																= 0;
	UINT64 value															= 0;
	UINTN bufferLeft														= bufferLength;
	UINTN index																= 0;
	for(index = 0; *formatString && index < bufferLength - 1; formatString ++)
	{
		if(*formatString != '%')
		{
			dstBuffer[index]												= *formatString;
			index															+= 1;
			bufferLeft														-= sizeof(CHAR8);
		}
		else
		{
			formatString													= _get_flags_and_width(formatString, &flags, &width, &marker);
			switch(*formatString)
			{
			case 'x':
				flags														|= LOWER_CASE;
				
			case 'X':
				{
					if((flags & LONG_TYPE) == LONG_TYPE)
						value												= VA_ARG(marker, UINT64);
					else
						value												= VA_ARG(marker, UINTN);

					_value_to_mhex_string(tempBuffer, value, flags, width);
					asciiString												= tempBuffer;

					for(; *asciiString && index < bufferLength - 1; asciiString ++, index ++)
						dstBuffer[index]									= *asciiString;
				}
				break;

			case 'd':
				{
					if((flags & LONG_TYPE) == LONG_TYPE)
						value												= VA_ARG(marker, UINT64);
					else
						value												= VA_ARG(marker, UINTN);

					_value_to_string(tempBuffer, value, flags, width);
					asciiString												= tempBuffer;

					for(; *asciiString && index < bufferLength - 1; asciiString ++, index ++)
						dstBuffer[index]									= *asciiString;
				}
				break;

			case 'S':
				{
					unicodeString											= VA_ARG(marker, CHAR16*);
					if(!unicodeString)
						unicodeString										= CHAR16_STRING(L"<null string>");

					for(count = 0; *unicodeString && index < bufferLength - 1; unicodeString ++, count ++)
					{
						dstBuffer[index]									= static_cast<CHAR8>(*unicodeString);
						index												+= 1;
					}

					for(; count < width && index < bufferLength - 1; count ++)
					{
						dstBuffer[index]									= ' ';
						index												+= 1;
					}
				}
				break;

			case 's':
			case 'a':
				{
					asciiString												= VA_ARG(marker, CHAR8*);
					if(!asciiString)
						asciiString											= CHAR8_STRING("<null string>");

					for(count = 0; *asciiString && index < bufferLength - 1; asciiString ++, count ++)
					{
						dstBuffer[index]									= *asciiString;
						index												+= 1;
					}

					for(; count < width && index < bufferLength - 1; count ++)
					{
						dstBuffer[index]									= ' ';
						index												+= 1;
					}
				}
				break;

			case 'c':
				dstBuffer[index]											= static_cast<CHAR8>(VA_ARG(marker, UINTN));
				index														+= 1;
				break;

			case '%':
			default:
				dstBuffer[index]											= *formatString;
				index														+= 1;
				break;
			}

			bufferLeft														= bufferLength - index;
		}
	}

	dstBuffer[index]														= 0;
	index																	+= 1;
	return index;
}

#undef LEFT_JUSTIFY
#undef PREFIX_SIGN
#undef PREFIX_BLANK
#undef COMMA_TYPE
#undef LONG_TYPE
#undef PREFIX_ZERO
#undef LOWER_CASE