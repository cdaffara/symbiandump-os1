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

#include "BITPANIC.H"
#include <fntstore.h>
#include <bitmap.h>
#include <bitstd.h>
#include <bitdev.h>
#include <bitdraw.h>
#include <bitdrawscaling.h>
#include <bitdrawinterfaceid.h>
#include <bmalphablend.h>
#include <graphics/fbsrasterizer.h>
#include <graphics/bitmap.inl>
#include <graphics/gdi/gdiinline.inl>

GLREF_C void XorBuffers(TUint32* aDestBuffer,const TUint32* aSrceBuffer,TInt aNumBytes);
GLREF_C void AndBuffers(TUint32* aDestBuffer,const TUint32* aSrceBuffer,TInt aNumBytes);
GLREF_C void InvertBuffer(TUint32* aDestBuffer,TInt aNumBytes);
GLREF_C void InvertBuffer(TUint8* aDestBuffer,TInt aNumBytes);
LOCAL_C void TileScanLine(TPtr8& aScanLine,
						  TInt aLengthInPixels,
						  const TPoint& aSrcPt,
						  const CBitwiseBitmap* aMaskBitmap,
						  TLineScanningPosition& aScanLinePos,
						  TUint32* aMaskBase,
						  TDisplayMode aDisplayMode
						  );

/** Draws from another CFbsBitGc.

@param aPoint The position to draw the top left corner of the piece of bitmap
@param aGc The source bitmap graphics context */
EXPORT_C void CFbsBitGc::BitBlt(const TPoint& aDest,const CFbsBitGc& aGc)
	{
	TRect deviceRect;
	aGc.iDevice->iDrawDevice->GetDrawRect(deviceRect);
	BitBlt(aDest,aGc,deviceRect);
	}

/** Draws a particular rectangle from another CFbsBitGc.

@param aPoint The position to draw the top left corner of the piece of bitmap.
@param aGc The source bitmap graphics context.
@param aSrceRect A rectangle defining the piece of the source to be drawn. */
EXPORT_C void CFbsBitGc::BitBlt(const TPoint& aDest,
								const CFbsBitGc& aGc,
								const TRect& aSrceRect)
	{
	if (CheckDevice(aSrceRect))
		return;
	aGc.CheckDevice();
	CFbsDevice* srceDevice = aGc.iDevice;
	if (!srceDevice)
		return;

	TRect srceRect(aSrceRect);
	TRect deviceRect;
	srceDevice->GetDrawRect(deviceRect);
	if (!srceRect.Intersects(deviceRect))
		return;
	srceRect.Intersection(deviceRect);

	const TPoint destPoint(aDest + iOrigin + srceRect.iTl - aSrceRect.iTl);
	const TRect destRect(destPoint,srceRect.Size());
	const TPoint offset(srceRect.iTl - destPoint);

	TRect clippedDestRect(destRect);
	AddRect(clippedDestRect);
	if (UserClipRect(clippedDestRect))
		return;

	SetupDevice();
	aGc.iDevice->DrawingBegin();
	iDevice->DrawingBegin();

	const TInt limit = iDefaultRegionPtr->Count();
	TBool opaqueSource = (!IsAlphaChannel(srceDevice->DisplayMode())) && (iDrawMode == EDrawModePEN);
	for(TInt count=0;count<limit;count++)
		{
		iClipRect=(*iDefaultRegionPtr)[count];
		if(!iClipRect.Intersects(clippedDestRect))
			continue;
		iClipRect.Intersection(clippedDestRect);

		TRect clippedSrceRect(iClipRect);
		clippedSrceRect.Move(offset);

		if (opaqueSource)
			iDrawMode = EDrawModeWriteAlpha;// ie write rather then blend
		DoBitBlt(iClipRect.iTl,srceDevice,clippedSrceRect);
		if (opaqueSource)
			iDrawMode = EDrawModePEN;// set it back how it was
		iDevice->iDrawDevice->UpdateRegion(iClipRect);
		}

	aGc.iDevice->DrawingEnd();
	iDevice->DrawingEnd();
	}


/** Draws the whole of a CFbsBitmap.

@param aDest   The position to draw the top left corner of the bitmap.
@param aBitmap The source bitmap. */
EXPORT_C void CFbsBitGc::BitBlt(const TPoint& aDest,const CFbsBitmap* aBitmap)
    {
    if (aBitmap == NULL || !aBitmap->Handle())
    	return;
    
	aBitmap->BeginDataAccess();
    BitBlt(aDest,aBitmap,TRect(aBitmap->SizeInPixels()));
	aBitmap->EndDataAccess(ETrue);
	}


/** Draws a particular rectangle from a CFbsBitmap.

@param aDest The position to draw the top left corner of the bitmap.
@param aBitmap The source bitmap.
@param aSrceRect A rectangle defining the piece of the source to be drawn. */
EXPORT_C void CFbsBitGc::BitBlt(const TPoint& aDest,
								const CFbsBitmap* aBitmap,
								const TRect& aSrceRect)
    {
	if (aBitmap == NULL || !aBitmap->Handle()|| CheckDevice(aSrceRect))
		return;

	aBitmap->BeginDataAccess();
		
	TRect srceRect(aSrceRect);
	const TRect area(aBitmap->SizeInPixels());
	if (!srceRect.Intersects(area))
		{
		aBitmap->EndDataAccess(ETrue);
		return;
		}
	srceRect.Intersection(area);

	const TPoint destPoint(aDest + iOrigin + srceRect.iTl - aSrceRect.iTl);
	const TPoint offset(srceRect.iTl - destPoint);

	TRect targetRect(destPoint,srceRect.Size());
	AddRect(targetRect);
	if (UserClipRect(targetRect))
		{
		aBitmap->EndDataAccess(ETrue);
		return;
		}

	SetupDevice();
	iDevice->DrawingBegin();

	CBitwiseBitmap* srce = ((CFbsBitGcBitmap*)aBitmap)->Address();
	BG_ASSERT_DEBUG(srce,EBitgdiPanicInvalidBitmap);

	CFbsRasterizer* rasterizer = PrepareRasterizerForExtendedBitmap(*aBitmap, targetRect, offset);

	TInt count;
	const TInt limit = iDefaultRegionPtr->Count();
	CGraphicsAccelerator* ga = GraphicsAccelerator();
	TBool opaqueSource = (!IsAlphaChannel(aBitmap->DisplayMode())) && (iDrawMode == EDrawModePEN);

	if(ga && (iShadowMode == CFbsDrawDevice::ENoShadow))
		{
	    TInt gaOperationResult = KErrUnknown;
		TAcceleratedBitmapSpec bitmapSpec(const_cast<CFbsBitmap*>(aBitmap));
        iDevice->DrawingEnd();

		for(count=0;count<limit;count++)
			{

			iClipRect=(*iDefaultRegionPtr)[count];
			if(!iClipRect.Intersects(targetRect))
				continue;
			iClipRect.Intersection(targetRect);

			TRect clippedSrceRect(iClipRect);
			clippedSrceRect.Move(offset);

			gaOperationResult = ga->Operation(TGopBitBlt(iClipRect.iTl,bitmapSpec,clippedSrceRect));
			if(gaOperationResult != KErrNone)
				break;
			iDevice->iDrawDevice->UpdateRegion(iClipRect);
			}
		if(gaOperationResult == KErrNone)
			goto finish;
		iDevice->DrawingBegin();
		}

	for(count=0;count<limit;count++)
		{
		iClipRect=(*iDefaultRegionPtr)[count];
		if(!iClipRect.Intersects(targetRect))
			continue;
		iClipRect.Intersection(targetRect);

		TRect clippedSrceRect(iClipRect);
		clippedSrceRect.Move(offset);

		if (opaqueSource)
			iDrawMode = EDrawModeWriteAlpha;// ie write rather then blend
		DoBitBlt(iClipRect.iTl,srce,aBitmap->DataAddress(),aBitmap->DataStride(),clippedSrceRect);
		if (opaqueSource)
			iDrawMode = EDrawModePEN;// set it back how it was
		iDevice->iDrawDevice->UpdateRegion(iClipRect);
		}

	iDevice->DrawingEnd();
finish:
	if (rasterizer)
		{
		rasterizer->EndBitmap(aBitmap->SerialNumber());
		}
	aBitmap->EndDataAccess(ETrue);
	}


/** Performs a masked bitmap block transfer.

The function provides a concrete implementation of the pure virtual
function CBitmapContext::BitBltMasked(). The function
behaviour is the same as documented in that class.

There are several points to note about this implementation of
BitBltMasked():

1.For best performance the aMaskBitmap and source aBitmap should
have the same display mode as the destination device/bitmap.

2.For performance reasons this implementation does not validate
the contents of the aMaskBitmap. The caller must ensure the mask
pixels are either black or white otherwise undefined blitting causing
unpredictable discoloration will result. This is especially true
for index (where pixel is palette entry) display modes (e.g. EColor16).
It is up to the caller to decide if they wish to utilise
CFbsBitmap::IsMonochrome().

3.Alpha blending is used when the display mode of the mask bitmap aMaskBitmap
is EGray256.

@see CBitmapContext::BitBltMasked() */
EXPORT_C void CFbsBitGc::BitBltMasked(const TPoint& aDest,
									  const CFbsBitmap* aBitmap,
									  const TRect& aSourceRect,
									  const CFbsBitmap* aMaskBitmap,
									  TBool aInvertMask)
    {
	if (aBitmap == NULL || !aBitmap->Handle() ||
		aMaskBitmap == NULL || !aMaskBitmap->Handle() ||
		CheckDevice(aSourceRect))
		return;

	aBitmap->BeginDataAccess();
	aMaskBitmap->BeginDataAccess();
		
	TRect srceRect(aSourceRect);
	const TRect area(aBitmap->SizeInPixels());
	if (!srceRect.Intersects(area))
		{
		aBitmap->EndDataAccess(ETrue);
		aMaskBitmap->EndDataAccess(ETrue);
		return;
		}
	srceRect.Intersection(area);

	const TPoint destPoint(aDest + iOrigin + srceRect.iTl - aSourceRect.iTl);
	const TRect destRect(destPoint,srceRect.Size());
	const TPoint offset(srceRect.iTl - destPoint);
	const TPoint ditherorigin(iDitherOrigin + aDest);

	TRect clippedDestRect(destRect);
	AddRect(clippedDestRect);
	if (UserClipRect(clippedDestRect))
		{
		aBitmap->EndDataAccess(ETrue);
		aMaskBitmap->EndDataAccess(ETrue);
		return;
		}

	SetupDevice();
	iDevice->DrawingBegin();


	CBitwiseBitmap* srcebmp = ((CFbsBitGcBitmap*)aBitmap)->Address();
	CBitwiseBitmap* maskbmp = ((CFbsBitGcBitmap*)aMaskBitmap)->Address();
	BG_ASSERT_DEBUG(srcebmp,EBitgdiPanicInvalidBitmap);
	BG_ASSERT_DEBUG(maskbmp,EBitgdiPanicInvalidBitmap);

	const TDisplayMode maskMode = maskbmp->DisplayMode();

	// Do the background fill the lazy way with flicker if any of the following are true:
	// 1. There is no anti-flicker buffer
	// 2. The source and mask bitmaps are the same
	// 3. The brush style is patterned and the mask is an alpha mask
	// 4. The brush style is not null or solid or patterned

	if (!iDevice->iBitBltMaskedBuffer ||
		srcebmp == maskbmp ||
		(maskMode == EGray256 && iBrushStyle == EPatternedBrush) ||
		iBrushStyle > EPatternedBrush)
		{
		iBrushBitmap.BeginDataAccess();
		CFbsRasterizer* brushRasterizer = PrepareRasterizerForExtendedBitmap(iBrushBitmap);
		RectFill(destRect);
		if (brushRasterizer)
			{
			brushRasterizer->EndBitmap(iBrushBitmap.SerialNumber());
			}
		iBrushBitmap.EndDataAccess(ETrue);
		}

	const TInt8 shadowMode = iShadowMode;
	iShadowMode = CFbsDrawDevice::ENoShadow;
	SetupDevice();

	CFbsRasterizer* rasterizer = PrepareRasterizerForExtendedBitmap(*aBitmap, clippedDestRect, offset);
	CFbsRasterizer* maskRasterizer = NULL;
	if (srcebmp != maskbmp)
		{
		if (aMaskBitmap->SizeInPixels().iWidth >= aBitmap->SizeInPixels().iWidth
			&& aMaskBitmap->SizeInPixels().iHeight >= aBitmap->SizeInPixels().iHeight)
			{
			// Mask is not tiled. Pass same region of interest as source bitmap to rasterizer.
			maskRasterizer = PrepareRasterizerForExtendedBitmap(*aMaskBitmap, clippedDestRect, offset);
			}
		else
			{
			// Mask is tiled. Do not pass any region of interest to rasterizer.
			maskRasterizer = PrepareRasterizerForExtendedBitmap(*aMaskBitmap);
			}
		}

    TInt count;
	const TInt limit = iDefaultRegionPtr->Count();
	CGraphicsAccelerator* ga = GraphicsAccelerator();
	TBool opaqueSource = (!IsAlphaChannel(aBitmap->DisplayMode())) && (iDrawMode == EDrawModePEN);

	if(ga)
		{
		if((iBrushStyle == ENullBrush) && aInvertMask &&
		   (shadowMode == CFbsDrawDevice::ENoShadow))
			{
			TInt gaOperationResult = KErrUnknown;
			TAcceleratedBitmapSpec bitmapSpec(const_cast<CFbsBitmap*>(aBitmap));
			TAcceleratedBitmapSpec bitmapMaskSpec(const_cast<CFbsBitmap*>(aMaskBitmap));
			iDevice->DrawingEnd();

			for(count=0;count<limit;count++)
				{
				iClipRect=(*iDefaultRegionPtr)[count];
				if(!iClipRect.Intersects(clippedDestRect))
					continue;
				iClipRect.Intersection(clippedDestRect);
				TRect clippedSrceRect(iClipRect);
				clippedSrceRect.Move(offset);

				if(maskbmp->DisplayMode() == EGray256)
					gaOperationResult = ga->Operation(TGopBitBltAlphaBitmap(iClipRect.iTl,
																			bitmapSpec,
																			clippedSrceRect,
																			bitmapMaskSpec));
				else
					gaOperationResult = ga->Operation(TGopBitBltMasked(iClipRect.iTl,
																	   bitmapSpec,
																	   clippedSrceRect,
																	   bitmapMaskSpec));

				if(gaOperationResult != KErrNone)
					break;
				iDevice->iDrawDevice->UpdateRegion(iClipRect);
				}
			if(gaOperationResult == KErrNone)
				goto finish;
			iDevice->DrawingBegin();
			}
		}

	for(count=0;count<limit;count++)
		{
		iClipRect=(*iDefaultRegionPtr)[count];
		if (!iClipRect.Intersects(clippedDestRect))
			continue;
		iClipRect.Intersection(clippedDestRect);

		TRect clippedSrceRect(iClipRect);
		clippedSrceRect.Move(offset);

		if (opaqueSource)
			iDrawMode = EDrawModeWriteAlpha;// ie write rather then blend
		DoBitBltMasked(iClipRect.iTl,
					   srcebmp,
					   aBitmap->DataAddress(),
					   clippedSrceRect,
					   maskbmp,
					   aMaskBitmap->DataAddress(),
					   aInvertMask,
					   ditherorigin,
					   shadowMode);
		if (opaqueSource)
			iDrawMode = EDrawModePEN;// set it back how it was
		iDevice->iDrawDevice->UpdateRegion(iClipRect);
		}

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
	iShadowMode = shadowMode;
	}
/**
Does BitBlt operation of source and bitmap per scanline.

*/
void CFbsBitGc::DoBitBlt(const TPoint& aDest,CFbsDevice* aSrce,const TRect& aSrceRect)
	{
	const TInt width = aSrceRect.Width();
	CFbsDrawDevice* drawDevice = iDevice->iDrawDevice;
#ifdef _DEBUG
	TRect deviceSrcRect;
	aSrce->iDrawDevice->GetDrawRect(deviceSrcRect);
	TRect deviceDestRect;
	drawDevice->GetDrawRect(deviceDestRect);
#endif
	BG_ASSERT_DEBUG(aSrceRect.iTl.iX >= deviceSrcRect.iTl.iX, EBitgdiPanicOutOfBounds);
	BG_ASSERT_DEBUG(aSrceRect.iTl.iY >= deviceSrcRect.iTl.iY, EBitgdiPanicOutOfBounds);
	BG_ASSERT_DEBUG(aSrceRect.iBr.iX <= deviceSrcRect.iBr.iX, EBitgdiPanicOutOfBounds);
	BG_ASSERT_DEBUG(aSrceRect.iBr.iY <= deviceSrcRect.iBr.iY, EBitgdiPanicOutOfBounds);
	BG_ASSERT_DEBUG(aDest.iX >= deviceDestRect.iTl.iX, EBitgdiPanicOutOfBounds);
	BG_ASSERT_DEBUG(aDest.iY >= deviceDestRect.iTl.iY, EBitgdiPanicOutOfBounds);
	BG_ASSERT_DEBUG((aDest.iX + aSrceRect.Width())  <= deviceDestRect.iBr.iX, EBitgdiPanicOutOfBounds);
	BG_ASSERT_DEBUG((aDest.iY + aSrceRect.Height()) <= deviceDestRect.iBr.iY, EBitgdiPanicOutOfBounds);

	CFbsDrawDevice* srcDrawDevice = aSrce->iDrawDevice;
	TAny* interface=NULL;
	if (iDrawMode==EDrawModeWriteAlpha &&
		iShadowMode == CFbsDrawDevice::ENoShadow &&
		(aSrceRect.iTl.iX >= 0) &&  (aSrceRect.iTl.iY >= 0) &&
		(aDest.iX >= 0) && (aDest.iY >= 0) &&
	    srcDrawDevice->DisplayMode() == drawDevice->DisplayMode() &&
		drawDevice->GetInterface(KFastBlit2InterfaceID, interface) == KErrNone)
		{
		// Conditions in CFbsBitGc allow for optimised blitting.
		// The draw device supports the optimised blitting function.
		// Operation may fail regardless due to unacceptable conditions in the draw device.
		BG_ASSERT_DEBUG(interface!=NULL, EBitgdiPanicNullPointer);
		MFastBlit2* fastBlit = reinterpret_cast<MFastBlit2*>(interface);
		if (fastBlit->WriteBitmapBlock(aDest, srcDrawDevice, aSrceRect) == KErrNone)
			{
			return;
			}
		}
	MFastBlend* fastBlend=NULL;
	if (FastBlendInterface(NULL,NULL,fastBlend)==KErrNone)
		{
		if (fastBlend->FastBlendBitmap(aDest, srcDrawDevice, aSrceRect, iDrawMode, iShadowMode)==KErrNone)
			{
			return;
			}
		}
	//scanLineBuffer is destination scanline buffer.
	TUint32* scanLineBuffer = drawDevice->ScanLineBuffer();
	const TInt scanLineBytes = drawDevice->ScanLineBytes();
	TPtr8 scanLineDes((TUint8*)scanLineBuffer,scanLineBytes,scanLineBytes);
	//dispMode is destination display mode.
	const TDisplayMode dispMode = ScanLineBufferDisplayMode(drawDevice);
	TInt destY = aDest.iY;
	//Gets the scanline from the source, into the buffer scanLineDes.
	//The DoGetScanLine operation is also responsible for converting
	// the buffer pixel format to destination display format.

	for (TInt row = aSrceRect.iTl.iY; row < aSrceRect.iBr.iY; row++,destY++)
		{
		aSrce->DoGetScanLine(scanLineDes,TPoint(aSrceRect.iTl.iX,row),width,dispMode);
		drawDevice->WriteLine(aDest.iX,destY,width,scanLineBuffer,iDrawMode);
		}
	}

/**
Calculates the position into the scanline for the given x coordinate
*/
TUint CFbsBitGc::MemoryOffsetForPixelPitch(TUint aX, TDisplayMode aDisplayMode)
	{
	switch (aDisplayMode)
		{
		case EColor16MU:
		case EColor16MA:
		case EColor16MAP:
			return aX << 2;

		case EColor16M:
			return aX * 3;

		case EColor4K:
		case EColor64K:
			return aX << 1;

		case EGray256:
		case EColor256:
			return aX;

		default:
			BG_PANIC_DEBUG(EBitgdiPanicInvalidDisplayMode);
		break;
		}
	return 0;
	}

/**
Gets the scanline pointer with the offset
*/
TUint32* CFbsBitGc::GetScanLineOffsetPtr(CBitwiseBitmap* aSrce, TUint32*& aSlptr, TInt aLength, TPoint aPixel,TUint32* aBase, TLineScanningPosition& aLineScanningPosition, TUint aXOffset)
	{
	aSrce->GetScanLinePtr(aSlptr, aLength, aPixel, aBase, aLineScanningPosition);
	return (TUint32*)((TUint8*)aSlptr + aXOffset);
	}

void CFbsBitGc::DoBitBlt(const TPoint& aDest,
						 CBitwiseBitmap* aSrce,
						 TUint32* aBase,
						 TInt aStride,
						 const TRect& aSrceRect)
	{
	// Does multiple bitmap widths for painting rects only
	const TInt width = aSrceRect.Width();
	CFbsDrawDevice* drawDevice = iDevice->iDrawDevice;
#ifdef _DEBUG
	TRect deviceDestRect;
	drawDevice->GetDrawRect(deviceDestRect);
#endif
	BG_ASSERT_DEBUG(aDest.iX >= deviceDestRect.iTl.iX, EBitgdiPanicOutOfBounds);
	BG_ASSERT_DEBUG(aDest.iY >= deviceDestRect.iTl.iY, EBitgdiPanicOutOfBounds);
	BG_ASSERT_DEBUG((aDest.iX + aSrceRect.Width())  <= deviceDestRect.iBr.iX, EBitgdiPanicOutOfBounds);
	BG_ASSERT_DEBUG((aDest.iY + aSrceRect.Height()) <= deviceDestRect.iBr.iY, EBitgdiPanicOutOfBounds);

	TSize srcSize = aSrce->SizeInPixels();
	if (srcSize.iWidth == 0 || srcSize.iHeight == 0)
		return; //no point doing anything if asked to draw zero size bitmap

	TAny* interface=NULL;
	if (iDrawMode==EDrawModeWriteAlpha &&
		iShadowMode == CFbsDrawDevice::ENoShadow &&
		aSrceRect.iTl.iX >= 0 &&
		aSrceRect.iTl.iY >= 0 &&
		aSrceRect.iBr.iX <= srcSize.iWidth &&
		aSrceRect.iBr.iY <= srcSize.iHeight &&
		(aDest.iX >= 0) && (aDest.iY >= 0) &&
		!aSrce->IsCompressed() &&
		aSrce->DisplayMode() == drawDevice->DisplayMode() &&
		drawDevice->GetInterface(KFastBlit2InterfaceID, interface) == KErrNone)
		{
		// Conditions in CFbsBitGc allow for optimised blitting.
		// The draw device supports the optimised blitting function.
		// Operation may fail regardless due to unacceptable conditions in the draw device.
		BG_ASSERT_DEBUG(interface!=NULL, EBitgdiPanicNullPointer);
		MFastBlit2* fastBlit = reinterpret_cast<MFastBlit2*>(interface);
		if (fastBlit && (fastBlit->WriteBitmapBlock(aDest, aBase, aStride, srcSize, aSrceRect) == KErrNone))
			{
			return;
			}
		}
	MFastBlend* fastBlend=NULL;
	if (FastBlendInterface(aSrce,NULL,fastBlend)==KErrNone)
		{
		if (fastBlend->FastBlendBitmap(aDest, aBase, aStride, srcSize, aSrceRect, aSrce->DisplayMode(), iDrawMode, iShadowMode)== KErrNone)
			{
			return;
			}
		}

	const TInt scanLineBytes = drawDevice->ScanLineBytes();
	TUint32* scanLineBuffer = drawDevice->ScanLineBuffer();
	TPtr8 scanLineDes((TUint8*)scanLineBuffer,scanLineBytes,scanLineBytes);

	const TDisplayMode dispMode = ScanLineBufferDisplayMode(drawDevice);

	const TBool useScanLinePtr = (!iShadowMode) && (dispMode == aSrce->DisplayMode()) && (TDisplayModeUtils::NumDisplayModeBitsPerPixel(dispMode)>=8);
	TUint32* slptr=NULL;
	TUint offset = 0;
	TUint32* lastScanLine = NULL;
	if (useScanLinePtr)
		lastScanLine = aSrce->ScanLineAddress(aBase,aSrceRect.iBr.iY-1);

	TInt srceWidth = srcSize.iWidth;
	TInt partlinestart = 0;
	partlinestart = aSrceRect.iTl.iX % srceWidth;
	if (partlinestart < 0)
		partlinestart += srceWidth;
	const TInt partlinelength = Min(srceWidth - partlinestart,width);
	TInt destX = aDest.iX;
	const TInt destXlimit=destX+width;

	// first part line
	if (partlinestart > 0 && partlinelength > 0)
		{
		TPoint srcecoord1(partlinestart,aSrceRect.iTl.iY);
		TInt desty = aDest.iY;
		TPoint ditherorigin(iDitherOrigin);
		ditherorigin.iX += aDest.iX;
		ditherorigin.iY += desty;

		TLineScanningPosition lineScanPos(aBase);

		if (useScanLinePtr)
			{
			offset = MemoryOffsetForPixelPitch(partlinestart, dispMode);
			if (aSrce->IsCompressed())
				{
				while (srcecoord1.iY < aSrceRect.iBr.iY)
					{
					scanLineBuffer = GetScanLineOffsetPtr(aSrce, slptr, partlinelength, srcecoord1, aBase, lineScanPos, offset);
					if (srcecoord1.iY==aSrceRect.iTl.iY)
						{
						aSrce->SetCompressionBookmark(lineScanPos,aBase,NULL);
						}
					drawDevice->WriteLine(aDest.iX,desty,partlinelength, scanLineBuffer,iDrawMode);
					srcecoord1.iY++,desty++,ditherorigin.iY++;
					}
				}
			else
				{
				while (srcecoord1.iY < aSrceRect.iBr.iY)
					{
					scanLineBuffer = GetScanLineOffsetPtr(aSrce, slptr, partlinelength, srcecoord1, aBase, lineScanPos, offset);
					do
						{
						drawDevice->WriteLine(aDest.iX,desty,partlinelength, scanLineBuffer,iDrawMode);
						scanLineBuffer = (TUint32*)((TUint8*)scanLineBuffer + aStride);
						srcecoord1.iY++,desty++,ditherorigin.iY++;
						}
					while ((srcecoord1.iY < aSrceRect.iBr.iY) && (scanLineBuffer < lastScanLine));
					}
				}
			}
		else
			{
			for (; srcecoord1.iY < aSrceRect.iBr.iY; srcecoord1.iY++,desty++,ditherorigin.iY++)
				{
				aSrce->GetScanLine(scanLineDes,srcecoord1,partlinelength,ETrue,
						   ditherorigin,dispMode,aBase, lineScanPos);
				if (srcecoord1.iY==aSrceRect.iTl.iY)
					{
					aSrce->SetCompressionBookmark(lineScanPos,aBase,NULL);
					}
				drawDevice->WriteLine(aDest.iX,desty,partlinelength, scanLineBuffer,iDrawMode);
				}
			}

		destX+=partlinelength;
		}

	// multiple complete lines - columns
	TInt numcolumns = 0;
	numcolumns = (destXlimit - destX) / srceWidth;
	if (numcolumns > 0)
		{
		TPoint srcecoord2(0,aSrceRect.iTl.iY);
		TInt desty = aDest.iY;
		TPoint ditherorigin(iDitherOrigin);
		ditherorigin.iX += destX;
		ditherorigin.iY += desty;

		TLineScanningPosition lineScanPos(aBase);

		if (useScanLinePtr)
			{
			if (aSrce->IsCompressed())
				{
				while (srcecoord2.iY < aSrceRect.iBr.iY)
					{
					TPoint coord(srcecoord2);
					aSrce->GetScanLinePtr(slptr, srceWidth, coord,aBase, lineScanPos);
					if (srcecoord2.iY==aSrceRect.iTl.iY)
						{
						aSrce->SetCompressionBookmark(lineScanPos, aBase,NULL);
						}
					TInt tempdestX = destX;
					for (TInt count = 0; count < numcolumns; count++,tempdestX+=srceWidth)
						{
						drawDevice->WriteLine(tempdestX,desty,srceWidth, slptr,iDrawMode);
						ditherorigin.iX += srceWidth;
						}
					srcecoord2.iY++,desty++,ditherorigin.iY++;
					}
				}
			else
				{
				while (srcecoord2.iY < aSrceRect.iBr.iY)
					{
					TPoint coord(srcecoord2);
					aSrce->GetScanLinePtr(slptr, srceWidth, coord,aBase, lineScanPos);
					do
						{
						TInt tempdestX = destX;
						for (TInt count = 0; count < numcolumns; count++,tempdestX+=srceWidth)
							{
							drawDevice->WriteLine(tempdestX,desty,srceWidth, slptr,iDrawMode);
							ditherorigin.iX += srceWidth;
							}
						slptr = (TUint32*)((TUint8*)slptr + aStride);
						srcecoord2.iY++,desty++,ditherorigin.iY++;
						}
					while ((srcecoord2.iY < aSrceRect.iBr.iY) && (slptr < lastScanLine));
					}
				}
			}
		else
			{
			for (; srcecoord2.iY < aSrceRect.iBr.iY; srcecoord2.iY++,desty++,ditherorigin.iY++)
				{
				TInt tempdestX = destX;
				TPoint coord(srcecoord2);
				aSrce->GetScanLinePtr(slptr, srceWidth, coord,aBase, lineScanPos);
				if (srcecoord2.iY==aSrceRect.iTl.iY)
					{
					aSrce->SetCompressionBookmark(lineScanPos, aBase,NULL);
					}
				for (TInt count = 0; count < numcolumns; count++,tempdestX+=srceWidth)
					{
					aSrce->GetScanLine(slptr, scanLineDes,coord,srceWidth,ETrue,
						   ditherorigin,dispMode);
					drawDevice->WriteLine(tempdestX,desty,srceWidth, scanLineBuffer,iDrawMode);
					ditherorigin.iX += srceWidth;
					}
				}
			}

		destX += numcolumns * srceWidth;
		}

	// final part line
	if (destX < destXlimit)
		{
		const TInt restofline = destXlimit - destX;
		TPoint srcecoord3(0,aSrceRect.iTl.iY);
		TInt desty = aDest.iY;
		TPoint ditherorigin(iDitherOrigin);
		ditherorigin.iX += destX;
		ditherorigin.iY += desty;

		TLineScanningPosition lineScanPos(aBase);

		if (useScanLinePtr)
			{
			offset = 0;
			if (aSrce->IsCompressed())
				{
				while (srcecoord3.iY < aSrceRect.iBr.iY)
					{
					scanLineBuffer = GetScanLineOffsetPtr(aSrce, slptr, srceWidth, srcecoord3, aBase, lineScanPos, offset);
					if (srcecoord3.iY==aSrceRect.iTl.iY)
						{
						aSrce->SetCompressionBookmark(lineScanPos,aBase,NULL);
						}
					drawDevice->WriteLine(destX,desty,restofline,scanLineBuffer,iDrawMode);
					srcecoord3.iY++,desty++,ditherorigin.iY++;
					}
				}
			else
				{
				while (srcecoord3.iY < aSrceRect.iBr.iY)
					{
					scanLineBuffer = GetScanLineOffsetPtr(aSrce, slptr, srceWidth, srcecoord3, aBase, lineScanPos, offset);
					do
						{
						drawDevice->WriteLine(destX,desty,restofline,scanLineBuffer,iDrawMode);
						scanLineBuffer = (TUint32*)((TUint8*)scanLineBuffer + aStride);
						srcecoord3.iY++,desty++,ditherorigin.iY++;
						}
					while ((srcecoord3.iY < aSrceRect.iBr.iY) && (scanLineBuffer < lastScanLine));
					}
				}
			}
		else
			{
			for (; srcecoord3.iY < aSrceRect.iBr.iY; srcecoord3.iY++,desty++,ditherorigin.iY++)
				{
				aSrce->GetScanLine(scanLineDes,srcecoord3,srceWidth,ETrue,
								   ditherorigin,dispMode,aBase,lineScanPos);
				if (srcecoord3.iY==aSrceRect.iTl.iY)
					{
					aSrce->SetCompressionBookmark(lineScanPos, aBase,NULL);
					}
				drawDevice->WriteLine(destX,desty,restofline,scanLineBuffer,iDrawMode);
				}
			}
		}
	}

void CFbsBitGc::DoBitBltMasked(const TPoint& aDest,
							   CBitwiseBitmap* aSourceBitmap,
							   TUint32* aSourceBase,
							   const TRect& aSourceRect,
							   CBitwiseBitmap* aMaskBitmap,
							   TUint32* aMaskBase,
							   TBool aInvertMask,
							   const TPoint& aDitherOrigin,
							   TInt aShadowMode)
	{
	CFbsDrawDevice* drawDevice = iDevice->iDrawDevice;
#ifdef _DEBUG
	TRect deviceDestRect;
	drawDevice->GetDrawRect(deviceDestRect);
#endif
	BG_ASSERT_DEBUG(aDest.iX >= deviceDestRect.iTl.iX, EBitgdiPanicOutOfBounds);
	BG_ASSERT_DEBUG(aDest.iY >= deviceDestRect.iTl.iY, EBitgdiPanicOutOfBounds);
	BG_ASSERT_DEBUG((aDest.iX + aSourceRect.Width()) <= deviceDestRect.iBr.iX, EBitgdiPanicOutOfBounds);
	BG_ASSERT_DEBUG((aDest.iY + aSourceRect.Height()) <= deviceDestRect.iBr.iY, EBitgdiPanicOutOfBounds);

	MFastBlend* fastBlend=NULL;
	if (FastBlendInterface(aSourceBitmap,aMaskBitmap,fastBlend)==KErrNone)
		{
		if (fastBlend->FastBlendBitmapMasked(aDest, aSourceBase, aSourceBitmap->DataStride(), aSourceBitmap->SizeInPixels(), aSourceRect, aSourceBitmap->DisplayMode(),
							aMaskBase, aMaskBitmap->DataStride(), aMaskBitmap->DisplayMode(), aMaskBitmap->SizeInPixels(), aSourceRect.iTl, aInvertMask,
							iDrawMode, aShadowMode)==KErrNone)
			{
			return;
			}
		}

	const TDisplayMode dispMode = ScanLineBufferDisplayMode(drawDevice);
	if (aMaskBitmap->DisplayMode() == EGray256)
		{
		DoBitBltAlpha(aDest,aSourceBitmap,aSourceBase,aSourceRect,
					  aMaskBitmap,aMaskBase,aSourceRect.iTl,aShadowMode, EFalse);
		}
	// if screen driver is 16MAP we avoid logical operator pen modes by using DoBitBltAlpha() for blitting
	else if (dispMode == EColor16MAP)
		{
		DoBitBltAlpha(aDest,aSourceBitmap,aSourceBase,aSourceRect,
					  aMaskBitmap,aMaskBase,aSourceRect.iTl,aShadowMode, aInvertMask);
		}

	else if (aSourceBitmap == aMaskBitmap)
		{
		const TInt width = aSourceRect.Width();
		TPoint ditherOrigin(aDitherOrigin + aDest);
		const TDrawMode drawMode = aInvertMask ? EDrawModeAND : EDrawModeOR;
		TPoint srcePoint(aSourceRect.iTl);
		TInt destY = aDest.iY;

		TLineScanningPosition lineScanPos(aSourceBase);

		const TBool useScanLinePtr = (!aShadowMode) && (dispMode == aSourceBitmap->DisplayMode() && (TDisplayModeUtils::NumDisplayModeBitsPerPixel(dispMode)>=8));
		if (useScanLinePtr)
			{
			TUint32* scanLineBuffer = NULL;
			TUint32* slptr=NULL;
			TUint offset = MemoryOffsetForPixelPitch(srcePoint.iX, dispMode);

			if (aSourceBitmap->IsCompressed())
				{
				for (; srcePoint.iY < aSourceRect.iBr.iY; destY++,srcePoint.iY++,ditherOrigin.iY++)
					{
					scanLineBuffer = GetScanLineOffsetPtr(aSourceBitmap, slptr, width, srcePoint, aSourceBase, lineScanPos, offset);
					drawDevice->WriteLine(aDest.iX,destY,width,scanLineBuffer,drawMode);
					}
				}
			else
				{
				TUint stride = aSourceBitmap->DataStride();
				TUint32* lastScanLine = aSourceBitmap->ScanLineAddress(aSourceBase,aSourceRect.iBr.iY-1);

				while (srcePoint.iY < aSourceRect.iBr.iY)
					{
					scanLineBuffer = GetScanLineOffsetPtr(aSourceBitmap, slptr, width, srcePoint, aSourceBase, lineScanPos, offset);
					do
						{
						drawDevice->WriteLine(aDest.iX,destY,width,scanLineBuffer,drawMode);
						scanLineBuffer = (TUint32*)((TUint8*)scanLineBuffer + stride);
						destY++,srcePoint.iY++,ditherOrigin.iY++;
						}
					while ((srcePoint.iY < aSourceRect.iBr.iY) && (scanLineBuffer < lastScanLine));
					}
				}
			}
		else
			{
			const TInt scanLineBytes = drawDevice->ScanLineBytes();
			TUint32* scanLineBuffer = drawDevice->ScanLineBuffer();
			TPtr8 scanLineDes((TUint8*)scanLineBuffer,scanLineBytes,scanLineBytes);
			for (; srcePoint.iY < aSourceRect.iBr.iY; destY++,srcePoint.iY++,ditherOrigin.iY++)
				{
				aSourceBitmap->GetScanLine(scanLineDes,srcePoint,width,
								   ETrue,ditherOrigin,dispMode,aSourceBase,lineScanPos);

				if (aShadowMode)
					{
					drawDevice->SetShadowMode(CFbsDrawDevice::TShadowMode(aShadowMode));
					drawDevice->ShadowBuffer(width,scanLineBuffer);
					drawDevice->SetShadowMode(CFbsDrawDevice::ENoShadow);
					}

				drawDevice->WriteLine(aDest.iX,destY,width,scanLineBuffer,drawMode);
				}
			}
		}
	else
		{
		if (iDevice->iBitBltMaskedBuffer)
			{
			if (iBrushStyle == ESolidBrush)
				DoBitBltMaskedNonFlickerSolid(aDest,aSourceBitmap,aSourceBase,aSourceRect,
											  aMaskBitmap,aMaskBase,aInvertMask,aDitherOrigin,
											  aShadowMode);
			else if (iBrushStyle == EPatternedBrush)
				DoBitBltMaskedNonFlickerPatterned(aDest,aSourceBitmap,aSourceBase,aSourceRect,
												  aMaskBitmap,aMaskBase,aInvertMask,
												  aDitherOrigin,aShadowMode);
			else
				DoBitBltMaskedNonFlicker(aDest,aSourceBitmap,aSourceBase,aSourceRect,
										 aMaskBitmap,aMaskBase,aInvertMask,aDitherOrigin,
										 aShadowMode);
			}
		else
			DoBitBltMaskedFlicker(aDest,aSourceBitmap,aSourceBase,aSourceRect,aMaskBitmap,
								  aMaskBase,aInvertMask,aDitherOrigin,aShadowMode);
		}
	}

void CFbsBitGc::DoBitBltMaskedFlicker(const TPoint& aDest,
									  CBitwiseBitmap* aSourceBitmap,
									  TUint32* aSourceBase,
									  const TRect& aSourceRect,
									  CBitwiseBitmap* aMaskBitmap,
									  TUint32* aMaskBase,
									  TBool aInvertMask,
									  const TPoint& aDitherOrigin,
									  TInt aShadowMode)
	{
	CFbsDrawDevice* drawDevice = iDevice->iDrawDevice;

	const TInt width = aSourceRect.Width();
	TInt destY = aDest.iY;
	TPoint srcePoint(aSourceRect.iTl);

	TLineScanningPosition lineScanPos(aSourceBase);
	TLineScanningPosition lineScanPosMask(aMaskBase);

	const TDisplayMode srcFormat = aSourceBitmap->DisplayMode();
	const TDisplayMode maskFormat = aMaskBitmap->DisplayMode();
	
	if (aMaskBitmap->IsCompressed()) 
       { 
       HBufC8* hBuf= CFbsBitmap::GetDecompressionBuffer(aMaskBitmap->DataStride() + 4); 
       if (!hBuf) 
          return; // Out of memory so do not draw anything 
       lineScanPosMask.iScanLineBuffer = hBuf; 
       } 

	TAny* interface=NULL;
	if ( (srcFormat == EColor16MU || srcFormat == EColor64K ) &&
		maskFormat == EGray2 &&
		!aShadowMode &&
		aSourceBitmap->SizeInPixels().iWidth <= aMaskBitmap->SizeInPixels().iWidth &&
		aSourceBitmap->SizeInPixels().iHeight <= aMaskBitmap->SizeInPixels().iHeight &&
		drawDevice->GetInterface(KFastBlitInterfaceID, interface) == KErrNone )
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

	TPoint ditherOrigin(aDitherOrigin + aDest);
	const TDisplayMode dispMode = ScanLineBufferDisplayMode(drawDevice);
	const TDrawMode drawMode = aInvertMask ? EDrawModeAND : EDrawModeANDNOT;

	TUint32* scanLineBuffer = drawDevice->ScanLineBuffer();
	const TInt scanLineBytes = drawDevice->ScanLineBytes();
	TPtr8 scanLineDes((TUint8*)scanLineBuffer,scanLineBytes,scanLineBytes);

	TLineScanningPosition lineScanPos2(aSourceBase);

	//scanline modifications required if using shadows, different modes, bits per pixel less than 8
	if ((!aShadowMode) && (dispMode == aSourceBitmap->DisplayMode()) && (TDisplayModeUtils::NumDisplayModeBitsPerPixel(dispMode)>=8))
		{
		TUint offset = MemoryOffsetForPixelPitch(srcePoint.iX, dispMode);
		TUint32* slptr=NULL;
		//mask scanline modifications required for EInvertPen, different screen modes
		if ((drawMode != EDrawModeANDNOT) && (dispMode == aMaskBitmap->DisplayMode()))
			{
			TUint32* scanLineBufferMask = NULL;
			//stride jumping not possible with compressed bitmaps
			if (aSourceBitmap->IsCompressed() || aMaskBitmap->IsCompressed())
				{
				for (; srcePoint.iY < aSourceRect.iBr.iY; destY++,srcePoint.iY++,ditherOrigin.iY++)
					{
		 			scanLineBuffer = GetScanLineOffsetPtr(aSourceBitmap, slptr, width, srcePoint, aSourceBase, lineScanPos, offset);
					drawDevice->WriteLine(aDest.iX,destY,width,scanLineBuffer,EDrawModeXOR);
		 			scanLineBufferMask = GetScanLineOffsetPtr(aMaskBitmap, slptr, width, srcePoint, aMaskBase, lineScanPosMask, offset);
					drawDevice->WriteLine(aDest.iX,destY,width,scanLineBufferMask,drawMode);
					drawDevice->WriteLine(aDest.iX,destY,width,scanLineBuffer,EDrawModeXOR);
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
						drawDevice->WriteLine(aDest.iX,destY,width,scanLineBuffer,EDrawModeXOR);
						drawDevice->WriteLine(aDest.iX,destY,width,scanLineBufferMask,drawMode);
						drawDevice->WriteLine(aDest.iX,destY,width,scanLineBuffer,EDrawModeXOR);
						destY++,srcePoint.iY++,ditherOrigin.iY++;
		 				}
		 			while ((srcePoint.iY < aSourceRect.iBr.iY) && (scanLineBuffer < lastScanLineSrc) && (scanLineBufferMask < lastScanLineMask)
		 				&& ((scanLineBuffer = (TUint32*)((TUint8*)scanLineBuffer + strideSrc))>(TUint32*)0)
		 				&& ((scanLineBufferMask = (TUint32*)((TUint8*)scanLineBufferMask + strideMask))>(TUint32*)0)
		 				);
					}
				}
			}
		else
			{
			TUint32* scanLineBufferPtr = NULL;
			//stride jumping not possible with compressed bitmaps
			if (aSourceBitmap->IsCompressed())
				{
				for (; srcePoint.iY < aSourceRect.iBr.iY; destY++,srcePoint.iY++,ditherOrigin.iY++)
					{
		 			scanLineBufferPtr = GetScanLineOffsetPtr(aSourceBitmap, slptr, width, srcePoint, aSourceBase, lineScanPos, offset);
					drawDevice->WriteLine(aDest.iX,destY,width,scanLineBufferPtr,EDrawModeXOR);
					aMaskBitmap->GetScanLine(scanLineDes,srcePoint,width,EFalse,ditherOrigin,dispMode, aMaskBase, lineScanPosMask);
					::TileScanLine(scanLineDes, width, srcePoint, aMaskBitmap, lineScanPosMask, aMaskBase, dispMode);
					drawDevice->WriteLine(aDest.iX,destY,width,scanLineBuffer,drawMode);
					drawDevice->WriteLine(aDest.iX,destY,width,scanLineBufferPtr,EDrawModeXOR);
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
						drawDevice->WriteLine(aDest.iX,destY,width,scanLineBufferPtr,EDrawModeXOR);
						aMaskBitmap->GetScanLine(scanLineDes,srcePoint,width,EFalse,ditherOrigin,dispMode,aMaskBase, lineScanPosMask);
						::TileScanLine(scanLineDes, width, srcePoint, aMaskBitmap, lineScanPosMask, aMaskBase, dispMode);
						drawDevice->WriteLine(aDest.iX,destY,width,scanLineBuffer,drawMode);
						drawDevice->WriteLine(aDest.iX,destY,width,scanLineBufferPtr,EDrawModeXOR);
						destY++,srcePoint.iY++,ditherOrigin.iY++;
		 				}
					while ((srcePoint.iY < aSourceRect.iBr.iY) && (scanLineBufferPtr < lastScanLine)
						&& ((scanLineBufferPtr = (TUint32*)((TUint8*)scanLineBufferPtr + stride))>(TUint32*)0));
					}
				}
			}
		}
	else
		{
		for (; srcePoint.iY < aSourceRect.iBr.iY; destY++,srcePoint.iY++,ditherOrigin.iY++)
			{
			aSourceBitmap->GetScanLine(scanLineDes,srcePoint,width,ETrue,ditherOrigin,
								dispMode,aSourceBase,lineScanPos);

			if (aShadowMode)
				{
				drawDevice->SetShadowMode(CFbsDrawDevice::TShadowMode(aShadowMode));
				drawDevice->ShadowBuffer(width,scanLineBuffer);
				drawDevice->SetShadowMode(CFbsDrawDevice::ENoShadow);
				}

			drawDevice->WriteLine(aDest.iX,destY,width,scanLineBuffer,EDrawModeXOR);

			aMaskBitmap->GetScanLine(scanLineDes,srcePoint,width,EFalse,ditherOrigin,dispMode,
									 aMaskBase, lineScanPosMask);
			::TileScanLine(scanLineDes, width, srcePoint, aMaskBitmap, lineScanPosMask,
						   aMaskBase, dispMode);
			drawDevice->WriteLine(aDest.iX,destY,width,scanLineBuffer,drawMode);

			aSourceBitmap->GetScanLine(scanLineDes,srcePoint,width,ETrue,ditherOrigin,dispMode,
							   aSourceBase,lineScanPos2);

			if(aShadowMode)
				{
				drawDevice->SetShadowMode(CFbsDrawDevice::TShadowMode(aShadowMode));
				drawDevice->ShadowBuffer(width,scanLineBuffer);
				drawDevice->SetShadowMode(CFbsDrawDevice::ENoShadow);
				}

			drawDevice->WriteLine(aDest.iX,destY,width,scanLineBuffer,EDrawModeXOR);
			}
		}
	}

void CFbsBitGc::DoBitBltMaskedNonFlicker(const TPoint& aDest,
										 CBitwiseBitmap* aSourceBitmap,
										 TUint32* aSourceBase,
										 const TRect& aSourceRect,
										 CBitwiseBitmap* aMaskBitmap,
										 TUint32* aMaskBase,
										 TBool aInvertMask,
										 const TPoint& aDitherOrigin,
										 TInt aShadowMode)
	{
	CFbsDrawDevice* drawDevice = iDevice->iDrawDevice;

	const TInt width = aSourceRect.Width();
	TInt destY = aDest.iY;
	TPoint srcePoint(aSourceRect.iTl);

	TLineScanningPosition lineScanPos(aSourceBase);
	TLineScanningPosition lineScanPosMask(aMaskBase);

	const TDisplayMode srcFormat = aSourceBitmap->DisplayMode();
	const TDisplayMode maskFormat = aMaskBitmap->DisplayMode();
	
	if (aMaskBitmap->IsCompressed()) 
       { 
       HBufC8* hBuf= CFbsBitmap::GetDecompressionBuffer(aMaskBitmap->DataStride() + 4); 
       if (!hBuf) 
          return; // Out of memory so do not draw anything 
       lineScanPosMask.iScanLineBuffer = hBuf; 
       } 

	TAny* interface=NULL;
	if ( (srcFormat == EColor16MU || srcFormat == EColor64K ) &&
		maskFormat == EGray2 &&
		!aShadowMode &&
		aSourceBitmap->SizeInPixels().iWidth <= aMaskBitmap->SizeInPixels().iWidth &&
		aSourceBitmap->SizeInPixels().iHeight <= aMaskBitmap->SizeInPixels().iHeight &&
		drawDevice->GetInterface(KFastBlitInterfaceID, interface) == KErrNone )
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

	TPoint ditherOrigin(aDitherOrigin + aDest);

	TUint32* scanLineBuffer = drawDevice->ScanLineBuffer();
	const TInt scanLineBytes = drawDevice->ScanLineBytes();
	TPtr8 scanLineDes((TUint8*)scanLineBuffer,scanLineBytes,scanLineBytes);

	TUint32* bitBltBuffer = (TUint32*)iDevice->iBitBltMaskedBuffer;
	const TDisplayMode dispMode = ScanLineBufferDisplayMode(drawDevice);
	const TInt bufferBytes = CFbsBitmap::ScanLineLength(width, dispMode);
	TUint32* scanLineCopy = (TUint32*) new TUint8[bufferBytes];

	TLineScanningPosition lineScanPos2(aSourceBase);
	for (; srcePoint.iY < aSourceRect.iBr.iY; destY++,srcePoint.iY++,ditherOrigin.iY++)
		{
		drawDevice->ReadLine(aDest.iX,destY,width,bitBltBuffer,dispMode);
		aSourceBitmap->GetScanLine(scanLineDes,srcePoint,width,ETrue,ditherOrigin,dispMode,
								   aSourceBase,lineScanPos);

		if (scanLineCopy)
			{
			Mem::Copy(scanLineCopy,scanLineBuffer,bufferBytes);
			}

		if (aShadowMode)
			{
			drawDevice->SetShadowMode(CFbsDrawDevice::TShadowMode(aShadowMode));
			drawDevice->ShadowBuffer(width,scanLineBuffer);
			drawDevice->SetShadowMode(CFbsDrawDevice::ENoShadow);
			}

		XorBuffers(bitBltBuffer,scanLineBuffer,bufferBytes);

		aMaskBitmap->GetScanLine(scanLineDes,srcePoint,width,EFalse,ditherOrigin,dispMode,
								 aMaskBase,lineScanPosMask);
		::TileScanLine(scanLineDes, width, srcePoint, aMaskBitmap, lineScanPosMask,
					   aMaskBase, dispMode);

		if (!aInvertMask)
			InvertBuffer(scanLineBuffer,bufferBytes);
		AndBuffers(bitBltBuffer,scanLineBuffer,bufferBytes);

		// Slow call when memory low?
		TUint32* scanLine;
		if (!scanLineCopy)
			{
			aSourceBitmap->GetScanLine(scanLineDes,srcePoint,width,ETrue,ditherOrigin,
									   dispMode,aSourceBase,lineScanPos2);
			scanLine = scanLineBuffer;
			}
		else
			scanLine = scanLineCopy;

		if(aShadowMode)
			{
			drawDevice->SetShadowMode(CFbsDrawDevice::TShadowMode(aShadowMode));
			drawDevice->ShadowBuffer(width,scanLine);
			drawDevice->SetShadowMode(CFbsDrawDevice::ENoShadow);
			}

		XorBuffers(bitBltBuffer,scanLine,bufferBytes);

		drawDevice->WriteLine(aDest.iX,destY,width,bitBltBuffer,iDrawMode);
		}
	if (scanLineCopy)
		{
		delete[] scanLineCopy;
		}
	}

void CFbsBitGc::DoBitBltMaskedNonFlickerSolid(const TPoint& aDest,
											  CBitwiseBitmap* aSourceBitmap,
											  TUint32* aSourceBase,
											  const TRect& aSourceRect,
											  CBitwiseBitmap* aMaskBitmap,
											  TUint32* aMaskBase,
											  TBool aInvertMask,
											  const TPoint& aDitherOrigin,
											  TInt aShadowMode)
	{
	CFbsDrawDevice* drawDevice = iDevice->iDrawDevice;
	const TInt width = aSourceRect.Width();
	TPoint ditherOrigin(aDitherOrigin + aDest);
	const TDisplayMode dispMode = ScanLineBufferDisplayMode(drawDevice);
	TInt destY = aDest.iY;
	TPoint srcePoint(aSourceRect.iTl);

	TUint32* scanLineBuffer = drawDevice->ScanLineBuffer();
	const TInt scanLineBytes = drawDevice->ScanLineBytes();
	TPtr8 scanLineDes((TUint8*)scanLineBuffer,scanLineBytes,scanLineBytes);

	TUint32* bitBltBuffer = (TUint32*)iDevice->iBitBltMaskedBuffer;
	TUint32* backgroundBuffer = (TUint32*)(iDevice->iBitBltMaskedBuffer + scanLineBytes);
	const TInt bufferBytes = CFbsBitmap::ScanLineLength(width, dispMode);

	drawDevice->WriteRgbMulti(aDest.iX,aDest.iY,width,1,iBrushColor,iDrawMode);
	drawDevice->ReadLine(aDest.iX,destY,width,backgroundBuffer,dispMode);

	TLineScanningPosition lineScanPos(aSourceBase);
	TLineScanningPosition lineScanPos2(aSourceBase);
	TLineScanningPosition lineScanPosMask(aMaskBase);

	for (; srcePoint.iY < aSourceRect.iBr.iY; destY++,srcePoint.iY++,ditherOrigin.iY++)
		{
		Mem::Copy(bitBltBuffer,backgroundBuffer,bufferBytes);

		aSourceBitmap->GetScanLine(scanLineDes,srcePoint,width,ETrue,ditherOrigin,dispMode,
								   aSourceBase,lineScanPos);
		XorBuffers(bitBltBuffer,scanLineBuffer,bufferBytes);

		aMaskBitmap->GetScanLine(scanLineDes,srcePoint,width,EFalse,ditherOrigin,dispMode,
								 aMaskBase,lineScanPosMask);
		::TileScanLine(scanLineDes, width, srcePoint, aMaskBitmap, lineScanPosMask,
					   aMaskBase, dispMode);

		if (!aInvertMask)
			InvertBuffer(scanLineBuffer,bufferBytes);
		AndBuffers(bitBltBuffer,scanLineBuffer,bufferBytes);
		aSourceBitmap->GetScanLine(scanLineDes,srcePoint,width,ETrue,ditherOrigin,
								   dispMode,aSourceBase,lineScanPos2);

		XorBuffers(bitBltBuffer,scanLineBuffer,bufferBytes);

		if(aShadowMode)
			drawDevice->SetShadowMode(CFbsDrawDevice::TShadowMode(aShadowMode));

		drawDevice->WriteLine(aDest.iX,destY,width,bitBltBuffer,iDrawMode);

		if(aShadowMode)
			drawDevice->SetShadowMode(CFbsDrawDevice::ENoShadow);
		}
	}

void CFbsBitGc::DoBitBltMaskedNonFlickerPatterned(const TPoint& aDest,
												  CBitwiseBitmap* aSourceBitmap,
												  TUint32* aSourceBase,
												  const TRect& aSourceRect,
												  CBitwiseBitmap* aMaskBitmap,
												  TUint32* aMaskBase,
												  TBool aInvertMask,
												  const TPoint& aDitherOrigin,
												  TInt aShadowMode)
	{
	CFbsDrawDevice* drawDevice = iDevice->iDrawDevice;
	const TDisplayMode dispMode = ScanLineBufferDisplayMode(drawDevice);

	drawDevice->SetShadowMode(CFbsDrawDevice::TShadowMode(aShadowMode));

	iBrushBitmap.BeginDataAccess();
	CBitwiseBitmap* brushBitmap = iBrushBitmap.Address();
	BG_ASSERT_ALWAYS(iBrushUsed, EBitgdiPanicInvalidBitmap);
	BG_ASSERT_ALWAYS(brushBitmap != NULL, EBitgdiPanicInvalidBitmap);

	CFbsRasterizer* brushRasterizer = PrepareRasterizerForExtendedBitmap(iBrushBitmap);
	TUint32* brushData = iBrushBitmap.DataAddress();
	TPoint brushSourcePoint(aDest - iBrushOrigin - iOrigin);
	const TSize brushSize(iBrushBitmap.SizeInPixels());
	if (brushSourcePoint.iX < 0 || brushSourcePoint.iX >= brushSize.iWidth)
		brushSourcePoint.iX %= brushSize.iWidth;
	if (brushSourcePoint.iX < 0)
		brushSourcePoint.iX += brushSize.iWidth;

	TUint32* scanLineBuffer = drawDevice->ScanLineBuffer();
	const TInt scanLineBytes = drawDevice->ScanLineBytes();
	TPtr8 scanLineDes((TUint8*)scanLineBuffer,scanLineBytes,scanLineBytes);

	TUint32* bitBltBuffer = (TUint32*)iDevice->iBitBltMaskedBuffer;
	TPtr8 bitBltDes(iDevice->iBitBltMaskedBuffer, scanLineBytes, scanLineBytes);

	TInt widthRemaining = aSourceRect.Width();
	TPoint sourcePoint(aSourceRect.iTl);
	TInt destX = aDest.iX;

	TInt width = Min(widthRemaining,brushSize.iWidth);

	if (brushSourcePoint.iX + widthRemaining > brushSize.iWidth)
		width = brushSize.iWidth - brushSourcePoint.iX;

	while (widthRemaining > 0)
		{
		TInt destY = aDest.iY;
		sourcePoint.iY = aSourceRect.iTl.iY;
		brushSourcePoint.iY = aDest.iY - iBrushOrigin.iY - iOrigin.iY;
		TPoint ditherOrigin(aDitherOrigin + TPoint(destX,aDest.iY));
		const TInt bufferBytes = CFbsBitmap::ScanLineLength(width, dispMode);

		TLineScanningPosition lineScanPosBrush(brushData);
		TLineScanningPosition lineScanPosMask(aMaskBase);
		TLineScanningPosition lineScanPosSrc(aSourceBase);
		TLineScanningPosition lineScanPosSrc2(aSourceBase);

		for ( ;sourcePoint.iY < aSourceRect.iBr.iY;
			   destY++,sourcePoint.iY++,ditherOrigin.iY++,brushSourcePoint.iY++)
			{
			brushBitmap->GetScanLine(bitBltDes,brushSourcePoint,width,ETrue,ditherOrigin,
									 dispMode,brushData,lineScanPosBrush);
			aSourceBitmap->GetScanLine(scanLineDes,sourcePoint,width,ETrue,ditherOrigin,
									   dispMode,aSourceBase,lineScanPosSrc);
			XorBuffers(bitBltBuffer,scanLineBuffer,scanLineBytes);

			aMaskBitmap->GetScanLine(scanLineDes,sourcePoint,width,EFalse,ditherOrigin,
									 dispMode,aMaskBase,lineScanPosMask);
			::TileScanLine(scanLineDes, width, sourcePoint, aMaskBitmap, lineScanPosMask,
						   aMaskBase, dispMode);

			if (!aInvertMask)
				InvertBuffer(scanLineBuffer,bufferBytes);
			AndBuffers(bitBltBuffer,scanLineBuffer,bufferBytes);

			aSourceBitmap->GetScanLine(scanLineDes,sourcePoint,width,ETrue,ditherOrigin,
									   dispMode,aSourceBase,lineScanPosSrc2);
			XorBuffers(bitBltBuffer,scanLineBuffer,bufferBytes);

			drawDevice->WriteLine(destX,destY,width,bitBltBuffer,iDrawMode);
			}

		widthRemaining -= width;
		sourcePoint.iX += width;
		brushSourcePoint.iX += width;
		destX += width;

		width = Min(widthRemaining,brushSize.iWidth);
		}
	if (brushRasterizer)
		{
		brushRasterizer->EndBitmap(iBrushBitmap.SerialNumber());
		}
	iBrushBitmap.EndDataAccess(ETrue);
	}

void CFbsBitGc::DoBitBltAlpha(const TPoint& aDest,CBitwiseBitmap* aSourceBitmap,
							  TUint32* aSourceBase,const TRect& aSourceRect,
							  CBitwiseBitmap* aMaskBitmap,TUint32* aMaskBase,
							  const TPoint& aAlphaPoint,TInt aShadowMode, TBool aInvertMask)
	{
	MFastBlend* fastBlend=NULL;
	if (FastBlendInterface(aSourceBitmap,aMaskBitmap,fastBlend)==KErrNone)
		{
		if (fastBlend->FastBlendBitmapMasked(aDest, aSourceBase, aSourceBitmap->DataStride(), aSourceBitmap->SizeInPixels(), aSourceRect, aSourceBitmap->DisplayMode(),
							aMaskBase, aMaskBitmap->DataStride(), aMaskBitmap->DisplayMode(), aMaskBitmap->SizeInPixels(), aAlphaPoint, aInvertMask,
							iDrawMode, aShadowMode)==KErrNone)
			{
			return;
			}
		}
	CFbsDrawDevice* drawDevice = iDevice->iDrawDevice;

	const TPoint KZeroPoint(0,0);
	const TInt KScanLineLength = 256;
	const TInt KRgbSize = 4;

	TUint8 srceRgbBuffer[KScanLineLength * KRgbSize];
	TUint8 maskBuffer[KScanLineLength];
	TUint8* srceRgbBufferPtr(srceRgbBuffer);

	TPtr8 srceRgbDes(srceRgbBuffer,KScanLineLength * KRgbSize,KScanLineLength * KRgbSize);
	TPtr8 maskDes(maskBuffer,KScanLineLength,KScanLineLength);

	drawDevice->SetShadowMode(CFbsDrawDevice::TShadowMode(aShadowMode));

	const TSize sourceSize = aSourceBitmap->SizeInPixels();
	// Convert negative or large offsets into sensible positive ones.
	TPoint alphaOffset(aAlphaPoint.iX % sourceSize.iWidth, aAlphaPoint.iY % sourceSize.iHeight);
	if ( alphaOffset.iX < 0 ) 
		alphaOffset.iX += sourceSize.iWidth;
	if ( alphaOffset.iY < 0 ) 
		alphaOffset.iY += sourceSize.iHeight;
	
	TInt srceY = aSourceRect.iTl.iY;
	TInt destY = aDest.iY;
	TInt alphaY = alphaOffset.iY;
	
	TLineScanningPosition lineScanPosSrc(aSourceBase);
	TLineScanningPosition lineScanPosMask(aMaskBase);

	TDisplayMode sourceMode = aSourceBitmap->DisplayMode();

	if (aMaskBitmap->IsCompressed())
		{
		HBufC8* hBuf= CFbsBitmap::GetDecompressionBuffer(aMaskBitmap->DataStride());
		if (!hBuf)
			{
			return;  // Out of memory so do not draw anything
			}
		lineScanPosMask.iScanLineBuffer = hBuf;
		}

	TAny* interface=NULL;
	if ( (sourceMode == EColor16MU || sourceMode == EColor64K) &&
			aMaskBitmap->DisplayMode() == EGray256 && // ensure a monochrome mask isn't passed in as an alpha channel
			aSourceBitmap->SizeInPixels().iWidth  <= aMaskBitmap->SizeInPixels().iWidth &&
			aSourceBitmap->SizeInPixels().iHeight <= aMaskBitmap->SizeInPixels().iHeight &&
			drawDevice->GetInterface(KFastBlitInterfaceID, interface) == KErrNone )
		{
		TInt length = aSourceRect.Width();
		const TInt srceX = aSourceRect.iTl.iX;				
		const TInt alphaX = alphaOffset.iX;
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

			fastBlit->WriteAlphaLineEx(destX,destY,length,srceX,srcPtr,sourceMode,alphaX,maskPtr,MAlphaBlend::EShdwBefore);

			srceY++;
			destY++;
			alphaY++;
			}
		return;
		}

	const TBool useScanLinePtr = ((!aShadowMode) && (EColor16MA == aSourceBitmap->DisplayMode()));
	TUint32* slptr=NULL;
	TUint offset = 0;
	TDisplayMode srcMode = ERgb;
	
	while (srceY < aSourceRect.iBr.iY)
		{
		TInt srceX = aSourceRect.iTl.iX;
		TInt destX = aDest.iX;
		TInt alphaX = alphaOffset.iX;
		
		while (srceX < aSourceRect.iBr.iX)
			{
			TPoint srcePoint(srceX,srceY);
			TPoint alphaPoint(alphaX,alphaY);
			const TInt width = Min(KScanLineLength,aSourceRect.iBr.iX - srceX);

			if (useScanLinePtr)
				{
				offset = MemoryOffsetForPixelPitch(srceX, EColor16MU);
				srceRgbBufferPtr = (TUint8*)GetScanLineOffsetPtr(aSourceBitmap, slptr, width, srcePoint, aSourceBase, lineScanPosSrc, offset);
				}
			else
				{
				aSourceBitmap->GetScanLine(srceRgbDes,srcePoint,width,EFalse,KZeroPoint,
								   srcMode,aSourceBase,lineScanPosSrc);
				}

			aMaskBitmap->GetScanLine(maskDes,alphaPoint,width,EFalse,KZeroPoint,
									 EGray256,aMaskBase,lineScanPosMask);
			::TileScanLine(maskDes, width, alphaPoint, aMaskBitmap, lineScanPosMask,
						   aMaskBase, EGray256);
			// aInvertMask is not used for alpha channels (EGray256 mask)
			if (aInvertMask && aMaskBitmap->DisplayMode() != EGray256)
				{
				for (TInt i = 0; i < width; ++i)
					maskBuffer[i] = ~maskBuffer[i];
				}
			drawDevice->WriteRgbAlphaLine(destX,destY,width,srceRgbBufferPtr,maskBuffer,iDrawMode);

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
The method performs an alpha blending of the source data - aSrcBmp1 and aSrcBmp2, using
the data from aAlphaBmp as an alpha blending factor.
@internalComponent
@see CFbsBitGc::AlphaBlendBitmaps.
*/
void CFbsBitGc::DoBitBltAlpha(const TPoint& aDestPt,
							  const CBitwiseBitmap* aSrcBmp1,
							  TUint32* aSrcBmpDataAddr1,
							  const CBitwiseBitmap* aSrcBmp2,
							  TUint32* aSrcBmpDataAddr2,
							  const CBitwiseBitmap* aAlphaBmp,
							  TUint32* aAlphaBmpDataAddr,
							  const TRect& aSrcRect1,
							  const TPoint& aSrcPt2,
							  const TPoint& aAlphaPt,
							  TInt aShadowMode)
	{
	CFbsDrawDevice* drawDevice = iDevice->iDrawDevice;
#ifdef _DEBUG
	TRect deviceDestRect;
	drawDevice->GetDrawRect(deviceDestRect);
#endif
	//Check the destination point.
	BG_ASSERT_DEBUG(aDestPt.iX >= deviceDestRect.iTl.iX, EBitgdiPanicOutOfBounds);
	BG_ASSERT_DEBUG(aDestPt.iY >= deviceDestRect.iTl.iY, EBitgdiPanicOutOfBounds);
	BG_ASSERT_DEBUG((aDestPt.iX + aSrcRect1.Width()) <= deviceDestRect.iBr.iX, EBitgdiPanicOutOfBounds);
	BG_ASSERT_DEBUG((aDestPt.iY + aSrcRect1.Height()) <= deviceDestRect.iBr.iY, EBitgdiPanicOutOfBounds);

	const TPoint KZeroPoint(0,0);
	const TInt KScanLineLength = 256;//128 is not enough to fill buffer for 16MA and 16MU display modes
	const TInt KRgbSize = 4;

	TUint8 srceRgbBuffer1[KScanLineLength * KRgbSize];
	TUint8 srceBuffer2[KScanLineLength * KRgbSize];
	TUint8 alphaBuffer[KScanLineLength];

	TPtr8 srceRgbDes1(srceRgbBuffer1,KScanLineLength * KRgbSize,KScanLineLength * KRgbSize);
	TPtr8 srceDes2(srceBuffer2,KScanLineLength * KRgbSize,KScanLineLength * KRgbSize);
	TPtr8 alphaDes(alphaBuffer,KScanLineLength,KScanLineLength);

	const TDisplayMode dispMode = ScanLineBufferDisplayMode(drawDevice);
	drawDevice->SetShadowMode(CFbsDrawDevice::TShadowMode(aShadowMode));

	TInt srceY1 = aSrcRect1.iTl.iY;
	TInt srceY2 = aSrcPt2.iY;
	TInt alphaY = aAlphaPt.iY;
	TInt destY = aDestPt.iY;

	TLineScanningPosition lineScanPosSrc1(aSrcBmpDataAddr1);
	TLineScanningPosition lineScanPosSrc2(aSrcBmpDataAddr2);
	TLineScanningPosition lineScanPosAlpha(aAlphaBmpDataAddr);

	while (srceY1 < aSrcRect1.iBr.iY)
		{
		TInt srceX1 = aSrcRect1.iTl.iX;
		TInt srceX2 = aSrcPt2.iX;
		TInt alphaX = aAlphaPt.iX;
		TInt destX = aDestPt.iX;

		while (srceX1 < aSrcRect1.iBr.iX)
			{
			TPoint srcePoint1(srceX1,srceY1);
			TPoint srcePoint2(srceX2,srceY2);
			TPoint alphaPoint(alphaX,alphaY);
			const TInt width = Min(KScanLineLength,aSrcRect1.iBr.iX - srceX1);

			aSrcBmp1->GetScanLine(srceRgbDes1,srcePoint1,width,EFalse,KZeroPoint,EColor16MU,
								  aSrcBmpDataAddr1,lineScanPosSrc1);
			aSrcBmp2->GetScanLine(srceDes2,srcePoint2,width,EFalse,KZeroPoint,dispMode,
								  aSrcBmpDataAddr2,lineScanPosSrc2);
			aAlphaBmp->GetScanLine(alphaDes,alphaPoint,width,EFalse,KZeroPoint,EGray256,
								   aAlphaBmpDataAddr,lineScanPosAlpha);
			::TileScanLine(alphaDes, width, alphaPoint, aAlphaBmp, lineScanPosAlpha, aAlphaBmpDataAddr, EGray256);

			drawDevice->WriteRgbAlphaLine(destX,
										  destY,
										  width,
										  srceRgbBuffer1,
										  srceBuffer2,
										  alphaBuffer,
										  iDrawMode);

			srceX1 += KScanLineLength;
			srceX2 += KScanLineLength;
			alphaX += KScanLineLength;
			destX += KScanLineLength;
			}

		srceY1++;
		srceY2++;
		alphaY++;
		destY++;
		}
	}

/**
The method performs an alpha blending of the source data - aSrcBmp1 and aSrcBmp2, using
the data from aAlphaBmp as an alpha blending factor.
The formula used for that, is:
(C1 * A + C2 * (255 - A)) / 255, where:
- C1 - a pixel from aSrcBmp1;
- C2 - a pixel from aSrcBmp2;
- A  - a pixel from aAlphaBmp;
The content of source and alpha bitmap is preserved.
The calculated alpha blended pixels are written to the destination - the screen or a bitmap.
@publishedAll
@released
@param aDestPt Position in the target the result should be drawn to.
@param aSrcBmp1 A pointer to the source bitmap 1.
@param aSrcBmp2 A pointer to the source bitmap 2.
@param aAlphaBmp A pointer to the bitmap used as an alpha blending factor.
@param aSrcRect1 A part of bitmap 1 that should be used as a source for the alpha blending.
@param aSrcPt2 Position of the first pixel in bitmap 2 that should be used as a source
               for the alpha blending. The size of the area is the same as the
			   bitmap 1 area - aSrcRect1 parameter.
@param aAlphaPt Position of the first pixel in the alpha bitmap that should be used as a source
                for the alpha blending. The size of the area is the same as the
                bitmap 1 area - aSrcRect1 parameter.
@pre !aSrcRect1.IsEmpty()
@pre aSrcBmp1 != NULL
@pre aSrcBmp1->Handle() != NULL
@pre aSrcBmp2 != NULL
@pre aSrcBmp2->Handle() != NULL
@pre aAlphaBmp != NULL
@pre aAlphaBmp->Handle() != NULL
@pre aAlphaBmp->DisplayMode() <= EGray256
@return KErrNone If the call is successful, KErrArgument otherwise.
*/
EXPORT_C  TInt CFbsBitGc::AlphaBlendBitmaps(const TPoint& aDestPt,
											const CFbsBitmap* aSrcBmp1,
											const CFbsBitmap* aSrcBmp2,
											const TRect& aSrcRect1,
											const TPoint& aSrcPt2,
											const CFbsBitmap* aAlphaBmp,
											const TPoint& aAlphaPt)
	{
	//Check the bitmap pointers and handles. Check the CFbsDevice instance -
	//it shouldn't be NULL - that means - CFbsBitGc instance (this pointer)
	//should be created either using CFbsDevice::CreateContext() or CFbsBitGc::Activate() -
	//before the method was called.
	if(!aSrcBmp1 || !aSrcBmp1->Handle() || !aSrcBmp2 || !aSrcBmp2->Handle() ||
	   !aAlphaBmp || !aAlphaBmp->Handle() || CheckDevice(aSrcRect1))
		{
		return KErrArgument;
		}
		
	aSrcBmp1->BeginDataAccess();
	aSrcBmp2->BeginDataAccess();
	aAlphaBmp->BeginDataAccess();
		
	//Check display mode of the alpha bitmap. Bitmaps which display mode is greater than
	//EGray256 can't be used as alpha bitmaps.
	if(aAlphaBmp->DisplayMode() > EGray256)
		{
		aSrcBmp1->EndDataAccess(ETrue);
		aSrcBmp2->EndDataAccess(ETrue);
		aAlphaBmp->EndDataAccess(ETrue);
		return KErrArgument;
		}
	//Check source rect 1. Bitmap 1 must contain the whole source rect 1.
	TRect srcRect1(aSrcRect1);
	TRect area1(aSrcBmp1->SizeInPixels());
	srcRect1.Intersection(area1);
	if(srcRect1 != aSrcRect1)
		{
		aSrcBmp1->EndDataAccess(ETrue);
		aSrcBmp2->EndDataAccess(ETrue);
		aAlphaBmp->EndDataAccess(ETrue);
		return KErrArgument;
		}
	//Create and check source rect 2. Bitmap 2 must contain the whole source rect 2.
	TRect srcRect2(TSize(aSrcRect1.Width(), aSrcRect1.Height()));
	srcRect2.Move(aSrcPt2);
	TRect srcRect2t(srcRect2);
	TRect area2(aSrcBmp2->SizeInPixels());
	srcRect2.Intersection(area2);
	if(srcRect2 != srcRect2t)
		{
		aSrcBmp1->EndDataAccess(ETrue);
		aSrcBmp2->EndDataAccess(ETrue);
		aAlphaBmp->EndDataAccess(ETrue);
		return KErrArgument;
		}
	//Calculate the destination rect
	TPoint destPt(aDestPt + iOrigin);
	TRect destRect(destPt, srcRect1.Size());
	TPoint offset(srcRect1.iTl - destPt);
	TPoint offset2(srcRect2.iTl - destPt);
	TRect clippedDestRect(destRect);
	AddRect(clippedDestRect);
	if(UserClipRect(clippedDestRect))
		{
		aSrcBmp1->EndDataAccess(ETrue);
		aSrcBmp2->EndDataAccess(ETrue);
		aAlphaBmp->EndDataAccess(ETrue);
		return KErrArgument;
		}
	//Save current shadow mode
	TInt8 shadowMode = iShadowMode;
	iShadowMode = CFbsDrawDevice::ENoShadow;
	//Setup the device and prevent the bitmaps from being cleaned away by client code.
	//Drawing begins.
	SetupDevice();
	iDevice->DrawingBegin();
	CBitwiseBitmap* srcBmp1 = ((CFbsBitGcBitmap*)aSrcBmp1)->Address();
	CBitwiseBitmap* srcBmp2 = ((CFbsBitGcBitmap*)aSrcBmp2)->Address();
	CBitwiseBitmap* alphaBmp = ((CFbsBitGcBitmap*)aAlphaBmp)->Address();
	BG_ASSERT_DEBUG(srcBmp1, EBitgdiPanicInvalidBitmap);
	BG_ASSERT_DEBUG(srcBmp2, EBitgdiPanicInvalidBitmap);
	BG_ASSERT_DEBUG(alphaBmp, EBitgdiPanicInvalidBitmap);

	TUint32* srcDataAddr1 = aSrcBmp1->DataAddress();
	TUint32* srcDataAddr2 = aSrcBmp2->DataAddress();
	TUint32* alphaDataAddr = aAlphaBmp->DataAddress();

	CFbsRasterizer* rasterizer1 = PrepareRasterizerForExtendedBitmap(*aSrcBmp1, clippedDestRect, offset);
	CFbsRasterizer* rasterizer2 = PrepareRasterizerForExtendedBitmap(*aSrcBmp2, clippedDestRect, offset2);
	CFbsRasterizer* alphaRasterizer;
	if (aAlphaPt.iX >= 0 && aAlphaPt.iY >= 0
		&& aAlphaPt.iX + aSrcRect1.Width() <= aAlphaBmp->SizeInPixels().iWidth
		&& aAlphaPt.iY + aSrcRect1.Height() <= aAlphaBmp->SizeInPixels().iHeight)
		{
		// Alpha blending bitmap is not tiled. Pass same region of interest as source bitmaps to rasterizer.
		alphaRasterizer = PrepareRasterizerForExtendedBitmap(*aAlphaBmp, clippedDestRect, aAlphaPt - destPt);
		}
	else
		{
		// Alpha blending bitmap is tiled. Do not pass any region of interest to rasterizer.
		alphaRasterizer = PrepareRasterizerForExtendedBitmap(*aAlphaBmp);
		}

	//For each region - find the clipping rect and draw
	TInt limit = iDefaultRegionPtr->Count();
	CGraphicsAccelerator* ga = GraphicsAccelerator();
	// Code for Graphics Accelerated Drawing
	if(ga && (shadowMode == CFbsDrawDevice::ENoShadow))
		{
		TInt gaOperationResult = KErrUnknown;
		TAcceleratedBitmapSpec srcBmp1Spec(const_cast<CFbsBitmap*>(aSrcBmp1));
		TAcceleratedBitmapSpec srcBmp2Spec(const_cast<CFbsBitmap*>(aSrcBmp2));
		TAcceleratedBitmapSpec alphaBmpSpec(const_cast<CFbsBitmap*>(aAlphaBmp));
		iDevice->DrawingEnd();

		for(TInt count=0;count<limit;count++)
			{
			iClipRect=(*iDefaultRegionPtr)[count];
			if(!iClipRect.Intersects(clippedDestRect))
				{
				continue;
				}
			//clippedDestRect was constructed from destRect. destRect was constructed from srcRect1.
			iClipRect.Intersection(clippedDestRect);
			TRect clippedSrcRect(iClipRect);
			clippedSrcRect.Move(offset);//clippedSrcRect - maps to a part of srcRect1 now.
			TPoint shift(clippedSrcRect.iTl - srcRect1.iTl);
			BG_ASSERT_DEBUG(shift.iX >= 0, EBitgdiPanicNegativeShift);
			BG_ASSERT_DEBUG(shift.iY >= 0, EBitgdiPanicNegativeShift);

			gaOperationResult = ga->Operation(TGopAlphaBlendTwoBitmaps(iClipRect.iTl,srcBmp1Spec,srcBmp2Spec,clippedSrcRect,aSrcPt2 + shift,alphaBmpSpec,aAlphaPt + shift));
			if(gaOperationResult != KErrNone)
				break;
			iDevice->iDrawDevice->UpdateRegion(iClipRect);
			}
		if(gaOperationResult == KErrNone)
			goto finish;
		iDevice->DrawingBegin();
		}

	// Code for non- Graphics Accelerated Drawing
	for(TInt count=0;count<limit;count++)
		{
		iClipRect=(*iDefaultRegionPtr)[count];
		if(!iClipRect.Intersects(clippedDestRect))
			{
			continue;
			}
		//clippedDestRect was constructed from destRect. destRect was constructed from srcRect1.
		iClipRect.Intersection(clippedDestRect);
		TRect clippedSrcRect(iClipRect);
		clippedSrcRect.Move(offset);//clippedSrcRect - maps to a part of srcRect1 now.
		TPoint shift(clippedSrcRect.iTl - srcRect1.iTl);
		BG_ASSERT_DEBUG(shift.iX >= 0, EBitgdiPanicNegativeShift);
		BG_ASSERT_DEBUG(shift.iY >= 0, EBitgdiPanicNegativeShift);

		TDrawMode drawMode = iDrawMode;
		iDrawMode = EDrawModeWriteAlpha; // this is the only mode currently supported
		DoBitBltAlpha(iClipRect.iTl,
					  srcBmp1,
					  srcDataAddr1,
					  srcBmp2,
					  srcDataAddr2,
					  alphaBmp,
					  alphaDataAddr,
					  clippedSrcRect,
					  aSrcPt2 + shift,
					  aAlphaPt + shift,
					  shadowMode);
		iDrawMode = drawMode; // restore the previous draw mode

		iDevice->iDrawDevice->UpdateRegion(iClipRect);
		}

	//Drawig ends. Restore the previous shadow mode
	iDevice->DrawingEnd();
finish:
	if (rasterizer1)
		{
		rasterizer1->EndBitmap(aSrcBmp1->SerialNumber());
		}
	if (rasterizer2)
		{
		rasterizer2->EndBitmap(aSrcBmp2->SerialNumber());
		}
	if (alphaRasterizer)
		{
		alphaRasterizer->EndBitmap(aAlphaBmp->SerialNumber());
		}
	aSrcBmp1->EndDataAccess(ETrue);
	aSrcBmp2->EndDataAccess(ETrue);
	aAlphaBmp->EndDataAccess(ETrue);
	iShadowMode = shadowMode;

	return KErrNone;
	}

/**
The method performs an alpha blending of the source data - aSrcBmp - with the existing image, using
the data from aAlphaBmp as an alpha blending factor.
The formula used for that, is:
(C * A + D * (255 - A)) / 255, where:
- C - a pixel from aSrcBmp;
- D - a pixel from the destination;
- A  - a pixel from aAlphaBmp;
The content of source and alpha bitmap is preserved.
The calculated alpha blended pixels are written to the destination - the screen or a bitmap.
@publishedAll
@released
@param aDestPt Position in the target the result should be drawn to.
@param aSrcBmp A pointer to the source bitmap.
@param aAlphaBmp A pointer to the bitmap used as an alpha blending factor.
@param aSrcRect A part of aSrcBmp that should be used as a source for the alpha blending.
				DISCLAIMER: if aSrcRect is bigger (width and/or height) the behaviour is undefined
@param aAlphaPt Position of the first pixel in the alpha bitmap that should be used as a source
                for the alpha blending. The size of the area is
                the same as the aSrcRect parameter (read DISCLAIMER above).
@pre !aSrcRect.IsEmpty()
@pre aSrcBmp != NULL
@pre aSrcBmp->Handle() != NULL
@pre aAlphaBmp != NULL
@pre aAlphaBmp->Handle() != NULL
@pre aAlphaBmp->DisplayMode() <= EGray256
@return KErrNone If the call is successfull, KErrArgument otherwise.
*/
EXPORT_C  TInt CFbsBitGc::AlphaBlendBitmaps(const TPoint& aDestPt,
											const CFbsBitmap* aSrcBmp,
											const TRect& aSrcRect,
											const CFbsBitmap* aAlphaBmp,
											const TPoint& aAlphaPt)
	{
	//Check the bitmap pointers and handles. Check the CFbsDevice instance -
	//it shouldn't be NULL - that means - CFbsBitGc instance (this pointer)
	//should be created either using CFbsDevice::CreateContext() or CFbsBitGc::Activate() -
	//before the method was called.
	if(!aSrcBmp || !aSrcBmp->Handle() ||
	   !aAlphaBmp || !aAlphaBmp->Handle() || CheckDevice(aSrcRect))
		{
		return KErrArgument;
		}
		
	aSrcBmp->BeginDataAccess();
	aAlphaBmp->BeginDataAccess();
		
	//Check display mode of the alpha bitmap. Bitmaps which display mode is greater than
	//EGray256 can't be used as alpha bitmaps.
	if(aAlphaBmp->DisplayMode() > EGray256)
		{
		aSrcBmp->EndDataAccess(ETrue);
		aAlphaBmp->EndDataAccess(ETrue);
		return KErrArgument;
		}
	//Taking the actual part of the source rect contained in the Bitmap.
	TRect srcRect(aSrcRect);
	TRect area(aSrcBmp->SizeInPixels());
	if(!srcRect.Intersects(area))
		{
		aSrcBmp->EndDataAccess(ETrue);
		aAlphaBmp->EndDataAccess(ETrue);
		return KErrArgument;
		}
	srcRect.Intersection(area);
	//Calculate the destination rect
	TPoint destPt(aDestPt + iOrigin);
	TRect destRect(destPt, srcRect.Size());
	TPoint offset(srcRect.iTl - destPt);
	TRect clippedDestRect(destRect);
	AddRect(clippedDestRect);
	if(UserClipRect(clippedDestRect))
		{
		aSrcBmp->EndDataAccess(ETrue);
		aAlphaBmp->EndDataAccess(ETrue);
		return KErrArgument;
		}
	//Save current shadow mode
	TInt8 shadowMode = iShadowMode;
	iShadowMode = CFbsDrawDevice::ENoShadow;
	//Setup the device and prevent the bitmaps from being cleaned away by client code.
	//Drawing begins.
	SetupDevice();
	iDevice->DrawingBegin();
	
	CBitwiseBitmap* srcBmp = ((CFbsBitGcBitmap*)aSrcBmp)->Address();
	CBitwiseBitmap* alphaBmp = ((CFbsBitGcBitmap*)aAlphaBmp)->Address();
	BG_ASSERT_DEBUG(srcBmp, EBitgdiPanicInvalidBitmap);
	BG_ASSERT_DEBUG(alphaBmp, EBitgdiPanicInvalidBitmap);
	TUint32* srcDataAddr = aSrcBmp->DataAddress();
	TUint32* alphaDataAddr = aAlphaBmp->DataAddress();

	CFbsRasterizer* rasterizer = PrepareRasterizerForExtendedBitmap(*aSrcBmp, clippedDestRect, offset);
	CFbsRasterizer* alphaRasterizer;
	if (aAlphaPt.iX >= 0 && aAlphaPt.iY >= 0
		&& aAlphaPt.iX + srcRect.Width() <= aAlphaBmp->SizeInPixels().iWidth
		&& aAlphaPt.iY + srcRect.Height() <= aAlphaBmp->SizeInPixels().iHeight)
		{
		// Alpha blending bitmap is not tiled. Pass same region of interest as source bitmap to rasterizer.
		alphaRasterizer = PrepareRasterizerForExtendedBitmap(*aAlphaBmp, clippedDestRect, aAlphaPt - destPt);
		}
	else
		{
		// Alpha blending bitmap is tiled. Do not pass any region of interest to rasterizer.
		alphaRasterizer = PrepareRasterizerForExtendedBitmap(*aAlphaBmp);
		}

	//For each region - find the clipping rect and draw
	TInt limit = iDefaultRegionPtr->Count();
	CGraphicsAccelerator* ga = GraphicsAccelerator();
	// Code for Graphics Accelerated Drawing
	if(ga && (shadowMode == CFbsDrawDevice::ENoShadow))
		{
		TInt gaOperationResult = KErrUnknown;
		TAcceleratedBitmapSpec srcBmpSpec(const_cast<CFbsBitmap*>(aSrcBmp));
		TAcceleratedBitmapSpec alphaBmpSpec(const_cast<CFbsBitmap*>(aAlphaBmp));
		iDevice->DrawingEnd();

		for(TInt count=0;count<limit;count++)
			{
			iClipRect=(*iDefaultRegionPtr)[count];
			if(!iClipRect.Intersects(clippedDestRect))
				{
				continue;
				}
			//clippedDestRect was constructed from destRect. destRect was constructed from srcRect.
			iClipRect.Intersection(clippedDestRect);
			TRect clippedSrcRect(iClipRect);
			clippedSrcRect.Move(offset);//clippedSrcRect - maps to a part of srcRect now.
			TPoint shift(clippedSrcRect.iTl - srcRect.iTl);
			BG_ASSERT_DEBUG(shift.iX >= 0, EBitgdiPanicNegativeShift);
			BG_ASSERT_DEBUG(shift.iY >= 0, EBitgdiPanicNegativeShift);

			gaOperationResult = ga->Operation(TGopAlphaBlendOneBitmap(iClipRect.iTl,srcBmpSpec,clippedSrcRect,alphaBmpSpec,aAlphaPt + shift));
			if(gaOperationResult != KErrNone)
				break;
			iDevice->iDrawDevice->UpdateRegion(iClipRect);
			}
		if(gaOperationResult == KErrNone)
			goto finish;
		iDevice->DrawingBegin();
		}

	// Code for non- Graphics Accelerated Drawing
	for(TInt count=0;count<limit;count++)
		{
		iClipRect=(*iDefaultRegionPtr)[count];
		if(!iClipRect.Intersects(clippedDestRect))
			{
			continue;
			}
		//clippedDestRect was constructed from destRect. destRect was constructed from srcRect.
		iClipRect.Intersection(clippedDestRect);
		TRect clippedSrcRect(iClipRect);
		clippedSrcRect.Move(offset);//clippedSrcRect - maps to a part of srcRect now.
		TPoint shift(clippedSrcRect.iTl - srcRect.iTl);
		BG_ASSERT_DEBUG(shift.iX >= 0, EBitgdiPanicNegativeShift);
		BG_ASSERT_DEBUG(shift.iY >= 0, EBitgdiPanicNegativeShift);
		DoBitBltAlpha(iClipRect.iTl,
					  srcBmp,
					  srcDataAddr,
					  clippedSrcRect,
					  alphaBmp,
					  alphaDataAddr,
					  aAlphaPt + shift,
					  shadowMode,
					  EFalse);
		iDevice->iDrawDevice->UpdateRegion(iClipRect);
		}

	// Drawing ends. Restore the previous shadow mode.
	iDevice->DrawingEnd();
finish:
	if (rasterizer)
		{
		rasterizer->EndBitmap(aSrcBmp->SerialNumber());
		}
	if (alphaRasterizer)
		{
		alphaRasterizer->EndBitmap(aAlphaBmp->SerialNumber());
		}
	aSrcBmp->EndDataAccess(ETrue);
	aAlphaBmp->EndDataAccess(ETrue);
	iShadowMode = shadowMode;

	return KErrNone;
	}

GLDEF_C void XorBuffers(TUint32* aDestBuffer,const TUint32* aSrceBuffer,TInt aNumBytes)
	{
	// Round up to nearest word boundary
	const TUint32* const destBufferLimit = aDestBuffer + ((aNumBytes + sizeof(TUint32) - 1) / sizeof(TUint32));

	while (aDestBuffer < destBufferLimit)
		*aDestBuffer++ ^= *aSrceBuffer++;
	}

GLDEF_C void AndBuffers(TUint32* aDestBuffer,const TUint32* aSrceBuffer,TInt aNumBytes)
	{
	// Round up to nearest word boundary
	const TUint32* const destBufferLimit = aDestBuffer + ((aNumBytes + sizeof(TUint32) - 1) / sizeof(TUint32));

	while (aDestBuffer < destBufferLimit)
		*aDestBuffer++ &= *aSrceBuffer++;
	}

GLDEF_C void InvertBuffer(TUint32* aDestBuffer,TInt aNumBytes)
	{
	// Round up to nearest word boundary
	const TUint32* const destBufferLimit = aDestBuffer + ((aNumBytes + sizeof(TUint32) - 1) / sizeof(TUint32));

	while (aDestBuffer < destBufferLimit)
		{
		*aDestBuffer = *aDestBuffer ^ KMaxTUint32;
		++aDestBuffer;
		}
	}

GLDEF_C void InvertBuffer(TUint8* aDestBuffer,TInt aNumBytes)
	{
	const TUint8* const destBufferLimit = aDestBuffer + aNumBytes;

	while (aDestBuffer < destBufferLimit)
		{
		*aDestBuffer = static_cast<TUint8>(*aDestBuffer ^ KMaxTUint8);
		++aDestBuffer;
		}
	}

/**
The method tiles the scan line if its length in pixels is less than
aLengthInPixels argument.
@internalComponent
@param aScanLine A pointer to the scan line buffer.
@param aLengthInPixels The scan line buffer should have that count of pixels after the method call.
@param aSrcPt Position of the first pixel in aMaskBitmap that should be used as a source
              for the pixels in scan line buffer.
@param aMaskBitmap Any additional pixels for the scan line buffer will be taken from here.
@param aScanLinePos This argument is used for some internal optimizations. It should not be
                    modified by the caller.
@param aMaskBase The base address of aMaskBitmap data.
@param aDisplayMode Any additional pixels should be taken from aMaskBitmap using aDisplayMode
                    as an argument for GetScanLine() call.
*/
LOCAL_C void TileScanLine(TPtr8& aScanLine,
						  TInt aLengthInPixels,
						  const TPoint& aSrcPt,
						  const CBitwiseBitmap* aMaskBitmap,
						  TLineScanningPosition& aScanLinePos,
						  TUint32* aMaskBase,
						  TDisplayMode aDisplayMode
						  )
	{
	TInt lengthInBytes = CFbsBitmap::ScanLineLength(aLengthInPixels, aDisplayMode);
	BG_ASSERT_DEBUG(lengthInBytes <= aScanLine.MaxLength(), EBitgdiPanicInvalidArg);
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
The method draws a masked rectangular section of the source
bitmap and does a compress/stretch to fit a given destination
rectangle.
@internalComponent
*/
void CFbsBitGc::DoDrawBitmapMasked(const TRect& aDestRect,
							   CBitwiseBitmap* aSourceBitmap,
							   TUint32* aSourceBase,
							   const TRect& aSourceRect,
							   CBitwiseBitmap* aMaskBitmap,
							   TUint32* aMaskBase,
							   TBool aInvertMask, const TPoint& aDitherOrigin)
	{
	CFbsDrawDevice* drawDevice = iDevice->iDrawDevice;
#ifdef _DEBUG
	TRect deviceDestRect;
	drawDevice->GetDrawRect(deviceDestRect);
	BG_ASSERT_DEBUG(iClipRect.iTl.iX >= deviceDestRect.iTl.iX, EBitgdiPanicOutOfBounds);
	BG_ASSERT_DEBUG(iClipRect.iTl.iY >= deviceDestRect.iTl.iY, EBitgdiPanicOutOfBounds);
	BG_ASSERT_DEBUG(iClipRect.iBr.iX <= deviceDestRect.iBr.iX, EBitgdiPanicOutOfBounds);
	BG_ASSERT_DEBUG(iClipRect.iBr.iY <= deviceDestRect.iBr.iY, EBitgdiPanicOutOfBounds);
#endif

	// The clipped version of the destination rectangle
	TRect clippedDestRect(aDestRect);
	clippedDestRect.Intersection(iClipRect);

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
						   aInvertMask,
						   aDitherOrigin,
						   CFbsDrawDevice::ENoShadow);
			}
		return;
		}
	MFastBlend* fastBlend=NULL;
	if (FastBlendInterface(aSourceBitmap,aMaskBitmap,fastBlend)==KErrNone)
		{
		if (fastBlend->FastBlendBitmapMaskedScaled(iClipRect, aDestRect, aSourceRect, aSourceBase, aSourceBitmap->DataStride(),
				aSourceBitmap->DisplayMode(),aSourceBitmap->SizeInPixels(), 
				aMaskBase, aMaskBitmap->DataStride(), aMaskBitmap->DisplayMode(), aMaskBitmap->SizeInPixels(), aInvertMask, 
				iDrawMode, iShadowMode)==KErrNone)
			{
			return;
			}
		}

	TUint32* scanLineBuffer = drawDevice->ScanLineBuffer();
	const TInt scanLineBytes = drawDevice->ScanLineBytes();
	TPtr8 scanLineDes(REINTERPRET_CAST(TUint8*,scanLineBuffer),scanLineBytes,scanLineBytes);

	// This constant is associated to the value used in TestGdi::DoDrawBitmapMaskedL, case #24.
	// If this value is changed, then that one must be updated as well otherwise the test will no longer be valid.	
	const TInt KScanLineLength = 256;
	const TInt KRgbSize = 4;
	TUint8 maskBuffer[KScanLineLength];

	TUint8 sourceBuffer[KScanLineLength*KRgbSize];
	TPtr8 sourceDes(sourceBuffer,KScanLineLength*KRgbSize,KScanLineLength*KRgbSize);

	const TDisplayMode dispMode = ScanLineBufferDisplayMode(drawDevice);
	TDrawMode drawMode = aInvertMask ? EDrawModeAND : EDrawModeANDNOT;
	// If the source bitmap and the mask bitmap are same, draw the source bitmap either
	// with EDrawModeAND or EDrawModeOR based on aInvertMask parameter.
	if (aSourceBitmap == aMaskBitmap)
		{
		drawMode = aInvertMask ? EDrawModeAND : EDrawModeOR;
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

	TPoint ditherOrigin(aDitherOrigin + clippedDestRect.iTl);
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

	while (yCoord.iY < clippedDestRect.iBr.iY)
		{
		// Draw only the source bitmap, if the source bitmap and the mask bitmap are same.
		// else draw both the bitmaps
		if (aSourceBitmap == aMaskBitmap)
			{
			aSourceBitmap->StretchScanLine(scanLineDes,TPoint(bitmapXStart,yCoord.iX),
								 clipStrch,clipWidth,destWidth,aSourceRect.iTl.iX,
								 srceWidth,ditherOrigin,dispMode,aSourceBase,lineScanPos);
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
								aSourceRect.iTl.iX,srceWidth,ditherOrigin,EColor16MU,aSourceBase,lineScanPos);
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
				drawDevice->WriteRgbAlphaLine(startClip,yCoord.iY,clipWidthPart,sourceBuffer,maskBuffer,iDrawMode);
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
									 srceWidth,ditherOrigin,dispMode,aSourceBase,lineScanPos2);
			drawDevice->WriteLine(clippedDestRect.iTl.iX,yCoord.iY,clipWidth,scanLineBuffer,EDrawModeXOR);

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
										srceWidth,ditherOrigin,dispMode,aMaskBase,lineScanPosMask);
				drawDevice->WriteLine(clippedDestRect.iTl.iX,yCoord.iY,clipWidth,scanLineBuffer,drawMode);
				// Redo stretching of the aSourceBitmap scanline
				aSourceBitmap->StretchScanLine(scanLineDes,TPoint(bitmapXStart,yCoord.iX),
									 	clipStrch,clipWidth,destWidth,aSourceRect.iTl.iX,
									 	srceWidth,ditherOrigin,dispMode,aSourceBase,lineScanPos2);
				}

			if (yCoord.iY==clippedDestRect.iTl.iY)
				{
				aSourceBitmap->SetCompressionBookmark(lineScanPos2,aSourceBase,NULL);
				aMaskBitmap->SetCompressionBookmark(lineScanPosMask,aMaskBase,NULL);
				}

			drawDevice->WriteLine(clippedDestRect.iTl.iX,yCoord.iY,clipWidth,scanLineBuffer,EDrawModeXOR);
			}
		yLine.NextStep(yCoord);
		ditherOrigin.iY++;
		}
	}

EXPORT_C TInt CFbsBitGc::AlphaBlendBitmaps(const TPoint& aDestPt,
											const CWsBitmap* aSrcBmp,
											const TRect& aSrcRect,
											const CWsBitmap* aAlphaBmp,
											const TPoint& aAlphaPt)
	{
	return AlphaBlendBitmaps(aDestPt, REINTERPRET_CAST(const CFbsBitmap*, aSrcBmp), aSrcRect, REINTERPRET_CAST(const CFbsBitmap*, aAlphaBmp), aAlphaPt);
	}

TInt CFbsBitGc::FastBlendInterface(const CBitwiseBitmap* aSource, const CBitwiseBitmap* aMask, MFastBlend*& aFastBlend) const
	{
#if defined(__ALLOW_FAST_BLEND_DISABLE__)
	if (iFastBlendDisabled)
		return(KErrNotSupported);
#endif
	if ((aSource && aSource->IsCompressed()) || (aMask && aMask->IsCompressed()))
		return(KErrNotSupported);
	TAny* interface=NULL;
	TInt ret=iDevice->iDrawDevice->GetInterface(KFastBlendInterfaceID, interface);
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
TDisplayMode CFbsBitGc::ScanLineBufferDisplayMode(CFbsDrawDevice* aDrawDevice)
	{
	return iDrawMode == EDrawModeWriteAlpha ? aDrawDevice->DisplayMode() : aDrawDevice->ScanLineDisplayMode();
	}

/** Prepares the rasterizer to retrieve scan lines from a bitmap if it is an
extended bitmap. No region of interest is passed to the rasterizer.
This function calls CFbsRasterizer::BeginBitmap() with the appropriate
parameters. When finished retrieving scan lines from the extended bitmap,
CFbsRasterizer::EndBitmap() must be called.
@param aBitmap The bitmap to retrieve scan lines from.
@return A pointer to the rasterizer owned by this thread's FBServ session.
@return NULL if the rasterizer is not present or aBitmap is not an extended bitmap.
@see CFbsRasterizer
@internalComponent
*/
CFbsRasterizer* CFbsBitGc::PrepareRasterizerForExtendedBitmap(const CFbsBitmap& aBitmap)
	{
	CFbsRasterizer* rasterizer = NULL;
	CFbsRasterizer::TBitmapDesc desc;
	desc.iDataType = aBitmap.ExtendedBitmapType();
	if (desc.iDataType != KNullUid)
		{
		rasterizer = CFbsBitmap::Rasterizer();
		if (rasterizer)
			{
			desc.iSizeInPixels = aBitmap.SizeInPixels();
			desc.iDispMode = aBitmap.DisplayMode();
			desc.iData = aBitmap.DataAddress();
			desc.iDataSize = aBitmap.DataSize();
			rasterizer->BeginBitmap(aBitmap.SerialNumber(), desc, NULL);
			}
		}
	return rasterizer;
	}

/** Prepares the rasterizer to retrieve scan lines from a bitmap if it is an
extended bitmap. The region of interest passed to the rasterizer is the
intersection of the clipping region and the specified rectangle.
This function calls CFbsRasterizer::BeginBitmap() with the appropriate
parameters. When finished retrieving scan lines from the extended bitmap,
CFbsRasterizer::EndBitmap() must be called.
@param aBitmap The bitmap to retrieve scan lines from.
@param aDestRect A rectangle in coordinates relative to the graphics context that
                 bounds the area that aBitmap is to be blitted onto.
@param aOffset An offset that converts aDestRect into coordinates relative to
               the source bitmap.
@return A pointer to the rasterizer owned by this thread's FBServ session.
@return NULL if the rasterizer is not present or aBitmap is not an extended bitmap.
@see CFbsRasterizer
@internalComponent
*/
CFbsRasterizer* CFbsBitGc::PrepareRasterizerForExtendedBitmap(const CFbsBitmap& aBitmap, const TRect& aDestRect, const TPoint& aOffset)
	{
	CFbsRasterizer* rasterizer = NULL;
	CFbsRasterizer::TBitmapDesc desc;
	desc.iDataType = aBitmap.ExtendedBitmapType();
	if (desc.iDataType != KNullUid)
		{
		rasterizer = CFbsBitmap::Rasterizer();
		if (rasterizer)
			{
			desc.iSizeInPixels = aBitmap.SizeInPixels();
			desc.iDispMode = aBitmap.DisplayMode();
			desc.iData = aBitmap.DataAddress();
			desc.iDataSize = aBitmap.DataSize();
			RRegion rgn;
			rgn.Copy(*iDefaultRegionPtr);
			rgn.ClipRect(aDestRect);
			rgn.Offset(aOffset);
			BG_ASSERT_DEBUG(rgn.IsContainedBy(desc.iSizeInPixels), EBitgdiPanicOutOfBounds);
			rasterizer->BeginBitmap(aBitmap.SerialNumber(), desc, &rgn);
			rgn.Close();
			}
		}
	return rasterizer;
	}

/** Prepares the rasterizer to retrieve scan lines from a bitmap if it is an
extended bitmap. The region of interest passed to the rasterizer is the intersection
of the clipping region, the clipping rectangle and the specified rectangle.
This function calls CFbsRasterizer::BeginBitmap() with the appropriate
parameters. When finished retrieving scan lines from the extended bitmap,
CFbsRasterizer::EndBitmap() must be called.
@param aBitmap The bitmap to retrieve scan lines from.
@param aDestRect A rectangle in coordinates relative to the graphics context that
                 bounds the area that aBitmap is to be drawn onto.
@param aSourceRect A rectangle in coordinates relative to the source bitmap that maps
                   to aDestRect after scaling and translation.
@return A pointer to the rasterizer owned by this thread's FBServ session.
@return NULL if the rasterizer is not present or aBitmap is not an extended bitmap.
@see CFbsRasterizer
@internalComponent
*/
CFbsRasterizer* CFbsBitGc::PrepareRasterizerForExtendedBitmap(const CFbsBitmap& aBitmap, const TRect& aDestRect, const TRect& aSourceRect)
	{
	CFbsRasterizer* rasterizer = NULL;
	CFbsRasterizer::TBitmapDesc desc;
	desc.iDataType = aBitmap.ExtendedBitmapType();
	if (desc.iDataType != KNullUid)
		{
		rasterizer = CFbsBitmap::Rasterizer();
		if (rasterizer)
			{
			desc.iSizeInPixels = aBitmap.SizeInPixels();
			desc.iDispMode = aBitmap.DisplayMode();
			desc.iData = aBitmap.DataAddress();
			desc.iDataSize = aBitmap.DataSize();
			RRegion rgn;
			// Calculate the parameters for the linear DDA algorithm used to scale the region
			// of interest before entering the rectangle loop, since they are invariant.
			TLinearDDA xLine0, yLine0;
			xLine0.Construct(TPoint(aSourceRect.iTl.iX, aDestRect.iTl.iX),
			                 TPoint(aSourceRect.iBr.iX, aDestRect.iBr.iX),
			                 TLinearDDA::ELeft);
			yLine0.Construct(TPoint(aSourceRect.iTl.iY, aDestRect.iTl.iY),
			                 TPoint(aSourceRect.iBr.iY, aDestRect.iBr.iY),
			                 TLinearDDA::ELeft);
			TInt n = iDefaultRegionPtr->Count();
			for (TInt i = 0; i < n; ++i)
				{
				TRect rect = (*iDefaultRegionPtr)[i];
				if (!rect.Intersects(iUserClipRect))
					{
					continue;
					}
				rect.Intersection(iUserClipRect);
				if (!rect.Intersects(aDestRect))
					{
					continue;
					}
				rect.Intersection(aDestRect);
				// Scale the region of interest from coordinates relative to the graphics
				// context to coordinates relative to the bitmap, one rectangle at a time.
				TLinearDDA xLine(xLine0), yLine(yLine0);
				TInt ax, ay, bx, by;
				xLine.JumpToYCoord(ax, rect.iTl.iX);
				yLine.JumpToYCoord(ay, rect.iTl.iY);
				xLine.JumpToYCoord(bx, rect.iBr.iX);
				yLine.JumpToYCoord(by, rect.iBr.iY);
				if (ax < bx && ay < by)
					{
					rgn.AddRect(TRect(ax, ay, bx, by));
					}
				}
			BG_ASSERT_DEBUG(rgn.IsContainedBy(desc.iSizeInPixels), EBitgdiPanicOutOfBounds);
			rasterizer->BeginBitmap(aBitmap.SerialNumber(), desc, &rgn);
			rgn.Close();
			}
		}
	return rasterizer;
	}
