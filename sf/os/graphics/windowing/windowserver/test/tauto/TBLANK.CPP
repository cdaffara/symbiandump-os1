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
// Test blank windows
// 
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/


#include "TBLANK.H"


CBaseWindow::CBaseWindow() : CTWin()
	{}

void CBaseWindow::ConstructL(CTWinBase &parent)
	{
	CTWin::ConstructL(parent);
	}

void CBaseWindow::Draw()
	{
	iGc->SetBrushColor(iRgb);
	iGc->Clear();
	}

void CBaseWindow::SetColor(const TRgb &aRgb)
	{
	iRgb=aRgb;
	iGc->Activate(iWin);
	Draw();
	iGc->Deactivate();
	}

//

TInt DestructCallbackBlank(TAny *aParam)
	{
	((CTBlank *)aParam)->doDestruct();
	return(0);
	}


CTBlank::CTBlank(CTestStep* aStep):
	CTWsGraphicsBase(aStep)
	{
	INFO_PRINTF1(_L("Testing TBlank functions"));
	}

void CTBlank::doDestruct()
	{
	BaseWin->SetVisible(ETrue);
	TestWin->SetVisible(ETrue);
	delete iBaseWin;
	delete iTestWin;
	iBaseWin = NULL;
	iTestWin = NULL;
//	CActiveScheduler::Stop();
	}

CTBlank::~CTBlank()
	{
	TCallBack callBack(DestructCallbackBlank,this);
	TheClient->SetRedrawCancelFunction(callBack);
//	CActiveScheduler::Start();
	}

void CTBlank::ConstructL()
	{
	BaseWin->SetVisible(EFalse);
	TestWin->SetVisible(EFalse);
	}

void CTBlank::SetColor(const TRgb &aRgb)
	{
	iTestWin->SetColor(aRgb);
	iBaseWin->SetColor(aRgb);
	}

void CTBlank::InvalidateTestWin(const TRect &aRect)
	{
	TRect rect(aRect);
	rect.Move(iTestWin->BaseWin()->InquireOffset(*(TheClient->iGroup->WinTreeNode())));
	CTUser::Splat(TheClient, rect,TRgb(0,0,0));
	}

void CTBlank::CheckBlankWindows()
	{
	User::Heap().Check();
	CheckRect(iBaseWin,iTestWin,TRect(BaseWin->Size()),_L("CheckBlankWindows - CheckRect(iBaseWin,iTestWin,TRect(BaseWin->Size()) failed"));
	}

void CTBlank::RunTestCaseL(TInt /*aCurTestCase*/)
	{
	TSize screenSize;
	TInt winWidth;
	TInt winHeight;
	((CTBlankStep*)iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);

	switch(++iTest->iState)
		{
/**
@SYMTestCaseID		GRAPHICS-WSERV-0022

@SYMDEF             DEF081259

@SYMTestCaseDesc    Create a base and test blank windows and invalidate an
					area of the test window

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Two windows are created and an area of a window is 
					invalidated

@SYMTestExpectedResults Windows create and invalidate without error
*/
	case 1:
		((CTBlankStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0022"));
		iTest->LogSubTest(_L("Blank 1"));
		screenSize=TheClient->iGroup->Size();
		winWidth=(screenSize.iWidth/3)-10;
		winHeight=screenSize.iHeight-10;
		iBaseWin=new(ELeave) CBaseWindow();
		iBaseWin->SetUpL(TPoint(screenSize.iWidth/3+5,5),TSize(winWidth,winHeight),TheClient->iGroup,*TheClient->iGc);
		iBaseWin->SetColor(TRgb(255,255,255));
		iTestWin=new(ELeave) CTBlankWindow();	
		iTestWin->SetUpL(TPoint(screenSize.iWidth/3*2+5,5),TSize(winWidth,winHeight),TheClient->iGroup,*TheClient->iGc);				
		SetColor(TRgb::Gray4(2));
		InvalidateTestWin(TRect(10,10,50,50));
		break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0023

@SYMDEF             DEF081259

@SYMTestCaseDesc    Check that a base blank window is the same as a
					test window which has had an area invalidated

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Check the test window is the same as the base window 

@SYMTestExpectedResults The windows are identical
*/
	case 2:
		((CTBlankStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0023"));
		iTest->LogSubTest(_L("Check Blank 1"));
		CheckBlankWindows();
		break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0024

@SYMDEF             DEF081259

@SYMTestCaseDesc    Set a color and invalidate two areas of a test blank
					window

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Invalidate two areas of a test blank window 

@SYMTestExpectedResults The window areas are invalidated without error
*/
	case 3:
		((CTBlankStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0024"));
		iTest->LogSubTest(_L("Blank 2"));
		SetColor(TRgb(0,0,0));
		InvalidateTestWin(TRect(1,1,150,20));
		InvalidateTestWin(TRect(10,-10,20,90));
		break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0025

@SYMDEF             DEF081259

@SYMTestCaseDesc    Check a base blank window is identical to a test 
					blank window which has had two areas invalidated

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Check the test window is the same as the base window 

@SYMTestExpectedResults The windows are identical
*/
	case 4:
		((CTBlankStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0025"));
		iTest->LogSubTest(_L("Check Blank 2"));
		CheckBlankWindows();
		break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0026

@SYMDEF             DEF081259

@SYMTestCaseDesc    Set a color and invalidate three areas of a test blank
					window

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Invalidate three areas of a test blank window 

@SYMTestExpectedResults The window areas are invalidated without error
*/
	case 5:
		((CTBlankStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0026"));
		iTest->LogSubTest(_L("Blank 3"));
		SetColor(TRgb(255,255,255));
		InvalidateTestWin(TRect(-1000,100,1000,120));
		InvalidateTestWin(TRect(1,1,150,20));
		InvalidateTestWin(TRect(10,30,20,60));
		break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0027

@@SYMDEF            DEF081259

@SYMTestCaseDesc    Check a base blank window is identical to a test 
					blank window which has had three areas invalidated

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Check the test window is the same as the base window 

@SYMTestExpectedResults The windows are identical
*/
	case 6:
		((CTBlankStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0027"));
		iTest->LogSubTest(_L("Check Blank 3"));
		CheckBlankWindows();
		delete iBaseWin;
		delete iTestWin;
		iBaseWin = NULL;
		iTestWin = NULL;
		break;	
	case 7:
		((CTBlankStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
		((CTBlankStep*)iStep)->CloseTMSGraphicsStep();
		TestComplete();
		break;
		}
	((CTBlankStep*)iStep)->RecordTestResultL();
	}

__WS_CONSTRUCT_STEP__(Blank)
