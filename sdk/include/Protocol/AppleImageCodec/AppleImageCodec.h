//********************************************************************
//	created:	28:8:2012   20:54
//	filename: 	AppleImageCodec.h
//	author:		tiamo
//	purpose:	image code
//********************************************************************

#ifndef _APPLE_IMAGE_CODEC_H_
#define _APPLE_IMAGE_CODEC_H_

#define APPLE_IMAGE_CODEC_PROTOCOL_GUID										{ 0x0dfce9f6, 0xc4e3, 0x45ee, {0xa0, 0x6a, 0xa8, 0x61, 0x3b, 0x98, 0xa5, 0x07} }

typedef EFI_STATUS (EFIAPI* RECOGNIZE_IMAGE_DATA)(VOID* ImageBuffer, UINTN ImageSize);
typedef EFI_STATUS (EFIAPI* GET_IMAGE_DIMS)(VOID* ImageBuffer, UINTN ImageSize, UINTN* ImageWidth, UINTN* ImageHeight);
typedef EFI_STATUS (EFIAPI* DECODE_IMAGE_DATA)(VOID* ImageBuffer, UINTN ImageSize, EFI_UGA_PIXEL** RawImageData, UINTN* RawImageDataSize);

typedef struct _APPLE_IMAGE_CODEC_PROTOCOL
{
	UINT64																	Version;
	UINTN																	FileExt;
	RECOGNIZE_IMAGE_DATA													RecognizeImageData;
	GET_IMAGE_DIMS															GetImageDims;
	DECODE_IMAGE_DATA														DecodeImageData;
} APPLE_IMAGE_CODEC_PROTOCOL;

#endif
