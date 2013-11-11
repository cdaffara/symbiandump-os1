// Copyright (c) 1997-20010 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#include "BMDRAW.H"
#include "BitDrawInterfaceId.h"
#include <graphics/lookuptable.h>
#include <graphics/blendingalgorithms.h>

#if defined(SYMBIAN_USE_FAST_FADING)
// 16bpp fast fade - half the contrast and brighten
const TInt K16bppFastFadeShift = 1;
const TUint16 K16bppFastFadeMask = 0x8410;
// Use the 32 -> 16 bit colour convesrion method to get
// the 16 bit fading constant (K16bppFastFadeOffset)
// from 32 bit fading constant (SYMBIAN_USE_FAST_FADING).
const TUint16 K16bppFastFadeOffset = ((SYMBIAN_USE_FAST_FADING & 0x0000f8) >> 3) |
									((SYMBIAN_USE_FAST_FADING & 0x00fc00) >> 5) |
									((SYMBIAN_USE_FAST_FADING & 0xf80000) >> 8);
#endif

// CDrawSixteenBppBitmapCommon

//Initializes iSize, iDrawRect, iLongWidth, iScanlineWords data members.
//It should be called every time when iSize is going to be changed - from Construct().
//@param aSize Physical screen size in pixels.
//@panic EScreenDriverPanicInvalidSize - Invalid aSize parameter. This might happen if the 
//device is scaled and the scaling origin goes outside physical drawing rectangle.
void CDrawSixteenBppBitmapCommon::SetSize(const TSize& aSize) 
	{
	CDrawBitmap::SetSize(aSize);
	__ASSERT_DEBUG(iSize == aSize, User::Invariant());
	iLongWidth = (iSize.iWidth + 1) & ~1;
	iScanLineWords = iLongWidth >> 1;
	}
 
TInt CDrawSixteenBppBitmapCommon::Construct(TSize aSize, TInt aStride)
	{
	iBits = NULL;
	CDrawBitmap::SetSize(aSize);
	__ASSERT_DEBUG(iSize == aSize, User::Invariant());
	if (aStride & 3)
		return KErrArgument;
	iLongWidth = aStride >> 1;
	if (iLongWidth < aSize.iWidth)
		return KErrArgument;
	iScanLineWords = aStride >> 2;
	TInt size = Max(aSize.iWidth,aSize.iHeight) << 1;
	if(size < 0)
		return KErrArgument;
	iScanLineBuffer = (TUint32*)(User::Heap().Alloc(size));
	if (iScanLineBuffer == NULL)
		return KErrNoMemory;
	return KErrNone;
	}

TUint16* CDrawSixteenBppBitmapCommon::PixelAddress(TInt aX,TInt aY) const
	{
	return(((TUint16*)iBits) + (aY * iLongWidth) + aX);
	}

void CDrawSixteenBppBitmapCommon::InvertBuffer(TInt aLength,TUint32* aBuffer)
	{
	__ASSERT_DEBUG(aLength>0,Panic(EScreenDriverPanicZeroLength));
	__ASSERT_DEBUG(aBuffer,Panic(EScreenDriverPanicNullPointer));

	const TUint32* limit = aBuffer + ((aLength + 1) >> 1);

	while (aBuffer < limit)
		*aBuffer++ ^= 0xffffffff;
	}

void CDrawSixteenBppBitmapCommon::ShadowArea(const TRect& aRect)
	{
	const TRect rect(DeOrientate(aRect));

	__ASSERT_DEBUG(rect.iTl.iX>=0 && rect.iBr.iX<=iSize.iWidth,Panic(EScreenDriverPanicOutOfBounds));
	__ASSERT_DEBUG(rect.iTl.iY>=0 && rect.iBr.iY<=iSize.iHeight,Panic(EScreenDriverPanicOutOfBounds));

	const TInt longWidth = iLongWidth;
	TUint16* pixelPtr = PixelAddress(rect.iTl.iX,rect.iTl.iY);
	const TUint16* pixelRowPtrLimit = pixelPtr + (rect.Height() * longWidth);

	if (iShadowMode & EFade)
		{
		TUint16* pixelRowPtr = pixelPtr;
		TUint16* pixelPtrLimit = pixelPtr + rect.Width();

		while (pixelRowPtr < pixelRowPtrLimit)
			{
			for (TUint16* tempPixelPtr = pixelRowPtr; tempPixelPtr < pixelPtrLimit; tempPixelPtr++)
				tempPixelPtr[0] = FadeIndex(tempPixelPtr[0]);

			pixelRowPtr += longWidth;
			pixelPtrLimit += longWidth;
			}
		}

	if (iShadowMode & EShadow)
		{
		TUint16* pixelRowPtr = pixelPtr;
		TUint16* pixelPtrLimit = pixelPtr + rect.Width();

		while (pixelRowPtr < pixelRowPtrLimit)
			{
			for (TUint16* tempPixelPtr = pixelRowPtr; tempPixelPtr < pixelPtrLimit; tempPixelPtr++)
				tempPixelPtr[0] = ShadowIndex(tempPixelPtr[0]);

			pixelRowPtr += longWidth;
			pixelPtrLimit += longWidth;
			}
		}
	}

void CDrawSixteenBppBitmapCommon::ShadowBuffer(TInt aLength,TUint32* aBuffer)
	{
	__ASSERT_DEBUG(aLength>0,Panic(EScreenDriverPanicZeroLength));
	__ASSERT_DEBUG(aBuffer,Panic(EScreenDriverPanicNullPointer));

	const TUint16* limit = ((TUint16*)aBuffer) + aLength;

	if (iShadowMode & EFade)
		{
		for (TUint16* buffer = (TUint16*)aBuffer; buffer < limit; buffer++)
			buffer[0] = FadeIndex(buffer[0]);
		}

	if (iShadowMode & EShadow)
		{
		for (TUint16* buffer = (TUint16*)aBuffer; buffer < limit; buffer++)
			buffer[0] = ShadowIndex(buffer[0]);
		}
	}

void CDrawSixteenBppBitmapCommon::ReadLine(TInt aX,TInt aY,TInt aLength,TAny* aBuffer) const
	{
	const TUint16* pixelPtr = PixelAddress(aX,aY);
	if (iOrientation == EOrientationNormal && iScalingOff)
		Mem::Copy(aBuffer,pixelPtr,aLength * 2);
	else
		{
		const TInt pixelPtrInc = LogicalPixelAddressIncrement();
		TUint16* bufferPtr = STATIC_CAST(TUint16*,aBuffer);
		const TUint16* bufferPtrLimit = bufferPtr + aLength;
		while (bufferPtr < bufferPtrLimit)
			{
			*bufferPtr++ = *pixelPtr;
			pixelPtr += pixelPtrInc;
			}
		}
	}

void CDrawSixteenBppBitmapCommon::WriteBinary(TInt aX,TInt aY,TUint32* aData,TInt aLength,TInt aHeight,TUint16 aColor)
	{
	DeOrientate(aX,aY);
	TInt pixelInc;
	TInt rowInc;
	SetPixelInc(pixelInc, rowInc);
	const TUint32* dataLimit = aData + aHeight;
	const TUint32 dataMaskLimit = (aLength < 32) ? 1 << aLength : 0;
	TUint16* pixelPtr = PixelAddress(aX,aY);
	const TUint16* bitsStart = reinterpret_cast <const TUint16*> (iBits);
	const TUint16* bitsEnd = bitsStart + iLongWidth * iSize.iHeight;
	TInt orgY = aY;
	while (aData < dataLimit)
		{
		TUint32 dataWord = *aData++;
		TUint32 dataMask = 1;
		TUint16* tempPixelPtr = pixelPtr;
		if (iScalingOff)
			{
			while (dataMask != dataMaskLimit)
				{
				if(dataWord & dataMask)
					*tempPixelPtr = aColor;

				tempPixelPtr += pixelInc;
				dataMask <<= 1;
				}
			}
		else
			{
			while (dataMask != dataMaskLimit)
				{
				if(dataWord & dataMask)
					{
					const TUint16* pixelRowPtrLimit = bitsStart + (aY + 1) * iLongWidth;
					SetPixels(tempPixelPtr, aColor, pixelRowPtrLimit, bitsStart, bitsEnd);
					}
				tempPixelPtr += pixelInc;
				dataMask <<= 1;
				IncScaledY(aY);
				}
			}
		pixelPtr += rowInc;
		IncScaledY(aY, orgY);		
		}
	}

void CDrawSixteenBppBitmapCommon::WriteBinaryOp(TInt aX,TInt aY,TUint32* aData,TInt aLength,TInt aHeight,TUint16 aColor,CGraphicsContext::TDrawMode aDrawMode)
	{
	if (aLength <= 0)
		return;

	DeOrientate(aX,aY);
	TUint16* pixelPtr = PixelAddress(aX,aY);
	const TUint32* dataPtrLimit = aData + aHeight;
	const TUint32 dataMaskLimit = (aLength < 32) ? 1 << aLength : 0;
	TInt pixelInc;
	TInt rowInc;
	SetPixelInc(pixelInc, rowInc);
	const TUint16* bitsStart = reinterpret_cast <const TUint16*> (iBits);
	const TUint16* bitsEnd = bitsStart + iLongWidth * iSize.iHeight;
	TInt orgY = aY;
	if (aColor)
		{
		while (aData < dataPtrLimit)
			{
			TUint32 dataWord = *aData++;
			TUint32 dataMask = 1;
			TUint16* tempPixelPtr = pixelPtr;
			if (iScalingOff)
				{
				while (dataMask != dataMaskLimit)
					{
					if(dataWord & dataMask)
						{
						if(aDrawMode==CGraphicsContext::EDrawModeXOR)
							*tempPixelPtr ^= aColor;
						else if(aDrawMode==CGraphicsContext::EDrawModeAND)
							*tempPixelPtr &= aColor;
						else if(aDrawMode==CGraphicsContext::EDrawModeOR)
							*tempPixelPtr |= aColor;
						}
					tempPixelPtr += pixelInc;
					dataMask <<= 1;
					}
				}
			else
				{
				while(dataMask != dataMaskLimit)
					{
					if(dataWord & dataMask)
						{
						const TUint16* pixelRowPtrLimit = bitsStart + (aY + 1) * iLongWidth;
						if(aDrawMode==CGraphicsContext::EDrawModeXOR)
							{
							XORPixels(tempPixelPtr, aColor, pixelRowPtrLimit, bitsStart, bitsEnd);
							}
						else if(aDrawMode==CGraphicsContext::EDrawModeAND)
							{
							ANDPixels(tempPixelPtr, aColor, pixelRowPtrLimit, bitsStart, bitsEnd);
							}
						else if(aDrawMode==CGraphicsContext::EDrawModeOR)
							{
							ORPixels(tempPixelPtr, aColor, pixelRowPtrLimit, bitsStart, bitsEnd);
							}
						}
					tempPixelPtr += pixelInc;
					dataMask <<= 1;
					IncScaledY(aY);
					}
				}
			pixelPtr += rowInc;
			IncScaledY(aY, orgY);			
			}
		}
	else if(aDrawMode==CGraphicsContext::EDrawModeAND)
		{
		while (aData < dataPtrLimit)
			{
			TUint32 dataWord = *aData++;
			TUint32 dataMask = 1;
			TUint16* tempPixelPtr = pixelPtr;
			if (iScalingOff)
				{
				while (dataMask != dataMaskLimit)
					{
					if(dataWord & dataMask)
						*tempPixelPtr = 0;

					tempPixelPtr += pixelInc;
					dataMask <<= 1;
					}
				}
			else
				{
				while(dataMask != dataMaskLimit)
					{
					if(dataWord & dataMask)
						{
						const TUint16* pixelRowPtrLimit = bitsStart + (aY + 1) * iLongWidth;
						SetPixels(tempPixelPtr, TUint16(0), pixelRowPtrLimit, bitsStart, bitsEnd);
						}
					tempPixelPtr += pixelInc;
					dataMask <<= 1;
					}
				}
			pixelPtr += rowInc;
			IncScaledY(aY, orgY);			
			}
		}
	}

void CDrawSixteenBppBitmapCommon::WriteBinaryLineVertical(TInt aX,TInt aY,TUint32* aData,TInt aHeight,TUint16 aColor,TBool aUp)
	{
	__ASSERT_DEBUG(iScalingOff, User::Invariant());	
	DeOrientate(aX,aY);

	TInt scanlineByteLength;

	switch(iOrientation)
		{
		case EOrientationNormal:
			scanlineByteLength = iLongWidth;
			break;
		case EOrientationRotated90:
			scanlineByteLength = -1;
			break;
		case EOrientationRotated180:
			scanlineByteLength = -iLongWidth;
			break;
		default: // EOrientationRotated270
			scanlineByteLength = 1;	
		}

	if (aUp)
		scanlineByteLength = -scanlineByteLength;

	TUint16* pixelPtr = PixelAddress(aX,aY);
	const TUint16* pixelPtrLimit = pixelPtr + (aHeight * scanlineByteLength);
	TUint32 dataWord = *aData;
	TUint32 dataMask = 1;

	while(pixelPtr != pixelPtrLimit)
		{
		if(!dataMask)
			{
			dataMask = 1;
			aData++;
			dataWord = *aData;
			}
			
		if(dataWord & dataMask)
			*pixelPtr = aColor;
		
		dataMask <<= 1;
		pixelPtr += scanlineByteLength;
		}
	}


void CDrawSixteenBppBitmapCommon::WriteRgbMulti(TInt aX,TInt aY,TInt aLength,TInt aHeight,TUint16 aColor)
	{
	const TInt longWidth = iLongWidth;
	const TInt scanLineWords = iScanLineWords;

	TUint16* pixelPtr = PixelAddress(aX,aY);
	const TUint16* pixelRowPtrLimit = pixelPtr + (aHeight * longWidth);

	if ((aColor >> 8) == (TUint8)aColor)
		{
		while (pixelPtr < pixelRowPtrLimit)
			{
			Mem::Fill(pixelPtr,aLength * 2,TUint8(aColor));
			pixelPtr += longWidth;
			}
		}
	else
		{
		const TBool leadingPixel = aX & 1;
		const TBool trailingPixel = (aX + aLength) & 1;
		const TUint32 colorWord = (aColor << 16) | aColor;

		TUint16* lastPixelPtr = pixelPtr + aLength - 1;
		TUint32* wordPtr = REINTERPRET_CAST(TUint32*,pixelPtr + (leadingPixel ? 1 : 0));
		TUint32* wordPtrLimit = REINTERPRET_CAST(TUint32*,lastPixelPtr + (trailingPixel ? 0 : 1));

		__ASSERT_DEBUG(!(TInt(wordPtr) & 3),Panic(EScreenDriverPanicInvalidPointer));
		__ASSERT_DEBUG(!(TInt(wordPtrLimit) & 3),Panic(EScreenDriverPanicInvalidPointer));

		if (leadingPixel)
			{
			while (pixelPtr < pixelRowPtrLimit)
				{
				pixelPtr[0] = aColor;
				pixelPtr += longWidth;
				}
			}

		while (wordPtr < (TUint32*)pixelRowPtrLimit)
			{
			MemFillTUint32(wordPtr, wordPtrLimit-wordPtr, colorWord);
			wordPtr += scanLineWords;
			wordPtrLimit += scanLineWords;
			}

		if (trailingPixel)
			{
			while (lastPixelPtr < pixelRowPtrLimit)
				{
				lastPixelPtr[0] = aColor;
				lastPixelPtr += longWidth;
				}
			}
		}
	}

void CDrawSixteenBppBitmapCommon::WriteRgbMultiXOR(TInt aX,TInt aY,TInt aLength,TInt aHeight,TUint16 aColor)
	{
	const TInt longWidth = iLongWidth;
	TUint16* pixelPtr = PixelAddress(aX,aY);
	TUint16* pixelPtrLimit = pixelPtr + aLength;
	const TUint16* pixelRowPtrLimit = pixelPtr + (aHeight * longWidth);

	while (pixelPtr < pixelRowPtrLimit)
		{
		for (TUint16* tempPixelPtr = pixelPtr; tempPixelPtr < pixelPtrLimit; tempPixelPtr++)
			tempPixelPtr[0] ^= aColor;

		pixelPtr += longWidth;
		pixelPtrLimit += longWidth;
		}
	}

void CDrawSixteenBppBitmapCommon::WriteRgbMultiAND(TInt aX,TInt aY,TInt aLength,TInt aHeight,TUint16 aColor)
	{
	const TInt longWidth = iLongWidth;
	TUint16* pixelPtr = PixelAddress(aX,aY);
	TUint16* pixelPtrLimit = pixelPtr + aLength;
	const TUint16* pixelRowPtrLimit = pixelPtr + (aHeight * longWidth);

	while (pixelPtr < pixelRowPtrLimit)
		{
		for (TUint16* tempPixelPtr = pixelPtr; tempPixelPtr < pixelPtrLimit; tempPixelPtr++)
			tempPixelPtr[0] &= aColor;

		pixelPtr += longWidth;
		pixelPtrLimit += longWidth;
		}
	}

void CDrawSixteenBppBitmapCommon::WriteRgbMultiOR(TInt aX,TInt aY,TInt aLength,TInt aHeight,TUint16 aColor)
	{
	const TInt longWidth = iLongWidth;
	TUint16* pixelPtr = PixelAddress(aX,aY);
	TUint16* pixelPtrLimit = pixelPtr + aLength;
	const TUint16* pixelRowPtrLimit = pixelPtr + (aHeight * longWidth);

	while (pixelPtr < pixelRowPtrLimit)
		{
		for (TUint16* tempPixelPtr = pixelPtr; tempPixelPtr < pixelPtrLimit; tempPixelPtr++)
			tempPixelPtr[0] |= aColor;

		pixelPtr += longWidth;
		pixelPtrLimit += longWidth;
		}
	}

void CDrawSixteenBppBitmapCommon::WriteLine(TInt aX,TInt aY,TInt aLength,TUint32* aBuffer)
	{
	TUint16* pixelPtr = PixelAddress(aX,aY);
	if (iOrientation == EOrientationNormal && iScalingOff)
		Mem::Copy(pixelPtr,aBuffer,aLength * 2);
	else
		{
		const TInt pixelPtrInc = LogicalPixelAddressIncrement();
		TUint16* bufferPtr = REINTERPRET_CAST(TUint16*,aBuffer);
		TUint16* bufferPtrLimit = bufferPtr + aLength;
		if (iScalingOff)
			{
			while (bufferPtr < bufferPtrLimit)
				{
				*pixelPtr = *bufferPtr++;
				pixelPtr += pixelPtrInc;
				}
			}
		else
			{
			const TUint16* bitsStart = reinterpret_cast <const TUint16*> (iBits);
			const TUint16* bitsEnd = bitsStart + iLongWidth * iSize.iHeight;
			while(bufferPtr < bufferPtrLimit)
				{
				const TUint16* pixelRowPtrLimit = bitsStart + (aY + 1) * iLongWidth;
				SetPixels(pixelPtr, *bufferPtr++, pixelRowPtrLimit, bitsStart, bitsEnd);
				pixelPtr += pixelPtrInc;
				IncScaledY(aY);
				}
			}
		}
	}

void CDrawSixteenBppBitmapCommon::WriteLineXOR(TInt aX,TInt aY,TInt aLength,TUint32* aBuffer)
	{
	TUint16* pixelPtr = PixelAddress(aX,aY);
	const TInt pixelPtrInc = LogicalPixelAddressIncrement();
	TUint16* bufferPtr = REINTERPRET_CAST(TUint16*,aBuffer);
	const TUint16* bufferPtrLimit = bufferPtr + aLength;
	if (iScalingOff)
		{
		while (bufferPtr < bufferPtrLimit)
			{
			*pixelPtr ^= *bufferPtr++;
			pixelPtr += pixelPtrInc;
			}
		}
	else
		{
		const TUint16* bitsStart = reinterpret_cast <const TUint16*> (iBits);
		const TUint16* bitsEnd = bitsStart + iLongWidth * iSize.iHeight;
		while(bufferPtr < bufferPtrLimit)
			{
			const TUint16* pixelRowPtrLimit = bitsStart + (aY + 1) * iLongWidth;
			XORPixels(pixelPtr, *bufferPtr++, pixelRowPtrLimit, bitsStart, bitsEnd);
			pixelPtr += pixelPtrInc;
			IncScaledY(aY);
			}
		}
	}

void CDrawSixteenBppBitmapCommon::WriteLineAND(TInt aX,TInt aY,TInt aLength,TUint32* aBuffer)
	{
	TUint16* pixelPtr = PixelAddress(aX,aY);
	const TInt pixelPtrInc = LogicalPixelAddressIncrement();
	TUint16* bufferPtr = REINTERPRET_CAST(TUint16*,aBuffer);
	const TUint16* bufferPtrLimit = bufferPtr + aLength;
	if (iScalingOff)
		{
		while (bufferPtr < bufferPtrLimit)
			{
			*pixelPtr &= *bufferPtr++;
			pixelPtr += pixelPtrInc;
			}
		}
	else
		{
		const TUint16* bitsStart = reinterpret_cast <const TUint16*> (iBits);
		const TUint16* bitsEnd = bitsStart + iLongWidth * iSize.iHeight;
		while(bufferPtr < bufferPtrLimit)
			{
			const TUint16* pixelRowPtrLimit = bitsStart + (aY + 1) * iLongWidth;
			ANDPixels(pixelPtr, *bufferPtr++, pixelRowPtrLimit, bitsStart, bitsEnd);
			pixelPtr += pixelPtrInc;
			IncScaledY(aY);
			}
		}
	}

void CDrawSixteenBppBitmapCommon::WriteLineOR(TInt aX,TInt aY,TInt aLength,TUint32* aBuffer)
	{
	TUint16* pixelPtr = PixelAddress(aX,aY);
	const TInt pixelPtrInc = LogicalPixelAddressIncrement();
	TUint16* bufferPtr = REINTERPRET_CAST(TUint16*,aBuffer);
	const TUint16* bufferPtrLimit = bufferPtr + aLength;
	if (iScalingOff)
		{
		while (bufferPtr < bufferPtrLimit)
			{
			*pixelPtr |= *bufferPtr++;
			pixelPtr += pixelPtrInc;
			}
		}
	else
		{
		const TUint16* bitsStart = reinterpret_cast <const TUint16*> (iBits);
		const TUint16* bitsEnd = bitsStart + iLongWidth * iSize.iHeight;
		while(bufferPtr < bufferPtrLimit)
			{
			const TUint16* pixelRowPtrLimit = bitsStart + (aY + 1) * iLongWidth;
			ORPixels(pixelPtr, *bufferPtr++, pixelRowPtrLimit, bitsStart, bitsEnd);
			pixelPtr += pixelPtrInc;
			IncScaledY(aY);
			}
		}
	}

/**
Implementation for CFbsDrawDevice::GetInterface().
Retrieves a pointer to a specified interface of CFbsDrawDevice implementation.
@param aInterfaceId Interface identifier of the interface to be retrieved.
@param aInterface Address of variable that retrieves the specified interface.
@return KErrNone If the interface is supported, KErrNotSupported otherwise.
*/

TInt CDrawSixteenBppBitmapCommon::GetInterface(TInt aInterfaceId, TAny*& aInterface)
	{
	aInterface = NULL;
	TInt ret = KErrNotSupported;
	
	if (aInterfaceId == KFastBlit2InterfaceID)
		{
		aInterface = static_cast<MFastBlit2*>(this);
		ret = KErrNone;
		}
	else 
		return CDrawBitmap::GetInterface(aInterfaceId, aInterface);
		
	return ret;
	}

/**
CDrawSixteenBppBitmapCommon::WriteBitmapBlock() implementation.
@internalTechnology
@see MFastBlit2::WriteBitmapBlock()
*/
TInt CDrawSixteenBppBitmapCommon::WriteBitmapBlock(const TPoint& aDest,
									CFbsDrawDevice* aSrcDrawDevice,
									const TRect& aSrcRect)
	{
	__ASSERT_DEBUG(aSrcDrawDevice && ((aSrcDrawDevice->DisplayMode()==EColor64K) || (aSrcDrawDevice->DisplayMode()==EColor4K)), Panic(EScreenDriverPanicInvalidParameter));
	
	TAny* interface=NULL;
	TInt ret = aSrcDrawDevice->GetInterface(KFastBlit2InterfaceID, interface);
	if (ret != KErrNone)
		{
		return KErrNotSupported;
		}

	TAny* interface1=NULL;
	ret = aSrcDrawDevice->GetInterface(KScalingSettingsInterfaceID, interface1);
	if(ret != KErrNone || (interface1 && !reinterpret_cast<MScalingSettings*>(interface1)->IsScalingOff()))
		{
		return KErrNotSupported;
		}

	ret = aSrcDrawDevice->GetInterface(KOrientationInterfaceID, interface1);
	if(ret != KErrNone || (interface1 && reinterpret_cast<MDrawDeviceOrientation*>(interface1)->Orientation() != 0))
		{
		return KErrNotSupported;
		}

	ret = aSrcDrawDevice->GetInterface(KDrawDeviceOriginInterfaceID, interface1);
	if(ret != KErrNone)
		{
		return KErrNotSupported;
		}
	
	if(interface1)
		{
	 	TPoint pt;
	 	reinterpret_cast<MDrawDeviceOrigin*>(interface1)->Get(pt);
	 	if(pt.iX != 0 || pt.iY != 0)
	 		{
			return KErrNotSupported;
	 		}
		}

	const TUint32* srcBase = reinterpret_cast<MFastBlit2*>(interface)->Bits();
	__ASSERT_DEBUG(srcBase!=NULL, Panic(EScreenDriverPanicInvalidParameter));
	TInt srcStride = aSrcDrawDevice->ScanLineBytes();  
	__ASSERT_DEBUG((srcStride&3)==0, Panic(EScreenDriverPanicInvalidParameter));  // stride is assumed to be a multiple of 4
	TSize srcSize = aSrcDrawDevice->SizeInPixels();

	return WriteBitmapBlock(aDest, srcBase, srcStride, srcSize, aSrcRect);
	}

								
/**
CDrawSixteenBppBitmapCommon::WriteBitmapBlock() implementation.
@internalTechnology
@see MFastBlit2::WriteBitmapBlock()
*/													
TInt CDrawSixteenBppBitmapCommon::WriteBitmapBlock(const TPoint& aDest,
									const TUint32* aSrcBase,
									TInt aSrcStride,
									const TSize& aSrcSize,
									const TRect& aSrcRect)
	{
	__ASSERT_DEBUG(aSrcBase, Panic(EScreenDriverPanicInvalidParameter));
	__ASSERT_DEBUG((aSrcStride&3)==0, Panic(EScreenDriverPanicInvalidParameter));
	__ASSERT_DEBUG(iBits, Panic(EScreenDriverPanicInvalidPointer));

	if (iShadowMode!=NULL ||
    	(iUserDispMode!=NULL && iUserDispMode!=iDispMode) ||
    	iOrientation!=EOrientationNormal ||
		!IsScalingOff() ||
		!iOriginIsZero)
		{
		return KErrNotSupported;
		}
	
	__ASSERT_DEBUG(aSrcRect.iTl.iX >= 0, Panic(EScreenDriverPanicOutOfBounds)); 
	__ASSERT_DEBUG(aSrcRect.iTl.iY >= 0, Panic(EScreenDriverPanicOutOfBounds));
	__ASSERT_DEBUG(aSrcRect.iBr.iX <= aSrcSize.iWidth,  Panic(EScreenDriverPanicOutOfBounds));
	__ASSERT_DEBUG(aSrcRect.iBr.iY <= aSrcSize.iHeight, Panic(EScreenDriverPanicOutOfBounds));
	__ASSERT_DEBUG(aDest.iX >= 0, Panic(EScreenDriverPanicOutOfBounds));
	__ASSERT_DEBUG(aDest.iY >= 0, Panic(EScreenDriverPanicOutOfBounds));
	__ASSERT_DEBUG((aDest.iX + aSrcRect.Width())  <= SizeInPixels().iWidth,  Panic(EScreenDriverPanicOutOfBounds));
	__ASSERT_DEBUG((aDest.iY + aSrcRect.Height()) <= SizeInPixels().iHeight, Panic(EScreenDriverPanicOutOfBounds));
	
	const TInt srcStride16 = aSrcStride     >> 1;
	const TInt dstStride16 = iScanLineWords << 1;
	
	if (aSrcSize.iWidth == aSrcRect.Width() &&
		aSrcSize.iWidth == SizeInPixels().iWidth &&
		srcStride16 == dstStride16)
		{
		// Optimum case - one memcpy
		__ASSERT_DEBUG(aSrcRect.iTl.iX==0 && aDest.iX==0, Panic(EScreenDriverPanicInvalidParameter));  // this is implied by the above conditions
		const TUint32* srcPtr = aSrcBase + (iScanLineWords * aSrcRect.iTl.iY);
		TUint32* dstPtr       = iBits    + (iScanLineWords * aDest.iY);
		const TInt length = aSrcStride * aSrcRect.Height();
		Mem::Move(dstPtr, srcPtr, length);
		return KErrNone;
		}
		
	// Sub-optimal case - one memcpy per line
	const TUint16* srcPtr = (TUint16*)aSrcBase + (srcStride16 * aSrcRect.iTl.iY) + aSrcRect.iTl.iX;
	TUint16* dstPtr       = (TUint16*)iBits    + (dstStride16 * aDest.iY       ) + aDest.iX;
	const TInt length = aSrcRect.Width() << 1;
	TInt lines = aSrcRect.Height();
	while (lines--)
		{
		Mem::Copy(dstPtr, srcPtr, length);
		srcPtr += srcStride16;
		dstPtr += dstStride16;
		}
	return KErrNone;
	}

/**
CDrawSixteenBppBitmapCommon::Bits() implementation.
@internalTechnology
@see MFastBlit2::Bits()
*/
const TUint32* CDrawSixteenBppBitmapCommon::Bits() const
	{
	return iBits;
	}

// CDrawSixteenBppBitmap

TInt CDrawSixteenBppBitmap::Construct(TSize aSize)
	{
	return Construct(aSize, ((aSize.iWidth + 1) & ~1) << 1);
	}

TInt CDrawSixteenBppBitmap::Construct(TSize aSize, TInt aStride)
	{
	iDispMode = EColor64K;
	return CDrawSixteenBppBitmapCommon::Construct(aSize, aStride);
	}

void CDrawSixteenBppBitmap::Shadow(TRgb& aColor)
	{
	if (iShadowMode & EFade)
		{
#if defined(SYMBIAN_USE_FAST_FADING)
		TUint16 color = aColor._Color64K();
		TInt alpha = aColor.Alpha();
		color = TUint16(((color >> K16bppFastFadeShift) & ~K16bppFastFadeMask) + K16bppFastFadeOffset);
		aColor = TRgb::_Color64K(color);
		aColor.SetAlpha(alpha);
#else
		TRgb fadeColor = TRgb::_Color64K(aColor._Color64K());
		fadeColor.SetAlpha(aColor.Alpha());
		aColor = FadeRgb(fadeColor);
#endif
		}

	if (iShadowMode & EShadow)
		{
		TRgb shadowColor = TRgb::_Color64K(ShadowIndex(TUint16(aColor._Color64K())));
		shadowColor.SetAlpha(aColor.Alpha());
		aColor = shadowColor;
		}
	}

/**
The overloaded function for Shadow(TRgb) which works directly with
the Red, Green and Blue colour components to increase the performance.
@param aRed Red component of colour.
@param aGreen Green component of colour.
@param aBlue Blue component of colour.
*/
FORCEINLINE void CDrawSixteenBppBitmap::Shadow(TInt& aRed, TInt& aGreen, TInt& aBlue)
	{
	if (iShadowMode & EFade)
		{
#if defined(SYMBIAN_USE_FAST_FADING)
		TUint16 color = PackColor64K(aRed, aGreen, aBlue);
		color = TUint16(((color >> K16bppFastFadeShift) & ~K16bppFastFadeMask) + K16bppFastFadeOffset);
		UnpackColor64K(color, aRed, aGreen, aBlue);
#else
		FadeRgb(aRed, aGreen, aBlue);
#endif
		}

	if (iShadowMode & EShadow)
		{
		ShadowIndex(aRed, aGreen, aBlue);
		}
	}

/**
The overloaded function for Shadow(TRgb) which works directly with
16 bit colour instead of TRgb to increase the performance.
@param a64KColor The 16 bit colour value.
*/
FORCEINLINE void CDrawSixteenBppBitmap::Shadow(TUint16& a64KColor)
	{
	if (iShadowMode & EFade)
		{
#if defined(SYMBIAN_USE_FAST_FADING)
		a64KColor = TUint16(((a64KColor >> K16bppFastFadeShift) & ~K16bppFastFadeMask) + K16bppFastFadeOffset);
#else
		TRgb fadeColor = TRgb::_Color64K(a64KColor);
		fadeColor.SetAlpha(0xFF);
		a64KColor = FadeRgb(fadeColor)._Color64K();
#endif
		}
	if (iShadowMode & EShadow)
		{
		a64KColor = ShadowIndex(a64KColor);
		}
	}

TUint16 CDrawSixteenBppBitmap::ShadowIndex(TUint16 aColor64KIndex)
	{
	TInt red = (aColor64KIndex & 0xf800) >> 11;
	TInt green = (aColor64KIndex & 0x07e0) >> 5;
	TInt blue = aColor64KIndex & 0x001f;

	red = Max(0,red-8);
	green = Max(0,green-16);
	blue = Max(0,blue-8);

	return TUint16((red << 11) | (green << 5) | blue);
	}

/**
The overloaded function for ShadowIndex(TUint16) which works directly with
the Red, Green and Blue colour components to increase the performance.
@param aRed Red component of colour.
@param aGreen Green component of colour.
@param aBlue Blue component of colour.
*/
FORCEINLINE void CDrawSixteenBppBitmap::ShadowIndex(TInt& aRed, TInt& aGreen, TInt& aBlue)
	{
	aRed = Max(0,aRed-8);
	aGreen = Max(0,aGreen-16);
	aBlue = Max(0,aBlue-8);
	}

TUint16 CDrawSixteenBppBitmap::FadeIndex(TUint16 aColor64KIndex)
	{
#if defined(SYMBIAN_USE_FAST_FADING)
	return TUint16(((aColor64KIndex >> K16bppFastFadeShift) & ~K16bppFastFadeMask) + K16bppFastFadeOffset);
#else
	return TUint16(FadeRgb(TRgb::_Color64K(aColor64KIndex))._Color64K());
#endif
	}

TRgb CDrawSixteenBppBitmap::ReadRgbNormal(TInt aX,TInt aY) const
	{
	return TRgb::_Color64K(*PixelAddress(aX,aY));
	}

void CDrawSixteenBppBitmap::WriteRgb(TInt aX,TInt aY,TRgb aColor)
	{
	register TUint16* pixelAddr = PixelAddress(aX, aY);
	register TUint16 aPixel = TUint16(aColor._Color64K());
	
	const TInt sourceAlpha = aColor.Alpha();	

	if (sourceAlpha==0)
		return;
	
	if (sourceAlpha<0xff)
		{
		const TUint32 srcInternal=aColor.Internal();
		const TUint32 srcRB=srcInternal & 0x00FF00FF;
		const TUint32 srcG=(srcInternal & 0xFF00) >> 8;
		aPixel = BlendTo16(srcRB, srcG, sourceAlpha, *pixelAddr);
		}

	if (iScalingOff)
		{
		*pixelAddr = aPixel;
		}
	else
		{
		const TUint16* bitsStart = reinterpret_cast <const TUint16*> (iBits);
		const TUint16* bitsEnd = bitsStart + iLongWidth * iSize.iHeight;
		const TUint16* pixelRowPtrLimit = bitsStart + (aY + 1) * iLongWidth;
		SetPixels(pixelAddr, aPixel, pixelRowPtrLimit, bitsStart, bitsEnd);
		}
	}

void CDrawSixteenBppBitmap::WriteBinary(TInt aX,TInt aY,TUint32* aData,TInt aLength,TInt aHeight,TRgb aColor)
	{
	const TInt sourceAlpha = aColor.Alpha();
	if (sourceAlpha==255)
		{
		CDrawSixteenBppBitmapCommon::WriteBinary(aX,aY,aData,aLength,aHeight,(TUint16)aColor._Color64K());
		return;
		}
	if (sourceAlpha==0)
		return;

	DeOrientate(aX,aY);

	TInt pixelInc;
	TInt rowInc;

	switch(iOrientation)
		{
		case EOrientationNormal:
			{
			pixelInc = 1;
			rowInc = iLongWidth;
			break;
			}
		case EOrientationRotated90:
			{
			pixelInc = iLongWidth;
			rowInc = -1;
			break;
			}
		case EOrientationRotated180:
			{
			pixelInc = -1;
			rowInc = -iLongWidth;
			break;
			}
		default: // EOrientationRotated270
			{
			pixelInc = -iLongWidth;
			rowInc = 1;
			}
		}

	const TUint32* dataLimit = aData + aHeight;
	const TUint32 dataMaskLimit = (aLength < 32) ? 1 << aLength : 0;

	TUint16* pixelPtr = PixelAddress(aX,aY);

	const TUint32 srcInternal=aColor.Internal();
	const TUint32 srcRB=srcInternal & 0x00FF00FF;
	const TUint32 srcG=(srcInternal & 0xFF00) >> 8;
	while (aData < dataLimit)
		{
		TUint32 dataWord = *aData++;
		TUint32 dataMask = 1;
		TUint16* tempPixelPtr = pixelPtr;

		while (dataMask != dataMaskLimit)
			{
			if(dataWord & dataMask)
				{
				*tempPixelPtr = BlendTo16(srcRB, srcG, sourceAlpha, *tempPixelPtr);
				}

			tempPixelPtr += pixelInc;
			dataMask <<= 1;
			}

		pixelPtr += rowInc;
		}
	}

void CDrawSixteenBppBitmap::WriteBinaryOp(TInt aX,TInt aY,TUint32* aData,TInt aLength,TInt aHeight,TRgb aColor,CGraphicsContext::TDrawMode aDrawMode)
	{
	CDrawSixteenBppBitmapCommon::WriteBinaryOp(aX,aY,aData,aLength,aHeight,(TUint16)aColor._Color64K(),aDrawMode);
	}

void CDrawSixteenBppBitmap::WriteBinaryLineVertical(TInt aX,TInt aY,TUint32* aData,TInt aHeight,TRgb aColor,TBool aUp)
	{
	const TInt sourceAlpha = aColor.Alpha();
	if (sourceAlpha==255)
		{
		CDrawSixteenBppBitmapCommon::WriteBinaryLineVertical(aX,aY,aData,aHeight,(TUint16)aColor._Color64K(),aUp);
		return;
		}
	if (sourceAlpha==0)
		return;

	DeOrientate(aX,aY);

	TInt scanlineByteLength;

	switch(iOrientation)
		{
		case EOrientationNormal:
			scanlineByteLength = iLongWidth;
			break;
		case EOrientationRotated90:
			scanlineByteLength = -1;
			break;
		case EOrientationRotated180:
			scanlineByteLength = -iLongWidth;
			break;
		default:// EOrientationRotated270
			scanlineByteLength = 1;	
		}

	if (aUp)
		scanlineByteLength = -scanlineByteLength;

	TUint16* pixelPtr = PixelAddress(aX,aY);
	const TUint16* pixelPtrLimit = pixelPtr + (aHeight * scanlineByteLength);
	TUint32 dataWord = *aData;
	TUint32 dataMask = 1;

	const TUint32 srcInternal=aColor.Internal();
	const TUint32 srcRB=srcInternal & 0x00FF00FF;
	const TUint32 srcG=(srcInternal & 0xFF00) >> 8;
	while(pixelPtr != pixelPtrLimit)
		{
		if(!dataMask)
			{
			dataMask = 1;
			aData++;
			dataWord = *aData;
			}

		if(dataWord & dataMask)
			{
			*pixelPtr = BlendTo16(srcRB, srcG, sourceAlpha, *pixelPtr);
			}
		dataMask <<= 1;
		pixelPtr += scanlineByteLength;
		}
	}

/**
MAlphaBlend::WriteRgbAlphaLine2() implementation.
@see MAlphaBlend::WriteRgbAlphaLine2()
*/
void CDrawSixteenBppBitmap::WriteRgbAlphaLine(TInt aX, TInt aY, TInt aLength,
                                              const TUint8* aRgbBuffer,
                                              const TUint8* aMaskBuffer,
                                              MAlphaBlend::TShadowing aShadowing,
                                              CGraphicsContext::TDrawMode /*aDrawMode*/)
    {
	DeOrientate(aX,aY);
	TUint16* pixelPtr = PixelAddress(aX,aY);
	const TInt pixelPtrInc = LogicalPixelAddressIncrement();
	const TUint8* maskBufferPtrLimit = aMaskBuffer + aLength;
	TUint16 pixelColor;

	__ASSERT_DEBUG( (((((TUint)pixelPtr)&1)==0) && ((((TUint)aRgbBuffer)&3)==0)), Panic(EScreenDriverPanicInvalidParameter));

	if (iScalingOff)
		{
		if (!(iShadowMode & (EFade | EShadow)) && iUserDispMode == ENone)
			{
			TUint32* rgbBuffer32 = (TUint32*)aRgbBuffer;
			while (aMaskBuffer < maskBufferPtrLimit)
				{
				pixelPtr[0] = Blend32To16(rgbBuffer32[0], aMaskBuffer[0], pixelPtr[0]);
				pixelPtr += pixelPtrInc;
				rgbBuffer32 ++;
				aMaskBuffer++;
				}
			}
		else
			{
			while (aMaskBuffer < maskBufferPtrLimit)
				{
				TInt blue = aRgbBuffer[0];
				TInt green = aRgbBuffer[1];
				TInt red = aRgbBuffer[2];
				if(aShadowing == MAlphaBlend::EShdwBefore)
					{
					Shadow(red,green,blue);
					}
				pixelColor = ::AlphaBlend(red,green,blue, pixelPtr[0],aMaskBuffer[0]);
				if(aShadowing == MAlphaBlend::EShdwAfter)
					{
					Shadow(pixelColor);
					}
				MapColorToUserDisplayMode(pixelColor);
				pixelPtr[0] = pixelColor;

				pixelPtr += pixelPtrInc;
				aRgbBuffer += 4;
				aMaskBuffer++;
				}
			}
		}
	else
		{
		const TUint16* bitsStart = reinterpret_cast <const TUint16*> (iBits);
		const TUint16* bitsEnd = bitsStart + iLongWidth * iSize.iHeight;
		while (aMaskBuffer < maskBufferPtrLimit)
			{
			TInt blue = aRgbBuffer[0];
			TInt green = aRgbBuffer[1];
			TInt red = aRgbBuffer[2];
            if(aShadowing == MAlphaBlend::EShdwBefore)
                {
                Shadow(red,green,blue);
                }
            pixelColor = ::AlphaBlend(red,green,blue,pixelPtr[0],aMaskBuffer[0]);
            if(aShadowing == MAlphaBlend::EShdwAfter)
                {
		        Shadow(pixelColor);
                }
			MapColorToUserDisplayMode(pixelColor);
			const TUint16* pixelRowPtrLimit = bitsStart + (aY + 1) * iLongWidth;
			SetPixels(pixelPtr, pixelColor, pixelRowPtrLimit, bitsStart, bitsEnd);
			pixelPtr += pixelPtrInc;
			aRgbBuffer += 4;
			aMaskBuffer++;
			IncScaledY(aY);
			}
		}
	}

void CDrawSixteenBppBitmap::WriteRgbMulti(TInt aX,TInt aY,TInt aLength,TInt aHeight,TRgb aColor)
	{
	CDrawSixteenBppBitmapCommon::WriteRgbMulti(aX,aY,aLength,aHeight,(TUint16)aColor._Color64K());
	}

void CDrawSixteenBppBitmap::BlendRgbMulti(TInt aX,TInt aY,TInt aLength,TInt aHeight,TRgb aColor)
	{
	const TInt sourceAlpha = aColor.Alpha();
	if (sourceAlpha==255)// opaque
		{
		CDrawSixteenBppBitmapCommon::WriteRgbMulti(aX,aY,aLength,aHeight,(TUint16)aColor._Color64K());
		return;
		}
	if (sourceAlpha==0)// transparent
		return;

	const TInt sourceRed = aColor.Red();
	const TInt sourceGreen = aColor.Green();
	const TInt sourceBlue = aColor.Blue();

	const TInt longWidth = iLongWidth;
	TUint16* pixelPtr = PixelAddress(aX,aY);
	TUint16* pixelPtrLimit = pixelPtr + aLength;
	const TUint16* pixelRowPtrLimit = pixelPtr + (aHeight * longWidth);
	const TInt mask=aColor.Alpha();
	const TUint32 srcInternal=aColor.Internal();
	const TUint32 srcRB=srcInternal & 0x00FF00FF;
	const TUint32 srcG=(srcInternal & 0xFF00) >> 8;
	while (pixelPtr < pixelRowPtrLimit)
		{
		for (TUint16* tempPixelPtr = pixelPtr; tempPixelPtr < pixelPtrLimit; tempPixelPtr++)
			{
			*tempPixelPtr = BlendTo16(srcRB, srcG, mask, *tempPixelPtr);
			}
		pixelPtr += longWidth;
		pixelPtrLimit += longWidth;
		}
	}

void CDrawSixteenBppBitmap::WriteRgbMultiXOR(TInt aX,TInt aY,TInt aLength,TInt aHeight,TRgb aColor)
	{
	CDrawSixteenBppBitmapCommon::WriteRgbMultiXOR(aX,aY,aLength,aHeight,(TUint16)aColor._Color64K());
	}

void CDrawSixteenBppBitmap::WriteRgbMultiAND(TInt aX,TInt aY,TInt aLength,TInt aHeight,TRgb aColor)
	{
	CDrawSixteenBppBitmapCommon::WriteRgbMultiAND(aX,aY,aLength,aHeight,(TUint16)aColor._Color64K());
	}

void CDrawSixteenBppBitmap::WriteRgbMultiOR(TInt aX,TInt aY,TInt aLength,TInt aHeight,TRgb aColor)
	{
	CDrawSixteenBppBitmapCommon::WriteRgbMultiOR(aX,aY,aLength,aHeight,(TUint16)aColor._Color64K());
	}

void CDrawSixteenBppBitmap::WriteRgbAlphaMulti(TInt aX,TInt aY,TInt aLength,TRgb aColor,const TUint8* aMaskBuffer)
	{
	const TInt alpha = aColor.Alpha();
	if (alpha==0 || aLength<=0)
		return;
	DeOrientate(aX,aY);
	TUint16* pixelPtr = PixelAddress(aX,aY);
	const TInt pixelPtrInc = LogicalPixelAddressIncrement();
	const TUint8* maskBufferPtrLimit = aMaskBuffer + aLength;

 	if (iShadowMode)
 		{
 		Shadow(aColor);
 		}
 
	if (iScalingOff)
		{
	    const TUint32 color16bpp=aColor.Color64K();
		const TUint32 srcInternal=aColor.Internal();
		const TUint32 srcRB=srcInternal & 0x00FF00FF;
		const TUint32 srcG=(srcInternal & 0xFF00) >> 8;
		if (alpha == 0xff)
			{
			while (aMaskBuffer < maskBufferPtrLimit)
				{
				const TUint32 mask=*aMaskBuffer++;
				if (mask)
					{
					if (mask==0xFF)
						*pixelPtr = color16bpp;
					else
						*pixelPtr = BlendTo16(srcRB, srcG, mask, *pixelPtr);
					}
				pixelPtr += pixelPtrInc;
				}
			}
		else
			{ // pen is semi-transparent, so we must blend using both the mask and pen alpha
			while (aMaskBuffer < maskBufferPtrLimit)
				{
				TUint blendAlpha = alpha;
				TUint maskAlpha = *aMaskBuffer++;
				if (maskAlpha)
					{
					if (maskAlpha!=0xFF)
						blendAlpha=((maskAlpha+1) * alpha)>>8;
					*pixelPtr = BlendTo16(srcRB, srcG, blendAlpha, *pixelPtr);
					}
				pixelPtr += pixelPtrInc;
				}
			}
		}
	else
		{
		const TInt red = aColor.Red();
		const TInt green = aColor.Green();
		const TInt blue = aColor.Blue();
		if (alpha == 0xff)
			{
			const TUint16* bitsStart = reinterpret_cast <const TUint16*> (iBits);
			const TUint16* bitsEnd = bitsStart + iLongWidth * iSize.iHeight;
			while(aMaskBuffer < maskBufferPtrLimit)
				{
				TUint16 pixelColor = AlphaBlend(red,green,blue, *pixelPtr, *aMaskBuffer);
				const TUint16* pixelRowPtrLimit = bitsStart + (aY + 1) * iLongWidth;
				SetPixels(pixelPtr, pixelColor, pixelRowPtrLimit, bitsStart, bitsEnd);
				pixelPtr += pixelPtrInc;
				aMaskBuffer++;
				IncScaledY(aY);
				}
			}
		else
			{ // require special handling for different alpha values
			const TUint16* bitsStart = reinterpret_cast <const TUint16*> (iBits);
			const TUint16* bitsEnd = bitsStart + iLongWidth * iSize.iHeight;
			while(aMaskBuffer < maskBufferPtrLimit)
				{
				const TInt maskAlpha = *aMaskBuffer;
				const TInt sourceAlpha = alpha * maskAlpha;
				const TInt inverseAlpha = 255*255 - sourceAlpha;

				TInt pixelRed;
				TInt pixelGreen;
				TInt pixelBlue;
				UnpackColor64K(*pixelPtr, pixelRed, pixelGreen, pixelBlue);
				TInt blueAfter = TUint8(((blue * sourceAlpha) + (pixelBlue * inverseAlpha)) / (255*255));
				TInt greenAfter = TUint8(((green * sourceAlpha) + (pixelGreen * inverseAlpha)) / (255*255));
				TInt redAfter = TUint8(((red * sourceAlpha) + (pixelRed * inverseAlpha)) / (255*255));
				TUint16 pixelColor = PackColor64K(redAfter, greenAfter, blueAfter);

				const TUint16* pixelRowPtrLimit = bitsStart + (aY + 1) * iLongWidth;
				SetPixels(pixelPtr, pixelColor, pixelRowPtrLimit, bitsStart, bitsEnd);
				pixelPtr += pixelPtrInc;
				aMaskBuffer++;
				IncScaledY(aY);
				}
			}
		}
	}

void CDrawSixteenBppBitmap::MapColorToUserDisplayMode(TRgb& aColor)
	{
	switch (iUserDispMode)
		{
	case EGray2:
		aColor = TRgb::_Gray2(aColor._Gray2());
		break;
	case EGray4:
		aColor = TRgb::_Gray4(aColor._Gray4());
		break;
	case EGray16:
		aColor = TRgb::_Gray16(aColor._Gray16());
		break;
	case EGray256:
		aColor = TRgb::_Gray256(aColor._Gray256());
		break;
	case EColor16:
		aColor = TRgb::Color16(aColor.Color16());
		break;
	case EColor256:
		aColor = TRgb::Color256(aColor.Color256());
		break;
	case EColor4K:
		aColor = TRgb::_Color4K(aColor._Color4K());
		break;
	default:
		break;
		}
	}

/**
The overloaded function for MapColorToUserDisplayMode(TRgb) which works directly with
16 bit colour instead of TRgb to increase the performance.
@param a64KColor The 16 bit colour value.
*/
void CDrawSixteenBppBitmap::MapColorToUserDisplayMode(TUint16& aColor64K)
	{
	TRgb color = TRgb::_Color64K(aColor64K);

	switch (iUserDispMode)
		{
	case EGray2:
		{
		color = TRgb::_Gray2(color._Gray2());
		}
		break;
	case EGray4:
		{
		color = TRgb::_Gray4(color._Gray4());
		}
		break;
	case EGray16:
		{
		color = TRgb::_Gray16(color._Gray16());
		}
		break;
	case EGray256:
		{
		color = TRgb::_Gray256(color._Gray256());
		}
		break;
	case EColor16:
		{
		color = TRgb::Color16(color.Color16());
		}
		break;
	case EColor256:
		{
		color = TRgb::Color256(color.Color256());
		}
		break;
	case EColor4K:
		{
		color = TRgb::_Color4K(color._Color4K());
		}
		break;
	default:
		break;
		}
	aColor64K = color._Color64K();
	}

void CDrawSixteenBppBitmap::MapBufferToUserDisplayMode(TInt aLength,TUint32* aBuffer)
	{
	TUint16* bufferPtr = (TUint16*)aBuffer;
	const TUint16* bufferLimit = bufferPtr + aLength;
	TRgb color;
	
	switch (iUserDispMode)
		{
	case EGray2:
		while (bufferPtr < bufferLimit)
			{
			color = TRgb::_Color64K(*bufferPtr);
			color = TRgb::_Gray2(color._Gray2());
			*bufferPtr++ = TUint16(color._Color64K());
			}
		break;
	case EGray4:
		while (bufferPtr < bufferLimit)
			{
			color = TRgb::_Color64K(*bufferPtr);
			color = TRgb::_Gray4(color._Gray4());
			*bufferPtr++ = TUint16(color._Color64K());
			}
		break;
	case EGray16:
		while (bufferPtr < bufferLimit)
			{
			color = TRgb::_Color64K(*bufferPtr);
			color = TRgb::_Gray16(color._Gray16());
			*bufferPtr++ = TUint16(color._Color64K());
			}
		break;
	case EGray256:
		while (bufferPtr < bufferLimit)
			{
			color = TRgb::_Color64K(*bufferPtr);
			color = TRgb::_Gray256(color._Gray256());
			*bufferPtr++ = TUint16(color._Color64K());
			}
		break;
	case EColor16:
		while (bufferPtr < bufferLimit)
			{
			color = TRgb::_Color64K(*bufferPtr);
			color = TRgb::Color16(color.Color16());
			*bufferPtr++ = TUint16(color._Color64K());
			}
		break;
	case EColor256:
		while (bufferPtr < bufferLimit)
			{
			color = TRgb::_Color64K(*bufferPtr);
			color = TRgb::Color256(color.Color256());
			*bufferPtr++ = TUint16(color._Color64K());
			}
		break;
	case EColor4K:
		while (bufferPtr < bufferLimit)
			{
			color = TRgb::_Color64K(*bufferPtr);
			color = TRgb::_Color4K(color._Color4K());
			*bufferPtr++ = TUint16(color._Color64K());
			}
		break;
	default:
		break;
		}
	}

TInt CDrawSixteenBppBitmap::WriteRgbOutlineAndShadow(TInt aX, TInt aY, const TInt aLength, 
													TUint32 aOutlinePenColor, TUint32 aShadowColor,
													TUint32 aFillColor, const TUint8* aDataBuffer)
	{
	DeOrientate(aX,aY);
	TUint16* pixelPtr = PixelAddress(aX,aY);
	const TInt pixelPtrInc = LogicalPixelAddressIncrement();
	const TUint8* dataBufferPtrLimit = aDataBuffer + aLength;	
	TInt blendedRedColor;
	TInt blendedGreenColor;
	TInt blendedBlueColor;
	TUint blendedAlpha;
	TUint32 finalColor;
	const TUint16* normTable = PtrTo16BitNormalisationTable();

	//Get red color. Equivalent to TRgb::Red()
	const TInt redOutlinePenColor = (aOutlinePenColor & 0xff0000) >> 16;
	const TInt redShadowColor = (aShadowColor & 0xff0000) >> 16;
	const TInt redFillColor = (aFillColor & 0xff0000) >> 16;

	//Get green color. Equivalent to TRgb::Green()
	const TInt greenOutlinePenColor = (aOutlinePenColor & 0xff00) >> 8;
	const TInt greenShadowColor = (aShadowColor & 0xff00) >> 8;
	const TInt greenFillColor = (aFillColor & 0xff00) >> 8;

	//Get blue color. Equivalent to TRgb::Blue()
	const TInt blueOutlinePenColor = aOutlinePenColor & 0xff;
	const TInt blueShadowColor = aShadowColor & 0xff;
	const TInt blueFillColor = aFillColor & 0xff;

	//Get alpha color. Equivalent to TRgb::Alpha()
	const TUint alphaOutlinePenColor = aOutlinePenColor >> 24;
	const TUint alphaShadowColor = aShadowColor >> 24;
	const TUint alphaFillColor = aFillColor >> 24;

	while (aDataBuffer < dataBufferPtrLimit)
		{
		TUint8 index = *aDataBuffer++;
		if (255 == FourColorBlendLookup[index][KBackgroundColorIndex])
			{
			//background colour
			//No drawing required
			}
		else if (255 == FourColorBlendLookup[index][KFillColorIndex])
			{
			//Use fill colour to draw
			finalColor = aFillColor;
			*pixelPtr = Blend32To16((finalColor | 0xff000000), alphaFillColor, *pixelPtr);
			}
		else if (255 == FourColorBlendLookup[index][KShadowColorIndex])
			{
			//Use shadow colour to draw
			finalColor = aShadowColor;
			*pixelPtr = Blend32To16((finalColor | 0xff000000), alphaShadowColor, *pixelPtr);
			}
		else if (255 == FourColorBlendLookup[index][KOutlineColorIndex])
			{
			//Use outline colour to draw
			finalColor = aOutlinePenColor; 
			*pixelPtr = Blend32To16((finalColor | 0xff000000), alphaOutlinePenColor, *pixelPtr);
			}
		else
			{
			//Get the background pixel colour. Using the lookup table to convert 16 to 32 bit colour
			blendedRedColor = (redOutlinePenColor * FourColorBlendLookup[index][KOutlineColorIndex] * alphaOutlinePenColor + 
								redShadowColor * FourColorBlendLookup[index][KShadowColorIndex] * alphaShadowColor +
								redFillColor * FourColorBlendLookup[index][KFillColorIndex] * alphaFillColor) >> 16;
	
			blendedGreenColor = (greenOutlinePenColor * FourColorBlendLookup[index][KOutlineColorIndex] * alphaOutlinePenColor  + 
								greenShadowColor * FourColorBlendLookup[index][KShadowColorIndex] * alphaShadowColor +
								greenFillColor * FourColorBlendLookup[index][KFillColorIndex] * alphaFillColor) >> 16;
	
			blendedBlueColor = (blueOutlinePenColor * FourColorBlendLookup[index][KOutlineColorIndex] * alphaOutlinePenColor  + 
								blueShadowColor * FourColorBlendLookup[index][KShadowColorIndex] * alphaShadowColor +
								blueFillColor * FourColorBlendLookup[index][KFillColorIndex] * alphaFillColor) >> 16;
	
			blendedAlpha = (alphaOutlinePenColor * FourColorBlendLookup[index][KOutlineColorIndex] + 
							alphaShadowColor * FourColorBlendLookup[index][KShadowColorIndex] +
							alphaFillColor * FourColorBlendLookup[index][KFillColorIndex]) >> 8;

			finalColor = PMA2NonPMAPixel((blendedAlpha << 24) | (blendedRedColor << 16) | (blendedGreenColor << 8) | blendedBlueColor, normTable);		
			*pixelPtr = Blend32To16((finalColor | 0xff000000), blendedAlpha, *pixelPtr);
			}
		pixelPtr += pixelPtrInc;
		}
	return KErrNone;
	}
