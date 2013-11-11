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

/** 
@see MDirectGdiEngine::CopyRect()
*/
void CSwDirectGdiEngine::CopyRect(const TPoint& aOffset, const TRect& aRect)
	{	
	TRect deviceRect;
	iDrawDevice->GetDrawRect(deviceRect);
	const TPoint back(TPoint(0,0) - aOffset);
	TRect rcpy(aRect);
	rcpy.Move(iOrigin);
	rcpy.Intersection(deviceRect);
	((TRegion*)iDefaultRegionPtr)->Sort(aOffset);
	
	TRect targetRect(rcpy);
	targetRect.Move(aOffset);
	targetRect.BoundingRect(rcpy);
	
	TRect clipRect(0,0,0,0);
	const TInt limit = iDefaultRegionPtr->Count();
	for(TInt count = 0; count < limit; count++)
		{
		clipRect = (*iDefaultRegionPtr)[count];
		
		clipRect.Move(back);
		if (!clipRect.Intersects(rcpy))
			{
			continue;
			}
		
		clipRect.Intersection(rcpy);
		DirectGdi::TDrawMode drawMode = iDrawMode;
		iDrawMode = DirectGdi::EDrawModeWriteAlpha;
		DoCopyRect(aOffset, clipRect);
		iDrawMode = drawMode; // restore the previous draw mode
		clipRect.Move(aOffset);
		iDrawDevice->UpdateRegion(clipRect);
		}
	}

/**
@see CopyRect()
@panic DGDIAdapter 1013, if aRect or aOffset are outside of the destination bounds (debug only).
 */ 
void CSwDirectGdiEngine::DoCopyRect(const TPoint& aOffset, const TRect& aRect)
	{
#ifdef _DEBUG
	TRect deviceRect;
	iDrawDevice->GetDrawRect(deviceRect);
#endif
	GRAPHICS_ASSERT_DEBUG(aRect.iTl.iX >= deviceRect.iTl.iX, EDirectGdiPanicOutOfBounds);
	GRAPHICS_ASSERT_DEBUG(aRect.iTl.iY >= deviceRect.iTl.iY, EDirectGdiPanicOutOfBounds);
	GRAPHICS_ASSERT_DEBUG(aRect.iBr.iX <= deviceRect.iBr.iX, EDirectGdiPanicOutOfBounds);
	GRAPHICS_ASSERT_DEBUG(aRect.iBr.iY <= deviceRect.iBr.iY, EDirectGdiPanicOutOfBounds);

	TRect offsetRect(aRect);
	offsetRect.Move(aOffset);

	GRAPHICS_ASSERT_DEBUG(offsetRect.iTl.iX >= deviceRect.iTl.iX, EDirectGdiPanicOutOfBounds);
	GRAPHICS_ASSERT_DEBUG(offsetRect.iTl.iY >= deviceRect.iTl.iY, EDirectGdiPanicOutOfBounds);
	GRAPHICS_ASSERT_DEBUG(offsetRect.iBr.iX <= deviceRect.iBr.iX, EDirectGdiPanicOutOfBounds);
	GRAPHICS_ASSERT_DEBUG(offsetRect.iBr.iY <= deviceRect.iBr.iY, EDirectGdiPanicOutOfBounds);

	TInt y1 = aRect.iTl.iY;
	TInt y2 = aRect.iBr.iY; 
	TInt yinc = 1; // default y2>y1
	if (aOffset.iY > 0)	// y1>y2
		{
		y1 = aRect.iBr.iY - 1;
		y2 = aRect.iTl.iY - 1;
		yinc = -1;
		}

	const TInt width = aRect.Width();
	const TInt xoffset = aRect.iTl.iX + aOffset.iX;
	const TDisplayMode dispMode = ScanLineBufferDisplayMode(iDrawDevice);
	TUint32* scanLineBuffer = iDrawDevice->ScanLineBuffer();

	for (TInt row = y1; row != y2; row += yinc)
		{
		iDrawDevice->ReadLine(aRect.iTl.iX, row, width, scanLineBuffer, dispMode);
		iDrawDevice->WriteLine(xoffset, row + aOffset.iY, width, scanLineBuffer, GcDrawMode(iDrawMode));
		}
	}

/** 
@see MDirectGdiEngine::DrawRect()
*/
void CSwDirectGdiEngine::DrawRect(const TRect& aRect)
	{
	TRect rcpy(aRect);

	iBrushBitmap.BeginDataAccess();
	
	if ((iPenSize.iWidth == 1) && (iPenSize.iHeight == 1) && iPenStyle != DirectGdi::ENullPen)
		{
		const TInt width = rcpy.Width();
		const TInt height = rcpy.Height();
		const TPoint currentLinePosition = iLinePosition;

		if (iPenStyle != DirectGdi::ESolidPen)
			{
			iDotParam = 0;
			}

		DoDrawLine(rcpy.iTl,TPoint(rcpy.iBr.iX,rcpy.iTl.iY),ETrue); // top

		if (height > 2 && width > 1)
			{
			DoDrawLine(TPoint(rcpy.iBr.iX-1,rcpy.iTl.iY+1),TPoint(rcpy.iBr.iX-1,rcpy.iBr.iY-1),ETrue); // right
			}

		if (height > 1)
			{
			DoDrawLine(TPoint(rcpy.iBr.iX-1,rcpy.iBr.iY-1),TPoint(rcpy.iTl.iX-1,rcpy.iBr.iY-1),ETrue); // bottom
			}

		if (height > 2)
			{
			DoDrawLine(TPoint(rcpy.iTl.iX,rcpy.iBr.iY-2),rcpy.iTl,ETrue); // left
			}
		
		// Restore internal line position in case it has been modified by DoDrawLine().
		// DrawRect() should not be modifying it.
		iLinePosition = currentLinePosition;

		if (width < 3 || height < 3)
			{
			goto nofill;
			}

		rcpy.Shrink(1,1);
		rcpy.Move(iOrigin);
		}
	else if ((iPenStyle != DirectGdi::ENullPen) && (iPenSize.iWidth >= 1 && iPenSize.iHeight >= 1))
		{
		rcpy.Move(iOrigin);

		const DirectGdi::TBrushStyle tempbrushstyle = iBrushStyle;
		const TRgb tempbrushColor = iBrushColor;
		const DirectGdi::TDrawMode tempdrawmode = iDrawMode;
				
		const TInt halfpenwidth = (iPenSize.iWidth - 1) >> 1;
		const TInt halfpenheight = (iPenSize.iHeight - 1) >> 1;
		const TInt otherhalfwidth = (iPenSize.iWidth >> 1) + 1;
		const TInt otherhalfheight = (iPenSize.iHeight >> 1) + 1;

		rcpy.iBr.iX--;
		rcpy.iBr.iY--;

		if (((rcpy.iBr.iY - rcpy.iTl.iY) <= (iPenSize.iHeight + 1)) || ((rcpy.iBr.iX - rcpy.iTl.iX) <= (iPenSize.iWidth + 1)))
			{
			iBrushColor = iPenColor;
			iBrushStyle = DirectGdi::ESolidBrush;
			if(iDrawMode != DirectGdi::EDrawModeWriteAlpha)
				{			
				iDrawMode = DirectGdi::EDrawModePEN;
				}
						
			rcpy.iTl.iX -= halfpenwidth;
			rcpy.iTl.iY -= halfpenheight;
			rcpy.iBr.iX += otherhalfwidth;
			rcpy.iBr.iY += otherhalfheight;			
			RectFill(rcpy);
			}
		else
			{
			// In the event the pen outline is semi-transparent, we must do the fill first so that the
			// outline is blended over the top.
			RectFill(rcpy);
			
			iBrushColor = iPenColor;
			iBrushStyle = DirectGdi::ESolidBrush;
			if(iDrawMode != DirectGdi::EDrawModeWriteAlpha)
				{			
				iDrawMode = DirectGdi::EDrawModePEN;
				}
			RectFill(TRect(rcpy.iTl.iX - halfpenwidth,rcpy.iTl.iY - halfpenheight,rcpy.iBr.iX + otherhalfwidth,rcpy.iTl.iY + otherhalfheight)); // top
			RectFill(TRect(rcpy.iTl.iX - halfpenwidth,rcpy.iTl.iY + otherhalfheight,rcpy.iTl.iX + otherhalfwidth,rcpy.iBr.iY - halfpenheight)); // left
			RectFill(TRect(rcpy.iBr.iX - halfpenwidth,rcpy.iTl.iY + otherhalfheight,rcpy.iBr.iX + otherhalfwidth,rcpy.iBr.iY - halfpenheight)); // right
			RectFill(TRect(rcpy.iTl.iX - halfpenwidth,rcpy.iBr.iY - halfpenheight,rcpy.iBr.iX + otherhalfwidth,rcpy.iBr.iY + otherhalfheight)); // bottom
			}
		
		iBrushStyle = tempbrushstyle;
		iBrushColor = tempbrushColor;
		iDrawMode = tempdrawmode;
		goto nofill;
		}
	else
		{
		rcpy.Move(iOrigin);
		}

	RectFill(rcpy);

nofill:
	iBrushBitmap.EndDataAccess(ETrue);
	}

/**
Fills the given area using the current brush.

@param aRect The area to fill.
@panic DGDIAdapter 1013, if the a clipping rectangle is fully outside of the destination bounds (debug only).
@panic DGDIAdapter 7, if using EPatternedBrush with no brush pattern bitmap.
 */ 
void CSwDirectGdiEngine::RectFill(const TRect& aRect)
	{	
	if (aRect.IsEmpty() || iBrushStyle == DirectGdi::ENullBrush)
		{
		return;
		}
	
	const TPoint origin = iOrigin + iBrushOrigin;
	const TInt limit = iDefaultRegionPtr->Count();
	TRect clipRect(0,0,0,0);
	for (TInt count = 0; count < limit; count++)
		{
		clipRect = (*iDefaultRegionPtr)[count];
		if (!clipRect.Intersects(aRect))
			{
			continue;
			}
		
		clipRect.Intersection(aRect);	
		
#ifdef _DEBUG
		TRect deviceRect;
		iDrawDevice->GetDrawRect(deviceRect);
		GRAPHICS_ASSERT_DEBUG(clipRect.iTl.iX >= deviceRect.iTl.iX, EDirectGdiPanicOutOfBounds);
		GRAPHICS_ASSERT_DEBUG(clipRect.iTl.iY >= deviceRect.iTl.iY, EDirectGdiPanicOutOfBounds);
		GRAPHICS_ASSERT_DEBUG(clipRect.iBr.iX <= deviceRect.iBr.iX, EDirectGdiPanicOutOfBounds);
		GRAPHICS_ASSERT_DEBUG(clipRect.iBr.iY <= deviceRect.iBr.iY, EDirectGdiPanicOutOfBounds);
#endif

		TInt xcoord = clipRect.iTl.iX;
		TInt ycoord = clipRect.iTl.iY;
		const CGraphicsContext::TDrawMode drawMode = GcDrawMode(iDrawMode);
		
		switch(iBrushStyle)
		{
		case DirectGdi::ESolidBrush:
			iDrawDevice->WriteRgbMulti(clipRect.iTl.iX,clipRect.iTl.iY,
							clipRect.Width(),clipRect.Height(),iBrushColor,drawMode);
			break;
		case DirectGdi::EPatternedBrush:
			{
			iBrushBitmap.BeginDataAccess();
			CBitwiseBitmap* brushbitmap = iBrushBitmap.Address();
			
			GRAPHICS_ASSERT_ALWAYS(brushbitmap != NULL, EDirectGdiPanicInvalidBitmap);
			
			TRect sourcerect(clipRect);
			sourcerect.Move(-origin);
						
			DoBitBlt(clipRect.iTl,brushbitmap,iBrushBitmap.DataAddress(),iBrushBitmap.DataStride(),sourcerect);
			iBrushBitmap.EndDataAccess(ETrue);
			break;
			}
		case DirectGdi::EHorizontalHatchBrush:
			iDrawDevice->WriteRgbMulti(clipRect.iTl.iX,clipRect.iTl.iY,
					clipRect.Width(),clipRect.Height(),iBrushColor,drawMode);
			while (Abs((ycoord - origin.iY) % 3) != 2)
				{
				ycoord++;
				}
			for (; ycoord < clipRect.iBr.iY; ycoord += 3)
				{
				iDrawDevice->WriteRgbMulti(clipRect.iTl.iX,ycoord,clipRect.Width(),1,iPenColor,drawMode);
				}
				break;
			case DirectGdi::EVerticalHatchBrush:
				iDrawDevice->WriteRgbMulti(clipRect.iTl.iX,clipRect.iTl.iY,
						clipRect.Width(),clipRect.Height(),iBrushColor,drawMode);
				while (Abs((xcoord - origin.iX) % 3) != 2)
					{
					xcoord++;
					}
				for (; xcoord < clipRect.iBr.iX; xcoord += 3)
					{
					iDrawDevice->WriteRgbMulti(xcoord,clipRect.iTl.iY,1,clipRect.Height(),iPenColor,drawMode);
					}
				break;
			case DirectGdi::ESquareCrossHatchBrush:
				iDrawDevice->WriteRgbMulti(clipRect.iTl.iX,clipRect.iTl.iY,
						clipRect.Width(),clipRect.Height(),iBrushColor,drawMode);
				while (Abs((ycoord - origin.iY) % 3) != 2)
					{
					ycoord++;
					}
				for (; ycoord < clipRect.iBr.iY; ycoord += 3) // horizontal lines
					{
					iDrawDevice->WriteRgbMulti(clipRect.iTl.iX,ycoord,clipRect.Width(),1,iPenColor,drawMode);
					}

				ycoord = clipRect.iTl.iY;
				while (Abs((ycoord - origin.iY) % 3) != 2 && ycoord < clipRect.iBr.iY) // above the top horizontal line
					{
					xcoord = clipRect.iTl.iX;
					while (Abs((xcoord - origin.iX) % 3) != 2)
						{
						xcoord++;
						}
					for (; xcoord < clipRect.iBr.iX; xcoord += 3)
						{
						iDrawDevice->WriteRgb(xcoord,ycoord,iPenColor,drawMode);
						}
					ycoord++;
					}
				ycoord += 3;
				for (; ycoord < clipRect.iBr.iY; ycoord += 3) // between the top and bottom horizontals
					{
					xcoord = clipRect.iTl.iX;
					while (Abs((xcoord - origin.iX) % 3) != 2)
						{
						xcoord++;
						}
					for (; xcoord < clipRect.iBr.iX; xcoord += 3)
						{
						iDrawDevice->WriteRgbMulti(xcoord,ycoord - 2,1,2,iPenColor,drawMode);
						}
					}

				ycoord -= 3;
				while (ycoord < clipRect.iBr.iY) // below the bottom horizontal
					{
					xcoord = clipRect.iTl.iX;
					while (Abs((xcoord - origin.iX) % 3) != 2)
						{
						xcoord++;
						}
					for (; xcoord < clipRect.iBr.iX; xcoord += 3)
						{
						iDrawDevice->WriteRgb(xcoord,ycoord,iPenColor,drawMode);
						}
					ycoord++;
					}
				break;
			case DirectGdi::EForwardDiagonalHatchBrush:
				iDrawDevice->WriteRgbMulti(clipRect.iTl.iX,clipRect.iTl.iY,
						clipRect.Width(),clipRect.Height(),iBrushColor,drawMode);
				for (; ycoord < clipRect.iBr.iY; ycoord++)
					{
					xcoord = clipRect.iTl.iX;
					TInt diff = (origin.iX + origin.iY - xcoord - ycoord) % 3;
					if (diff < 0)
						{
						diff += 3;
						}
					xcoord += diff;
					for (; xcoord < clipRect.iBr.iX; xcoord += 3)
						{
						iDrawDevice->WriteRgb(xcoord,ycoord,iPenColor,drawMode);
						}
					}
				break;
			case DirectGdi::ERearwardDiagonalHatchBrush:
				iDrawDevice->WriteRgbMulti(clipRect.iTl.iX,clipRect.iTl.iY,
						clipRect.Width(),clipRect.Height(),iBrushColor,drawMode);
				for (; ycoord < clipRect.iBr.iY; ycoord++)
					{
					xcoord = clipRect.iTl.iX;
					TInt diff = (origin.iX - origin.iY - xcoord + ycoord) % 3;
					if (diff < 0)
						{
						diff += 3;
						}
					xcoord += diff;
					for (; xcoord < clipRect.iBr.iX; xcoord += 3)
						{
						iDrawDevice->WriteRgb(xcoord,ycoord,iPenColor,drawMode);
						}
					}
				break;
			case DirectGdi::EDiamondCrossHatchBrush:
				{
				iDrawDevice->WriteRgbMulti(clipRect.iTl.iX,clipRect.iTl.iY,
						clipRect.Width(),clipRect.Height(),iBrushColor,drawMode);
				TInt sum = xcoord + ycoord - origin.iX - origin.iY;
				for (; ycoord < clipRect.iBr.iY; ycoord++,sum++)
					{
					TInt currentsum = sum;
					for (xcoord = clipRect.iTl.iX; xcoord < clipRect.iBr.iX; xcoord++,currentsum++)
						{
						if((currentsum & 1) == 0 && ((currentsum & 3) != 0 || ((xcoord-origin.iX) & 1) == 1))
							{
							iDrawDevice->WriteRgb(xcoord,ycoord,iPenColor,drawMode);
							}
						}
					}
				break;
				}
			default:
				return;
				}

			iDrawDevice->UpdateRegion(clipRect);
			}		
	}
