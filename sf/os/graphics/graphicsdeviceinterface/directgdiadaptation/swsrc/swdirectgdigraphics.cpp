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

#include "directgdiadapter.h"
#include "swdirectgdiengine.h"
#include "swdirectgdiellipse.h"

/** 
@see MDirectGdiEngine::Clear(const TRect&)
*/	
void CSwDirectGdiEngine::Clear(const TRect& aRect)
	{
	TRect rcpy(aRect);
	rcpy.Move(iOrigin);	

	DirectGdi::TBrushStyle tempbrushstyle = iBrushStyle;
	iBrushStyle = DirectGdi::ESolidBrush;

	RectFill(rcpy);	
	
	iBrushStyle = tempbrushstyle;
	}

/**
@see MDirectGdiEngine::Clear()
*/
void CSwDirectGdiEngine::Clear()
	{
	TRect deviceRect;
	iDrawDevice->GetDrawRect(deviceRect);
	if ((iOrigin.iX!=0) || (iOrigin.iY!=0))
		{
		deviceRect.Move(-iOrigin);
		}
	Clear(deviceRect);
	}

/**
@see MDirectGdiEngine::Plot()
*/
void CSwDirectGdiEngine::Plot(const TPoint& aPoint)
	{
	TRect targetRect(aPoint + iOrigin, TSize(1,1));
	targetRect.Grow((iPenSize.iWidth >> 1) + 1,(iPenSize.iHeight >> 1) + 1);
	DoPlot(aPoint);	
	}

/**
Draws a single point.

@see Plot()

@param aPoint The location to plot at.
@panic DGDIAdapter 1013, if the point being plotted is out of bounds of the target (debug only).
 */
void CSwDirectGdiEngine::DoPlot(const TPoint& aPoint)
	{
	const TSize oneByOne(1,1);
	const TPoint point(aPoint + iOrigin);

	TRect temp(point,oneByOne);
	if (iPenSize.iWidth > 1 || iPenSize.iHeight > 1)
		{
		temp.Grow(iPenSize.iWidth >> 1, iPenSize.iHeight >> 1);
		}
	
	const DirectGdi::TPenStyle oldPenStyle = iPenStyle;	
	iPenStyle = DirectGdi::ESolidPen;

#if defined(_DEBUG)
	TRect deviceRect;
	iDrawDevice->GetDrawRect(deviceRect);
#endif

	TRect clipRect;
	const TInt limit = iDefaultRegionPtr->Count();
	for (TInt count = 0; count < limit; count++)
		{
		clipRect = (*iDefaultRegionPtr)[count];
		if (!clipRect.Intersects(temp))
			{
			continue;
			}

		clipRect.Intersection(temp);	
		if (iPenSize == oneByOne)
			{
			if (clipRect.Contains(point))
				{
				GRAPHICS_ASSERT_DEBUG(point.iX >= deviceRect.iTl.iX, EDirectGdiPanicOutOfBounds);
				GRAPHICS_ASSERT_DEBUG(point.iY >= deviceRect.iTl.iY, EDirectGdiPanicOutOfBounds);
				GRAPHICS_ASSERT_DEBUG(point.iX <= deviceRect.iBr.iX, EDirectGdiPanicOutOfBounds);
				GRAPHICS_ASSERT_DEBUG(point.iY <= deviceRect.iBr.iY, EDirectGdiPanicOutOfBounds);
				
				iDrawDevice->WriteRgb(point.iX,point.iY,iPenColor, GcDrawMode(iDrawMode));
				}
			}
		else
			{
			PenDrawClipped(point, clipRect);
			}

		iDrawDevice->UpdateRegion(clipRect);
		}

	iPenStyle = oldPenStyle;
	}

/**
Clips the horizontal drawing of a line between two points.
The vertical location must be inside the clip rect.

@param aLeft The left hand side coordinate.
@param aRight The right hand side coordinate.
@param aClipRect The rectangle to which the line is clipped.
 */
void CSwDirectGdiEngine::ClipFillLine(TPoint aLeft,TPoint aRight, TRect aClipRect)
	{
	if (aLeft.iY < aClipRect.iTl.iY || aLeft.iY >= aClipRect.iBr.iY)
		return;

	aLeft.iX = Max(aLeft.iX,aClipRect.iTl.iX);
	aRight.iX = Min(aRight.iX,aClipRect.iBr.iX-1);
	if (aLeft.iX > aRight.iX)
		{
		return;
		}

	TInt xcoord = aLeft.iX;
	TInt length = aRight.iX - aLeft.iX + 1;
	TPoint origin(iOrigin + iBrushOrigin);
	const CGraphicsContext::TDrawMode drawMode = GcDrawMode(iDrawMode);

	switch(iBrushStyle)
		{
	case DirectGdi::ESolidBrush:
		iDrawDevice->WriteRgbMulti(aLeft.iX,aLeft.iY,length,1,iBrushColor,drawMode);
		return;
	case DirectGdi::EPatternedBrush:
		{
		iBrushBitmap.BeginDataAccess();
		CBitwiseBitmap* brushBitmap = iBrushBitmap.Address();
		if (brushBitmap)
			{
			TRect sourcerect(aLeft,TSize(length,1));
			sourcerect.Move(-origin);
			DoBitBlt(aLeft,brushBitmap,iBrushBitmap.DataAddress(),iBrushBitmap.DataStride(),sourcerect);
			}
		iBrushBitmap.EndDataAccess(ETrue);
		return;
		}
	case DirectGdi::EHorizontalHatchBrush:
		iDrawDevice->WriteRgbMulti(aLeft.iX,aLeft.iY,length,1,iBrushColor,drawMode);
		if (Abs((aLeft.iY - origin.iY) % 3) == 2)
			iDrawDevice->WriteRgbMulti(aLeft.iX,aLeft.iY,length,1,iPenColor,drawMode);
		return;
	case DirectGdi::EVerticalHatchBrush:
		iDrawDevice->WriteRgbMulti(aLeft.iX,aLeft.iY,length,1,iBrushColor,drawMode);
		while (Abs((xcoord - origin.iX) % 3) != 2)
			xcoord++;
		for (; xcoord < aLeft.iX + length; xcoord += 3)
			iDrawDevice->WriteRgb(xcoord,aLeft.iY,iPenColor,drawMode);
		return;
	case DirectGdi::ESquareCrossHatchBrush:
		iDrawDevice->WriteRgbMulti(aLeft.iX,aLeft.iY,length,1,iBrushColor,drawMode);
		if (Abs((aLeft.iY - origin.iY) % 3) == 2)
			iDrawDevice->WriteRgbMulti(aLeft.iX,aLeft.iY,length,1,iPenColor,drawMode);
		else
			{
			while (Abs((xcoord - origin.iX) % 3) != 2)
				xcoord++;
			for (; xcoord < aLeft.iX + length; xcoord += 3)
				iDrawDevice->WriteRgb(xcoord,aLeft.iY,iPenColor,drawMode);
			}
		return;
	case DirectGdi::EForwardDiagonalHatchBrush:
		{
		iDrawDevice->WriteRgbMulti(aLeft.iX,aLeft.iY,length,1,iBrushColor,drawMode);
		TInt diff = (origin.iX + origin.iY - aLeft.iX - aLeft.iY) % 3;
		if (diff < 0)
			diff += 3;
		xcoord += diff;
		for (; xcoord < aLeft.iX + length; xcoord += 3)
			iDrawDevice->WriteRgb(xcoord,aLeft.iY,iPenColor,drawMode);
		}
		return;
	case DirectGdi::ERearwardDiagonalHatchBrush:
		{
		iDrawDevice->WriteRgbMulti(aLeft.iX,aLeft.iY,length,1,iBrushColor,drawMode);
		TInt diff = (origin.iX - origin.iY - aLeft.iX + aLeft.iY) % 3;
		if (diff < 0)
			diff += 3;
		xcoord += diff;
		for (; xcoord < aLeft.iX + length; xcoord += 3)
			iDrawDevice->WriteRgb(xcoord,aLeft.iY,iPenColor,drawMode);
		}
		return;
	case DirectGdi::EDiamondCrossHatchBrush:
		{
		iDrawDevice->WriteRgbMulti(aLeft.iX,aLeft.iY,length,1,iBrushColor,drawMode);
		TInt sum = aLeft.iX + aLeft.iY - origin.iX - origin.iY;
		for (; xcoord < aLeft.iX + length; xcoord++,sum++)
			if ((sum & 1) == 0 && ((sum & 3) != 0 || ((xcoord-origin.iX) & 1) == 1))
				iDrawDevice->WriteRgb(xcoord,aLeft.iY,iPenColor,drawMode);
		}
		return;
	default:
		return;
		}
	}

/**
Creates the pen using the current pen size.

@return KErrNoMermory if memory could not be allocated, KErrNone otherwise.
*/ 
TInt CSwDirectGdiEngine::PenAllocate()
	{		
	ResetPenArray();
	
	if (iPenSize.iWidth == 1 && iPenSize.iHeight == 1)
		{
		return KErrNone;
		}

	const TInt doublePenHeight = iPenSize.iHeight << 1;

	TInt* penArray = new TInt[doublePenHeight];
	if (penArray == NULL)
		{
		return KErrNoMemory;
		}

	SetPenArray(penArray);

	if (iPenSize.iWidth == 1 || iPenSize.iWidth == 2 || iPenSize.iHeight == 1 || iPenSize.iHeight == 2)
		{				
		for (TInt count = 0; count < iPenSize.iHeight; count += 2)
			{
			iPenArray[doublePenHeight - count - 2] = 0;
			iPenArray[doublePenHeight - count - 1] = iPenSize.iWidth - 1;
			iPenArray[count] = 0;
			iPenArray[count + 1] = iPenSize.iWidth - 1;
			}
		}
	else
		{
		TPoint tl,tr,bl,br;
		TSwDirectGdiEllipse ellipse;
		ellipse.Construct(TRect(iPenSize));		
		for (TInt count = 0; count < iPenSize.iHeight; count += 2)
			{
			//coverity[check_return]
			//coverity[unchecked_value]
			ellipse.NextStep(tl,tr,bl,br);
			iPenArray[doublePenHeight - count - 2] = bl.iX;
			iPenArray[doublePenHeight - count - 1] = br.iX;
			iPenArray[count] = tl.iX;
			iPenArray[count + 1] = tr.iX;
			}
		}	
	
	return KErrNone;
	}

/**
Draws at a given point using the current settings, if within clipping rectangle.

@pre The pen size is greater than one.
 
@param aPoint The location to draw at.
@param aClipRect The clipping rectangle.
@panic DGDIAdapter 1016, if the current pen size is zero (debug only).
*/ 
void CSwDirectGdiEngine::PenDrawClipped(TPoint aPoint, const TRect& aClipRect)
	{	
	GRAPHICS_ASSERT_DEBUG(iPenSize.iWidth > 0,EDirectGdiPanicZeroLength);
	GRAPHICS_ASSERT_DEBUG(iPenSize.iHeight > 0,EDirectGdiPanicZeroLength);

	aPoint.iX -= ((iPenSize.iWidth - 1) >> 1);
	aPoint.iY -= ((iPenSize.iHeight - 1) >> 1);

	if (iPenSize.iWidth == 1 && iPenSize.iHeight == 1)
		{
		if ( (iPenStyle == DirectGdi::ESolidPen || (iDotMask & (1 << (iDotParam % iDotLength)))) &&
		      aClipRect.Contains(aPoint) )
			{
			iDrawDevice->WriteRgb(aPoint.iX, aPoint.iY, iPenColor, GcDrawMode(iDrawMode));		
			}				
		}
	else if (iPenArray != NULL)
		{
		TInt yCoord = aPoint.iY;
		const TInt maxdim = Max(iPenSize.iWidth,iPenSize.iHeight);
		const TInt doublePenHeight = iPenSize.iHeight << 1;

		if (iPenStyle == DirectGdi::ESolidPen || (iDotMask & (1 << ((iDotParam / maxdim) % iDotLength))))
			{
			for (TInt ix = 0; ix < doublePenHeight; yCoord++, ix += 2)
				{
				if (yCoord >= aClipRect.iTl.iY && yCoord < aClipRect.iBr.iY)
					{
					TInt left = aPoint.iX + iPenArray[ix];
					TInt right = aPoint.iX + iPenArray[ix+1];
					if (left < aClipRect.iTl.iX)
						{
						left = aClipRect.iTl.iX;
						}
					if (right >= aClipRect.iBr.iX)
						{
						right = aClipRect.iBr.iX - 1;
						}
					if (left <= right)
						{
						iDrawDevice->WriteRgbMulti(left, yCoord, right - left + 1, 1, iPenColor, CGraphicsContext::EDrawModePEN);
						}
					}
				}
			}
		}
	else
		{
		TPoint tl,tr,bl,br;
		TSwDirectGdiEllipse ellipse;
		ellipse.Construct(TRect(aPoint,iPenSize));
		while (!ellipse.NextStep(tl,tr,bl,br))
			{
			if (tl.iY >= aClipRect.iTl.iY && tl.iY < aClipRect.iBr.iY)
				{
				if (tl.iX < aClipRect.iTl.iX)
					{
					tl.iX = aClipRect.iTl.iX;
					}
				if (tr.iX >= aClipRect.iBr.iX)
					{
					tr.iX = aClipRect.iBr.iX-1;
					}
				if (tl.iX <= tr.iX)
					{
					iDrawDevice->WriteRgbMulti(tl.iX,tl.iY,tr.iX - tl.iX + 1,1,iPenColor,CGraphicsContext::EDrawModePEN);
					}
				}
			if (bl.iY >= aClipRect.iTl.iY && bl.iY < aClipRect.iBr.iY)
				{
				if (bl.iX < aClipRect.iTl.iX)
					{
					bl.iX = aClipRect.iTl.iX;
					}
				if (br.iX >= aClipRect.iBr.iX)
					{
					br.iX = aClipRect.iBr.iX - 1;
					}
				if (bl.iX <= br.iX)
					{
					iDrawDevice->WriteRgbMulti(bl.iX,bl.iY,br.iX - bl.iX + 1,1,iPenColor,CGraphicsContext::EDrawModePEN);
					}
				}
			}

		if (tl.iY == bl.iY && tl.iY >= aClipRect.iTl.iY && tl.iY < aClipRect.iBr.iY)
			{
			if (tl.iX < aClipRect.iTl.iX)
				{
				tl.iX = aClipRect.iTl.iX;
				}
			if (tr.iX >= aClipRect.iBr.iX)
				{
				tr.iX = aClipRect.iBr.iX - 1;
				}
			if (tl.iX <= tr.iX)
				{
				iDrawDevice->WriteRgbMulti(tl.iX,tl.iY,tr.iX - tl.iX + 1,1, iPenColor, CGraphicsContext::EDrawModePEN);
				}
			}
		}
	}

/**
@panic DGDIAdapter 1016, if the current pen size is zero (debug only).
*/
void CSwDirectGdiEngine::PenDrawDeferred(TPoint aPoint,TInt* aArray, TInt aFirstElement)
	{
	GRAPHICS_ASSERT_DEBUG(iPenArray,EDirectGdiPanicZeroLength);
	GRAPHICS_ASSERT_DEBUG(iPenSize.iWidth > 0,EDirectGdiPanicZeroLength);
	GRAPHICS_ASSERT_DEBUG(iPenSize.iHeight > 0,EDirectGdiPanicZeroLength);

	aPoint.iX -= ((iPenSize.iWidth - 1) >> 1);
	const TInt doublepenheight = iPenSize.iHeight << 1;

	for (TInt ix = 0; ix < doublepenheight; ix++,aFirstElement++)
		{
		if (aFirstElement == doublepenheight)
			{
			aFirstElement = 0;
			}
		TInt newval = aPoint.iX + iPenArray[ix];
		if (newval < aArray[aFirstElement])
			{
			aArray[aFirstElement] = newval;
			}

		ix++;
		aFirstElement++;
		newval = aPoint.iX + iPenArray[ix];
		if (newval > aArray[aFirstElement])
			{
			aArray[aFirstElement] = newval;
			}
		}
	}
