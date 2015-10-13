//********************************************************************
//	created:	19:9:2012   18:30
//	filename: 	FileVault.cpp
//	author:		tiamo	
//	purpose:	FileVault
//********************************************************************

#include "StdAfx.h"
#include "rijndael/aes.h"
#include "rijndael/aesxts.h"

#include <pshpack1.h>
typedef struct _CORE_STORAGE_VOLUME_HEADER
{
	//
	// 0x00 checksum
	//
	UINT64																	Checksum;

	//
	// 0x08 offset
	//
	UINT16																	Offset08;

	//
	// 0x0a major
	//
	UINT8																	MajorVersion;

	//
	// 0x0b minor
	//
	UINT8																	MinorVersion;

	//
	// 0x0c
	//
	UINT8																	Offset0C[0x58 - 0x0c];

	//
	// 0x58 signature
	//
	UINT32																	Signature;

	//
	// 0x5c
	//
	UINT8																	Offset5C[0xa8 - 0x5c];

	//
	// 0xa8 key length
	//
	UINT16																	KeyLength;

	//
	// 0xaa
	//
	UINT16																	OffsetAA;

	//
	// 0xac crypto algorithm
	//
	UINT16																	CryptoMethod;

	//
	// 0xae
	//
	UINT16																	OffsetAE;

	//
	// 0xb0 key1
	//
	UINT8																	EncryptedRootKey1[16];

	//
	// 0xc0
	//
	UINT8																	OffsetC0[0x200 - 0xc0];
}CORE_STORAGE_VOLUME_HEADER;

//
// key sign data
//
typedef struct _KEY_SIGN_DATA
{
	//
	// unknown
	//
	UINT64																	Reserved;

	//
	// hash round
	//
	UINT32																	HashRound;

	//
	// 0x9c sign
	//
	UINT8																	SignResult[0x20];

	//
	// sign part1
	//
	UINT8																	SignPart1[0x20];

	//
	// sign part3
	//
	UINT8																	SignPart3[0x20];
}KEY_SIGN_DATA;

//
// key wrapped data
//
typedef struct _KEY_WRAPPED_DATA
{
	//
	// cryptor method
	//
	UINT32																	CryptorMethod;

	//
	// ciphertext length
	//
	UINT32																	CiphertextLength;

	//
	// ciphertext
	//
	UINT8																	Ciphertext[0x88];

	//
	// sign
	//
	KEY_SIGN_DATA															SignData;
}KEY_WRAPPED_DATA;

//
// key unwrapped data
//
typedef struct _KEY_UNWRAPPED_DATA
{
	//
	// cryptor method
	//
	UINT32																	CryptorMethod;

	//
	// data length
	//
	UINT32																	DataLength;

	//
	// data buffer
	//
	UINT8																	DataBuffer[0x80];
}KEY_UNWRAPPED_DATA;

//
// passphase wrapped data
//
typedef struct _PASSPHASE_WRAPPED_DATA
{
	//
	// HMAC method
	//
	UINT32																	HMACMethod;

	//
	// salt length
	//
	UINT32																	SaltLength;

	//
	// salt
	//
	UINT8																	Salt[0x10];

	//
	// key length
	//
	UINT32																	KeyLength;

	//
	// ciphertext length
	//
	UINT32																	CiphertextLength;

	//
	// ciphertext
	//
	UINT8																	Ciphertext[0x88];

	//
	// round count
	//
	UINT32																	RoundCount;

	//
	// 0xac
	//
	UINT32																	OffsetAC;

	//
	// sign
	//
	KEY_SIGN_DATA															SignData;
}PASSPHASE_WRAPPED_DATA;

//
// passphase unwrapped data
//
typedef struct _PASSPHASE_UNWRAPPED_DATA
{
	//
	// key length
	//
	UINT32																	DataLength;

	//
	// key
	//
	UINT8																	DataBuffer[0x80];
}PASSPHASE_UNWRAPPED_DATA;

//
// key store data
//
typedef struct _KEY_STORE_DATA_HEADER
{
	//
	// next offset
	//
	UINT32																	NextOffset;

	//
	// type
	//
	UINT32																	Type;

	//
	// sub type
	//
	UINT32																	SubType;

	//
	// ident
	//
	UINT8																	Ident[0x10];

	//
	// ident2
	//
	UINT8																	Ident2[0x10];

	//
	// data length
	//
	UINT32																	DataLength;
}KEY_STORE_DATA_HEADER;
#include <poppack.h>

//
// volume key info
//
typedef struct _VOLUME_KEY_INFO
{
	//
	// wrapped volume key xml tag
	//
	XML_TAG*																WrappedVolumeKeyDictTag;

	//
	// unwrapped volume key data
	//
	KEY_UNWRAPPED_DATA														UnwrappedVolumeKeyData;
}VOLUME_KEY_INFO;

STATIC XML_TAG* FvpEncryptedRootPlistTag									= nullptr;
STATIC UINTN FvpWrappedVolumeKeyCount										= 0;
STATIC XML_TAG* FvpWrappedVolumeKeysListTag									= nullptr;
STATIC XML_TAG* FvpMasterKeyUserDictTag										= nullptr;
STATIC XML_TAG* FvpRecoveryKeyUserDictTag									= nullptr;

STATIC CHAR8 CONST* FvpResetIdent											= nullptr;
STATIC CHAR8 CONST* FvpLoginUnlockIdent										= nullptr;
STATIC CHAR8 CONST* FvpUserPassword											= nullptr;
STATIC CHAR8 CONST* FvpKeyEncryptingKeyIdent								= nullptr;
STATIC UINTN FvpVolumeKeyKEKLength											= 0;
STATIC UINT8 FvpVolumeKeyKEK[0x80]											= {0};

STATIC UINTN FvpVolumeKeyCount												= 0;
STATIC VOLUME_KEY_INFO* FvpVolumeKeyList									= nullptr;

//
// checksum
//
STATIC UINT64 FvpChecksum(VOID CONST* inputBuffer, UINTN bufferLength, UINT64 checksum)
{
	STATIC UINT32 crcTable[256] =
	{
		0x00000000, 0xf26b8303, 0xe13b70f7, 0x1350f3f4, 0xc79a971f, 0x35f1141c, 0x26a1e7e8, 0xd4ca64eb, 0x8ad958cf, 0x78b2dbcc, 0x6be22838, 0x9989ab3b, 0x4d43cfd0, 0xbf284cd3, 0xac78bf27, 0x5e133c24, 
		0x105ec76f, 0xe235446c, 0xf165b798, 0x030e349b, 0xd7c45070, 0x25afd373, 0x36ff2087, 0xc494a384, 0x9a879fa0, 0x68ec1ca3, 0x7bbcef57, 0x89d76c54, 0x5d1d08bf, 0xaf768bbc, 0xbc267848, 0x4e4dfb4b, 
		0x20bd8ede, 0xd2d60ddd, 0xc186fe29, 0x33ed7d2a, 0xe72719c1, 0x154c9ac2, 0x061c6936, 0xf477ea35, 0xaa64d611, 0x580f5512, 0x4b5fa6e6, 0xb93425e5, 0x6dfe410e, 0x9f95c20d, 0x8cc531f9, 0x7eaeb2fa, 
		0x30e349b1, 0xc288cab2, 0xd1d83946, 0x23b3ba45, 0xf779deae, 0x05125dad, 0x1642ae59, 0xe4292d5a, 0xba3a117e, 0x4851927d, 0x5b016189, 0xa96ae28a, 0x7da08661, 0x8fcb0562, 0x9c9bf696, 0x6ef07595, 
		0x417b1dbc, 0xb3109ebf, 0xa0406d4b, 0x522bee48, 0x86e18aa3, 0x748a09a0, 0x67dafa54, 0x95b17957, 0xcba24573, 0x39c9c670, 0x2a993584, 0xd8f2b687, 0x0c38d26c, 0xfe53516f, 0xed03a29b, 0x1f682198, 
		0x5125dad3, 0xa34e59d0, 0xb01eaa24, 0x42752927, 0x96bf4dcc, 0x64d4cecf, 0x77843d3b, 0x85efbe38, 0xdbfc821c, 0x2997011f, 0x3ac7f2eb, 0xc8ac71e8, 0x1c661503, 0xee0d9600, 0xfd5d65f4, 0x0f36e6f7, 
		0x61c69362, 0x93ad1061, 0x80fde395, 0x72966096, 0xa65c047d, 0x5437877e, 0x4767748a, 0xb50cf789, 0xeb1fcbad, 0x197448ae, 0x0a24bb5a, 0xf84f3859, 0x2c855cb2, 0xdeeedfb1, 0xcdbe2c45, 0x3fd5af46, 
		0x7198540d, 0x83f3d70e, 0x90a324fa, 0x62c8a7f9, 0xb602c312, 0x44694011, 0x5739b3e5, 0xa55230e6, 0xfb410cc2, 0x092a8fc1, 0x1a7a7c35, 0xe811ff36, 0x3cdb9bdd, 0xceb018de, 0xdde0eb2a, 0x2f8b6829, 
		0x82f63b78, 0x709db87b, 0x63cd4b8f, 0x91a6c88c, 0x456cac67, 0xb7072f64, 0xa457dc90, 0x563c5f93, 0x082f63b7, 0xfa44e0b4, 0xe9141340, 0x1b7f9043, 0xcfb5f4a8, 0x3dde77ab, 0x2e8e845f, 0xdce5075c, 
		0x92a8fc17, 0x60c37f14, 0x73938ce0, 0x81f80fe3, 0x55326b08, 0xa759e80b, 0xb4091bff, 0x466298fc, 0x1871a4d8, 0xea1a27db, 0xf94ad42f, 0x0b21572c, 0xdfeb33c7, 0x2d80b0c4, 0x3ed04330, 0xccbbc033, 
		0xa24bb5a6, 0x502036a5, 0x4370c551, 0xb11b4652, 0x65d122b9, 0x97baa1ba, 0x84ea524e, 0x7681d14d, 0x2892ed69, 0xdaf96e6a, 0xc9a99d9e, 0x3bc21e9d, 0xef087a76, 0x1d63f975, 0x0e330a81, 0xfc588982, 
		0xb21572c9, 0x407ef1ca, 0x532e023e, 0xa145813d, 0x758fe5d6, 0x87e466d5, 0x94b49521, 0x66df1622, 0x38cc2a06, 0xcaa7a905, 0xd9f75af1, 0x2b9cd9f2, 0xff56bd19, 0x0d3d3e1a, 0x1e6dcdee, 0xec064eed, 
		0xc38d26c4, 0x31e6a5c7, 0x22b65633, 0xd0ddd530, 0x0417b1db, 0xf67c32d8, 0xe52cc12c, 0x1747422f, 0x49547e0b, 0xbb3ffd08, 0xa86f0efc, 0x5a048dff, 0x8ecee914, 0x7ca56a17, 0x6ff599e3, 0x9d9e1ae0, 
		0xd3d3e1ab, 0x21b862a8, 0x32e8915c, 0xc083125f, 0x144976b4, 0xe622f5b7, 0xf5720643, 0x07198540, 0x590ab964, 0xab613a67, 0xb831c993, 0x4a5a4a90, 0x9e902e7b, 0x6cfbad78, 0x7fab5e8c, 0x8dc0dd8f, 
		0xe330a81a, 0x115b2b19, 0x020bd8ed, 0xf0605bee, 0x24aa3f05, 0xd6c1bc06, 0xc5914ff2, 0x37faccf1, 0x69e9f0d5, 0x9b8273d6, 0x88d28022, 0x7ab90321, 0xae7367ca, 0x5c18e4c9, 0x4f48173d, 0xbd23943e, 
		0xf36e6f75, 0x0105ec76, 0x12551f82, 0xe03e9c81, 0x34f4f86a, 0xc69f7b69, 0xd5cf889d, 0x27a40b9e, 0x79b737ba, 0x8bdcb4b9, 0x988c474d, 0x6ae7c44e, 0xbe2da0a5, 0x4c4623a6, 0x5f16d052, 0xad7d5351, 
	};

	UINT32 temp																= static_cast<UINT32>(checksum);
	UINT8 CONST* byteBuffer													= static_cast<UINT8 CONST*>(inputBuffer);
	for(UINTN i = 0; i < bufferLength; i ++, byteBuffer ++)
		temp																= crcTable[*byteBuffer ^ static_cast<UINT8>(temp)] ^ (temp >> 8);
	return (checksum & 0xffffffff00000000ULL) | temp;
}

//
// check volume header
//
STATIC BOOLEAN FvpValidateVolumeHeader(CORE_STORAGE_VOLUME_HEADER* volumeHeader)
{
	if(volumeHeader->MajorVersion != 0x10 || volumeHeader->MinorVersion != 0)
		return FALSE;

	if(volumeHeader->Signature == 0x5343)
		return TRUE;

	if(volumeHeader->Signature != 0x00015343)
		return FALSE;

	return FvpChecksum(&volumeHeader->Offset08, sizeof(CORE_STORAGE_VOLUME_HEADER) - EFI_FIELD_OFFSET(CORE_STORAGE_VOLUME_HEADER, Offset08), static_cast<UINT64>(-1)) == volumeHeader->Checksum;
}

//
// load core storage config
//
STATIC EFI_STATUS FvpLoadCoreStorageConfig(EFI_HANDLE coreStoragePartitionHandle)
{
	EFI_STATUS status														= EFI_SUCCESS;
	CHAR8* fileBuffer														= nullptr;
	UINTN fileSize															= 0;

	__try
	{
		//
		// skip netboot
		//
		if(IoBootingFromNet())
			try_leave(status = EFI_NOT_FOUND);

		//
		// read EncryptedRoot.plist.wipekey
		//
		if(EFI_ERROR(status = IoReadWholeFile(nullptr, CHAR8_CONST_STRING("System\\Library\\Caches\\com.apple.corestorage\\EncryptedRoot.plist.wipekey"), &fileBuffer, &fileSize, TRUE)))
		{
			if(EFI_ERROR(status = IoReadWholeFile(nullptr, CHAR8_CONST_STRING("EncryptedRoot.plist.wipekey"), &fileBuffer, &fileSize, TRUE)))
				try_leave(NOTHING);
		}

		//
		// check file size
		//
		if(!fileSize)
			try_leave(status = EFI_NOT_FOUND);

		//
		// open core storage block io protocol
		//
		EFI_BLOCK_IO_PROTOCOL* blockIoProtocol								= nullptr;
		if(EFI_ERROR(status = EfiBootServices->HandleProtocol(coreStoragePartitionHandle, &EfiBlockIoProtocolGuid, reinterpret_cast<VOID**>(&blockIoProtocol))))
			try_leave(NOTHING);

		//
		// open disk io protocol
		//
		EFI_DISK_IO_PROTOCOL* diskIoProtocol								= nullptr;
		if(EFI_ERROR(status = EfiBootServices->HandleProtocol(coreStoragePartitionHandle, &EfiDiskIoProtocolGuid, reinterpret_cast<VOID**>(&diskIoProtocol))))
			try_leave(NOTHING);

		//
		// read volume header
		//
		CORE_STORAGE_VOLUME_HEADER localHeader								= {0};
		UINT64 offset														= blockIoProtocol->Media->LastBlock * blockIoProtocol->Media->BlockSize;
		if(EFI_ERROR(status = diskIoProtocol->ReadDisk(diskIoProtocol, blockIoProtocol->Media->MediaId, offset, sizeof(localHeader), &localHeader)))
		{
			if(EFI_ERROR(status = diskIoProtocol->ReadDisk(diskIoProtocol, blockIoProtocol->Media->MediaId, 0, sizeof(localHeader), &localHeader)))
				try_leave(NOTHING);
		}

		//
		// check volume header
		//
		if(!FvpValidateVolumeHeader(&localHeader) || localHeader.CryptoMethod != 2)
			try_leave(status = EFI_VOLUME_CORRUPTED);

		//
		// decrypt with aes-xts
		//
		symmetric_xts xtsContext											= {0};
		UINT8 initVector[0x10]												= {0};
		xts_start(0, initVector, localHeader.EncryptedRootKey1, localHeader.KeyLength, initVector, 0x10, 0, 0, &xtsContext);
		xts_decrypt(fileBuffer, static_cast<unsigned long>(fileSize), fileBuffer, initVector, &xtsContext);
		xts_done(&xtsContext);

		//
		// load as plist
		//
		if(EFI_ERROR(status = CmParseXmlFile(fileBuffer, &FvpEncryptedRootPlistTag)))
			try_leave(NOTHING);
		
		//
		// get wrapped volume key list
		//
		FvpWrappedVolumeKeysListTag											= CmGetTagValueForKey(FvpEncryptedRootPlistTag, CHAR8_CONST_STRING("WrappedVolumeKeys"));
		if(!FvpWrappedVolumeKeysListTag || FvpWrappedVolumeKeysListTag->Type != XML_TAG_ARRAY)
			try_leave(status = EFI_VOLUME_CORRUPTED);

		//
		// get wrapped volume key count
		//
		FvpWrappedVolumeKeyCount											= CmGetListTagElementsCount(FvpWrappedVolumeKeysListTag);
		if(!FvpWrappedVolumeKeyCount)
			try_leave(status = EFI_VOLUME_CORRUPTED);

		//
		// get user list
		//
		XML_TAG* cryptoUserListTag											= CmGetTagValueForKey(FvpEncryptedRootPlistTag, CHAR8_CONST_STRING("CryptoUsers"));
		if(!cryptoUserListTag || cryptoUserListTag->Type != XML_TAG_ARRAY)
			try_leave(status = EFI_NOT_FOUND);

		//
		// check count
		//
		UINTN count															= CmGetListTagElementsCount(cryptoUserListTag);
		if(!count)
			try_leave(status = EFI_NOT_FOUND);

		//
		// get panel user count
		//
		for(UINTN i = 0; i < count; i ++)
		{
			//
			// get user dict
			//
			XML_TAG* cryptoUserDictTag										= CmGetListTagElementByIndex(cryptoUserListTag, i);
			if(!cryptoUserDictTag || cryptoUserDictTag->Type != XML_TAG_DICT)
				break;

			//
			// get user type
			//
			XML_TAG* userTypeTag											= CmGetTagValueForKey(cryptoUserDictTag, CHAR8_CONST_STRING("UserType"));
			if(!userTypeTag || userTypeTag->Type != XML_TAG_INTEGER)
				break;

			//
			// count panel user
			//
			if(userTypeTag->IntegerValue == 0x20080007)
				FvpMasterKeyUserDictTag										= cryptoUserDictTag;
			else if(userTypeTag->IntegerValue == 0x10010005)
				FvpRecoveryKeyUserDictTag									= cryptoUserDictTag;
		}
	}
	__finally
	{
		if(fileBuffer)
			MmFreePool(fileBuffer);
	}

	return status;
}

//
// aes unwrap
//
STATIC BOOLEAN FvpAESUnwrap(VOID CONST* kekBuffer, UINTN kekLength, UINT64 initValue, VOID CONST* ciphertext, UINTN ciphertextLength, VOID* plaintext, UINTN* plaintextLength)
{
	UINT8 A[sizeof(UINT64)]													= {0};
	memcpy(A, ciphertext, sizeof(A));

	UINTN n																	= ciphertextLength / sizeof(UINT64) - 1;
	UINT8* R																= static_cast<UINT8*>(plaintext);
	memcpy(R, Add2Ptr(ciphertext, sizeof(A), VOID CONST*), sizeof(UINT64) * n);

	aes_decrypt_ctx aesContext												= {0};
	aes_decrypt_key(static_cast<UINT8 CONST*>(kekBuffer), static_cast<INT32>(kekLength), &aesContext);
	
	for(INT32 j = 5; j >= 0; j --)
	{
		R																	= Add2Ptr(plaintext, (n - 1) * sizeof(UINT64), UINT8*);
		for(UINTN i = n; i >= 1; i --)
		{
			UINT8 B[16]														= {0};
			memcpy(B, A, sizeof(A));
			B[7]															^= n * j + i;

			memcpy(B + sizeof(A), R, sizeof(UINT64));
			aes_decrypt(B, B, &aesContext);
			memcpy(A, B, sizeof(A));
			memcpy(R, B + sizeof(A), sizeof(UINT64));
			R																-= sizeof(UINT64);
		}
	}
	
	return !memcmp(A, &initValue, sizeof(initValue));
}

//
// verify key data
//
STATIC BOOLEAN FvpVerifyKeyData(KEY_SIGN_DATA* keySignData, VOID CONST* messegeBuffer, UINTN messageLength)
{
	STATIC UINT8 localBuffer[0x200]											= {0};
	UINT8 d1[0x20]															= {0};
	UINT8 d2[0x20]															= {0};
	memcpy(localBuffer, keySignData->SignPart1, sizeof(keySignData->SignPart1));
	memcpy(localBuffer + sizeof(keySignData->SignPart1) + sizeof(UINT8), messegeBuffer, messageLength);
	memcpy(localBuffer + sizeof(keySignData->SignPart1) + sizeof(UINT8) + messageLength, keySignData->SignPart3, sizeof(keySignData->SignPart3));
	SHA256(localBuffer, sizeof(keySignData->SignPart1) + messageLength + sizeof(keySignData->SignPart3), d1);
	for(UINTN i = 1; i < keySignData->HashRound; i ++)
	{
		memcpy(d2, d1, sizeof(d1));
		SHA256(d2, sizeof(d2), d1);
	}

	for(UINTN i = 0; i < sizeof(d1); i ++)
	{
		if(d1[i] != keySignData->SignResult[i])
			return FALSE;
	}
	return TRUE;
}

//
// key unwrap data
//
STATIC BOOLEAN FvpKeyUnwrapData(VOID CONST* kekBuffer, UINTN kekLength, KEY_WRAPPED_DATA* keyWrappedData, KEY_UNWRAPPED_DATA* keyUnwrappedData)
{
	UINTN plaintextLength													= keyWrappedData->CiphertextLength - 8;
	UINT64 initValue														= 0xa6a6a6a6a6a6a6a6ULL;
	if(!FvpAESUnwrap(kekBuffer, kekLength, initValue, keyWrappedData->Ciphertext, keyWrappedData->CiphertextLength, keyUnwrappedData->DataBuffer, &plaintextLength))
		return FALSE;

	keyUnwrappedData->DataLength											= static_cast<UINT32>(plaintextLength);
	keyUnwrappedData->CryptorMethod											= keyWrappedData->CryptorMethod;
	return FvpVerifyKeyData(&keyWrappedData->SignData, keyUnwrappedData->DataBuffer, keyUnwrappedData->DataLength);
}

//
// decrypt volume kek with master key user
//
STATIC EFI_STATUS FvpDecryptVolumeKEKWithMasterKeyUser()
{
	if(!FvpMasterKeyUserDictTag || FvpMasterKeyUserDictTag->Type != XML_TAG_DICT)
		return EFI_NOT_FOUND;

	XML_TAG* externalKeyPropsDictTag										= CmGetTagValueForKey(FvpMasterKeyUserDictTag, CHAR8_CONST_STRING("ExternalKeyProps"));
	if(!externalKeyPropsDictTag || externalKeyPropsDictTag->Type != XML_TAG_DICT)
		return EFI_NOT_FOUND;

	XML_TAG* revertUserVersion												= CmGetTagValueForKey(externalKeyPropsDictTag, CHAR8_CONST_STRING("revert-user-version"));
	if(!revertUserVersion || revertUserVersion->Type != XML_TAG_INTEGER || revertUserVersion->IntegerValue != 1)
		return EFI_NOT_FOUND;

	XML_TAG* keyWrappedKEKStructTag											= CmGetTagValueForKey(FvpMasterKeyUserDictTag, CHAR8_CONST_STRING("KeyWrappedKEKStruct"));
	if(!keyWrappedKEKStructTag || keyWrappedKEKStructTag->Type != XML_TAG_DATA)
		return EFI_NOT_FOUND;

	XML_TAG* wrapperVersionTag												= CmGetTagValueForKey(FvpMasterKeyUserDictTag, CHAR8_CONST_STRING("WrapperVersion"));
	if(!wrapperVersionTag || wrapperVersionTag->Type != XML_TAG_INTEGER || wrapperVersionTag->IntegerValue != 1)
		return EFI_NOT_FOUND;

	XML_TAG* keyEncryptingKeyIdentTag										= CmGetTagValueForKey(FvpMasterKeyUserDictTag, CHAR8_CONST_STRING("KeyEncryptingKeyIdent"));
	if(!keyEncryptingKeyIdentTag || keyEncryptingKeyIdentTag->Type != XML_TAG_STRING)
		return EFI_NOT_FOUND;

	FvpKeyEncryptingKeyIdent												= keyEncryptingKeyIdentTag->StringValue;
	KEY_UNWRAPPED_DATA keyUnwrappedData										= {0};
	STATIC UINT8 keyBuffer[16]												= {0};
	if(!FvpKeyUnwrapData(keyBuffer, sizeof(keyBuffer), static_cast<KEY_WRAPPED_DATA*>(keyWrappedKEKStructTag->DataValue), &keyUnwrappedData))
		return EFI_NOT_FOUND;

	FvpVolumeKeyKEKLength													= keyUnwrappedData.DataLength;
	memcpy(FvpVolumeKeyKEK, keyUnwrappedData.DataBuffer, FvpVolumeKeyKEKLength);
	return EFI_SUCCESS;
}

//
// hmac-sha256
//
STATIC VOID FvpHMACSHA256(VOID CONST* messageBuffer, UINTN messageLength, VOID CONST* keyBuffer, UINTN keyLength, UINT8* resultBuffer)
{
	SHA256_CONTEXT sha256Context											= {0};
	UINT8 kPad[0x40]														= {0};
	UINT8 tk[0x20]															= {0};
	if(keyLength > sizeof(kPad))
	{
		SHA256_Init(&sha256Context);
		SHA256_Update(keyBuffer, keyLength, &sha256Context);
		SHA256_Final(tk, &sha256Context);

		keyBuffer															= tk;
		keyLength															= sizeof(tk);
	}

	memcpy(kPad, keyBuffer, keyLength);
	for(UINT32 i = 0; i < sizeof(kPad); i++)
		kPad[i]																^= 0x36;

	SHA256_Init(&sha256Context);
	SHA256_Update(kPad, sizeof(kPad), &sha256Context);
	SHA256_Update(messageBuffer, messageLength, &sha256Context);
	SHA256_Final(resultBuffer, &sha256Context);

	memset(kPad, 0, sizeof(kPad));
	memcpy(kPad, keyBuffer, keyLength);
	for(UINT32 i = 0; i < sizeof(kPad); i++)
		kPad[i]																^= 0x5c;

	SHA256_Init(&sha256Context);
	SHA256_Update(kPad, sizeof(kPad), &sha256Context);
	SHA256_Update(resultBuffer, sizeof(tk), &sha256Context);
	SHA256_Final(resultBuffer, &sha256Context);
}

//
// pbkdf2
//
STATIC BOOLEAN FvpPBKDF2(VOID CONST* password, UINTN passwordLength, UINT8 CONST* salt, UINTN saltLength, UINT32 hmacMethod, UINT32 roundCount, VOID* keyBuffer, UINTN keyLength)
{
	if(!salt || !saltLength || hmacMethod != 3 || !roundCount || !keyBuffer || !keyLength)
		return FALSE;

	UINT8 d1[0x20]															= {0};
	UINT8 d2[0x20]															= {0};
	UINT8 output[0x20]														= {0};
	UINT8 localSalt[0x100]													= {0};
	memcpy(localSalt, salt, saltLength);

	for(UINT32 count = 1; keyLength > 0; count ++)
	{
		localSalt[saltLength + 0]											= (count >> 24) & 0xff;
		localSalt[saltLength + 1]											= (count >> 16) & 0xff;
		localSalt[saltLength + 2]											= (count >>  8) & 0xff;
		localSalt[saltLength + 3]											= (count >>  0) & 0xff;
		FvpHMACSHA256(localSalt, saltLength + 4, password, passwordLength, d1);
		memcpy(output, d1, sizeof(output));

		for(UINT32 i = 1; i < roundCount; i ++) 
		{
			FvpHMACSHA256(d1, sizeof(d1), password, passwordLength, d2);
			memcpy(d1, d2, sizeof(d2));
			for(UINT32 j = 0; j < sizeof(output); j ++)
				output[j]													^= d1[j];
		}

		UINTN length														= keyLength < sizeof(output) ? keyLength : sizeof(output);
		memcpy(keyBuffer, output, length);
		keyBuffer															= Add2Ptr(keyBuffer, length, VOID*);
		keyLength															-= length;
	}
	
	return TRUE;
}

//
// passphase unwrap data
//
STATIC BOOLEAN FvpPassphaseUnwrapData(CHAR8 CONST* password, UINTN passwordLength, PASSPHASE_WRAPPED_DATA* passphaseWrappedData, PASSPHASE_UNWRAPPED_DATA* passphaseUnwrappedData)
{
	STATIC UINT8 keyBuffer[0x100]											= {0};
	if(!FvpPBKDF2(password, passwordLength, passphaseWrappedData->Salt, passphaseWrappedData->SaltLength, passphaseWrappedData->HMACMethod, passphaseWrappedData->RoundCount, keyBuffer, passphaseWrappedData->KeyLength))
		return FALSE;

	UINTN plaintextLength													= passphaseWrappedData->CiphertextLength - 8;
	UINT64 initValue														= 0xa6a6a6a6a6a6a6a6ULL;
	if(!FvpAESUnwrap(keyBuffer, passphaseWrappedData->KeyLength, initValue, passphaseWrappedData->Ciphertext, passphaseWrappedData->CiphertextLength, passphaseUnwrappedData->DataBuffer, &plaintextLength))
		return FALSE;

	passphaseUnwrappedData->DataLength										= static_cast<UINT32>(plaintextLength);
	return FvpVerifyKeyData(&passphaseWrappedData->SignData, passphaseUnwrappedData->DataBuffer, passphaseUnwrappedData->DataLength);
}

//
// decrypt volume kek with user password
//
STATIC EFI_STATUS FvpDecryptVolumeKEKWithUserPassword(XML_TAG* cryptoUserDictTag, XML_TAG* resetUserDictTag, CHAR8 CONST* password)
{
	if(!cryptoUserDictTag || cryptoUserDictTag->Type != XML_TAG_DICT)
		return EFI_NOT_FOUND;

	XML_TAG* passphraseWrappedKEKStruct										= CmGetTagValueForKey(cryptoUserDictTag, CHAR8_CONST_STRING("PassphraseWrappedKEKStruct"));
	if(!passphraseWrappedKEKStruct || passphraseWrappedKEKStruct->Type != XML_TAG_DATA)
		return EFI_NOT_FOUND;

	XML_TAG* wrapperVersionTag												= CmGetTagValueForKey(cryptoUserDictTag, CHAR8_CONST_STRING("WrapVersion"));
	if(!wrapperVersionTag || wrapperVersionTag->Type != XML_TAG_INTEGER || wrapperVersionTag->IntegerValue != 1)
		return EFI_NOT_FOUND;

	XML_TAG* keyEncryptingKeyIdentTag										= CmGetTagValueForKey(cryptoUserDictTag, CHAR8_CONST_STRING("KeyEncryptingKeyIdent"));
	if(!keyEncryptingKeyIdentTag || keyEncryptingKeyIdentTag->Type != XML_TAG_STRING)
		return EFI_NOT_FOUND;

	XML_TAG* userIdentTag													= CmGetTagValueForKey(cryptoUserDictTag, CHAR8_CONST_STRING("UserIdent"));
	if(!userIdentTag || userIdentTag->Type != XML_TAG_STRING)
		return EFI_NOT_FOUND;

	PASSPHASE_UNWRAPPED_DATA passphaseUnwrappedData							= {0};
	if(!FvpPassphaseUnwrapData(password, strlen(password), static_cast<PASSPHASE_WRAPPED_DATA*>(passphraseWrappedKEKStruct->DataValue), &passphaseUnwrappedData))
		return EFI_NOT_FOUND;

	if(resetUserDictTag)
	{
		XML_TAG* resetUserIdentTag											= CmGetTagValueForKey(resetUserDictTag, CHAR8_CONST_STRING("UserIdent"));
		if(!resetUserIdentTag || resetUserIdentTag->Type != XML_TAG_STRING)
			return EFI_NOT_FOUND;

		FvpResetIdent														= resetUserIdentTag->StringValue;
	}

	FvpLoginUnlockIdent														= userIdentTag->StringValue;
	FvpUserPassword															= password;
	FvpKeyEncryptingKeyIdent												= keyEncryptingKeyIdentTag->StringValue;
	FvpVolumeKeyKEKLength													= passphaseUnwrappedData.DataLength;
	memcpy(FvpVolumeKeyKEK, passphaseUnwrappedData.DataBuffer, FvpVolumeKeyKEKLength);

	return EFI_SUCCESS;
}

//
// find crypto user dict by name
//
STATIC XML_TAG* FvpFindCryptoUserDictTypeByName(CHAR8 CONST* userName)
{
	//
	// get user list
	//
	XML_TAG* cryptoUserListTag												= CmGetTagValueForKey(FvpEncryptedRootPlistTag, CHAR8_CONST_STRING("CryptoUsers"));
	if(!cryptoUserListTag || cryptoUserListTag->Type != XML_TAG_ARRAY)
		return nullptr;

	//
	// check count
	//
	UINTN count																= CmGetListTagElementsCount(cryptoUserListTag);
	if(!count)
		return nullptr;

	//
	// get panel user count
	//
	for(UINTN i = 0; i < count; i ++)
	{
		//
		// get user dict
		//
		XML_TAG* cryptoUserDictTag											= CmGetListTagElementByIndex(cryptoUserListTag, i);
		if(!cryptoUserDictTag || cryptoUserDictTag->Type != XML_TAG_DICT)
			continue;

		//
		// get name data array
		//
		XML_TAG* userNameDataListTag										= CmGetTagValueForKey(cryptoUserDictTag, CHAR8_CONST_STRING("UserNamesData"));
		if(!userNameDataListTag)
			continue;

		//
		// single name
		//
		if(userNameDataListTag->Type == XML_TAG_STRING)
		{
			if(!strnicmp(userNameDataListTag->StringValue, userName, strlen(userName)))
				return cryptoUserDictTag;
		}
		else if(userNameDataListTag->Type == XML_TAG_ARRAY)
		{
			UINTN nameDataCount												= CmGetListTagElementsCount(userNameDataListTag);
			if(!nameDataCount)
				continue;

			for(UINTN j = 0; j < nameDataCount; j ++)
			{
				XML_TAG* userNameDataTag									= CmGetListTagElementByIndex(userNameDataListTag, j);
				if(!userNameDataTag || userNameDataTag->Type != XML_TAG_DATA)
					continue;

				if(!strnicmp(static_cast<CHAR8*>(userNameDataTag->DataValue), userName, userNameDataTag->DataLength))
					return cryptoUserDictTag;
			}
		}
	}

	return nullptr;
}

//
// read input
//
STATIC VOID FvpReadInput(CHAR8** inputBuffer, UINTN* inputLength, UINTN maxLength, BOOLEAN echoInput)
{
	STATIC CHAR16* inputBuffer16											= nullptr;
	UINTN inputLength16														= 0;
	UINTN bufferLength														= 0;
	UINTN maxColumn															= 0;
	UINTN maxRow															= 0;
	EfiSystemTable->ConOut->QueryMode(EfiSystemTable->ConOut, EfiSystemTable->ConOut->Mode->Mode, &maxColumn, &maxRow);
	while(!maxLength || inputLength16 < maxLength)
	{
		UINTN eventIndex													= 0;
		EFI_INPUT_KEY inputKey												= {0};
		EfiBootServices->WaitForEvent(1, &EfiSystemTable->ConIn->WaitForKey, &eventIndex);
		if(EFI_ERROR(EfiSystemTable->ConIn->ReadKeyStroke(EfiSystemTable->ConIn, &inputKey)))
			continue;

		if(inputKey.UnicodeChar == CHAR_CARRIAGE_RETURN)
		{
			EfiSystemTable->ConOut->OutputString(EfiSystemTable->ConOut, CHAR16_STRING(L"\r\n"));
			break;
		}
		else if(inputKey.UnicodeChar == CHAR_BACKSPACE)
		{
			if(inputLength16)
			{
				inputLength16												-= 1;
				UINTN curColumn												= static_cast<UINTN>(EfiSystemTable->ConOut->Mode->CursorColumn);
				UINTN curRow												= static_cast<UINTN>(EfiSystemTable->ConOut->Mode->CursorRow);
				if(curColumn)
				{
					curColumn												-= 1;
				}
				else
				{
					curColumn												= maxColumn - 1;
					if(curRow)
						curRow												-= 1;
				}
				EfiSystemTable->ConOut->SetCursorPosition(EfiSystemTable->ConOut, curColumn, curRow);
				EfiSystemTable->ConOut->OutputString(EfiSystemTable->ConOut, CHAR16_STRING(L" "));
				EfiSystemTable->ConOut->SetCursorPosition(EfiSystemTable->ConOut, curColumn, curRow);
			}
		}
		else if(inputKey.UnicodeChar >= ' ')
		{
			if(inputLength16 >= bufferLength)
			{
				bufferLength												= inputLength16 * 2;
				if(bufferLength < 80)
					bufferLength											= 80;
				CHAR16* newBuffer											= static_cast<CHAR16*>(MmAllocatePool(bufferLength * sizeof(CHAR16)));
				memcpy(newBuffer, inputBuffer16, inputLength16);
				MmFreePool(inputBuffer16);
				inputBuffer16												= newBuffer;
			}
			
			inputBuffer16[inputLength16]									= inputKey.UnicodeChar;
			inputLength16													+= 1;

			CHAR16 outputString[2]											= {echoInput ? inputKey.UnicodeChar : L'*', 0};
			EfiSystemTable->ConOut->OutputString(EfiSystemTable->ConOut, outputString);
		}
	}

	if(*inputLength < inputLength16 * 3 + 2)
	{
		if(*inputBuffer)
			MmFreePool(*inputBuffer);

		*inputLength														= inputLength16 * 3 + 2;
		*inputBuffer														= static_cast<CHAR8*>(MmAllocatePool(*inputLength));
	}

	BlUnicodeToUtf8(inputBuffer16, inputLength16, *inputBuffer, *inputLength);
}

//
// read selection
//
STATIC UINTN FvpReadSelection(UINTN maxCount)
{
	while(TRUE)
	{
		UINTN eventIndex													= 0;
		EFI_INPUT_KEY inputKey												= {0};
		EfiBootServices->WaitForEvent(1, &EfiSystemTable->ConIn->WaitForKey, &eventIndex);
		if(EFI_ERROR(EfiSystemTable->ConIn->ReadKeyStroke(EfiSystemTable->ConIn, &inputKey)))
			continue;

		if(inputKey.UnicodeChar >= '1' && inputKey.UnicodeChar < '1' + maxCount)
			return inputKey.UnicodeChar - '1';
	}
	return 0;
}

//
// unlock core volume
//
STATIC EFI_STATUS FvpUnlockCoreVolumeKey()
{
	//
	// unlock with master key user
	//
	if(EFI_ERROR(FvpDecryptVolumeKEKWithMasterKeyUser()))
	{
		CsSetConsoleMode(TRUE, TRUE);
		EfiSystemTable->ConOut->ClearScreen(EfiSystemTable->ConOut);
		CHAR8* userName														= nullptr;
		UINTN userNameLength												= 0;
		CHAR8* password														= nullptr;
		UINTN passwordLength												= 0;
		while(TRUE)
		{
			//
			// read user name
			//
			CsPrintf(CHAR8_CONST_STRING("\nLogin: "));
			FvpReadInput(&userName, &userNameLength, 0, TRUE);
			
			//
			// check user name
			//
			XML_TAG* cryptoUserDictTag										= FvpFindCryptoUserDictTypeByName(userName);
			if(cryptoUserDictTag)
			{
				//
				// small menu
				//
				CsPrintf(CHAR8_CONST_STRING("\t1.Enter password\n\t2.Reset password with recovery key\n\t3.Reboot\n\t4.Shutdown\n"));
				UINTN selection												= FvpReadSelection(4);
				if(!selection)
				{
					//
					// read password
					//
					CsPrintf(CHAR8_CONST_STRING("Password: "));
					FvpReadInput(&password, &passwordLength, 0, FALSE);

					//
					// unlock with it
					//
					if(!EFI_ERROR(FvpDecryptVolumeKEKWithUserPassword(cryptoUserDictTag, nullptr, password)))
						break;
					CsPrintf(CHAR8_CONST_STRING("Login failed: invalid password\n"));
				}
				else if(selection == 1)
				{
					//
					// read recovery key
					//
					CsPrintf(CHAR8_CONST_STRING("Recovery key: "));
					FvpReadInput(&password, &passwordLength, 0, TRUE);

					//
					// 1->L,l->L,S->5
					//
					for(UINTN i = 0; password[i]; i ++)
					{
						CHAR8& ch											= password[i];
						if(ch == '1' || ch == 'l')
							ch												= 'L';
						else if(ch == 'S')
							ch												= '5';
					}

					//
					// unlock with it
					//
					if(!EFI_ERROR(FvpDecryptVolumeKEKWithUserPassword(FvpRecoveryKeyUserDictTag, cryptoUserDictTag, password)))
						break;
					CsPrintf(CHAR8_CONST_STRING("Login failed: invalid recovery key\n"));
				}
				else if(selection == 2)
				{
					EfiRuntimeServices->ResetSystem(EfiResetCold, EFI_SUCCESS, 0, nullptr);
				}
				else if(selection == 3)
				{
					EfiRuntimeServices->ResetSystem(EfiResetShutdown, EFI_SUCCESS, 0, nullptr);
				}
			}
			else
			{
				CsPrintf(CHAR8_CONST_STRING("Login failed: invalid user\n"));
			}
		}
	}

	//
	// allocate key buffer
	//
	FvpVolumeKeyCount														= 0;
	UINTN length															= sizeof(VOLUME_KEY_INFO) * FvpWrappedVolumeKeyCount;
	FvpVolumeKeyList														= static_cast<VOLUME_KEY_INFO*>(MmAllocatePool(length));
	if(!FvpVolumeKeyList)
		return EFI_OUT_OF_RESOURCES;

	//
	// decrypt volume key
	//
	for(UINTN i = 0; i < FvpWrappedVolumeKeyCount; i ++)
	{
		XML_TAG* wrappedVolumeKeyDictTag									= CmGetListTagElementByIndex(FvpWrappedVolumeKeysListTag, i);
		if(!wrappedVolumeKeyDictTag || wrappedVolumeKeyDictTag->Type != XML_TAG_DICT)
			continue;

		XML_TAG* keyEncryptingKeyIdentStringTag								= CmGetTagValueForKey(wrappedVolumeKeyDictTag, CHAR8_CONST_STRING("KeyEncryptingKeyIdent"));
		if(!keyEncryptingKeyIdentStringTag || keyEncryptingKeyIdentStringTag->Type != XML_TAG_STRING || strcmp(keyEncryptingKeyIdentStringTag->StringValue, FvpKeyEncryptingKeyIdent))
			continue;

		XML_TAG* kekWrappedVolumeKeyStructTag								= CmGetTagValueForKey(wrappedVolumeKeyDictTag, CHAR8_CONST_STRING("KEKWrappedVolumeKeyStruct"));
		if(!kekWrappedVolumeKeyStructTag || kekWrappedVolumeKeyStructTag->Type != XML_TAG_DATA)
			continue;

		XML_TAG* wrapVersionTag												= CmGetTagValueForKey(wrappedVolumeKeyDictTag, CHAR8_CONST_STRING("WrapVersion"));
		if(!wrapVersionTag || wrapVersionTag->Type != XML_TAG_INTEGER || wrapVersionTag->IntegerValue != 1)
			continue;

		KEY_WRAPPED_DATA* keyWrappedData									= static_cast<KEY_WRAPPED_DATA*>(kekWrappedVolumeKeyStructTag->DataValue);
		VOLUME_KEY_INFO* volumeKeyInfo										= FvpVolumeKeyList + FvpVolumeKeyCount;
		if(!FvpKeyUnwrapData(FvpVolumeKeyKEK, FvpVolumeKeyKEKLength, keyWrappedData, &volumeKeyInfo->UnwrappedVolumeKeyData))
			continue;

		volumeKeyInfo->WrappedVolumeKeyDictTag								= wrappedVolumeKeyDictTag;
		FvpVolumeKeyCount													+= 1;
	}
	return EFI_SUCCESS;
}

//
// load and unlock core volume
//
EFI_STATUS FvLookupUnlockCoreVolumeKey(EFI_DEVICE_PATH_PROTOCOL* bootDevicePath, BOOLEAN resumeFromCoreStorage)
{
	//
	// core storage partition is followed by recovery partition
	//
	UINT32 partionNumber													= DevPathGetPartitionNumber(bootDevicePath);
	if(partionNumber == -1)
		return EFI_INVALID_PARAMETER;

	//
	// get core storage partition
	//
	EFI_HANDLE coreStoragePartitionHandle									= DevPathGetPartitionHandleByNumber(bootDevicePath, partionNumber - 1);
	if(!coreStoragePartitionHandle)
		return EFI_INVALID_PARAMETER;

	//
	// load core storage config
	//
	EFI_STATUS status														= FvpLoadCoreStorageConfig(coreStoragePartitionHandle);
	if(EFI_ERROR(status))
		return status;

	//
	// unlock
	//
	BlSetBootMode(BOOT_MODE_HAS_FILE_VAULT2_CONFIG, 0);
	CsConnectDevice(TRUE, resumeFromCoreStorage);
	return FvpUnlockCoreVolumeKey();
}

//
// find volume key
//
BOOLEAN FvFindCoreVolumeKey(UINT8 CONST* volumeIdent, UINT8* volumeKey, UINTN volumeKeyLength)
{
	for(UINTN i = 0; i < FvpVolumeKeyCount; i ++)
	{
		XML_TAG* volumeIdentTag												= CmGetTagValueForKey(FvpVolumeKeyList[i].WrappedVolumeKeyDictTag, CHAR8_CONST_STRING("VolumeKeyIdent"));
		if(!volumeIdentTag || volumeIdentTag->Type != XML_TAG_STRING)
			continue;

		UINT8 theVolumeIdent[0x10]											= {0};
		if(!BlUUIDStringToBuffer(volumeIdentTag->StringValue, theVolumeIdent))
			continue;

		if(memcmp(volumeIdent, theVolumeIdent, sizeof(theVolumeIdent)))
			continue;

		if(volumeKeyLength > FvpVolumeKeyList[i].UnwrappedVolumeKeyData.DataLength)
			volumeKeyLength													= FvpVolumeKeyList[i].UnwrappedVolumeKeyData.DataLength;
		
		memcpy(volumeKey, FvpVolumeKeyList[i].UnwrappedVolumeKeyData.DataBuffer, volumeKeyLength);
		return TRUE;
	}

	return FALSE;
}

//
// setup device tree
//
EFI_STATUS FvSetupDeviceTree(UINT64* keyStorePhysicalAddress, UINTN* keyStoreSize, BOOLEAN setupDeviceTree)
{
	if(FvpLoginUnlockIdent && setupDeviceTree)
	{
		DEVICE_TREE_NODE* chosenNode										= DevTreeFindNode(CHAR8_CONST_STRING("/chosen"), TRUE);
		if(chosenNode)
		{
			if(FvpResetIdent)
				DevTreeAddProperty(chosenNode, CHAR8_CONST_STRING("efilogin-reset-ident"), FvpResetIdent, static_cast<UINT32>(strlen(FvpResetIdent) + 1), FALSE);

			DevTreeAddProperty(chosenNode, CHAR8_CONST_STRING("efilogin-unlock-ident"), FvpLoginUnlockIdent, static_cast<UINT32>(strlen(FvpLoginUnlockIdent) + 1), FALSE);
			DevTreeAddProperty(chosenNode, CHAR8_CONST_STRING("efilogin-user-ident"), FvpLoginUnlockIdent, static_cast<UINT32>(strlen(FvpLoginUnlockIdent) + 1), FALSE);
		}
	}

	UINTN totalLength														= FvpUserPassword ? (sizeof(KEY_STORE_DATA_HEADER) + strlen(FvpUserPassword) + 7) & ~7 : 0;
	totalLength																+= FvpKeyEncryptingKeyIdent ? (sizeof(KEY_STORE_DATA_HEADER) + FvpVolumeKeyKEKLength + 7) & ~7 : 0;
	*keyStoreSize															= totalLength;

	if(totalLength && keyStorePhysicalAddress)
	{
		if(!*keyStorePhysicalAddress)
			*keyStorePhysicalAddress										= MmAllocateKernelMemory(&totalLength, nullptr);

		if(!*keyStorePhysicalAddress)
			return EFI_OUT_OF_RESOURCES;

		KEY_STORE_DATA_HEADER* header										= ArchConvertAddressToPointer(*keyStorePhysicalAddress, KEY_STORE_DATA_HEADER*);
		UINT32 nextOffset													= 0;

		if(FvpUserPassword)
		{
			header->Type													= 2;
			header->SubType													= 0;
			header->DataLength												= static_cast<UINT32>(strlen(FvpUserPassword));
			nextOffset														+= (header->DataLength + sizeof(KEY_STORE_DATA_HEADER) + 7) & ~7;
			header->NextOffset												= nextOffset;
			BlUUIDStringToBuffer(FvpLoginUnlockIdent, header->Ident);
			memset(header->Ident2, 0, sizeof(header->Ident2));
			memcpy(header + 1, FvpUserPassword, header->DataLength);
			header															= Add2Ptr(*keyStorePhysicalAddress, nextOffset, KEY_STORE_DATA_HEADER*);
		}

		if(FvpKeyEncryptingKeyIdent)
		{
			header->Type													= 1;
			header->SubType													= 2;
			header->DataLength												= static_cast<UINT32>(FvpVolumeKeyKEKLength);
			nextOffset														+= (header->DataLength + sizeof(KEY_STORE_DATA_HEADER) + 7) & ~7; 
			header->NextOffset												= nextOffset;
			BlUUIDStringToBuffer(FvpKeyEncryptingKeyIdent, header->Ident);
			memset(header->Ident2, 0, sizeof(header->Ident2));
			memcpy(header + 1, FvpVolumeKeyKEK, header->DataLength);
		}

		header->NextOffset													= 0;
	}

	return EFI_SUCCESS;
}
