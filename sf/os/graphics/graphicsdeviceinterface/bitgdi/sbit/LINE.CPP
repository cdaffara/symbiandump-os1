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

#include <bitstd.h>
#include <bitdev.h>
#include "BITPANIC.H"
#include <bitdraw.h>
#include <graphics/fbsrasterizer.h>
#include "bitgcextradata.h"


/**  Draws a straight line between two points.

The function provides a concrete implementation of the pure virtual
function CGraphicsContext::DrawLine(). The function
behaviour is the same as documented in that class. */	
EXPORT_C void CFbsBitGc::DrawLine(const TPoint& aPt1,const TPoint& aPt2)
    {
	CheckDevice();

	TRect lineBoundingRect(aPt1,aPt2);
	lineBoundingRect.Normalize();
	lineBoundingRect.Move(iOrigin);
	lineBoundingRect.Grow((iPenSize.iWidth >> 1) + 1,(iPenSize.iHeight >> 1) + 1);
	if(!lineBoundingRect.Intersects(iUserClipRect))
		{
		iLinePosition = aPt2;// if DrawLine returns due to aPt2 being outside of the clipping rect then subsequent line are drawn from correct point.
		return;
		}

	SetupDevice();
	iDevice->DrawingBegin();
	DoDrawLine(aPt1,aPt2,ETrue);
	iDevice->DrawingEnd();
	}

/** Draws a straight line from the current drawing point to a specified
point.

The function provides a concrete implementation of the pure virtual
function CGraphicsContext::DrawLineTo(). The function
behaviour is the same as documented in that class. */
EXPORT_C void CFbsBitGc::DrawLineTo(const TPoint& aPoint)
	{
	DrawLine(iLinePosition,aPoint);
	}


/** Draws a straight line relative to the current drawing point, using a
vector.

The function provides a concrete implementation of the pure virtual
function CGraphicsContext::DrawLineBy(). The function
behaviour is the same as documented in that class. */	
EXPORT_C void CFbsBitGc::DrawLineBy(const TPoint& aVector)
    {
	DrawLine(iLinePosition,iLinePosition + aVector);
	}

/** Draws a polyline from a set of points specified in a list.

The functions provides a concrete implementation of the pure virtual
functions CGraphicsContext::DrawPolyLine(). The function
behaviour is the same as documented in that class. */
EXPORT_C void CFbsBitGc::DrawPolyLine(const CArrayFix<TPoint>* aPointList)
	{
	if(!aPointList || iPenSize.iWidth < 1 || iPenSize.iHeight < 1)
		return;

	const TInt vertexes = aPointList->Count()-1;

	for (TInt count = 0; count < vertexes; count++)
		DrawLine((*aPointList)[count],(*aPointList)[count + 1]);

	if (iPenStyle == CGraphicsContext::ESolidPen && vertexes >= 0)
		Plot((*aPointList)[vertexes]);
	}

/** Draws a polyline from a set of points specified in an array, but does not draw 
the final point of the last line.

@param aPointList An array containing the points on the polyline. */
EXPORT_C void CFbsBitGc::DrawPolyLineNoEndPoint(const CArrayFix<TPoint>* aPointList)
	{
	if(!aPointList || iPenSize.iWidth < 1 || iPenSize.iHeight < 1)
		return;

	const TInt vertexes = aPointList->Count() - 1;

	for (TInt count = 0; count < vertexes; count++)
		DrawLine((*aPointList)[count],(*aPointList)[count + 1]);
	}

/** Draws a polyline from a set of points specified in a list.

The functions provides a concrete implementation of the pure virtual
functions CGraphicsContext::DrawPolyLine(). The function
behaviour is the same as documented in that class. */
EXPORT_C void CFbsBitGc::DrawPolyLine(const TPoint* aPointList,TInt aNumPoints)
	{
    DrawPolyLineNoEndPoint(aPointList,aNumPoints);

	if (iPenStyle == CGraphicsContext::ESolidPen)
		Plot(aPointList[aNumPoints - 1]);
	}

/** Draws a polyline from a set of points specified in a list, but does not 
draw the final point of the last line.

@param aPointList Pointer to a set of points on the polyline. 
@param aNumPoints Number of points in the list. */
EXPORT_C void CFbsBitGc::DrawPolyLineNoEndPoint(const TPoint* aPointList,TInt aNumPoints)
	{
	if (!aPointList || iPenSize.iWidth < 1 || iPenSize.iHeight < 1)
		return;

	const TInt vertexes = aNumPoints - 1;

	for (TInt count = 0; count < vertexes; count++)
		DrawLine(aPointList[count],aPointList[count + 1]);
	}

/** Draws and fills a polygon defined using a list of points.

The function provides a concrete implementation of the pure virtual
function CGraphicsContext::DrawPolygon(). The function
behaviour is the same as documented in that class. */
EXPORT_C TInt CFbsBitGc::DrawPolygon(const CArrayFix<TPoint>* aPointList,TFillRule aFillRule)
	{
	CheckDevice();

	if (!aPointList)
		return KErrArgument;

	const TInt numpoints = aPointList->Count();

	if (numpoints == 0)
		return KErrNone; // Nothing to do!
	
	SetupDevice();
	iDevice->DrawingBegin(&iBrushBitmap);
	CFbsRasterizer* brushRasterizer = PrepareRasterizerForExtendedBitmap(iBrushBitmap);

	if (iBrushStyle != ENullBrush)
		{
		TRect pointrect(0,0,0,0);
		TRect truncrect(0,0,0,0);
		TBool largepolygon = EFalse;

		for (TInt count = 0; count < numpoints; count++)
			{
			pointrect.iTl = (*aPointList)[count] + iOrigin;
			truncrect.iTl = pointrect.iTl;
			iDevice->TruncateRect(truncrect);

			if (pointrect.iTl != truncrect.iTl)
				{
				largepolygon = ETrue;
				break;
				}
			}

		if (largepolygon)
			PolyFillLarge(aPointList,aFillRule);
		else
			PolyFill(aPointList,aFillRule);
		}

	if (iPenStyle != ENullPen)
		if (iPenSize.iWidth > 0 && iPenSize.iHeight > 0)
			PolyOutline(aPointList);

	if (brushRasterizer)
		{
		brushRasterizer->EndBitmap(iBrushBitmap.SerialNumber());
		}
	iDevice->DrawingEnd(&iBrushBitmap);

	return KErrNone;
	}


/** Draws and fills a polygon defined using a list of points.

The function provides a concrete implementation of the pure virtual
function CGraphicsContext::DrawPolygon(). The function
behaviour is the same as documented in that class. */	
EXPORT_C TInt CFbsBitGc::DrawPolygon(const TPoint* aPointList,
									 TInt aNumPoints,
									 CGraphicsContext::TFillRule aFillRule)
    {
	CheckDevice();

	if (!aPointList || aNumPoints < 0)
		return KErrArgument;

	if (aNumPoints == 0)
		return KErrNone; // Nothing to do!

	SetupDevice();
	iDevice->DrawingBegin(&iBrushBitmap);
	CFbsRasterizer* brushRasterizer = PrepareRasterizerForExtendedBitmap(iBrushBitmap);

	if (iBrushStyle != ENullBrush)
		{
		TRect pointrect(0,0,0,0);
		TRect truncrect(0,0,0,0);
		TBool largepolygon = EFalse;

		for (TInt count = 0; count < aNumPoints; count++)
			{
			pointrect.iTl = aPointList[count] + iOrigin;
			truncrect.iTl = pointrect.iTl;
			iDevice->TruncateRect(truncrect);

			if (pointrect.iTl != truncrect.iTl)
				{
				largepolygon = ETrue;
				break;
				}
			}

		if (largepolygon)
			PolyFillLarge(aPointList,aNumPoints,aFillRule);
		else
			PolyFill(aPointList,aNumPoints,aFillRule);
		}

	if (iPenStyle != ENullPen && iPenSize.iWidth > 0 && iPenSize.iHeight > 0)
		PolyOutline(aPointList,aNumPoints);

	if (brushRasterizer)
		{
		brushRasterizer->EndBitmap(iBrushBitmap.SerialNumber());
		}
	iDevice->DrawingEnd(&iBrushBitmap);

	return KErrNone;
	}

void CFbsBitGc::DoDrawLine(TPoint aPt1,TPoint aPt2,TBool aDrawStartPoint)
	{
	iLinePosition = aPt2;

	if (aPt1 == aPt2 || iPenStyle == ENullPen || !iPenSize.iWidth || !iPenSize.iHeight)
		return;

	aPt1 += iOrigin;
	aPt2 += iOrigin;

	TRect temp(aPt1,aPt2);
	temp.Normalize();
	temp.Grow(iPenSize.iWidth,iPenSize.iHeight);
	AddRect(temp);
	if (UserClipRect(temp))
		return;

	CFbsDrawDevice* drawDevice = iDevice->iDrawDevice;

	TRect screenRect;
	drawDevice->GetDrawRect(screenRect);
	screenRect.Grow(iPenSize.iWidth,iPenSize.iHeight);

	const TInt dotParam = iDotParam;
	TPoint plotpt(0,0);

	for (TInt count = 0; count < iDefaultRegionPtr->Count(); count++)
		{
		iDotParam = dotParam;
		iClipRect = (*iDefaultRegionPtr)[count];

		if (!iClipRect.Intersects(temp))
			{
			TLinearDDA line;
			line.Construct(aPt1,aPt2);
			line.JumpToRect(screenRect);
			if (iPenStyle != ESolidPen)
				while (!line.SingleStep(plotpt))
					iDotParam += iDotDirection;
			continue;
			}

		iClipRect.Intersection(temp);

		if ((iPenSize.iWidth > 1 || iPenSize.iHeight > 1) && iPenStyle == ESolidPen) // wide solid line
			DoDrawSolidWideLine(aPt1,aPt2,aDrawStartPoint,screenRect);
		else if (iPenSize.iWidth > 1 || iPenSize.iHeight > 1) // dotted line
			DoDrawDottedWideLine(aPt1,aPt2,aDrawStartPoint,screenRect);
		else if (iPenStyle != ESolidPen) // single pixel dotted line
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
				PenDrawClipped(plotpt);
				iDotParam += iDotDirection;
				}
			}
		else if (aPt1.iY == aPt2.iY && 
				 (aPt1.iY >= iClipRect.iTl.iY && aPt1.iY < iClipRect.iBr.iY))
			{ // single pixel solid horizontal line
			TInt start = Min(aPt1.iX,aPt2.iX + 1);
			TInt length = Abs(aPt2.iX - aPt1.iX);

			if (!aDrawStartPoint)
				if (aPt1.iX < aPt2.iX)
					start++;
				else
					length--;
			if (start < iClipRect.iTl.iX)
				{
				length += start - iClipRect.iTl.iX;
				start = iClipRect.iTl.iX;
				}
			if (start + length > iClipRect.iBr.iX)
				length = iClipRect.iBr.iX - start;

			if (length > 0)
				{
				BG_ASSERT_DEBUG(start >= iUserClipRect.iTl.iX,EBitgdiPanicOutOfBounds);
				BG_ASSERT_DEBUG(aPt1.iY >= iUserClipRect.iTl.iY,EBitgdiPanicOutOfBounds);
				BG_ASSERT_DEBUG(start + length <= iUserClipRect.iBr.iX,EBitgdiPanicOutOfBounds);
				BG_ASSERT_DEBUG(aPt1.iY < iUserClipRect.iBr.iY,EBitgdiPanicOutOfBounds);

				drawDevice->WriteRgbMulti(start,aPt1.iY,length,1,iPenColor,iDrawMode);
				}
			}
		else if (aPt1.iX == aPt2.iX && (aPt1.iX >= iClipRect.iTl.iX && aPt1.iX < iClipRect.iBr.iX))
			{ // single pixel solid vertical line
			TInt start = Min(aPt1.iY,aPt2.iY + 1);
			TInt length = Abs(aPt2.iY - aPt1.iY);

			if (!aDrawStartPoint)
				if (aPt1.iY < aPt2.iY)
					start++;
				else
					length--;

			if (start < iClipRect.iTl.iY)
				{
				length += start - iClipRect.iTl.iY;
				start = iClipRect.iTl.iY;
				}
			if (start + length > iClipRect.iBr.iY)
				length = iClipRect.iBr.iY - start;

			if (length > 0)
				{
				BG_ASSERT_DEBUG(aPt1.iX >= iUserClipRect.iTl.iX,EBitgdiPanicOutOfBounds);
				BG_ASSERT_DEBUG(start >= iUserClipRect.iTl.iY,EBitgdiPanicOutOfBounds);
				BG_ASSERT_DEBUG(aPt1.iX < iUserClipRect.iBr.iX,EBitgdiPanicOutOfBounds);
				BG_ASSERT_DEBUG(start + length <= iUserClipRect.iBr.iY,EBitgdiPanicOutOfBounds);

				drawDevice->WriteRgbMulti(aPt1.iX,start,1,length,iPenColor,iDrawMode);
				}
			}
		else
			{ // single pixel solid diagonal line
			TLinearDDA line;
			line.Construct(aPt1,aPt2);

			line.JumpToRect(screenRect);

			if (!aDrawStartPoint)
				line.SingleStep(plotpt);

			while (!line.SingleStep(plotpt))
				{
				if (iClipRect.Contains(plotpt))
					{
					BG_ASSERT_DEBUG(plotpt.iX >= iUserClipRect.iTl.iX,EBitgdiPanicOutOfBounds);
					BG_ASSERT_DEBUG(plotpt.iY >= iUserClipRect.iTl.iY,EBitgdiPanicOutOfBounds);
					BG_ASSERT_DEBUG(plotpt.iX < iUserClipRect.iBr.iX,EBitgdiPanicOutOfBounds);
					BG_ASSERT_DEBUG(plotpt.iY < iUserClipRect.iBr.iY,EBitgdiPanicOutOfBounds);

					drawDevice->WriteRgb(plotpt.iX,plotpt.iY,iPenColor,iDrawMode);
					}
				}
			}

		drawDevice->UpdateRegion(iClipRect);
		}
	}

void CFbsBitGc::DoDrawSolidWideLine(const TPoint& aPt1,
									const TPoint& aPt2,
									TBool aDrawStartPoint,
									const TRect& aScreenRect)
	{
	CFbsDrawDevice* drawDevice = iDevice->iDrawDevice;

	TLinearDDA line;
	line.Construct(aPt1,aPt2);

	TPoint plotpt(aPt1);
	line.JumpToRect(aScreenRect);
	if (!aScreenRect.Contains(plotpt) || !aDrawStartPoint)
		line.SingleStep(plotpt);

	TInt* deferred = NULL;
	const TInt doubleheight = iPenSize.iHeight << 1;

	if (iFbsBitGcExtraData->PenArray())
		deferred = new TInt[doubleheight];

	if (!iFbsBitGcExtraData->PenArray() || !deferred)
		{
		while (!line.SingleStep(plotpt))
			PenDrawClipped(plotpt);
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
			nexty -= ((iPenSize.iHeight - 1) >> 1);
		else
			nexty += (iPenSize.iHeight >> 1);

		TInt lasty = plotpt.iY;

		while (!line.SingleStep(plotpt))
			{
			if (plotpt.iY != lasty)
				{
				if (nexty >= iClipRect.iTl.iY && nexty < iClipRect.iBr.iY)
					{
					TInt left = deferred[nextline];
					TInt right = deferred[nextline + 1];
					if (left < iClipRect.iTl.iX)
						left = iClipRect.iTl.iX;
					if (right >= iClipRect.iBr.iX)
						right = iClipRect.iBr.iX - 1;

					if (left <= right)
						drawDevice->WriteRgbMulti(left,nexty,right - left + 1,1,iPenColor,CGraphicsContext::EDrawModePEN);
					}

				if (down)
					nexty++;
				else
					nexty--;
				lasty = plotpt.iY;
				deferred[nextline++] = KMaxTInt;
				deferred[nextline++] = KMinTInt;
				if (nextline == doubleheight)
					nextline = 0;
				}

			PenDrawDeferred(plotpt,deferred,nextline);
			}

		for (TInt restofline = 0; restofline < doubleheight; restofline += 2,nextline += 2)
			{
			if (nextline == doubleheight)
				nextline = 0;

			if (nexty >= iClipRect.iTl.iY && nexty < iClipRect.iBr.iY)
				{
				TInt left = deferred[nextline];
				TInt right = deferred[nextline+1];
				if (left < iClipRect.iTl.iX)
					left = iClipRect.iTl.iX;
				if (right >= iClipRect.iBr.iX)
					right = iClipRect.iBr.iX-1;

				if (left <= right)
					drawDevice->WriteRgbMulti(left,nexty,right - left + 1,1,iPenColor,CGraphicsContext::EDrawModePEN);
				}

			if (down)
				nexty++;
			else
				nexty--;
			}

		delete[] deferred;
		}
	}


void CFbsBitGc::DoDrawDottedWideLine(const TPoint& aPt1,
									 const TPoint& aPt2,
									 TBool aDrawStartPoint,
									 const TRect& aScreenRect)
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

	const TInt maxdim = Max(iPenSize.iWidth,iPenSize.iHeight);

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

		DoDrawSolidWideLine(startdash,enddash,ETrue,aScreenRect);
		}
	}

// if iBrushBitmap is an extended bitmap, PrepareRasterizerForExtendedBitmap() must have been called before this method
void CFbsBitGc::PolyFill(const CArrayFix<TPoint>* aPointList,TFillRule aFillRule)
	{
	TBool exists;
	TInt scanline;
	TInt pixelRunStart;
	TInt pixelRunEnd;

	const TInt limit = iDefaultRegionPtr->Count();
	for (TInt count = 0; count < limit; count++)
		{
		iClipRect = (*iDefaultRegionPtr)[count];
		AddRect(iClipRect);
		if (UserClipRect(iClipRect))
			continue;

		CPolygonFiller polyfill;
		polyfill.Construct(aPointList,aFillRule);

		for(polyfill.GetNextPixelRun(exists,scanline,pixelRunStart,pixelRunEnd);exists;
						polyfill.GetNextPixelRun(exists,scanline,pixelRunStart,pixelRunEnd))
			{
			TPoint start(pixelRunStart,scanline),end(pixelRunEnd,scanline);
			start += iOrigin;
			end += iOrigin;
			ClipFillLine(start,end);
			}

		polyfill.Reset();
		iDevice->iDrawDevice->UpdateRegion(iClipRect);
		}
	}

// if iBrushBitmap is an extended bitmap, PrepareRasterizerForExtendedBitmap() must have been called before this method
void CFbsBitGc::PolyFillLarge(const CArrayFix<TPoint>* aPointList,TFillRule aFillRule)
	{
	TBool exists;
	TInt pixelRunStart;
	TInt pixelRunEnd;

	const TInt limit = iDefaultRegionPtr->Count();
	for (TInt count = 0; count < limit; count++)
		{
		iClipRect = (*iDefaultRegionPtr)[count];
		AddRect(iClipRect);
		if (UserClipRect(iClipRect))
			continue;

		CPolygonFiller polyfill;
		polyfill.Construct(aPointList,aFillRule,CPolygonFiller::EGetPixelRunsSequentiallyForSpecifiedScanLines);
		TInt clipRectOffsetStart = iClipRect.iTl.iY - iOrigin.iY;
		TInt clipRectOffsetEnd = iClipRect.iBr.iY - iOrigin.iY;

		for (TInt scanline = clipRectOffsetStart; scanline < clipRectOffsetEnd; scanline++)
			{
			polyfill.GetNextPixelRunOnSpecifiedScanLine(exists,scanline,pixelRunStart,pixelRunEnd);
			while (exists)
				{
				TPoint start(pixelRunStart,scanline),end(pixelRunEnd,scanline);
				start += iOrigin;
				end += iOrigin;
				ClipFillLine(start,end);
				polyfill.GetNextPixelRunOnSpecifiedScanLine(exists,scanline,pixelRunStart,pixelRunEnd);
				}
			}

		polyfill.Reset();
		iDevice->iDrawDevice->UpdateRegion(iClipRect);
		}
	}

void CFbsBitGc::PolyOutline(const CArrayFix<TPoint>* aPointList)
	{
	const TInt vertexes = aPointList->Count();

	for (TInt count = 0; count < vertexes; count++)
		{
		TPoint point1((*aPointList)[count]);
		TPoint point2((*aPointList)[(count + 1) % vertexes]);

		if (point1.iY < point2.iY)
			DoDrawLine(point1,point2,ETrue);
		else
			{
			iDotDirection = -1;
			iDotParam += Max(Abs(point2.iX - point1.iX),Abs(point2.iY - point1.iY));
			const TInt dotParam = iDotParam;
			DoDrawLine(point2,point1,EFalse);

			if (Abs(point2.iX - point1.iX) > Abs(point2.iY - point1.iY))
				{
				if (iPenStyle == CGraphicsContext::ESolidPen || (iDotMask & (1 << ((iDotParam / iPenSize.iWidth) % iDotLength))))
					DoPlot((*aPointList)[count]);
				}
			else
				{
				if (iPenStyle == CGraphicsContext::ESolidPen || (iDotMask & (1 << ((iDotParam / iPenSize.iHeight) % iDotLength))))
					DoPlot((*aPointList)[count]);
				}

			iDotDirection = 1;
			iDotParam = dotParam;
			}
		}
	}

// if iBrushBitmap is an extended bitmap, PrepareRasterizerForExtendedBitmap() must have been called before this method
void CFbsBitGc::PolyFill(const TPoint* aPointList,TInt aNumPoints,TFillRule aFillRule)
	{
	TBool exists;
	TInt scanline;
	TInt pixelRunStart;
	TInt pixelRunEnd;

	const TInt limit = iDefaultRegionPtr->Count();
	for (TInt count = 0; count < limit; count++)
		{
		iClipRect = (*iDefaultRegionPtr)[count];
		AddRect(iClipRect);
		if (UserClipRect(iClipRect))
			continue;

		CPolygonFiller polyfill;
		polyfill.Construct(aPointList,aNumPoints,aFillRule);

		for (polyfill.GetNextPixelRun(exists,scanline,pixelRunStart,pixelRunEnd);exists;
						polyfill.GetNextPixelRun(exists,scanline,pixelRunStart,pixelRunEnd))
			{
			TPoint start(pixelRunStart,scanline),end(pixelRunEnd,scanline);
			start += iOrigin;
			end += iOrigin;
			ClipFillLine(start,end);
			}

		polyfill.Reset();
		iDevice->iDrawDevice->UpdateRegion(iClipRect);
		}
	}

// if iBrushBitmap is an extended bitmap, PrepareRasterizerForExtendedBitmap() must have been called before this method
void CFbsBitGc::PolyFillLarge(const TPoint* aPointList,TInt aNumPoints,TFillRule aFillRule)
	{
	TBool exists;
	TInt pixelRunStart;
	TInt pixelRunEnd;

	const TInt limit = iDefaultRegionPtr->Count();
	for (TInt count = 0; count < limit; count++)
		{
		iClipRect = (*iDefaultRegionPtr)[count];
		AddRect(iClipRect);
		if (UserClipRect(iClipRect))
			continue;

		CPolygonFiller polyfill;
		polyfill.Construct(aPointList,aNumPoints,aFillRule,CPolygonFiller::EGetPixelRunsSequentiallyForSpecifiedScanLines);
		TInt clipRectOffsetStart = iClipRect.iTl.iY - iOrigin.iY;
		TInt clipRectOffsetEnd = iClipRect.iBr.iY - iOrigin.iY;

		for (TInt scanline = clipRectOffsetStart; scanline < clipRectOffsetEnd; scanline++)
			{
			polyfill.GetNextPixelRunOnSpecifiedScanLine(exists,scanline,pixelRunStart,pixelRunEnd);
			while (exists)
				{
				TPoint start(pixelRunStart,scanline),end(pixelRunEnd,scanline);
				start += iOrigin;
				end += iOrigin;
				ClipFillLine(start,end);

				polyfill.GetNextPixelRunOnSpecifiedScanLine(exists,scanline,pixelRunStart,pixelRunEnd);
				}
			}

		polyfill.Reset();
		iDevice->iDrawDevice->UpdateRegion(iClipRect);
		}
	}

void CFbsBitGc::PolyOutline(const TPoint* aPointList,TInt aNumPoints)
	{
	for (TInt count = 0; count < aNumPoints; count++)
		{
		TPoint point1(aPointList[count]);
		TPoint point2(aPointList[(count + 1) % aNumPoints]);

		if (point1.iY < point2.iY)
			DoDrawLine(point1,point2,ETrue);
		else
			{
			iDotDirection = -1;
			iDotParam += Max(Abs(point2.iX - point1.iX),Abs(point2.iY - point1.iY));
			const TInt dotParam = iDotParam;

			DoDrawLine(point2,point1,EFalse);

			if (Abs(point2.iX - point1.iX) > Abs(point2.iY - point1.iY))
				{
				if (iPenStyle == CGraphicsContext::ESolidPen || (iDotMask & (1 << ((iDotParam / iPenSize.iWidth) % iDotLength))))
					DoPlot(aPointList[count]);
				}
			else
				{
				if (iPenStyle == CGraphicsContext::ESolidPen || (iDotMask & (1 << ((iDotParam / iPenSize.iHeight) % iDotLength))))
					DoPlot(aPointList[count]);
				}

			iDotDirection = 1;
			iDotParam = dotParam;
			}
		}
	}

