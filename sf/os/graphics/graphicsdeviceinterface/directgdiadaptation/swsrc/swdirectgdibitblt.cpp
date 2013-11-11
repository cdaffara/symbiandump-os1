// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "swdirectgdiengine.h"
#include "swdirectgdidriverimpl.h"
#include "directgdiadapter.h"
#include <bitdrawinterfaceid.h>
#include <bmalphablend.h>
#include <graphics/bitmap.inl>
#include <graphics/gdi/gdiinline.inl>

/** 
@see MDirectGdiEngine::BitBlt()
@panic DGDIAdapter 7, aBitmap is invalid (debug only). 
*/
void CSwDirectGdiEngine::BitBlt(const TPoint& aDestPos, 
								const CFbsBitmap& aBitmap,	
								const TRect& aSrceRect)
	{
	if (aBitmap.ExtendedBitmapType() != KNullUid)
		{
		iDriver->SetError(KErrNotSupported); // Not supported for extended bitmaps
		return;
		}
	
	TRect srceRect(aSrceRect);
	const TPoint destPoint(aDestPos + iOrigin + srceRect.iTl - aSrceRect.iTl);
	const TPoint offset(srceRect.iTl - destPoint);

	TRect targetRect(destPoint,srceRect.Size());
	aBitmap.BeginDataAccess();

	CBitwiseBitmap* srce = static_cast<const CSwDirectGdiBitmap&>(aBitmap).Address();
	GRAPHICS_ASSERT_DEBUG(srce,EDirectGdiPanicInvalidBitmap);
	
	const TInt limit = iDefaultRegionPtr->Count();		
			
	TBool opaqueSource = (!IsAlphaChannel(aBitmap.DisplayMode())) && (iDrawMode == DirectGdi::EDrawModePEN);	

	TRect clipRect;
	for (TInt count = 0; count < limit; count++)
		{
		clipRect = (*iDefaultRegionPtr)[count];
		if(!clipRect.Intersects(targetRect))
			{
			continue;
			}
		
		clipRect.Intersection(targetRect);
		
		TRect clippedSrceRect(clipRect);
		clippedSrceRect.Move(offset);

		if (opaqueSource)
			{
			iDrawMode = DirectGdi::EDrawModeWriteAlpha; // write rather than blend.
			}		
		
		DoBitBlt(clipRect.iTl, srce, aBitmap.DataAddress(), aBitmap.DataStride(), clippedSrceRect);
		
		if (opaqueSource)
			{
			iDrawMode = DirectGdi::EDrawModePEN; // set it back to how it was.
			}
		
		iDrawDevice->UpdateRegion(clipRect);
		}
	
	aBitmap.EndDataAccess(ETrue);	
	}

/**
@see MDirectGdiEngine::BitBltMasked(const TPoint&, const CFbsBitmap&, const TRect&, const CFbsBitmap&, TBool)
@panic DGDIAdapter 7, if either aMaskBitmap or aBitmap are invalid (debug only).
*/
void CSwDirectGdiEngine::BitBltMasked(const TPoint& aDestPos,
			  const CFbsBitmap& aBitmap,
			  const TRect& aSrcRect,
			  const CFbsBitmap& aMaskBitmap,
			  TBool aInvertMask)
	{
	if ((aBitmap.ExtendedBitmapType() != KNullUid) || (aMaskBitmap.ExtendedBitmapType() != KNullUid))
		{
		iDriver->SetError(KErrNotSupported); // Not supported for extended bitmaps
		return;
		}
	
	TRect localSrcRect(aSrcRect);
	const TPoint destPoint(aDestPos + iOrigin + localSrcRect.iTl - aSrcRect.iTl);
	const TRect destRect(destPoint, localSrcRect.Size());
	const TPoint offset(localSrcRect.iTl - destPoint);
	
	TRect targetRect(destRect);
	aBitmap.BeginDataAccess();
	aMaskBitmap.BeginDataAccess();
	
	CBitwiseBitmap* srcebmp = static_cast<const CSwDirectGdiBitmap&>(aBitmap).Address();
	CBitwiseBitmap* maskbmp = static_cast<const CSwDirectGdiBitmap&>(aMaskBitmap).Address();		
	
	GRAPHICS_ASSERT_DEBUG(srcebmp,EDirectGdiPanicInvalidBitmap);
	GRAPHICS_ASSERT_DEBUG(maskbmp,EDirectGdiPanicInvalidBitmap);
	
	const TDisplayMode maskMode = maskbmp->DisplayMode();
	const TInt limit = iDefaultRegionPtr->Count();
	TBool opaqueSource = (!IsAlphaChannel(aBitmap.DisplayMode())) && (iDrawMode == DirectGdi::EDrawModePEN);
	TRect clipRect;
	for (TInt count = 0; count < limit; count++)
		{
		clipRect = (*iDefaultRegionPtr)[count];
		if (!clipRect.Intersects(targetRect))
			{
			continue;
			}
		
		clipRect.Intersection(targetRect);
		TRect clippedSrceRect(clipRect);
		clippedSrceRect.Move(offset);
		
		if (opaqueSource)
			{
			iDrawMode = DirectGdi::EDrawModeWriteAlpha; // ie write rather than blend
			}
		
		DoBitBltMasked(clipRect.iTl, srcebmp, aBitmap.DataAddress(), clippedSrceRect, maskbmp, 
				aMaskBitmap.DataAddress(), aInvertMask);
		
		if (opaqueSource)
			{
			iDrawMode = DirectGdi::EDrawModePEN; // set to default
			}
		iDrawDevice->UpdateRegion(clipRect);
		}
	
	aBitmap.EndDataAccess(ETrue);
	aMaskBitmap.EndDataAccess(ETrue);	
	}


/**
@see MDirectGdiEngine::BitBltMasked(const TPoint&, const CFbsBitmap&, const TRect&, const CFbsBitmap&, const TPoint&)
@panic DGDIAdapter 7, if either aBitmap or aMaskBitmap are invalid.
@panic DGDIAdapter 1022, if the top-left corner of aSrcRect is out of bounds (debug only).
*/
void CSwDirectGdiEngine::BitBltMasked(const TPoint& aDestPt,
		const CFbsBitmap& aBitmap, const TRect& aSrcRect,
		const CFbsBitmap& aMaskBitmap, const TPoint& aAlphaPt)
	{
	if ((aBitmap.ExtendedBitmapType() != KNullUid) || (aMaskBitmap.ExtendedBitmapType() != KNullUid))
		{
		iDriver->SetError(KErrNotSupported); // Not supported for extended bitmaps
		return;
		}
	
	TRect srcRect(aSrcRect);
	//Calculate the destination rect
	TPoint destPt(aDestPt + iOrigin);
	TRect destRect(destPt, srcRect.Size());
	TPoint offset(srcRect.iTl - destPt);
	TRect targetRect(destRect);

	aBitmap.BeginDataAccess();
	aMaskBitmap.BeginDataAccess();
	CBitwiseBitmap* srcBmp = static_cast<const CSwDirectGdiBitmap&>(aBitmap).Address();
	CBitwiseBitmap* alphaBmp = static_cast<const CSwDirectGdiBitmap&>(aMaskBitmap).Address();
	GRAPHICS_ASSERT_DEBUG(srcBmp, EDirectGdiPanicInvalidBitmap);
	GRAPHICS_ASSERT_DEBUG(alphaBmp, EDirectGdiPanicInvalidBitmap);
	TUint32* srcDataAddr = aBitmap.DataAddress();
	TUint32* alphaDataAddr = aMaskBitmap.DataAddress();

	//For each region - find the clipping rect and draw
	TInt limit = iDefaultRegionPtr->Count ();
	TRect clipRect;
	for (TInt count=0; count<limit;count++)
		{
		clipRect=(*iDefaultRegionPtr)[count];
		if ( !clipRect.Intersects(targetRect))
			{
			continue;
			}
		//targetRect was constructed from destRect. destRect was constructed from srcRect.
		clipRect.Intersection (targetRect);
		TRect clippedSrcRect(clipRect);
		clippedSrcRect.Move(offset);//clippedSrcRect - maps to a part of srcRect now.
		TPoint shift(clippedSrcRect.iTl - srcRect.iTl);
		GRAPHICS_ASSERT_DEBUG(shift.iX >= 0, EDirectGdiPanicNegativeShift);
		GRAPHICS_ASSERT_DEBUG(shift.iY >= 0, EDirectGdiPanicNegativeShift);
		DoBitBltAlpha (clipRect.iTl, srcBmp, srcDataAddr, clippedSrcRect,
				alphaBmp, alphaDataAddr, aAlphaPt + shift, EFalse);
		iDrawDevice->UpdateRegion (clipRect);
		}

	aBitmap.EndDataAccess(ETrue);
	aMaskBitmap.EndDataAccess(ETrue);
	return;
	}			

/** 
Calculates the position into the scanline for the given x coordinate.

@param aX The given x-coordinate.
@param aDisplayMode The applied display mode.
@return The memory offset, or 0 if the mode is not supported.
@panic DGDIAdapter 1009, if aDisplayMode is not supported (debug only).
*/
TUint CSwDirectGdiEngine::MemoryOffsetForPixelPitch(TUint aX, TDisplayMode aDisplayMode) const
	{	
	switch (aDisplayMode)
		{
		case EColor16MU:
		case EColor16MAP:
			return aX << 2;
		case EColor64K:
			return aX << 1;
		default:
			GRAPHICS_PANIC_DEBUG(EDirectGdiPanicInvalidDisplayMode);
		}
	return 0;
	}

TUint32* CSwDirectGdiEngine::GetScanLineOffsetPtr(CBitwiseBitmap* aSrce, TUint32*& aSlptr, 
		TInt aLength, TPoint aPixel, TUint32* aBase, 
		TLineScanningPosition& aLineScanningPosition, TUint aXOffset)
	{
	aSrce->GetScanLinePtr(aSlptr, aLength, aPixel, aBase, aLineScanningPosition);
	return (TUint32*)((TUint8*)aSlptr + aXOffset);
	}

/**
Performs the actual bitblt to the device.
This function may also be called by DrawBitmap(), and DrawRect() when using a patterned brush, 
so any changes to this function may impact on them also.

@pre aSrce A bitmap with non-zero dimensions. aSrceRect has been clipped against the target.

@param aDest The target position on the device which will contain the top left 
             corner of the source bitmap.
@param aSrce The bitmap object that contains the pixels to draw.
@param aBase The address of the bitmap pixels.
@param aStride The length in bytes between scanlines in memory.
@param aSrceRect The area of the bitmap to draw from.
@panic DGDIAdapter 1013, if aDest is fully outside of the bounds of the target, or aSrceRect.iTl is not
       within the drawing area (debug only).
*/
void CSwDirectGdiEngine::DoBitBlt(const TPoint& aDest,
		 CBitwiseBitmap* aSrce,
		 TUint32* aBase,
		 TInt aStride,
		 const TRect& aSrceRect)
	{
	// Does multiple bitmap widths for painting rects only
	const TInt width = aSrceRect.Width ();

#ifdef _DEBUG
	TRect deviceDestRect;
	iDrawDevice->GetDrawRect(deviceDestRect);
	GRAPHICS_ASSERT_DEBUG(aDest.iX >= deviceDestRect.iTl.iX, EDirectGdiPanicOutOfBounds);
	GRAPHICS_ASSERT_DEBUG(aDest.iY >= deviceDestRect.iTl.iY, EDirectGdiPanicOutOfBounds);
	GRAPHICS_ASSERT_DEBUG((aDest.iX + aSrceRect.Width()) <= deviceDestRect.iBr.iX, EDirectGdiPanicOutOfBounds);
	GRAPHICS_ASSERT_DEBUG((aDest.iY + aSrceRect.Height()) <= deviceDestRect.iBr.iY, EDirectGdiPanicOutOfBounds);
	GRAPHICS_ASSERT_DEBUG(aDest.iX >= 0 && aDest.iY >= 0, EDirectGdiPanicOutOfBounds);
	GRAPHICS_ASSERT_DEBUG(aSrceRect.iTl.iX >= 0 && aSrceRect.iTl.iY >= 0, EDirectGdiPanicOutOfBounds);
#endif

	TSize srcSize = aSrce->SizeInPixels ();
	
	const TPoint KZeroPoint(0,0);
	TAny* interface = NULL;
	if (iDrawMode == DirectGdi::EDrawModeWriteAlpha &&
			aSrceRect.iBr.iX <= srcSize.iWidth && 
			aSrceRect.iBr.iY <= srcSize.iHeight &&
			!aSrce->IsCompressed() && 
			aSrce->DisplayMode() == iDrawDevice->DisplayMode() && 
			iDrawDevice->GetInterface(KFastBlit2InterfaceID, interface) == KErrNone)
		{
		// Conditions in CFbsBitGc allow for optimised blitting.
		// The draw device supports the optimised blitting function.
		// Operation may fail regardless due to unacceptable conditions in the draw device.
		MFastBlit2* fastBlit = reinterpret_cast<MFastBlit2*>(interface);
		if (fastBlit && (fastBlit->WriteBitmapBlock(aDest, aBase, aStride, srcSize, aSrceRect) == KErrNone))			
			{
			return;
			}			
		}

	MFastBlend* fastBlend=NULL;
	if (FastBlendInterface(aSrce,NULL,fastBlend)==KErrNone)
		{
		if (fastBlend->FastBlendBitmap(aDest, aBase, aStride, srcSize, aSrceRect, aSrce->DisplayMode(), GcDrawMode(iDrawMode), CFbsDrawDevice::ENoShadow)== KErrNone)
			{
			return;
			}
		}
	
	const TInt scanLineBytes = iDrawDevice->ScanLineBytes();
	TUint32* scanLineBuffer = iDrawDevice->ScanLineBuffer();
	TPtr8 scanLineDes((TUint8*)scanLineBuffer, scanLineBytes, scanLineBytes);

	const TDisplayMode dispMode = ScanLineBufferDisplayMode(iDrawDevice);
	
	const TBool useScanLinePtr = (dispMode == aSrce->DisplayMode()) && 
		(TDisplayModeUtils::NumDisplayModeBitsPerPixel(dispMode) >= 8);

	TUint32* slptr = NULL;
	TUint offset = 0;
	TUint32* lastScanLine = NULL;
	if (useScanLinePtr)
		{
		lastScanLine = aSrce->ScanLineAddress(aBase, aSrceRect.iBr.iY-1);
		}

	TInt srceWidth = srcSize.iWidth;
	TInt partlinestart = aSrceRect.iTl.iX % srceWidth;
	
	if (partlinestart < 0)
		{
		partlinestart += srceWidth;
		}
	
	const TInt partlinelength = Min(srceWidth - partlinestart, width);
	TInt destX = aDest.iX;
	const TInt destXlimit = destX+width;
	const CGraphicsContext::TDrawMode drawMode = GcDrawMode(iDrawMode);

	// first part line
	if (partlinestart > 0 && partlinelength > 0)
		{
		TPoint srcecoord1(partlinestart, aSrceRect.iTl.iY);
		TInt desty = aDest.iY;

		TLineScanningPosition lineScanPos(aBase);

		if (useScanLinePtr)
			{
			offset = MemoryOffsetForPixelPitch(partlinestart, dispMode);
			if (aSrce->IsCompressed ())
				{
				
				while (srcecoord1.iY < aSrceRect.iBr.iY)
					{
					scanLineBuffer = GetScanLineOffsetPtr (aSrce, slptr,
							partlinelength, srcecoord1, aBase, lineScanPos, offset);
					if (srcecoord1.iY == aSrceRect.iTl.iY)
						{
						aSrce->SetCompressionBookmark(lineScanPos, aBase, NULL);
						}
					iDrawDevice->WriteLine(aDest.iX, desty, partlinelength,
							scanLineBuffer, drawMode);
					srcecoord1.iY++;
					desty++;				
					}
				}
			else
				{
				while (srcecoord1.iY < aSrceRect.iBr.iY)
					{
					scanLineBuffer = GetScanLineOffsetPtr (aSrce, slptr,
							partlinelength, srcecoord1, aBase, lineScanPos,
							offset);
					do
						{
						iDrawDevice->WriteLine (aDest.iX, desty,
								partlinelength, scanLineBuffer, drawMode);
						scanLineBuffer = (TUint32*)((TUint8*)scanLineBuffer + aStride);
						srcecoord1.iY++;
						desty++;
						}
					while ((srcecoord1.iY < aSrceRect.iBr.iY) && (scanLineBuffer < lastScanLine)) ;
					}
				}
			}
		else
			{
			for ( ; srcecoord1.iY < aSrceRect.iBr.iY; srcecoord1.iY++, desty++)
				{
				aSrce->GetScanLine (scanLineDes, srcecoord1, partlinelength,
						EFalse, KZeroPoint, dispMode, aBase, lineScanPos);
				if ( srcecoord1.iY==aSrceRect.iTl.iY)
					{
					aSrce->SetCompressionBookmark (lineScanPos, aBase, NULL);
					}
				iDrawDevice->WriteLine (aDest.iX, desty, partlinelength,
						scanLineBuffer, drawMode);
				}
			}

		destX += partlinelength;
		}

	// multiple complete lines - columns
	TInt numcolumns = (destXlimit - destX) / srceWidth;
	
	if (numcolumns > 0)
		{
		TPoint srcecoord2(0, aSrceRect.iTl.iY);
		TInt desty = aDest.iY;

		TLineScanningPosition lineScanPos(aBase);

		if (useScanLinePtr)
			{
			if (aSrce->IsCompressed())
				{
				while (srcecoord2.iY < aSrceRect.iBr.iY)
					{
					TPoint coord(srcecoord2);
					aSrce->GetScanLinePtr (slptr, srceWidth, coord, aBase, lineScanPos);
					if (srcecoord2.iY == aSrceRect.iTl.iY)
						{
						aSrce->SetCompressionBookmark(lineScanPos, aBase, NULL);
						}
					TInt tempdestX = destX;
					for (TInt count = 0; count < numcolumns; count++, tempdestX += srceWidth)
						{
						iDrawDevice->WriteLine(tempdestX, desty, srceWidth, slptr, drawMode);
						}
					srcecoord2.iY++;
					desty++;
					}
				}
			else
				{
				while (srcecoord2.iY < aSrceRect.iBr.iY)
					{
					TPoint coord(srcecoord2);
					aSrce->GetScanLinePtr (slptr, srceWidth, coord, aBase, lineScanPos);
					do
						{
						TInt tempdestX = destX;
						for (TInt count = 0; count < numcolumns; count++, tempdestX += srceWidth)
							{
							iDrawDevice->WriteLine (tempdestX, desty, srceWidth, slptr, drawMode);
							}
						slptr = (TUint32*)((TUint8*)slptr + aStride);
						srcecoord2.iY++;
						desty++;
						}
					while ((srcecoord2.iY < aSrceRect.iBr.iY) && (slptr < lastScanLine));
					}
				}
			}
		else
			{
			for (; srcecoord2.iY < aSrceRect.iBr.iY; srcecoord2.iY++, desty++)
				{
				TInt tempdestX = destX;
				TPoint coord(srcecoord2);
				aSrce->GetScanLinePtr (slptr, srceWidth, coord, aBase, lineScanPos);
				if (srcecoord2.iY == aSrceRect.iTl.iY)
					{
					aSrce->SetCompressionBookmark (lineScanPos, aBase, NULL);
					}
				for (TInt count = 0; count < numcolumns; count++, tempdestX += srceWidth)
					{
					aSrce->GetScanLine(slptr, scanLineDes, coord, srceWidth,
							EFalse, KZeroPoint, dispMode);
					iDrawDevice->WriteLine(tempdestX, desty, srceWidth, scanLineBuffer, drawMode);
					}
				}
			}

		destX += numcolumns * srceWidth;
		}

	// final part line
	if (destX < destXlimit)
		{
		const TInt restofline = destXlimit - destX;
		TPoint srcecoord3(0, aSrceRect.iTl.iY);
		TInt desty = aDest.iY;

		TLineScanningPosition lineScanPos(aBase);

		if (useScanLinePtr)
			{
			offset = 0;
			if (aSrce->IsCompressed())
				{
				while (srcecoord3.iY < aSrceRect.iBr.iY)
					{
					scanLineBuffer = GetScanLineOffsetPtr (aSrce, slptr,
							srceWidth, srcecoord3, aBase, lineScanPos, offset);
					if (srcecoord3.iY == aSrceRect.iTl.iY)
						{
						aSrce->SetCompressionBookmark (lineScanPos, aBase, NULL);
						}
					iDrawDevice->WriteLine(destX, desty, restofline, scanLineBuffer, drawMode);
					srcecoord3.iY++;
					desty++;
					}
				}
			else
				{
				while (srcecoord3.iY < aSrceRect.iBr.iY)
					{
					scanLineBuffer = GetScanLineOffsetPtr (aSrce, slptr,
							srceWidth, srcecoord3, aBase, lineScanPos, offset);
					do
						{
						iDrawDevice->WriteLine(destX, desty, restofline, scanLineBuffer, drawMode);
						scanLineBuffer = (TUint32*)((TUint8*)scanLineBuffer + aStride);
						srcecoord3.iY++;
						desty++;
						}
					while ((srcecoord3.iY < aSrceRect.iBr.iY) && (scanLineBuffer < lastScanLine));
					}
				}
			}
		else
			{
			for (; srcecoord3.iY < aSrceRect.iBr.iY; srcecoord3.iY++, desty++)
				{
				aSrce->GetScanLine (scanLineDes, srcecoord3, srceWidth, EFalse,
						KZeroPoint, dispMode, aBase, lineScanPos);
				if (srcecoord3.iY == aSrceRect.iTl.iY)
					{
					aSrce->SetCompressionBookmark (lineScanPos, aBase, NULL);
					}
				iDrawDevice->WriteLine(destX, desty, restofline, scanLineBuffer, drawMode);
				}
			}
		}
	}
/** 
Performs the masked bitblt to the device.

@param aDest The target position on the device which will contain the top left 
             corner of the source bitmap.
@param aSourceBitmap The bitmap object that contains the pixels to draw.
@param aSourceBase The address of the source bitmap pixels.
@param aSourceRect The area of the bitmap to draw from.
@param aMaskBitmap The bitmap object that contains the mask.
@param aMaskBase The address of the mask pixels.
@param aInvertMask Inverts the mask if ETrue.
@panic DGDIAdapter 1013, if aDest is outside of the device bounds (debug only).
*/
void CSwDirectGdiEngine::DoBitBltMasked(const TPoint& aDest,
		   CBitwiseBitmap* aSourceBitmap,
		   TUint32* aSourceBase,
		   const TRect& aSourceRect,
		   CBitwiseBitmap* aMaskBitmap,
		   TUint32* aMaskBase,
		   TBool aInvertMask)
	{
#ifdef _DEBUG
	TRect deviceDestRect;
	iDrawDevice->GetDrawRect (deviceDestRect);
#endif
	
    GRAPHICS_ASSERT_DEBUG (aDest.iX >= deviceDestRect.iTl.iX, EDirectGdiPanicOutOfBounds);
	GRAPHICS_ASSERT_DEBUG (aDest.iY >= deviceDestRect.iTl.iY, EDirectGdiPanicOutOfBounds);
	GRAPHICS_ASSERT_DEBUG ((aDest.iX + aSourceRect.Width()) <= deviceDestRect.iBr.iX, EDirectGdiPanicOutOfBounds);
	GRAPHICS_ASSERT_DEBUG ((aDest.iY + aSourceRect.Height()) <= deviceDestRect.iBr.iY, EDirectGdiPanicOutOfBounds);	
	const TPoint KZeroPoint(0,0);

	MFastBlend* fastBlend=NULL;
	if (FastBlendInterface(aSourceBitmap,aMaskBitmap,fastBlend)==KErrNone)
		{
		if (fastBlend->FastBlendBitmapMasked(aDest, aSourceBase, aSourceBitmap->DataStride(), aSourceBitmap->SizeInPixels(), aSourceRect, aSourceBitmap->DisplayMode(),
							aMaskBase, aMaskBitmap->DataStride(), aMaskBitmap->DisplayMode(), aMaskBitmap->SizeInPixels(), aSourceRect.iTl, aInvertMask,
							GcDrawMode(iDrawMode), CFbsDrawDevice::ENoShadow)==KErrNone)
			{
			return;
			}
		}
	
	if (aMaskBitmap->DisplayMode() == EGray256)
		{
		DoBitBltAlpha (aDest, aSourceBitmap, aSourceBase, aSourceRect,
				aMaskBitmap, aMaskBase, aSourceRect.iTl, EFalse);
		}
	// if screen driver is 16MAP we avoid logical operator pen modes by using DoBitBltAlpha() for blitting
	else if(iDrawDevice->ScanLineDisplayMode() == EColor16MAP)
		{
		DoBitBltAlpha (aDest, aSourceBitmap, aSourceBase, aSourceRect,
				aMaskBitmap, aMaskBase, aSourceRect.iTl, aInvertMask);
		}
	else if (aSourceBitmap == aMaskBitmap)
		{
		const TInt width = aSourceRect.Width();
		const TDisplayMode dispMode = ScanLineBufferDisplayMode(iDrawDevice);
		const CGraphicsContext::TDrawMode drawMode = aInvertMask ? CGraphicsContext::EDrawModeAND : CGraphicsContext::EDrawModeOR;
		TPoint srcePoint(aSourceRect.iTl);
		TInt destY = aDest.iY;
		
		TLineScanningPosition lineScanPos(aSourceBase);

		const TBool useScanLinePtr = (dispMode == aSourceBitmap->DisplayMode() && 
				(TDisplayModeUtils::NumDisplayModeBitsPerPixel(dispMode)>=8));

		if (useScanLinePtr)
			{
			TUint32* scanLineBuffer = NULL;
			TUint32* slptr = NULL;
			TUint offset = MemoryOffsetForPixelPitch (srcePoint.iX, dispMode);

			if (aSourceBitmap->IsCompressed())
				{
				for ( ; srcePoint.iY < aSourceRect.iBr.iY; destY++,
						srcePoint.iY++)
					{
					scanLineBuffer = GetScanLineOffsetPtr (
							aSourceBitmap, slptr, width, srcePoint,
							aSourceBase, lineScanPos, offset);
					
					iDrawDevice->WriteLine (aDest.iX, destY, width,
							scanLineBuffer, drawMode);
					}
				}
			else
				{
				TUint stride = aSourceBitmap->DataStride ();
				TUint32* lastScanLine = aSourceBitmap->ScanLineAddress(aSourceBase, aSourceRect.iBr.iY-1);

				while (srcePoint.iY < aSourceRect.iBr.iY)
					{
					scanLineBuffer = GetScanLineOffsetPtr(aSourceBitmap, slptr, width, srcePoint,
							aSourceBase, lineScanPos, offset);
					do
						{
						iDrawDevice->WriteLine (aDest.iX, destY, width, scanLineBuffer, drawMode);
						scanLineBuffer = (TUint32*)((TUint8*)scanLineBuffer + stride);
						destY++;
						srcePoint.iY++;
						}
					while ((srcePoint.iY < aSourceRect.iBr.iY) && (scanLineBuffer < lastScanLine));
					}
				}
			}
		else
			{
			const TInt scanLineBytes = iDrawDevice->ScanLineBytes();
			TUint32* scanLineBuffer = iDrawDevice->ScanLineBuffer();
			TPtr8 scanLineDes((TUint8*)scanLineBuffer, scanLineBytes,
					scanLineBytes);
			for (; srcePoint.iY < aSourceRect.iBr.iY; destY++,
					srcePoint.iY++)
				{
				aSourceBitmap->GetScanLine (scanLineDes, srcePoint,
						width, EFalse, KZeroPoint, dispMode,
						aSourceBase, lineScanPos);

				iDrawDevice->WriteLine (aDest.iX, destY, width,
						scanLineBuffer, drawMode);
				}
			}
		}
	else
		{
		DoBitBltMaskedFlicker(aDest, aSourceBitmap, aSourceBase,
				aSourceRect, aMaskBitmap, aMaskBase, aInvertMask);		
		}
	}
		
/**
@see DoBitBltMasked()
 */
void CSwDirectGdiEngine::DoBitBltMaskedFlicker(const TPoint& aDest,
				  CBitwiseBitmap* aSourceBitmap,
				  TUint32* aSourceBase,
				  const TRect& aSourceRect,
				  CBitwiseBitmap* aMaskBitmap,
				  TUint32* aMaskBase,
				  TBool aInvertMask)
	{
	const TInt width = aSourceRect.Width();
	TInt destY = aDest.iY;
	TPoint srcePoint(aSourceRect.iTl);
	
	TLineScanningPosition lineScanPos(aSourceBase);
	TLineScanningPosition lineScanPosMask(aMaskBase);
	
	const TDisplayMode srcFormat = aSourceBitmap->DisplayMode();
	const TDisplayMode maskFormat = aMaskBitmap->DisplayMode();
	
	if (aMaskBitmap->IsCompressed()) 
		{ 
		HBufC8* hBuf = CFbsBitmap::GetDecompressionBuffer(aMaskBitmap->DataStride() + 4);
		if (!hBuf) 
			{
			iDriver->SetError(KErrNoMemory);
			return; // Out of memory so do not draw anything 
			}
		lineScanPosMask.iScanLineBuffer = hBuf; 
		} 
	
	TAny* interface = NULL;
	if ( (srcFormat == EColor16MU || srcFormat == EColor64K ) &&
			maskFormat == EGray2 && 
			aSourceBitmap->SizeInPixels().iWidth <= aMaskBitmap->SizeInPixels().iWidth &&
			aSourceBitmap->SizeInPixels().iHeight <= aMaskBitmap->SizeInPixels().iHeight &&
			iDrawDevice->GetInterface(KFastBlitInterfaceID, interface) == KErrNone )
		{
		// Parameters allow optimised code path
		TInt length = width;
		TUint32* srcPtr=NULL;
		TUint32* maskPtr=NULL;
		MFastBlit* fastBlit = reinterpret_cast<MFastBlit*>(interface);
		while (srcePoint.iY < aSourceRect.iBr.iY)
			{
			aSourceBitmap->GetScanLinePtr(srcPtr, length, srcePoint, aSourceBase, lineScanPos);
			aMaskBitmap->GetScanLinePtr(maskPtr, length, srcePoint, aMaskBase, lineScanPosMask);
			
			fastBlit->WriteMaskLineEx(aDest.iX,destY,length,srcePoint.iX,srcPtr,srcFormat,srcePoint.iX,maskPtr,aInvertMask);
			
			destY++;
			++srcePoint.iY;
			}
		return;
		}
	
	const TDisplayMode dispMode = ScanLineBufferDisplayMode(iDrawDevice);
	const CGraphicsContext::TDrawMode drawMode = aInvertMask ? CGraphicsContext::EDrawModeAND : CGraphicsContext::EDrawModeANDNOT;
	
	TUint32* scanLineBuffer = iDrawDevice->ScanLineBuffer();
	const TInt scanLineBytes = iDrawDevice->ScanLineBytes();
	TPtr8 scanLineDes((TUint8*)scanLineBuffer,scanLineBytes,scanLineBytes);
	TLineScanningPosition lineScanPos2(aSourceBase);
	const TPoint KZeroPoint(0,0);
	
	//scanline modifications required if using different modes, bits per pixel less than 8
	if ( (dispMode == aSourceBitmap->DisplayMode()) && 
			(TDisplayModeUtils::NumDisplayModeBitsPerPixel(dispMode)>=8))
		{
		TUint offset = MemoryOffsetForPixelPitch(srcePoint.iX, dispMode);
		TUint32* slptr=NULL;
		//mask scanline modifications required for EInvertPen, different screen modes
		if ((drawMode != CGraphicsContext::EDrawModeANDNOT) && (dispMode == aMaskBitmap->DisplayMode()))
			{
			TUint32* scanLineBufferMask = NULL;
			//stride jumping not possible with compressed bitmaps
			if (aSourceBitmap->IsCompressed() || aMaskBitmap->IsCompressed())
				{
				for (; srcePoint.iY < aSourceRect.iBr.iY; destY++,srcePoint.iY++)
					{
					scanLineBuffer = GetScanLineOffsetPtr(aSourceBitmap, slptr, width, srcePoint, aSourceBase, lineScanPos, offset);
					iDrawDevice->WriteLine(aDest.iX,destY,width,scanLineBuffer, CGraphicsContext::EDrawModeXOR);
					scanLineBufferMask = GetScanLineOffsetPtr(aMaskBitmap, slptr, width, srcePoint, aMaskBase, lineScanPosMask, offset);
					iDrawDevice->WriteLine(aDest.iX,destY,width,scanLineBufferMask,drawMode);
					iDrawDevice->WriteLine(aDest.iX,destY,width,scanLineBuffer, CGraphicsContext::EDrawModeXOR);
					}
				}
			else
				{
				TUint strideSrc = aSourceBitmap->DataStride();
				TUint strideMask = aMaskBitmap->DataStride();
				TUint32* lastScanLineSrc = aSourceBitmap->ScanLineAddress(aSourceBase,aSourceRect.iBr.iY-1);
				TUint32* lastScanLineMask = aMaskBitmap->ScanLineAddress(aMaskBase,aSourceRect.iBr.iY-1);

				while (srcePoint.iY < aSourceRect.iBr.iY)
					{
					scanLineBuffer = GetScanLineOffsetPtr(aSourceBitmap, slptr, width, srcePoint, aSourceBase, lineScanPos, offset);
					scanLineBufferMask = GetScanLineOffsetPtr(aMaskBitmap, slptr, width, srcePoint, aMaskBase, lineScanPosMask, offset);
					do
						{
						iDrawDevice->WriteLine(aDest.iX,destY,width,scanLineBuffer,CGraphicsContext::EDrawModeXOR);
						iDrawDevice->WriteLine(aDest.iX,destY,width,scanLineBufferMask,drawMode);
						iDrawDevice->WriteLine(aDest.iX,destY,width,scanLineBuffer,CGraphicsContext::EDrawModeXOR);
						destY++;
						srcePoint.iY++;
						}
					while ((srcePoint.iY < aSourceRect.iBr.iY) && 
							(scanLineBuffer < lastScanLineSrc) && 
							(scanLineBufferMask < lastScanLineMask)	&& 
							((scanLineBuffer = (TUint32*)((TUint8*)scanLineBuffer + strideSrc))>(TUint32*)0) && 
							((scanLineBufferMask = (TUint32*)((TUint8*)scanLineBufferMask + strideMask))>(TUint32*)0) );
					}
				}
			}
		else
			{
			TUint32* scanLineBufferPtr = NULL;
			//stride jumping not possible with compressed bitmaps
			if (aSourceBitmap->IsCompressed())
				{
				for (; srcePoint.iY < aSourceRect.iBr.iY; destY++,srcePoint.iY++)
					{
					scanLineBufferPtr = GetScanLineOffsetPtr(aSourceBitmap, slptr, width, srcePoint, aSourceBase, lineScanPos, offset);
					iDrawDevice->WriteLine(aDest.iX,destY,width,scanLineBufferPtr,CGraphicsContext::EDrawModeXOR);
					aMaskBitmap->GetScanLine(scanLineDes,srcePoint,width,EFalse , KZeroPoint, dispMode, aMaskBase, lineScanPosMask);
					TileScanLine(scanLineDes, width, srcePoint, aMaskBitmap, lineScanPosMask, aMaskBase, dispMode);
					iDrawDevice->WriteLine(aDest.iX,destY,width,scanLineBuffer,drawMode);
					iDrawDevice->WriteLine(aDest.iX,destY,width,scanLineBufferPtr,CGraphicsContext::EDrawModeXOR);
					}
				}
			else
				{
				TUint stride = aSourceBitmap->DataStride();
				TUint32* lastScanLine = aSourceBitmap->ScanLineAddress(aSourceBase,aSourceRect.iBr.iY-1);
				while (srcePoint.iY < aSourceRect.iBr.iY)
					{
					scanLineBufferPtr = GetScanLineOffsetPtr(aSourceBitmap, slptr, width, srcePoint, aSourceBase, lineScanPos, offset);
					do
						{
						iDrawDevice->WriteLine(aDest.iX,destY,width,scanLineBufferPtr,CGraphicsContext::EDrawModeXOR);
						aMaskBitmap->GetScanLine(scanLineDes,srcePoint,width,EFalse, KZeroPoint, dispMode,aMaskBase, lineScanPosMask);
						TileScanLine(scanLineDes, width, srcePoint, aMaskBitmap, lineScanPosMask, aMaskBase, dispMode);
						iDrawDevice->WriteLine(aDest.iX,destY,width,scanLineBuffer,drawMode);
						iDrawDevice->WriteLine(aDest.iX,destY,width,scanLineBufferPtr,CGraphicsContext::EDrawModeXOR);
						destY++;
						srcePoint.iY++;
						}
					while ((srcePoint.iY < aSourceRect.iBr.iY) && (scanLineBufferPtr < lastScanLine) && 
							((scanLineBufferPtr = (TUint32*)((TUint8*)scanLineBufferPtr + stride))>(TUint32*)0));
					}
				}
			}
		}
	else
		{
		for (; srcePoint.iY < aSourceRect.iBr.iY; destY++,srcePoint.iY++)
			{
			aSourceBitmap->GetScanLine(scanLineDes,srcePoint,width,EFalse, KZeroPoint,
					dispMode,aSourceBase,lineScanPos);
			
			iDrawDevice->WriteLine(aDest.iX,destY,width,scanLineBuffer,CGraphicsContext::EDrawModeXOR);
			aMaskBitmap->GetScanLine(scanLineDes,srcePoint,width,EFalse , KZeroPoint, dispMode,
					aMaskBase, lineScanPosMask);
			TileScanLine(scanLineDes, width, srcePoint, aMaskBitmap, lineScanPosMask, aMaskBase, dispMode);
			iDrawDevice->WriteLine(aDest.iX,destY,width,scanLineBuffer,drawMode);
			aSourceBitmap->GetScanLine(scanLineDes,srcePoint,width,EFalse , KZeroPoint ,dispMode,
					aSourceBase,lineScanPos2);		
			
			iDrawDevice->WriteLine(aDest.iX,destY,width,scanLineBuffer,CGraphicsContext::EDrawModeXOR);
			}
		}
	}		
				
/**
@see DoBitBltMasked()
 */
void CSwDirectGdiEngine::DoBitBltAlpha(const TPoint& aDest ,CBitwiseBitmap* aSourceBitmap,
		TUint32* aSourceBase, const TRect& aSourceRect,
		CBitwiseBitmap* aMaskBitmap, TUint32* aMaskBase,
		const TPoint& aAlphaPoint, TBool aInvertMask)
	{
	MFastBlend* fastBlend=NULL;
	if (FastBlendInterface(aSourceBitmap,aMaskBitmap,fastBlend)==KErrNone)
		{
		if (fastBlend->FastBlendBitmapMasked(aDest, aSourceBase, aSourceBitmap->DataStride(), aSourceBitmap->SizeInPixels(), aSourceRect, aSourceBitmap->DisplayMode(),
							aMaskBase, aMaskBitmap->DataStride(), aMaskBitmap->DisplayMode(), aMaskBitmap->SizeInPixels(), aAlphaPoint, aInvertMask,
							GcDrawMode(iDrawMode), CFbsDrawDevice::ENoShadow)==KErrNone)
			{
			return;
			}
		}

	const TPoint KZeroPoint(0,0);
	const TInt KScanLineLength = 256;
	const TInt KRgbSize = 4;
	
	TUint8 srceRgbBuffer[KScanLineLength * KRgbSize];
	TUint8 maskBuffer[KScanLineLength];
	TUint8* srceRgbBufferPtr(srceRgbBuffer);
	
	TPtr8 srceRgbDes(srceRgbBuffer, KScanLineLength * KRgbSize, KScanLineLength * KRgbSize);
	TPtr8 maskDes(maskBuffer, KScanLineLength, KScanLineLength);	
	
	TInt srceY = aSourceRect.iTl.iY;
	TInt destY = aDest.iY;
	TInt alphaY = aAlphaPoint.iY;
	
	TLineScanningPosition lineScanPosSrc(aSourceBase);
	TLineScanningPosition lineScanPosMask(aMaskBase);
	TDisplayMode sourceMode = aSourceBitmap->DisplayMode();
	
	if (aMaskBitmap->IsCompressed())
		{
		HBufC8* hBuf= CFbsBitmap::GetDecompressionBuffer(aMaskBitmap->DataStride());
		if (hBuf == NULL)
			{
			iDriver->SetError(KErrNoMemory); // Out of memory so do not draw anything
			return;
			}
		lineScanPosMask.iScanLineBuffer = hBuf;
		}
	
	TAny* interface = NULL;
	if ( (sourceMode == EColor16MU || sourceMode == EColor64K) &&
		aMaskBitmap->DisplayMode() == EGray256 && // ensure a monochrome mask isn't passed in as an alpha channel
		aSourceBitmap->SizeInPixels().iWidth <= aMaskBitmap->SizeInPixels().iWidth &&
		aSourceBitmap->SizeInPixels().iHeight <= aMaskBitmap->SizeInPixels().iHeight &&
		iDrawDevice->GetInterface(KFastBlitInterfaceID, interface) == KErrNone )
		{
		TInt length = aSourceRect.Width();
		const TInt srceX = aSourceRect.iTl.iX;
		const TInt alphaX = aAlphaPoint.iX;
		const TInt destX = aDest.iX;
		MFastBlit* fastBlit = reinterpret_cast<MFastBlit*>(interface);
		
		while (srceY < aSourceRect.iBr.iY)
			{
			TUint32* srcPtr;
			TUint32* maskPtr;
			TPoint srcPoint(srceX, srceY);
			TPoint maskPoint(alphaX, alphaY);
			
			aSourceBitmap->GetScanLinePtr(srcPtr, length, srcPoint, aSourceBase, lineScanPosSrc);
			aMaskBitmap->GetScanLinePtr(maskPtr, length, maskPoint, aMaskBase, lineScanPosMask);
			
			fastBlit->WriteAlphaLineEx(destX, destY, length, srceX, srcPtr,
					sourceMode, alphaX, maskPtr, MAlphaBlend::EShdwBefore);
			srceY++;
			destY++;
			alphaY++;
			}
		
		return;
		}
	
	const TBool useScanLinePtr = ( (EColor16MA == aSourceBitmap->DisplayMode()));
	TUint32* slptr = NULL;
	TUint offset = 0;
	
	while (srceY < aSourceRect.iBr.iY)
		{
		TInt srceX = aSourceRect.iTl.iX;
		TInt destX = aDest.iX;
		TInt alphaX = aAlphaPoint.iX;
		
		while (srceX < aSourceRect.iBr.iX)
			{
			TPoint srcePoint(srceX,srceY);
			TPoint alphaPoint(alphaX,alphaY);
			const TInt width = Min(KScanLineLength, aSourceRect.iBr.iX - srceX);
			
			if (useScanLinePtr)
				{
				offset = MemoryOffsetForPixelPitch(srceX, EColor16MU);
				srceRgbBufferPtr = (TUint8*)GetScanLineOffsetPtr(aSourceBitmap, slptr, width, 
						                       srcePoint, aSourceBase, lineScanPosSrc, offset);
				}
			else
				{
				aSourceBitmap->GetScanLine(srceRgbDes,srcePoint,width,EFalse,KZeroPoint,
						               ERgb,aSourceBase,lineScanPosSrc);
				}
			
			aMaskBitmap->GetScanLine(maskDes, alphaPoint, width, EFalse, KZeroPoint,
					                EGray256, aMaskBase, lineScanPosMask);
			TileScanLine(maskDes, width, alphaPoint, aMaskBitmap, lineScanPosMask, aMaskBase, EGray256);
			
			// aInvertMask is not used for alpha channels (EGray256 mask)
			if (aInvertMask && aMaskBitmap->DisplayMode() != EGray256)
				{
				for (TInt i = 0; i < width; ++i)
					{
					maskBuffer[i] = ~maskBuffer[i];
					}
				}
			
			iDrawDevice->WriteRgbAlphaLine(destX, destY, width, srceRgbBufferPtr, maskBuffer, GcDrawMode(iDrawMode));
			
			srceX += KScanLineLength;
			destX += KScanLineLength;
			alphaX += KScanLineLength;
			}
		
		srceY++;
		destY++;
		alphaY++;
		}		
	}

/**
Tiles the scan line if its length in pixels is less than aLengthInPixels.

@param aScanLine A pointer to the scan line buffer.
@param aLengthInPixels The scan line size in pixels.
@param aSrcPt Position of the first pixel in aMaskBitmap that should be used as a source
              for the pixels in scan line buffer.
@param aMaskBitmap Any additional pixels for the scan line buffer will be taken from here.
@param aScanLinePos This argument is used for some internal optimisations. It should not be
                    modified by the caller.
@param aMaskBase The base address of aMaskBitmap data.
@param aDisplayMode Any additional pixels should be taken from aMaskBitmap using aDisplayMode
                    as an argument for GetScanLine() call.
@panic DGDIAdapter 1021, if the memory required for the scanline is greater than the size of aScanLine (debug only).
*/
void CSwDirectGdiEngine::TileScanLine(TPtr8& aScanLine,
						  TInt aLengthInPixels,
						  const TPoint& aSrcPt,
						  const CBitwiseBitmap* aMaskBitmap,
						  TLineScanningPosition& aScanLinePos,
						  TUint32* aMaskBase,
						  TDisplayMode aDisplayMode
						  )
	{
	TInt lengthInBytes = CFbsBitmap::ScanLineLength(aLengthInPixels, aDisplayMode);
	GRAPHICS_ASSERT_DEBUG(lengthInBytes <= aScanLine.MaxLength(), EDirectGdiPanicInvalidArg);
	TInt scanLineLength = aScanLine.Length();
	if(scanLineLength < lengthInBytes && aSrcPt.iX > 0)
		{
		//If, for example, src bmp is 100 pixels width, mask bmp is 20 pixels width and src
		//rect is (10, 0, 100, 1) -> We will read only pixels 10..19 from the first scan line
		//of the mask bmp. We have to have 90 mask bmp pixels.
		//So we have to make a second mask bmp read startig from pixel 0 - 10 pixels length.
		TInt maxLen = Min(aScanLine.MaxLength() - scanLineLength, aSrcPt.iX);
		TPtr8 maskDes2(const_cast <TUint8*> (aScanLine.Ptr()) + scanLineLength, maxLen, maxLen);
		TPoint srcPt(0, aSrcPt.iY);
		TPoint zeroPt(0, 0);
		aMaskBitmap->GetScanLine(maskDes2, srcPt, maxLen, EFalse, zeroPt, aDisplayMode, aMaskBase, aScanLinePos);
		aScanLine.SetLength(scanLineLength + maskDes2.Length());
		scanLineLength = aScanLine.Length();
		}
	if(scanLineLength >= lengthInBytes || scanLineLength == 0)
		{
		return;
		}
	//If we still don't have enough mask bmp pixels - we have to tile the scan line
	TInt repeatCnt = lengthInBytes / scanLineLength - 1;
	TInt bytesLeft = lengthInBytes % scanLineLength;
	const TUint8* src = aScanLine.Ptr();
	TUint8* dest = const_cast <TUint8*> (src) + scanLineLength;
	for(;repeatCnt>0;dest+=scanLineLength,repeatCnt--)
		{
		Mem::Copy(dest, src, scanLineLength);
		}
	if(bytesLeft)
		{
		Mem::Copy(dest, src, bytesLeft);
		}
	aScanLine.SetLength(lengthInBytes);
	}

/**
Draws a masked rectangular section of the source bitmap and does a compress/stretch to 
fit a given destination rectangle. It uses DoBitBltMasked() if no stretching is involved. 

@see DrawBitmapMasked()

@param aDestRect The target position on the device containing the top left corner of the source bitmap.
@param aSourceBitmap The bitmap object that contains the pixels to draw.
@param aSourceBase The address of the source bitmap pixels.
@param aSourceRect The area of the bitmap to draw from.
@param aMaskBitmap The bitmap object that contains the mask.
@param aMaskBase The address of the mask pixels.
@param aInvertMask Inverts the mask if ETrue.
@param aClipRect A clipping rectangle.
@panic DGDIAdapter 1013, if the clipping rectangle is fully outside of the device bounds (debug only).
*/
void CSwDirectGdiEngine::DoDrawBitmapMasked(const TRect& aDestRect,
							   CBitwiseBitmap* aSourceBitmap,
							   TUint32* aSourceBase,
							   const TRect& aSourceRect,
							   CBitwiseBitmap* aMaskBitmap,
							   TUint32* aMaskBase,
							   TBool aInvertMask,
							   const TRect& aClipRect)
	{
	CFbsDrawDevice* drawDevice = iDrawDevice;
#ifdef _DEBUG
	TRect deviceDestRect;
	drawDevice->GetDrawRect(deviceDestRect);
	GRAPHICS_ASSERT_DEBUG(aClipRect.iTl.iX >= deviceDestRect.iTl.iX, EDirectGdiPanicOutOfBounds);
	GRAPHICS_ASSERT_DEBUG(aClipRect.iTl.iY >= deviceDestRect.iTl.iY, EDirectGdiPanicOutOfBounds);
	GRAPHICS_ASSERT_DEBUG(aClipRect.iBr.iX <= deviceDestRect.iBr.iX, EDirectGdiPanicOutOfBounds);
	GRAPHICS_ASSERT_DEBUG(aClipRect.iBr.iY <= deviceDestRect.iBr.iY, EDirectGdiPanicOutOfBounds);
#endif

	// The clipped version of the destination rectangle
	TRect clippedDestRect(aDestRect);
	clippedDestRect.Intersection(aClipRect);

	// If the source rectangle and the destination rectangle are same,
	// no stretch/compress operation required, just do BitBltMasked
	if (aDestRect.Size() == aSourceRect.Size())
		{
		if (!clippedDestRect.IsEmpty())
			{
			const TPoint destPoint(clippedDestRect.iTl);
			clippedDestRect.Move(aSourceRect.iTl - aDestRect.iTl);
			DoBitBltMasked(destPoint,
						   aSourceBitmap,
						   aSourceBase,
						   clippedDestRect,
						   aMaskBitmap,
						   aMaskBase,
						   aInvertMask);
			}
		return;
		}

	MFastBlend* fastBlend=NULL;
	if (FastBlendInterface(aSourceBitmap,aMaskBitmap,fastBlend)==KErrNone)
		{
		if (fastBlend->FastBlendBitmapMaskedScaled(aClipRect, aDestRect, aSourceRect, aSourceBase, aSourceBitmap->DataStride(),
				aSourceBitmap->DisplayMode(),aSourceBitmap->SizeInPixels(), 
				aMaskBase, aMaskBitmap->DataStride(), aMaskBitmap->DisplayMode(), aMaskBitmap->SizeInPixels(), aInvertMask, 
				GcDrawMode(iDrawMode), CFbsDrawDevice::ENoShadow)==KErrNone)
			{
			return;
			}
		}
	
	TUint32* scanLineBuffer = drawDevice->ScanLineBuffer();
	const TInt scanLineBytes = drawDevice->ScanLineBytes();
	TPtr8 scanLineDes(reinterpret_cast<TUint8*>(scanLineBuffer),scanLineBytes,scanLineBytes);

	const TInt KScanLineLength = 256;
	const TInt KRgbSize = 4;
	TUint8 maskBuffer[KScanLineLength];

	TUint8 sourceBuffer[KScanLineLength*KRgbSize];
	TPtr8 sourceDes(sourceBuffer,KScanLineLength*KRgbSize,KScanLineLength*KRgbSize);

	const TDisplayMode dispMode = ScanLineBufferDisplayMode(drawDevice);
	CGraphicsContext::TDrawMode drawMode = aInvertMask ? CGraphicsContext::EDrawModeAND : CGraphicsContext::EDrawModeANDNOT;
	// If the source bitmap and the mask bitmap are same, draw the source bitmap either
	// with EDrawModeAND or EDrawModeOR based on aInvertMask parameter.
	if (aSourceBitmap == aMaskBitmap)
		{
		drawMode = aInvertMask ? CGraphicsContext::EDrawModeAND : CGraphicsContext::EDrawModeOR;
		}

	TLinearDDA xLine;
	TInt bitmapXStart = 0;
	xLine.Construct(TPoint(aSourceRect.iTl.iX,aDestRect.iTl.iX),
					TPoint(aSourceRect.iBr.iX,aDestRect.iBr.iX),TLinearDDA::ELeft);
	xLine.JumpToYCoord2(bitmapXStart,clippedDestRect.iTl.iX);

	TLinearDDA yLine;
	TPoint yCoord(aSourceRect.iTl.iY,aDestRect.iTl.iY);
	yLine.Construct(yCoord,TPoint(aSourceRect.iBr.iY,aDestRect.iBr.iY),TLinearDDA::ELeft);
	TInt dummy;
	yLine.JumpToYCoord2(dummy,clippedDestRect.iTl.iY);
	yCoord.SetXY(dummy,clippedDestRect.iTl.iY);

	const TInt srceWidth = aSourceRect.Width();
	const TInt destWidth = aDestRect.Width();
	const TInt clipWidth = clippedDestRect.Width();
	const TInt clipStrch = clippedDestRect.iTl.iX - aDestRect.iTl.iX;
	const TInt sourceBmpWidth = aSourceBitmap->SizeInPixels().iWidth;
	const TInt maskWidth = aMaskBitmap->SizeInPixels().iWidth;
	const TInt maskHeight = aMaskBitmap->SizeInPixels().iHeight;

	TLineScanningPosition lineScanPos(aSourceBase);
	TLineScanningPosition lineScanPos2(aSourceBase);
	TLineScanningPosition lineScanPosMask(aMaskBase);

	HBufC8* alphaBuffer = NULL;
	TPtr8 alphaBufferDes(NULL, 0);
	const TDisplayMode maskDisplayMode = aMaskBitmap->DisplayMode();

	// Mask inversion is not supported if the original source mask format is EGray256.
	// Note that this is only used for pre-multiplied alpha targets.
	TUint8 maskInverter = (aInvertMask && maskDisplayMode != EGray256) ? 0xFF : 0x00;

	if (aSourceBitmap != aMaskBitmap)
		{
		// Get buffer to be used to convert non-EGray256 masks to EGray256 when display mode is EColor16MAP
		// or to tile the mask when the mask width is smaller than the source bitmap width.
		if (maskDisplayMode != EGray256 && (dispMode == EColor16MAP || maskWidth < sourceBmpWidth))
			{
			alphaBuffer = CFbsBitmap::GetExtraBuffer(CFbsBitmap::ScanLineLength(maskWidth, EGray256));
			if (!alphaBuffer)
				{
				return;  // Out of memory so do not draw anything 
				}
			alphaBufferDes.Set(alphaBuffer->Des());
			}

		// Get buffer to be used for decompressing compressed masks when mask is EGray256
		if (maskDisplayMode == EGray256 && aMaskBitmap->IsCompressed())
			{
			HBufC8* hBuf= CFbsBitmap::GetDecompressionBuffer(aMaskBitmap->DataStride());
			if (!hBuf)
				{
				return;  // Out of memory so do not draw anything
				}
			lineScanPosMask.iScanLineBuffer = hBuf;
			}
		}
	const TPoint KZeroPoint(0,0);

	while (yCoord.iY < clippedDestRect.iBr.iY)
		{
		// Draw only the source bitmap, if the source bitmap and the mask bitmap are same.
		// else draw both the bitmaps
		if (aSourceBitmap == aMaskBitmap)
			{
			aSourceBitmap->StretchScanLine(scanLineDes,TPoint(bitmapXStart,yCoord.iX),
								 clipStrch,clipWidth,destWidth,aSourceRect.iTl.iX,
								 srceWidth, KZeroPoint,dispMode,aSourceBase,lineScanPos);
			if (yCoord.iY==clippedDestRect.iTl.iY)
				aSourceBitmap->SetCompressionBookmark(lineScanPos,aSourceBase,NULL);
			drawDevice->WriteLine(clippedDestRect.iTl.iX,yCoord.iY,clipWidth,scanLineBuffer,drawMode);
			}
		else if ((maskDisplayMode == EGray256) || (dispMode == EColor16MAP))
			{
			// Stretch the source bitmap and the mask bitmap for KScanLineLength as stretch length
			// then do alpha blending for this length. If the length is more then KScanLineLength
			// repeat it till you stretch complete destination length.
			const TPoint startPt(bitmapXStart,yCoord.iX);
			TInt clipWidthPart = clippedDestRect.Width();
			TBool loopLast = ETrue;
			if(clipWidthPart > KScanLineLength)
				{
				clipWidthPart = KScanLineLength;
				loopLast = EFalse;
				}
			TInt clipIncStrch = clippedDestRect.iTl.iX - aDestRect.iTl.iX;
			TInt startClip=clippedDestRect.iTl.iX;
			TPoint sourceDestXCoords(bitmapXStart,clippedDestRect.iTl.iX);
			xLine.Construct(TPoint(aSourceRect.iTl.iX,aDestRect.iTl.iX),
							TPoint(aSourceRect.iBr.iX,aDestRect.iBr.iX),TLinearDDA::ELeft);
			xLine.JumpToYCoord2(sourceDestXCoords.iX,sourceDestXCoords.iY);
			TPoint srcPixel(bitmapXStart % maskWidth,yCoord.iX % maskHeight);
			TInt spaceLeft = 0;
			TRgb maskRgbValue;
			TUint32* maskScanLinePtr32 = NULL;
			TPoint currentYValue(0,srcPixel.iY);
			aMaskBitmap->GetScanLinePtr(maskScanLinePtr32, currentYValue, maskWidth, aMaskBase, lineScanPosMask);
			// To implement non EGray256 mask support with EColor16MAP display mode, we convert
			// the mask to EGray256.
			if (maskDisplayMode != EGray256) // Convert scan-line to EGray256 and set maskScanLinePtr32 to the conversion.
				{
				aMaskBitmap->GetScanLine(maskScanLinePtr32, alphaBufferDes, currentYValue, maskWidth, EFalse, TPoint(0, 0), EGray256);
				maskScanLinePtr32 = (TUint32*)alphaBuffer->Ptr();
				}
			TUint8* maskScanLinePtr = reinterpret_cast<TUint8*>(maskScanLinePtr32);

			// Outer loop over all KScanLineLengths
			FOREVER
				{
				aSourceBitmap->StretchScanLine(sourceDes,startPt,clipIncStrch,clipWidthPart,destWidth,
								aSourceRect.iTl.iX,srceWidth, KZeroPoint ,EColor16MU,aSourceBase,lineScanPos);
				// Inner loop to tile the mask if necessary
				spaceLeft = clipWidthPart;
				do	{
					srcPixel.iX = sourceDestXCoords.iX % maskWidth;
			
					// Invert the mask using the inversion mask.
					maskBuffer[(sourceDestXCoords.iY-clippedDestRect.iTl.iX)%KScanLineLength]=
						maskInverter^maskScanLinePtr[srcPixel.iX];
					xLine.NextStep(sourceDestXCoords);
					} while (--spaceLeft>0);
				
				if (yCoord.iY == clippedDestRect.iTl.iY && startClip == clippedDestRect.iTl.iX)
					{
					aSourceBitmap->SetCompressionBookmark(lineScanPos,aSourceBase,NULL);
					aMaskBitmap->SetCompressionBookmark(lineScanPosMask,aMaskBase,NULL);
					}
				drawDevice->WriteRgbAlphaLine(startClip,yCoord.iY,clipWidthPart,sourceBuffer,maskBuffer, GcDrawMode(iDrawMode));
				if (loopLast)
					{
					break;
					}
				startClip+=KScanLineLength;
				if (clippedDestRect.iBr.iX - startClip <= KScanLineLength)
 					{
					loopLast = ETrue;
					clipWidthPart = clippedDestRect.iBr.iX - startClip;
					}
				clipIncStrch += KScanLineLength;
				}
			}
		else
			{
			aSourceBitmap->StretchScanLine(scanLineDes,TPoint(bitmapXStart,yCoord.iX),
									 clipStrch,clipWidth,destWidth,aSourceRect.iTl.iX,
									 srceWidth, KZeroPoint ,dispMode,aSourceBase,lineScanPos2);
			drawDevice->WriteLine(clippedDestRect.iTl.iX,yCoord.iY,clipWidth,scanLineBuffer,CGraphicsContext::EDrawModeXOR);

			TInt maskXStart = bitmapXStart % maskWidth;
			if(maskWidth < sourceBmpWidth)
				{
				TPoint sourceDestXCoords(bitmapXStart,clippedDestRect.iTl.iX);
				xLine.Construct(TPoint(aSourceRect.iTl.iX,aDestRect.iTl.iX),
								TPoint(aSourceRect.iBr.iX,aDestRect.iBr.iX),TLinearDDA::ELeft);
				xLine.JumpToYCoord2(sourceDestXCoords.iX,sourceDestXCoords.iY);
				TPoint srcPixel(maskXStart,yCoord.iX % maskHeight);
				TInt spaceLeft = clipWidth;
				TPoint prevSourceDestXCoords(-1,-1);
				TRgb maskRgbValue;
				aMaskBitmap->GetScanLine(alphaBufferDes, TPoint(0,srcPixel.iY), maskWidth, EFalse, TPoint(0, 0), EGray256, aMaskBase, lineScanPosMask);

				// Loop to tile the mask
				do	{
					if (sourceDestXCoords.iY != prevSourceDestXCoords.iY)
						{
						if (sourceDestXCoords.iX != prevSourceDestXCoords.iX)
							{
							srcPixel.iX = sourceDestXCoords.iX % maskWidth;
							if (srcPixel.iX < 0)
								srcPixel.iX += maskWidth;
							maskRgbValue = TRgb::Gray256((*alphaBuffer)[srcPixel.iX]);
							}
						drawDevice->WriteRgb(sourceDestXCoords.iY,yCoord.iY,maskRgbValue,drawMode);
						spaceLeft--;
						}
					prevSourceDestXCoords = sourceDestXCoords;
					xLine.SingleStep(sourceDestXCoords);
					} while (spaceLeft > 0);
				}
			else
				{
				// No need to tile the mask
				aMaskBitmap->StretchScanLine(scanLineDes,TPoint(maskXStart,yCoord.iX % maskHeight),
										clipStrch,clipWidth,destWidth,aSourceRect.iTl.iX,
										srceWidth, KZeroPoint ,dispMode,aMaskBase,lineScanPosMask);
				drawDevice->WriteLine(clippedDestRect.iTl.iX,yCoord.iY,clipWidth,scanLineBuffer,drawMode);
				// Redo stretching of the aSourceBitmap scanline
				aSourceBitmap->StretchScanLine(scanLineDes,TPoint(bitmapXStart,yCoord.iX),
									 	clipStrch,clipWidth,destWidth,aSourceRect.iTl.iX,
									 	srceWidth, KZeroPoint ,dispMode,aSourceBase,lineScanPos2);
				}

			if (yCoord.iY==clippedDestRect.iTl.iY)
				{
				aSourceBitmap->SetCompressionBookmark(lineScanPos2,aSourceBase,NULL);
				aMaskBitmap->SetCompressionBookmark(lineScanPosMask,aMaskBase,NULL);
				}

			drawDevice->WriteLine(clippedDestRect.iTl.iX,yCoord.iY,clipWidth,scanLineBuffer,CGraphicsContext::EDrawModeXOR);
			}
		yLine.NextStep(yCoord);
		}
	}

TInt CSwDirectGdiEngine::FastBlendInterface(const CBitwiseBitmap* aSource, const CBitwiseBitmap* aMask, MFastBlend*& aFastBlend) const
	{
	#if defined(__ALLOW_FAST_BLEND_DISABLE__)
	if (iFastBlendDisabled)
		return(KErrNotSupported);
	#endif
	if ((aSource && aSource->IsCompressed()) || (aMask && aMask->IsCompressed()))
		return(KErrNotSupported);
	TAny* interface=NULL;
	TInt ret= iDrawDevice->GetInterface(KFastBlendInterfaceID, interface);
	aFastBlend=(MFastBlend*)interface;
	return(ret);
	}

/*
Returns the pixel-format to be used when extracting a scan-line through CBitwiseBitmap::GetScanLine(), CBitwiseBitmap::GetVerticalScanLine(), and CBitwiseBitmap::StretchScanLine() for consumption by CFbsDrawDevice::WriteLine() and associated methods.

@see CBitwiseBitmap::GetScanLine()
@see CBitwiseBitmap::GetVerticalScanLine()
@see CBitwiseBitmap::StretchScanLine()
@see CFbsDrawDevice::WriteLine()
@internalComponent
*/
TDisplayMode CSwDirectGdiEngine::ScanLineBufferDisplayMode(CFbsDrawDevice* aDrawDevice)
	{
	return iDrawMode == DirectGdi::EDrawModeWriteAlpha ? aDrawDevice->DisplayMode() : aDrawDevice->ScanLineDisplayMode();
	}
