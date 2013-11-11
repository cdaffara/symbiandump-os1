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

#include <s32file.h>
#include <hal.h>
#include <libc/limits.h> //UINT_MAX

#include "t_pseudoappeng.h"
#include "t_pseudoapputils.h"
#include "t_wservconsts.h"
#include "t_winutils.h"
#include <graphics/suerror.h>
#include "t_pseudoappshared.h"

//
// class CTPseudoAppEng
//
CTPseudoAppEng::CTPseudoAppEng(RWsSession& aClient, CWsScreenDevice& aScreenDevice, RWindow& aWindow)
: CTimer(CActive::EPriorityStandard),
	iClient(aClient),
	iWindow(aWindow),
	iScreenDevice(aScreenDevice),
	iDrawing(EFalse)
	{
	}

CTPseudoAppEng* CTPseudoAppEng::NewL(RWsSession& aClient, CWsScreenDevice& aScreenDevice, RWindow& aWindow)
    {
    CTPseudoAppEng* self = new (ELeave) CTPseudoAppEng(aClient, aScreenDevice, aWindow);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(); // self;
    return self;
    }

CTPseudoAppEng::~CTPseudoAppEng()
	{
	iSurfaceUpdateSession.Close();

    if(IsActive())
    	{
	    Cancel();
    	}

	delete iPseudoAppShared;
	delete iUtils;
	}

void CTPseudoAppEng::ConstructL()
	{
	CTimer::ConstructL();

	//Create CIniData class for reading in values form ini files
#ifndef T_PSEUDOAPP1
	iUtils = CIniData::NewL(KWServPseudoAppConfigFile);
#else
	iUtils = CIniData::NewL(KWServPseudoApp1ConfigFile);
#endif //T_PSEUDOAPP1

	//Connect to the surface update server
	TInt err = iSurfaceUpdateSession.Connect();
	if (err!=KErrNone)
		{
		RDebug::Print(_L("error in connecting surface update session\n"));
	 	User::Exit(0);
		}

	User::LeaveIfError(HAL::Get(HALData::EFastCounterFrequency, iCounterFreq));

	//Enter the animation loop
	RDebug::Print(_L("Enter Animation Loop\n"));

	CActiveScheduler::Add(this);
	}

void CTPseudoAppEng::StartDrawingL()
	{
	/* Read in the configuration fron the config file *
	 * and check the validity of the parameters. If   *
	 * invalid, take remedial action                  *
	 * Exit if the config is really unusable          *
	 * Window and animation classes are created if    *
	 * the configs are ok                             */
	RDebug::Print(_L("Import configuration for t_pseudoapp class and create test screen(s)\n"));
	ImportPseudoAppConfigL();
	RDebug::Print(_L("Test screen(s) created for t_pseudoapp\n"));

	if (iDrawing)
		{
		User::Panic(KTPseudoAppPanicTxt(), TPseudoAppEngAlreadyStarted);
		}

	iDrawing = ETrue;

	//A value of 0 will provoke a E32User-Cbase 46 panic
	After(TTimeIntervalMicroSeconds32(100000));

	//Set iStartTime to current time
	iStartTime.UniversalTime();
	}

void CTPseudoAppEng::StopDrawing()
	{
	if (!iDrawing)
		{
		User::Panic(KTPseudoAppPanicTxt(), TPseudoAppEngAlreadyStopped);
		}

	// Cancel timer and display
	Cancel();
	iDrawing = EFalse;
	}

void CTPseudoAppEng::TestFinishedL(const TDesC& aFileName)
	{
	RFs myFs;
	User::LeaveIfError(myFs.Connect());
	RFileWriteStream writer;
	writer.PushL(); // writer on cleanup stack
	User::LeaveIfError(writer.Replace(myFs, aFileName, EFileWrite));
	writer << _L("Surfaces Test Finished");
	writer.CommitL();
	CleanupStack::PopAndDestroy(&writer);
	myFs.Close();
	}

void CTPseudoAppEng::WriteResultsL(const TDesC& aFileName)
	{
	RFs myFs;
	User::LeaveIfError(myFs.Connect());

	TInt err = myFs.MkDirAll(aFileName);

	if (err==KErrNone || err==KErrAlreadyExists)
		{
		RFileWriteStream writer;
		writer.PushL(); // writer on cleanup stack

		User::LeaveIfError(writer.Replace(myFs, aFileName, EFileStreamText|EFileWrite));
		writer.CommitL();
		CleanupStack::PopAndDestroy(&writer); // writer

		CIniData* myData = CIniData::NewL(aFileName);
		CleanupStack::PushL(myData);

		TBuf<255> tempStore;
		_LIT(KIntData, "%d");
		_LIT(KRealData, "%4.1f");

		//Write out the test result, which is essentially a pass if the code reaches here
		tempStore.Format(KIntData, 1);
		TInt err = myData->AddValue(KDefaultSectionName, KGraphicsWservSurfaceTestResult, tempStore);
		User::LeaveIfError(err);
		myData->WriteToFileL();

		//Write out the theoretical maximum frame rate
		TReal frameRate = (iCounterFreq*static_cast<TReal>(iFrameDuration))/iGceTestResults.iTotalCompoTime;

		tempStore.Format(KRealData, frameRate);
		err = myData->AddValue(KDefaultSectionName, KGraphicsWservSurfaceFrameRate, tempStore);
		myData->WriteToFileL();

		//Write out the total test time
		TReal compoTime = static_cast<TReal>(iGceTestResults.iElapsedTime)/1000;

		tempStore.Format(KRealData, compoTime);
		err = myData->AddValue(KDefaultSectionName, KGraphicsWservSurfaceTotalTestTime, tempStore);
		myData->WriteToFileL();

		CleanupStack::PopAndDestroy(myData);
		}

	myFs.Close();
	}

// Timer's RunL()
void CTPseudoAppEng::RunL()
	{
	if(iFrameCounter != iFrameDuration)
		{
		//If no surface update is required, aSurfaceId will remain as 0
		TSurfaceId temp;
		iSurfDetails.aSurfaceId = temp.CreateNullId() ;

		while(iScreenCounter < iPseudoAppShared->iTestScreens.Count())
			{
			iPseudoAppShared->iTestScreens[iScreenCounter]->UpdateL(iSurfDetails);
			iScreenCounter++;
			break;
			}

		if(!iSurfDetails.aSurfaceId.IsNull())
			{
			if(iUseGlobalUpdate)
				{
				SendUpdateRequest(KAllScreens);				
				}
			else
				{
				SendUpdateRequest(iScreenCounter - 1);
				}
			}

		if(iScreenCounter == iPseudoAppShared->iTestScreens.Count())
			{
			iScreenCounter = 0;
			iFrameCounter++;
			}

		// Renew request
		After(TTimeIntervalMicroSeconds32(iFrameDelay));
		}
	else
		{
#ifndef T_PSEUDOAPP1
		WriteResultsL(KWServPseudoAppResultFile);
		TestFinishedL(KWServPseudoAppFinishFile);
#else
		WriteResultsL(KWServPseudoApp1ResultFile);
		TestFinishedL(KWServPseudoApp1FinishFile);
#endif //T_PSEUDOAPP1
		}
	}

// Timer's DoCancel()
void CTPseudoAppEng::DoCancel()
	{
	// Cancel timer
	CTimer::DoCancel();
	}

TBool  CTPseudoAppEng::Drawing()
	{
	return iDrawing;
	}

void CTPseudoAppEng::SendUpdateRequest(TInt /*aScreen*/)
 	{
 	//Submit update
	TRect rc[1] =
			{
			TRect(0, 0, iSurfDetails.aSurfaceSize.iWidth, iSurfDetails.aSurfaceSize.iHeight)
			};

	RRegion region(1, rc);

	//Use timestamp notification to measure frame rate
	TRequestStatus status;

	TTimeStamp timeStamp;
	iSurfaceUpdateSession.NotifyWhenDisplayed(status, timeStamp);
	iTimeStampBefore = User::FastCounter();

	TInt ret = iSurfaceUpdateSession.SubmitUpdate(KAllScreens, iSurfDetails.aSurfaceId, iSurfDetails.aBufferNumber, &region);

	User::WaitForRequest(status);

	TUint64 time = timeStamp();

	if(time < iTimeStampBefore)
		{
		time += UINT_MAX;
		}

	iTotalCompositionTime += (time - iTimeStampBefore);

	TTime currentTime;
	currentTime.UniversalTime();
	TTimeIntervalMicroSeconds elapsedTime = currentTime.MicroSecondsFrom(iStartTime);

	iGceTestResults.iElapsedTime = (TUint32)(elapsedTime.Int64()/1000);
	iGceTestResults.iTotalCompoTime = (TUint32)(iTotalCompositionTime);

	if(ret != KErrNone)
		{
		RDebug::Print(_L("Error submitting update request\n"));
		}
 	}

void CTPseudoAppEng::ImportPseudoAppConfigL()
	{
	TDisplayMode mode;
	TSize aScreenSize;
	TInt numberOfScreens;

	READ_INI1(number_of_screens, numberOfScreens, iUtils);

	iPseudoAppShared = new(ELeave) CTPseudoAppShared;
	for(TInt i=0; i<numberOfScreens; i++)
		{
		RDebug::Print(_L("Set up screen %d\n"), i);
		TImportPseudoAppConfig::ImportPseudoAppConfigL(i, iFrameDuration, iFrameDelay, mode, aScreenSize, iUtils);
#ifndef T_PSEUDOAPP1
		iPseudoAppShared->AddTestScreenL(i, mode, iFrameDuration, aScreenSize, &iGceTestResults, KWServPseudoAppConfigFile);
#else
		iPseudoAppShared->AddTestScreenL(i, mode, iFrameDuration, aScreenSize, &iGceTestResults, KWServPseudoApp1ConfigFile);
#endif //T_PSEUDOAPP1
		}
	READ_INI2A(KNullDesC, use_global_surface_update, True, False, iUseGlobalUpdate, ETrue, EFalse, iUtils);
	}

void CTPseudoAppEng::RotateL()
	{
	if (iPseudoAppShared)
		{
		for(TInt i=0; i<iPseudoAppShared->iTestScreens.Count(); i++)
			{
			if (iPseudoAppShared->iTestScreens[i])
				{
				if(iPseudoAppShared->iTestScreens[i]->Rotation())
					{
					iPseudoAppShared->iTestScreens[i]->RotateL(i, iFrameCounter);
					}
				}
			}
		}
	}
