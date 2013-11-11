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
// Test out of memory redraws
// 
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/

#include "TOOMDRAW.H"

void DrawTestScreen(CWindowGc *aGc, const TSize &aSize)
	{
	aGc->SetPenSize(TSize(10,10));
	aGc->SetBrushStyle(CGraphicsContext::EVerticalHatchBrush);
	aGc->DrawRect(TRect(aSize));
	}

CTestWin::CTestWin()
	{
	}

void CTestWin::Draw()
	{
#if defined(FULLLOGGING)
	_LIT(KLog,"Drawing Test Win:%d  %d,%d");
	TLogMessageText buf;
	buf.Format(KLog,Size().iWidth>320?1:0,Size().iWidth,Size().iHeight);
	TheClient->LogMessage(buf);
#endif
	DrawTestScreen(iGc,Size());
	}

CBaseTestWin::CBaseTestWin() : CTBackedUpWin(EGray4)
	{}

void CBaseTestWin::Resized(const TSize &aNewSize)
	{
	TheGc->Activate(*DrawableWin());
	DrawTestScreen(TheGc,aNewSize);
	TheGc->Deactivate();
	}

CToomDraw::CToomDraw(CTestStep* aStep):
	CTWsGraphicsBase(aStep)
	{
	}

CToomDraw::~CToomDraw()
	{
	TheClient->iWs.HeapSetFail(RHeap::ENone,0);		//In case the test crashes and it hasn't been reset
	DeleteWindows();
	}

void CToomDraw::DeleteWindows()
	{
	delete iTestWin;
	iTestWin=NULL;
	delete iBaseWin;
	iBaseWin=NULL;
	delete iBackWin;
	iBackWin=NULL;
	delete iBlankWin;
	iBlankWin=NULL;
	}

void CToomDraw::ConstructL()
	{
	iScrSize=TheClient->iScreen->SizeInPixels();
	iBlankWin=new(ELeave) CTBlankWindow;
	iBlankWin->ConstructL(*TheClient->iGroup);
	iBlankWin->SetColor(TRgb::Gray4(2));		//Grey
	iBlankWin->Activate();
	iBackWin=new(ELeave) CTestWin();
	iBackWin->SetUpL(TPoint(0,iScrSize.iHeight/4), TSize(iScrSize.iWidth,iScrSize.iHeight/2), TheClient->iGroup, *TheClient->iGc);
	iBaseWin=new(ELeave) CBaseTestWin();
	iBaseWin->SetUpL(TPoint(iScrSize.iWidth*2/3,0), TSize(iScrSize.iWidth/3,iScrSize.iHeight), TheClient->iGroup, *TheClient->iGc);
	}

void CToomDraw::CreateTestWinL()
	{
	iTestWin=new(ELeave) CTestWin();
	iTestWin->ConstructL(*TheClient->iGroup);
	iTestWin->SetExtL(TPoint(0,0), TSize(iScrSize.iWidth/3,iScrSize.iHeight));
	iTestWin->AssignGC(*TheClient->iGc);
	iTestWin->BaseWin()->SetShadowHeight(2);
	}

void CToomDraw::TriggerOomDrawsL(TInt aCount)
	{
	if(iTestWin)
		{
		iTestWin->SetExtL(TPoint(),iScrSize);		
		}
	delete iTestWin;
	iTestWin=NULL;
	CreateTestWinL();
	TheClient->iWs.HeapSetFail(RHeap::EDeterministic,aCount);
	iTestWin->Activate();
	TheClient->iWs.Finish();
	}

TBool CToomDraw::CheckWindowsL()
	{
	TLogMessageText buf;
	TBool pass = DoCheckRect(iTestWin,iBaseWin);
	if (pass)
		{
		TEST(ETrue);		//Adds one to test count
	#if defined(FULLLOGGING)
		_LIT(KLog,"OOM Draw Test Pass, State=%d");
		buf.Format(KLog,iState);
	#endif
		iRetryCount=0;
		}
	else
		{
		if (iRetryCount==4)
			{
			if ((iTest->RedrawStoreTypeL()==EPartialRedraw_PreserveStoredCmds ||
			iTest->RedrawStoreTypeL()==EPartialRedraw_FullRedrawSupport) && (iState==19 || iState==30) ||
			((iState>=2 && iState<=6) && (iTest->RedrawStoreTypeL()==EPartialRedraw_None || iTest->RedrawStoreTypeL()==EPartialRedraw_FullRedrawSupport)))
				{
			#if defined(FULLLOGGING)
				_LIT(KLog,"OOM Draw Test Known F a i l, State=%d");
				buf.Format(KLog,iState);
			#endif
				pass=ETrue;
				}
			else
				{
				if (iFailAt==0)
					iFailAt=iState;
				++iFails; //mark this test as failed
			#if defined(LOGGING)
				_LIT(KLog,"OOM Draw Test FAIL, State=%d  <------- FAIL");
				buf.Format(KLog,iState);
			#endif
				++iState; //and advance to next test
				}
			iRetryCount=0;
			}
		else
			{
		#if defined(FULLLOGGING)
			_LIT(KLog,"OOM Draw Test F a i l, State=%d retrying");
			buf.Format(KLog,iState);
		#endif
			if (++iRetryCount%2==0)
				iBackWin->DrawNow();
			}
		}
	if (buf.Length()>0)
		TheClient->LogMessage(buf);
	return pass;
	}

void CToomDraw::RunTestCaseL(TInt aCurTestCase)
	{
	TBool testFinished = false;
	((CToomDrawStep*)iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);
	switch(aCurTestCase)
		{
/**
@SYMTestCaseID		GRAPHICS-WSERV-0110

@SYMDEF             DEF081259

@SYMTestCaseDesc    Test trying to draw when out of memory

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Set out of memory and try drawing

@SYMTestExpectedResults Out of memory drawing is handle correctly
*/
	case 1:
		((CToomDrawStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0110"));
		while (!testFinished)
			{
		if (iDoCheck)
			{
			TheClient->iWs.HeapSetFail(RHeap::ENone,0);
			TheClient->WaitForRedrawsToFinish();
			if (CheckWindowsL())
				{
				++iState;
				if (iState==EMaxOOMState)
					DeleteWindows();
				}
			iDoCheck=EFalse;
			TheClient->iWs.Finish();
			}
		else
			{
			if (iState==EMaxOOMState)
				{
				if (iFails>0)
					{
			#if defined(LOGING)
					_LIT(KLog,"OOM Draw Test Failed, First Fail At %d, Number of Fails %d");
					TLogMessageText buf;
					buf.Format(KLog,iFailAt,iFails);
					TheClient->LogMessage(buf);
			#endif
					TEST(iFails==0);
					}
				User::After(500000);	// Let the rest of the world catch up after OOM death
				testFinished = true;
				}
			else if (iState < 3 && iTest->IsFullRomL())
				testFinished = true;
			else
				TriggerOomDrawsL(iState);
			iDoCheck=ETrue;
			}
			}
		break;
	case 2:
		((CToomDrawStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
		((CToomDrawStep*)iStep)->CloseTMSGraphicsStep();
		TestComplete();
		break;
		}
	((CToomDrawStep*)iStep)->RecordTestResultL();
	}

__WS_CONSTRUCT_STEP__(oomDraw)
