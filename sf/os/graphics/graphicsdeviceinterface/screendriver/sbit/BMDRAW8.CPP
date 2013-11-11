// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

// CDrawEightBppBitmapCommon
//EColor256 screen/bitmap device might be scaled.
//In this case right-bottom coordinate might go outside the drawing rectangle.
//Then - we need to check that we do not try to draw something outside the drawing rectangle.

//Initializes iSize, iDrawRect, iLongWidth, iScanlineWords data members.
//It should be called every time when iSize is going to be changed - from Construct().
//@param aSize Physical screen size in pixels.
//@panic EScreenDriverPanicInvalidSize - Invalid aSize parameter. This might happen if the 
//device is scaled and the scaling origin goes outside physical drawing rectangle.
void CDrawEightBppBitmapCommon::SetSize(const TSize& aSize) 
	{
	CDrawBitmap::SetSize(aSize);
	__ASSERT_DEBUG(iSize == aSize, User::Invariant());
	iLongWidth = (iSize.iWidth + 3) & ~3;
	iScanLineWords = iLongWidth / 4;
	}
 
TInt CDrawEightBppBitmapCommon::Construct(TSize aSize, TInt aStride)
	{
	iBits = NULL;
	CDrawBitmap::SetSize(aSize);
	__ASSERT_DEBUG(iSize == aSize, User::Invariant());
	if (aStride & 3)
		return KErrArgument;
	iLongWidth = aStride;
	if (iLongWidth < aSize.iWidth)
		return KErrArgument;
	iScanLineWords = aStride >> 2;
	TInt size = Max(aSize.iWidth,aSize.iHeight);
	if(size < 0)
		return KErrArgument;
	iScanLineBuffer = (TUint32*)(User::Heap().Alloc(size));
	if(iScanLineBuffer == NULL)
		return KErrNoMemory;
	return KErrNone;
	}

//aX, aY - physical coordinates
TUint8* CDrawEightBppBitmapCommon::PixelAddress(TInt aX,TInt aY) const
	{
	return (TUint8*)iBits + aY * iLongWidth + aX;
	}

void CDrawEightBppBitmapCommon::InvertBuffer(TInt aLength,TUint32* aBuffer)
	{
	const TUint32* limit = aBuffer + (aLength + 3) / 4;

	while (aBuffer < limit)
		*aBuffer++ ^= 0xffffffff;
	}

//CDrawEightBppBitmapCommon::ReadLine() called from CDrawBitmap::ReadLine()
//aX and aY - physical coordinates.
//Reads aLength pixel values and places them in aBuffer.
//aBuffer size should be at least aLength.
void CDrawEightBppBitmapCommon::ReadLine(TInt aX,TInt aY,TInt aLength,TAny* aBuffer) const
	{
	const TUint8* pixelPtr = PixelAddress(aX,aY);

	if (iOrientation == EOrientationNormal && iScalingOff)
		Mem::Copy(aBuffer,pixelPtr,aLength);
	else
		{
		register TInt pixelPtrInc = LogicalPixelAddressIncrement();

		TUint8* bufferPtr = static_cast <TUint8*> (aBuffer);
		const TUint8* bufferPtrLimit = bufferPtr + aLength;

		while (bufferPtr < bufferPtrLimit)
			{
			*bufferPtr++ = *pixelPtr;
			pixelPtr += pixelPtrInc;
			}
		}
	}

//CDrawEightBppBitmapCommon::WriteRgb() called from exported CDrawBitmap::WriteRgb()
//aX, aY - physical coordinates
void CDrawEightBppBitmapCommon::WriteRgb(TInt aX,TInt aY,TUint8 aPixel)
	{
	register TUint8* pixelAddr = PixelAddress(aX, aY);
	if(iScalingOff)
		{
		*pixelAddr = aPixel;
		}
	else
		{
		const TUint8* bitsStart = reinterpret_cast <const TUint8*> (iBits);
		const TUint8* bitsEnd = bitsStart + iLongWidth * iSize.iHeight;
		const TUint8* pixelRowPtrLimit = bitsStart + (aY + 1) * iLongWidth;
		SetPixels(pixelAddr, aPixel, pixelRowPtrLimit, bitsStart, bitsEnd);
		}
	}

//CDrawEightBppBitmapCommon::WriteBinary() called from exported CDrawBitmap::WriteBinary()
//aX, aY - logical coordinates
//"aData" parameter has "aHeight" 32 bits words
//"aLength" parameter tells how many bits (up to 32) has to be used from each 32 bit word
//This method is used for bitmap font symbols drawing
//One possible optimization: If "no scaling" And "pixelPtrInc == 1" Then
//writing could be made on 32 bits words (they has to be aligned before the operation)
void CDrawEightBppBitmapCommon::WriteBinary(TInt aX,TInt aY,TUint32* aData,TInt aLength,
											TInt aHeight,TUint8 aPixel)
	{
	DeOrientate(aX,aY);//aX, aY - physical coordinates
	TInt pixelInc;
	TInt rowInc;
	SetPixelInc(pixelInc, rowInc);
	const TUint32* dataLimit = aData + aHeight;
	register TUint32 dataMaskLimit = (aLength < 32) ? 1 << aLength : 0;
	TUint8* pixelPtr = PixelAddress(aX,aY);
	const TUint8* bitsStart = reinterpret_cast <const TUint8*> (iBits);
	const TUint8* bitsEnd = bitsStart + iLongWidth * iSize.iHeight;
	TInt orgY = aY;
	while (aData < dataLimit)
		{
		register TUint32 dataWord = *aData++;
		register TUint32 dataMask = 1;
		register TUint8* tempPixelPtr = pixelPtr;
		if(iScalingOff)
			{
			while (dataMask != dataMaskLimit)
				{
				if(dataWord & dataMask)
					{
					*tempPixelPtr = aPixel;
					}
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
					const TUint8* pixelRowPtrLimit = bitsStart + (aY + 1) * iLongWidth;
					SetPixels(tempPixelPtr, aPixel, pixelRowPtrLimit, bitsStart, bitsEnd);
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

//CDrawEightBppBitmapCommon::WriteBinaryOp() called from exported CDrawBitmap::WriteBinary()
//aX, aY - logical coordinates
//"aData" parameter has "aHeight" 32 bits words
//"aLength" parameter tells how many bits (up to 32) has to be used from each 32 bit word
//This method is used for bitmap font symbols drawing
//One possible optimization: If "no scaling" And "pixelPtrInc == 1" Then
//writing could be made on 32 bits words (they has to be aligned before the operation)
void CDrawEightBppBitmapCommon::WriteBinaryOp(TInt aX,TInt aY,TUint32* aData,TInt aLength,
											  TInt aHeight,TUint8 aPixel,
											  CGraphicsContext::TDrawMode aDrawMode)
	{
	if(aLength <= 0)
		{
		return;
		}
	DeOrientate(aX,aY);//aX, aY - physical coordinates
	TUint8* pixelPtr = PixelAddress(aX,aY);
	const TUint32* dataPtrLimit = aData + aHeight;
	register TUint32 dataMaskLimit = (aLength < 32) ? 1 << aLength : 0;
	TInt pixelInc;
	TInt rowInc;
	SetPixelInc(pixelInc, rowInc);
	const TUint8* bitsStart = reinterpret_cast <const TUint8*> (iBits);
	const TUint8* bitsEnd = bitsStart + iLongWidth * iSize.iHeight;
	TInt orgY = aY;
	if(aPixel)
		{
		while(aData < dataPtrLimit)
			{
			register TUint32 dataWord = *aData++;
			register TUint32 dataMask = 1;
			register TUint8* tempPixelPtr = pixelPtr;
			if(iScalingOff)
				{
				while(dataMask != dataMaskLimit)
					{
					if(dataWord & dataMask)
						{
						if(aDrawMode==CGraphicsContext::EDrawModeXOR)
							{
							*tempPixelPtr ^= aPixel;
							}
						else if(aDrawMode==CGraphicsContext::EDrawModeAND)
							{
							*tempPixelPtr &= aPixel;
							}
						else if(aDrawMode==CGraphicsContext::EDrawModeOR)
							{
							*tempPixelPtr |= aPixel;
							}
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
						const TUint8* pixelRowPtrLimit = bitsStart + (aY + 1) * iLongWidth;
						if(aDrawMode==CGraphicsContext::EDrawModeXOR)
							{
							XORPixels(tempPixelPtr, aPixel, pixelRowPtrLimit, bitsStart, bitsEnd);
							}
						else if(aDrawMode==CGraphicsContext::EDrawModeAND)
							{
							ANDPixels(tempPixelPtr, aPixel, pixelRowPtrLimit, bitsStart, bitsEnd);
							}
						else if(aDrawMode==CGraphicsContext::EDrawModeOR)
							{
							ORPixels(tempPixelPtr, aPixel, pixelRowPtrLimit, bitsStart, bitsEnd);
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
		while(aData < dataPtrLimit)
			{
			register TUint32 dataWord = *aData++;
			register TUint32 dataMask = 1;
			register TUint8* tempPixelPtr = pixelPtr;
			if(iScalingOff)
				{
				while(dataMask != dataMaskLimit)
					{
					if(dataWord & dataMask)
						{
						*tempPixelPtr = 0;
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
						const TUint8* pixelRowPtrLimit = bitsStart + (aY + 1) * iLongWidth;
						SetPixels(tempPixelPtr, TUint8(0), pixelRowPtrLimit, bitsStart, bitsEnd);
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

//aX, aY - logical coordinates
//"aData" parameter has 32 bits words
//"aLength" parameter tells how many pixels have to be written
//This method is used for bitmap font symbols vertical drawing
//The method should not be called if the scaling is ON!
void CDrawEightBppBitmapCommon::WriteBinaryLineVertical(TInt aX,TInt aY,TUint32* aData,
														TInt aLength,TUint8 aPixel,TBool aUp)
	{
	__ASSERT_DEBUG(iScalingOff, User::Invariant());

	DeOrientate(aX,aY);//aX, aY - physical coordinates

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

	register TUint8* pixelPtr = PixelAddress(aX,aY);
	const TUint8* pixelPtrLimit = pixelPtr + aLength * scanlineByteLength;
	register TUint32 dataWord = *aData;
	register TUint32 dataMask = 1;

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
			*pixelPtr = aPixel;
			}
		dataMask <<= 1;
		pixelPtr += scanlineByteLength;
		}
	}

//CDrawEightBppBitmapCommon::WriteLine() called from CDrawBitmap::WriteLine()
//aX and aY - physical coordinates
void CDrawEightBppBitmapCommon::WriteLine(TInt aX,TInt aY,TInt aLength,TUint32* aBuffer)
	{
	TUint8* pixelPtr = PixelAddress(aX,aY);
	if (iOrientation == EOrientationNormal && iScalingOff)
		{
		Mem::Copy(pixelPtr,aBuffer,aLength);
		}
	else
		{
		register TInt pixelPtrInc = LogicalPixelAddressIncrement();
		TUint8* bufferPtr = reinterpret_cast <TUint8*> (aBuffer);
		const TUint8* bufferPtrLimit = bufferPtr + aLength;
		if(iScalingOff)
			{
			while(bufferPtr < bufferPtrLimit)
				{
				*pixelPtr = *bufferPtr++;
				pixelPtr += pixelPtrInc;
				}
			}
		else
			{
			const TUint8* bitsStart = reinterpret_cast <const TUint8*> (iBits);
			const TUint8* bitsEnd = bitsStart + iLongWidth * iSize.iHeight;
			while(bufferPtr < bufferPtrLimit)
				{
				const TUint8* pixelRowPtrLimit = bitsStart + (aY + 1) * iLongWidth;
				SetPixels(pixelPtr, *bufferPtr++, pixelRowPtrLimit, bitsStart, bitsEnd);
				pixelPtr += pixelPtrInc;
				IncScaledY(aY);
				}
			}
		}
	}

//CDrawEightBppBitmapCommon::WriteLineXOR() called from CDrawBitmap::WriteLine()
//aX and aY - physical coordinates
//One possible optimization: If "no scaling" And "pixelPtrInc == 1" Then
//XOR could be made on 32 bits words (they  has to be aligned before the operation)
void CDrawEightBppBitmapCommon::WriteLineXOR(TInt aX,TInt aY,TInt aLength,TUint32* aBuffer)
	{
	TUint8* pixelPtr = PixelAddress(aX,aY);
	register TInt pixelPtrInc = LogicalPixelAddressIncrement();
	TUint8* bufferPtr = reinterpret_cast <TUint8*> (aBuffer);
	const TUint8* bufferPtrLimit = bufferPtr + aLength;
	if(iScalingOff)
		{
		while(bufferPtr < bufferPtrLimit)
			{
			*pixelPtr ^= *bufferPtr++;
			pixelPtr += pixelPtrInc;
			}
		}
	else
		{
		const TUint8* bitsStart = reinterpret_cast <const TUint8*> (iBits);
		const TUint8* bitsEnd = bitsStart + iLongWidth * iSize.iHeight;
		while(bufferPtr < bufferPtrLimit)
			{
			const TUint8* pixelRowPtrLimit = bitsStart + (aY + 1) * iLongWidth;
			XORPixels(pixelPtr, *bufferPtr++, pixelRowPtrLimit, bitsStart, bitsEnd);
			pixelPtr += pixelPtrInc;
			IncScaledY(aY);
			}
		}
	}

//CDrawEightBppBitmapCommon::WriteLineAND() called from CDrawBitmap::WriteLine()
//aX and aY - deorientated and scaled
//aLength - not scaled
//One possible optimization: If "no scaling" And "pixelPtrInc == 1" Then
//AND could be made on 32 bits words (they  has to be aligned before the operation)
void CDrawEightBppBitmapCommon::WriteLineAND(TInt aX,TInt aY,TInt aLength,TUint32* aBuffer)
	{
	TUint8* pixelPtr = PixelAddress(aX,aY);
	register TInt pixelPtrInc = LogicalPixelAddressIncrement();
	TUint8* bufferPtr = reinterpret_cast <TUint8*> (aBuffer);
	const TUint8* bufferPtrLimit = bufferPtr + aLength;
	if(iScalingOff)
		{
		while(bufferPtr < bufferPtrLimit)
			{
			*pixelPtr &= *bufferPtr++;
			pixelPtr += pixelPtrInc;
			}
		}
	else
		{
		const TUint8* bitsStart = reinterpret_cast <const TUint8*> (iBits);
		const TUint8* bitsEnd = bitsStart + iLongWidth * iSize.iHeight;
		while(bufferPtr < bufferPtrLimit)
			{
			const TUint8* pixelRowPtrLimit = bitsStart + (aY + 1) * iLongWidth;
			ANDPixels(pixelPtr, *bufferPtr++, pixelRowPtrLimit, bitsStart, bitsEnd);
			pixelPtr += pixelPtrInc;
			IncScaledY(aY);
			}
		}
	}

//CDrawEightBppBitmapCommon::WriteLineOR() called from CDrawBitmap::WriteLine()
//aX and aY - physical coordinates
//One possible optimization: If "no scaling" And "pixelPtrInc == 1" Then
//OR could be made on 32 bits words (they  has to be aligned before the operation)
void CDrawEightBppBitmapCommon::WriteLineOR(TInt aX,TInt aY,TInt aLength,TUint32* aBuffer)
	{
	TUint8* pixelPtr = PixelAddress(aX,aY);
	register TInt pixelPtrInc = LogicalPixelAddressIncrement();
	TUint8* bufferPtr = reinterpret_cast <TUint8*> (aBuffer);
	const TUint8* bufferPtrLimit = bufferPtr + aLength;
	if(iScalingOff)
		{
		while(bufferPtr < bufferPtrLimit)
			{
			*pixelPtr |= *bufferPtr++;
			pixelPtr += pixelPtrInc;
			}
		}
	else
		{
		const TUint8* bitsStart = reinterpret_cast <const TUint8*> (iBits);
		const TUint8* bitsEnd = bitsStart + iLongWidth * iSize.iHeight;
		while(bufferPtr < bufferPtrLimit)
			{
			const TUint8* pixelRowPtrLimit = bitsStart + (aY + 1) * iLongWidth;
			ORPixels(pixelPtr, *bufferPtr++, pixelRowPtrLimit, bitsStart, bitsEnd);
			pixelPtr += pixelPtrInc;
			IncScaledY(aY);
			}
		}
	}

//CDrawEightBppBitmapCommon::WriteRgbMulti() called from CDrawBitmap::WriteRgbMulti()
//aX and aY - physical coordinates
//aLength and aRows - physical length and rows
void CDrawEightBppBitmapCommon::WriteRgbMulti(TInt aX,TInt aY,TInt aLength,TInt aRows,
											  TUint8 aPixel)
	{
	register TInt longWidth = iLongWidth;
	TUint8* pixelPtr = PixelAddress(aX,aY);
	const TUint8* pixelRowPtrLimit = pixelPtr + aRows * longWidth;
	register const TUint8* bitsEnd = reinterpret_cast <const TUint8*> (iBits) + iLongWidth * iSize.iHeight;
	if(pixelRowPtrLimit >= bitsEnd)
		{
		pixelRowPtrLimit = bitsEnd;
		}

	while (pixelPtr < pixelRowPtrLimit)
		{
		Mem::Fill(pixelPtr,aLength,aPixel);
		pixelPtr += longWidth;
		}
	}

//CDrawEightBppBitmapCommon::WriteRgbMultiXOR() called from CDrawBitmap::WriteRgbMulti()
//aX and aY - physical coordinates
//aLength and aRows - physical length and rows
void CDrawEightBppBitmapCommon::WriteRgbMultiXOR(TInt aX,TInt aY,TInt aLength,TInt aRows,
												 TUint8 aPixel)
	{
	register TInt longWidth = iLongWidth;
	TUint8* pixelPtr = PixelAddress(aX,aY);
	TUint8* pixelPtrLimit = pixelPtr + aLength;
	const TUint8* pixelRowPtrLimit = pixelPtr + aRows * longWidth;
	register const TUint8* bitsEnd = reinterpret_cast <const TUint8*> (iBits) + iLongWidth * iSize.iHeight;
	if(pixelRowPtrLimit >= bitsEnd)
		{
		pixelRowPtrLimit = bitsEnd;
		}

	while (pixelPtr < pixelRowPtrLimit)
		{
		TUint8* tempPixelPtr = pixelPtr;

		while (tempPixelPtr < pixelPtrLimit)
			*tempPixelPtr++ ^= aPixel;

		pixelPtr += longWidth;
		pixelPtrLimit += longWidth;
		}
	}

//CDrawEightBppBitmapCommon::WriteRgbMultiAND() called from CDrawBitmap::WriteRgbMulti()
//aX and aY - physical coordinates
//aLength and aRows - physical length and rows
void CDrawEightBppBitmapCommon::WriteRgbMultiAND(TInt aX,TInt aY,TInt aLength,TInt aRows,
												 TUint8 aPixel)
	{
	register TInt longWidth = iLongWidth;
	TUint8* pixelPtr = PixelAddress(aX,aY);
	const TUint8* pixelPtrLimit = pixelPtr + aLength;
	const TUint8* pixelRowPtrLimit = pixelPtr + aRows * longWidth;
	register const TUint8* bitsEnd = reinterpret_cast <const TUint8*> (iBits) + iLongWidth * iSize.iHeight;
	if(pixelRowPtrLimit >= bitsEnd)
		{
		pixelRowPtrLimit = bitsEnd;
		}

	while (pixelPtr < pixelRowPtrLimit)
		{
		TUint8* tempPixelPtr = pixelPtr;

		while (tempPixelPtr < pixelPtrLimit)
			*tempPixelPtr++ &= aPixel;

		pixelPtr += longWidth;
		pixelPtrLimit += longWidth;
		}
	}

//CDrawEightBppBitmapCommon::WriteRgbMultiOR() called from CDrawBitmap::WriteRgbMulti()
//aX and aY - physical coordinates
//aLength and aRows - physical length and rows
void CDrawEightBppBitmapCommon::WriteRgbMultiOR(TInt aX,TInt aY,TInt aLength,TInt aRows,TUint8 aPixel)
	{
	register TInt longWidth = iLongWidth;
	TUint8* pixelPtr = PixelAddress(aX,aY);
	TUint8* pixelPtrLimit = pixelPtr + aLength;
	const TUint8* pixelRowPtrLimit = pixelPtr + aRows * longWidth;
	register const TUint8* bitsEnd = reinterpret_cast <const TUint8*> (iBits) + iLongWidth * iSize.iHeight;
	if(pixelRowPtrLimit >= bitsEnd)
		{
		pixelRowPtrLimit = bitsEnd;
		}

	while (pixelPtr < pixelRowPtrLimit)
		{
		TUint8* tempPixelPtr = pixelPtr;

		while (tempPixelPtr < pixelPtrLimit)
			*tempPixelPtr++ |= aPixel;

		pixelPtr += longWidth;
		pixelPtrLimit += longWidth;
		}
	}

/**
Implementation for CFbsDrawDevice::GetInterface().
Retrieves a pointer to a specified interface of CFbsDrawDevice implementation.
@param aInterfaceId Interface identifier of the interface to be retrieved.
@param aInterface Address of variable that retrieves the specified interface.
@return KErrNone If the interface is supported, KErrNotSupported otherwise.
*/

TInt CDrawEightBppBitmapCommon::GetInterface(TInt aInterfaceId, TAny*& aInterface)
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
CDrawEightBppBitmapCommon::WriteBitmapBlock() implementation.
@internalTechnology
@see MFastBlit2::WriteBitmapBlock()
*/
TInt CDrawEightBppBitmapCommon::WriteBitmapBlock(const TPoint& aDest,
									CFbsDrawDevice* aSrcDrawDevice,
									const TRect& aSrcRect)
	{
	__ASSERT_DEBUG(aSrcDrawDevice && ((aSrcDrawDevice->DisplayMode()==EColor256) || (aSrcDrawDevice->DisplayMode()==EGray256)), Panic(EScreenDriverPanicInvalidParameter));
	
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
CDrawEightBppBitmapCommon::WriteBitmapBlock() implementation.
@internalTechnology
@see MFastBlit2::WriteBitmapBlock()
*/													
TInt CDrawEightBppBitmapCommon::WriteBitmapBlock(const TPoint& aDest,
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
	
	const TInt srcStride8 = aSrcStride;
	const TInt dstStride8 = iScanLineWords << 2;
	
	if (aSrcSize.iWidth == aSrcRect.Width() &&
		aSrcSize.iWidth == SizeInPixels().iWidth &&
		srcStride8 == dstStride8)
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
	const TUint8* srcPtr = (TUint8*)aSrcBase + (srcStride8 * aSrcRect.iTl.iY) + aSrcRect.iTl.iX;
	TUint8* dstPtr       = (TUint8*)iBits    + (dstStride8 * aDest.iY       ) + aDest.iX;
	const TInt length = aSrcRect.Width();
	TInt lines = aSrcRect.Height();
	while (lines--)
		{
		Mem::Copy(dstPtr, srcPtr, length);
		srcPtr += srcStride8;
		dstPtr += dstStride8;
		}
	return KErrNone;
	}


/**
CDrawEightBppBitmapCommon::Bits() implementation.
@internalTechnology
@see MFastBlit2::Bits()
*/
const TUint32* CDrawEightBppBitmapCommon::Bits() const
	{
	return iBits;
}
