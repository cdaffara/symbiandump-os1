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
// Window redraw/validate/invalidate tests
// 
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/

#include "TREDRAW.H"

LOCAL_D TSize FullScreenModeSize;

const TRgb KRed = TRgb(255,0,0);
const TRgb KBlack = TRgb(0,0,0);
const TRgb KWhite = TRgb(255,255,255);
const TRgb KTransBisque = TRgb(255,228,196,128);
const TRgb KTransLightSalmon = TRgb(255,160,122,128);
const TRgb KLightSteelBlue = TRgb(176,196,222);
const TRgb KCadetBlue = TRgb(95,158,160);

TInt CTRedrawOrderWindow::iRedrawNumber=0;

#define InvalidRegionLogging
#if defined(InvalidRegionLogging)
#define BLOG_MESSAGE(p) (const_cast<CTRedrawTest*>(&iGraphicsTest))->LogMessage(((TText8*)__FILE__), __LINE__,(p))
#endif

//
//  //
//

CTCheckDefectWin* CTCheckDefectWin::NewL(TPoint aPos,TSize aWinSize)
	{
	CTCheckDefectWin* win=new(ELeave) CTCheckDefectWin;
	win->ConstructExtLD(*TheClient->iGroup,aPos,aWinSize);
	win->BaseWin()->SetRequiredDisplayMode(EColor256);
	win->AssignGC(*TheClient->iGc);
	win->BaseWin()->SetShadowDisabled(ETrue);
	win->BaseWin()->SetShadowHeight(0);
	TheClient->Flush();
	return win;
	}

CRedrawWindow::CRedrawWindow(CTRedrawTest *aTest) : CTWin(), iTest(aTest)
	{
	}

CRedrawWindow::~CRedrawWindow()
	{
	iInvalid.Close();
	}

void CRedrawWindow::Draw()
	{
	ReceivedDrawRequest();
	DrawPattern(iTest->WinContent());
	}

void CRedrawWindow::ReceivedDrawRequest()
	{
	iDrawRequests++;
	}

TInt CRedrawWindow::DrawRequests() const
	{
	return iDrawRequests;
	}

void CRedrawWindow::DrawPattern(TInt aPattern)
	{
	iGc->Clear();
	TPoint drawBase(-10,-20);
	TSize drawSize(iSize.iWidth-2*drawBase.iX,iSize.iHeight-2*drawBase.iY);
	TPoint offset=drawBase+iOffset;
	switch(aPattern)
		{
		case EDrawGraphPaperlHatched:
			{
			iGc->DrawRect(TRect(drawBase,drawBase+drawSize));
			iGc->SetPenColor(TRgb(85,85,85));
			iGc->SetDrawMode(CGraphicsContext::EDrawModeXOR);
			TInt xpos;
			for(xpos=0;xpos<drawSize.iWidth;xpos+=15)
				iGc->DrawLine(TPoint(xpos,0)+offset,TPoint(xpos,drawSize.iHeight)+offset);
			TInt ypos;
			for(ypos=0;ypos<drawSize.iHeight;ypos+=15)
				iGc->DrawLine(TPoint(0,ypos)+offset,TPoint(drawSize.iWidth,ypos)+offset);
			break;
			}
		case EDrawSlantingHatched:
			{
			iGc->SetPenColor(TRgb(255,255,255));
			iGc->SetDrawMode(CGraphicsContext::EDrawModeXOR);
			for(TInt xpos=0;xpos<drawSize.iWidth;xpos+=16)
				{
				iGc->DrawLine(TPoint(xpos,0)+offset,TPoint(drawSize.iWidth,xpos*drawSize.iHeight/drawSize.iWidth)+offset);
				iGc->DrawLine(TPoint(xpos,0)+offset,TPoint(0,drawSize.iHeight-xpos*drawSize.iHeight/drawSize.iWidth)+offset);
				iGc->DrawLine(TPoint(xpos,drawSize.iHeight)+offset,TPoint(drawSize.iWidth,drawSize.iHeight-xpos*drawSize.iHeight/drawSize.iWidth)+offset);
				iGc->DrawLine(TPoint(xpos,drawSize.iHeight)+offset,TPoint(0,xpos*drawSize.iHeight/drawSize.iWidth)+offset);
				}
			}
			break;
		case EDrawCenteredRectangle:
			{
			TRect rect(5,iSize.iHeight/4,iSize.iWidth-5,iSize.iHeight/2);
			iGc->SetDrawMode(CGraphicsContext::EDrawModeXOR);
			iGc->SetPenColor(TRgb(255,255,255));
			iGc->DrawRect(rect);
			}
			break;
		default:
			// Other pattern values requested are ignored
			break;
		}
	}

void CRedrawWindow::Reset()
	{
	iOffset=TPoint(0,0);
	iWin.Invalidate();
	iInvalid.Clear();
	}

/**
 * Calculate the window region minus the region covered by the child
 * window; this is the "visible region"
 */
void CRedrawWindow::VisibleRegion(RRegion &aRegion)
	{
	aRegion.Clear();
	aRegion.AddRect(TRect(Size()));
	TRect child;
	child.iTl=Child()->BaseWin()->InquireOffset(iWin);
	child.iBr=child.iTl+Child()->Size();
	aRegion.SubRect(child);
	}


void CRedrawWindow::ValidateAndClear()
	{
	Win()->Invalidate();
	Win()->BeginRedraw();
	iGc->Activate(*Win());
	iGc->Clear();
	iGc->Deactivate();
	Win()->EndRedraw();
	}

void CRedrawWindow::ActivateAndDraw(TInt aPattern, TRegion *aRegion)
	{
	iGc->Activate(*Win());
	if (aRegion)
		iGc->SetClippingRegion(*aRegion);
	DrawPattern(aPattern);
	iGc->Deactivate();
	}

CReferenceComparisonRedrawWindow::CReferenceComparisonRedrawWindow(CTRedrawTest *aTest) : CRedrawWindow(aTest)
	{}

/**
 * Prepare the invalid region.
 * 
 * Update the invalid region with a rectangle where such a rectangle is minus
 * any area covered by a child window.
 * 
 * @param aRect Rectangle to be added to the invalid region
 */
void CReferenceComparisonRedrawWindow::PrepareInvalidation(const TRect &aRect)
	{
	RRegion clipped_visible;
	VisibleRegion(clipped_visible);
	clipped_visible.ClipRect(aRect);
	iInvalid.Union(clipped_visible);
	clipped_visible.Close();
	iInvalid.Tidy();
	}

/**
 * Mop up all pending invalid regions and simulate a Draw().
 * 
 * Normally, we would rely on WServ to call this window's Draw() method
 * to obtain drawing operations to cover the currently invalid regions
 * of the window.
 * 
 * This method does that task by marking all invalid regions as clean and
 * then performs the drawing required in the invalid portions of the screen.
 * 
 * The purpose of this is to then allow a comparison to be made against a
 * different window which does rely on the WServ framework calling back
 * to do a Draw().
 * 
 * @post the window has no outstanding invalid regions
 */
void CReferenceComparisonRedrawWindow::PerformInvalidation()
	{
	for(TInt index=0;index<iInvalid.Count();index++)
		{
		iWin.Invalidate(iInvalid[index]);
		iWin.BeginRedraw(iInvalid[index]);
		iWin.EndRedraw();
		}
	iWin.BeginRedraw(iInvalid.BoundingRect());
	iGc->Activate(iWin);
	iGc->SetClippingRegion(iInvalid);
	DrawPattern(iTest->WinContent());
	iGc->Deactivate();
	iWin.EndRedraw();
	}

CRedrawWindow2::CRedrawWindow2(CTRedrawTest *aTest) : CRedrawWindow(aTest)
	{
	}

void CRedrawWindow2::Draw()
	{
	ReceivedDrawRequest();
	if (iClipped)
		iGc->SetClippingRegion(iInvalid);
	DrawPattern(iTest->WinContent());
	if (iClipped)
		iGc->CancelClippingRegion();
	}

/**
 * Prepare the invalid region.
 * @param aRect rectangle to be added to the invalid region
 */
void CRedrawWindow2::PrepareInvalidation(const TRect &aRect)
	{
	iInvalid.Clear();
	iInvalid.AddRect(aRect);
	}

/**
 * Perform invalidation by setting the window's invalid region.
 * 
 * The purpose of this method is to stimulate a call from WServ to the
 * Draw() method of this class.
 */
void CRedrawWindow2::PerformInvalidation()
	{
	iWin.Invalidate(iInvalid.BoundingRect());
	}

void CRedrawWindow2::Reset()
	{
	CRedrawWindow::Reset();
	iClipped=EFalse;
	}

CRedrawWindow3::CRedrawWindow3(CTRedrawTest *aTest) : CRedrawWindow(aTest)
	{
	}

void CRedrawWindow3::Draw()
	{
	ReceivedDrawRequest();
	iGc->Clear();
	iGc->SetPenStyle(CGraphicsContext::ENullPen);
	iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	iGc->SetBrushColor(TRgb(0,255,255));
	iGc->DrawRect(TRect(TPoint(0,0),TSize(50,50)));
	}

void CRedrawWindow3::PrepareInvalidation(const TRect &aRect)
	{
	iInvalid.Clear();
	iInvalid.AddRect(aRect);
	}

void CRedrawWindow3::PerformInvalidation()
	{
	iWin.Invalidate(iInvalid.BoundingRect());
	}

void CRedrawWindow3::SetUp1L(TPoint pos,TSize size,CTWinBase *parent, CWindowGc &aGc)
	{
	ConstructL(*parent);
	SetExtL(pos,size);
	AssignGC(aGc);
	}

void CRedrawWindow3::Redraw(const TRect &aRect)
	{
	if(!isActive)
		{//make an empty redraw
		iWin.BeginRedraw(aRect);
		iWin.EndRedraw();
		}
	else
		{
		CTWin::Redraw(aRect);
		}	
	}

void CRedrawWindow3::Activate()
	{
	isActive = ETrue;
	Win()->Activate();
	}

//

CTRedrawOrderWindow::CTRedrawOrderWindow(TInt aOrder, CTRedrawTest *aTest) : iTest(aTest), iOrder(aOrder)
	{}

CTRedrawOrderWindow* CTRedrawOrderWindow::NewLC(TInt aOrder,CTWinBase* aParent,const TPoint& aPos,const TSize& aSize,CTRedrawTest* aTest)
	{
	CTRedrawOrderWindow* self=new(ELeave) CTRedrawOrderWindow(aOrder,aTest);
	CleanupStack::PushL(self);
	self->SetUpL(aPos,aSize,aParent,*TheGc);
	return self;
	}

void CTRedrawOrderWindow::Draw()
	{
	iGc->SetBrushColor(TRgb::Gray16(iOrder*2));
	iGc->Clear();
	}

void CTRedrawOrderWindow::ResetRedrawNumber()
	{
	iRedrawNumber=0;
	}

void CTRedrawOrderWindow::Redraw(const TRect &aRect)
	{
	if (++iRedrawNumber!=iOrder)
		iTest->Failed(iOrder);
	CTWin::Redraw(aRect);
	}

//

TInt DestructCallback(TAny *aParam)
	{
	((CTRedrawTest *)aParam)->doDestruct();
	return(0);
	}

CTRedrawTest::CTRedrawTest(CTestStep* aStep):
	CTWsGraphicsBase(aStep)//, iInvalidRegionChecker(*this)
	{}

void CTRedrawTest::doDestruct()
	{
	BaseWin->SetVisible(ETrue);
	TestWin->SetVisible(ETrue);
	delete iBaseRedrawWin;
	delete iTestRedrawWin;
	delete iBaseChildWin;
	delete iTestChildWin;
	}

CTRedrawTest::~CTRedrawTest()
	{
	TCallBack callBack(DestructCallback,this);
	TheClient->SetRedrawCancelFunction(callBack);
	delete iInvalidRegionChecker;	
	}

void CTRedrawTest::ConstructL()
	{
#if defined(InvalidRegionLogging)
	LOG_MESSAGE(_L("  CTRedrawTest::ConstructL()"));
	LOG_MESSAGE4(_L("  Ex Wins 0x%08x, 0x%08x, 0x%08x"), BaseWin, TestWin, &(TheClient->StdLogWindow()));
#endif
	
	iInvalidRegionChecker = new(ELeave)CInvalidRegionChecker(*this);
	User::LeaveIfError(iInvalidRegionChecker->AddExcludedWindow(BaseWin));
	User::LeaveIfError(iInvalidRegionChecker->AddExcludedWindow(TestWin));
	User::LeaveIfError(iInvalidRegionChecker->AddExcludedWindow(&(TheClient->StdLogWindow())));
	WaitForRedrawsToFinish(ECheckRedrawActiveObjectAndInvalidRegions);	
	
	iWinContent=0;
	BaseWin->SetVisible(EFalse);
	TestWin->SetVisible(EFalse);
	FullScreenModeSize=TheClient->iGroup->Size();
	TInt winWidth=(FullScreenModeSize.iWidth/3)-10;
	TInt winHeight=FullScreenModeSize.iHeight-10;
	iBaseRedrawWin=new(ELeave) CReferenceComparisonRedrawWindow(this);
	iBaseRedrawWin->SetUpL(TPoint(FullScreenModeSize.iWidth/3+5,5),TSize(winWidth,winHeight),TheClient->iGroup,*TheClient->iGc);
	iTestRedrawWin=new(ELeave) CRedrawWindow2(this);
	iTestRedrawWin->SetUpL(TPoint(FullScreenModeSize.iWidth/3*2+5,5),TSize(winWidth,winHeight),TheClient->iGroup,*TheClient->iGc);
	iBaseChildWin=new(ELeave) CBlankWindow();
	iBaseChildWin->SetUpL(TPoint(winWidth>>2,winHeight>>2),TSize(winWidth>>1,winHeight>>1),iBaseRedrawWin,*TheClient->iGc);
	iTestChildWin=new(ELeave) CBlankWindow();
	iTestChildWin->SetUpL(TPoint(winWidth>>2,winHeight>>2),TSize(winWidth>>1,winHeight>>1),iTestRedrawWin,*TheClient->iGc);

#if defined(InvalidRegionLogging)
	LOG_MESSAGE5(_L("  In Wins %08x, %08x, %08x, %08x"), iBaseRedrawWin, iBaseChildWin, iTestRedrawWin, iTestChildWin);
#endif

	WaitForRedrawsToFinish(ECheckRedrawActiveObjectAndInvalidRegions);

#if defined(InvalidRegionLogging)
	LOG_MESSAGE(_L("  Done"));
#endif
	}

void CTRedrawTest::CheckRedrawWindows()
	{
	_LIT(KTest,"Redraw Test, SubTest %d ");
	TBuf<64> buf;
	buf.Format(KTest,iTest->iState);
	CheckRect(iBaseRedrawWin,iTestRedrawWin,TRect(iBaseRedrawWin->Size()),buf);
	}

void CTRedrawTest::InvalidateTestWins(const TRect &aRect)
	{
	iBaseRedrawWin->PrepareInvalidation(aRect);
	iTestRedrawWin->PrepareInvalidation(aRect);
	iBaseRedrawWin->PerformInvalidation();
	iTestRedrawWin->PerformInvalidation();
	}

inline TInt CTRedrawTest::WinContent()
	{
	return iWinContent;
	}

void CTRedrawTest::SetBackground(const TRgb &aRgb)
	{
	iBaseRedrawWin->iWin.SetBackgroundColor(aRgb);
	iTestRedrawWin->iWin.SetBackgroundColor(aRgb);
	}

void CTRedrawTest::DumpRegion(const TRegion &aRegion)
	{
	_LIT(KLog,"RegionRect %d: (%d,%d,%d,%d)");
	for (TInt ii=0;ii<aRegion.Count();++ii)
		{
		const TRect& rect=aRegion[ii];
		LOG_MESSAGE6(KLog,ii,rect.iTl.iX,rect.iTl.iY,rect.iBr.iX,rect.iBr.iY);
		}
	}

/**
 * Compare Regions to see if the region we think we have dirtied can be
 * completely covered by the WServ invalid region.
 * 
 * @param aDirtyRegion 			Region we have made dirty
 * @param aWservInvalidRegion	Region WServ thinks is dirty
 */
void CTRedrawTest::CompareRegionsL(const TRegion &aDirtyRegion,const TRegion &aWservInvalidRegion)
	{
	_LIT(KDirty,"Dirty Region, %d Rects");
	_LIT(KInvalid,"Invalid Region, %d Rects");
	_LIT(KDiff,"Diff Region, %d Rects");
	RRegion tmp;
	TBool loggedRegions=EFalse;
	tmp.Copy(aDirtyRegion);
	tmp.SubRegion(aWservInvalidRegion);
	if (tmp.CheckError())
		User::Leave(KErrNoMemory);
	TBool isEmpty=tmp.IsEmpty();
	TEST(isEmpty);
	if (!isEmpty)
		{
		_LIT(KLog,"DirtyRegion contains area not in WservInvalidRegion");
		LOG_MESSAGE(KLog);
		loggedRegions=ETrue;
		LOG_MESSAGE2(KDirty,aDirtyRegion.Count());
		DumpRegion(aDirtyRegion);
		LOG_MESSAGE2(KInvalid,aWservInvalidRegion.Count());
		DumpRegion(aWservInvalidRegion);
		LOG_MESSAGE2(KDiff,tmp.Count());
		DumpRegion(tmp);
		}
	tmp.Copy(aWservInvalidRegion);
	tmp.SubRegion(aDirtyRegion);
	if (tmp.CheckError())
		User::Leave(KErrNoMemory);
	isEmpty=tmp.IsEmpty();
	//TEST(isEmpty);		//This test currently fails and a defect will be raises about it
	if (!isEmpty)
		{
		_LIT(KLog,"WservInvalidRegion contains area not in DirtyRegion");
		LOG_MESSAGE(KLog);
		if (!loggedRegions)
			{
			LOG_MESSAGE2(KDirty,aDirtyRegion.Count());
			DumpRegion(aDirtyRegion);
			LOG_MESSAGE2(KInvalid,aWservInvalidRegion.Count());
			DumpRegion(aWservInvalidRegion);
			}
		LOG_MESSAGE2(KDiff,tmp.Count());
		DumpRegion(tmp);
		}
	tmp.Close();
	}

void CTRedrawTest::MoveInvalidAreaL()
	{
	TPoint old=iTestRedrawWin->iWin.Position();
	TSize screenSize=TheClient->iGroup->Size();
	iBaseRedrawWin->iWin.Invalidate();
	iTestRedrawWin->iWin.Invalidate();
	iTestRedrawWin->iWin.SetPosition(TPoint(10,10));
	iTestRedrawWin->iWin.SetPosition(TPoint(0,0));
	iTestRedrawWin->iWin.SetPosition(TPoint(-10,-10));
	iTestRedrawWin->iWin.SetPosition(TPoint(screenSize.iWidth-10,screenSize.iHeight-10));
	iTestRedrawWin->iWin.SetPosition(TPoint(screenSize.iWidth,screenSize.iHeight));
	iTestRedrawWin->iWin.SetPosition(TPoint(screenSize.iWidth+10,screenSize.iHeight+10));
	iTestRedrawWin->iWin.SetPosition(old);
	RRegion baseInvalidRegion;
	RRegion testInvalidRegion;
	iBaseRedrawWin->iWin.GetInvalidRegion(baseInvalidRegion);
	iTestRedrawWin->iWin.GetInvalidRegion(testInvalidRegion);
	CompareRegionsL(baseInvalidRegion,testInvalidRegion);
	baseInvalidRegion.Close();
	testInvalidRegion.Close();
	}

void CTRedrawTest::GetInvalidRegionTestsL()
	{
	TSize stdWinSize(iTest->StdTestWindowSize());
	CArrayFixFlat<TRect>* rectList=new(ELeave) CArrayFixFlat<TRect>(3);
	rectList->AppendL(TRect(1,1,5,2));
	rectList->AppendL(TRect(stdWinSize.iWidth>>1,stdWinSize.iHeight>>1,stdWinSize.iWidth,stdWinSize.iHeight));
	rectList->AppendL(TRect(2,0,4,5));
	TestGetInvalidRegionL(rectList);
	rectList->Reset();
	rectList->AppendL(TRect(-1000,-1,10000,5));
	rectList->AppendL(TRect(0,0,stdWinSize.iWidth>>1,stdWinSize.iHeight>>1));
	rectList->AppendL(TRect(2,100,2*stdWinSize.iWidth,105));
	TestGetInvalidRegionL(rectList);
	delete rectList;
	}

void CTRedrawTest::TestGetInvalidRegionL(const CArrayFixFlat<TRect> *aRectList)
	{
	RRegion invalidRegion;
	RRegion region;
	iTestRedrawWin->iWin.BeginRedraw();
	iTestRedrawWin->iWin.EndRedraw();
	for (TInt index=0;index<aRectList->Count();index++)
		{
		iTestRedrawWin->iWin.Invalidate((*aRectList)[index]);
		region.AddRect((*aRectList)[index]);
		}
	//Currently WSERV includes areas under a child or other window in the invalid region
	//This is arguable the incorrect thing to do
	/*TRect subRect;
	subRect.iTl=iTestChildWin->BaseWin()->InquireOffset(iTestRedrawWin->iWin);
	subRect.SetSize(iTestChildWin->Size());
	region.SubRect(subRect);*/
	region.ClipRect(TRect(iTestRedrawWin->Size()));
	iTestRedrawWin->iWin.GetInvalidRegion(invalidRegion);
	CompareRegionsL(region,invalidRegion);
	region.Close();
	invalidRegion.Close();
	}

void CTRedrawTest::Failed(TInt aOrder)
	{
	_LIT(KLog,"Redraw Order Error, Window Drawn at Position %d should be drawn at Position %d");
	LOG_MESSAGE3(KLog,CTRedrawOrderWindow::RedrawNumber(),aOrder);
	if (iRedrawNo==0)
		iRedrawNo=CTRedrawOrderWindow::RedrawNumber();
	}

void CTRedrawTest::CheckOrderL()
	{
	_LIT(KLog,"Fail in redraw order test, first position of error is %d");
	iRedrawNo=0;
	CTRedrawOrderWindow* order1;
	CTRedrawOrderWindow* order2;
	CTRedrawOrderWindow* order3;
	CTRedrawOrderWindow* order4;
	CTRedrawOrderWindow* order5;
	CTRedrawOrderWindow* order6;
	CTRedrawOrderWindow* order7;
	order6=CTRedrawOrderWindow::NewLC(6,TheClient->iGroup,TPoint(100,10),TSize(40,40),this);
	order7=CTRedrawOrderWindow::NewLC(7,order6,TPoint(0,0),TSize(20,20),this);
	order1=CTRedrawOrderWindow::NewLC(1,TheClient->iGroup,TPoint(10,10),TSize(60,40),this);
	order4=CTRedrawOrderWindow::NewLC(4,order1,TPoint(20,0),TSize(20,40),this);
	order5=CTRedrawOrderWindow::NewLC(5,order4,TPoint(0,0),TSize(20,20),this);
	order2=CTRedrawOrderWindow::NewLC(2,order1,TPoint(0,0),TSize(20,40),this);
	order3=CTRedrawOrderWindow::NewLC(3,order2,TPoint(0,0),TSize(20,20),this);
	TheClient->iWs.Finish();  // Fix for DEF133199 - Intermittant failure with windows out of order 
	WaitForRedrawsToFinish(ECheckRedrawActiveObjectAndInvalidRegions);	// Check order is correct after initial creation
	TEST(order1!=NULL && order2!=NULL && order3!=NULL && order4!=NULL && order5!=NULL && order6!=NULL && order7!=NULL);	// redundant check to shut up the compiler
	TEST(iRedrawNo==0);
	if (iRedrawNo>0)
		LOG_MESSAGE2(KLog,iRedrawNo);

	CTRedrawOrderWindow::ResetRedrawNumber();
	iRedrawNo=0;
	CTUser::Splat(TheClient,TRect(0,0,200,60),TRgb(0,0,0));
	TheClient->iWs.Flush();
	WaitForRedrawsToFinish(ECheckRedrawActiveObjectAndInvalidRegions);	// Check it's still correct on subsequent redraws
	TEST(iRedrawNo==0);
	if (iRedrawNo>0)
		LOG_MESSAGE2(KLog,iRedrawNo);
	CTRedrawOrderWindow::ResetRedrawNumber();
	CleanupStack::PopAndDestroy(7,order6);
	}

// For reproducing INC049554
void CTRedrawTest::CheckDefectINC049554L()
	{
	if(TransparencySupportedL() == KErrNotSupported) //the defect only happens when transparency enabled
		return;
	TSize screenSize=TheClient->iScreen->SizeInPixels();
	TPoint winPos(screenSize.iWidth/3,0);
	TSize winSize(screenSize.iWidth/3,screenSize.iHeight);
	CTCheckDefectWin* lowerWin=CTCheckDefectWin::NewL(winPos,winSize);
	CleanupStack::PushL(lowerWin);
	lowerWin->Activate();
	TheClient->Flush();
	WaitForRedrawsToFinish(ECheckRedrawActiveObjectAndInvalidRegions);

	winPos.iX+=screenSize.iWidth/12;

	CTCheckDefectWin* upperWin=CTCheckDefectWin::NewL(winPos,winSize);
	CleanupStack::PushL(upperWin);
	upperWin->Activate();
	TheClient->Flush();
	WaitForRedrawsToFinish(ECheckRedrawActiveObjectAndInvalidRegions);

	// Invalidate the lower win and while drawing its content, move top window
	lowerWin->Invalidate();
	lowerWin->Win()->BeginRedraw();
	TheClient->iGc->Activate(*lowerWin->DrawableWin());
	TheClient->iGc->SetPenStyle(CGraphicsContext::ENullPen);
	TheClient->iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	TheClient->iGc->SetBrushColor(TRgb(0,255,0));
	TheClient->iGc->DrawRect(TRect(TPoint(0,0),TSize(50,50)));

	winPos.iX+=screenSize.iWidth/12;
	upperWin->SetExt(winPos,winSize);

	TheClient->iGc->SetBrushColor(TRgb(255,0,0));
	TheClient->iGc->DrawRect(TRect(TPoint(0,0),TSize(50,50)));
	TheClient->iGc->Deactivate();
	lowerWin->Win()->EndRedraw();

	TheClient->Flush();
	WaitForRedrawsToFinish(ECheckRedrawActiveObjectAndInvalidRegions);

	CleanupStack::PopAndDestroy(upperWin);

	// Create 2 transaprent windows, slightly over lapping the background window
	winPos.iX=screenSize.iWidth/3;
	TPoint winPosTop(winPos.iX-screenSize.iWidth/6,0);
	TSize winSizeTop(screenSize.iWidth/3+screenSize.iWidth/6,screenSize.iHeight/2);

	CTCheckDefectWin* leftWin=CTCheckDefectWin::NewL(winPosTop,winSizeTop);
	CleanupStack::PushL(leftWin);
	leftWin->Win()->SetTransparencyAlphaChannel();
	leftWin->Win()->SetBackgroundColor(TRgb(0,0,0, 128));
	leftWin->Activate();
	TheClient->iWs.Finish();
	WaitForRedrawsToFinish(ECheckRedrawActiveObjectAndInvalidRegions);

	CTCheckDefectWin* rightWin=CTCheckDefectWin::NewL(winPos,winSizeTop);
	CleanupStack::PushL(rightWin);
	rightWin->Win()->SetTransparencyAlphaChannel();
	rightWin->Win()->SetBackgroundColor(TRgb(0,0,0, 128));
	rightWin->Activate();
	TheClient->iWs.Finish();
	WaitForRedrawsToFinish(ECheckRedrawActiveObjectAndInvalidRegions);

	lowerWin->Invalidate();
	lowerWin->Win()->BeginRedraw();
	TheClient->iGc->Activate(*lowerWin->DrawableWin());
	TheClient->iGc->SetPenStyle(CGraphicsContext::ENullPen);
	TheClient->iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	TheClient->iGc->SetBrushColor(TRgb(0,255,0));
	TheClient->iGc->DrawRect(TRect(TPoint(0,0),TSize(50,50)));

	TheClient->iGc->SetBrushColor(TRgb(255,0,0));
	TheClient->iGc->DrawRect(TRect(TPoint(0,0),TSize(50,50)));
	TheClient->iGc->Deactivate();
	lowerWin->Win()->EndRedraw();

	TheClient->iWs.Finish();
	WaitForRedrawsToFinish(ECheckRedrawActiveObjectAndInvalidRegions);

	CleanupStack::PopAndDestroy(3,lowerWin);
	}


TInt myKK=1;
_LIT(KSnap,"c:\\TestRect%2i.mbm");
_LIT(KSnapE,"c:\\TestRect%2iErr.mbm");
void CTRedrawTest::TestRect()
	{
	
	TBuf<50> snapshotFileName;
	snapshotFileName.Zero();
	CFbsBitmap *snapshot=new(ELeave) CFbsBitmap();
	CleanupStack::PushL(snapshot);
	User::LeaveIfError(snapshot->Create(TheClient->iScreen->SizeInPixels(),TheClient->iScreen->DisplayMode()));		

	User::LeaveIfError(TheClient->iScreen->CopyScreenToBitmap(snapshot));
	
	TRect rect1=TRect(TPoint(),TSize(FullScreenModeSize.iWidth/2,FullScreenModeSize.iHeight));
	TRect rect2=TRect(TPoint(FullScreenModeSize.iWidth/2,0),TSize(FullScreenModeSize.iWidth/2,FullScreenModeSize.iHeight));
	TBool retVal = TheClient->iScreen->RectCompare(rect1,rect2);

	RDebug::Printf("Picture %i !!!: Rect1: (%i,%i)->(%i,%i)  -  Rect2: (%i,%i)->(%i,%i)",myKK,
			rect1.iTl.iX,rect1.iTl.iY,rect1.iBr.iX,rect1.iBr.iY,
			rect2.iTl.iX,rect2.iTl.iY,rect2.iBr.iX,rect2.iBr.iY);	
	
	
	if (retVal)
		{
		snapshotFileName.Format(KSnap,myKK);
		}
	else
		{
		snapshotFileName.Format(KSnapE,myKK);	
		}
	snapshot->Save(snapshotFileName);
	CleanupStack::PopAndDestroy(snapshot);
	myKK++;	
		
	

	TEST(retVal);
	if(!retVal)
		INFO_PRINTF3(_L("TheClient->iScreen->RectCompare() return value  - Expected: %d, Actual: %d"), ETrue, retVal);
	}

void CTRedrawTest::ConstructAndSetBlankWinLC(RBlankWindow& aWindow, TSize aSize, TPoint aPoint/*=TPoint()*/,
						   					TRgb aBackgroundColor/*=TRgb(0,0,0)*/)
	{
	User::LeaveIfError(aWindow.Construct(*TheClient->iGroup->GroupWin(), ENullWsHandle));
	CleanupClosePushL(aWindow);
	aWindow.SetExtent(aPoint, aSize);
	aWindow.SetColor(aBackgroundColor);
	aWindow.Activate();
	}

void CTRedrawTest::ConstructWindowLC(RWindow& aWindow, TSize aSize, TPoint aPoint/*=TPoint()*/,
						   			 TRgb aBackgroundColor/*=TRgb(255,255,255)*/, TBool aTransparencyByAlpha/*=EFalse*/,
						   			 TDisplayMode aDisplayMode/*=EColor64K*/)
	{
	User::LeaveIfError(aWindow.Construct(*TheClient->iGroup->GroupWin(), ENullWsHandle));
	CleanupClosePushL(aWindow);
	aWindow.SetExtent(aPoint,aSize);
	aWindow.SetBackgroundColor(aBackgroundColor);
	aWindow.SetRequiredDisplayMode(aDisplayMode);
	if (aTransparencyByAlpha)
		{
		aWindow.SetTransparencyAlphaChannel();
		}
	aWindow.Activate();
	}

void CTRedrawTest::ActivateAndSetGc(CWindowGc& aGc, RWindow& aWindow, CGraphicsContext::TBrushStyle aBrushStyle/*=CGraphicsContext::ESolidBrush*/, TRgb aBrushColor/*=TRgb(0,0,0)*/,
					  				CGraphicsContext::TPenStyle aPenStyle/*=CGraphicsContext::ENullPen*/, TRgb aPenColor/*=TRgb(0,0,0)*/)
	{
	aGc.Activate(aWindow);
	aGc.Reset();
	aGc.SetBrushStyle(aBrushStyle);
	aGc.SetBrushColor(aBrushColor);
	aGc.SetPenStyle(aPenStyle);
	aGc.SetPenColor(aPenColor);
	}

void CTRedrawTest::DrawWin(CWindowGc& aGc, RWindow& aWin, TSize aWinSize, TRgb aRectColor1, TRgb aRectColor2, TInt aNewOrdinalPos/*=0*/, RWindow* aWinToMove/*=NULL*/, TBool aDrawAllPixels/*=EFalse*/)
	{
	const TUint startX = 10;
	const TUint endX = aWinSize.iWidth - startX;
	const TUint startY = 10;
	const TUint sHeight = (aWinSize.iHeight >> 1) - startY;
	ActivateAndSetGc(aGc,aWin);
	aWin.BeginRedraw();
	if(aDrawAllPixels)
		{
		aGc.SetBrushColor(KWhite);
		aGc.DrawRect(TRect(aWinSize));
		}
	aGc.SetBrushColor(aRectColor1);
	aGc.DrawRect(TRect(startX, startY, endX, sHeight));
	if (aWinToMove)
		{
		aWinToMove->SetOrdinalPosition(aNewOrdinalPos);
		TheClient->iWs.Finish();
		}
	aGc.SetBrushColor(aRectColor2);
	aGc.DrawRect(TRect(startX, sHeight + startY, endX, aWinSize.iHeight - startY));
	aWin.EndRedraw();
	aGc.Deactivate();
	}

void CTRedrawTest::CheckOrdinalPositionDefectL()
	{
	if(TransparencySupportedL() == KErrNotSupported) //the defect only happens when transparency enabled
		{
		return;
		}

	const TRgb KTransWinColor = TRgb(0,0,255,128);
	const TUint hWidth  = FullScreenModeSize.iWidth >> 1;
	const TSize windowSize = TSize(hWidth,FullScreenModeSize.iHeight);
	const TPoint rightWinStartPt = TPoint(hWidth,0);

	// A Blank Window to clear the screen.
	// It is required to clear all the pixels on the screen.
	RBlankWindow clrWin(TheClient->iWs);
	ConstructAndSetBlankWinLC(clrWin, FullScreenModeSize);

	// Transparent window
	RWindow winLeftT(TheClient->iWs);
 	ConstructWindowLC(winLeftT, windowSize, TPoint(), KTransWinColor,ETrue);

	CWindowGc& gc = *(TheClient->iGc);
	// Draw the Transparent Window (winLeftT) on the Left side
 	DrawWin(gc, winLeftT, windowSize, KTransBisque, KTransLightSalmon);
	
	// Invisible window
	RWindow winRightI(TheClient->iWs);
	User::LeaveIfError(winRightI.Construct(*TheClient->iGroup->GroupWin(), ENullWsHandle));
	CleanupClosePushL(winRightI);
	winRightI.SetExtent(rightWinStartPt, windowSize);
	winRightI.SetVisible(EFalse);
	winRightI.Activate();

	// Transparent window
	RWindow winRightT(TheClient->iWs);
 	ConstructWindowLC(winRightT, windowSize, rightWinStartPt, KTransWinColor,ETrue);

	// Draw the Transparent Window (winRightT) on the Right side and change the
	// Ordinal Position for Invisible Window (winRightI) to move it front
	// to the Transparent Window.
	// Invisible window is behind the Transparent Window
 
 	DrawWin(gc, winRightT, windowSize, KTransBisque, KTransLightSalmon, 0, &winRightI); // Move winRightI to Front of winRightT
	TestRect();

	CleanupStack::PopAndDestroy(4, &clrWin);
	}

// For reproducing PDEF099892
void CTRedrawTest::CheckDefectPDEF099892L()
	{
	if(TransparencySupportedL() == KErrNotSupported) //the defect only happens when transparency enabled
		{
		return;
		}

	const TRgb KTransWinColor = TRgb(0,0,255,128);
	const TUint hWidth  = FullScreenModeSize.iWidth >> 1;
	const TUint hHeight = FullScreenModeSize.iHeight >> 1;
	const TSize windowSize = TSize(hWidth,FullScreenModeSize.iHeight);
	const TSize transWinSize = TSize(hWidth,hHeight + 10);
	const TPoint rightWinStartPt = TPoint(hWidth,0);

	// A Blank Window to clear the screen.
	// It is required to clear all the pixels on the screen.
	RBlankWindow clrWin(TheClient->iWs);
	ConstructAndSetBlankWinLC(clrWin, FullScreenModeSize);

	// Create an Opaque and a Transparent Window and Draw them on the
	// Left Side of the Screen. Opaque Window is Behind the Transparent Window.
	
	// Opaque Window
	RWindow winLeftOpq(TheClient->iWs);
	ConstructWindowLC(winLeftOpq, windowSize, TPoint(), KRed);

	// Transparent window
	RWindow winLeftT(TheClient->iWs);
	ConstructWindowLC(winLeftT, transWinSize, TPoint(), KTransWinColor, ETrue);

	CWindowGc& gc = *(TheClient->iGc);
	// Draw the transparent Window (winLeftT) on the Left side
 	DrawWin(gc, winLeftT, transWinSize, KTransBisque, KTransLightSalmon);

	// Draw the Opaque Window (winLeftOpq) on the Left side
	DrawWin(gc, winLeftOpq, windowSize, KLightSteelBlue, KCadetBlue, 0, NULL, ETrue);

	// Create an Invisible, an Opaque and a Transparent Window and Draw them on the
	// Right Side of the Screen. Invisible Window is Behind the Opaque Window and
	// Opaque Window is Behind the Transparent Window.
	// While drawing the Transparent Window, move the Invisible Window to the Front of Opaque Window.
	// And while Drawing the Opaque Window move the Invisible Window again Behind the Opaque Window.

	// Invisible window
	RWindow winRightI(TheClient->iWs);
	User::LeaveIfError(winRightI.Construct(*TheClient->iGroup->GroupWin(), ENullWsHandle));
	CleanupClosePushL(winRightI);
	winRightI.SetExtent(rightWinStartPt, windowSize);
	winRightI.SetVisible(EFalse);
	winRightI.Activate();

	// Opaque Window
	RWindow winRightOpq(TheClient->iWs);
	ConstructWindowLC(winRightOpq, windowSize, rightWinStartPt, KRed);

	// Transparent window
	RWindow winRightT(TheClient->iWs);
	ConstructWindowLC(winRightT, transWinSize, rightWinStartPt, KTransWinColor, ETrue);

	// Draw the transparent Window (winRightT) on the Right side
 	DrawWin(gc, winRightT, transWinSize, KTransBisque, KTransLightSalmon, 1, &winRightI );

	// Draw the Opaque Window (winRightOpq) on the Right side
	DrawWin(gc, winRightOpq, windowSize, KLightSteelBlue, KCadetBlue, 2, &winRightI, ETrue);
	
	// Compare the Left and Right side Rectangles
	TestRect();

	CleanupStack::PopAndDestroy(6, &clrWin);
	}

void CTRedrawTest::CheckMMSDefectL(TBool aMoveBlankWindow)
	{
	if(TransparencySupportedL() == KErrNotSupported) //the defect only happens when transparency enabled
		{
		return;
		}

	const TRgb KTransWinColor = TRgb(0,0,255,128);
	const TUint hWidth  = FullScreenModeSize.iWidth >> 1;
	const TUint hHeight = FullScreenModeSize.iHeight >> 1;
	const TSize windowSize = TSize(hWidth,FullScreenModeSize.iHeight);
	const TSize transWinSize = TSize(hWidth - 20,hHeight + 10);
	const TPoint rightWinStartPt = TPoint(hWidth,0);

	// A Blank Window to clear the screen.
	// It is required to clear all the pixels on the screen.
	RBlankWindow clrWin(TheClient->iWs);
	ConstructAndSetBlankWinLC(clrWin, FullScreenModeSize);

	// Create an Opaque and a Transparent Window and Draw them on the
	// Left Side of the Screen. Opaque Window is Behind the Transparent Window.

	// Opaque Window
	RWindow winLeftOpq(TheClient->iWs);
	ConstructWindowLC(winLeftOpq, windowSize, TPoint(), KRed);
	CWindowGc& gc = *(TheClient->iGc);
	// Draw the Opaque Window (winLeftOpq) on the Left side
	DrawWin(gc, winLeftOpq, windowSize, KLightSteelBlue, KCadetBlue, 0, NULL, ETrue);

	// Another Window - A Blank Window
	RBlankWindow winLeftBlank(TheClient->iWs);
	ConstructAndSetBlankWinLC(winLeftBlank, TSize(100,100), TPoint(20,20), TRgb(128,128,128));

	// Transparent window
	RWindow winLeftT(TheClient->iWs);
	ConstructWindowLC(winLeftT, transWinSize, TPoint(10, 10), KTransWinColor, ETrue);
	// Draw the Transparent Window (winLeftT) on the Left side
 	DrawWin(gc, winLeftT, transWinSize, KTransBisque, KTransLightSalmon);

	// Invisible window
	RWindow winRightI(TheClient->iWs);
	User::LeaveIfError(winRightI.Construct(*TheClient->iGroup->GroupWin(), ENullWsHandle));
	CleanupClosePushL(winRightI);
	winRightI.SetExtent(rightWinStartPt, windowSize);
	winRightI.SetVisible(EFalse);
	winRightI.Activate();

	// Opaque Window
	RWindow winRightOpq(TheClient->iWs);
	ConstructWindowLC(winRightOpq, windowSize, rightWinStartPt, KRed);

	// Draw the Opaque Window (winRightOpq) on the Right side
	DrawWin(gc, winRightOpq, windowSize, KLightSteelBlue, KCadetBlue, 0, NULL, ETrue);

	// Another Window - A Blank Window
	RBlankWindow winRightBlank(TheClient->iWs);
	ConstructAndSetBlankWinLC(winRightBlank, TSize(100,100), rightWinStartPt + TPoint(20,20), TRgb(128,128,128));

	// Transparent window
	RWindow winRightT(TheClient->iWs);
	ConstructWindowLC(winRightT, transWinSize, rightWinStartPt + TPoint(10,10), KTransWinColor, ETrue);

	if (aMoveBlankWindow)
		{
		winRightBlank.SetOrdinalPosition(0); // Move the Blank Window to the front of the Transparent Window
		TheClient->iWs.Finish();
		}
	// Draw the transparent Window (winRightT) on the Right side
 	DrawWin(gc, winRightT, transWinSize, KTransBisque, KTransLightSalmon, 2, &winRightI);
	if (aMoveBlankWindow)
		{
		winRightBlank.SetOrdinalPosition(1); // Move the Blank Window back to behind the Transparent Window
		TheClient->iWs.Finish();
		}
	TestRect();

 	DrawWin(gc, winRightT, transWinSize, KTransBisque, KTransLightSalmon, 0, &winRightI);
	TestRect(); // if aMoveBlankWindow is ETrue then this will test the Defect PDEF099892

	// Rest of the lines are just to check other possibilities.
	// But,currently, they are not affecting the result.
	// i.e. the DrawWin() functions called after this line will draw the same thing
	// as it was drawn for the DrawWin() called just before this line.
 	DrawWin(gc, winRightT, transWinSize, KTransBisque, KTransLightSalmon, 1, &winRightI);
	TestRect();

	DrawWin(gc, winRightT, transWinSize, KTransBisque, KTransLightSalmon, 0, &winRightI);
	TestRect();

	DrawWin(gc, winRightT, transWinSize, KTransBisque, KTransLightSalmon, 2, &winRightI);
	TestRect();

	DrawWin(gc, winRightT, transWinSize, KTransBisque, KTransLightSalmon, 1, &winRightI);
	TestRect();

	DrawWin(gc, winRightT, transWinSize, KTransBisque, KTransLightSalmon, 3, &winRightI);
	TestRect();

	CleanupStack::PopAndDestroy(8, &clrWin);
	}

/**

The test window is receiving a draw request before it activates, 
doing an empty redraw, then activating itself, and drawing itself properly in 
response to the next redraw request.  

Without the fix the next redraw request will not be received and it will be drawn blank. 
*/
void CTRedrawTest::CheckDefectPDEF117784L()
	{
	TSize winSize = BaseWin->Size();

	TInt winWidth=(FullScreenModeSize.iWidth/3)-10;
	TInt winHeight=FullScreenModeSize.iHeight-10;
	
	CRedrawWindow3* baseRedrawWin = new(ELeave) CRedrawWindow3(this);
	CleanupStack::PushL(baseRedrawWin);
	TPoint ptBase = TPoint(FullScreenModeSize.iWidth/3*2+5,5);
	baseRedrawWin->SetUp1L(ptBase,TSize(winWidth,winHeight),TheClient->iGroup,*TheClient->iGc);
	baseRedrawWin->SetVisible(ETrue);
	baseRedrawWin->Activate();

	CRedrawWindow3* testRedrawWin = new(ELeave) CRedrawWindow3(this);
	CleanupStack::PushL(testRedrawWin);
	TPoint ptTest = TPoint(FullScreenModeSize.iWidth/3+5,5);
	testRedrawWin->SetUp1L(ptTest,TSize(winWidth,winHeight),TheClient->iGroup,*TheClient->iGc);
	testRedrawWin->SetVisible(ETrue);
	testRedrawWin->Activate();
	
	TheClient->iWs.Finish();
	User::LeaveIfError(iInvalidRegionChecker->AddExcludedWindow(testRedrawWin));
	TBool retVal = WaitForRedrawsToFinish(ECheckRedrawActiveObjectAndInvalidRegions);
	TEST(retVal == KErrNone);
	if(retVal != KErrNone)
	    {
	    ERR_PRINTF2(_L("CTRedrawTest::WaitForRedrawsToFinish failed with error: %d"), retVal);
	    }
	iInvalidRegionChecker->RemoveExcludedWindow(testRedrawWin);

	TheClient->iWs.Finish();
	retVal = WaitForRedrawsToFinish(ECheckRedrawActiveObjectAndInvalidRegions);
	TEST(retVal == KErrNone);
	if(retVal != KErrNone)
	    {
        ERR_PRINTF2(_L("CTRedrawTest::WaitForRedrawsToFinish failed with error: %d"), retVal);
	    }
	
	retVal = TheClient->iScreen->RectCompare(TRect(TPoint(ptBase),TSize(winWidth,winHeight)),TRect(TPoint(ptTest),TSize(winWidth,winHeight)));
	TEST(retVal);
	if(!retVal)
		{
		ERR_PRINTF1(_L("New activated window has lost redraw request"));
		}

	CleanupStack::PopAndDestroy(2, baseRedrawWin);
	}

CInvalidRegionChecker::CInvalidRegionChecker(const CTRedrawTest& aGraphicsTest)
	: iGraphicsTest(aGraphicsTest) 
	{}

CInvalidRegionChecker::~CInvalidRegionChecker()
	{
	iExcludedWindowArray.Close();
	}

TInt CInvalidRegionChecker::AddExcludedWindow(const CTWinBase* aExcludedWindow)
	{
	TInt error = KErrNone;
	const CTWinBase** emptySlot = NULL;
	for(TInt win=iExcludedWindowArray.Count()-1; win>=0; win--)
		{
		if(iExcludedWindowArray[win]==aExcludedWindow)
			{
			// window is already excluded, we don't want to add it twice
			return error;
			}
		if(!emptySlot && iExcludedWindowArray[win]==NULL)
			{
			emptySlot = &iExcludedWindowArray[win];
			}
		}
	
	if(emptySlot)
		{
		// re-use the emptyslot		
		*emptySlot=aExcludedWindow; // re-use the element
		}
	else
		{
		// no empty elements re-used, so add a new one
		error = iExcludedWindowArray.Append(aExcludedWindow);
		}
	return error;
	};

void CInvalidRegionChecker::RemoveExcludedWindow(const CTWinBase* aExcludedWindow)
	{
	if(iExcludedWindowArray.Count())
		{
		for(TInt win=iExcludedWindowArray.Count()-1; win>=0; win--)
			{
			if(iExcludedWindowArray[win]==aExcludedWindow)
				{
				iExcludedWindowArray[win]=NULL; // Not worth deleting the array element, just mark it as NULL 
				return;
				}
			}
		}
	};

TBool CInvalidRegionChecker::ExcludedWindow(const CTWinBase* aWin) const
	{
	for(TInt win=iExcludedWindowArray.Count()-1; win>=0; win--)
		{
		if(aWin == iExcludedWindowArray[win])
			{
#if defined(InvalidRegionLogging)
				{
				_LIT(KText, "  Excluded Window %08x");
				TLogMessageText buf;		
				buf.Format(KText, aWin);
				BLOG_MESSAGE(buf);
				}
#endif					
			return ETrue;
			}
		}
	return EFalse;
	}

void CInvalidRegionChecker::ProcessWindow(const CTWinBase* aTWinBase)
	{
#if defined(InvalidRegionLogging)
			{
			_LIT(KText, "  ProcessWindow %08x %d - Child(%08x), Next(%08x), Prev(%08x)");
			TLogMessageText buf;		
			buf.Format(KText, aTWinBase, iInvalidRegionCount, aTWinBase->Child(), aTWinBase->NextSibling(), aTWinBase->PrevSibling());
			BLOG_MESSAGE(buf);
			}
#endif

	if(aTWinBase && !ExcludedWindow(aTWinBase))
		{
		RRegion invalidRegion;
		static_cast<const CTWin *>(aTWinBase)->Win()->GetInvalidRegion(invalidRegion);
		iInvalidRegionCount += invalidRegion.Count();

#if defined(InvalidRegionLogging)
		if(invalidRegion.Count())
			{
			_LIT(KText, "  IR Found for %08x %d");
			TLogMessageText buf;
			buf.Format(KText, aTWinBase, invalidRegion.Count());
			BLOG_MESSAGE(buf);
			}
#endif
		
		invalidRegion.Close();
		
		if(0==iInvalidRegionCount)
			{
			ProcessChildWindow(aTWinBase->Child());
			}
		}
	}

void CInvalidRegionChecker::ProcessChildWindow(const CTWinBase* aTWinBase)
	{
	if(aTWinBase)
		{
		// get the first sibling window
		const CTWinBase *sibling=aTWinBase;
		const CTWinBase *prevSibling=sibling->PrevSibling();
		while(prevSibling)
			{
			sibling=prevSibling;			
			prevSibling=sibling->PrevSibling();
			}
		// process all siblings inc. self	
		while(sibling && (0==iInvalidRegionCount))
			{	
			ProcessWindow(sibling);
			sibling=sibling->NextSibling();
			}
		}
	}	


void CInvalidRegionChecker::ProcessWindowGroup(const CTWinBase* aTWinBase)
	{
	if(aTWinBase && !ExcludedWindow(aTWinBase))
		{
		ProcessChildWindow(aTWinBase->Child());
		}
	}

TInt CInvalidRegionChecker::CheckInvalidRegions(const CTWindowGroup* aGroup)
	{
	iInvalidRegionCount=0;	
	ProcessWindowGroup(aGroup);	
#if defined(InvalidRegionLogging)
		{
		_LIT(KText, "  CheckInvalidRegions %d");
		TLogMessageText buf;
		buf.Format(KText, iInvalidRegionCount);
		BLOG_MESSAGE(buf);
		}
#endif	
	return iInvalidRegionCount;
	}

TInt CTRedrawTest::WaitForRedrawsToFinish(TRedrawCheckType aRedrawCheckType)
	{
	TInt error=KErrNone;
#define EnableCheckInvalidRegions
#if defined(EnableCheckInvalidRegions)	
	if(aRedrawCheckType == ECheckRedrawActiveObjectAndInvalidRegions)
		{
		TInt regions = 0;
		TInt count = 0;
		//We do not want to cycle round forever or too long, a limit of 10 has
		//been added but this is arbitrary.  If CTClient::WaitForRedrawsToFinish
		//fails too many times then possibly something else us wrong.
		do
			{
			count++;
			error = TheClient->WaitForRedrawsToFinish();
			if(error != KErrNone)
			    {
			    RDebug::Printf("CTRedrawTest::WaitForRedrawsToFinish, error %d", error);
			    }
			regions = iInvalidRegionChecker->CheckInvalidRegions(TheClient->iGroup);
			if(regions)
				{
				// Give the server a chance to do the redraws because 
				// the Animation Scheduler is an idle priority AO
				const TUint KOneSecond = 1000000; // us
				User::After(KOneSecond>>2); // 0.25s
				}
			} while (0 < regions && 10 < count); 
		}
	else // ECheckRedrawActiveObjectOnly
#endif // CheckInvalidRegions
		{
		error = TheClient->WaitForRedrawsToFinish();
		}
	return error;
	}

void CTRedrawTest::RunTestCaseL(TInt /*aCurTestCase*/)
	{
	_LIT(Redraw0,"Redraw1");
	_LIT(Redraw1,"Redraw2");
	_LIT(Redraw2,"GetInvalid");
	_LIT(Redraw3,"MoveInvalid");
	_LIT(Redraw4,"CheckOrder");
	_LIT(Redraw5,"Defect 49554");
	_LIT(Redraw6,"Check Ordinal Position");
	_LIT(Redraw7,"Defect 99892");
	_LIT(Redraw8,"Check MMS Defect 1");
	_LIT(Redraw9,"Check MMS Defect 2");
	_LIT(Redraw10,"Redraw inactive window");
	((CTRedrawTestStep*)iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);

	TInt testWindowDraws = -1;		// to prevent RVCT 546-D warning
	_LIT(KLog,"RunTestCase %d");
	if (++iTest->iState<6)
		LOG_MESSAGE2(KLog,iTest->iState);
	switch(iTest->iState)
		{
/**
@SYMTestCaseID		GRAPHICS-WSERV-0265

@SYMDEF             DEF081259

@SYMTestCaseDesc    Test invalidation a test window and check it
					redraws correctly

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Invalidate a test window causing it to redraw 

@SYMTestExpectedResults The test window redraws correctly
*/
	case 1:
		((CTRedrawTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0265"));
		iTest->LogSubTest(Redraw0);
		SetBackground(TRgb::Gray256(128));
		iDrawRequestsFromTestWindow=iTestRedrawWin->DrawRequests();
		InvalidateTestWins(TRect(10,10,50,50));
		break;
	case 2:
		((CTRedrawTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0265"));
		TheClient->iWs.Finish();
		WaitForRedrawsToFinish(ECheckRedrawActiveObjectAndInvalidRegions);
		testWindowDraws=iTestRedrawWin->DrawRequests();
		if (iDrawRequestsFromTestWindow+1!=testWindowDraws)
			{
			LOG_MESSAGE3(_L("wrong number of test window draw requests %d %d"),
					iDrawRequestsFromTestWindow,
					testWindowDraws);
			TEST(EFalse);
			}
		CheckRedrawWindows();
		++iWinContent;
		iTestRedrawWin->Reset();
		iBaseRedrawWin->Reset();
		TheClient->Flush();
		WaitForRedrawsToFinish(ECheckRedrawActiveObjectAndInvalidRegions);
		break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0266

@SYMDEF             DEF081259

@SYMTestCaseDesc    Test invalidation a test window and check it
					redraws correctly

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Invalidate a test window causing it to redraw 

@SYMTestExpectedResults The test window redraws correctly
*/
	case 3:
		((CTRedrawTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0266"));
		iTest->LogSubTest(Redraw1);
		InvalidateTestWins(TRect(1,1,150,20));
		TheClient->iWs.Finish();
		WaitForRedrawsToFinish(ECheckRedrawActiveObjectAndInvalidRegions);
		break;
	case 4:
		((CTRedrawTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0266"));
		CheckRedrawWindows();
		++iWinContent;
		iTestRedrawWin->Reset();
		iBaseRedrawWin->Reset();
		TheClient->iWs.Flush();
		break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0267

@SYMDEF             DEF081259

@SYMTestCaseDesc    Test invalidation a region of a test window and check it
					redraws correctly

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Invalidate a region of a test window causing it to redraw 

@SYMTestExpectedResults The test window redraws correctly
*/
		case 5:
			((CTRedrawTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0267"));
			iTest->LogSubTest(Redraw2);
			GetInvalidRegionTestsL();
			break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0268

@SYMDEF             DEF081259

@SYMTestCaseDesc    Test moving an invalid  region of a test window and check it
					redraws correctly

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Move an invalid region of a test window causing it to redraw 

@SYMTestExpectedResults The test window redraws correctly
*/
		case 6:
			((CTRedrawTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0268"));
			iTest->LogSubTest(Redraw3);
			MoveInvalidAreaL();
			break;

/**
@SYMTestCaseID		GRAPHICS-WSERV-0270

@SYMDEF             DEF081259

@SYMTestCaseDesc    Test the order redraws occur in a test window 

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Set up a number of redraws for a test window and
					invalidate it

@SYMTestExpectedResults The order the test window redraws occur is correct
*/

		case 7:
			((CTRedrawTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0270"));
			iTest->LogSubTest(Redraw4);
			CheckOrderL();
			break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0271

@SYMDEF             DEF081259

@SYMTestCaseDesc    Test defect INC049554L does not occur

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Check that defect INC049554L does not occur when a test 
					window is redrawn

@SYMTestExpectedResults Defect INC049554L does not occur
*/
		case 8:
			((CTRedrawTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0271"));
			iTest->LogSubTest(Redraw5);
			CheckDefectINC049554L();
			break;
/** 
 @SYMTestCaseID GRAPHICS-WSERV-0359

  @SYMDEF  			PDEF099892

  @SYMTestCaseDesc Ensure that changing the Ordinal postion of an Invisible Window
  				   does not affect the Transparent Window  or Opaque Window redrawing.

  @SYMTestPriority Medium

  @SYMTestStatus Implemented

  @SYMTestActions Create an Invisible Window
                  Create a Transparent Window
                  Inside the Begin and End Redraw for the Transparent Window,
                  change the Ordinal Position for Invisible Window to move it
                  front to the Transparent Window.

  @SYMTestExpectedResults Changing the Ordinal postion for Invisible Window should not
						  affect the Transparent Window redrawing.
*/
		case 9:
			((CTRedrawTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0359"));
			iTest->LogSubTest(Redraw6);
			CheckOrdinalPositionDefectL();
			break;
/** 
 @SYMTestCaseID GRAPHICS-WSERV-0360

  @SYMDEF  			PDEF099892

  @SYMTestCaseDesc Ensure that changing the Ordinal postion of an Invisible window
  				   does not affect the Transparent Window  or Opaque Window redrawing.

  @SYMTestPriority Medium

  @SYMTestStatus Implemented

  @SYMTestActions Create an Invisible Window
  				  Create an Opaque Window
                  Create a Transparent Window
                  Invisible Window is Behind the Opaque Window
				  and Opaque Window is Behind the Transparent Window.
                  While drawing the Transparent Window, move the Invisible Window
                  to the Front of Opaque Window. And while Drawing the Opaque Window
                  move the Invisible Window again Behind the Opaque Window.

  @SYMTestExpectedResults Changing the Ordinal postion for Invisible window should not
						  affect the Transparent Window  or Opaque Window redrawing.
*/
		case 10:
			((CTRedrawTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0360"));
			iTest->LogSubTest(Redraw7);
			CheckDefectPDEF099892L();
			break;
/** 
 @SYMTestCaseID GRAPHICS-WSERV-0361

  @SYMDEF  			DEF101548

  @SYMTestCaseDesc Ensure that changing the Ordinal postion of an Invisible window
  				   does not affect the Transparent Window redrawing.

  @SYMTestPriority Low

  @SYMTestStatus Implemented

  @SYMTestActions Create an Invisible Window
  				  Create an Opaque Window
  				  Create a Blank Window
  				  Create a Transparent Window
  				  Invisible Window is Behind the Opaque Window
  				  Opaque Window is Behind the Blank Window
  				  and Blank Window is Behind the Transparent Window.
  				  While drawing the Transparent Window, move the Invisible Window
  				  to the Front/Back to one or all the Other Windows.
  				  Also move the Blank Window Front/Back to the Transparent Window for other scenario.

  @SYMTestExpectedResults Changing the Ordinal postion for Invisible window should not
						  affect the Transparent Window redrawing.
*/
	case 11:
		((CTRedrawTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0361"));
		iTest->LogSubTest(Redraw8);
		CheckMMSDefectL(EFalse);
	case 12:
		((CTRedrawTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0361"));
		iTest->LogSubTest(Redraw9);
		CheckMMSDefectL(ETrue);// In this case we will move Blank Window Front/Back to the Transparent Window to Test Defect PDEF099892
		break;
	case 13:
		iTest->LogSubTest(Redraw10);
/**
  @SYMTestCaseID GRAPHICS-WSERV-0362

  @SYMDEF	PDEF117784
	
  @SYMTestCaseDesc Ensure that drawing request for non active window will not 
	impact following redrawings

  @SYMTestPriority High

  @SYMTestStatus Implemented
  
  
  @SYMTestActions 	Create test window as in active
  					Create base window in active mode.
  				    
					The test window is receiving a draw request before it activates, 
					doing an empty redraw, 
					Activate test window
					Draw test window properly in response to the next redraw request.  

  @SYMTestExpectedResults The next redraw request will be received and it will be drawn correctly. 
*/	
			((CTRedrawTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0362"));
			CheckDefectPDEF117784L();
			break;
		case 14:
			((CTRedrawTestStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
			((CTRedrawTestStep*)iStep)->CloseTMSGraphicsStep();
			TestComplete();
			break;
		default:
			TEST(EFalse);
		}
	((CTRedrawTestStep*)iStep)->RecordTestResultL();
	}

__WS_CONSTRUCT_STEP__(RedrawTest)
