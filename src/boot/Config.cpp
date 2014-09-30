//********************************************************************
//	created:	8:11:2009   18:24
//	filename: 	Config.cpp
//	author:		tiamo
//	purpose:	config
//********************************************************************

#include "stdafx.h"

//
// symbol
//
typedef struct _XML_SYMBOL
{
	//
	// ref count
	//
	INTN																	RefCount;

	//
	// next link
	//
	struct _XML_SYMBOL*														Next;

	//
	// string buffer
	//
	CHAR8																	StringBuffer[1];
}XML_SYMBOL;

//
// global
//
STATIC XML_TAG* CmpConfigPlistTag											= nullptr;
STATIC XML_TAG* CmpNextFreeTag												= nullptr;
STATIC XML_SYMBOL* CmpSymbolsHead											= nullptr;
STATIC INTN CmpParseNextTag(CHAR8* contentBuffer, XML_TAG** theTag);

//
// get token
//
STATIC VOID CmpGetToken(CHAR8 CONST* inputBuffer, CHAR8 CONST** theToken, UINTN* tokenLength)
{
	if(*inputBuffer == '"')
	{
		inputBuffer															+= 1;
		*theToken															= inputBuffer;

		while(*inputBuffer && *inputBuffer != '"')
			inputBuffer														+= 1;
	}
	else
	{
		*theToken															= inputBuffer;
		while(*inputBuffer && !isspace(*inputBuffer) && *inputBuffer != '=')
			inputBuffer														+= 1;
	}

	*tokenLength															= inputBuffer - *theToken;
}

//
// get string value for key
//
STATIC CHAR8 CONST* CmpGetCommandStringValueForKey(CHAR8 CONST* commandLine, CHAR8 CONST* keyName, UINTN* valueLength)
{
	while(commandLine && *commandLine)
	{
		while(isspace(*commandLine))
			commandLine														+= 1;

		CHAR8 CONST* theKey													= 0;
		UINTN keyLength														= 0;
		CmpGetToken(commandLine, &theKey, &keyLength);
		commandLine															= theKey + keyLength;

		CHAR8 CONST* theValue												= commandLine;
		UINTN localLength													= 0;
		if(*commandLine && !isspace(*commandLine))
		{
			commandLine														+= 1;
			CmpGetToken(commandLine, &theValue, &localLength);
			commandLine														= theValue + localLength;
		}

		if(strnicmp(keyName, theKey, keyLength) || (keyName[keyLength] != 0 && keyName[keyLength] != '=' && !isspace(keyName[keyLength])))
			continue;

		if(valueLength)
			*valueLength													= localLength;

		return theValue;
	}

	return nullptr;
}

//
// emit string
//
STATIC VOID CmpEmitString(CHAR8 CONST* theString, BOOLEAN escapeXMLChar, CHAR8** outputBuffer, UINTN* outputPosition, UINTN* outputLength)
{
	while(TRUE)
	{
		CHAR8 theChar														= *theString ++;
		UINTN theLength														= sizeof(theChar);
		CHAR8 CONST* writeString											= &theChar;

		if(escapeXMLChar)
		{
			BOOLEAN computeLength											= TRUE;
			if(theChar == '<')
				writeString													= CHAR8_CONST_STRING("&lt;");
			else if(theChar == '>')
				writeString													= CHAR8_CONST_STRING("&gt;");
			else if(theChar == '&')
				writeString													= CHAR8_CONST_STRING("&amp;");
			else
				computeLength												= FALSE;

			if(computeLength)
				theLength													= strlen(writeString);
		}

		if(*outputPosition + theLength >= *outputLength)
		{
			UINTN newLength													= *outputLength + 1024;
			CHAR8* newBuffer												= static_cast<CHAR8*>(MmAllocatePool(newLength));
			if(!newBuffer)
				return;

			memcpy(newBuffer, *outputBuffer, *outputPosition);
			MmFreePool(*outputBuffer);
			*outputBuffer													= newBuffer;
			*outputLength													= newLength;
		}

		for(UINTN i = 0; i < theLength; i ++, *outputPosition += 1)
			(*outputBuffer)[*outputPosition]								= writeString[i];
	}
}

//
// emit string
//
STATIC VOID CmpEmitFormatString(CHAR8 CONST* formatString, BOOLEAN escapeXMLChar, CHAR8** outputBuffer, UINTN* outputPosition, UINTN* outputLength, ...)
{
	VA_LIST list;
	VA_START(list, outputLength);
	STATIC CHAR8 buffer[4096]												= {0};
	vsnprintf(buffer, ARRAYSIZE(buffer) - 1, formatString, list);
	VA_END(list);

	CmpEmitString(buffer, escapeXMLChar, outputBuffer, outputPosition, outputLength);
}

//
// dict to string
//
STATIC VOID CmpTagToString(XML_TAG* dictTag, CHAR8** outputBuffer, UINTN* outputPosition, UINTN* outputLength)
{
	STATIC CHAR8 CONST* tagNameArray[] =
	{
		CHAR8_CONST_STRING("none"),
		CHAR8_CONST_STRING("dict"),
		CHAR8_CONST_STRING("key"),
		CHAR8_CONST_STRING("string"),
		CHAR8_CONST_STRING("integer"),
		CHAR8_CONST_STRING("data"),
		CHAR8_CONST_STRING("date"),
		CHAR8_CONST_STRING("false"),
		CHAR8_CONST_STRING("true"),
		CHAR8_CONST_STRING("array")
	};

	XML_TAG* theTag															= dictTag;
	while(theTag)
	{
		CHAR8 CONST* tagName												= nullptr;
		if(theTag->Type >= ARRAYSIZE(tagNameArray))
			tagName															= CHAR8_CONST_STRING("unknown");
		else
			tagName															= tagNameArray[theTag->Type];

		CmpEmitFormatString(CHAR8_CONST_STRING("<%a"), FALSE, outputBuffer, outputPosition, outputLength, tagName);
		BOOLEAN tagOpened													= TRUE;

		if(theTag->StringValue)
		{
			CmpEmitString(CHAR8_CONST_STRING(">"), FALSE, outputBuffer, outputPosition, outputLength);
			CmpEmitString(theTag->StringValue, TRUE, outputBuffer, outputPosition, outputLength);
			tagOpened														= FALSE;
		}

		if(theTag->Type == XML_TAG_INTEGER)
		{
			CmpEmitString(CHAR8_CONST_STRING(">"), FALSE, outputBuffer, outputPosition, outputLength);
			CmpEmitFormatString(CHAR8_CONST_STRING("%d"), FALSE, outputBuffer, outputPosition, outputLength, theTag->IntegerValue);
			tagOpened														= FALSE;
		}

		if(theTag->TagValue)
		{
			if(tagOpened)
				CmpEmitString(CHAR8_CONST_STRING(">"), FALSE, outputBuffer, outputPosition, outputLength);

			tagOpened														= FALSE;
			if(theTag->Type == XML_TAG_KEY)
				CmpEmitFormatString(CHAR8_CONST_STRING("</%a>"), FALSE, outputBuffer, outputPosition, outputLength, tagName);
			else
				tagOpened													= TRUE;

			CmpTagToString(theTag->TagValue, outputBuffer, outputPosition, outputLength);

			if(tagOpened)
				CmpEmitFormatString(CHAR8_CONST_STRING("</%a>"), FALSE, outputBuffer, outputPosition, outputLength, tagName);
		}
		else
		{
			if(tagOpened)
				CmpEmitString(CHAR8_CONST_STRING("/>"), FALSE, outputBuffer, outputPosition, outputLength);
			else
				CmpEmitFormatString(CHAR8_CONST_STRING("</%a>"), FALSE, outputBuffer, outputPosition, outputLength, tagName);
		}

		theTag																= theTag->NextTag;
	}
}

//
// get next tag
//
STATIC INTN CmpGetNextTag(CHAR8* contentBuffer, CHAR8** tagName, INTN* startPosition)
{
	INTN curPosition														= 0;
	while(contentBuffer[curPosition] != 0 && contentBuffer[curPosition] != '<')
		curPosition															+= 1;

	if(contentBuffer[curPosition] == 0)
		return -1;

	INTN endPosition														= curPosition + 1;
	while(contentBuffer[endPosition] != 0 && contentBuffer[endPosition] != '>')
		endPosition															+= 1;

	if(contentBuffer[endPosition] == 0)
		return -1;

	*tagName																= contentBuffer + curPosition + 1;
	contentBuffer[endPosition]												= 0;
	if(startPosition)
		*startPosition														= curPosition;

	return endPosition + 1;
}

//
// find symbol
//
STATIC XML_SYMBOL* CmpFindSymbol(CHAR8 CONST* stringValue, XML_SYMBOL** prevSymbolP)
{
	XML_SYMBOL* theSymbol													= CmpSymbolsHead;
	XML_SYMBOL* prevSymbol													= nullptr;

	while(theSymbol)
	{
		if(!strcmp(theSymbol->StringBuffer, stringValue))
			break;

		prevSymbol															= theSymbol;
		theSymbol															= theSymbol->Next;
	}

	if(theSymbol && prevSymbolP)
		*prevSymbolP														= prevSymbol;

	return theSymbol;
}

//
// new symbol
//
STATIC CHAR8* CmpNewSymbol(CHAR8* stringValue)
{
	XML_SYMBOL* theSymbol													= CmpFindSymbol(stringValue, nullptr);
	if(!theSymbol)
	{
		theSymbol															= static_cast<XML_SYMBOL*>(MmAllocatePool(EFI_FIELD_OFFSET(XML_SYMBOL, StringBuffer) + 1 + strlen(stringValue)));
		if(!theSymbol)
			return nullptr;

		theSymbol->RefCount													= 0;
		strcpy(theSymbol->StringBuffer, stringValue);

		theSymbol->Next														= CmpSymbolsHead;
		CmpSymbolsHead														= theSymbol;
	}

	theSymbol->RefCount														+= 1;
	return theSymbol->StringBuffer;
}

//
// free symbol
//
STATIC VOID CmpFreeSymbol(CHAR8* stringValue)
{
	XML_SYMBOL* prevSymbol													= nullptr;
	XML_SYMBOL* theSymbol													= CmpFindSymbol(stringValue, &prevSymbol);
	if(!theSymbol)
		return;

	theSymbol->RefCount														-= 1;
	if(theSymbol->RefCount)
		return;

	if(prevSymbol)
		prevSymbol->Next													= theSymbol->Next;
	else
		CmpSymbolsHead														= theSymbol->Next;

	MmFreePool(theSymbol);
}

//
// new tag
//
STATIC XML_TAG* CmpNewTag()
{
	if(!CmpNextFreeTag)
	{
		XML_TAG* allocatedTags												= static_cast<XML_TAG*>(MmAllocatePool(sizeof(XML_TAG) * EFI_PAGE_SIZE));
		if(!allocatedTags)
			return nullptr;

		for(UINT32 i = 0; i < EFI_PAGE_SIZE; i ++)
		{
			allocatedTags[i].Type											= XML_TAG_NONE;
			allocatedTags[i].IntegerValue									= 0;
			allocatedTags[i].StringValue									= nullptr;
			allocatedTags[i].TagValue										= nullptr;
			allocatedTags[i].DataValue										= nullptr;
			allocatedTags[i].NextTag										= allocatedTags + i + 1;
		}

		allocatedTags[EFI_PAGE_SIZE - 1].NextTag							= nullptr;
		CmpNextFreeTag														= allocatedTags;
	}

	XML_TAG* theTag															= CmpNextFreeTag;
	CmpNextFreeTag															= theTag->NextTag;
	return theTag;
}

STATIC INTN CmpFixDataMatchingTag(CHAR8* contentBuffer, CHAR8 CONST* tagName)
{
	INTN start																= 0;
	INTN length																= 0;
	INTN stop																= 0;
	while(TRUE)
	{
		CHAR8* endTag														= nullptr;
		length																= CmpGetNextTag(contentBuffer + start, &endTag, &stop);
		if(length == -1)
			return -1;

		if(*endTag == '/' && !strcmp(endTag + 1, tagName))
			break;

		start																+= length;
	}

	contentBuffer[start + stop]												= 0;
	return start + length;
}

//
// parse list
//
STATIC INTN CmpParseTagList(CHAR8* contentBuffer, XML_TAG** theTag, UINTN tagType, BOOLEAN emptyTag)
{
	XML_TAG* tagList														= nullptr;
	XML_TAG* prevTag														= nullptr;
	INTN position															= 0;

	__try
	{
		if(!emptyTag)
		{
			while(TRUE)
			{
				XML_TAG* tempTag											= nullptr;
				INTN length													= CmpParseNextTag(contentBuffer + position, &tempTag);
				if(length == -1)
					try_leave(position = -1);

				position													+= length;
				if(!tempTag)
					break;

				if(!tagList)
					tagList													= tempTag;

				if(prevTag)
					prevTag->NextTag										= tempTag;

				prevTag														= tempTag;
			}
		}

		XML_TAG* tempTag													= CmpNewTag();
		if(!tempTag)
			try_leave(position = -1);

		tempTag->Type														= tagType;
		tempTag->StringValue												= nullptr;
		tempTag->IntegerValue												= 0;
		tempTag->DataValue													= nullptr;
		tempTag->TagValue													= tagList;
		tempTag->NextTag													= nullptr;
		*theTag																= tempTag;
	}
	__finally
	{
		if(position == -1 && tagList)
			CmFreeTag(tagList);
	}

	return position;
}

STATIC INTN CmpParseTagKey(CHAR8* contentBuffer, XML_TAG** theTag)
{
	INTN length																= 0;
	INTN length2															= 0;
	INTN retValue															= -1;
	XML_TAG* subTag															= nullptr;
	XML_TAG* tempTag														= nullptr;

	__try
	{
		length																= CmpFixDataMatchingTag(contentBuffer, CHAR8_CONST_STRING("key"));
		if(length == -1)
			try_leave(NOTHING);


		length2																= CmpParseNextTag(contentBuffer + length, &subTag);
		if(length2 == -1)
			try_leave(NOTHING);

		tempTag																= CmpNewTag();
		if(!tempTag)
			try_leave(NOTHING);

		CHAR8* stringValue													= CmpNewSymbol(contentBuffer);
		if(!stringValue)
			try_leave(NOTHING);

		tempTag->Type														= XML_TAG_KEY;
		tempTag->StringValue												= stringValue;
		tempTag->IntegerValue												= 0;
		tempTag->DataValue													= nullptr;
		tempTag->TagValue													= subTag;
		tempTag->NextTag													= nullptr;
		*theTag																= tempTag;
		retValue															= length2 + length;
	}
	__finally
	{
		if(retValue == -1)
		{
			if(subTag)
				CmFreeTag(subTag);

			if(tempTag)
				CmFreeTag(tempTag);
		}
	}

	return retValue;
}

//
// parse string
//
STATIC INTN CmpParseTagString(CHAR8* contentBuffer, XML_TAG** theTag)
{
	INTN length																= CmpFixDataMatchingTag(contentBuffer, CHAR8_CONST_STRING("string"));
	if(length == -1)
		return -1;

	XML_TAG* tempTag														= CmpNewTag();
	if(!tempTag)
		return -1;

	CHAR8* stringValue														= CmpNewSymbol(contentBuffer);
	if(!stringValue)
	{
		CmFreeTag(tempTag);
		return -1;
	}

	tempTag->Type															= XML_TAG_STRING;
	tempTag->IntegerValue													= 0;
	tempTag->StringValue													= stringValue;
	tempTag->DataValue														= nullptr;
	tempTag->TagValue														= nullptr;
	tempTag->NextTag														= nullptr;
	*theTag																	= tempTag;

	return length;
}

//
// parse integer
//
STATIC INTN CmpParseTagInteger(CHAR8* contentBuffer, XML_TAG** theTag)
{
	INTN length																= CmpFixDataMatchingTag(contentBuffer, CHAR8_CONST_STRING("integer"));
	if(length == -1)
		return -1;

	XML_TAG* tempTag														= CmpNewTag();
	if(!tempTag)
		return -1;

	tempTag->Type															= XML_TAG_INTEGER;
	tempTag->IntegerValue													= atoi64(contentBuffer);
	tempTag->StringValue													= nullptr;
	tempTag->DataValue														= nullptr;
	tempTag->TagValue														= nullptr;
	tempTag->NextTag														= nullptr;
	*theTag																	= tempTag;

	return length;
}

//
// parse data
//
STATIC INTN CmpParseTagData(CHAR8* contentBuffer, XML_TAG** theTag)
{
	INTN length																= CmpFixDataMatchingTag(contentBuffer, CHAR8_CONST_STRING("data"));
	if(length == -1)
		return -1;

	UINTN dataLength														= length + 1;
	VOID* dataBuffer														= MmAllocatePool(dataLength);
	if(!dataBuffer)
		return -1;

	Base64Decode(contentBuffer, length, dataBuffer, &dataLength);
	
	XML_TAG* tempTag														= CmpNewTag();
	if(!tempTag)
		return -1;

	tempTag->Type															= XML_TAG_DATA;
	tempTag->IntegerValue													= 0;
	tempTag->StringValue													= nullptr;
	tempTag->DataValue														= dataBuffer;
	tempTag->DataLength														= dataLength;
	tempTag->TagValue														= nullptr;
	tempTag->NextTag														= nullptr;
	*theTag																	= tempTag;

	return length;
}

//
// parse date
//
STATIC INTN CmpParseTagDate(CHAR8* contentBuffer, XML_TAG** theTag)
{
	INTN length																= CmpFixDataMatchingTag(contentBuffer, CHAR8_CONST_STRING("date"));
	if(length == -1)
		return -1;

	XML_TAG* tempTag														= CmpNewTag();
	if(!tempTag)
		return -1;

	tempTag->Type															= XML_TAG_DATE;
	tempTag->IntegerValue													= 0;
	tempTag->StringValue													= nullptr;
	tempTag->DataValue														= nullptr;
	tempTag->TagValue														= nullptr;
	tempTag->NextTag														= nullptr;
	*theTag																	= tempTag;

	return length;
}

//
// parse boolean
//
STATIC INTN CmpParseTagBoolean(CHAR8* contentBuffer, XML_TAG** theTag, UINTN tagType)
{
	XML_TAG* tempTag														= CmpNewTag();
	if(!tempTag)
		return -1;

	tempTag->Type															= tagType;
	tempTag->IntegerValue													= 0;
	tempTag->StringValue													= nullptr;
	tempTag->DataValue														= nullptr;
	tempTag->TagValue														= nullptr;
	tempTag->NextTag														= nullptr;
	*theTag																	= tempTag;

	return 0;
}

//
// parse next tag
//
STATIC INTN CmpParseNextTag(CHAR8* contentBuffer, XML_TAG** theTag)
{
	CHAR8* tagName															= nullptr;
	*theTag																	= nullptr;
	INTN length																= CmpGetNextTag(contentBuffer, &tagName, nullptr);
	if(length == -1)
		return -1;

	INTN position															= length;
	if(!strnicmp(tagName, CHAR8_CONST_STRING("plist "), 6))
		length																= 0;
	else if(!strcmp(tagName, CHAR8_CONST_STRING("dict")))
		length																= CmpParseTagList(contentBuffer + position, theTag, XML_TAG_DICT, FALSE);
	else if(!strcmp(tagName, CHAR8_CONST_STRING("dict/")))
		length																= CmpParseTagList(contentBuffer + position, theTag, XML_TAG_DICT, TRUE);
	else if(!strcmp(tagName, CHAR8_CONST_STRING("key")))
		length																= CmpParseTagKey(contentBuffer + position, theTag);
	else if(!strcmp(tagName, CHAR8_CONST_STRING("string")))
		length																= CmpParseTagString(contentBuffer + position, theTag);
	else if(!strcmp(tagName, CHAR8_CONST_STRING("integer")))
		length																= CmpParseTagInteger(contentBuffer + position, theTag);
	else if(!strcmp(tagName, CHAR8_CONST_STRING("data")))
		length																= CmpParseTagData(contentBuffer + position, theTag);
	else if(!strcmp(tagName, CHAR8_CONST_STRING("date")))
		length																= CmpParseTagDate(contentBuffer + position, theTag);
	else if(!strcmp(tagName, CHAR8_CONST_STRING("false/")))
		length																= CmpParseTagBoolean(contentBuffer + position, theTag, XML_TAG_FALSE);
	else if(!strcmp(tagName, CHAR8_CONST_STRING("true/")))
		length																= CmpParseTagBoolean(contentBuffer + position, theTag, XML_TAG_TRUE);
	else if(!strcmp(tagName, CHAR8_CONST_STRING("array")))
		length																= CmpParseTagList(contentBuffer + position, theTag, XML_TAG_ARRAY, FALSE);
	else if(!strcmp(tagName, CHAR8_CONST_STRING("array/")))
		length																= CmpParseTagList(contentBuffer + position, theTag, XML_TAG_ARRAY, TRUE);
	else
		length																= 0;

	if(length == -1)
		return -1;

	return position + length;
}

//
// parse config file
//
EFI_STATUS CmParseXmlFile(CHAR8* fileBuffer, XML_TAG** rootTag)
{
	if(!rootTag)
		rootTag																= &CmpConfigPlistTag;

	XML_TAG* dictTag														= nullptr;
	INTN position															= 0;
	*rootTag																= nullptr;
	while(TRUE)
	{
		INTN length															= CmpParseNextTag(fileBuffer + position, &dictTag);
		if(length == -1)
			return EFI_INVALID_PARAMETER;

		position															+= length;
		if(!dictTag)
			continue;

		if(dictTag->Type == XML_TAG_DICT)
			break;

		CmFreeTag(dictTag);
		dictTag																= nullptr;
	}

	*rootTag																= dictTag;
	return dictTag ? EFI_SUCCESS : EFI_INVALID_PARAMETER;
}

//
// free tag
//
VOID CmFreeTag(XML_TAG* theTag)
{
	if(!theTag)
		return;

	if(theTag->StringValue)
		CmpFreeSymbol(theTag->StringValue);

	if(theTag->DataValue)
		MmFreePool(theTag->DataValue);

	CmFreeTag(theTag->TagValue);
	CmFreeTag(theTag->NextTag);

	theTag->Type															= XML_TAG_NONE;
	theTag->StringValue														= nullptr;
	theTag->IntegerValue													= 0;
	theTag->TagValue														= nullptr;
	theTag->DataValue														= nullptr;
	theTag->NextTag															= CmpNextFreeTag;
	CmpNextFreeTag															= theTag;
}

//
// get string value for key
//
CHAR8 CONST* CmGetStringValueForKeyAndCommandLine(CHAR8 CONST* commandLine, CHAR8 CONST* keyName, UINTN* valueLength, BOOLEAN checkConfigFile)
{
	if(valueLength)
		*valueLength														= 0;

	CHAR8 CONST* retValue													= nullptr;
	if(commandLine)
		retValue															= CmpGetCommandStringValueForKey(commandLine, keyName, valueLength);

	if(!retValue && checkConfigFile)
		retValue															= CmGetStringValueForKey(nullptr, keyName, valueLength);

	return retValue;
}

//
// get dict string value
//
CHAR8 CONST* CmSerializeValueForKey(CHAR8 CONST* keyName, UINTN* valueLength)
{
	if(valueLength)
		*valueLength														= 0;

	XML_TAG* theTag															= CmGetTagValueForKey(CmpConfigPlistTag, keyName);
	if(!theTag)
		return nullptr;

	CHAR8* outputBuffer														= nullptr;
	UINTN outputPosition													= 0;
	UINTN outputLength														= 0;
	CmpTagToString(theTag, &outputBuffer, &outputPosition, &outputLength);

	if(valueLength)
		*valueLength														= outputPosition;

	return outputBuffer;
}

//
// get tag
//
XML_TAG* CmGetTagValueForKey(XML_TAG* dictTag, CHAR8 CONST* keyName)
{
	if(!dictTag || dictTag->Type != XML_TAG_DICT)
		return nullptr;

	XML_TAG* theKey															= dictTag->TagValue;
	while(theKey)
	{
		if(theKey->Type == XML_TAG_KEY && !strcmp(theKey->StringValue, keyName))
			return theKey->TagValue;

		theKey																= theKey->NextTag;
	}

	return nullptr;
}

//
// get integer value for key
//
EFI_STATUS CmGetIntegerValueForKey(CHAR8 CONST* keyName, INT64* integerValue)
{
	XML_TAG* theTag															= CmGetTagValueForKey(CmpConfigPlistTag, keyName);
	if(!theTag)
		return EFI_NOT_FOUND;

	if(integerValue)
		*integerValue														= theTag->IntegerValue;

	return EFI_SUCCESS;
}

//
// get string value for key
//
CHAR8 CONST* CmGetStringValueForKey(XML_TAG* dictTag, CHAR8 CONST* keyName, UINTN* valueLength)
{
	if(valueLength)
		*valueLength														= 0;

	XML_TAG* theTag															= CmGetTagValueForKey(dictTag ? dictTag : CmpConfigPlistTag, keyName);
	if(!theTag)
		return nullptr;

	if(valueLength)
		*valueLength														= theTag->StringValue ? strlen(theTag->StringValue) : 0;

	return theTag->StringValue;
}

//
// get list element count
//
UINTN CmGetListTagElementsCount(XML_TAG* listTag)
{
	if(!listTag || listTag->Type != XML_TAG_ARRAY)
		return 0;

	UINTN retValue															= 0;
	XML_TAG* tagValue														= listTag->TagValue;
	while(tagValue)
	{
		retValue															+= 1;
		tagValue															= tagValue->NextTag;
	}

	return retValue;
}

//
// get list element by index
//
XML_TAG* CmGetListTagElementByIndex(XML_TAG* listTag, UINTN index)
{
	if(!listTag || listTag->Type != XML_TAG_ARRAY)
		return nullptr;

	XML_TAG* tagValue														= listTag->TagValue;
	for(UINTN i = 0; i < index && tagValue; i ++)
		tagValue															= tagValue->NextTag;

	return tagValue;
}