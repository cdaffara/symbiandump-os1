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
#include "swdirectgdibitmap.h"
#include "swdirectgdidriverimpl.h"
#include <bmalphablend.h>
#include <graphics/bitmap.inl>
#include <graphics/gdi/gdiinline.inl>

/**
@see MDirectGdiEngine::DrawBitmap(cosnt TRect&, const CFbsBitmap&, const TRect&)
@panic DGDIAdapter 7, if the bitmap is invalid (debug only). 
*/
void CSwDirectGdiEngine::DrawBitmap(const TRect& aDestRect,
									const CFbsBitmap& aSource,
									const TRect& aSourceRect)
	{
	if (aSource.ExtendedBitmapType() != KNullUid)
		{
		iDriver->SetError(KErrNotSupported); // Not supported for extended bitmaps
		return;
		}
	
	TRect destRect(aDestRect);
	destRect.Move(iOrigin);
	TRect targetRect(destRect);		 
	const CSwDirectGdiBitmap& srce = static_cast<const CSwDirectGdiBitmap&>(aSource);	

	CBitwiseBitmap* bmp = srce.Address();
	GRAPHICS_ASSERT_DEBUG(bmp,EDirectGdiPanicInvalidBitmap);

	const TInt limit = iDefaultRegionPtr->Count();
	const TBool opaqueSource = (!IsAlphaChannel(aSource.DisplayMode())) && (iDrawMode == DirectGdi::EDrawModePEN);
	if (opaqueSource)
		{
		iDrawMode = DirectGdi::EDrawModeWriteAlpha;
		}

	TRect clipRect(0,0,0,0);
	for (TInt count = 0; count < limit; count++)
		{
		clipRect = (*iDefaultRegionPtr)[count];
		if (!clipRect.Intersects(targetRect))
			{
			continue;
			}

		clipRect.Intersection(targetRect);
		DoDrawBitmap(destRect,bmp,aSource.DataAddress(),aSource.DataStride(),aSourceRect, clipRect);

		iDrawDevice->UpdateRegion(clipRect);
		}
	if (opaqueSource)
		{
		iDrawMode = DirectGdi::EDrawModePEN;
		}
	}

/*
Draws the bitmap. If aSrceRect equals aDestRect, a DoBitBlt() is performed instead.
@panic DGDIAdapter 1013, if the clipping rectangle is dully outside the destination bounds (debug only).
*/
void CSwDirectGdiEngine::DoDrawBitmap(const TRect& aDestRect,
							 CBitwiseBitmap* aBitmap,
							 TUint32* aBase,
							 TInt aStride,
							 const TRect& aSrceRect,
							 const TRect& aClipRect)
	{
#ifdef _DEBUG
	TRect deviceDestRect;
	iDrawDevice->GetDrawRect(deviceDestRect);
	GRAPHICS_ASSERT_DEBUG(aClipRect.iTl.iX >= deviceDestRect.iTl.iX, EDirectGdiPanicOutOfBounds);
	GRAPHICS_ASSERT_DEBUG(aClipRect.iTl.iY >= deviceDestRect.iTl.iY, EDirectGdiPanicOutOfBounds);
	GRAPHICS_ASSERT_DEBUG(aClipRect.iBr.iX <= deviceDestRect.iBr.iX, EDirectGdiPanicOutOfBounds);
	GRAPHICS_ASSERT_DEBUG(aClipRect.iBr.iY <= deviceDestRect.iBr.iY, EDirectGdiPanicOutOfBounds);
#endif

	if (aDestRect.Size() == aSrceRect.Size())
		{
		TRect clippedRect(aDestRect);
		clippedRect.Intersection(aClipRect);

		const TPoint destPoint(clippedRect.iTl);
		clippedRect.Move(aSrceRect.iTl - aDestRect.iTl);
		DoBitBlt(destPoint,aBitmap,aBase,aStride,clippedRect);

		return;
		}

	MFastBlend* fastBlend=NULL;
	if (FastBlendInterface(aBitmap,NULL,fastBlend)==KErrNone)
		{
		if (fastBlend->FastBlendBitmapScaled(aClipRect, aDestRect, aSrceRect, aBase, aStride, aBitmap->DisplayMode(), aBitmap->SizeInPixels(), GcDrawMode(iDrawMode), CFbsDrawDevice::ENoShadow)== KErrNone)
			{
			return;
			}
		}
	
	TUint32* scanLineBuffer = iDrawDevice->ScanLineBuffer();
	const TInt scanLineBytes = iDrawDevice->ScanLineBytes();
	TPtr8 scanLineDes(reinterpret_cast<TUint8*>(scanLineBuffer), scanLineBytes, scanLineBytes);

	// For EColor16MU targets, don't use EColor16MAP when draw mode is EDrawModeWriteAlpha.
	// Format conversion provides no performance gain and WriteLine expects EColor16MU 
	// in this case.
	const TDisplayMode dispMode = iDrawDevice->DisplayMode() == EColor16MU && iDrawMode == DirectGdi::EDrawModeWriteAlpha ? EColor16MU : iDrawDevice->ScanLineDisplayMode();

	TLinearDDA xLine;
	TInt bitmapXStart = 0;
	xLine.Construct(TPoint(aSrceRect.iTl.iX,aDestRect.iTl.iX),
					TPoint(aSrceRect.iBr.iX,aDestRect.iBr.iX),TLinearDDA::ELeft);
	xLine.JumpToYCoord(bitmapXStart,aClipRect.iTl.iX);

	TLinearDDA yLine;
	TPoint yCoord(aSrceRect.iTl.iY,aDestRect.iTl.iY);
	yLine.Construct(yCoord,TPoint(aSrceRect.iBr.iY,aDestRect.iBr.iY),TLinearDDA::ELeft);
	TInt dummy;
	yLine.JumpToYCoord2(dummy,aClipRect.iTl.iY);
	yCoord.SetXY(dummy,aClipRect.iTl.iY);

	const TInt srceWidth = aSrceRect.Width();
	const TInt destWidth = aDestRect.Width();
	const TInt clipWidth = aClipRect.Width();
	const TInt clipStrch = aClipRect.iTl.iX - aDestRect.iTl.iX;

	TLineScanningPosition lineScanPos(aBase);
	TBool finished = EFalse;
	const TPoint KZeroPoint(0,0);
	const CGraphicsContext::TDrawMode drawMode = GcDrawMode(iDrawMode);
	while (yCoord.iY < aClipRect.iBr.iY && !finished)
		{
		aBitmap->StretchScanLine(scanLineDes,TPoint(bitmapXStart,yCoord.iX),
								 clipStrch,clipWidth,destWidth,aSrceRect.iTl.iX,
								 srceWidth,KZeroPoint,dispMode,aBase,lineScanPos);
		if (yCoord.iY == aClipRect.iTl.iY)
			{
			aBitmap->SetCompressionBookmark(lineScanPos, aBase,NULL);
			}
		iDrawDevice->WriteLine(aClipRect.iTl.iX,yCoord.iY,clipWidth, scanLineBuffer, drawMode);
		finished = yLine.NextStep(yCoord);
		}
	}

/**
@see MDirectGdiEngine::DrawMaskedBitmap(cosnt TRect&, const CFbsBitmap&, const TRect&, const CFbsBitmap&, TBool)
@panic DGDIAdapter 7, if the source or mask bitmap is invalid (debug only).
*/
void CSwDirectGdiEngine::DrawBitmapMasked(const TRect& aDestRect,
						 const CFbsBitmap& aBitmap,
						 const TRect& aSourceRect,
						 const CFbsBitmap& aMaskBitmap,
						 TBool aInvertMask)
	{
	if ((aBitmap.ExtendedBitmapType() != KNullUid) || (aMaskBitmap.ExtendedBitmapType() != KNullUid))
		{
		iDriver->SetError(KErrNotSupported); // Not supported for extended bitmaps
		return;
		}

	aBitmap.BeginDataAccess();
	aMaskBitmap.BeginDataAccess();
	
	TRect destRect(aDestRect);
	destRect.Move(iOrigin);
	TRect targetRect(destRect);	
	const TBool opaqueSource = (!IsAlphaChannel(aBitmap.DisplayMode())) && (iDrawMode == DirectGdi::EDrawModePEN);
	
	CBitwiseBitmap* srcebmp = ((CSwDirectGdiBitmap&)aBitmap).Address();
	CBitwiseBitmap* maskbmp = ((CSwDirectGdiBitmap&)aMaskBitmap).Address();
	GRAPHICS_ASSERT_DEBUG(srcebmp,EDirectGdiPanicInvalidBitmap);
	GRAPHICS_ASSERT_DEBUG(maskbmp,EDirectGdiPanicInvalidBitmap);

	const TInt limit = iDefaultRegionPtr->Count();
	if (opaqueSource)
		{
		iDrawMode = DirectGdi::EDrawModeWriteAlpha;
		}
	TRect clipRect(0,0,0,0);
	for (TInt count = 0; count < limit; count++)
		{
		clipRect = (*iDefaultRegionPtr)[count];
		if (!clipRect.Intersects(targetRect))
			{
			continue;
			}
		clipRect.Intersection(targetRect);
		DoDrawBitmapMasked(destRect,
					   srcebmp,
					   aBitmap.DataAddress(),
					   aSourceRect,
					   maskbmp,
					   aMaskBitmap.DataAddress(),
					   aInvertMask,
					   clipRect);
		iDrawDevice->UpdateRegion(clipRect);
		}
	if (opaqueSource)
		{
		iDrawMode = DirectGdi::EDrawModePEN;
		}

	aBitmap.EndDataAccess(ETrue);
	aMaskBitmap.EndDataAccess(ETrue);
	}
