// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "tsu_mmf_devsubtitle.h"

_LIT(KSrtSourceFilename, "c:\\mm\\mmf\\testfiles\\devsubtitle\\devsubtitle.srt");

const TInt KWindowClipRectTlX(50);
const TInt KWindowClipRectTlY(50);
const TInt KWindowClipRectBrX(200);
const TInt KWindowClipRectBrY(200);

/**
Normal test step
*/
RDevSubtitleTestStep::RDevSubtitleTestStep(const TDesC& aStepName)
	{
	iTestStepName = aStepName;
	}

TVerdict RDevSubtitleTestStep::DoTestStepPreambleL()
	{
	__MM_HEAP_MARK;
	
	// Install the Active Scheduler
	iActiveScheduler = new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(iActiveScheduler);
	iActiveSchedulerStarted = EFalse;	   	
	
	iCommonUtils = new (ELeave) CDevSubtitleTestStepCommonUtils();
	
	RFbsSession::Connect();
	
	iCommonUtils->DoTestStepPreambleL();
	
    return EPass;
	}

TVerdict RDevSubtitleTestStep::DoTestStepPostambleL()
	{
	iCommonUtils->DoTestStepPostambleL();
	delete iCommonUtils;
	
	RFbsSession::Disconnect();
	
	CActiveScheduler::Install(NULL);
	delete iActiveScheduler;
    iActiveScheduler = NULL;
    
    __MM_HEAP_MARKEND;
    
    return EPass;
	}
	
CMMFDevSubtitle* RDevSubtitleTestStep::DevSubtitle()
	{
	return iCommonUtils->iDevSubtitle;
	}
	
/**
Async test steps
*/
RDevSubtitleTestStepAsync::RDevSubtitleTestStepAsync(const TDesC& aStepName)
	{
	iTestStepName = aStepName;
	}
	
CMMFDevSubtitle* RDevSubtitleTestStepAsync::DevSubtitle()
	{
	return iCommonUtils->iDevSubtitle;
	}
	
	
void RDevSubtitleTestStepAsync::StopTest()
	{
	RAsyncTestStep::StopTest();
	}
	
void RDevSubtitleTestStepAsync::StopTest(TInt aError)
	{
	RAsyncTestStep::StopTest(aError);
	}
		
void RDevSubtitleTestStepAsync::Test(TBool aResult)
	{
	if ( !aResult )
		{
		// Debug messages?
		StopTest(-999);
		}
		
	TestBooleanTrue(aResult, __FILE8__, __LINE__) ;
	}
		
void RDevSubtitleTestStepAsync::KickoffTestL()
	{
	TInt alloced;
	User::Heap().AllocSize(alloced);
	
	__MM_HEAP_MARK;
	
	iCommonUtils = new (ELeave) CDevSubtitleTestStepCommonUtils();
	
	RFbsSession::Connect();
	
	iCommonUtils->DoTestStepPreambleL();
	}

void RDevSubtitleTestStepAsync::CloseTest()
	{
	RFbsSession::Disconnect();
	
	TRAPD(err, iCommonUtils->DoTestStepPostambleL());
	StopTest(err);
	
	TInt alloced;
	User::Heap().AllocSize(alloced);
	
	delete iCommonUtils;
    
	User::Heap().AllocSize(alloced);
	
    __MM_HEAP_MARKEND;
	}

/**
Common utilities
*/
void CDevSubtitleTestStepCommonUtils::InitWservL()
    {
    TInt err = iWs.Connect();
    
	User::LeaveIfError(err);

    iScreen = new (ELeave) CWsScreenDevice(iWs); // make device for this session
    User::LeaveIfError(iScreen->Construct()); // and complete its construction

    iRootWindow = RWindowGroup(iWs);
    User::LeaveIfError(iRootWindow.Construct((TUint32)this, ETrue));

    iWindow = new(ELeave) RWindow(iWs);
    User::LeaveIfError(((RWindow*)iWindow)->Construct(iRootWindow,((TUint32)(this)) + 1));
    iWindow->SetExtent(TPoint(0,0), iScreen->SizeInPixels());
    iWindow->SetVisible(ETrue);
    iWindow->SetRequiredDisplayMode(EColor16MA);

	// Setup transparency
    TInt errorCode = iWindow->SetTransparencyAlphaChannel();
    TRgb backgroundColour = TRgb(255, 255, 255);
    backgroundColour.SetAlpha(0);
    iWindow->SetBackgroundColor(backgroundColour);
    
    iGc=new(ELeave) CWindowGc(iScreen);
	User::LeaveIfError(iGc->Construct());
	
    iWindow->Activate();
    iWs.Flush();
    }

void CDevSubtitleTestStepCommonUtils::DeInitWserv()
    {
    if (iWindow)
        {
        iWindow->Close();
        delete iWindow;
        iWindow = NULL;
        }
    
    iRootWindow.Close();
    delete iScreen;
    iScreen = NULL;
    
    delete iGc;
    iGc = NULL;
    
    iWs.Close();   
	iFs.Close();
    }

//
// CWsGraphicBase::PrepGc
// activate a gc & clear the two rects
//
void CDevSubtitleTestStepCommonUtils::PrepGc()
	{
	iGc->Activate(*iWindow);
	iWindow->Invalidate();
	iWindow->BeginRedraw();
	iGc->Clear(TRect(iScreen->SizeInPixels()));
	iWs.Flush();
	}

//
// CWsGraphicBase::RetireGc
// deactivate a gc & flush any outstanding RWindow requests
void CDevSubtitleTestStepCommonUtils::RetireGc()
	{
	iGc->Deactivate();
	iWindow->EndRedraw();
	iWs.Flush();
	}

TVerdict CDevSubtitleTestStepCommonUtils::StartCrpDrawL(TInt aWindowId)
	{	
	PrepGc();
	
	// DevSubtitle needs to be constructed and a valid configuration added	
	TWsGraphicId crpId(TWsGraphicId::EUninitialized);
	TRect region;
	iDevSubtitle->GetCrpParametersL(aWindowId, crpId, region);
    iGc->DrawWsGraphic(crpId, region);

	TPoint regionTl(KWindowClipRectTlX,KWindowClipRectTlY);
    TPoint regionBr(KWindowClipRectBrX, KWindowClipRectBrY);
           
    TInt width=iScreen->SizeInPixels().iWidth;
    TInt height=iScreen->SizeInPixels().iHeight;

#ifdef DEBUG_MARK_WINDOW
    // Draw a square border
    iGc->SetPenColor(TRgb(255,0,0));
    iGc->DrawLine(TPoint(0,0),TPoint(0,height-1));
    iGc->DrawLine (TPoint (0, height-1), TPoint (width-1, height-1));
    iGc->DrawLine(TPoint(width-1,height-1),TPoint(width-1,0));
    iGc->DrawLine (TPoint (width-1, 0), TPoint (0, 0));
	
    // Draw a line between the corners of the window
    iGc->DrawLine(TPoint(0,0),TPoint(width, height));
    iGc->DrawLine (TPoint (0, height), TPoint (width, 0));
#endif
    
    // Draw a box around the display region
    iGc->SetPenColor(TRgb(255,0,0));
    iGc->DrawLine(regionTl, TPoint(width, height));
    
    iGc->DrawLine(TPoint(KWindowClipRectTlX, KWindowClipRectTlY), TPoint(KWindowClipRectBrX, KWindowClipRectBrY));
	
	RetireGc();
	
	return EPass;
	}

void CDevSubtitleTestStepCommonUtils::DoTestStepPreambleL()
	{
	iSubtitleSource = CSrtReader::NewL(KSrtSourceFilename);
	iDevSubtitle = CMMFDevSubtitle::NewL(*iSubtitleSource);
	InitWservL();
	}

void CDevSubtitleTestStepCommonUtils::DoTestStepPostambleL()
	{
	DeInitWserv();
	
	delete iDevSubtitle;
	iDevSubtitle = NULL;
	
	delete iSubtitleSource;
	iSubtitleSource = NULL;
	}

/**
Base class for ASync test steps
*/	
CDevSubtitleTestStepAoBase::CDevSubtitleTestStepAoBase(CMMFDevSubtitle* aDevSubtitle, RDevSubtitleTestStepAsync& aParent):
	CTimer(EPriorityStandard),
	iParent(aParent)
	{
	iDevSubtitle = aDevSubtitle;
	CActiveScheduler::Add(this);
	}
	
CDevSubtitleTestStepAoBase::~CDevSubtitleTestStepAoBase()
	{
	
	}
	
void CDevSubtitleTestStepAoBase::DoCancel()
	{
	
	}
