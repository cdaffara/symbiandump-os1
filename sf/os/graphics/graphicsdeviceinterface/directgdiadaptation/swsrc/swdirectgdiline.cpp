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
#include "swdirectgdipolygon.h"

/**
@see MDirectGdiEngine::DrawLine()
*/
void CSwDirectGdiEngine::DrawLine(const TPoint& aPt1,const TPoint& aPt2)
    {
	DoDrawLine(aPt1,aPt2,ETrue);
	}

/**
@see MDirectGdiEngine::DrawLineTo()
*/
void CSwDirectGdiEngine::DrawLineTo(const TPoint& aPoint)
	{
	DrawLine(iLinePosition,aPoint);
	}


/**
@see MDirectGdiEngine::DrawLineBy()
*/	
void CSwDirectGdiEngine::DrawLineBy(const TPoint& aVector)
    {
	DrawLine(iLinePosition,iLinePosition + aVector);
	}

/**
@see MDirectGdiEngine::DrawPolyLine()

@panic DGDIAdapter 27, if the passed point list has too few points (debug only).
*/
void CSwDirectGdiEngine::DrawPolyLine(const TArray<TPoint>& aPointList)
	{
	DrawPolyLineNoEndPoint(aPointList);
	
	if (iPenStyle == DirectGdi::ESolidPen)
		{
		Plot(aPointList[aPointList.Count()-1]);
		}
	}

/**
@see MDirectGdiEngine::DrawPolyLineNoEndPoint()

@panic DGDIAdapter 27, if the passed point list has too few points (debug only).
*/
void CSwDirectGdiEngine::DrawPolyLineNoEndPoint(const TArray<TPoint>& aPointList)
	{
	GRAPHICS_ASSERT_DEBUG(aPointList.Count() > 0, EDirectGdiPanicInvalidPointArray);

	const TInt vertexes = aPointList.Count()-1;

	for (TInt count = 0; count < vertexes; count++)
		{
		DrawLine(aPointList[count], aPointList[count + 1]);
		}
	}

/**
@see MDirectGdiEngine::DrawPolygon()
*/
void CSwDirectGdiEngine::DrawPolygon(const TArray<TPoint>& aPointList, DirectGdi::TFillRule aFillRule)
	{
	const TInt numpoints = aPointList.Count();

	if (iBrushStyle != DirectGdi::ENullBrush)
		{
		TRect pointrect(0,0,0,0);
		TRect truncrect(0,0,0,0);
		TBool largepolygon = EFalse;

		for (TInt count = 0; count < numpoints; count++)
			{
			pointrect.iTl = aPointList[count] + iOrigin;
			truncrect.iTl = pointrect.iTl;
			TruncateRect(truncrect);

			if (pointrect.iTl != truncrect.iTl)
				{
				largepolygon = ETrue;
				break;
				}
			}

		if (largepolygon)
			{
			PolyFillLarge(&aPointList, aFillRule);
			}
		else
			{
			PolyFill(&aPointList, aFillRule);
			}
		}

	if (iPenStyle != DirectGdi::ENullPen)
		{
		if (iPenSize.iWidth > 0 && iPenSize.iHeight > 0)
			{
			PolyOutline(&aPointList);
			}
		}
	}

/**
Draws a straight line from the start to the end position using current pen size, colour and style.

@param	aPt1 Start position.
@param	aPt2 End position.
@param	aDrawStartPoint	If ETrue, draws the first pixel of the line.

@post   The internal drawing position is set to the line's endpoint.
@see CSwDirectGdiEngine::DrawLine()
*/
void CSwDirectGdiEngine::DoDrawLine(TPoint aPt1, TPoint aPt2, TBool aDrawStartPoint)
	{
	iLinePosition = aPt2;

	if ((aPt1 == aPt2))
		{
		return;
		}

	aPt1 += iOrigin;
	aPt2 += iOrigin;

	TRect temp(aPt1,aPt2);
	temp.Normalize();
	temp.Grow(iPenSize.iWidth, iPenSize.iHeight);

	TRect screenRect;
	iDrawDevice->GetDrawRect(screenRect);
	screenRect.Grow(iPenSize.iWidth, iPenSize.iHeight);

	const TInt dotParam = iDotParam;
	TPoint plotpt(0,0);
	const CGraphicsContext::TDrawMode drawMode = GcDrawMode(iDrawMode);

	TRect clipRect(0,0,0,0);
	for (TInt count = 0; count < iDefaultRegionPtr->Count(); count++)
		{
		iDotParam = dotParam;
		clipRect = (*iDefaultRegionPtr)[count];

		if (!clipRect.Intersects(temp))
			{
			TLinearDDA line;
			line.Construct(aPt1,aPt2);
			line.JumpToRect(screenRect);
			if (iPenStyle != DirectGdi::ESolidPen)
				{
				while (!line.SingleStep(plotpt))
					{
					iDotParam += iDotDirection;
					}
				}
			continue;
			}

		clipRect.Intersection(temp);

		if ((iPenSize.iWidth > 1 || iPenSize.iHeight > 1) && (iPenStyle == DirectGdi::ESolidPen)) // wide solid line
			{
			DoDrawSolidWideLine(aPt1, aPt2, aDrawStartPoint, screenRect, clipRect);
			}
		else if (iPenSize.iWidth > 1 || iPenSize.iHeight > 1) // dotted line
			{
			DoDrawDottedWideLine(aPt1, aPt2, aDrawStartPoint, screenRect, clipRect);
			}
		else if (iPenStyle != DirectGdi::ESolidPen) // single pixel dotted line
			{
			TLinearDDA line;
			line.Construct(aPt1,aPt2);
			line.JumpToRect(screenRect);

			iDotParam = dotParam;
			if (!aDrawStartPoint)
				{
				line.SingleStep(plotpt);
				iDotParam += iDotDirection;
				}

			while (!line.SingleStep(plotpt))
				{
				PenDrawClipped(plotpt, clipRect);
				iDotParam += iDotDirection;
				}
			}
		else if (aPt1.iY == aPt2.iY && 
				(aPt1.iY >= clipRect.iTl.iY && 
				 aPt1.iY < clipRect.iBr.iY))
			{ // single pixel solid horizontal line
			TInt start = Min(aPt1.iX,aPt2.iX + 1);
			TInt length = Abs(aPt2.iX - aPt1.iX);

			if (!aDrawStartPoint)
				{
				if (aPt1.iX < aPt2.iX)
					{
					start++;
					}
				else
					{
					length--;
					}
				}
			if (start < clipRect.iTl.iX)
				{
				length += start - clipRect.iTl.iX;
				start = clipRect.iTl.iX;
				}
			if ( (start + length) > clipRect.iBr.iX)
				{
				length = clipRect.iBr.iX - start;
				}

			if (length > 0)
				{
				iDrawDevice->WriteRgbMulti(start, aPt1.iY, length, 1, iPenColor, drawMode);
				}
			}
		else if (aPt1.iX == aPt2.iX && (aPt1.iX >= clipRect.iTl.iX && aPt1.iX < clipRect.iBr.iX))
			{ // single pixel solid vertical line
			TInt start = Min(aPt1.iY,aPt2.iY + 1);
			TInt length = Abs(aPt2.iY - aPt1.iY);

			if (!aDrawStartPoint)
				{
				if (aPt1.iY < aPt2.iY)
					{
					start++;
					}
				else
					{
					length--;
					}
				}

			if (start < clipRect.iTl.iY)
				{
				length += start - clipRect.iTl.iY;
				start = clipRect.iTl.iY;
				}
			if (start + length > clipRect.iBr.iY)
				{
				length = clipRect.iBr.iY - start;
				}

			if (length > 0)
				{			
				iDrawDevice->WriteRgbMulti(aPt1.iX,start,1,length,iPenColor, drawMode);
				}
			}
		else
			{ // single pixel solid diagonal line
			TLinearDDA line;
			line.Construct(aPt1,aPt2);

			line.JumpToRect(screenRect);

			if (!aDrawStartPoint)
				{
				line.SingleStep(plotpt);
				}

			while (!line.SingleStep(plotpt))
				{
				if (clipRect.Contains(plotpt))
					{
					iDrawDevice->WriteRgb(plotpt.iX, plotpt.iY, iPenColor, drawMode);
					}
				}
			}

		iDrawDevice->UpdateRegion(clipRect);
		}
	}

/**
Draws a straight line from the start to the end position using pen sizes larger than 1x1 pixel. 

@param	aPt1 Start position.
@param	aPt2 End position.
@param	aDrawStartPoint	If ETrue, draws the first pixel of the line.
@param  aScreenRect Rectangle representing the screen boundary.
@param aClipRect The rectangle to which the line is clipped.
@see CSwDirectGdiEngine::DrawLine()
*/
void CSwDirectGdiEngine::DoDrawSolidWideLine(const TPoint& aPt1,
									const TPoint& aPt2,
									TBool aDrawStartPoint,
									const TRect& aScreenRect,
									TRect aClipRect)
	{
	CFbsDrawDevice* drawDevice = iDrawDevice;

	TLinearDDA line;
	line.Construct(aPt1,aPt2);

	TPoint plotpt(aPt1);
	line.JumpToRect(aScreenRect);
	if (!aDrawStartPoint)
		line.SingleStep(plotpt);

	TInt* deferred = NULL;
	const TInt doubleheight = iPenSize.iHeight << 1;

	if (iPenArray)
		{
		deferred = new TInt[doubleheight];
		}

	if (!iPenArray || !deferred)
		{
		while (!line.SingleStep(plotpt))
			PenDrawClipped(plotpt, aClipRect);
		}
	else
		{
		const TBool down = (aPt2.iY >= aPt1.iY);

		for (TInt fillcount = 0; fillcount < doubleheight; )
			{
			deferred[fillcount++] = KMaxTInt;
			deferred[fillcount++] = KMinTInt;
			}

		TInt nextline = 0;
		TInt nexty = plotpt.iY;
		if (down)
			{
			nexty -= ((iPenSize.iHeight - 1) >> 1);
			}
		else
			{
			nexty += (iPenSize.iHeight >> 1);
			}

		TInt lasty = plotpt.iY;

		while (!line.SingleStep(plotpt))
			{
			if (plotpt.iY != lasty)
				{
				if (nexty >= aClipRect.iTl.iY && nexty < aClipRect.iBr.iY)
					{
					TInt left = deferred[nextline];
					TInt right = deferred[nextline + 1];
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
						drawDevice->WriteRgbMulti(left,nexty,right - left + 1,1,iPenColor,CGraphicsContext::EDrawModePEN);
						}
					}

				if (down)
					{
					nexty++;
					}
				else
					{
					nexty--;
					}
				
				lasty = plotpt.iY;
				deferred[nextline++] = KMaxTInt;
				deferred[nextline++] = KMinTInt;
				if (nextline == doubleheight)
					{
					nextline = 0;
					}
				}

			PenDrawDeferred(plotpt,deferred,nextline);
			}

		for (TInt restofline = 0; restofline < doubleheight; restofline += 2,nextline += 2)
			{
			if (nextline == doubleheight)
				nextline = 0;

			if (nexty >= aClipRect.iTl.iY && nexty < aClipRect.iBr.iY)
				{
				TInt left = deferred[nextline];
				TInt right = deferred[nextline+1];
				if (left < aClipRect.iTl.iX)
					{
					left = aClipRect.iTl.iX;
					}
				if (right >= aClipRect.iBr.iX)
					{
					right = aClipRect.iBr.iX-1;
					}

				if (left <= right)
					{
					drawDevice->WriteRgbMulti(left,nexty,right - left + 1,1,iPenColor,CGraphicsContext::EDrawModePEN);
					}
				}

			if (down)
				{
				nexty++;
				}
			else
				{
				nexty--;
				}
			}

		delete[] deferred;
		}
	}

/**
Draws a dotted straight line from the start to the end position using pen sizes larger than 1x1 pixel.

@param	aPt1 Start position.
@param	aPt2 End position.
@param	aDrawStartPoint	If ETrue, draws the first pixel of the line.
@param  aScreenRect Rectangle representing the screen boundary.
@param	aClipRect The rectangle to which the line is clipped.
@see CSwDirectGdiEngine::DrawLine()
*/
void CSwDirectGdiEngine::DoDrawDottedWideLine(const TPoint& aPt1,
									 const TPoint& aPt2,
									 TBool aDrawStartPoint,
									 const TRect& aScreenRect,
									 TRect aClipRect)
	{
	TLinearDDA line;
	line.Construct(aPt1,aPt2);

	TPoint plotpt(aPt1);
	line.JumpToRect(aScreenRect);
	if (!aDrawStartPoint)
		{
		line.SingleStep(plotpt);
		iDotParam += iDotDirection;
		}
	
	const TInt maxdim = Max(iPenSize.iWidth, iPenSize.iHeight);

	TBool done = EFalse;
	while (!done)
		{
		while (!done && !(iDotMask & (1 << ((iDotParam / maxdim) % iDotLength))))
			{
			done = line.SingleStep(plotpt);
			iDotParam += iDotDirection;
			}

		TPoint startdash(plotpt);
		TPoint enddash(plotpt);

		while (!done && (iDotMask & (1 << ((iDotParam / maxdim) % iDotLength))))
			{
			enddash = plotpt;
			done = line.SingleStep(plotpt);
			iDotParam += iDotDirection;
			}

		DoDrawSolidWideLine(startdash,enddash,ETrue,aScreenRect,aClipRect);
		}
	}

/**
Fills a polygon defined using an array of points. The first point in the array defines the 
start of the first side of the polygon. The final side of the polygon is drawn using the last point 
from the array. The area is filled with the current brush settings. 

Self-crossing polygons are filled according to the specified fill rule.

@param	aPointList	Array of points specifying the vertices of the polygon.
@param	aFillRule	Polygon filling rule.
*/
void CSwDirectGdiEngine::PolyFill(const TArray<TPoint>* aPointList, DirectGdi::TFillRule aFillRule)
	{
	TBool exists;
	TInt scanline;
	TInt pixelRunStart;
	TInt pixelRunEnd;

	TRect clipRect(0,0,0,0);
	const TInt limit = iDefaultRegionPtr->Count();
	for (TInt count = 0; count < limit; count++)
		{
		clipRect = (*iDefaultRegionPtr)[count];
		CSwDirectGdiPolygonFiller polyfill;
		polyfill.Construct(aPointList,aFillRule);

		for(polyfill.GetNextPixelRun(exists,scanline,pixelRunStart,pixelRunEnd);exists;
						polyfill.GetNextPixelRun(exists,scanline,pixelRunStart,pixelRunEnd))
			{
			TPoint start(pixelRunStart, scanline);
			TPoint end(pixelRunEnd, scanline);
			start += iOrigin;
			end += iOrigin;
			ClipFillLine(start,end,clipRect);
			}

		polyfill.Reset();
		iDrawDevice->UpdateRegion(clipRect);
		}
	}


/**
Fills a polygon defined using an array of points. The first point in the array defines the 
start of the first side of the polygon. The final side of the polygon is drawn using the last point 
from the array. The area is filled with the current brush settings. Optimized for polygons that are
much larger than the screen.

Self-crossing polygons are filled according to the specified fill rule.

@param	aPointList	Array of points specifying the vertices of the polygon.
@param	aFillRule	Polygon filling rule.
*/
void CSwDirectGdiEngine::PolyFillLarge(const TArray<TPoint>* aPointList, DirectGdi::TFillRule aFillRule)
	{
	TBool exists;
	TInt pixelRunStart;
	TInt pixelRunEnd;
	
	TRect clipRect(0,0,0,0);
	const TInt limit = iDefaultRegionPtr->Count();
	for (TInt count = 0; count < limit; count++)
		{
		clipRect = (*iDefaultRegionPtr)[count];
		CSwDirectGdiPolygonFiller polyfill;
		polyfill.Construct(aPointList,aFillRule,CSwDirectGdiPolygonFiller::EGetPixelRunsSequentiallyForSpecifiedScanLines);
		TInt clipRectOffsetStart = clipRect.iTl.iY - iOrigin.iY;
		TInt clipRectOffsetEnd = clipRect.iBr.iY - iOrigin.iY;

		for (TInt scanline = clipRectOffsetStart; scanline < clipRectOffsetEnd; scanline++)
			{
			polyfill.GetNextPixelRunOnSpecifiedScanLine(exists,scanline,pixelRunStart,pixelRunEnd);
			while (exists)
				{
				TPoint start(pixelRunStart,scanline);
				TPoint end(pixelRunEnd,scanline);
				start += iOrigin;
				end += iOrigin;
				ClipFillLine(start,end,clipRect);
				polyfill.GetNextPixelRunOnSpecifiedScanLine(exists,scanline,pixelRunStart,pixelRunEnd);
				}
			}

		polyfill.Reset();
		iDrawDevice->UpdateRegion(clipRect);
		}
	}

/**
Draws a polygon defined by an array of points using the current pen settings. The first point in the array defines the 
start of the first side of the polygon. The final side of the polygon is drawn using the last point 
from the array, and the line is drawn to the start point of the first side.

@param	aPointList List of points specifying the vertices of the polygon.
*/	
void CSwDirectGdiEngine::PolyOutline(const TArray<TPoint>* aPointList)
	{
	const TInt vertexes = aPointList->Count();
	
	for (TInt count = 0; count < vertexes; count++)
		{
		TPoint point1((*aPointList)[count]);
		TPoint point2((*aPointList)[(count + 1) % vertexes]);

		if (point1.iY < point2.iY)
			{
			DoDrawLine(point1,point2,ETrue);
			}
		else
			{
			iDotDirection = -1;
			iDotParam += Max(Abs(point2.iX - point1.iX),Abs(point2.iY - point1.iY));
			const TInt dotParam = iDotParam;
			DoDrawLine(point2,point1,EFalse);

			if (Abs(point2.iX - point1.iX) > Abs(point2.iY - point1.iY))
				{
				if (iPenStyle == DirectGdi::ESolidPen || (iDotMask & (1 << ((iDotParam / iPenSize.iWidth) % iDotLength))))
					DoPlot((*aPointList)[count]);
				}
			else
				{
				if (iPenStyle == DirectGdi::ESolidPen || (iDotMask & (1 << ((iDotParam / iPenSize.iHeight) % iDotLength))))
					DoPlot((*aPointList)[count]);
				}

			iDotDirection = 1;
			iDotParam = dotParam;
			}
		}
	}
