// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/
#include "TTransptAnim.h"

struct FrameSet
	{
	TRgb* iFrames;
	const TInt iFrameCount;
	FrameSet(TRgb* aFrames, const TInt aFrameCount)
	: iFrames(aFrames),
	iFrameCount(aFrameCount)
		{
		}
	};

TRgb FrameSequenceAlpha[]= 
{ TRgb(255, 0, 0, 50), TRgb(0, 255, 0, 70), TRgb(0, 0, 255, 100), TRgb(255, 255, 0, 150), TRgb(0, 255, 255, 200) };
TRgb FrameSequenceNoAlpha[] =
{ TRgb(255, 0, 0), TRgb(0, 255, 0), TRgb(0, 0, 255), TRgb(255, 255, 0), TRgb(0, 255, 255) };

FrameSet frameSetAlpha(FrameSequenceAlpha, 	sizeof(FrameSequenceAlpha)/sizeof(TRgb));
FrameSet frameSetNoAlpha(FrameSequenceNoAlpha, sizeof(FrameSequenceNoAlpha)/sizeof(TRgb));
FrameSet TestFrameSets[] =
	{
	frameSetAlpha, frameSetNoAlpha
	};

static const TInt 	FRAME_DELAY 			= 2000000;
static const TInt 	DRAW_LINE_ANIMATION 	= 36;
static const TBool 	ALPHATRANSPARENCY_ON 	= ETrue;
static const TInt	ALPHA_FRAMESET 			= 0;

//*****************************************************************************
CTTestCase* CTTransparentAnim::CreateTestCaseL(CTTransparentAnim* aTransAnim, TInt aTestCaseId)
	{
	CTTestCase* testCase = aTransAnim->GetCurrent();
	
	if (!testCase)
		{
		switch (aTestCaseId)
			{
		case GraphicsWServ0483 :
			testCase =  CTGraphicsWServ0483::NewL();
			break;
			}
		}
	
	return testCase;
	}

//*****************************************************************************
CTTransparentAnim::CTTransparentAnim(CTestStep* aStep)
: CTWsGraphicsBase(aStep), iCurrentTestCase(NULL)
	{}

CTTransparentAnim::~CTTransparentAnim()
	{
	TheClient->iWs.SetAutoFlush(EFalse);
	}

void CTTransparentAnim::ConstructL()
	{
	TheClient->iWs.SetAutoFlush(ETrue);
	}

void CTTransparentAnim::RunTestCaseL(TInt)
	{
	((CTTransparentAnimStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0483"));
	CTTestCase* testCase = CTTransparentAnim::CreateTestCaseL(this, iTest->iState);
	
	if (!testCase)
		{
		TestComplete();
		return ;
		}

	TInt err = KErrNone;
	
	if (!testCase->IsAlreadyRunning())
		{
		SetCurrent(testCase);
		iTest->LogSubTest(testCase->TestCaseName());
		TRAP(err, testCase->StartL());
		testCase->SetError(err);
		}
	
	if (testCase->IsComplete() || err != KErrNone)
		{
		((CTTransparentAnimStep*)iStep)->RecordTestResultL();
		((CTTransparentAnimStep*)iStep)->CloseTMSGraphicsStep();
		if (testCase->HasFailed())
			{
			INFO_PRINTF3(_L("ErrorStatus  - Expected: %d, Actual: %d"), ETrue, EFalse);
			}
		++iTest->iState;
		SetCurrent(NULL);
		delete testCase;
		}
	else
		{
		// Prevent test harness from repeatedly running the test case too quickly.
		User::After(TTimeIntervalMicroSeconds32(FRAME_DELAY));
		}
	}

void CTTransparentAnim::SetCurrent(CTTestCase* aTestCase)
	{
	iCurrentTestCase = aTestCase;
	}

CTTestCase* CTTransparentAnim::GetCurrent()
	{
	return iCurrentTestCase;
	}

//*****************************************************************************
CTTAnimation::CTTAnimation(CTWin* aWin, TInt aDrawArg)
: iWin(aWin), iFrameCount(0), iCurrentFrameSet(0), iIsComplete(EFalse), iDrawArg(aDrawArg)
	{
	iAnimDll = RAnimDll(TheClient->iWs);
	iAnimDll.Load(KAnimDLLName);
	iDrawAnim = RTestAnim(iAnimDll);
	iDrawAnim.Construct(*iWin->BaseWin(), EAnimTypeTrans, TPckgBuf<TRect>(aWin->Size()));
	}

CTTAnimation::~CTTAnimation()
	{
	iDrawAnim.Close();
	iAnimDll.Close();
	delete iWin;
	}

void CTTAnimation::StartL()
	{
	iAnimTimer.ConstructL();
	iAnimTimer.Start(FRAME_DELAY, TCallBack(CTTAnimation::NextFrame,this));
	}

TBool CTTAnimation::IsComplete()
	{
	return iIsComplete;
	}

void CTTAnimation::SetCurrentFrameSet(TInt aFrameSet)
	{
	iCurrentFrameSet = aFrameSet;
	}

CTWin* CTTAnimation::GetWindow()
	{
	return iWin;
	}

void CTTAnimation::Attach(MAnimationObserver* aObserver)
	{
	iObserver = aObserver;
	}

TInt CTTAnimation::NextFrame(TAny* aAnim)
	{
	STATIC_CAST(CTTAnimation*,aAnim)->NextFrame();

	return KErrNone;
	}

void CTTAnimation::NextFrame()
	{
	TRgb Color;
	TPckgBuf<TFrameData> paramsPckg;

	if (iFrameCount == TestFrameSets[iCurrentFrameSet].iFrameCount)
		{
		iIsComplete = ETrue;
		iObserver->Update(this);
		return ;
		}
	
	Color = TestFrameSets[iCurrentFrameSet].iFrames[iFrameCount++];
	
	paramsPckg().draw = iDrawArg;
	paramsPckg().color = Color;
	
	iDrawAnim.Command(EADllNextFrame, paramsPckg);

	iAnimTimer.Start(FRAME_DELAY, TCallBack(CTTAnimation::NextFrame,this));
	}

//*****************************************************************************
CTransAnimTimer::CTransAnimTimer() 
: CTimer(EPriorityStandard)
	{}

CTransAnimTimer::~CTransAnimTimer()
	{}

void CTransAnimTimer::ConstructL()
	{
	CTimer::ConstructL();
	CActiveScheduler::Add(this);
	}

void CTransAnimTimer::RunL()
	{
	iCallBack.CallBack();
	}

void CTransAnimTimer::Start(TTimeIntervalMicroSeconds32 aInterval,TCallBack aCallBack)
	{
	iCallBack = aCallBack;
	After(aInterval);
	}

//*****************************************************************************
CTTestCase::CTTestCase()
: iError(KErrNone), iIsComplete(EFalse), iIsRunning(EFalse)
	{}

TBool CTTestCase::IsAlreadyRunning()
	{
	return iIsRunning;
	}

void CTTestCase::Update(CTTAnimation* aAnim)
	{
	iIsComplete = aAnim->IsComplete();
	}

TBool CTTestCase::IsComplete()
	{
	return iIsComplete;
	}

void CTTestCase::StartL()
	{
	if (!iIsRunning)
		{
		RunTestCaseL();
		iIsRunning = ETrue;
		}
	}

TBool CTTestCase::HasFailed()
	{
	TBool ret = EFalse;
	if (iError != KErrNone)
		{
		ret = ETrue;
		}
	return ret;
	}

void CTTestCase::SetIsCompleted(TBool aIsComplete)
	{
	iIsComplete = aIsComplete;
	}

void CTTestCase::SetError(TInt aError)
	{
	iError = aError;
	}

//*********************** New Test Case Classes go here************************

/**
 * @SYMTestCaseID GRAPHICS-WSERV-0483
 *
 * @SYMTestCaseDesc There are two animations running, one attached to a transparent
 * window using SetTransparencyAlphaChannel and a second animation with a non-transparent
 * window. The animations draw a line and then change the color of the line in subsequent
 * frames. After the last frame the alpha channel for the first animation is set to
 * opaque and then compared with the second animation to make certain that no artifacts
 * are present.
 *
 * @SYMTestPriority High
 *
 * @SYMTestStatus Implemented
 *
 * @SYMTestActions An anim is attached with a Transparent window.
 * 	The anim draws several frames using WSERV's Anim animation functionality.
 *
 * @SYMTestExpectedResults Both animations draw a line and then change it's color, the first
 * animation doing it in a tranparent window and the second doing it in a non-transparent window.
 **/
CTGraphicsWServ0483* CTGraphicsWServ0483::NewL()
	{
	CTGraphicsWServ0483* testCase = new(ELeave) CTGraphicsWServ0483();
	
	CleanupStack::PushL(testCase);
	testCase->ConstructL();
	CleanupStack::Pop();
	
	return testCase;
	}

CTGraphicsWServ0483::~CTGraphicsWServ0483()
	{
	delete iAnim;
	delete iCheckAnim;
	}

void CTGraphicsWServ0483::ConstructL()
	{
	iAnim = new(ELeave) CTTAnimation(CTAnimWin<ALPHATRANSPARENCY_ON>::NewL(
										BaseWin->GetBorderWin()->Position(),
										BaseWin->GetBorderWin()->Size()), DRAW_LINE_ANIMATION);
	
	iCheckAnim = new(ELeave) CTTAnimation(CTAnimWin<>::NewL(
										TestWin->GetBorderWin()->Position(),
										TestWin->GetBorderWin()->Size()), DRAW_LINE_ANIMATION);
	TheClient->WaitForRedrawsToFinish();
	}

void CTGraphicsWServ0483::RunTestCaseL()
	{
	iAnim->Attach(this);
	iCheckAnim->Attach(this);
	
	iAnim->SetCurrentFrameSet(ALPHA_FRAMESET);
	iAnim->StartL();
	
	iCheckAnim->SetCurrentFrameSet(ALPHA_FRAMESET);
	iCheckAnim->StartL();	
	}

void CTGraphicsWServ0483::Update(CTTAnimation*)
	{
	if (iAnim->IsComplete() && iCheckAnim->IsComplete())
		{
		TRgb bgColor(150, 150, 150, 255); 
		iAnim->GetWindow()->Win()->SetBackgroundColor(bgColor);
		iAnim->GetWindow()->Win()->Invalidate();
		TheClient->iWs.Finish();
		TheClient->WaitForRedrawsToFinish();
		if (!DoCheckRect(iAnim->GetWindow(), iCheckAnim->GetWindow()))
			{
			SetError(KErrGeneral);
			}
		SetIsCompleted(ETrue);
		}
	}

TPtrC CTGraphicsWServ0483::TestCaseName()
	{
	_LIT(KTransparentAlphaChannelAnimationTest, "Transparent Anim Test");
	
	return TPtrC(KTransparentAlphaChannelAnimationTest);
	}

__WS_CONSTRUCT_STEP__(TransparentAnim)
