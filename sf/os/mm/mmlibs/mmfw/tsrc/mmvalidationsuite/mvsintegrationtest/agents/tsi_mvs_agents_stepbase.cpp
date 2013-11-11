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
//

#include "tsi_mvs_agents_stepbase.h"
#include <e32const.h>
#include <hal.h>

_LIT(KIniControllerConfigKeyName, "UseNonGceController");

const TUid KMmfTestAviNonGcePlayControllerUid = { 0x10283420 };
const TUint KCacheSize = 1000000; //1MB
const TUint KMaxCacheSize = 40000000; //40MB

//
// CTestMVSCallbackSupport
//

/**
 * NewL
 */
CTestMVSCallbackSupport* CTestMVSCallbackSupport::NewL(MTestMVSClientObserver& aCallback)
    {
    return new(ELeave) CTestMVSCallbackSupport(aCallback);
    }

/**
 * UpdateStateChange
 */
void CTestMVSCallbackSupport::UpdateStateChange(TMVSState aState, TInt aErrorCode)
    {
    iCallback.UpdateStateChangeTest(aState, aErrorCode, *this);
    }
/**
 * MvsResourceNotification
 */
void CTestMVSCallbackSupport::MvsResourceNotification(const TDesC8& aNotificationData)
    {
    iCallback.MvsResourceNotificationTest(aNotificationData, *this);
    }
/**
 * Constructor
 */
CTestMVSCallbackSupport::CTestMVSCallbackSupport(MTestMVSClientObserver& aCallback):iCallback(aCallback)
    {

    }

//
// CStopActiveSchedulerTimer
//

/**
 * Class used by the test harness to stop the active scheduler 
 * after a timeout period
 */
CStopActiveSchedulerTimer* CStopActiveSchedulerTimer::NewL()
    {
    CStopActiveSchedulerTimer* self = new(ELeave)CStopActiveSchedulerTimer();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();    
    return self;
    }

/**
 *
 * CStopActiveSchedulerTimer::ConstructL()
 *
 */    
void CStopActiveSchedulerTimer::ConstructL()
    {
    CTimer::ConstructL();
    CActiveScheduler::Add(this);
    }

/**
 *
 * CStopActiveSchedulerTimer::CStopActiveSchedulerTimer()
 *
 */    
CStopActiveSchedulerTimer::CStopActiveSchedulerTimer() : CTimer(EPriorityHigh)
    {
    }

/**
 *
 * CStopActiveSchedulerTimer::StopAfter
 *
 */    
void CStopActiveSchedulerTimer::StopAfter(TTimeIntervalMicroSeconds32 aTimeInterval)
    {
    Cancel();
    After(aTimeInterval);    
    }

/**
 *
 * CStopActiveSchedulerTimer::RunL()
 *
 */    
void CStopActiveSchedulerTimer::RunL()
    {
    if (CActiveScheduler::Current()->StackDepth())
        {
        CActiveScheduler::Stop();
        }
    }

//
// RTestStepMVSBase
//

/**
 *
 * RTestStepMVSBase
 * Base Test step constructor
 *
 */
RTestStepMVSBase::RTestStepMVSBase(const TDesC& aTestName, const TDesC& aSectName,
                                     const TDesC& aKeyName) 
    {
    iTestStepName = aTestName;
    iSectName = aSectName;
    iKeyName = aKeyName;
    }

/**
 *
 * SetTestSuite
 *
 */
void RTestStepMVSBase::SetTestSuite(const CTestSuiteMVSAgent* aTestSuite)
    {
    iMVSAgentTestSuite = aTestSuite;
    }

/**
 *
 * DoTestStepPreamble
 *
 */
TVerdict RTestStepMVSBase::DoTestStepPreambleL()
    {    
    TVerdict preamRes;
    preamRes = EPass;
    
    // Install the Active Scheduler
    iAS = new (ELeave) CActiveScheduler;
    CActiveScheduler::Install(iAS);
    
    // Get the filename
    TPtrC tmpFilename;
    if(!GetStringFromConfig(iSectName, iKeyName, tmpFilename))
        {
        ERR_PRINTF1(_L("ERROR : Cannot Get value from Config."));
        return EInconclusive;
        }
    GetDriveName(iFilename);
    iFilename.Append(tmpFilename);
    
    // Create the directory for the MVS Agents to Log
    MakeDir(KMVSAgentsLogPath);
    
    // Delete Log
    DeleteFileL(KMVSAgentsLogPath);
    
    // Create the MVS Plugin Info Class
    iMVSControllerPluginInfo = CMVSControllerPluginInfo::NewL();
    if(iMVSControllerPluginInfo == NULL)
        {
        ERR_PRINTF1(_L("ERROR : iMVSControllerPluginInfo cannot be created."));
        return EInconclusive;
        }
    
    return preamRes;
    }    


/**
 *
 * DoTestStepPostambleL
 *
 */
TVerdict RTestStepMVSBase::DoTestStepPostambleL()
    {
    TVerdict postRes;
    postRes = EPass;

    if (iMVSControllerPluginInfo)
        {
        delete iMVSControllerPluginInfo;
        iMVSControllerPluginInfo = NULL;
        }

    if (iAS)
        {
        delete iAS;
        iAS = NULL;
        }

    return postRes;
    }

/**
 *
 * GetDriveName()
 * Method to get the drive name where the data files are present
 *
 */
void RTestStepMVSBase::GetDriveName(TDes& aFileName)
{
#ifdef __WINS__
    aFileName = _L("c:");
#elif defined (__MARM__) || defined (__X86GCC__)
    #ifdef TEST_FILES_ON_C_DRIVE
        aFileName = _L("c:");
    #else
        aFileName = _L("z:");
    #endif // TEST_FILES_ON_C_DRIVE
#endif
}


/**
 *
 * MakeDir
 *
 */
TInt RTestStepMVSBase::MakeDir(const TDesC &aDirName)
  {
  RFs fs;
  TInt err;
  err = fs.Connect();
  if(err != KErrNone)
        return err;
  fs.MkDirAll(aDirName);
  fs.Close();
  return err;
  }

/**
 *
 * DeleteFile
 *
 */
TInt RTestStepMVSBase::DeleteFileL(const TDesC &aFileName)
    {
    // Errors ignored while deleting a file...
    RFs fs;
    TInt err = fs.Connect();
    if(err != KErrNone)
        return err;
    CleanupClosePushL(fs);
    err = fs.Delete(aFileName);
    CleanupStack::PopAndDestroy();
    INFO_PRINTF3(_L("DeleteFileL(%S) returned : %d"), &aFileName ,err);
    return err;
    }

/**
 *
 * CheckLogFileL
 *
 */
TBool RTestStepMVSBase::CheckLogFileL(const TDesC& aOrigLogFile, const TDesC8 &aCheckString8, const TDesC &aCheckString)
    {
    TInt returnCode = 0;
    TBool result = ETrue;
    
    // Create File Session
    RFs fileSystem;
    returnCode=fileSystem.Connect();
    if (returnCode != KErrNone)
        {
        ERR_PRINTF2(_L("LOG-CHECK=>Failed to connect to file server, error %d"), returnCode);
        fileSystem.Close();
        return EFalse;
        }
    CleanupClosePushL(fileSystem);

    // read the original log file
    RFile theFile;
    returnCode = theFile.Open(fileSystem, aOrigLogFile, EFileRead | EFileStreamText | EFileShareAny);
    if (returnCode != KErrNone)
        {
        ERR_PRINTF3(_L("LOG-CHECK=>Cannot Open the Log file '%S', error : %d"), &aOrigLogFile, returnCode);
        theFile.Close();
        return EFalse;
        }
    CleanupClosePushL(theFile);
    
    // Check the size
    TInt fileSize;
    returnCode = theFile.Size(fileSize);
    if (returnCode != KErrNone)
        {
        ERR_PRINTF3(_L("LOG-CHECK=>Cannot Get file size for '%S', error : %d"), &aOrigLogFile, returnCode);
        return EFalse;
        }
    if (fileSize < 10) // Arbitrary number - File size expected to be more than 10 bytes.
        {
        ERR_PRINTF2(_L("LOG-CHECK=>Size of the File Size is less than 10 Bytes '%S'"), &aOrigLogFile);
        return EFalse;
        }
    
    // Read the file into a descriptor
    TPtr8 theString(REINTERPRET_CAST(TUint8*, User::AllocLC(fileSize)), 0, fileSize);
    theFile.Read(theString, fileSize);
    
    // Get the last modified date of the logfile and compare with the current time.
    // Should not be any difference, and even if there is should only be milliseconds.
    TTime modifiedTime;
    returnCode = theFile.Modified(modifiedTime);
    if (returnCode != KErrNone)
        {
        ERR_PRINTF3(_L("LOG-CHECK=>Cannot Get Last Modified Date for '%S', error : %d"), &aOrigLogFile, returnCode);
        return EFalse;
        }
    TTime now;
    now.HomeTime();
    modifiedTime.HomeTime();
    TUint testDuration = I64INT(now.MicroSecondsFrom(modifiedTime).Int64());
    testDuration /= 1000; // to microseconds
    if (testDuration > 50)
        {
        ERR_PRINTF2(_L("LOG-CHECK=>Difference between current-time and last-modified-time = 0.%03d sec"), testDuration % 1000);
        result = EFalse;    
        }
    else
        {
        INFO_PRINTF2(_L("LOG-CHECK=>Difference between current-time and last-modified-time = 0.%03d sec"), testDuration % 1000);
        }  
    // see if our sample test string is present. if it is, we've passed.
    returnCode = theString.Find(aCheckString8);
    if(returnCode == KErrNotFound)
        {
        ERR_PRINTF3(_L("LOG-CHECK=>Test string '%S' NOT found in log file '%S'"), &aCheckString, &aOrigLogFile);
        result = EFalse;
        }
    else
        {
        INFO_PRINTF3(_L("LOG-CHECK=>Test string '%S' found in log file '%S'"), &aCheckString, &aOrigLogFile);
        }

    // close file logger session
    theFile.Close();
    fileSystem.Close();
    CleanupStack::PopAndDestroy(3);    // theString, fileSystem, theFile
    return result; // should be ETrue if we've got here
    }
    
//
// RTestStepMVSAgent
//

/**
 *
 * RTestStepMVSAgent
 * Test step constructor
 *
 */
RTestStepMVSAgent::RTestStepMVSAgent(const TDesC& aTestName, const TDesC& aSectName,
                                     const TDesC& aKeyName) 
    : RTestStepMVSBase(aTestName, aSectName, aKeyName)
    {}

/**
 *
 * DoTestStepPreamble
 *
 */
TVerdict RTestStepMVSAgent::DoTestStepPreambleL()
    {    
    TVerdict preamRes;
    preamRes = EPass;
    
    TInt err = KErrNone;
    
    // Call base DoTestStepPreambleL()
    preamRes = RTestStepMVSBase::DoTestStepPreambleL();
    if (preamRes != EPass)
        {
        return preamRes;
        }

    if (err) 
        {
        ERR_PRINTF1(_L("ERROR : Cannot create CTimer"));
        return EInconclusive;
        }
    
    // Initialise Callback handlers
    iAudioPlayAgentCallback = CTestMVSCallbackSupport::NewL(*this);
    if (iAudioPlayAgentCallback == NULL)
        {
        INFO_PRINTF1(_L("Error during iAudioPlayAgentCallback "));
        }
        
    iAudioRecordAgentCallback = CTestMVSCallbackSupport::NewL(*this);
    if (iAudioRecordAgentCallback == NULL)
        {
        INFO_PRINTF1(_L("Error during iAudioRecordAgentCallback "));
        }
    iVideoPlayAgentCallback = CTestMVSCallbackSupport::NewL(*this);
    if (iVideoPlayAgentCallback == NULL)
        {
        INFO_PRINTF1(_L("Error during iVideoPlayAgentCallback "));
        }
        
    iVideoRecordAgentCallback = CTestMVSCallbackSupport::NewL(*this);
    if (iVideoRecordAgentCallback == NULL)
        {
        INFO_PRINTF1(_L("Error during iVideoRecordAgentCallback "));
        }
    
    // Initialise the State variables
    ResetState();    
    return preamRes;
    }    


/**
 *
 * DoTestStepPostambleL
 *
 */
TVerdict RTestStepMVSAgent::DoTestStepPostambleL()
    {
    // Base Postamble.
    return RTestStepMVSBase::DoTestStepPostambleL();
    }

/**
 *
 * UpdateStateChangeTest
 *
 */
void RTestStepMVSAgent::UpdateStateChangeTest(TMVSState aState, TInt aErrorCode, CTestMVSCallbackSupport& aTestMVSCallbackSupport)
    {
    // Update local variables
    iPreviousState = iCallbackState;
    iCallbackState = aState;
    iError = aErrorCode;
    
    // Get the agent the event was generated from...
    if (&aTestMVSCallbackSupport == iAudioPlayAgentCallback)
        {
        iCallbackAgent = ETestAudioPlay;
        }
    else if (&aTestMVSCallbackSupport == iAudioRecordAgentCallback)
        {
        iCallbackAgent = ETestAudioRecord;
        }
    else if (&aTestMVSCallbackSupport == iVideoPlayAgentCallback)
        {
        iCallbackAgent = ETestVideoPlay;
        }
    else if (&aTestMVSCallbackSupport == iVideoRecordAgentCallback)
        {
        iCallbackAgent = ETestVideoRecord;
        }
                
    // Check for the expected agent, state and error
    if (iCallbackAgent != iActiveAgent)
        {
        ERR_PRINTF1(_L("The UpdateStateChange event arised from the wrong agent."));
        iTestStepResult = EFail;
        CActiveScheduler::Stop();
        }
    else if (iExpectedError != iError)
        {
        // DEF127335
		// OggRecordController does not support recording into an already existing file.
		// This is due to unavailability of APIs at the level of Ogg Vorbis C libraries.
		if (iExpectedState == EAudioRecording && iError == KErrNotSupported && (iTestStepName == _L("MM-MVS-AUDIOAGENT-I-0001-HP") || iTestStepName == _L("MM-MVS-AUDIOAGENT-I-0003-HP")))
			{
			INFO_PRINTF1(_L("Expected Result: OggRecordController returned KErrNotSupported(-5)"));
			iTestStepResult = EPass;
			CActiveScheduler::Stop();
			}
		// DEF136957  
		// The navi enigne does not support recording at  48 k, this is a known error.
		else if (iExpectedState == EAudioRecording && iError == KErrNotFound && (iTestStepName == _L("MM-MVS-AUDIOAGENT-I-0007-HP") || iTestStepName == _L("MM-MVS-AUDIOAGENT-I-0003-HP")))
			{
			TInt machineType;
			TInt err = HAL::Get(HALData::EMachineUid, machineType);
			if (err)
				{
				INFO_PRINTF1(_L("Error Getting Device information"));
				ERR_PRINTF3(_L("ExpectedState : %d, ActualState : %d"), iExpectedState, iCallbackState);
				iTestStepResult = EFail;
				CActiveScheduler::Stop();
				}
			else
				{
			    //if this in a navi engine
			   if (machineType==0x102864F7)
				   {
			       ERR_PRINTF2(_L("CMVSAudioRecordAgent->RecordL() left with error : %d"), err);
			       INFO_PRINTF1(_L("This is a Known Failure on the naviengine"));
			       iTestStepResult = EKnownFailure;
			       CActiveScheduler::Stop();
			       }
				}
			}
	
		else
			{
	        ERR_PRINTF3(_L("ExpectedError : %d, ActualError : %d"), iExpectedError, iError);
	        iTestStepResult = EFail;
	        CActiveScheduler::Stop();
			}
        }
    else if (iCallbackState != iExpectedState)
        {
        ERR_PRINTF3(_L("ExpectedState : %d, ActualState : %d"), iExpectedState, iCallbackState);
        iTestStepResult = EFail;
        CActiveScheduler::Stop();
        }
    // Do action based on the current state
    else if(iCallbackState == ENotReady)
        {
        if (iPreviousState == EAudioOpening)
             {
            if (iCallbackAgent == ETestAudioPlay)
                {
                iCallbackEvent = EAudPlayerOpenComplete;
                }
            else if (iCallbackAgent == ETestAudioRecord)
                {
                iCallbackEvent = EAudRecorderOpenComplete;
                }
            TRAP(iError, FsmL(iCallbackEvent));
            }
        else if ((iPreviousState == ENotReady) && (iExpectedError != KErrNone))
            {
            if (iCallbackAgent == ETestVideoPlay)
                {
                iCallbackEvent = EVidPlayerOpening;
                }
            else if (iCallbackAgent == ETestVideoRecord)
                {
                iCallbackEvent = EVidRecorderOpening;
                }
            TRAP(iError, FsmL(iCallbackEvent));
            }
        }
    else if(iCallbackState == EAudioOpening)
        {
        // this is synchronous, so just wait for the next Async callback
        // Set Next State and Wait for Callback.
        PrepareState(EAudioOpened, KErrNone);
        }
    else if(iCallbackState == EAudioOpened)
        {
        if (iPreviousState == EAudioOpening)
            {
            if (iCallbackAgent == ETestAudioPlay)
               {
               iCallbackEvent = EAudPlayerOpenComplete;
               }
            else if (iCallbackAgent == ETestAudioRecord)
               {
               iCallbackEvent = EAudRecorderOpenComplete;
               }
            }
        else if (iPreviousState == EAudioPlaying)
            {
            if (iCallbackAgent == ETestAudioPlay)
                {
                iCallbackEvent = EAudPlayerPlayComplete;
                }
            else if (iCallbackAgent == ETestAudioRecord)
                {
                iCallbackEvent = EAudRecorderPlayComplete;
                }    
            }
        // Call the state handler (to do the next action)
        TRAP(iError, FsmL(iCallbackEvent));
        }
    else if(iCallbackState == EAudioPlaying)
        {
        if (iCallbackAgent == ETestAudioRecord)
            {
            iCallbackEvent = EAudRecorderPlayProgress;
            // Call the state handler (to do the next action)
            TRAP(iError, FsmL(iCallbackEvent));
            }
        }
    else if(iCallbackState == EAudioRecording)
        {
        iCallbackEvent = EAudRecorderRecordProgress;
        // Call the state handler (to do the next action)
        TRAP(iError, FsmL(iCallbackEvent));
        }
    else if(iCallbackState == EAudioStopped)
        {
        if (iCallbackAgent == ETestAudioPlay)
            {
            if (iPreviousState == EAudioPlaying)
                {
                iCallbackEvent = EAudPlayerPlayComplete;
                }
            }
        else if (iCallbackAgent == ETestAudioRecord)
            {
            if (iPreviousState == EAudioPlaying)
                {
                iCallbackEvent = EAudRecorderPlayComplete;
                }
            else if (iPreviousState == EAudioRecording)
                {
                iCallbackEvent = EAudRecorderRecordComplete;
                }
            }
        }
    else if(iCallbackState == EVideoOpening)
        {
        if (iCallbackAgent == ETestVideoPlay)
            {
            iCallbackEvent = EVidPlayerOpening;
            }
        else if (iCallbackAgent == ETestVideoRecord)
            {
            iCallbackEvent = EVidRecorderOpening;
            }
        // Call the state handler (to do the next action)
        TRAP(iError, FsmL(iCallbackEvent));
        }
    else if(iCallbackState == EVideoOpened)
        {
        if (iPreviousState == EVideoOpening)
            {
            if (iCallbackAgent == ETestVideoPlay)
                {
                iCallbackEvent = EVidPlayerOpenComplete;
                }
            else if (iCallbackAgent == ETestVideoRecord)
                {
                iCallbackEvent = EVidRecorderOpenComplete;
                }
            }
        else if (iPreviousState == EVideoPlaying)
            {
            if (iCallbackAgent == ETestVideoPlay)
                {
                iCallbackEvent = EVidPlayerPlayComplete;
                }
            }
        // Call the state handler (to do the next action)
        TRAP(iError, FsmL(iCallbackEvent));
        }
    else if(iCallbackState == EVideoRecording)
        {
        // this is synchronous, so just wait for the next Async callback
        // Set Next State and Wait for Callback.
        PrepareState(EVideoStopped, KErrNone);
        }
    else if (iCallbackState == EVideoPaused)
	    {
		if (iPreviousState == EVideoRecording)	    	
			{
			if (iCallbackAgent == ETestVideoRecord)
				{
				iCallbackEvent = EVidRecorderRecording;	
				}
			}
	    }
    else if(iCallbackState == EVideoStopped)
        {
        if (iPreviousState == EVideoPlaying)
            {
            if (iCallbackAgent == ETestVideoPlay)
                {
                iCallbackEvent = EVidPlayerPlayComplete;
                }
            }
        else if (iPreviousState == EVideoRecording)
            {
            if (iCallbackAgent == ETestVideoRecord)
                {
                iCallbackEvent = EVidRecorderRecordComplete;
                }    
            }
        }
    }
    
/**
 *
 * MvsResourceNotificationTest
 *
 */    
void RTestStepMVSAgent::MvsResourceNotificationTest(const TDesC8& /*aNotificationData*/, CTestMVSCallbackSupport& /*aTestMVSCallbackSupport*/)
	{

	}
	
/**
 *
 * ResetState
 *
 */
void RTestStepMVSAgent::ResetState()
    {
    iPreviousState = ENotReady;
    iCallbackState = ENotReady;
    iExpectedError = KErrNone;
    iExpectedState = ENotReady;
    }

/**
 *
 * PrepareState
 *
 */
void RTestStepMVSAgent::PrepareState(TMVSState aExpectedState, TInt aExpectedError)
    {
    iExpectedError = aExpectedError;
    iExpectedState = aExpectedState;
    }

/**
 *
 * Time comparison utility function
 * returns whether actual timed value fall within deviation limits
 *
 */ 
TBool RTestStepMVSAgent::TimeComparison(const TUint aActual, const TUint aExpected, const TUint aDeviation)
    {
    // save unnessary conditions
    if(aActual == aExpected)
        return ETrue;    

    // Prevent unsigned wrapping errors 
    TUint difference;
    if(aActual > aExpected)
        difference = aActual - aExpected;
    else
        difference = aExpected - aActual;

    // comapare
    if(difference < aDeviation)
        return ETrue;
    return EFalse;
    }

//
// RTestStepMVSPluginInfo
//

/**
 *
 * RTestStepMVSPluginInfo
 * Test step constructor
 *
 */
RTestStepMVSPluginInfo::RTestStepMVSPluginInfo(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TUid aExpectedMediaType, const TUid aControllerUid)
    : RTestStepMVSBase(aTestName, aSectName, aKeyName)
    {
    iExpectedMediaType = aExpectedMediaType;
    iControllerUid = aControllerUid;
    
    if(iExpectedMediaType == KUidMediaTypeAudio)
            {
            iMediaType = EFalse;
            }
        else
            {
            iMediaType = ETrue;    
            }
    }

/**
 *
 * DoTestStepPreamble
 *
 */
TVerdict RTestStepMVSPluginInfo::DoTestStepPreambleL()
    {    
    TVerdict preamRes;
    preamRes = EPass;
    
    // Call base DoTestStepPreambleL()
    preamRes = RTestStepMVSBase::DoTestStepPreambleL();
    if (preamRes != EPass)
        {
        return preamRes;
        }
        
    // Initialise Arrays
    iPluginArray = new(ELeave) CDesCArrayFlat(KMaxPlugin);
    iExtArray = new(ELeave) CDesCArrayFlat(KMaxPlugin);
    iCodecArray = new(ELeave) CDesCArrayFlat(KMaxPlugin);

    return preamRes;
    }    


/**
 *
 * DoTestStepPostambleL
 *
 */
TVerdict RTestStepMVSPluginInfo::DoTestStepPostambleL()
    {
    // Reset & Destroy Arrays
    iPluginArray->Reset();
    iCodecArray->Reset();
    delete iPluginArray;
    delete iCodecArray;
    iPluginArray = NULL;
    iCodecArray = NULL;
    
    // Base Postamble.
    return RTestStepMVSBase::DoTestStepPostambleL();
    }

//
// RTestStepMVSAudioAgent
//

/**
 *
 * RTestStepMVSAudioAgent
 *
 */
RTestStepMVSAudioAgent::RTestStepMVSAudioAgent(const TDesC& aTestName, const TDesC& aSectName,
                                           const TDesC& aKeyName)
    : RTestStepMVSAgent(aTestName, aSectName, aKeyName)
    {}

/**
 *
 * DoTestStepPreamble
 *
 */
TVerdict RTestStepMVSAudioAgent::DoTestStepPreambleL()
    {    
    TVerdict PreamRes;
    PreamRes = EPass;
    
    // Call base DoTestStepPreambleL()
    PreamRes = RTestStepMVSAgent::DoTestStepPreambleL();
    if (PreamRes != EPass)
        {
        return PreamRes;
        }
    
    // Prepare the state for the UpdateStateChange event after Initialise
    PrepareState(ENotReady, KErrNone);
        
    // Create the MVS Agents
    iActiveAgent = ETestAudioPlay;
    iMVSAudioPlayAgent = CMVSAudioPlayAgent::NewL(*iAudioPlayAgentCallback);
    if(iMVSAudioPlayAgent == NULL)
        {
        ERR_PRINTF1(_L("ERROR : iMVSAudioPlayAgent cannot be created."));
        return EInconclusive;
        }
    iActiveAgent = ETestAudioRecord;
    iMVSAudioRecordAgent = CMVSAudioRecordAgent::NewL(*iAudioRecordAgentCallback);
    if(iMVSAudioRecordAgent == NULL)
        {
        ERR_PRINTF1(_L("ERROR : iMVSAudioRecordAgent cannot be created."));
        return EInconclusive;
        }
        
    return PreamRes;
    }    


/**
 *
 * DoTestStepPostambleL
 *
 */
TVerdict RTestStepMVSAudioAgent::DoTestStepPostambleL()
    {
    // Prepare the state for the UpdateStateChange event after Reset
    PrepareState(ENotReady, KErrNone);
    
    // Delete CMVSAudioPlayAgent
    if (iMVSAudioPlayAgent)
        {
        iActiveAgent = ETestAudioPlay;
        iMVSAudioPlayAgent->Reset();
        delete iMVSAudioPlayAgent;
        iMVSAudioPlayAgent = NULL;
        }
        
    // Delete CMVSAudioRecordAgent
    if (iMVSAudioRecordAgent)
        {
        iActiveAgent = ETestAudioRecord;
        iMVSAudioRecordAgent->Reset();
        delete iMVSAudioRecordAgent;
        iMVSAudioRecordAgent = NULL;
        }
    
    // Base Postamble.
    return RTestStepMVSAgent::DoTestStepPostambleL();
    }

/**
 *
 * DoTestStepL
 *
 */
TVerdict RTestStepMVSAudioAgent::DoTestStepL()
    {
    return( PerformTestL() );
    }


//
// RTestStepMVSVideoAgent
//

/**
 *
 * RTestStepMVSVideoAgent()
 *
 */
RTestStepMVSVideoAgent::RTestStepMVSVideoAgent(const TDesC& aTestName, 
                                               const TDesC& aSectName,
                                               const TDesC& aKeyName) : 
    RTestStepMVSAgent(aTestName, aSectName, aKeyName)
    {}

/**
 *
 * DoTestStepPreamble
 *
 */
TVerdict RTestStepMVSVideoAgent::DoTestStepPreambleL()
    {
    TVerdict PreamRes;
    PreamRes = EPass;
    
    // Call base DoTestStepPreambleL()
    PreamRes = RTestStepMVSAgent::DoTestStepPreambleL();
    if (PreamRes != EPass)
        {
        return PreamRes;
        }
        
    // check if a non GCE controller should be used for video playback testing
    GetBoolFromConfig(iSectName, KIniControllerConfigKeyName, iUseNonGceController);
    
    // Initialise Window server objects
    InitWservL();

    // Create the MVS Client
    iMVSVideoPlayAgent = CMVSVideoPlayAgent::NewL(*iVideoPlayAgentCallback);
    if(iMVSVideoPlayAgent == NULL)
        {
        ERR_PRINTF1(_L("ERROR : iMVSVideoPlayAgent cannot be created."));
        return EInconclusive;
        }
    iMVSVideoRecordAgent = CMVSVideoRecordAgent::NewL(*iVideoRecordAgentCallback);
    if(iMVSVideoRecordAgent == NULL)
        {
        ERR_PRINTF1(_L("ERROR : iMVSVideoRecordAgent cannot be created."));
        return EInconclusive;
        }
    
    // SetVideoOutput for iVideoPlayAgent
    TRect rect, clipRect;
    TRAP(iError, iMVSVideoPlayAgent->SetVideoOutputL(iWs, *iScreen, *iWindow, rect, clipRect));
    if (iError != KErrNone)
        {
        ERR_PRINTF2(_L("ERROR : iMVSVideoPlayAgent->SetVideoOutputL() left with error : %d"), iError);
        return EInconclusive;
        }
    
    return PreamRes;
    }    

/**
 *
 * DoTestStepPostambleL
 *
 */
TVerdict RTestStepMVSVideoAgent::DoTestStepPostambleL()
    {
    // Delete CMVSVideoPlayAgent
    if (iMVSVideoPlayAgent)
        {
        delete iMVSVideoPlayAgent;
        iMVSVideoPlayAgent = NULL;
        }
    
    // Delete CMVSVideoRecordAgent
    if (iMVSVideoRecordAgent)
        {
        delete iMVSVideoRecordAgent;
        iMVSVideoRecordAgent = NULL;
        }
    
    // Destroy Window server objects
    DeInitWserv();
    
    // Base Postamble.
    return RTestStepMVSAgent::DoTestStepPostambleL();
    }

/**
 *
 * DoTestStepL
 *
 */
TVerdict RTestStepMVSVideoAgent::DoTestStepL()
    {
    return( PerformTestL() );
    }

/**
 *
 *	OpenFileL
 *
 **/
void RTestStepMVSVideoAgent::OpenFileL(CMVSVideoPlayAgent& aVideoPlayAgent,
                                       const TDesC& aFileName)
    {    
    if (!iUseNonGceController)
        {
        aVideoPlayAgent.OpenFileL(aFileName);
        }
    else
        {                    
        aVideoPlayAgent.OpenFileL(aFileName, KMmfTestAviNonGcePlayControllerUid);        
        }
    }
        
/**
 *
 *	PlayL
 *
 **/
void RTestStepMVSVideoAgent::PlayL(CMVSVideoPlayAgent& aVideoPlayAgent)    
    {
#ifdef SYMBIAN_BUILD_GCE    
    if (aVideoPlayAgent.SupportVideoPlayerUtility2())
        {
        aVideoPlayAgent.RemoveDisplayWindowL(*iWindow);
        aVideoPlayAgent.AddDisplayWindowL(iWs, *iScreen, *iWindow);
        }               
#endif        
    aVideoPlayAgent.Play();            
    }

/**
 *
 *	PlayL
 *
 **/

#ifdef SYMBIAN_BUILD_GCE 
void RTestStepMVSVideoAgent::PlayL(CMVSVideoPlayAgent& aVideoPlayAgent,
                                   const TRect& aVideoExtent,
                                   const TRect& aWindowClipping)
#else
void RTestStepMVSVideoAgent::PlayL(CMVSVideoPlayAgent& aVideoPlayAgent,
                                   const TRect& /*aVideoExtent*/,
                                   const TRect& /*aWindowClipping*/)
#endif // SYMBIAN_BUILD_GCE                                  
           
    {
#ifdef SYMBIAN_BUILD_GCE    
    if (aVideoPlayAgent.SupportVideoPlayerUtility2())
        {
        aVideoPlayAgent.RemoveDisplayWindowL(*iWindow);
        aVideoPlayAgent.AddDisplayWindowL(iWs, *iScreen, *iWindow, aVideoExtent, aWindowClipping);
        }
#endif        
    aVideoPlayAgent.Play();            
    }

    
//#ifdef __WINS__
/**
 *
 * InitWservL
 *
 */
void RTestStepMVSVideoAgent::InitWservL()
    {
    TInt err = iWs.Connect();
    if (err != KErrNone)
        {
        // Access violation if ws is null
        INFO_PRINTF1(_L("TestSuite error: Cannot test, no window server available"));
        User::Leave(err);
        }

    iScreen = new (ELeave) CWsScreenDevice(iWs); // make device for this session
    User::LeaveIfError(iScreen->Construct()); // and complete its construction

    iRootWindow = RWindowGroup(iWs);
    User::LeaveIfError(iRootWindow.Construct((TUint32)this, ETrue));

    iWindow = new(ELeave) RWindow(iWs);
    User::LeaveIfError(((RWindow*)iWindow)->Construct(iRootWindow,((TUint32)(this)) + 1));
    iWindow->SetExtent(TPoint(0,0), TSize(400,200));
    iWindow->SetVisible(ETrue);
    iWindow->Activate();
    
#ifdef SYMBIAN_BUILD_GCE	    
    // enable the display mode that has an alpha channel for video display using graphics 
    // surface
    iWindow->SetRequiredDisplayMode(EColor16MA);	    
    iWindow->SetSizeErr(iScreen->SizeInPixels());
    
    // XXX - this forcing of transparency may not be needed, as there is 
    //        a defect in the graphics component right now
    // force the transparency change to take place
    iWindow->SetBackgroundColor(TRgb(0x000000, 0)); 
    iWindow->BeginRedraw();
    iWindow->EndRedraw();    
#endif             
    iWs.Flush();
    }

/**
 *
 * DeInitWserv
 *
 */
void RTestStepMVSVideoAgent::DeInitWserv()
{
    if (iWindow)
        {
        iWindow->Close();
        delete iWindow;
        iWindow = NULL;
        }
    iRootWindow.Close();
    delete iScreen;
    iScreen = NULL;
    iWs.Flush();
    iWs.Close();
}

/**
 * set finished on callback from a timer
 */
void RTestStepMVSVideoAgent::Finished()
	{
	// will be implemented in the derived classes
	INFO_PRINTF1(_L("RTestStepMVSVideoAgent::Finished()"));
	}
    
//
// RTestMVSVideoAgentSetScrIdAndPlayBase
//

/**
 * RTestMVSVideoAgentSetScrIdAndPlayBase::Constructor
 *	 This class enables the MVS video agent SecDisp test steps. 
 *	 This the base class for all the 
 *	 SecDisp related test steps
 */
RTestMVSVideoAgentSetScrIdAndPlayBase::RTestMVSVideoAgentSetScrIdAndPlayBase(const TDesC& aTestName, 
                                                                             const TDesC& aSectName, 
                                                                             const TDesC& aKeyName, 
                                                                             TInt aExpectedError,
                                                                             TInt aScreenNumber)
	{
	iTestStepName=aTestName;
	iSectName = aSectName;
	iKeyName = aKeyName;
	iTestExpectedError = aExpectedError;
	 
	iHeapSize = 1000000; //1MB
	iScreenNumber = aScreenNumber;
	iCurrentScreenNumber = 0;
	}
    
/**
 *
 * SetTestSuite
 *
 */
void RTestMVSVideoAgentSetScrIdAndPlayBase::SetTestSuite(const CTestSuiteMVSAgent* aTestSuite)
	{
	iMVSAgentTestSuite = aTestSuite;
	}  
	
/**
 *
 * GetDriveName()
 * Method to get the drive name where the data files are present
 *
 */
void RTestMVSVideoAgentSetScrIdAndPlayBase::GetDriveName(TDes& aFileName)
	{
	#ifdef __WINS__
	    aFileName = _L("c:");
	#elif defined (__MARM__) || defined(__X86GCC__)
	    #ifdef TEST_FILES_ON_C_DRIVE
	        aFileName = _L("c:");
	    #else
	        aFileName = _L("z:");
	    #endif // TEST_FILES_ON_C_DRIVE
	#endif
	}
	    
/**
 *
 * InitWservL
 *
 */
void RTestMVSVideoAgentSetScrIdAndPlayBase::InitWservL()
	{
	TInt err = iWs.Connect();
	if (err != KErrNone)
		{
		// Access violation if ws is null
		INFO_PRINTF1(_L("TestSuite error: Cannot test, no window server available"));
		User::Leave(err);
		}

	iScreen = new (ELeave) CWsScreenDevice(iWs); // make device for this session
	User::LeaveIfError(iScreen->Construct()); // and complete its construction

	iRootWindow = RWindowGroup(iWs);
	User::LeaveIfError(iRootWindow.Construct((TUint32)this, ETrue));

	iWindow = new(ELeave) RWindow(iWs);
	User::LeaveIfError(((RWindow*)iWindow)->Construct(iRootWindow,((TUint32)(this)) + 1));
	iWindow->SetExtent(TPoint(0,0), TSize(400,200));
	iWindow->SetVisible(ETrue);
	iWindow->Activate();
	
#ifdef SYMBIAN_BUILD_GCE	
    // enable the display mode that has an alpha channel for video display using graphics 
    // surface
    iWindow->SetRequiredDisplayMode(EColor16MA);	
    
    // XXX - this forcing of transparency may not be needed, as there is 
    //        a defect in the graphics component right now
    // force the transparency change to take place
    iWindow->SetBackgroundColor(TRgb(0x000000, 0)); 
    iWindow->BeginRedraw();
    iWindow->EndRedraw();        
#endif         
	iWs.Flush();
	}

/**
 *
 * DeInitWserv
 *
 */
void RTestMVSVideoAgentSetScrIdAndPlayBase::DeInitWserv()
	{
	if (iWindow)
		{
		iWindow->Close();
		delete iWindow;
		iWindow = NULL;
		}
	iRootWindow.Close();
	delete iScreen;
	iScreen = NULL;
	iWs.Flush();
	iWs.Close();
	}


// start test
void RTestMVSVideoAgentSetScrIdAndPlayBase::KickoffTestL()
	{
	// re-initialise data - orphan any pointer, as would relate to
	// previous run's heap
	User::LeaveIfError(iFs.Connect());

    // Get the filename
    TPtrC tmpFilename;
    if(!GetStringFromConfig(iSectName, iKeyName, tmpFilename))
	    {
	    ERR_PRINTF1(_L("ERROR : Cannot Get value from Config."));
		}
    GetDriveName(iFileName);
    iFileName.Append(tmpFilename);
    
    // check if a non GCE controller should be used for video playback testing
    GetBoolFromConfig(iSectName, KIniControllerConfigKeyName, iUseNonGceController);    
			    
    // Initialise Window server objects
    InitWservL();
    
    //Creating callBack  object
    iVideoPlayAgentCallback = CTestMVSCallbackSupport::NewL(*this);
    if (iVideoPlayAgentCallback == NULL)
        {
        INFO_PRINTF1(_L("Error during iVideoPlayAgentCallback "));
        }
    
	// Create the MVS Video Agent
    iMVSVideoPlayAgent = CMVSVideoPlayAgent::NewL(*iVideoPlayAgentCallback);
    if(iMVSVideoPlayAgent == NULL)
    	{
        ERR_PRINTF1(_L("ERROR : iMVSVideoPlayAgent cannot be created."));
        StopTest(iError);
        }
 
	// SetVideoOutput for iMVSVideoPlayAgent
	TRect rect, clipRect;
	TRAP(iError, iMVSVideoPlayAgent->SetVideoOutputL(iWs, *iScreen, *iWindow, rect, clipRect));
	if (iError != KErrNone)
	    {
	    ERR_PRINTF2(_L("ERROR : iMVSVideoPlayAgent->SetVideoOutputL() left with error : %d"), iError);
	    StopTest(iError);
	    }

	// the derived class should implement DoKickoffTestL()
	DoKickoffTestL();

	// Call the state machine to start test actions
	FsmL(EMvsIdle);
	}
	
/**
 *
 * Finished()
 *
 */
void RTestMVSVideoAgentSetScrIdAndPlayBase::Finished()
    {
    INFO_PRINTF1(_L("RTestVclntSetScrIdAndPlayBase::Finished"));
    //overridden in SetScreenIDDuringPlayandPlayVideoFile tests.
    }

TBool RTestMVSVideoAgentSetScrIdAndPlayBase::ValidateEvent()
	{
	if((iExpectedEvent == iCallbackEvent) && (iExpectedError == iError))
		{
		return ETrue;
		}
	else
		{
		// Print trace stating the mismatches
		TPtrC16 ptrExpEvent(KTestMvsStates[iExpectedEvent]);
		TPtrC16 ptrEvent(KTestMvsStates[iCallbackEvent]);
		INFO_PRINTF3(_L("iExpectedState = [%S] , iState = [%S]"), &ptrExpEvent, &ptrEvent);
		INFO_PRINTF3(_L("iExpectedError = [%d] , iError = [%d]"), iExpectedError, iError);
		return EFalse;
		}
	}
	
//	PrepareState
void RTestMVSVideoAgentSetScrIdAndPlayBase::PrepareState(TMVSState aExpectedState, TInt aExpectedError)
    {
    iExpectedError = aExpectedError;
    iExpectedState = aExpectedState;
    } 
    
//	UpdateStateChangeTest
void RTestMVSVideoAgentSetScrIdAndPlayBase::UpdateStateChangeTest(TMVSState aState, TInt aErrorCode, CTestMVSCallbackSupport& aTestMVSCallbackSupport)
	{
	iVideoPlayAgentCallback=&aTestMVSCallbackSupport;
	iPreviousState=iCallbackState;
	iCallbackState=aState;
	iError = aErrorCode;
	 
	// Print trace stating the mismatches
	TPtrC16 ptrPrevState(KTestMvsStates[iPreviousState]);
	TPtrC16 ptrCallbackState(KTestMvsStates[iCallbackState]);
	INFO_PRINTF4(_L("---> iPreviousState = [%S] , iCallbackState = [%S], iError=[%d]"), &ptrPrevState, &ptrCallbackState,iError);
	
	if (iExpectedError != iError)
		{
		if(iError == iTestExpectedError)
		   {
		   ERR_PRINTF3(_L("iTestExpectedError : %d, ActualError : %d"), iTestExpectedError, iError);
		   StopTest(iError, EPass);	
		   }
		else
		   {
		   	if(iError != KErrNotSupported)
				{
				ERR_PRINTF3(_L("ExpectedError : %d, ActualError : %d"), iExpectedError, iError);
				StopTest(iError, EFail);
				}
		   }
		}
    else if (iCallbackState != iExpectedState)
		{
		ERR_PRINTF3(_L("---> ExpectedState : %d, ActualState : %d"), iExpectedState, iCallbackState);
		StopTest(iError, EFail);
		}
        
    // Do action based on the current state
	if(iPreviousState==ENotReady && iCallbackState==EVideoOpening)
		{
		TRAPD(err, FsmL(EVidPlayerOpening));
		   if (err != KErrNone)
			{
			iError = err;  
			StopTest(iError, EFail);  
			}
		}
	else if(iPreviousState == EVideoOpened && iCallbackState == EVideoOpening)
		{
		TRAPD(err, FsmL(EVidPlayerOpening));
		if (err != KErrNone)
			{
			iError = err;  
			StopTest(iError, EFail);  
			}
		}

	if(iPreviousState==EVideoOpening && iCallbackState==EVideoOpened)
		{
		#ifdef SYMBIAN_VARIABLE_BITRATE_CODEC
		    TRAPD(err, FsmL(EVidPlayerOpenComplete));
		    if (err != KErrNone)
			    {
			    iError = err;
			    StopTest(iError, EFail);
			    }
		#else
		if(iError == KErrNotSupported)
			{
		    INFO_PRINTF1(_L("The MVS Play Controller is not available in this OS version, hence KNOWN FAILURE"));
		    StopTest(iError, EKnownFailure);
		    }
		else
		    {
		    ERR_PRINTF2(_L("Unexpected error in UpdateStateChangeTest %d"), iError);
		    StopTest(iError, EFail);
		    }
	    #endif // SYMBIAN_VARIABLE_BITRATE_CODEC
	    }
	 else if((iPreviousState==EVideoOpened || iPreviousState==ENotReady)  && iCallbackState==ENotReady)
		{
		#ifdef SYMBIAN_VARIABLE_BITRATE_CODEC
		#ifdef __EABI__ // secondary screen is not supported in HW.
		if ((iCurrentScreenNumber != 0) && (iError == KErrNotSupported))
			{
		    INFO_PRINTF1(_L("The secondary screen is not supported in HW"));
		    StopTest(iError, EKnownFailure);
		    }
		else if((iCurrentScreenNumber == 0) && (iError == KErrNone))
		    {
		    TRAPD(err, FsmL(EVidPlayerOpenComplete));
		    if (err != KErrNone)
		 	   {
			   iError = err;
			   StopTest(iError, EFail);
			   }
		    }
		else
		    {
		    ERR_PRINTF2(_L("Unexpected error in UpdateStateChangeTest %d"), iError);
		    StopTest(iError, EFail);
		    }
		#endif  // __EABI__ 
		#else
		if(iError == KErrNotSupported)
			{
		    INFO_PRINTF1(_L("The MVS Play Controller is not available in this OS version, hence KNOWN FAILURE"));
		    StopTest(iError, EKnownFailure);
		    }
		else
		    {
		    ERR_PRINTF2(_L("Unexpected error in UpdateStateChangeTest %d"), iError);
		    StopTest(iError, EFail);
		    }
	    #endif // SYMBIAN_VARIABLE_BITRATE_CODEC
	    }
	else if(iPreviousState==EVideoPlaying && iCallbackState==EVideoOpened)
		{

		TRAPD(err, FsmL(EVidPlayerPlayComplete));
		if (err != KErrNone)
			{
			iError = err;
		    StopTest(iError, EFail);
			}	
		}
	    
	}
	
/**
 *
 * MvsResourceNotificationTest
 *
 */    
void RTestMVSVideoAgentSetScrIdAndPlayBase::MvsResourceNotificationTest(const TDesC8& /*aNotificationData*/, CTestMVSCallbackSupport& /*aTestMVSCallbackSupport*/)
	{

	}
	
// cleanup at end
void RTestMVSVideoAgentSetScrIdAndPlayBase::CloseTest()
	{
	iFs.Close();

	// Delete CMVSVideoPlayAgent
	if (iMVSVideoPlayAgent)
	   {
	   delete iMVSVideoPlayAgent;
	   iMVSVideoPlayAgent = NULL;
	   }	    
	// Delete Window server objects
	DeInitWserv();
	}

// OpenFileL
void RTestMVSVideoAgentSetScrIdAndPlayBase::OpenFileL(CMVSVideoPlayAgent& aVideoPlayAgent,
                                                      const TDesC& aFileName)
    {    
    if (!iUseNonGceController)
        {
        aVideoPlayAgent.OpenFileL(aFileName);
        }
    else
        {                    
        aVideoPlayAgent.OpenFileL(aFileName, KMmfTestAviNonGcePlayControllerUid);        
        }
    }

// PlayL
void RTestMVSVideoAgentSetScrIdAndPlayBase::PlayL(CMVSVideoPlayAgent& aVideoPlayAgent)    
    {
#ifdef SYMBIAN_BUILD_GCE    
    if (aVideoPlayAgent.SupportVideoPlayerUtility2())
        {
        aVideoPlayAgent.RemoveDisplayWindowL(*iWindow);
        aVideoPlayAgent.AddDisplayWindowL(iWs, *iScreen, *iWindow);
        }       
#endif        
    aVideoPlayAgent.Play();            
    }

//
//				 RTestStepMVSAgentsAPRBase
//
    
/**
*	RTestStepMVSAgentsAPRBase() Constructor
* 
*	 This class enables the MVS video agent APR test steps. 
*	 This the base class for all the 
*	 APR related test steps	
*
*/
RTestStepMVSAgentsAPRBase::RTestStepMVSAgentsAPRBase(const TDesC& aStepName, const TDesC& aSectionName) :	
    iSectionName(aSectionName)
	{
	iTestStepName = aStepName;
	iExpEventCount = 1;
	iCanStateChange = EFalse;
	iRegistered=0;
	iHeapSize=1000000;
	}
/**
 *
 * SetTestSuite
 *
 */
void RTestStepMVSAgentsAPRBase::SetTestSuite(const CTestSuiteMVSAgent* aTestSuite)
	{
	iMVSAgentTestSuite = aTestSuite;
	}
/**
 *
 * InitWservL
 *
 */
void RTestStepMVSAgentsAPRBase::InitWservL()
	{
	TInt err = iWs.Connect();
	if (err != KErrNone)
		{
		// Access violation if ws is null
		INFO_PRINTF1(_L("TestSuite error: Cannot test, no window server available"));
		User::Leave(err);
		}
	iScreen = new (ELeave) CWsScreenDevice(iWs); // make device for this session
	User::LeaveIfError(iScreen->Construct()); // and complete its construction

	iRootWindow = RWindowGroup(iWs);
	User::LeaveIfError(iRootWindow.Construct((TUint32)this, ETrue));

	iWindow = new(ELeave) RWindow(iWs);
	User::LeaveIfError(((RWindow*)iWindow)->Construct(iRootWindow,((TUint32)(this)) + 1));
	
	iWindow->SetExtent(TPoint(0,0), TSize(400,200));
	iWindow->SetVisible(ETrue);
	iWindow->Activate();
	
#ifdef SYMBIAN_BUILD_GCE	
    // enable the display mode that has an alpha channel for video display using graphics 
    // surface
    iWindow->SetRequiredDisplayMode(EColor16MA);	
    
    // XXX - this forcing of transparency may not be needed, as there is 
    //        a defect in the graphics component right now
    // force the transparency change to take place
    iWindow->SetBackgroundColor(TRgb(0x000000, 0));     
    iWindow->BeginRedraw();
    iWindow->EndRedraw();        
#endif         	

	iWs.Flush();
	}

/**
 *
 * DeInitWserv
 *
 */
void RTestStepMVSAgentsAPRBase::DeInitWserv()
	{
	if (iWindow)
		{
		iWindow->Close();
		delete iWindow;
		iWindow = NULL;
		}
	iRootWindow.Close();
	delete iScreen;
	iScreen = NULL;
	iWs.Flush();
	iWs.Close();
	}
		
/**
 *
 * GetDriveName()
 * Method to get the drive name where the data files are present
 *
 */
void RTestStepMVSAgentsAPRBase::GetDriveName(TDes& aFileName)
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
			
/**
*	
*		KickoffTestL() -	start test
*
*/ 
void RTestStepMVSAgentsAPRBase::KickoffTestL()
	{
	// re-initialise data - orphan any pointer, as would relate to
	// previous run's heap

	User::LeaveIfError(SetCacheSize());
    	
	TPtrC filename;		  
    // Initialise Window server objects
    InitWservL();    
	User::LeaveIfError(iFs.Connect());	
	iFs.Delete(filename); // ensure we start with no file. Ignore errors.	
	// Get 1st Video file name
	if(!GetStringFromConfig(iSectionName, _L("VideoFile1"), filename))
		{
		ERR_PRINTF1(_L("Error Video file name"));
		StopTest(EInconclusive);
		}
	GetDriveName(iFilename1);
	iFilename1.Append(filename);	
	
    // check if a non GCE controller should be used for video playback testing
    GetBoolFromConfig(iSectionName, KIniControllerConfigKeyName, iUseNonGceController);	

	// Initialise Video  Play Agent1
	iVideoPlayAgentCallback1 = CTestMVSCallbackSupport::NewL(*this);
	CleanupStack::PushL(iVideoPlayAgentCallback1);
	// Create the MVS Video Agent1
    iMVSVideoPlayAgent1 = CMVSVideoPlayAgent::NewL(*iVideoPlayAgentCallback1);
    if(iMVSVideoPlayAgent1 == NULL)
        {
        ERR_PRINTF1(_L("ERROR : iMVSVideoPlayAgent1 cannot be created."));
        StopTest(iError);
        }
    CleanupStack::PushL(iMVSVideoPlayAgent1);
    // SetVideoOutputL for iMVSVideoPlayAgent1
	TRect rect, clipRect;
	TRAP(iError, iMVSVideoPlayAgent1->SetVideoOutputL(iWs, *iScreen, *iWindow, rect, clipRect));
	if (iError != KErrNone)
	    {
	    ERR_PRINTF2(_L("ERROR : iMVSVideoPlayAgent1->SetVideoOutputL() left with error : %d"), iError);
	    StopTest(iError);
	    }
    // Get  Audio file name
	if(!GetStringFromConfig(iSectionName, _L("AudioFile1"), filename))
		{
		ERR_PRINTF1(_L("Error Video file name"));
		StopTest(EInconclusive);
		}
	GetDriveName(iFilename2);
	iFilename2.Append(filename);    
    
    // Initialise Audio Play Agent
	INFO_PRINTF2(_L("Initialise CAudioPlayAgent : %S, EMdaPriorityMax"), &iFilename2);
	iAudioPlayAgentCallback = CTestMVSCallbackSupport::NewL(*this);
	CleanupStack::PushL(iAudioPlayAgentCallback);  
    // Create the MVS Audio Agents
    iMVSAudioPlayAgent = CMVSAudioPlayAgent::NewL(*iAudioPlayAgentCallback);
    if(iMVSAudioPlayAgent == NULL)
        {
        ERR_PRINTF1(_L("ERROR : iMVSAudioPlayAgent cannot be created."));
          StopTest(iError);
        }
    CleanupStack::PushL(iMVSAudioPlayAgent);
    // set priority of iMVSAudioPlayAgent1 to EMdaPriorityMax
    iMVSAudioPlayAgent->SetPriority(EMdaPriorityMax, EMdaPriorityPreferenceTimeAndQuality);

	// Get 1st Video file name
	if(!GetStringFromConfig(iSectionName, _L("VideoFile2"), filename))
		{
		ERR_PRINTF1(_L("Error Video file name"));
		StopTest(EInconclusive);
		}
	GetDriveName(iFilename3);
	iFilename3.Append(filename);
	    
    // Initialise 2nd Video Play Agent
	iVideoPlayAgentCallback2 = CTestMVSCallbackSupport::NewL(*this);
	CleanupStack::PushL(iVideoPlayAgentCallback2);    
    // Create the MVS Audio Agents
    iMVSVideoPlayAgent2 = CMVSVideoPlayAgent::NewL(*iVideoPlayAgentCallback2);
    if(iMVSVideoPlayAgent2 == NULL)
        {
        ERR_PRINTF1(_L("ERROR : iMVSVideoPlayAgent2 cannot be created."));
       StopTest(iError);
        }
    CleanupStack::PushL(iMVSVideoPlayAgent2);
    
    // SetVideoOutputL for iMVSVideoPlayAgent2
	TRAP(iError, iMVSVideoPlayAgent2->SetVideoOutputL(iWs, *iScreen, *iWindow, rect, clipRect));
	if (iError != KErrNone)
	    {
	    ERR_PRINTF2(_L("ERROR : iMVSVideoPlayAgent2->SetVideoOutputL() left with error : %d"), iError);
	    StopTest(iError);
	    }	    	        
    CleanupStack::Pop(6);		
	// the derived class should implement DoKickoffTestL()
	DoKickoffTestL();	
	// Call the state machine to start test actions
	FsmL(EMvsIdle, KErrNone);
	}

/**
 *
 *	OpenFileL
 *
 **/
void RTestStepMVSAgentsAPRBase::OpenFileL(CMVSVideoPlayAgent& aVideoPlayAgent,
                                          const TDesC& aFileName)
    {    
    if (!iUseNonGceController)
        {
        aVideoPlayAgent.OpenFileL(aFileName);
        }
    else
        {                    
        aVideoPlayAgent.OpenFileL(aFileName, KMmfTestAviNonGcePlayControllerUid);        
        }
    }
    
/**
 *
 *	PlayL
 *
 **/
void RTestStepMVSAgentsAPRBase::PlayL(CMVSVideoPlayAgent& aVideoPlayAgent)    
    {
#ifdef SYMBIAN_BUILD_GCE    
    if (aVideoPlayAgent.SupportVideoPlayerUtility2())
        {
        aVideoPlayAgent.RemoveDisplayWindowL(*iWindow);
        aVideoPlayAgent.AddDisplayWindowL(iWs, *iScreen, *iWindow);
        }       
#endif        
    aVideoPlayAgent.Play();            
    }
    
/**
 *
 *	ValidateEvent
 *
 **/
TBool RTestStepMVSAgentsAPRBase::ValidateEvent(TMvsTestAgentEvents aEventCode, TInt aError)
	{
	// analyse if we can go to the next state.
	TInt analysisError = 0;
	analysisError = AnalyseInput(aEventCode, aError);
	if (analysisError == -1)
		{
		StopTest(EFail);
		return EFalse;
		}
	else
		{
		return ETrue;
		}
	
	}
	
/**
 * 	 AnalyseInput
 * returns 1 = it can move to a new state
 * returns 0 = still waiting for more events
 * returns -1 = some error has occured
 */
 TInt RTestStepMVSAgentsAPRBase::AnalyseInput(TMvsTestAgentEvents aEventCode, TInt aError)
	{
	TInt res = 0;
	TBool matchFound = EFalse;
	for(TInt i = 0; i < iExpEventCount; i++)
		{
		//check for client and event.
		if((iExpectedEvent[i] == aEventCode) && (iExpectedError[i]== aError)) 
			{
			iActEventCount++;
			matchFound = ETrue;
			break;
			}
		}	
	if(matchFound == EFalse)
		{
		for(TInt i = 0; i < iExpEventCount; i++)
			{
			//check for client and event.
			ERR_PRINTF4(_L("ExpectedEvent[%d]=%d, ActualEvent=%d"), i, iExpectedEvent[i], aEventCode);
			ERR_PRINTF4(_L("ExpectedActiveAgent[%d]=%d, ActualActiveAgent=%d"), i, iActiveAgent, iCallbackAgent);
			ERR_PRINTF4(_L("ExpectedError[%d]=%d, ActualError=%d"), i, iExpectedError[i], iError);
			}
		return -1;
		}
	if(iActEventCount == iExpEventCount)
		{
		iCanStateChange = ETrue;
		iExpEventCount = 1;
		iActEventCount = 0;
		}
	else 
		{
		iCanStateChange = EFalse;
		}			
	return res;
	}	
	
/**
*
*	PrepareState
*
**/
void RTestStepMVSAgentsAPRBase::PrepareState(TMVSState aExpectedState,TMvsTestAgentEvents aEventCode, TInt aExpectedError, TMvsTestAgents aActiveAgent, TInt aEventIndex)
    {
    iExpectedEvent[aEventIndex-1] = aEventCode;
	iExpectedError[aEventIndex-1] = aExpectedError;
	iActiveAgent = aActiveAgent;
	iActEventCount = 0;
	iExpectedState= aExpectedState;
	iCanStateChange=EFalse;
    }    
    
/**
*	
*	UpdateStateChangeTest
*
**/
void RTestStepMVSAgentsAPRBase::UpdateStateChangeTest(TMVSState aState, TInt aErrorCode, CTestMVSCallbackSupport& aTestMVSCallbackSupport)
	{
	// Update local variables
    iError = aErrorCode;
    TInt iTempError;
        
    // Get the agent the event was generated from...
    if (&aTestMVSCallbackSupport == iAudioPlayAgentCallback)
        {
        iCallbackAgent = ETestAudioPlay;
        iPreviousStateAP = iCallbackStateAP;
        iCallbackStateAP = aState;

	    // Print the Trace on the log file.. 
	    TPtrC16 ptr1(KTestMvsAgents[iCallbackAgent]);
	    TPtrC16 ptr2(KTestMvsStates[aState]);
	    INFO_PRINTF4(_L(">>>UpdateStateChange => Source=[%S] ; CurrentState=[%S] ; Error=[%d]"), &ptr1, &ptr2, aErrorCode);
        
        if(iCallbackStateAP == EAudioOpened)
			{
			if (iCallbackAgent == ETestAudioPlay)
				{
				if (iPreviousStateAP == EAudioOpening)
					{
					iCallbackEvent = EAudPlayerOpenComplete;
					}
				else if (iPreviousStateAP == EAudioPlaying && iSecPlay)
					{
					iCallbackEvent = EAudPlayerPlayComplete1;
					}
				else if (iPreviousStateAP == EAudioPlaying)
					{
					iCallbackEvent = EAudPlayerPlayComplete;
					}				
				}
			// Call the state handler (to do the next action)
			TRAP(iError, FsmL(iCallbackEvent, iError));
			}
        }
    else if (&aTestMVSCallbackSupport == iVideoPlayAgentCallback2)
        {
        iCallbackAgent = ETestVideoPlay2;
        iPreviousStateVP2 = iCallbackStateVP2;
        iCallbackStateVP2 = aState;
	    // Print the Trace on the log file.. 
	    TPtrC16 ptr1(KTestMvsAgents[iCallbackAgent]);
	    TPtrC16 ptr2(KTestMvsStates[aState]);
	    INFO_PRINTF4(_L(">>>UpdateStateChange => Source=[%S] ; CurrentState=[%S] ; Error=[%d]"),&ptr1, &ptr2, aErrorCode);
	    
        // Do action based on the current state
	    if(iCallbackStateVP2 == EVideoOpening)
	        {
       	 	if (iPreviousStateVP2 == ENotReady)
				{
				iCallbackEvent = EVidPlayerOpening2;
				}
			TRAP(iError, FsmL(iCallbackEvent, iError));
	        }    
	    else if(iCallbackStateVP2 == EVideoOpened)
	        {
	        if (iPreviousStateVP2 == EVideoOpening)
				{
				iCallbackEvent = EVidPlayerOpenComplete2;
				}
			else if (iPreviousStateVP2 == EVideoPlaying)
				{
				iCallbackEvent = EVidPlayerPlayComplete2;
				}
	        // Call the state handler (to do the next action)
	        TRAP(iError, FsmL(iCallbackEvent, iError));
	        }
	    else if(iCallbackStateVP2 == ENotReady)
	        {
	        if(iPreviousStateVP2 == EVideoPlaying)
		        {
		        iCallbackEvent = EErrInUseVideoPlayer1;	
		        }		        
		    // Call the state handler (to do the next action)
	      	TRAP(iTempError, FsmL(iCallbackEvent, iError));
	        }
        }
    else if (&aTestMVSCallbackSupport == iVideoPlayAgentCallback1)
        {
        iCallbackAgent = ETestVideoPlay;
        iPreviousStateVP1 = iCallbackStateVP1;
        iCallbackStateVP1 = aState;

	    // Print the Trace on the log file.. 
	    TPtrC16 ptr1(KTestMvsAgents[iCallbackAgent]);
	    TPtrC16 ptr2(KTestMvsStates[aState]);
	    INFO_PRINTF4(_L(">>>UpdateStateChange => Source=[%S] ; CurrentState=[%S] ; Error=[%d]"), &ptr1, &ptr2, aErrorCode);
        
        // Do action based on the current state
	    if(iCallbackStateVP1 == EVideoOpening)
	        {
       	 	if (iPreviousStateVP1 == ENotReady)
				{
				iCallbackEvent = EVidPlayerOpening;
				}
			TRAP(iError, FsmL(iCallbackEvent, iError));
	        }
	    else if(iCallbackStateVP1 == EVideoOpened)
	        {
	        if (iPreviousStateVP1 == EVideoOpening)
				{
				iCallbackEvent = EVidPlayerOpenComplete;
				}
			else if (iPreviousStateVP1 == EVideoPlaying)
				{
				iCallbackEvent = EVidPlayerPlayComplete;
				}
	        // Call the state handler (to do the next action)
	        TRAP(iError, FsmL(iCallbackEvent, iError));
	        }
	    else if(iCallbackStateVP1 == ENotReady)
	        {
	        if(iPreviousStateVP1 == EVideoPlaying && iSecPlay)
		    	{
			    iCallbackEvent = EErrInUseVideoPlayer1;
		        }
	        else if(iPreviousStateVP1 == EVideoPlaying)
		        {
			    iCallbackEvent = EErrInUseVideoPlayer;
		        }
	        // Call the state handler (to do the next action)
      		TRAP(iTempError, FsmL(iCallbackEvent, iError));
			}
        }
	}

//MvsResourceNotificationTest
void RTestStepMVSAgentsAPRBase::MvsResourceNotificationTest(const TDesC8& /*aNotificationData*/, CTestMVSCallbackSupport& aTestMVSCallbackSupport)	
	{
	if (&aTestMVSCallbackSupport == iVideoPlayAgentCallback1)
		{
		INFO_PRINTF1(_L(">>iMVSVideoPlayAgent1 received Resource Notification"));
		iCallbackAgent = ETestVideoPlay;
		iCallbackEvent = EVidPlayerResourceAvailable;		
		}
	else if (&aTestMVSCallbackSupport == iVideoPlayAgentCallback2)	
		{
		INFO_PRINTF1(_L(">>iMVSVideoPlayAgent2 received Resource Notification"));
        iCallbackAgent = ETestVideoPlay2;		
		iCallbackEvent = EVidPlayerResourceAvailable2;        	
		}
	// Call the state handler (to do the next action)
	TRAP(iError, FsmL(iCallbackEvent, iError));	
	}

// cleanup at end
void RTestStepMVSAgentsAPRBase::CloseTest()
	{
	iTimer=NULL;
	delete iTimer;
	if(RevertCacheSize()!=KErrNone)
		{
		INFO_PRINTF1(_L("Could Not Revert the Cache Size"));
		StopTest(EFail);
		}
	iFs.Close();
	
	// Delete CMVSVideoPlayAgent
	if (iMVSVideoPlayAgent1)
	   {
	   delete iMVSVideoPlayAgent1;
	   iMVSVideoPlayAgent1 = NULL;
	   }
	// Delete CMVSVideoPlayAgent
	if (iMVSAudioPlayAgent)
	   {
	   delete iMVSAudioPlayAgent;
	   iMVSAudioPlayAgent = NULL;
	   }
	// Delete CMVSVideoPlayAgent
	if (iMVSVideoPlayAgent2)
	   {
	   delete iMVSVideoPlayAgent2;
	   iMVSVideoPlayAgent2 = NULL;
	   }	    
	// Delete Window server objects
	DeInitWserv();
	}
	
// timer callback
TInt RTestStepMVSAgentsAPRBase::TimerCallback(TAny* aPtr)
	{
	static_cast<RTestStepMVSAgentsAPRBase*>(aPtr)->DoTimerCallback();
	return KErrNone;
	}
	
void RTestStepMVSAgentsAPRBase::DoTimerCallback()
	{
	iTimer->Cancel(); 
	}

TInt RTestStepMVSAgentsAPRBase::SetCacheSize()
	{
	TInt error=UserSvr::HalFunction(EHalGroupVM,EVMHalSetCacheSize,(TAny*)KCacheSize,(TAny*)KMaxCacheSize);
	TInt ret=CheckCacheError(error);
		
	return ret;
	}

TInt RTestStepMVSAgentsAPRBase::CheckCacheError(TInt aError)
	{
	TInt ret=KErrNone;
	#ifdef __WINSCW__
	//Winscw does not support getting cache size. Ignoring -5 error
	if(aError!=KErrNone)
		{
		if(aError!=KErrNotSupported)
			{
			INFO_PRINTF2(_L("Could not get the cache size  %d"),aError);
			ret=aError;
			}
		}
	#else
	if(aError!=KErrNone)
		{//For ARMV5 we stop for all errors
		INFO_PRINTF2(_L("Could not get the cache size  %d"),aError);
		ret=aError;
		}
	#endif
	return ret;
	}

TInt RTestStepMVSAgentsAPRBase::RevertCacheSize()
	{
	TUint defaultCacheSize = 0; 
	//Done with the test. Setting 0 makes the cache size revert back to boot up values
	TInt error=UserSvr::HalFunction(EHalGroupVM,EVMHalSetCacheSize,(TAny*)defaultCacheSize,(TAny*)defaultCacheSize);
	return CheckCacheError(error);
  	}


//#endif // __WINS__

