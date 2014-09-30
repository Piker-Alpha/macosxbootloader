//********************************************************************
//	created:	4:11:2009   10:40
//	filename: 	Console.cpp
//	author:		tiamo
//	purpose:	Console
//********************************************************************

#include "stdafx.h"
#include "PictData.h"

//
// global
//
STATIC EFI_CONSOLE_CONTROL_SCREEN_MODE CspConsoleMode						= EfiConsoleControlScreenGraphics;
STATIC EFI_CONSOLE_CONTROL_SCREEN_MODE CspRestoreMode						= EfiConsoleControlScreenGraphics;
STATIC EFI_CONSOLE_CONTROL_PROTOCOL* CspConsoleControlProtocol				= nullptr;
STATIC EFI_GRAPHICS_OUTPUT_PROTOCOL* CspGraphicsOutputProtocol				= nullptr;
STATIC EFI_UGA_DRAW_PROTOCOL* CspUgaDrawProtocol							= nullptr;
STATIC APPLE_DEVICE_CONTROL_PROTOCOL* CspDeviceControlProtocol				= nullptr;
STATIC APPLE_GRAPH_CONFIG_PROTOCOL* CspGraphConfigProtocol					= nullptr;
STATIC UINT64 CspFrameBufferAddress											= 0;
STATIC UINT64 CspFrameBufferSize											= 0;
STATIC UINT32 CspBytesPerRow												= 0;
STATIC UINT32 CspHorzRes													= 0;
STATIC UINT32 CspVertRes													= 0;
STATIC UINT32 CspColorDepth													= 0;
STATIC BOOLEAN CspScreenNeedRedraw											= TRUE;
STATIC EFI_UGA_PIXEL CspBackgroundClear										= {0, 0, 0, 0};
STATIC UINTN CspIndicatorWidth												= 0;
STATIC UINTN CspIndicatorHeight												= 0;
STATIC UINTN CspIndicatorFrameCount											= 0;
STATIC UINTN CspIndicatorOffsetY											= 0;
STATIC UINTN CspIndicatorCurrentFrame										= 0;
STATIC EFI_UGA_PIXEL* CspIndicatorImage										= nullptr;
STATIC UINTN CspNetIndicatorCurrentFrame									= 0;
STATIC EFI_EVENT CspIndicatorRefreshTimerEvent								= nullptr;
STATIC BOOLEAN CspHiDPIMode													= FALSE;
STATIC INT32 CspGfxSavedConfigRestoreStatus									= -1;

//
// convert image
//
STATIC EFI_STATUS CspConvertImage(EFI_UGA_PIXEL** imageBuffer, UINT8 CONST* imageData, UINTN width, UINTN height, UINTN frameCount, UINT8 CONST* lookupTable)
{
	UINTN dataLength														= width * height * frameCount;
	UINTN imageLength														= dataLength * sizeof(EFI_UGA_PIXEL);
	EFI_UGA_PIXEL* theImage													= static_cast<EFI_UGA_PIXEL*>(MmAllocatePool(imageLength));
	if(!theImage)
		return EFI_OUT_OF_RESOURCES;

	*imageBuffer															= theImage;
	for(UINTN i = 0; i < dataLength; i ++, theImage ++, imageData ++)
	{
		UINTN index															= *imageData * 3;
		theImage->Red														= lookupTable[index + 0];
		theImage->Green														= lookupTable[index + 1];
		theImage->Blue														= lookupTable[index + 2];
	}
	return EFI_SUCCESS;
}

//
// scale image
//
STATIC EFI_STATUS CspScaleImage(EFI_UGA_PIXEL* inputData, UINTN inputWidth, UINTN inputHeight, EFI_UGA_PIXEL** outputData, UINTN* outputWidth, UINTN* outputHeight, INTN scaleRate)
{
	EFI_STATUS status														= EFI_SUCCESS;

	__try
	{
		//
		// compute dims
		//
		UINTN scaledWidth													= inputWidth * scaleRate / 1000;
		UINTN scaledHeight													= inputHeight * scaleRate / 1000;
		if(!scaledWidth || !scaledHeight || scaledWidth > CspHorzRes || scaledHeight > CspVertRes)
			try_leave(status = EFI_INVALID_PARAMETER);

		//
		// allocate buffer
		//
		EFI_UGA_PIXEL* scaledData											= static_cast<EFI_UGA_PIXEL*>(MmAllocatePool(scaledWidth * scaledHeight * sizeof(EFI_UGA_PIXEL)));
		if(!scaledData)
			try_leave(status = EFI_OUT_OF_RESOURCES);

		//
		// output
		//
		*outputWidth														= scaledWidth;
		*outputHeight														= scaledHeight;
		*outputData															= scaledData;

		//
		// scale it
		//
		for(UINTN y = 0; y < scaledHeight; y ++)
		{
			for(UINTN x = 0; x < scaledWidth; x ++, scaledData ++)
				*scaledData													= *(inputData + x * inputWidth / scaledWidth + y * inputHeight / scaledHeight * inputWidth);
		}
	}
	__finally
	{

	}

	return status;
}

//
// fill screen
//
STATIC EFI_STATUS CspFillRect(UINTN x, UINTN y, UINTN width, UINTN height, EFI_UGA_PIXEL pixelColor)
{
	CspScreenNeedRedraw														= TRUE;

	if(CspGraphicsOutputProtocol)
		return CspGraphicsOutputProtocol->Blt(CspGraphicsOutputProtocol, &pixelColor, EfiBltVideoFill, 0, 0, x, y, width, height, 0);

	if(CspUgaDrawProtocol)
		return CspUgaDrawProtocol->Blt(CspUgaDrawProtocol, &pixelColor, EfiUgaVideoFill, 0, 0, x, y, width, height, 0);

	return EFI_UNSUPPORTED;
}

//
// draw rect
//
STATIC EFI_STATUS CspDrawRect(UINTN x, UINTN y, UINTN width, UINTN height, EFI_UGA_PIXEL* pixelBuffer)
{
	CspScreenNeedRedraw														= TRUE;

	if(CspGraphicsOutputProtocol)
		return CspGraphicsOutputProtocol->Blt(CspGraphicsOutputProtocol, pixelBuffer, EfiBltBufferToVideo, 0, 0, x, y, width, height, 0);

	if(CspUgaDrawProtocol)
		return CspUgaDrawProtocol->Blt(CspUgaDrawProtocol, pixelBuffer, EfiUgaBltBufferToVideo, 0, 0, x, y, width, height, 0);

	return EFI_UNSUPPORTED;
}

//
// load logo file
//
EFI_STATUS CspLoadLogoFile(CHAR8 CONST* logoFileName, EFI_UGA_PIXEL** logoImage, UINTN* imageWidth, UINTN* imageHeight)
{
	EFI_STATUS status														= EFI_SUCCESS;
	CHAR8* fileBuffer														= nullptr;
	UINTN fileSize															= 0;
	EFI_HANDLE* handleArray													= nullptr;

	__try
	{
		//
		// read file
		//
		if(EFI_ERROR(status = IoReadWholeFile(nullptr, logoFileName, &fileBuffer, &fileSize, FALSE)))
			try_leave(NOTHING);

		//
		// get image decoders
		//
		UINTN totalHandles													= 0;
		if(EFI_ERROR(status = EfiBootServices->LocateHandleBuffer(ByProtocol, &AppleImageCodecProtocolGuid, nullptr, &totalHandles, &handleArray)))
			try_leave(NOTHING);

		//
		// search decoder
		//
		APPLE_IMAGE_CODEC_PROTOCOL* imageCodecProtocol						= nullptr;
		status																= EFI_NOT_FOUND;
		for(UINTN i = 0; i < totalHandles; i ++)
		{
			//
			// get codec protocol
			//
			EFI_HANDLE theHandle											= handleArray[i];
			if(!theHandle)
				continue;

			if(EFI_ERROR(status = EfiBootServices->HandleProtocol(theHandle, &AppleImageCodecProtocolGuid, reinterpret_cast<VOID**>(&imageCodecProtocol))))
				try_leave(NOTHING);

			//
			// can decode this file
			//
			if(!EFI_ERROR(status = imageCodecProtocol->RecognizeImageData(fileBuffer, fileSize)))
				break;
		}

		//
		// decoder not found
		//
		if(EFI_ERROR(status))
			try_leave(NOTHING);

		//
		// get image dims
		//
		if(EFI_ERROR(status = imageCodecProtocol->GetImageDims(fileBuffer, fileSize, imageWidth, imageHeight)))
			try_leave(NOTHING);

		//
		// decode buffer
		//
		UINTN imageDataSize													= 0;
		if(EFI_ERROR(status = imageCodecProtocol->DecodeImageData(fileBuffer, fileSize, logoImage, &imageDataSize)))
			try_leave(NOTHING);

		//
		// alpha blend with background
		//
		UINTN pixelCount													= imageDataSize / sizeof(EFI_UGA_PIXEL);
		EFI_UGA_PIXEL* imageData											= *logoImage;
		for(UINTN i = 0; i < pixelCount; i ++, imageData ++)
		{
			UINT8 alpha														= 255 - imageData->Reserved;
			if(alpha == 255)
				continue;

			UINT32 red														= (imageData->Red * alpha + imageData->Reserved * CspBackgroundClear.Red) / 255;
			imageData->Red													= static_cast<UINT8>(red > 255 ? 255 : red);

			UINT32 green													= (imageData->Green * alpha + imageData->Reserved * CspBackgroundClear.Green) / 255;
			imageData->Green												= static_cast<UINT8>(green > 255 ? 255 : green);

			UINT32 blue														= (imageData->Blue * alpha + imageData->Reserved * CspBackgroundClear.Blue) / 255;
			imageData->Blue													= static_cast<UINT8>(blue > 255 ? 255 : blue);
		}
	}
	__finally
	{
		if(fileBuffer)
			MmFreePool(fileBuffer);

		if(handleArray)
			EfiBootServices->FreePool(handleArray);
	}

	return status;
}

//
// convert logo file
//
STATIC EFI_STATUS CspConvertLogoImage(BOOLEAN normalLogo, EFI_UGA_PIXEL** logoImage, UINTN* imageWidth, UINTN* imageHeight)
{
	//
	// get logo file name
	//
	EFI_STATUS status														= EFI_SUCCESS;
	EFI_UGA_PIXEL* efiAllocatedData											= nullptr;
	UINT8* imageData														= nullptr;
	*logoImage																= nullptr;
	*imageWidth																= 0;
	*imageHeight															= 0;

	__try
	{
		CHAR8 CONST* logoFileName											= CmGetStringValueForKey(nullptr, normalLogo ? CHAR8_CONST_STRING("Boot Logo") : CHAR8_CONST_STRING("Boot Fail Logo"), nullptr);

		//
		// using alt image
		//
		if(logoFileName)
		{
			//
			// load file
			//
			if(!EFI_ERROR(CspLoadLogoFile(logoFileName, &efiAllocatedData, imageWidth, imageHeight)))
			{
				//
				// get scale rate
				//
				INT64 scaleRate												= 125;
				CmGetIntegerValueForKey(CHAR8_CONST_STRING("Boot Logo Scale"), &scaleRate);

				//
				// no scale
				//
				if(scaleRate <= 0 || CspVertRes >= 1080)
				{
					UINTN imageSize											= *imageWidth * *imageHeight * sizeof(EFI_UGA_PIXEL);
					*logoImage												= static_cast<EFI_UGA_PIXEL*>(MmAllocatePool(imageSize));
					if(*logoImage)
						memcpy(*logoImage, efiAllocatedData, imageSize);

					try_leave(status = *logoImage ? EFI_SUCCESS : EFI_OUT_OF_RESOURCES);
				}

				if(!EFI_ERROR(CspScaleImage(efiAllocatedData, *imageWidth, *imageHeight, logoImage, imageWidth, imageHeight, static_cast<INTN>(scaleRate))))
					try_leave(NOTHING);

				MmFreePool(*logoImage);
			}
		}

		//
		// then try the default one
		//
		typedef struct _builtin_image_info
		{
			UINTN															Width;
			UINTN															Height;
			UINTN															BufferSize;
			VOID*															Buffer;
			UINT8*															LookupTable;
		}builtin_image_info;

		//
		// 4 images(normal, normal@2x, failed, failed@2x)
		//
		STATIC builtin_image_info imageInfo[4] =
		{
			/* normal */		{ 84, 103, sizeof(CspNormalLogo), CspNormalLogo, CspNormalLogoLookupTable},
			/* normal@2x */		{168, 206, sizeof(CspNormalLogo2x), CspNormalLogo2x, CspNormalLogoLookupTable2x},
			/* failed */		{100, 100, sizeof(CspFailedLogo), CspFailedLogo, CspFailedLogoLookupTable},
			/* failed@2x */		{200, 200, sizeof(CspFailedLogo2x), CspFailedLogo2x, CspFailedLogoLookupTable2x},
		};

		//
		// decompress it
		//
		UINTN index															= (normalLogo ? 0 : 1) * 2  + (CspHiDPIMode ? 1 : 0);
		*imageWidth															= imageInfo[index].Width;
		*imageHeight														= imageInfo[index].Height;
		UINTN imageSize														= *imageWidth * *imageHeight;
		imageData															= static_cast<UINT8*>(MmAllocatePool(imageSize));
		if(!imageData)
			try_leave(status = EFI_OUT_OF_RESOURCES);

		if(EFI_ERROR(status = BlDecompress(imageInfo[index].Buffer, imageInfo[index].BufferSize, imageData, imageSize, &imageSize)))
			try_leave(NOTHING);

		//
		// convert it
		//
		if(EFI_ERROR(status = CspConvertImage(logoImage, imageData, *imageWidth, *imageHeight, 1, imageInfo[index].LookupTable)))
			try_leave(NOTHING);
	}
	__finally
	{
		if(imageData)
			MmFreePool(imageData);

		if(efiAllocatedData)
			EfiBootServices->FreePool(efiAllocatedData);

		if(EFI_ERROR(status))
		{
			*imageWidth														= 0;
			*imageHeight													= 0;
			*logoImage														= nullptr;
		}
	}

	return status;
}

//
// restore graph config
//
STATIC EFI_STATUS CspRestoreGraphConfig(UINT32 count, VOID* p1, VOID* p2, VOID* p3)
{
	if(ArchConvertPointerToAddress(CspGraphConfigProtocol) >= 2)
		return CspGraphConfigProtocol->RestoreConfig(CspGraphConfigProtocol, 2, count, p1, p2, p3);

	return EFI_SUCCESS;
}

//
// timer event
//
STATIC VOID EFIAPI CspIndicatorRefreshTimerEventNotifyRoutine(EFI_EVENT theEvent, VOID* theContext)
{
	if(!theContext)
	{
		EFI_UGA_PIXEL* curBuffer											= CspIndicatorImage + CspIndicatorHeight * CspIndicatorWidth * CspIndicatorCurrentFrame;
		CspIndicatorCurrentFrame											= (CspIndicatorCurrentFrame + 1) % CspIndicatorFrameCount;
		CspDrawRect((CspHorzRes - CspIndicatorWidth) / 2, (CspVertRes - CspIndicatorHeight) / 2 + CspIndicatorOffsetY, CspIndicatorWidth, CspIndicatorHeight, curBuffer);
	}
	else
	{
		CspFillRect((CspHorzRes - CspIndicatorWidth) / 2, (CspVertRes - CspIndicatorHeight) / 2 + CspIndicatorOffsetY, CspIndicatorWidth, CspIndicatorHeight, CspBackgroundClear);
	}
}

//
// decode and draw preview buffer
//
STATIC VOID CspDecodeAndDrawPreviewBufferColored(HIBERNATE_PREVIEW* previewBuffer, UINT32 imageIndex, UINT32 horzRes, UINT32 vertRes, UINT32 pixelPerRow, UINT64 frameBuffer, VOID* config)
{
	UINT32* row																= nullptr;

	__try
	{
		VOID* output														= ArchConvertAddressToPointer(frameBuffer, VOID*);
		UINT8* dataBuffer													= reinterpret_cast<UINT8*>(previewBuffer + 1) + previewBuffer->ImageCount * vertRes;
		row																	= static_cast<UINT32*>(MmAllocatePool(horzRes * sizeof(UINT32)));
		if(!row)
			try_leave(NOTHING);

		if(config)
		{
			INT32 delta														= 0;
			UINT16* configBuffer											= static_cast<UINT16*>(config);
			for(INT32 i = 0; i < 0x400; i ++, configBuffer ++)
			{
				INT32 index													= (i - delta) / 4;
				delta														= index / 85 + 1;
				configBuffer[i]												= (dataBuffer[index + 0x000] << 8) | dataBuffer[index + 0x000];
				configBuffer[i + 0x400]										= (dataBuffer[index + 0x100] << 8) | dataBuffer[index + 0x100];
				configBuffer[i + 0x800]										= (dataBuffer[index + 0x200] << 8) | dataBuffer[index + 0x200];
			}
		}

		for(UINT32 j = 0; j < vertRes; j ++)
		{
			UINT32* input													= reinterpret_cast<UINT32*>(previewBuffer + 1) + imageIndex * vertRes + j;
			input															= Add2Ptr(previewBuffer, *input, UINT32*);

			UINT32 count													= 0;
			UINT32 repeat													= 0;
			BOOLEAN fetch													= FALSE;
			UINT32 data														= 0;
			for(UINT32 i = 0; i < horzRes; i ++)
			{
				if(!count)
				{
					count													= *input++;
					repeat													= (count & 0xff000000);
					count													^= repeat;
					fetch													= TRUE;
				}
				else
				{
					fetch													= !repeat;
				}

				count														-= 1;

				if(fetch)
				{
					data													= *input++;
					if(dataBuffer)
						data												= (dataBuffer[(data  >> 16) & 0xff] << 16) | (dataBuffer[0x100 | ((data >> 8) & 0xff)] << 8) | dataBuffer[0x200 | (data & 0xff)];
				}

				row[i]														= data;
			}

			EfiBootServices->CopyMem(output, row, horzRes * sizeof(UINT32));
			output															= Add2Ptr(output, pixelPerRow * sizeof(UINT32), VOID*);
		}
	}
	__finally
	{
		if(row)
			MmFreePool(row);
	}
}

//
// decode and draw preview buffer
//
STATIC EFI_STATUS CspDecodeAndDrawPreviewBuffer(HIBERNATE_PREVIEW* previewBuffer, UINT32 imageIndex, UINT64 frameBuffer, UINT32 horzRes, UINT32 vertRes, UINT32 colorDepth, UINT32 bytesPerRow, BOOLEAN colorMode, VOID* config)
{
	EFI_STATUS status														= EFI_SUCCESS;
	UINT16* sc0																= nullptr;
	UINT16* sc1																= nullptr;
	UINT16* sc2																= nullptr;
	UINT16* sc3																= nullptr;
	UINT32* row																= nullptr;

	__try
	{
		//
		// check match
		//
		if(imageIndex >= previewBuffer->ImageCount || (colorDepth >> 3) != previewBuffer->Depth || horzRes != previewBuffer->Width || vertRes != previewBuffer->Height || colorDepth != 32)
			try_leave(status = EFI_INVALID_PARAMETER);

		//
		// color mode
		//
		if(colorMode)
			try_leave(CspDecodeAndDrawPreviewBufferColored(previewBuffer, imageIndex, horzRes, vertRes, bytesPerRow / sizeof(UINT32), frameBuffer, config));

		//
		// get info
		//
		VOID* output														= ArchConvertAddressToPointer(frameBuffer, VOID*);
		UINT32 pixelPerRow													= bytesPerRow / sizeof(UINT32);
		row																	= static_cast<UINT32*>(MmAllocatePool(horzRes * sizeof(UINT32)));
		sc0																	= static_cast<UINT16*>(MmAllocatePool((horzRes + 2) * sizeof(UINT16)));
		sc1																	= static_cast<UINT16*>(MmAllocatePool((horzRes + 2) * sizeof(UINT16)));
		sc2																	= static_cast<UINT16*>(MmAllocatePool((horzRes + 2) * sizeof(UINT16)));
		sc3																	= static_cast<UINT16*>(MmAllocatePool((horzRes + 2) * sizeof(UINT16)));
		if(!sc0 || !sc1 || !sc2 || !sc3 || !row)
			try_leave(status = EFI_OUT_OF_RESOURCES);

		memset(sc0, 0, (horzRes + 2) * sizeof(UINT16));
		memset(sc1, 0, (horzRes + 2) * sizeof(UINT16));
		memset(sc2, 0, (horzRes + 2) * sizeof(UINT16));
		memset(sc3, 0, (horzRes + 2) * sizeof(UINT16));

		for(UINT32 j = 0; j < vertRes + 2; j ++)
		{
			UINT32* input													= reinterpret_cast<UINT32*>(previewBuffer + 1) + imageIndex * vertRes;
			if(j < vertRes)
				input														+= j;
			else
				input														+= vertRes - 1;

			input															= Add2Ptr(previewBuffer, *input, UINT32*);

			UINT32 count													= 0;
			UINT32 repeat													= 0;
			UINT32 sr0														= 0;
			UINT32 sr1														= 0;
			UINT32 sr2														= 0;
			UINT32 sr3														= 0;
			BOOLEAN fetch													= FALSE;
			UINT32 data														= 0;
			for(UINT32 i = 0; i < horzRes + 2; i ++)
			{
				if(i < horzRes)
				{
					if(!count)
					{
						count												= *input++;
						repeat												= (count & 0xff000000);
						count												^= repeat;
						fetch												= TRUE;
					}
					else
					{
						fetch												= !repeat;
					}

					count													-= 1;

					if(fetch)
					{
						data												= *input++;
						data												= (((13933 * (0xff & (data >> 24)) + 46871 * (0xff & (data >> 16)) + 4732 * (0xff & data)) >> 16) * 19661 + (103 << 16)) >> 16;
					}
				}

				UINT32 tmp2													= sr0 + data;
				sr0															= data;
				UINT32 tmp1													= sr1 + tmp2;
				sr1															= tmp2;
				tmp2														= sr2 + tmp1;
				sr2															= tmp1;
				tmp1														= sr3 + tmp2;
				sr3															= tmp2;

				tmp2														= sc0[i] + tmp1;
				sc0[i]														= static_cast<UINT16>(tmp1);
				tmp1														= sc1[i] + tmp2;
				sc1[i]														= static_cast<UINT16>(tmp2);
				tmp2														= sc2[i] + tmp1;
				sc2[i]														= static_cast<UINT16>(tmp1);
				UINT32 out													= ((128 + sc3[i] + tmp2) >> 8) & 0xff;
				sc3[i]														= static_cast<UINT16>(tmp2);

				if(i > 1 && j > 1)
					row[i - 2]												= out | (out << 8) | (out << 16);
			}

			if(j > 1)
			{
				EfiBootServices->CopyMem(output, row, horzRes * sizeof(UINT32));
				output														= Add2Ptr(output, pixelPerRow * sizeof(UINT32), VOID*);
			}
		}
	}
	__finally
	{
		if(sc0)
			MmFreePool(sc0);

		if(sc1)
			MmFreePool(sc1);

		if(sc2)
			MmFreePool(sc2);

		if(sc3)
			MmFreePool(sc3);

		if(row)
			MmFreePool(row);
	}

	return status;
}

//
// initialize
//
EFI_STATUS CsInitialize()
{
	//
	// get background clear
	//
	UINTN dataSize															= sizeof(CspBackgroundClear);
	if(!EFI_ERROR(EfiRuntimeServices->GetVariable(CHAR16_STRING(L"BackgroundClear"), &AppleFirmwareVariableGuid, nullptr, &dataSize, &CspBackgroundClear)))
		CspScreenNeedRedraw													= FALSE;
	else
		CspScreenNeedRedraw													= TRUE;

	//
	// get console control protocol
	//
	EFI_STATUS status														= EFI_SUCCESS;
	if(EFI_ERROR(status = EfiBootServices->LocateProtocol(&EfiConsoleControlProtocolGuid, nullptr, reinterpret_cast<VOID**>(&CspConsoleControlProtocol))))
		return status;

	//
	// get console mode
	//
	if(EFI_ERROR(status = CspConsoleControlProtocol->GetMode(CspConsoleControlProtocol, &CspConsoleMode, nullptr, nullptr)))
		return status;

	//
	// read UIScale
	//
	UINT8 uiScale															= 0;
	dataSize																= sizeof(uiScale);
	if(!EFI_ERROR(EfiRuntimeServices->GetVariable(CHAR16_STRING(L"UIScale"), &AppleFirmwareVariableGuid, nullptr, &dataSize, &uiScale)))
	{
		UINT16 actualDensity												= 0;
		dataSize															= sizeof(actualDensity);
		EfiRuntimeServices->GetVariable(CHAR16_STRING(L"ActualDensity"), &AppleFirmwareVariableGuid, nullptr, &dataSize, &actualDensity);

		UINT16 densityThreshold												= 0;
		dataSize															= sizeof(densityThreshold);
		EfiRuntimeServices->GetVariable(CHAR16_STRING(L"DensityThreshold"), &AppleFirmwareVariableGuid, nullptr, &dataSize, &densityThreshold);

		CspHiDPIMode														= uiScale >= 2;
	}

	//
	// save old mode
	//
	CspRestoreMode															= CspConsoleMode;
	return EFI_SUCCESS;
}

//
// set text mode
//
EFI_STATUS CsSetConsoleMode(BOOLEAN textMode, BOOLEAN force)
{
	EFI_CONSOLE_CONTROL_SCREEN_MODE screenMode								= textMode ? EfiConsoleControlScreenText : EfiConsoleControlScreenGraphics;
	if(!force && screenMode == CspConsoleMode)
		return EFI_SUCCESS;

	EFI_STATUS status														= CspConsoleControlProtocol->SetMode(CspConsoleControlProtocol, screenMode);
	if(EFI_ERROR(status))
		return status;

	CspConsoleMode															= screenMode;
	CspScreenNeedRedraw														= TRUE;
	return EFI_SUCCESS;
}

//
// setup graph mode
//
EFI_STATUS CsInitializeGraphMode()
{
	EFI_STATUS status;
	if(EFI_ERROR(status = CsSetConsoleMode(FALSE, FALSE)))
		return status;

	if(EFI_ERROR(status = EfiBootServices->HandleProtocol(EfiSystemTable->ConsoleOutHandle, &EfiGraphicsOutputProtocolGuid, reinterpret_cast<VOID**>(&CspGraphicsOutputProtocol))))
	{
		if(EFI_ERROR(status = EfiBootServices->HandleProtocol(EfiSystemTable->ConsoleOutHandle, &EfiUgaDrawProtocolGuid, reinterpret_cast<VOID**>(&CspUgaDrawProtocol))))
			return status;
	}

	BlSetBootMode(BOOT_MODE_GRAPH, 0);
	INT64 clearColor														= 0xffbfbfbf;
	CmGetIntegerValueForKey(CHAR8_CONST_STRING("Background Color"), &clearColor);

	EFI_UGA_PIXEL clearPixel;
	clearPixel.Red															= static_cast<UINT8>((clearColor >>  0) & 0xff);
	clearPixel.Green														= static_cast<UINT8>((clearColor >>  8) & 0xff);
	clearPixel.Blue															= static_cast<UINT8>((clearColor >> 16) & 0xff);

	if(clearPixel.Red != CspBackgroundClear.Red || clearPixel.Green != CspBackgroundClear.Green || clearPixel.Blue != CspBackgroundClear.Blue)
		CspScreenNeedRedraw													= TRUE;

	CspBackgroundClear														= clearPixel;
	CsInitializeBootVideo(nullptr);

	return EFI_SUCCESS;
}

//
// setup boot video
//
EFI_STATUS CsInitializeBootVideo(BOOT_VIDEO* bootVideo)
{
	//
	// get info
	//
	if(!CspFrameBufferAddress)
	{
		//
		// check graphics output protocol
		//
		EFI_GRAPHICS_OUTPUT_PROTOCOL* graphicsOutputProtocol				= nullptr;
		if(!EFI_ERROR(EfiBootServices->HandleProtocol(EfiSystemTable->ConsoleOutHandle, &EfiGraphicsOutputProtocolGuid, reinterpret_cast<VOID**>(&graphicsOutputProtocol))))
		{
			CspFrameBufferAddress											= graphicsOutputProtocol->Mode->FrameBufferBase;
			if(CspFrameBufferAddress)
			{
				CspColorDepth												= 0;
				CspFrameBufferSize											= graphicsOutputProtocol->Mode->FrameBufferSize;
				CspHorzRes													= graphicsOutputProtocol->Mode->Info->HorizontalResolution;
				CspVertRes													= graphicsOutputProtocol->Mode->Info->VerticalResolution;

				if(graphicsOutputProtocol->Mode->Info->PixelFormat == PixelBitMask)
				{
					UINT32 colorMask										= graphicsOutputProtocol->Mode->Info->PixelInformation.BlueMask;
					colorMask												|= graphicsOutputProtocol->Mode->Info->PixelInformation.GreenMask;
					colorMask												|= graphicsOutputProtocol->Mode->Info->PixelInformation.RedMask;

					for(UINTN i = 0; i < 32; i ++, colorMask >>= 1)
					{
						if(colorMask & 1)
							CspColorDepth									+= 1;
					}

					CspColorDepth											= (CspColorDepth + 7) & ~7;
				}
				else if(graphicsOutputProtocol->Mode->Info->PixelFormat != PixelBltOnly)
				{
					CspColorDepth											= 32;
				}

				if(!CspColorDepth)
				{
					CspFrameBufferSize										= 0;
					CspFrameBufferAddress									= 0;
					CspHorzRes												= 0;
					CspVertRes												= 0;
				}
				else
				{
					CspBytesPerRow											= graphicsOutputProtocol->Mode->Info->PixelsPerScanLine * (CspColorDepth >> 3);
				}
			}
		}

		//
		// check device protocol?
		//
		if(!CspFrameBufferAddress)
		{
			APPLE_GRAPH_INFO_PROTOCOL* graphInfoProtocol					= nullptr;
			if(!EFI_ERROR(EfiBootServices->LocateProtocol(&AppleGraphInfoProtocolGuid, 0, reinterpret_cast<VOID**>(&graphInfoProtocol))))
			{
				if(EFI_ERROR(graphInfoProtocol->GetInfo(graphInfoProtocol, &CspFrameBufferAddress, &CspFrameBufferSize, &CspBytesPerRow, &CspHorzRes, &CspVertRes, &CspColorDepth)))
					CspFrameBufferAddress									= 0;
			}
		}
	}

	if(bootVideo)
	{
		bootVideo->BaseAddress												= static_cast<UINT32>(CspFrameBufferAddress);
		bootVideo->BytesPerRow												= CspBytesPerRow;
		bootVideo->ColorDepth												= CspColorDepth;
		bootVideo->HorzRes													= CspHorzRes;
		bootVideo->VertRes													= CspVertRes;
	}

	return CspFrameBufferAddress ? EFI_SUCCESS : EFI_DEVICE_ERROR;
}

//
// print string
//
VOID CsPrintf(CHAR8 CONST* printForamt, ...)
{
	if(EfiSystemTable && EfiSystemTable->ConOut && EfiSystemTable->ConsoleOutHandle)
	{
		STATIC CHAR8 utf8Buffer[1024]										= {0};
		STATIC CHAR16 unicodeBuffer[1024]									= {0};
		VA_LIST list;
		VA_START(list, printForamt);
		vsnprintf(utf8Buffer, ARRAYSIZE(utf8Buffer) - 1, printForamt, list);
		VA_END(list);

		CHAR16 tempBuffer[2]												= {0};
		BlUtf8ToUnicode(utf8Buffer, strlen(utf8Buffer), unicodeBuffer, ARRAYSIZE(unicodeBuffer) - 1);
		for(UINTN i = 0; i < ARRAYSIZE(unicodeBuffer) && unicodeBuffer[i]; i ++)
		{
			if(unicodeBuffer[i] == L'\n')
			{
				EfiSystemTable->ConOut->OutputString(EfiSystemTable->ConOut, CHAR16_STRING(L"\r\n"));
			}
			else if(unicodeBuffer[i] == L'\t')
			{
				EfiSystemTable->ConOut->OutputString(EfiSystemTable->ConOut, CHAR16_STRING(L"    "));
			}
			else
			{
				tempBuffer[0]												= unicodeBuffer[i];
				EfiSystemTable->ConOut->OutputString(EfiSystemTable->ConOut, tempBuffer);
			}
		}
	}
}

//
// connect device
//
EFI_STATUS CsConnectDevice(BOOLEAN connectAll, BOOLEAN connectDisplay)
{
	EFI_STATUS status														= EFI_SUCCESS;
	if(!CspDeviceControlProtocol)
	{
		if(EFI_ERROR(EfiBootServices->LocateProtocol(&AppleDeviceControlProtocolGuid, nullptr, reinterpret_cast<VOID**>(&CspDeviceControlProtocol))))
			CspDeviceControlProtocol										= ArchConvertAddressToPointer(1, APPLE_DEVICE_CONTROL_PROTOCOL*);
	}

	if(ArchConvertPointerToAddress(CspDeviceControlProtocol) >= 2)
	{
		if(connectAll)
			status															= CspDeviceControlProtocol->ConnectAll();
		if(connectDisplay)
			status															= CspDeviceControlProtocol->ConnectDisplay();
	}

	if(!CspGraphConfigProtocol)
	{
		if(EFI_ERROR(EfiBootServices->LocateProtocol(&AppleGraphConfigProtocolGuid, nullptr, reinterpret_cast<VOID**>(&CspGraphConfigProtocol))))
			CspGraphConfigProtocol											= ArchConvertAddressToPointer(1, APPLE_GRAPH_CONFIG_PROTOCOL*);
	}

	if(connectAll && connectDisplay)
		CspRestoreGraphConfig(0, nullptr, nullptr, nullptr);

	UINT32 result[sizeof(INTN) / sizeof(UINT32)]							= {0};
	UINTN dataSize															= sizeof(result);
	if(!EFI_ERROR(EfiRuntimeServices->GetVariable(CHAR16_STRING(L"gfx-saved-config-restore-status"), &AppleFirmwareVariableGuid, nullptr, &dataSize, result)))
		CspGfxSavedConfigRestoreStatus										= static_cast<INT32>(result[0] | (result[ARRAYSIZE(result) - 1] & 0x80000000));

	return status;
}

//
// draw boot image
//
EFI_STATUS CsDrawBootImage(BOOLEAN normalLogo)
{
	EFI_STATUS status														= EFI_SUCCESS;
	UINT8* dataBuffer														= nullptr;

	__try
	{
		//
		// check graph mode
		//
		if(!CspFrameBufferAddress || CspConsoleMode != EfiConsoleControlScreenGraphics)
			try_leave(NOTHING);

		//
		// convert logo image
		//
		EFI_UGA_PIXEL* logoImage											= nullptr;
		UINTN imageWidth													= 0;
		UINTN imageHeight													= 0;
		if(EFI_ERROR(status = CspConvertLogoImage(normalLogo, &logoImage, &imageWidth, &imageHeight)))
			try_leave(NOTHING);

		//
		// draw it
		//
		CsClearScreen();
		status																= CspDrawRect((CspHorzRes - imageWidth) / 2, (CspVertRes - imageHeight) / 2, imageWidth, imageHeight, logoImage);
		MmFreePool(logoImage);

		//
		// show indicator only in netboot mode
		//
		if(EFI_ERROR(status) || !normalLogo || !BlTestBootMode(BOOT_MODE_NET))
			try_leave(NOTHING);

		//
		// setup indicator info
		//
		CspIndicatorWidth													= CspHiDPIMode ? 64 : 32;
		CspIndicatorHeight													= CspIndicatorWidth;
		CspIndicatorOffsetY													= CspHiDPIMode ? 400 : 200;
		CspIndicatorFrameCount												= 18;
		UINTN imageSize														= CspIndicatorWidth * CspIndicatorHeight * CspIndicatorFrameCount;
		dataBuffer															= static_cast<UINT8*>(MmAllocatePool(imageSize));
		if(!dataBuffer)
			try_leave(status = EFI_OUT_OF_RESOURCES);

		//
		// decompress data
		//
		if(EFI_ERROR(status = BlDecompress(CspHiDPIMode ? CspIndicator2x : CspIndicator, CspHiDPIMode ? sizeof(CspIndicator2x) : sizeof(CspIndicator), dataBuffer, imageSize, &imageSize)))
			try_leave(NOTHING);

		//
		// convert image
		//
		if(EFI_ERROR(status = CspConvertImage(&CspIndicatorImage, dataBuffer, CspIndicatorWidth, CspIndicatorHeight, CspIndicatorFrameCount, CspHiDPIMode ? CspIndicatorLookupTable2x : CspIndicatorLookupTable)))
			try_leave(NOTHING);

		//
		// draw the 1st frame
		//
		CspIndicatorRefreshTimerEventNotifyRoutine(CspIndicatorRefreshTimerEvent, nullptr);

		//
		// create timer event
		//
		if(EFI_ERROR(status = EfiBootServices->CreateEvent(EFI_EVENT_TIMER | EFI_EVENT_NOTIFY_SIGNAL, EFI_TPL_NOTIFY, &CspIndicatorRefreshTimerEventNotifyRoutine, nullptr, &CspIndicatorRefreshTimerEvent)))
			try_leave(NOTHING);

		//
		// setup 100ms timer
		//
		status																= EfiBootServices->SetTimer(CspIndicatorRefreshTimerEvent, TimerPeriodic, 1000000);
	}
	__finally
	{
		if(dataBuffer)
			MmFreePool(dataBuffer);
	}

	return status;
}

//
// draw panic image
//
EFI_STATUS CsDrawPanicImage()
{
	EFI_STATUS status														= EFI_SUCCESS;
	UINT8* imageData														= nullptr;
	EFI_UGA_PIXEL* panicImage												= nullptr;

	__try
	{
		//
		// check graph mode
		//
		if(!CspFrameBufferAddress || CspConsoleMode != EfiConsoleControlScreenGraphics)
			try_leave(NOTHING);
		
		//
		// decompress data
		//
		UINTN imageWidth													= CspHiDPIMode ? 920 : 460;;
		UINTN imageHeight													= CspHiDPIMode ? 570 : 285;
		UINTN imageSize														= imageWidth * imageHeight;
		if(EFI_ERROR(status = BlDecompress(CspHiDPIMode ? CspPanicDialog2x : CspPanicDialog, CspHiDPIMode ? sizeof(CspPanicDialog2x) : sizeof(CspPanicDialog), imageData, imageSize, &imageSize)))
			try_leave(NOTHING);

		//
		// convert it
		//
		if(EFI_ERROR(status = CspConvertImage(&panicImage, imageData, imageWidth, imageHeight, 1, CspHiDPIMode ? CspPanicDialogLookupTable2x : CspPanicDialogLookupTable)))
			try_leave(NOTHING);

		//
		// draw it
		//
		CsClearScreen();
		status																= CspDrawRect((CspHorzRes - imageWidth) / 2, (CspVertRes - imageHeight) / 2, imageWidth, imageHeight, panicImage);
	}
	__finally
	{
		if(imageData)
			MmFreePool(imageData);

		if(panicImage)
			MmFreePool(panicImage);
	}

	return status;
}

//
// setup device tree
//
EFI_STATUS CsSetupDeviceTree(BOOT_ARGS* bootArgs)
{
	EFI_STATUS status														= EFI_SUCCESS;

	__try
	{
		//
		// boot video
		//
		bootArgs->BootVideo.BaseAddress										= 0;
		CsInitializeBootVideo(&bootArgs->BootVideo);
		if(!bootArgs->BootVideo.BaseAddress)
			memset(&bootArgs->BootVideo, 0, sizeof(bootArgs->BootVideo));

		//
		// setup display mode
		//
		bootArgs->BootVideo.DisplayMode										= BlTestBootMode(BOOT_MODE_GRAPH) ? 1 : 2;

		//
		// hi-dpi mode
		//
		if(CspHiDPIMode)
			bootArgs->Flags													|= 2;	// kBootArgsFlagHiDPI

		//
		// allocate FailedCLUT
		//
		UINTN bufferLength													= CspHiDPIMode ? sizeof(CspFailedLogoLookupTable2x) : sizeof(CspFailedLogoLookupTable);
		UINTN allocatedLength												= bufferLength;
		UINT64 physicalAddress												= MmAllocateKernelMemory(&allocatedLength, nullptr);
		if(!physicalAddress)
			try_leave(status = EFI_OUT_OF_RESOURCES);

		//
		// add memory node
		//
		BlAddMemoryRangeNode(CHAR8_CONST_STRING("FailedCLUT"), physicalAddress, bufferLength);

		//
		// copy FailedCLUT
		//
		memcpy(ArchConvertAddressToPointer(physicalAddress, VOID*), CspHiDPIMode ? CspFailedLogoLookupTable2x : CspFailedLogoLookupTable, bufferLength);

		//
		// boot progress info
		//
		typedef struct _BOOT_PROGRESS_ELEMENT
		{
			//
			// width
			//
			UINT32															Width;

			//
			// heigth
			//
			UINT32															Height;

			//
			// y offset
			//
			UINT32															OffsetY;

			//
			// reserved
			//
			UINT32															Reserved[5];
		}BOOT_PROGRESS_ELEMENT;

		//
		// allocate FailedImage
		//
		bufferLength														= (CspHiDPIMode ? sizeof(CspFailedLogo2x) : sizeof(CspFailedLogo)) + sizeof(BOOT_PROGRESS_ELEMENT);
		allocatedLength														= bufferLength;
		physicalAddress														= MmAllocateKernelMemory(&allocatedLength, 0);
		if(!physicalAddress)
			try_leave(status = EFI_OUT_OF_RESOURCES);

		//
		// add memory node
		//
		BlAddMemoryRangeNode(CHAR8_CONST_STRING("FailedImage"), physicalAddress, bufferLength);

		//
		// setup info and copy FailedImage
		//
		BOOT_PROGRESS_ELEMENT* element										= ArchConvertAddressToPointer(physicalAddress, BOOT_PROGRESS_ELEMENT*);
		element->Height														= CspHiDPIMode ? 200 : 100;
		element->Width														= element->Height;
		element->OffsetY													= 0;
		memcpy(element + 1, CspHiDPIMode ? CspFailedLogo2x : CspFailedLogo, bufferLength - sizeof(BOOT_PROGRESS_ELEMENT));
	}
	__finally
	{
	}

	return status;
}

//
// clear screen
//
VOID CsClearScreen()
{
	CspScreenNeedRedraw														= FALSE;
	CspFillRect(0, 0, CspHorzRes, CspVertRes, CspBackgroundClear);
}

//
// finalize()
//
EFI_STATUS CsFinalize()
{
	if(!CspIndicatorRefreshTimerEvent)
		return EFI_SUCCESS;

	EfiBootServices->CloseEvent(CspIndicatorRefreshTimerEvent);
	CspIndicatorRefreshTimerEvent											= nullptr;
	CspIndicatorRefreshTimerEventNotifyRoutine(CspIndicatorRefreshTimerEvent, &CspIndicatorRefreshTimerEvent);

	return EFI_SUCCESS;
}

//
// draw preview
//
VOID CsDrawPreview(HIBERNATE_PREVIEW* previewBuffer, UINT32 imageIndex, UINT8 progressSaveUnder[HIBERNATE_PROGRESS_COUNT][HIBERNATE_PROGRESS_SAVE_UNDER_SIZE], BOOLEAN colorMode, BOOLEAN fromFV2, INT32* gfxRestoreStatus)
{
	VOID* configBuffer														= nullptr;

	__try
	{
		//
		// setup graph
		//
		if(!fromFV2 && (EFI_ERROR(CsConnectDevice(FALSE, TRUE)) || EFI_ERROR(CsInitializeGraphMode())))
			try_leave(NOTHING);

		//
		// get graph info
		//
		if(EFI_ERROR(CsInitializeBootVideo(nullptr)) || !CspFrameBufferAddress)
			try_leave(NOTHING);

		//
		// save gfx restore status
		//
		if(gfxRestoreStatus)
			*gfxRestoreStatus												= CspGfxSavedConfigRestoreStatus;

		//
		// allocate config buffer
		//
		if(ArchConvertPointerToAddress(CspGraphConfigProtocol) >= 2)
			configBuffer													= MmAllocatePool(0x1800);

		//
		// error gfx status
		//
		if(CspGfxSavedConfigRestoreStatus < -1)
			previewBuffer													= nullptr;

		//
		// show preview buffer or boot image
		//
		if(previewBuffer && !EFI_ERROR(CspDecodeAndDrawPreviewBuffer(previewBuffer, imageIndex, CspFrameBufferAddress, CspHorzRes, CspVertRes, CspColorDepth, CspBytesPerRow, colorMode, configBuffer)))
		{
			if(ArchConvertPointerToAddress(CspGraphConfigProtocol) >= 2)
			{
				if(colorMode)
					CspRestoreGraphConfig(0x400, configBuffer, Add2Ptr(configBuffer, 0x800, VOID*), Add2Ptr(configBuffer, 0x1000, VOID*));
				else
					CspRestoreGraphConfig(0, nullptr, nullptr, nullptr);
				
			}
		}
		else if(!colorMode)
		{
			CsDrawBootImage(TRUE);
			CspRestoreGraphConfig(0, nullptr, nullptr, nullptr);
		}

		//
		// calc pixel shift (16 = 1, 32 = 2)
		//
		UINT32 pixelShift													= CspColorDepth >> 4;
		if(!progressSaveUnder || pixelShift < 1)
			try_leave(NOTHING);

		//
		// calc screen buffer
		//
		UINT8* screenBuffer													= ArchConvertAddressToPointer(CspFrameBufferAddress, UINT8*);
		screenBuffer														+= (CspHorzRes - HIBERNATE_PROGRESS_COUNT * (HIBERNATE_PROGRESS_WIDTH + HIBERNATE_PROGRESS_SPACING)) << (pixelShift - 1);
		screenBuffer														+= (CspVertRes - HIBERNATE_PROGRESS_ORIGINY - HIBERNATE_PROGRESS_HEIGHT) * CspBytesPerRow;

		UINTN index[HIBERNATE_PROGRESS_COUNT]								= {0};
		for(UINTN y = 0; y < HIBERNATE_PROGRESS_HEIGHT; y ++)
		{
			VOID* outputBuffer												= screenBuffer + y * CspBytesPerRow;
			for(UINTN blob = 0; blob < HIBERNATE_PROGRESS_COUNT; blob ++)
			{
				UINT32 color												= blob ? HIBERNATE_PROGRESS_DARK_GRAY : HIBERNATE_PROGRESS_MID_GRAY;
				for(UINTN x = 0; x < HIBERNATE_PROGRESS_WIDTH; x ++)
				{
					UINT8 alpha												= HbpProgressAlpha[y][x];
					UINT32 result											= color;
					if(alpha)
					{
						if(0xff != alpha)
						{
							UINT8 dstColor									= *static_cast<UINT8*>(outputBuffer);
							if(pixelShift == 1)
								dstColor									= ((dstColor & 0x1f) << 3) | ((dstColor & 0x1f) >> 2);

							progressSaveUnder[blob][index[blob]]			= dstColor;
							index[blob]										+= 1;
							result											= ((255 - alpha) * dstColor + alpha * result) / 255;
						}

						if(pixelShift == 1)
						{
							result											>>= 3;
							*static_cast<UINT16*>(outputBuffer)				= static_cast<UINT16>((result << 10) | (result << 5) | result);
						}
						else
						{
							*static_cast<UINT32*>(outputBuffer)				= (result << 16) | (result << 8) | result;
						}
					}

					outputBuffer											= Add2Ptr(outputBuffer, static_cast<UINT32>(1 << pixelShift), VOID*);
				}

				outputBuffer												= Add2Ptr(outputBuffer, HIBERNATE_PROGRESS_SPACING << pixelShift, VOID*);
			}
		}
	}
	__finally
	{
		if(configBuffer)
			MmFreePool(configBuffer);
	}
}

//
// update progress
//
VOID CsUpdateProgress(UINT8 progressSaveUnder[HIBERNATE_PROGRESS_COUNT][HIBERNATE_PROGRESS_SAVE_UNDER_SIZE], UINTN prevBlob, UINTN currentBlob)
{
	//
	// calc pixel shift (16 = 1, 32 = 2)
	//
	UINT32 pixelShift														= CspColorDepth >> 4;
	if(pixelShift < 1 || !CspFrameBufferAddress)
		return;

	//
	// calc screen buffer
	//
	UINT8* screenBuffer														= ArchConvertAddressToPointer(CspFrameBufferAddress, UINT8*);
	screenBuffer															+= (CspHorzRes - HIBERNATE_PROGRESS_COUNT * (HIBERNATE_PROGRESS_WIDTH + HIBERNATE_PROGRESS_SPACING)) << (pixelShift - 1);
	screenBuffer															+= (CspVertRes - HIBERNATE_PROGRESS_ORIGINY - HIBERNATE_PROGRESS_HEIGHT) * CspBytesPerRow;

	UINTN lastBlob															= currentBlob < HIBERNATE_PROGRESS_COUNT ? currentBlob : HIBERNATE_PROGRESS_COUNT - 1;
	screenBuffer															+= (prevBlob * (HIBERNATE_PROGRESS_WIDTH + HIBERNATE_PROGRESS_SPACING)) << pixelShift;
	UINTN index[HIBERNATE_PROGRESS_COUNT]									= {0};
	for(UINTN y = 0; y < HIBERNATE_PROGRESS_HEIGHT; y ++)
	{
		VOID* outputBuffer													= screenBuffer + y * CspBytesPerRow;
		for(UINTN blob = prevBlob; blob <= lastBlob; blob ++)
		{
			UINT32 color													= blob < currentBlob ? HIBERNATE_PROGRESS_LIGHT_GRAY : HIBERNATE_PROGRESS_MID_GRAY;
			for(UINTN x = 0; x < HIBERNATE_PROGRESS_WIDTH; x ++)
			{
				UINT8 alpha													= HbpProgressAlpha[y][x];
				UINT32 result												= color;
				if(alpha)
				{
					if(0xff != alpha)
					{
						UINT8 dstColor										= progressSaveUnder[blob][index[blob]];
						index[blob]											+= 1;
						result												= ((255 - alpha) * dstColor + alpha * result) / 255;
					}

					if(pixelShift == 1)
					{
						result												>>= 3;
						*static_cast<UINT16*>(outputBuffer)					= static_cast<UINT16>((result << 10) | (result << 5) | result);
					}
					else
					{
						*static_cast<UINT32*>(outputBuffer)					= (result << 16) | (result << 8) | result;
					}
				}

				outputBuffer												= Add2Ptr(outputBuffer, static_cast<UINT32>(1 << pixelShift), VOID*);
			}

			outputBuffer													= Add2Ptr(outputBuffer, HIBERNATE_PROGRESS_SPACING << pixelShift, VOID*);
		}
	}
}