// Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <gdi.h>

#ifdef __ARMCC__
#pragma arm
#pragma O3
#pragma Otime
#endif

EXPORT_C TLinearDDA::TLinearDDA():
	iCount(0),
	iDifference(),
	iFinish(),
	iGradient(0),
	iInc(),
	iPos(),
	iStart(),
	iBoundingRect(),
	iBoundingRectSet(EFalse),
	iInsideX(EFalse),
	iInsideY(EFalse),
	iStatus(EComplete)
/** Constructs the default linear DDA.

No start or end point is defined for the line. */
	{}

 
EXPORT_C TLinearDDA::TLinearDDA(const TLinearDDA& aLine):
	iCount(aLine.iCount),
	iDifference(aLine.iDifference),
	iFinish(aLine.iFinish),
	iGradient(aLine.iGradient),
	iInc(aLine.iInc),
	iPos(aLine.iPos),
	iStart(aLine.iStart),
	iBoundingRect(aLine.iBoundingRect),
	iBoundingRectSet(aLine.iBoundingRectSet),
	iInsideX(aLine.iInsideX),
	iInsideY(aLine.iInsideY),
	iStatus(aLine.iStatus)
/** Copy constructs a linear DDA from the specified linear DDA.

@param aLine The linear DDA to be copied. */
	{}

 
EXPORT_C void TLinearDDA::Construct(const TPoint& aStart,const TPoint& aFinish,TLineMode aMode)
/** Constructs a linear DDA, setting the start and end points of the line.

@param aStart The start point of the line. 
@param aFinish The end point of the line. 
@param aMode The mode of the line; defaults to centred. */
	{
	iStart=aStart;
	iFinish=aFinish;
	iDifference=(iFinish-iStart).AsSize();
	iDifference.iWidth=Abs(iDifference.iWidth);
	iDifference.iHeight=Abs(iDifference.iHeight);
	iInc.iX=(iStart.iX>iFinish.iX)?-1:1;
	iInc.iY=(iStart.iY>iFinish.iY)?-1:1;
	if(iDifference.iWidth)
		iGradient=(iFinish.iY-iStart.iY)/(iFinish.iX-iStart.iX);
	iPos=iStart;
	if(!iGradient)
		iCount=iDifference.iWidth;
	else
		iCount=iDifference.iHeight;
	if(aMode==ECenter)
		iCount>>=1;
	else
		if(iCount)
			iCount--;
	iStatus=EInitialised;
	if(aStart==aFinish)
		iStatus=EComplete;
	iBoundingRectSet=EFalse;
	iInsideX = EFalse;
	iInsideY = EFalse;
	}

 
EXPORT_C TBool TLinearDDA::SingleStep(TPoint& aPosition)
/** Gets the pixel co-ordinates of the next pixel on the pixel line.

The function is called repeatedly until the whole line has been traversed or, 
if JumpToRect() has been called, until the part of the line inside the rectangle 
has been traversed. Note that, for performance reasons, JumpToRect() may fail 
to detect the intersection of the line with the rectangle accurately and 
SingleStep() may return more points than strictly necessary.

@param aPosition On entry to the first call, this can be a reference to any 
point. On return from the first call, this is the position of the first pixel 
in the line, as specified during construction of this object. On return from 
subsequent calls, this is the position of subsequent pixels in the line, as 
calculated by the function. On return from the final call, this is the position 
of the last pixel in the line, as specified during construction of this object.
@return ETrue, when the position of the last pixel is returned; EFalse, 
otherwise. */
    {
    switch (iStatus)
        {
        case EInitialised:
            aPosition = iStart;
            iStatus = ECurrent;
            return EFalse;
        case ECurrent:
            if (iDifference.iHeight == 0) // horizontal line
                {
                iPos.iX += iInc.iX;
                if (iPos.iX == iFinish.iX)
                    {
                    iStatus = EComplete;
                    }
                }
            else if (iDifference.iWidth == 0) // vertical line
                {
                iPos.iY += iInc.iY;
                if (iPos.iY == iFinish.iY)
                    {
                    iStatus = EComplete;
                    }
                }
            else // diagonal stripes
                {
                if (!iGradient)
                    {
                    iCount -= iDifference.iHeight;
                    iPos.iX += iInc.iX;
                    if (iCount < 0)
                        {
                        iCount += iDifference.iWidth;
                        iPos.iY += iInc.iY;
                        }
                    }
                else
                    {
                    iCount -= iDifference.iWidth;
                    iPos.iY += iInc.iY;
                    if (iCount < 0)
                        {
                        iCount += iDifference.iHeight;
                        iPos.iX += iInc.iX;
                        }
                    }
                if ((iPos.iX == iFinish.iX) && (iPos.iY == iFinish.iY))
                    {
                    iStatus = EComplete;
                    }
                }
            // common
            aPosition = iPos;
            if (iStatus == EComplete)
                {
                return ETrue;
                }
            if(iBoundingRectSet)
                {
                if (iPos.iX >= iBoundingRect.iTl.iX && iPos.iX < iBoundingRect.iBr.iX)
                    iInsideX = ETrue;
                else
                    if (iInsideX)
                        {
                        iStatus=EComplete;
                        return(ETrue);
                        }
                if (iPos.iY >= iBoundingRect.iTl.iY && iPos.iY < iBoundingRect.iBr.iY)
                    iInsideY = ETrue;
                else
                    if (iInsideY)
                        {
                        iStatus=EComplete;
                        return(ETrue);
                        }
                }
            return EFalse;
        default:
            aPosition = iFinish;
            return ETrue;
        }
    }
 
EXPORT_C TBool TLinearDDA::NextStep(TPoint& aPosition)
/** Gets the pixel co-ordinates of the start of the next scan line.

The best line that joins the start and end points is formed from all the scan 
lines returned by this function.

The function is called repeatedly until the start position of all scanlines 
has been returned.

The start and end points passed to the constructor of this object define the 
boundaries of the line. Successive scan lines move from the start point to 
the end point.

@param aPosition On entry to the first call, this can be a reference to any 
point. On return from the first call, this is the position of the pixel that 
defines the leftmost position of the first scan line. On return from subsequent 
calls, this is the position of the pixel that defines the leftmost position 
of the next scan line. On return from the final call, this is the position 
of the last pixel in the line, as specified during construction. 
@return ETrue, when the position of the last pixel is returned; EFalse, 
otherwise. */
    {
    if (!iDifference.iHeight) // horizontal line
        {
        iPos = iFinish;
        iStatus = EComplete;
        aPosition = iFinish;
        return ETrue;
        }
    if (!iDifference.iWidth || iGradient || (iStatus != ECurrent))
        {
        return SingleStep(aPosition);
        }
    // !iGradient && (iStatus != EInitialised)
    if(iBoundingRectSet)
        { // slower version
        while ((iCount - iDifference.iHeight) >= 0)
            {
            if (SingleStep(aPosition))
                return ETrue;
            }
        return SingleStep(aPosition);
        }
    // faster version avoids function calls
    TBool lastLoop = EFalse;
    do {
        if ((iCount - iDifference.iHeight) < 0)
            {
            lastLoop = ETrue;
            }
        iCount -= iDifference.iHeight;
        iPos.iX += iInc.iX;
        if (iCount < 0)
            {
            iCount += iDifference.iWidth;
            iPos.iY += iInc.iY;
            }

        if ((iPos.iX == iFinish.iX) && (iPos.iY == iFinish.iY))
            {
            aPosition = iFinish;
            iStatus = EComplete;
            return ETrue;
            }
        }
        while (!lastLoop);
    aPosition = iPos;
    return EFalse;
    }
 
EXPORT_C TBool TLinearDDA::SingleScanline(TPoint& aStartPosition,TPoint& aEndPosition)
/** Gets the start and end pixel co-ordinates that define the next scan line.

The best line that joins the start and end points is formed from all the scan 
lines returned by this function.

The function is called repeatedly until the position of all scanlines has 
been returned.

The start and end points passed to the constructor of this object define the 
boundaries of the line. Successive scan lines move from the start point to 
the end point.

@param aStartPosition On entry to the first call, this can be a reference 
to any point. On return from the first call, this is the position of the pixel 
that defines the leftmost position of the first scan line. On return from 
subsequent calls, this is the position of the pixel that defines the leftmost 
position of the next scan line. On return from the final call, either this 
or aEndPosition is set to the end point, as specified during construction.
@param aEndPosition On entry to the first call, this can be a reference to 
any point. On return from the first call, this is the position of the pixel 
that defines the rightmost position of the first scan line. On return from 
subsequent calls, this is the position of the pixel that defines the rightmost 
position of the next scan line. On return from the final call, either this 
or aStartPosition is set to the end point, as specified during construction.
@return ETrue, when the position of the last scan line includes the end point; 
EFalse, otherwise. */
    {
    TBool done=EFalse;
    if(iDifference.iHeight==0)
        {
        aStartPosition=iStart;
        aEndPosition=iFinish;
        return(ETrue);
        }
    if(iDifference.iWidth==0 || iGradient)
        {
        done=SingleStep(aStartPosition);
        aEndPosition=aStartPosition;
        return(done);
        }
    // !iGradient
    done=SingleStep(aStartPosition);
    aEndPosition=aStartPosition;
    while(iCount-iDifference.iHeight>=0 && !done)
        {
        iCount -= iDifference.iHeight;
        iPos.iX += iInc.iX;
        if (iCount < 0)
            {
            iCount += iDifference.iWidth;
            iPos.iY += iInc.iY;
            }

        if ((iPos.iX == iFinish.iX) && (iPos.iY == iFinish.iY))
            {
            iStatus = EComplete;
            done = ETrue;
            }
        }
    aEndPosition = iPos;
    return done;
    }
 
EXPORT_C void TLinearDDA::JumpToRect(const TRect& aRect)
/** Jumps to start of a clipping rectangle.

This will accelerate the linear DDA to the vicinity of the specified rectangle. 
It is NOT guaranteed to reach the rectangle, but will reduce co-ordinates 
that are 1000's out to co-ordinates that are 10's out. Because of this, failure 
to intersect the rectangle may not be detected. If it is, or the line has 
not been constructed or has been run to completion, then a subsequent call 
to the stepping functions returns ETrue.

@param aRect The rectangle to be jumped to. */
	{
	if(aRect.IsEmpty() || iStatus!=EInitialised) return;
	iBoundingRect=aRect;
	iBoundingRectSet=ETrue;

	TInt nearestx = 0;
	if (iStart.iX < aRect.iTl.iX)
		nearestx = aRect.iTl.iX;
	else if (iStart.iX >= aRect.iBr.iX)
		nearestx = aRect.iBr.iX;
	else
		iInsideX = ETrue;
	TInt nearesty = 0;
	if (iStart.iY < aRect.iTl.iY)
		nearesty = aRect.iTl.iY;
	else if (iStart.iY >= aRect.iBr.iY)
		nearesty = aRect.iBr.iY;
	else
		iInsideY = ETrue;
	if (iInsideX && iInsideY)
		return;

	TInt dummy;
	if(!iGradient)
		{
		if (iInsideX)
			return;
		JumpToXCoord(nearestx,dummy);
		}
	else
		{
		if (iInsideY)
			return;
		JumpToYCoord(dummy,nearesty);
		}
	}

 
EXPORT_C void TLinearDDA::JumpToXCoord(const TInt aXCoord,TInt& aYCoord)
/** Jumps to x co-ordinate.

The other co-ordinate of the intersection is returned through a reference 
argument. After a jump call, the line is ready to continue through calls to 
the stepping functions.

This function accelerates the Linear DDA stepping functions (e.g. SingleStep()) 
making them return EFalse when they reach the specified co-ordinate. If the 
line does not cross the co-ordinate, has not been constructed, has been run 
to completion or the intersection is the end point of the line then the stepping 
functions will return ETrue.

@param aXCoord x co-ordinate to jump to 
@param aYCoord On return, this parameter holds the y co-ordinate which corresponds 
to the specified x co-ordinate */
	{
	if(iStatus==EComplete) return; // not constructed
	if((iStart.iX<aXCoord && iFinish.iX<aXCoord) || (iStart.iX>aXCoord && iFinish.iX>aXCoord))
		return; // no intersection
	aYCoord=iStart.iY;
	if(iStart.iX==aXCoord) return; // trivial first intersection
	iStatus=ECurrent;
	if(iDifference.iHeight==0) // horizontal line
		iPos.iX=aXCoord;
	else
		{
		if(!iGradient)
			{
			TInt64 numsteps=Abs(aXCoord-iPos.iX);
			TInt64 tempcount=TInt64(iCount)-(TInt64(iDifference.iHeight)*numsteps);
			numsteps=Abs(tempcount/iDifference.iWidth);
			tempcount+=numsteps*iDifference.iWidth;
			while(tempcount<0)
				{
				tempcount+=iDifference.iWidth;
				numsteps++;
				}
			iCount = I64INT(tempcount);
			iPos.iY += (iInc.iY * I64INT(numsteps));
			iPos.iX=aXCoord;
			aYCoord=iPos.iY;
			}
		else
			{
			while(iPos.iX!=aXCoord)
				{
				iCount-=iDifference.iWidth;
				if(iCount<0)
					{
					iCount+=iDifference.iHeight;
					iPos.iX+=iInc.iX;
					}
				iPos.iY+=iInc.iY;
				}
			aYCoord=iPos.iY;
			}
		}
    if ((iPos.iX == iFinish.iX) && (iPos.iY == iFinish.iY))
        {
        iStatus=EComplete;
        }
	}

 
EXPORT_C void TLinearDDA::JumpToYCoord(TInt& aXCoord,const TInt aYCoord)
/** Jumps to a y co-ordinate.

The other co-ordinate of the intersection is returned through a reference 
argument. After a jump call, the line is ready to continue through calls to 
the stepping functions.

This function accelerates the Linear DDA stepping functions (e.g. SingleStep()) 
making them return EFalse when they reach the specified co-ordinate. If the 
line does not cross the co-ordinate, has not been constructed, has been run 
to completion or the intersection is the end point of the line then they will 
return ETrue. 

@param aXCoord On return, this parameter holds the x co-ordinate which corresponds 
to the specified y co-ordinate. 
@param aYCoord y co-ordinate to jump to */
	{
	if(iStatus==EComplete) return; // not constructed
	if((iStart.iY<aYCoord && iFinish.iY<aYCoord) || (iStart.iY>aYCoord && iFinish.iY>aYCoord))
		return; // no intersection
	aXCoord=iStart.iX;
	if(iStart.iY==aYCoord) return; // trivial first intersection
	iStatus=ECurrent;
	if(iDifference.iWidth==0) // vertical line
		iPos.iY=aYCoord;
	else
		{
		if(!iGradient)
			{
			while(iPos.iY!=aYCoord)
				{
				iCount-=iDifference.iHeight;
				if(iCount<0)
					{
					iCount+=iDifference.iWidth;
					iPos.iY+=iInc.iY;
					}
				iPos.iX+=iInc.iX;
				}
			aXCoord=iPos.iX;
			}
		else
			{
			TInt64 numsteps=Abs(aYCoord-iPos.iY);
			TInt64 tempcount=TInt64(iCount)-(TInt64(iDifference.iWidth)*numsteps);
			numsteps=Abs(tempcount/iDifference.iHeight);
			tempcount+=numsteps*iDifference.iHeight;
			while (tempcount<0)
				{
				tempcount+=iDifference.iHeight;
				numsteps++;
				}
			iCount = I64INT(tempcount);
			iPos.iX += (iInc.iX * I64INT(numsteps));
			iPos.iY=aYCoord;
			aXCoord=iPos.iX;
			}
		}
    if ((iPos.iX == iFinish.iX) && (iPos.iY == iFinish.iY))
        {
        iStatus=EComplete;
        }
	}

void TLinearDDA::UpdatePosition()
	{
	}

EXPORT_C void TLinearDDA::JumpToXCoord2(TInt aXCoord,TInt& aYCoord)
/**
Jumps to x co-ordinate.

This works in the same way as TLinearDDA::JumpToXCoord except that it make sure
that using the NextStep function does not return the same value twice.

@param aXCoord x co-ordinate to jump to
@param aYCoord On return, this parameter holds the y co-ordinate which corresponds
to the specified x co-ordinate
@see TLinearDDA::JumpToXCoord(TInt, TInt&)
*/
	{
	JumpToXCoord(aXCoord,aYCoord);
	iStatus=ECurrent;
	}

EXPORT_C void TLinearDDA::JumpToYCoord2(TInt& aXCoord,TInt aYCoord)
/**
Jumps to a y co-ordinate.

This works in the same way as TLinearDDA::JumpToYCoord except that it make sure
that using the NextStep function does not return the same value twice.

@param aXCoord On return, this parameter holds the x co-ordinate which corresponds
to the specified y co-ordinate.
@param aYCoord y co-ordinate to jump to
@see TLinearDDA::JumpToYCoord(TInt&, TInt)
*/ 
	{
	JumpToYCoord(aXCoord,aYCoord);
	iStatus=ECurrent;
	}
