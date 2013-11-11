// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "char_videoplay_common.h"


/* GetDriveName()
 * Method to get the drive name where the data files are present
 *****/
TAny GetDriveName(TDes& aFileName)
{
#ifdef __WINS__
    aFileName = _L("c:");    
#elif defined(__MARM__) || defined(__X86GCC__)
    #ifdef TEST_FILES_ON_C_DRIVE
        aFileName = _L("c:");
    #else
        aFileName = _L("z:");    
    #endif // TEST_FILES_ON_C_DRIVE
#endif    
}

RTestBaseVideoPlay::RTestBaseVideoPlay(const TDesC& aStepName, const TDesC& aSectName) :
	iUtility(NULL),
	iTimer(NULL),
	iState(EVPIdle),
	iSectName (aSectName)
	{
	iTestStepName = aStepName;
	iExpState = EVPIdle;
	iExpError = KErrNone;
	}

void RTestBaseVideoPlay::MvpuoOpenComplete(TInt aError)
	{
	ASSERT ((iState==EVPOpening) || (iState==EVPOpening1));
	iError = aError;
	INFO_PRINTF2(_L("-- iUtility->MVideoPlayerUtilityObserver OpenComplete = %d "), iError);
	if (iState==EVPOpening)
    	{
    	iState = EVPOpenComplete;
    	}
	else
	    {
	    iState = EVPOpenComplete1;
	    }
	Fsm();
	}
void RTestBaseVideoPlay::MvpuoPrepareComplete(TInt aError)
	{
	ASSERT((iState==EVPPreparing) || (iState==EVPPreparing1));
	iError = aError;
	INFO_PRINTF2(_L("-- iUtility->MVideoPlayerUtilityObserver PrepareComplete = %d "), iError);
	if (iState==EVPPreparing)
    	{
    	iState = EVPPrepareComplete;
    	}
	else
	    {
	    iState = EVPPrepareComplete1;
	    }
	Fsm();
	}

void RTestBaseVideoPlay::MvpuoPlayComplete(TInt aError)
	{
	ASSERT((iState==EVPPlaying) || (iState==EVPPlayResumed));
	iError = aError;
	#ifndef __EABI__	
	INFO_PRINTF2(_L("-- iUtility->MVideoPlayerUtilityObserver PlayComplete = %d "), iError);
	#endif
    iState = EVPPlayComplete;
	Fsm();
	}

void RTestBaseVideoPlay::MvpuoFrameReady(CFbsBitmap &/*aFrame*/, TInt  aError )
	{
	ASSERT((iState==EVPIdle) || (iState==EVPFrameRequested));
	iError = aError;
	INFO_PRINTF2(_L("-- iUtility->MVideoPlayerUtilityObserver FrameReady = %d "), iError);
	if ((iState == EVPFrameRequested) || (iState == EVPIdle))
    	{
    	 iState = EVPStopped;   
    	}
	Fsm();
	}
void RTestBaseVideoPlay::MvpuoEvent(const TMMFEvent &/*aEvent*/)
	{
	}

/**
 * RTestBaseVideoPlay::InitWservL()
 */
void RTestBaseVideoPlay::InitWservL()
    {
    TInt err = iWs.Connect();
    if (err != KErrNone)
        {
        // Access violation if ws is null
        INFO_PRINTF1(_L("Cannot test, no window server available"));
        User::Leave(err);
        }

    iScreenDevice = new (ELeave) CWsScreenDevice(iWs); // make device for this session
    User::LeaveIfError(iScreenDevice->Construct()); // and complete its construction

    RWindowGroup iRootWindow = RWindowGroup(iWs);
    User::LeaveIfError(iRootWindow.Construct((TUint32)this, ETrue));

    iWindow = new(ELeave) RWindow(iWs);
    User::LeaveIfError(((RWindow*)iWindow)->Construct(iRootWindow,((TUint32)(this)) + 1));
    iWindow->SetExtent(TPoint(0,0), TSize(100,100));
    iWindow->SetVisible(ETrue);
    iWindow->Activate();
    iWs.Flush();
    }

/**
 * RTestBaseVideoPlay::DeInitWserv
 */
void RTestBaseVideoPlay::DeInitWserv()
    {
    if (iWindow)
        {
        iWindow->Close();
        delete iWindow;
        iWindow = NULL;
        }
    iRootWindow.Close();
    delete iScreenDevice;
    iScreenDevice = NULL;
    iWs.Flush();
    iWs.Close();
    }

// start test
void RTestBaseVideoPlay::KickoffTestL()
	{
	// re-initialise data - orphan any pointer, as would relate to
	// previous run's heap
	iUtility = NULL;
	iTimer = NULL;
	iState = EVPIdle;
	
	TPtrC filename;
	//Getting filename from ini file
    if(!GetStringFromConfig(iSectName, _L("filename"), filename))
		{
		ERR_PRINTF1(_L("ERROR : Cannot Get value from Config."));
		}
	
	GetDriveName(iFilename);
	iFilename.Append(filename);
	
    InitWservL();

	TRect rect(TPoint(10,10), TPoint(150,150)), clipRect(TPoint(10,10), TPoint(150,150));
	
	// initialise instance of CVideoPlayerUtility
	iUtility = CVideoPlayerUtility::NewL(*this, EMdaPriorityNormal, EMdaPriorityPreferenceNone, iWs, *iScreenDevice, *iWindow, rect, clipRect);
	iTimer = CPeriodic::NewL(CActive::EPriorityHigh);
	iCallBack = TCallBack(TimerCallback, this);
	
	// the derived class should implement DoKickoffTestL()
	DoKickoffTestL();
	
	// Call the state machine to start test actions
	Fsm();
	}

// this is to set the expected state & error
void RTestBaseVideoPlay::SetExpectedState(TVideoPlayState aExpState, TInt aExpError)
	{
	iExpState = aExpState;
	iExpError = aExpError;
	}

// validate the state after the callbacks
TBool RTestBaseVideoPlay::ValidateState()
	{
	if((iExpState == iState) && (iExpError == iError))
		{
		return ETrue;
		}
	else
		{
		// Print trace stating the mismatches
		TPtrC16 ptrExpState(KVideoPlayState[iExpState]);
		TPtrC16 ptrState(KVideoPlayState[iState]);
		INFO_PRINTF3(_L("iExpState = [%S] , iState = [%S]"), &ptrExpState, &ptrState);
		INFO_PRINTF3(_L("iExpError = [%d] , iError = [%d]"), iExpError, iError);
		return EFalse;
		}
	}

// cleanup at end
void RTestBaseVideoPlay::CloseTest()
	{
    // delete other instances
	delete iUtility;
	iUtility = NULL;
	delete iTimer;
	iTimer = NULL;
	
	// Destroy Window server objects
    DeInitWserv();
	}


// timer callback
TInt RTestBaseVideoPlay::TimerCallback(TAny* aPtr)
	{
	static_cast<RTestBaseVideoPlay*>(aPtr)->DoTimerCallback();
	return KErrNone;
	}

void RTestBaseVideoPlay::DoTimerCallback()
	{
	// The derived class may provide implementation
	// this is to avoid link errors
	}
	
