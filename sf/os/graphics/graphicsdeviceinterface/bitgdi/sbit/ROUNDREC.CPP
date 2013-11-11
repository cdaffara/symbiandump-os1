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

/**
Draws and fills a rectangle with rounded corners.

The function provides a concrete implementation of the pure virtual
function <code>CGraphicsContext::DrawRoundRect()</code>. The function
behaviour is the same as documented in that class.
*/
EXPORT_C void CFbsBitGc::DrawRoundRect(const TRect& aRect,const TSize& aSize)
	{
	if (CheckDevice(aRect))
		return;

	TSize ellsize(aSize);
	ellsize.iWidth <<= 1;
	ellsize.iHeight <<= 1;

	if (ellsize.iWidth < 3 || ellsize.iHeight < 3)
		{
		DrawRect(aRect);
		return;
		}

	if (aRect.Width() < ellsize.iWidth && aRect.Height() < ellsize.iHeight)
		{
		DrawEllipse(aRect);
		return;
		}

	TRect rcpy(aRect);
	rcpy.Move(iOrigin);
	iDevice->TruncateRect(rcpy);
	ellsize.iWidth = Min(rcpy.Width(),ellsize.iWidth);
	ellsize.iHeight = Min(rcpy.Height(),ellsize.iHeight);

	TRect clippedBoundingRect(rcpy);
	clippedBoundingRect.Grow((iPenSize.iWidth >> 1) + 1,(iPenSize.iHeight >> 1) + 1);
	if (!clippedBoundingRect.Intersects(iUserClipRect))
		return;

	SetupDevice();
	iDevice->DrawingBegin(&iBrushBitmap);
	CFbsRasterizer* brushRasterizer = PrepareRasterizerForExtendedBitmap(iBrushBitmap);

	if (iBrushStyle != ENullBrush)
		RoundRectFill(rcpy,ellsize);

	if (iPenStyle != ENullPen && iPenSize.iWidth > 0 && iPenSize.iHeight > 0)
		RoundRectOutline(rcpy,ellsize);

	if (brushRasterizer)
		{
		brushRasterizer->EndBitmap(iBrushBitmap.SerialNumber());
		}
	iDevice->DrawingEnd(&iBrushBitmap);
	}

// if iBrushBitmap is an extended bitmap, PrepareRasterizerForExtendedBitmap() must have been called before this method
void CFbsBitGc::RoundRectFill(const TRect& aRect,TSize aSize)
	{
	TRect rcpy(aRect);
	if (iPenSize.iWidth < 1 || iPenSize.iHeight < 1)
		rcpy.Grow(1,1);
	AddRect(rcpy);

	if (rcpy.iBr.iX - rcpy.iTl.iX < aSize.iWidth)
		aSize.iWidth = rcpy.Width();
	if (rcpy.iBr.iY - rcpy.iTl.iY < aSize.iHeight)
		aSize.iHeight = rcpy.Height();

	TInt xoff = rcpy.Width() - aSize.iWidth;
	TInt yoff = rcpy.Height() - aSize.iHeight;
	TPoint tl,tr,bl,br;
	TInt prevlev = 0;
	TBool draw = EFalse;

	const TInt limit = iDefaultRegionPtr->Count();
	for (TInt count = 0; count < limit; count++)
		{
		iClipRect = (*iDefaultRegionPtr)[count];
		if (UserClipRect(iClipRect))
			continue;
		if (!iClipRect.Intersects(aRect))
			continue;

		draw = ETrue;
		iClipRect.Intersection(aRect);

		TEllipse ellipse;
		ellipse.Construct(TRect(rcpy.iTl,rcpy.iTl + aSize));
		ellipse.SingleStep(tl,tr,bl,br);
		prevlev = tl.iY;

		while (!ellipse.SingleStep(tl,tr,bl,br))
			{
			if (tl.iY == prevlev)
				continue;

			tl.iX++;
			tr.iX += xoff - 1;
			bl.iX++;
			bl.iY += yoff;
			br.iX += xoff - 1;
			br.iY += yoff;

			ClipFillLine(tl,tr);
			ClipFillLine(bl,br);

			prevlev = tl.iY;
			}

		iDevice->iDrawDevice->UpdateRegion(iClipRect);
		}

	if (!draw)
		return;

	if (tl.iY >= bl.iY)
		{
		tl.iY--;
		br.iY++;
		}

	tl.iX++;
	tl.iY++;
	br.iX += xoff;
	br.iY += yoff;

	RectFill(TRect(tl,br));
	}

void CFbsBitGc::RoundRectOutline(const TRect& aRect,TSize aSize)
	{
	TRect rcpy(aRect);
	const TInt halfpenwidth = (iPenSize.iWidth + 1) >> 1;
	const TInt halfpenheight = (iPenSize.iWidth + 1) >> 1;
	rcpy.Grow(halfpenwidth,halfpenheight);
	AddRect(rcpy);

	if (aRect.Width() < aSize.iWidth)
		aSize.iWidth = aRect.Width();
	if (aRect.Height() < aSize.iHeight)
		aSize.iHeight = aRect.Height();

	TPoint tl,tr,bl,br;
	const TInt xoff = aRect.Width() - aSize.iWidth;
	const TInt yoff = aRect.Height() - aSize.iHeight;
	const TInt dotparam = iDotParam;

	const TInt limit = iDefaultRegionPtr->Count();
	for (TInt count = 0; count < limit; count++)
		{
		iClipRect = (*iDefaultRegionPtr)[count];
		if (!iClipRect.Intersects(rcpy))
			continue;
		iClipRect.Intersection(rcpy);
		if (UserClipRect(iClipRect))
			continue;

		iDotParam = Max(iPenSize.iWidth >> 1,iPenSize.iHeight >> 1);
		TInt column = aRect.iTl.iX + (aSize.iWidth >> 1);
		TInt lastcolumn = aRect.iTl.iX + xoff + (aSize.iWidth >> 1);

		for (; column < lastcolumn; column++)
			{
			PenDrawClipped(TPoint(column,aRect.iTl.iY));
			PenDrawClipped(TPoint(column,aRect.iBr.iY - 1));
			iDotParam += iDotDirection;
			}

		TEllipse ellipse;
		ellipse.Construct(TRect(aRect.iTl,aRect.iTl + aSize));
		while (!ellipse.SingleStep(tl,tr,bl,br))
			{
			tr.iX += xoff;
			bl.iY += yoff;
			br.iX += xoff;
			br.iY += yoff;

			PenDrawClipped(tl);
			PenDrawClipped(tr);
			PenDrawClipped(bl);
			PenDrawClipped(br);

			iDotParam += iDotDirection;
			}

		if (tl.iY >= bl.iY)
			{
			tl.iY--;
			bl.iY++;
			}

		bl.iY += yoff;

		for (column = tl.iY + 1; column < bl.iY; column++)
			{
			PenDrawClipped(TPoint(aRect.iTl.iX,column));
			PenDrawClipped(TPoint(aRect.iBr.iX - 1,column));
			iDotParam += iDotDirection;
			}

		iDevice->iDrawDevice->UpdateRegion(iClipRect);
		}

	iDotParam = dotparam;
	}

