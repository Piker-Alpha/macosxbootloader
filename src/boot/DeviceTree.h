//********************************************************************
//	created:	4:11:2009   10:44
//	filename: 	DeviceTree.h
//	author:		tiamo
//	purpose:	device tree
//********************************************************************

#pragma once

//
// device tree property
//
typedef struct _DEVICE_TREE_PROPERTY
{
	//
	// name
	//
	CHAR8																	Name[32];

	//
	// value length
	//
	UINT32																	Length;

	//
	// value buffer
	//
	VOID*																	Value;

	//
	// free value buffer
	//
	BOOLEAN																	ValueBufferAllocated;

	//
	// next property
	//
	struct _DEVICE_TREE_PROPERTY*											Next;
}DEVICE_TREE_PROPERTY;

//
// device tree node
//
typedef struct _DEVICE_TREE_NODE
{
	//
	// properties
	//
	DEVICE_TREE_PROPERTY*													Properties;

	//
	// last property
	//
	DEVICE_TREE_PROPERTY*													LastProperty;

	//
	// children node
	//
	struct _DEVICE_TREE_NODE*												Children;

	//
	// next node
	//
	struct _DEVICE_TREE_NODE*												Next;
}DEVICE_TREE_NODE;

//
// initialize
//
EFI_STATUS DevTreeInitialize();

//
// get name
//
CHAR8 CONST* DevTreeGetName(DEVICE_TREE_NODE* theNode);

//
// get property
//
VOID CONST* DevTreeGetProperty(DEVICE_TREE_NODE* theNode, CHAR8 CONST* propertyName, UINT32* propertyLength);

//
// find node by name
//
DEVICE_TREE_NODE* DevTreeFindNode(CHAR8 CONST* nodePath, BOOLEAN createIfMissing);

//
// add child
//
DEVICE_TREE_NODE* DevTreeAddChild(DEVICE_TREE_NODE* parentNode, CHAR8 CONST* childName);

//
// add property
//
EFI_STATUS DevTreeAddProperty(DEVICE_TREE_NODE* theNode, CHAR8 CONST* propertyName, VOID CONST* propertyValue, UINT32 valueLength, BOOLEAN allocAndCopy);

//
// free device tree
//
VOID DevTreeFinalize();

//
// flatten
//
EFI_STATUS DevTreeFlatten(VOID** flattenBuffer, UINT32* bufferLength);