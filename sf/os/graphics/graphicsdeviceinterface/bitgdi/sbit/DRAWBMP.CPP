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

#include <fntstore.h>
#include <bitmap.h>
#include <bitstd.h>
#include <bitdev.h>
#include "BITPANIC.H"
#include <bitdrawinterfaceid.h>
#include <bmalphablend.h>
#include <bitdraw.h>
#include <graphics/fbsrasterizer.h>
#include <graphics/bitmap.inl>
#include <graphics/gdi/gdiinline.inl>

/**
Draws a bitmap.

The function has 3 overloads. The first draws the bitmap given the top
left hand corner, doing a compress/stretch based on its internally
stored size in twips. The second does a compress/stretch to fit a
given rectangle. The third takes a rectangular section of the source
bitmap and does a compress/stretch to fit a given destination
rectangle.The functions provide a concrete implementation of the pure
virtual function <code>CGraphicsContext::DrawBitmap()</code>. The
function behaviour is the same as documented in that class.
*/
EXPORT_C void CFbsBitGc::DrawBitmap(const TPoint& aPos,const CFbsBitmap* aSource)
	{
	if (aSource == NULL || !aSource->Handle())
		return;

	aSource->BeginDataAccess();
	const TSize bitmapTwips = aSource->SizeInTwips();
	if (bitmapTwips.iWidth == 0 || bitmapTwips.iHeight == 0)
		{
		aSource->EndDataAccess(ETrue);
		return;
		}

	TSize scrpixels;
	scrpixels.iWidth = iDevice->HorizontalTwipsToPixels(bitmapTwips.iWidth);
	scrpixels.iHeight = iDevice->VerticalTwipsToPixels(bitmapTwips.iHeight);

	DrawBitmap(TRect(aPos, scrpixels), aSource, TRect(aSource->SizeInPixels()));
	aSource->EndDataAccess(ETrue);
	}

EXPORT_C void CFbsBitGc::DrawBitmap(const TRect& aDestRect,const CFbsBitmap* aSource)
	{
	if (aSource == NULL || !aSource->Handle())
		return;

	aSource->BeginDataAccess();
	DrawBitmap(aDestRect, aSource, TRect(aSource->SizeInPixels()));
	aSource->EndDataAccess(ETrue);
	}

EXPORT_C void CFbsBitGc::DrawBitmap(const TRect& aDestRect,
									const CFbsBitmap* aSource,
									const TRect& aSourceRect)
	{
	if (CheckDevice(aDestRect) || aSourceRect.IsEmpty() || aSource == NULL || !aSource->Handle())
		return;
	
	aSource->BeginDataAccess();
	//aSourceRect should be in the bounds of the bitmap
	TSize bitmapSize(aSource->SizeInPixels());
	if ( aSourceRect.iTl.iX < 0
			|| aSourceRect.iTl.iY < 0
			|| aSourceRect.iBr.iX > bitmapSize.iWidth
			|| aSourceRect.iBr.iY > bitmapSize.iHeight)
		{
		aSource->EndDataAccess(ETrue);
		return;
		}

	TRect destRect(aDestRect);
	destRect.Move(iOrigin);
	AddRect(destRect);

	TRect clippedDestRect(destRect);
	if (UserClipRect(clippedDestRect))
		{
		aSource->EndDataAccess(ETrue);
		return;
		}

	CFbsBitGcBitmap* srce = (CFbsBitGcBitmap*)aSource;
	SetupDevice();
	iDevice->DrawingBegin();

	CBitwiseBitmap* bmp = srce->Address();
	BG_ASSERT_DEBUG(bmp,EBitgdiPanicInvalidBitmap);

	CFbsRasterizer* rasterizer = PrepareRasterizerForExtendedBitmap(*aSource, destRect, aSourceRect);

	const TPoint ditherOrigin(iDitherOrigin + aDestRect.iTl);

	const TInt limit = iDefaultRegionPtr->Count();
	CGraphicsAccelerator* ga = GraphicsAccelerator();
	if (ga && iShadowMode==CFbsDrawDevice::ENoShadow && iDrawMode==EDrawModePEN)
		{
		TInt gaOperationResult = KErrUnknown;
		TAcceleratedBitmapSpec bitmapSpec(const_cast<CFbsBitmap*>(aSource));
		iDevice->DrawingEnd();

		if (destRect.Size() == aSourceRect.Size())
			{
			const TPoint offset(aSourceRect.iTl - destRect.iTl);
			for(TInt count=0;count<limit;count++)
				{
				iClipRect=(*iDefaultRegionPtr)[count];
				if(!iClipRect.Intersects(clippedDestRect))
					continue;
				iClipRect.Intersection(clippedDestRect);

				TRect clippedSrceRect(iClipRect);
				clippedSrceRect.Move(offset);
				gaOperationResult = ga->Operation(TGopBitBlt(iClipRect.iTl,bitmapSpec,clippedSrceRect));
				if (gaOperationResult!=KErrNone)
					break;
				}
			iDevice->iDrawDevice->UpdateRegion(iClipRect);
			}
		else
			{
			RRegion clippedRegion;
			for(TInt clipIt=0;clipIt<limit;clipIt++)
				{
				TRect clipRect((*iDefaultRegionPtr)[clipIt]);
				clipRect.Intersection(clippedDestRect);
				if (!clipRect.IsEmpty())
					clippedRegion.AddRect(clipRect);
				}
			TRect srcCopy(aSourceRect);	// Needed because TGopScaledBitBlt takes a non-const value
			// const_cast needed because Operation takes a non-const region ptr :(
			gaOperationResult = ga->Operation(TGopScaledBitBlt(destRect,bitmapSpec,srcCopy),clippedRegion.Count(),const_cast<TRect*>(clippedRegion.RectangleList()));
			if (gaOperationResult==KErrNone) 	 
				iDevice->iDrawDevice->Update(clippedRegion);
			clippedRegion.Close();
			}
		if(gaOperationResult == KErrNone)
			goto finish;
		iDevice->DrawingBegin();
		
		}
//
	{	// to stop error from gccxml build about previous jump to finish skipping initialisation of opaqueSource 
	const TBool opaqueSource = (!IsAlphaChannel(aSource->DisplayMode())) && (iDrawMode == EDrawModePEN);
	if (opaqueSource)
		{
		iDrawMode = EDrawModeWriteAlpha;
		}

	for (TInt count = 0; count < limit; count++)
		{
		iClipRect = (*iDefaultRegionPtr)[count];
		if (!iClipRect.Intersects(clippedDestRect))
			continue;

		iClipRect.Intersection(clippedDestRect);
		DoDrawBitmap(destRect,bmp,aSource->DataAddress(),aSource->DataStride(),aSourceRect,ditherOrigin);

		iDevice->iDrawDevice->UpdateRegion(iClipRect);
		}
	if (opaqueSource)
		{
		iDrawMode = EDrawModePEN;
		}

	}
	iDevice->DrawingEnd();
	
finish:
	if (rasterizer)
		{
		rasterizer->EndBitmap(aSource->SerialNumber());
		}
	aSource->EndDataAccess(ETrue);
	}


void CFbsBitGc::DoDrawBitmap(const TRect& aDestRect,
							 CBitwiseBitmap* aBitmap,
							 TUint32* aBase,
							 TInt aStride,
							 const TRect& aSrceRect,
							 const TPoint& aDitherOrigin)
	{
	CFbsDrawDevice* drawDevice = iDevice->iDrawDevice;
#ifdef _DEBUG
	TRect deviceDestRect;
	drawDevice->GetDrawRect(deviceDestRect);
#endif
	BG_ASSERT_DEBUG(iClipRect.iTl.iX >= deviceDestRect.iTl.iX, EBitgdiPanicOutOfBounds);
	BG_ASSERT_DEBUG(iClipRect.iTl.iY >= deviceDestRect.iTl.iY, EBitgdiPanicOutOfBounds);
	BG_ASSERT_DEBUG(iClipRect.iBr.iX <= deviceDestRect.iBr.iX, EBitgdiPanicOutOfBounds);
	BG_ASSERT_DEBUG(iClipRect.iBr.iY <= deviceDestRect.iBr.iY, EBitgdiPanicOutOfBounds);

	if (aDestRect.Size() == aSrceRect.Size())
		{
		TRect clippedRect(aDestRect);
		clippedRect.Intersection(iClipRect);

		if (!clippedRect.IsEmpty())
			{
			const TPoint destPoint(clippedRect.iTl);
			clippedRect.Move(aSrceRect.iTl - aDestRect.iTl);
			DoBitBlt(destPoint,aBitmap,aBase,aStride,clippedRect);
			}

		return;
		}
	MFastBlend* fastBlend=NULL;
	if (FastBlendInterface(aBitmap,NULL,fastBlend)==KErrNone)
		{
		if (fastBlend->FastBlendBitmapScaled(iClipRect, aDestRect, aSrceRect, aBase, aStride, aBitmap->DisplayMode(), aBitmap->SizeInPixels(), iDrawMode, iShadowMode)== KErrNone)
			{
			return;
			}
		}

	TUint32* scanLineBuffer = drawDevice->ScanLineBuffer();
	const TInt scanLineBytes = drawDevice->ScanLineBytes();
	TPtr8 scanLineDes(REINTERPRET_CAST(TUint8*,scanLineBuffer),scanLineBytes,scanLineBytes);

	// For EColor16MU targets, don't use EColor16MAP when draw mode is EDrawModeWriteAlpha.
	// Format conversion provides no performance gain and WriteLine expects EColor16MU 
	// in this case.
	const TDisplayMode dispMode = drawDevice->DisplayMode() == EColor16MU && iDrawMode == EDrawModeWriteAlpha ? EColor16MU : drawDevice->ScanLineDisplayMode();

	TLinearDDA xLine;
	TInt bitmapXStart = 0;
	xLine.Construct(TPoint(aSrceRect.iTl.iX,aDestRect.iTl.iX),
					TPoint(aSrceRect.iBr.iX,aDestRect.iBr.iX),TLinearDDA::ELeft);
	xLine.JumpToYCoord(bitmapXStart,iClipRect.iTl.iX);

	TLinearDDA yLine;
	TPoint yCoord(aSrceRect.iTl.iY,aDestRect.iTl.iY);
	yLine.Construct(yCoord,TPoint(aSrceRect.iBr.iY,aDestRect.iBr.iY),TLinearDDA::ELeft);
	TInt dummy;
	yLine.JumpToYCoord2(dummy,iClipRect.iTl.iY);
	yCoord.SetXY(dummy,iClipRect.iTl.iY);

	TBool finished = EFalse;
	TPoint ditherOrigin(aDitherOrigin + iClipRect.iTl);
	const TInt srceWidth = aSrceRect.Width();
	const TInt destWidth = aDestRect.Width();
	const TInt clipWidth = iClipRect.Width();
	const TInt clipStrch = iClipRect.iTl.iX - aDestRect.iTl.iX;

	TLineScanningPosition lineScanPos(aBase);
	while (yCoord.iY < iClipRect.iBr.iY && !finished)
		{
		aBitmap->StretchScanLine(scanLineDes,TPoint(bitmapXStart,yCoord.iX),
								 clipStrch,clipWidth,destWidth,aSrceRect.iTl.iX,
								 srceWidth,ditherOrigin,dispMode,aBase,lineScanPos);
		if (yCoord.iY==iClipRect.iTl.iY)
			aBitmap->SetCompressionBookmark(lineScanPos, aBase,NULL);
		drawDevice->WriteLine(iClipRect.iTl.iX,yCoord.iY,clipWidth, scanLineBuffer,iDrawMode);
		finished = yLine.NextStep(yCoord);
		ditherOrigin.iY++;
		}
	}

/** The method draws a specified rectangle from a bitmap and
its mask into another rectangle and does a compress/stretch
to fit a given destination rectangle.

@note When using this function with a 256 Mask bitmap, it blends.
Otherwise (e.g. with a 4bpp mask), this function masks rather than blends.
If a user wants to blend the source into the destination they should use
CFbsBitGc::AlphaBlendBitmaps() instead.

@publishedAll
@released

@param aDestRect The rectangle within which the masked bitmap is to be drawn.
@param aBitmap A pointer to the source bitmap.
@param aSourceRect The rectangle in the source bitmap that is copied to the
destination rectangle.
@param aMaskBitmap A pointer to the mask bitmap.
@param aInvertMask If false, a source pixel that is masked by a black pixel
is not transferred to the destination rectangle. If true, then a source pixel
that is masked by a white pixel is not transferred to the destination rectangle.

@pre aBitmap != NULL
@pre aBitmap->Handle() != 0
@pre aMaskBitmap != NULL
@pre aMaskBitmap->Handle() != 0
@pre !aSourceRect.IsEmpty()
@pre aSourceRect should be in the bounds of the bitmap
*/
EXPORT_C void CFbsBitGc::DrawBitmapMasked(const TRect& aDestRect,
						 const CFbsBitmap* aBitmap,
						 const TRect& aSourceRect,
						 const CFbsBitmap* aMaskBitmap,
						 TBool aInvertMask)
	{
	if (aBitmap == NULL || !aBitmap->Handle() || aMaskBitmap == NULL ||
		!aMaskBitmap->Handle() || aSourceRect.IsEmpty() || CheckDevice(aDestRect))
		{
		return;
		}
	
	aBitmap->BeginDataAccess();
	aMaskBitmap->BeginDataAccess();
	
	//aSourceRect should be in the bounds of the bitmap
	TSize bitmapSize(aBitmap->SizeInPixels());
	if ( aSourceRect.iTl.iX < 0
			|| aSourceRect.iTl.iY < 0
			|| aSourceRect.iBr.iX > bitmapSize.iWidth
			|| aSourceRect.iBr.iY > bitmapSize.iHeight)
		{
		aBitmap->EndDataAccess(ETrue);
		aMaskBitmap->EndDataAccess(ETrue);
		return;
		}   		
	
	TRect destRect(aDestRect);
	destRect.Move(iOrigin);
	AddRect(destRect);
	TRect clippedDestRect(destRect);
	if (UserClipRect(clippedDestRect))
		{
		aBitmap->EndDataAccess(ETrue);
		aMaskBitmap->EndDataAccess(ETrue);
		return;
		}

	SetupDevice();
	const TBool opaqueSource = (!IsAlphaChannel(aBitmap->DisplayMode())) && (iDrawMode == EDrawModePEN);
	iDevice->DrawingBegin();

	CBitwiseBitmap* srcebmp = ((CFbsBitGcBitmap*)aBitmap)->Address();
	CBitwiseBitmap* maskbmp = ((CFbsBitGcBitmap*)aMaskBitmap)->Address();
	BG_ASSERT_DEBUG(srcebmp,EBitgdiPanicInvalidBitmap);
	BG_ASSERT_DEBUG(maskbmp,EBitgdiPanicInvalidBitmap);

	CFbsRasterizer* rasterizer = PrepareRasterizerForExtendedBitmap(*aBitmap, destRect, aSourceRect);
	CFbsRasterizer* maskRasterizer = NULL;
	if (srcebmp != maskbmp)
		{
		if (aMaskBitmap->SizeInPixels().iWidth >= aBitmap->SizeInPixels().iWidth
			&& aMaskBitmap->SizeInPixels().iHeight >= aBitmap->SizeInPixels().iHeight)
			{
			// Mask is not tiled. Pass same region of interest as source bitmap to rasterizer.
			maskRasterizer = PrepareRasterizerForExtendedBitmap(*aMaskBitmap, destRect, aSourceRect);
			}
		else
			{
			// Mask is tiled. Do not pass any region of interest to rasterizer.
			maskRasterizer = PrepareRasterizerForExtendedBitmap(*aMaskBitmap);
			}
		}

	const TPoint ditherOrigin(iDitherOrigin + aDestRect.iTl);
	const TInt limit = iDefaultRegionPtr->Count();
//
	CGraphicsAccelerator* ga = GraphicsAccelerator();
	if (ga && iShadowMode==CFbsDrawDevice::ENoShadow && (aMaskBitmap->DisplayMode()!=EGray2 || !aInvertMask) && iDrawMode==EDrawModePEN)
		{
		TInt gaOperationResult = KErrUnknown;
		TAcceleratedBitmapSpec bitmapSpec(const_cast<CFbsBitmap*>(aBitmap));
		iDevice->DrawingEnd();
		TAcceleratedBitmapSpec maskSpec(const_cast<CFbsBitmap*>(aMaskBitmap));
		const TPoint offset(aSourceRect.iTl - destRect.iTl);

		if (destRect.Size() == aSourceRect.Size())
			{
			for(TInt count=0;count<limit;count++)
				{
				iClipRect=(*iDefaultRegionPtr)[count];
				if(!iClipRect.Intersects(clippedDestRect))
					continue;
				iClipRect.Intersection(clippedDestRect);

				TRect clippedSrceRect(iClipRect);
				clippedSrceRect.Move(offset);

				gaOperationResult = ga->Operation(TGopBitBltMasked(iClipRect.iTl,bitmapSpec,clippedSrceRect,maskSpec)); 	 
				if (gaOperationResult!=KErrNone) 	 
					break; 	 
				iDevice->iDrawDevice->UpdateRegion(iClipRect);
				}
			}
		else
			{
			RRegion clippedRegion;
			for(TInt clipIt=0;clipIt<limit;clipIt++)
				{
				TRect clipRect((*iDefaultRegionPtr)[clipIt]);
				clipRect.Intersection(clippedDestRect);
				if (!clipRect.IsEmpty())
					clippedRegion.AddRect(clipRect);
				}
			TRect srcCopy(aSourceRect);	// Needed because TGopScaledBitBltMasked takes a non-const value
			// const_cast needed because Operation takes a non-const region ptr :(
			gaOperationResult = ga->Operation(TGopScaledBitBltMasked(destRect,bitmapSpec,srcCopy,maskSpec),clippedRegion.Count(),const_cast<TRect*>(clippedRegion.RectangleList()));
			if (gaOperationResult==KErrNone) 	 
				iDevice->iDrawDevice->Update(clippedRegion);
			clippedRegion.Close();
			} 	 
		if (gaOperationResult==KErrNone) 	 
			goto finish;
		iDevice->DrawingBegin(); 	 
		}
//
	if (opaqueSource)
		iDrawMode = EDrawModeWriteAlpha;
	for (TInt count = 0; count < limit; count++)
		{
		iClipRect = (*iDefaultRegionPtr)[count];
		if (!iClipRect.Intersects(clippedDestRect))
			continue;
		iClipRect.Intersection(clippedDestRect);
		DoDrawBitmapMasked(destRect,
					   srcebmp,
					   aBitmap->DataAddress(),
					   aSourceRect,
					   maskbmp,
					   aMaskBitmap->DataAddress(),
					   aInvertMask, ditherOrigin);
		iDevice->iDrawDevice->UpdateRegion(iClipRect);
		}
	if (opaqueSource)
		iDrawMode = EDrawModePEN;
	iDevice->DrawingEnd();
finish:
	if (rasterizer)
		{
		rasterizer->EndBitmap(aBitmap->SerialNumber());
		}
	if (maskRasterizer)
		{
		maskRasterizer->EndBitmap(aMaskBitmap->SerialNumber());
		}
	aBitmap->EndDataAccess(ETrue);
	aMaskBitmap->EndDataAccess(ETrue);
	}

/** The method draws a specified rectangle from a bitmap and
its mask into another rectangle and does a compress/stretch
to fit a given destination rectangle.

This is an overload, which takes CWsBitmap* as argument, which
in turn calls the other overload.

Note: A pointer to CWsBitmap must have the same pointer value as a pointer
to the associated CFbsBitmap, otherwise code in BitGdi component will be
Broken.

@note When using this function with a 256 Mask bitmap, it blends.
Otherwise (e.g. with a 4bpp mask), this function masks rather than blends.
If a user wants to blend the source into the destination they should use
CFbsBitGc::AlphaBlendBitmaps() instead.

@publishedAll
@released

@param aDestRect The rectangle within which the masked bitmap is to be drawn.
@param aBitmap A pointer to the source bitmap.
@param aSourceRect The rectangle in the source bitmap that is copied to the
destination rectangle.
@param aMaskBitmap A pointer to the mask bitmap.
@param aInvertMask If false, a source pixel that is masked by a black pixel
is not transferred to the destination rectangle. If true, then a source pixel
that is masked by a white pixel is not transferred to the destination rectangle.

@pre aBitmap != NULL
@pre aBitmap->Handle() != 0
@pre aMaskBitmap != NULL
@pre aMaskBitmap->Handle() != 0
@pre !aSourceRect.IsEmpty()
*/
EXPORT_C void CFbsBitGc::DrawBitmapMasked(const TRect& aDestRect,
						 const CWsBitmap* aBitmap,
						 const TRect& aSourceRect,
						 const CWsBitmap* aMaskBitmap,
						 TBool aInvertMask)
	{
	DrawBitmapMasked(aDestRect,REINTERPRET_CAST(const CFbsBitmap*,aBitmap),aSourceRect,REINTERPRET_CAST(const CFbsBitmap*,aMaskBitmap),aInvertMask);
	}
