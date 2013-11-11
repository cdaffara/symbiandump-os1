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

#include "char_audioplay_common.h"

RTestBaseAudioPlay::RTestBaseAudioPlay(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName) :
	iUtility(NULL),
	iTimer(NULL),
	iState(EAPIdle),
	iExpState(EAPIdle),
	iExpError(KErrNone)
	{
	iTestStepName = aStepName;
	iSectName	  = aSectName;
	iKeyName	  = aKeyName;
	}

// Callback function overrides

void RTestBaseAudioPlay::MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& /*aDuration*/)
	{
	INFO_PRINTF2(_L("--MapcInitComplete() received with error: %d"), aError);
	ASSERT(iState==EAPOpening);
	
	if (aError != iExpError)
		{
		StopTest(aError, EFail);
		}
	
	iState = EAPOpenComplete;
	Fsm();
	}

void RTestBaseAudioPlay::MapcPlayComplete(TInt aError)
	{
	INFO_PRINTF2(_L("--MapcPlayComplete() received with error: %d"), aError);
	ASSERT((iState==EAPPlaying) || (iState==EAPPlayingAgain));
	
	if (aError != iExpError)
		{
		StopTest(aError, EFail);
		}
		
	iState = EAPPlayComplete;
	Fsm();
	}
	
void RTestBaseAudioPlay::MarncResourceAvailable(TUid /*aNotificationEventId*/, const TDesC8 &/*aNotificationData*/)
	{
	
	iState = EAPPResourceAvailable;
	Fsm();
	}

// Start test
void RTestBaseAudioPlay::KickoffTestL()
	{
	// Re-initialise data - orphan any pointer, as would relate to
	// previous run's heap
	iUtility = NULL;
	iTimer = NULL;
	iState = EAPIdle;
	
	// Initialise instance of CMdaAudioPlayerUtility
	iUtility = CMdaAudioPlayerUtility::NewL(*this);
	iTimer = CPeriodic::NewL(CActive::EPriorityHigh);
	iCallBack = TCallBack(TimerCallback, this);
	
	// The derived class should implement DoKickoffTestL()
	DoKickoffTestL();
	
	// Call the state machine to start test actions
	Fsm();
	}

// Function to set the expected state and error
void RTestBaseAudioPlay::SetExpectedState(TAudioPlayState aExpState, TInt aExpError)
	{
	iExpState = aExpState;
	iExpError = aExpError;
	}

// Function to set the data member iFileName
void RTestBaseAudioPlay::InitialiseFileName()
	{
	// Get the filename
	TPtrC tmpFileName;
	if(!GetStringFromConfig(iSectName, iKeyName, tmpFileName))
		{
		ERR_PRINTF1(_L("ERROR: Cannot Get value from Config."));
		StopTest(KErrNotFound, EInconclusive);
		}
		
#ifdef __WINS__
	iFileName = _L("c:");
#elif defined(__MARM__) || defined(__X86GCC__)
	#ifdef TEST_FILES_ON_C_DRIVE
		iFileName = _L("c:");
	#else
		iFileName = _L("z:");
	#endif // TEST_FILES_ON_C_DRIVE
#endif
	iFileName.Append(tmpFileName);
	}

// Function to compare duration with expected time with an allowed deviation
TBool RTestBaseAudioPlay::TimeComparison(const TUint aActual, const TUint aExpected, const TUint aDeviation)
	{
	// Save unnessary conditions
	if(aActual == aExpected)
		{
		return ETrue;
		}
		
	// Prevent unsigned wrapping errors 
	TUint difference;
	if(aActual > aExpected)
		{
		difference = aActual - aExpected;
		}
	else
		{
		difference = aExpected - aActual;
		}

	// Comapare difference with allowed deviation
	if(difference < aDeviation)
		{
		return ETrue;		
		}
	else
		{
		return EFalse;		
		}
	}
	
// Function to end test when the wrong state is encountered
void RTestBaseAudioPlay::WrongState(TInt aError)
	{
	// Print trace stating the mismatches
	TPtrC16 ptrExpState(KAudioPlayState[iExpState]);
	TPtrC16 ptrState(KAudioPlayState[iState]);
	ERR_PRINTF3(_L("iExpState = [%S] , iState = [%S]"), &ptrExpState, &ptrState);
	StopTest(aError, EFail);	
	}

// Cleanup at end
void RTestBaseAudioPlay::CloseTest()
	{
	delete iUtility;
	iUtility = NULL;
	delete iTimer;
	iTimer = NULL;
	}


// Timer callback
TInt RTestBaseAudioPlay::TimerCallback(TAny* aPtr)
	{
	static_cast<RTestBaseAudioPlay*>(aPtr)->DoTimerCallback();
	return KErrNone;
	}

void RTestBaseAudioPlay::DoTimerCallback()
	{
	// The derived class may provide implementation
	// this is to avoid link errors
	}
	
