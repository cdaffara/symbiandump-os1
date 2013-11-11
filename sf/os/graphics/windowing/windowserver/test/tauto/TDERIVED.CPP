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
// Test classes derived from Wserv client side classes
// 
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/

#include "TDERIVED.H"

CTDerived::CTDerived(CTestStep* aStep) : CTWsGraphicsBase(aStep)
	{
	}

CTDerived::~CTDerived()
	{
	}

//
// CDerivedWindowGc //
//

CDerivedWindowGc::CDerivedWindowGc(CWsScreenDevice *aDevice) : CWindowGc(aDevice)
	{
	}

CDerivedWindowGc::~CDerivedWindowGc()
	{
	}

void CDerivedWindowGc::DrawLine(const TPoint &aPoint1,const TPoint &aPoint2)
	{
	CWindowGc::DrawLine(aPoint1+TPoint(10,10), aPoint2-TPoint(10,10));
	}

//
// RDerivedSession //
//

TInt RDerivedSession::ResourceCount()
	{
	return(RWsSession::ResourceCount()+1);
	}

void RDerivedSession::EnableAutoFlush()
	{
	SetAutoFlush(ETrue);
	}

//
// Derived windows //
//

RDerivedBlankWindow::RDerivedBlankWindow()
	{}
RDerivedBlankWindow::RDerivedBlankWindow(RWsSession &aWs) : RBlankWindow(aWs)
	{
	}

void RDerivedBlankWindow::SetColor(TRgb aColor)
	{
	RBlankWindow::SetColor(~aColor);
	}

void RDerivedBlankWindow::SetColorGray()
	{
	RBlankWindow::SetColor(TRgb::Gray16(8));
	}

RDerivedWindow::RDerivedWindow()
	{}
RDerivedWindow::RDerivedWindow(RWsSession &aWs) : RWindow(aWs)
	{
	}

void RDerivedWindow::SetBackgroundColor(TRgb aColor)
	{
	RWindow::SetBackgroundColor(~aColor);
	}

void RDerivedWindow::SetBackgroundColorGray()
	{
	RWindow::SetBackgroundColor(TRgb::Gray16(8));
	}

RDerivedBackedUpWindow::RDerivedBackedUpWindow()
	{}
RDerivedBackedUpWindow::RDerivedBackedUpWindow(RWsSession &aWs) : RBackedUpWindow(aWs)
	{
	}

TInt RDerivedBackedUpWindow::BitmapHandle()
	{
	MaintainBackup();
	return(RBackedUpWindow::BitmapHandle()+1);
	};

RDerivedWindowGroup::RDerivedWindowGroup()
	{}

RDerivedWindowGroup::RDerivedWindowGroup(RWsSession &aWs) : RWindowGroup(aWs)
	{
	}

TName RDerivedWindowGroup::Name() const
	{
	return(_L("Fred"));
	}

//
// CDerivedScreenDevice //
//

CDerivedScreenDevice::CDerivedScreenDevice()
	{}

CDerivedScreenDevice::CDerivedScreenDevice(RWsSession &aWs) : CWsScreenDevice(aWs)
	{}

TSize CDerivedScreenDevice::SizeInPixels() const
	{
	TSize size=CWsScreenDevice::SizeInPixels();
	return(TSize(size.iWidth*2,size.iHeight*2));
	}

//
// Derived Sprite/Pointer cursor //
//

RDerivedSprite::RDerivedSprite()
	{
	}

RDerivedSprite::RDerivedSprite(RWsSession &aWs) : RWsSprite(aWs)
	{
	}

RDerivedPointerCursor::RDerivedPointerCursor()
	{
	}

RDerivedPointerCursor::RDerivedPointerCursor(RWsSession &aWs) : RWsPointerCursor(aWs)
	{
	}

//

void CTDerived::ConstructL()
	{
	TheGc->Activate(*BaseWin->Win());
	TheGc->Clear();
	TheGc->SetBrushColor(TRgb::Gray16(12));
	TheGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	TheGc->SetPenStyle(CGraphicsContext::ENullPen);
	TheGc->DrawRect(TRect(BaseWin->Win()->Size()));
	TheGc->Deactivate();
	TheGc->Activate(*TestWin->Win());
	TheGc->Clear();
	TheGc->SetBrushColor(TRgb::Gray16(4));
	TheGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	TheGc->SetPenStyle(CGraphicsContext::ENullPen);
	TheGc->DrawRect(TRect(TestWin->Win()->Size()));
	TheGc->Deactivate();
	}

void CTDerived::DerivedWindowGcL()
	{
	CDerivedWindowGc *dgc=new(ELeave) CDerivedWindowGc(TheClient->iScreen);
	dgc->Construct();
	dgc->Activate(*TestWin->Win());
	dgc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	TestWin->Win()->BeginRedraw();
	dgc->DrawRect(TRect(TestWin->Win()->Size()));
	dgc->DrawLine(TPoint(0,0),TestWin->Win()->Size().AsPoint());
	TestWin->Win()->EndRedraw();
	dgc->Deactivate();
	delete dgc;
	dgc = NULL;
	CWindowGc *wgc=new(ELeave) CWindowGc(TheClient->iScreen);
	wgc->Construct();
	wgc->Activate(*BaseWin->Win());
	wgc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	BaseWin->Win()->BeginRedraw();
	wgc->DrawRect(TRect(TestWin->Win()->Size()));
	wgc->DrawLine(TPoint(10,10),TestWin->Win()->Size().AsPoint()-TPoint(10,10));
	BaseWin->Win()->EndRedraw();
	wgc->Deactivate();
	delete wgc;
	wgc = NULL;
	CompareWindows(_L("CTDerived::DerivedWindowGcL()"));
	}

void CTDerived::DerivedSession()
	{
	TInt err;
	err = iDSession.Connect();
	if (err!=KErrNone)
		return;
	
	// use the correct screen
	CWsScreenDevice* screen = NULL;
	TRAP(err, screen = new (ELeave) CWsScreenDevice(iDSession));
	if (err!=KErrNone)
		{
		iDSession.Close();
		return;
		}
	
	if ((err=screen->Construct(iTest->iScreenNumber))!=KErrNone)
		{
		delete screen;
		iDSession.Close();
		return;
		}

	delete screen;
	
	TInt retVal = iDSession.ResourceCount();	
	TEST(retVal==1);
	if (retVal!=1)
		INFO_PRINTF3(_L("iDSession.ResourceCount() return value - Expected: %d, Actual: %d"), 1, retVal);		
	
	iDSession.SetAutoFlush(EFalse);
	iDSession.EnableAutoFlush();
	
	TBool retBool = iDSession.SetAutoFlush(EFalse);	
	TEST(retBool);
	if (!retBool)
		INFO_PRINTF3(_L("iDSession.SetAutoFlush(EFalse) return value - Expected: %d, Actual: %d"), ETrue, retBool);		
	
	iDSession.Close();
	}

void CTDerived::DerivedWindows()
	{
//
// Window group
//
	iDGroup=RDerivedWindowGroup(TheClient->iWs);
	iDGroup.Construct(222);
	iDGroup.Name();
//
// Redraw up window
//
	iDRedraw=RDerivedWindow(TheClient->iWs);
	iDRedraw.Construct(iDGroup,12);
	iDRedraw.SetBackgroundColor(TRgb(1,2,3));
	iDRedraw.SetBackgroundColorGray();
	iDRedraw.Close();
//
// Blank up window
//
	iDBlank=RDerivedBlankWindow(TheClient->iWs);
	iDBlank.Construct(iDGroup,123);
	iDBlank.SetColor(TRgb(1,20,50));
	iDBlank.SetColorGray();
	iDBlank.Close();
//
// Backed up window
//
	iDBackedUp=RDerivedBackedUpWindow(TheClient->iWs);
	iDBackedUp.Construct(iDGroup,EGray4,1234);
	iDBackedUp.BitmapHandle();
	iDBackedUp.Close();
//
	iDGroup.Close();
	}

void CTDerived::DerivedScreenDeviceL()
	{
	CWsScreenDevice *wsd=new(ELeave) CWsScreenDevice(TheClient->iWs);
	User::LeaveIfError(wsd->Construct(iTest->iScreenNumber));
	TSize size1(wsd->SizeInPixels());
	delete wsd;
	CDerivedScreenDevice *dsd=new(ELeave) CDerivedScreenDevice(TheClient->iWs);
	User::LeaveIfError(dsd->Construct(iTest->iScreenNumber));
	TSize size2(dsd->SizeInPixels());
	delete dsd;
	TEST(size2.iWidth==size1.iWidth*2 && size2.iHeight==size1.iHeight*2);
	}

void CTDerived::DerivedSprite()
	{
	iDSprite=RDerivedSprite(TheClient->iWs);
	TSpriteMember spm;
	spm.iBitmap=NULL;
	spm.iMaskBitmap=NULL;
	spm.iInvertMask=EFalse;
	spm.iOffset=TPoint(0,0);				// Offset from sprites central position
	spm.iInterval=TTimeIntervalMicroSeconds32(1000000);
	iDSprite.Construct(*TestWin->Win(), TPoint(10,10), 0);
	iDSprite.Close();
//
	iDPCursor=RDerivedPointerCursor(TheClient->iWs);
	iDPCursor.Construct(0);
	iDPCursor.Close();
	};

void CTDerived::RunTestCaseL(TInt /*aCurTestCase*/)
	{
	((CTDerivedStep*)iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);
	switch(++iTest->iState)
		{
/**
@SYMTestCaseID		GRAPHICS-WSERV-0075

@SYMDEF             DEF081259

@SYMTestCaseDesc    Test derived window graphics context

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Draw using a derived window gc and a normal
					window gc and check the dawing is the same

@SYMTestExpectedResults Drawing is the same with a derived window gc
*/
		case 1:
			((CTDerivedStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0075"));
			iTest->LogSubTest(_L("Derived Gc"));
			DerivedWindowGcL();
			break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0076

@SYMDEF             DEF081259

@SYMTestCaseDesc    Test derived window server session

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Test that a derived window server session functions
					the same as a normal window server session

@SYMTestExpectedResults The derived window server session functions correctly
*/
		case 2:
			((CTDerivedStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0076"));
			iTest->LogSubTest(_L("Derived Session"));
			DerivedSession();
			break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0077

@SYMDEF             DEF081259

@SYMTestCaseDesc    Test derived windows and window groups function correctly

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Test that a derived windows and window groups function
					the same as the classes they are derived from

@SYMTestExpectedResults The derived windows and window group functions correctly
*/
		case 3:
			((CTDerivedStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0077"));
			iTest->LogSubTest(_L("Derived Blank Window"));
			DerivedWindows();
			break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0078

@SYMDEF             DEF081259

@SYMTestCaseDesc    Test derived screen devices function correctly

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Test that a derived screen device function
					the same as the class it is derived from

@SYMTestExpectedResults The derived screen device functions correctly
*/
		case 4:
			((CTDerivedStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0078"));
			iTest->LogSubTest(_L("Derived Screed Device"));
			DerivedScreenDeviceL();
			break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0079

@SYMDEF             DEF081259

@SYMTestCaseDesc    Test derived sprites and pointer cursors function
					correctly

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Test that a derived sprite and pointer cursor
					function correctly

@SYMTestExpectedResults The derived sprite and pointer cursor function 
					correctly
*/
		case 5:
			((CTDerivedStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0079"));
			iTest->LogSubTest(_L("Derived Sprites"));
			DerivedSprite();
			break;
		case 6:
			((CTDerivedStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
			((CTDerivedStep*)iStep)->CloseTMSGraphicsStep();
			TestComplete();	
			break;
		}
	((CTDerivedStep*)iStep)->RecordTestResultL();
	}
	
__WS_CONSTRUCT_STEP__(Derived)
