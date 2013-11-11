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
#include "swdirectgdiellipse.h"
   
/**
@see MDirectGdiEngine::DrawRoundRect()
*/
void CSwDirectGdiEngine::DrawRoundRect(const TRect& aRect, const TSize& aCornerSize)
	{
	TSize cornerSize(aCornerSize);
	cornerSize.iWidth <<= 1;
	cornerSize.iHeight <<= 1;

	
	TRect rcpy(aRect);
	rcpy.Move(iOrigin);
	TruncateRect(rcpy);
	cornerSize.iWidth = Min(rcpy.Width(),cornerSize.iWidth);
	cornerSize.iHeight = Min(rcpy.Height(),cornerSize.iHeight);

	TRect targetRect(rcpy);
	targetRect.Grow((iPenSize.iWidth >> 1) + 1,(iPenSize.iHeight >> 1) + 1);

	if (iBrushStyle != DirectGdi::ENullBrush)
		{
		RoundRectFill(rcpy,cornerSize);
		}

	if ((iPenStyle != DirectGdi::ENullPen) && ((iPenSize.iWidth > 0) && (iPenSize.iHeight > 0)))
		{
		RoundRectOutline(rcpy, cornerSize);
		}
	}

/**
@see DrawRoundRect()
*/
void CSwDirectGdiEngine::RoundRectFill(const TRect& aRect, const TSize& aCornerSize)
	{
	TRect rcpy(aRect);
	if (iPenSize.iWidth < 1 || iPenSize.iHeight < 1)
		{
		rcpy.Grow(1,1);
		}

	TInt xoff = rcpy.Width() - aCornerSize.iWidth;
	TInt yoff = rcpy.Height() - aCornerSize.iHeight;
	TPoint tl, tr, bl, br;
	TInt prevlev = 0;
	TBool draw = EFalse;

	TRect clipRect(0,0,0,0);
	const TInt limit = iDefaultRegionPtr->Count();
	for (TInt count = 0; count < limit; count++)
		{
		clipRect = (*iDefaultRegionPtr)[count];	
		if (!clipRect.Intersects(aRect))
			{
			continue;
			}

		draw = ETrue;
		clipRect.Intersection(aRect);

		TSwDirectGdiEllipse ellipse;
		ellipse.Construct(TRect(rcpy.iTl,rcpy.iTl + aCornerSize));
		ellipse.SingleStep(tl, tr, bl, br);
		prevlev = tl.iY;

		while (!ellipse.SingleStep(tl, tr, bl, br))
			{
			if (tl.iY == prevlev)
				{
				continue;
				}

			tl.iX++;
			tr.iX += xoff - 1;
			bl.iX++;
			bl.iY += yoff;
			br.iX += xoff - 1;
			br.iY += yoff;

			ClipFillLine(tl, tr,clipRect);
			ClipFillLine(bl, br,clipRect);

			prevlev = tl.iY;
			}

		iDrawDevice->UpdateRegion(clipRect);
		}

	if (!draw)
		{
		return;
		}

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

/**
@see DrawRoundRect()
*/
void CSwDirectGdiEngine::RoundRectOutline(const TRect& aRect, const TSize& aCornerSize)
	{
	TRect rcpy(aRect);
	const TInt halfpenwidth = (iPenSize.iWidth + 1) >> 1;
	const TInt halfpenheight = (iPenSize.iHeight + 1) >> 1;
	rcpy.Grow(halfpenwidth,halfpenheight);

	TPoint tl, tr, bl, br;
	const TInt xoff = aRect.Width() - aCornerSize.iWidth;
	const TInt yoff = aRect.Height() - aCornerSize.iHeight;
	const TInt dotparam = iDotParam;

	TRect clipRect(0,0,0,0);
	const TInt limit = iDefaultRegionPtr->Count();
	for (TInt count = 0; count < limit; count++)
		{
		clipRect = (*iDefaultRegionPtr)[count];
		if (!clipRect.Intersects(rcpy))
			{
			continue;
			}
		clipRect.Intersection(rcpy);

		iDotParam = Max(iPenSize.iWidth >> 1,iPenSize.iHeight >> 1);
		TInt column = aRect.iTl.iX + (aCornerSize.iWidth >> 1);
		TInt lastcolumn = aRect.iTl.iX + xoff + (aCornerSize.iWidth >> 1);

		for (; column < lastcolumn; column++)
			{
			PenDrawClipped(TPoint(column,aRect.iTl.iY), clipRect);
			PenDrawClipped(TPoint(column,aRect.iBr.iY - 1), clipRect);
			iDotParam += iDotDirection;
			}

		TSwDirectGdiEllipse ellipse;
		ellipse.Construct(TRect(aRect.iTl,aRect.iTl + aCornerSize));
		while (!ellipse.SingleStep(tl,tr,bl,br))
			{
			tr.iX += xoff;
			bl.iY += yoff;
			br.iX += xoff;
			br.iY += yoff;

			PenDrawClipped(tl, clipRect);
			PenDrawClipped(tr, clipRect);
			PenDrawClipped(bl, clipRect);
			PenDrawClipped(br, clipRect);

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
			PenDrawClipped(TPoint(aRect.iTl.iX,column), clipRect);
			PenDrawClipped(TPoint(aRect.iBr.iX - 1,column), clipRect);
			iDotParam += iDotDirection;
			}

		iDrawDevice->UpdateRegion(clipRect);
		}

	iDotParam = dotparam;
	}
