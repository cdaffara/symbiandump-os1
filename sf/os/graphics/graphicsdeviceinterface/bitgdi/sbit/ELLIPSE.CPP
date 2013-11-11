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

/*
 * TEllipse
 */
 
 /**
 Initialises the values of the ellipse so that it conforms to a rectangle entered as a parameter.
 @param aRect the rectangle within which the ellipse is drawn
 */
EXPORT_C void TEllipse::Construct(const TRect& aRect)
	{
	TInt width=aRect.Width();
	TInt height=aRect.Height();
	iA=(width-1)>>1;
	iB=(height-1)>>1;
	iXAdj=(width+1)&1;
	iYAdj=(height+1)&1;
	iOffset=aRect.iTl;
	iX=0;
	iY=iB;
	iASquared=iA*iA;
	iBSquared=iB*iB;
	iASquBSqu=iASquared*iBSquared;
	iD1=iBSquared-iASquared*iB+(iASquared>>1);
	if(width<=0 || height<=0)
		iStatus=EComplete;
	else if(width<=2 || height<=2)
		iStatus=ELine;
	else if(iA+iXAdj==0 || iB+iYAdj==0)
		iStatus=EComplete;
	else
		iStatus=EInitialised;
	}

/**
Does the next stage in producing an ellipse by taking four points (the corners of 
the rectangle the ellipse should fill) as parameters. Updates TEllipse status 
accordingly and calls <code>Output(aTopLeft,aTopRight,aBottomLeft,aBottomRight)</code>.
@param aTopLeft Top left corner of rectangle 
@param aTopRight Top right corner of rectangle
@param aBottomLeft Bottom left corner of rectangle
@param aBottomRight Bottom right corner of rectangle
@return TBool ETrue if step completed successfully.
*/
EXPORT_C TBool TEllipse::SingleStep(TPoint& aTopLeft,TPoint& aTopRight,
									TPoint& aBottomLeft,TPoint& aBottomRight)
	{
	TBool ret=EFalse;
	if(iStatus==EFirstSector)
		{
		if(iD1<0)
			iD1+=iBSquared*((iX<<1)+3);
		else if(iY>0)
			{
			iD1+=iBSquared*((iX<<1)+3)+iASquared*(2-(iY<<1));
			iY--;
			}
		iX++;
		ret=Output(aTopLeft,aTopRight,aBottomLeft,aBottomRight);
		if(iStatus==EComplete && iX<iA)
			{
			iStatus=EFirstSector;
			return(EFalse);
			}
		if(iASquared*iY<=iBSquared*(iX+1) && ret==EFalse)
			{
			iStatus=ESecondSector;
			iD2=-iASquBSqu+iBSquared*iX*iX+iASquared*(iY-1)*(iY-1);
			}
		return(ret);
		}
	if(iStatus==ESecondSector)
		{
		if(iD2<0)
			{
			iD2+=iBSquared*((iX<<1)+2)+iASquared*(3-(iY<<1));
			iX++;
			}
		else
			iD2+=iASquared*(3-(iY<<1));
		iY--;
		return(Output(aTopLeft,aTopRight,aBottomLeft,aBottomRight));
		}
	if(iStatus==ELine)
		{
		ret=Output(aTopLeft,aTopRight,aBottomLeft,aBottomRight);
		if(iA==0)
			iY--;
		else
			{
			iX++;
			if(iX>iA+iXAdj) ret=ETrue;
			else
				{
				iStatus=ELine;
				ret=EFalse;
				}
			}
		return(ret);
		}
	if(iStatus==EInitialised)
		{
		iStatus=EFirstSector;
		return(Output(aTopLeft,aTopRight,aBottomLeft,aBottomRight));
		}
	Output(aTopLeft,aTopRight,aBottomLeft,aBottomRight);
	return(ETrue);
	}

/**
Sets the absolute points that define the ellipse as calculated using its iOffset 
from the origin and using the half width and half height of the rectangle iA and iB.
@param aTopLeft The absolute (x,y) position for the top left point.
@param aTopRight The absolute (x,y) position for the top right point.
@param aBottomLeft The absolute (x,y) position for the bottom left point.
@param aBottomRight The absolute (x,y) position for the bottom right point.
@return TBool ETrue if a valid rectangle is produced, else EFalse. Also sets
iStatus to EComplete.
*/
EXPORT_C TBool TEllipse::Output(TPoint& aTopLeft,TPoint& aTopRight,
								TPoint& aBottomLeft,TPoint& aBottomRight)
	{
	TInt lx=iA-iX+iOffset.iX;
	TInt ty=iB-iY+iOffset.iY;
	TInt rx=iA+iX+iXAdj+iOffset.iX;
	TInt by=iB+iY+iYAdj+iOffset.iY;
	aTopLeft.SetXY(lx,ty);
	aTopRight.SetXY(rx,ty);
	aBottomLeft.SetXY(lx,by);
	aBottomRight.SetXY(rx,by);
	if(iY<=0)
		{
		iStatus=EComplete;
		if(iYAdj==0 || ty>by)
			return(ETrue);
		}
	return(EFalse);
	}

/**
By analysing the current state of the ellipse the process is taken to the next appropriate step.
If iStatus = EInitialised only one step will be taken, if the ellipse is already semi constructed then 
it will be taken to completion. Takes in four point parameters that defines the rectangle in order to pass to 
SingleStep(aTopLeft,aTopRight,aBottomLeft,aBottomRight).
@param aTopLeft Top left corner of rectangle 
@param aTopRight Top right corner of rectangle
@param aBottomLeft Bottom left corner of rectangle
@param aBottomRight Bottom right corner of rectangle
@return TBool ETrue if a valid rectangle is produced, else EFalse.
*/
EXPORT_C TBool TEllipse::NextStep(TPoint& aTopLeft,TPoint& aTopRight,
								  TPoint& aBottomLeft,TPoint& aBottomRight)
	{
	if(iStatus==EInitialised)
		return(SingleStep(aTopLeft,aTopRight,aBottomLeft,aBottomRight));
	TInt prevlev=iY;
	TBool ret;
	do
		ret=SingleStep(aTopLeft,aTopRight,aBottomLeft,aBottomRight);
	while(prevlev==iY && ret==EFalse);
	return(ret);
	}

/**
Constructs an ellipse from the rectangle which it is given and assesses the 
points position with regard to the ellipse and where they intersect. 
@param aRect The rectangle within which the ellipse is drawn.
@param aPoint A point to compare with the ellipse to determine if intersection occurs. 
@return TPoint The point is set to the corner which the intersection is nearest to.
*/
EXPORT_C TPoint TEllipse::Intersection(const TRect& aRect,const TPoint& aPoint)
	{
	Construct(aRect);					//constructs the rect (an elipse object)
	TPoint centre=aRect.Center();		//centre of ellipse
	TPoint ptcpy(aPoint);				
	ptcpy-=iOffset;						//ptcpy = aPoint - iOffset - TPoint(iA,iB)	//radius from centre of ellipse		
	ptcpy-=TPoint(iA,iB);				
	TPoint pt[4],opt[4];			
	TInt mpt[4],ompt[4];
	TInt count=0;
	for(;count<4;count++)
		ompt[count]=KMaxTInt;			//fills ompt 1->4 with KMaxTInt
	while(SingleStep(pt[0],pt[1],pt[2],pt[3])==EFalse) 	//creates a complete ellipse with pts as rect
		for(count=0;count<4;count++)
			{
			mpt[count]=Abs((pt[count].iY-iOffset.iY-iB)*(ptcpy.iX)-(ptcpy.iY)*(pt[count].iX-iOffset.iX-iA));
			if(mpt[count]<ompt[count]) //use the larger number set.
				{
				ompt[count]=mpt[count];
				opt[count]=pt[count];
				}						
			}
	if(pt[0].iY==pt[2].iY)	//if it is horizontal
		for(count=0;count<4;count++)
			{
			mpt[count]=Abs((pt[count].iY-iOffset.iY-iB)*(ptcpy.iX)-(ptcpy.iY)*(pt[count].iX-iOffset.iX-iA));
			if(mpt[count]<ompt[count]) //use the larger number set.
				{
				ompt[count]=mpt[count];
				opt[count]=pt[count];
				}
			}
	if(ptcpy.iX<0 && ptcpy.iY<0)	//if point is further left and higher than centre of rect
		return(opt[0]);
	if(ptcpy.iY<0)			//if point is higher than centre of rect
		return(opt[1]);
	if(ptcpy.iX<0)			//if point is further left than centre of rect
		return(opt[2]);
	if(aPoint.iX<centre.iX && aPoint.iY<centre.iY)	//if point is further left and higher than centre of rect
		return(opt[0]);
	if(aPoint.iY<centre.iY)	//if point is higher than centre of rect
		return(opt[1]);
	if(aPoint.iX<centre.iX)	//if point is further left than centre of rect
		return(opt[2]);
	return(opt[3]);			//else 
	}

//
// Ellipse drawing
//

/**
Draws and fills an ellipse.

The function provides a concrete implementation of the pure virtual
function <code>CGraphicsContext::DrawEllipse()</code>. The function
behaviour is the same as documented in that class.
*/
EXPORT_C void CFbsBitGc::DrawEllipse(const TRect& aRect)
	{
	if(CheckDevice(aRect)) return;
	TRect rcpy(aRect);
	rcpy.Move(iOrigin);
	iDevice->TruncateRect(rcpy);
	TRect clippedRect(rcpy);
	clippedRect.Grow((iPenSize.iWidth>>1)+1,(iPenSize.iHeight>>1)+1);
	if(UserClipRect(clippedRect)) return;
	SetupDevice();
	iDevice->DrawingBegin(&iBrushBitmap);
	CFbsRasterizer* brushRasterizer = PrepareRasterizerForExtendedBitmap(iBrushBitmap);
	if(iBrushStyle!=ENullBrush)
		EllipseFill(rcpy);
	if(iPenStyle!=ENullPen)
		{
		if(iPenSize.iWidth>1 && iPenSize.iHeight>1)
			EllipseOutlineWide(rcpy);
		else
			if(iPenSize.iWidth==1 || iPenSize.iHeight==1)
				EllipseOutline(rcpy);
		}
	if (brushRasterizer)
		{
		brushRasterizer->EndBitmap(iBrushBitmap.SerialNumber());
		}
	iDevice->DrawingEnd(&iBrushBitmap);
	}

void CFbsBitGc::EllipseOutline(const TRect& aRect)
	{
	CFbsDrawDevice* drawDevice = iDevice->iDrawDevice;
	TPoint tl,tr,bl,br;
	AddRect(aRect);
#if defined(_DEBUG)
	TRect deviceDestRect;
	drawDevice->GetDrawRect(deviceDestRect);
#endif
	for(TInt count=0;count<iDefaultRegionPtr->Count();count++)
		{
		iClipRect=(*iDefaultRegionPtr)[count];
		if(!iClipRect.Intersects(aRect))
			continue;
		iClipRect.Intersection(aRect);
		if(UserClipRect(iClipRect)) continue;
		BG_ASSERT_DEBUG(iClipRect.iTl.iX >= deviceDestRect.iTl.iX, EBitgdiPanicOutOfBounds);
		BG_ASSERT_DEBUG(iClipRect.iTl.iY >= deviceDestRect.iTl.iY, EBitgdiPanicOutOfBounds);
		BG_ASSERT_DEBUG(iClipRect.iBr.iX <= deviceDestRect.iBr.iX, EBitgdiPanicOutOfBounds);
		BG_ASSERT_DEBUG(iClipRect.iBr.iY <= deviceDestRect.iBr.iY, EBitgdiPanicOutOfBounds);
		TEllipse ellipse;
		ellipse.Construct(aRect);
		TInt pattern=0;
		while(!ellipse.SingleStep(tl,tr,bl,br))
			{
			if(iPenStyle==CGraphicsContext::ESolidPen || (iDotMask&(1<<(pattern%iDotLength))))
				{
				if(tl.iY>=iClipRect.iTl.iY && tl.iY<iClipRect.iBr.iY)
					{
					if(tl.iX>=iClipRect.iTl.iX && tl.iX<iClipRect.iBr.iX)
						drawDevice->WriteRgb(tl.iX,tl.iY,iPenColor,iDrawMode);
					if(tr.iX>=iClipRect.iTl.iX && tr.iX<iClipRect.iBr.iX && tl.iX!=tr.iX)
						drawDevice->WriteRgb(tr.iX,tr.iY,iPenColor,iDrawMode);
					}
				if(bl.iY>=iClipRect.iTl.iY && bl.iY<iClipRect.iBr.iY)
					{
					if(bl.iX>=iClipRect.iTl.iX && bl.iX<iClipRect.iBr.iX)
						drawDevice->WriteRgb(bl.iX,bl.iY,iPenColor,iDrawMode);
					if(br.iX>=iClipRect.iTl.iX && br.iX<iClipRect.iBr.iX && bl.iX!=br.iX)
						drawDevice->WriteRgb(br.iX,br.iY,iPenColor,iDrawMode);
					}
				}
			pattern++;
			}
		if(tl.iY==bl.iY && tl.iY>=iClipRect.iTl.iY && tl.iY<iClipRect.iBr.iY)
			{
			if(tl.iX>=iClipRect.iTl.iX && tl.iX<iClipRect.iBr.iX)
				drawDevice->WriteRgb(tl.iX,tl.iY,iPenColor,iDrawMode);
			if(tr.iX>=iClipRect.iTl.iX && tr.iX<iClipRect.iBr.iX && tl.iX!=tr.iX)
				drawDevice->WriteRgb(tr.iX,tr.iY,iPenColor,iDrawMode);
			}
		drawDevice->UpdateRegion(iClipRect);
		}
	}

void CFbsBitGc::EllipseOutlineWide(const TRect& aRect)
	{
	TRect rcpy(aRect);
	TPoint tl,tr,bl,br;
	TInt halfpenwidth=(iPenSize.iWidth+1)>>1;
	TInt halfpenheight=(iPenSize.iHeight+1)>>1;
	rcpy.Grow(halfpenwidth,halfpenheight);
	AddRect(rcpy);
	TInt dp=iDotParam;
	for(TInt count=0;count<iDefaultRegionPtr->Count();count++)
		{
		iClipRect=(*iDefaultRegionPtr)[count];
		if(!iClipRect.Intersects(rcpy))
			continue;
		iClipRect.Intersection(rcpy);
		if(UserClipRect(iClipRect)) continue;
		TEllipse ellipse;
		ellipse.Construct(aRect);
		iDotParam=Max(iPenSize.iWidth>>1,iPenSize.iHeight>>1);
		while(!ellipse.SingleStep(tl,tr,bl,br))
			{
			PenDrawClipped(tl);
			PenDrawClipped(tr);
			PenDrawClipped(bl);
			PenDrawClipped(br);
			iDotParam+=iDotDirection;
			}
		if(tl.iY==bl.iY)
			{
			PenDrawClipped(tl);
			PenDrawClipped(tr);
			}
		iDevice->iDrawDevice->UpdateRegion(iClipRect);
		}
	iDotParam=dp;
	}

// if iBrushBitmap is an extended bitmap, PrepareRasterizerForExtendedBitmap() must have been called before this method
void CFbsBitGc::EllipseFill(const TRect& aRect)
	{
	TRect rcpy(aRect);
	if(iPenSize.iWidth==0 || iPenSize.iHeight==0)
		rcpy.Grow(1,1);
	AddRect(aRect);
	TPoint tl,tr,bl,br;
	for(TInt count=0;count<iDefaultRegionPtr->Count();count++)
		{
		iClipRect=(*iDefaultRegionPtr)[count];
		if(!iClipRect.Intersects(rcpy))
			continue;
		iClipRect.Intersection(rcpy);
		if(UserClipRect(iClipRect)) continue;
		TEllipse ellipse;
		ellipse.Construct(rcpy);
		while(!ellipse.NextStep(tl,tr,bl,br))
			{
			tl.iX++;
			tr.iX--;
			bl.iX++;
			br.iX--;
			ClipFillLine(tl,tr);
			ClipFillLine(bl,br);
			}
		if(tl.iY==bl.iY)
			{
			tl.iX++;
			tr.iX--;
			ClipFillLine(tl,tr);
			}
		iDevice->iDrawDevice->UpdateRegion(iClipRect);
		}
	}

