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

#include <e32test.h>
#include "TTYPES.H"

TestLine::TestLine(const TPoint& aStart,const TPoint& aFinish, CTTypes* aTest):
	iStart(aStart),
	iFinish(aFinish),
	iTest(aTest)
	{}

/** 
	TestLine::Test
	
	Method to test the functionality within the TLinearDDA class (Linear Digital Differential Analyser)
	Called from the TTypes test script
*/
void TestLine::Test()
	{
	iLine.Construct(iStart,iFinish);
	TestSingleStep();
	iLine.Construct(iStart,iFinish);
	TestSingleScanLine();
	iLine.Construct(iStart,iFinish);
	TestNextStep();
	iLine.Construct(iStart,iFinish);
	TestJumpToXCoord();
	iLine.Construct(iStart,iFinish);
	TestJumpToYCoord();
	iLine.Construct(iStart,iFinish);
	TestJumpToRect();
	}

/** 
	TestLine::TestSingleStep
	
	Iterate through a line of pixels & confirm the final call to SingleStep rests on the last pixel
*/
void TestLine::TestSingleStep()
	{
	TPoint point;
	while(!iLine.SingleStep(point))
		{
		iTest->TEST((point.iX>=iStart.iX && point.iX<=iFinish.iX) || (point.iX<=iStart.iX && point.iX>=iFinish.iX));
		iTest->TEST((point.iY>=iStart.iY && point.iY<=iFinish.iY) || (point.iY<=iStart.iY && point.iY>=iFinish.iY));
		}
	iTest->TEST(point==iFinish);
	}

/** 
	TestLine::TestSingleScanLine
	
	Iterate through each scaline & confirm successive scan lines are concatenated. 
	Confirm the last scanline holds the endppoint that was specified during construction of TLinearDDA
*/
void TestLine::TestSingleScanLine()
	{
	TPoint point1,point2;
	if(!iLine.SingleScanline(point1,point2))
		iTest->TEST(point1==iStart);
	TInt lastycoord=point1.iY;
	while(!iLine.SingleScanline(point1,point2))
		{
		iTest->TEST((point1.iX>=iStart.iX && point1.iX<=iFinish.iX) || (point1.iX<=iStart.iX && point1.iX>=iFinish.iX));
		iTest->TEST((point1.iY>=iStart.iY && point1.iY<=iFinish.iY) || (point1.iY<=iStart.iY && point1.iY>=iFinish.iY));
		iTest->TEST((point2.iX>=iStart.iX && point2.iX<=iFinish.iX) || (point2.iX<=iStart.iX && point2.iX>=iFinish.iX));
		iTest->TEST(point1.iY==point2.iY);
		iTest->TEST(Abs(point1.iY-lastycoord)==1);
		lastycoord=point1.iY;
		}
	iTest->TEST(point2==iFinish);
	}

/** 
	TestLine::TestNextStep
	
	Iterate trhough a the start position of each scanline & confirm they are concatenated. 
	Confirm the last scanline holds the endpoint specified during TLinearDDA construction (iFinish)
*/
void TestLine::TestNextStep()
	{
	TPoint point,oldpoint;
	iLine.NextStep(oldpoint);
	iTest->TEST(oldpoint==iStart);
	while(!iLine.NextStep(point))
		{
		iTest->TEST((point.iX>=iStart.iX && point.iX<=iFinish.iX) || (point.iX<=iStart.iX && point.iX>=iFinish.iX));
		iTest->TEST((point.iY>=iStart.iY && point.iY<=iFinish.iY) || (point.iY<=iStart.iY && point.iY>=iFinish.iY));
		iTest->TEST(Abs(point.iY-oldpoint.iY)==1);
		oldpoint=point;
		}
	iTest->TEST(point==iFinish);
	}

/** 
	TestLine::TestJumpToRect
	
	Test the JumpToRect functionality, confirming that the start of the clipping rectangle is correctly detected (note we iterate through SingleStep to find the position exactly)
*/
void TestLine::TestJumpToRect()
	{
	TPoint midpoint=iStart+iFinish;
	midpoint.iX>>=1;
	midpoint.iY>>=1;
	TSize rsize(Abs(iStart.iX-iFinish.iX),Abs(iStart.iY-iFinish.iY));
	TRect rect(TPoint(iFinish.iX-(rsize.iWidth>>1),iFinish.iY-(rsize.iHeight>>1)),rsize);
	TRect largerect(TPoint(iFinish.iX-(rsize.iWidth<<1),iFinish.iY-(rsize.iHeight<<1)),TSize(rsize.iWidth<<2,rsize.iHeight<<2));
	TRect offsetrect(rect);
	offsetrect.Move(iStart-iFinish);
	if(iFinish.iX>iStart.iX)
		offsetrect.Move(TPoint(-(rsize.iWidth>>2),0));
	else
		offsetrect.Move(TPoint(rsize.iWidth>>2,0));
	if(iFinish.iY>iStart.iY)
		offsetrect.Move(TPoint(0,(rsize.iHeight>>1)+4));
	else
		offsetrect.Move(TPoint(0,-(rsize.iHeight>>1)-4));
	TPoint intersect,point;
	TLinearDDA line;
	iLine.JumpToRect(rect);
	iLine.SingleStep(intersect);
	line.Construct(iStart,iFinish);
	while(!line.SingleStep(point))
		if(point==intersect)
			break;
	iTest->TEST(point==intersect);
	while(!iLine.SingleStep(intersect))
		{
		line.SingleStep(point);
		iTest->TEST(point==intersect);
		}
	line.SingleStep(point);
	iTest->TEST(point==intersect);
	iTest->TEST(intersect==iFinish);
	iLine.Construct(iStart,iFinish);
	iLine.JumpToRect(largerect);
	iLine.SingleStep(point);
	iTest->TEST(Abs(point.iX-iStart.iX)<=1);
	iTest->TEST(Abs(point.iY-iStart.iY)<=1);
	iLine.Construct(iStart,iFinish);
	iLine.JumpToRect(offsetrect);
	iLine.SingleStep(point);
	if(rsize.iWidth>rsize.iHeight)
		iTest->TEST(point==iStart);
	else
		iTest->TEST(Min(Abs(point.iY-offsetrect.iTl.iY),Abs(point.iY-offsetrect.iBr.iY))==1);
	}

/** 
	TestLine::TestJumpToXCoord
	
	Test the JumpToXCoord functionailty, confirming the positions by SingleStep calls
*/
void TestLine::TestJumpToXCoord()
	{
	TInt xc1=(iStart.iX+iFinish.iX)>>1;
	TInt xc2=iStart.iX;
	TInt xc3=iFinish.iX;
	TInt xc4=Min(iStart.iX,iFinish.iX)-Abs(xc1);
	TInt xc5=Max(iStart.iX,iFinish.iX)+Abs(xc1);
	TInt y=0;
	TPoint intersect,point;
	TLinearDDA line;
	iLine.JumpToXCoord(xc1,y);
	line.Construct(iStart,iFinish);
	while(!line.SingleStep(point))
		if(point.iX==xc1)
			break;
	iTest->TEST(y==point.iY);
	while(!iLine.SingleStep(point))
		{
		line.SingleStep(intersect);
		iTest->TEST(point.iX==intersect.iX);
		iTest->TEST(point.iY==intersect.iY);
		}
	TBool done=line.SingleStep(intersect);
	iTest->TEST(done);
	iLine.Construct(iStart,iFinish);
	iLine.JumpToXCoord(xc2,y);
	iTest->TEST(y==iStart.iY);
	iLine.Construct(iStart,iFinish);
	iLine.JumpToXCoord(xc3,y);
	iLine.SingleStep(point);
	iTest->TEST(point.iX==xc3);
	iLine.Construct(iStart,iFinish);
	iLine.JumpToXCoord(xc4,y);
	iLine.Construct(iStart,iFinish);
	iLine.JumpToXCoord(xc5,y);
	}

/** 
	TestLine::TestJumpToYCoord
	
	Test the JumpToYCoord functionailty, confirming the positions by SingleStep calls
*/
void TestLine::TestJumpToYCoord()
	{
	TInt yc1=(iStart.iY+iFinish.iY)>>1;
	TInt yc2=iStart.iY;
	TInt yc3=iFinish.iY;
	TInt yc4=Min(iStart.iY,iFinish.iY)-Abs(yc1);
	TInt yc5=Max(iStart.iY,iFinish.iY)+Abs(yc1);
	TInt x=0;
	TPoint intersect,point;
	TLinearDDA line;
	iLine.JumpToYCoord(x,yc1);
	line.Construct(iStart,iFinish);
	while(!line.SingleStep(point))
		if(point.iY==yc1)
			break;
	iTest->TEST(TPoint(x,yc1)==point);
	while(!iLine.SingleStep(point))
		{
		line.SingleStep(intersect);
		iTest->TEST(point==intersect);
		}
	TBool done=line.SingleStep(intersect);
	iTest->TEST(done);
	iLine.Construct(iStart,iFinish);
	iLine.JumpToYCoord(x,yc2);
	iTest->TEST(x==iStart.iX);
	iLine.Construct(iStart,iFinish);
	iLine.JumpToYCoord(x,yc3);
	iLine.SingleStep(point);
	iTest->TEST(point.iY==yc3);
	iLine.Construct(iStart,iFinish);
	iLine.JumpToYCoord(x,yc4);
	iLine.Construct(iStart,iFinish);
	iLine.JumpToYCoord(x,yc5);
	}

