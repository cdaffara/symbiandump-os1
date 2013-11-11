// Copyright (c) 2003-2010 Nokia Corporation and/or its subsidiary(-ies).
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

TInt CDrawUTwentyFourBppBitmap::Construct(TSize aSize)
	{
	return Construct(aSize, aSize.iWidth << 2);
	}

TInt CDrawUTwentyFourBppBitmap::Construct(TSize aSize, TInt aStride)
	{
	iDispMode = EColor16MU;
	return CDrawThirtyTwoBppBitmapCommon::Construct(aSize, aStride);
	}

/**
MAlphaBlend::WriteRgbAlphaLine() implementation.
@see MAlphaBlend::WriteRgbAlphaLine()
*/
void CDrawUTwentyFourBppBitmap::WriteRgbAlphaLine(TInt aX, TInt aY, TInt aLength,
													const TUint8* aRgbBuffer,
													const TUint8* aMaskBuffer,
													MAlphaBlend::TShadowing aShadowing,
													CGraphicsContext::TDrawMode /*aDrawMode*/)
	{
	// precondition for this function is that the aRgbBuffer lies on a word boundary
    // Assert checks that the pointer is at a word boundary
    __ASSERT_DEBUG(!(((TUint)aRgbBuffer) & 0x3), Panic(EScreenDriverPanicInvalidPointer));
    
	DeOrientate(aX,aY);
	TUint32* pixelPtr = PixelAddress(aX,aY);
	TUint32* rgbBuffer = (TUint32*)aRgbBuffer;
	const TInt pixelPtrInc = PixelAddressIncrement();
	const TUint8* maskBufferPtrLimit = aMaskBuffer + aLength;
	
	if (!(iShadowMode & (EFade | EShadow)) && (iUserDispMode == ENone))
		{
		while (aMaskBuffer < maskBufferPtrLimit)
			{
			*pixelPtr = CalcAlphaPixel(*rgbBuffer, *aMaskBuffer, *pixelPtr);
			aMaskBuffer++;
			pixelPtr += pixelPtrInc;
			rgbBuffer++;
			}
		}
	else
		{
		while (aMaskBuffer < maskBufferPtrLimit)
			{
			TInt mask = *aMaskBuffer++;
			if (mask)
				{     
				TInt b = aRgbBuffer[0];
				TInt g = aRgbBuffer[1];
				TInt r = aRgbBuffer[2];
				if(aShadowing == MAlphaBlend::EShdwBefore)
					{
					if (iShadowMode & EFade)
						{
						r = ((r * iFadeMapFactor) >> 8) + iFadeMapOffset;
						g = ((g * iFadeMapFactor) >> 8) + iFadeMapOffset;
						b = ((b * iFadeMapFactor) >> 8) + iFadeMapOffset;
						}
					if (iShadowMode & EShadow)
						{
						r = (Max(0,r-0x40));
						g = (Max(0,g-0x40));
						b = (Max(0,b-0x40));
						}
					}
				if (mask != 0xff)
					{
					// (mask * r + (255 - mask) * value) / 255 =
					// ((257 * mask * (r - value)) >> 16) + value
					TInt value = *pixelPtr & 0xffffff;
					mask = (mask << 8) + mask; // mask = mask * 257
					TInt v = value >> 16;
					r = ((mask * (r - v)) >> 16) + v;
					v = (value >> 8) & 0xff;
					g = ((mask * (g - v)) >> 16) + v;
					v = value & 0xff;
					b = ((mask * (b - v)) >> 16) + v;
					}
				if(aShadowing == MAlphaBlend::EShdwAfter)
					{
					if (iShadowMode & EFade)
						{  
						r = ((r * iFadeMapFactor) >> 8) + iFadeMapOffset;
						g = ((g * iFadeMapFactor) >> 8) + iFadeMapOffset;
						b = ((b * iFadeMapFactor) >> 8) + iFadeMapOffset;            
						}
					if (iShadowMode & EShadow)
						{
						r = (Max(0,r-0x40));
						g = (Max(0,g-0x40));
						b = (Max(0,b-0x40));                  
						}
					}
				// Convert colour if an incompatible UserDisplayMode is being used
				CDrawBitmap::MapColorToUserDisplayMode(r,g,b);
				*pixelPtr = (r<<16) | (g<<8) | b | 0xff000000;
				}
			aRgbBuffer += 4;
			pixelPtr += pixelPtrInc;
			}
		}
	}

void CDrawUTwentyFourBppBitmap::ReadLine(TInt aX, TInt aY, TInt aLength, TAny* aBuffer, TDisplayMode aDispMode) const
	{
	if (aDispMode == EColor16MAP)
		{
		DeOrientate(aX, aY);
		CDrawThirtyTwoBppBitmapCommon::ReadLine(aX, aY, aLength, aBuffer);
		
		//Overwrite unused byte with 0xff to produce valid 16MAP data
		TUint8* alphaptr = (TUint8*) aBuffer+3;
		TUint8* bufEnd = (TUint8*) aBuffer + (aLength << 2);
		while (alphaptr < bufEnd)
			{
			*alphaptr = 0xff;
			alphaptr+=4;
			}
		
		return;
		}
	CDrawBitmap::ReadLine(aX, aY, aLength, aBuffer, aDispMode);
	}

void CDrawUTwentyFourBppBitmap::BlendRgbMulti(TInt aX,TInt aY,TInt aLength,TInt aHeight,TRgb aColor)
	{
	const TInt sourceAlpha = aColor.Alpha();
	if (sourceAlpha==255)// opaque
		{
		WriteRgbMulti(aX,aY,aLength,aHeight,aColor);
		return;
		}
	if (sourceAlpha==0)// transparent
		return;

	TUint32* pixelPtr = PixelAddress(aX,aY);
	TUint32* pixelRowPtrLimit = pixelPtr + (aHeight * iScanLineWords);
	TUint32* pixelPtrLimit = pixelPtr + aLength;

	const TUint32 sourceInternal=aColor.Internal();
	const TUint32 s_rb = sourceInternal & 0x00FF00FF;
	const TUint32 s_g = (sourceInternal & 0xFF00) >> 8;
	const TUint32 mask2 = sourceAlpha | (sourceAlpha << 16);
	while (pixelPtr < pixelRowPtrLimit)
		{
		for (TUint32* tempPixelPtr = pixelPtr; tempPixelPtr < pixelPtrLimit; tempPixelPtr++)
			{
			const TUint32 d = *tempPixelPtr;
			const TUint32 d_rb = d & 0x00FF00FF;
			const TUint32 rb = ((((sourceAlpha * ((0x01000100 + s_rb) - d_rb)) >> 8) + d_rb) - mask2) & 0x00FF00FF;

			const TInt d_g = (d & 0xFF00) >> 8;
			const TInt g = ((sourceAlpha * (s_g - d_g)) >> 8) + d_g;
	
			*tempPixelPtr = rb | (g<<8) | 0xff000000;
			}

		pixelPtr += iScanLineWords;
		pixelPtrLimit += iScanLineWords;
		}
	}

void CDrawUTwentyFourBppBitmap::BlendLine(TInt aX,TInt aY,TInt aLength,TUint32* aBuffer)
	{
	TUint32* pixelPtr = PixelAddress(aX,aY);

	const TUint32* bufferPtrLimit = aBuffer + aLength;
	const TInt pixelPtrInc = (iOrientation == EOrientationNormal) ? 1 : PixelAddressIncrement();
				
	while (aBuffer < bufferPtrLimit)
		{
		if((*aBuffer &0xFF000000) == 0xFF000000)
			{
			*pixelPtr = *aBuffer;
			}
		else if((*aBuffer & 0xFF000000))
			{	
			// specialization of pre-multiplied blending when the destination alpha is always 255
			const TUint32 src_rb = *aBuffer & 0x00FF00FF;
			const TUint32 src_g = *aBuffer & 0x0000FF00;
			const TUint32 mask = 0x100 - (*aBuffer >> 24);
			TUint32 dst_rb = *pixelPtr & 0x00FF00FF;
			TUint32 dst_g = *pixelPtr & 0x0000FF00;
			dst_rb = (src_rb + ((mask * dst_rb) >> 8)) & 0x00FF00FF;
			dst_g = (src_g + ((mask * dst_g) >> 8)) & 0x0000FF00;
			*pixelPtr = 0xFF000000 | dst_rb | dst_g;
			}
			
		aBuffer++;
		pixelPtr += pixelPtrInc;
		}
	}

TRgb CDrawUTwentyFourBppBitmap::RgbColor(TUint32 aColor) const
	{
	return TRgb::_Color16MU(aColor);
	}

TUint32 CDrawUTwentyFourBppBitmap::Color(const TRgb& aColor)
	{
	return aColor._Color16MA() | 0xff000000;	
	}


// Copies an EColor64K pixel to an EColor16MU screen
FORCEINLINE static void CopyPixel(const TUint16*& aSrcPtr, TUint32*& aDestPtr, TInt aPixelPtrInc, const TUint32* aHighAdd, const TUint16* aLowAdd)
	{
	*aDestPtr = aHighAdd[(*aSrcPtr) >> 8] | aLowAdd[(*aSrcPtr) & 0xff];
	aDestPtr += aPixelPtrInc;
	aSrcPtr++;
	}


// Copies two EColor64K pixels to an EColor16MU screen
FORCEINLINE static void CopyTwoPixels(const TUint32*& aSrcPtr, TUint32*& aDestPtr, TInt aPixelPtrInc, const TUint32* aHighAdd, const TUint16* aLowAdd)
	{
	const TUint16* scanPtr = reinterpret_cast<const TUint16*&>(aSrcPtr);
	*aDestPtr = aHighAdd[(*scanPtr) >> 8] | aLowAdd[(*scanPtr) & 0xff];
 	aDestPtr += aPixelPtrInc;
	scanPtr++;
	*aDestPtr = aHighAdd[(*scanPtr) >> 8] | aLowAdd[(*scanPtr) & 0xff];
 	aDestPtr += aPixelPtrInc;
 	aSrcPtr++;
	}


// Copies an EColor16MU pixel to an EColor16MU screen if necessary.
FORCEINLINE static void ProcessMaskPixel(const TUint32*& aSrcPtr, const TUint32 aMaskWord, TUint32& aSingleBitMask, TUint32*& aDestPtr, TInt aPixelPtrInc)
	{
	if (aMaskWord & aSingleBitMask)
		{
		*aDestPtr = *aSrcPtr;
		}
	aSrcPtr++;
	aDestPtr += aPixelPtrInc;
	aSingleBitMask <<= 1;
	}


// Copies an EColor64K pixel to an EColor16MU screen if necessary.	
FORCEINLINE static void ProcessMaskPixel(const TUint16*& aSrcPtr, const TUint32 aMaskWord, TUint32& aSingleBitMask, TUint32*& aDestPtr, TInt aPixelPtrInc, const TUint32* aHighAdd, const TUint16* aLowAdd)
	{	
	if (aMaskWord & aSingleBitMask)
		{
		*aDestPtr = aHighAdd[(*aSrcPtr) >> 8] | aLowAdd[(*aSrcPtr) & 0xff];
		}
	aSrcPtr++;
	aDestPtr += aPixelPtrInc;
	aSingleBitMask <<= 1;
	}


// Alpha-blends an EColor16MU pixel to an EColor16MU screen using a fixed mask value.
FORCEINLINE static void BlendAlphaPixel(const TUint32*& aSrcPtr, const TUint8 aMask, TUint32*& aDestPtr, TInt aPixelPtrInc)
	{
	const TUint32 s = *aSrcPtr++;
	const TUint32 d = *aDestPtr;
	
	// (a)  (mask * src + (255 - mask) * dest) / 255           This ideal formula
	// (b)  ((mask * (src - dest)) >> 8) + dest                A faster approximation to (a)
	// (c)  ((mask * (256 + src - dest) >> 8) + dest - mask    Equivalent to (b) but can be used on multiple colors at a time

	const TUint32 s_rb = s & 0x00FF00FF;
	const TUint32 d_rb = d & 0x00FF00FF;
	const TUint32 mask2 = aMask | (aMask << 16);
	const TUint32 rb = ((((aMask * ((0x01000100 + s_rb) - d_rb)) >> 8) + d_rb) - mask2) & 0x00FF00FF;

	const TInt s_g = (s & 0xFF00) >> 8;
	const TInt d_g = (d & 0xFF00) >> 8;
	const TInt g = ((aMask * (s_g - d_g)) >> 8) + d_g;
	
	*aDestPtr = rb | (g<<8) | 0xff000000;
	aDestPtr += aPixelPtrInc;
	}


// Alpha-blends an EColor16MU pixel to an EColor16MU screen if necessary.	
FORCEINLINE static void ProcessAlphaPixel(const TUint32*& aSrcPtr, const TUint8*& aMaskPtr, TUint32*& aDestPtr, TInt aPixelPtrInc)
	{
	const TInt mask = *aMaskPtr++;

	if (!mask)
		{
		// pixel is masked
		aSrcPtr++;
		aDestPtr += aPixelPtrInc;
		}
	else if (mask == 0xFF)
		{
		// pixel is unmasked
		*aDestPtr = *aSrcPtr++;
		aDestPtr += aPixelPtrInc;
		}
	else
		{
		BlendAlphaPixel(aSrcPtr, mask, aDestPtr, aPixelPtrInc);
		}
	}
	

// Alpha-blends an EColor64K pixel to an EColor16MU screen using a fixed mask value.
FORCEINLINE static void BlendAlphaPixel(const TUint16*& aSrcPtr, const TUint8 aMask, TUint32*& aDestPtr, TInt aPixelPtrInc)
	{
	const TUint32 s = *aSrcPtr++;
	const TUint32 d = *aDestPtr;
	
	// convert the source EColor64K red / blue pixels to EColor16MU
	// the top two bits in EColor64K are used to fill the bottom two bits in EColor16MU
	
	const TUint32 s_rb = ((s & 0xF800) << 8) | ((s & 0xE000) << 3) | ((s & 0x1F) << 3) | ((s & 0x1C) >>2);	
	const TUint32 d_rb = d & 0x00FF00FF;
	const TUint32 mask2 = aMask | (aMask << 16);
	const TUint32 rb = ((((aMask * ((0x01000100 + s_rb) - d_rb)) >> 8) + d_rb) - mask2) & 0x00FF00FF;
	
	// convert the source EColor64K green pixel to EColor16MU
	const TInt s_g = ((s & 0x07E0) >> 3) | ((s & 0x0600) >> 9);
	const TInt d_g = (d & 0xFF00) >> 8;
	const TInt g = ((aMask * (s_g - d_g)) >> 8) + d_g;
	
	*aDestPtr = rb | (g<<8) | 0xff000000;
	aDestPtr += aPixelPtrInc;
	}

	
// Alpha-blends an EColor64K pixel to an EColor16MU screen if necessary.	
FORCEINLINE static void ProcessAlphaPixel(const TUint16*& aSrcPtr, const TUint8*& aMaskPtr, TUint32*& aDestPtr, TInt aPixelPtrInc, const TUint32* aHighAdd, const TUint16* aLowAdd)
	{
	const TInt mask = *aMaskPtr++;

	if (!mask)
		{
		// pixel is masked
		aSrcPtr++;
		aDestPtr += aPixelPtrInc;
		}
	else if (mask == 0xFF) 
		{
		// pixel is unmasked
		CopyPixel(aSrcPtr, aDestPtr, aPixelPtrInc, aHighAdd, aLowAdd);
		}
	else
		{
		BlendAlphaPixel(aSrcPtr, mask, aDestPtr, aPixelPtrInc);
		}
	}


/**
CDrawUTwentyFourBppBitmap::WriteAlphaLineEx() implementation.
@internalTechnology
@see MFastBlit::WriteAlphaLineEx()
*/
void CDrawUTwentyFourBppBitmap::WriteAlphaLineEx(TInt aX, TInt aY, TInt aLength, TInt aSrcX,
													const TUint32* aSrcPtr,  TDisplayMode aSrcFormat,
													TInt aMaskX, const TUint32* aMaskPtr,
													MAlphaBlend::TShadowing aShadowing)
	{
	// Only certain pixel formats are supported.  Caller should check this.
	__ASSERT_DEBUG(aSrcFormat ==EColor16MU || aSrcFormat ==EColor64K, User::Invariant());
	
	DeOrientate(aX,aY);
	TUint32* pixelPtr = PixelAddress(aX,aY);
	const TInt pixelPtrInc = PixelAddressIncrement();
	
	if (aSrcFormat==EColor16MU)
		{		
		if (!(iShadowMode & EFade) && !(iShadowMode & EShadow) && ((iUserDispMode==ENone) || (iUserDispMode==EColor16MU)) )
			{
			aSrcPtr += aSrcX;
			const TUint32* maskWordPtr = aMaskPtr + (aMaskX >> 2);
			const TInt startBit = aMaskX & 0x3;
			
			if (startBit)
				{
				// Process initial incomplete mask word
				const TUint32 maskWord = *maskWordPtr++;
				TInt numPix = Min(aLength, 4 - startBit);  // number of pixels to process from the first word of the mask
				aLength -= numPix;
				
				if (!maskWord)
					{
					// maskWord is fully masked - skip the pixels
					aSrcPtr += numPix;
					pixelPtr += pixelPtrInc * numPix;
					}
				else if (maskWord == 0xFFFFFFFF)
					{
					// maskWord is fully unmasked - copy the pixels
					while (numPix--)
						{
						*pixelPtr = *aSrcPtr++;
						pixelPtr += pixelPtrInc;
						}
					}
				else
					{
					// At least one of the pixels needs to be blended
					const TUint8* maskPtr8 = reinterpret_cast<const TUint8*>(&maskWord);
					maskPtr8 += startBit;
					while (numPix--)
						{
						ProcessAlphaPixel(aSrcPtr, maskPtr8, pixelPtr, pixelPtrInc);
						}
					}
				}
			
			TInt numMaskWords = aLength >> 2;
			aLength &= 0x3;
			while (numMaskWords--)
				{
				// Process a complete mask word - 4 pixels
				const TUint32 maskWord = *maskWordPtr++;
				
				if (!maskWord)
					{
					// maskWord is fully masked - skip 4 pixels
					aSrcPtr += 4;
					pixelPtr += pixelPtrInc << 2;
					}
				else if (maskWord == 0xFFFFFFFF)
					{
					// maskWord is fully unmasked - copy 4 pixels
					*pixelPtr = *aSrcPtr++; pixelPtr += pixelPtrInc;
					*pixelPtr = *aSrcPtr++; pixelPtr += pixelPtrInc;
					*pixelPtr = *aSrcPtr++; pixelPtr += pixelPtrInc;
					*pixelPtr = *aSrcPtr++; pixelPtr += pixelPtrInc;
					}
				else
					{	
					// At least one of the 4 pixels needs to be blended
					const TUint8* maskPtr8 = reinterpret_cast<const TUint8*>(&maskWord);

					if (maskPtr8[0] && (maskPtr8[0] != 0xFF) &&
						maskPtr8[1] && (maskPtr8[1] != 0xFF) &&
						maskPtr8[2] && (maskPtr8[2] != 0xFF) &&
						maskPtr8[3] && (maskPtr8[3] != 0xFF))
						{
						// Blend all 4 pixels inline
						BlendAlphaPixel(aSrcPtr, maskPtr8[0], pixelPtr, pixelPtrInc);
						BlendAlphaPixel(aSrcPtr, maskPtr8[1], pixelPtr, pixelPtrInc);
						BlendAlphaPixel(aSrcPtr, maskPtr8[2], pixelPtr, pixelPtrInc);
						BlendAlphaPixel(aSrcPtr, maskPtr8[3], pixelPtr, pixelPtrInc);
						}
					else
						{
						ProcessAlphaPixel(aSrcPtr, maskPtr8, pixelPtr, pixelPtrInc);
						ProcessAlphaPixel(aSrcPtr, maskPtr8, pixelPtr, pixelPtrInc);
						ProcessAlphaPixel(aSrcPtr, maskPtr8, pixelPtr, pixelPtrInc);
						ProcessAlphaPixel(aSrcPtr, maskPtr8, pixelPtr, pixelPtrInc);
						}
					}
				}

			if (aLength)
				{
				const TUint32 maskWord = *maskWordPtr;
				if (!maskWord)
					{
					// maskWord is fully masked - skip the pixels
					return;
					}
				if (maskWord == 0xFFFFFFFF)
					{
					// maskWord is fully unmasked - copy the pixels
					while (aLength--)
						{
						*pixelPtr = *aSrcPtr++;
						pixelPtr += pixelPtrInc;
						}
					}
				else
					{
					// At least one of the pixels needs to be blended
					const TUint8* maskPtr8 = reinterpret_cast<const TUint8*>(&maskWord);
					while (aLength--)
						{
						ProcessAlphaPixel(aSrcPtr, maskPtr8, pixelPtr, pixelPtrInc);
						}
					}
				}
			}
		else
			{
			// Non-optimised path including shadowing and UserDisplayMode conversion
			const TUint8* srcPtr8  = reinterpret_cast<const TUint8*>(aSrcPtr + aSrcX);
			const TUint8* maskPtr8 = reinterpret_cast<const TUint8*>(aMaskPtr) + aMaskX;
			
			while (aLength--)
				{
				TInt mask = *maskPtr8++;
				if (mask)
					{
					TInt b = srcPtr8[0];
					TInt g = srcPtr8[1];
					TInt r = srcPtr8[2];
					if(aShadowing == MAlphaBlend::EShdwBefore)
						{
						if (iShadowMode & EFade)
							{
							r = ((r * iFadeMapFactor) >> 8) + iFadeMapOffset;
							g = ((g * iFadeMapFactor) >> 8) + iFadeMapOffset;
							b = ((b * iFadeMapFactor) >> 8) + iFadeMapOffset;
							}
						if (iShadowMode & EShadow)
							{
							r = (Max(0,r-0x40));
							g = (Max(0,g-0x40));
							b = (Max(0,b-0x40));
							}
						}
					if (mask != 0xff)
						{
						// (mask * r + (255 - mask) * value) / 255 =
						// ((257 * mask * (r - value)) >> 16) + value
						TInt value = *pixelPtr & 0xffffff;
						mask = (mask << 8) + mask; // mask = mask * 257
						TInt v = value >> 16;
						r = ((mask * (r - v)) >> 16) + v;
						v = (value >> 8) & 0xff;
						g = ((mask * (g - v)) >> 16) + v;
						v = value & 0xff;
						b = ((mask * (b - v)) >> 16) + v;
						}
					if(aShadowing == MAlphaBlend::EShdwAfter)
						{
						if (iShadowMode & EFade)
							{  
							r = ((r * iFadeMapFactor) >> 8) + iFadeMapOffset;
							g = ((g * iFadeMapFactor) >> 8) + iFadeMapOffset;
							b = ((b * iFadeMapFactor) >> 8) + iFadeMapOffset;            
							}
						if (iShadowMode & EShadow)
							{
							r = (Max(0,r-0x40));
							g = (Max(0,g-0x40));
							b = (Max(0,b-0x40));
							}
						}
					// Convert colour if an incompatible UserDisplayMode is being used
					CDrawBitmap::MapColorToUserDisplayMode(r,g,b);
					*pixelPtr = (r<<16) | (g<<8) | b | 0xff000000;
					}
				pixelPtr += pixelPtrInc;
				srcPtr8 += 4;
				}
			}
		return;
		}
	else   // (aSrcFormat==EColor64K)
		{
		const TUint16* srcPtr16 = reinterpret_cast<const TUint16*>(aSrcPtr) + aSrcX;
		
		if (!(iShadowMode & EFade) && !(iShadowMode & EShadow) && ((iUserDispMode==ENone) || (iUserDispMode==EColor16MU)))
			{			
			const TUint16* lowAdd = Convert16to32bppLow();
			const TUint32* highAdd = Convert16to32bppHigh();
			const TUint32* maskWordPtr = aMaskPtr + (aMaskX >> 2);
			const TInt startBit = aMaskX & 0x3;
			
			if (startBit)
				{
				// Process initial incomplete mask word
				const TUint32 maskWord = *maskWordPtr++;
				TInt numPix = Min(aLength, 4 - startBit);  // number of pixels to process from the first word of the mask
				aLength -= numPix;
				
				if (!maskWord)
					{
					// maskWord is fully masked
					srcPtr16 += numPix;
					pixelPtr += pixelPtrInc * numPix;
					}
				else if (maskWord == 0xFFFFFFFF)
					{
					// maskWord is fully unmasked
					while (numPix--)
						{
						CopyPixel(srcPtr16, pixelPtr, pixelPtrInc, highAdd, lowAdd);
						}
					}
				else
					{
					// At least one of the pixels needs to be blended
					const TUint8* maskPtr8 = reinterpret_cast<const TUint8*>(&maskWord);
					maskPtr8 += startBit;
					while (numPix--)
						{
						ProcessAlphaPixel(srcPtr16, maskPtr8, pixelPtr, pixelPtrInc, highAdd, lowAdd);
						}
					}
				}
			
			TInt numMaskWords = aLength >> 2;
			aLength &= 0x3;
			while (numMaskWords--)
				{
				// Process 4 mask pixels
				const TUint32 maskWord = *maskWordPtr++;
				
				if (!maskWord)
					{
					// maskWord is fully masked - skip 4 pixels
					srcPtr16 += 4;
					pixelPtr += pixelPtrInc << 2;
					}
				else if (maskWord == 0xFFFFFFFF)
					{
					// maskWord is fully unmasked - copy and convert 4 pixels
					const TUint32* srcPtr32 = (const TUint32*)srcPtr16;
					CopyTwoPixels(srcPtr32, pixelPtr, pixelPtrInc, highAdd, lowAdd);
					CopyTwoPixels(srcPtr32, pixelPtr, pixelPtrInc, highAdd, lowAdd);
					srcPtr16 = (const TUint16*)srcPtr32;
					}
				else
					{	
					// At least one of the 4 pixels needs to be blended
					const TUint8* maskPtr8 = reinterpret_cast<const TUint8*>(&maskWord);

					if (maskPtr8[0] && (maskPtr8[0] != 0xFF) &&
						maskPtr8[1] && (maskPtr8[1] != 0xFF) &&
						maskPtr8[2] && (maskPtr8[2] != 0xFF) &&
						maskPtr8[3] && (maskPtr8[3] != 0xFF))
						{
						// Blend all 4 pixels inline
						BlendAlphaPixel(srcPtr16, maskPtr8[0], pixelPtr, pixelPtrInc);
						BlendAlphaPixel(srcPtr16, maskPtr8[1], pixelPtr, pixelPtrInc);
						BlendAlphaPixel(srcPtr16, maskPtr8[2], pixelPtr, pixelPtrInc);
						BlendAlphaPixel(srcPtr16, maskPtr8[3], pixelPtr, pixelPtrInc);
						}
					else
						{
						ProcessAlphaPixel(srcPtr16, maskPtr8, pixelPtr, pixelPtrInc, highAdd, lowAdd);
						ProcessAlphaPixel(srcPtr16, maskPtr8, pixelPtr, pixelPtrInc, highAdd, lowAdd);
						ProcessAlphaPixel(srcPtr16, maskPtr8, pixelPtr, pixelPtrInc, highAdd, lowAdd);
						ProcessAlphaPixel(srcPtr16, maskPtr8, pixelPtr, pixelPtrInc, highAdd, lowAdd);
						}
					}
				}

			if (aLength)
				{
				// Process final incomplete mask word
				const TUint32 maskWord = *maskWordPtr;   // this will over-read
				if (!maskWord)
					{
					// maskWord is fully masked	- skip the pixels
					return;
					}
				if (maskWord == 0xFFFFFFFF)
					{
					// maskWord is fully unmasked - copy and convert the pixels
					while (aLength--)
						{
						CopyPixel(srcPtr16, pixelPtr, pixelPtrInc, highAdd, lowAdd);
						}
					}
				else
					{
					// At least one of the pixels needs to be blended
					const TUint8* maskPtr8 = reinterpret_cast<const TUint8*>(&maskWord);
					while (aLength--)
						{
						ProcessAlphaPixel(srcPtr16, maskPtr8, pixelPtr, pixelPtrInc, highAdd, lowAdd);
						}
					}
				}
			}
		else
			{
			// Non-optimised path including shadowing and UserDisplayMode conversion
			const TUint8* maskPtr8  = reinterpret_cast<const TUint8*>(aMaskPtr) + aMaskX;
			while (aLength--)
				{
				TInt mask = *maskPtr8++;
				if (mask)
					{
					const TUint32 src = *srcPtr16;
					TInt r  = (src & 0xF800) >> 8;
					     r |= (src & 0xE000) >>13;
					TInt g  = (src & 0x07E0) >> 3;
					     g |= (src & 0x0600) >> 9;
					TInt b  = (src & 0x001F) << 3;
					     b |= (src & 0x001C) >> 2;

					if(aShadowing == MAlphaBlend::EShdwBefore)
						{
						if (iShadowMode & EFade)
							{
							r = ((r * iFadeMapFactor) >> 8) + iFadeMapOffset;
							g = ((g * iFadeMapFactor) >> 8) + iFadeMapOffset;
							b = ((b * iFadeMapFactor) >> 8) + iFadeMapOffset;
							}
						if (iShadowMode & EShadow)
							{
							r = Max(0,r-0x40);
							g = Max(0,g-0x40);
							b = Max(0,b-0x40);
							}
						}
					if (mask != 0xff)
						{
						// (mask * r + (255 - mask) * value) / 255 =
						// ((257 * mask * (r - value)) >> 16) + value
						const TInt value = *pixelPtr & 0xffffff;
						mask = (mask << 8) + mask; // mask = mask * 257
						TInt v = value >> 16;
						r = ((mask * (r - v)) >> 16) + v;
						v = (value >> 8) & 0xff;
						g = ((mask * (g - v)) >> 16) + v;
						v = value & 0xff;
						b = ((mask * (b - v)) >> 16) + v;
						}
					if(aShadowing == MAlphaBlend::EShdwAfter)
						{
						if (iShadowMode & EFade)
							{  
							r = ((r * iFadeMapFactor) >> 8) + iFadeMapOffset;
							g = ((g * iFadeMapFactor) >> 8) + iFadeMapOffset;
							b = ((b * iFadeMapFactor) >> 8) + iFadeMapOffset;            
							}
						if (iShadowMode & EShadow)
							{
							r = Max(0,r-0x40);
							g = Max(0,g-0x40);
							b = Max(0,b-0x40);                  
							}
						}
					// Convert colour if an incompatible UserDisplayMode is being used
					CDrawBitmap::MapColorToUserDisplayMode(r,g,b);
					*pixelPtr = (r<<16) | (g<<8) | b | 0xff000000;
					}
				srcPtr16++;
				pixelPtr += pixelPtrInc;
				}
			}
		return;
		}	
	}

/**
CDrawUTwentyFourBppBitmap::WriteMaskLineEx() implementation.
@internalTechnology
@see MFastBlit::WriteMaskLineEx()
*/
void CDrawUTwentyFourBppBitmap::WriteMaskLineEx(TInt aX, TInt aY, TInt aLength, TInt aSrcX,
													const TUint32* aSrcPtr,  TDisplayMode aSrcFormat,
													TInt aMaskX, const TUint32* aMaskPtr, TBool aInvertMask)
	{
	// Only certain pixel formats are supported.  Caller should check this.
	__ASSERT_DEBUG(aSrcFormat ==EColor16MU || aSrcFormat ==EColor64K, User::Invariant());

	DeOrientate(aX,aY);
	TUint32* pixelPtr = PixelAddress(aX,aY);
	const TInt pixelPtrInc = PixelAddressIncrement();
	const TUint32 invertWord = (aInvertMask ? 0xFFFFFFFF : 0);  // to be XORed with the mask

	if (aSrcFormat==EColor16MU)
		{
		aSrcPtr += aSrcX;
	
		if ((iUserDispMode==ENone) || (iUserDispMode==EColor16MU))
			{
			const TUint32* maskWordPtr = aMaskPtr + (aMaskX >> 5);
			const TInt startBit = aMaskX & 0x1F;

			if (startBit)
				{
				// Process initial incomplete mask word
				TUint32 maskWord = *maskWordPtr++;
				maskWord ^= invertWord;
				TInt numPix = Min(aLength, 32 - startBit);  // number of pixels to process from the first word of the mask
				aLength -= numPix;

				if (!maskWord)
					{
					// maskWord is fully masked - skip the pixels
					aSrcPtr += numPix;
					pixelPtr += pixelPtrInc * numPix;
					}
				else if (maskWord == 0xFFFFFFFF)
					{
					// maskWord is fully unmasked - copy the pixels
					while (numPix--)
						{
						*pixelPtr = *aSrcPtr++;
						pixelPtr += pixelPtrInc;
						}
					}
				else
					{
					// maskWord is partially masked - process each pixel
					TUint32 singleBitMask = 1 << startBit;
					while (numPix--)
						{
						ProcessMaskPixel(aSrcPtr, maskWord, singleBitMask, pixelPtr, pixelPtrInc);
						}
					}
				}
			
			TInt numMaskWords = aLength >> 5;
			aLength &= 0x1F;
			while (numMaskWords--)
				{
				// Process a complete mask word (32 pixels)
				TUint32 maskWord = *maskWordPtr++;
				maskWord ^= invertWord;
				
				if (!maskWord)
					{
					// maskWord is fully masked - skip 32 pixels
					aSrcPtr += 32;
					pixelPtr += pixelPtrInc << 5;
					}
				else if (maskWord == 0xFFFFFFFF)
					{
					// maskWord is fully unmasked - copy 32 pixels
					if (pixelPtrInc==1)
						{
						pixelPtr=(TUint32*)Mem::Move(pixelPtr, aSrcPtr, 128);
						aSrcPtr+= 32;
						}
					else
						{
						*pixelPtr = *aSrcPtr++; pixelPtr += pixelPtrInc;
						*pixelPtr = *aSrcPtr++; pixelPtr += pixelPtrInc;
						*pixelPtr = *aSrcPtr++; pixelPtr += pixelPtrInc;
						*pixelPtr = *aSrcPtr++; pixelPtr += pixelPtrInc;
						*pixelPtr = *aSrcPtr++; pixelPtr += pixelPtrInc;
						*pixelPtr = *aSrcPtr++; pixelPtr += pixelPtrInc;
						*pixelPtr = *aSrcPtr++; pixelPtr += pixelPtrInc;
						*pixelPtr = *aSrcPtr++; pixelPtr += pixelPtrInc;
						*pixelPtr = *aSrcPtr++; pixelPtr += pixelPtrInc;
						*pixelPtr = *aSrcPtr++; pixelPtr += pixelPtrInc;
						*pixelPtr = *aSrcPtr++; pixelPtr += pixelPtrInc;
						*pixelPtr = *aSrcPtr++; pixelPtr += pixelPtrInc;
						*pixelPtr = *aSrcPtr++; pixelPtr += pixelPtrInc;
						*pixelPtr = *aSrcPtr++; pixelPtr += pixelPtrInc;
						*pixelPtr = *aSrcPtr++; pixelPtr += pixelPtrInc;
						*pixelPtr = *aSrcPtr++; pixelPtr += pixelPtrInc;
						*pixelPtr = *aSrcPtr++; pixelPtr += pixelPtrInc;
						*pixelPtr = *aSrcPtr++; pixelPtr += pixelPtrInc;
						*pixelPtr = *aSrcPtr++; pixelPtr += pixelPtrInc;
						*pixelPtr = *aSrcPtr++; pixelPtr += pixelPtrInc;
						*pixelPtr = *aSrcPtr++; pixelPtr += pixelPtrInc;
						*pixelPtr = *aSrcPtr++; pixelPtr += pixelPtrInc;
						*pixelPtr = *aSrcPtr++; pixelPtr += pixelPtrInc;
						*pixelPtr = *aSrcPtr++; pixelPtr += pixelPtrInc;
						*pixelPtr = *aSrcPtr++; pixelPtr += pixelPtrInc;
						*pixelPtr = *aSrcPtr++; pixelPtr += pixelPtrInc;
						*pixelPtr = *aSrcPtr++; pixelPtr += pixelPtrInc;
						*pixelPtr = *aSrcPtr++; pixelPtr += pixelPtrInc;
						*pixelPtr = *aSrcPtr++; pixelPtr += pixelPtrInc;
						*pixelPtr = *aSrcPtr++; pixelPtr += pixelPtrInc;
						*pixelPtr = *aSrcPtr++; pixelPtr += pixelPtrInc;
						*pixelPtr = *aSrcPtr++; pixelPtr += pixelPtrInc;
						}
					}
				else
					{	
					// maskWord is partially masked - process each of the 32 pixels
					TUint32 singleBitMask = 1;
					ProcessMaskPixel(aSrcPtr, maskWord, singleBitMask, pixelPtr, pixelPtrInc);
					ProcessMaskPixel(aSrcPtr, maskWord, singleBitMask, pixelPtr, pixelPtrInc);
					ProcessMaskPixel(aSrcPtr, maskWord, singleBitMask, pixelPtr, pixelPtrInc);
					ProcessMaskPixel(aSrcPtr, maskWord, singleBitMask, pixelPtr, pixelPtrInc);
					ProcessMaskPixel(aSrcPtr, maskWord, singleBitMask, pixelPtr, pixelPtrInc);
					ProcessMaskPixel(aSrcPtr, maskWord, singleBitMask, pixelPtr, pixelPtrInc);
					ProcessMaskPixel(aSrcPtr, maskWord, singleBitMask, pixelPtr, pixelPtrInc);
					ProcessMaskPixel(aSrcPtr, maskWord, singleBitMask, pixelPtr, pixelPtrInc);
					ProcessMaskPixel(aSrcPtr, maskWord, singleBitMask, pixelPtr, pixelPtrInc);
					ProcessMaskPixel(aSrcPtr, maskWord, singleBitMask, pixelPtr, pixelPtrInc);
					ProcessMaskPixel(aSrcPtr, maskWord, singleBitMask, pixelPtr, pixelPtrInc);
					ProcessMaskPixel(aSrcPtr, maskWord, singleBitMask, pixelPtr, pixelPtrInc);
					ProcessMaskPixel(aSrcPtr, maskWord, singleBitMask, pixelPtr, pixelPtrInc);
					ProcessMaskPixel(aSrcPtr, maskWord, singleBitMask, pixelPtr, pixelPtrInc);
					ProcessMaskPixel(aSrcPtr, maskWord, singleBitMask, pixelPtr, pixelPtrInc);
					ProcessMaskPixel(aSrcPtr, maskWord, singleBitMask, pixelPtr, pixelPtrInc);
					ProcessMaskPixel(aSrcPtr, maskWord, singleBitMask, pixelPtr, pixelPtrInc);
					ProcessMaskPixel(aSrcPtr, maskWord, singleBitMask, pixelPtr, pixelPtrInc);
					ProcessMaskPixel(aSrcPtr, maskWord, singleBitMask, pixelPtr, pixelPtrInc);
					ProcessMaskPixel(aSrcPtr, maskWord, singleBitMask, pixelPtr, pixelPtrInc);
					ProcessMaskPixel(aSrcPtr, maskWord, singleBitMask, pixelPtr, pixelPtrInc);
					ProcessMaskPixel(aSrcPtr, maskWord, singleBitMask, pixelPtr, pixelPtrInc);
					ProcessMaskPixel(aSrcPtr, maskWord, singleBitMask, pixelPtr, pixelPtrInc);
					ProcessMaskPixel(aSrcPtr, maskWord, singleBitMask, pixelPtr, pixelPtrInc);
					ProcessMaskPixel(aSrcPtr, maskWord, singleBitMask, pixelPtr, pixelPtrInc);
					ProcessMaskPixel(aSrcPtr, maskWord, singleBitMask, pixelPtr, pixelPtrInc);
					ProcessMaskPixel(aSrcPtr, maskWord, singleBitMask, pixelPtr, pixelPtrInc);
					ProcessMaskPixel(aSrcPtr, maskWord, singleBitMask, pixelPtr, pixelPtrInc);
					ProcessMaskPixel(aSrcPtr, maskWord, singleBitMask, pixelPtr, pixelPtrInc);
					ProcessMaskPixel(aSrcPtr, maskWord, singleBitMask, pixelPtr, pixelPtrInc);
					ProcessMaskPixel(aSrcPtr, maskWord, singleBitMask, pixelPtr, pixelPtrInc);
					ProcessMaskPixel(aSrcPtr, maskWord, singleBitMask, pixelPtr, pixelPtrInc);
					}
				}


			if (aLength)
				{
				// Process final incomplete mask word
				TUint32 maskWord = *maskWordPtr;
				maskWord ^= invertWord;
				
				if (!maskWord)
					{
					// maskWord is fully masked - skip the pixels
					return;
					}
				if (maskWord == 0xFFFFFFFF)
					{
					// maskWord is fully unmasked - copy the pixels
					while (aLength--)
						{
						*pixelPtr = *aSrcPtr++;
						pixelPtr += pixelPtrInc;
						}
					}
				else
					{
					// maskWord is partially masked - process each pixel
					TUint32 singleBitMask = 1;
					while (aLength--)
						{
						ProcessMaskPixel(aSrcPtr, maskWord, singleBitMask, pixelPtr, pixelPtrInc);
						}
					}
				}
			}
		else
			{
			// Non-optimised path.  UserDisplay mode is different to the true display mode
			while (aLength--)
				{
				TUint32 mask = *(aMaskPtr + (aMaskX >> 5)) & (1 << (aMaskX & 0x1F));
				if (aInvertMask)
					{
					mask = !mask;
					}
				if (mask)
					{
					TRgb pixel(*aSrcPtr);
					MapColorToUserDisplayMode(pixel);
					*pixelPtr = pixel.Value();
					}
				aSrcPtr++;
				aMaskX++;
				pixelPtr += pixelPtrInc;
				}
			}
		return;
		}
	else  // (aSrcFormat==EColor64K)
		{
		const TUint16* srcPtr16 = reinterpret_cast<const TUint16*>(aSrcPtr) + aSrcX;
	
		if ((iUserDispMode==ENone) || (iUserDispMode==EColor16MU))
			{
			const TUint16* lowAdd = Convert16to32bppLow();
			const TUint32* highAdd = Convert16to32bppHigh();
			const TUint32* maskWordPtr = aMaskPtr + (aMaskX >> 5);
			const TInt startBit = aMaskX & 0x1F;
			
			if (startBit)
				{
				// Process initial incomplete mask word
				TUint32 maskWord = *maskWordPtr++;
				maskWord ^= invertWord;
				TInt numPix = Min(aLength, 32 - startBit);  // number of pixels to process from the first word of the mask
				aLength -= numPix;
				
				if (!maskWord)
					{
					// maskWord is fully masked	- skip the pixels
					srcPtr16 += numPix;
					pixelPtr += pixelPtrInc * numPix;
					}
				else if (maskWord == 0xFFFFFFFF)
					{
					// maskWord is fully unmasked - copy and convert the pixels
					while (numPix--)
						{
						CopyPixel(srcPtr16, pixelPtr, pixelPtrInc, highAdd, lowAdd);
						}
					}
				else
					{
					// maskWord is partially masked - process each of the pixels
					TUint32 singleBitMask = 1 << startBit;
					while (numPix--)
						{
						ProcessMaskPixel(srcPtr16, maskWord, singleBitMask, pixelPtr, pixelPtrInc, highAdd, lowAdd);
						}
					}
				}
			
			TInt numMaskWords = aLength >> 5;
			aLength &= 0x1F;
			while (numMaskWords--)
				{
				// Process complete mask words
				TUint32 maskWord = *maskWordPtr++;
				maskWord ^= invertWord;
				
				if (!maskWord)
					{
					// maskWord is fully masked - skip 32 pixels
					srcPtr16 += 32;
					pixelPtr += pixelPtrInc << 5;
					}
				else if (maskWord == 0xFFFFFFFF)
					{
					// maskWord is fully unmasked - copy and convert 32 pixels
					const TUint32* srcPtr32 = (const TUint32*)srcPtr16;
					CopyTwoPixels(srcPtr32, pixelPtr, pixelPtrInc, highAdd, lowAdd);
					CopyTwoPixels(srcPtr32, pixelPtr, pixelPtrInc, highAdd, lowAdd);
					CopyTwoPixels(srcPtr32, pixelPtr, pixelPtrInc, highAdd, lowAdd);
					CopyTwoPixels(srcPtr32, pixelPtr, pixelPtrInc, highAdd, lowAdd);
					CopyTwoPixels(srcPtr32, pixelPtr, pixelPtrInc, highAdd, lowAdd);
					CopyTwoPixels(srcPtr32, pixelPtr, pixelPtrInc, highAdd, lowAdd);
					CopyTwoPixels(srcPtr32, pixelPtr, pixelPtrInc, highAdd, lowAdd);
					CopyTwoPixels(srcPtr32, pixelPtr, pixelPtrInc, highAdd, lowAdd);
					CopyTwoPixels(srcPtr32, pixelPtr, pixelPtrInc, highAdd, lowAdd);
					CopyTwoPixels(srcPtr32, pixelPtr, pixelPtrInc, highAdd, lowAdd);
					CopyTwoPixels(srcPtr32, pixelPtr, pixelPtrInc, highAdd, lowAdd);
					CopyTwoPixels(srcPtr32, pixelPtr, pixelPtrInc, highAdd, lowAdd);
					CopyTwoPixels(srcPtr32, pixelPtr, pixelPtrInc, highAdd, lowAdd);
					CopyTwoPixels(srcPtr32, pixelPtr, pixelPtrInc, highAdd, lowAdd);
					CopyTwoPixels(srcPtr32, pixelPtr, pixelPtrInc, highAdd, lowAdd);
					CopyTwoPixels(srcPtr32, pixelPtr, pixelPtrInc, highAdd, lowAdd);
					srcPtr16 = (const TUint16*)srcPtr32;
					}
				else
					{
					// maskWord is partially masked - process each of the 32 pixels					
					TUint32 singleBitMask = 1;
					ProcessMaskPixel(srcPtr16, maskWord, singleBitMask, pixelPtr, pixelPtrInc, highAdd, lowAdd);
					ProcessMaskPixel(srcPtr16, maskWord, singleBitMask, pixelPtr, pixelPtrInc, highAdd, lowAdd);
					ProcessMaskPixel(srcPtr16, maskWord, singleBitMask, pixelPtr, pixelPtrInc, highAdd, lowAdd);
					ProcessMaskPixel(srcPtr16, maskWord, singleBitMask, pixelPtr, pixelPtrInc, highAdd, lowAdd);
					ProcessMaskPixel(srcPtr16, maskWord, singleBitMask, pixelPtr, pixelPtrInc, highAdd, lowAdd);
					ProcessMaskPixel(srcPtr16, maskWord, singleBitMask, pixelPtr, pixelPtrInc, highAdd, lowAdd);
					ProcessMaskPixel(srcPtr16, maskWord, singleBitMask, pixelPtr, pixelPtrInc, highAdd, lowAdd);
					ProcessMaskPixel(srcPtr16, maskWord, singleBitMask, pixelPtr, pixelPtrInc, highAdd, lowAdd);
					ProcessMaskPixel(srcPtr16, maskWord, singleBitMask, pixelPtr, pixelPtrInc, highAdd, lowAdd);
					ProcessMaskPixel(srcPtr16, maskWord, singleBitMask, pixelPtr, pixelPtrInc, highAdd, lowAdd);
					ProcessMaskPixel(srcPtr16, maskWord, singleBitMask, pixelPtr, pixelPtrInc, highAdd, lowAdd);
					ProcessMaskPixel(srcPtr16, maskWord, singleBitMask, pixelPtr, pixelPtrInc, highAdd, lowAdd);
					ProcessMaskPixel(srcPtr16, maskWord, singleBitMask, pixelPtr, pixelPtrInc, highAdd, lowAdd);
					ProcessMaskPixel(srcPtr16, maskWord, singleBitMask, pixelPtr, pixelPtrInc, highAdd, lowAdd);
					ProcessMaskPixel(srcPtr16, maskWord, singleBitMask, pixelPtr, pixelPtrInc, highAdd, lowAdd);
					ProcessMaskPixel(srcPtr16, maskWord, singleBitMask, pixelPtr, pixelPtrInc, highAdd, lowAdd);
					ProcessMaskPixel(srcPtr16, maskWord, singleBitMask, pixelPtr, pixelPtrInc, highAdd, lowAdd);
					ProcessMaskPixel(srcPtr16, maskWord, singleBitMask, pixelPtr, pixelPtrInc, highAdd, lowAdd);
					ProcessMaskPixel(srcPtr16, maskWord, singleBitMask, pixelPtr, pixelPtrInc, highAdd, lowAdd);
					ProcessMaskPixel(srcPtr16, maskWord, singleBitMask, pixelPtr, pixelPtrInc, highAdd, lowAdd);
					ProcessMaskPixel(srcPtr16, maskWord, singleBitMask, pixelPtr, pixelPtrInc, highAdd, lowAdd);
					ProcessMaskPixel(srcPtr16, maskWord, singleBitMask, pixelPtr, pixelPtrInc, highAdd, lowAdd);
					ProcessMaskPixel(srcPtr16, maskWord, singleBitMask, pixelPtr, pixelPtrInc, highAdd, lowAdd);
					ProcessMaskPixel(srcPtr16, maskWord, singleBitMask, pixelPtr, pixelPtrInc, highAdd, lowAdd);
					ProcessMaskPixel(srcPtr16, maskWord, singleBitMask, pixelPtr, pixelPtrInc, highAdd, lowAdd);
					ProcessMaskPixel(srcPtr16, maskWord, singleBitMask, pixelPtr, pixelPtrInc, highAdd, lowAdd);
					ProcessMaskPixel(srcPtr16, maskWord, singleBitMask, pixelPtr, pixelPtrInc, highAdd, lowAdd);
					ProcessMaskPixel(srcPtr16, maskWord, singleBitMask, pixelPtr, pixelPtrInc, highAdd, lowAdd);
					ProcessMaskPixel(srcPtr16, maskWord, singleBitMask, pixelPtr, pixelPtrInc, highAdd, lowAdd);
					ProcessMaskPixel(srcPtr16, maskWord, singleBitMask, pixelPtr, pixelPtrInc, highAdd, lowAdd);
					ProcessMaskPixel(srcPtr16, maskWord, singleBitMask, pixelPtr, pixelPtrInc, highAdd, lowAdd);
					ProcessMaskPixel(srcPtr16, maskWord, singleBitMask, pixelPtr, pixelPtrInc, highAdd, lowAdd);
					}
				}
			
			if (aLength)
				{
				// Process final incomplete mask word
				TUint32 maskWord = *maskWordPtr;		// this will over-read
				maskWord ^= invertWord;
				
				if (!maskWord)
					{
					// maskWord is masked - skip the pixels
					return;
					}
					
				if (maskWord == 0xFFFFFFFF)
					{
					// maskWord is fully unmasked - copy and convert the pixels
					while (aLength--)
						{
						CopyPixel(srcPtr16, pixelPtr, pixelPtrInc, highAdd, lowAdd);
						}
					}
				else
					{
					// maskWord is partially masked - process each of the pixels
					TUint32 singleBitMask = 1;
					while (aLength--)
						{
						ProcessMaskPixel(srcPtr16, maskWord, singleBitMask, pixelPtr, pixelPtrInc, highAdd, lowAdd);
						}
					}
				}
			}
		else
			{
			// Non-optimised - expects aMaskPtr untouched and srcPtr16 to be accurate
			while (aLength--)
				{
				TUint32 mask = *(aMaskPtr + (aMaskX >> 5)) & (1 << (aMaskX & 0x1F));
				if (aInvertMask)
					{
					mask = !mask;
					}
				if (mask)
					{
					const TUint32 src = *srcPtr16;
					TUint32 color = (src & 0xF800) << 8; // R top 5
					color |= (src & 0xE000) << 3; // R bottom 3 
					color |= (src & 0x07E0) << 5; // G top 6
					color |= (src & 0x0600) >> 1; // G bottom 2
					color |= (src & 0x001F) << 3; // B top 5
					color |= (src & 0x001C) >> 2; // B bottom 3
					TRgb pixel(color);
					MapColorToUserDisplayMode(pixel);
					*pixelPtr = pixel.Value();
					}
				pixelPtr += pixelPtrInc;
				srcPtr16++;
				aMaskX++;
				}
			}
		return;
		}
	}

/**
Implementation for CFbsDrawDevice::GetInterface().
Retrieves a pointer to a specified interface of CFbsDrawDevice implementation.
@param aInterfaceId Interface identifier of the interface to be retrieved.
@param aInterface Address of variable that retrieves the specified interface.
@return KErrNone If the interface is supported, KErrNotSupported otherwise.
*/
TInt CDrawUTwentyFourBppBitmap::GetInterface(TInt aInterfaceId, TAny*& aInterface)
	{
	aInterface = NULL;
	TInt ret = KErrNotSupported;
	
	switch (aInterfaceId)
		{
		case KFastBlitInterfaceID:
			{
			aInterface = static_cast<MFastBlit*>(this);
			ret = KErrNone;
			break;
			}
		default:
			{
			return CDrawThirtyTwoBppBitmapCommon::GetInterface(aInterfaceId, aInterface);
			}
		}
		
	return ret;
	}

TInt CDrawUTwentyFourBppBitmap::WriteRgbOutlineAndShadow(TInt aX, TInt aY, const TInt aLength,
														TUint32 aOutlinePenColor, TUint32 aShadowColor,
														TUint32 aFillColor, const TUint8* aDataBuffer)
	{
	DeOrientate(aX,aY);
	TUint32* pixelPtr = PixelAddress(aX,aY);
	const TInt pixelPtrInc = PixelAddressIncrement();
	const TUint8* dataBufferPtrLimit = aDataBuffer + aLength;
	TInt blendedRedColor;
	TInt blendedGreenColor;
	TInt blendedBlueColor;
	TInt blendedAlpha;
	TUint8 index = 0;
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
	const TInt alphaOutlinePenColor = aOutlinePenColor >> 24;
	const TInt alphaShadowColor = aShadowColor >> 24;
	const TInt alphaFillColor = aFillColor >> 24;

	while (aDataBuffer < dataBufferPtrLimit)
		{
		index = *aDataBuffer++;
		if (255 == FourColorBlendLookup[index][KBackgroundColorIndex])
			{
			//background colour
			//No drawing required
			}
		else if (255 == FourColorBlendLookup[index][KFillColorIndex])
			{
			//Use fill colour to draw
			finalColor = aFillColor;
			AlphaBlendPixelToDest((finalColor | 0xff000000), alphaFillColor, pixelPtr);
			}
		else if (255 == FourColorBlendLookup[index][KShadowColorIndex])
			{
			//Use shadow colour to draw
			finalColor = aShadowColor;
			AlphaBlendPixelToDest((finalColor | 0xff000000), alphaShadowColor, pixelPtr);
			}
		else if (255 == FourColorBlendLookup[index][KOutlineColorIndex])
			{
			//Use outline colour to draw
			finalColor = aOutlinePenColor;
			AlphaBlendPixelToDest((finalColor | 0xff000000), alphaOutlinePenColor, pixelPtr);
			}
		else
			{
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
			AlphaBlendPixelToDest(finalColor | 0xff000000, blendedAlpha, pixelPtr);
			}
		pixelPtr += pixelPtrInc;
		}
	return KErrNone;
	}
