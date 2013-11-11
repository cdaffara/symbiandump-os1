// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalComponent
*/

#include <eikenv.h>
#include <s32file.h>
#include <f32file.h>
#include <iniparser.h>
#include "t_inidata.h"
#include "t_wservconsts.h"
#include "t_dsaappeng.h"
#include "t_perfdata.h"


_LIT(KDsaPanicTxt,                  "t_dsaapp.exe");
_LIT(KDsaRotationAbortCount,        "KDsaRotationAbortCount");

//
// class CTDsaAppEng
//
CTDsaAppEng::CTDsaAppEng(RWsSession& aClient, CWsScreenDevice& aScreenDevice, RWindow& aWindow)
: CTimer(CActive::EPriorityStandard),
	iClient(aClient),
	iScreenDevice(aScreenDevice),
	iWindow(aWindow),
    iDrawing(EFalse)
	{    
	}

CTDsaAppEng* CTDsaAppEng::NewL(RWsSession& aClient, CWsScreenDevice& aScreenDevice, 
                               RWindow& aWindow)
    {
    CTDsaAppEng* self = new (ELeave) CTDsaAppEng(aClient, aScreenDevice, aWindow);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(); // self;
    return self;
    }

CTDsaAppEng::~CTDsaAppEng()
	{
    if(IsActive())
    	{
	    Cancel();
    	}

	delete iDirectScreenAccess;	
	iDirectScreenAccess = NULL;
	delete iPerfData;
	iPerfData = NULL;
	iRegion->Close();
	iSemaphore.Close();
	}

void CTDsaAppEng::ConstructL()
	{
	CTimer::ConstructL();
	iDirectScreenAccess = CDirectScreenAccess::NewL(iClient, iScreenDevice, iWindow, *this);
	iPerfData = CTPerfData::NewL();
	READ_INT(KDsaAppIterations, KWServDsaAppConfigFile, iFinishTesting);
	User::LeaveIfError(iSemaphore.CreateGlobal(KWservDsaSemaphoreName(), 0));
	CActiveScheduler::Add(this);
	}

void CTDsaAppEng::StartDrawing()
	{
	if (iDrawing)
		{
		User::Panic(KDsaPanicTxt, DirScrAccEngAlreadyStarted);
		}
    
 	TRAPD(dsaErr, iDirectScreenAccess->StartL());
    if(dsaErr == KErrNone)
        {

	    // Get graphics context for it
	    iGc = iDirectScreenAccess->Gc();

	    // Get region that DSA can draw in
	    iRegion = iDirectScreenAccess->DrawingRegion();

	    // Set the display to clip to this region
	    iGc->SetClippingRegion(iRegion);

        iDrawing = ETrue;

        // request a timer event after a defined interval
		After(TTimeIntervalMicroSeconds32(0));
        }
	}

void CTDsaAppEng::StopDrawing()
	{
	if (!iDrawing)
		{
		User::Panic(KDsaPanicTxt, DirScrAccEngAlreadyStopped);
		}
	
	// Cancel timer and display
	Cancel();
	iDrawing = EFalse;
	}
	
// Implement MDirectScreenAccess
void CTDsaAppEng::Restart(RDirectScreenAccess::TTerminationReasons /*aReason*/)
	{
	// Restart display   
	TInt scrMode = iScreenDevice.CurrentScreenMode();
	TPixelsTwipsAndRotation sizeAndRotation;
	iScreenDevice.GetScreenModeSizeAndRotation(scrMode, sizeAndRotation);
	
	RDebug::Print(_L("CTDsaAppEng::Restart - Screen Mode: %d"), scrMode);
	RDebug::Print(_L("CTDsaAppEng::Restart - Previous Rotation: %d"), iPrevRotation);
	RDebug::Print(_L("CTDsaAppEng::Restart - Rotation: %d"), sizeAndRotation.iRotation);
	RDebug::Print(_L("CTDsaAppEng::Restart - Height: %d"), sizeAndRotation.iPixelSize.iHeight);
	RDebug::Print(_L("CTDsaAppEng::Restart - Width: %d"), sizeAndRotation.iPixelSize.iWidth);
	
	// since RDirectScreenAccess::ETerminateRegion takes precedence over 
	// RDirectScreenAccess::ETerminateRotation a check of the current screen
	// rotation against the previous rotation is performed to determine whether
	// the abort reason is due to a rotation event
	if (iPrevRotation!=sizeAndRotation.iRotation)
		{
		iRotationAbortCount++;
		iPrevRotation=sizeAndRotation.iRotation;
		}
		
	StartDrawing();    
	}

void CTDsaAppEng::AbortNow(RDirectScreenAccess::TTerminationReasons /*aReason*/)
	{
	// Cancel timer and display
	StopDrawing();
	}

void CTDsaAppEng::CreateTestFileL(const TDesC& aFileName)
	{
	RFs myFs;
	User::LeaveIfError(myFs.Connect());
	RFileWriteStream writer;
	writer.PushL(); // writer on cleanup stack
	User::LeaveIfError(writer.Replace(myFs, aFileName, EFileWrite));
	writer << _L("DSA Test");
	writer.CommitL();
	CleanupStack::PopAndDestroy(&writer);
	myFs.Close();
	}

void CTDsaAppEng::WriteResultsL(const TDesC& aFileName)
	{
	CIniData* myData=CIniData::NewL(aFileName);
	CleanupStack::PushL(myData);
	
	TBuf<255> tempStore;
	_LIT(KIntData, "%d");
	tempStore.Format(KIntData,iRotationAbortCount);
	TInt err2 = myData->AddValue(KDefaultSectionName, KDsaRotationAbortCount, tempStore);
	if (err2)
		{
		RDebug::Print(_L("CTDsaAppEng::WriteResultsL - unable to add abort count to result file: %d"), err2);
		}
	myData->WriteToFileL();
		
	CleanupStack::PopAndDestroy(myData);
	}

// Timer's RunL()
void CTDsaAppEng::RunL()
	{
	if (iFrameCount==0)
		{
		iPerfData->StartCounter();
		CreateTestFileL(KWServDsaAppStartFile());
		}
	else
		{
		iPerfData->StopCounterL();
		iPerfData->StartCounter();
		}
	
	if (iFrameCount == 100 || iFrameCount == 200)
		{
		iSemaphore.Wait();
		}
		
	if (iFrameCount==iFinishTesting)
		{
		iDirectScreenAccess->Cancel();
		iPerfData->WriteResultsL(KWServDsaAppResultFile());
		TRAP_IGNORE(WriteResultsL(KWServDsaAppResultFile()));
		TRAPD(err,CreateTestFileL(KWServDsaAppFinishFile()));
		if (err)
			{
			User::Panic(KDsaPanicTxt, err);
			}
		}
	else
		{
		iFrameCount++;

	    iDirectScreenAccess->ScreenDevice()->Update();
	    iGc->Clear();
		TRgb color(0,0,255);
		
		if (iFrameCount%2)
			{
			color.SetRed(0);
			color.SetBlue(0);
			color.SetGreen(255);
			}
	    
	    iGc->SetBrushColor(color);
	    iGc->SetPenColor(color);
	    iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
		TRect myRect(iWindow.Size());
		iGc->DrawRect(myRect);

		// Renew request
		After(TTimeIntervalMicroSeconds32(0));
		}
	}

// Timer's DoCancel()
void CTDsaAppEng::DoCancel()
	{
	// Cancel timer
	CTimer::DoCancel();
	
	// Cancel DSA
	iDirectScreenAccess->Cancel();    
	}


