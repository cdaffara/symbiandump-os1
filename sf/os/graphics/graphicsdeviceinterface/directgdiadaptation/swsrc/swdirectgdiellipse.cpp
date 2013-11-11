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
#include "swdirectgdiellipse.h"
#include "swdirectgdiengine.h"

/*
 * TSwDirectGdiEllipse
 */
 
 /**
 Initialises the values of the ellipse so that it conforms to a rectangle entered as a parameter.
 @param aRect the rectangle within which the ellipse is drawn
 */
void TSwDirectGdiEllipse::Construct(const TRect& aRect)
	{
	TInt width = aRect.Width();
	TInt height = aRect.Height();
	iA = (width-1) >> 1;
	iB = (height-1) >> 1;
	iXAdj = (width+1) & 1;
	iYAdj = (height+1) & 1;
	iOffset = aRect.iTl;
	iX = 0;
	iY = iB;
	iASquared = iA * iA;
	iBSquared = iB * iB;
	iASquBSqu = iASquared * iBSquared;
	iD1 = iBSquared - iASquared * iB + (iASquared>>1);
	if (width<=0 || height<=0)
		{
		iStatus = EComplete;
		}
	else if (width<=2 || height<=2)
		{
		iStatus = ELine;
		}
	else
		{
		iStatus = EInitialised;
		}
	}

/**
Does the next stage in producing an ellipse by taking four points (the corners of 
the rectangle the ellipse should fill) as parameters. Updates TSwDirectGdiEllipse status 
accordingly and calls Output(aTopLeft,aTopRight,aBottomLeft,aBottomRight).

@param aTopLeft Top left corner of rectangle. 
@param aTopRight Top right corner of rectangle.
@param aBottomLeft Bottom left corner of rectangle.
@param aBottomRight Bottom right corner of rectangle.
@return TBool ETrue if step completed successfully.
*/
TBool TSwDirectGdiEllipse::SingleStep(TPoint& aTopLeft, TPoint& aTopRight,
									TPoint& aBottomLeft, TPoint& aBottomRight)
	{
	TBool ret = EFalse;
	if (iStatus == EFirstSector)
		{
		if (iD1 < 0)
			{
			iD1 += iBSquared * ((iX<<1)+3);
			}
		else if (iY > 0)
			{
			iD1 += iBSquared * ((iX<<1)+3) + iASquared * (2-(iY<<1));
			iY--;
			}
		iX++;
		ret = Output(aTopLeft,aTopRight,aBottomLeft,aBottomRight);
		if (iStatus == EComplete && iX<iA)
			{
			iStatus = EFirstSector;
			return EFalse;
			}
		if (iASquared*iY<=iBSquared*(iX+1) && ret==EFalse)
			{
			iStatus = ESecondSector;
			iD2 =- iASquBSqu + iBSquared * iX * iX + iASquared * (iY-1) * (iY-1);
			}
		return ret;
		}
	if (iStatus == ESecondSector)
		{
		if (iD2 < 0)
			{
			iD2 += iBSquared * ((iX<<1)+2) + iASquared * (3-(iY<<1));
			iX++;
			}
		else
			{
			iD2 += iASquared * (3-(iY<<1));
			}
		iY--;
		return Output(aTopLeft, aTopRight, aBottomLeft, aBottomRight);
		}
	if (iStatus == ELine)
		{
		ret = Output(aTopLeft, aTopRight, aBottomLeft, aBottomRight);
		if (iA == 0)
			{
			iY--;
			}
		else
			{
			iX++;
			if (iX > iA+iXAdj) 
				{
				ret = ETrue;
				}
			else
				{
				iStatus = ELine;
				ret = EFalse;
				}
			}
		return ret;
		}
	if (iStatus == EInitialised)
		{
		iStatus = EFirstSector;
		return Output(aTopLeft, aTopRight, aBottomLeft, aBottomRight);
		}
	Output(aTopLeft, aTopRight, aBottomLeft, aBottomRight);
	return ETrue;
	}

/**
Sets the absolute points that define the ellipse as calculated using its iOffset 
from the origin and using the half width and half height of the rectangle iA and iB.

@param aTopLeft The absolute (x,y) position for the top left point.
@param aTopRight The absolute (x,y) position for the top right point.
@param aBottomLeft The absolute (x,y) position for the bottom left point.
@param aBottomRight The absolute (x,y) position for the bottom right point.
@return ETrue if a valid rectangle is produced, else EFalse. Also sets
iStatus to EComplete.
*/
TBool TSwDirectGdiEllipse::Output(TPoint& aTopLeft, TPoint& aTopRight,
								TPoint& aBottomLeft, TPoint& aBottomRight)
	{
	TInt lx = iA-iX+iOffset.iX;
	TInt ty = iB-iY+iOffset.iY;
	TInt rx = iA+iX+iXAdj+iOffset.iX;
	TInt by = iB+iY+iYAdj+iOffset.iY;
	aTopLeft.SetXY(lx,ty);
	aTopRight.SetXY(rx,ty);
	aBottomLeft.SetXY(lx,by);
	aBottomRight.SetXY(rx,by);
	if (iY <= 0)
		{
		iStatus = EComplete;
		if (iYAdj==0 || ty>by)
			return ETrue;
		}
	return EFalse;
	}

/**
By analysing the current state of the ellipse the process is taken to the next appropriate step.
If iStatus = EInitialised only one step will be taken, if the ellipse is already semi constructed then 
it will be taken to completion. Takes in four point parameters that define the rectangle in order to pass to 
SingleStep(aTopLeft,aTopRight,aBottomLeft,aBottomRight).

@param aTopLeft Top-left corner of rectangle.
@param aTopRight Top-right corner of rectangle.
@param aBottomLeft Bottom-left corner of rectangle.
@param aBottomRight Bottom-right corner of rectangle.
@return ETrue if a valid rectangle is produced, else EFalse.
*/
TBool TSwDirectGdiEllipse::NextStep(TPoint& aTopLeft, TPoint& aTopRight,
								  TPoint& aBottomLeft, TPoint& aBottomRight)
	{
	if(iStatus == EInitialised)
		{
		return(SingleStep(aTopLeft,aTopRight,aBottomLeft,aBottomRight));
		}
	TInt prevlev = iY;
	TBool ret;
	do
		{
		ret = SingleStep(aTopLeft,aTopRight,aBottomLeft,aBottomRight);
		} 
	while (prevlev==iY && ret==EFalse);	
	return ret;
	}

/**
Constructs an ellipse from the rectangle that has been added. Assesses the position of 
the points and the places where they intersect the ellipse.
 
@param aRect The rectangle within which the ellipse is drawn.
@param aPoint A point to compare with the ellipse to determine if intersection occurs. 
@return TPoint The point is set to the corner which the intersection is nearest to.
*/
TPoint TSwDirectGdiEllipse::Intersection(const TRect& aRect, const TPoint& aPoint)
	{
	Construct(aRect);					//constructs the rect (an elipse object)
	TPoint centre = aRect.Center();		//centre of ellipse
	TPoint ptcpy(aPoint);				
	ptcpy -= iOffset;						//ptcpy = aPoint - iOffset - TPoint(iA,iB)	//radius from centre of ellipse		
	ptcpy -= TPoint(iA,iB);				
	TPoint pt[4], opt[4];			
	TInt mpt[4], ompt[4];
	TInt count = 0;
	for( ; count < 4; count++)
		{
		ompt[count]=KMaxTInt;			//fills ompt 1->4 with KMaxTInt
		}
	while (SingleStep(pt[0], pt[1], pt[2], pt[3]) == EFalse) 	//creates a complete ellipse with pts as rect
		{
		for (count = 0; count < 4; count++)
			{
			mpt[count] = Abs((pt[count].iY-iOffset.iY-iB)*(ptcpy.iX)-(ptcpy.iY)*(pt[count].iX-iOffset.iX-iA));
			if (mpt[count] < ompt[count]) //use the larger number set.
				{
				ompt[count] = mpt[count];
				opt[count] = pt[count];
				}						
			}
		}
	if (pt[0].iY == pt[2].iY)	//if it is horizontal
		{
		for (count = 0; count < 4; count++)
			{
			mpt[count] = Abs((pt[count].iY-iOffset.iY-iB)*(ptcpy.iX)-(ptcpy.iY)*(pt[count].iX-iOffset.iX-iA));
			if (mpt[count] < ompt[count]) //use the larger number set.
				{
				ompt[count] = mpt[count];
				opt[count] = pt[count];
				}
			}
		}
	if (ptcpy.iX<0 && ptcpy.iY<0)	//if point is further left and higher than centre of rect
		{
		return opt[0];
		}
	if (ptcpy.iY < 0)			//if point is higher than centre of rect
		{
		return opt[1];
		}
	if (ptcpy.iX < 0)			//if point is further left than centre of rect
		{
		return opt[2];
		}
	if (aPoint.iX<centre.iX && aPoint.iY<centre.iY)	//if point is further left and higher than centre of rect
		{
		return opt[0];
		}
	if (aPoint.iY < centre.iY)	//if point is higher than centre of rect
		{
		return opt[1];
		}
	if (aPoint.iX < centre.iX)	//if point is further left than centre of rect
		{
		return opt[2];
		}
	return(opt[3]);			//else 
	}

//
// Ellipse drawing
//

/**
@see MDirectGdiEngine::DrawEllipse()
*/
void CSwDirectGdiEngine::DrawEllipse(const TRect& aRect)
	{
	TRect rcpy(aRect);
	rcpy.Move(iOrigin);
	TruncateRect(rcpy);
	iBrushBitmap.BeginDataAccess();
	if(iBrushStyle!=DirectGdi::ENullBrush)
		EllipseFill(rcpy);
	
	if(iPenStyle!=DirectGdi::ENullPen)
		{
		if(iPenSize.iWidth>1 && iPenSize.iHeight>1)
			{
			EllipseOutlineWide(rcpy);
			}
		else if(iPenSize.iWidth==1 || iPenSize.iHeight==1)
			{
			EllipseOutline(rcpy);
			}			
		}
	iBrushBitmap.EndDataAccess(ETrue);
	}

/**
Draws an ellipse inside the given rectangle. Current pen settings apply.
@param	aRect The rectangle in which to draw the ellipse.
*/
void CSwDirectGdiEngine::EllipseOutline(const TRect& aRect)
	{
	TPoint tl,tr,bl,br;
#if defined(_DEBUG)
	TRect deviceDestRect;
	iDrawDevice->GetDrawRect(deviceDestRect);
#endif
	TRect clipRect(0,0,0,0);
	const CGraphicsContext::TDrawMode drawMode = GcDrawMode(iDrawMode);
	for(TInt count=0;count<iDefaultRegionPtr->Count();count++)
		{
		clipRect=(*iDefaultRegionPtr)[count];
		if(!clipRect.Intersects(aRect))
			continue;
		clipRect.Intersection(aRect);
		GRAPHICS_ASSERT_DEBUG(clipRect.iTl.iX >= deviceDestRect.iTl.iX, EDirectGdiPanicOutOfBounds);
		GRAPHICS_ASSERT_DEBUG(clipRect.iTl.iY >= deviceDestRect.iTl.iY, EDirectGdiPanicOutOfBounds);
		GRAPHICS_ASSERT_DEBUG(clipRect.iBr.iX <= deviceDestRect.iBr.iX, EDirectGdiPanicOutOfBounds);
		GRAPHICS_ASSERT_DEBUG(clipRect.iBr.iY <= deviceDestRect.iBr.iY, EDirectGdiPanicOutOfBounds);
		TSwDirectGdiEllipse ellipse;
		ellipse.Construct(aRect);
		TInt pattern=0;
		while(!ellipse.SingleStep(tl,tr,bl,br))
			{
			if(iPenStyle==DirectGdi::ESolidPen || (iDotMask&(1<<(pattern%iDotLength))))
				{
				if(tl.iY>=clipRect.iTl.iY && tl.iY<clipRect.iBr.iY)
					{
					if(tl.iX>=clipRect.iTl.iX && tl.iX<clipRect.iBr.iX)
						iDrawDevice->WriteRgb(tl.iX,tl.iY,iPenColor,drawMode);
					if(tr.iX>=clipRect.iTl.iX && tr.iX<clipRect.iBr.iX && tl.iX!=tr.iX)
						iDrawDevice->WriteRgb(tr.iX,tr.iY,iPenColor,drawMode);
					}
				if(bl.iY>=clipRect.iTl.iY && bl.iY<clipRect.iBr.iY)
					{
					if(bl.iX>=clipRect.iTl.iX && bl.iX<clipRect.iBr.iX)
						iDrawDevice->WriteRgb(bl.iX,bl.iY,iPenColor,drawMode);
					if(br.iX>=clipRect.iTl.iX && br.iX<clipRect.iBr.iX && bl.iX!=br.iX)
						iDrawDevice->WriteRgb(br.iX,br.iY,iPenColor,drawMode);
					}
				}
			pattern++;
			}
		if(tl.iY==bl.iY && tl.iY>=clipRect.iTl.iY && tl.iY<clipRect.iBr.iY)
			{
			if(tl.iX>=clipRect.iTl.iX && tl.iX<clipRect.iBr.iX)
				iDrawDevice->WriteRgb(tl.iX,tl.iY,iPenColor,drawMode);
			if(tr.iX>=clipRect.iTl.iX && tr.iX<clipRect.iBr.iX && tl.iX!=tr.iX)
				iDrawDevice->WriteRgb(tr.iX,tr.iY,iPenColor,drawMode);
			}
		iDrawDevice->UpdateRegion(clipRect);
		}
	}

/**
Draws an ellipse inside the given rectangle. Current pen settings apply.
@param	aRect The rectangle in which to draw the ellipse.
*/
void CSwDirectGdiEngine::EllipseOutlineWide(const TRect& aRect)
	{
	TRect rcpy(aRect);
	TPoint tl,tr,bl,br;
	TInt halfpenwidth=(iPenSize.iWidth+1)>>1;
	TInt halfpenheight=(iPenSize.iHeight+1)>>1;
	rcpy.Grow(halfpenwidth, halfpenheight);
	TInt dp=iDotParam;
	TRect clipRect(0,0,0,0);
	for(TInt count=0;count<iDefaultRegionPtr->Count();count++)
		{
		clipRect=(*iDefaultRegionPtr)[count];
		if(!clipRect.Intersects(rcpy))
			continue;
		clipRect.Intersection(rcpy);
		TSwDirectGdiEllipse ellipse;
		ellipse.Construct(aRect);
		iDotParam=Max(iPenSize.iWidth>>1,iPenSize.iHeight>>1);
		while(!ellipse.SingleStep(tl,tr,bl,br))
			{
			PenDrawClipped(tl, clipRect);
			PenDrawClipped(tr, clipRect);
			PenDrawClipped(bl, clipRect);
			PenDrawClipped(br, clipRect);
			iDotParam+=iDotDirection;
			}
		if(tl.iY==bl.iY)
			{
			PenDrawClipped(tl, clipRect);
			PenDrawClipped(tr, clipRect);
			}
		iDrawDevice->UpdateRegion(clipRect);
		}
	iDotParam=dp;
	}

/**
Fills an ellipse inside the given rectangle. Current brush settings apply.
@param	aRect The rectangle in which to draw the ellipse.
*/
void CSwDirectGdiEngine::EllipseFill(const TRect& aRect)
	{
	TRect rcpy(aRect);
	if(iPenSize.iWidth==0 || iPenSize.iHeight==0)
		{
		rcpy.Grow(1,1);
		}
	TPoint tl,tr,bl,br;
	TRect clipRect(0,0,0,0);
	for(TInt count=0;count<iDefaultRegionPtr->Count();count++)
		{
		clipRect=(*iDefaultRegionPtr)[count];
		if(!clipRect.Intersects(rcpy))
			continue;
		clipRect.Intersection(rcpy);
		TSwDirectGdiEllipse ellipse;
		ellipse.Construct(rcpy);
		while(!ellipse.NextStep(tl,tr,bl,br))
			{
			tl.iX++;
			tr.iX--;
			bl.iX++;
			br.iX--;
			ClipFillLine(tl,tr, clipRect);
			ClipFillLine(bl,br, clipRect);
			}
		if(tl.iY==bl.iY)
			{
			tl.iX++;
			tr.iX--;
			ClipFillLine(tl,tr, clipRect);
			}
		iDrawDevice->UpdateRegion(clipRect);
		}
	}
