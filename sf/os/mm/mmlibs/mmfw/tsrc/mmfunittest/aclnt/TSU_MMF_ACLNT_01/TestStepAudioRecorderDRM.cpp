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
// Unit tests. The client is opened with DRM intent and played.
// 
//

#include "TestStepAudioRecorderDRM.h"

#define INFO_PRINTF1_IF_LOGS_ENABLED( a ) if(iEnableLogs) 		\
											{					\
											INFO_PRINTF1( a );	\
											}											
											
#define INFO_PRINTF2_IF_LOGS_ENABLED( a, b ) if(iEnableLogs)					\
												{								\
												INFO_PRINTF2( ( a ), ( b ) );	\
												}
											
#define INFO_PRINTF3_IF_LOGS_ENABLED( a, b, c ) if(iEnableLogs)								\
													{										\
													INFO_PRINTF3( ( a ), ( b ), ( c ) );	\
													}
													
#define INFO_PRINTF4_IF_LOGS_ENABLED( a, b, c, d ) if(iEnableLogs)								\
													{											\
													INFO_PRINTF4( ( a ), ( b ), ( c ), ( d ) );	\
													}													
											
#define ERR_PRINTF1_IF_LOGS_ENABLED( a ) if(iEnableLogs)		\
											{					\
											ERR_PRINTF1( a );	\
											}											
											
#define ERR_PRINTF2_IF_LOGS_ENABLED( a, b ) if(iEnableLogs)						\
												{								\
												ERR_PRINTF2( ( a ), ( b ) );	\
												}
											
#define ERR_PRINTF3_IF_LOGS_ENABLED( a, b, c ) if(iEnableLogs)							\
													{									\
													ERR_PRINTF3( ( a ), ( b ), ( c ) );	\
													}						

// Unique Id of the content
_LIT(KRightsRecUniqueId001, "AudioTestmainTstId001");
_LIT(KRightsRecUniqueId002, "\\Container001\\AudioTestmainTst2Id002");

_LIT(KRightsRecUniqueId003, "AudioTestNoRightsId001");
_LIT(KRightsRecInvalidUniqueId, "AudioTestInvalidUniqueId");

// Used in a time out function, 1 second (in microSeconds)
const TInt KOneSecond = 1000000;


// CTestMmfDRMAudioRecorderBase implementation

/**
*
* CTestMmfDRMAudioRecorderBase constructor.
*
*/
CTestMmfDRMAudioRecorderBase::CTestMmfDRMAudioRecorderBase()
	{
	iEnableLogs = ETrue;//By deafult log the messages    
	}
	
/**
*
* FsmL()
*
* @param TMmfAudioRecorderEvents aEventCode
* @param CMdaAudioClipUtility::TState aRecorderState
* @param TInt aError
*
*/
void CTestMmfDRMAudioRecorderBase::FsmL(TMmfAudioRecorderEvents aEventCode, CMdaAudioClipUtility::TState aRecorderState, TInt aError)
    {
    if ((iExpectedError != KErrUnderflow) && (aError == KErrUnderflow))
        {
        INFO_PRINTF1_IF_LOGS_ENABLED(_L("Underflow!"));
        }
    else if (iExpectedRecEvent != aEventCode)
        {
        ERR_PRINTF3_IF_LOGS_ENABLED(_L("ExpectedEvent : %d, ActualEvent : %d"), iExpectedRecEvent, aEventCode);        
        iTestStepResult = EFail;
        CActiveScheduler::Stop();
        }
    else if (iExpectedError != aError)
        {
        ERR_PRINTF3_IF_LOGS_ENABLED(_L("ExpectedError : %d, ActualError : %d"), iExpectedError, aError);         
        iTestStepResult = EFail;
		CActiveScheduler::Stop();
        }
    else if (iExpectedRecState != aRecorderState)
        {
        ERR_PRINTF3_IF_LOGS_ENABLED(_L("ExpectedRecorderState : %d, ActualRecorderState : %d"), iExpectedRecState, aRecorderState);              
        iTestStepResult = EFail;
        CActiveScheduler::Stop();
        }
    else
        {
        switch (iState)
            {
            case 0:
            	INFO_PRINTF2_IF_LOGS_ENABLED(_L("Opening the file for PLAY intent: %S"), &iFileName);
                if (iTestStepName == _L("MM-MMF-ACLNT-U-0915-HP"))
                    {
                    iAudioRecorder->OpenFileL(TMMFileSource(iFileName, KRightsRecUniqueId002, ContentAccess::EPlay));
                    }
                else
                    {
                    iAudioRecorder->OpenFileL(TMMFileSource(iFileName, KRightsRecUniqueId001, ContentAccess::EPlay));
                    }
                StateChange(EMoscoStateChange, CMdaAudioClipUtility::EOpen);
                break;

            case 1:
                if (aError != KErrNone) // Check if there's error while opening the file
                    {
                    ERR_PRINTF2_IF_LOGS_ENABLED(_L("Opening the audio file failed, unexpected error %d."), iError);
                    CActiveScheduler::Stop();
                    }
                else
                    {
                    if (iTestStepName == _L("MM-MMF-ACLNT-U-0913-HP"))
                        {
                        INFO_PRINTF1_IF_LOGS_ENABLED(_L("Playing the audio file...for 3 seconds."));
                        iAudioRecorder->PlayL();
                        User::After(3000000);                        
                        INFO_PRINTF1_IF_LOGS_ENABLED(_L("Stoping the play of the audio file..."));
                        iAudioRecorder->Stop();
                   		StateChange(EMoscoStateChange, CMdaAudioClipUtility::EOpen);
                        }
                    else
                        {
                        INFO_PRINTF1_IF_LOGS_ENABLED(_L("Playing the audio file..."));
                        iAudioRecorder->PlayL();
                        StateChange(EMoscoStateChange, CMdaAudioClipUtility::EPlaying);
                        }
                     }
                break;

            case 2:
                if (aError != KErrNone) // Check if there's error while playing the file
                    {
                    ERR_PRINTF2_IF_LOGS_ENABLED(_L("Playing the audio file failed, unexpected error %d."), iError);
                    }
                else
                    {
                    INFO_PRINTF1_IF_LOGS_ENABLED(_L("Audio file opened and played."));
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

/**
*
* FsmHandleL()
*
* @param TMmfAudioRecorderEvents aEventCode
* @param CMdaAudioClipUtility::TState aRecorderState
* @param TInt aError
*
*/
void CTestMmfDRMAudioRecorderBase::FsmHandleL(TMmfAudioRecorderEvents aEventCode, CMdaAudioClipUtility::TState aRecorderState, TInt aError)
    {
    if ((iExpectedError != KErrUnderflow) && (aError == KErrUnderflow))
        {
        INFO_PRINTF1_IF_LOGS_ENABLED(_L("Underflow!"));
        }
    else if (iExpectedRecEvent != aEventCode)
        {
        ERR_PRINTF3_IF_LOGS_ENABLED(_L("ExpectedEvent : %d, ActualEvent : %d"), iExpectedRecEvent, aEventCode);
        iTestStepResult = EFail;
        CActiveScheduler::Stop();
        }
    else if (iExpectedError != aError)
        {
        ERR_PRINTF3_IF_LOGS_ENABLED(_L("ExpectedError : %d, ActualError : %d"), iExpectedError, aError);
        iTestStepResult = EFail;
        CActiveScheduler::Stop();
        }
    else if (iExpectedRecState != aRecorderState)
        {
        ERR_PRINTF3_IF_LOGS_ENABLED(_L("ExpectedRecorderState : %d, ActualRecorderState : %d"), iExpectedRecState, aRecorderState);
        iTestStepResult = EFail;
        CActiveScheduler::Stop();
        }
    else
        {
        switch (iState)
            {
            case 0:
            	INFO_PRINTF2_IF_LOGS_ENABLED(_L("Opening the file for PLAY intent: %S"), &iFileName);
                iAudioRecorder->OpenFileL(TMMFileHandleSource(iFile, KRightsRecUniqueId001, ContentAccess::EPlay));
                StateChange(EMoscoStateChange, CMdaAudioClipUtility::EOpen);
                break;

            case 1:
                if (iError != KErrNone) // Check if there's error while opening the file
                    {
                    ERR_PRINTF2_IF_LOGS_ENABLED(_L("Opening the audio file failed, unexpected error %d."), iError);
                    iAudioRecorder->Close();
                    CActiveScheduler::Stop();
                    }
                else
                    {
                    if (iTestStepName == _L("MM-MMF-ACLNT-U-0913-HP"))
                        {
                        INFO_PRINTF1_IF_LOGS_ENABLED(_L("Playing the audio file...for 3 seconds."));
                        iAudioRecorder->PlayL();
                        User::After(3000000);

						INFO_PRINTF1_IF_LOGS_ENABLED(_L("Stoping the play of the audio file..."));
                        iAudioRecorder->Stop();    
                        // Moves iState to the next state
                        StateChange(EMoscoStateChange, CMdaAudioClipUtility::EOpen);                                             
                        }
                    else
                        {
                        INFO_PRINTF1_IF_LOGS_ENABLED(_L("Playing the audio file..."));
                        iAudioRecorder->PlayL();
                        StateChange(EMoscoStateChange, CMdaAudioClipUtility::EPlaying);
                        }
                     }
                break;

            case 2:
                if (iError != KErrNone) // Check if there's error while playing the file
                    {
                    ERR_PRINTF2_IF_LOGS_ENABLED(_L("Playing the audio file failed, unexpected error %d."), iError);
                    }
                else
                    {
                    INFO_PRINTF1_IF_LOGS_ENABLED(_L("Audio file opened and played."));
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

/**
*
* Test step Preamble.
*
* @return enum TVerdict: The result of the test step.
* 
*/
enum TVerdict CTestMmfDRMAudioRecorderBase::DoTestStepPreambleL()
    {
    // Install the scheduler
    enum TVerdict verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();

    iError = KErrNone;

    // Get the filename
    // iFileName will be the content file, containing audio files
    if (!GetStringFromConfig(_L("SectionDRM"), _L("AudioContent"), iFileName))
        {
        return EInconclusive;
        }

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
* @return enum TVerdict: The result of the test step.
* 
*/
enum TVerdict CTestMmfDRMAudioRecorderBase::DoTestStepPostambleL()
    {
    delete iAudioRecorder;
    iAudioRecorder = NULL;
    User::After(KOneSecond); // wait for deletion to shut down devsound
    return CTestStepUnitMMFAudClient::DoTestStepPostambleL(); // Destroy the scheduler
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
    	ERR_PRINTF1_IF_LOGS_ENABLED(_L("AudioRecPlay failed."));
    	iAudioRecorder->Close();
    	CActiveScheduler::Stop();
    	}
    else
    	{
    	INFO_PRINTF1_IF_LOGS_ENABLED(_L("MoscoStateChangeEvent."));
    	INFO_PRINTF4_IF_LOGS_ENABLED(_L("Error: %d, PreviousState: %d, CurrentState: %d"), iError, iPreviousState, iCurrentState);
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
    iExpectedRecEvent = EIdleRecorder;
    iExpectedRecState = CMdaAudioClipUtility::ENotReady;
    iExpectedError = KErrNone;
    }

/**
*
* StateChange()
*
* @param TMmfAudioRecorderEvents aExpectedEvent
* @param CMdaAudioClipUtility::TState aExpectedRecorderState
* @param TInt aExpectedError
*
*/
void CTestMmfDRMAudioRecorderBase::StateChange(TMmfAudioRecorderEvents aExpectedEvent, 
    CMdaAudioClipUtility::TState aExpectedRecorderState, TInt aExpectedError)
    {
    iExpectedRecEvent = aExpectedEvent;
    iExpectedRecState = aExpectedRecorderState;
    iExpectedError = aExpectedError;

    iState++;
    }
/**
*
* EnableLogs()
*
* @param TBool aEnable
*
*/
inline void CTestMmfDRMAudioRecorderBase::EnableLogs(TBool aEnable)
{
	iEnableLogs = aEnable;	
}

// Positive Tests

/**
*
* Ctor for CTestMmfDRMAudioRecPlayFileSource
*
* @return CTestMmfDRMAudioRecPlayFileSource*: constructed CTestMmfDRMAudioRecPlayFileSource object
*
*/
CTestMmfDRMAudioRecPlayFileSource* CTestMmfDRMAudioRecPlayFileSource::NewL()
    {
    return new (ELeave) CTestMmfDRMAudioRecPlayFileSource;
    }

/**
*
* Test step constructor.
* Each test step initialises its own name.
*
*/
CTestMmfDRMAudioRecPlayFileSource::CTestMmfDRMAudioRecPlayFileSource()
    {
    // Store the name of this test case
    // This is the name that is used by the script file
    iTestStepName = _L("MM-MMF-ACLNT-U-0911-HP");
    }

/**
*
* Do the test step.
* Each test step must supply an implementation for DoTestStepL.
*
* @return TVerdict: The result of the test step.
*
*/
TVerdict CTestMmfDRMAudioRecPlayFileSource::DoTestStepL()
    {
    INFO_PRINTF1(_L("Open and play a protected audio clip from a file for PLAY intent. (TMMFileSource)"));
	iTestStepResult = EFail;
    // Initialise the state variables
    StateInit();

    TRAPD(err, FsmL(EIdleRecorder, CMdaAudioClipUtility::ENotReady)); // Call to open the audio file
    if (err == KErrNone)
    	{
 		CActiveScheduler::Start(); // ActiveScheduler started ONLY once   	
    	}
     return iTestStepResult;
    }



/**
*
* Ctor for CTestMmfDRMAudioRecPlayFileHandleSource
*
* @return CTestMmfDRMAudioRecPlayFileHandleSource*: 
*                           constructed CTestMmfDRMAudioRecPlayFileHandleSource object
*
*/
CTestMmfDRMAudioRecPlayFileHandleSource* CTestMmfDRMAudioRecPlayFileHandleSource::NewL()
    {
    return new (ELeave) CTestMmfDRMAudioRecPlayFileHandleSource;
    }

/**
*
* Test step constructor.
* Each test step initialises its own name.
*
*/
CTestMmfDRMAudioRecPlayFileHandleSource::CTestMmfDRMAudioRecPlayFileHandleSource()
    {
    iTestStepName = _L("MM-MMF-ACLNT-U-0912-HP");
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
void CTestMmfDRMAudioRecPlayFileHandleSource::MoscoStateChangeEvent(CBase* aObject, 
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
      	INFO_PRINTF4_IF_LOGS_ENABLED(_L("Error: %d, PreviousState: %d, CurrentState: %d"), iError, iPreviousState, iCurrentState);
    	TRAP(iError, FsmHandleL(EMoscoStateChange, iAudioRecorder->State(), aErrorCode));
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
TVerdict CTestMmfDRMAudioRecPlayFileHandleSource::DoTestStepL()
    {
    INFO_PRINTF1(_L("Open and play a protected audio clip from a file for PLAY intent. (TMMFileHandleSource)"));
    iTestStepResult = EFail;
    User::LeaveIfError(iFs.Connect());
    User::LeaveIfError(iFs.ShareProtected());
    User::LeaveIfError(iFile.Open(iFs, iFileName, EFileRead));

    // Initialise the state variables
    StateInit();

    TRAPD(err, FsmHandleL(EIdleRecorder, CMdaAudioClipUtility::ENotReady)); // Call to open the audio file thru file handle
    if (err == KErrNone)
    	{
    	CActiveScheduler::Start(); // ActiveScheduler started ONLY once
    	}
    
    iFile.Close();
    iFs.Close();

    return iTestStepResult;
    }



/**
*
* Ctor for CTestMmfDRMAudioRecPlayStopFileSource
*
* @return CTestMmfDRMAudioRecPlayStopFileSource*: 
*                   constructed CTestMmfDRMAudioRecPlayStopFileSource object
*
*/
CTestMmfDRMAudioRecPlayStopFileSource* CTestMmfDRMAudioRecPlayStopFileSource::NewL()
    {
    return new (ELeave) CTestMmfDRMAudioRecPlayStopFileSource;
    }

/**
*
* Test step constructor.
* Each test step initialises its own name.
*
*/
CTestMmfDRMAudioRecPlayStopFileSource::CTestMmfDRMAudioRecPlayStopFileSource()
    {
    iTestStepName = _L("MM-MMF-ACLNT-U-0913-HP");
    }

/**
*
* Do the test step.
* Each test step must supply an implementation for DoTestStepL.
*
* @return TVerdict: The result of the test step.
*
*/
TVerdict CTestMmfDRMAudioRecPlayStopFileSource::DoTestStepL()
    {
    INFO_PRINTF1(_L("Open, play and stop the play of the protected audio clip from a file for PLAY/STOP intents. (TMMFileSource)"));
    iTestStepResult = EFail;
    // Initialise the state variables
    StateInit();

    TRAPD(err,FsmL(EIdleRecorder, CMdaAudioClipUtility::ENotReady)); // Call to open the audio file
    if (err == KErrNone)
    	{
    	CActiveScheduler::Start(); 
    	}
    else
    	{
		INFO_PRINTF2(_L("FsmL(EIdleRecorder, CMdaAudioClipUtility::ENotReady)) left with error %d"),err);
		return EFail;
    	}
    
    return iTestStepResult;
    }



/**
*
* Ctor for CTestMmfDRMAudioRecPlayAllocFileSource
*
* @return CTestMmfDRMAudioRecPlayAllocFileSource*: 
*                           constructed CTestMmfDRMAudioRecPlayAllocFileSource object
*
*/
CTestMmfDRMAudioRecPlayAllocFileSource* CTestMmfDRMAudioRecPlayAllocFileSource::NewL()
    {
    return new (ELeave) CTestMmfDRMAudioRecPlayAllocFileSource;
    }

/**
*
* Test step constructor.
* Each test step initialises its own name.
*
*/
CTestMmfDRMAudioRecPlayAllocFileSource::CTestMmfDRMAudioRecPlayAllocFileSource()
    {
    iTestStepName = _L("MM-MMF-ACLNT-U-0914-HP");
    }

/**
*
* Play audio by marking heap.
*
*/
void CTestMmfDRMAudioRecPlayAllocFileSource::AudioRecPlayAllocL()
    {
    __MM_HEAP_MARK;

    // Initialise the state variables
    StateInit();

    TRAPD(err, FsmL(EIdleRecorder, CMdaAudioClipUtility::ENotReady)); // Call to open the audio file
    if (err == KErrNone)
    	{
    	CActiveScheduler::Start(); // ActiveScheduler started ONLY once
    	}

	 iAudioRecorder->Close();
    
    __MM_HEAP_MARKEND;
    }

 
/**
*
* Do the test step.
* Each test step must supply an implementation for DoTestStepL.
*
* @return TVerdict: The result of the test step.
*
*/
TVerdict CTestMmfDRMAudioRecPlayAllocFileSource::DoTestStepL()
    {
    INFO_PRINTF1(_L("Alloc test to open and play a protected audio clip from a file for PLAY intent. (TMMFileSource)"));

    TRAPD(err, AudioRecPlayAllocL());
    
    if (err != KErrNone)
    	{
    	iError = err;
    	}
    
    
    if (iError != KErrNone)
        {
        INFO_PRINTF2(_L("Opening/playing the audio file failed, unexpected error %d."), iError);
        return EFail;
        }

    iTestStepResult  = EPass;

    TInt failCount   = 1;
    TBool completed  = EFalse;
    TBool badResult  = EFalse;
    TBool reachedEnd = EFalse;
    TInt  error = KErrNone;

	EnableLogs(EFalse);
    for(;;)
        {
        __UHEAP_SETFAIL(RHeap::EFailNext, failCount);
        __MM_HEAP_MARK;
		
		TRAP(error, AudioRecPlayAllocL());
		if (error != KErrNone)
			{
			iError = error;
			}

        completed = EFalse;
        if (iError == KErrNone)
            {
            TAny *testAlloc = User::Alloc(1); // when this fails, we passed through all allocs within test
            if (testAlloc == NULL)
                {
                reachedEnd = ETrue;
                failCount--;
                }
            else
                {
                User::Free(testAlloc);
                testAlloc = NULL;
                }

            if (iTestStepResult != EPass)
                {
                badResult = ETrue;
                }

            completed = reachedEnd || badResult;
            }
        else if (iError != KErrNoMemory)
            {
            __UHEAP_SETFAIL(RHeap::ENone, 0);
            ERR_PRINTF2(_L("Bad error code %d"), iError);
            completed = ETrue;
            badResult = EFail;
            }

        __MM_HEAP_MARKEND;
        __UHEAP_SETFAIL(RHeap::ENone, 0);

        if (completed)
            {
            break;
            }

        failCount++;
        }

    if (iError != KErrNone || badResult)
        {
        if (badResult)
            {
            INFO_PRINTF3(_L("Bad result with %d memory allocations tested. The Error returned is %d."), failCount, iError);
            }
        else
            {
            INFO_PRINTF3(_L("Error(%d) with %d memory allocations tested."), failCount, iError);
            }

        iTestStepResult = EFail;
        }
    else
        {
        INFO_PRINTF2(_L("Completed OK with %d memory allocations tested.\n"), failCount);
        iTestStepResult = EPass;
        }

    return iTestStepResult;
    }



/**
*
* Ctor for CTestMmfDRMAudioRecPlayUniqueIdFileSource
*
* @return CTestMmfDRMAudioRecPlayUniqueIdFileSource*: 
*                               constructed CTestMmfDRMAudioRecPlayUniqueIdFileSource object
*
*/
CTestMmfDRMAudioRecPlayUniqueIdFileSource* CTestMmfDRMAudioRecPlayUniqueIdFileSource::NewL()
    {
    return new (ELeave) CTestMmfDRMAudioRecPlayUniqueIdFileSource;
    }

/**
*
* Test step constructor.
* Each test step initialises its own name.
*
*/
CTestMmfDRMAudioRecPlayUniqueIdFileSource::CTestMmfDRMAudioRecPlayUniqueIdFileSource()
    {
    iTestStepName = _L("MM-MMF-ACLNT-U-0915-HP");
    }

/**
*
* Do the test step.
* Each test step must supply an implementation for DoTestStepL.
*
* @return TVerdict: The result of the test step.
*
*/
TVerdict CTestMmfDRMAudioRecPlayUniqueIdFileSource::DoTestStepL()
    {
    INFO_PRINTF1(_L("Open and play a protected audio clip from a file for PLAY intent with Unique Id. (TMMFileSource)"));
    iTestStepResult = EFail;
    // Initialise the state variables
    StateInit();

    TRAPD(err, FsmL(EIdleRecorder, CMdaAudioClipUtility::ENotReady)); // Call to open the audio file
    if (err == KErrNone)
    	{
	    CActiveScheduler::Start(); // ActiveScheduler started ONLY once
	   	}

    return iTestStepResult;
    }



/**
*
* Ctor for CTestMmfDRMAudioRecPlayAllocFileHandleSource
*
* @return CTestMmfDRMAudioRecPlayAllocFileHandleSource*: 
*                    constructed CTestMmfDRMAudioRecPlayAllocFileHandleSource object
*
*/
CTestMmfDRMAudioRecPlayAllocFileHandleSource* CTestMmfDRMAudioRecPlayAllocFileHandleSource::NewL()
    {
    return new (ELeave) CTestMmfDRMAudioRecPlayAllocFileHandleSource;
    }

/**
*
* Test step constructor.
* Each test step initialises its own name.
*
*/
CTestMmfDRMAudioRecPlayAllocFileHandleSource::CTestMmfDRMAudioRecPlayAllocFileHandleSource()
    {
    iTestStepName = _L("MM-MMF-ACLNT-U-0916-HP");
    }

/**
*
* Implementation of the MoscoStateChangeEvent interface function.
*
* @param TInt aErrorCode
*
*/
void CTestMmfDRMAudioRecPlayAllocFileHandleSource::MoscoStateChangeEvent(CBase* aObject, 
    TInt aPreviousState, TInt aCurrentState, TInt aErrorCode)
    {
    iObject = aObject;
    iPreviousState = aPreviousState;
    iCurrentState = aCurrentState;
    iError = aErrorCode;

	if (iError != KErrNone)
    	{
    	ERR_PRINTF1_IF_LOGS_ENABLED(_L("AudioRecPlay failed."));
    	iAudioRecorder->Close();
    	CActiveScheduler::Stop();
    	}
    else
    	{
    	INFO_PRINTF1_IF_LOGS_ENABLED(_L("MoscoStateChangeEvent."));
    	INFO_PRINTF4_IF_LOGS_ENABLED(_L("Error: %d, PreviousState: %d, CurrentState: %d"), iError, iPreviousState, iCurrentState);	    	
    	TRAP(iError, FsmHandleL(EMoscoStateChange, iAudioRecorder->State(), aErrorCode));
    	}
    }

/**
*
* Play audio by marking heap.
*
*/
void CTestMmfDRMAudioRecPlayAllocFileHandleSource::AudioRecPlayAllocL()
    {
    __MM_HEAP_MARK;

	INFO_PRINTF1_IF_LOGS_ENABLED(_L("Open and play a protected audio clip from a file for PLAY intent. (TMMFileHandleSource)"));

    User::LeaveIfError(iFs.Connect());
    User::LeaveIfError(iFs.ShareProtected());
    User::LeaveIfError(iFile.Open(iFs, iFileName, EFileRead | EFileShareReadersOnly));


    // Initialise the state variables
    StateInit();

    TRAPD(err, FsmHandleL(EIdleRecorder, CMdaAudioClipUtility::ENotReady)); // Call to open the audio file
    if (err == KErrNone)
    	{
    	CActiveScheduler::Start(); // ActiveScheduler started ONLY once
    	}
   
	iAudioRecorder->Close();
   
    iFile.Close();
    iFs.Close();
    
    __MM_HEAP_MARKEND;
    }

/**
*
* Do the test step.
* Each test step must supply an implementation for DoTestStepL.
*
* @return TVerdict: The result of the test step.
*
*/
TVerdict CTestMmfDRMAudioRecPlayAllocFileHandleSource::DoTestStepL()
    {
    INFO_PRINTF1(_L("Alloc test to open and play a protected audio clip from a file for PLAY intent. (TMMFileHandleSource)"));

    TRAP(iError, AudioRecPlayAllocL());
    if (iError != KErrNone)
        {
        INFO_PRINTF2(_L("Opening/playing the audio file failed, unexpected error %d."), iError);
        return EFail;
        }

    iTestStepResult  = EPass;

    TInt failCount   = 1;
    TBool completed  = EFalse;
    TBool badResult  = EFalse;
    TBool reachedEnd = EFalse;

	EnableLogs(EFalse);
    for(;;)
        {
        __UHEAP_SETFAIL(RHeap::EFailNext, failCount);
        __MM_HEAP_MARK;

        TRAP(iError, AudioRecPlayAllocL());

        completed = EFalse;
        if (iError == KErrNone)
            {
            TAny *testAlloc = User::Alloc(1); // when this fails, we passed through all allocs within test
            if (testAlloc == NULL)
                {
                reachedEnd = ETrue;
                failCount--;
                }
            else
                {
                User::Free(testAlloc);
                testAlloc = NULL;
                }

            if (iTestStepResult != EPass)
                {
                badResult = ETrue;
                }

            completed = reachedEnd || badResult;
            }
        else if (iError != KErrNoMemory)
            {
            __UHEAP_SETFAIL(RHeap::ENone, 0);
            ERR_PRINTF2(_L("Bad error code %d"), iError);
            completed = ETrue;
            badResult = EFail;
            }

        __MM_HEAP_MARKEND;
        __UHEAP_SETFAIL(RHeap::ENone, 0);

        if (completed)
            {
            break;
            }

        failCount++;
        }

    if (iError != KErrNone || badResult)
        {
        if (badResult)
            {
            INFO_PRINTF3(_L("Bad result with %d memory allocations tested. The Error returned is %d."), failCount, iError);
            }
        else
            {
            INFO_PRINTF3(_L("Error(%d) with %d memory allocations tested."), failCount, iError);
            }

        iTestStepResult = EFail;
        }
    else
        {
        INFO_PRINTF2(_L("Completed OK with %d memory allocations tested.\n"), failCount);
        iTestStepResult = EPass;
        }

    return iTestStepResult;
    }



// Negative Tests

/**
*
* Ctor for CTestMmfDRMAudioRecOpenUnknownFileSource
*
* @return CTestMmfDRMAudioRecOpenUnknownFileSource*: 
*                           constructed CTestMmfDRMAudioRecOpenUnknownFileSource object
*
*/
CTestMmfDRMAudioRecOpenUnknownFileSource* CTestMmfDRMAudioRecOpenUnknownFileSource::NewL()
    {
    return new (ELeave) CTestMmfDRMAudioRecOpenUnknownFileSource;
    }

/**
*
* Test step constructor.
* Each test step initialises its own name.
*
*/
CTestMmfDRMAudioRecOpenUnknownFileSource::CTestMmfDRMAudioRecOpenUnknownFileSource()
    {
    iTestStepName = _L("MM-MMF-ACLNT-U-0931-HP");
    }

/**
*
* Implementation of the MoscoStateChangeEvent interface function.
*
* @param TInt aErrorCode
*
*/
void CTestMmfDRMAudioRecOpenUnknownFileSource::MoscoStateChangeEvent(CBase* /*aObject*/, 
   TInt /*aPreviousState*/, TInt /*aCurrentState*/, TInt aErrorCode)
    {
    INFO_PRINTF1(_L("MoscoStateChangeEvent."));
    iError = aErrorCode;
    CActiveScheduler::Stop();
    }

/**
*
* Do the test step.
* Each test step must supply an implementation for DoTestStepL.
*
* @return TVerdict: The result of the test step.
*
*/
TVerdict CTestMmfDRMAudioRecOpenUnknownFileSource::DoTestStepL()
    {
    INFO_PRINTF1(_L("Open a protected audio clip from a file with rights for UNKNOWN intent. (TMMFileSource)"));

    iTestStepResult = EFail;

    INFO_PRINTF2(_L("Opening the file for PLAY intent: %S"), &iFileName);
    iAudioRecorder->OpenFileL(TMMFileSource(iFileName, KRightsRecUniqueId001, ContentAccess::EUnknown));

    CActiveScheduler::Start();

    if (iError == KErrCANoPermission)
        {
        INFO_PRINTF1(_L("No Permission to open the audio file for Unknown intent!"));
        iTestStepResult = EPass;
        }
    else
        {
        INFO_PRINTF3(_L("Unexpected error %d, expected %d."), iError, KErrCANoPermission);
        }

    return iTestStepResult;
    }



/**
*
* Ctor for CTestMmfDRMAudioRecPlayNoRightsFileSource
*
* @return CTestMmfDRMAudioRecPlayNoRightsFileSource*: 
*                       constructed CTestMmfDRMAudioRecPlayNoRightsFileSource object
*
*/
CTestMmfDRMAudioRecPlayNoRightsFileSource* CTestMmfDRMAudioRecPlayNoRightsFileSource::NewL()
    {
    return new (ELeave) CTestMmfDRMAudioRecPlayNoRightsFileSource;
    }

/**
*
* Test step constructor.
* Each test step initialises its own name.
*
*/
CTestMmfDRMAudioRecPlayNoRightsFileSource::CTestMmfDRMAudioRecPlayNoRightsFileSource()
    {
    iTestStepName = _L("MM-MMF-ACLNT-U-0932-HP");
    }

/**
*
* Implementation of the MoscoStateChangeEvent interface function.
*
* @param TInt aErrorCode
*
*/
void CTestMmfDRMAudioRecPlayNoRightsFileSource::MoscoStateChangeEvent(CBase* /*aObject*/, 
    TInt /*aPreviousState*/, TInt /*aCurrentState*/, TInt aErrorCode)
    {
    INFO_PRINTF1(_L("MoscoStateChangeEvent."));
    iError = aErrorCode;
    CActiveScheduler::Stop();
    }

/**
*
* Do the test step.
* Each test step must supply an implementation for DoTestStepL.
*
* @return TVerdict: The result of the test step.
*
*/
TVerdict CTestMmfDRMAudioRecPlayNoRightsFileSource::DoTestStepL()
    {
    INFO_PRINTF1(_L("Open a protected audio clip from a file for PLAY intent with NO rights. (TMMFileSource)."));

    iTestStepResult = EFail;

    // Get the filename
    // iFileName will be the content file, containing audio files
    if (!GetStringFromConfig(_L("SectionDRM"), _L("AudioContentNoRights"), iFileName))
        {
        return EInconclusive;
        }

    INFO_PRINTF2(_L("Opening the file for PLAY intent: %S"), &iFileName);
    iAudioRecorder->OpenFileL(TMMFileSource(iFileName, KRightsRecUniqueId003, ContentAccess::EPlay));

    CActiveScheduler::Start();

    if (iError == KErrCANoRights)
        {
        INFO_PRINTF1(_L("Expected failure: No rights."));
        iTestStepResult = EPass;
        }
    else
        {
        INFO_PRINTF3(_L("Unexpected error %d, expected %d."), iError, KErrCANoRights);
        }

    return iTestStepResult;
    }



/**
*
* Ctor for CTestMmfDRMAudioRecPlayInvalidIdFileSource
*
* @return CTestMmfDRMAudioRecPlayInvalidIdFileSource*: 
*                           constructed CTestMmfDRMAudioRecPlayInvalidIdFileSource object
*
*/
CTestMmfDRMAudioRecPlayInvalidIdFileSource* CTestMmfDRMAudioRecPlayInvalidIdFileSource::NewL()
    {
    return new (ELeave) CTestMmfDRMAudioRecPlayInvalidIdFileSource;
    }

/**
*
* Test step constructor.
* Each test step initialises its own name.
*
*/
CTestMmfDRMAudioRecPlayInvalidIdFileSource::CTestMmfDRMAudioRecPlayInvalidIdFileSource()
    {
    iTestStepName = _L("MM-MMF-ACLNT-U-0933-HP");
    }

/**
*
* Implementation of the MoscoStateChangeEvent interface function.
*
* @param TInt aErrorCode
*
*/
void CTestMmfDRMAudioRecPlayInvalidIdFileSource::MoscoStateChangeEvent(CBase* /*aObject*/, 
    TInt /*aPreviousState*/, TInt /*aCurrentState*/, TInt aErrorCode)
    {
    INFO_PRINTF1(_L("MoscoStateChangeEvent."));
    iError = aErrorCode;
    CActiveScheduler::Stop();
    }

/**
*
* Do the test step.
* Each test step must supply an implementation for DoTestStepL.
*
* @return TVerdict: The result of the test step.
*
*/
TVerdict CTestMmfDRMAudioRecPlayInvalidIdFileSource::DoTestStepL()
    {
    INFO_PRINTF1(_L("Open a protected audio clip from a file for PLAY intent with INVALID Unique Id. (TMMFileSource)."));

    iTestStepResult = EFail;

    INFO_PRINTF2(_L("Opening the file for PLAY intent: %S"), &iFileName);
    iAudioRecorder->OpenFileL(TMMFileSource(iFileName, KRightsRecInvalidUniqueId, ContentAccess::EPlay));

    CActiveScheduler::Start();

    if (iError == KErrNotFound)
        {
        INFO_PRINTF1(_L("Expected failure: Audio file not found!"));
        iTestStepResult = EPass;
        }
    else
        {
        INFO_PRINTF3(_L("Unexpected error %d, expected %d."), iError, KErrNotFound);
        }

    return iTestStepResult;
    }
