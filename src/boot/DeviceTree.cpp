//********************************************************************
//	created:	4:11:2009   10:54
//	filename: 	DeviceTree.cpp
//	author:		tiamo
//	purpose:	device tree
//********************************************************************

#include "stdafx.h"

//
// flatten property
//
typedef struct _DEVICE_TREE_PROPERTY_FLATTEN
{
	//
	// name
	//
	CHAR8																	Name[32];

	//
	// length
	//
	UINT32																	Length;
}DEVICE_TREE_PROPERTY_FLATTEN;

//
// flatten node
//
typedef struct _DEVICE_TREE_NODE_FLATTEN
{
	//
	// properties
	//
	UINT32																	Properties;

	//
	// children
	//
	UINT32																	Children;
}DEVICE_TREE_NODE_FLATTEN;

//
// global
//
STATIC DEVICE_TREE_NODE* DevTreepRootNode									= nullptr;
STATIC DEVICE_TREE_NODE* DevTreepNextFreeNode								= nullptr;
STATIC DEVICE_TREE_NODE* DevTreepAllocatedNodes								= nullptr;
STATIC DEVICE_TREE_PROPERTY* DevTreepNextFreeProperty						= nullptr;
STATIC DEVICE_TREE_PROPERTY* DevTreepAllocatedProperties					= nullptr;
STATIC UINT32 DevTreepTotalNodes											= 0;
STATIC UINT32 DevTreepTotalProperties										= 0;
STATIC UINT32 DevTreepTotalPropertiesSize									= 0;

//
// initialize
//
EFI_STATUS DevTreeInitialize()
{
	if(!DevTreeAddChild(nullptr, CHAR8_CONST_STRING("/")))
		return EFI_OUT_OF_RESOURCES;

	return EFI_SUCCESS;
}

//
// get property
//
STATIC DEVICE_TREE_PROPERTY* DevTreepGetProperty()
{
	if(!DevTreepNextFreeProperty)
	{
		VOID* propertiesBuffer												= MmAllocatePool(4096);
		if(!propertiesBuffer)
			return nullptr;

		DEVICE_TREE_PROPERTY* allocatedProperty								= static_cast<DEVICE_TREE_PROPERTY*>(propertiesBuffer);
		allocatedProperty->Next												= DevTreepAllocatedProperties;
		DevTreepAllocatedProperties											= allocatedProperty;
		allocatedProperty->Value											= propertiesBuffer;
		allocatedProperty->ValueBufferAllocated								= TRUE;

		allocatedProperty													+= 1;
		for(UINT32 i = 1; i < 4096 / sizeof(DEVICE_TREE_PROPERTY); i ++)
		{
			allocatedProperty->Next											= DevTreepNextFreeProperty;
			DevTreepNextFreeProperty										= allocatedProperty;
			allocatedProperty												+= 1;
		}
	}

	DEVICE_TREE_PROPERTY* theProperty										= DevTreepNextFreeProperty;
	DevTreepNextFreeProperty												= DevTreepNextFreeProperty->Next;
	memset(theProperty, 0, sizeof(DEVICE_TREE_PROPERTY));
	DevTreepTotalProperties													+= 1;
	return theProperty;
}

//
// free property
//
STATIC VOID DevTreepFreeProperty(DEVICE_TREE_PROPERTY* theProperty)
{
	if(theProperty->ValueBufferAllocated)
		MmFreePool(theProperty->Value);

	theProperty->Next														= DevTreepNextFreeProperty;
	DevTreepNextFreeProperty												= theProperty;
	DevTreepTotalProperties													-= 1;
}

//
// get node
//
STATIC DEVICE_TREE_NODE* DevTreepGetNode()
{
	if(!DevTreepNextFreeNode)
	{
		VOID* nodesBuffer													= MmAllocatePool(4096);
		if(!nodesBuffer)
			return nullptr;

		DEVICE_TREE_NODE* allocatedNode										= static_cast<DEVICE_TREE_NODE*>(nodesBuffer);
		allocatedNode->Next													= DevTreepAllocatedNodes;
		DevTreepAllocatedNodes												= allocatedNode;
		allocatedNode->Children												= allocatedNode;

		allocatedNode														+= 1;
		for(UINT32 i = 1; i < 4096 / sizeof(DEVICE_TREE_NODE); i ++)
		{
			allocatedNode->Next												= DevTreepNextFreeNode;
			DevTreepNextFreeNode											= allocatedNode;
			allocatedNode													+= 1;
		}
	}

	DEVICE_TREE_NODE* theNode												= DevTreepNextFreeNode;
	DevTreepNextFreeNode													= DevTreepNextFreeNode->Next;
	memset(theNode, 0, sizeof(DEVICE_TREE_NODE));
	DevTreepTotalNodes														+= 1;
	return theNode;
}

//
// free node
//
STATIC VOID DevTreepFreeNode(DEVICE_TREE_NODE* theNode)
{
	DEVICE_TREE_PROPERTY* theProperty										= theNode->Properties;
	while(theProperty)
	{
		DEVICE_TREE_PROPERTY* theNextProperty								= theProperty->Next;
		DevTreepFreeProperty(theProperty);
		theProperty															= theNextProperty;
	}

	theNode->Next															= DevTreepNextFreeNode;
	DevTreepNextFreeNode													= theNode;
	DevTreepTotalNodes														-= 1;
}

//
// get name
//
CHAR8 CONST* DevTreeGetName(DEVICE_TREE_NODE* theNode)
{
	for(DEVICE_TREE_PROPERTY* theProperty	= theNode->Properties; theProperty; theProperty = theProperty->Next)
	{
		if(!strcmp(theProperty->Name, CHAR8_CONST_STRING("name")))
			return static_cast<CHAR8*>(theProperty->Value);
	}

	return CHAR8_CONST_STRING("(null)");
}


//
// get property
//
VOID CONST* DevTreeGetProperty(DEVICE_TREE_NODE* theNode, CHAR8 CONST* propertyName, UINT32* propertyLength)
{
	for(DEVICE_TREE_PROPERTY* theProperty	= theNode->Properties; theProperty; theProperty = theProperty->Next)
	{
		if(!strcmp(theProperty->Name, propertyName))
		{
			if(propertyLength)
				*propertyLength												= theProperty->Length;

			return theProperty->Value;
		}
	}

	if(propertyLength)
		*propertyLength														= 0;

	return nullptr;
}

//
// find node by name
//
DEVICE_TREE_NODE* DevTreeFindNode(CHAR8 CONST* nodePath, BOOLEAN createIfMissing)
{
	DEVICE_TREE_NODE* theNode												= DevTreepRootNode;
	while(theNode)
	{
		while(*nodePath == '/')
			nodePath														+= 1;

		CHAR8 nameBuffer[ARRAYSIZE(theNode->Properties->Name)]				= {0};
		for(UINT32 i = 0; i < ARRAYSIZE(nameBuffer) && *nodePath && *nodePath != '/'; i ++)
			nameBuffer[i]													= *nodePath ++;

		if(nameBuffer[0] == 0)
			break;

		DEVICE_TREE_NODE* childNode											= theNode->Children;
		for(; childNode; childNode = childNode->Next)
		{
			if(!strcmp(DevTreeGetName(childNode), nameBuffer))
				break;
		}

		if(!childNode && createIfMissing)
			childNode														= DevTreeAddChild(theNode, nameBuffer);

		theNode																= childNode;
	}

	return theNode;
}

//
// add child
//
DEVICE_TREE_NODE* DevTreeAddChild(DEVICE_TREE_NODE* parentNode, CHAR8 CONST* childName)
{
	DEVICE_TREE_NODE* theNode												= DevTreepGetNode();
	if(!theNode)
		return nullptr;

	if(parentNode)
	{
		theNode->Next														= parentNode->Children;
		parentNode->Children												= theNode;
	}
	else
	{
		DevTreepRootNode													= theNode;
	}

	EFI_STATUS status														= DevTreeAddProperty(theNode, CHAR8_CONST_STRING("name"), childName, static_cast<UINT32>(strlen(childName) + 1) * sizeof(CHAR8), TRUE);
	if(!EFI_ERROR(status))
		return theNode;

	DevTreepFreeNode(theNode);
	return nullptr;
}

//
// add property
//
EFI_STATUS DevTreeAddProperty(DEVICE_TREE_NODE* theNode, CHAR8 CONST* propertyName, VOID CONST* propertyValue, UINT32 valueLength, BOOLEAN allocAndCopy)
{
	DEVICE_TREE_PROPERTY* theProperty										= DevTreepGetProperty();
	if(!theProperty)
		return EFI_OUT_OF_RESOURCES;

	strcpy(theProperty->Name, propertyName);
	theProperty->Length														= valueLength;

	if(allocAndCopy)
	{
		theProperty->Value													= MmAllocatePool(valueLength);
		if(!theProperty->Value)
			return EFI_OUT_OF_RESOURCES;

		memcpy(theProperty->Value, propertyValue, valueLength);
		theProperty->ValueBufferAllocated									= TRUE;
	}
	else
	{
		theProperty->Value													= const_cast<VOID*>(propertyValue);
		theProperty->ValueBufferAllocated									= FALSE;
	}

	if(!theNode->Properties)
		theNode->Properties													= theProperty;
	else
		theNode->LastProperty->Next											= theProperty;

	theNode->LastProperty													= theProperty;
	DevTreepTotalPropertiesSize												+= ((valueLength + sizeof(UINT32) - 1) & ~(sizeof(UINT32) - 1));

	return EFI_SUCCESS;
}

//
// free device tree
//
VOID DevTreeFinalize()
{
	DEVICE_TREE_NODE* theNode												= DevTreepAllocatedNodes;
	while(theNode)
	{
		DEVICE_TREE_NODE* theNextNode										= theNode->Next;
		MmFreePool(theNode->Children);
		theNode																= theNextNode;
	}
	DevTreepNextFreeNode													= nullptr;
	DevTreepAllocatedNodes													= nullptr;
	DevTreepRootNode														= nullptr;
	DevTreepTotalNodes														= 0;

	DEVICE_TREE_PROPERTY* theProperty										= DevTreepAllocatedProperties;
	while(theProperty)
	{
		DEVICE_TREE_PROPERTY* theNextProperty								= theProperty->Next;
		MmFreePool(theProperty->Value);
		theProperty															= theNextProperty;
	}
	DevTreepNextFreeProperty												= nullptr;
	DevTreepAllocatedProperties												= nullptr;
	DevTreepTotalPropertiesSize												= 0;
	DevTreepTotalProperties													= 0;
}

//
// flatten node
//
STATIC UINT8* DevTreepFlattenNode(DEVICE_TREE_NODE* theNode, UINT8* flattenBuffer)
{
	if(!theNode)
		return flattenBuffer;

	DEVICE_TREE_NODE_FLATTEN* flatNode										= static_cast<DEVICE_TREE_NODE_FLATTEN*>(static_cast<VOID*>(flattenBuffer));
	flattenBuffer															+= sizeof(DEVICE_TREE_NODE_FLATTEN);
	for(DEVICE_TREE_PROPERTY* theProperty	= theNode->Properties; theProperty; theProperty = theProperty->Next)
	{
		DEVICE_TREE_PROPERTY_FLATTEN* flatProperty							= static_cast<DEVICE_TREE_PROPERTY_FLATTEN*>(static_cast<VOID*>(flattenBuffer));
		strcpy(flatProperty->Name, theProperty->Name);
		flatProperty->Length												= theProperty->Length;
		flattenBuffer														+= sizeof(DEVICE_TREE_PROPERTY_FLATTEN);
		memcpy(flattenBuffer, theProperty->Value, theProperty->Length);
		flattenBuffer														+= ((theProperty->Length + sizeof(UINT32) - 1) & ~(sizeof(UINT32) - 1));
		flatNode->Properties												+= 1;
	}

	for(DEVICE_TREE_NODE* childNode = theNode->Children; childNode; childNode = childNode->Next)
	{
		flattenBuffer														= DevTreepFlattenNode(childNode, flattenBuffer);
		flatNode->Children													+= 1;
	}
	return flattenBuffer;
}

//
// flatten device tree
//
EFI_STATUS DevTreeFlatten(VOID** flattenBuffer, UINT32* bufferLength)
{
	UINT32 totalLength														= DevTreepTotalNodes * sizeof(DEVICE_TREE_NODE_FLATTEN);
	totalLength																+= DevTreepTotalProperties * sizeof(DEVICE_TREE_PROPERTY_FLATTEN) + DevTreepTotalPropertiesSize;
	VOID* deviceTreeBuffer													= nullptr;
	if(flattenBuffer)
	{
		if(totalLength)
		{
			if(!*flattenBuffer)
				deviceTreeBuffer											= MmAllocatePool(totalLength);
			else
				deviceTreeBuffer											= *flattenBuffer;

			if(!deviceTreeBuffer)
				return EFI_OUT_OF_RESOURCES;

			memset(deviceTreeBuffer, 0, totalLength);
			DevTreepFlattenNode(DevTreepRootNode, static_cast<UINT8*>(deviceTreeBuffer));
		}
		*flattenBuffer														= deviceTreeBuffer;
	}

	if(bufferLength)
		*bufferLength														= totalLength;

	return EFI_SUCCESS;
}