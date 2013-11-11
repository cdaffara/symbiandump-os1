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
// Window scroll tests
// 
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/

#include "TSCROLL.H"
	
#pragma warning(disable : 4710)

//
// Scroll window, used to test the Scroll() command
//

void DrawScrollWin(CBitmapContext *aGc, const TPoint &aOffset, const TSize &aSize)
	{
	aGc->Clear();
	TPoint drawBase(-10,-20);
	TSize drawSize(aSize.iWidth-2*drawBase.iX,aSize.iHeight-2*drawBase.iY);
	aGc->DrawRect(TRect(drawBase,drawBase+drawSize));
	TPoint offset=drawBase+aOffset;
	aGc->SetPenColor(TRgb::Gray4(2));
//	aGc->SetDrawMode(CGraphicsContext::EDrawModeXOR);
	aGc->DrawRect(TRect(offset+TPoint(13,23),offset+TPoint(22,32)));
	TInt xpos,xplus;
	for(xpos=0,xplus=2;xpos<drawSize.iWidth;xplus+=2,xpos+=xplus)
		aGc->DrawLine(TPoint(xpos,0)+offset,TPoint(xpos,drawSize.iHeight)+offset);
	for(TInt ypos=0,yplus=2;ypos<drawSize.iHeight;yplus+=2,ypos+=yplus)
		aGc->DrawLine(TPoint(0,ypos)+offset,TPoint(drawSize.iWidth,ypos)+offset);
//
	aGc->SetPenColor(TRgb(0,0,0));
	aGc->SetDrawMode(CGraphicsContext::EDrawModePEN);
	TheClient->iWs.Flush();
	for(xpos=0;xpos<drawSize.iWidth;xpos+=16)
		{
		aGc->DrawLine(TPoint(xpos,0)+offset,TPoint(drawSize.iWidth,xpos*drawSize.iHeight/drawSize.iWidth)+offset);
		aGc->DrawLine(TPoint(xpos,0)+offset,TPoint(0,drawSize.iHeight-xpos*drawSize.iHeight/drawSize.iWidth)+offset);
		aGc->DrawLine(TPoint(xpos,drawSize.iHeight)+offset,TPoint(drawSize.iWidth,drawSize.iHeight-xpos*drawSize.iHeight/drawSize.iWidth)+offset);
		aGc->DrawLine(TPoint(xpos,drawSize.iHeight)+offset,TPoint(0,xpos*drawSize.iHeight/drawSize.iWidth)+offset);
		}
		
	}

CScrollWindowNorm::CScrollWindowNorm() : CTWin()
	{
	}

void DrawScrollWin(const TPoint &aOffset, const TSize &aSize)
	{
	::DrawScrollWin(TheClient->iGc, aOffset, aSize);
	}

void CScrollWindowNorm::Draw()
	{
	DrawScrollWin(iScrollTest->Offset(), Size());
	}

void CScrollWindowNorm::SetScrollTest(CScrollTestBase *aScrollTest)
	{
	iScrollTest=aScrollTest;
	}

//
// CScrollWindowBackedUp
//

CScrollWindowBackedUp::CScrollWindowBackedUp(TDisplayMode aDisplayMode) : CTBackedUpWin(aDisplayMode)
	{
	}

void CScrollWindowBackedUp::Draw()
	{
	DrawScrollWin(iScrollTest->Offset(), Size());
	}

void CScrollWindowBackedUp::SetScrollTest(CScrollTestBase *aScrollTest)
	{
	iScrollTest=aScrollTest;
	}

//
// CScrollTestBase
//

void CScrollTestBase::Reset()
	{
	iOffset=TPoint(0,0);
	iInvalid.Clear();
	}

void CScrollTestBase::DoInvalidate()
	{
	}

TPoint CScrollTestBase::Offset() const
	{
	return(iOffset);
	}

CScrollTestBase::~CScrollTestBase()
	{
	delete iBlankWin;
	iInvalid.Close();
	}

void CScrollTestBaseNorm::ConstructL(const TPoint &aPos, const TSize &aSize)
	{
	iScrollWin=new(ELeave) CScrollWindowNorm();
	iScrollWin->SetUpL(aPos, aSize, TheClient->iGroup, *TheClient->iGc);
	iScrollWin->SetScrollTest(this);
	iSize=iScrollWin->Size();
	}

CScrollTestBaseNorm::~CScrollTestBaseNorm()
	{
	delete iScrollWin;
	}

void CScrollTestBaseNorm::Redraw()
	{
	iScrollWin->Redraw();
	}

void CScrollTestBaseNorm::Reset()
	{
	iScrollWin->Win()->Invalidate();
	iScrollWin->Redraw();
	CScrollTestBase::Reset();
	}

CTDrawableWin *CScrollTestBaseNorm::ScrollWin() const
	{
	return(iScrollWin);
	}

void CScrollTestBaseNorm::AdjustTestScrollRegion(const TRect &aClipRect, const TPoint &aOffset, const TRect &aRect)
	{
	RRegion clipped_visible;
	VisibleRegion(clipped_visible);
	clipped_visible.ClipRect(aClipRect);
//
//
	RRegion invalidate(aRect);
	invalidate.SubRegion(clipped_visible);
	invalidate.Offset(aOffset);
	invalidate.ClipRect(aClipRect);
//
	RRegion invalidate2(aRect);
	invalidate2.ClipRect(aClipRect);
	invalidate2.Intersect(iInvalid);
	invalidate2.Offset(aOffset);
	invalidate2.ClipRect(aClipRect);
//
	RRegion validate(aRect);
	validate.Intersect(clipped_visible);
	validate.SubRegion(iInvalid);
	validate.Offset(aOffset);
	validate.ClipRect(aClipRect);
//
	iInvalid.SubRegion(validate);
	iInvalid.Union(invalidate);
	iInvalid.Union(invalidate2);
	RRegion visible;
	VisibleRegion(visible);
	iInvalid.Intersect(visible);
	iInvalid.Tidy();
//
	invalidate2.Close();
	invalidate.Close();
	validate.Close();
	visible.Close();
	clipped_visible.Close();

	iOffset+=aOffset;
	}

void CScrollTestBaseNorm::VisibleRegion(RRegion &aRegion)
	{
	aRegion.Clear();
	aRegion.AddRect(TRect(iSize));
	TRect child;
	child.iTl=iBlankWin->BaseWin()->InquireOffset(*ScrollWin()->BaseWin());
	child.iBr=child.iTl+iBlankWin->BaseWin()->Size();
	aRegion.SubRect(child);
	}

void CScrollWindow1::DoTestScroll(const TRect &aClipRect, const TPoint &aOffset, const TRect &aRect)
	{
	TheGc->Activate(*iScrollWin->DrawableWin());
	RRegion region(aRect);
	RRegion visible;
	VisibleRegion(visible);
	visible.ClipRect(aClipRect);
	region.Intersect(visible);
	visible.Close();
	region.SubRegion(iInvalid);
	region.Offset(aOffset);
	region.ClipRect(aClipRect);
	TheGc->SetClippingRegion(region);
	TheGc->CopyRect(aOffset,aRect);
	TheGc->Deactivate();
	AdjustTestScrollRegion(aClipRect, aOffset,aRect);
	iInvalid.Union(TRegionFix<1>(TRect(aClipRect)));
	region.Close();
	}

void CScrollWindow1::DoTestScroll(const TPoint &aOffset)
	{
	DoTestScroll(TRect(TPoint(0,0),iSize), aOffset,TRect(-aOffset,iSize));
	}

void CScrollWindow1::DoTestScroll(const TPoint &aOffset, const TRect &aRect)
	{
	DoTestScroll(TRect(TPoint(0,0),iSize), aOffset, aRect);
	}

void CScrollWindow1::DoTestScroll(const TRect &aClipRect, const TPoint &aOffset)
	{
	DoTestScroll(aClipRect, aOffset, TRect(-aOffset.iX,-aOffset.iY,iSize.iWidth-aOffset.iX, iSize.iHeight-aOffset.iY));
	}

void CScrollWindow1::DoInvalidate()
	{
	for(TInt index=0;index<iInvalid.Count();index++)
		iScrollWin->Invalidate(iInvalid[index]);
	}

void CScrollWindow2::Reset()
	{
	CScrollTestBaseNorm::Reset();
	iClipped=EFalse;
	}

CScrollWindow2::CScrollWindow2(CTScroll *aTest) : iTest(aTest)
	{
	}

void CScrollWindow2::Draw()
	{
	if (iClipped)
		TheClient->iGc->SetClippingRegion(iInvalid);
	iScrollWin->Draw();
	if (iClipped)
		TheClient->iGc->CancelClippingRegion();
	}

void CScrollWindow2::DoTestScroll(const TPoint &aOffset)
	{
	AdjustTestScrollRegion(TRect(iSize), aOffset, TRect(-aOffset.iX,-aOffset.iY,iSize.iWidth-aOffset.iX, iSize.iHeight-aOffset.iY));
	iClipped=ETrue;
	iScrollWin->DrawableWin()->Scroll(aOffset);
	}

void CScrollWindow2::DoTestScroll(const TRect &aClipRect, const TPoint &aOffset)
	{
	AdjustTestScrollRegion(aClipRect, aOffset, TRect(-aOffset.iX,-aOffset.iY,iSize.iWidth-aOffset.iX, iSize.iHeight-aOffset.iY));
	iClipped=ETrue;
	iScrollWin->DrawableWin()->Scroll(aClipRect, aOffset);
	}

void CScrollWindow2::DoTestScroll(const TPoint &aOffset, const TRect &aRect)
	{
	AdjustTestScrollRegion(TRect(iSize), aOffset, aRect);
	iClipped=ETrue;
	iScrollWin->DrawableWin()->Scroll(aOffset, aRect);
	}

void CScrollWindow2::DoTestScroll(const TRect &aClipRect, const TPoint &aOffset, const TRect &aRect)
	{
	AdjustTestScrollRegion(aClipRect, aOffset, aRect);
	iClipped=ETrue;
	iScrollWin->DrawableWin()->Scroll(aClipRect, aOffset, aRect);
	}

void CScrollWindow3::DrawScrollBit()
	{
	::DrawScrollWin(iBitmapGc, iOffset, iSize);
	}

void CScrollWindow3::Reset()
	{
	CScrollTestBase::Reset();
	iBitmapGc->Clear(TRect(iSize));
	DrawScrollBit();
	DoInvalidate();
	}

CScrollWindow3::~CScrollWindow3()
	{
	delete iBitmapGc;
	delete iBitmapDevice;
	delete iBitmap;
	}

void CScrollWindow3::ConstructL(const TPoint &aPos, const TSize &aSize)
	{
	CScrollTestBaseNorm::ConstructL(aPos,aSize);
	iBitmap=new(ELeave) CFbsBitmap();
	User::LeaveIfError(iBitmap->Create(iSize,EGray4));
	iBitmapDevice=CFbsBitmapDevice::NewL(iBitmap);
	User::LeaveIfError(iBitmapDevice->CreateContext(iBitmapGc));
	DrawScrollBit();
	}

void CScrollWindow3::DoTestScroll(const TRect &aClipRect, const TPoint &aOffset, const TRect &aRect)
	{
	iOffset+=aOffset;
	RRegion blank(aRect);
	TRect clipRect(aClipRect);
	clipRect.Intersection(TRect(iSize));
	blank.SubRect(clipRect);
	blank.Offset(aOffset);
	blank.ClipRect(clipRect);
	TRect rect(aRect);
	rect.Intersection(clipRect);		
	TRect rect2(clipRect);
	rect2.Move(-aOffset);
	rect.Intersection(rect2);			
	iBitmapGc->CopyRect(aOffset,rect);
	iBitmapGc->SetClippingRegion(&blank);
	iBitmapGc->Clear(TRect(iSize));
	iBitmapGc->SetClippingRegion(NULL);
	blank.Close();
	}

void CScrollWindow3::DoTestScroll(const TPoint &aOffset)
	{
	DoTestScroll(TRect(iSize), aOffset,TRect(-aOffset.iX,-aOffset.iY,iSize.iWidth-aOffset.iX, iSize.iHeight-aOffset.iY));
	}

void CScrollWindow3::DoTestScroll(const TPoint &aOffset, const TRect &aRect)
	{
	DoTestScroll(TRect(iSize), aOffset, aRect);
	}

void CScrollWindow3::DoTestScroll(const TRect &aClipRect, const TPoint &aOffset)
	{
	DoTestScroll(aClipRect, aOffset, TRect(-aOffset.iX,-aOffset.iY,iSize.iWidth-aOffset.iX, iSize.iHeight-aOffset.iY));
	}

void CScrollWindow3::Draw()
	{
	TheGc->BitBlt(TPoint(),iBitmap);
	}

void CScrollWindow3::DoInvalidate()
	{
	RWindow win=*iScrollWin->Win();
	win.Invalidate();
	win.BeginRedraw();
	TheGc->Activate(win);
	Draw();
	TheGc->Deactivate();
	win.EndRedraw();
	}

void CScrollWindow4::ConstructL(const TPoint &aPos, const TSize &aSize)
	{
	iScrollWin=new(ELeave) CScrollWindowBackedUp(EGray4);
	iScrollWin->SetUpL(aPos, aSize, TheClient->iGroup, *TheClient->iGc);
	iScrollWin->SetScrollTest(this);
	iSize=iScrollWin->Size();
	TheClient->iGc->Activate(*iScrollWin->DrawableWin());
	TheClient->iGc->Clear();
	DrawScrollWin(TPoint(), iSize);
	TheClient->iGc->Deactivate();
	}

CScrollWindow4::~CScrollWindow4()
	{
	delete iScrollWin;
	}

void CScrollWindow4::Redraw()
	{
	}

void CScrollWindow4::Reset()
	{
	TheClient->iGc->Activate(*iScrollWin->DrawableWin());
	TheClient->iGc->Clear();
	DrawScrollWin(TPoint(), iSize);
	TheClient->iGc->Deactivate();
	CScrollTestBase::Reset();
	}

void CScrollWindow4::DoTestScroll(const TPoint &aOffset)
	{
	iScrollWin->DrawableWin()->Scroll(aOffset);
	iOffset+=aOffset;
	}

void CScrollWindow4::DoTestScroll(const TPoint &aOffset, const TRect &aRect)
	{
	iScrollWin->DrawableWin()->Scroll(aOffset, aRect);
	iOffset+=aOffset;
	}

void CScrollWindow4::DoTestScroll(const TRect &aClipRect, const TPoint &aOffset)
	{
	iScrollWin->DrawableWin()->Scroll(aClipRect, aOffset);
	iOffset+=aOffset;
	}

void CScrollWindow4::DoTestScroll(const TRect &aClipRect, const TPoint &aOffset, const TRect &aRect)
	{
	iScrollWin->DrawableWin()->Scroll(aClipRect, aOffset, aRect);
	iOffset+=aOffset;
	}

CTDrawableWin *CScrollWindow4::ScrollWin() const
	{
	return(iScrollWin);
	}

//

CTScroll::CTScroll(CTestStep* aStep):
	CTWsGraphicsBase(aStep)
	{
	}

CTScroll::~CTScroll()
	{
	BaseWin->SetVisible(ETrue);
	TestWin->SetVisible(ETrue);
	delete iBaseScrollWin;
	delete iTestScrollWin;
	}

void CTScroll::ConstructL()
	{
	BaseWin->SetVisible(EFalse);
	TestWin->SetVisible(EFalse);
//
	iTest->iState=1;
	TSize screenSize=TheClient->iGroup->Size();
	TInt winWidth=(screenSize.iWidth/3)-10;
	TInt winHeight=screenSize.iHeight-10;
	TInt winBaseX=(screenSize.iWidth/3);
	if (iMode==0)
		{
		iBaseScrollWin=new(ELeave) CScrollWindow1();
		iTestScrollWin=new(ELeave) CScrollWindow2(this);
		}
	else
		{
		iBaseScrollWin=new(ELeave) CScrollWindow3();
		iTestScrollWin=new(ELeave) CScrollWindow4();
		}
	iWinSize=TSize(winWidth,winHeight);
	iBaseScrollWin->ConstructL(TPoint(winBaseX+5,5),iWinSize);
	iTestScrollWin->ConstructL(TPoint(winBaseX+winWidth+15,5),iWinSize);

	iBaseScrollWin->iBlankWin=new(ELeave) CTBlankWindow();
	iBaseScrollWin->iBlankWin->SetUpL(iBaseScrollWin->ScrollWin()->BaseWin()->InquireOffset(*TheClient->iGroup->GroupWin())+TPoint(winWidth>>2,winHeight>>2),TSize(winWidth>>2,winHeight>>1),TheClient->iGroup,*TheClient->iGc);
	iBaseScrollWin->iBlankWin->BlankWin()->SetShadowHeight(2);
	
	iTestScrollWin->iBlankWin=new(ELeave) CTBlankWindow();
	iTestScrollWin->iBlankWin->SetUpL(iTestScrollWin->ScrollWin()->BaseWin()->InquireOffset(*TheClient->iGroup->GroupWin())+TPoint(winWidth>>2,winHeight>>2),TSize(winWidth>>2,winHeight>>1),TheClient->iGroup,*TheClient->iGc);
	}

void CTScroll::CheckScrollWindows()
	{
	CheckRect(iBaseScrollWin->ScrollWin(),iTestScrollWin->ScrollWin(),TRect(iBaseScrollWin->ScrollWin()->Size()),_L("CTScroll::CheckScrollWindows()"));
	}

void CTScroll::DoTestScroll(const TPoint &aOffset)
	{
	iTestScrollWin->DoTestScroll(aOffset);
	iBaseScrollWin->DoTestScroll(aOffset);
	}

void CTScroll::DoTestScroll(const TRect &aClipRect, const TPoint &aOffset)
	{
	iTestScrollWin->DoTestScroll(aClipRect,aOffset);
	iBaseScrollWin->DoTestScroll(aClipRect,aOffset);
	}

void CTScroll::DoTestScroll(const TPoint &aOffset, const TRect &aRect)
	{
	iTestScrollWin->DoTestScroll(aOffset,aRect);
	iBaseScrollWin->DoTestScroll(aOffset,aRect);
	}

void CTScroll::DoTestScroll(const TRect &aClipRect, const TPoint &aOffset, const TRect &aRect)
	{
	iTestScrollWin->DoTestScroll(aClipRect,aOffset,aRect);
	iBaseScrollWin->DoTestScroll(aClipRect,aOffset,aRect);
	}

void CTScroll::FinishedRectScrolls()
	{
	iBaseScrollWin->DoInvalidate();
	}

void CTScroll::CheckOomScroll()
	{
	TheClient->iWs.HeapSetFail(RHeap::ENone,0);
	FinishedRectScrolls();
	iTestScrollWin->Redraw();
	iBaseScrollWin->Redraw();
//	if (iMode!=1)				// Out of memory scrolling of backed up windows can not work 100% when out of memory
//		CheckScrollWindows();	// Redraw window code can't check these cases properly yet
	}

void CTScroll::OomScrolling()
	{
	iTestScrollWin->iBlankWin->BaseWin()->SetShadowHeight(0);
	iBaseScrollWin->iBlankWin->BaseWin()->SetShadowHeight(0);
	iTestScrollWin->ScrollWin()->BaseWin()->SetShadowHeight(0);
	iTestScrollWin->Redraw();
	iBaseScrollWin->Redraw();
	for(TInt count=1;count<20;count++)
		{
		TheClient->iWs.HeapSetFail(RHeap::EDeterministic,count);
		DoTestScroll(TPoint(50,0));
		CheckOomScroll();
	//
		TheClient->iWs.HeapSetFail(RHeap::EDeterministic,count);
		iTestScrollWin->Reset();
		iBaseScrollWin->Reset();
		DoTestScroll(TPoint(-6,-14),TRect(20,20,100,50));
		CheckOomScroll();
	//
		TheClient->iWs.HeapSetFail(RHeap::EDeterministic,count);
		iTestScrollWin->Reset();
		iBaseScrollWin->Reset();
		DoTestScroll(TRect(TPoint(1,1),iWinSize-TSize(1,1)),TPoint(50,0),TRect(10,10,50,50));
		CheckOomScroll();
		}
	}

void CTScroll::RunTestCaseL(TInt /*aCurTestCase*/)
	{
	_LIT(KScroll,"Scroll");
	//if (iTest->iState==0) iTest->iState=22;
	if (iDoCheck)
		{
		if (iMode==1)		// The test code does not mimic scrolling to a non-backed up window properly
			{
			TheClient->iWs.Flush();
			TheClient->iWs.Finish();
			CheckScrollWindows();
			}
		iTestScrollWin->Reset();
		iBaseScrollWin->Reset();
		TheClient->iWs.Flush();
		++iTest->iState;
		iDoCheck=EFalse;
		return;
		}
	((CTScrollStep*)iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);
	switch(iTest->iState)
		{
/**
@SYMTestCaseID		GRAPHICS-WSERV-0249

@SYMDEF  			DEF081259

@SYMTestCaseDesc    Test window scrolling by different offsets

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Pass different offsets for scrolling a window by and
					check the window is scrolled correctly

@SYMTestExpectedResults Window scrolls correctly to offsets
*/	 
		case 1:
			((CTScrollStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0249"));
			//DemoWindowScroll();
			iTest->LogSubTest(KScroll);
			DoTestScroll(TPoint(6,0));
			DoTestScroll(TPoint(-6,-14));
			DoTestScroll(TPoint(-6,-14));
			DoTestScroll(TPoint(0,10));
			DoTestScroll(TPoint(0,0));
			DoTestScroll(TPoint(10,23));		
			break;
		case 2:
			((CTScrollStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0249"));
			iTest->LogSubTest(KScroll);
			DoTestScroll(TPoint(50,0));
			break;
		case 3:
			((CTScrollStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0249"));
			iTest->LogSubTest(KScroll);
			DoTestScroll(TPoint(0,33));
			break;
		case 4:
			((CTScrollStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0249"));
			iTest->LogSubTest(KScroll);
			DoTestScroll(TPoint(-10,0));
			break;
		case 5:
			((CTScrollStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0249"));
			iTest->LogSubTest(KScroll);
			DoTestScroll(TPoint(0,-13));
			break;
		case 6:
			((CTScrollStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0249"));
			iTest->LogSubTest(KScroll);
			DoTestScroll(TPoint(0,0));
			break;
		case 7:
			((CTScrollStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0249"));
			iTest->LogSubTest(KScroll);
			DoTestScroll(TPoint(-234,-1400));
			break;
		case 8:
			((CTScrollStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0249"));
			iTest->LogSubTest(KScroll);
			DoTestScroll(TPoint(-69,15));
			break;
		case 9:
			((CTScrollStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0249"));
			iTest->LogSubTest(KScroll);
			DoTestScroll(TPoint(-234,-1400));
			DoTestScroll(TPoint(233,1401));
			break;
		case 10:
			((CTScrollStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0249"));
			iTest->LogSubTest(KScroll);
			DoTestScroll(TPoint(10,-1));
			DoTestScroll(TPoint(11,-10));
			DoTestScroll(TPoint(-5,-40));
			DoTestScroll(TPoint(0,50));
			break;
			
/**
@SYMTestCaseID		GRAPHICS-WSERV-0250

@SYMDEF  			DEF081259

@SYMTestCaseDesc    Test window scrolling by different offsets and
				source rectangles

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Pass different offsets for scroll a window by and
				different source rectangles.
				Check the window is scrolled correctly

@SYMTestExpectedResults Window scrolls correctly to offsets and source
					rectangles
*/	 
		case 11:
			((CTScrollStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0250"));
			iTest->LogSubTest(KScroll);
			DoTestScroll(TPoint(-6,-14),TRect(20,20,100,50));
			DoTestScroll(TPoint(-6,-14),TRect(20,20,100,50));
			DoTestScroll(TPoint(-6,-14),TRect(40,10,130,130));
			break;
		case 12:
			((CTScrollStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0250"));
			iTest->LogSubTest(KScroll);
			DoTestScroll(TPoint(10,-1),TRect(20,20,21,21));
			break;
		case 13:
			((CTScrollStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0250"));
			iTest->LogSubTest(KScroll);
			DoTestScroll(TPoint(-20,-14),TRect(10,10,10,10));
			break;
		case 14:
			((CTScrollStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0250"));
			iTest->LogSubTest(KScroll);
			DoTestScroll(TPoint(-6,-14),TRect(20,20,200,300));
			DoTestScroll(TPoint(10,-24),TRect(20,20,200,300));
			DoTestScroll(TPoint(3,12),TRect(20,20,200,300));
			DoTestScroll(TPoint(-20,19),TRect(20,20,200,300));
			break;
		case 15:
			((CTScrollStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0250"));
			iTest->LogSubTest(KScroll);
			DoTestScroll(TPoint(-6,-14),TRect(20000,200000,30000,300000));
			DoTestScroll(TPoint(10,-24),TRect(20,20,-200,-300));
			DoTestScroll(TPoint(10,-24),TRect(20,20,-200,300));
			DoTestScroll(TPoint(10,-24),TRect(20,20,200,-300));
			DoTestScroll(TPoint(3,12),TRect(-100,-200,1000,2000));
			DoTestScroll(TPoint(-20,19),TRect(0,0,0,0));
			break;
			
/**
@SYMTestCaseID		GRAPHICS-WSERV-0251

@SYMDEF  			DEF081259

@SYMTestCaseDesc    Test window scrolling by different offsets and
				clipping rectangles

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Pass different offsets for scroll a window by and
				different clipping rectangles.
				Check the window is scrolled correctly

@SYMTestExpectedResults Window scrolls correctly to offsets and clipping
					rectangles
*/	
		case 16:
			((CTScrollStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0251"));
			iTest->LogSubTest(KScroll);
			DoTestScroll(TRect(10,10,200,200),TPoint(-6,-14));
			break;
		case 17:
			((CTScrollStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0251"));
			iTest->LogSubTest(KScroll);
			DoTestScroll(TRect(10,50,210,200),TPoint(10,20));
			DoTestScroll(TRect(0,0,100,100),TPoint(-22,4));
			DoTestScroll(TRect(50,50,300,500),TPoint(-5,-2));
			break;
		case 18:
			((CTScrollStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0251"));
			iTest->LogSubTest(KScroll);
			DoTestScroll(TRect(-1000,-10000,23456,1111),TPoint(-6,-14));
			break;
		case 19:
			((CTScrollStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0251"));
			iTest->LogSubTest(KScroll);
			DoTestScroll(TRect(20,5,198,202),TPoint(-100,1));
			DoTestScroll(TRect(1,1,200,123),TPoint(2,1));
			DoTestScroll(TRect(23,23,200,400),TPoint(5,-1));
			break;
		case 20:
			((CTScrollStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0251"));
			iTest->LogSubTest(KScroll);
			DoTestScroll(TRect(0,0,0,0),TPoint(-10,1));
			DoTestScroll(TRect(-1000,1,2000,123),TPoint(2,1));
			DoTestScroll(TRect(23,-23,200,40000),TPoint(5,-1));
			DoTestScroll(TRect(-1000,-10000,23456,1111),TPoint(-6,-14));
			break;
			
/**
@SYMTestCaseID		GRAPHICS-WSERV-0252

@SYMDEF  			DEF081259

@SYMTestCaseDesc    Test window scrolling by different offsets and
				clipping ans source rectangles

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Pass different offsets for scroll a window by and
				different clipping and source rectangles.
				Check the window is scrolled correctly

@SYMTestExpectedResults Window scrolls correctly to offsets and source
					and clipping rectangles
*/	
		case 21:
			((CTScrollStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0252"));
			iTest->LogSubTest(KScroll);
			DoTestScroll(TRect(TPoint(1,1),iWinSize-TSize(1,1)),TPoint(-6,-14),TRect(10,10,50,50));
			DoTestScroll(TRect(TPoint(1,1),iWinSize-TSize(1,1)),TPoint(16,4),TRect(10,10,50,50));
			break;
		case 22:
			((CTScrollStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0252"));
			iTest->LogSubTest(KScroll);
			DoTestScroll(TRect(TPoint(10,100),iWinSize-TSize(0,10)),TPoint(26,22),TRect(iWinSize));
			DoTestScroll(TRect(iWinSize),TPoint(2,-1),TRect(-10,-10,50,50));
			break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0253

@SYMDEF  			DEF081259

@SYMTestCaseDesc    Out of memory window scrolling test 

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Out of memory window scrolling test 

@SYMTestExpectedResults Out of memory scrolling is handled correctly
*/
		case 23:		
			((CTScrollStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0253"));
			iTest->LogSubTest(KScroll);
			OomScrolling();
			iTest->CloseAllPanicWindows();
			++iTest->iState;
			return;
		case 24:		
			((CTScrollStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0253"));
			++iTest->iState;
			++iMode;
			if (iMode==1)
				{
				delete iBaseScrollWin;
				delete iTestScrollWin;
				ConstructL();
				}
			return;
		default:
			((CTScrollStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
			((CTScrollStep*)iStep)->CloseTMSGraphicsStep();
			TestComplete();
			break;
		}
	((CTScrollStep*)iStep)->RecordTestResultL();
	FinishedRectScrolls();
	iDoCheck=ETrue;
	}

void CTScroll::DemoWindowScrollL()
	//Function to see what scrolling does.
	{
	CBlankWindow* iWin;
	iWin=new(ELeave) CBlankWindow(TRgb(64,64,224));
	iWin->SetUpL(TPoint(20,10),TSize(120,100),TheClient->iGroup,*TheClient->iGc);
	iWin->BaseWin()->SetRequiredDisplayMode(EColor256);
	iWin->Win()->SetBackgroundColor(TRgb(64,224,64));
	iWin->RealDraw(ETrue);
	TheClient->WaitForRedrawsToFinish();
	iWin->DrawableWin()->Scroll(TPoint(8,5),TRect(20,20,100,80));
	TheClient->Flush();
	TheClient->WaitForRedrawsToFinish();
	TheClient->Flush();
	}


__WS_CONSTRUCT_STEP__(Scroll)
#pragma warning(default : 4710)
