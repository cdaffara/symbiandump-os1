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

#include <iniparser.h>
#include "t_app1eng.h"
#include "t_inidata.h"
#include "t_wservconsts.h"


_LIT(KTApp1ScrMode,  "KTApp1ScrMode%d");
_LIT(KTApp1Rotation, "KTApp1Rotation%d");
_LIT(KTApp1PanicTxt,   "t_app1.exe");

//
// class CTApp1Eng
//
CTApp1Eng::CTApp1Eng(RWsSession& aClient, CWsScreenDevice& aScreenDevice, RWindow& aWindow)
: CTimer(CActive::EPriorityLow),
	iClient(aClient),
	iWindow(aWindow),
	iScreenDevice(aScreenDevice),
    iDrawing(EFalse),
    iRotationFlag(EFalse)
	{
	}

CTApp1Eng* CTApp1Eng::NewL(RWsSession& aClient, CWsScreenDevice& aScreenDevice, RWindow& aWindow)
    {
    CTApp1Eng* self = new (ELeave) CTApp1Eng(aClient, aScreenDevice, aWindow);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(); // self;
    return self;
    }

CTApp1Eng::~CTApp1Eng()
	{
    if(IsActive())
    	{
	    Cancel();
    	}

    iRotationList.Close();
    iScrModeList.Close();
    iSemaphore.Close();
	}

void CTApp1Eng::ConstructL()
	{
	CTimer::ConstructL();
	User::LeaveIfError(iScreenDevice.CreateContext(iGc));

	HBufC* rotFlag = NULL;
	READ_STR(KTApp1RotationFlag, KWServTApp1ConfigFile, rotFlag);

	// check to see if instead of the default behaviour of drawing to
	// window with alternate frames of red/blue, that the app is to
	// periodically cause the screen to rotate, which is all defined
	// in the configuration file created by the test step
	if (rotFlag!=NULL && rotFlag->Des().Compare(_L("ETrue")) == KErrNone)
		{
		delete rotFlag;
		iRotationFlag=ETrue;

		// setup rotation and screen mode lists
		TBool moreData=ETrue;
		TInt index=0;
		TBuf<255> tempStore;
		CIniData * iniData = CIniData::NewL(KWServTApp1ConfigFile);
		CleanupStack::PushL(iniData);

		// read in rotations to be performed
		while (moreData)
			{
			TInt scrMode;
			tempStore.Format(KTApp1ScrMode, ++index);
			moreData = iniData->FindVar(KDefaultSectionName, tempStore, scrMode);
			if (moreData)
				{
				TInt rotation;
				tempStore.Format(KTApp1Rotation, index);
				moreData = iniData->FindVar(KDefaultSectionName, tempStore, rotation);
				if (moreData)
					{
					RDebug::Print(_L("CTApp1Eng::ConstructL - Screen Mode: %d, Rotation: %d"),scrMode,rotation);
					iRotationList.AppendL(rotation);
					iScrModeList.AppendL(scrMode);
					}
				else
					{
					User::Panic(KTApp1PanicTxt(), KErrNotFound);
					}
				}
			else
				{
				moreData=EFalse;
				}
			}

		CleanupStack::PopAndDestroy(iniData);

		// setup the number of frames to be counted between each
		// screen rotation
		TInt iters = 0;
		READ_INT(KTApp1Iterations, KWServTApp1ConfigFile, iters);
		iRotationTimer = iters/(iRotationList.Count()+1);
		RDebug::Print(_L("CTApp1Eng::ConstructL - Rotation Timer: %d"), iRotationTimer);

		if (iRotationList.Count()==0)
			{
			User::Panic(KTApp1PanicTxt(), KErrNotFound);
			}
		}
	User::LeaveIfError(iSemaphore.OpenGlobal(KWservDsaSemaphoreName));

	CActiveScheduler::Add(this);
	}

void CTApp1Eng::StartDrawing()
	{
	if (iDrawing)
		{
		User::Panic(KTApp1PanicTxt(), TApp1EngAlreadyStarted);
		}

	iDrawing=ETrue;

	After(TTimeIntervalMicroSeconds32(0));
	}

void CTApp1Eng::StopDrawing()
	{
	if (!iDrawing)
		{
		User::Panic(KTApp1PanicTxt(), TApp1EngAlreadyStopped);
		}

	// Cancel timer and display
	Cancel();
	iDrawing = EFalse;
	}


// Timer's RunL()
void CTApp1Eng::RunL()
	{
	iFrameCount++;

	if (iFrameCount==1)
		{
		RDebug::Print(_L("CTApp1Eng::RunL - Rotation Flag: %d"), iRotationFlag);
		}

	// just draw to window, with alternate frames of red and blue
	if (iRotationFlag==EFalse)
		{
		iGc->Activate(iWindow);
		TRect myRect(iWindow.Size());
		TRgb color(0,0,255);

		if (iFrameCount%2)
			{
			color.SetRed(255);
			color.SetBlue(0);
			}

	    iGc->SetBrushColor(color);
	    iWindow.SetBackgroundColor(color);
	    iGc->SetPenColor(color);
	    iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
		iGc->DrawRect(myRect);
		iGc->Deactivate();

		iDrawing=EFalse;
		iWindow.Invalidate();
		}
	// else rotate screen
	else
		{
		if (iFrameCount==iRotationTimer)
			{
			iSemaphore.Signal();
			iFrameCount=0;

			if (iRotationCount<iRotationList.Count())
				{
				TInt rotation = iRotationList[iRotationCount];
				TInt scrMode = iScrModeList[iRotationCount];

				switch (rotation)
					{
					case 0:
						{
						iScreenDevice.SetCurrentRotations(scrMode,CFbsBitGc::EGraphicsOrientationNormal);
						break;
						}
					case 90:
						{
						iScreenDevice.SetCurrentRotations(scrMode,CFbsBitGc::EGraphicsOrientationRotated90);
						iScreenDevice.SetScreenMode(scrMode);
						break;
						}
					case 180:
						{
						iScreenDevice.SetCurrentRotations(scrMode,CFbsBitGc::EGraphicsOrientationRotated180);
						break;
						}
					case 270:
						{
						iScreenDevice.SetCurrentRotations(scrMode,CFbsBitGc::EGraphicsOrientationRotated270);
						break;
						}
					default:
						{
						RDebug::Print(_L("CTApp1Eng::RunL - Invalid Rotation: %d"),rotation);
						User::Leave(KErrAbort);
						break;
						}
					}
				RDebug::Print(_L("CTApp1Eng::RunL - Screen Mode: %d, Rotation: %d"),scrMode, rotation);
				iScreenDevice.SetScreenMode(scrMode);
				}

			iRotationCount++;
			}

		After(TTimeIntervalMicroSeconds32(0));
		}
	}

// Timer's DoCancel()
void CTApp1Eng::DoCancel()
	{
	// Cancel timer
	CTimer::DoCancel();
	}


