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


#include "testvideorecorder.h"
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <mmf/common/mmfvideoenums.h>
#endif
const TInt KBufMimeTypeGranularity = 4;
const TUid KMmfVideoTestControllerUid = {0x101F88D9};
_LIT(KDummyExtension, ".dummy");
const TUint KCacheSize = 1000000; //1MB
const TUint KMaxCacheSize = 30000000; //30MB

//
// RTestVclntRecordAviFile
//

/**
 * RTestVclntRecordAviFile::Constructor
 */
RTestVclntRecordAviFile::RTestVclntRecordAviFile(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError, const TBool aRec)
    : RTestVclntAviRecorderStep(aTestName, aSectName, aKeyName, aExpectedError), iRec (aRec)
    {
    }

/**
 * RTestVclntRecordAviFile::NewL
 */
RTestVclntRecordAviFile* RTestVclntRecordAviFile::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError,const TBool aRec)
    {
    RTestVclntRecordAviFile* self = new (ELeave) RTestVclntRecordAviFile(aTestName, aSectName, aKeyName, aExpectedError, aRec);
    return self;
    }

/**
 * RTestVclntRecordAviFile::NewLC
 */
RTestVclntRecordAviFile* RTestVclntRecordAviFile::NewLC(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError,const TBool aRec)
    {
    RTestVclntRecordAviFile* self = RTestVclntRecordAviFile::NewL(aTestName, aSectName, aKeyName, aExpectedError, aRec);
    CleanupStack::PushL(self);
    return self;
    }

/**
 * RTestVclntRecordAviFile::DoTestStepL()
 */
TVerdict RTestVclntRecordAviFile::DoTestStepL()
    {
    INFO_PRINTF1(_L("Test : Video Recorder - File"));
    
    // Call the state handler from IDLE state
    FsmL(EVRIdle);
    
    // Start the scheduler - Done only once !
    CActiveScheduler::Start();
    
    return iTestStepResult;
    }

/**
 * RTestVclntRecordAviFile::FsmL
 */
void RTestVclntRecordAviFile::FsmL(TVclntTestRecordEvents aEventCode)
    {
    if (FsmCheck(aEventCode))
        {
        switch (aEventCode)
            {
            case EVRIdle:
                // Open iVideoPlayer
                INFO_PRINTF2(_L("iVideoRecorder->OpenFileL() %S"), &iFilename);
                TRAP(iError,iVideoRecorder->OpenFileL(iFilename, iCameraHandle, KMmfTestAviRecordControllerUid, KAviRecordFormatUid));
                PrepareState(EVROpenComplete, KErrNone);
                break;
            case EVROpenComplete:
                // Prepare iVideoPlayer
                INFO_PRINTF1(_L("iVideoRecorder->Prepare()"));
                iVideoRecorder->Prepare();
                PrepareState(EVRPrepareComplete, KErrNone);
                break;
            case EVRPrepareComplete:
                // complete the remaining test activities
                iTestStepResult = DoTestL(iVideoRecorder);
                CActiveScheduler::Stop();
                break;
            }
        }
    }
    
/**
* set finished on callback from a timer
* Will be implemented in derived class, 
* added dummy implementation to resolve build errors.
**/
void RTestVclntRecordAviFile::FinishedL()
	{
	}
	
/**
 * RTestVclntRecordAviFile::DoTestStepL()
 */
TVerdict RTestVclntRecordAviFile::DoTestL(CVideoRecorderUtility* aRec)
    {
    TVerdict ret = EFail;

    if(iRec)
        {
        iError = KErrTimedOut;
        INFO_PRINTF1(_L("iVideoRecorder->Record()"));
        iVideoRecorder->SetAudioEnabledL(EFalse);
        aRec->Record();
        PrepareState(EVRRecordComplete, KErrNone);
        User::After(KFiveSeconds);
        aRec->Stop();
        ret = EPass;
        }
    else
        {
        CActiveScheduler::Stop();
        ret = EPass;            
        }
    return ret;
    }

//
// RTestVclntRecordAviDes
//

/**
 * RTestVclntRecordAviDes::RTestVclntRecordAviDes()
 */
RTestVclntRecordAviDes::RTestVclntRecordAviDes(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError, const TBool aRec)
    : RTestVclntAviRecorderStep(aTestName, aSectName, aKeyName, aExpectedError), iBuffer(NULL,0,0), iRec (aRec)
    {
    iHeapSize = 0xFFFFF;
    }

/**
 * RTestVclntRecordAviDes::NewL()
 */
RTestVclntRecordAviDes* RTestVclntRecordAviDes::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError,const TBool aRec)
    {
    RTestVclntRecordAviDes* self = new (ELeave) RTestVclntRecordAviDes(aTestName, aSectName, aKeyName, aExpectedError, aRec);
    return self;
    }

/**
 * RTestVclntRecordAviDes::NewLC()
 */
RTestVclntRecordAviDes* RTestVclntRecordAviDes::NewLC(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError,const TBool aRec)
    {
    RTestVclntRecordAviDes* self = RTestVclntRecordAviDes::NewL(aTestName, aSectName, aKeyName, aExpectedError, aRec);
    CleanupStack::PushL(self);
    return self;
    }

/**
 * RTestVclntRecordAviDes::DoTestStepPreambleL()
 */
TVerdict  RTestVclntRecordAviDes::DoTestStepPreambleL()
    {
    TVerdict preamRes;
    preamRes = EPass;
    
    // Base DoTestStepPreambleL
    preamRes = RTestVclntAviRecorderStep::DoTestStepPreambleL();
    if (preamRes != EPass)
        {
        return preamRes;
        }
        
    RFs fs;
    RFile file;

    // connect to file system and open file
    User::LeaveIfError(fs.Connect());
    CleanupClosePushL(fs);
//    User::LeaveIfError(file.Open(fs,iFilename,EFileRead));
    CleanupClosePushL(file);

    CleanupStack::PopAndDestroy(2); //file & fs
    return preamRes;
    }

/**
 * RTestVclntRecordAviDes::DoTestStepPostambleL()
 */
TVerdict RTestVclntRecordAviDes::DoTestStepPostambleL()
    {
    iVideo = NULL;
    delete iVideo;
    iBuffer.Set(NULL, 0 ,0);
    return RTestVclntAviRecorderStep::DoTestStepPostambleL();
    }

/**
 * RTestVclntRecordAviDes::DoTestStepL()
 */
TVerdict RTestVclntRecordAviDes::DoTestStepL()
    {
    INFO_PRINTF1(_L("Test : Video Recorder - Descriptor"));
    
    // Call the state handler from IDLE state
    FsmL(EVRIdle);
    
    // Start the scheduler - Done only once !
    CActiveScheduler::Start();
    
    return iTestStepResult;
    }

/**
 * RTestVclntRecordAviDes::FsmL()
 */
void RTestVclntRecordAviDes::FsmL(TVclntTestRecordEvents aEventCode)
    {
    if (FsmCheck(aEventCode))
        {
        switch (aEventCode)
            {
            case EVRIdle:
                // Open iVideoPlayer
                INFO_PRINTF2(_L("iVideoRecorder->OpenDesL() %S"), &iFilename);
                //iBuffer = iVideo->Des();
                TRAP(iError, iVideoRecorder->OpenDesL(iBuffer, NULL, KMmfTestAviRecordControllerUid, KAviRecordFormatUid));
                PrepareState(EVROpenComplete, KErrNotSupported);
                break;
            case EVROpenComplete:
                // Prepare iVideoPlayer
                INFO_PRINTF1(_L("iVideoRecorder->Prepare()"));
                iVideoRecorder->Prepare();
                PrepareState(EVRPrepareComplete, KErrNotSupported);
                break;
            case EVRPrepareComplete:
                iTestStepResult = DoTestL(iVideoRecorder);
                break;
            case EVRRecordComplete:
                CActiveScheduler::Stop();
                iTestStepResult = EPass;
                break;
            }
        }
    }


/**
 * RTestVclntRecordAviDes::DoTestL()
 */
TVerdict RTestVclntRecordAviDes::DoTestL(CVideoRecorderUtility* aRec)
    {
    TVerdict ret = EFail;

    if(iRec)
        {
        iError = KErrTimedOut;
        INFO_PRINTF1(_L("iVideoRecorder->Record()"));
        aRec->Record();
        PrepareState(EVRRecordComplete, KErrNotSupported);
        }
    else
        {
        CActiveScheduler::Stop();
        ret = EPass;            
        }
    return ret;
    }

//
// RTestVclntRecordAviUrl
//

/**
 * RTestVclntRecordAviUrl::RTestVclntRecordAviUrl()
 */
 
RTestVclntRecordAviUrl::RTestVclntRecordAviUrl(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError, const TBool aRec)
    : RTestVclntAviRecorderStep(aTestName, aSectName, aKeyName, aExpectedError), iRec (aRec)
    {
    }

/**
 * RTestVclntRecordAviUrl::NewL()
 */

RTestVclntRecordAviUrl* RTestVclntRecordAviUrl::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError,const TBool aRec)
    {
    RTestVclntRecordAviUrl* self = new (ELeave) RTestVclntRecordAviUrl(aTestName, aSectName, aKeyName, aExpectedError, aRec);
    return self;
    }

/**
 * RTestVclntRecordAviUrl::NewLC()
 */

RTestVclntRecordAviUrl* RTestVclntRecordAviUrl::NewLC(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError,const TBool aRec)
    {
    RTestVclntRecordAviUrl* self = RTestVclntRecordAviUrl::NewL(aTestName, aSectName, aKeyName, aExpectedError, aRec);
    CleanupStack::PushL(self);
    return self;
    }


/**
 * RTestVclntRecordAviUrl::DoTestStepL()
 */

TVerdict RTestVclntRecordAviUrl::DoTestStepL()
    {
    INFO_PRINTF1(_L("Test : Video Recorder - URL"));
    
    // Call the state handler from IDLE state
    FsmL(EVRIdle);
    
    // Start the scheduler - Done only once !
    CActiveScheduler::Start();
    
    return iTestStepResult;
    }

/**
 * RTestVclntRecordAviUrl::FsmL
 */

void RTestVclntRecordAviUrl::FsmL(TVclntTestRecordEvents aEventCode)
    {
    if (FsmCheck(aEventCode))
        {
        switch (aEventCode)
            {
            case EVRIdle:
                // Open iVideoPlayer
                INFO_PRINTF2(_L("iVideoRecorder->OpenUrlL() %S"), &iFilename);
                TRAP(iError,iVideoRecorder->OpenUrlL(iFilename, KUseDefaultIap, NULL, KMmfTestAviRecordControllerUid, KAviRecordFormatUid));
                PrepareState(EVROpenComplete, KErrNone);
                break;
            case EVROpenComplete:
                // Prepare iVideoPlayer
                INFO_PRINTF1(_L("iVideoRecorder->Prepare()"));
                iVideoRecorder->Prepare();
                PrepareState(EVRPrepareComplete, KErrNone);
                break;
            case EVRPrepareComplete:
                iTestStepResult = DoTestL(iVideoRecorder);
                break;
            case EVRRecordComplete:
                CActiveScheduler::Stop();
                iTestStepResult = EPass;
                break;
            }
        }
    }


/**
 * RTestVclntRecordAviUrl::DoTestL()
 */

TVerdict RTestVclntRecordAviUrl::DoTestL(CVideoRecorderUtility* aRec)
    {
    TVerdict ret = EFail;

    if(iRec)
        {
        iError = KErrTimedOut;
        INFO_PRINTF1(_L("iVideoRecorder->Record()"));
        aRec->Record();
        PrepareState(EVRRecordComplete, KErrNotSupported);
        }
    else
        {
        CActiveScheduler::Stop();
        ret = EPass;            
        }
    return ret;
    }

//
// RTestVclntRecordConfig
//

/**
 * RTestVclntRecordConfig::RTestVclntRecordConfig()
 */
RTestVclntRecordConfig::RTestVclntRecordConfig(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError)
    : RTestVclntRecordAviFile(aTestName, aSectName, aKeyName, aExpectedError, EFalse)
    {
    }

/**
 * RTestVclntRecordConfig::NewL()
 */
RTestVclntRecordConfig* RTestVclntRecordConfig::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError)
    {
    RTestVclntRecordConfig* self = new (ELeave) RTestVclntRecordConfig(aTestName, aSectName, aKeyName, aExpectedError);
    return self;
    }

/**
 * RTestVclntRecordConfig::DoTestL()
 */
TVerdict RTestVclntRecordConfig::DoTestL(CVideoRecorderUtility* aRec)
    {
    TVerdict ret = EFail;
    TInt cumErr = KErrNone;

    if(iRec)
        {
        iError = KErrTimedOut;
        INFO_PRINTF1(_L("iVideoRecorder->Record()"));
        aRec->Record();
        PrepareState(EVRRecordComplete, KErrNone);
        User::After(KFiveSeconds);
        aRec->Stop();
        ret = EPass;
        }

    // Functions to test (for coverage)
    TRAPD(err, aRec->SetAudioEnabledL(ETrue));
    INFO_PRINTF2(_L("SetAudioEnabledL() returned with err : %d"), err);
    cumErr +=err;

    TRAP(err, aRec->SetAudioTypeL(KMMFFourCCCodePCM16));
    INFO_PRINTF2(_L("SetAudioCodecL() returned with err : %d"), err);    
    cumErr +=err;
    
    _LIT8(KAviVideoCodec,"XVID");
    TRAP(err, aRec->SetVideoTypeL(KAviVideoCodec));
    INFO_PRINTF2(_L("SetVideoCodecL() returned with err : %d"), err);
    cumErr +=err;
    
    RArray<TFourCC> audioTypes;
    TRAP(err, aRec->GetSupportedAudioTypesL(audioTypes));
    INFO_PRINTF2(_L("GetSupportedAudioCodecsL() returned with err : %d"), err);
    cumErr +=err;

    CDesC8ArrayFlat* vidTypes = new (ELeave) CDesC8ArrayFlat(4);
    TRAP(err, aRec->GetSupportedVideoTypesL(*vidTypes));
    INFO_PRINTF2(_L("GetSupportedVideoCodecsL() returned with err : %d"), err);
    cumErr +=err;

    TRAP(err, aRec->SetVideoFrameRateL(10.00));
    INFO_PRINTF2(_L("SetVideoFrameRateL() returned with err : %d"), err);
    cumErr +=err;

    TReal32 frameRate = 0;
    TRAP(err, frameRate = aRec->VideoFrameRateL());
    INFO_PRINTF3(_L("VideoFrameRateL() returned %d, err : %d"), frameRate, err);
    cumErr +=err;

    TSize frameSize;
    frameSize.iWidth = 176;
    frameSize.iHeight = 144;
    TRAP(err, aRec->SetVideoFrameSizeL(frameSize));
    INFO_PRINTF2(_L("SetVideoFrameSizeL() returned with err : %d"), err);
    cumErr +=err;

    TSize getFrameSize;
    TRAP(err, aRec->GetVideoFrameSizeL(getFrameSize));
    INFO_PRINTF2(_L("GetVideoFrameSizeL() returned with err : %d"), err);
    cumErr +=err;

    TRAP(err, aRec->SetVideoBitRateL(64000));
    INFO_PRINTF2(_L("SetVideoBitRateL() returned with err : %d"), err);
    cumErr +=err;

    TInt getVideoBitrate = 0;
    TRAP(err, getVideoBitrate = aRec->VideoBitRateL());
    INFO_PRINTF3(_L("VideoBitRateL() returned %d, err : %d"), getVideoBitrate, err);
    cumErr +=err;

    TRAP(err, aRec->SetAudioBitRateL(705600));
    INFO_PRINTF2(_L("SetAudioBitRateL() returned with err : %d"), err);
    cumErr +=err;

    TInt getAudioBitrate = 0;
    TRAP(err, getAudioBitrate = aRec->AudioBitRateL());
    INFO_PRINTF3(_L("AudioBitRateL() returned %d, err : %d"), getAudioBitrate, err);
    cumErr +=err;
    
    if(cumErr)
        {
        INFO_PRINTF1(_L("Some errors in the process"));
        ret = EFail;
        }
    else
        {
        ret = EPass;
        }

    return ret;
    }


//
// RTestVclntRecordCloseOpen
//


/**
 * RTestVclntRecordCloseOpen::RTestVclntRecordCloseOpen()
 */
RTestVclntRecordCloseOpen::RTestVclntRecordCloseOpen(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError)
    : RTestVclntRecordAviFile(aTestName, aSectName, aKeyName, aExpectedError,EFalse)
    {}

/**
 * RTestVclntRecordCloseOpen::NewL()
 */
RTestVclntRecordCloseOpen* RTestVclntRecordCloseOpen::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError)
    {
    RTestVclntRecordCloseOpen* self = new (ELeave) RTestVclntRecordCloseOpen(aTestName, aSectName, aKeyName, aExpectedError);
    return self;
    }

/**
 * RTestVclntRecordCloseOpen::DoTestStepL
 */
TVerdict RTestVclntRecordCloseOpen::DoTestStepL()
    {
    // Call the state handler from IDLE state
    FsmL(EVRIdle);
    
    // Start the scheduler - Done only once !
    CActiveScheduler::Start();
    
    return iTestStepResult;
    }

/**
 * RTestVclntRecordCloseOpen::FsmL
 */
void RTestVclntRecordCloseOpen::FsmL(TVclntTestRecordEvents aEventCode)
    {
    if (FsmCheck(aEventCode))
        {
        switch (aEventCode)
            {
            case EVRIdle:
                // Open iVideoRecorder First time
                iOpenCount = 1;
                INFO_PRINTF2(_L("iVideoRecorder->OpenFileL() %S"), &iFilename);
                TRAP(iError,iVideoRecorder->OpenFileL(iFilename, iCameraHandle, KMmfTestAviRecordControllerUid, KAviRecordFormatUid));
                PrepareState(EVROpenComplete, KErrNone);
                break;
            case EVROpenComplete:
                if (iOpenCount == 1)
                    {
                    // Prepare iVideoRecorder
                    INFO_PRINTF1(_L("iVideoRecorder->Prepare()"));
                    iVideoRecorder->Prepare();
                    PrepareState(EVRPrepareComplete, KErrNone);    
                    }
                else
                    {
                    CActiveScheduler::Stop();
                    iTestStepResult = EPass;
                    break;
                    }
                break;
            case EVRPrepareComplete:
                // Close iVideoRecorder
                iVideoRecorder->Close();
                User::After(KOneSecond);
                // Open iVideoPlayer Second time
                iOpenCount = 2;
                INFO_PRINTF2(_L("iVideoRecorder->OpenFileL()"), &iFilename);
                TRAP(iError,iVideoRecorder->OpenFileL(iFilename, iCameraHandle, KMmfTestAviRecordControllerUid, KAviRecordFormatUid));
                PrepareState(EVROpenComplete, KErrNone);
                break;
            }
        }
    }


//
// RTestVclntRecordPause 
//

/**
 * RTestVclntRecordPause::RTestVclntRecordPause()
 */
RTestVclntRecordPause::RTestVclntRecordPause(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError)
    : RTestVclntRecordAviFile(aTestName, aSectName, aKeyName, aExpectedError,EFalse)
    {}

/**
 * RTestVclntRecordPause::NewL()
 */
RTestVclntRecordPause* RTestVclntRecordPause::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError)
    {
    RTestVclntRecordPause* self = new (ELeave) RTestVclntRecordPause(aTestName, aSectName, aKeyName, aExpectedError);
    return self;
    }

/**
 * RTestVclntRecordPause::DoTestStepL
 */
TVerdict RTestVclntRecordPause::DoTestStepL()
    {
    // Call the state handler from IDLE state
    FsmL(EVRIdle);
    
    // Start the scheduler - Done only once !
    CActiveScheduler::Start();
    
    return iTestStepResult;
    }

/**
 * RTestVclntRecordPause::FsmL
 */
void RTestVclntRecordPause::FsmL(TVclntTestRecordEvents aEventCode)
    {
    TInt duration = 0;
    TTime stop = NULL;
    TTime start = NULL;
    
    if (FsmCheck(aEventCode))
        {
        iCurrentEvent = aEventCode;
        switch (aEventCode)
            {
            case EVRIdle:
                // Open iVideoRecorder
                INFO_PRINTF2(_L("iVideoRecorder->OpenFileL() %S"), &iFilename);
                TRAP(iError,iVideoRecorder->OpenFileL(iFilename, iCameraHandle, KMmfTestAviRecordControllerUid, KAviRecordFormatUid));
                PrepareState(EVROpenComplete, KErrNone);
                break;
            case EVROpenComplete:
                // Prepare iVideoRecorder
                INFO_PRINTF1(_L("iVideoRecorder->Prepare()"));
                iVideoRecorder->Prepare();
                PrepareState(EVRPrepareComplete, KErrNone);
                break;
            case EVRPrepareComplete:
                INFO_PRINTF1(_L("iVideoRecorder->SetAudioEnabledL(EFalse)"));
                iVideoRecorder->SetAudioEnabledL(EFalse);
              
                // Record iVideoRecorder
                INFO_PRINTF1(_L("iVideoRecorder->Record()"));
                iVideoRecorder->Record();
                  
				TRAPD(err, iRecordTimer = CTestVclntAviRecordTimer::NewL(this, 1000000));
				if (err != KErrNone)
				{
				ERR_PRINTF2(_L("CTestVclntAviRecordTimer::NewL left with error = %d"), err);
				iTestStepResult = EFail;
				CActiveScheduler::Stop();
				break;
				}
				iRecordTimer->Start();      

                PrepareState(EVRRecordPaused, KErrNone);
                break;
            case EVRRecordPaused:
            	// Cancel the timer
                iRecordTimer->Cancel();
                delete iRecordTimer;
                iRecordTimer = NULL;
                
                INFO_PRINTF1(_L("-- iVideoRecorder->PauseL();"));
                TRAP(err,iVideoRecorder->PauseL());
			    if (err != KErrNone)
			        {
			        INFO_PRINTF2(_L("Pause() returns with error :  %d"),err);
			        iTestStepResult = EFail;
			        CActiveScheduler::Stop();
			        }
		              
                TRAP(err, iRecordTimer = CTestVclntAviRecordTimer::NewL(this, 1000000));
                if (err != KErrNone)                       
					{
					ERR_PRINTF2(_L("CTestVclntAviRecordTimer::NewL left with error = %d"), err);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				iRecordTimer->Start();                          
                PrepareState(EVRRecordResumed, KErrNone);
                break; 
                
            case EVRRecordResumed:
            	// Cancel the timer
                iRecordTimer->Cancel();
                delete iRecordTimer;
                iRecordTimer = NULL;
            
            	// Resume Record iVideoRecorder
                INFO_PRINTF1(_L("iVideoRecorder->Record()"));
                iVideoRecorder->Record();            
				TRAP(err, iRecordTimer = CTestVclntAviRecordTimer::NewL(this, 1000000));
				if (err != KErrNone)
				{
				ERR_PRINTF2(_L("CTestVclntAviRecordTimer::NewL left with error = %d"), err);
				iTestStepResult = EFail;
				CActiveScheduler::Stop();
				break;
				}
				iRecordTimer->Start();      
                PrepareState(EVRRecordComplete, KErrNone);
                break; 
            case EVRRecordComplete:
            	
                // Stop iVideoRecorder
                INFO_PRINTF1(_L("iVideoRecorder->Stop()"));
                iVideoRecorder->Stop();                       
                // Get the duration of the file.
                duration = I64INT(iVideoRecorder->DurationL().Int64());
                if(TimeComparison(duration,2000000,KExpectedDeviation))
					{
					iTestStepResult = EPass;
					CActiveScheduler::Stop();
					break;                	
					}
                else
	                {
					iTestStepResult = EFail;
	            	CActiveScheduler::Stop();
                	break;		                	
	                }               
            }
        }
    }

/**
* set finished on callback from a timer
*
**/

void RTestVclntRecordPause::FinishedL()
	{
	if(iCurrentEvent == EVRPrepareComplete)
		{
		FsmL(EVRRecordPaused);	
		}
	else if(iCurrentEvent == EVRRecordPaused)
    	{
		FsmL(EVRRecordResumed);	
		}
	else if(iCurrentEvent == EVRRecordResumed)
		{
		FsmL(EVRRecordComplete);		
		}
		
	}
    
//
// CTestVclntAviRecordTimer added new class
//

/**
 * NewL
 */
CTestVclntAviRecordTimer* CTestVclntAviRecordTimer::NewL(RTestVclntRecordAviFile* aParent, TTimeIntervalMicroSeconds32 aWaitTime)
    {
    CTestVclntAviRecordTimer* self = new (ELeave) CTestVclntAviRecordTimer(aParent, aWaitTime);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;    
    }
    
/**
 * Constructor
 */
CTestVclntAviRecordTimer::CTestVclntAviRecordTimer(RTestVclntRecordAviFile* aParent, TTimeIntervalMicroSeconds32 aWaitTime)
    : CTimer(EPriorityNormal), iParent(aParent), iWaitTime(aWaitTime)
    {    
    CActiveScheduler::Add(this);
    }    
    
/**
 * Start() - Start the timer
 */
void CTestVclntAviRecordTimer::Start()
    {
    After(iWaitTime);
    }

/**
 * RunL() - see CActive class for more info
 */
void CTestVclntAviRecordTimer::RunL()
    {        
    // (iParent->Finished() should call Stop() or not, as we decide)    
    iParent->FinishedL();
    }

/**
 * RunError() - see CActive class for more info
 */
TInt CTestVclntAviRecordTimer::RunError(TInt aError)
    {
    CActiveScheduler::Stop();
    return aError;
    }


//
// RTestVclntRecordDuration
//

/**
 * RTestVclntRecordDuration::RTestVclntRecordDuration()
 */
RTestVclntRecordDuration::RTestVclntRecordDuration(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError, const TTimeIntervalMicroSeconds aDuration)
    : RTestVclntRecordAviFile(aTestName, aSectName, aKeyName, aExpectedError,EFalse)
    {
    iDuration = aDuration;
    }

/**
 * RTestVclntRecordDuration::NewL()
 */
RTestVclntRecordDuration* RTestVclntRecordDuration::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError, const TTimeIntervalMicroSeconds aDuration)
    {
    RTestVclntRecordDuration* self = new (ELeave) RTestVclntRecordDuration(aTestName, aSectName, aKeyName, aExpectedError, aDuration);
    return self;
    }

/**
 * RTestVclntRecordDuration::DoTestStepL()
 */
TVerdict RTestVclntRecordDuration::DoTestL(CVideoRecorderUtility* aRec)
    {
    TVerdict ret = EFail;

    INFO_PRINTF1(_L("Test : Video Recorder - Duration"));

    if (I64INT(iDuration.Int64()) == 0)
        {
        INFO_PRINTF1(_L("iVideoRecorder->Record()"));
        PrepareState(EVRRecordComplete, KErrNone);
        aRec->Record();
        TTime start;
        start.HomeTime();
        User::After(KFiveSeconds);
        aRec->Stop();
        TTime stop;
        stop.HomeTime();
        
        TInt duration = I64INT(aRec->DurationL().Int64());
        TUint actualDuration = I64INT(stop.MicroSecondsFrom(start).Int64());

        INFO_PRINTF6(_L("Error : %d Start = %d Stop = %d Duration = %d ActualDuration = %d"),
            iError, I64INT(start.Int64()), I64INT(stop.Int64()), duration, actualDuration);
        if((iError == KErrNone) && (TimeComparison(actualDuration, duration, KExpectedDeviation)))
            {
            ret = EPass;
            }
        }
    else
        {
        if(aRec->DurationL() == iDuration)
            {
            ret = EPass;
            }
        }
    
    return ret;
    }

//
// RTestVclntRecordTime
//

/**
 * RTestVclntRecordTime::RTestVclntRecordTime()
 */
RTestVclntRecordTime::RTestVclntRecordTime(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError, const TInt aTime)
    : RTestVclntRecordAviFile(aTestName, aSectName, aKeyName, aExpectedError,EFalse)
    {
    iTime = aTime;
    }

/**
 * RTestVclntRecordTime::NewL()
 */
RTestVclntRecordTime* RTestVclntRecordTime::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError, const TInt aTime)
    {
    RTestVclntRecordTime* self = new (ELeave) RTestVclntRecordTime(aTestName, aSectName, aKeyName, aExpectedError, aTime);
    return self;
    }

/**
 * RTestVclntRecordTime::DoTestL()
 */
TVerdict RTestVclntRecordTime::DoTestL(CVideoRecorderUtility* aRec)
    {
    TVerdict ret = EPass;
    TTimeIntervalMicroSeconds recTime(0);
    TTimeIntervalMicroSeconds recTime1(0);

    INFO_PRINTF1(_L("Test : Video Recorder - RecordTimeAvailable()"));
    
    recTime = aRec->RecordTimeAvailable();
    
    TRAPD(err, aRec->SetMaxClipSizeL(800));
    if (err != KErrNone)
        {
        INFO_PRINTF2(_L("SetMaxClipSizeL() returned %d"), err);
        ret = EFail;
        }

    // The AVI record controller returns KErrNotSupported. but that can't be captured here...
    if (recTime != recTime1)
        {
        INFO_PRINTF1(_L("This feature is not supported presently. RecordTimeAvailable() returned 0"));
        ret = EPass;
        }
    else
        {
        INFO_PRINTF2(_L("RecordTimeAvailable() returned %d"), I64INT(recTime.Int64()));
        ret = EFail;
        }
    
    return ret;
    }


//
// RTestVclntRecordGain
//

/**
 * RTestVclntRecordGain::RTestVclntRecordGain()
 */
RTestVclntRecordGain::RTestVclntRecordGain(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError, const TInt aGain)
    : RTestVclntRecordAviFile(aTestName, aSectName, aKeyName, aExpectedError,EFalse)
    {
    iGain = aGain;
    }

/**
 * RTestVclntRecordGain::NewL()
 */
RTestVclntRecordGain* RTestVclntRecordGain::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError, const TInt aGain)
    {
    RTestVclntRecordGain* self = new (ELeave) RTestVclntRecordGain(aTestName, aSectName, aKeyName, aExpectedError, aGain);
    return self;
    }

/**
 * RTestVclntRecordGain::DoTestL()
 */
TVerdict RTestVclntRecordGain::DoTestL(CVideoRecorderUtility* aRec)
    {
    INFO_PRINTF1(_L("Test : Video Recorder - Gain"));

    TVerdict ret = EFail;
    TInt err = KErrNone;
    
    // enable audio while recording
    TRAP(err, aRec->SetAudioEnabledL(ETrue));
    INFO_PRINTF2(_L("SetAudioEnabledL(ETrue) left with error %d"), err);
    if((err != KErrNone) && (err != KErrNotSupported))
        {
        return EFail;
        }
    
    // set gain to maximum
    if(iGain == 0)
        {
        TRAP(err, iGain = aRec->MaxGainL());
        INFO_PRINTF2(_L("MaxGainL() left with error %d"), err);
        if((err != KErrNone) && (err != KErrNotSupported))
            {
            return EFail;
            }
        }
    
    // set gain to value passed (iGain)
    TRAP(err, aRec->SetGainL(iGain));
    INFO_PRINTF3(_L("SetGainL(%d) left with error %d"), iGain, err);
    if ((iTestExpectedError != KErrNone) && (err == iTestExpectedError))
        {
        return EPass;
        }
    else if((err != KErrNone) && (err != KErrNotSupported))
        {
        return EFail;
        }
    
    // Get the gain value
    TInt newGain = 0;
    TRAP(err, newGain = aRec->GainL());
    INFO_PRINTF2(_L("GainL() left with error %d"), err);
    if((err != KErrNone) && (err != KErrNotSupported))
        {
        ret = EFail;
        }
    
    // alter the expected values (truncated)
    if(iGain < 0)
        {
        iGain = 0;
        }
    else if (iGain > aRec->MaxGainL())
        {
        iGain = aRec->MaxGainL();
        }
    
    // check for the expected values
    if(iGain == newGain)
        {
        INFO_PRINTF3(_L("set (%d) and get (%d) gain values match"), iGain, newGain);
        ret = EPass;
        }
    
    return ret;
    }

//
// RTestVclntRecordMeta
//

/**
 * RTestVclntRecordMeta::DoTestStepL()
 */
RTestVclntRecordMeta::RTestVclntRecordMeta(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError)
    : RTestVclntRecordAviFile(aTestName, aSectName, aKeyName, aExpectedError,EFalse)
    {}

/**
 * RTestVclntRecordMeta::NewL()
 */
RTestVclntRecordMeta* RTestVclntRecordMeta::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError)
    {
    RTestVclntRecordMeta* self = new (ELeave) RTestVclntRecordMeta(aTestName, aSectName, aKeyName, aExpectedError);
    return self;
    }

/**
 * RTestVclntRecordMeta::DoTestL()
 */
TVerdict RTestVclntRecordMeta::DoTestL(CVideoRecorderUtility* aRec)
    {
    INFO_PRINTF1(_L("Test : Video Recorder - Metadata"));

    TInt numEntries = 0;
    
    // Get the Number of MetaData Entries
    TRAPD(err, numEntries = aRec->NumberOfMetaDataEntriesL() );
    INFO_PRINTF3(_L("NumberOfMetaDataEntriesL() returned Error = %d, NumEntries = %d>"), err, numEntries);
    if (err != KErrNotSupported || numEntries != 0)
        {
        return EFail;
        }

    // Get the first meta data entry
    CMMFMetaDataEntry* theEntry=NULL;
    TRAP(err, theEntry = aRec->MetaDataEntryL(1));
    INFO_PRINTF2(_L("Get entry : Error : %d"), err);
    if ((err != KErrNotSupported) && (theEntry != NULL))
        {
        return EFail;
        }
    else
        {
        delete theEntry;
        }

    // Create Meta Entries
    _LIT(name,"<Copyright>");
    _LIT(data,"<Symbian (c) 2002>");
    CMMFMetaDataEntry* metaData = NULL;
    metaData = CMMFMetaDataEntry::NewL(name,data);
    CleanupStack::PushL(metaData);
    
    // Add the meta data entry
    TRAP(err, aRec->AddMetaDataEntryL(*metaData));    // not supported in test controller
    INFO_PRINTF2(_L("AddMetaDataEntryL() returned : %d"), err);
    if(err != KErrNotSupported)
        {
        return EFail;
        }
        
    // Remove the meta data entry
    TRAP(err, aRec->RemoveMetaDataEntryL(0));    // not supported in test controller
    INFO_PRINTF2(_L("RemoveMetaDataEntryL() returned : %d"), err);
    if(err != KErrNotSupported)
        {
        return EFail;
        }
    
    // Replace the meta data entry
    TRAP(err, aRec->ReplaceMetaDataEntryL(0, *metaData));    // not supported in test controller
    INFO_PRINTF2(_L("ReplaceMetaDataEntryL() returned : %d"), err);
    if(err != KErrNotSupported)
        {
        return EFail;
        }

    CleanupStack::PopAndDestroy(metaData);
    return EPass;
    }

//
// RTestVclntRecordPriority
//

/**
 * RTestVclntRecordPriority::RTestVclntRecordPriority()
 */
RTestVclntRecordPriority::RTestVclntRecordPriority(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError, const TInt aPriority)
    : RTestVclntRecordAviFile(aTestName, aSectName, aKeyName, aExpectedError,EFalse)
    {
    iPriority = aPriority;
    }

/**
 * RTestVclntRecordPriority::NewL()
 */
RTestVclntRecordPriority* RTestVclntRecordPriority::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError, const TInt aPriority)
    {
    RTestVclntRecordPriority* self = new (ELeave) RTestVclntRecordPriority(aTestName, aSectName, aKeyName, aExpectedError, aPriority);
    return self;
    }

/**
 * RTestVclntRecordPriority::DoTestL()
 */
TVerdict RTestVclntRecordPriority::DoTestL(CVideoRecorderUtility* aRec)
    {
    TVerdict ret = EFail;

    INFO_PRINTF1(_L("Test : Video Recorder - Priority"));

    aRec->SetPriorityL(iPriority, EMdaPriorityPreferenceNone);    
    TInt thePriority;
    TMdaPriorityPreference thePref;
    aRec->GetPriorityL(thePriority, thePref);
    INFO_PRINTF3(_L("Priority = %d (expecting %d)"), thePriority, iPriority);
    INFO_PRINTF3(_L("Pref = %d (expecting %d)"), thePref, EMdaPriorityPreferenceNone);
    if( (thePriority == iPriority) && (thePref == EMdaPriorityPreferenceNone) )
        {
        ret = EPass;
        }
        
    return ret;
    }


//
// RTestVclntRecordSize
//

/**
 * RTestVclntRecordSize::RTestVclntRecordSize()
 */
RTestVclntRecordSize::RTestVclntRecordSize(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError)
    : RTestVclntRecordAviFile(aTestName, aSectName, aKeyName, aExpectedError,EFalse)
    {}

/**
 * RTestVclntRecordSize::NewL()
 */
RTestVclntRecordSize* RTestVclntRecordSize::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError)
    {
    RTestVclntRecordSize* self = new (ELeave) RTestVclntRecordSize(aTestName, aSectName, aKeyName, aExpectedError);
    return self;
    }

/**
 * RTestVclntRecordSize::DoTestL()
 */
TVerdict RTestVclntRecordSize::DoTestL(CVideoRecorderUtility* aRec)
    {
    INFO_PRINTF1(_L("Test : Video Recorder - SetMaxClipSize"));
    
    TVerdict ret = EFail;
    
    // call max clip size with 'no max clip size' value
    TRAPD(err, aRec->SetMaxClipSizeL( KMMFNoMaxClipSize ));
    INFO_PRINTF2(_L("SetMaxClipSizeL() left with error : %d"), err);
    if (err != KErrArgument)
        {
        ret = EFail;
        }
    else
        {
        ret = EPass;
        }

    return ret;
    }

//
// RTestVclntRecordTypes
//

/**
 * RTestVclntRecordTypes::RTestVclntRecordTypes()
 */
RTestVclntRecordTypes::RTestVclntRecordTypes(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError)
    : RTestVclntRecordAviFile(aTestName, aSectName, aKeyName, aExpectedError,EFalse)
    {}

/**
 * RTestVclntRecordTypes::NewL()
 */
RTestVclntRecordTypes* RTestVclntRecordTypes::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError)
    {
    RTestVclntRecordTypes* self = new (ELeave) RTestVclntRecordTypes(aTestName, aSectName, aKeyName, aExpectedError);
    return self;
    }

/**
 * RTestVclntRecordTypes::DoTestL()
 */
TVerdict RTestVclntRecordTypes::DoTestL(CVideoRecorderUtility* aRec)
    {
    TInt cumulErr = 0;
    TInt i;
    INFO_PRINTF1(_L("Test : Video Recorder - Audio/Video Types"));
    
    // GetSupportedAudioTypesL
    RArray<TFourCC> theAudioTypes;
    TRAPD(err, aRec->GetSupportedAudioTypesL(theAudioTypes));
    INFO_PRINTF2(_L("iVideoRecorder->GetSupportedAudioTypesL() : Error = %d"), err);
    if(err != KErrNone && err != KErrNotSupported)
        {
        cumulErr++;
        }
        
    // GetSupportedVideoTypesL
    CDesC8ArrayFlat* theVideoTypes = new (ELeave) CDesC8ArrayFlat(KBufMimeTypeGranularity);
    CleanupStack::PushL(theVideoTypes);
    TRAP(err, aRec->GetSupportedVideoTypesL(*theVideoTypes));
    INFO_PRINTF2(_L("iVideoRecorder->GetSupportedVideoTypesL() : Error = %d"), err);
    if(err != KErrNone && err != KErrNotSupported)
        {
        cumulErr++;
        }
    else
        {
        for (i = 0; i < theVideoTypes->Count(); i++)
            {
            TBuf<64> temp;
            temp.Copy((*theVideoTypes)[i]);
            INFO_PRINTF3(_L("Video Mime Type #%d = %S"), i, &temp);
            }
        }
    CleanupStack::PopAndDestroy(theVideoTypes);
    
    // SetAudioTypeL
    TRAP(err,aRec->SetAudioTypeL(KMMFFourCCCodePCMU16));
    INFO_PRINTF2(_L("iVideoRecorder->SetAudioTypeL() : Error = %d"), err);
    if(err != KErrNone && err != KErrNotSupported)
        {
        cumulErr++;
        }
    
    // SetVideoTypeL
    _LIT8(KAviVideoCodec,"XVID");
    TRAP(err, aRec->SetVideoTypeL(KAviVideoCodec));
    INFO_PRINTF2(_L("iVideoRecorder->SetVideoTypeL() : Error = %d"), err);
    if(err != KErrNone && err != KErrNotSupported)
        {
        cumulErr++;
        }
    
    // SetAudioEnabledL
    TBool enabled = ETrue;
    TRAP(err, aRec->SetAudioEnabledL(enabled));
    INFO_PRINTF2(_L("iVideoRecorder->SetAudioEnabledL() : Error = %d"), err);
    if(err != KErrNone && err != KErrNotSupported)
        {
        cumulErr++;
        }
    
    // Ensure AudioEnabled is Set Correctly
    TBool checkEnabled = EFalse;
    TRAP(err, checkEnabled = aRec->AudioEnabledL());
    if (!checkEnabled)
        {
        INFO_PRINTF2(_L("iVideoRecorder->AudioEnabledL() < Error = %d, Audio is NOT Enabled >"), err);
        cumulErr++;
        }

    // Querying the record formats supported
    INFO_PRINTF1(_L("<<< Querying record formats >>>"));

    CMMFControllerPluginSelectionParameters* cSelect = CMMFControllerPluginSelectionParameters::NewLC();
    CMMFFormatSelectionParameters* fSelect = CMMFFormatSelectionParameters::NewLC();

    RArray<TUid> mediaIds;
    mediaIds.Append(KUidMediaTypeVideo);
    cSelect->SetMediaIdsL(mediaIds, CMMFPluginSelectionParameters::EAllowOtherMediaIds);
    cSelect->SetRequiredRecordFormatSupportL(*fSelect); 

    RMMFControllerImplInfoArray controllers; // Array to hold all the controllers support the match data
    CleanupResetAndDestroyPushL(controllers);
    cSelect->ListImplementationsL(controllers);  // Populates the array with all the suitable controllers
    TInt numControllers = controllers.Count();
    if(!numControllers)
        {
        ERR_PRINTF1(_L("Could not find any controllers"));
        cumulErr++;
        }
    else
        {
        INFO_PRINTF2(_L("Found %d controllers"), numControllers);
        for(int i = 0; i < numControllers; i++)
            {
            RMMFFormatImplInfoArray cFormats = controllers[i]->RecordFormats();
            // NB : a controller may have no record formats
            if(!cFormats.Count())
                {
                INFO_PRINTF2(_L("- Controller: %d supports no record formats"), i);
                }
            else
                {
                const CDesC8Array* fileExtensions = &cFormats[0]->SupportedMimeTypes();
                const TDesC& dispName = controllers[i]->DisplayName();
                INFO_PRINTF2(_L("--- Controller#%d"), i);
                INFO_PRINTF2(_L("---- DisplayName = '%S'"), &dispName);
                for(int j = 0; j < fileExtensions->Count(); j++)
                    {
                    TBuf<24> ext16;
                    ext16.Copy(fileExtensions->MdcaPoint(j));
                    INFO_PRINTF3(_L("----Mime#%d = '%S'"), j, &ext16);
                    }
                }
            }
        }
    CleanupStack::PopAndDestroy(3);//controllers, fSelect, cSelect

    // Querying the play formats supported
    INFO_PRINTF1(_L("<<< Querying play formats >>>"));

    cSelect = CMMFControllerPluginSelectionParameters::NewLC();
    fSelect = CMMFFormatSelectionParameters::NewLC();

    cSelect->SetMediaIdsL(mediaIds, CMMFPluginSelectionParameters::EAllowOtherMediaIds);
    cSelect->SetRequiredPlayFormatSupportL(*fSelect); 

    CleanupResetAndDestroyPushL(controllers);
    cSelect->ListImplementationsL(controllers);  // Populates the array with all the suitable controllers
    numControllers = controllers.Count();
    if(!numControllers)
        {
        ERR_PRINTF1(_L("Could not find any controllers"));
        cumulErr++;
        }
    else
        {
        INFO_PRINTF2(_L("Found %d controllers"), numControllers);
        for(int i = 0; i < numControllers; i++)
            {
            RMMFFormatImplInfoArray cFormats = controllers[i]->PlayFormats();
            // NB : a controller may have no play formats
            if(!cFormats.Count())
                {
                INFO_PRINTF2(_L("- Controller: %d supports no play formats"), i);
                }
            else
                {
                const CDesC8Array* fileExtensions = &cFormats[0]->SupportedMimeTypes();
                const TDesC& dispName = controllers[i]->DisplayName();
                INFO_PRINTF2(_L("--- Controller#%d"), i);
                INFO_PRINTF2(_L("---- DisplayName = '%S'"), &dispName);
                for(int j = 0; j < fileExtensions->Count(); j++)
                    {
                    TBuf<24> ext16;
                    ext16.Copy(fileExtensions->MdcaPoint(j));
                    INFO_PRINTF3(_L("----Mime#%d = '%S'"), j, &ext16);
                    }
                }
            }
        }

    CleanupStack::PopAndDestroy(3);//controllers, fSelect, cSelect
    
    return (cumulErr ? EFail : EPass);
    }

//
// RTestVclntRecordDelete
//

/**
 * RTestVclntRecordDelete::RTestVclntRecordDelete()
 */
RTestVclntRecordDelete::RTestVclntRecordDelete(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError)
    : RTestVclntRecordAviFile(aTestName, aSectName, aKeyName, aExpectedError,EFalse)
    {}

/**
 * RTestVclntRecordDelete::NewL()
 */
RTestVclntRecordDelete* RTestVclntRecordDelete::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError)
    {
    RTestVclntRecordDelete* self = new (ELeave) RTestVclntRecordDelete(aTestName, aSectName, aKeyName, aExpectedError);
    return self;
    }

/**
 * RTestVclntRecordDelete::DoTestStepL
 */
TVerdict RTestVclntRecordDelete::DoTestStepL()
    {
    // Call the state handler from IDLE state
    FsmL(EVRIdle);
    
    // Start the scheduler - Done only once !
    CActiveScheduler::Start();
    
    return iTestStepResult;
    }

/**
 * RTestVclntRecordDelete::FsmL
 */
void RTestVclntRecordDelete::FsmL(TVclntTestRecordEvents aEventCode)
    {
    if (FsmCheck(aEventCode))
        {
        switch (aEventCode)
            {
            case EVRIdle:
                // Open iVideoRecorder
                INFO_PRINTF2(_L("iVideoRecorder->OpenFileL()"), &iFilename);
                TRAP(iError,iVideoRecorder->OpenFileL(iFilename, iCameraHandle, KMmfTestAviRecordControllerUid, KAviRecordFormatUid));
                PrepareState(EVROpenComplete, KErrNone);
                break;
            case EVROpenComplete:
                // Prepare iVideoRecorder
                INFO_PRINTF1(_L("iVideoRecorder->Prepare()"));
                iVideoRecorder->Prepare();
                PrepareState(EVRPrepareComplete, KErrNone);    
                break;
            case EVRPrepareComplete:
                // Record iVideoRecorder
                INFO_PRINTF1(_L("iVideoRecorder->Record()"));
                iVideoRecorder->Record();
                PrepareState(EVRRecordComplete, KErrNone);
                // wait for 5 secs
                User::After(KFiveSeconds);
                INFO_PRINTF1(_L("delete iVideoRecorder before Record completes"));
                delete iVideoRecorder; // destroy iVideoRecorder before Record has completed
                iVideoRecorder = NULL;
                CActiveScheduler::Stop();
                iTestStepResult = EPass;
                break;
            }
        }
    }

//
// RTestVclntRecordAviRFile
//

/**
 * RTestVclntRecordAviRFile::Constructor
 */
RTestVclntRecordAviRFile::RTestVclntRecordAviRFile(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError)
    : RTestVclntAviRecorderStep(aTestName, aSectName, aKeyName, aExpectedError)
    {
    }

/**
 * RTestVclntRecordAviRFile::NewL
 */
RTestVclntRecordAviRFile* RTestVclntRecordAviRFile::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError)
    {
    RTestVclntRecordAviRFile* self = new (ELeave) RTestVclntRecordAviRFile(aTestName, aSectName, aKeyName, aExpectedError);
    return self;
    }

/**
 * RTestVclntRecordAviRFile::NewLC
 */
RTestVclntRecordAviRFile* RTestVclntRecordAviRFile::NewLC(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError)
    {
    RTestVclntRecordAviRFile* self = RTestVclntRecordAviRFile::NewL(aTestName, aSectName, aKeyName, aExpectedError);
    CleanupStack::PushL(self);
    return self;
    }


/**
 * RTestVclntRecordAviRFile::DoTestStepL
 */
TVerdict RTestVclntRecordAviRFile::DoTestStepL()
    {
    INFO_PRINTF1(_L("Test : Video Recorder - OpenFileL(RFile&)"));
    
    // Connect to RFs
    User::LeaveIfError(iFs.Connect());
    User::LeaveIfError(iFs.ShareProtected());
    
    // Open RFile
    User::LeaveIfError(iFile.Replace(iFs, iFilename, EFileRead));
    
    // Call the state handler from IDLE state
    FsmL(EVRIdle);
    
    // Start the scheduler - Done only once !
    CActiveScheduler::Start();
    
    iFile.Close();
    iFs.Close();

    return iTestStepResult;
    }

/**
 * RTestVclntRecordAviRFile::FsmL()
 */
void RTestVclntRecordAviRFile::FsmL(TVclntTestRecordEvents aEventCode)
    {
    if (FsmCheck(aEventCode))
        {
        switch (aEventCode)
            {
            case EVRIdle:
                // Open iVideoRecorder
                INFO_PRINTF2(_L("iVideoRecorder->OpenFileL() %S"), &iFilename);
                TRAP(iError, iVideoRecorder->OpenFileL(iFile, iCameraHandle, KMmfTestAviRecordControllerUid, KAviRecordFormatUid));
                User::LeaveIfError(iError);
                PrepareState(EVROpenComplete, KErrNone);
                break;
            case EVROpenComplete:
                // Prepare iVideoRecorder
                INFO_PRINTF1(_L("iVideoRecorder->Prepare()"));
                PrepareState(EVRPrepareComplete, KErrNone);
                iVideoRecorder->Prepare();
                break;
            case EVRPrepareComplete:
                // Record iVideoRecorder
                INFO_PRINTF1(_L("iVideoRecorder->Record()"));
                iVideoRecorder->Record();
                PrepareState(EVRRecordComplete, KErrNone);
                User::After(KFiveSeconds);
                iVideoRecorder->Stop();
                // close
                iVideoRecorder->Close();
                CActiveScheduler::Stop();
                iTestStepResult = EPass;
                break;
            }
        }
    }

//
// RTestVclntRecordAviRFileAllParams
//

/**
 * RTestVclntRecordAviRFileAllParams::Constructor
 */
RTestVclntRecordAviRFileAllParams::RTestVclntRecordAviRFileAllParams(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError, TBool aAlloc, TUid aControllerUid, TUid aVideoFormat, TFourCC aAudioType)
    : RTestVclntAviRecorderStep(aTestName, aSectName, aKeyName, aExpectedError), iAlloc(aAlloc), iControllerUid(aControllerUid), iVideoFormat(aVideoFormat), iAudioType(aAudioType)
    {
    iHeapSize = 100000;
    }

/**
 * RTestVclntRecordAviRFileAllParams::NewL
 */
RTestVclntRecordAviRFileAllParams* RTestVclntRecordAviRFileAllParams::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError,const TBool aAlloc, TUid aControllerUid, TUid aVideoFormat, TFourCC aAudioType)
    {
    RTestVclntRecordAviRFileAllParams* self = new (ELeave) RTestVclntRecordAviRFileAllParams(aTestName, aSectName, aKeyName, aExpectedError, aAlloc, aControllerUid, aVideoFormat, aAudioType);
    return self;
    }

/**
 * RTestVclntRecordAviRFileAllParams::NewLC
 */
RTestVclntRecordAviRFileAllParams* RTestVclntRecordAviRFileAllParams::NewLC(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError,const TBool aAlloc, TUid aControllerUid, TUid aVideoFormat, TFourCC aAudioType)
    {
    RTestVclntRecordAviRFileAllParams* self = RTestVclntRecordAviRFileAllParams::NewL(aTestName, aSectName, aKeyName, aExpectedError, aAlloc, aControllerUid, aVideoFormat, aAudioType);
    CleanupStack::PushL(self);
    return self;
    }

/**
 * RTestVclntRecordAviRFileAllParams::DoTestStepL
 */
TVerdict RTestVclntRecordAviRFileAllParams::DoTestStepL()
    {
	if(SetCacheSize()!=KErrNone)
    	{
    	return EInconclusive;
    	}

    if (iAlloc)
        {
        TVerdict iAllocTestStepResult=EPass;
        TInt err = KErrNone;
        TBool result = EFalse;

        //>>>>>>>>>>>>>>>>>>>>>>>>Test Method Call<<<<<<<<<<<<<<<<<<<<<<<<<<
        TRAP(err, iAllocTestStepResult = PerformTestStepL())
        if (err == KErrNone && iAllocTestStepResult != EPass)
            {
            err = iError;
            }
        
        if (err != KErrNone)
            {
            INFO_PRINTF2(_L("Test error, returned error code =  %d"), err);
            User::Leave(err);
            }
        else
            {
            //Check the iAllocTestStepResult
            if (iAllocTestStepResult != EPass)
                {
                result = ETrue;
                }
            }    

        TInt failCount = 1;
        TBool completed = EFalse;
        iAllocTestStepResult = EPass; // XXX check?? assume pass
        TBool reachedEnd = EFalse; // Note: declare outside loop to help with debugging
        for(;;)    
            {
            __UHEAP_SETFAIL(RHeap::EFailNext, failCount);
            __MM_HEAP_MARK;
            TInt s = User::CountAllocCells();

            //INFO_PRINTF2(_L("CVideoRecorderUtility: Alloc Test Loop: %d"), failCount);
            
            //>>>>>>>>>>>>>>>>>>>>>>>>Test Method Call<<<<<<<<<<<<<<<<<<<<<<<<<<
            TVerdict verdict = EFail;
            TRAP(err, verdict = PerformTestStepL())
            if (err == KErrNone && verdict != EPass)
                {
                err = iError;
                }

            completed = EFalse;
            if (err == KErrNone)
                {
                TAny *testAlloc = User::Alloc(1); // when this fails, we passed through all allocs within test
                if (testAlloc == NULL)
                    {
                    reachedEnd = ETrue;
                    failCount -= 1;
                    }
                else
                    {
                    User::Free(testAlloc);    
                    }            
                
                //Check the iAllocTestStepResult
                if (iAllocTestStepResult != EPass)
                    {
                    result = ETrue;
                    }
                
                completed = reachedEnd || result;
                }
            else if (err != KErrNoMemory) // bad error code
                {
                completed = ETrue;
                result = EFail;
                }            
        s = User::CountAllocCells();
        	delete iVideoRecorder;
        	iVideoRecorder = NULL;
            __MM_HEAP_MARKEND;
            __UHEAP_SETFAIL(RHeap::ENone, 0);

            if (completed)
                {
                break; // exit loop
                }

            failCount++;
            }

        failCount -= 1; // Failcount of 1 equates to 0 successful allocs, etc

        if (err != KErrNone || result)
            {
            iAllocTestStepResult = EFail;
            TBuf<80> format;
            if (result)
                {
                format.Format(_L("  Bad result with %d memory allocations tested\n"), failCount);
                }
            else
                {
                format.Format(_L("  Error(%d) with %d memory allocations tested\n"), err, failCount);
                }
            Log(format);
            }
        else 
            {
            TBuf<80> format;
            format.Format(_L("  Completed OK with %d memory allocations tested\n"), failCount);
            Log(format);
            }

    	TUint defaultCacheSize = 0; 
    	//Done with the test. Setting 0 makes the cache size revert back to boot up values
    	TInt error=UserSvr::HalFunction(EHalGroupVM,EVMHalSetCacheSize,(TAny*)defaultCacheSize,(TAny*)defaultCacheSize);
        if(CheckCacheError(error)!=KErrNone)
        	{
        	INFO_PRINTF1(_L("Could not revert the cache size to default"));
        	iAllocTestStepResult=EInconclusive;
        	}

        return iAllocTestStepResult;
        }
    else
        {
        return( PerformTestStepL() );
        }
    }
    
/**
 * RTestVclntRecordAviRFileAllParams::PerformTestStepL
 */
TVerdict RTestVclntRecordAviRFileAllParams::PerformTestStepL()
    {
    INFO_PRINTF1(_L("Test : Video Recorder - OpenFileL(RFile&)"));
    
    // delete CVideoRecorderUtility object, and recreate
    delete iVideoRecorder;
    iVideoRecorder = NULL;
        
    iVideoRecorder = CVideoRecorderUtility::NewL(*this);
        
    // Connect to RFs
    User::LeaveIfError(iFs.Connect());
    User::LeaveIfError(iFs.ShareProtected());
    CleanupClosePushL(iFs);
    
    // Open RFile
    User::LeaveIfError(iFile.Replace(iFs, iFilename, EFileRead));
    CleanupClosePushL(iFile);
    
    // Call the state handler from IDLE state
    iError = KErrNone;
    PrepareState(EVRIdle, KErrNone);
    FsmL(EVRIdle);
    
    // Start the scheduler - Done only once !
    CActiveScheduler::Start();
    
    CleanupStack::PopAndDestroy(2);
    
    // Leave if the Expected error is KErrNone    
    if (iTestExpectedError == KErrNone)
        {
        User::LeaveIfError(iError);    
        }
    
    return iTestStepResult;
    }

/**
 * RTestVclntRecordAviRFileAllParams::FsmL()
 */
void RTestVclntRecordAviRFileAllParams::FsmL(TVclntTestRecordEvents aEventCode)
    {
    if (FsmCheck(aEventCode))
        {
        switch (aEventCode)
            {
            case EVRIdle:
                // Open iVideoRecorder
                INFO_PRINTF2(_L("iVideoRecorder->OpenFileL() %S"), &iFilename);
                TRAP(iError,iVideoRecorder->OpenFileL(iFile, iCameraHandle, KMmfTestAviRecordControllerUid, KAviRecordFormatUid, KNullDesC8, KMMFFourCCCodeNULL));
                User::LeaveIfError(iError);
                PrepareState(EVROpenComplete, KErrNone);
                break;
            case EVROpenComplete:
                // Prepare iVideoRecorder
                INFO_PRINTF1(_L("iVideoRecorder->Prepare()"));
                PrepareState(EVRPrepareComplete, KErrNone);
                iVideoRecorder->Prepare();
                break;
            case EVRPrepareComplete:
                // Record iVideoRecorder
                INFO_PRINTF1(_L("iVideoRecorder->Record()"));
                iVideoRecorder->Record();
                PrepareState(EVRRecordComplete, KErrNone);
                User::After(KFiveSeconds);
                iVideoRecorder->Stop();
                // close
                iVideoRecorder->Close();
                CActiveScheduler::Stop();
                iTestStepResult = EPass;
                break;
            }
        }
    }

TInt RTestVclntRecordAviRFileAllParams::SetCacheSize()
	{
	TInt error=UserSvr::HalFunction(EHalGroupVM,EVMHalSetCacheSize,(TAny*)KCacheSize,(TAny*)KMaxCacheSize);
	TInt ret=CheckCacheError(error);
		
	return ret;
	}

TInt RTestVclntRecordAviRFileAllParams::CheckCacheError(TInt aError)
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


//
// CTestVclntAviRecordSetVideoEnabled
//

/**
CTestVclntAviRecordSetVideoEnabled::Constructor
*/
CTestVclntAviRecordSetVideoEnabled::CTestVclntAviRecordSetVideoEnabled(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError)
    : RTestVclntAviRecorderStep(aTestName, aSectName, aKeyName, aExpectedError)
    {
    }

/**
CTestVclntAviRecordSetVideoEnabled::NewL
*/
CTestVclntAviRecordSetVideoEnabled* CTestVclntAviRecordSetVideoEnabled::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError)
	{
	CTestVclntAviRecordSetVideoEnabled* self = new (ELeave) CTestVclntAviRecordSetVideoEnabled(aTestName, aSectName, aKeyName, aExpectedError);
	return self;
	}
/**
CTestVclntAviRecordSetVideoEnabled::DoTestStepL
*/
TVerdict CTestVclntAviRecordSetVideoEnabled::DoTestStepL()
    {
    INFO_PRINTF1(_L("Test : Video Recorder - OpenFileL(RFile&)"));
    
    // Connect to RFs
    User::LeaveIfError(iFs.Connect());
    User::LeaveIfError(iFs.ShareProtected());
    
    // Open RFile
    User::LeaveIfError(iFile.Replace(iFs, iFilename, EFileRead));
    
    // Call the state handler from IDLE state
    FsmL(EVRIdle);
    
    // Start the scheduler - Done only once !
    CActiveScheduler::Start();
    
    iFile.Close();
    iFs.Close();

    return iTestStepResult;
    }

/**
CTestVclntAviRecordSetVideoEnabled::FsmL()
*/
void CTestVclntAviRecordSetVideoEnabled::FsmL(TVclntTestRecordEvents aEventCode)
    {
    if (FsmCheck(aEventCode))
        {
        TBool failTestFlag = EFalse;
        TInt err;
        switch (aEventCode)
            {
            case EVRIdle:
	        	// Calling SetVideoEnabledL before opening file should return KErrNotReady
	            INFO_PRINTF1(_L("calling iVideoRecorder->SetVideoEnabledL()"));
	            TRAP(err, iVideoRecorder->SetVideoEnabledL(EFalse));
	            if(err != KErrNotReady)
	            	{
	                INFO_PRINTF2(_L("SetVideoEnabledL() returned wrong error=%d"),err);
		            failTestFlag = ETrue;		            
	                break;
	            	}
	
	            // Calling VideoEnabledL before opening file should return KErrNotReady
	            INFO_PRINTF1(_L("calling iVideoRecorder->VideoEnabledL()"));
	            TRAP(err, iVideoRecorder->VideoEnabledL());
	            if(err != KErrNotReady)
	            	{
	                INFO_PRINTF2(_L("VideoEnabledL() returned wrong error=%d"),err);
		            failTestFlag = ETrue;		            
	                break;	            	
	            	}
            
                // Open iVideoRecorder
                INFO_PRINTF2(_L("iVideoRecorder->OpenFileL() %S"), &iFilename);
                // If extension of file is .dummy open file on test controller.
                if( KErrNotFound == iFilename.Find(KDummyExtension) )
                	{
                	TRAP(iError, iVideoRecorder->OpenFileL(iFile, iCameraHandle, KMmfTestAviRecordControllerUid, KAviRecordFormatUid));
                	}
                else
                	{
                	TRAP(iError, iVideoRecorder->OpenFileL(iFile, NULL, KMmfVideoTestControllerUid, KUidMdaBmpClipFormat));                	
                	}
                User::LeaveIfError(iError);
                PrepareState(EVROpenComplete, KErrNone);
                break;
            case EVROpenComplete:
            	TBool videoEnableFlag;
            	
            	// Check that by default video is enabled.
	            INFO_PRINTF1(_L("calling iVideoRecorder->VideoEnabledL()"));
	            TRAP(err, videoEnableFlag = iVideoRecorder->VideoEnabledL());
	            if( (videoEnableFlag != (TBool)ETrue) || (err != KErrNone) )
	            	{
	            	INFO_PRINTF2(_L("VideoEnabledL returned err=%d"),err);	            	
		            INFO_PRINTF2(_L("VideoEnabledL() returned %d"),videoEnableFlag);
		            failTestFlag = ETrue;		            
	                break;
	            	}
	            
            	// Try to disable video.
	            INFO_PRINTF1(_L("calling iVideoRecorder->SetVideoEnabledL()"));
	            TRAP(err, iVideoRecorder->SetVideoEnabledL(EFalse));
	            if(err == KErrNotSupported)
	            	{
	            	INFO_PRINTF1(_L("SetVideoEnabledL KErrNotSupported"));	            	
	            	}
	            else if(err != KErrNone)
	            	{
	            	INFO_PRINTF2(_L("VideoEnabledL returned err=%d"),err);	            		            	
		            failTestFlag = ETrue;		            
	                break;
	            	}
	            else // success case
	            	{
		            INFO_PRINTF1(_L("calling iVideoRecorder->VideoEnabledL()"));
		            TRAP(err, videoEnableFlag = iVideoRecorder->VideoEnabledL());
		            if( (videoEnableFlag != EFalse) || (err != KErrNone) )
		            	{
		            	INFO_PRINTF2(_L("VideoEnabledL returned err=%d"),err);	            	
			            INFO_PRINTF2(_L("VideoEnabledL() returned %d"),videoEnableFlag);
			            failTestFlag = ETrue;		            
		                break;
		            	}
	            	}

	            // Prepare iVideoRecorder
                INFO_PRINTF1(_L("iVideoRecorder->Prepare()"));
                PrepareState(EVRPrepareComplete, KErrNone);
                iVideoRecorder->Prepare();
                break;
            case EVRPrepareComplete:
                // Record iVideoRecorder
                INFO_PRINTF1(_L("iVideoRecorder->Record()"));
                iVideoRecorder->Record();
                PrepareState(EVRRecordComplete, KErrNone);
                User::After(KFiveSeconds);
	        	
                // Calling SetVideoEnabledL while recording should return KErrNotReady
	            INFO_PRINTF1(_L("calling iVideoRecorder->SetVideoEnabledL()"));
	            TRAP(err, iVideoRecorder->SetVideoEnabledL(ETrue));
	            if(err != KErrNotReady)
	            	{
	                INFO_PRINTF2(_L("SetVideoEnabledL() returned wrong error=%d"),err);
		            failTestFlag = ETrue;		            
	                break;
	            	}
	            
                iVideoRecorder->Stop();
                // close
                iVideoRecorder->Close();
                CActiveScheduler::Stop();
                iTestStepResult = EPass;
                break;
            }
        	if(failTestFlag)
        		{
		        iVideoRecorder->Stop();
		        iVideoRecorder->Close();
		        CActiveScheduler::Stop();
		        iTestStepResult = EFail;
		        return;
        		}
        }
    }


//
// CTestVclntAviRecordSetVideoQuality
//

/**
CTestVclntAviRecordSetVideoQuality::Constructor
*/
CTestVclntAviRecordSetVideoQuality::CTestVclntAviRecordSetVideoQuality(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError)
    : RTestVclntAviRecorderStep(aTestName, aSectName, aKeyName, aExpectedError)
    {
    }

/**
CTestVclntAviRecordSetVideoQuality::NewL
*/
CTestVclntAviRecordSetVideoQuality* CTestVclntAviRecordSetVideoQuality::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError)
	{
	CTestVclntAviRecordSetVideoQuality* self = new (ELeave) CTestVclntAviRecordSetVideoQuality(aTestName, aSectName, aKeyName, aExpectedError);
	return self;
	}
/**
CTestVclntAviRecordSetVideoQuality::DoTestStepL
*/
TVerdict CTestVclntAviRecordSetVideoQuality::DoTestStepL()
    {
    INFO_PRINTF1(_L("Test : Video Recorder - OpenFileL(RFile&)"));
    
    // Connect to RFs
    User::LeaveIfError(iFs.Connect());
    User::LeaveIfError(iFs.ShareProtected());
    
    // Open RFile
    User::LeaveIfError(iFile.Replace(iFs, iFilename, EFileRead));
    
    // Call the state handler from IDLE state
    FsmL(EVRIdle);
    
    // Start the scheduler - Done only once !
    CActiveScheduler::Start();
    
    iFile.Close();
    iFs.Close();

    return iTestStepResult;
    }

/**
CTestVclntAviRecordSetVideoQuality::FsmL()
*/
void CTestVclntAviRecordSetVideoQuality::FsmL(TVclntTestRecordEvents aEventCode)
    {
    if (FsmCheck(aEventCode))
        {
        TBool failTestFlag = EFalse;
        TInt err = KErrNone;
    	TInt videoQuality = EVideoQualityNormal;
    	
        switch (aEventCode)
            {
            case EVRIdle:
	        	// Calling SetVideoFrameRateFixedL before opening file should return KErrNotReady
	            INFO_PRINTF1(_L("calling iVideoRecorder->SetVideoQualityL()"));
                TRAP(err, iVideoRecorder->SetVideoQualityL(EVideoQualityNormal));
	            if(err != KErrNotReady)
	            	{
	                INFO_PRINTF2(_L("SetVideoQualityL() returned wrong error=%d"),err);
		            failTestFlag = ETrue;		            
	                break;	            	
	            	}
	
	            // Calling VideoQualityL before opening file should return KErrNotReady
	            INFO_PRINTF1(_L("calling iVideoRecorder->VideoQualityL()"));
	            TRAP(err, iVideoRecorder->VideoQualityL());
	            if(err != KErrNotReady)
	            	{
	                INFO_PRINTF2(_L("VideoQualityL() returned wrong error=%d"),err);
		            failTestFlag = ETrue;		            
	                break;	            	
	            	}            
                // Open iVideoRecorder
                INFO_PRINTF2(_L("iVideoRecorder->OpenFileL() %S"), &iFilename);

                // If extension of file is .dummy open file on test controller.
                if( KErrNotFound == iFilename.Find(KDummyExtension) )
                	{
                	TRAP(iError, iVideoRecorder->OpenFileL(iFile, iCameraHandle, KMmfTestAviRecordControllerUid, KAviRecordFormatUid));
                	}
                else
                	{
                	TRAP(iError, iVideoRecorder->OpenFileL(iFile, NULL, KMmfVideoTestControllerUid, KUidMdaBmpClipFormat));                	
                	}
                User::LeaveIfError(iError);
                PrepareState(EVROpenComplete, KErrNone);
                break;
            case EVROpenComplete:

            	// Check that by default video quality is EVideoQualityNormal.
	            INFO_PRINTF1(_L("calling iVideoRecorder->VideoQualityL()"));
	            TRAP(err, videoQuality = iVideoRecorder->VideoQualityL());
	            if( (videoQuality != EVideoQualityNormal) || (err != KErrNone) )
	            	{
	            	INFO_PRINTF2(_L("VideoQualityL returned err=%d"),err);	            	
		            INFO_PRINTF2(_L("VideoQualityL() returned %d"),videoQuality);
		            failTestFlag = ETrue;	            
	                break;
	            	}
	            
	            videoQuality = EVideoQualityLow;
	            
                while(1)
                	{
	            	if( videoQuality == EVideoQualityLow)
	            		{
	            		videoQuality = EVideoQualityNormal;
	            		}
	            	else if( videoQuality == EVideoQualityNormal)
	            		{
	            		videoQuality = EVideoQualityHigh;
	            		}
	            	else if( videoQuality == EVideoQualityHigh)
	            		{
	            		videoQuality = EVideoQualityLossless;
	            		}
	            	else if( videoQuality == EVideoQualityLossless)
		            	{
		            	videoQuality = 101;
		            	}
	            	else if( videoQuality == 101)
		            	{
		            	videoQuality = 0;
		            	}
	            	else if( videoQuality == 0)
		            	{
		            	videoQuality = -1;
		            	}
	            	else if( videoQuality == -1)
		            	{
		            	videoQuality = EVideoQualityLow;
		            	}

                    INFO_PRINTF2(_L("Calling SetVideoQualityL(%d)"),videoQuality );
                    TRAP(err, iVideoRecorder->SetVideoQualityL(videoQuality));
                    if( err == KErrNotSupported)
                    	{
	                    INFO_PRINTF1(_L("SetVideoQualityL returned KErrNotSupported"));
                    	}
                    else if( (err == KErrArgument ) && 
                        ( (videoQuality < 0) || (videoQuality > EVideoQualityLossless) ) )
                        {
	                    INFO_PRINTF1(_L("SetVideoQualityL returned correct error KErrArgument"));
                        }
                    else if( err != KErrNone )
    	            	{
    	            	INFO_PRINTF2(_L("SetVideoQualityL returned err=%d"),err);	            	
    		            failTestFlag = ETrue;
    	                break;
    	            	}
                    else // success case
                    	{
                    	TInt latestVideoQuality = EVideoQualityNormal;
                    	// Check that VideoQualityL is returning the value set.
        	            INFO_PRINTF1(_L("calling iVideoRecorder->VideoQualityL()"));
        	            TRAP(err, latestVideoQuality = iVideoRecorder->VideoQualityL());
        	            if( (videoQuality != latestVideoQuality) || (err != KErrNone) )
        	            	{
        	            	INFO_PRINTF2(_L("VideoQualityL returned err=%d"),err);	            	
        		            INFO_PRINTF2(_L("VideoQualityL() returned %d"),videoQuality);
        		            failTestFlag = ETrue;	            
        	                break;
        	            	}	                    	
                    	}
	            	if( videoQuality == EVideoQualityLow)
    	            	{
    	            	break;//Break the forever while loop;
    	            	}
		            }
                
            	if(failTestFlag)
            		{
	            	break;//Fail the test, there is an error in above while loop;
            		}
                // Prepare iVideoRecorder
                INFO_PRINTF1(_L("iVideoRecorder->Prepare()"));
                PrepareState(EVRPrepareComplete, KErrNone);
                iVideoRecorder->Prepare();
                break;
            case EVRPrepareComplete:
                // Record iVideoRecorder
                INFO_PRINTF1(_L("iVideoRecorder->Record()"));
                iVideoRecorder->Record();
                PrepareState(EVRRecordComplete, KErrNone);
                User::After(KFiveSeconds);
                
                // Calling SetVideoQualityL while recording should return KErrNotReady
	            INFO_PRINTF1(_L("calling iVideoRecorder->SetVideoQualityL()"));
                TRAP(err, iVideoRecorder->SetVideoQualityL(EVideoQualityLossless));
	            if(err != KErrNotReady)
	            	{
	                INFO_PRINTF2(_L("SetVideoQualityL() returned wrong error=%d"),err);
		            failTestFlag = ETrue;		            
	                break;
	            	}
	            
                iVideoRecorder->Stop();
                // close
                iVideoRecorder->Close();
                CActiveScheduler::Stop();
                iTestStepResult = EPass;
                break;
            }
        	if(failTestFlag)
        		{
		        iVideoRecorder->Stop();
		        iVideoRecorder->Close();
		        CActiveScheduler::Stop();
		        iTestStepResult = EFail;
		        return;
        		}
        }
    }

//
// CTestVclntAviRecordSetVideoFrameRateFixed
//

/**
CTestVclntAviRecordSetVideoFrameRateFixed::Constructor
*/
CTestVclntAviRecordSetVideoFrameRateFixed::CTestVclntAviRecordSetVideoFrameRateFixed(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError)
    : RTestVclntAviRecorderStep(aTestName, aSectName, aKeyName, aExpectedError)
    {
    }

/**
CTestVclntAviRecordSetVideoFrameRateFixed::NewL
*/
CTestVclntAviRecordSetVideoFrameRateFixed* CTestVclntAviRecordSetVideoFrameRateFixed::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError)
	{
	CTestVclntAviRecordSetVideoFrameRateFixed* self = new (ELeave) CTestVclntAviRecordSetVideoFrameRateFixed(aTestName, aSectName, aKeyName, aExpectedError);
	return self;
	}
/**
CTestVclntAviRecordSetVideoFrameRateFixed::DoTestStepL
*/
TVerdict CTestVclntAviRecordSetVideoFrameRateFixed::DoTestStepL()
    {
    INFO_PRINTF1(_L("Test : Video Recorder - OpenFileL(RFile&)"));
    
    // Connect to RFs
    User::LeaveIfError(iFs.Connect());
    User::LeaveIfError(iFs.ShareProtected());
    
    // Open RFile
    User::LeaveIfError(iFile.Replace(iFs, iFilename, EFileRead));
    
    // Call the state handler from IDLE state
    FsmL(EVRIdle);
    
    // Start the scheduler - Done only once !
    CActiveScheduler::Start();
    
    iFile.Close();
    iFs.Close();

    return iTestStepResult;
    }

/**
CTestVclntAviRecordSetVideoFrameRateFixed::FsmL()
*/
void CTestVclntAviRecordSetVideoFrameRateFixed::FsmL(TVclntTestRecordEvents aEventCode)
    {
    if (FsmCheck(aEventCode))
        {
        TBool failTestFlag = EFalse;
        TInt err;
        switch (aEventCode)
            {
            case EVRIdle:
	        	// Calling SetVideoFrameRateFixedL before opening file should return KErrNotReady
	            INFO_PRINTF1(_L("calling iVideoRecorder->SetVideoFrameRateFixedL(ETrue)"));
	            TRAP(err, iVideoRecorder->SetVideoFrameRateFixedL(ETrue));
	            if(err != KErrNotReady)
	            	{
	                INFO_PRINTF2(_L("SetVideoFrameRateFixedL() returned wrong error=%d"),err);
		            failTestFlag = ETrue;		            
	                break;	            	
	            	}

	            // Calling VideoFrameRateFixedL before opening file should return KErrNotReady
	            INFO_PRINTF1(_L("calling iVideoRecorder->VideoFrameRateFixedL()"));
	            TRAP(err, iVideoRecorder->VideoFrameRateFixedL());
	            if(err != KErrNotReady)
	            	{
	                INFO_PRINTF2(_L("SetVideoFrameRateFixedL() returned wrong error=%d"),err);
		            failTestFlag = ETrue;		            
	                break;	            	
	            	}

                // Open iVideoRecorder
                INFO_PRINTF2(_L("iVideoRecorder->OpenFileL() %S"), &iFilename);

                // If extension of file is .dummy open file on test controller.
                if( KErrNotFound == iFilename.Find(KDummyExtension) )
                	{
                	TRAP(iError, iVideoRecorder->OpenFileL(iFile, iCameraHandle, KMmfTestAviRecordControllerUid, KAviRecordFormatUid));
                	}
                else
                	{
                	TRAP(iError, iVideoRecorder->OpenFileL(iFile, NULL, KMmfVideoTestControllerUid, KUidMdaBmpClipFormat));                	
                	}
                User::LeaveIfError(iError);
                PrepareState(EVROpenComplete, KErrNone);
                break;
            case EVROpenComplete:
            	TBool frameRateFixedFlag;
            	
            	// Check that by default FixedFrameRate is enabled.
	            INFO_PRINTF1(_L("calling iVideoRecorder->VideoFrameRateFixedL()"));
	            TRAP(err, frameRateFixedFlag = iVideoRecorder->VideoFrameRateFixedL());
	            if( (frameRateFixedFlag != (TBool)EFalse) || (err != KErrNone) )
	            	{
	            	INFO_PRINTF2(_L("VideoFrameRateFixedL returned err=%d"),err);	            	
		            INFO_PRINTF2(_L("VideoFrameRateFixedL() returned %d"),frameRateFixedFlag);
		            failTestFlag = ETrue;		            
	                break;
	            	}
	            
            	// Try to enable FixedFrameRate.
	            INFO_PRINTF1(_L("calling iVideoRecorder->SetVideoFrameRateFixedL(ETrue)"));
	            TRAP(err, iVideoRecorder->SetVideoFrameRateFixedL(ETrue));
	            if(err == KErrNotSupported)
	            	{
	            	INFO_PRINTF1(_L("SetVideoFrameRateFixedL KErrNotSupported"));	            	
	            	}
	            else if(err != KErrNone)
	            	{
	            	INFO_PRINTF2(_L("SetVideoFrameRateFixedL returned err=%d"),err);	            		            	
		            failTestFlag = ETrue;		            
	                break;
	            	}
	            else // success case
	            	{
		            INFO_PRINTF1(_L("calling iVideoRecorder->VideoFrameRateFixedL()"));
		            TRAP(err, frameRateFixedFlag = iVideoRecorder->VideoFrameRateFixedL());
		            if( (frameRateFixedFlag != (TBool)ETrue) || (err != KErrNone) )
		            	{
		            	INFO_PRINTF2(_L("VideoFrameRateFixedL returned err=%d"),err);	            	
			            INFO_PRINTF2(_L("VideoFrameRateFixedL() returned %d"),frameRateFixedFlag);
			            failTestFlag = ETrue;		            
		                break;
		            	}
	            	}

	            // Prepare iVideoRecorder
                INFO_PRINTF1(_L("iVideoRecorder->Prepare()"));
                PrepareState(EVRPrepareComplete, KErrNone);
                iVideoRecorder->Prepare();
                break;
            case EVRPrepareComplete:
                // Record iVideoRecorder
                INFO_PRINTF1(_L("iVideoRecorder->Record()"));
                iVideoRecorder->Record();
                PrepareState(EVRRecordComplete, KErrNone);
                User::After(KFiveSeconds);
                
	        	// Calling SetVideoFrameRateFixedL while recording should return KErrNotReady
	            INFO_PRINTF1(_L("calling iVideoRecorder->SetVideoFrameRateFixedL(ETrue)"));
	            TRAP(err, iVideoRecorder->SetVideoFrameRateFixedL(ETrue));
	            if(err != KErrNotReady)
	            	{
	                INFO_PRINTF2(_L("SetVideoFrameRateFixedL() returned wrong error=%d"),err);
		            failTestFlag = ETrue;		            
	                break;	            	
	            	}
	            
                iVideoRecorder->Stop();
                iVideoRecorder->Close();
                CActiveScheduler::Stop();
                iTestStepResult = EPass;
                break;
            }
        	if(failTestFlag)
        		{
		        iVideoRecorder->Stop();
		        iVideoRecorder->Close();
		        CActiveScheduler::Stop();
		        iTestStepResult = EFail;
		        return;
        		}
        }
    }
