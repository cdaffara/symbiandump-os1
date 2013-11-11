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
#include <graphicsaccelerator.h>
#include <bitdraw.h>
#include <graphics/fbsrasterizer.h>

/**
Copies a rectangle.

The function provides a concrete implementation of the pure virtual
function <code>CBitmapContext::CopyRect()</code>. The function
behaviour is the same as documented in that class.
*/
EXPORT_C void CFbsBitGc::CopyRect(const TPoint& aOffset,const TRect& aRect)
	{
	if(CheckDevice(aRect) || aRect.IsEmpty() || aOffset == TPoint(0,0))
		return;

	CFbsDrawDevice* drawDevice = iDevice->iDrawDevice;

	TRect deviceRect;
	drawDevice->GetDrawRect(deviceRect);
	const TPoint back(TPoint(0,0)-aOffset);
	TRect rcpy(aRect);
	rcpy.Move(iOrigin);
	rcpy.Intersection(deviceRect);
	((TRegion*)iDefaultRegionPtr)->Sort(aOffset);

	TRect clippedBoundingRect(rcpy);
	clippedBoundingRect.Move(aOffset);
	AddRect(clippedBoundingRect);
	clippedBoundingRect.BoundingRect(rcpy);
	if(!clippedBoundingRect.Intersects(iUserClipRect))
		return;

	SetupDevice();
	iDevice->DrawingBegin();

	const TInt limit=iDefaultRegionPtr->Count();
	for(TInt count=0;count<limit;count++)
		{
		iClipRect=(*iDefaultRegionPtr)[count];
		if(UserClipRect(iClipRect)) 
			continue;
		iClipRect.Move(back);
		if(!iClipRect.Intersects(rcpy))
			continue;

		iClipRect.Intersection(rcpy);
		TDrawMode drawMode = iDrawMode;
		iDrawMode = EDrawModeWriteAlpha;
		DoCopyRect(aOffset,iClipRect);
		iDrawMode = drawMode; // restore the previous draw mode
		iClipRect.Move(aOffset);
		drawDevice->UpdateRegion(iClipRect);
		}

	iDevice->DrawingEnd();
	}

void CFbsBitGc::DoCopyRect(const TPoint& aOffset,const TRect& rect)
	{
	CFbsDrawDevice* drawDevice = iDevice->iDrawDevice;
#ifdef _DEBUG
	TRect deviceRect;
	drawDevice->GetDrawRect(deviceRect);
#endif
	BG_ASSERT_DEBUG(rect.iTl.iX >= deviceRect.iTl.iX, EBitgdiPanicOutOfBounds);
	BG_ASSERT_DEBUG(rect.iTl.iY >= deviceRect.iTl.iY, EBitgdiPanicOutOfBounds);
	BG_ASSERT_DEBUG(rect.iBr.iX <= deviceRect.iBr.iX, EBitgdiPanicOutOfBounds);
	BG_ASSERT_DEBUG(rect.iBr.iY <= deviceRect.iBr.iY, EBitgdiPanicOutOfBounds);

	TRect offrect(rect);
	offrect.Move(aOffset);

	BG_ASSERT_DEBUG(offrect.iTl.iX >= deviceRect.iTl.iX, EBitgdiPanicOutOfBounds);
	BG_ASSERT_DEBUG(offrect.iTl.iY >= deviceRect.iTl.iY, EBitgdiPanicOutOfBounds);
	BG_ASSERT_DEBUG(offrect.iBr.iX <= deviceRect.iBr.iX, EBitgdiPanicOutOfBounds);
	BG_ASSERT_DEBUG(offrect.iBr.iY <= deviceRect.iBr.iY, EBitgdiPanicOutOfBounds);

	TInt y1 = rect.iTl.iY,y2 = rect.iBr.iY,yinc = 1; // default y2>y1
	if (aOffset.iY > 0)	// y1>y2
		{
		y1 = rect.iBr.iY - 1;
		y2 = rect.iTl.iY - 1;
		yinc = -1;
		}

	const TInt width = rect.Width();
	const TInt xoffset = rect.iTl.iX + aOffset.iX;
	const TDisplayMode dispMode = ScanLineBufferDisplayMode(drawDevice);
	TUint32* scanLineBuffer = drawDevice->ScanLineBuffer();

	for (TInt row = y1; row != y2; row += yinc)
		{
		drawDevice->ReadLine(rect.iTl.iX,row,width,scanLineBuffer,dispMode);
		drawDevice->WriteLine(xoffset,row + aOffset.iY,width, scanLineBuffer,iDrawMode);
		}
	}

/**
Draws and fills a rectangle.

The function provides a concrete implementation of the pure virtual
function <code>CGraphicsContext::DrawRect()</code>. The function
behaviour is the same as documented in that class.
*/
EXPORT_C void CFbsBitGc::DrawRect(const TRect& aRect)
	{
	if (CheckDevice(aRect))
		return;

	TRect rcpy(aRect);
	TRect clippedBoundingRect(rcpy);
	clippedBoundingRect.Move(iOrigin);
	clippedBoundingRect.Grow((iPenSize.iWidth >> 1) + 1,(iPenSize.iHeight >> 1) + 1);
	if(!clippedBoundingRect.Intersects(iUserClipRect))
		return;

	CGraphicsAccelerator* ga = GraphicsAccelerator();

	SetupDevice();
	iDevice->DrawingBegin();
	iBrushBitmap.BeginDataAccess();
	CFbsRasterizer* brushRasterizer = PrepareRasterizerForExtendedBitmap(iBrushBitmap);

	if (iPenSize.iWidth == 1 && iPenSize.iHeight == 1 && iPenStyle != ENullPen)
		{
		const TInt width = rcpy.Width();
		const TInt height = rcpy.Height();
		const TPoint currentLinePosition = iLinePosition;

		if (iPenStyle != ESolidPen)
			iDotParam = 0;

		if (width)
			DoDrawLine(rcpy.iTl,TPoint(rcpy.iBr.iX,rcpy.iTl.iY),ETrue); // top

		if (height > 2 && width > 1)
			DoDrawLine(TPoint(rcpy.iBr.iX-1,rcpy.iTl.iY+1),TPoint(rcpy.iBr.iX-1,rcpy.iBr.iY-1),ETrue); // right

		if (width && height > 1)
			DoDrawLine(TPoint(rcpy.iBr.iX-1,rcpy.iBr.iY-1),TPoint(rcpy.iTl.iX-1,rcpy.iBr.iY-1),ETrue); // bottom

		if (height > 2)
			DoDrawLine(TPoint(rcpy.iTl.iX,rcpy.iBr.iY-2),rcpy.iTl,ETrue); // left
		
		// Restore internal line position in case it has been modified by DoDrawLine().
		// DrawRect() should not be modifying it.
		iLinePosition = currentLinePosition;

		if (width < 3 || height < 3)
			goto nofill;

		rcpy.Shrink(1,1);
		rcpy.Move(iOrigin);
		}
	else if (iPenStyle != ENullPen && (iPenSize.iWidth >= 1 && iPenSize.iHeight >= 1))
		{
		rcpy.Move(iOrigin);

		const TBrushStyle tempbrushstyle = iBrushStyle;
		iBrushStyle = ESolidBrush;
		const TRgb tempbrushColor = iBrushColor;
		iBrushColor = iPenColor;
		const CGraphicsContext::TDrawMode tempdrawmode = iDrawMode;

		if(iDrawMode != CGraphicsContext::EDrawModeWriteAlpha)
			{			
			iDrawMode = CGraphicsContext::EDrawModePEN;
			}

		const TInt halfpenwidth = (iPenSize.iWidth - 1) >> 1;
		const TInt halfpenheight = (iPenSize.iHeight - 1) >> 1;
		const TInt otherhalfwidth = (iPenSize.iWidth >> 1) + 1;
		const TInt otherhalfheight = (iPenSize.iHeight >> 1) + 1;

		rcpy.iBr.iX--;
		rcpy.iBr.iY--;

		if (rcpy.iBr.iY - rcpy.iTl.iY <= iPenSize.iHeight + 1 || rcpy.iBr.iX - rcpy.iTl.iX <= iPenSize.iWidth + 1)
			{
			rcpy.iTl.iX -= halfpenwidth;
			rcpy.iTl.iY -= halfpenheight;
			rcpy.iBr.iX += otherhalfwidth;
			rcpy.iBr.iY += otherhalfheight;

			RectFill(rcpy);

			iBrushStyle = tempbrushstyle;
			iBrushColor = tempbrushColor;
			iDrawMode = tempdrawmode;

			goto nofill;
			}
		else
			{
			RectFill(TRect(rcpy.iTl.iX - halfpenwidth,rcpy.iTl.iY - halfpenheight,rcpy.iBr.iX + otherhalfwidth,rcpy.iTl.iY + otherhalfheight)); // top
			RectFill(TRect(rcpy.iTl.iX - halfpenwidth,rcpy.iTl.iY + otherhalfheight,rcpy.iTl.iX + otherhalfwidth,rcpy.iBr.iY - halfpenheight)); // left
			RectFill(TRect(rcpy.iBr.iX - halfpenwidth,rcpy.iTl.iY + otherhalfheight,rcpy.iBr.iX + otherhalfwidth,rcpy.iBr.iY - halfpenheight)); // right
			RectFill(TRect(rcpy.iTl.iX - halfpenwidth,rcpy.iBr.iY - halfpenheight,rcpy.iBr.iX + otherhalfwidth,rcpy.iBr.iY + otherhalfheight)); // bottom

			rcpy.iTl.iX += otherhalfwidth;
			rcpy.iTl.iY += otherhalfheight;
			rcpy.iBr.iX -= halfpenwidth;
			rcpy.iBr.iY -= halfpenheight;

			iBrushStyle = tempbrushstyle;
			iBrushColor = tempbrushColor;
			iDrawMode = tempdrawmode;
			}
		}
	else
		rcpy.Move(iOrigin);

	//use Graphics accelerator if available
	if(ga)
		{
		TInt i = -1;
		//Draw rect
		if(iPenStyle == ENullPen && iShadowMode == CFbsDrawDevice::ENoShadow)
			{
			if(iBrushStyle == ESolidBrush )
				{
				// EDrawModePEN and EDrawModeWriteAlpha mode should use the same method when
				// (1) solid brush with opaque color is used. Or
				// (2) solid brush with transparent color is used but display mode is
				//     other than EColor64K, EColor16MU, EColor16MA, EColor16MAP.
				//  in the same way as the software implemantation does and calls WriteRgbMulti method.
				if(iDrawMode == EDrawModeWriteAlpha)
					{
					i = 0;
					}
				else if(iDrawMode == EDrawModePEN)
					{
					if(iBrushColor.Alpha() == 255)
						{
						i = 0;
						}
					else
						{
						TDisplayMode dispMode = iDevice->DisplayMode();
						if(dispMode != EColor64K && dispMode != EColor16MU && dispMode != EColor16MA && dispMode != EColor16MAP)
							{
							i = 0;
							}
						}
					}
				//Invert color
				else if(iDrawMode == EDrawModeNOTSCREEN)
					{
					i = 1;
					}
				}
			//use a patter brush
			else if(iBrushStyle == EPatternedBrush)
				{
				i = 2;
				}
			}

		if(i != -1)
			{
	        TInt gaOperationResult = KErrUnknown;
            iDevice->DrawingEnd();

			const TInt limit=iDefaultRegionPtr->Count();
			for (TInt count = 0; count < limit; count++)
				{
				iClipRect = (*iDefaultRegionPtr)[count];
				if (!iClipRect.Intersects(rcpy))
					continue;

				iClipRect.Intersection(rcpy);
				if (UserClipRect(iClipRect))
					continue;

				switch(i)
					{
				case 0:
					gaOperationResult = ga->Operation(TGopFilledRect(iClipRect,iBrushColor));
					break;
				case 1:
					gaOperationResult = ga->Operation(TGopInvertRect(iClipRect));
					break;
				case 2:
					CFbsBitmap* brushbitmap = (CFbsBitmap*)&iBrushBitmap;
					BG_ASSERT_ALWAYS(iBrushUsed,EBitgdiPanicInvalidBitmap);
					BG_ASSERT_ALWAYS(brushbitmap != NULL,EBitgdiPanicInvalidBitmap);

					TAcceleratedBitmapSpec brushBitmapSpec(brushbitmap);
					TGopFillPattern gopFillPattern;
					gopFillPattern.iBitmap = brushBitmapSpec;
					gopFillPattern.iOrigin = iBrushOrigin;
					gaOperationResult = ga->Operation(TGopFilledRectWithPattern(iClipRect,gopFillPattern));
					break;
					}			
				if(gaOperationResult != KErrNone)
					break;
				}
			if(gaOperationResult == KErrNone)
				goto finish;
			iDevice->DrawingBegin();
			}
		}

	RectFill(rcpy);

nofill:
	iDevice->DrawingEnd();
finish:
	if (brushRasterizer)
		{
		brushRasterizer->EndBitmap(iBrushBitmap.SerialNumber());
		}
	iBrushBitmap.EndDataAccess(ETrue);
	}

// if iBrushBitmap is an extended bitmap, PrepareRasterizerForExtendedBitmap() must have been called before this method
void CFbsBitGc::RectFill(const TRect& aRect)
	{
	if (aRect.IsEmpty() || iBrushStyle == ENullBrush)
		return;

	CFbsDrawDevice* drawDevice = iDevice->iDrawDevice;

	AddRect(aRect);

	const TPoint origin = iOrigin + iBrushOrigin;

	const TInt limit=iDefaultRegionPtr->Count();
	for (TInt count = 0; count < limit; count++)
		{
		iClipRect = (*iDefaultRegionPtr)[count];
		if (!iClipRect.Intersects(aRect))
			continue;

		iClipRect.Intersection(aRect);
		if (UserClipRect(iClipRect))
			continue;

#ifdef _DEBUG
		TRect deviceRect;
		drawDevice->GetDrawRect(deviceRect);
#endif
		BG_ASSERT_DEBUG(iClipRect.iTl.iX >= deviceRect.iTl.iX, EBitgdiPanicOutOfBounds);
		BG_ASSERT_DEBUG(iClipRect.iTl.iY >= deviceRect.iTl.iY, EBitgdiPanicOutOfBounds);
		BG_ASSERT_DEBUG(iClipRect.iBr.iX <= deviceRect.iBr.iX, EBitgdiPanicOutOfBounds);
		BG_ASSERT_DEBUG(iClipRect.iBr.iY <= deviceRect.iBr.iY, EBitgdiPanicOutOfBounds);

		TInt xcoord = iClipRect.iTl.iX;
		TInt ycoord = iClipRect.iTl.iY;


		switch(iBrushStyle)
			{
		case ESolidBrush:
			drawDevice->WriteRgbMulti(iClipRect.iTl.iX,iClipRect.iTl.iY,
					iClipRect.Width(),iClipRect.Height(),iBrushColor,iDrawMode);
			break;
		case EPatternedBrush:
			{
			CBitwiseBitmap* brushbitmap = iBrushBitmap.Address();
			BG_ASSERT_ALWAYS(iBrushUsed,EBitgdiPanicInvalidBitmap);
			BG_ASSERT_ALWAYS(brushbitmap != NULL,EBitgdiPanicInvalidBitmap);

			TRect sourcerect(iClipRect);
			sourcerect.Move(-origin);
			DoBitBlt(iClipRect.iTl,brushbitmap,iBrushBitmap.DataAddress(),iBrushBitmap.DataStride(),sourcerect);
			break;
			}
		case EHorizontalHatchBrush:
			drawDevice->WriteRgbMulti(iClipRect.iTl.iX,iClipRect.iTl.iY,
					iClipRect.Width(),iClipRect.Height(),iBrushColor,iDrawMode);
			while (Abs((ycoord - origin.iY) % 3) != 2)
				ycoord++;
			for (; ycoord < iClipRect.iBr.iY; ycoord += 3)
				drawDevice->WriteRgbMulti(iClipRect.iTl.iX,ycoord,iClipRect.Width(),1,iPenColor,iDrawMode);
			break;
		case EVerticalHatchBrush:
			drawDevice->WriteRgbMulti(iClipRect.iTl.iX,iClipRect.iTl.iY,
					iClipRect.Width(),iClipRect.Height(),iBrushColor,iDrawMode);
			while (Abs((xcoord - origin.iX) % 3) != 2)
				xcoord++;
			for (; xcoord < iClipRect.iBr.iX; xcoord += 3)
				drawDevice->WriteRgbMulti(xcoord,iClipRect.iTl.iY,1,iClipRect.Height(),iPenColor,iDrawMode);
			break;
		case ESquareCrossHatchBrush:
			drawDevice->WriteRgbMulti(iClipRect.iTl.iX,iClipRect.iTl.iY,
					iClipRect.Width(),iClipRect.Height(),iBrushColor,iDrawMode);
			while (Abs((ycoord - origin.iY) % 3) != 2)
				ycoord++;
			for (; ycoord < iClipRect.iBr.iY; ycoord += 3) // horizontal lines
				drawDevice->WriteRgbMulti(iClipRect.iTl.iX,ycoord,iClipRect.Width(),1,iPenColor,iDrawMode);

			ycoord = iClipRect.iTl.iY;
			while (Abs((ycoord - origin.iY) % 3) != 2 && ycoord < iClipRect.iBr.iY) // above the top horizontal line
				{
				xcoord = iClipRect.iTl.iX;
				while (Abs((xcoord - origin.iX) % 3) != 2)
					xcoord++;
				for (; xcoord < iClipRect.iBr.iX; xcoord += 3)
					drawDevice->WriteRgb(xcoord,ycoord,iPenColor,iDrawMode);
				ycoord++;
				}

			ycoord += 3;
			for (; ycoord < iClipRect.iBr.iY; ycoord += 3) // between the top and bottom horizontals
				{
				xcoord = iClipRect.iTl.iX;
				while (Abs((xcoord - origin.iX) % 3) != 2)
					xcoord++;
				for (; xcoord < iClipRect.iBr.iX; xcoord += 3)
					drawDevice->WriteRgbMulti(xcoord,ycoord - 2,1,2,iPenColor,iDrawMode);
				}

			ycoord -= 3;
			while (ycoord < iClipRect.iBr.iY) // below the bottom horizontal
				{
				xcoord = iClipRect.iTl.iX;
				while (Abs((xcoord - origin.iX) % 3) != 2)
					xcoord++;
				for (; xcoord < iClipRect.iBr.iX; xcoord += 3)
					drawDevice->WriteRgb(xcoord,ycoord,iPenColor,iDrawMode);
				ycoord++;
				}
			break;
		case EForwardDiagonalHatchBrush:
			drawDevice->WriteRgbMulti(iClipRect.iTl.iX,iClipRect.iTl.iY,
					iClipRect.Width(),iClipRect.Height(),iBrushColor,iDrawMode);
			for (; ycoord < iClipRect.iBr.iY; ycoord++)
				{
				xcoord = iClipRect.iTl.iX;
				TInt diff = (origin.iX + origin.iY - xcoord - ycoord) % 3;
				if (diff < 0)
					diff += 3;
				xcoord += diff;
				for (; xcoord < iClipRect.iBr.iX; xcoord += 3)
					drawDevice->WriteRgb(xcoord,ycoord,iPenColor,iDrawMode);
				}
			break;
		case ERearwardDiagonalHatchBrush:
			drawDevice->WriteRgbMulti(iClipRect.iTl.iX,iClipRect.iTl.iY,
					iClipRect.Width(),iClipRect.Height(),iBrushColor,iDrawMode);
			for (; ycoord < iClipRect.iBr.iY; ycoord++)
				{
				xcoord = iClipRect.iTl.iX;
				TInt diff = (origin.iX - origin.iY - xcoord + ycoord) % 3;
				if (diff < 0)
					diff += 3;
				xcoord += diff;
				for (; xcoord < iClipRect.iBr.iX; xcoord += 3)
					drawDevice->WriteRgb(xcoord,ycoord,iPenColor,iDrawMode);
				}
			break;
		case EDiamondCrossHatchBrush:
			{
			drawDevice->WriteRgbMulti(iClipRect.iTl.iX,iClipRect.iTl.iY,
					iClipRect.Width(),iClipRect.Height(),iBrushColor,iDrawMode);
			TInt sum = xcoord + ycoord - origin.iX - origin.iY;
			for (; ycoord < iClipRect.iBr.iY; ycoord++,sum++)
				{
				TInt currentsum = sum;
				for (xcoord = iClipRect.iTl.iX; xcoord < iClipRect.iBr.iX; xcoord++,currentsum++)
					{
					if((currentsum & 1) == 0 && ((currentsum & 3) != 0 || ((xcoord-origin.iX) & 1) == 1))
						drawDevice->WriteRgb(xcoord,ycoord,iPenColor,iDrawMode);
					}
				}
			break;
			}
		default:
			return;
			}

		drawDevice->UpdateRegion(iClipRect);
		}
	}

