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

#include "subtitlegraphicteststep.h"
#include "subtitlecommonutils.h"

_LIT(KTestBitmap1, "c:\\mm\\mmf\\testfiles\\subtitlegraphic\\subtitletestdata.png");
_LIT(KTestBitmap2, "c:\\mm\\mmf\\testfiles\\subtitlegraphic\\subtitletestdata2.png");
_LIT(KTestBitmap3, "c:\\mm\\mmf\\testfiles\\subtitlegraphic\\subtitletestdata3.png");
         
const TInt KTestBitmap1XSize(200);
const TInt KTestBitmap1YSize(151);

const TInt KTestBitmap2XSize(193);
const TInt KTestBitmap2YSize(151);

const TInt KTestBitmap3XSize(32);
const TInt KTestBitmap3YSize(32);

const TInt KSubtitleRegionXPos(0);
const TInt KSubtitleRegionYPos(50);

// Enable to draw a crosshatch over the main window.
//#define DEBUG_MARK_WINDOW 1

RSubtitleGraphicTestStep::RSubtitleGraphicTestStep(const TDesC& aStepName) :
	iCRPId(TWsGraphicId::EUninitialized)
	{
	iTestStepName = aStepName;
	}

TVerdict RSubtitleGraphicTestStep::DoTestStepPreambleL()
	{
	User::LeaveIfError(iFs.Connect());
	
    // Install the Active Scheduler
    iActiveScheduler = new(ELeave) CActiveScheduler;
    CActiveScheduler::Install(iActiveScheduler);
	iActiveSchedulerStarted = EFalse;
	
	InitWservL();
	
	InitCrpL();

	User::LeaveIfError(RFbsSession::Connect());
	
    return EPass;
	}

TVerdict RSubtitleGraphicTestStep::DoTestStepPostambleL()
	{
	RFbsSession::Disconnect();
	
	iFs.Close(); 
	
    // Destroy Window server objects
    DeInitWserv();
    
    DestroyBitmap1();
	DestroyBitmap2();
	DestroyBitmap3();
	DestroyCrp();
	
	CActiveScheduler::Install(NULL);
	delete iActiveScheduler;
    iActiveScheduler = NULL;
    
    return EPass;
	}

void RSubtitleGraphicTestStep::InitWservL()
    {
    TInt err = iWs.Connect();
    
    if (err != KErrNone)
        {
        // Access violation if ws is null
        ERR_PRINTF1(_L("Cannot test, no window server available"));
        User::Leave(err);
        }

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
    TRgb backgroundColour = TRgb(0, 255, 255);
    backgroundColour.SetAlpha(0);
    iWindow->SetBackgroundColor(backgroundColour);
    
    iGc=new(ELeave) CWindowGc(iScreen);
	User::LeaveIfError(iGc->Construct());
	
    iWindow->Activate();
    iWs.Flush();
    }

void RSubtitleGraphicTestStep::DeInitWserv()
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
    
    iWs.Flush();
    iWs.Close();
    }

TVerdict RSubtitleGraphicTestStep::StartCrpDrawL()
	{	
	PrepGc();
	
	// Draw the CRP
	TPoint regionTl(KSubtitleRegionXPos,KSubtitleRegionYPos);
    TPoint regionBr(iScreen->SizeInPixels().iWidth, iScreen->SizeInPixels().iHeight);
    
    iGc->DrawWsGraphic(iCRPGraphic->Id(), TRect(regionTl, regionBr));
           
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
    
    iGc->DrawLine(TPoint(KSubtitleRegionXPos, height), TPoint(width, KSubtitleRegionYPos));
	
	RetireGc();

    return iTestStepResult;
	}

//
// CWsGraphicBase::PrepGc
// activate a gc & clear the two rects
//
void RSubtitleGraphicTestStep::PrepGc()
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
void RSubtitleGraphicTestStep::RetireGc()
	{
	iGc->Deactivate();
	iWindow->EndRedraw();
	iWs.Flush();
	}

void RSubtitleGraphicTestStep::InitCrpL()
	{
	iCRPGraphic = CMMFSubtitleGraphic::NewL();
	}

void RSubtitleGraphicTestStep::DestroyCrp()
	{
	delete iCRPGraphic;
	iCRPGraphic = NULL;
	}

TBool RSubtitleGraphicTestStep::CreateBitmap1L()
	{
	if ( iBitmap1 )
		{
		User::Invariant();
		}
		
	iBitmap1 = new (ELeave) CFbsBitmap();
	TSize testBitmap1Size(KTestBitmap1XSize, KTestBitmap1YSize);
	
	// Load test bitmap data
	TInt err = iBitmap1->Create(testBitmap1Size, EColor16MA);
	
	if ( err != KErrNone )
		{
		ERR_PRINTF2(_L("Failed on iBitmap1->Create(KTestBitmap1Size, EColor16MA) error code %d"), err);
		delete iBitmap1;
		iBitmap1=NULL;
		return EFalse;
		}
		
	TRAP(err, SubtitleCommonUtils::Png2BmpL(iFs, KTestBitmap1, *iBitmap1));
	
	if ( err != KErrNone )
		{
		ERR_PRINTF2(_L("Failed on SubtitleCommonUtils::Png2BmpL(iFs, KTestBitmap1, *iBitmap1) error code %d"), err);
		delete iBitmap1;
		iBitmap1=NULL;
		return EFalse;
		}
		
	return ETrue;
	}

TBool RSubtitleGraphicTestStep::CreateBitmap2L()
	{
	if ( iBitmap2 )
		{
		User::Invariant();
		}
	
	iBitmap2 = new (ELeave) CFbsBitmap();
	TSize testBitmap2Size(KTestBitmap2XSize, KTestBitmap2YSize);
	
	// Load test bitmap data
	TInt err = iBitmap2->Create(testBitmap2Size, EColor16MA);
	
	if ( err != KErrNone )
		{
		ERR_PRINTF2(_L("Failed on iBitmap2->Create(KTestBitmap2Size, EColor16MA) error code %d"), err);
		delete iBitmap2;
		iBitmap2=NULL;
		return EFalse;
		}
		
	TRAP(err, SubtitleCommonUtils::Png2BmpL(iFs, KTestBitmap2, *iBitmap2));
	
	if ( err != KErrNone )
		{
		ERR_PRINTF2(_L("Failed on SubtitleCommonUtils::Png2BmpL(iFs, KTestBitmap1, *iBitmap2) error code %d"), err);
		delete iBitmap2;
		iBitmap2=NULL;
		return EFalse;
		}
	
	return ETrue;
	}
	
TBool RSubtitleGraphicTestStep::CreateBitmap3L()
	{
	if ( iBitmap3 )
		{
		User::Invariant();
		}
	
	iBitmap3 = new (ELeave) CFbsBitmap();
	TSize testBitmap3Size(KTestBitmap3XSize, KTestBitmap3YSize);
	
	// Load test bitmap data
	TInt err = iBitmap3->Create(testBitmap3Size, EColor16MA);
	
	if ( err != KErrNone )
		{
		ERR_PRINTF2(_L("Failed on iBitmap3->Create(KTestBitmap3Size, EColor16MA) error code %d"), err);
		delete iBitmap3;
		iBitmap3=NULL;
		return EFalse;
		}
		
	TRAP(err, SubtitleCommonUtils::Png2BmpL(iFs, KTestBitmap3, *iBitmap3));
	
	if ( err != KErrNone )
		{
		ERR_PRINTF2(_L("Failed on SubtitleCommonUtils::Png2BmpL(iFs, KTestBitmap3, *iBitmap3) error code %d"), err);
		delete iBitmap3;
		iBitmap3=NULL;
		return EFalse;
		}
	
	return ETrue;
	}
	
void RSubtitleGraphicTestStep::DestroyBitmap1()
	{
	delete iBitmap1;
	iBitmap1 = NULL;
	}


void RSubtitleGraphicTestStep::DestroyBitmap2()
	{
	delete iBitmap2;
	iBitmap2 = NULL;
	}
	
void RSubtitleGraphicTestStep::DestroyBitmap3()
	{
	delete iBitmap3;
	iBitmap3 = NULL;
	}
