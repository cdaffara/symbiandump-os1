// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Integration tests.
// 
//

#include "TestAudioRecorderDRM.h"

// CTestMmfDRMAudioRecorderBase implementation

/**
*
* CTestMmfDRMAudioRecorderBase()
*
* Constructor to set the Test step name, section name and key names.
*
*/
CTestMmfDRMAudioRecorderBase::CTestMmfDRMAudioRecorderBase(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TDesC& aUniqueId)
	{
	iTestStepName = aTestName;
	iSectName = aSectName;
	iKeyName = aKeyName;
	iUniqueId = aUniqueId;
	}

/**
*
* FsmL()
*
* @param TMmfAudioRecEvents aEventCode
* @param CMdaAudioClipUtility::TState aRecorderState
* @param TInt aError
*
*/
void CTestMmfDRMAudioRecorderBase::FsmL(TMmfAudioRecEvents aEventCode, CMdaAudioClipUtility::TState aRecorderState, TInt aError)
    {
    if ((iExpectedError != KErrUnderflow) && (aError == KErrUnderflow))
        {
        INFO_PRINTF1(_L("Underflow!"));
        }
    else if (iExpectedRecEvent != aEventCode)
        {
        ERR_PRINTF3(_L("ExpectedEvent : %d, ActualEvent : %d"), iExpectedRecEvent, aEventCode);
        iTestStepResult = EFail;
        CActiveScheduler::Stop();
        }
    else if (iExpectedError != aError)
        {
        ERR_PRINTF3(_L("ExpectedError : %d, ActualError : %d"), iExpectedError, aError);
        iTestStepResult = EFail;
        CActiveScheduler::Stop();
        }
    else if (iExpectedRecState != aRecorderState)
        {
        ERR_PRINTF3(_L("ExpectedRecorderState : %d, ActualRecorderState : %d"), iExpectedRecState, aRecorderState);
        iTestStepResult = EFail;
        CActiveScheduler::Stop();
        }
    else
        {
        switch (iState)
            {
            case 0:
                INFO_PRINTF3(_L("Opening the file for PLAY intent: %S, UniqueID : %S"), &iFileName, &iUniqueId);
                iAudioRecorder->OpenFileL(TMMFileSource(iFileName, iUniqueId, ContentAccess::EPlay));
                StateChange(EMoscoStateChange, CMdaAudioClipUtility::EOpen);
                break;

            case 1:
                if (aError != KErrNone) // Check if there's error while opening the file
                    {
                    ERR_PRINTF2(_L("Opening the audio file failed, unexpected error %d."), iError);
                    CActiveScheduler::Stop();
                    }
                else
                    {
                    AfterOpenL();
                    INFO_PRINTF1(_L("Playing the audio file..."));
                    iAudioRecorder->PlayL();
                    StateChange(EMoscoStateChange, CMdaAudioClipUtility::EPlaying);
                    }
                break;
			
			case 2:
				StateChange(EMoscoStateChange, CMdaAudioClipUtility::EOpen);
				break;
				
            case 3:
                if (aError != KErrNone) // Check if there's error while playing the file
                    {
                    ERR_PRINTF2(_L("Playing the audio file failed, unexpected error %d."), iError);
                    }
                else
                    {
                    INFO_PRINTF1(_L("Audio file opened and played."));
                    iTestStepResult = EPass;
                    }
                iAudioRecorder->Close();
                CActiveScheduler::Stop();
                break;

            default:
                INFO_PRINTF1(_L("Invalid Audio event!"));
            }
        }
    }
    
void CTestMmfDRMAudioRecorderBase::AfterOpenL()
	{
    }

/**
*
* Test step Preamble.
*
* @return TVerdict: The result of the test step.
* 
*/
TVerdict CTestMmfDRMAudioRecorderBase::DoTestStepPreambleL()
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
TVerdict CTestMmfDRMAudioRecorderBase::DoTestStepPostambleL()
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
void CTestMmfDRMAudioRecorderBase::MoscoStateChangeEvent(CBase* aObject, 
    TInt aPreviousState, TInt aCurrentState, TInt aErrorCode)
    {
    iObject = aObject;
    iPreviousState = aPreviousState;
    iCurrentState = aCurrentState;
    iError = aErrorCode;
	if (iError != KErrNone)
    	{
    	ERR_PRINTF2(_L("AudioRecPlay failed, unexpected error %d."), aErrorCode);
    	iAudioRecorder->Close();
    	CActiveScheduler::Stop();
    	}
    else
		{
		INFO_PRINTF1(_L("MoscoStateChangeEvent."));
		INFO_PRINTF4(_L("Error: %d, PreviousState: %d, CurrentState: %d"), iError, iPreviousState, iCurrentState);
		TRAP(iError, FsmL(EMoscoStateChange, iAudioRecorder->State(), aErrorCode));
		}
	}

/**
*
* StateInit()
*
*/
void CTestMmfDRMAudioRecorderBase::StateInit()
    {
    iState = 0;
    iExpectedRecEvent = EIdleAudioRecorder;
    iExpectedRecState = CMdaAudioClipUtility::ENotReady;
    iExpectedError = KErrNone;
    }

/**
*
* StateChange()
*
* @param TMmfAudioRecEvents aExpectedEvent
* @param CMdaAudioClipUtility::TState aExpectedRecorderState
* @param TInt aExpectedError
*
*/
void CTestMmfDRMAudioRecorderBase::StateChange(TMmfAudioRecEvents aExpectedEvent, 
    CMdaAudioClipUtility::TState aExpectedRecorderState, TInt aExpectedError)
    {
    iExpectedRecEvent = aExpectedEvent;
    iExpectedRecState = aExpectedRecorderState;
    iExpectedError = aExpectedError;

    iState++;
    }


// Positive Tests

/**
*
* NewL()
*
* @param const TDesC& aTestName
*
* @return CTestMmfAudioRecPlayDRMEnableAgentUI*
*                       Constructed CTestMmfAudioRecPlayDRMEnableAgentUI object
*
*/
CTestMmfAudioRecPlayDRMEnableAgentUI* CTestMmfAudioRecPlayDRMEnableAgentUI::NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TDesC& aUniqueId)
    {
    return new (ELeave) CTestMmfAudioRecPlayDRMEnableAgentUI(aTestName, aSectName, aKeyName, aUniqueId);
    }

/**
*
* Test step constructor.
* Ctor for CTestMmfAudioRecPlayDRMEnableAgentUI.
*
* @param const TDesC& aTestName
*
*/
CTestMmfAudioRecPlayDRMEnableAgentUI::CTestMmfAudioRecPlayDRMEnableAgentUI(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TDesC& aUniqueId)
	:CTestMmfDRMAudioRecorderBase(aTestName, aSectName, aKeyName, aUniqueId)
    {
    }

void CTestMmfAudioRecPlayDRMEnableAgentUI::AfterOpenL()
	{
    // Create DRM custom command
    MMMFDRMCustomCommand* drmCustomCommand;
    drmCustomCommand = iAudioRecorder->GetDRMCustomCommand();

    if (drmCustomCommand == NULL)
        {
        ERR_PRINTF1(_L("MMMFDRMCustomCommand is NULL."));
        iError = KErrUnknown;
        User::Leave(KErrUnknown);
        }

    INFO_PRINTF1(_L("Enabling Agent's user interface for errors and confirmation requests."));
    iError = drmCustomCommand->SetAgentProperty(ContentAccess::EAgentPropertyAgentUI, 3);
    // After DevCR JFOT-6EMFVL (which changes the CContentFile::SetAgentProperty to call 
    // SetProperty on a member of CData instead of CContent), the underlying reference test
    // agent returns a different error value (KErrCANotSupported instead of KErrNone).
    if (iError == KErrCANotSupported)
        {
        // Reset the error code as it's not really an error :)
		iError = KErrNone;
        }
    else
    	{
        ERR_PRINTF2(_L("Enabling Agent's user interface failed, unexpected error %d."), iError);
        User::Leave(iError);
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
TVerdict CTestMmfAudioRecPlayDRMEnableAgentUI::DoTestStepL()
    {
    INFO_PRINTF1(_L("Enable agent's user interface for errors and confirmation requests property. Open and play a protected audio clip from a file for PLAY intent."));

    iTestStepResult = EFail;

    // Initialise the state variables
    StateInit();

    TRAPD(err,FsmL(EIdleAudioRecorder, CMdaAudioClipUtility::ENotReady)); // Call to open the audio file
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
* NewL()
*
* @param const TDesC& aTestName
*
* @return CTestMmfAudioRecPlayDRMDisableAutoIntent*
*                       Constructed CTestMmfAudioRecPlayDRMDisableAutoIntent object
*
*/
CTestMmfAudioRecPlayDRMDisableAutoIntent* CTestMmfAudioRecPlayDRMDisableAutoIntent::NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TDesC& aUniqueId)
    {
    return new (ELeave) CTestMmfAudioRecPlayDRMDisableAutoIntent(aTestName, aSectName, aKeyName, aUniqueId);
    }

/**
*
* Test step constructor.
* Ctor for CTestMmfAudioRecPlayDRMDisableAutoIntent.
*
* @param const TDesC& aTestName
*
*/
CTestMmfAudioRecPlayDRMDisableAutoIntent::CTestMmfAudioRecPlayDRMDisableAutoIntent(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TDesC& aUniqueId)
	:CTestMmfDRMAudioRecorderBase(aTestName, aSectName, aKeyName, aUniqueId)
    {
    }


void CTestMmfAudioRecPlayDRMDisableAutoIntent::AfterOpenL()
	{
    // Create DRM custom command
    MMMFDRMCustomCommand* drmCustomCommand;
    drmCustomCommand = iAudioRecorder->GetDRMCustomCommand();

    if (drmCustomCommand == NULL)
        {
        ERR_PRINTF1(_L("MMMFDRMCustomCommand is NULL."));
        iError = KErrUnknown;
        User::Leave(KErrUnknown);
        }

    INFO_PRINTF1(_L("Disabling automatic intent..."));
    iError = drmCustomCommand->DisableAutomaticIntent(EFalse);
    if (iError != KErrNone)
        {
        ERR_PRINTF2(_L("Disabling the automatic intent failed, unexpected error %d."), iError);
        User::Leave(iError);
        }
    
    INFO_PRINTF1(_L("Executing intent..."));
	iError = drmCustomCommand->ExecuteIntent(ContentAccess::EPlay);
	if (iError != KErrNone)
    	{
    	ERR_PRINTF2(_L("Executing the intent failed, unexpected error %d."), iError);
    	User::Leave(iError);
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
TVerdict CTestMmfAudioRecPlayDRMDisableAutoIntent::DoTestStepL()
    {
    INFO_PRINTF1(_L("Open a protected audio file and disable automatic intent. Play."));

    iTestStepResult = EFail;

    // Initialise the state variables
    StateInit();

    TRAPD(err,FsmL(EIdleAudioRecorder, CMdaAudioClipUtility::ENotReady)); // Call to open the audio file
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
