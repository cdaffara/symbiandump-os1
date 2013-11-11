// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "Accelhard.h"

// Windows headers...
#define UNICODE
#pragma warning( disable : 4201 )
#define WIN32_LEAN_AND_MEAN
#define NOSERVICE
#include <windows.h>
#include <windowsx.h>
#pragma warning( default : 4201 )

#include "_WININC.H"

LOCAL_D const TGraphicsAcceleratorCaps HardwareAcceleratorCapabilities = 
	{
	sizeof(TGraphicsAcceleratorCaps),							//TInt iStructureSize;	// The size of this class
	1,															//TInt iVersion;		// == 1 to specify current API
	0,															//Tint iVendorUid;		// Optional ID
	1<<EColor256,												//iDisplayModes;
	0,															//iClipping;
	TGraphicsAcceleratorCaps::EMaskBitmapMatchingDisplayMode,	//iMaskType;
	0,															//iTransparency;
	0,															//iAlphaChannel;
	TGraphicsAcceleratorCaps::EAlphaBitmapGray256,				//iAlphaBitmap;
	(TUint)TGraphicsAcceleratorCaps::EPatternSizeAny,			//iPatternSizes;
	TGraphicsAcceleratorCaps::EPatternMatchingDisplayMode,		//iPattern;
	0,															//iPolygon;
	{TGraphicsAcceleratorCaps::EOrientationCapNormal,0,0,0}		//iReserved[0] is used for supported orientations, iReserved[1]-iReserved[3] are reserved;
	};

//
// CHardwareGraphicsAccelerator
//

/** 
Allocates and constructs an instance of a derived class and initialises its 
capabilities.

@param aBitmap The hardware bitmap for the accelerator to draw to.
@return Pointer to the initialised graphics accelerator object.
@see TGraphicsAcceleratorCaps::iDisplayModes 
@leave KErrNoMemory There is no memory to allocate the graphics accelerator
       KErrNotSupported There is no graphics accelerator supporting the display mode given by the bitmap parameter
*/
EXPORT_C CHardwareGraphicsAccelerator* CHardwareGraphicsAccelerator::NewL(RHardwareBitmap aBitmap)
	{
	TAcceleratedBitmapInfo hwBitmapInfo;
	aBitmap.GetInfo(hwBitmapInfo);

	switch(hwBitmapInfo.iDisplayMode)
		{
		case EColor256:
			return CColor256HardwareGraphicsAccelerator::NewL(aBitmap);
		default:
			User::Leave(KErrNotSupported);
			return NULL;
		}
	}

/** 
Gets the generic capabilities of the accelerator, including which display modes 
are supported for the bitmap passed to NewL().

Generic capabilities apply to all hardware graphics accelerators on the device. 
The function is static, so it can be used to find out the capabilities of 
graphics accelerators before deciding on whether or not to create one.

The CGraphicsAccelerator::Capabilities() function provides information about 
the capabilities of a particular graphics accelerator.

As capabilities may vary depending on the display mode of a bitmap, this method 
should indicate as supported any features which are only available in some 
display modes.
@return Generic capabilities for hardware graphics accelerators. 
*/
EXPORT_C const TGraphicsAcceleratorCaps* CHardwareGraphicsAccelerator::GenericCapabilities()
	{
	return &HardwareAcceleratorCapabilities;
	}

//
// CColor256HardwareGraphicsAccelerator
//

CColor256HardwareGraphicsAccelerator* CColor256HardwareGraphicsAccelerator::NewL(RHardwareBitmap aBitmap)
	{
	CColor256HardwareGraphicsAccelerator* self = new (ELeave) CColor256HardwareGraphicsAccelerator;
	CleanupStack::PushL(self);
	self->ConstructL(aBitmap);
	CleanupStack::Pop();
	return(self);
	}

void CColor256HardwareGraphicsAccelerator::ConstructL(RHardwareBitmap aBitmap)
	{
	CHardwareGraphicsAcceleratorBase::ConstructL(aBitmap);
	iCaps = &HardwareAcceleratorCapabilities; // We only do Color256
 	//Some explanations about "-aBitmap.iHandle - 1" expression.
    //Before "Multiple screens" update aBitmap.iHandle with value -1 meant that the related
    //RHardwareBitmap object is a screen bitmap. Positive aBitmap.iHandle values - it was
    //an in-memory RHardwareBitmap object.
    //After "Multiple screens" update, the meaning of aBitmap.iHandle's value is:
    // - Positive value - in-memory hardware bitmap;
    // - Negative value - screen number: "-1" - screen 0, "-2" - screen 1, "-3" - screen 2, ...;
    if (aBitmap.iHandle < 0)
    	{
    	iScreenNo = -aBitmap.iHandle - 1;	
	    }
  	}

TUint32 CColor256HardwareGraphicsAccelerator::FillValue(TRgb aColor)
	{
	TUint32 value = aColor.Color256();
	value |= value<<8;
	value |= value<<16;
	return value;
	}

void CColor256HardwareGraphicsAccelerator::DoUpdateScreen(const TRect& aRect)
	{
	TInt lx = aRect.iTl.iX & ~3;
	TInt rx = (aRect.iBr.iX + 3) & ~3;

	TUint8* srcePtr = iBitmapInfo.iAddress+iBitmapInfo.iLinePitch*aRect.iTl.iY+lx;
	TUint8* srcePtrLimit = srcePtr + rx - lx;

	TInt linePitch = iBitmapInfo.iLinePitch;

    RWindows* window = ::WindowHandler(iScreenNo);
	for(TInt row = aRect.iTl.iY; row < aRect.iBr.iY; row++)
		{
		TUint8* tempSrcePtr = srcePtr;
		TUint8* destPixel = window->PixelAddress(lx,row);
		TRgb pixelColor;

		if(window->iPalette)
			{
			while (tempSrcePtr < srcePtrLimit)
				{
				pixelColor = window->iPalette[*tempSrcePtr++];
				destPixel[0] = TUint8(pixelColor.Blue());
				destPixel[1] = TUint8(pixelColor.Green());
				destPixel[2] = TUint8(pixelColor.Red());
				destPixel += 3;
				}
			}
		else
			{
			while (tempSrcePtr < srcePtrLimit)
				{
				pixelColor = TRgb::Color256(*tempSrcePtr++);
				destPixel[0] = TUint8(pixelColor.Blue());
				destPixel[1] = TUint8(pixelColor.Green());
				destPixel[2] = TUint8(pixelColor.Red());
				destPixel += 3;
				}
			}

		srcePtr += linePitch;
		srcePtrLimit += linePitch;
		}

	RRegion region;
	region.AddRect(aRect);
	window->Update(region,window->iEpocBitmapSize);
	region.Close();
	}

void CColor256HardwareGraphicsAccelerator::DoFillRect(const TRect& aRect,TUint32 aFillValue)
	{
	//Lock();

	TUint32* ptr = (TUint32*)iBitmapInfo.iAddress;
	TUint32* ptrLimit;
	TUint32 leftMask;
	TUint32 rightMask;
	TInt ptrPitch;
	TInt pitch;
	TInt height;

	{
	TInt yStart = aRect.iTl.iY;
	TInt yEnd = aRect.iBr.iY;

	if(yStart<0)
		goto error;
	if(yStart>=yEnd)
		goto error;
	if(yEnd>iBitmapInfo.iSize.iHeight)
		goto error;

	ptr = (TUint32*)((TUint8*)ptr+yStart*iBitmapInfo.iLinePitch);
	height = yEnd-yStart;
	}

	{
	TInt xStart = aRect.iTl.iX;
	TInt xEnd = aRect.iBr.iX;

	if(xStart<0)
		goto error;
	if(xStart>=xEnd)
		goto error;
	if(xEnd>iBitmapInfo.iSize.iWidth)
		goto error;

	xStart <<= iBitmapInfo.iPixelShift;
	xEnd <<= iBitmapInfo.iPixelShift;

	leftMask = ~((1<<(xStart&0x1F))-1);
	rightMask = (1<<(xEnd&0x1F))-1;

	xStart >>= 5;
	xEnd = (xEnd-1)>>5;
	ptrLimit = ptr+xEnd;
	ptr += xStart;

	pitch = iBitmapInfo.iLinePitch;
	ptrPitch = pitch-((TInt)ptrLimit-(TInt)ptr);
	}

	if((ptr==ptrLimit) && (leftMask!=0xFFFFFFFF)) // all pixels in a line are in the same word
		{
		if((leftMask != 0) && (rightMask !=0))
			leftMask &= rightMask;
		else if(rightMask != 0)
				leftMask = rightMask;

		aFillValue &= leftMask;
		do
			{
			TUint32 pixels = *ptr;
			pixels &= ~ leftMask;
			pixels |= aFillValue;
			*(ptr) = pixels;
			ptr = (TUint32*)((TUint8*)ptr+ptrPitch);
			}
		while(--height);
		}
	else if((leftMask==0xFFFFFFFF)&&(rightMask==0)) // all pixels in a line fit into whole words
		{
		do
			{
			while(ptr<ptrLimit)
				*(ptr++) = aFillValue;
			*ptr = aFillValue;

			ptr = (TUint32*)((TUint8*)ptr+ptrPitch);
			ptrLimit = (TUint32*)((TUint8*)ptrLimit+pitch);
			}
		while(--height);
		}
	else if(leftMask==0xFFFFFFFF) // first pixel starts on a word boundary but last doesn't
		{
		do
			{
			while(ptr<ptrLimit) 
				*(ptr++) = aFillValue;

			TUint32 pixels = *ptr;
			pixels &= ~ rightMask;
			pixels |= aFillValue&rightMask;
			*ptr = pixels;

			ptr = (TUint32*)((TUint8*)ptr+ptrPitch);
			ptrLimit = (TUint32*)((TUint8*)ptrLimit+pitch);
			}
		while(--height);
		}
	else if(rightMask==0) // last pixel lies on a word boundary but first doesn't
		{
		do
			{
			TUint32 pixels = *ptr;
			pixels &= ~ leftMask;
			pixels |= aFillValue&leftMask;
			*(ptr++) = pixels;

			while(ptr<ptrLimit)
				*(ptr++) = aFillValue;
			*ptr = aFillValue;

			ptr = (TUint32*)((TUint8*)ptr+ptrPitch);
			ptrLimit = (TUint32*)((TUint8*)ptrLimit+pitch);
			}
		while(--height);
		}
	else // both last and first pixels in a line don't lie on a word boundary
		{
		do
			{
			TUint32 pixels = *ptr;
			pixels &= ~ leftMask;
			pixels |= aFillValue&leftMask;
			*(ptr++) = pixels;

			while(ptr<ptrLimit)
				*(ptr++) = aFillValue;

			pixels = *ptr;
			pixels &= ~ rightMask;
			pixels |= aFillValue&rightMask;
			*ptr = pixels;

			ptr = (TUint32*)((TUint8*)ptr+ptrPitch);
			ptrLimit = (TUint32*)((TUint8*)ptrLimit+pitch);
			}
		while(--height);
		}

	//Unlock();
	return;
error:
//	Unlock();
	GraphicsAcceleratorPanic(EGraphicsAcceleratorAttemptedDrawOutsideBitmap);
	}

TInt CColor256HardwareGraphicsAccelerator::DoFilledRectWithPattern(const TRect& aRect,TGopFillPattern aPattern)
	{
	if(aPattern.iBitmap.Type() != TAcceleratedBitmapSpec::EHardwareBitmap)
		return KErrNotSupported;

	TInt handle = aPattern.iBitmap.Handle();
	RHardwareBitmap aBitmap(handle);

	TAcceleratedBitmapInfo patternBitmapInfo;
	aBitmap.GetInfo(patternBitmapInfo);

    if(patternBitmapInfo.iDisplayMode != EColor256)
		return KErrNotSupported;

	//point to the first pixel of a row
	TUint8* ptrPattern8 = (TUint8*)patternBitmapInfo.iAddress;
	//point to the last pixels of a row
	TUint8* ptrLimitPattern8 = ptrPattern8 + (patternBitmapInfo.iSize.iWidth - 1);
	//current pixel to copy
	TUint8* ptrCurrentPixelPattern8 = ptrPattern8;
	TInt pitchPattern = patternBitmapInfo.iLinePitch;
	//point to the last pixel of the pattern bitmap
    TUint8* ptrEndPattern8 = ptrLimitPattern8 + pitchPattern * (patternBitmapInfo.iSize.iHeight - 1);
	TInt xOffsetPattern = 0;
	TInt difOrigins = 0;

	TUint8* ptr8 = (TUint8*)iBitmapInfo.iAddress;
	TUint8* ptrLimit8;
	TInt ptrPitch;
	TInt pitch;
	TInt height;

	{
	TInt yStart = aRect.iTl.iY;
	TInt yEnd = aRect.iBr.iY;

	if(yStart<0)
		goto error;
	if(yStart>=yEnd)
		goto error;
	if(yEnd>iBitmapInfo.iSize.iHeight)
		goto error;

	ptr8 = ptr8+yStart*iBitmapInfo.iLinePitch;
	height = yEnd-yStart;
	}

	{
	TInt xStart = aRect.iTl.iX;
	TInt xEnd = aRect.iBr.iX;

	if(xStart<0)
		goto error;
	if(xStart>=xEnd)
		goto error;
	if(xEnd>iBitmapInfo.iSize.iWidth)
		goto error;

	ptrLimit8 = ptr8 + xEnd;
	ptr8 += xStart;

	pitch = iBitmapInfo.iLinePitch;
	ptrPitch = pitch-((TInt)ptrLimit8-(TInt)ptr8);
	}
		
	difOrigins = (aRect.iTl.iX - aPattern.iOrigin.iX) % patternBitmapInfo.iSize.iWidth;
    if(difOrigins < 0)
		{
		ptrCurrentPixelPattern8 = ptrLimitPattern8 + difOrigins + 1;
		xOffsetPattern = patternBitmapInfo.iSize.iWidth + difOrigins;
		}
	if(difOrigins > 0)
		{
		ptrCurrentPixelPattern8 = ptrPattern8 + difOrigins;
		xOffsetPattern = difOrigins;
		}

	difOrigins = (aRect.iTl.iY - aPattern.iOrigin.iY) % patternBitmapInfo.iSize.iHeight;
    if(difOrigins < 0)
		{
		TInt totalPitch = pitchPattern*(patternBitmapInfo.iSize.iHeight + difOrigins);
		ptrCurrentPixelPattern8 += totalPitch;
		ptrPattern8 += totalPitch ;
		ptrLimitPattern8 += totalPitch;
		}
	if(difOrigins > 0)
		{
		TInt totalPitch = (pitchPattern * (difOrigins));
		ptrCurrentPixelPattern8 += totalPitch;
		ptrPattern8 += totalPitch;
		ptrLimitPattern8 += totalPitch;
		}

	do
		{
		while(ptr8<ptrLimit8)
			{
			if(ptrCurrentPixelPattern8 == ptrLimitPattern8)
				{
				*ptr8 = *ptrCurrentPixelPattern8;
				ptr8++;
				ptrCurrentPixelPattern8 = ptrPattern8;
				}
			else
				*(ptr8++) = *(ptrCurrentPixelPattern8++);
			} 
			//check if on the last row
			if(ptrLimitPattern8 == ptrEndPattern8)
				{
				ptrPattern8 = (TUint8*)patternBitmapInfo.iAddress;
				ptrLimitPattern8 = ptrPattern8 + patternBitmapInfo.iSize.iWidth - 1;
                ptrCurrentPixelPattern8 = ptrPattern8 + xOffsetPattern;
				}
			else
				{
				ptrPattern8 += pitchPattern;
				ptrLimitPattern8 += pitchPattern;
                ptrCurrentPixelPattern8 = ptrPattern8 + xOffsetPattern;
				}

		ptr8 = ptr8+ptrPitch;
		ptrLimit8= ptrLimit8+pitch;
		}
	while(--height);

	return KErrNone;
error:
	GraphicsAcceleratorPanic(EGraphicsAcceleratorAttemptedDrawOutsideBitmap);
	return KErrNone; // Will not be executed but prevents compiler warnings
	}

void CColor256HardwareGraphicsAccelerator::DoInvertRect(const TRect& aRect)
	{
	//Lock();

	TUint32* ptr = (TUint32*)iBitmapInfo.iAddress;
	TUint32* ptrLimit;
	TUint32 leftMask;
	TUint32 rightMask;
	TInt ptrPitch;
	TInt pitch;
	TInt height;

	{
	TInt yStart = aRect.iTl.iY;
	TInt yEnd = aRect.iBr.iY;

	if(yStart<0)
		goto error;
	if(yStart>=yEnd)
		goto error;
	if(yEnd>iBitmapInfo.iSize.iHeight)
		goto error;

	ptr = (TUint32*)((TUint8*)ptr+yStart*iBitmapInfo.iLinePitch);
	height = yEnd-yStart;
	}

	{
	TInt xStart = aRect.iTl.iX;
	TInt xEnd = aRect.iBr.iX;

	if(xStart<0)
		goto error;
	if(xStart>=xEnd)
		goto error;
	if(xEnd>iBitmapInfo.iSize.iWidth)
		goto error;

	xStart <<= iBitmapInfo.iPixelShift;
	xEnd <<= iBitmapInfo.iPixelShift;

	leftMask = ~((1<<(xStart&0x1F))-1);
	rightMask = (1<<(xEnd&0x1F))-1;

	xStart >>= 5;
	xEnd = (xEnd-1)>>5;
	ptrLimit = ptr+xEnd;
	ptr += xStart;

	pitch = iBitmapInfo.iLinePitch;
	ptrPitch = pitch-((TInt)ptrLimit-(TInt)ptr);
	}

	TUint32 pixelsInverted;
	if((ptr==ptrLimit) && (leftMask!=0xFFFFFFFF)) // all pixels in a line are in the same word
		{
		if((leftMask != 0) && (rightMask !=0))
			leftMask &= rightMask;
		else if(rightMask != 0)
				leftMask = rightMask;

		do
			{
			TUint32 pixels = *ptr;
			pixelsInverted = ~ pixels;

			pixels &= ~ leftMask;
			pixelsInverted &= leftMask;

			pixels |= pixelsInverted;
			*(ptr) = pixels;
			ptr = (TUint32*)((TUint8*)ptr+ptrPitch);
			}
		while(--height);
		}
	else if((leftMask==0xFFFFFFFF)&&(rightMask==0)) // all pixels in a line fit into whole words
		{
		do
			{
			while(ptr<ptrLimit)
				{
				*(ptr++) = ~(*(ptr)) ;
				}
			*ptr = ~(*(ptr));

			ptr = (TUint32*)((TUint8*)ptr+ptrPitch);
			ptrLimit = (TUint32*)((TUint8*)ptrLimit+pitch);
			}
		while(--height);
		}
	else if(leftMask==0xFFFFFFFF) // first pixel starts on a word boundary but last doesn't
		{
		do
			{
			while(ptr<ptrLimit)
				{
				*(ptr++) = ~(*(ptr)) ;
				}

			TUint32 pixels = *ptr;
			pixelsInverted = ~pixels;
			pixels &= ~ rightMask;
			pixels |= (pixelsInverted & rightMask);
			*ptr = pixels;

			ptr = (TUint32*)((TUint8*)ptr+ptrPitch);
			ptrLimit = (TUint32*)((TUint8*)ptrLimit+pitch);
			}
		while(--height);
		}
	else if(rightMask==0) // last pixel lies on a word boundary but first doesn't
		{
		do
			{
			TUint32 pixels = *ptr;
			pixelsInverted = ~pixels;
			pixels &= ~ leftMask;
			pixels |= (pixelsInverted & leftMask);
			*(ptr++) = pixels;

			while(ptr<ptrLimit)
				{
				*(ptr++) = ~(*(ptr)) ;
				}
			*ptr = ~(*(ptr));

			ptr = (TUint32*)((TUint8*)ptr+ptrPitch);
			ptrLimit = (TUint32*)((TUint8*)ptrLimit+pitch);
			}
		while(--height);
		}
	else // both last and first pixels in a line don't lie on a word boundary
		{
		do
			{
			TUint32 pixels = *ptr;
			pixelsInverted = ~pixels;
			pixels &= ~ leftMask;
			pixels |= (pixelsInverted & leftMask);
			*(ptr++) = pixels;

			while(ptr<ptrLimit)
				{
				*(ptr++) = ~(*(ptr));
				}

			pixels = *ptr;
			pixelsInverted = ~pixels;
			pixels &= ~ rightMask;
			pixels |= (pixelsInverted & rightMask);
			*ptr = pixels;

			ptr = (TUint32*)((TUint8*)ptr+ptrPitch);
			ptrLimit = (TUint32*)((TUint8*)ptrLimit+pitch);
			}
		while(--height);
		}

	return;
error:
	GraphicsAcceleratorPanic(EGraphicsAcceleratorAttemptedDrawOutsideBitmap);
	}

void CColor256HardwareGraphicsAccelerator::DoFadeRect(const TRect& aRect,const TGopFadeParams aFade)
	{

	TUint8* ptr8 = (TUint8*)iBitmapInfo.iAddress;
	TUint8* prtLimit8;
	TInt ptrPitch;
	TInt pitch;
	TInt height;

	TInt scale = aFade.iScale;
	TInt offset = aFade.iOffset;

	TRgb color;

	{
	TInt yStart = aRect.iTl.iY;
	TInt yEnd = aRect.iBr.iY;

	if(yStart<0)
		goto error;
	if(yStart>=yEnd)
		goto error;
	if(yEnd>iBitmapInfo.iSize.iHeight)
		goto error;

	ptr8 = (ptr8 + yStart*iBitmapInfo.iLinePitch);
	height = yEnd-yStart;
	}

	{
	TInt xStart = aRect.iTl.iX;
	TInt xEnd = aRect.iBr.iX;

	if(xStart<0)
		goto error;
	if(xStart>=xEnd)
		goto error;
	if(xEnd>iBitmapInfo.iSize.iWidth)
		goto error;

	prtLimit8 = ptr8 + (xEnd);
	ptr8 += xStart;
	pitch = iBitmapInfo.iLinePitch;
	ptrPitch = pitch-((TInt)prtLimit8-(TInt)ptr8);
	}
	
	do
		{
		while(ptr8<prtLimit8)
			{
			TUint8 index = *ptr8;
			color = TRgb::Color256(index);

			TInt32 value = color.Internal();

			TInt b = (((value & 0x000000ff) * scale) >> 8)  + offset;
			TInt g = (((value & 0x0000ff00) * scale) >> 16) + offset;
			//the multiplication by scale can overflow into the sign bit, so we shift down in two steps
			TInt r = ((((value & 0x00ff0000) >> 16) * scale) >> 8) + offset; 

			color = TRgb(r,g,b);
			*(ptr8++) = (TUint8)color.Color256();
			}

		ptr8 = ptr8+ptrPitch;
		prtLimit8 = prtLimit8+pitch;
		}
	while(--height);

	//Unlock();
	return;
error:
	//Unlock();
	GraphicsAcceleratorPanic(EGraphicsAcceleratorAttemptedDrawOutsideBitmap);
	}

TInt CColor256HardwareGraphicsAccelerator::DoBitBlt(const TPoint& aDestination,const TAcceleratedBitmapSpec& aSourceBitmapSpec,const TRect& aSourceRect)
	{
	if(aSourceBitmapSpec.Type() != TAcceleratedBitmapSpec::EHardwareBitmap)
		return KErrNotSupported;

	TInt handle = aSourceBitmapSpec.Handle();
	RHardwareBitmap sourceBitmap(handle);

	TAcceleratedBitmapInfo sourceBitmapInfo;
	sourceBitmap.GetInfo(sourceBitmapInfo);

    if(sourceBitmapInfo.iDisplayMode != EColor256)
		return KErrNotSupported;

	TUint32* ptrSource32 = (TUint32*)sourceBitmapInfo.iAddress;
	TUint32* prtLimitSource32;

	TUint32* ptrDest32 = (TUint32*)iBitmapInfo.iAddress;
	TUint32* ptrLimitDest32;

	TUint8* ptrSource8 = (TUint8*)ptrSource32;
	TUint8* prtLimitSource8;

	TUint8* ptrDest8 = (TUint8*)ptrDest32;
	TUint8* ptrLimitDest8;


	TInt ySourceStart = aSourceRect.iTl.iY;
	TInt ySourceEnd = aSourceRect.iBr.iY;
	TInt xSourceStart = aSourceRect.iTl.iX;
	TInt xSourceEnd = aSourceRect.iBr.iX;

	TInt yDestStart;
	TInt yDestEnd;
	TInt xDestStart;
	TInt xDestEnd;

	TUint32 leftMask;
	TUint32 rightMask;

	TInt ptrPitchSource;
	TInt pitchSource;
	TInt ptrPitchDest;
	TInt pitchDest;

	TInt height;


	TRect area(aSourceRect); //final area from aSourceRect to blit
	TRect destRect(aSourceRect.Size());

	//aSource
	if(ySourceStart<0)
		goto error;
	if(ySourceStart>=ySourceEnd)
		goto error;
	if(ySourceEnd>sourceBitmapInfo.iSize.iHeight)
		goto error;

	if(xSourceStart<0)
		goto error;
	if(xSourceStart>=xSourceEnd)
		goto error;
	if(xSourceEnd>sourceBitmapInfo.iSize.iWidth)
		goto error;

    destRect.Move(aDestination);
	if(!destRect.Intersects(TRect(iBitmapInfo.iSize)))
	   return KErrNone;

    //save parameters
	yDestStart = destRect.iTl.iY;
	yDestEnd = destRect.iBr.iY;
	xDestStart = destRect.iTl.iX;
	xDestEnd = destRect.iBr.iX;

	destRect.Intersection(TRect(iBitmapInfo.iSize));

	ySourceStart += destRect.iTl.iY - yDestStart;
	ySourceEnd += destRect.iBr.iY - yDestEnd;
	xSourceStart += destRect.iTl.iX - xDestStart;
	xSourceEnd += destRect.iBr.iX - xDestEnd;

	yDestStart = destRect.iTl.iY;
	yDestEnd = destRect.iBr.iY;
	xDestStart = destRect.iTl.iX;
	xDestEnd = destRect.iBr.iX;

	area.SetRect(xSourceStart,ySourceStart,xSourceEnd,ySourceEnd);
	height = ySourceEnd - ySourceStart;


	if(!((xSourceStart&0x1F) == (xDestStart&0x1F))) // source and dest are not aligned
		{
 	    ptrSource8 = (ptrSource8 + ySourceStart*sourceBitmapInfo.iLinePitch);
		prtLimitSource8 = ptrSource8 + (xSourceEnd);
		ptrSource8 += xSourceStart;

		ptrDest8 = (ptrDest8 + yDestStart*iBitmapInfo.iLinePitch);
		ptrLimitDest8 = ptrDest8 + (xDestEnd);
		ptrDest8 += xDestStart;

		pitchSource = sourceBitmapInfo.iLinePitch;
		ptrPitchSource = pitchSource -((TInt)prtLimitSource8 - (TInt)ptrSource8);

		pitchDest = iBitmapInfo.iLinePitch;
		ptrPitchDest = pitchDest - ((TInt)ptrLimitDest8 - (TInt)ptrDest8);

		do
			{
			while(ptrSource8<prtLimitSource8)
				*(ptrDest8++) = *(ptrSource8++);

			ptrSource8 = ptrSource8+ptrPitchSource;
			prtLimitSource8 = prtLimitSource8+pitchSource;
			ptrDest8 = ptrDest8+ptrPitchDest;
			ptrLimitDest8 = ptrLimitDest8+pitchDest;
			}
		while(--height);
		}
	else
		{
		//distances in bit from the left edge
		xSourceStart <<= sourceBitmapInfo.iPixelShift;
		xSourceEnd <<= sourceBitmapInfo.iPixelShift;
		xDestStart <<= iBitmapInfo.iPixelShift;
		xDestEnd <<= iBitmapInfo.iPixelShift;

		ptrSource32 = (TUint32*)((TUint8*)ptrSource32 + ySourceStart*sourceBitmapInfo.iLinePitch);
	    ptrDest32 = (TUint32*)((TUint8*)ptrDest32 + destRect.iTl.iY*iBitmapInfo.iLinePitch);

	    leftMask = ~((1<<(xSourceStart&0x1F))-1);
	    rightMask = (1<<(xSourceEnd&0x1F))-1;

		xSourceStart >>= 5;
		xSourceEnd = (xSourceEnd-1) >> 5;
		xDestStart >>= 5;
		xDestEnd = (xDestEnd-1) >> 5;

		prtLimitSource32 = ptrSource32 + xSourceEnd;
		ptrSource32 += xSourceStart;
		ptrLimitDest32 = ptrDest32 + xDestEnd;
		ptrDest32 += xDestStart;

		pitchSource = sourceBitmapInfo.iLinePitch;
		ptrPitchSource = pitchSource -((TInt)prtLimitSource32 - (TInt)ptrSource32);
		pitchDest = iBitmapInfo.iLinePitch;
		ptrPitchDest = pitchDest - ((TInt)ptrLimitDest32 - (TInt)ptrDest32);

	    if((ptrSource32==prtLimitSource32) && (leftMask!=0xFFFFFFFF)) // all pixels in a line are in the same word
			{
			if((leftMask != 0) && (rightMask !=0))
				leftMask &= rightMask;
			else if(rightMask != 0)
					leftMask = rightMask;
			do
				{
				TInt pixelsSource = *ptrSource32;
				TInt pixelsDest = *ptrDest32;
				pixelsSource &= leftMask;
				pixelsDest &= ~ leftMask;
				pixelsDest |= pixelsSource;
				*(ptrDest32) = pixelsDest;
				ptrSource32 = (TUint32*)((TUint8*)ptrSource32+ptrPitchSource);
				ptrDest32 = (TUint32*)((TUint8*)ptrDest32+ptrPitchDest);
				}
			while(--height);
			}
		else if((leftMask==0xFFFFFFFF)&&(rightMask==0)) // all pixels in a line fit into whole words
		{
			do
				{
				while(ptrSource32<prtLimitSource32)
					*(ptrDest32++) = *(ptrSource32++);
				*ptrDest32 = *ptrSource32;

				ptrSource32 = (TUint32*)((TUint8*)ptrSource32+ptrPitchSource);
				prtLimitSource32 = (TUint32*)((TUint8*)prtLimitSource32+pitchSource);
				ptrDest32 = (TUint32*)((TUint8*)ptrDest32+ptrPitchDest);
				ptrLimitDest32 = (TUint32*)((TUint8*)ptrLimitDest32+pitchDest);
				}
			while(--height);
			}
			else if(leftMask==0xFFFFFFFF) // first pixel starts on a word boundary but last doesn't
				{
				do
					{
					while(ptrSource32<prtLimitSource32) 
					*(ptrDest32++) = *(ptrSource32++);

					TInt pixelsSource = *ptrSource32;
					TInt pixelsDest = *ptrDest32;
					pixelsSource &= rightMask;
					pixelsDest &= ~ rightMask;
					pixelsDest |= pixelsSource;
					*ptrDest32 = pixelsDest;

					ptrSource32 = (TUint32*)((TUint8*)ptrSource32+ptrPitchSource);
					prtLimitSource32 = (TUint32*)((TUint8*)prtLimitSource32+pitchSource);
					ptrDest32 = (TUint32*)((TUint8*)ptrDest32+ptrPitchDest);
					ptrLimitDest32 = (TUint32*)((TUint8*)ptrLimitDest32+pitchDest);
					}
				while(--height);
				}
			else if(rightMask==0) // last pixel lies on a word boundary but first doesn't
				{
				do
					{
					TInt pixelsSource = *ptrSource32;
					TInt pixelsDest = *ptrDest32;
					pixelsSource &= leftMask;
					pixelsDest &= ~ leftMask;
					pixelsDest |= pixelsSource;
					*(ptrDest32++) = pixelsDest;
					ptrSource32++;

					while(ptrSource32<prtLimitSource32)
						*(ptrDest32++) = *(ptrSource32++);
					*ptrDest32 = *ptrSource32;

					ptrSource32 = (TUint32*)((TUint8*)ptrSource32+ptrPitchSource);
					prtLimitSource32 = (TUint32*)((TUint8*)prtLimitSource32+pitchSource);
					ptrDest32 = (TUint32*)((TUint8*)ptrDest32+ptrPitchDest);
					ptrLimitDest32 = (TUint32*)((TUint8*)ptrLimitDest32+pitchDest);
					}
				while(--height);
				}
			else // both last and first pixels in a line don't lie on a word boundary
				{
				do
					{
					TInt pixelsSource = *ptrSource32;
					TInt pixelsDest = *ptrDest32;
					pixelsSource &= leftMask;
					pixelsDest &= ~ leftMask;
					pixelsDest |= pixelsSource;
					*(ptrDest32++) = pixelsDest;
					ptrSource32++;

					while(ptrSource32<prtLimitSource32)
						*(ptrDest32++) = *(ptrSource32++);

					pixelsSource = *ptrSource32;
					pixelsDest = *ptrDest32;
					pixelsSource &= rightMask;
					pixelsDest &= ~ rightMask;
					pixelsDest |= pixelsSource;
					*ptrDest32 = pixelsDest;

					ptrSource32 = (TUint32*)((TUint8*)ptrSource32+ptrPitchSource);
					prtLimitSource32 = (TUint32*)((TUint8*)prtLimitSource32+pitchSource);
					ptrDest32 = (TUint32*)((TUint8*)ptrDest32+ptrPitchDest);
					ptrLimitDest32 = (TUint32*)((TUint8*)ptrLimitDest32+pitchDest);
					}
				while(--height);
				}
			}
	return KErrNone;
error:
	GraphicsAcceleratorPanic(EGraphicsAcceleratorAttemptedDrawOutsideBitmap);
	return KErrNone; // Will not be executed but prevents compiler warnings
	}

TInt CColor256HardwareGraphicsAccelerator::DoBitBltMasked(const TPoint& aDestination,const TAcceleratedBitmapSpec& aSourceBitmapSpec,const TRect& aSourceRect,const TAcceleratedBitmapSpec& aMaskBitmapSpec)
	{
    //Destination Bitmap
	TUint8* ptrDest8 = (TUint8*)iBitmapInfo.iAddress;
	TUint8* ptrLimitDest8;

	//Source Bitmap
	if(aSourceBitmapSpec.Type() != TAcceleratedBitmapSpec::EHardwareBitmap)
		return KErrNotSupported;

	TInt handleSourceBitmap = aSourceBitmapSpec.Handle();
	RHardwareBitmap sourceBitmap(handleSourceBitmap);

	TAcceleratedBitmapInfo sourceBitmapInfo;
	sourceBitmap.GetInfo(sourceBitmapInfo);

	TUint8* ptrSource8 = (TUint8*)sourceBitmapInfo.iAddress;
	TUint8* prtLimitSource8;


	//Mask Bitmap
	if(aMaskBitmapSpec.Type() != TAcceleratedBitmapSpec::EHardwareBitmap)
		return KErrNotSupported;

	TInt handleMaskBitmap = aMaskBitmapSpec.Handle();
	RHardwareBitmap maskBitmap(handleMaskBitmap);

	TAcceleratedBitmapInfo maskBitmapInfo;
	maskBitmap.GetInfo(maskBitmapInfo);

    if((sourceBitmapInfo.iDisplayMode != EColor256) || (maskBitmapInfo.iDisplayMode != EColor256))
		return KErrNotSupported;

	
	TUint8* ptrMask8 = (TUint8*)maskBitmapInfo.iAddress;
	TUint8* ptrLimitMask8;


	TInt ySourceStart = aSourceRect.iTl.iY;
	TInt ySourceEnd = aSourceRect.iBr.iY;
	TInt xSourceStart = aSourceRect.iTl.iX;
	TInt xSourceEnd = aSourceRect.iBr.iX;

	TInt yDestStart;
	TInt yDestEnd;
	TInt xDestStart;
	TInt xDestEnd;

	TInt ptrPitchSource;
	TInt pitchSource;
	TInt ptrPitchDest;
	TInt pitchDest;
	TInt ptrPitchMask;
	TInt pitchMask;

	TInt height;

	TRect area(aSourceRect); //final area from aSourceRect to blit
	TRect destRect(aSourceRect.Size());
    
	if(sourceBitmapInfo.iSize != maskBitmapInfo.iSize)
		return KErrNotSupported;

	//aSource
	if(ySourceStart<0)
		goto error;
	if(ySourceStart>=ySourceEnd)
		goto error;
	if(ySourceEnd>sourceBitmapInfo.iSize.iHeight)
		goto error;

	if(xSourceStart<0)
		goto error;
	if(xSourceStart>=xSourceEnd)
		goto error;
	if(xSourceEnd>sourceBitmapInfo.iSize.iWidth)
		goto error;


    destRect.Move(aDestination);
	if(!destRect.Intersects(TRect(iBitmapInfo.iSize)))
	   return KErrNone;

    //save parameters
	yDestStart = destRect.iTl.iY;
	yDestEnd = destRect.iBr.iY;
	xDestStart = destRect.iTl.iX;
	xDestEnd = destRect.iBr.iX;

	destRect.Intersection(TRect(iBitmapInfo.iSize));

	ySourceStart += destRect.iTl.iY - yDestStart;
	ySourceEnd += destRect.iBr.iY - yDestEnd;
	xSourceStart += destRect.iTl.iX - xDestStart;
	xSourceEnd += destRect.iBr.iX - xDestEnd;

	yDestStart = destRect.iTl.iY;
	yDestEnd = destRect.iBr.iY;
	xDestStart = destRect.iTl.iX;
	xDestEnd = destRect.iBr.iX;

	area.SetRect(xSourceStart,ySourceStart,xSourceEnd,ySourceEnd);
	height = ySourceEnd - ySourceStart;

    //Source Bitmap
 	ptrSource8 = (ptrSource8 + ySourceStart*sourceBitmapInfo.iLinePitch);
	prtLimitSource8 = ptrSource8 + (xSourceEnd);
	ptrSource8 += xSourceStart;

	pitchSource = sourceBitmapInfo.iLinePitch;
	ptrPitchSource = pitchSource -((TInt)prtLimitSource8 - (TInt)ptrSource8);

    //Destination Bitmap
	ptrDest8 = (ptrDest8 + yDestStart*iBitmapInfo.iLinePitch);
	ptrLimitDest8 = ptrDest8 + (xDestEnd);
	ptrDest8 += xDestStart;

	pitchDest = iBitmapInfo.iLinePitch;
	ptrPitchDest = pitchDest - ((TInt)ptrLimitDest8 - (TInt)ptrDest8);

    //Mask Bitmap
 	ptrMask8 = (ptrMask8 + ySourceStart*maskBitmapInfo.iLinePitch);
	ptrLimitMask8 = ptrMask8 + (xSourceEnd);
	ptrMask8 += xSourceStart;

	pitchMask = maskBitmapInfo.iLinePitch;
	ptrPitchMask = pitchMask -((TInt)ptrLimitMask8 - (TInt)ptrMask8);
 
	do
		{
		while(ptrSource8<prtLimitSource8)
			{
		    *ptrDest8 = (TUint8)((*ptrSource8 & (~(*ptrMask8))) | (*ptrDest8 & *ptrMask8));
			ptrMask8++;
			ptrDest8++;
			ptrSource8++;
			}

		ptrSource8 = ptrSource8+ptrPitchSource;
		prtLimitSource8 = prtLimitSource8+pitchSource;
		ptrDest8 = ptrDest8+ptrPitchDest;
		ptrLimitDest8 = ptrLimitDest8+pitchDest;
		ptrMask8 = ptrMask8+ptrPitchMask;
		ptrLimitMask8 = ptrLimitMask8+pitchMask;
		}
	while(--height);

	
	return KErrNone;
error:
	GraphicsAcceleratorPanic(EGraphicsAcceleratorAttemptedDrawOutsideBitmap);
	return KErrNone; // Will not be executed but prevents compiler warnings
	}

TInt CColor256HardwareGraphicsAccelerator::DoBitBltAlphaBitmap(const TPoint& aDestination,const TAcceleratedBitmapSpec& aSourceBitmapSpec,const TRect& aSourceRect,const TAcceleratedBitmapSpec& aAlphaBitmapSpec)
	{

    //Destination Bitmap
	TUint8* ptrDest8 = (TUint8*)iBitmapInfo.iAddress;
	TUint8* ptrLimitDest8;

	//Source Bitmap
	if(aSourceBitmapSpec.Type() != TAcceleratedBitmapSpec::EHardwareBitmap)
		return KErrNotSupported;

	TInt handleSourceBitmap = aSourceBitmapSpec.Handle();
	RHardwareBitmap sourceBitmap(handleSourceBitmap);

	TAcceleratedBitmapInfo sourceBitmapInfo;
	sourceBitmap.GetInfo(sourceBitmapInfo);

	TUint8* ptrSource8 = (TUint8*)sourceBitmapInfo.iAddress;
	TUint8* prtLimitSource8;

	//Alpha Bitmap
	if(aAlphaBitmapSpec.Type() != TAcceleratedBitmapSpec::EHardwareBitmap)
		return KErrNotSupported;

	TInt handleAlphaBitmap = aAlphaBitmapSpec.Handle();
	RHardwareBitmap alphaBitmap(handleAlphaBitmap);

	TAcceleratedBitmapInfo alphaBitmapInfo;
	alphaBitmap.GetInfo(alphaBitmapInfo);

    if((sourceBitmapInfo.iDisplayMode != EColor256) || (alphaBitmapInfo.iDisplayMode != EGray256))
		return KErrNotSupported;

	
	TUint8* ptrAlpha8 = (TUint8*)alphaBitmapInfo.iAddress;
	TUint8* ptrLimitAlpha8;

	TInt ySourceStart = aSourceRect.iTl.iY;
	TInt ySourceEnd = aSourceRect.iBr.iY;
	TInt xSourceStart = aSourceRect.iTl.iX;
	TInt xSourceEnd = aSourceRect.iBr.iX;

	TInt yDestStart;
	TInt yDestEnd;
	TInt xDestStart;
	TInt xDestEnd;

	TInt ptrPitchSource;
	TInt pitchSource;
	TInt ptrPitchDest;
	TInt pitchDest;
	TInt ptrPitchAlpha;
	TInt pitchAlpha;

	TInt height;

	TRect area(aSourceRect); //final area from aSourceRect to blit
	TRect destRect(aSourceRect.Size());

	TRgb color;
    
	if(sourceBitmapInfo.iSize != alphaBitmapInfo.iSize)
		return KErrNotSupported;

	//aSource
	if(ySourceStart<0)
		goto error;
	if(ySourceStart>=ySourceEnd)
		goto error;
	if(ySourceEnd>sourceBitmapInfo.iSize.iHeight)
		goto error;

	if(xSourceStart<0)
		goto error;
	if(xSourceStart>=xSourceEnd)
		goto error;
	if(xSourceEnd>sourceBitmapInfo.iSize.iWidth)
		goto error;


    destRect.Move(aDestination);
	if(!destRect.Intersects(TRect(iBitmapInfo.iSize)))
	   return KErrNone;

    //save parameters
	yDestStart = destRect.iTl.iY;
	yDestEnd = destRect.iBr.iY;
	xDestStart = destRect.iTl.iX;
	xDestEnd = destRect.iBr.iX;

	destRect.Intersection(TRect(iBitmapInfo.iSize));

	ySourceStart += destRect.iTl.iY - yDestStart;
	ySourceEnd += destRect.iBr.iY - yDestEnd;
	xSourceStart += destRect.iTl.iX - xDestStart;
	xSourceEnd += destRect.iBr.iX - xDestEnd;

	yDestStart = destRect.iTl.iY;
	yDestEnd = destRect.iBr.iY;
	xDestStart = destRect.iTl.iX;
	xDestEnd = destRect.iBr.iX;

	area.SetRect(xSourceStart,ySourceStart,xSourceEnd,ySourceEnd);
	height = ySourceEnd - ySourceStart;

    //Source Bitmap
 	ptrSource8 = (ptrSource8 + ySourceStart*sourceBitmapInfo.iLinePitch);
	prtLimitSource8 = ptrSource8 + (xSourceEnd);
	ptrSource8 += xSourceStart;

	pitchSource = sourceBitmapInfo.iLinePitch;
	ptrPitchSource = pitchSource -((TInt)prtLimitSource8 - (TInt)ptrSource8);

    //Destination Bitmap
	ptrDest8 = (ptrDest8 + yDestStart*iBitmapInfo.iLinePitch);
	ptrLimitDest8 = ptrDest8 + (xDestEnd);
	ptrDest8 += xDestStart;

	pitchDest = iBitmapInfo.iLinePitch;
	ptrPitchDest = pitchDest - ((TInt)ptrLimitDest8 - (TInt)ptrDest8);

    //Alpha Bitmap
 	ptrAlpha8 = (ptrAlpha8 + ySourceStart*alphaBitmapInfo.iLinePitch);
	ptrLimitAlpha8 = ptrAlpha8 + (xSourceEnd);
	ptrAlpha8 += xSourceStart;

	pitchAlpha = alphaBitmapInfo.iLinePitch;
	ptrPitchAlpha = pitchAlpha -((TInt)ptrLimitAlpha8 - (TInt)ptrAlpha8);
	
	do
		{
		while(ptrSource8<prtLimitSource8)
			{
			//Source color information
			TUint8 index = *ptrSource8;
			color = TRgb::Color256(index);
			TUint32 value = color.Internal();
			TInt blueSource = (value & 0x000000ff);
			TInt greenSource = (value  & 0x0000ff00) >> 8;
			TInt redSource = (value  & 0x00ff0000) >> 16;

			//Dest color information
			index= *ptrDest8;
			color = TRgb::Color256(index);
			value = color.Internal();
			TInt blueDest = (value & 0x000000ff);
			TInt greenDest = (value & 0x0000ff00) >> 8;
			TInt redDest= (value & 0x00ff0000) >> 16;

            TInt inverseMask = 255 - *ptrAlpha8;

			redDest = ((redSource * (*ptrAlpha8) + redSource) + (redDest * inverseMask + redDest)) >> 8; 
			greenDest = ((greenSource * (*ptrAlpha8) + greenSource) + (greenDest * inverseMask + greenDest)) >> 8; 
			blueDest = ((blueSource * (*ptrAlpha8) + blueSource) + (blueDest * inverseMask + blueDest)) >> 8; 

			color = TRgb(redDest,greenDest,blueDest);
			*ptrDest8 = TUint8(color.Color256());

			ptrAlpha8++;
			ptrDest8++;
			ptrSource8++;
			}

		ptrSource8 = ptrSource8+ptrPitchSource;
		prtLimitSource8 = prtLimitSource8+pitchSource;
		ptrDest8 = ptrDest8+ptrPitchDest;
		ptrLimitDest8 = ptrLimitDest8+pitchDest;
		ptrAlpha8 = ptrAlpha8+ptrPitchAlpha;
		ptrLimitAlpha8 = ptrLimitAlpha8+pitchAlpha;
		}
	while(--height);

	
	return KErrNone;
error:
	GraphicsAcceleratorPanic(EGraphicsAcceleratorAttemptedDrawOutsideBitmap);
	return KErrNone; // Will not be executed but prevents compiler warnings
	}	
	
TInt CColor256HardwareGraphicsAccelerator::DoAlphaBlendTwoBitmaps(const TPoint& aDestination,const TAcceleratedBitmapSpec& aSrceBitmapSpec1,const TAcceleratedBitmapSpec& aSrceBitmapSpec2,const TRect& aSourceRect,const TPoint& aSrcPt2,const TAcceleratedBitmapSpec& aAlphaBitmapSpec,const TPoint& aAlphaPt)
	{
	//Destination Bitmap
	TUint8* ptrDest8 = (TUint8*)iBitmapInfo.iAddress;

	// First Source Bitmap
	if(aSrceBitmapSpec1.Type() != TAcceleratedBitmapSpec::EHardwareBitmap)
		return KErrNotSupported;

	TInt handleSourceBitmap1 = aSrceBitmapSpec1.Handle();
	RHardwareBitmap sourceBitmap1(handleSourceBitmap1);

	TAcceleratedBitmapInfo sourceBitmap1Info;
	sourceBitmap1.GetInfo(sourceBitmap1Info);

	TUint8* ptrSourceOne8 = (TUint8*)sourceBitmap1Info.iAddress;
	
	// Second Source Bitmap
	if(aSrceBitmapSpec2.Type() != TAcceleratedBitmapSpec::EHardwareBitmap)
		return KErrNotSupported;

	TInt handleSourceBitmap2 = aSrceBitmapSpec2.Handle();
	RHardwareBitmap sourceBitmap2(handleSourceBitmap2);

	TAcceleratedBitmapInfo sourceBitmap2Info;
	sourceBitmap2.GetInfo(sourceBitmap2Info);

	TUint8* ptrSourceTwo8 = (TUint8*)sourceBitmap2Info.iAddress;

	//Alpha Bitmap
	if(aAlphaBitmapSpec.Type() != TAcceleratedBitmapSpec::EHardwareBitmap)
		return KErrNotSupported;

	TInt handleAlphaBitmap = aAlphaBitmapSpec.Handle();
	RHardwareBitmap alphaBitmap(handleAlphaBitmap);

	TAcceleratedBitmapInfo alphaBitmapInfo;
	alphaBitmap.GetInfo(alphaBitmapInfo);

	if((sourceBitmap1Info.iDisplayMode != EColor256) || (sourceBitmap2Info.iDisplayMode != EColor256) || (alphaBitmapInfo.iDisplayMode != EGray256))
		return KErrNotSupported;

	TUint8* ptrAlpha8 = (TUint8*)alphaBitmapInfo.iAddress;

	TRect destRect(aSourceRect.Size());
    
	if((sourceBitmap1Info.iSize != alphaBitmapInfo.iSize) && (sourceBitmap2Info.iSize != alphaBitmapInfo.iSize))
		return KErrNotSupported;

	// Check bounds on the source rectangle
	if(aSourceRect.iTl.iY<0)
		goto error;
	if(aSourceRect.iTl.iY>=aSourceRect.iBr.iY)
		goto error;
	if(aSourceRect.iBr.iY>sourceBitmap1Info.iSize.iHeight)
		goto error;
	if(aSourceRect.iTl.iX<0)
		goto error;
	if(aSourceRect.iTl.iX>=aSourceRect.iBr.iX)
		goto error;
	if(aSourceRect.iBr.iX>sourceBitmap1Info.iSize.iWidth)
		goto error;

	// Find writeable area of the destination rectangle
	destRect.Move(aDestination);
	if(!destRect.Intersects(TRect(iBitmapInfo.iSize)))
	   return KErrNone;
	destRect.Intersection(TRect(iBitmapInfo.iSize));

	// Iterate through the pixels and blend to the destination
	TInt srceY = aSourceRect.iTl.iY;
	TInt src2Y = aSrcPt2.iY;
	TInt srcaY = aAlphaPt.iY;
	TInt destY = destRect.iTl.iY;
	while (destY < destRect.iBr.iY)
		{
		TInt srceX = aSourceRect.iTl.iX;
		TInt src2X = aSrcPt2.iX;
		TInt destX = destRect.iTl.iX;
		TInt srcaX = aAlphaPt.iX;
	  	
		while (destX < destRect.iBr.iX)
			{
			// First Source color information
			TUint8 index = ptrSourceOne8[srceY*sourceBitmap1Info.iLinePitch+srceX];
			TRgb color = TRgb::Color256(index);
			TUint32 value = color.Internal();
			TInt blueSource1 = (value & 0x000000ff);
			TInt greenSource1 = (value  & 0x0000ff00) >> 8;
			TInt redSource1 = (value  & 0x00ff0000) >> 16;

			// Second Source color information
			index = ptrSourceTwo8[src2Y*sourceBitmap2Info.iLinePitch+src2X];
			color = TRgb::Color256(index);
			value = color.Internal();
			TInt blueSource2 = (value & 0x000000ff);
			TInt greenSource2 = (value  & 0x0000ff00) >> 8;
			TInt redSource2 = (value  & 0x00ff0000) >> 16;

			TUint8 alpha = ptrAlpha8[srcaY*alphaBitmapInfo.iLinePitch+srcaX];
			TInt inverseMask = 255 - alpha;

			// Blend
			TInt redDest = ((alpha * (redSource1 - redSource2)) >> 8) + redSource2;
			TInt greenDest = ((alpha * (greenSource1 - greenSource2)) >> 8) + greenSource2;
			TInt blueDest = ((alpha * (blueSource1 - blueSource2)) >> 8) + blueSource2;

			color = TRgb(redDest,greenDest,blueDest);
			ptrDest8[destY*iBitmapInfo.iLinePitch+destX] = TUint8(color.Color256());

			++srceX;
			++src2X;
			++destX;
			++srcaX;
			}

		++srceY;
		++destY;
		++src2Y;
		++srcaY;
		}

	return KErrNone;
error:
	GraphicsAcceleratorPanic(EGraphicsAcceleratorAttemptedDrawOutsideBitmap);
	return KErrNone; // Will not be executed but prevents compiler warnings
	}

TInt CColor256HardwareGraphicsAccelerator::DoAlphaBlendOneBitmap(const TPoint& aDestination,const TAcceleratedBitmapSpec& aSrceBitmapSpec,const TRect& aSourceRect,const TAcceleratedBitmapSpec& aAlphaBitmapSpec,const TPoint& aAlphaPt)
	{
	//Destination Bitmap
	TUint8* ptrDest8 = (TUint8*)iBitmapInfo.iAddress;

	// Source Bitmap
	if(aSrceBitmapSpec.Type() != TAcceleratedBitmapSpec::EHardwareBitmap)
		return KErrNotSupported;

	TInt handleSourceBitmap = aSrceBitmapSpec.Handle();
	RHardwareBitmap sourceBitmap(handleSourceBitmap);

	TAcceleratedBitmapInfo sourceBitmapInfo;
	sourceBitmap.GetInfo(sourceBitmapInfo);

	TUint8* ptrSource8 = (TUint8*)sourceBitmapInfo.iAddress;

	//Alpha Bitmap
	if(aAlphaBitmapSpec.Type() != TAcceleratedBitmapSpec::EHardwareBitmap)
		return KErrNotSupported;

	TInt handleAlphaBitmap = aAlphaBitmapSpec.Handle();
	RHardwareBitmap alphaBitmap(handleAlphaBitmap);

	TAcceleratedBitmapInfo alphaBitmapInfo;
	alphaBitmap.GetInfo(alphaBitmapInfo);

	if((sourceBitmapInfo.iDisplayMode != EColor256) || (alphaBitmapInfo.iDisplayMode != EGray256))
		return KErrNotSupported;

	TUint8* ptrAlpha8 = (TUint8*)alphaBitmapInfo.iAddress;

	TRect destRect(aSourceRect.Size());
    
	if(sourceBitmapInfo.iSize != alphaBitmapInfo.iSize)
		return KErrNotSupported;

	// Check bounds on the source rectangle
	if(aSourceRect.iTl.iY<0)
		goto error;
	if(aSourceRect.iTl.iY>=aSourceRect.iBr.iY)
		goto error;
	if(aSourceRect.iBr.iY>sourceBitmapInfo.iSize.iHeight)
		goto error;
	if(aSourceRect.iTl.iX<0)
		goto error;
	if(aSourceRect.iTl.iX>=aSourceRect.iBr.iX)
		goto error;
	if(aSourceRect.iBr.iX>sourceBitmapInfo.iSize.iWidth)
		goto error;

	// Find writeable area of the destination rectangle
	destRect.Move(aDestination);
	if(!destRect.Intersects(TRect(iBitmapInfo.iSize)))
	   return KErrNone;
	destRect.Intersection(TRect(iBitmapInfo.iSize));
	
	// Iterate through the pixels and blend to the destination
	TInt srceY = aSourceRect.iTl.iY;
	TInt srcaY = aAlphaPt.iY;
	TInt destY = destRect.iTl.iY;
	while (destY < destRect.iBr.iY)
		{
		TInt srceX = aSourceRect.iTl.iX;
		TInt destX = destRect.iTl.iX;
		TInt srcaX = aAlphaPt.iX;
	  	
		while (destX < destRect.iBr.iX)
			{
			// First Source color information
			TUint8 index = ptrSource8[srceY*sourceBitmapInfo.iLinePitch+srceX];
			TRgb color = TRgb::Color256(index);
			TUint32 value = color.Internal();
			TInt blueSource = (value & 0x000000ff);
			TInt greenSource = (value  & 0x0000ff00) >> 8;
			TInt redSource = (value  & 0x00ff0000) >> 16;

			// Second Source color information
			index = ptrDest8[destY*iBitmapInfo.iLinePitch+destX];
			color = TRgb::Color256(index);
			value = color.Internal();
			TInt blueDest = (value & 0x000000ff);
			TInt greenDest = (value  & 0x0000ff00) >> 8;
			TInt redDest = (value  & 0x00ff0000) >> 16;

			TUint8 alpha = ptrAlpha8[srcaY*alphaBitmapInfo.iLinePitch+srcaX];
			TInt inverseMask = 255 - alpha;
			
			// Blend
			redDest = ((alpha * (redSource - redDest)) >> 8) + redDest;
			greenDest = ((alpha * (greenSource - greenDest)) >> 8) + greenDest;
			blueDest = ((alpha * (blueSource - blueDest)) >> 8) + blueDest;

			color = TRgb(redDest,greenDest,blueDest);
			ptrDest8[destY*iBitmapInfo.iLinePitch+destX] = TUint8(color.Color256());
			
			++srceX;
			++destX;
			++srcaX;
			}

		++srceY;
		++destY;
		++srcaY;
		}

	return KErrNone;
error:
	GraphicsAcceleratorPanic(EGraphicsAcceleratorAttemptedDrawOutsideBitmap);
	return KErrNone; // Will not be executed but prevents compiler warnings
	}

//
// CHardwareGraphicsAcceleratorBase
//

void CHardwareGraphicsAcceleratorBase::ConstructL(RHardwareBitmap aBitmap)
	{
	aBitmap.GetInfo(iBitmapInfo);
	iIsScreen = (aBitmap.iHandle < 0)?1:0;
	}

const TGraphicsAcceleratorCaps* CHardwareGraphicsAcceleratorBase::Capabilities()
	{
	return iCaps;
	}

TInt CHardwareGraphicsAcceleratorBase::Operation(const TGraphicsOperation& aOperation)
	{
	switch(aOperation.Function())
		{
		case TGraphicsOperation::EFilledRect:
			{
			const TGopFilledRect& op = *(const TGopFilledRect*)&aOperation;
			DoFillRect(op.iRect,FillValue(op.iColor));
			UpdateScreen(op.iRect);
			return KErrNone;
			}
		case TGraphicsOperation::EFilledRectWithPattern:
			{
			const TGopFilledRectWithPattern& op = *(const TGopFilledRectWithPattern*)&aOperation;
			TInt error = DoFilledRectWithPattern(op.iRect,op.iPattern);
			if(error==KErrNone)
				UpdateScreen(op.iRect);
			return error;
			}
		case TGraphicsOperation::EInvertRect:
			{
			const TGopInvertRect& op = *(const TGopInvertRect*)&aOperation;
			DoInvertRect(op.iRect);
			UpdateScreen(op.iRect);
			return KErrNone;
			}
		case TGraphicsOperation::EFadeRect:
			{
			const TGopFadeRect& op = *(const TGopFadeRect*)&aOperation;
			DoFadeRect(op.iRect,op.iFade);
			UpdateScreen(op.iRect);
			return KErrNone;
			}
		case TGraphicsOperation::EBitBlt:
			{
			const TGopBitBlt& op = *(const TGopBitBlt*)&aOperation;
			TInt error = DoBitBlt(op.iDestination,op.iSourceBitmap,op.iSourceRect);
			if(error==KErrNone)
				{
				TRect rect(op.iSourceRect.Size());
				rect.Move(op.iDestination);
				UpdateScreen(rect);
				}
			return error;
			}
		case TGraphicsOperation::EBitBltMasked:
			{
			const TGopBitBltMasked& op = *(const TGopBitBltMasked*)&aOperation;
			TInt error = DoBitBltMasked(op.iDestination,op.iSourceBitmap,op.iSourceRect,op.iMask);
			if(error==KErrNone)
				{
				TRect rect(op.iSourceRect.Size());
				rect.Move(op.iDestination);
				UpdateScreen(rect);
				}
			return error;
			}
		case TGraphicsOperation::EBitBltTransparent:
		case TGraphicsOperation::EBitBltAlphaChannel:
		case TGraphicsOperation::EBitBltAlphaBitmap:
			{
			const TGopBitBltAlphaBitmap& op = *(const TGopBitBltAlphaBitmap*)&aOperation;
			TInt error = DoBitBltAlphaBitmap(op.iDestination,op.iSourceBitmap,op.iSourceRect,op.iAlphaBitmap);
			if(error==KErrNone)
				{
				TRect rect(op.iSourceRect.Size());
				rect.Move(op.iDestination);
				UpdateScreen(rect);
				}
			return error;
			}
		case TGraphicsOperation::EAlphaBlendTwoBitmaps:
			{
			const TGopAlphaBlendTwoBitmaps& op = *(const TGopAlphaBlendTwoBitmaps*)&aOperation;
			TInt error = DoAlphaBlendTwoBitmaps(op.iDestination,op.iSourceBmp1,op.iSourceBmp2,op.iSourceRect,op.iSrcPt2,op.iAlphaBmp,op.iAlphaPt);
			if(error==KErrNone)
				{
				TRect rect(op.iSourceRect.Size());
				rect.Move(op.iDestination);
				UpdateScreen(rect);
				}
			return error;
			}
		case TGraphicsOperation::EAlphaBlendOneBitmap:
			{
			const TGopAlphaBlendOneBitmap& op = *(const TGopAlphaBlendOneBitmap*)&aOperation;
			TInt error = DoAlphaBlendOneBitmap(op.iDestination,op.iSourceBmp,op.iSourceRect,op.iAlphaBmp,op.iAlphaPt);
			if(error==KErrNone)
				{
				TRect rect(op.iSourceRect.Size());
				rect.Move(op.iDestination);
				UpdateScreen(rect);
				}
			return error;
			}
		case TGraphicsOperation::EFilledRectUsingDrawMode:
		case TGraphicsOperation::EScaledBitBlt:
		case TGraphicsOperation::EScaledBitBltMasked:
		case TGraphicsOperation::EScaledBitBltTransparent:
		case TGraphicsOperation::EScaledBitBltAlphaChannel:
		case TGraphicsOperation::EScaledBitBltAlphaBitmap:
		case TGraphicsOperation::EFilledPolygon:
		case TGraphicsOperation::EFilledPolygonWithPattern:
		default:
			return KErrNotSupported;
		}
	}

TInt CHardwareGraphicsAcceleratorBase::Operation(const TGraphicsOperation& aOperation,TInt /*aNumClipRects*/,TRect* /*aClipRects*/)
	{
	switch(aOperation.Function())
		{
		case TGraphicsOperation::EFilledRect:
		case TGraphicsOperation::EFilledRectUsingDrawMode:
		case TGraphicsOperation::EFilledRectWithPattern:
		case TGraphicsOperation::EInvertRect:
		case TGraphicsOperation::EFadeRect:
		case TGraphicsOperation::EBitBlt:
		case TGraphicsOperation::EBitBltMasked:
		case TGraphicsOperation::EBitBltTransparent:
		case TGraphicsOperation::EBitBltAlphaChannel:
		case TGraphicsOperation::EBitBltAlphaBitmap:
		case TGraphicsOperation::EScaledBitBlt:
		case TGraphicsOperation::EScaledBitBltMasked:
		case TGraphicsOperation::EScaledBitBltTransparent:
		case TGraphicsOperation::EScaledBitBltAlphaChannel:
		case TGraphicsOperation::EScaledBitBltAlphaBitmap:
		case TGraphicsOperation::EFilledPolygon:
		case TGraphicsOperation::EFilledPolygonWithPattern:
		default:
			return KErrNotSupported;
		}
	}

TInt CHardwareGraphicsAcceleratorBase::Operation(TDes8& aBuffer)
	{
	//Lock();

	TInt error = KErrNone;
	TGraphicsOperation* gop = (TGraphicsOperation*) aBuffer.Ptr();
	TGraphicsOperation* limit = (TGraphicsOperation*) ((TUint8*)gop+aBuffer.Length());

	while(gop<limit)
		{
		error = Operation(*gop);
		if(error!=KErrNone)
			break;
		gop = gop->Next();
		}

	//Unlock();

	if(error==KErrNone)
		{
		__ASSERT_ALWAYS(gop==limit,GraphicsAcceleratorPanic(EGraphicsAcceleratorPanicInvalidOperationBuffer));
		}
	else
		{
		aBuffer.SetLength((TUint8*)gop-aBuffer.Ptr());
		}

	return error;
	}

TInt CHardwareGraphicsAcceleratorBase::Operation(TDes8& aBuffer,TInt aNumClipRects,TRect* aClipRects)
	{
	//Lock();

	TInt error = KErrNone;
	TGraphicsOperation* gop = (TGraphicsOperation*) aBuffer.Ptr();
	TGraphicsOperation* limit = (TGraphicsOperation*) ((TUint8*)gop+aBuffer.Length());

	while(gop<limit)
		{
		error = Operation(*gop,aNumClipRects,aClipRects);
		if(error!=KErrNone)
			break;
		gop = gop->Next();
		}

	//Unlock();

	if(error==KErrNone)
		{
		__ASSERT_ALWAYS(gop==limit,GraphicsAcceleratorPanic(EGraphicsAcceleratorPanicInvalidOperationBuffer));
		}
	else
		{
		aBuffer.SetLength((TUint8*)gop-aBuffer.Ptr());
		}

	return error;
	}

// Empty implementations
void CHardwareGraphicsAcceleratorBase::Reserved_1() {};
void CHardwareGraphicsAcceleratorBase::Reserved_2() {};
void CHardwareGraphicsAcceleratorBase::Reserved_3() {};
void CHardwareGraphicsAcceleratorBase::Reserved_4() {};

//
// Misc
//

GLREF_C void GraphicsAcceleratorPanic(TGraphicsAcceleratorPanic aPanicCode)
	{
	_LIT(KSCDVGraphicsAcceleratorPanicCategory,"Graphics Accelerator");
	User::Panic(KSCDVGraphicsAcceleratorPanicCategory,aPanicCode);
	}

