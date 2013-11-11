// Copyright (c) 1996-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// GDI.CPP
// Test GDI functions
// 
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/

#include "TGDI.H"

#define TAUTO_BITMAP_NAME _L("Z:\\WSTEST\\TAUTO.MBM")

//#define LOGGING on		//Uncomment this line to get extra test logging - useful if debugging a failure of one of the tests

typedef CGdiTestLowLevel *(*GdiTestFunc)();

_LIT(KMonospaceTestFontTypefaceName,"Arial");
const TInt KMaxFontSize = 200;

static inline TBool IsLengthEqual(TInt aLLen, TInt aRLen, TInt aBbLen)
	{
// The bounding box of a 'w' is one pixel wider than it should be. W overlaps in to the bounding box of 'x' as the two  
// are stuck together. So add a tolerance of -1.
	return (((aLLen+aRLen)==aBbLen) || ((aLLen+aRLen-1)==aBbLen));
	}

struct GdiTest
	{
	GdiTestFunc func;
	const TText *txt;
	};

GdiTest GdiTestFuncs[]={
	{BoxTextTest,_S("BoxText")},
	{VertTextTest,_S("VertText")},
	/*{JustifiedTextTest,_S("JustifiedText")},*/ // DEF107985 The test for JustifiedText has been removed as it was a bad/useless test
	{VertBoxTextTest,_S("VertBoxText")},
	{SetOriginTest,_S("SetOrigin")},
	{PolyLineTest,_S("PolyLine")},
	{PolygonTest,_S("Polygon")},
	{DrawArcTest,_S("DrawArc")},
	{DrawPieTest,_S("DrawPie")},
	{DrawRoundRectTest,_S("DrawRoundRect")},
	{WideLineTest,_S("WideLine")},
	{DrawTextTest,_S("DrawText")},
	{DrawRectTest,_S("DrawRect")},
	};

CTestWindow::CTestWindow(TRgb aCol) : CBlankWindow(aCol)
	{
	}

CTestWindow::~CTestWindow()
	{
	}

void CTestWindow::ConstructL(TPoint aPos,TSize aSize,CTWinBase* aParent, CWindowGc& aGc)
	{
	iWin=RWindow(TheClient->iWs);
	User::LeaveIfError(iWin.Construct(*(aParent->WinTreeNode()),ENullWsHandle));
	SetExt(aPos,aSize);
	Activate();
	AssignGC(aGc);
	}

void CGdiTestLowLevel::SetGdiTest(CTGdi *aGdiTest)
	{
	iGdiTest=aGdiTest;
	}

void CGdiTestLowLevel::AssignGdi(CWindowGc *aGc)
	{
	iGdi=aGc;
	}

void CGdiTestLowLevel::ConstructL(TInt)
	{
	}

void CGdiTestLowLevel::PreTestSetupL(const TRect &,TInt)
	{
	}

//=======================================//
// Individual GDI low level test classes //
//=======================================//


void CGdiRect::ConstructL(TInt)
	{
	}

void CGdiRect::PreTestSetupL(const TRect &, TInt aCount)
	{
	iBrushCol=TRgb::Gray256(aCount&0x1 ? 0xFF : 0x60);
	iPenCol=TRgb::Gray256(aCount&0x1 ? 0xA0 : 0x0);
	}

TInt CGdiRect::Count(TBool )
	{
	return(1);
	}

void CGdiDrawRect::BaseTest(const TRect &aRect, TInt aCount)
	{
	TheTest(aRect,aCount);
	}

void CGdiDrawRect::TheTest(const TRect &aRect, TInt)
	{
	iGdi->SetBrushColor(iBrushCol);
	iGdi->SetPenColor(iPenCol);
	iGdi->DrawRect(aRect);
	}

TInt CGdiDrawRect::Count(TBool aMode)
	{
	return(aMode ? 4:1);
	}

TInt CGdiDrawRoundRect::Count(TBool aMode)
	{
	return(aMode ? 4:1);
	}

void CGdiDrawRoundRect::BaseTest(const TRect &aRect, TInt aCount)
	{
	TheTest(aRect,aCount);
	}

void CGdiDrawRoundRect::TheTest(const TRect &aRect, TInt aCount)
	{
	iGdi->SetBrushColor(iBrushCol);
	iGdi->SetPenColor(iPenCol);
	TSize size((aRect.iBr.iX-aRect.iTl.iX)/4,(aRect.iBr.iY-aRect.iTl.iY)/4);
	switch(aCount)
		{
		case 0:
			size.SetSize((aRect.iBr.iX-aRect.iTl.iX)/4,(aRect.iBr.iY-aRect.iTl.iY)/4);
			break;
		case 1:
			size.SetSize(0,0);
			break;
		case 2:
			size.SetSize((aRect.iBr.iX-aRect.iTl.iX)/2,(aRect.iBr.iY-aRect.iTl.iY)/2);
			break;
		case 3:
			size.SetSize((aRect.iBr.iX-aRect.iTl.iX),(aRect.iBr.iY-aRect.iTl.iY));
			break;
		}
	iGdi->DrawRoundRect(aRect,size);
	}

TInt CGdiDrawACP::Count(TBool )
	{
	return(1);
	}

void CGdiDrawACP::ConstructL(TInt)
	{
	}

void CGdiDrawACP::PreTestSetupL(const TRect &, TInt )
	{
	iBrushCol=TRgb::Gray256(0xff);
	iPenCol=TRgb(0,0,0);
	}

void CGdiDrawArc::BaseTest(const TRect &aRect, TInt aCount)
	{
	TheTest(aRect,aCount);
	}

void CGdiDrawArc::TheTest(const TRect &aRect, TInt)
	{
	iGdi->SetBrushColor(iBrushCol);
	iGdi->SetPenColor(iPenCol);
	TPoint start(aRect.iTl.iX/2+aRect.iBr.iX,aRect.iTl.iY/2+aRect.iBr.iY);
	iGdi->DrawArc(aRect,start,TPoint(0,0));
	}

void CGdiDrawPie::BaseTest(const TRect &aRect, TInt aCount)
	{
	TheTest(aRect,aCount);
	}

void CGdiDrawPie::TheTest(const TRect &aRect, TInt)
	{
	iGdi->SetBrushColor(iBrushCol);
	iGdi->SetPenColor(iPenCol);
	TPoint start(aRect.iTl.iX/2+aRect.iBr.iX,aRect.iTl.iY/2+aRect.iBr.iY);
	iGdi->DrawPie(aRect,start,TPoint(0,0));
	}

//
// Polyline tests
//

CGdiPolyLine::CGdiPolyLine() : iPnts(5)
	{}

void CGdiPolyLine::PreTestSetupL(const TRect &aRect, TInt aCount)
	{
	iPnts.Reset();
	TInt wid=aRect.Width();
	TInt hgt=aRect.Height();
	if (aCount==0)
		{
		TInt pos;
		for(pos=0;pos<wid;pos+=10)
			{
			iPnts.AppendL(TPoint(aRect.iTl.iX+pos,aRect.iTl.iY));
			iPnts.AppendL(TPoint(aRect.iTl.iX+pos+5,aRect.iBr.iY));
			}
		for(pos=0;pos<hgt;pos+=10)
			{
			iPnts.AppendL(TPoint(aRect.iTl.iX,aRect.iTl.iY+pos));
			iPnts.AppendL(TPoint(aRect.iBr.iX,aRect.iTl.iY+pos+5));
			}
		}
	else
		{
		TInt hgt=aRect.Height();
		iPnts.AppendL(TPoint(aRect.iTl.iX,aRect.iTl.iY));
		iPnts.AppendL(TPoint(aRect.iBr.iX,aRect.iTl.iY+hgt/2));
		iPnts.AppendL(TPoint(aRect.iTl.iX,aRect.iBr.iY));
		iPnts.AppendL(TPoint(aRect.iBr.iX,aRect.iBr.iY));
		iPnts.AppendL(TPoint(aRect.iTl.iX,aRect.iTl.iY+hgt/2));
		iPnts.AppendL(TPoint(aRect.iBr.iX,aRect.iTl.iY));
		iPnts.AppendL(TPoint(aRect.iTl.iX,aRect.iTl.iY));
		}
	}

void CGdiPolyLine::ConstructL(TInt )
	{
	}

TInt CGdiPolyLine::Count(TBool aMainTests)
	{
	return(aMainTests ? 2 : 1);
	}

void CGdiPolyLine::BaseTest(const TRect &, TInt )
	{
	TPoint prev;
	TInt index;
	for(index=0;index<iPnts.Count();index++)
		{
		if (index>0)
			iGdi->DrawLine(prev,iPnts[index]);
		prev=iPnts[index];
		}
	if (index>0)
		iGdi->Plot(iPnts[index-1]);
	}

void CGdiPolyLine::TheTest(const TRect &, TInt )
	{
	iGdi->DrawPolyLine(&iPnts);
	}

//
// Polygon tests //
//

CGdiPolygon::CGdiPolygon() : iPnts(5)
	{}

void CGdiPolygon::PreTestSetupL(const TRect &aRect, TInt )
	{
	iPnts.Reset();
	TPoint pnt;
	TInt maxPoints=Min((TInt)KMaxPolygonPoints,Min(aRect.Width(),aRect.Height())/KPolygonStep);
	TInt numPoints=0;
	if (maxPoints>0)
		{
		iPnts.AppendL(aRect.iTl);
		while(numPoints<maxPoints)
			{
			if (numPoints&1)
				pnt.SetXY(aRect.iTl.iX+(numPoints+1)*KPolygonStep,aRect.iTl.iY+numPoints*KPolygonStep);
			else
				pnt.SetXY(aRect.iTl.iX+numPoints*KPolygonStep,aRect.iTl.iY+(numPoints+1)*KPolygonStep);
			iPnts.AppendL(pnt);
			numPoints++;
			}
		pnt.SetXY(aRect.iTl.iX+numPoints*KPolygonStep,aRect.iTl.iY+numPoints*KPolygonStep);
		iPnts.AppendL(pnt);
		while(numPoints>0)
			{
			numPoints--;
			if (numPoints&1)
				pnt.SetXY(aRect.iTl.iX+numPoints*KPolygonStep,aRect.iTl.iY+(numPoints+1)*KPolygonStep);
			else
				pnt.SetXY(aRect.iTl.iX+(numPoints+1)*KPolygonStep,aRect.iTl.iY+numPoints*KPolygonStep);
			iPnts.AppendL(pnt);
			}
		}
	}

void CGdiPolygon::ConstructL(TInt )
	{
	}

TInt CGdiPolygon::Count(TBool aMainTests)
	{
	return(aMainTests ? 2 : 1);
	}

void CGdiPolygon::BaseTest(const TRect &aRect, TInt )
	{
	iGdi->SetPenColor(TRgb::Gray4(2));
	iGdi->SetBrushColor(TRgb::Gray4(1));
	iGdi->SetBrushStyle(CGraphicsContext::ESolidBrush);
	TInt maxPoints=Min((TInt)KMaxPolygonPoints,Min(aRect.Width(),aRect.Height())/KPolygonStep);
	for(TInt numPoints=0;numPoints<maxPoints;numPoints++)
		iGdi->DrawRect(TRect(aRect.iTl.iX+numPoints*KPolygonStep + 1,aRect.iTl.iY+numPoints*KPolygonStep + 1,
							 aRect.iTl.iX+(numPoints+1)*KPolygonStep ,aRect.iTl.iY+(numPoints+1)*KPolygonStep));
	}

void CGdiPolygon::TheTest(const TRect &, TInt )
//This is the only one that can leave so it's not worth adding an 'L'
	{
	iGdi->SetPenColor(TRgb::Gray4(2));
	iGdi->SetBrushColor(TRgb::Gray4(1));
	iGdi->SetBrushStyle(CGraphicsContext::ESolidBrush);
	User::LeaveIfError(iGdi->DrawPolygon(&iPnts,CGraphicsContext::EAlternate));
	}

//

TInt CGdiWideLine::Count(TBool aMainTests)
	{
	return(aMainTests ? EMainNumWidths : EMinorNumWidths);
	}

void CGdiWideLine::ConstructL(TInt)
	{
	}

void CGdiWideLine::PreTestSetupL(const TRect &, TInt aCount)
	{
	TInt widths[]={1,4,50,-2,
				   48,12345,0,3,
				   48,123,0,3,
				   4,111,555,1000,
				   10,-1,-10,-100};
	iWidth=widths[aCount];
	}

void CGdiWideLine::BaseTest(const TRect &aRect, TInt)
	{
	iGdi->SetPenSize(TSize(iWidth,iWidth));
	iGdi->DrawLine(aRect.iTl,aRect.iBr);
	iGdi->MoveTo(aRect.iTl+TPoint(0,10));
	iGdi->DrawLineBy(aRect.iBr-aRect.iTl);
	iGdi->MoveBy(aRect.iTl-aRect.iBr+TPoint(0,10));
	iGdi->DrawLineTo(aRect.iBr+TPoint(0,20));
	}

void CGdiWideLine::TheTest(const TRect &aRect, TInt)
	{
	iGdi->SetPenSize(TSize(iWidth,iWidth));
	iGdi->MoveTo(aRect.iTl);
	iGdi->DrawLineBy(aRect.iBr-aRect.iTl);
	iGdi->MoveBy(aRect.iTl-aRect.iBr+TPoint(0,10));
	iGdi->DrawLineTo(aRect.iBr+TPoint(0,10));
	iGdi->DrawLine(aRect.iTl+TPoint(0,20),aRect.iBr+TPoint(0,20));
	}

//
// Set Origin //
//

CGdiSetOrigin::~CGdiSetOrigin()
	{
	delete iPolylineArray;
	delete iPolygonArray;
	TheClient->iScreen->ReleaseFont(iFont);
	}

TInt CGdiSetOrigin::Count(TBool )
	{
	return(1);
	}

void CGdiSetOrigin::ConstructL(TInt)
	{
	iPolylineArray=new(ELeave) CArrayFixFlat<TPoint>(3);
	iPolylineArray->AppendL(TPoint(40,1));
	iPolylineArray->AppendL(TPoint(50,11));
	iPolylineArray->AppendL(TPoint(40,21));
	iPolygonArray=new(ELeave) CArrayFixFlat<TPoint>(3);
	iPolygonArray->AppendL(TPoint(40,30));
	iPolygonArray->AppendL(TPoint(50,40));
	iPolygonArray->AppendL(TPoint(40,45));
	TFontSpec fspec(KMonospaceTestFontTypefaceName,KMaxFontSize);
	User::LeaveIfError(TheClient->iScreen->GetNearestFontToDesignHeightInTwips((CFont *&)iFont,fspec));
	}

void CGdiSetOrigin::PreTestSetupL(const TRect &, TInt )
	{
	}

void CGdiSetOrigin::DrawIt(const TPoint &aOffset)
	{
	iGdi->DrawLine(TPoint(10,10)+aOffset,TPoint(20,30)+aOffset);
	iGdi->DrawLineBy(TPoint(-10,5));
	iGdi->DrawLineTo(TPoint(20,5)+aOffset);
	iGdi->MoveTo(TPoint(20,10)+aOffset);
	iGdi->DrawLineTo(TPoint(30,15)+aOffset);
	iGdi->MoveBy(TPoint(-10,2));
	iGdi->DrawLineBy(TPoint(10,0));
	iGdi->UseFont(iFont);
	iGdi->DrawText(_L("Set Origin Test"),TPoint(40,20)+aOffset);
	iGdi->DiscardFont();
	iGdi->DrawRect(TRect(TPoint(10,45)+aOffset,TPoint(20,55)+aOffset));
	iGdi->DrawRoundRect(TRect(TPoint(21,45)+aOffset,TPoint(30,55)+aOffset),TSize(3,2));
	iGdi->DrawEllipse(TRect(TPoint(31,45)+aOffset,TPoint(40,55)+aOffset));
	TInt index;
	for(index=0;index<iPolylineArray->Count();index++)
		(*iPolylineArray)[index]+=aOffset;
	iGdi->DrawPolyLine(iPolylineArray);
	for(index=0;index<iPolylineArray->Count();index++)
		(*iPolylineArray)[index]-=aOffset;
	for(index=0;index<iPolygonArray->Count();index++)
		(*iPolygonArray)[index]+=aOffset;
	iGdi->SetBrushStyle(CGraphicsContext::ESolidBrush);
	User::LeaveIfError(iGdi->DrawPolygon(iPolygonArray,CGraphicsContext::EAlternate));		//Doesn't cause any harm if it leaves
	for(index=0;index<iPolygonArray->Count();index++)
		(*iPolygonArray)[index]-=aOffset;
	}

void CGdiSetOrigin::BaseTest(const TRect &aRect, TInt)
	{
	DrawIt(aRect.iTl);
	}

void CGdiSetOrigin::TheTest(const TRect &aRect, TInt)
	{
	iGdi->SetOrigin(aRect.iTl);
	DrawIt(TPoint(0,0));
	}

//
// Draw text //
//

CGdiDrawText::~CGdiDrawText()
	{
	TheClient->iScreen->ReleaseFont(iFont);
	}

TInt CGdiDrawText::Count(TBool )
	{
	return(1);
	}

void CGdiDrawText::ConstructL(TInt)
	{
	TFontSpec fspec(KMonospaceTestFontTypefaceName,KMaxFontSize);
	User::LeaveIfError(TheClient->iScreen->GetNearestFontToDesignHeightInTwips((CFont *&)iFont,fspec));
	TInt index;
	for(index=0;index<iBigBuffer.MaxLength();index++)
		iBigBuffer.Append(TChar((index%26)+'a'));
	const TInt characters=iBigBuffer.Length();
	TInt bbLen=iFont->TextWidthInPixels(iBigBuffer);
	for(index=0;index<iBigBuffer.MaxLength();index+=33)
		{
		TInt lLen=iFont->TextWidthInPixels(iBigBuffer.Left(index));
		TInt rLen=iFont->TextWidthInPixels(iBigBuffer.Right(characters-index));
		TBool result = IsLengthEqual(lLen,rLen,bbLen);
		iGdiTest->TEST(result);
		if (!result)
			{
			_LIT(KLog,"Font width metrics error, Width(%dchars)=%d, Width(%dchars)=%d, Width(%dchars)=%d");
			iGdiTest->LOG_MESSAGE7(KLog,index,lLen,characters-index,rLen,characters,bbLen);
			}
		}
	}

void CGdiDrawText::PreTestSetupL(const TRect&,TInt)
	{}

void CGdiDrawText::BaseTest(const TRect& aRect,TInt)
	{
	TInt bbLen=iFont->TextWidthInPixels(iBigBuffer);
	TInt fAscent=iFont->AscentInPixels();
	TInt fHeight=iFont->HeightInPixels();
	iGdi->UseFont(iFont);
//
	iGdi->DrawText(_L("Hello"), TPoint(aRect.iTl.iX, aRect.iTl.iY+fAscent));
	TInt missingChars=iFont->TextCount(iBigBuffer,bbLen-aRect.iBr.iX)-1;	// -1 so one off screen char is drawn to handle overhang
	if (missingChars>=0)
		{
		TPtrC bbb=iBigBuffer.Right(iBigBuffer.Length()-missingChars);
		TInt lLen=iFont->TextWidthInPixels(iBigBuffer.Left(missingChars));
		TInt rLen=iFont->TextWidthInPixels(bbb);
		TBool result = IsLengthEqual(lLen,rLen,bbLen);
		iGdiTest->TEST(result);
		if (!result)
			{
			_LIT(KLog,"Font width metrics error, missingChars=%d  %d+%d=%d");
			iGdiTest->LOG_MESSAGE5(KLog,missingChars,lLen,rLen,bbLen);
			}
		iGdi->DrawText(bbb,TPoint(aRect.iBr.iX-rLen,aRect.iTl.iY+fAscent+fHeight));
		}
	iGdi->DiscardFont();
	}

void CGdiDrawText::TheTest(const TRect& aRect,TInt)
	{
	TInt bbLen=iFont->TextWidthInPixels(iBigBuffer);
	TInt fAscent=iFont->AscentInPixels();
	TInt fHeight=iFont->HeightInPixels();
	iGdi->UseFont(iFont);
//
	iGdi->DrawText(_L("Hello"), TPoint(aRect.iTl.iX, aRect.iTl.iY+fAscent));
	iGdi->DrawText(iBigBuffer,TPoint(aRect.iBr.iX-bbLen,aRect.iTl.iY+fAscent+fHeight));
	iGdi->DiscardFont();
	}

//
// Draw vertical text //
//

CGdiDrawVertText::~CGdiDrawVertText()
	{
	TheClient->iScreen->ReleaseFont(iFont);
	}

TInt CGdiDrawVertText::Count(TBool )
	{
	return(1);
	}

void CGdiDrawVertText::ConstructL(TInt)
	{
	TFontSpec fspec(KMonospaceTestFontTypefaceName,KMaxFontSize);
	User::LeaveIfError(TheClient->iScreen->GetNearestFontToDesignHeightInTwips((CFont *&)iFont,fspec));
	TInt index;
	for(index=0;index<iBigBuffer.MaxLength();index++)
		iBigBuffer.Append(TChar((index%26)+'a'));
//
	TInt bbLen=iFont->TextWidthInPixels(iBigBuffer);
	for(index=0;index<iBigBuffer.MaxLength();index+=33)
		{
		TInt lLen=iFont->TextWidthInPixels(iBigBuffer.Left(index));
		TInt rLen=iFont->TextWidthInPixels(iBigBuffer.Right(iBigBuffer.Length()-index));
		TBool result = IsLengthEqual(lLen,rLen,bbLen);
		iGdiTest->TEST(result);
		if (!result)
			iGdiTest->INFO_PRINTF3(_L("iFont->TextWidthInPixels(iBigBuffer) return value - Expected: %d, Actual: %d"), lLen+rLen, bbLen);		
		}
	}

void CGdiDrawVertText::PreTestSetupL(const TRect &, TInt )
	{
	}

void CGdiDrawVertText::BaseTest(const TRect &aRect, TInt)
	{
	TInt bbLen=iFont->TextWidthInPixels(iBigBuffer);
	TInt fAscent=iFont->AscentInPixels();
	TInt fHeight=iFont->HeightInPixels();
	iGdi->UseFont(iFont);
//
	TInt tlen=iFont->TextWidthInPixels(_L("Hello"));
	iGdi->DrawTextVertical(_L("Hello"), TPoint(aRect.iTl.iX+fAscent, aRect.iTl.iY+tlen), ETrue);
	iGdi->DrawTextVertical(_L("Hello"), TPoint(aRect.iBr.iX-fAscent, aRect.iTl.iY), EFalse);
	TInt missingChars=iFont->TextCount(iBigBuffer,bbLen-aRect.iBr.iY)-1;	// -1 so one off screen char is drawn to handle overhang
	if (missingChars>=0)
		{
		TPtrC bbb=iBigBuffer.Right(iBigBuffer.Length()-missingChars);
		TInt lLen=iFont->TextWidthInPixels(iBigBuffer.Left(missingChars));
		TInt rLen=iFont->TextWidthInPixels(bbb);
		TBool result = IsLengthEqual(lLen,rLen,bbLen);
		iGdiTest->TEST(result);
		if (!result)
			iGdiTest->INFO_PRINTF3(_L("iFont->TextWidthInPixels(iBigBuffer) return value - Expected: %d, Actual: %d"), lLen+rLen, bbLen);		

		iGdi->DrawTextVertical(bbb,TPoint(aRect.iTl.iX+fHeight-fAscent+fHeight,aRect.iBr.iY-rLen),EFalse);
		}
	iGdi->DiscardFont();
	}

void CGdiDrawVertText::TheTest(const TRect &aRect, TInt)
	{
	TInt bbLen=iFont->TextWidthInPixels(iBigBuffer);
	TInt fAscent=iFont->AscentInPixels();
	TInt fHeight=iFont->HeightInPixels();
	iGdi->UseFont(iFont);
//
	TInt tlen=iFont->TextWidthInPixels(_L("Hello"));
	iGdi->DrawTextVertical(_L("Hello"), TPoint(aRect.iTl.iX+fAscent, aRect.iTl.iY+tlen), ETrue);
	iGdi->DrawTextVertical(_L("Hello"), TPoint(aRect.iBr.iX-fAscent, aRect.iTl.iY), EFalse);
	iGdi->DrawTextVertical(iBigBuffer,TPoint(aRect.iTl.iX+fHeight-fAscent+fHeight,aRect.iBr.iY-bbLen),EFalse);
	iGdi->DiscardFont();
	}

//
// Draw Justified text //
//

// DEF107985 The test for JustifiedText has been removed as it was a bad/useless test. The original test was 
// broken after it was migrated to TEF TGdi test. It was not worth fixing as the purpose of test was not clear, 
// and fixing it basically required rewriting the whole test. 
//  


//
// Box text //
//

CGdiBoxText::~CGdiBoxText()
	{
	TheClient->iScreen->ReleaseFont(iFont);
	}

TInt CGdiBoxText::Count(TBool )
	{
	return(1);
	}

void CGdiBoxText::ConstructL(TInt)
	{
	TFontSpec fspec(KMonospaceTestFontTypefaceName,KMaxFontSize);
	User::LeaveIfError(TheClient->iScreen->GetNearestFontToDesignHeightInTwips((CFont *&)iFont,fspec));
	for(TInt index=0;index<iBigBuffer.MaxLength();index++)
		iBigBuffer.Append(TChar((index%26)+'a'));
	iBbLen=iFont->TextWidthInPixels(iBigBuffer);
	}

void CGdiBoxText::PreTestSetupL(const TRect &, TInt )
	{
	}

void CGdiBoxText::BaseTest(const TRect &aRect, TInt)
	{
	TInt rWid=aRect.iBr.iX-aRect.iTl.iX;
	TInt fAscent=iFont->AscentInPixels();
	TInt fHeight=iFont->HeightInPixels();
	iGdi->UseFont(iFont);
	iGdi->SetBrushStyle(CGraphicsContext::ESolidBrush);
//
	TRect rect(aRect);
	rect.iBr.iY=rect.iTl.iY+fHeight;
//
	TInt missingChars=iFont->TextCount(iBigBuffer,iBbLen-rWid);
	TPtrC bbb=iBigBuffer.Right(iBigBuffer.Length()-missingChars);
	TInt lLen=iFont->TextWidthInPixels(iBigBuffer.Left(missingChars));
	TInt rLen=iFont->TextWidthInPixels(bbb);
	TBool result = IsLengthEqual(lLen,rLen,iBbLen);
	iGdiTest->TEST(result);	
	if (!result)
			iGdiTest->INFO_PRINTF3(_L("(lLen+rLen)==iBbLen return value - Expected: %d, Actual: %d"), lLen+rLen, iBbLen);		

	iGdi->DrawText(bbb, rect, fAscent,CGraphicsContext::ELeft, rWid-rLen);
//
	rect.Move(TPoint(0,fHeight));
	iGdi->DrawText(iBigBuffer, rect, fAscent,CGraphicsContext::ELeft, 0);
//
	TBuf<10> buf2(_L("1234567890"));
	rect.Move(TPoint(0,fHeight));
	iGdi->DrawText(buf2,rect,fAscent,CGraphicsContext::ERight,rect.Width()-iFont->TextWidthInPixels(buf2));
	}

void CGdiBoxText::TheTest(const TRect &aRect, TInt)
	{
	TInt rWid=aRect.iBr.iX-aRect.iTl.iX;
	TInt fAscent=iFont->AscentInPixels();
	TInt fHeight=iFont->HeightInPixels();
	iGdi->UseFont(iFont);
	iGdi->SetBrushStyle(CGraphicsContext::ESolidBrush);
	TRect rect(aRect);
	rect.iBr.iY=rect.iTl.iY+fHeight;
//
	iGdi->DrawText(iBigBuffer, rect, fAscent, CGraphicsContext::ELeft, rWid-iBbLen);
//
	rect.Move(TPoint(0,fHeight));
	iGdi->Clear(rect);
	iGdi->SetClippingRegion(TRegionFix<1>(rect));
	iGdi->DrawText(iBigBuffer, rect.iTl+TPoint(0,fAscent));
	TBuf<10> buf2(_L("1234567890"));
	rect.Move(TPoint(0,fHeight));
	iGdi->CancelClippingRegion();
	iGdi->DrawText(buf2,rect,fAscent);
	}

//
// Vert Box text //
//

CGdiVertBoxText::~CGdiVertBoxText()
	{
	TheClient->iScreen->ReleaseFont(iFont);
	}

TInt CGdiVertBoxText::Count(TBool )
	{
	return(1);
	}

void CGdiVertBoxText::ConstructL(TInt)
	{
	TFontSpec fspec(KMonospaceTestFontTypefaceName,KMaxFontSize);
	User::LeaveIfError(TheClient->iScreen->GetNearestFontToDesignHeightInTwips((CFont *&)iFont,fspec));
	for(TInt index=0;index<iBigBuffer.MaxLength();index++)
		iBigBuffer.Append(TChar((index%26)+'a'));
	iBbLen=iFont->TextWidthInPixels(iBigBuffer);
	}

void CGdiVertBoxText::PreTestSetupL(const TRect &, TInt )
	{
	}

void CGdiVertBoxText::BaseTest(const TRect &aRect, TInt)
	{
	TInt rWid=aRect.iBr.iY-aRect.iTl.iY;
	TInt fAscent=iFont->AscentInPixels();
	TInt fHeight=iFont->HeightInPixels();
	iGdi->UseFont(iFont);
	iGdi->SetBrushStyle(CGraphicsContext::ESolidBrush);
//
	TRect rect(aRect);
	rect.iBr.iX=rect.iTl.iX+fHeight;
//
	TInt missingChars=iFont->TextCount(iBigBuffer,iBbLen-rWid);
	TPtrC bbb=iBigBuffer.Right(iBigBuffer.Length()-missingChars+1);
	TInt lLen=iFont->TextWidthInPixels(iBigBuffer.Left(missingChars-1));
	TInt rLen=iFont->TextWidthInPixels(bbb);
	TBool result = IsLengthEqual(lLen,rLen,iBbLen);
	iGdiTest->TEST(result);
	if (!result)
			iGdiTest->INFO_PRINTF3(_L("(lLen+rLen)==iBbLen return value - Expected: %d, Actual: %d"), lLen+rLen, iBbLen);		

	iGdi->DrawTextVertical(bbb, rect, fAscent, ETrue,CGraphicsContext::ELeft, rWid-rLen);
//
	rect.iTl.iX=rect.iBr.iX;
	rect.iBr.iX=rect.iTl.iX+fHeight;
	iGdi->DrawTextVertical(iBigBuffer, rect, fAscent, ETrue,CGraphicsContext::ELeft, 0);
	}

void CGdiVertBoxText::TheTest(const TRect &aRect, TInt)
	{
	TInt rWid=aRect.iBr.iY-aRect.iTl.iY;
	TInt fAscent=iFont->AscentInPixels();
	TInt fHeight=iFont->HeightInPixels();
	iGdi->UseFont(iFont);
	iGdi->SetBrushStyle(CGraphicsContext::ESolidBrush);
	TRect rect(aRect);
	rect.iBr.iX=rect.iTl.iX+fHeight;
//
	iGdi->DrawTextVertical(iBigBuffer, rect, fAscent, ETrue, CGraphicsContext::ELeft, rWid-iBbLen);
//
	rect.iTl.iX=rect.iBr.iX;
	rect.iBr.iX=rect.iTl.iX+fHeight;
	iGdi->Clear(rect);
	iGdi->SetClippingRegion(TRegionFix<1>(rect));
	iGdi->DrawTextVertical(iBigBuffer, TPoint(rect.iTl.iX+fAscent,rect.iBr.iY), ETrue);
	}


//
//
CGdiBlitMasked::~CGdiBlitMasked()
	{
	for(TInt index=0;index<ENumMasks;index++)
		delete iMask[index];
	delete iBitmap;
	delete iScratch;
	delete iScratchDevice;
	delete iScratchGc;
	delete iScratchMask;
	delete iScratchMaskDevice;
	delete iScratchMaskGc;
	}

TInt CGdiBlitMasked::Count(TBool aMainTests)
	{
	return(aMainTests ? ENumMasks*2 : ENumMasks*2);
	}

void CGdiBlitMasked::doCreateTestBitmapL(CFbsBitmap *aBitmap, CFbsBitGc *&aGc, CFbsBitmapDevice *&aDevice, const TSize &aSize)
	{
	User::LeaveIfNull(aDevice=CFbsBitmapDevice::NewL(aBitmap));
	User::LeaveIfError(aDevice->CreateContext(aGc));
//
	aGc->SetBrushColor(TRgb(128,128,128));
	aGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	aGc->SetPenStyle(CGraphicsContext::ENullPen);
	aGc->DrawRect(TRect(aBitmap->SizeInPixels()));
	aGc->SetPenStyle(CGraphicsContext::ESolidPen);
	for(TInt index=(-aSize.iHeight);index<aSize.iWidth;index+=5)
		{
		aGc->DrawLine(TPoint(index,0),TPoint(index+aSize.iHeight,aSize.iHeight));
		aGc->DrawLine(TPoint(index,aSize.iHeight),TPoint(index+aSize.iHeight,0));
		}
	}

void CGdiBlitMasked::createTestBitmapL(CFbsBitmap *&aBitmap, const TSize &aSize)
	{
	CFbsBitGc *gc=NULL;
	CFbsBitmapDevice *device=NULL;
	aBitmap=new(ELeave) CFbsBitmap();
	User::LeaveIfError(aBitmap->Create(aSize,EGray16));
	TRAPD(err,doCreateTestBitmapL(aBitmap, gc, device, aSize));
	delete gc;
	delete device;
	User::LeaveIfError(err);
	}

void CGdiBlitMasked::ConstructL(TInt)
	{
	TSize max(0,0);
	for(TInt index=0;index<ENumMasks;index++)
		{
		iMask[index]=new(ELeave) CFbsBitmap();
		User::LeaveIfError(iMask[index]->Load(TAUTO_BITMAP_NAME,2+index));
		TSize size=iMask[index]->SizeInPixels();
		if (max.iWidth<size.iWidth)
			max.iWidth=size.iWidth;
		if (max.iHeight<size.iHeight)
			max.iHeight=size.iHeight;
		}
	iScratch=new(ELeave) CFbsBitmap();
	User::LeaveIfError(iScratch->Create(max,EGray16));
	iScratchDevice=CFbsBitmapDevice::NewL(iScratch);
	User::LeaveIfError(iScratchDevice->CreateContext(iScratchGc));

	iScratchMask=new(ELeave) CFbsBitmap();
	User::LeaveIfError(iScratchMask->Create(max,EGray16));
	iScratchMaskDevice=CFbsBitmapDevice::NewL(iScratchMask);
	User::LeaveIfError(iScratchMaskDevice->CreateContext(iScratchMaskGc));
//
	createTestBitmapL(iBitmap, max);
	}

void CGdiBlitMasked::PreTestSetupL(const TRect &, TInt aCount)
	{
	iInvertMask=aCount&0x1;
	aCount>>=1;
	iCurrMask= iMask[aCount%ENumMasks];
	iDrawMode=CGraphicsContext::EDrawModePEN;
	}

void CGdiBlitMasked::BaseTest(const TRect &aRect, TInt)
	{
// needs re-writing to emulate tiling of the source rect
	if (!aRect.Intersects(TRect(BaseWin->Size())))
		return;
	TSize size(aRect.Size());
	TSize bitSize=iBitmap->SizeInPixels();
	if (size.iWidth>bitSize.iWidth)
		size.iWidth=bitSize.iWidth;
	if (size.iHeight>bitSize.iHeight)
		size.iHeight=bitSize.iHeight;
//
// Set up the scratch mask as a black and white bitmap containing the mask to blit
// The mask pattern is replicated all over the scratchmask bitmap
//
	iScratchMaskGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	iScratchMaskGc->SetPenStyle(CGraphicsContext::ENullPen);
	iScratchMaskGc->SetBrushColor(TRgb(0,0,0));
	iScratchMaskGc->DrawRect(TRect(iScratchMask->SizeInPixels()));
	iScratchMaskGc->SetPenColor(TRgb(255,255,255));
	iScratchMaskGc->SetPenStyle(CGraphicsContext::ESolidPen);
	TSize maskSize(iCurrMask->SizeInPixels());
	TPoint pos;
	TRgb *rgbBuf=(TRgb *)User::AllocL(maskSize.iWidth*sizeof(TRgb));		//Doesn't do any harm if it leaves
	for(pos.iY=0;pos.iY<maskSize.iHeight;pos.iY++)
		{
		TPtr8 ptr((TUint8 *)rgbBuf,maskSize.iWidth*sizeof(TRgb));
		iCurrMask->GetScanLine(ptr, pos, maskSize.iWidth, ERgb);
		for(TInt index=0;index<maskSize.iWidth;index++)
			{
			iScratchMaskGc->SetPenColor(rgbBuf[index]);
//			if ((isLow && !iLowCutOff) || (!isLow && iLowCutOff))
				iScratchMaskGc->Plot(TPoint(index,pos.iY));
			}
		}
	User::Free(rgbBuf);

	for(pos.iY=0;pos.iY<size.iHeight;pos.iY+=maskSize.iHeight)
		for(pos.iX=0;pos.iX<size.iWidth;pos.iX+=maskSize.iWidth)
			iScratchMaskGc->CopyRect(pos, TRect(maskSize));
//
// Blit this to the screen in ANDNOT mode to clear all the pixels we want the mask blit to draw to
//
	iGdi->SetDrawMode(CGraphicsContext::EDrawModeANDNOT);
	iGdi->BitBlt(aRect.iTl, iScratchMask, TRect(size));
//
// Copy the test bitmap to the scratch bitmap then use the scratch mask to clear all the bits
// that should masked out of the draw to the screen
//
	iScratchGc->SetDrawMode(CGraphicsContext::EDrawModePEN);
	iScratchGc->BitBlt(TPoint(0,0), iBitmap);
	iScratchGc->SetDrawMode(CGraphicsContext::EDrawModeAND);
	iScratchGc->BitBlt(TPoint(0,0), iScratchMask);
//
// Now copy the scratch bitmap to the screen in OR mode to get the final result
//
	iGdi->SetDrawMode(CGraphicsContext::EDrawModeOR);
	iGdi->BitBlt(aRect.iTl, iScratch, TRect(size));
	}

void CGdiBlitMasked::TheTest(const TRect &aRect, TInt)
	{
	iGdi->BitBltMasked(aRect.iTl,iBitmap,TRect((aRect.iBr-aRect.iTl).AsSize()),
						iCurrMask,iInvertMask);
	}
//
//

CTGdi::CTGdi(CTestStep* aStep) : CTWsGraphicsBase(aStep), iGdiTest (NULL), iTextOffsetX(10),iTextOffsetY(10)
	{
	}

void CTGdi::ConstructL()
	{
	iWinSize=TestWin->Size();
	}

TSize CTGdi::WinSize()
	{
	return(iWinSize);
	}

void CTGdi::DoGdiTestL(const TRect &aRect, TInt aNum)
	{
//
	iGdiTest->PreTestSetupL(aRect,aNum);
//
	BaseWin->Win()->Invalidate();
	BaseWin->Win()->BeginRedraw();
	TheGc->Activate(*BaseWin->Win());
	iTest->DrawTestBackground(EFalse,BaseWin->Size());
	iGdiTest->BaseTest(aRect,aNum);
	TheGc->Deactivate();
	BaseWin->Win()->EndRedraw();
//
	TestWin->Win()->Invalidate();
	TestWin->Win()->BeginRedraw();
	TheGc->Activate(*TestWin->Win());
	iTest->DrawTestBackground(EFalse,TestWin->Size());
	iGdiTest->TheTest(aRect,aNum);
	TheGc->Deactivate();
	TestWin->Win()->EndRedraw();
//
	TheClient->iWs.Flush();
	TheClient->WaitForRedrawsToFinish();
	TheClient->iWs.Finish();
//
	_LIT(KLog,"GDI Substate=%d  test=%d  rect=(%d,%d,%d,%d) ");
	TBuf<64> buf;
	buf.Format(KLog,iSubState,aNum,aRect.iTl.iX,aRect.iTl.iY,aRect.iBr.iX,aRect.iBr.iY);
	iTestPassing=CompareWindows(buf);
	}

void CTGdi::GdiTestL(CGdiTestLowLevel *aTest)
	{
	TTestRects::ResetSeed();
	iTestPassing=ETrue;
	iGdiTest=aTest;
	iGdiTest->SetGdiTest(this);
	iGdiTest->AssignGdi(TheGc);
	iGdiTest->ConstructL(iTest->iTestRects.Count1());
	TInt index;
	TInt index2;
	for(index=0; iTestPassing && index<iTest->iTestRects.Count2(); ++index)
		{
		TInt max=iGdiTest->Count(index<iTest->iTestRects.Count1());
		TRect rect=iTest->iTestRects[index];
	#if defined(LOGGING)
		_LIT(KLog1,"Testing rectangle %d (%d,%d,%d,%d) 1st pass");
		LOG_MESSAGE6(KLog1,index,rect.iTl.iX,rect.iTl.iY,rect.iBr.iX,rect.iBr.iY);
	#endif
		for(index2=0; iTestPassing && index2<max; ++index2)
			DoGdiTestL(rect,index2);
		if (!iTestPassing)
			{
			_LIT(KLog,"Test fail on rectangle %d at iteration %d");
			LOG_MESSAGE3(KLog,index,index2);
			}
		}
	for(; iTestPassing && index<iTest->iTestRects.Count3(); index++)
		{
		TRect rect=iTest->iTestRects[index];
	#if defined(LOGGING)
		_LIT(KLog2,"Testing rectangle %d (%d,%d,%d,%d) 2nd pass");
		LOG_MESSAGE6(KLog2,index,rect.iTl.iX,rect.iTl.iY,rect.iBr.iX,rect.iBr.iY);
	#endif
		DoGdiTestL(rect,0);
		}
	delete iGdiTest;
	iGdiTest=NULL;
	}

void CTGdi::NonDrawing()
//
// Test all non-drawing GDI functions
//
	{
	TDisplayMode mode=TheClient->iScreen->DisplayMode();
	TEST((mode!=ERgb) && (mode!=ENone));

	RWsSession session;
	TInt res = session.Connect();
	TEST(res == KErrNone);
	
	CArrayFixFlat<TInt>* modeList= new CArrayFixFlat<TInt> (15);
	if(!modeList)
		return;
	
#if defined(SYMBIAN_GRAPHICS_GCE)
	res = session.GetColorModeList(iTest->iScreenNumber, modeList);
#else
	res = session.GetColorModeList(modeList);
#endif //SYMBIAN_GRAPHICS_GCE
	session.Close();
	TEST(res == KErrNone);
	if(res != KErrNone)
		{
		modeList->Reset();
		delete modeList;
		return;
		}
	
	TBool found = EFalse;
	for(TInt ii = 0; ii < modeList->Count() && !found; ii++)
		{
		TDisplayMode mode1 = (TDisplayMode) ((*modeList)[ii]);
		if(mode == mode1)
			{
			found = ETrue;
			}
		}
	modeList->Reset();
	delete modeList;

	TEST(found);	
	}

//
// Auxiliary Fn for Test Case ID CTGdi_TestDefetct_DEF045746
// This method draws the text represented by aText parameter on the window
// passed to it.
//
void CTGdi::DrawTextOnWindow(const TDesC& aText,CTWin* aWin)
	{
	aWin->Win()->BeginRedraw();
	TheGc->Activate(*aWin->Win());
	TheGc->Device()->GetNearestFontToDesignHeightInTwips(iFont,TFontSpec(_L("Arial"),250));
	TheGc->UseFont(iFont);
	TheGc->Clear();
	TheGc->DrawRect(TRect(aWin->Win()->Size()));
	TheGc->DrawText(aText,TPoint(iTextOffsetX,iTextOffsetY));
	TheGc->Device()->ReleaseFont(iFont);
	TheGc->Deactivate();
	aWin->Win()->EndRedraw();
	}


void CTGdi::TestDefetct_DEF045746L()
	{
	_LIT(KString,"This is a test window for the defect fix DEF045746 \
		propagated from Opera browser. Most window graphics context drawing\
		functions map to equivalent CFbsBitGc functions they are implemented\
		on the screen with any co-ordinates being relative to the top left\
		corner of the window. However extra clipping is applied. The drawing\
		will always be clipped to the visible part of the window, in addition\
		it will be clipped to the non-invalid part if you are not doing a\
		redraw and to the region being validated if you are doing a redraw.");

	TPtrC TestText(KString().Ptr(),100);
	TPtrC LargeText(KString().Ptr());
	TPtrC ShortText(KString().Ptr(),200);

	TSize scrSize=TheClient->iScreen->SizeInPixels();
	TSize winSize;
	const TInt windowGap=5;
	winSize.SetSize(scrSize.iWidth -2* windowGap,scrSize.iHeight/2 - windowGap);

	iWin=new(ELeave) CTestWindow(TRgb(0,0,0));
	iWin->ConstructL(TPoint(5,5),TSize(winSize),TheClient->iGroup,*TheClient->iGc);
	CTestWindow* expectWin= new(ELeave) CTestWindow(TRgb(0,0,0));
	expectWin->ConstructL(TPoint(5,scrSize.iHeight/2 + windowGap),TSize(winSize),TheClient->iGroup,*TheClient->iGc);
	DrawTextOnWindow(ShortText,iWin);
	DrawTextOnWindow(TestText,expectWin);
	TInt fHeight=iFont->HeightInPixels();//Used to compare only pixels where text is drawn
	TRect iRect=winSize;
	iRect.iTl.iX=iTextOffsetX;
	iRect.iTl.iY=iTextOffsetY;
	iRect.iBr.iY=iTextOffsetY+fHeight;
	CheckRect(iWin,expectWin,iRect,_L("TestDefetct_DEF045746L A"));
	iWin->Invalidate();
	DrawTextOnWindow(LargeText,iWin);
	CheckRect(iWin,expectWin,iRect,_L("TestDefetct_DEF045746L B"));
	delete iWin;
	iWin=NULL;
	delete expectWin;
	expectWin=NULL;
	}

void CTGdi::RunTestCaseL(TInt /*aCurTestCase*/)
	{
	//_LIT(KTest1,"NonDrawing"); this test is not running
	_LIT(KTest2,"DEF045746");
	_LIT(KTest3,"Drawing Tests");
	((CTGdiStep*)iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);
	static TTime startTime;
	switch(++iTest->iState)
		{
/**
@SYMTestCaseID		GRAPHICS-WSERV-0262

@SYMDEF             DEF081259

@SYMTestCaseDesc    Test all non-drawing GDI functions

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Check all non-drawing GDI functions operate correctly

@SYMTestExpectedResults Non-drawing GDI functions operate correctly
*/
		case 1:
		    // start time log
		    startTime.HomeTime();
			((CTGdiStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0262"));
			//iTest->LogSubTest(KTest1);
			//PeterI this tests whether the display mode is EGray4 or EGray16.
			//The actual display mode is EColor16MU so this test will always fail
			//NonDrawingL();
			break;
/**
 * @SYMTestCaseID GRAPHICS-WSERV-0263
 *
 * @SYMPREQ DEF045746
 *
 * @SYMTestCaseDesc Test defect fix for DEF045746
 *
 * @SYMTestPriority  High
 *
 * @SYMTestStatus Implemented
 *
 * @SYMTestActions The method tests CWindowGc::DrawText().The test is carried
 * out by writing small & very large strings to the window graphic context.
 *
 * @SYMTestExpectedResults The window with large / small strings written is
 * compared with an expected result window. In both the cases strings should
 * be displayed.
 *
 */
		case 2:
			((CTGdiStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0263"));
			iTest->LogSubTest(KTest2);
			TestDefetct_DEF045746L();
			break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0264

@SYMDEF             DEF081259

@SYMTestCaseDesc    Test all drawing GDI functions

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Check all drawing GDI functions operate correctly

@SYMTestExpectedResults Drawing GDI functions operate correctly
*/
		case 3:
			{
			((CTGdiStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0264"));
			iTest->LogSubTest(KTest3);
			iSubState = 0;
			TheClient->WaitForRedrawsToFinish();		//Just in case the two test windows have any invalid areas
			TInt numTests = sizeof(GdiTestFuncs) / sizeof(GdiTestFuncs[0]);
			while (iSubState < numTests)
				{
				LOG_MESSAGE(TPtrC(GdiTestFuncs[iSubState].txt));
				GdiTestL((*GdiTestFuncs[iSubState].func)());
				iSubState++;
				}
			}
			break;
		default:
            ((CTGdiStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
			((CTGdiStep*)iStep)->CloseTMSGraphicsStep();
			TestComplete();
			// log time and print duration
			TTime endTime;
			endTime.HomeTime();
			TInt64 duration = endTime.MicroSecondsFrom(startTime).Int64();
			float time = (float) I64LOW(duration) / 1000.0f;
			_LIT(KDuration,"Total test duration is %f millisec");
			RDebug::Print(KDuration, time);
			break;
		}
	((CTGdiStep*)iStep)->RecordTestResultL();
	}

CGdiTestLowLevel *SetOriginTest()
	{
	return(new(ELeave) CGdiSetOrigin());
	}

CGdiTestLowLevel *PolyLineTest()
	{
	return(new(ELeave) CGdiPolyLine());
	}

CGdiTestLowLevel *PolygonTest()
	{
	return(new(ELeave) CGdiPolygon());
	}

CGdiTestLowLevel *BlitMaskedTest()
	{
	return(new(ELeave) CGdiBlitMasked());
	}

CGdiTestLowLevel *DrawArcTest()
	{
	return(new(ELeave) CGdiDrawArc());
	}

CGdiTestLowLevel *DrawPieTest()
	{
	return(new(ELeave) CGdiDrawPie());
	}

CGdiTestLowLevel *DrawRoundRectTest()
	{
	return(new(ELeave) CGdiDrawRoundRect());
	}

CGdiTestLowLevel *BoxTextTest()
	{
	return(new(ELeave) CGdiBoxText());
	}

CGdiTestLowLevel *VertTextTest()
	{
	return(new(ELeave) CGdiDrawVertText());
	}

CGdiTestLowLevel *VertBoxTextTest()
	{
	return(new(ELeave) CGdiVertBoxText());
	}

CGdiTestLowLevel *WideLineTest()
	{
	return(new(ELeave) CGdiWideLine());
	}

CGdiTestLowLevel *DrawTextTest()
	{
	return(new(ELeave) CGdiDrawText());
	}

CGdiTestLowLevel *DrawRectTest()
	{
	return(new(ELeave) CGdiDrawRect());
	}

__WS_CONSTRUCT_STEP__(Gdi)
