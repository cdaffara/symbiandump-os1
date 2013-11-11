// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// TestAudioRecOgg.cpp
// 
//

#include "testoggRecorder.h"

/**
*
* CTestMmfOggAudioRecorderBase()
*
* Constructor to set the Test step name, section name and key names.
*
*/
CTestMmfOggAudioRecorderBase::CTestMmfOggAudioRecorderBase(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName)
	{
	iTestStepName = aTestName;
	iSectName = aSectName;
	iKeyName = aKeyName;
	}

/**
*
* StateInit()
*
*/
void CTestMmfOggAudioRecorderBase::StateInit()
    {
    iState = 0;
    iExpectedRecEvent = EOggIdleAudioRecorder;
    iExpectedRecState = CMdaAudioClipUtility::ENotReady;
    iExpectedError = KErrNone;
    }

/**
*
* StateChange()
*
* @param TMmfAudioOggRecEvents aExpectedEvent
* @param CMdaAudioClipUtility::TState aExpectedRecorderState
* @param TInt aExpectedError
*
*/
void CTestMmfOggAudioRecorderBase::StateChange(TMmfAudioOggRecEvents aExpectedEvent, 
    CMdaAudioClipUtility::TState aExpectedRecorderState, TInt aExpectedError)
    {
    iExpectedRecEvent = aExpectedEvent;
    iExpectedRecState = aExpectedRecorderState;
    iExpectedError = aExpectedError;

    iState++;
    }

/**
*
* FsmL()
*
* @param TMmfAudioOggRecEvents aEventCode
* @param CMdaAudioClipUtility::TState aRecorderState
* @param TInt aError
*
*/
void CTestMmfOggAudioRecorderBase::FsmL(TMmfAudioOggRecEvents aEventCode, CMdaAudioClipUtility::TState aRecorderState, TInt aError)
    {
    if ((iExpectedError != KErrUnderflow) && (iError == KErrUnderflow))
        {
        INFO_PRINTF1(_L("Underflow!"));
        }
    else if (iExpectedRecEvent != aEventCode)
        {
        ERR_PRINTF3(_L("ExpectedEvent : %d, ActualEvent : %d"), iExpectedRecEvent, aEventCode);
        CActiveScheduler::Stop();
        }
    else if (iExpectedError != aError)
        {
        ERR_PRINTF3(_L("ExpectedError : %d, ActualError : %d"), iExpectedError, iError);
        CActiveScheduler::Stop();
        }
    else if (iExpectedRecState != aRecorderState)
        {
        ERR_PRINTF3(_L("ExpectedRecorderState : %d, ActualRecorderState : %d"), iExpectedRecState, aRecorderState);
        CActiveScheduler::Stop();
        }
    else
        {
        switch (iState)
            {
            case 0:
                {
                INFO_PRINTF2(_L("Opening the file %S.."), &iFileName);
                iAudioRecorder->OpenFileL(iFileName);
                StateChange(EOggMoscoStateChange, CMdaAudioClipUtility::EOpen);
                break;
                }

            case 1:
                {
                //AfterOpenL();
                INFO_PRINTF1(_L("iAudioRecorder->SetMaxWriteLength(40000)"));
                iAudioRecorder->SetMaxWriteLength(KMaxWriteLength);
                INFO_PRINTF1(_L("Record the audio file..."));
                iAudioRecorder->RecordL();
                StateChange(EOggMoscoStateChange, CMdaAudioClipUtility::ERecording);
                break;    
                }
			
			case 2:
			    {
			    StateChange(EOggMoscoStateChange, CMdaAudioClipUtility::EOpen);
				break;
			    }
				
            case 3:
                {
                // Check if there's error while recording the file
                if (aError != KErrNone)
                    {
                    ERR_PRINTF2(_L("Recording the audio file failed, unexpected error %d."), aError);
                    }
                else
                    {
                    INFO_PRINTF1(_L("Audio file opened and recorded."));
                    iTestStepResult = EPass;
                    }
                iAudioRecorder->Close();
                CActiveScheduler::Stop();
                break;    
                }

            default:
                {
                INFO_PRINTF1(_L("Invalid Audio event!"));
                }
            }
        }
    }
 
/**
*
* Do the test step.
* Each test step must supply an implementation for DoTestStepL.
*
* @return TVerdict: The result of the test step.
*
*/
TVerdict CTestMmfOggAudioRecorderBase::DoTestStepL()
    {
    iTestStepResult = EFail;

    // Initialise the state variables
    StateInit();

    TRAPD(err,FsmL(EOggIdleAudioRecorder, CMdaAudioClipUtility::ENotReady)); // Call to open the audio file
    if(err == KErrNone)
    	{
        CActiveScheduler::Start(); // ActiveScheduler started ONLY once
		if (iError == KErrNone)
			{
			iTestStepResult = EPass;
			}
    	}
    return iTestStepResult;
    }
/**
*
* Test step Preamble.
*
* @return TVerdict: The result of the test step.
* 
*/
TVerdict CTestMmfOggAudioRecorderBase::DoTestStepPreambleL()
    {
    // Install the scheduler
    TVerdict verdict = CTestMmfAclntStep::DoTestStepPreambleL();

    iError = KErrNone;

    // Get the filename
    TPtrC filename1;
    if (!GetStringFromConfig(iSectName, iKeyName, filename1))
        {
        return EInconclusive;
        }
    GetDriveName(iFileName);
    iFileName.Append(filename1);

    // Create an audio recorder
    iAudioRecorder = CMdaAudioRecorderUtility::NewL(*this);
    if (iAudioRecorder == NULL)
        {
        return EInconclusive;
        }

    return verdict;
    }

/**
*
* Test step Postamble.
*
* @return TVerdict: The result of the test step.
* 
*/
TVerdict CTestMmfOggAudioRecorderBase::DoTestStepPostambleL()
    {
    delete iAudioRecorder;
    iAudioRecorder = NULL;
    User::After(KOneSecond); // wait for deletion to shut down devsound
    return CTestMmfAclntStep::DoTestStepPostambleL(); // Destroy the scheduler
    }

/**
*
* Implementation of the MoscoStateChangeEvent interface function.
*
* @param CBase* aObject
* @param TInt aPreviousState
* @param TInt aCurrentState
* @param TInt aErrorCode
*
*/
void CTestMmfOggAudioRecorderBase::MoscoStateChangeEvent(CBase* aObject, 
    TInt aPreviousState, TInt aCurrentState, TInt aErrorCode)
    {
    iObject = aObject;
    iPreviousState = aPreviousState;
    iCurrentState = aCurrentState;
    iError = aErrorCode;
	if ((iError != KErrEof) && (iError != KErrNone))
    	{
    	ERR_PRINTF2(_L("AudioRecPlay failed, unexpected error %d."), aErrorCode);
    	iAudioRecorder->Close();
    	CActiveScheduler::Stop();
    	}
    else
		{
		INFO_PRINTF1(_L("MoscoStateChangeEvent."));
		INFO_PRINTF4(_L("Error: %d, PreviousState: %d, CurrentState: %d"), iError, iPreviousState, iCurrentState);
		TRAP(iError, FsmL(EOggMoscoStateChange, iAudioRecorder->State(), aErrorCode));
		if (iError != KErrNone)
		    {
		    ERR_PRINTF2(_L("Problem in state change, unexpected error %d."), iError);
		    }
		}
	}

/**
*
* NewL()
*
* @param const TDesC& aTestName
*
* @return CTestMmfOggAudioRecorderBase*
*                       Constructed CTestMmfAudioRecOgg object
*
*/
CTestMmfOggAudioRecorderBase* CTestMmfOggAudioRecorderBase::NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName)
    {
    return new (ELeave) CTestMmfOggAudioRecorderBase(aTestName, aSectName, aKeyName);
    }

