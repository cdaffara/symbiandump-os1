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

#include "char_audiorec_common.h"


//
// Local Function(s)
//
/*****
 * GetDriveName()
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

RAudioRecTest::RAudioRecTest(const TDesC& aStepName, const TDesC& aSectName) :
	iSectName (aSectName),
	iRecorder(NULL)
	{
	iTestStepName = aStepName;
	iTimer = NULL;
	}
	
// start test
void RAudioRecTest::KickoffTestL()
	{
	// re-initialise data - orphan any pointer, as would relate to
	// previous run's heap
	iRecorder = NULL; 
	iTimer = NULL;
	TPtrC filename;
	TPtrC filename2;
	TPtrC filename3;
	User::LeaveIfError(iFs.Connect());
	//Getting filename from ini file
    if(!GetStringFromConfig(iSectName, _L("filename"), filename))
		{
		ERR_PRINTF1(_L("ERROR : Cannot Get value from Config."));
		}
	GetDriveName(iFileName);
	iFileName.Append(filename);
	
	if(!GetStringFromConfig(iSectName, _L("filename2"), filename2))
		{
		ERR_PRINTF1(_L("ERROR : Cannot Get value(filename2) from Config."));
		}	
	GetDriveName(iFileName2);
	iFileName2.Append(filename2);
	if(!GetStringFromConfig(iSectName, _L("filename3"), filename3))
		{
		ERR_PRINTF1(_L("ERROR : Cannot Get OGG Metadata File from Config."));
		}	
	GetDriveName(iFileName3);
	iFileName3.Append(filename3);
	
	//initialise instance of CMdaAudioPlayerUtility
	iRecorder = CMdaAudioRecorderUtility::NewL(*this);
	iTimer = CPeriodic::NewL(CActive::EPriorityHigh);
	iCallBack = TCallBack(TimerCallback, this);
	
	//The derived class should implement DoKickoffTestL()
	DoKickoffTestL();
	//ensure we start with no file. Ignore errors.
	iFs.Delete(iFileName);
	//Call the state machine to start test actions
	Fsm(EARStateIdle);
	}

void RAudioRecTest::SetExpectedEventAndError(TAudioRecorderState aExpectedState, TInt aExpectedError)
	{
  	iExpectedState = aExpectedState;
  	iExpectedError = aExpectedError;
	}

TBool RAudioRecTest::ValidateState()
	{
	if((iExpectedState == iState) && (iExpectedError == iError))
		{
		return ETrue;
		}
	else
		{
		// Print trace stating the mismatches
		TPtrC16 ptrExpState(KAudioRecorderState[iExpectedState]);
		TPtrC16 ptrState(KAudioRecorderState[iState]);
		INFO_PRINTF3(_L("iExpectedState = [%S] , iState = [%S]"), &ptrExpState, &ptrState);
		INFO_PRINTF3(_L("iExpectedError = [%d] , iError = [%d]"), iExpectedError, iError);
		return EFalse;
		}
	}

void RAudioRecTest::MoscoStateChangeEvent(CBase* aObject, TInt aPreviousState, TInt aCurrentState, TInt aErrorCode)
	{
	aObject = aObject; // shutup warning
	INFO_PRINTF4(_L("RAudioRecTest::MoscoStateChangeEvent(%d,%d,%d)"), aPreviousState, aCurrentState, aErrorCode);
	iError = aErrorCode;
	INFO_PRINTF2(_L("iError = %d"), iError);

	// DEF127335
	// OggRecordController does not support recording into an already existing file.
	// This is due to unavailability of APIs at the level of Ogg Vorbis C libraries.
	if (iExpectedState == EARStateRecording && iError == KErrNotSupported && (iTestStepName == _L("MM-MMF-ARCLNT-CHRTZ-OGG-I-0102") || iTestStepName == _L("MM-MMF-ARCLNT-CHRTZ-OGG-I-0103")))
		{
		INFO_PRINTF1(_L("Expected Result: OggRecordController returned KErrNotSupported(-5)"));
		StopTest(KErrNotSupported,EPass);
		return;
		}

	// only interested in some state transitions...
	if (aPreviousState==CMdaAudioRecorderUtility::ENotReady && 
	        aCurrentState==CMdaAudioRecorderUtility::ENotReady)
		{
		// have opened the file
		INFO_PRINTF1(_L("iRecorder->state is Before Opened "));
		iState = EARStateBeforeOpen;
		Fsm(EARStateBeforeOpen);
		}
	else if (aPreviousState==CMdaAudioRecorderUtility::ENotReady && 
	        aCurrentState==CMdaAudioRecorderUtility::EOpen)
		{
		// have opened the file
		INFO_PRINTF1(_L("iRecorder->state is EARStateOpened "));
		iState = EARStateOpened;
		Fsm(EARStateOpened);
		}
	else if (aPreviousState==CMdaAudioRecorderUtility::EOpen && 
	         aCurrentState==CMdaAudioRecorderUtility::ERecording)
		{
		INFO_PRINTF1(_L("iRecorder->state is EARStateRecording "));
		iState = EARStateRecording;
		Fsm(EARStateRecording);
		}
	else if (aPreviousState==CMdaAudioRecorderUtility::ERecording && 
	         aCurrentState==CMdaAudioRecorderUtility::ERecording)
		{
		INFO_PRINTF1(_L("iRecorder->state is EARStateRecordStopped "));
		iState = EARStateRecordInRecording;
		Fsm(EARStateRecordInRecording);
		}
    else if (aPreviousState==CMdaAudioRecorderUtility::ERecording && 
	         aCurrentState==CMdaAudioRecorderUtility::EOpen)
		{
		INFO_PRINTF1(_L("iRecorder->state is EARStateRecordStopped "));
		iState = EARStateRecordStopped;
		Fsm(EARStateRecordStopped);
		}
   else if(aPreviousState==CMdaAudioRecorderUtility::EOpen && 
	         aCurrentState==CMdaAudioRecorderUtility::EPlaying)
		{
		INFO_PRINTF1(_L("iRecorder->state is EARStatePlaying "));
		iState = EARStatePlaying;
		Fsm(EARStatePlaying);
		}
   else if(aPreviousState==CMdaAudioRecorderUtility::EPlaying && 
	         aCurrentState==CMdaAudioRecorderUtility::EPlaying)
		{
		INFO_PRINTF1(_L("iRecorder->state is EARStatePlaying while in playing "));
		iState = EARStatePlayWhilePlaying;
		Fsm(EARStatePlayWhilePlaying);
		}
   else if(aPreviousState==CMdaAudioRecorderUtility::EPlaying && 
	         aCurrentState==CMdaAudioRecorderUtility::EOpen)
		{
		INFO_PRINTF1(_L("iRecorder->state is EARStatePlayStopped "));
		iState = EARStatePlayStopped;
		Fsm(EARStatePlayStopped);
		}
	else if(aPreviousState==CMdaAudioRecorderUtility::EOpen && 
	         aCurrentState==CMdaAudioRecorderUtility::EOpen)
		{
		INFO_PRINTF1(_L("iRecorder->state has not changed"));
		iState = EARStateError;
		Fsm(EARStateError);
		}
	}	

// cleanup at end
void RAudioRecTest::CloseTest()
	{
	iRecorder->Close();
	delete iRecorder;
	iRecorder = NULL;
	
	delete iTimer;
	iTimer = NULL;
	
	iFs.Close();
	}

// timer callback
TInt RAudioRecTest::TimerCallback(TAny* aPtr)
	{
	static_cast<RAudioRecTest*>(aPtr)->DoTimerCallback();
	return KErrNone;
	}
	
void RAudioRecTest::DoTimerCallback()
	{
	// The derived class may provide implementation
	}
	
/**
 * Time comparison utility function
 *
 * @param	"const TUint aActual"
 *			The actual timer value produced
 * @param	"const TUint aExpected"
 *			Expected timer value
 * @param	"const TUint aDeviation"
 *			Allowed deviation of the expected value
 *			from the actual value.
 * @return	"TBool"
 *			Did actual timed value fall within deviation limits
 */ 
TBool RAudioRecTest::TimeComparison(const TUint aActual, const TUint aExpected, const TUint aDeviation)
	{
	// save unnessary conditions
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
	// comapare
	if(difference < aDeviation)
		{
		return ETrue;
		}
	return EFalse;
	}
