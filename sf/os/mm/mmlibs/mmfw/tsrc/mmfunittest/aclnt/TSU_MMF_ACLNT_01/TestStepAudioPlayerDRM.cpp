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

#include "TestStepAudioPlayerDRM.h"


// Unique Id of the content
_LIT(KRightsPlayerUniqueId001, "AudioTestmainTstId001");
_LIT(KRightsPlayerUniqueId002, "\\Container001\\AudioTestmainTst2Id002");
_LIT(KRightsPlayerUniqueId003, "AudioTestmainTst3Id003");

_LIT(KRightsPlayerUniqueId004, "AudioTestNoRightsId001");
_LIT(KRightsPlayerInvalidUniqueId, "AudioTestInvalidUniqueId");

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
													
// Used in a time out function, 1 second (in microSeconds)
const TInt KOneSecond = 1000000;

// CTestMmfDRMAudioPlayerBase implementation
/**
*
* CTestMmfDRMAudioPlayerBase constructor.
*
*/
CTestMmfDRMAudioPlayerBase::CTestMmfDRMAudioPlayerBase()
	{
	iEnableLogs = ETrue;//By deafult log the messages    
	}
/**
*
* FsmL()
*
* @param TMmfAudioPlayerEvents aEvent
*
*/
void CTestMmfDRMAudioPlayerBase::FsmL(TMmfAudioPlayerEvents aEvent)
    {
    iTestStepResult = EFail;
    switch (aEvent)
        {
        case EAudioOpen:
        	INFO_PRINTF2_IF_LOGS_ENABLED(_L("Opening the file for PLAY intent: %S"), &iFileName);
            if (iTestStepName == _L("MM-MMF-ACLNT-U-0905-HP"))
                {
                iAudioPlayer->OpenFileL(TMMFileSource(iFileName, KRightsPlayerUniqueId002, ContentAccess::EPlay));
                }
            else if (iTestStepName == _L("MM-MMF-ACLNT-U-0923-HP"))
                {
                iAudioPlayer->OpenFileL(TMMFileSource(iFileName, KRightsPlayerUniqueId003, ContentAccess::EPlay));
                }
            else
                {
                iAudioPlayer->OpenFileL(TMMFileSource(iFileName, KRightsPlayerUniqueId001, ContentAccess::EPlay));
                }
            break;

        case EAudioPlay:
            if (iTestStepName == _L("MM-MMF-ACLNT-U-0903-HP"))
                {
                INFO_PRINTF1_IF_LOGS_ENABLED(_L("Playing the audio file...for 2 seconds."));
                iAudioPlayer->Play();
                User::After(2000000);

				INFO_PRINTF1_IF_LOGS_ENABLED(_L("Pausing the audio file...for 1 second."));
                User::LeaveIfError(iAudioPlayer->Pause());
                User::After(1000000);

				INFO_PRINTF1_IF_LOGS_ENABLED(_L("Continuing the play of the audio file...for 2 seconds."));
               	iAudioPlayer->Play();
                User::After(2000000);
             
                }
            else if (iTestStepName == _L("MM-MMF-ACLNT-U-0907-HP"))
            	{
            	INFO_PRINTF1_IF_LOGS_ENABLED(_L("Playing of the audio file...for 2 seconds."));
               	iAudioPlayer->Play();
                User::After(2000000);
                
                INFO_PRINTF1_IF_LOGS_ENABLED(_L("Stop the play of the audio file...."));
             	iAudioPlayer->Stop();                         	
            	}
            else if (iTestStepName == _L("MM-MMF-ACLNT-U-0909-HP"))
            	{
            	for(TInt i=0; i<=50; i++)
                	{
                	INFO_PRINTF1(_L("Playing the audio file...for 0.05 second."));
                	iAudioPlayer->Play();
                	User::After(50000);

                	INFO_PRINTF1(_L("Pausing the audio file...for 0.05 second."));
                	User::LeaveIfError(iAudioPlayer->Pause());
                	User::After(50000);
                	}                        	
            	}

            else
                {
                INFO_PRINTF1_IF_LOGS_ENABLED(_L("Playing the audio file..."));
                iAudioPlayer->Play();
                }
            break;

        case EAudioPlayEnd:
        	INFO_PRINTF1_IF_LOGS_ENABLED(_L("Audio file opened and played."));
            iTestStepResult = EPass;
			iAudioPlayer->Close();
            CActiveScheduler::Stop();
            break;

        default:        	
            INFO_PRINTF1(_L("Invalid Audio event!"));
        }
        
      };
/**
*
* FsmHandleL()
*
* @param TMmfAudioPlayerEvents aEvent
*
*/
void CTestMmfDRMAudioPlayerBase::FsmHandleL(TMmfAudioPlayerEvents aEvent)
    {
    iTestStepResult = EFail;
    switch (aEvent)
        {
        case EAudioOpen:
        	INFO_PRINTF2_IF_LOGS_ENABLED(_L("Opening the file for PLAY intent: %S"), &iFileName);
            iAudioPlayer->OpenFileL(TMMFileHandleSource(iFile, KRightsPlayerUniqueId001, ContentAccess::EPlay));
            break;

        case EAudioPlay:
        	INFO_PRINTF1_IF_LOGS_ENABLED(_L("Playing the audio file..."));
            iAudioPlayer->Play();
            break;

        case EAudioPlayEnd:
        	INFO_PRINTF1_IF_LOGS_ENABLED(_L("Audio file opened and played."));
            iTestStepResult = EPass;
     		iAudioPlayer->Close();
            CActiveScheduler::Stop();
            break;

        default:
            INFO_PRINTF1(_L("Invalid Audio event!"));
        }
    };
/**
*
* EnableLogs()
*
* @param TBool aEnable
*
*/
inline void CTestMmfDRMAudioPlayerBase::EnableLogs(TBool aEnable)
{
	iEnableLogs = aEnable;	
}
/**
*
* Test step Preamble.
*
* @return enum TVerdict: The result of the test step.
* 
*/
enum TVerdict CTestMmfDRMAudioPlayerBase::DoTestStepPreambleL()
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

    // Create an audio player
    iAudioPlayer = CMdaAudioPlayerUtility::NewL(*this);
    if (iAudioPlayer == NULL)
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
enum TVerdict CTestMmfDRMAudioPlayerBase::DoTestStepPostambleL()
    {
  	delete iAudioPlayer;
    iAudioPlayer = NULL;
    User::After(KOneSecond); // wait for deletion to shut down devsound
    return CTestStepUnitMMFAudClient::DoTestStepPostambleL(); // Destroy the scheduler
    }

/**
*
* Implementation of the MMdaAudioPlayerCallback interface function.
*
* @param TInt aError
*
*/
void CTestMmfDRMAudioPlayerBase::MapcInitComplete(TInt aError, 
    const TTimeIntervalMicroSeconds& /*aDuration*/)
    {
    INFO_PRINTF2_IF_LOGS_ENABLED(_L("MMdaAudioPlayerCallback Init Complete. Error = %d"), aError);
    iError = aError;
    if (iError != KErrNone)
    	{
    	ERR_PRINTF1_IF_LOGS_ENABLED(_L("Opening the audio file failed."));
    	iAudioPlayer->Close();
    	CActiveScheduler::Stop();
    	}
    else
    	{
    	TRAP(iError, FsmL(EAudioPlay)); // Call to play audio
    	if (iError != KErrNone)
    		{
    		ERR_PRINTF2_IF_LOGS_ENABLED(_L("Got an error from the FsmL(EAudioPlay) %d."), iError);
    		iAudioPlayer->Close();
    		CActiveScheduler::Stop();    		
    		}
	    else
    		{
    		TRAP(iError, FsmL(EAudioPlayEnd)); 
    		if (iError != KErrNone)
   		 		{
   		 		ERR_PRINTF2_IF_LOGS_ENABLED(_L("Got an error from the FsmL(EAudioPlayEnd) %d."), iError);
    			}  	
    		}
    	}
    }

/**
*
* Implementation of the MMdaAudioPlayerCallback interface function.
*
* @param TInt aError
*
*/
void CTestMmfDRMAudioPlayerBase::MapcPlayComplete(TInt aError)
    {
    INFO_PRINTF2_IF_LOGS_ENABLED(_L("MMdaAudioPlayerCallback Play Complete. Error = %d"), aError);
    iError = aError;
    if (iError != KErrNone)
    	{
    	ERR_PRINTF2_IF_LOGS_ENABLED(_L("Playing the audio file failed, unexpected error %d."), aError);
    	iAudioPlayer->Close();
    	CActiveScheduler::Stop();
    	}
    else
    	{
    	TRAP(iError, FsmL(EAudioPlayEnd)); 
    	if (iError != KErrNone)
    		{
    		ERR_PRINTF2_IF_LOGS_ENABLED(_L("Got an error from the FsmL(EAudioPlayEnd) %d."), iError);
    		}    	
    	}
    }

// Positive Tests

/**
*
* Ctor for CTestMmfDRMAudioPlayFileSource
*
* @return CTestMmfDRMAudioPlayFileSource*: 
*                           constructed CTestMmfDRMAudioPlayFileSource object
*
*/
CTestMmfDRMAudioPlayFileSource* CTestMmfDRMAudioPlayFileSource::NewL()
    {
    return new (ELeave) CTestMmfDRMAudioPlayFileSource;
    }

/**
*
* Test step constructor.
* Each test step initialises its own name.
*
*/
CTestMmfDRMAudioPlayFileSource::CTestMmfDRMAudioPlayFileSource()
    {
    // Store the name of this test case
    // This is the name that is used by the script file
    iTestStepName = _L("MM-MMF-ACLNT-U-0901-HP");
    }

/**
*
* Do the test step.
* Each test step must supply an implementation for DoTestStepL.
*
* @return TVerdict: The result of the test step.
*
*/
TVerdict CTestMmfDRMAudioPlayFileSource::DoTestStepL()
    {
    INFO_PRINTF1(_L("Open and play a protected audio clip from a file for PLAY intent. (TMMFileSource)."));
    iTestStepResult = EFail;
    TRAPD(err, FsmL(EAudioOpen)); // Call to open the audio file
    if (err == KErrNone)
    	{
    	CActiveScheduler::Start(); // ActiveScheduler started ONLY once
    	}
    return iTestStepResult;
    }



/**
*
* Ctor for CTestMmfDRMAudioPlayFileHandleSource
*
* @return CTestMmfDRMAudioPlayFileHandleSource*: 
*                         constructed CTestMmfDRMAudioPlayFileHandleSource object
*
*/
CTestMmfDRMAudioPlayFileHandleSource* CTestMmfDRMAudioPlayFileHandleSource::NewL()
    {
    return new (ELeave) CTestMmfDRMAudioPlayFileHandleSource;
    }

/**
*
* Test step constructor.
* Each test step initialises its own name.
*
*/
CTestMmfDRMAudioPlayFileHandleSource::CTestMmfDRMAudioPlayFileHandleSource()
    {
    iTestStepName = _L("MM-MMF-ACLNT-U-0902-HP");
    }

/**
*
* Implementation of the MMdaAudioPlayerCallback interface function.
*
* @param TInt aError
*
*/
void CTestMmfDRMAudioPlayFileHandleSource::MapcInitComplete(TInt aError, 
    const TTimeIntervalMicroSeconds& /*aDuration*/)
    {
    INFO_PRINTF2(_L("MMdaAudioPlayerCallback Init Complete. Error = %d"), aError);
    iError = aError;
	if (iError != KErrNone)
    	{
    	ERR_PRINTF2(_L("Opening the audio file failed, unexpected error %d."), aError);
    	iAudioPlayer->Close();
    	CActiveScheduler::Stop();
    	}
    else
    	{
    	TRAP(iError, FsmHandleL(EAudioPlay)); // Call to play audio
    	}

    }

/**
*
* Implementation of the MMdaAudioPlayerCallback interface function.
*
* @param TInt aError
*
*/
void CTestMmfDRMAudioPlayFileHandleSource::MapcPlayComplete(TInt aError)
    {
    INFO_PRINTF2(_L("MMdaAudioPlayerCallback Play Complete. Error = %d"), aError);
    iError = aError;
    if (iError != KErrNone)
    	{
    	ERR_PRINTF2(_L("Play complete returned unexpected error %d."), aError);
    	iAudioPlayer->Close();
    	CActiveScheduler::Stop();
    	}
    else
    	{
    	TRAP(iError, FsmHandleL(EAudioPlayEnd)); 
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
TVerdict CTestMmfDRMAudioPlayFileHandleSource::DoTestStepL()
    {
    INFO_PRINTF1(_L("Open and play a protected audio clip from a file for PLAY intent. (TMMFileHandleSource)."));
	iTestStepResult = EFail;
    User::LeaveIfError(iFs.Connect());
    User::LeaveIfError(iFs.ShareProtected());
    User::LeaveIfError(iFile.Open(iFs, iFileName, EFileRead));

    TRAPD(err, FsmHandleL(EAudioOpen)); // Call to open the audio file thru file handle
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
* Ctor for CTestMmfDRMAudioPPCSFileSource
*
* @return CTestMmfDRMAudioPPCSFileSource*: constructed CTestMmfDRMAudioPPCSFileSource object
*
*/
CTestMmfDRMAudioPPCSFileSource* CTestMmfDRMAudioPPCSFileSource::NewL()
    {
    return new (ELeave) CTestMmfDRMAudioPPCSFileSource;
    }

/**
*
* Test step constructor.
* Each test step initialises its own name.
*
*/
CTestMmfDRMAudioPPCSFileSource::CTestMmfDRMAudioPPCSFileSource()
    {
    iTestStepName = _L("MM-MMF-ACLNT-U-0903-HP");
    }

/**
*
* Do the test step.
* Each test step must supply an implementation for DoTestStepL.
*
* @return TVerdict: The result of the test step.
*
*/
TVerdict CTestMmfDRMAudioPPCSFileSource::DoTestStepL()
    {
    INFO_PRINTF1(_L("Open, play, pause, continue and stop the play of the protected audio clip from a file for PLAY/PAUSE/CONTINUE/STOP intents. (TMMFileSource)."));
	iTestStepResult = EFail;
    TRAPD(err,FsmL(EAudioOpen)); // Call to open the audio file
  	if (err == KErrNone)
    	{
    	CActiveScheduler::Start(); // ActiveScheduler started ONLY once
    	}
    else
    	{
    	iError = err;
    	CActiveScheduler::Stop();
    	}
       
    return iTestStepResult;
    }



/**
*
* Ctor for CTestMmfDRMAudioPlayAllocFileSource
*
* @return CTestMmfDRMAudioPlayAllocFileSource*: 
*                       constructed CTestMmfDRMAudioPlayAllocFileSource object
*
*/
CTestMmfDRMAudioPlayAllocFileSource* CTestMmfDRMAudioPlayAllocFileSource::NewL()
    {
    return new (ELeave) CTestMmfDRMAudioPlayAllocFileSource;
    }

/**
*
* Test step constructor.
* Each test step initialises its own name.
*
*/
CTestMmfDRMAudioPlayAllocFileSource::CTestMmfDRMAudioPlayAllocFileSource()
    {
    iTestStepName = _L("MM-MMF-ACLNT-U-0904-HP");
    }

/**
*
* Play audio by marking heap.
*
*/
void CTestMmfDRMAudioPlayAllocFileSource::AudioPlayAllocL()
    {
    __MM_HEAP_MARK;

    TRAPD(err, FsmL(EAudioOpen)); // Call to open the audio file
    if (err == KErrNone)
    	{
    	CActiveScheduler::Start(); // ActiveScheduler started ONLY once{
    	}
    	
	iAudioPlayer->Close();
		
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
TVerdict CTestMmfDRMAudioPlayAllocFileSource::DoTestStepL()
    {
    INFO_PRINTF1(_L("Alloc test to open and play a protected audio clip from a file for PLAY intent. (TMMFileSource)."));
	iTestStepResult = EFail;
    TRAP(iError, AudioPlayAllocL());
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

		TInt error = KErrNone;
        TRAP(error, AudioPlayAllocL());

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
* Ctor for CTestMmfDRMAudioPlayUniqueIdFileSource
*
* @return CTestMmfDRMAudioPlayUniqueIdFileSource*: 
*                          constructed CTestMmfDRMAudioPlayUniqueIdFileSource object
*
*/
CTestMmfDRMAudioPlayUniqueIdFileSource* CTestMmfDRMAudioPlayUniqueIdFileSource::NewL()
    {
    return new (ELeave) CTestMmfDRMAudioPlayUniqueIdFileSource;
    }

/**
*
* Test step constructor.
* Each test step initialises its own name.
*
*/
CTestMmfDRMAudioPlayUniqueIdFileSource::CTestMmfDRMAudioPlayUniqueIdFileSource()
    {
    iTestStepName = _L("MM-MMF-ACLNT-U-0905-HP");
    }

/**
*
* Do the test step.
* Each test step must supply an implementation for DoTestStepL.
*
* @return TVerdict: The result of the test step.
*
*/
TVerdict CTestMmfDRMAudioPlayUniqueIdFileSource::DoTestStepL()
    {
    INFO_PRINTF1(_L("Open and play a protected audio clip from a file for PLAY intent with Unique Id. (TMMFileSource)."));
	iTestStepResult = EFail;
    TRAPD(err, FsmL(EAudioOpen)); // Call to open the audio file
    if (err == KErrNone)
    	{
    	CActiveScheduler::Start(); // ActiveScheduler started ONLY once
    	}
    
    return iTestStepResult;
    }



/**
*
* Ctor for CTestMmfDRMAudioPlayAllocFileHandleSource
*
* @return CTestMmfDRMAudioPlayAllocFileHandleSource*: 
*                           constructed CTestMmfDRMAudioPlayAllocFileHandleSource object
*
*/
CTestMmfDRMAudioPlayAllocFileHandleSource* CTestMmfDRMAudioPlayAllocFileHandleSource::NewL()
    {
    return new (ELeave) CTestMmfDRMAudioPlayAllocFileHandleSource;
    }

/**
*
* Test step constructor.
* Each test step initialises its own name.
*
*/
CTestMmfDRMAudioPlayAllocFileHandleSource::CTestMmfDRMAudioPlayAllocFileHandleSource()
    {
    iTestStepName = _L("MM-MMF-ACLNT-U-0906-HP");
    }

/**
*
* Implementation of the MMdaAudioPlayerCallback interface function.
*
* @param TInt aError
*
*/
void CTestMmfDRMAudioPlayAllocFileHandleSource::MapcInitComplete(TInt aError, 
    const TTimeIntervalMicroSeconds& /*aDuration*/)
    {
    INFO_PRINTF2_IF_LOGS_ENABLED(_L("MMdaAudioPlayerCallback Init Complete. Error = %d"), aError);
    iError = aError;
    if (iError != KErrNone)
    	{
    	ERR_PRINTF1_IF_LOGS_ENABLED(_L("Opening the audio file failed."));
    	iAudioPlayer->Close();
       	CActiveScheduler::Stop();
    	}
    else
    	{
    	TRAP(iError, FsmHandleL(EAudioPlay)); // Call to play audio
    	}
	}

/**
*
* Implementation of the MMdaAudioPlayerCallback interface function.
*
* @param TInt aError
*
*/
void CTestMmfDRMAudioPlayAllocFileHandleSource::MapcPlayComplete(TInt aError)
    {
    INFO_PRINTF2_IF_LOGS_ENABLED(_L("MMdaAudioPlayerCallback Play Complete. Error = %d"), aError);
    iError = aError;
    if (iError != KErrNone)
    	{
    	ERR_PRINTF2_IF_LOGS_ENABLED(_L("Audio playback failed with unexpected error %d."), aError);
    	iAudioPlayer->Close();
      	CActiveScheduler::Stop();
    	}
    else
    	{
    	TRAP(iError, FsmHandleL(EAudioPlayEnd)); // end audio playback
    	}
	
    }

/**
*
* Play audio by marking heap.
*
*/
void CTestMmfDRMAudioPlayAllocFileHandleSource::AudioPlayAllocL()
    {
    __MM_HEAP_MARK;

    User::LeaveIfError(iFs.Connect());
    User::LeaveIfError(iFs.ShareProtected());
    User::LeaveIfError(iFile.Open(iFs, iFileName, EFileRead | EFileShareReadersOnly));

    TRAPD(err, FsmHandleL(EAudioOpen)); // Call to open the audio file thru file handle
    if (err == KErrNone)
    	{
    	CActiveScheduler::Start(); 
    	}
    
    iAudioPlayer->Close();
 
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
TVerdict CTestMmfDRMAudioPlayAllocFileHandleSource::DoTestStepL()
    {
    INFO_PRINTF1(_L("Alloc test to open and play a protected audio clip from a file for PLAY intent. (TMMFileHandleSource)."));
#ifndef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
	// As a part of DEF123331, this test has been disabled for non-A3F build.
	INFO_PRINTF1(_L("As a part of DEF123331, this test has been disabled for non-A3F build."));
	return EPass;
#else
	TRAPD(err, AudioPlayAllocL());
    if(err != KErrNone)
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
    TInt error = KErrNone;

	EnableLogs(EFalse);
    for(;;)
        {
        __UHEAP_SETFAIL(RHeap::EFailNext, failCount);
        __MM_HEAP_MARK;

	    TRAP(error, AudioPlayAllocL());
        if (error != KErrNone)
        	{
        	iError = error;
        	iFile.Close();
        	iFs.Close();
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
#endif    
    }



// Negative Tests

/**
*
* Ctor for CTestMmfDRMAudioOpenUnknownFileSource
*
* @return CTestMmfDRMAudioOpenUnknownFileSource*: 
*                           constructed CTestMmfDRMAudioOpenUnknownFileSource object
*
*/
CTestMmfDRMAudioOpenUnknownFileSource* CTestMmfDRMAudioOpenUnknownFileSource::NewL()
    {
    return new (ELeave) CTestMmfDRMAudioOpenUnknownFileSource;
    }

/**
*
* Test step constructor.
* Each test step initialises its own name.
*
*/
CTestMmfDRMAudioOpenUnknownFileSource::CTestMmfDRMAudioOpenUnknownFileSource()
    {
    iTestStepName = _L("MM-MMF-ACLNT-U-0921-HP");
    }

/**
*
* Implementation of the MMdaAudioPlayerCallback interface function.
*
* @param TInt aError
*
*/
void CTestMmfDRMAudioOpenUnknownFileSource::MapcInitComplete(TInt aError, 
    const TTimeIntervalMicroSeconds& /*aDuration*/)
    {
    INFO_PRINTF2(_L("MMdaAudioPlayerCallback Init Complete. Error = %d"), aError);
    iError = aError;
    CActiveScheduler::Stop();
    }

void CTestMmfDRMAudioOpenUnknownFileSource::MapcPlayComplete(TInt /*aError*/)
    {
    // Not used in this test.
    }

/**
*
* Do the test step.
* Each test step must supply an implementation for DoTestStepL.
*
* @return TVerdict: The result of the test step.
*
*/
TVerdict CTestMmfDRMAudioOpenUnknownFileSource::DoTestStepL()
    {
    INFO_PRINTF1(_L("Open a protected audio clip from a file with rights for UNKNOWN intent. (TMMFileSource)."));

    iTestStepResult = EFail;

    INFO_PRINTF2(_L("Opening the file for PLAY intent: %S"), &iFileName);
    iAudioPlayer->OpenFileL(TMMFileSource(iFileName, KRightsPlayerUniqueId001, ContentAccess::EUnknown));
	
	
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
* Ctor for CTestMmfDRMAudioPlayNoRightsFileSource
*
* @return CTestMmfDRMAudioPlayNoRightsFileSource*: 
*                           constructed CTestMmfDRMAudioPlayNoRightsFileSource object
*
*/
CTestMmfDRMAudioPlayNoRightsFileSource* CTestMmfDRMAudioPlayNoRightsFileSource::NewL()
    {
    return new (ELeave) CTestMmfDRMAudioPlayNoRightsFileSource;
    }

/**
*
* Test step constructor.
* Each test step initialises its own name.
*
*/
CTestMmfDRMAudioPlayNoRightsFileSource::CTestMmfDRMAudioPlayNoRightsFileSource()
    {
    iTestStepName = _L("MM-MMF-ACLNT-U-0922-HP");
    }

/**
*
* Implementation of the MMdaAudioPlayerCallback interface function.
*
* @param TInt aError
*
*/
void CTestMmfDRMAudioPlayNoRightsFileSource::MapcInitComplete(TInt aError, 
    const TTimeIntervalMicroSeconds& /*aDuration*/)
    {
    INFO_PRINTF2(_L("MMdaAudioPlayerCallback Init Complete. Error = %d"), aError);
    iError = aError;
    CActiveScheduler::Stop();
    }

void CTestMmfDRMAudioPlayNoRightsFileSource::MapcPlayComplete(TInt /*aError*/)
    {
    // Not used in this test.
    }

/**
*
* Do the test step.
* Each test step must supply an implementation for DoTestStepL.
*
* @return TVerdict: The result of the test step.
*
*/
TVerdict CTestMmfDRMAudioPlayNoRightsFileSource::DoTestStepL()
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
    iAudioPlayer->OpenFileL(TMMFileSource(iFileName, KRightsPlayerUniqueId004, ContentAccess::EPlay));

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
* Ctor for CTestMmfDRMAudioPlayRightsCountFileSource
*
* @return CTestMmfDRMAudioPlayRightsCountFileSource*: 
*                           constructed CTestMmfDRMAudioPlayRightsCountFileSource object
*
*/
CTestMmfDRMAudioPlayRightsCountFileSource* CTestMmfDRMAudioPlayRightsCountFileSource::NewL()
    {
    return new (ELeave) CTestMmfDRMAudioPlayRightsCountFileSource;
    }

/**
*
* Test step constructor.
* Each test step initialises its own name.
*
*/
CTestMmfDRMAudioPlayRightsCountFileSource::CTestMmfDRMAudioPlayRightsCountFileSource()
    {
    iTestStepName = _L("MM-MMF-ACLNT-U-0923-HP");
    }

/**
*
* Do the test step.
* Each test step must supply an implementation for DoTestStepL.
*
* @return TVerdict: The result of the test step.
*
*/
TVerdict CTestMmfDRMAudioPlayRightsCountFileSource::DoTestStepL()
    {
    INFO_PRINTF1(_L("Open and play a protected audio clip from a file for PLAY intent 4 times with rights count set to 3. (TMMFileSource)."));

    // Open and play the audio file for three times
    // The rights count for this protected audio file is set to 3.
    // Hence looped four times to check failure while opening for the fourth time.
    for (TInt rightscount = 1; rightscount <= 4; rightscount++)
        {
        iTestStepResult = EFail;
        INFO_PRINTF2(_L("Rights count = %d "), rightscount);

        TRAPD(err, FsmL(EAudioOpen)); // Call to open the audio file
        if (err == KErrNone)
    		{
    		CActiveScheduler::Start(); // ActiveScheduler started ONLY once 
    		} 

        if ((iError == KErrCANoPermission) && (rightscount == 4))
            {
            INFO_PRINTF1(_L("No Permission to open the audio file!"));
            iTestStepResult = EPass;
            }
        else if (iError != KErrNone)
            {
            ERR_PRINTF2(_L("Opening the audio file failed, unexpected error %d."), iError);
            return iTestStepResult;
            }
        }

    return iTestStepResult;
    }



/**
*
* Ctor for CTestMmfDRMAudioPlayInvalidIdFileSource
*
* @return CTestMmfDRMAudioPlayInvalidIdFileSource*: 
*                           constructed CTestMmfDRMAudioPlayInvalidIdFileSource object
*
*/
CTestMmfDRMAudioPlayInvalidIdFileSource* CTestMmfDRMAudioPlayInvalidIdFileSource::NewL()
    {
    return new (ELeave) CTestMmfDRMAudioPlayInvalidIdFileSource;
    }

/**
*
* Test step constructor.
* Each test step initialises its own name.
*
*/
CTestMmfDRMAudioPlayInvalidIdFileSource::CTestMmfDRMAudioPlayInvalidIdFileSource()
    {
    iTestStepName = _L("MM-MMF-ACLNT-U-0924-HP");
    }

/**
*
* Implementation of the MMdaAudioPlayerCallback interface function.
*
* @param TInt aError
*
*/
void CTestMmfDRMAudioPlayInvalidIdFileSource::MapcInitComplete(TInt aError, 
    const TTimeIntervalMicroSeconds& /*aDuration*/)
    {
    INFO_PRINTF2(_L("MMdaAudioPlayerCallback Init Complete. Error = %d"), aError);
    iError = aError;
    CActiveScheduler::Stop();
    }

void CTestMmfDRMAudioPlayInvalidIdFileSource::MapcPlayComplete(TInt /*aError*/)
    {
    // Not used in this test.
    }

/**
*
* Do the test step.
* Each test step must supply an implementation for DoTestStepL.
*
* @return TVerdict: The result of the test step.
*
*/
TVerdict CTestMmfDRMAudioPlayInvalidIdFileSource::DoTestStepL()
    {
    INFO_PRINTF1(_L("Open a protected audio clip from a file for PLAY intent with INVALID Unique Id. (TMMFileSource)."));

    iTestStepResult = EFail;

    INFO_PRINTF2(_L("Opening the file for PLAY intent: %S"), &iFileName);
    iAudioPlayer->OpenFileL(TMMFileSource(iFileName, KRightsPlayerInvalidUniqueId, ContentAccess::EPlay));

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
/**
*
* Ctor for CTestMmfDRMAudioPlayAndStopFileSource
*
* @return CTestMmfDRMAudioPlayAndStopFileSource*: constructed CTestMmfDRMAudioPlayAndStopFileSource object
*
*/
CTestMmfDRMAudioPlayAndStopFileSource* CTestMmfDRMAudioPlayAndStopFileSource::NewL()
    {
    return new (ELeave) CTestMmfDRMAudioPlayAndStopFileSource;
    }

/**
*
* Test step constructor.
* Each test step initialises its own name.
*
*/
CTestMmfDRMAudioPlayAndStopFileSource::CTestMmfDRMAudioPlayAndStopFileSource()
    {
    iTestStepName = _L("MM-MMF-ACLNT-U-0907-HP");
    }

/**
*
* Do the test step.
* Each test step must supply an implementation for DoTestStepL.
*
* @return TVerdict: The result of the test step.
*
*/
TVerdict CTestMmfDRMAudioPlayAndStopFileSource::DoTestStepL()
    {
    INFO_PRINTF1(_L("Open, play, and stop the play of the protected audio clip from a file for PLAY/STOP intents. (TMMFileSource)."));
	iTestStepResult = EFail;
    TRAPD(err,FsmL(EAudioOpen)); // Call to open the audio file
  	if (err == KErrNone)
    	{
    	CActiveScheduler::Start(); // ActiveScheduler started ONLY once
    	}
    else
    	{
    	iError = err;
    	CActiveScheduler::Stop();
    	}
       
    return iTestStepResult;
    }
CTestMmfDRMAudioPPCFFileSource* CTestMmfDRMAudioPPCFFileSource::NewL()
{
return new (ELeave) CTestMmfDRMAudioPPCFFileSource;
}

/**
*
* Test step constructor.
* Each test step initialises its own name.
*
*/
CTestMmfDRMAudioPPCFFileSource::CTestMmfDRMAudioPPCFFileSource()
{
iTestStepName = _L("MM-MMF-ACLNT-U-0909-HP");
}

/**
*
* Do the test step.
* Each test step must supply an implementation for DoTestStepL.
*
* @return TVerdict: The result of the test step.
*
*/
TVerdict CTestMmfDRMAudioPPCFFileSource::DoTestStepL()
{
INFO_PRINTF1(_L("Open a protected audio clip, switch the state between play and pause very quickly (0.05 second each) for PLAY/PAUSE/CONTINUE/STOP intents to test INC115501. (TMMFileSource)."));
 iTestStepResult = EFail;
TRAPD(err,FsmL(EAudioOpen)); // Call to open the audio file

     
if (err == KErrNone)
	{
	CActiveScheduler::Start(); // ActiveScheduler started ONLY once
	}
else
	{
	iError = err;
	CActiveScheduler::Stop();
	}
   
return iTestStepResult;
}

