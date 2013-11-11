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
#include <bitdraw.h>
#include <graphics/fbsrasterizer.h>
#include "bitgcextradata.h"


/** Clears a rectangular area.

The cleared area is filled with the current brush colour.The function
provides a concrete implementation of the pure virtual function
CBitmapContext::Clear(const TRect& aRect). The function
behaviour is the same as documented in that class. */	
EXPORT_C void CFbsBitGc::Clear(const TRect& aRect)
    {
	if (CheckDevice(aRect))
		return;

	TRect rcpy(aRect);
	rcpy.Move(iOrigin);
	if (UserClipRect(rcpy))
		return;

	TBrushStyle tempbrushstyle = iBrushStyle;
	iBrushStyle = ESolidBrush;

	SetupDevice();
	iDevice->DrawingBegin();
	RectFill(rcpy);
	iDevice->DrawingEnd();

	iBrushStyle = tempbrushstyle;
	}



/** Clears the whole bitmap or a rectangular area of a bitmap.

The cleared area is filled with the current brush colour.

The function provides a concrete implementation of the pure virtual function 
CBitmapContext::Clear(). The function behaviour is the same as documented 
in that class. 

@see CBitmapContext::Clear() */
EXPORT_C void CFbsBitGc::Clear()
	{
	TRect deviceRect;
	iDevice->iDrawDevice->GetDrawRect(deviceRect);
	if ((iOrigin.iX!=0) || (iOrigin.iY!=0))
		{
		deviceRect.Move(-iOrigin);
		}
	Clear(deviceRect);
	}

/** Draws a single point.

The point is drawn with the current pen settings using the current
drawing mode.The function provides a concrete implementation of the
pure virtual function CGraphicsContext::Plot(). The
function behaviour is the same as documented in that class. */
EXPORT_C void CFbsBitGc::Plot(const TPoint& aPoint)
	{
	if (iPenStyle == ENullPen || (iPenSize.iWidth == 0 && iPenSize.iHeight == 0))
		return;

	CheckDevice();

	TRect plotRect(aPoint + iOrigin,TSize(1,1));
	plotRect.Grow((iPenSize.iWidth >> 1) + 1,(iPenSize.iHeight >> 1) + 1);
	if (!plotRect.Intersects(iUserClipRect))
		return;

	SetupDevice();
	iDevice->DrawingBegin();
	DoPlot(aPoint);
	iDevice->DrawingEnd();
	}

void CFbsBitGc::DoPlot(const TPoint& aPoint)
	{
	CFbsDrawDevice* drawDevice = iDevice->iDrawDevice;

	const TSize oneByOne(1,1);
	const TPoint point(aPoint + iOrigin);

	TRect temp(point,oneByOne);
	if (iPenSize.iWidth > 1 || iPenSize.iHeight > 1)
		temp.Grow(iPenSize.iWidth >> 1,iPenSize.iHeight >> 1);
	AddRect(temp);

	const CGraphicsContext::TPenStyle penStyle = iPenStyle;
	iPenStyle = CGraphicsContext::ESolidPen;

#if defined(_DEBUG)
	TRect deviceRect;
	drawDevice->GetDrawRect(deviceRect);
#endif

	const TInt limit = iDefaultRegionPtr->Count();
	for (TInt count = 0; count < limit; count++)
		{
		iClipRect = (*iDefaultRegionPtr)[count];
		if (!iClipRect.Intersects(temp))
			continue;

		iClipRect.Intersection(temp);
		if (UserClipRect(iClipRect))
			continue;

		if (iPenSize == oneByOne)
			{
			if (iClipRect.Contains(point))
				{
				BG_ASSERT_DEBUG(point.iX >= deviceRect.iTl.iX, EBitgdiPanicOutOfBounds);
				BG_ASSERT_DEBUG(point.iY >= deviceRect.iTl.iY, EBitgdiPanicOutOfBounds);
				BG_ASSERT_DEBUG(point.iX <= deviceRect.iBr.iX, EBitgdiPanicOutOfBounds);
				BG_ASSERT_DEBUG(point.iY <= deviceRect.iBr.iY, EBitgdiPanicOutOfBounds);

				drawDevice->WriteRgb(point.iX,point.iY,iPenColor,iDrawMode);
				}
			}
		else if (iPenSize.iWidth > 0 && iPenSize.iHeight > 0)
			PenDrawClipped(point);

		drawDevice->UpdateRegion(iClipRect);
		}

	iPenStyle = penStyle;
	}


/** Sets the shadow area.

@param aRegion The region defining the shadow area. */
EXPORT_C void CFbsBitGc::ShadowArea(const TRegion* aRegion)
	{
	ShadowFadeArea(aRegion,CFbsDrawDevice::EShadow);
	}

 
/** Sets the fade area.

@param aRegion The region defining the fade area. */
EXPORT_C void CFbsBitGc::FadeArea(const TRegion* aRegion)
	{
	ShadowFadeArea(aRegion,CFbsDrawDevice::EFade);
	}

void CFbsBitGc::ShadowFadeArea(const TRegion* aRegion,TInt8 aShadowMode)
	{
	if (!aRegion || aRegion->CheckError())
		return;

	CFbsDrawDevice* drawDevice = iDevice->iDrawDevice;

	CheckDevice();

	TRect deviceRect;
	drawDevice->GetDrawRect(deviceRect);

	TInt8 shadowMode = iShadowMode;
	iShadowMode = aShadowMode;

	CGraphicsAccelerator* ga = GraphicsAccelerator();

	SetupDevice();
	iDevice->DrawingBegin();

	const TInt limit = aRegion->Count();
	TInt count;
    //use Graphics accelerator if available
	if(ga)
		{
		if(iShadowMode & CFbsDrawDevice::EFade)
			{
	        TInt gaOperationResult = KErrUnknown;
            iDevice->DrawingEnd();

			TGopFadeParams gopFadeParams;
			gopFadeParams.iScale = iFadeWhiteMap-iFadeBlackMap+1;
			gopFadeParams.iOffset = iFadeBlackMap;

			for (count = 0; count < limit; count++)
				{
				iClipRect = (*aRegion)[count];
				iClipRect.Move(iOrigin);
				if(!iClipRect.Intersects(deviceRect))
					continue;

				iClipRect.Intersection(deviceRect);
				AddRect(iClipRect);

				gaOperationResult = ga->Operation(TGopFadeRect(iClipRect,gopFadeParams));
				if(gaOperationResult != KErrNone)
					break;
				}
			if(gaOperationResult == KErrNone)
				goto finish;
			iDevice->DrawingBegin();
			}
		}

    //use graphics contex
	for (count = 0; count < limit; count++)
		{
		iClipRect = (*aRegion)[count];
		iClipRect.Move(iOrigin);
		if(!iClipRect.Intersects(deviceRect))
			continue;

		iClipRect.Intersection(deviceRect);
		AddRect(iClipRect);

		drawDevice->ShadowArea(iClipRect);
		drawDevice->UpdateRegion(iClipRect);
		}

	iDevice->DrawingEnd();

finish:
	iShadowMode = shadowMode;
	}

// if iBrushBitmap is an extended bitmap, PrepareRasterizerForExtendedBitmap() must have been called before this method
void CFbsBitGc::ClipFillLine(TPoint aLeft,TPoint aRight)
	{
	if (iBrushStyle == ENullBrush || 
		aLeft.iY < iClipRect.iTl.iY || aLeft.iY >= iClipRect.iBr.iY)
		return;

	aLeft.iX = Max(aLeft.iX,iClipRect.iTl.iX);
	aRight.iX = Min(aRight.iX,iClipRect.iBr.iX-1);
	if (aLeft.iX > aRight.iX)
		return;

	BG_ASSERT_DEBUG(iUserClipRect.Contains(aLeft),EBitgdiPanicOutOfBounds);

	TInt xcoord = aLeft.iX;
	TInt length = aRight.iX - aLeft.iX + 1;
	TPoint origin(iOrigin + iBrushOrigin);

	BG_ASSERT_DEBUG(aLeft.iX + length <= iUserClipRect.iBr.iX,EBitgdiPanicOutOfBounds);

	CFbsDrawDevice* drawDevice = iDevice->iDrawDevice;

	switch(iBrushStyle)
		{
	case ESolidBrush:
		drawDevice->WriteRgbMulti(aLeft.iX,aLeft.iY,length,1,iBrushColor,iDrawMode);
		return;
	case EPatternedBrush:
		{
		CBitwiseBitmap* brushBitmap = iBrushBitmap.Address();
		BG_ASSERT_ALWAYS(iBrushUsed,EBitgdiPanicInvalidBitmap);
		BG_ASSERT_ALWAYS(brushBitmap != NULL,EBitgdiPanicInvalidBitmap);

		TRect sourcerect(aLeft,TSize(length,1));
		sourcerect.Move(-origin);
		DoBitBlt(aLeft,brushBitmap,iBrushBitmap.DataAddress(),iBrushBitmap.DataStride(),sourcerect);
		return;
		}
	case EHorizontalHatchBrush:
		drawDevice->WriteRgbMulti(aLeft.iX,aLeft.iY,length,1,iBrushColor,iDrawMode);
		if (Abs((aLeft.iY - origin.iY) % 3) == 2)
			drawDevice->WriteRgbMulti(aLeft.iX,aLeft.iY,length,1,iPenColor,iDrawMode);
		return;
	case EVerticalHatchBrush:
		drawDevice->WriteRgbMulti(aLeft.iX,aLeft.iY,length,1,iBrushColor,iDrawMode);
		while (Abs((xcoord - origin.iX) % 3) != 2)
			xcoord++;
		for (; xcoord < aLeft.iX + length; xcoord += 3)
			drawDevice->WriteRgb(xcoord,aLeft.iY,iPenColor,iDrawMode);
		return;
	case ESquareCrossHatchBrush:
		drawDevice->WriteRgbMulti(aLeft.iX,aLeft.iY,length,1,iBrushColor,iDrawMode);
		if (Abs((aLeft.iY - origin.iY) % 3) == 2)
			drawDevice->WriteRgbMulti(aLeft.iX,aLeft.iY,length,1,iPenColor,iDrawMode);
		else
			{
			while (Abs((xcoord - origin.iX) % 3) != 2)
				xcoord++;
			for (; xcoord < aLeft.iX + length; xcoord += 3)
				drawDevice->WriteRgb(xcoord,aLeft.iY,iPenColor,iDrawMode);
			}
		return;
	case EForwardDiagonalHatchBrush:
		{
		drawDevice->WriteRgbMulti(aLeft.iX,aLeft.iY,length,1,iBrushColor,iDrawMode);
		TInt diff = (origin.iX + origin.iY - aLeft.iX - aLeft.iY) % 3;
		if (diff < 0)
			diff += 3;
		xcoord += diff;
		for (; xcoord < aLeft.iX + length; xcoord += 3)
			drawDevice->WriteRgb(xcoord,aLeft.iY,iPenColor,iDrawMode);
		}
		return;
	case ERearwardDiagonalHatchBrush:
		{
		drawDevice->WriteRgbMulti(aLeft.iX,aLeft.iY,length,1,iBrushColor,iDrawMode);
		TInt diff = (origin.iX - origin.iY - aLeft.iX + aLeft.iY) % 3;
		if (diff < 0)
			diff += 3;
		xcoord += diff;
		for (; xcoord < aLeft.iX + length; xcoord += 3)
			drawDevice->WriteRgb(xcoord,aLeft.iY,iPenColor,iDrawMode);
		}
		return;
	case EDiamondCrossHatchBrush:
		{
		drawDevice->WriteRgbMulti(aLeft.iX,aLeft.iY,length,1,iBrushColor,iDrawMode);
		TInt sum = aLeft.iX + aLeft.iY - origin.iX - origin.iY;
		for (; xcoord < aLeft.iX + length; xcoord++,sum++)
			if ((sum & 1) == 0 && ((sum & 3) != 0 || ((xcoord-origin.iX) & 1) == 1))
				drawDevice->WriteRgb(xcoord,aLeft.iY,iPenColor,iDrawMode);
		}
		return;
	default:
		return;
		}
	}

void CFbsBitGc::PenAllocate()
	{
	iFbsBitGcExtraData->ResetPenArray();
	if (iPenSize.iWidth == 1 && iPenSize.iHeight == 1)
		return;

	const TInt doublepenheight = iPenSize.iHeight << 1;

	TInt* penArray = new TInt[doublepenheight];
	if (!penArray)
		return;

	iFbsBitGcExtraData->SetPenArray(penArray);

	if (iPenSize.iWidth == 1 || iPenSize.iWidth == 2 || iPenSize.iHeight == 1 || iPenSize.iHeight == 2)
		{
		TInt* bitGcPenArray = iFbsBitGcExtraData->PenArray();
		for (TInt count = 0; count < iPenSize.iHeight; count += 2)
			{
			bitGcPenArray[doublepenheight - count - 2] = 0;
			bitGcPenArray[doublepenheight - count - 1] = iPenSize.iWidth - 1;
			bitGcPenArray[count] = 0;
			bitGcPenArray[count + 1] = iPenSize.iWidth - 1;
			}
		}
	else
		{
		TPoint tl,tr,bl,br;
		TEllipse ellipse;
		ellipse.Construct(TRect(iPenSize));
		TInt* bitGcPenArray = iFbsBitGcExtraData->PenArray();
		for (TInt count = 0; count < iPenSize.iHeight; count += 2)
			{
			//coverity[check_return]
			//coverity[unchecked_value]
			ellipse.NextStep(tl,tr,bl,br);
			bitGcPenArray[doublepenheight - count - 2] = bl.iX;
			bitGcPenArray[doublepenheight - count - 1] = br.iX;
			bitGcPenArray[count] = tl.iX;
			bitGcPenArray[count + 1] = tr.iX;
			}
		}
	}

void CFbsBitGc::PenDrawClipped(TPoint aPoint)
	{
	BG_ASSERT_DEBUG(iPenSize.iWidth > 0,EBitgdiPanicZeroLength);
	BG_ASSERT_DEBUG(iPenSize.iHeight > 0,EBitgdiPanicZeroLength);

	aPoint.iX -= ((iPenSize.iWidth - 1) >> 1);
	aPoint.iY -= ((iPenSize.iHeight - 1) >> 1);

	BG_ASSERT_DEBUG(iClipRect.iTl.iX >= iUserClipRect.iTl.iX,EBitgdiPanicOutOfBounds);
	BG_ASSERT_DEBUG(iClipRect.iTl.iY >= iUserClipRect.iTl.iY,EBitgdiPanicOutOfBounds);
	BG_ASSERT_DEBUG(iClipRect.iBr.iX <= iUserClipRect.iBr.iX,EBitgdiPanicOutOfBounds);
	BG_ASSERT_DEBUG(iClipRect.iBr.iY <= iUserClipRect.iBr.iY,EBitgdiPanicOutOfBounds);

	CFbsDrawDevice* drawDevice = iDevice->iDrawDevice;

	if (iPenSize.iWidth == 1 && iPenSize.iHeight == 1)
		{
		if (iPenStyle == CGraphicsContext::ESolidPen || (iDotMask & (1 << (iDotParam % iDotLength))))
			if (iClipRect.Contains(aPoint))
				drawDevice->WriteRgb(aPoint.iX,aPoint.iY,iPenColor,iDrawMode);
		}
	else if (iFbsBitGcExtraData->PenArray())
		{
		TInt ycoord = aPoint.iY;
		const TInt maxdim = Max(iPenSize.iWidth,iPenSize.iHeight);
		const TInt doublepenheight = iPenSize.iHeight << 1;

		if (iPenStyle == CGraphicsContext::ESolidPen || (iDotMask & (1 << ((iDotParam / maxdim) % iDotLength))))
			{
			for (TInt ix = 0; ix < doublepenheight; ycoord++,ix += 2)
				{
				if (ycoord >= iClipRect.iTl.iY && ycoord < iClipRect.iBr.iY)
					{
					TInt left = aPoint.iX + iFbsBitGcExtraData->PenArray()[ix];
					TInt right = aPoint.iX + iFbsBitGcExtraData->PenArray()[ix+1];
					if (left < iClipRect.iTl.iX)
						left = iClipRect.iTl.iX;
					if (right >= iClipRect.iBr.iX)
						right = iClipRect.iBr.iX - 1;
					if (left <= right)
						drawDevice->WriteRgbMulti(left,ycoord,right - left + 1,1,iPenColor,CGraphicsContext::EDrawModePEN);
					}
				}
			}
		}
	else
		{
		TPoint tl,tr,bl,br;
		TEllipse ellipse;
		ellipse.Construct(TRect(aPoint,iPenSize));
		while (!ellipse.NextStep(tl,tr,bl,br))
			{
			if (tl.iY >= iClipRect.iTl.iY && tl.iY < iClipRect.iBr.iY)
				{
				if (tl.iX < iClipRect.iTl.iX)
					tl.iX = iClipRect.iTl.iX;
				if (tr.iX >= iClipRect.iBr.iX)
					tr.iX = iClipRect.iBr.iX-1;
				if (tl.iX <= tr.iX)
					drawDevice->WriteRgbMulti(tl.iX,tl.iY,tr.iX - tl.iX + 1,1,iPenColor,CGraphicsContext::EDrawModePEN);
				}
			if (bl.iY >= iClipRect.iTl.iY && bl.iY < iClipRect.iBr.iY)
				{
				if (bl.iX < iClipRect.iTl.iX)
					bl.iX = iClipRect.iTl.iX;
				if (br.iX >= iClipRect.iBr.iX)
					br.iX = iClipRect.iBr.iX - 1;
				if (bl.iX <= br.iX)
					drawDevice->WriteRgbMulti(bl.iX,bl.iY,br.iX - bl.iX + 1,1,iPenColor,CGraphicsContext::EDrawModePEN);
				}
			}

		if (tl.iY == bl.iY && tl.iY >= iClipRect.iTl.iY && tl.iY < iClipRect.iBr.iY)
			{
			if (tl.iX < iClipRect.iTl.iX)
				tl.iX = iClipRect.iTl.iX;
			if (tr.iX >= iClipRect.iBr.iX)
				tr.iX = iClipRect.iBr.iX - 1;
			if (tl.iX <= tr.iX)
				drawDevice->WriteRgbMulti(tl.iX,tl.iY,tr.iX - tl.iX + 1,1,iPenColor,CGraphicsContext::EDrawModePEN);
			}
		}
	}

void CFbsBitGc::PenDrawDeferred(TPoint aPoint,TInt* aArray,TInt aFirstElement)
	{
	BG_ASSERT_DEBUG(iFbsBitGcExtraData->PenArray(),EBitgdiPanicZeroLength);
	BG_ASSERT_DEBUG(iPenSize.iWidth > 0,EBitgdiPanicZeroLength);
	BG_ASSERT_DEBUG(iPenSize.iHeight > 0,EBitgdiPanicZeroLength);
	BG_ASSERT_DEBUG(iClipRect.iTl.iX >= iUserClipRect.iTl.iX,EBitgdiPanicOutOfBounds);
	BG_ASSERT_DEBUG(iClipRect.iTl.iY >= iUserClipRect.iTl.iY,EBitgdiPanicOutOfBounds);
	BG_ASSERT_DEBUG(iClipRect.iBr.iX <= iUserClipRect.iBr.iX,EBitgdiPanicOutOfBounds);
	BG_ASSERT_DEBUG(iClipRect.iBr.iY <= iUserClipRect.iBr.iY,EBitgdiPanicOutOfBounds);

	aPoint.iX -= ((iPenSize.iWidth - 1) >> 1);
	const TInt doublepenheight = iPenSize.iHeight << 1;

	for (TInt ix = 0; ix < doublepenheight; ix++,aFirstElement++)
		{
		if (aFirstElement == doublepenheight)
			aFirstElement = 0;
		TInt newval = aPoint.iX + iFbsBitGcExtraData->PenArray()[ix];
		if (newval < aArray[aFirstElement])
			aArray[aFirstElement] = newval;

		ix++;
		aFirstElement++;
		newval = aPoint.iX + iFbsBitGcExtraData->PenArray()[ix];
		if (newval > aArray[aFirstElement])
			aArray[aFirstElement] = newval;
		}
	}

//Default implementation of reserved virtual
EXPORT_C void CFbsBitGc::Reserved_CGraphicsContext_2()
	{
	CBitmapContext::Reserved_CGraphicsContext_2();
	}

//Default implementation of reserved virtual
EXPORT_C void CFbsBitGc::Reserved_CBitmapContext_1()
	{
	CBitmapContext::Reserved_CBitmapContext_1();
	}

//Default implementation of reserved virtual
EXPORT_C void CFbsBitGc::Reserved_CBitmapContext_2()
	{
	CBitmapContext::Reserved_CBitmapContext_2();
	}

//Default implementation of reserved virtual
EXPORT_C void CFbsBitGc::Reserved_CBitmapContext_3()
	{
	CBitmapContext::Reserved_CBitmapContext_3();
	}

//Default implementation of reserved virtual
EXPORT_C void CFbsBitGc::Reserved_CFbsBitGc_1()
	{
	}

//Default implementation of reserved virtual
EXPORT_C void CFbsBitGc::Reserved_CFbsBitGc_2()
	{
	}

//Default implementation of reserved virtual
EXPORT_C void CFbsBitGc::Reserved_CFbsBitGc_3()
	{
	}

//Default implementation of reserved virtual
EXPORT_C void CFbsBitGc::Reserved_CFbsBitGc_4()
	{
	}

//Default implementation of reserved virtual
EXPORT_C void CFbsBitGc::Reserved_CFbsBitGc_5()
	{
	}
