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

// TPieArc

class TArc : public TEllipse
	{
public:
	void Construct(const TRect& aRect,const TPoint& aStart,const TPoint& aEnd);
	TBool SingleStep(TPoint& aTl,TBool& aDoTl,TPoint& aTr,TBool& aDoTr,TPoint& aBl,TBool& aDoBl,TPoint& aBr,TBool& aDoBr);
	void Step(TPoint& aTl,TBool& aDoTl,TPoint& aTr,TBool& aDoTr,TPoint& aBl,TBool& aDoBl,TPoint& aBr,TBool& aDoBr);
public:
	TPoint iStart;
	TPoint iEnd;
private:
	TBool iTlquad;
	TBool iTrquad;
	TBool iBlquad;
	TBool iBrquad;
	TBool iStartquadenabled;
	TBool iEndquadenabled;
	TBool iStartquaddone;
	TBool iEndquaddone;
	TInt iStartquad;
	TInt iEndquad;
	TBool iSlice;
	};

void TArc::Construct(const TRect& aRect,const TPoint& aStart,const TPoint& aEnd)
	{
	iStart=Intersection(aRect,aStart);
	iEnd=Intersection(aRect,aEnd);
	TEllipse::Construct(aRect);
	iTlquad=EFalse;
	iTrquad=EFalse;
	iBlquad=EFalse;
	iBrquad=EFalse;
	iStartquadenabled=EFalse;
	iEndquadenabled=EFalse;
	iSlice=EFalse;
	iStartquad=0;
	iEndquad=0;
	TPoint center=aRect.Center();
	if (iStart.iX>=center.iX) iStartquad=1;
	if (iStart.iY>=center.iY) iStartquad+=2;
	if (iEnd.iX>=center.iX) iEndquad=1;
	if (iEnd.iY>=center.iY) iEndquad+=2;
	if (iStartquad!=iEndquad)
		{
		if (iStartquad==0 && iEndquad==1)
			{
			iBlquad=ETrue;
			iBrquad=ETrue;
			}
		else if (iStartquad==0 && iEndquad==3)
			iBlquad=ETrue;
		else if (iStartquad==1 && iEndquad==2)
			iTlquad=ETrue;
		else if (iStartquad==1 && iEndquad==3)
			{
			iTlquad=ETrue;
			iBlquad=ETrue;
			}
		else if (iStartquad==2 && iEndquad==0)
			{
			iTrquad=ETrue;
			iBrquad=ETrue;
			}
		else if (iStartquad==2 && iEndquad==1)
			iBrquad=ETrue;
		else if (iStartquad==3 && iEndquad==0)
			iTrquad=ETrue;
		else if (iStartquad==3 && iEndquad==2)
			{
			iTlquad=ETrue;
			iTrquad=ETrue;
			}
		}
	else if (iStart==iEnd)
		{
		iTlquad=ETrue;
		iTrquad=ETrue;
		iBlquad=ETrue;
		iBrquad=ETrue;
		}
	else
		{
		iSlice=ETrue;
		if (iStartquad==0 && (iStart.iX<iEnd.iX || iStart.iY>iEnd.iY))
			{
			iTrquad=ETrue;
			iBlquad=ETrue;
			iBrquad=ETrue;
			iSlice=EFalse;
			}
		else if (iStartquad==1 && (iStart.iX<iEnd.iX || iStart.iY<iEnd.iY))
			{
			iTlquad=ETrue;
			iBlquad=ETrue;
			iBrquad=ETrue;
			iSlice=EFalse;
			}
		else if (iStartquad==2 && (iStart.iX>iEnd.iX || iStart.iY>iEnd.iY))
			{
			iTlquad=ETrue;
			iTrquad=ETrue;
			iBrquad=ETrue;
			iSlice=EFalse;
			}
		else if (iStartquad==3 && (iStart.iX>iEnd.iX || iStart.iY<iEnd.iY))
			{
			iTlquad=ETrue;
			iTrquad=ETrue;
			iBlquad=ETrue;
			iSlice=EFalse;
			}
		}
	if (iStartquad==1 || iStartquad==2)
		iStartquadenabled=ETrue;
	if (iEndquad==0 || iEndquad==3)
		iEndquadenabled=ETrue;
	iStartquaddone=EFalse;
	iEndquaddone=EFalse;
	}

TBool TArc::SingleStep(TPoint& aTopLeft,TBool& aDoTl,TPoint& aTopRight,
					   TBool& aDoTr,TPoint& aBottomLeft,TBool& aDoBl,
					   TPoint& aBottomRight,TBool& aDoBr)
	{
	TBool finished=TEllipse::SingleStep(aTopLeft,aTopRight,aBottomLeft,aBottomRight);
	Step(aTopLeft,aDoTl,aTopRight,aDoTr,aBottomLeft,aDoBl,aBottomRight,aDoBr);

	return finished;
	}

void TArc::Step(TPoint& aTopLeft,TBool& aDoTl,TPoint& aTopRight,TBool& aDoTr,
				TPoint& aBottomLeft,TBool& aDoBl,TPoint& aBottomRight,TBool& aDoBr)
	{
	aDoTl=iTlquad;
	aDoTr=iTrquad;
	aDoBl=iBlquad;
	aDoBr=iBrquad;
	if (!iStartquaddone)
		{
		if (!iStartquadenabled)
			{
			if (iStartquad==0 && aTopLeft.iX<=iStart.iX && aTopLeft.iY>=iStart.iY)
				{
				iStartquadenabled=ETrue;
				iStartquaddone=ETrue;
				}
			if (iStartquad==3 && aBottomRight.iX>=iStart.iX && aBottomRight.iY<=iStart.iY)
				{
				iStartquadenabled=ETrue;
				iStartquaddone=ETrue;
				}
			}
		else
			{
			if (iStartquad==1 && (aTopRight.iX>iStart.iX || aTopRight.iY>iStart.iY))
				{
				iStartquadenabled=EFalse;
				iStartquaddone=ETrue;
				}
			if (iStartquad==2 && (aBottomLeft.iX<iStart.iX || aBottomLeft.iY<iStart.iY))
				{
				iStartquadenabled=EFalse;
				iStartquaddone=ETrue;
				}
			}
		}
	if (!iEndquaddone)
		{
		if (iEndquadenabled)
			{
			if (iEndquad==0 && (aTopLeft.iX<iEnd.iX || aTopLeft.iY>iEnd.iY))
				{
				iEndquadenabled=EFalse;
				iEndquaddone=ETrue;
				}
			if (iEndquad==3 && (aBottomRight.iX>iEnd.iX || aBottomRight.iY<iEnd.iY))
				{
				iEndquadenabled=EFalse;
				iEndquaddone=ETrue;
				}
			}
		else
			{
			if (iEndquad==1 && aTopRight.iX>=iEnd.iX && aTopRight.iY>=iEnd.iY)
				{
				iEndquadenabled=ETrue;
				iEndquaddone=ETrue;
				}
			if (iEndquad==2 && aBottomLeft.iX<=iEnd.iX && aBottomLeft.iY<=iEnd.iY)
				{
				iEndquadenabled=ETrue;
				iEndquaddone=ETrue;
				}
			}
		}
	if (iStartquad!=iEndquad)
		{
		if (iStartquadenabled)
			{
			if (iStartquad==0) aDoTl=ETrue;
			else if (iStartquad==1) aDoTr=ETrue;
			else if (iStartquad==2) aDoBl=ETrue;
			else if (iStartquad==3) aDoBr=ETrue;
			}
		if (iEndquadenabled)
			{
			if (iEndquad==0) aDoTl=ETrue;
			else if (iEndquad==1) aDoTr=ETrue;
			else if (iEndquad==2) aDoBl=ETrue;
			else if (iEndquad==3) aDoBr=ETrue;
			}
		}
	else
		{
		if (iSlice)
			{
			if (iStartquadenabled && iEndquadenabled)
				{
				if (iStartquad==0) aDoTl=ETrue;
				else if (iStartquad==1) aDoTr=ETrue;
				else if (iStartquad==2) aDoBl=ETrue;
				else if (iStartquad==3) aDoBr=ETrue;
				}
			}
		else
			{
			if (iStartquadenabled || iEndquadenabled)
				{
				if (iStartquad==0) aDoTl=ETrue;
				else if (iStartquad==1) aDoTr=ETrue;
				else if (iStartquad==2) aDoBl=ETrue;
				else if (iStartquad==3) aDoBr=ETrue;
				}
			}
		}
	if (aTopLeft.iX==aTopRight.iX)
		{
		if (aDoTl && aDoTr) aDoTr=EFalse;
		if (aDoBl && aDoBr) aDoBr=EFalse;
		}
	}

//
// Pie and Arc drawing functions
//

/**
Draws an arc.

The function provides a concrete implementation of the pure virtual
function <code>CGraphicsContext::DrawArc()</code>. The function
behaviour is the same as documented in that class.
*/
EXPORT_C void CFbsBitGc::DrawArc(const TRect& aRect,const TPoint& aStartradius,
								 const TPoint& aEndradius)
	{
	if (!iPenStyle || !iPenSize.iWidth  || !iPenSize.iHeight || aRect.IsEmpty() || CheckDevice(aRect))
		return;

	TRect rcpy(aRect);
	rcpy.Move(iOrigin);
	iDevice->TruncateRect(rcpy);
	TRect arcBoundingRect(rcpy);
	arcBoundingRect.Grow((iPenSize.iWidth>>1)+1,(iPenSize.iHeight>>1)+1);
	if (!arcBoundingRect.Intersects(iUserClipRect))
		return;

	SetupDevice();
	iDevice->DrawingBegin();
	PieArcOutline(rcpy,aStartradius+iOrigin,aEndradius+iOrigin,EFalse);
	iDevice->DrawingEnd();
	}

/**
Draws and fills a pie slice.

The function provides a concrete implementation of the pure virtual
function <code>CGraphicsContext::DrawPie()</code>. The function
behaviour is the same as documented in that class.
*/
EXPORT_C void CFbsBitGc::DrawPie(const TRect& aRect,const TPoint& aStartradius,
								 const TPoint& aEndradius)
	{
	if (CheckDevice(aRect))
		return;

	TRect rcpy(aRect);
	rcpy.Move(iOrigin);
	iDevice->TruncateRect(rcpy);
	TRect pieBoundingRect(rcpy);
	pieBoundingRect.Grow((iPenSize.iWidth>>1)+1,(iPenSize.iHeight>>1)+1);
	if (!pieBoundingRect.Intersects(iUserClipRect))
		return;

	TPoint startIntersect = aStartradius + iOrigin, endIntersect = aEndradius + iOrigin;
	TInt startQuadrant, endQuadrant;
	TBool quadrants[5];
	const TBool isEllipse = AnalyseEllipse(
		rcpy, startIntersect, endIntersect, startQuadrant, endQuadrant, quadrants);

	SetupDevice();
	iDevice->DrawingBegin(&iBrushBitmap);
	CFbsRasterizer* brushRasterizer = PrepareRasterizerForExtendedBitmap(iBrushBitmap);
	if (iBrushStyle!=ENullBrush)
		{
		if (isEllipse)
			EllipseFill(rcpy);
		else
			PieFill(rcpy, startIntersect, endIntersect, startQuadrant, endQuadrant, quadrants);
		}
	if (iPenStyle!=ENullPen && iPenSize.iWidth>0 && iPenSize.iHeight>0)
		PieArcOutline(rcpy,aStartradius+iOrigin,aEndradius+iOrigin,ETrue);
	if (brushRasterizer)
		{
		brushRasterizer->EndBitmap(iBrushBitmap.SerialNumber());
		}
	iDevice->DrawingEnd(&iBrushBitmap);
	}

/**
Determine the pie constructed within the given rectangle.
@internalTechnology
@param rc Input - The bounding rectangle.
@param srad Input/Output - The intersection point from the geometric centre
of the ellipse to the given start point
@param erad Input/Output - The intersection point from the geometric centre
of the ellipse to the given end point
@param startq Output - The start quadrant
@param endq Output - The end quadrant
@param quads Output - Quadrants
@pre Input params rc, srad, erad are to be given.
@post Output params srad, erad, startq, endq, quads will be populated.
@return ETrue if the pie is an ellipse, otherwise EFalse.
*/
TBool CFbsBitGc::AnalyseEllipse(const TRect& rc,TPoint& srad,TPoint& erad,
							   TInt& startq,TInt& endq,TBool* quads)
	{
	startq=0;
	endq=0;
	const TPoint center = rc.Center();
	TEllipse ellipse;
	srad=ellipse.Intersection(rc,srad);
	erad=ellipse.Intersection(rc,erad);
	if (srad==erad)
		{
		quads[0]=EFalse;
		quads[1]=ETrue;
		quads[2]=ETrue;
		quads[3]=ETrue;
		quads[4]=ETrue;
		return ETrue;
		}
	const TInt startx = srad.iX - center.iX, starty = srad.iY - center.iY;
	const TInt endx = erad.iX - center.iX, endy = erad.iY - center.iY;
	if (startx>=0) startq=1;
	if (starty>=0) startq+=2;
	if (endx>=0) endq=1;
	if (endy>=0) endq+=2;
	quads[1]=EFalse,quads[2]=EFalse,quads[3]=EFalse,quads[4]=EFalse; // complete quadrants to draw
	quads[0]=EFalse; // ellipse is a sliver completely within a quadrant
	if (startq==endq)
		{
		if (startq==0 && (startx<endx || starty>endy))
			{
			quads[2]=ETrue;
			quads[3]=ETrue;
			quads[4]=ETrue;
			}
		else if (startq==1 && (startx<endx || starty<endy))
			{
			quads[1]=ETrue;
			quads[3]=ETrue;
			quads[4]=ETrue;
			}
		else if (startq==2 && (startx>endx || starty>endy))
			{
			quads[1]=ETrue;
			quads[2]=ETrue;
			quads[4]=ETrue;
			}
		else if (startq==3 && (startx>endx || starty<endy))
			{
			quads[1]=ETrue;
			quads[2]=ETrue;
			quads[3]=ETrue;
			}
		else quads[0]=ETrue; // "slice"
		}
	else
		{
		if (startq==0 && endq==1)
			{
			quads[3]=ETrue;
			quads[4]=ETrue;
			}
		else if (startq==0 && endq==3)
			quads[3]=ETrue;
		else if (startq==1 && endq==2)
			quads[1]=ETrue;
		else if (startq==1 && endq==3)
			{
			quads[1]=ETrue;
			quads[3]=ETrue;
			}
		else if (startq==2 && endq==0)
			{
			quads[2]=ETrue;
			quads[4]=ETrue;
			}
		else if (startq==2 && endq==1)
			quads[4]=ETrue;
		else if (startq==3 && endq==0)
			quads[2]=ETrue;
		else if (startq==3 && endq==2)
			{
			quads[1]=ETrue;
			quads[2]=ETrue;
			}
		}
	return EFalse;
	}

void CFbsBitGc::PieArcOutline(const TRect& ellrect,const TPoint& startradius,
							  const TPoint& endradius,TBool pie)
	{
	// arc runs counterclockwise, from startradius-center/ellipse intersect to endradius-center/ellipse intersect
	TInt dotparam=iDotParam;
	TRect rcpy(ellrect);
	TPoint pt[4];
	TInt halfpenwidth=iPenSize.iWidth>>1;
	TInt halfpenheight=iPenSize.iHeight>>1;
	TInt otherhalfwidth=(iPenSize.iWidth+1)>>1;
	TInt otherhalfheight=(iPenSize.iHeight+1)>>1;
	rcpy.iTl.iX-=halfpenwidth;
	rcpy.iTl.iY-=halfpenheight;
	rcpy.iBr.iX+=otherhalfwidth;
	rcpy.iBr.iY+=otherhalfheight;
	AddRect(rcpy);
	for(TInt count=0;count<iDefaultRegionPtr->Count();count++)
		{
		iClipRect=(*iDefaultRegionPtr)[count];
		if (!iClipRect.Intersects(rcpy))
			continue;
		iClipRect.Intersection(rcpy);
		if (UserClipRect(iClipRect)) continue;
		TArc arc;
		arc.Construct(ellrect,startradius,endradius);
		iDotParam=Max(iPenSize.iWidth>>1,iPenSize.iHeight>>1);
		TBool dotl,dotr,dobl,dobr;
		while(!arc.SingleStep(pt[0],dotl,pt[1],dotr,pt[2],dobl,pt[3],dobr))
			{
			if (dotl) PenDrawClipped(pt[0]);
			if (dotr) PenDrawClipped(pt[1]);
			if (dobl) PenDrawClipped(pt[2]);
			if (dobr) PenDrawClipped(pt[3]);
			iDotParam+=iDotDirection;
			}
		if (pt[0].iY==pt[2].iY)
			{
			if (dotl) PenDrawClipped(pt[0]);
			if (dotr) PenDrawClipped(pt[1]);
			}
		if (pie)
			{
			TPoint temp;
			const TPoint center = ellrect.Center();
			TLinearDDA line;
			line.Construct(arc.iStart,center);
			line.SingleStep(temp);
			while(!line.SingleStep(temp))
				{
				PenDrawClipped(temp);
				iDotParam+=iDotDirection;
				}
			line.Construct(arc.iEnd,center);
			line.SingleStep(temp);
			while(!line.SingleStep(temp))
				{
				PenDrawClipped(temp);
				iDotParam+=iDotDirection;
				}
			PenDrawClipped(center);
			}
		iDevice->iDrawDevice->UpdateRegion(iClipRect);
		}
	iDotParam=dotparam;
	}

// if iBrushBitmap is an extended bitmap, PrepareRasterizerForExtendedBitmap() must have been called before this method
void CFbsBitGc::PieFill(const TRect& ellrect, const TPoint& aStartIntersect, const TPoint& aEndIntersect,
						TInt aStartQuadrant, TInt aEndQuadrant, const TBool* aQuadrants)
	{
	// arc runs counterclockwise, from startradius-center/ellipse intersect to endradius-center/ellipse intersect
	AddRect(ellrect);
	const TInt limit = iDefaultRegionPtr->Count();
	for(TInt count=0;count<limit;count++)
		{
		iClipRect=(*iDefaultRegionPtr)[count];
		if (!iClipRect.Intersects(ellrect))
			continue;
		iClipRect.Intersection(ellrect);
		if (UserClipRect(iClipRect)) continue;
		if (!aQuadrants[0])
			PieShell(ellrect, aStartIntersect, aEndIntersect, aQuadrants, aStartQuadrant, aEndQuadrant);
		else
			PieSliver(ellrect, aStartIntersect, aEndIntersect, aStartQuadrant);
		iDevice->iDrawDevice->UpdateRegion(iClipRect);
		}
	}

// if iBrushBitmap is an extended bitmap, PrepareRasterizerForExtendedBitmap() must have been called before this method
void CFbsBitGc::PieShell(const TRect& ellrect,const TPoint& startradius,
						 const TPoint& endradius, const TBool* quads, TInt startquad, TInt endquad)
	{
	TEllipse ellipse;
	ellipse.Construct(ellrect);
	TInt c=ellrect.Center().iX;
	TPoint pt[4];
	TPoint tl,tr,bl,br;
	TBool donestart=EFalse,doneend=EFalse;
	TBool todostart=EFalse,todoend=EFalse;
	while(!ellipse.NextStep(pt[0],pt[1],pt[2],pt[3]))
		{
		if (pt[startquad].iY==startradius.iY)
			todostart=ETrue;
		if (pt[endquad].iY==endradius.iY)
			todoend=ETrue;
		pt[0].iX++;
		pt[1].iX--;
		pt[2].iX++;
		pt[3].iX--;
		tl.iY=pt[0].iY;
		tr.iY=pt[1].iY;
		bl.iY=pt[2].iY;
		br.iY=pt[3].iY;
		if (quads[1] || (endquad==0 && !doneend)) // tl quadrant, half top end chord
			tl.iX=pt[0].iX;
		else tl.iX=c+1;
		if (quads[2] || (startquad==1 && !donestart)) // tr quadrant, half top start chord
			tr.iX=pt[1].iX;
		else tr.iX=c;
		if (quads[3] || (startquad==2 && !donestart)) // bl quadrant, half top start chord
			bl.iX=pt[2].iX;
		else bl.iX=c+1;
		if (quads[4] || (endquad==3 && !doneend)) // br quadrant, half top end chord
			br.iX=pt[3].iX;
		else br.iX=c;
		ClipFillLine(tl,tr);
		ClipFillLine(bl,br);
		// do partial quadrants
		if (todostart)
			{
			if (startquad==0)
				{
				tl.iX=pt[0].iX;
				tr.iX=startradius.iX;
				ClipFillLine(tl,tr);
				}
			else if (startquad==3)
				{
				bl.iX=startradius.iX;
				br.iX=pt[3].iX;
				ClipFillLine(bl,br);
				}
			}
		if (todoend)
			{
			if (endquad==2)
				{
				bl.iX=pt[2].iX;
				br.iX=endradius.iX;
				ClipFillLine(bl,br);
				}
			else if (endquad==1)
				{
				tl.iX=endradius.iX;
				tr.iX=pt[1].iX;
				ClipFillLine(tl,tr);
				}
			}
		donestart=todostart;
		doneend=todoend;
		}
	tl.iX=c+1;
	tr.iX=c;
	if (pt[0].iY==pt[2].iY) // congruent mid lines
		{
		if (pt[startquad].iY==startradius.iY)
			todostart=ETrue;
		if (pt[endquad].iY==endradius.iY)
			todoend=ETrue;
		pt[0].iX++;
		pt[1].iX--;
		tl.iY=pt[0].iY;
		tr.iY=tl.iY;
		TBool leftflag=EFalse,rightflag=EFalse;
		if (quads[1] || (endquad==0 && !doneend) ||
			quads[3] || (startquad==2 && !donestart) ||
			(todostart && startquad==0) || (todoend && endquad==2))
			leftflag=ETrue;
		if (quads[2] || (startquad==1 && !donestart) ||
			quads[4] || (endquad==3 && !doneend) ||
			(todostart && startquad==3) || (todoend && endquad==1))
			rightflag=ETrue;
		if (leftflag) tl.iX=pt[0].iX;
		if (rightflag) tr.iX=pt[1].iX;
		ClipFillLine(tl,tr);
		}
	else
		{
		tl.iY=ellrect.Center().iY;
		tr.iY=tl.iY;
		if (startquad==3) tr.iX=startradius.iX-1;
		if (endquad==2) tl.iX=endradius.iX+1;
		ClipFillLine(tl,tr);
		}
	PieTriangles(startquad==1 || startquad==2,startradius,ellrect.Center());
	PieTriangles(endquad==0 || endquad==3,endradius,ellrect.Center());
	}

// if iBrushBitmap is an extended bitmap, PrepareRasterizerForExtendedBitmap() must have been called before this method
void CFbsBitGc::PieTriangles(TBool aInside,const TPoint& aStart,const TPoint& aEnd)
	{
	TInt x=aInside?aEnd.iX:aStart.iX;
	if (aStart.iX>aEnd.iX)
		{
		if (aInside)
			x++;
		else
			x--;
		}
	else
		if (!aInside)
			x++;
	TLinearDDA line;
	TPoint pt,left,right;
	line.Construct(aStart,aEnd);
	line.NextStep(pt);
	while(!line.NextStep(pt))
		{
		if (pt.iY==aEnd.iY) break;
		left.iX=Min(pt.iX,x);
		right.iX=Max(pt.iX,x);
		left.iY=right.iY=pt.iY;
		ClipFillLine(left,right);
		}
	}

// if iBrushBitmap is an extended bitmap, PrepareRasterizerForExtendedBitmap() must have been called before this method
void CFbsBitGc::PieSliver(const TRect& ellrect,const TPoint& startradius,
						  const TPoint& endradius,TInt quad)
	{
	TPoint center=ellrect.Center(),left,right;
	TPoint nearinter(startradius),farinter(endradius);
	if (Abs(nearinter.iY-center.iY)>Abs(farinter.iY-center.iY))
		{
		nearinter=endradius;
		farinter=startradius;
		}
	TBool ellipseComplete = EFalse;
	TPoint pt[4];
	TEllipse ellipse;
	ellipse.Construct(ellrect);
	TLinearDDA mainline;
	mainline.Construct(farinter,center);
	ellipseComplete = ellipse.SingleStep(pt[0],pt[1],pt[2],pt[3]);
	mainline.SingleStep(right);
	do	{
		while(!ellipseComplete && pt[quad].iY!=right.iY)
			ellipseComplete = ellipse.NextStep(pt[0],pt[1],pt[2],pt[3]);
		left=pt[quad];
		while(!ellipseComplete && pt[quad].iY==right.iY)
			{
			left=pt[quad];
			ellipseComplete = ellipse.NextStep(pt[0],pt[1],pt[2],pt[3]);
			}
		if (right.iY==nearinter.iY || (ellipseComplete && (pt[0].iY != pt[2].iY)))
			break;
		if (left.iX>right.iX)
			{
			TInt temp=left.iX;
			left.iX=right.iX;
			right.iX=temp;
			}
		if(right==farinter && left.iX<=right.iX)
			{
			continue;
			}
		left.iX++;
		right.iX--;
		if (left.iX<=right.iX)
			ClipFillLine(left,right);
		}
	while(!mainline.NextStep(right));
	TPoint temppt;
	TLinearDDA line;
	line.Construct(nearinter,center);
	TBool linestat=EFalse;
	do
		linestat=line.SingleStep(temppt);
	while(temppt.iY!=right.iY && !linestat);
	do	{
		do	{
			left=temppt;
			linestat=line.SingleStep(temppt);
			}
		while(temppt.iY==right.iY && !linestat);
		if (ellipseComplete)
			break;
		if (left.iX>right.iX)
			{
			TInt temp=left.iX;
			left.iX=right.iX;
			right.iX=temp;
			}
		if(right==farinter && left.iX<=right.iX)
			{
			continue;
			}
		left.iX++;
		right.iX--;
		if (left.iX<=right.iX)
			ClipFillLine(left,right);
		}
	while(!mainline.NextStep(right));
	}

