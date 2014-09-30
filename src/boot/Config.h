//********************************************************************
//	created:	8:11:2009   18:25
//	filename: 	Config.h
//	author:		tiamo
//	purpose:	config
//********************************************************************

#pragma once

//
// define
//
#define XML_TAG_NONE														0
#define XML_TAG_DICT														1
#define XML_TAG_KEY															2
#define XML_TAG_STRING														3
#define XML_TAG_INTEGER														4
#define XML_TAG_DATA														5
#define XML_TAG_DATE														6
#define XML_TAG_FALSE														7
#define XML_TAG_TRUE														8
#define XML_TAG_ARRAY														9

//
// xml tag
//
typedef struct _XML_TAG
{
	//
	// type
	//
	UINTN																	Type;

	//
	// string value
	//
	CHAR8*																	StringValue;

	//
	// int value
	//
	INT64																	IntegerValue;

	//
	// data buffer
	//
	VOID*																	DataValue;

	//
	// data length
	//
	UINTN																	DataLength;

	//
	// tag value
	//
	struct _XML_TAG*														TagValue;

	//
	// next tag
	//
	struct _XML_TAG*														NextTag;
}XML_TAG;

//
// get value for key
//
CHAR8 CONST* CmGetStringValueForKeyAndCommandLine(CHAR8 CONST* commandLine, CHAR8 CONST* keyName, UINTN* valueLength, BOOLEAN checkConfigFile);

//
// parse config file
//
EFI_STATUS CmParseXmlFile(CHAR8* fileBuffer, XML_TAG** rootTag);

//
// get tag
//
XML_TAG* CmGetTagValueForKey(XML_TAG* dictTag, CHAR8 CONST* keyName);

//
// get integer value for key
//
EFI_STATUS CmGetIntegerValueForKey(CHAR8 CONST* keyName, INT64* integerValue);

//
// get string value for key
//
CHAR8 CONST* CmGetStringValueForKey(XML_TAG* dictTag, CHAR8 CONST* keyName, UINTN* valueLength);

//
// get list element count
//
UINTN CmGetListTagElementsCount(XML_TAG* listTag);

//
// get list element by index
//
XML_TAG* CmGetListTagElementByIndex(XML_TAG* listTag, UINTN index);

//
// serialize value
//
CHAR8 CONST* CmSerializeValueForKey(CHAR8 CONST* keyName, UINTN* valueLength);

//
// free tag
//
VOID CmFreeTag(XML_TAG* theTag);