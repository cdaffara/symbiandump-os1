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
A utility class to efficiently draw arcs.
@see CSwDirectGdiEngine::PieArcOutline

@internalComponent
*/
class TSwDirectGdiArc : public TSwDirectGdiEllipse
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

/**
Constructs a TSwDirectGdiArc. 
@param aRect   The bounding rectangle. 
@param aStart  The arc start point.
@param aEnd	   The arc end point.
@see CSwDirectGdiEngine::PieArcOutline
*/
void TSwDirectGdiArc::Construct(const TRect& aRect,const TPoint& aStart,const TPoint& aEnd)
	{
	iStart=Intersection(aRect,aStart);
	iEnd=Intersection(aRect,aEnd);
	TSwDirectGdiEllipse::Construct(aRect);
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

/**
Produces the next stage creating an arc, taking four points (the corners of 
the rectangle the arc should fill) as parameters.

@param aTopLeft Top left corner of rectangle.
@param aDoTl Span the top left quadrant.
@param aTopRight Top right corner of rectangle.
@param aDoTr Span the top right quadrant.
@param aBottomLeft Bottom left corner of rectangle.
@param aDoBl Span the bottom left quadrant.
@param aBottomRight Bottom right corner of rectangle.
@param aDoBr Span the bottom right quadrant.
@return TBool ETrue if step completed successfully.
@see TSwDirectGdiEllipse::SingleStep
*/
TBool TSwDirectGdiArc::SingleStep(TPoint& aTopLeft,TBool& aDoTl,TPoint& aTopRight,
					   TBool& aDoTr,TPoint& aBottomLeft,TBool& aDoBl,
					   TPoint& aBottomRight,TBool& aDoBr)
	{
	TBool finished=TSwDirectGdiEllipse::SingleStep(aTopLeft,aTopRight,aBottomLeft,aBottomRight);
	Step(aTopLeft,aDoTl,aTopRight,aDoTr,aBottomLeft,aDoBl,aBottomRight,aDoBr);

	return finished;
	}

/**
Determines how many quadrants are left to draw.

@param aTopLeft Top left corner of rectangle.
@param aDoTl Span the top left quadrant.
@param aTopRight Top right corner of rectangle.
@param aDoTr Span the top right quadrant.
@param aBottomLeft Bottom left corner of rectangle.
@param aDoBl Span the bottom left quadrant.
@param aBottomRight Bottom right corner of rectangle.
@param aDoBr Span the bottom right quadrant.
@see TSwDirectGdiArc::SingleStep
@see TSwDirectGdiArc::Construct
*/
void TSwDirectGdiArc::Step(TPoint& aTopLeft,TBool& aDoTl,TPoint& aTopRight,TBool& aDoTr,
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
@see MDirectGdiEngine::DrawArc()
*/
void CSwDirectGdiEngine::DrawArc(const TRect& aRect,const TPoint& aStart,
								 const TPoint& aEnd)
	{
	TRect rcpy(aRect);
	rcpy.Move(iOrigin);
	TruncateRect(rcpy);
	TRect targetRect(rcpy);
	targetRect.Grow((iPenSize.iWidth>>1)+1,(iPenSize.iHeight>>1)+1);
	PieArcOutline(rcpy,aStart+iOrigin,aEnd+iOrigin,EFalse);
	}

/**
@see MDirectGdiEngine::DrawPie()
*/
void CSwDirectGdiEngine::DrawPie(const TRect& aRect,const TPoint& aStart,
								 const TPoint& aEnd)
	{
	TRect rcpy(aRect);
	rcpy.Move(iOrigin);
	TruncateRect(rcpy);
	TPoint startIntersect = aStart + iOrigin, endIntersect = aEnd + iOrigin;
	TInt startQuadrant, endQuadrant;
	TBool quadrants[5];
	const TBool isEllipse = AnalyseEllipse(
		rcpy, startIntersect, endIntersect, startQuadrant, endQuadrant, quadrants);

	if (iBrushStyle!=DirectGdi::ENullBrush)
		{
		if (isEllipse)
			EllipseFill(rcpy);
		else
			PieFill(rcpy, startIntersect, endIntersect, startQuadrant, endQuadrant, quadrants);
		}
	if ((iPenStyle!=DirectGdi::ENullPen) && (iPenSize.iWidth>0) && (iPenSize.iHeight>0))
		PieArcOutline(rcpy,aStart+iOrigin,aEnd+iOrigin,ETrue);
	}

/**
Calculates which quadrants are completely filled and which quadrants contain the 
start and end points.

@param aRect      The bounding rectangle. 
@param aStart     The arc start point.
@param aEnd	      The arc end point.
@param aStartQuad On return, contains the quadrant which contains aStart. 
@param aEndQuad   On return, contains the quadrant which contains aEnd.
@param aQuads     On return, populates an array of full quadrants to fill.
@pre Input params aRect, aStart, aEnd are to be given.
@post Output params aStart, aEnd, aStartQuad, aEndQuad, aQuads will be populated.
@return ETrue if the pie is an ellipse, otherwise EFalse.
*/
TBool CSwDirectGdiEngine::AnalyseEllipse(const TRect& aRect,TPoint& aStart,TPoint& aEnd,
							   TInt& aStartQuad,TInt& aEndQuad,TBool* aQuads)
	{
	aStartQuad=0;
	aEndQuad=0;
	const TPoint center = aRect.Center();
	TSwDirectGdiEllipse ellipse;
	aStart=ellipse.Intersection(aRect,aStart);
	aEnd=ellipse.Intersection(aRect,aEnd);
	if (aStart==aEnd)
		{
		aQuads[0]=EFalse;
		aQuads[1]=ETrue;
		aQuads[2]=ETrue;
		aQuads[3]=ETrue;
		aQuads[4]=ETrue;
		return ETrue;
		}
	const TInt startx = aStart.iX - center.iX, starty = aStart.iY - center.iY;
	const TInt endx = aEnd.iX - center.iX, endy = aEnd.iY - center.iY;
	if (startx>=0) aStartQuad=1;
	if (starty>=0) aStartQuad+=2;
	if (endx>=0) aEndQuad=1;
	if (endy>=0) aEndQuad+=2;
	aQuads[1]=EFalse,aQuads[2]=EFalse,aQuads[3]=EFalse,aQuads[4]=EFalse; // complete quadrants to draw
	aQuads[0]=EFalse; // ellipse is a sliver completely within a quadrant
	if (aStartQuad==aEndQuad)
		{
		if (aStartQuad==0 && (startx<endx || starty>endy))
			{
			aQuads[2]=ETrue;
			aQuads[3]=ETrue;
			aQuads[4]=ETrue;
			}
		else if (aStartQuad==1 && (startx<endx || starty<endy))
			{
			aQuads[1]=ETrue;
			aQuads[3]=ETrue;
			aQuads[4]=ETrue;
			}
		else if (aStartQuad==2 && (startx>endx || starty>endy))
			{
			aQuads[1]=ETrue;
			aQuads[2]=ETrue;
			aQuads[4]=ETrue;
			}
		else if (aStartQuad==3 && (startx>endx || starty<endy))
			{
			aQuads[1]=ETrue;
			aQuads[2]=ETrue;
			aQuads[3]=ETrue;
			}
		else aQuads[0]=ETrue; // "slice"
		}
	else
		{
		if (aStartQuad==0 && aEndQuad==1)
			{
			aQuads[3]=ETrue;
			aQuads[4]=ETrue;
			}
		else if (aStartQuad==0 && aEndQuad==3)
			aQuads[3]=ETrue;
		else if (aStartQuad==1 && aEndQuad==2)
			aQuads[1]=ETrue;
		else if (aStartQuad==1 && aEndQuad==3)
			{
			aQuads[1]=ETrue;
			aQuads[3]=ETrue;
			}
		else if (aStartQuad==2 && aEndQuad==0)
			{
			aQuads[2]=ETrue;
			aQuads[4]=ETrue;
			}
		else if (aStartQuad==2 && aEndQuad==1)
			aQuads[4]=ETrue;
		else if (aStartQuad==3 && aEndQuad==0)
			aQuads[2]=ETrue;
		else if (aStartQuad==3 && aEndQuad==2)
			{
			aQuads[1]=ETrue;
			aQuads[2]=ETrue;
			}
		}
	return EFalse;
	}

/**
Draws the outline of a pie or arc.

@param aRect  The bounding rectangle. 
@param aStart The arc start point.
@param aEnd	  The arc end point.
@param aPie If this is the outline for a pie ETrue, else EFalse
*/
void CSwDirectGdiEngine::PieArcOutline(const TRect& aRect,const TPoint& aStart,
							  const TPoint& aEnd,TBool aPie)
	{
	// arc runs counterclockwise, from startradius-center/ellipse intersect to endradius-center/ellipse intersect
	TInt dotparam=iDotParam;
	TRect rcpy(aRect);
	TPoint pt[4];
	TInt halfpenwidth=iPenSize.iWidth>>1;
	TInt halfpenheight=iPenSize.iHeight>>1;
	TInt otherhalfwidth=(iPenSize.iWidth+1)>>1;
	TInt otherhalfheight=(iPenSize.iHeight+1)>>1;
	rcpy.iTl.iX-=halfpenwidth;
	rcpy.iTl.iY-=halfpenheight;
	rcpy.iBr.iX+=otherhalfwidth;
	rcpy.iBr.iY+=otherhalfheight;
	TRect clipRect(0,0,0,0);
	for(TInt count=0;count<iDefaultRegionPtr->Count();count++)
		{
		clipRect=(*iDefaultRegionPtr)[count];
		if (!clipRect.Intersects(rcpy))
			continue;
		clipRect.Intersection(rcpy);
		TSwDirectGdiArc arc;
		arc.Construct(aRect,aStart,aEnd);
		iDotParam=Max(iPenSize.iWidth>>1,iPenSize.iHeight>>1);
		TBool dotl,dotr,dobl,dobr;
		while(!arc.SingleStep(pt[0],dotl,pt[1],dotr,pt[2],dobl,pt[3],dobr))
			{
			if (dotl) PenDrawClipped(pt[0], clipRect);
			if (dotr) PenDrawClipped(pt[1], clipRect);
			if (dobl) PenDrawClipped(pt[2], clipRect);
			if (dobr) PenDrawClipped(pt[3], clipRect);
			iDotParam+=iDotDirection;
			}
		if (pt[0].iY==pt[2].iY)
			{
			if (dotl) PenDrawClipped(pt[0], clipRect);
			if (dotr) PenDrawClipped(pt[1], clipRect);
			}
		if (aPie)
			{
			TPoint temp;
			const TPoint center = aRect.Center();
			TLinearDDA line;
			line.Construct(arc.iStart,center);
			line.SingleStep(temp);
			while(!line.SingleStep(temp))
				{
				PenDrawClipped(temp, clipRect);
				iDotParam+=iDotDirection;
				}
			line.Construct(arc.iEnd,center);
			line.SingleStep(temp);
			while(!line.SingleStep(temp))
				{
				PenDrawClipped(temp, clipRect);
				iDotParam+=iDotDirection;
				}
			PenDrawClipped(center, clipRect);
			}
		iDrawDevice->UpdateRegion(clipRect);
		}
	iDotParam=dotparam;
	}

/**
Fills a pie.

@param aRect      The bounding rectangle. 
@param aStart     The arc start point.
@param aEnd	      The arc end point.
@param aStartQuad The quadrant which contains aStart. 
@param aEndQuad   The quadrant which contains aEnd.
@param aQuads     An array of full quadrants to fill.
@see CSwDirectGdiEngine::DrawPie
@see CSwDirectGdiEngine::AnalyseEllipse 
*/
void CSwDirectGdiEngine::PieFill(const TRect& aRect, const TPoint& aStart, const TPoint& aEnd,
						TInt aStartQuad, TInt aEndQuad, const TBool* aQuads)
	{
	// arc runs counterclockwise, from startradius-center/ellipse intersect to endradius-center/ellipse intersect
	const TInt limit = iDefaultRegionPtr->Count();
	TRect clipRect(0,0,0,0);
	for(TInt count=0;count<limit;count++)
		{
		clipRect=(*iDefaultRegionPtr)[count];
		if (!clipRect.Intersects(aRect))
			{
			continue;
			}
		clipRect.Intersection(aRect);
		if (!aQuads[0])
			{
			PieShell(aRect, aStart, aEnd, aQuads, aStartQuad, aEndQuad, clipRect);
			}
		else
			{
			PieSliver(aRect, aStart, aEnd, aStartQuad, clipRect);
			}
		iDrawDevice->UpdateRegion(clipRect);
		}
	}

/**
Draws a pie spanning one or more quadrants.

@param aRect      The bounding rectangle. 
@param aStart     The arc start point.
@param aEnd	      The arc end point.
@param aQuads     Array of full quadrants to fill.
@param aStartQuad The quadrant which contains aStart. 
@param aEndQuad   The quadrant which contains aEnd.
@param aClipRect  The rectangle to which the pie is clipped.
*/
void CSwDirectGdiEngine::PieShell(const TRect& aRect,const TPoint& aStart,
						 const TPoint& aEnd, const TBool* aQuads, TInt aStartQuad, 
						 TInt aEndQuad, TRect aClipRect)
	{
	TSwDirectGdiEllipse ellipse;
	ellipse.Construct(aRect);
	TInt c=aRect.Center().iX;
	TPoint pt[4];
	TPoint tl,tr,bl,br;
	TBool donestart=EFalse;
	TBool doneend=EFalse;
	TBool todostart=EFalse;
	TBool todoend=EFalse;
	while(!ellipse.NextStep(pt[0],pt[1],pt[2],pt[3]))
		{
		if (pt[aStartQuad].iY==aStart.iY)
			todostart=ETrue;
		if (pt[aEndQuad].iY==aEnd.iY)
			todoend=ETrue;
		pt[0].iX++;
		pt[1].iX--;
		pt[2].iX++;
		pt[3].iX--;
		tl.iY=pt[0].iY;
		tr.iY=pt[1].iY;
		bl.iY=pt[2].iY;
		br.iY=pt[3].iY;
		if (aQuads[1] || (aEndQuad==0 && !doneend)) // tl quadrant, half top end chord
			tl.iX=pt[0].iX;
		else tl.iX=c+1;
		if (aQuads[2] || (aStartQuad==1 && !donestart)) // tr quadrant, half top start chord
			tr.iX=pt[1].iX;
		else tr.iX=c;
		if (aQuads[3] || (aStartQuad==2 && !donestart)) // bl quadrant, half top start chord
			bl.iX=pt[2].iX;
		else bl.iX=c+1;
		if (aQuads[4] || (aEndQuad==3 && !doneend)) // br quadrant, half top end chord
			br.iX=pt[3].iX;
		else br.iX=c;
		ClipFillLine(tl,tr,aClipRect);
		ClipFillLine(bl,br,aClipRect);
		// do partial quadrants
		if (todostart)
			{
			if (aStartQuad==0)
				{
				tl.iX=pt[0].iX;
				tr.iX=aStart.iX;
				ClipFillLine(tl,tr,aClipRect);
				}
			else if (aStartQuad==3)
				{
				bl.iX=aStart.iX;
				br.iX=pt[3].iX;
				ClipFillLine(bl,br,aClipRect);
				}
			}
		if (todoend)
			{
			if (aEndQuad==2)
				{
				bl.iX=pt[2].iX;
				br.iX=aEnd.iX;
				ClipFillLine(bl,br,aClipRect);
				}
			else if (aEndQuad==1)
				{
				tl.iX=aEnd.iX;
				tr.iX=pt[1].iX;
				ClipFillLine(tl,tr,aClipRect);
				}
			}
		donestart=todostart;
		doneend=todoend;
		}
	tl.iX=c+1;
	tr.iX=c;
	if (pt[0].iY==pt[2].iY) // congruent mid lines
		{
		if (pt[aStartQuad].iY==aStart.iY)
			todostart=ETrue;
		if (pt[aEndQuad].iY==aEnd.iY)
			todoend=ETrue;
		pt[0].iX++;
		pt[1].iX--;
		tl.iY=pt[0].iY;
		tr.iY=tl.iY;
		TBool leftflag=EFalse;
		TBool rightflag=EFalse;
		if (aQuads[1] || (aEndQuad==0 && !doneend) ||
			aQuads[3] || (aStartQuad==2 && !donestart) ||
			(todostart && aStartQuad==0) || (todoend && aEndQuad==2))
			leftflag=ETrue;
		if (aQuads[2] || (aStartQuad==1 && !donestart) ||
			aQuads[4] || (aEndQuad==3 && !doneend) ||
			(todostart && aStartQuad==3) || (todoend && aEndQuad==1))
			rightflag=ETrue;
		if (leftflag) tl.iX=pt[0].iX;
		if (rightflag) tr.iX=pt[1].iX;
		ClipFillLine(tl,tr,aClipRect);
		}
	else
		{
		tl.iY=aRect.Center().iY;
		tr.iY=tl.iY;
		if (aStartQuad==3) tr.iX=aStart.iX-1;
		if (aEndQuad==2) tl.iX=aEnd.iX+1;
		ClipFillLine(tl,tr,aClipRect);
		}
	PieTriangles(aStartQuad==1 || aStartQuad==2,aStart,aRect.Center(), aClipRect);
	PieTriangles(aEndQuad==0 || aEndQuad==3,aEnd,aRect.Center(), aClipRect);
	}

/**
Draws a filled triangle which forms part of a partially filled quadrant.

@param aInside A boolean value indicating which side of the line to fill.
@param aStart The start of the line which forms the hypotenuse of the triangle.
@param aEnd The end of the line which forms the hypotenuse of the triangle.
@param aClipRect The rectangle to which the pie is clipped.
@see CSwDirectGdiEngine::PieShell 
*/
void CSwDirectGdiEngine::PieTriangles(TBool aInside,const TPoint& aStart,const TPoint& aEnd, TRect aClipRect)
	{
	TInt x=aInside?aEnd.iX:aStart.iX;
	if (aStart.iX>aEnd.iX)
		{
		if (aInside)
			{
			x++;
			}
		else
			{
			x--;
			}
		}
	else
		{
		if (!aInside)
			{
			x++;
			}
		}
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
		ClipFillLine(left,right,aClipRect);
		}
	}

/**
Draws a pie entirely contained in one quadrant.

@param aRect The rectangle in which to draw the ellipse. 
@param aStart The arc start point.
@param aEnd	The arc end point.
@param aQuad The quadrant containing the pie.
@param aClipRect The rectangle to which the pie is clipped.
*/
void CSwDirectGdiEngine::PieSliver(const TRect& aRect,const TPoint& aStart,
						  const TPoint& aEnd, TInt aQuad, TRect aClipRect)
	{
	TPoint center=aRect.Center(),left,right;
	TPoint nearinter(aStart),farinter(aEnd);
	if (Abs(nearinter.iY-center.iY)>Abs(farinter.iY-center.iY))
		{
		nearinter=aEnd;
		farinter=aStart;
		}
	TBool ellipseComplete = EFalse;
	TPoint pt[4];
	TSwDirectGdiEllipse ellipse;
	ellipse.Construct(aRect);
	TLinearDDA mainline;
	mainline.Construct(farinter,center);
	ellipseComplete = ellipse.SingleStep(pt[0],pt[1],pt[2],pt[3]);
	mainline.SingleStep(right);
	do	{
		while(!ellipseComplete && pt[aQuad].iY!=right.iY)
			ellipseComplete = ellipse.NextStep(pt[0],pt[1],pt[2],pt[3]);
		left=pt[aQuad];
		while(!ellipseComplete && pt[aQuad].iY==right.iY)
			{
			left=pt[aQuad];
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
			ClipFillLine(left,right,aClipRect);
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
			ClipFillLine(left,right,aClipRect);
		}
	while(!mainline.NextStep(right));
	}

