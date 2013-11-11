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


#include "testvideorecorder_par_audio.h"


//
// RTestVclntSetGetPARRec
//
//MM-MMF-VCLNTAVI-I-0300, MM-MMF-VCLNTAVI-I-0315
//

/**
 * RTestVclntSetGetPARRec::RTestVclntSetGetPARRec()
 */
RTestVclntSetGetPARRec::RTestVclntSetGetPARRec(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError, TInt aNumerator, TInt aDenominator)
    : RTestVclntRecordAviFile(aTestName, aSectName, aKeyName, aExpectedError, EFalse)
	{
	iAspectRatio.iNumerator = aNumerator;
	iAspectRatio.iDenominator = aDenominator;
	iNewAspectRatio.iNumerator = 0;
	iNewAspectRatio.iDenominator = 0;
    iTestStepResult = EFail;
	}

/**
 * RTestVclntSetGetPARRec::NewL()
 */
RTestVclntSetGetPARRec* RTestVclntSetGetPARRec::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError, TInt aNumerator, TInt aDenominator)
    {
    RTestVclntSetGetPARRec* self = new (ELeave) RTestVclntSetGetPARRec(aTestName, aSectName, aKeyName, aExpectedError, aNumerator, aDenominator);
    return self;
    }

/**
 *RTestVclntSetGetPARRec::DoTestStepL()
 */
TVerdict RTestVclntSetGetPARRec::DoTestStepL()
    {
    // Call the state handler from IDLE state
    FsmL(EVRIdle);

    // Start the scheduler - Done only once !
    CActiveScheduler::Start();

    return iTestStepResult;
    }

 /**
 * RTestVclntSetGetPARRec::FsmL
 */
void RTestVclntSetGetPARRec::FsmL(TVclntTestRecordEvents aEventCode)
    {
    if (FsmCheck(aEventCode))
        {
        switch (aEventCode)
            {
            case EVRIdle:
                // Open iVideoRecorder
                INFO_PRINTF2(_L("iVideoRecorder->OpenFileL(%S)"), &iFilename);
                TRAP(iError,iVideoRecorder->OpenFileL(iFilename, iCameraHandle, KMmfTestAviRecordControllerUid, KAviRecordFormatUid));
                PrepareState(EVROpenComplete, KErrNone);
                break;
            case EVROpenComplete:
            	{
				//Set Video Frame size and Video Codec
				TSize frameSize;
				frameSize.iWidth = 176;
				frameSize.iHeight = 144;
				TRAPD(err, iVideoRecorder->SetVideoFrameSizeL(frameSize));
				if (err != KErrNone)
					{
					INFO_PRINTF2(_L("SetVideoFrameSizeL() returns with error :  %d"),iError);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				INFO_PRINTF2(_L("SetVideoFrameSizeL() returned with err : %d"), err);
				_LIT8(KAviVideoCodec,"XVID");
				TRAP(err, iVideoRecorder->SetVideoTypeL(KAviVideoCodec));
				if (err != KErrNone)
					{
					INFO_PRINTF2(_L("SetVideoTypeL() returns with error :  %d"),iError);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				INFO_PRINTF2(_L("SetVideoTypeL() returned with err : %d"), err);
				//Set pixel aspect ratio to 1:1
				TInt iSetError;//used for invalid AspectRatio;
				TRAP(iError,iVideoRecorder->SetPixelAspectRatioL(iAspectRatio));
				iSetError = iError;
				if (iError != iTestExpectedError)
					{
					INFO_PRINTF4(_L("SetPixelAspectRatioL(%d:%d) returns with error :  %d "),iAspectRatio.iNumerator, iAspectRatio.iDenominator, iError);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				INFO_PRINTF4(_L("SetPixelAspectRatioL(%d:%d) returns with error :  %d "),iAspectRatio.iNumerator, iAspectRatio.iDenominator, iError);
				//Get pixel aspect ratio as 1:1
				TRAP(iError,iVideoRecorder->GetPixelAspectRatioL(iNewAspectRatio));
				if (iError != KErrNone)
					{
					INFO_PRINTF2(_L("GetPixelAspectRatioL() returns with error :  %d"),iError);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				if(iAspectRatio == iNewAspectRatio)
					{
					INFO_PRINTF4(_L("GetPixelAspectRatioL() returned %d:%d with err : %d"), iNewAspectRatio.iNumerator, iNewAspectRatio.iDenominator, iError);
					}
				else
					{
					if(iSetError != KErrNotSupported)
						{
						INFO_PRINTF4(_L("GetPixelAspectRatioL() returned %d:%d with err : %d"), iNewAspectRatio.iNumerator, iNewAspectRatio.iDenominator, iError);
						iTestStepResult = EFail;
						CActiveScheduler::Stop();
						break;
						}
					else
						{
						INFO_PRINTF4(_L("GetPixelAspectRatioL() returned %d:%d with err : %d"), iNewAspectRatio.iNumerator, iNewAspectRatio.iDenominator, iError);
						}
					}
				// Prepare iVideoRecorder
                INFO_PRINTF1(_L("iVideoRecorder->Prepare()"));
                iVideoRecorder->Prepare();
                PrepareState(EVRPrepareComplete, KErrNone);
                break;
            	}
            case EVRPrepareComplete:
                // Record iVideoRecorder
                INFO_PRINTF1(_L("iVideoRecorder->Record()"));
                iVideoRecorder->Record();
                TRAPD(err, iRecordTimer = CTestVclntAviRecordTimer::NewL(this, KFiveSeconds));
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
           		// Cancel the timer
                iRecordTimer->Cancel();
                delete iRecordTimer;
                iRecordTimer = NULL;

                CActiveScheduler::Stop();
                iTestStepResult = EPass;
                break;
            }
        }
    }

/**
* set finished on callback from a timer
*
**/
void RTestVclntSetGetPARRec::FinishedL()
	{
    // Stop and close iVideoRecorder
    iVideoRecorder->Stop();
    INFO_PRINTF1(_L("iVideoRecorder->Stop()"));
	iVideoRecorder->Close();
	INFO_PRINTF1(_L("iVideoRecorder->Close()"));
	TRAPD(err, FsmL(EVRRecordComplete));
    if (err != KErrNone)
		{
		iError = err;
		CActiveScheduler::Stop();
		iTestStepResult = EFail;
		}
	}

//
// RTestVclntGetSuppPAR
//
//MM-MMF-VCLNTAVI-I-0301
//

/**
 * RTestVclntGetSuppPAR::RTestVclntGetSuppPAR()
 */
RTestVclntGetSuppPAR::RTestVclntGetSuppPAR(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError)
    : RTestVclntRecordAviFile(aTestName, aSectName, aKeyName, aExpectedError,EFalse)
    {
    iTestStepResult = EFail;
    }

/**
 * RTestVclntGetSuppPAR::NewL()
 */
RTestVclntGetSuppPAR* RTestVclntGetSuppPAR::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError)
    {
    RTestVclntGetSuppPAR* self = new (ELeave) RTestVclntGetSuppPAR(aTestName, aSectName, aKeyName, aExpectedError);
    return self;
    }

/**
 * RTestVclntGetSuppPAR::DoTestStepL
 */
TVerdict RTestVclntGetSuppPAR::DoTestStepL()
    {
    // Call the state handler from IDLE state
    FsmL(EVRIdle);

    // Start the scheduler - Done only once !
    CActiveScheduler::Start();

    return iTestStepResult;
    }

/**
 * RTestVclntGetSuppPAR::FsmL
 */
void RTestVclntGetSuppPAR::FsmL(TVclntTestRecordEvents aEventCode)
    {
    if (FsmCheck(aEventCode))
        {
        switch (aEventCode)
            {
            case EVRIdle:
                // Open iVideoRecorder
                INFO_PRINTF2(_L("iVideoRecorder->OpenFileL(%S)"), &iFilename);
                TRAP(iError,iVideoRecorder->OpenFileL(iFilename, iCameraHandle, KMmfTestAviRecordControllerUid, KAviRecordFormatUid));
                PrepareState(EVROpenComplete, KErrNone);
                break;
            case EVROpenComplete:
            	{
				//Set Video Frame size and Video Codec
				TSize frameSize;
				frameSize.iWidth = 176;
				frameSize.iHeight = 144;
				TRAPD(err, iVideoRecorder->SetVideoFrameSizeL(frameSize));
				if (err != KErrNone)
					{
					INFO_PRINTF2(_L("SetVideoFrameSizeL() returns with error :  %d"),iError);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				INFO_PRINTF2(_L("SetVideoFrameSizeL() returned with err : %d"), err);
				_LIT8(KAviVideoCodec,"XVID");
				TRAP(err, iVideoRecorder->SetVideoTypeL(KAviVideoCodec));
				if (err != KErrNone)
					{
					INFO_PRINTF2(_L("SetVideoTypeL() returns with error :  %d"),iError);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				INFO_PRINTF2(_L("SetVideoTypeL() returned with err : %d"), err);
				//Get supported pixel aspect ratio as 1:1
				TRAP(iError,iVideoRecorder->GetSupportedPixelAspectRatiosL(iAspectRatios));
				if (iError != KErrNone)
					{
					INFO_PRINTF2(_L("GetSupportedPixelAspectRatiosL() returns with error :  %d"),iError);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				if(iAspectRatios.Count() >= 1)
					{
					INFO_PRINTF3(_L("GetSupportedPixelAspectRatiosL() returned iAspectRatios.Count: %d with err : %d "),iAspectRatios.Count(), iError);
					for(int i = 0; i< iAspectRatios.Count(); i++)
						{
						INFO_PRINTF3(_L("Supported AspectRatio %d:%d"),(iAspectRatios)[i].iNumerator, (iAspectRatios)[i].iDenominator);
						}
					}
				else
					{
					INFO_PRINTF2(_L("GetSupportedPixelAspectRatiosL() returned with iAspectRatios.Count: %d"), iAspectRatios.Count());
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				// Prepare iVideoRecorder
                INFO_PRINTF1(_L("iVideoRecorder->Prepare()"));
                iVideoRecorder->Prepare();
                PrepareState(EVRPrepareComplete, KErrNone);
                break;
            	}
            case EVRPrepareComplete:
                // Record iVideoRecorder
                INFO_PRINTF1(_L("iVideoRecorder->Record()"));
                iVideoRecorder->Record();
                TRAPD(err, iRecordTimer = CTestVclntAviRecordTimer::NewL(this, KFiveSeconds));
                if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CTestVclntAviRecordTimer::NewL left with error = %d"), iError);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				iRecordTimer->Start();
                PrepareState(EVRRecordComplete, KErrNone);
                break;
           	case EVRRecordComplete:
           		// Cancel the timer
                iRecordTimer->Cancel();
                delete iRecordTimer;
                iRecordTimer = NULL;

                CActiveScheduler::Stop();
                iTestStepResult = EPass;
                break;
            }
        }
    }

/**
* set finished on callback from a timer
*
**/
void RTestVclntGetSuppPAR::FinishedL()
	{
    // Stop and close iVideoRecorder
    iVideoRecorder->Stop();
	INFO_PRINTF1(_L("iVideoRecorder->Stop()"));
	iVideoRecorder->Close();
	INFO_PRINTF1(_L("iVideoRecorder->Close()"));
	TRAPD(err, FsmL(EVRRecordComplete));
    if (err != KErrNone)
		{
		iError = err;
		CActiveScheduler::Stop();
		iTestStepResult = EFail;
		}
	}

//
//  RTestVclntSetGetPARAftOpenPrepThenRec
//
//  MM-MMF-VCLNTAVI-I-0302
//

/**
 * RTestVclntSetGetPARAftOpenPrepThenRec::RTestVclntSetGetPARAftOpenPrepThenRec()
 */
RTestVclntSetGetPARAftOpenPrepThenRec::RTestVclntSetGetPARAftOpenPrepThenRec(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError)
    : RTestVclntRecordAviFile(aTestName, aSectName, aKeyName, aExpectedError,EFalse)
	{
	iAspectRatio.iNumerator = 1;
	iAspectRatio.iDenominator = 1;
	iNewAspectRatio.iNumerator = 0;
	iNewAspectRatio.iDenominator = 0;
    iTestStepResult = EFail;
	}

/**
 * RTestVclntSetGetPARAftOpenPrepThenRec::NewL()
 */
RTestVclntSetGetPARAftOpenPrepThenRec* RTestVclntSetGetPARAftOpenPrepThenRec::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError)
    {
    RTestVclntSetGetPARAftOpenPrepThenRec* self = new (ELeave) RTestVclntSetGetPARAftOpenPrepThenRec(aTestName, aSectName, aKeyName, aExpectedError);
    return self;
    }

/**
 *RTestVclntSetGetPARAftOpenPrepThenRec::DoTestStepL
 */
TVerdict RTestVclntSetGetPARAftOpenPrepThenRec::DoTestStepL()
    {
    // Call the state handler from IDLE state
    FsmL(EVRIdle);

    // Start the scheduler - Done only once !
    CActiveScheduler::Start();

    return iTestStepResult;
    }

/* RTestVclntSetGetPARAftOpenPrepThenRec::FsmL
 */
void RTestVclntSetGetPARAftOpenPrepThenRec::FsmL(TVclntTestRecordEvents aEventCode)
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
				{
				//Set Video Frame size and Video Codec
				TSize frameSize;
				frameSize.iWidth = 176;
				frameSize.iHeight = 144;
				TRAPD(err, iVideoRecorder->SetVideoFrameSizeL(frameSize));
				if (err != KErrNone)
					{
					INFO_PRINTF2(_L("SetVideoFrameSizeL() returns with error :  %d"),iError);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				INFO_PRINTF2(_L("SetVideoFrameSizeL() returned with err : %d"), err);
				_LIT8(KAviVideoCodec,"XVID");
				TRAP(err, iVideoRecorder->SetVideoTypeL(KAviVideoCodec));
				if (err != KErrNone)
					{
					INFO_PRINTF2(_L("SetVideoTypeL() returns with error :  %d"),iError);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				INFO_PRINTF2(_L("SetVideoTypeL() returned with err : %d"), err);
                // Prepare iVideoRecorder
                INFO_PRINTF1(_L("iVideoRecorder->Prepare()"));
                iVideoRecorder->Prepare();
                PrepareState(EVRPrepareComplete, KErrNone);
                break;
				}
            case EVRPrepareComplete:
				//Set pixel aspect ratio to 1:1
				TRAP(iError,iVideoRecorder->SetPixelAspectRatioL(iAspectRatio));
				if (iError != iTestExpectedError)
					{
					INFO_PRINTF4(_L("SetPixelAspectRatioL(%d:%d) returns with error :  %d "),iAspectRatio.iNumerator, iAspectRatio.iDenominator, iError);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				INFO_PRINTF4(_L("SetPixelAspectRatioL(%d:%d) returns with error :  %d "),iAspectRatio.iNumerator, iAspectRatio.iDenominator, iError);
				//Get pixel aspect ratio as 1:1
				TRAP(iError,iVideoRecorder->GetPixelAspectRatioL(iNewAspectRatio));
				if (iError != KErrNone)
					{
					INFO_PRINTF2(_L("GetPixelAspectRatioL() returns with error :  %d"),iError);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				if(iAspectRatio == iNewAspectRatio)
					{
					INFO_PRINTF4(_L("GetPixelAspectRatioL() returned %d:%d with err : %d"), iNewAspectRatio.iNumerator, iNewAspectRatio.iDenominator, iError);
					}
				else
					{
					INFO_PRINTF4(_L("GetPixelAspectRatioL() returned %d:%d with err : %d"), iNewAspectRatio.iNumerator, iNewAspectRatio.iDenominator, iError);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
                // Record iVideoRecorder
                INFO_PRINTF1(_L("iVideoRecorder->Record()"));
                iVideoRecorder->Record();
                TRAPD(err, iRecordTimer = CTestVclntAviRecordTimer::NewL(this, KFiveSeconds));
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
           		// Cancel the timer
                iRecordTimer->Cancel();
                delete iRecordTimer;
                iRecordTimer = NULL;

                CActiveScheduler::Stop();
                iTestStepResult = EPass;
                break;
            }
        }
    }

/**
* set finished on callback from a timer
*
**/
void RTestVclntSetGetPARAftOpenPrepThenRec::FinishedL()
	{
    // Stop and close iVideoRecorder
	iVideoRecorder->Stop();
    INFO_PRINTF1(_L("iVideoRecorder->Stop()"));
	iVideoRecorder->Close();
	INFO_PRINTF1(_L("iVideoRecorder->Close()"));
	TRAPD(err, FsmL(EVRRecordComplete));
    if (err != KErrNone)
		{
		iError = err;
		CActiveScheduler::Stop();
		iTestStepResult = EFail;
		}
	}

//
//   RTestVclntSetPARAftOpenWoutSetVidAudCodec
//
//    MM-MMF-VCLNTAVI-I-0303
//

/**
 * RTestVclntSetPARAftOpenWoutSetVidAudCodec::RTestVclntSetPARAftOpenWoutSetVidAudCodec()
 */
RTestVclntSetPARAftOpenWoutSetVidAudCodec::RTestVclntSetPARAftOpenWoutSetVidAudCodec(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError)
    : RTestVclntRecordAviFile(aTestName, aSectName, aKeyName, aExpectedError,EFalse)
    {
	iAspectRatio.iNumerator = 1;
	iAspectRatio.iDenominator = 1;
    iTestStepResult = EFail;
    }

/**
 * RTestVclntSetPARAftOpenWoutSetVidAudCodec::NewL()
 */
RTestVclntSetPARAftOpenWoutSetVidAudCodec* RTestVclntSetPARAftOpenWoutSetVidAudCodec::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError)
    {
    RTestVclntSetPARAftOpenWoutSetVidAudCodec* self = new (ELeave) RTestVclntSetPARAftOpenWoutSetVidAudCodec(aTestName, aSectName, aKeyName, aExpectedError);
    return self;
    }

/**
 *RTestVclntSetPARAftOpenWoutSetVidAudCodec::DoTestStepL
 */
TVerdict RTestVclntSetPARAftOpenWoutSetVidAudCodec::DoTestStepL()
    {
    // Call the state handler from IDLE state
    FsmL(EVRIdle);

    // Start the scheduler - Done only once !
    CActiveScheduler::Start();

    return iTestStepResult;
    }

/* RTestVclntSetPARAftOpenWoutSetVidAudCodec::FsmL
 */
void RTestVclntSetPARAftOpenWoutSetVidAudCodec::FsmL(TVclntTestRecordEvents aEventCode)
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
				//Set pixel aspect ratio to 1:1
				TRAP(iError,iVideoRecorder->SetPixelAspectRatioL(iAspectRatio));
				if (iError != iTestExpectedError)
					{
					INFO_PRINTF4(_L("SetPixelAspectRatioL(%d:%d) returns with error :  %d "),iAspectRatio.iNumerator, iAspectRatio.iDenominator, iError);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				INFO_PRINTF4(_L("SetPixelAspectRatioL(%d:%d) returns with error :  %d "),iAspectRatio.iNumerator, iAspectRatio.iDenominator, iError);
                // Prepare iVideoRecorder
                INFO_PRINTF1(_L("iVideoRecorder->Prepare()"));
                iVideoRecorder->Prepare();
                PrepareState(EVRPrepareComplete, KErrNone);
                break;
            case EVRPrepareComplete:
                // Record iVideoRecorder
                INFO_PRINTF1(_L("iVideoRecorder->Record()"));
                iVideoRecorder->Record();
                TRAPD(err, iRecordTimer = CTestVclntAviRecordTimer::NewL(this, KFiveSeconds));
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
           		// Cancel the timer
                iRecordTimer->Cancel();
                delete iRecordTimer;
                iRecordTimer = NULL;

                CActiveScheduler::Stop();
                iTestStepResult = EPass;
                break;
            }
        }
    }

/**
* set finished on callback from a timer
*
**/
void RTestVclntSetPARAftOpenWoutSetVidAudCodec::FinishedL()
	{
    // Stop and close iVideoRecorder
	iVideoRecorder->Stop();
    INFO_PRINTF1(_L("iVideoRecorder->Stop()"));
	iVideoRecorder->Close();
	INFO_PRINTF1(_L("iVideoRecorder->Close()"));
	TRAPD(err, FsmL(EVRRecordComplete));
    if (err != KErrNone)
		{
		iError = err;
		CActiveScheduler::Stop();
		iTestStepResult = EFail;
		}
	}

//
// RTestVclntSetGetAudChanRec
//
//MM-MMF-VCLNTAVI-I-0304, MM-MMF-VCLNTAVI-I-0318
//

/**
 * RTestVclntSetGetAudChanRec::RTestVclntSetGetAudChanRec()
 */
RTestVclntSetGetAudChanRec::RTestVclntSetGetAudChanRec(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError, TInt aAudioChannel)
    : RTestVclntRecordAviFile(aTestName, aSectName, aKeyName, aExpectedError,EFalse)
    {
    iAudioChannel = aAudioChannel;
    iNewAudioChannel = 0;
    iTestStepResult = EFail;
    }

/**
 *RTestVclntSetGetAudChanRec::NewL()
 */
RTestVclntSetGetAudChanRec* RTestVclntSetGetAudChanRec::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError, TInt aAudioChannel)
    {
    RTestVclntSetGetAudChanRec* self = new (ELeave) RTestVclntSetGetAudChanRec(aTestName, aSectName, aKeyName, aExpectedError, aAudioChannel);
    return self;
    }

/**
 * RTestVclntSetGetAudChanRec::DoTestStepL
 */
TVerdict RTestVclntSetGetAudChanRec::DoTestStepL()
    {
    DeleteFileL(iFilename);

    // Call the state handler from IDLE state
    FsmL(EVRIdle);

    // Start the scheduler - Done only once !
    CActiveScheduler::Start();

    return iTestStepResult;
    }

/**
 * RTestVclntSetGetAudChanRec::FsmL
 */
void RTestVclntSetGetAudChanRec::FsmL(TVclntTestRecordEvents aEventCode)
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
				{
				//Set Video Frame size and Video Codec
				TSize frameSize;
				frameSize.iWidth = 176;
				frameSize.iHeight = 144;
				TRAPD(err, iVideoRecorder->SetVideoFrameSizeL(frameSize));
				if (err != KErrNone)
					{
					INFO_PRINTF2(_L("SetVideoFrameSizeL() returns with error :  %d"),iError);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				INFO_PRINTF2(_L("SetVideoFrameSizeL() returned with err : %d"), err);
				_LIT8(KAviVideoCodec,"XVID");
				TRAP(err, iVideoRecorder->SetVideoTypeL(KAviVideoCodec));
				if (err != KErrNone)
					{
					INFO_PRINTF2(_L("SetVideoTypeL() returns with error :  %d"),iError);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				INFO_PRINTF2(_L("SetVideoTypeL() returned with err : %d"), err);
				// SetAudioEnabledL
				TBool enabled = ETrue;
				TRAP(err, iVideoRecorder->SetAudioEnabledL(enabled));
				if (err != KErrNone)
					{
					INFO_PRINTF2(_L("SetAudioEnabledL() returns with error :  %d"),err);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				INFO_PRINTF2(_L("iVideoRecorder->SetAudioEnabledL() returns with error : %d"), err);
				//Set Audio Channel no. to 2.
				TInt iSetError;//used for invalid iAudioChannel;
				TRAP(iError,iVideoRecorder->SetAudioChannelsL(iAudioChannel));
				iSetError = iError;
				if (iError != iTestExpectedError)
					{
					INFO_PRINTF3(_L("SetAudioChannelsL(%d) returns with error : %d"),iAudioChannel, iError);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				INFO_PRINTF3(_L("SetAudioChannelsL(%d) returns with error : %d"),iAudioChannel, iError);
				//Get the Audio Channel no as 2.
				TRAP(iError,iNewAudioChannel=iVideoRecorder->AudioChannelsL());
				if (iError != KErrNone)
					{
					INFO_PRINTF3(_L("AudioChannelsL() returned iNewAudioChannel : %d with error : %d"),iNewAudioChannel, iError);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				if(iNewAudioChannel == iAudioChannel)
					{
					INFO_PRINTF3(_L("AudioChannelsL() returned iNewAudioChannel : %d with error : %d"),iNewAudioChannel, iError);
					}
				else
					{
					if(iSetError != KErrNotSupported)
						{
						INFO_PRINTF3(_L("AudioChannelsL() returned iNewAudioChannel : %d with error : %d"),iNewAudioChannel, iError);
						iTestStepResult = EFail;
						CActiveScheduler::Stop();
						break;
						}
					else
						{
						INFO_PRINTF3(_L("AudioChannelsL() returned iNewAudioChannel : %d with error : %d"),iNewAudioChannel, iError);
						}
					}
                // Prepare iVideoRecorder
                INFO_PRINTF1(_L("iVideoRecorder->Prepare()"));
                iVideoRecorder->Prepare();
                PrepareState(EVRPrepareComplete, KErrNone);
                break;
				}
            case EVRPrepareComplete:
                // Record iVideoRecorder
                INFO_PRINTF1(_L("iVideoRecorder->Record()"));
                iVideoRecorder->Record();
                TRAPD(err, iRecordTimer = CTestVclntAviRecordTimer::NewL(this, KFiveSeconds));
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
           		// Cancel the timer
                iRecordTimer->Cancel();
                delete iRecordTimer;
                iRecordTimer = NULL;

                CActiveScheduler::Stop();
                iTestStepResult = EPass;
                break;
            }
        }
    }

/**
* set finished on callback from a timer
*
**/
void RTestVclntSetGetAudChanRec::FinishedL()
	{
    // Stop and close iVideoRecorder
    iVideoRecorder->Stop();
    INFO_PRINTF1(_L("iVideoRecorder->Stop()"));
	iVideoRecorder->Close();
	INFO_PRINTF1(_L("iVideoRecorder->Close()"));
	TRAPD(err, FsmL(EVRRecordComplete));
    if (err != KErrNone)
		{
		iError = err;
		CActiveScheduler::Stop();
		iTestStepResult = EFail;
		}
	}

//
// RTestVclntGetSuppAudChan
//
//MM-MMF-VCLNTAVI-I-0305
//

/**
 * RTestVclntGetSuppAudChan::RTestVclntGetSuppAudChan()
 */
RTestVclntGetSuppAudChan::RTestVclntGetSuppAudChan(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError)
    : RTestVclntRecordAviFile(aTestName, aSectName, aKeyName, aExpectedError, EFalse)
    {
    iTestStepResult = EFail;
    }

/**
 * RTestVclntGetSuppAudChan::NewL()
 */
RTestVclntGetSuppAudChan* RTestVclntGetSuppAudChan::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError)
    {
    RTestVclntGetSuppAudChan* self = new (ELeave) RTestVclntGetSuppAudChan(aTestName, aSectName, aKeyName, aExpectedError);
    return self;
    }

/**
 * RTestVclntGetSuppAudChan::DoTestStepL
 */
TVerdict RTestVclntGetSuppAudChan::DoTestStepL()
    {
    // Call the state handler from IDLE state
    FsmL(EVRIdle);

    // Start the scheduler - Done only once !
    CActiveScheduler::Start();

    return iTestStepResult;
    }

/* RTestVclntGetSuppAudChan::FsmL
 */
void RTestVclntGetSuppAudChan::FsmL(TVclntTestRecordEvents aEventCode)
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
				{
				//Set Video Frame size and Video Codec
				TSize frameSize;
				frameSize.iWidth = 176;
				frameSize.iHeight = 144;
				TRAPD(err, iVideoRecorder->SetVideoFrameSizeL(frameSize));
				if (err != KErrNone)
					{
					INFO_PRINTF2(_L("SetVideoFrameSizeL() returns with error :  %d"),iError);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				INFO_PRINTF2(_L("SetVideoFrameSizeL() returned with err : %d"), err);
				_LIT8(KAviVideoCodec,"XVID");
				TRAP(err, iVideoRecorder->SetVideoTypeL(KAviVideoCodec));
				if (err != KErrNone)
					{
					INFO_PRINTF2(_L("SetVideoTypeL() returns with error :  %d"),iError);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				INFO_PRINTF2(_L("SetVideoTypeL() returned with err : %d"), err);
				// SetAudioEnabledL
				TBool enabled = ETrue;
				TRAP(err, iVideoRecorder->SetAudioEnabledL(enabled));
				if (err != KErrNone)
					{
					INFO_PRINTF2(_L("SetAudioEnabledL() returns with error :  %d"),err);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				INFO_PRINTF2(_L("iVideoRecorder->SetAudioEnabledL() returns with error = %d"), err);
				//Get supported number of Audio Channels as 2.
				TRAP(iError,iVideoRecorder->GetSupportedAudioChannelsL(iAudioChannels));
				if (iError != KErrNone)
					{
					INFO_PRINTF2(_L("GetSupportedAudioChannelsL() returns with error :  %d"),iError);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				if(iAudioChannels.Count() >= 1)
					{
					INFO_PRINTF3(_L("GetSupportedAudioChannelsL() returned iAudioChannels.Count: %d with err : %d "),iAudioChannels.Count(), iError);
					for(int i = 0; i< iAudioChannels.Count(); i++)
						{
						INFO_PRINTF2(_L("Supported iAudioChannels:%d"), (iAudioChannels)[i]);
						}
					}
				else
					{
					INFO_PRINTF2(_L("GetSupportedAudioChannelsL() returned with iAudioChannels.Count: %d"), iAudioChannels.Count());
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
                // Prepare iVideoRecorder
                INFO_PRINTF1(_L("iVideoRecorder->Prepare()"));
                iVideoRecorder->Prepare();
                PrepareState(EVRPrepareComplete, KErrNone);
                break;
				}
            case EVRPrepareComplete:
                // Record iVideoRecorder
                INFO_PRINTF1(_L("iVideoRecorder->Record()"));
                iVideoRecorder->Record();
                TRAPD(err, iRecordTimer = CTestVclntAviRecordTimer::NewL(this, KFiveSeconds));
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
           		// Cancel the timer
                iRecordTimer->Cancel();
                delete iRecordTimer;
                iRecordTimer = NULL;

                CActiveScheduler::Stop();
                iTestStepResult = EPass;
                break;
            }
        }
    }

/**
* set finished on callback from a timer
*
**/
void RTestVclntGetSuppAudChan::FinishedL()
	{
    // Stop and close iVideoRecorder
    iVideoRecorder->Stop();
    INFO_PRINTF1(_L("iVideoRecorder->Stop()"));
	iVideoRecorder->Close();
	INFO_PRINTF1(_L("iVideoRecorder->Close()"));
    // Close iVideoRecorder
	iVideoRecorder->Close();
	TRAPD(err, FsmL(EVRRecordComplete));
    if (err != KErrNone)
		{
		iError = err;
		CActiveScheduler::Stop();
		iTestStepResult = EFail;
		}
	}

//
// RTestVclntSetGetAudChanAftOpenPrepThenRec
//
//MM-MMF-VCLNTAVI-I-0306
//

/**
 * RTestVclntSetGetAudChanAftOpenPrepThenRec::RTestVclntSetGetAudChanAftOpenPrepThenRec()
 */
RTestVclntSetGetAudChanAftOpenPrepThenRec::RTestVclntSetGetAudChanAftOpenPrepThenRec(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError)
    : RTestVclntRecordAviFile(aTestName, aSectName, aKeyName, aExpectedError,EFalse)
    {
    iAudioChannel = 2;
    iNewAudioChannel = 0;
    iTestStepResult = EFail;
    }

/**
 * RTestVclntSetGetAudChanAftOpenPrepThenRec::NewL()
 */
RTestVclntSetGetAudChanAftOpenPrepThenRec* RTestVclntSetGetAudChanAftOpenPrepThenRec::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError)
    {
    RTestVclntSetGetAudChanAftOpenPrepThenRec* self = new (ELeave) RTestVclntSetGetAudChanAftOpenPrepThenRec(aTestName, aSectName, aKeyName, aExpectedError);
    return self;
    }

/**
 * RTestVclntSetGetAudChanAftOpenPrepThenRec::DoTestStepL
 */
TVerdict RTestVclntSetGetAudChanAftOpenPrepThenRec::DoTestStepL()
    {
    // Call the state handler from IDLE state
    FsmL(EVRIdle);

    // Start the scheduler - Done only once !
    CActiveScheduler::Start();

    return iTestStepResult;
    }

/**
 * RTestVclntSetGetAudChanAftOpenPrepThenRec::FsmL
 */
void RTestVclntSetGetAudChanAftOpenPrepThenRec::FsmL(TVclntTestRecordEvents aEventCode)
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
				{
				//Set Video Frame size and Video Codec
				TSize frameSize;
				frameSize.iWidth = 176;
				frameSize.iHeight = 144;
				TRAPD(err, iVideoRecorder->SetVideoFrameSizeL(frameSize));
				if (err != KErrNone)
					{
					INFO_PRINTF2(_L("SetVideoFrameSizeL() returns with error :  %d"),err);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				INFO_PRINTF2(_L("SetVideoFrameSizeL() returned with err : %d"), err);
				_LIT8(KAviVideoCodec,"XVID");
				TRAP(err, iVideoRecorder->SetVideoTypeL(KAviVideoCodec));
				if (err != KErrNone)
					{
					INFO_PRINTF2(_L("SetVideoTypeL() returns with error :  %d"),err);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				INFO_PRINTF2(_L("SetVideoTypeL() returned with err : %d"), err);
				// SetAudioEnabledL
				TBool enabled = ETrue;
				TRAP(err, iVideoRecorder->SetAudioEnabledL(enabled));
				if (err != KErrNone)
					{
					INFO_PRINTF2(_L("SetAudioEnabledL() returns with error :  %d"),err);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				INFO_PRINTF2(_L("iVideoRecorder->SetAudioEnabledL() : Error = %d"), err);
                // Prepare iVideoRecorder
                INFO_PRINTF1(_L("iVideoRecorder->Prepare()"));
                iVideoRecorder->Prepare();
                PrepareState(EVRPrepareComplete, KErrNone);
                break;
				}
            case EVRPrepareComplete:
				//Set Audio Channel no. to 2.
				TRAP(iError,iVideoRecorder->SetAudioChannelsL(iAudioChannel));
				if (iError != iTestExpectedError)
					{
					INFO_PRINTF3(_L("SetAudioChannelsL(%d) returns with error : %d"),iAudioChannel, iError);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				INFO_PRINTF3(_L("SetAudioChannelsL(%d) returns with error : %d"),iAudioChannel, iError);
				//Get the Audio Channel no as 2.
				TRAP(iError,iNewAudioChannel=iVideoRecorder->AudioChannelsL());
				if (iError != KErrNone)
					{
					INFO_PRINTF3(_L("AudioChannelsL() returned iNewAudioChannel : %d with error : %d"),iNewAudioChannel, iError);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				INFO_PRINTF3(_L("AudioChannelsL() returned iNewAudioChannel : %d with error : %d"),iNewAudioChannel, iError);
                // Record iVideoRecorder
                INFO_PRINTF1(_L("iVideoRecorder->Record()"));
                iVideoRecorder->Record();
                TRAPD(err, iRecordTimer = CTestVclntAviRecordTimer::NewL(this, KFiveSeconds));
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
           		// Cancel the timer
                iRecordTimer->Cancel();
                delete iRecordTimer;
                iRecordTimer = NULL;

                CActiveScheduler::Stop();
                iTestStepResult = EPass;
                break;
            }
        }
    }

/**
* set finished on callback from a timer
*
**/
void RTestVclntSetGetAudChanAftOpenPrepThenRec::FinishedL()
	{
    // Stop and close iVideoRecorder
    iVideoRecorder->Stop();
    INFO_PRINTF1(_L("iVideoRecorder->Stop()"));
	iVideoRecorder->Close();
	INFO_PRINTF1(_L("iVideoRecorder->Close()"));
	TRAPD(err, FsmL(EVRRecordComplete));
    if (err != KErrNone)
		{
		iError = err;
		CActiveScheduler::Stop();
		iTestStepResult = EFail;
		}
	}


//
//   RTestVclntGetSuppAudChanSetGetAudChanBefAudCodRec
//
//    MM-MMF-VCLNTAVI-I-0307
//

/**
 * RTestVclntGetSuppAudChanSetGetAudChanBefAudCodRec::RTestVclntGetSuppAudChanSetGetAudChanBefAudCodRec()
 */
RTestVclntGetSuppAudChanSetGetAudChanBefAudCodRec::RTestVclntGetSuppAudChanSetGetAudChanBefAudCodRec(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError)
    : RTestVclntRecordAviFile(aTestName, aSectName, aKeyName, aExpectedError,EFalse)
    {
    iAudioChannel = 2;
    iNewAudioChannel = 0;
    iTestStepResult = EFail;
    }

/**
 * RTestVclntGetSuppAudChanSetGetAudChanBefAudCodRec::NewL()
 */
RTestVclntGetSuppAudChanSetGetAudChanBefAudCodRec* RTestVclntGetSuppAudChanSetGetAudChanBefAudCodRec::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError)
    {
    RTestVclntGetSuppAudChanSetGetAudChanBefAudCodRec* self = new (ELeave) RTestVclntGetSuppAudChanSetGetAudChanBefAudCodRec(aTestName, aSectName, aKeyName, aExpectedError);
    return self;
    }

/**
 *RTestVclntGetSuppAudChanSetGetAudChanBefAudCodRec::DoTestStepL
 */
TVerdict RTestVclntGetSuppAudChanSetGetAudChanBefAudCodRec::DoTestStepL()
    {
    // Call the state handler from IDLE state
    FsmL(EVRIdle);

    // Start the scheduler - Done only once !
    CActiveScheduler::Start();

    return iTestStepResult;
    }

/* RTestVclntGetSuppAudChanSetGetAudChanBefAudCodRec::FsmL
 */
void RTestVclntGetSuppAudChanSetGetAudChanBefAudCodRec::FsmL(TVclntTestRecordEvents aEventCode)
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
				//Set Audio Channel no. to 2.
				TRAP(iError,iVideoRecorder->SetAudioChannelsL(iAudioChannel));
				if (iError != iTestExpectedError)
					{
					INFO_PRINTF3(_L("SetAudioChannelsL(%d) returns with error : %d"),iAudioChannel, iError);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				INFO_PRINTF3(_L("SetAudioChannelsL(%d) returns with error : %d"),iAudioChannel, iError);
				//Get the Audio Channel no as 2.
				TRAP(iError,iNewAudioChannel=iVideoRecorder->AudioChannelsL());
				if (iError != KErrNone)
					{
					INFO_PRINTF3(_L("AudioChannelsL() returned iNewAudioChannel : %d with error : %d"),iNewAudioChannel, iError);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				if(iNewAudioChannel == iAudioChannel)
					{
					INFO_PRINTF3(_L("AudioChannelsL() returned iNewAudioChannel : %d with error : %d"),iNewAudioChannel, iError);
					}
				else
					{
					INFO_PRINTF3(_L("AudioChannelsL() returned iNewAudioChannel : %d with error : %d"),iNewAudioChannel, iError);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				//Get supported number of Audio Channels as 2.
				TRAP(iError,iVideoRecorder->GetSupportedAudioChannelsL(iAudioChannels));
				if (iError != KErrNone)
					{
					INFO_PRINTF2(_L("GetSupportedAudioChannelsL() returns with error :  %d"),iError);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				if(iAudioChannels.Count() >= 1)
					{
					INFO_PRINTF3(_L("GetSupportedAudioChannelsL() returned iAudioChannels.Count: %d with err : %d "),iAudioChannels.Count(), iError);
					for(int i = 0; i< iAudioChannels.Count(); i++)
						{
						INFO_PRINTF2(_L("Supported iAudioChannels :%d"), (iAudioChannels)[i]);
						}
					}
				else
					{
					INFO_PRINTF2(_L("GetSupportedAudioChannelsL() returned with iAudioChannels.Count: %d"), iAudioChannels.Count());
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
                // Prepare iVideoRecorder
                INFO_PRINTF1(_L("iVideoRecorder->Prepare()"));
                iVideoRecorder->Prepare();
                PrepareState(EVRPrepareComplete, KErrNone);
                break;
            case EVRPrepareComplete:
                // Record iVideoRecorder
                INFO_PRINTF1(_L("iVideoRecorder->Record()"));
                iVideoRecorder->Record();

                TRAPD(err, iRecordTimer = CTestVclntAviRecordTimer::NewL(this, KFiveSeconds));
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
           		// Cancel the timer
                iRecordTimer->Cancel();
                delete iRecordTimer;
                iRecordTimer = NULL;

                CActiveScheduler::Stop();
                iTestStepResult = EPass;
                break;
            }
        }
    }

/**
* set finished on callback from a timer
*
**/
void RTestVclntGetSuppAudChanSetGetAudChanBefAudCodRec::FinishedL()
	{
    // Stop and close iVideoRecorder
	iVideoRecorder->Stop();
    INFO_PRINTF1(_L("iVideoRecorder->Stop()"));
	iVideoRecorder->Close();
	INFO_PRINTF1(_L("iVideoRecorder->Close()"));
	TRAPD(err, FsmL(EVRRecordComplete));
    if (err != KErrNone)
		{
		iError = err;
		CActiveScheduler::Stop();
		iTestStepResult = EFail;
		}
	}

//
// RTestVclntSetGetAudSRRec
//
//MM-MMF-VCLNTAVI-I-0308, MM-MMF-VCLNTAVI-I-0321
//

/**
 * RTestVclntSetGetAudSRRec::RTestVclntSetGetAudSRRec()
 */
RTestVclntSetGetAudSRRec::RTestVclntSetGetAudSRRec(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError, TInt aAudioSampleRate)
    : RTestVclntRecordAviFile(aTestName, aSectName, aKeyName, aExpectedError, EFalse)
    {
    iAudioSampleRate = aAudioSampleRate;
    iNewAudioSampleRate = 0;
    iTestStepResult = EFail;
    }

/**
 * RTestVclntGetSuppAudChan::NewL()
 */
RTestVclntSetGetAudSRRec* RTestVclntSetGetAudSRRec::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError, TInt aAudioSampleRate)
    {
    RTestVclntSetGetAudSRRec* self = new (ELeave) RTestVclntSetGetAudSRRec(aTestName, aSectName, aKeyName, aExpectedError, aAudioSampleRate);
    return self;
    }

/**
 * RTestVclntSetGetAudSRRec::DoTestStepL
 */
TVerdict RTestVclntSetGetAudSRRec::DoTestStepL()
    {
    // Call the state handler from IDLE state
    FsmL(EVRIdle);

    // Start the scheduler - Done only once !
    CActiveScheduler::Start();

    return iTestStepResult;
    }

/* RTestVclntSetGetAudSRRec::FsmL
 */
void RTestVclntSetGetAudSRRec::FsmL(TVclntTestRecordEvents aEventCode)
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
            	{
				//Set Video Frame size and Video Codec
				TSize frameSize;
				frameSize.iWidth = 176;
				frameSize.iHeight = 144;
				TRAPD(err, iVideoRecorder->SetVideoFrameSizeL(frameSize));
				if (err != KErrNone)
					{
					INFO_PRINTF2(_L("SetVideoFrameSizeL() returns with error :  %d"),iError);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				INFO_PRINTF2(_L("SetVideoFrameSizeL() returned with err : %d"), err);
				_LIT8(KAviVideoCodec,"XVID");
				TRAP(err, iVideoRecorder->SetVideoTypeL(KAviVideoCodec));
				if (err != KErrNone)
					{
					INFO_PRINTF2(_L("SetVideoTypeL() returns with error :  %d"),iError);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				INFO_PRINTF2(_L("SetVideoTypeL() returned with err : %d"), err);
				// SetAudioEnabledL
				TBool enabled = ETrue;
				TRAP(err, iVideoRecorder->SetAudioEnabledL(enabled));
				if (err != KErrNone)
					{
					INFO_PRINTF2(_L("SetAudioEnabledL() returns with error :  %d"),err);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				INFO_PRINTF2(_L("iVideoRecorder->SetAudioEnabledL() : Error = %d"), err);
				//Set audio sample rate to 22050Hz.
				TInt iSetError; // used for invalid iAudioSampleRate
				TRAP(iError,iVideoRecorder->SetAudioSampleRateL(iAudioSampleRate));
				iSetError = iError;
				if (iError != iTestExpectedError)
					{
					INFO_PRINTF3(_L("SetAudioSampleRateL(%d) returns with error :  %d"),iAudioSampleRate, iError);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				INFO_PRINTF3(_L("SetAudioSampleRateL(%d) returns with error :  %d"),iAudioSampleRate, iError);
				//Get the audio sample rate as 22050Hz.
				TRAP(iError,iNewAudioSampleRate=iVideoRecorder->AudioSampleRateL());
				if (iError != KErrNone)
					{
					INFO_PRINTF3(_L("AudioSampleRateL() returned iNewAudioSampleRate : %d with error :  %d"), iNewAudioSampleRate, iError);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				if(iNewAudioSampleRate == iAudioSampleRate)
					{
					INFO_PRINTF3(_L("AudioSampleRateL() returned iNewAudioSampleRate : %d with error :  %d"), iNewAudioSampleRate, iError);
					}
				else
					{
					if(iSetError != KErrNotSupported)
						{
						INFO_PRINTF3(_L("AudioSampleRateL() returned iNewAudioSampleRate : %d with error :  %d"), iNewAudioSampleRate, iError);
						iTestStepResult = EFail;
						CActiveScheduler::Stop();
						break;
						}
					else
						{
						INFO_PRINTF3(_L("AudioSampleRateL() returned iNewAudioSampleRate : %d with error :  %d"), iNewAudioSampleRate, iError);
						}
					}
                // Prepare iVideoRecorder
                INFO_PRINTF1(_L("iVideoRecorder->Prepare()"));
                iVideoRecorder->Prepare();
                PrepareState(EVRPrepareComplete, KErrNone);
                break;
            	}
            case EVRPrepareComplete:
                // Record iVideoRecorder
                INFO_PRINTF1(_L("iVideoRecorder->Record()"));
                iVideoRecorder->Record();
                TRAPD(err, iRecordTimer = CTestVclntAviRecordTimer::NewL(this, KFiveSeconds));
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
           		// Cancel the timer
                iRecordTimer->Cancel();
                delete iRecordTimer;
                iRecordTimer = NULL;

                CActiveScheduler::Stop();
                iTestStepResult = EPass;
                break;
            }
        }
    }

/**
* set finished on callback from a timer
*
**/
void RTestVclntSetGetAudSRRec::FinishedL()
	{
    // Stop and close iVideoRecorder
	iVideoRecorder->Stop();
    INFO_PRINTF1(_L("iVideoRecorder->Stop()"));
	iVideoRecorder->Close();
	INFO_PRINTF1(_L("iVideoRecorder->Close()"));
	TRAPD(err, FsmL(EVRRecordComplete));
    if (err != KErrNone)
		{
		iError = err;
		CActiveScheduler::Stop();
		iTestStepResult = EFail;
		}
	}

//
// RTestVclntGetSuppAudSR
//
//MM-MMF-VCLNTAVI-I-0309
//

/**
 * RTestVclntGetSuppAudSR::RTestVclntGetSuppAudSR()
 */
RTestVclntGetSuppAudSR::RTestVclntGetSuppAudSR(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError)
    : RTestVclntRecordAviFile(aTestName, aSectName, aKeyName, aExpectedError,EFalse)
    {
    iTestStepResult = EFail;
    }

/**
 * RTestVclntGetSuppAudSR::NewL()
 */
RTestVclntGetSuppAudSR* RTestVclntGetSuppAudSR::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError)
    {
    RTestVclntGetSuppAudSR* self = new (ELeave) RTestVclntGetSuppAudSR(aTestName, aSectName, aKeyName, aExpectedError);
    return self;
    }

/**
 * RTestVclntGetSuppAudSR::DoTestStepL
 */
TVerdict RTestVclntGetSuppAudSR::DoTestStepL()
    {
    // Call the state handler from IDLE state
    FsmL(EVRIdle);

    // Start the scheduler - Done only once !
    CActiveScheduler::Start();

    return iTestStepResult;
    }

/* RTestVclntGetSuppAudSR::FsmL
 */
void RTestVclntGetSuppAudSR::FsmL(TVclntTestRecordEvents aEventCode)
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
				{
				//Set Video Frame size and Video Codec
				TSize frameSize;
				frameSize.iWidth = 176;
				frameSize.iHeight = 144;
				TRAPD(err, iVideoRecorder->SetVideoFrameSizeL(frameSize));
				if (err != KErrNone)
					{
					INFO_PRINTF2(_L("SetVideoFrameSizeL() returns with error :  %d"),iError);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				INFO_PRINTF2(_L("SetVideoFrameSizeL() returned with err : %d"), err);
				_LIT8(KAviVideoCodec,"XVID");
				TRAP(err, iVideoRecorder->SetVideoTypeL(KAviVideoCodec));
				if (err != KErrNone)
					{
					INFO_PRINTF2(_L("SetVideoTypeL() returns with error :  %d"),iError);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				INFO_PRINTF2(_L("SetVideoTypeL() returned with err : %d"), err);
				// SetAudioEnabledL
				TBool enabled = ETrue;
				TRAP(err, iVideoRecorder->SetAudioEnabledL(enabled));
				if (err != KErrNone)
					{
					INFO_PRINTF2(_L("SetAudioEnabledL() returns with error :  %d"),err);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				INFO_PRINTF2(_L("iVideoRecorder->SetAudioEnabledL() : Error = %d"), err);
				//Get the supported Audio Sample Rate.
				TRAP(iError,iVideoRecorder->GetSupportedAudioSampleRatesL(iAudioSampleRates));
				if (iError != KErrNone)
					{
					INFO_PRINTF2(_L("GetSupportedAudioSampleRatesL() returns with error :  %d"),iError);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				if(iAudioSampleRates.Count() >= 1)
					{
					INFO_PRINTF3(_L("GetSupportedAudioSampleRatesL() returned iAudioSampleRates.Count: %d with err : %d "),iAudioSampleRates.Count(), iError);
					for(int i = 0; i< iAudioSampleRates.Count(); i++)
						{
						INFO_PRINTF2(_L("Supported iAudioChannels :%d"), (iAudioSampleRates)[i]);
						}
					}
				else
					{
					INFO_PRINTF2(_L("GetSupportedAudioSampleRatesL() returned with iAudioSampleRates.Count: %d"), iAudioSampleRates.Count());
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
                // Prepare iVideoRecorder
                INFO_PRINTF1(_L("iVideoRecorder->Prepare()"));
                iVideoRecorder->Prepare();
                PrepareState(EVRPrepareComplete, KErrNone);
                break;
				}
            case EVRPrepareComplete:
                // Record iVideoRecorder
                INFO_PRINTF1(_L("iVideoRecorder->Record()"));
                iVideoRecorder->Record();
                TRAPD(err, iRecordTimer = CTestVclntAviRecordTimer::NewL(this, KFiveSeconds));
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
           		// Cancel the timer
                iRecordTimer->Cancel();
                delete iRecordTimer;
                iRecordTimer = NULL;

                CActiveScheduler::Stop();
                iTestStepResult = EPass;
                break;
            }
        }
    }

/**
* set finished on callback from a timer
*
**/
void RTestVclntGetSuppAudSR::FinishedL()
	{
    // Stop and close iVideoRecorder
	iVideoRecorder->Stop();
    INFO_PRINTF1(_L("iVideoRecorder->Stop()"));
	iVideoRecorder->Close();
	INFO_PRINTF1(_L("iVideoRecorder->Close()"));
	TRAPD(err, FsmL(EVRRecordComplete));
    if (err != KErrNone)
		{
		iError = err;
		CActiveScheduler::Stop();
		iTestStepResult = EFail;
		}
	}

//
// RTestVclntSetGetSRAftOpenPrepRec
//
//MM-MMF-VCLNTAVI-I-0310
//

/**
 * RTestVclntSetGetSRAftOpenPrepRec::RTestVclntSetGetSRAftOpenPrepRec()
 */
RTestVclntSetGetSRAftOpenPrepRec::RTestVclntSetGetSRAftOpenPrepRec(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError)
    : RTestVclntRecordAviFile(aTestName, aSectName, aKeyName, aExpectedError,EFalse)
    {
    iAudioSampleRate = 22050;
    iNewAudioSampleRate = 0;
    iTestStepResult = EFail;
    }

/**
 * RTestVclntSetGetSRAftOpenPrepRec::NewL()
 */
RTestVclntSetGetSRAftOpenPrepRec* RTestVclntSetGetSRAftOpenPrepRec::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError)
    {
    RTestVclntSetGetSRAftOpenPrepRec* self = new (ELeave) RTestVclntSetGetSRAftOpenPrepRec(aTestName, aSectName, aKeyName, aExpectedError);
    return self;
    }

/**
 * RTestVclntSetGetSRAftOpenPrepRec::DoTestStepL
 */
TVerdict RTestVclntSetGetSRAftOpenPrepRec::DoTestStepL()
    {
    // Call the state handler from IDLE state
    FsmL(EVRIdle);

    // Start the scheduler - Done only once !
    CActiveScheduler::Start();

    return iTestStepResult;
    }

/**
 * RTestVclntSetGetSRAftOpenPrepRec::FsmL
 */
void RTestVclntSetGetSRAftOpenPrepRec::FsmL(TVclntTestRecordEvents aEventCode)
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
            	{
				//Set Video Frame size and Video Codec
				TSize frameSize;
				frameSize.iWidth = 176;
				frameSize.iHeight = 144;
				TRAPD(err, iVideoRecorder->SetVideoFrameSizeL(frameSize));
				if (err != KErrNone)
					{
					INFO_PRINTF2(_L("SetVideoFrameSizeL() returns with error :  %d"),err);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				INFO_PRINTF2(_L("SetVideoFrameSizeL() returned with err : %d"), err);
				_LIT8(KAviVideoCodec,"XVID");
				TRAP(err, iVideoRecorder->SetVideoTypeL(KAviVideoCodec));
				if (err != KErrNone)
					{
					INFO_PRINTF2(_L("SetVideoTypeL() returns with error :  %d"),err);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				INFO_PRINTF2(_L("SetVideoTypeL() returned with err : %d"), err);
				// SetAudioEnabledL
				TBool enabled = ETrue;
				TRAP(err, iVideoRecorder->SetAudioEnabledL(enabled));
				if (err != KErrNone)
					{
					INFO_PRINTF2(_L("SetAudioEnabledL() returns with error :  %d"),err);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				INFO_PRINTF2(_L("iVideoRecorder->SetAudioEnabledL() : Error = %d"), err);
                // Prepare iVideoRecorder
                INFO_PRINTF1(_L("iVideoRecorder->Prepare()"));
                iVideoRecorder->Prepare();
                PrepareState(EVRPrepareComplete, KErrNone);
                break;
            	}
            case EVRPrepareComplete:
				//Set audio sample rate to 22050Hz.
				TRAP(iError,iVideoRecorder->SetAudioSampleRateL(iAudioSampleRate));
				if (iError != iTestExpectedError)
					{
					INFO_PRINTF3(_L("SetAudioSampleRateL(%d) returns with error :  %d"),iAudioSampleRate, iError);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				INFO_PRINTF3(_L("SetAudioSampleRateL(%d) returns with error :  %d"),iAudioSampleRate, iError);
				//Get the audio sample rate as 22050Hz.
				TRAP(iError,iNewAudioSampleRate=iVideoRecorder->AudioSampleRateL());
				if (iError != KErrNone)
					{
					INFO_PRINTF3(_L("AudioSampleRateL() returned iNewAudioSampleRate : %d with error :  %d"), iNewAudioSampleRate, iError);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				INFO_PRINTF3(_L("AudioSampleRateL() returned iNewAudioSampleRate : %d with error :  %d"), iNewAudioSampleRate, iError);
                // Record iVideoRecorder
                INFO_PRINTF1(_L("iVideoRecorder->Record()"));
                iVideoRecorder->Record();
                TRAPD(err, iRecordTimer = CTestVclntAviRecordTimer::NewL(this, KFiveSeconds));
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
           		// Cancel the timer
                iRecordTimer->Cancel();
                delete iRecordTimer;
                iRecordTimer = NULL;

                CActiveScheduler::Stop();
                iTestStepResult = EPass;
                break;
            }
        }
    }

/**
* set finished on callback from a timer
*
**/
void RTestVclntSetGetSRAftOpenPrepRec::FinishedL()
	{
    // Stop and close iVideoRecorder
	iVideoRecorder->Stop();
    INFO_PRINTF1(_L("iVideoRecorder->Stop()"));
	iVideoRecorder->Close();
	INFO_PRINTF1(_L("iVideoRecorder->Close()"));
	TRAPD(err, FsmL(EVRRecordComplete));
    if (err != KErrNone)
		{
		iError = err;
		CActiveScheduler::Stop();
		iTestStepResult = EFail;
		}
	}

//
// RTestVclntGetSuppSRSetGetSRBefAudCodRec
//
//MM-MMF-VCLNTAVI-I-0311
//

/**
 * RTestVclntGetSuppSRSetGetSRBefAudCodRec::RTestVclntGetSuppSRSetGetSRBefAudCodRec()
 */
RTestVclntGetSuppSRSetGetSRBefAudCodRec::RTestVclntGetSuppSRSetGetSRBefAudCodRec(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError)
    : RTestVclntRecordAviFile(aTestName, aSectName, aKeyName, aExpectedError,EFalse)
    {
    iAudioSampleRate = 22050;
    iNewAudioSampleRate = 0;
    iTestStepResult = EFail;
    }

/**
 * RTestVclntGetSuppSRSetGetSRBefAudCodRec::NewL()
 */
RTestVclntGetSuppSRSetGetSRBefAudCodRec* RTestVclntGetSuppSRSetGetSRBefAudCodRec::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError)
    {
    RTestVclntGetSuppSRSetGetSRBefAudCodRec* self = new (ELeave) RTestVclntGetSuppSRSetGetSRBefAudCodRec(aTestName, aSectName, aKeyName, aExpectedError);
    return self;
    }

/**
 * RTestVclntGetSuppSRSetGetSRBefAudCodRec::DoTestStepL
 */
TVerdict RTestVclntGetSuppSRSetGetSRBefAudCodRec::DoTestStepL()
    {
    // Call the state handler from IDLE state
    FsmL(EVRIdle);

    // Start the scheduler - Done only once !
    CActiveScheduler::Start();

    return iTestStepResult;
    }

/**
 * RTestVclntGetSuppSRSetGetSRBefAudCodRec::FsmL
 */
void RTestVclntGetSuppSRSetGetSRBefAudCodRec::FsmL(TVclntTestRecordEvents aEventCode)
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
				//Set audio sample rate to 22050Hz.
				TRAP(iError,iVideoRecorder->SetAudioSampleRateL(iAudioSampleRate));
				if (iError != iTestExpectedError)
					{
					INFO_PRINTF3(_L("SetAudioSampleRateL(%d) returns with error :  %d"),iAudioSampleRate, iError);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				INFO_PRINTF3(_L("SetAudioSampleRateL(%d) returns with error :  %d"),iAudioSampleRate, iError);
				//Get the audio sample rate as 22050Hz.
				TRAP(iError,iNewAudioSampleRate=iVideoRecorder->AudioSampleRateL());
				if (iError != KErrNone)
					{
					INFO_PRINTF3(_L("AudioSampleRateL() returned iNewAudioSampleRate : %d with error :  %d"), iNewAudioSampleRate, iError);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				if(iNewAudioSampleRate == iAudioSampleRate)
					{
					INFO_PRINTF3(_L("AudioSampleRateL() returned iNewAudioSampleRate : %d with error :  %d"), iNewAudioSampleRate, iError);
					}
				else
					{
					INFO_PRINTF3(_L("AudioSampleRateL() returned iNewAudioSampleRate : %d with error :  %d"), iNewAudioSampleRate, iError);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				//Get the supported Audio Sample Rate.
				TRAP(iError,iVideoRecorder->GetSupportedAudioSampleRatesL(iAudioSampleRates));
				if (iError != KErrNone)
					{
					INFO_PRINTF2(_L("GetSupportedAudioSampleRatesL() returns with error :  %d"),iError);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				if(iAudioSampleRates.Count() >= 1)
					{
					INFO_PRINTF3(_L("GetSupportedAudioSampleRatesL() returned iAudioSampleRates.Count: %d with err : %d "),iAudioSampleRates.Count(), iError);
					for(int i = 0; i< iAudioSampleRates.Count(); i++)
						{
						INFO_PRINTF2(_L("Supported iAudioChannels :%d"), (iAudioSampleRates)[i]);
						}
					}
				else
					{
					INFO_PRINTF2(_L("GetSupportedAudioSampleRatesL() returned with iAudioSampleRates.Count: %d"), iAudioSampleRates.Count());
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
                // Prepare iVideoRecorder
                INFO_PRINTF1(_L("iVideoRecorder->Prepare()"));
                iVideoRecorder->Prepare();
                PrepareState(EVRPrepareComplete, KErrNone);
                break;
            case EVRPrepareComplete:
                // Record iVideoRecorder
                INFO_PRINTF1(_L("iVideoRecorder->Record()"));
                iVideoRecorder->Record();

                TRAPD(err, iRecordTimer = CTestVclntAviRecordTimer::NewL(this, KFiveSeconds));
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
           		// Cancel the timer
                iRecordTimer->Cancel();
                delete iRecordTimer;
                iRecordTimer = NULL;

                CActiveScheduler::Stop();
                iTestStepResult = EPass;
                break;
            }
        }
    }

/**
* set finished on callback from a timer
*
**/
void RTestVclntGetSuppSRSetGetSRBefAudCodRec::FinishedL()
	{
    // Stop and close iVideoRecorder
	iVideoRecorder->Stop();
    INFO_PRINTF1(_L("iVideoRecorder->Stop()"));
	iVideoRecorder->Close();
	INFO_PRINTF1(_L("iVideoRecorder->Close()"));
	TRAPD(err, FsmL(EVRRecordComplete));
    if (err != KErrNone)
		{
		iError = err;
		CActiveScheduler::Stop();
		iTestStepResult = EFail;
		}
	}

//
// RTestVclntGetDefPARAudSRAudChanAftOpenBefPrep
//
//MM-MMF-VCLNTAVI-I-0312
//

/**
 * RTestVclntGetDefPARAudSRAudChanAftOpenBefPrep::RTestVclntGetDefPARAudSRAudChanAftOpenBefPrep()
 */
RTestVclntGetDefPARAudSRAudChanAftOpenBefPrep::RTestVclntGetDefPARAudSRAudChanAftOpenBefPrep(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError)
    : RTestVclntRecordAviFile(aTestName, aSectName, aKeyName, aExpectedError,EFalse)
    {
    iNewAudioChannel = 0;
    iNewAudioSampleRate = 0;
    iNewAspectRatio.iNumerator = 0;
    iNewAspectRatio.iDenominator = 0;
    iTestStepResult = EFail;
    }

/**
 * RTestVclntGetDefPARAudSRAudChanAftOpenBefPrep::NewL()
 */
RTestVclntGetDefPARAudSRAudChanAftOpenBefPrep* RTestVclntGetDefPARAudSRAudChanAftOpenBefPrep::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError)
    {
    RTestVclntGetDefPARAudSRAudChanAftOpenBefPrep* self = new (ELeave) RTestVclntGetDefPARAudSRAudChanAftOpenBefPrep(aTestName, aSectName, aKeyName, aExpectedError);
    return self;
    }

/**
 * RTestVclntGetDefPARAudSRAudChanAftOpenBefPrep::DoTestStepL
 */
TVerdict RTestVclntGetDefPARAudSRAudChanAftOpenBefPrep::DoTestStepL()
    {
    // Call the state handler from IDLE state
    FsmL(EVRIdle);

    // Start the scheduler - Done only once !
    CActiveScheduler::Start();

    return iTestStepResult;
    }

/* RTestVclntGetDefPARAudSRAudChanAftOpenBefPrep::FsmL
 */
void RTestVclntGetDefPARAudSRAudChanAftOpenBefPrep::FsmL(TVclntTestRecordEvents aEventCode)
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
				{
				//Set Video Frame size and Video Codec
				TSize frameSize;
				frameSize.iWidth = 176;
				frameSize.iHeight = 144;
				TRAPD(err, iVideoRecorder->SetVideoFrameSizeL(frameSize));
				if (err != KErrNone)
					{
					INFO_PRINTF2(_L("SetVideoFrameSizeL() returns with error :  %d"),iError);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				INFO_PRINTF2(_L("SetVideoFrameSizeL() returned with err : %d"), err);
				_LIT8(KAviVideoCodec,"XVID");
				TRAP(err, iVideoRecorder->SetVideoTypeL(KAviVideoCodec));
				if (err != KErrNone)
					{
					INFO_PRINTF2(_L("SetVideoTypeL() returns with error :  %d"),iError);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				INFO_PRINTF2(_L("SetVideoTypeL() returned with err : %d"), err);
				// SetAudioEnabledL
				TBool enabled = ETrue;
				TRAP(err, iVideoRecorder->SetAudioEnabledL(enabled));
				if (err != KErrNone)
					{
					INFO_PRINTF2(_L("SetAudioEnabledL() returns with error :  %d"),err);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				INFO_PRINTF2(_L("iVideoRecorder->SetAudioEnabledL() : Error = %d"), err);
				//Get pixel aspect ratio as 1:1
				TRAP(iError,iVideoRecorder->GetPixelAspectRatioL(iNewAspectRatio));
				if (iError != KErrNone)
					{
					INFO_PRINTF2(_L("GetPixelAspectRatioL() returns with error :  %d"),iError);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				INFO_PRINTF4(_L("GetPixelAspectRatioL() returned default PAR %d:%d with err : %d"), iNewAspectRatio.iNumerator, iNewAspectRatio.iDenominator, iError);
				//Get the Audio Channel no as 2.
				TRAP(iError,iNewAudioChannel=iVideoRecorder->AudioChannelsL());
				if (iError != KErrNone)
					{
					INFO_PRINTF3(_L("AudioChannelsL() returned default AudioChannel : %d with error : %d"),iNewAudioChannel, iError);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				INFO_PRINTF3(_L("AudioChannelsL() returned default AudioChannel : %d with error : %d"),iNewAudioChannel, iError);
				//Get the audio sample rate as 22050Hz.
				TRAP(iError,iNewAudioSampleRate=iVideoRecorder->AudioSampleRateL());
				if (iError != KErrNone)
					{
					INFO_PRINTF3(_L("AudioSampleRateL() returned default AudioSampleRate : %d with error :  %d"), iNewAudioSampleRate, iError);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				INFO_PRINTF3(_L("AudioSampleRateL() returned default AudioSampleRate : %d with error :  %d"), iNewAudioSampleRate, iError);
                // Prepare iVideoRecorder
                INFO_PRINTF1(_L("iVideoRecorder->Prepare()"));
                iVideoRecorder->Prepare();
                PrepareState(EVRPrepareComplete, KErrNone);
                break;
				}
            case EVRPrepareComplete:
                // Record iVideoRecorder
                INFO_PRINTF1(_L("iVideoRecorder->Record()"));
                iVideoRecorder->Record();
                TRAPD(err, iRecordTimer = CTestVclntAviRecordTimer::NewL(this, KFiveSeconds));
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
           		// Cancel the timer
                iRecordTimer->Cancel();
                delete iRecordTimer;
                iRecordTimer = NULL;

                CActiveScheduler::Stop();
                iTestStepResult = EPass;
                break;
            }
        }
    }

/**
* set finished on callback from a timer
*
**/
void RTestVclntGetDefPARAudSRAudChanAftOpenBefPrep::FinishedL()
	{
    // Stop and close iVideoRecorder
	iVideoRecorder->Stop();
    INFO_PRINTF1(_L("iVideoRecorder->Stop()"));
	iVideoRecorder->Close();
	INFO_PRINTF1(_L("iVideoRecorder->Close()"));
	TRAPD(err, FsmL(EVRRecordComplete));
    if (err != KErrNone)
		{
		iError = err;
		CActiveScheduler::Stop();
		iTestStepResult = EFail;
		}
	}

//
// RTestVclntSetGetPARAudChaAudSRBRGain
//
//MM-MMF-VCLNTAVI-I-0313
//

/**
 * RTestVclntSetGetPARAudVidBRAudGain::RTestVclntSetGetPARAudVidBRAudGain()
 */
RTestVclntSetGetPARAudVidBRAudGain::RTestVclntSetGetPARAudVidBRAudGain(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError)
    : RTestVclntRecordAviFile(aTestName, aSectName, aKeyName, aExpectedError,EFalse)
    {
    iGain = 10;
	iAspectRatio.iNumerator = 1;
	iAspectRatio.iDenominator = 1;
	iNewAspectRatio.iNumerator = 0;
	iNewAspectRatio.iDenominator = 0;
    iTestStepResult = EFail;
    }

/**
 * RTestVclntSetGetPARAudVidBRAudGain::NewL()
 */
RTestVclntSetGetPARAudVidBRAudGain* RTestVclntSetGetPARAudVidBRAudGain::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError)
    {
    RTestVclntSetGetPARAudVidBRAudGain* self = new (ELeave) RTestVclntSetGetPARAudVidBRAudGain(aTestName, aSectName, aKeyName, aExpectedError);
    return self;
    }

/**
 * RTestVclntSetGetPARAudVidBRAudGain::DoTestStepL
 */
TVerdict RTestVclntSetGetPARAudVidBRAudGain::DoTestStepL()
    {
    // Call the state handler from IDLE state
    FsmL(EVRIdle);

    // Start the scheduler - Done only once !
    CActiveScheduler::Start();

    return iTestStepResult;
    }

/* RTestVclntSetGetPARAudVidBRAudGain::FsmL
 */
void RTestVclntSetGetPARAudVidBRAudGain::FsmL(TVclntTestRecordEvents aEventCode)
    {
    if (FsmCheck(aEventCode))
        {
        iCurrentEvent = aEventCode;
        switch (aEventCode)
            {
            case EVRIdle:
                // Open iVideoRecorder
                INFO_PRINTF2(_L("iVideoRecorder->OpenFileL()"), &iFilename);
                TRAP(iError,iVideoRecorder->OpenFileL(iFilename, iCameraHandle, KMmfTestAviRecordControllerUid, KAviRecordFormatUid));
                PrepareState(EVROpenComplete, KErrNone);
                break;
            case EVROpenComplete:
				{
				//Set Video Frame size and Video Codec
				TSize frameSize;
				frameSize.iWidth = 176;
				frameSize.iHeight = 144;
				TRAPD(err, iVideoRecorder->SetVideoFrameSizeL(frameSize));
				if (err != KErrNone)
					{
					INFO_PRINTF2(_L("SetVideoFrameSizeL() returns with error :  %d"),iError);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				INFO_PRINTF2(_L("SetVideoFrameSizeL() returned with err : %d"), err);
				_LIT8(KAviVideoCodec,"XVID");
				TRAP(err, iVideoRecorder->SetVideoTypeL(KAviVideoCodec));
				if (err != KErrNone)
					{
					INFO_PRINTF2(_L("SetVideoTypeL() returns with error :  %d"),iError);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				INFO_PRINTF2(_L("SetVideoTypeL() returned with err : %d"), err);
				//Set pixel aspect ratio to 1:1
				TRAP(iError,iVideoRecorder->SetPixelAspectRatioL(iAspectRatio));
				if (iError != iTestExpectedError)
					{
					INFO_PRINTF4(_L("SetPixelAspectRatioL(%d:%d) returns with error :  %d "),iAspectRatio.iNumerator, iAspectRatio.iDenominator, iError);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				INFO_PRINTF4(_L("SetPixelAspectRatioL(%d:%d) returns with error :  %d "),iAspectRatio.iNumerator, iAspectRatio.iDenominator, iError);
				//SetVideoBitRateL
				TRAP(err, iVideoRecorder->SetVideoBitRateL(64000));
				if((err != KErrNone) && (err != KErrNotSupported))
					{
					INFO_PRINTF2(_L("SetVideoBitRateL(64000) returned with err : %d"), err);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				INFO_PRINTF2(_L("SetVideoBitRateL(64000) returned with err : %d"), err);
				// enable audio while recording
				TRAP(err, iVideoRecorder->SetAudioEnabledL(ETrue));
				if((err != KErrNone) && (err != KErrNotSupported))
					{
					INFO_PRINTF2(_L("SetAudioEnabledL(ETrue) left with error %d"), err);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				INFO_PRINTF2(_L("SetAudioEnabledL(ETrue) left with error %d"), err);
				// SetAudioTypeL
				TRAP(err,iVideoRecorder->SetAudioTypeL(KMMFFourCCCodePCMU16));
				if((err != KErrNone) && (err != KErrNotSupported))
					{
					INFO_PRINTF2(_L("iVideoRecorder->SetAudioTypeL(KMMFFourCCCodePCMU16) : Error = %d"), err);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				INFO_PRINTF2(_L("iVideoRecorder->SetAudioTypeL(KMMFFourCCCodePCMU16) : Error = %d"), err);
    			//SetAudioBitRateL
				TRAP(err, iVideoRecorder->SetAudioBitRateL(705600));
				if((err != KErrNone) && (err != KErrNotSupported))
					{
					INFO_PRINTF2(_L("SetAudioBitRateL(705600) returned with err : %d"), err);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				INFO_PRINTF2(_L("SetAudioBitRateL(705600) returned with err : %d"), err);
				// set audio gain to value passed (iGain)
				TRAP(err, iVideoRecorder->SetGainL(iGain));
				if((err != KErrNone) && (err != KErrNotSupported))
					{
					INFO_PRINTF3(_L("SetGainL(%d) left with error %d"), iGain, err);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				INFO_PRINTF3(_L("SetGainL(%d) left with error %d"), iGain, err);
                // Prepare iVideoRecorder
                INFO_PRINTF1(_L("iVideoRecorder->Prepare()"));
                iVideoRecorder->Prepare();
                PrepareState(EVRPrepareComplete, KErrNone);
                break;
				}
            case EVRPrepareComplete:
                // Record iVideoRecorder
                INFO_PRINTF1(_L("iVideoRecorder->Record()"));
                iVideoRecorder->Record();
                TRAPD(err, iRecordTimer = CTestVclntAviRecordTimer::NewL(this, KFiveSeconds));
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
				TRAP(err, iRecordTimer = CTestVclntAviRecordTimer::NewL(this, KFiveSeconds));
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CTestVclntAviRecordTimer::NewL left with error = %d"), err);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				iRecordTimer->Start();
                PrepareState(EVRRecordStopped, KErrNone);
                break;
            case EVRRecordStopped:
				{
				// Cancel the timer
                iRecordTimer->Cancel();
                delete iRecordTimer;
                iRecordTimer = NULL;
                // Stop Record iVideoRecorder
                INFO_PRINTF1(_L("iVideoRecorder->Stop()"));
                iVideoRecorder->Stop();
				//Get pixel aspect ratio as 1:1
				TRAP(iError,iVideoRecorder->GetPixelAspectRatioL(iNewAspectRatio));
				if (iError != KErrNone)
					{
					INFO_PRINTF2(_L("GetPixelAspectRatioL() returns with error :  %d"),iError);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				if(iAspectRatio == iNewAspectRatio)
					{
					INFO_PRINTF4(_L("GetPixelAspectRatioL() returned %d:%d with err : %d"), iNewAspectRatio.iNumerator, iNewAspectRatio.iDenominator, iError);
					}
				else
					{
					INFO_PRINTF4(_L("GetPixelAspectRatioL() returned %d:%d with err : %d"), iNewAspectRatio.iNumerator, iNewAspectRatio.iDenominator, iError);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				TInt getVideoBitrate = 0;
				TRAPD(err, getVideoBitrate = iVideoRecorder->VideoBitRateL());
				if((err != KErrNone) && (err != KErrNotSupported))
					{
					INFO_PRINTF3(_L("VideoBitRateL returned getVideoBitrate : %d with err : %d"), getVideoBitrate, err);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				if(getVideoBitrate == 64000)
					{
					INFO_PRINTF3(_L("VideoBitRateL returned getVideoBitrate : %d with err : %d"), getVideoBitrate, err);
					}
				else
					{
					INFO_PRINTF3(_L("VideoBitRateL returned getVideoBitrate : %d with err : %d"), getVideoBitrate, err);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				TInt getAudioBitrate = 0;
				TRAP(err, getAudioBitrate = iVideoRecorder->AudioBitRateL());
				if((err != KErrNone) && (err != KErrNotSupported))
					{
					INFO_PRINTF3(_L("AudioBitRateL returned getAudioBitrate : %d with err : %d"), getAudioBitrate, err);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				if(getAudioBitrate == 705600)
					{
					INFO_PRINTF3(_L("AudioBitRateL returned getAudioBitrate : %d with err : %d"), getAudioBitrate, err);
					}
				else
					{
					INFO_PRINTF3(_L("AudioBitRateL returned getAudioBitrate : %d with err : %d"), getAudioBitrate, err);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				// Get the gain value
				TInt newGain = 0;
				TRAP(err, newGain = iVideoRecorder->GainL());
				if((err != KErrNone) && (err != KErrNotSupported))
					{
					INFO_PRINTF3(_L("GainL returned newGain : %d with err : %d"), newGain, err);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				if(newGain == 10)
					{
					INFO_PRINTF3(_L("GainL returned newGain : %d with err : %d"), newGain, err);
					}
				else
					{
					INFO_PRINTF3(_L("GainL returned newGain : %d with err : %d"), newGain, err);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
                // Record iVideoRecorder
                INFO_PRINTF1(_L("iVideoRecorder->Record()"));
                iVideoRecorder->Record();
                TRAP(err, iRecordTimer = CTestVclntAviRecordTimer::NewL(this, KFiveSeconds));
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
				}
           	case EVRRecordComplete:
           		// Cancel the timer
                iRecordTimer->Cancel();
                delete iRecordTimer;
                iRecordTimer = NULL;

                CActiveScheduler::Stop();
                iTestStepResult = EPass;
                break;
            }
        }
    }

/**
* set finished on callback from a timer
*
**/
void RTestVclntSetGetPARAudVidBRAudGain::FinishedL()
	{
	if(iCurrentEvent == EVRPrepareComplete)
		{
		TRAPD(err, FsmL(EVRRecordPaused));
		if (err != KErrNone)
			{
			iError = err;
			CActiveScheduler::Stop();
			iTestStepResult = EFail;
			}
		}
	else if(iCurrentEvent == EVRRecordPaused)
    	{
		TRAPD(err, FsmL(EVRRecordResumed));
		if (err != KErrNone)
			{
			iError = err;
			CActiveScheduler::Stop();
			iTestStepResult = EFail;
			}
		}
	else if(iCurrentEvent == EVRRecordResumed)
		{
		TRAPD(err, FsmL(EVRRecordStopped));
		if (err != KErrNone)
			{
			iError = err;
			CActiveScheduler::Stop();
			iTestStepResult = EFail;
			}
		}
	else if(iCurrentEvent == EVRRecordStopped)
		{
    	// Stop and close iVideoRecorder
		iVideoRecorder->Stop();
    	INFO_PRINTF1(_L("iVideoRecorder->Stop()"));
		iVideoRecorder->Close();
		INFO_PRINTF1(_L("iVideoRecorder->Close()"));
		TRAPD(err, FsmL(EVRRecordComplete));
		if (err != KErrNone)
			{
			iError = err;
			CActiveScheduler::Stop();
			iTestStepResult = EFail;
			}
		}
	}

//
// RTestVclntSetGetAudSRAudChanWithAudDisable
//
//MM-MMF-VCLNTAVI-I-0314
//

/**
 * RTestVclntSetGetAudSRAudChanWithAudDisable::RTestVclntSetGetAudSRAudChanWithAudDisable()
 */
RTestVclntSetGetAudSRAudChanWithAudDisable::RTestVclntSetGetAudSRAudChanWithAudDisable(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError)
    : RTestVclntRecordAviFile(aTestName, aSectName, aKeyName, aExpectedError,EFalse)
    {
    iAudioSampleRate = 22050;
    iAudioChannel = 2;
    iNewAudioSampleRate = 0;
    iNewAudioChannel = 0;
    iTestStepResult = EFail;
    }

/**
 * RTestVclntSetValidSetGetAudSRChanAudDisable::NewL()
 */
RTestVclntSetGetAudSRAudChanWithAudDisable* RTestVclntSetGetAudSRAudChanWithAudDisable::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError)
    {
    RTestVclntSetGetAudSRAudChanWithAudDisable* self = new (ELeave) RTestVclntSetGetAudSRAudChanWithAudDisable(aTestName, aSectName, aKeyName, aExpectedError);
    return self;
    }

/**
 * RTestVclntSetValidSetGetAudSRChanAudDisable::DoTestStepL
 */
TVerdict RTestVclntSetGetAudSRAudChanWithAudDisable::DoTestStepL()
    {
    // Call the state handler from IDLE state
    FsmL(EVRIdle);

    // Start the scheduler - Done only once !
    CActiveScheduler::Start();

    return iTestStepResult;
    }

/**
 * RTestVclntSetValidSetGetAudSRChanAudDisable::FsmL
 */
void RTestVclntSetGetAudSRAudChanWithAudDisable::FsmL(TVclntTestRecordEvents aEventCode)
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
				{
				//Set Video Frame size and Video Codec
				TSize frameSize;
				frameSize.iWidth = 176;
				frameSize.iHeight = 144;
				TRAPD(err, iVideoRecorder->SetVideoFrameSizeL(frameSize));
				if (err != KErrNone)
					{
					INFO_PRINTF2(_L("SetVideoFrameSizeL() returns with error :  %d"),iError);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				INFO_PRINTF2(_L("SetVideoFrameSizeL() returned with err : %d"), err);
				_LIT8(KAviVideoCodec,"XVID");
				TRAP(err, iVideoRecorder->SetVideoTypeL(KAviVideoCodec));
				if (err != KErrNone)
					{
					INFO_PRINTF2(_L("SetVideoTypeL() returns with error :  %d"),iError);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				INFO_PRINTF2(_L("SetVideoTypeL() returned with err : %d"), err);					
				// SetAudioEnabledL
				TBool enabled = EFalse;
				TRAP(err, iVideoRecorder->SetAudioEnabledL(enabled));
				if (err != KErrNone)
					{
					INFO_PRINTF2(_L("SetAudioEnabledL(EFalse) returns with error :  %d"),err);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				INFO_PRINTF2(_L("iVideoRecorder->SetAudioEnabledL(EFalse) : Error = %d"), err);

				//Set audio sample rate to 22050Hz.
				TInt iSetError;//used for invalid iAudioSampleRate
				TRAP(iError,iVideoRecorder->SetAudioSampleRateL(iAudioSampleRate));
				iSetError = iError;
				if (iError != iTestExpectedError)
					{
					INFO_PRINTF3(_L("SetAudioSampleRateL(%d) returns with error :  %d"),iAudioSampleRate, iError);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				INFO_PRINTF3(_L("SetAudioSampleRateL(%d) returns with error :  %d"),iAudioSampleRate, iError);
				//Get the audio sample rate as 22050Hz.
				TRAP(iError,iNewAudioSampleRate=iVideoRecorder->AudioSampleRateL());
				if (iError != KErrNone)
					{
					INFO_PRINTF3(_L("AudioSampleRateL() returned iNewAudioSampleRate : %d with error :  %d"), iNewAudioSampleRate, iError);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				if(iNewAudioSampleRate == iAudioSampleRate)
					{
					INFO_PRINTF3(_L("AudioSampleRateL() returned iNewAudioSampleRate : %d with error :  %d"), iNewAudioSampleRate, iError);
					}
				else
					{
					if(iSetError != KErrNotSupported)
						{
						INFO_PRINTF3(_L("AudioSampleRateL() returned iNewAudioSampleRate : %d with error :  %d"), iNewAudioSampleRate, iError);
						iTestStepResult = EFail;
						CActiveScheduler::Stop();
						break;
						}
					else
						{
						INFO_PRINTF3(_L("AudioSampleRateL() returned iNewAudioSampleRate : %d with error :  %d"), iNewAudioSampleRate, iError);
						}
					}
				//Set Audio Channel no. to 2.
				TRAP(iError,iVideoRecorder->SetAudioChannelsL(iAudioChannel));
				iSetError = iError;// used for invalid iAudioChannel
				if (iError != iTestExpectedError)
					{
					INFO_PRINTF3(_L("SetAudioChannelsL(%d) returns with error : %d"),iAudioChannel, iError);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				INFO_PRINTF3(_L("SetAudioChannelsL(%d) returns with error : %d"),iAudioChannel, iError);
				//Get the Audio Channel no as 2.
				TRAP(iError,iNewAudioChannel=iVideoRecorder->AudioChannelsL());
				if (iError != KErrNone)
					{
					INFO_PRINTF3(_L("AudioChannelsL() returned iNewAudioChannel : %d with error : %d"),iNewAudioChannel, iError);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				if(iNewAudioChannel == iAudioChannel)
					{
					INFO_PRINTF3(_L("AudioChannelsL() returned iNewAudioChannel : %d with error : %d"),iNewAudioChannel, iError);
					}
				else
					{
					if(iSetError != KErrNotSupported)
						{
						INFO_PRINTF3(_L("AudioChannelsL() returned iNewAudioChannel : %d with error : %d"),iNewAudioChannel, iError);
						iTestStepResult = EFail;
						CActiveScheduler::Stop();
						break;
						}
					else
						{
						INFO_PRINTF3(_L("AudioChannelsL() returned iNewAudioChannel : %d with error : %d"),iNewAudioChannel, iError);
						}
					}
                // Prepare iVideoRecorder
                INFO_PRINTF1(_L("iVideoRecorder->Prepare()"));
                iVideoRecorder->Prepare();
                PrepareState(EVRPrepareComplete, KErrNone);
                break;
				}
            case EVRPrepareComplete:
                // Record iVideoRecorder
                INFO_PRINTF1(_L("iVideoRecorder->Record()"));
                iVideoRecorder->Record();
                TRAPD(err, iRecordTimer = CTestVclntAviRecordTimer::NewL(this, KFiveSeconds));
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
           		// Cancel the timer
                iRecordTimer->Cancel();
                delete iRecordTimer;
                iRecordTimer = NULL;

                CActiveScheduler::Stop();
                iTestStepResult = EPass;
                break;
            }
        }
    }

/**
* set finished on callback from a timer
*
**/
void RTestVclntSetGetAudSRAudChanWithAudDisable::FinishedL()
	{
    // Stop and close iVideoRecorder
	iVideoRecorder->Stop();
    INFO_PRINTF1(_L("iVideoRecorder->Stop()"));
	iVideoRecorder->Close();
	INFO_PRINTF1(_L("iVideoRecorder->Close()"));
	TRAPD(err, FsmL(EVRRecordComplete));
    if (err != KErrNone)
		{
		iError = err;
		CActiveScheduler::Stop();
		iTestStepResult = EFail;
		}
	}

//
//                                       NEGATIVE TESTS
//

//
//   RTestVclntGetSuppPARSetGetPARBefOpenThenOpenRec
//
//    MM-MMF-VCLNTAVI-I-0316
//

/**
 * RTestVclntGetSuppPARSetGetPARBefOpenThenOpenRec::RTestVclntGetSuppPARSetGetPARBefOpenThenOpenRec()
 */
RTestVclntGetSuppPARSetGetPARBefOpenThenOpenRec::RTestVclntGetSuppPARSetGetPARBefOpenThenOpenRec(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError)
    : RTestVclntRecordAviFile(aTestName, aSectName, aKeyName, aExpectedError,EFalse)
    {
	iAspectRatio.iNumerator = 1;
	iAspectRatio.iDenominator = 1;
	iNewAspectRatio.iNumerator = 0;
	iNewAspectRatio.iDenominator = 0;
    iTestStepResult = EFail;
    }

/**
 * RTestVclntGetSuppPARSetGetPARBefOpenThenOpenRec::NewL()
 */
RTestVclntGetSuppPARSetGetPARBefOpenThenOpenRec* RTestVclntGetSuppPARSetGetPARBefOpenThenOpenRec::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError)
    {
    RTestVclntGetSuppPARSetGetPARBefOpenThenOpenRec* self = new (ELeave) RTestVclntGetSuppPARSetGetPARBefOpenThenOpenRec(aTestName, aSectName, aKeyName, aExpectedError);
    return self;
    }

/**
 *RTestVclntGetSuppPARSetGetPARBefOpenThenOpenRec::DoTestStepL
 */
TVerdict RTestVclntGetSuppPARSetGetPARBefOpenThenOpenRec::DoTestStepL()
    {
    // Call the state handler from IDLE state
    FsmL(EVRIdle);

    // Start the scheduler - Done only once !
    CActiveScheduler::Start();

    return iTestStepResult;
    }

/* RTestVclntGetSuppPARSetGetPARBefOpenThenOpenRec::FsmL
 */
void RTestVclntGetSuppPARSetGetPARBefOpenThenOpenRec::FsmL(TVclntTestRecordEvents aEventCode)
    {
    if (FsmCheck(aEventCode))
        {
        switch (aEventCode)
            {
            case EVRIdle:
				//Set pixel aspect ratio to 1:1
				TRAP(iError,iVideoRecorder->SetPixelAspectRatioL(iAspectRatio));
				if (iError != iTestExpectedError)
					{
					INFO_PRINTF4(_L("SetPixelAspectRatioL(%d:%d) returns with error :  %d "),iAspectRatio.iNumerator, iAspectRatio.iDenominator, iError);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				INFO_PRINTF4(_L("SetPixelAspectRatioL(%d:%d) returns with error :  %d "),iAspectRatio.iNumerator, iAspectRatio.iDenominator, iError);
				//Get pixel aspect ratio as 1:1
				TRAP(iError,iVideoRecorder->GetPixelAspectRatioL(iNewAspectRatio));
				if (iError != iTestExpectedError)
					{
					INFO_PRINTF2(_L("GetPixelAspectRatioL() returns with error :  %d"),iError);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				INFO_PRINTF4(_L("GetPixelAspectRatioL() returned %d:%d with err : %d"), iNewAspectRatio.iNumerator, iNewAspectRatio.iDenominator, iError);
				//Get supported pixel aspect ratio as 1:1
				TRAP(iError,iVideoRecorder->GetSupportedPixelAspectRatiosL(iAspectRatios));
				if (iError != iTestExpectedError)
					{
					INFO_PRINTF2(_L("GetSupportedPixelAspectRatiosL() returns with error :  %d"),iError);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
                INFO_PRINTF2(_L("GetSupportedPixelAspectRatiosL() returns with error :  %d"),iError);
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
                TRAPD(err, iRecordTimer = CTestVclntAviRecordTimer::NewL(this, KFiveSeconds));
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
           		// Cancel the timer
                iRecordTimer->Cancel();
                delete iRecordTimer;
                iRecordTimer = NULL;

                CActiveScheduler::Stop();
                iTestStepResult = EPass;
                break;
            }
        }
    }

/**
* set finished on callback from a timer
*
**/
void RTestVclntGetSuppPARSetGetPARBefOpenThenOpenRec::FinishedL()
	{
    // Stop and close iVideoRecorder
    iVideoRecorder->Stop();
    INFO_PRINTF1(_L("iVideoRecorder->Stop()"));
	iVideoRecorder->Close();
	INFO_PRINTF1(_L("iVideoRecorder->Close()"));
	TRAPD(err, FsmL(EVRRecordComplete));
    if (err != KErrNone)
		{
		iError = err;
		CActiveScheduler::Stop();
		iTestStepResult = EFail;
		}
	}

//
//   RTestVclntSetGetValidPARWhileRec
//
//    MM-MMF-VCLNTAVI-I-0317
//

/**
 * RTestVclntSetGetValidPARWhileRec::RTestVclntSetGetValidPARWhileRec()
 */
RTestVclntSetGetValidPARWhileRec::RTestVclntSetGetValidPARWhileRec(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError)
    : RTestVclntRecordAviFile(aTestName, aSectName, aKeyName, aExpectedError,EFalse)
    {
	iAspectRatio.iNumerator = 1;
	iAspectRatio.iDenominator = 1;
	iNewAspectRatio.iNumerator = 0;
	iNewAspectRatio.iDenominator = 0;
    iTestStepResult = EFail;
    }

/**
 * RTestVclntSetGetValidPARWhileRec::NewL()
 */
RTestVclntSetGetValidPARWhileRec* RTestVclntSetGetValidPARWhileRec::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError)
    {
    RTestVclntSetGetValidPARWhileRec* self = new (ELeave) RTestVclntSetGetValidPARWhileRec(aTestName, aSectName, aKeyName, aExpectedError);
    return self;
    }

/**
 *RTestVclntSetGetValidPARWhileRec::DoTestStepL
 */
TVerdict RTestVclntSetGetValidPARWhileRec::DoTestStepL()
    {
    // Call the state handler from IDLE state
    FsmL(EVRIdle);

    // Start the scheduler - Done only once !
    CActiveScheduler::Start();

    return iTestStepResult;
    }

/* RTestVclntSetGetValidPARWhileRec::FsmL
 */
void RTestVclntSetGetValidPARWhileRec::FsmL(TVclntTestRecordEvents aEventCode)
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
            	{
				//Set Video Frame size and Video Codec
				TSize frameSize;
				frameSize.iWidth = 176;
				frameSize.iHeight = 144;
				TRAPD(err, iVideoRecorder->SetVideoFrameSizeL(frameSize));
				if (err != KErrNone)
					{
					INFO_PRINTF2(_L("SetVideoFrameSizeL() returns with error :  %d"),iError);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				INFO_PRINTF2(_L("SetVideoFrameSizeL() returned with err : %d"), err);
				_LIT8(KAviVideoCodec,"XVID");
				TRAP(err, iVideoRecorder->SetVideoTypeL(KAviVideoCodec));
				if (err != KErrNone)
					{
					INFO_PRINTF2(_L("SetVideoTypeL() returns with error :  %d"),iError);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				INFO_PRINTF2(_L("SetVideoTypeL() returned with err : %d"), err);					
                // Prepare iVideoRecorder
                INFO_PRINTF1(_L("iVideoRecorder->Prepare()"));
                iVideoRecorder->Prepare();
                PrepareState(EVRPrepareComplete, KErrNone);
                break;
            	}
            case EVRPrepareComplete:
                // Record iVideoRecorder
                INFO_PRINTF1(_L("iVideoRecorder->Record()"));
                iVideoRecorder->Record();
                TRAPD(err, iRecordTimer = CTestVclntAviRecordTimer::NewL(this, KFiveSeconds));
                if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CTestVclntAviRecordTimer::NewL left with error = %d"), err);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				iRecordTimer->Start();
				//Set pixel aspect ratio to 1:1
				TRAP(iError,iVideoRecorder->SetPixelAspectRatioL(iAspectRatio));
				if (iError != iTestExpectedError)
					{
					INFO_PRINTF4(_L("SetPixelAspectRatioL(%d:%d) returns with error :  %d "),iAspectRatio.iNumerator, iAspectRatio.iDenominator, iError);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				INFO_PRINTF4(_L("SetPixelAspectRatioL(%d:%d) returns with error :  %d "),iAspectRatio.iNumerator, iAspectRatio.iDenominator, iError);
				//Get pixel aspect ratio as 1:1
				TRAP(iError,iVideoRecorder->GetPixelAspectRatioL(iNewAspectRatio));
				if (iError != KErrNone)
					{
					INFO_PRINTF2(_L("GetPixelAspectRatioL() returns with error :  %d"),iError);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				if(iAspectRatio == iNewAspectRatio)
					{
					INFO_PRINTF4(_L("GetPixelAspectRatioL() returned %d:%d with err : %d"), iNewAspectRatio.iNumerator, iNewAspectRatio.iDenominator, iError);
					}
				else
					{
					INFO_PRINTF4(_L("GetPixelAspectRatioL() returned %d:%d with err : %d"), iNewAspectRatio.iNumerator, iNewAspectRatio.iDenominator, iError);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
                PrepareState(EVRRecordComplete, KErrNone);
                break;
           	case EVRRecordComplete:
           		// Cancel the timer
                iRecordTimer->Cancel();
                delete iRecordTimer;
                iRecordTimer = NULL;

                CActiveScheduler::Stop();
                iTestStepResult = EPass;
                break;
            }
        }
    }

/**
* set finished on callback from a timer
*
**/
void RTestVclntSetGetValidPARWhileRec::FinishedL()
	{
    // Stop and close iVideoRecorder
	iVideoRecorder->Stop();
    INFO_PRINTF1(_L("iVideoRecorder->Stop()"));
	iVideoRecorder->Close();
	INFO_PRINTF1(_L("iVideoRecorder->Close()"));
	TRAPD(err, FsmL(EVRRecordComplete));
    if (err != KErrNone)
		{
		iError = err;
		CActiveScheduler::Stop();
		iTestStepResult = EFail;
		}
	}

//
//   RTestVclntGetSuppChanSetGetAudChanBefOpenThenOpenRec
//
//    MM-MMF-VCLNTAVI-I-0319
//

/**
 * RTestVclntGetSuppChanSetGetAudChanBefOpenThenOpenRec::RTestVclntGetSuppChanSetGetAudChanBefOpenThenOpenRec()
 */
RTestVclntGetSuppChanSetGetAudChanBefOpenThenOpenRec::RTestVclntGetSuppChanSetGetAudChanBefOpenThenOpenRec(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError)
    : RTestVclntRecordAviFile(aTestName, aSectName, aKeyName, aExpectedError,EFalse)
    {
    iAudioChannel = 2;
    iNewAudioChannel = 0;
    iTestStepResult = EFail;
    }

/**
 * RTestVclntGetSuppChanSetGetAudChanBefOpenThenOpenRec::NewL()
 */
RTestVclntGetSuppChanSetGetAudChanBefOpenThenOpenRec* RTestVclntGetSuppChanSetGetAudChanBefOpenThenOpenRec::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError)
    {
    RTestVclntGetSuppChanSetGetAudChanBefOpenThenOpenRec* self = new (ELeave) RTestVclntGetSuppChanSetGetAudChanBefOpenThenOpenRec(aTestName, aSectName, aKeyName, aExpectedError);
    return self;
    }

/**
 *RTestVclntGetSuppChanSetGetAudChanBefOpenThenOpenRec::DoTestStepL
 */
TVerdict RTestVclntGetSuppChanSetGetAudChanBefOpenThenOpenRec::DoTestStepL()
    {
    // Call the state handler from IDLE state
    FsmL(EVRIdle);

    // Start the scheduler - Done only once !
    CActiveScheduler::Start();

    return iTestStepResult;
    }

/* RTestVclntGetSuppChanSetGetAudChanBefOpenThenOpenRec::FsmL
 */
void RTestVclntGetSuppChanSetGetAudChanBefOpenThenOpenRec::FsmL(TVclntTestRecordEvents aEventCode)
    {
    if (FsmCheck(aEventCode))
        {
        switch (aEventCode)
            {
            case EVRIdle:
				//Set Audio Channel no. to 2.
				TRAP(iError,iVideoRecorder->SetAudioChannelsL(iAudioChannel));
				if (iError != iTestExpectedError)
					{
					INFO_PRINTF3(_L("SetAudioChannelsL(%d) returns with error : %d"),iAudioChannel, iError);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				INFO_PRINTF3(_L("SetAudioChannelsL(%d) returns with error : %d"),iAudioChannel, iError);
				//Get the Audio Channel no as 2.
				TRAP(iError,iNewAudioChannel=iVideoRecorder->AudioChannelsL());
				if (iError != iTestExpectedError)
					{
					INFO_PRINTF3(_L("AudioChannelsL() returned iNewAudioChannel : %d with error : %d"),iNewAudioChannel, iError);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
					INFO_PRINTF3(_L("AudioChannelsL() returned iNewAudioChannel : %d with error : %d"),iNewAudioChannel, iError);
				//Get supported number of Audio Channels as 2.
				TRAP(iError,iVideoRecorder->GetSupportedAudioChannelsL(iAudioChannels));
				if (iError != iTestExpectedError)
					{
					INFO_PRINTF2(_L("GetSupportedAudioChannelsL() returns with error :  %d"),iError);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
					INFO_PRINTF2(_L("GetSupportedAudioChannelsL() returns with error :  %d"),iError);
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
                TRAPD(err, iRecordTimer = CTestVclntAviRecordTimer::NewL(this, KFiveSeconds));
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
           		// Cancel the timer
                iRecordTimer->Cancel();
                delete iRecordTimer;
                iRecordTimer = NULL;

                CActiveScheduler::Stop();
                iTestStepResult = EPass;
                break;
            }
        }
    }

/**
* set finished on callback from a timer
*
**/
void RTestVclntGetSuppChanSetGetAudChanBefOpenThenOpenRec::FinishedL()
	{
    // Stop and close iVideoRecorder
	iVideoRecorder->Stop();
    INFO_PRINTF1(_L("iVideoRecorder->Stop()"));
	iVideoRecorder->Close();
	INFO_PRINTF1(_L("iVideoRecorder->Close()"));
	TRAPD(err, FsmL(EVRRecordComplete));
    if (err != KErrNone)
		{
		iError = err;
		CActiveScheduler::Stop();
		iTestStepResult = EFail;
		}
	}

//
// RTestVclntSetGetValidAudChanWhileRec
//
//MM-MMF-VCLNTAVI-I-0320
//

/**
 * RTestVclntSetGetValidAudChanWhileRec::RTestVclntSetGetValidAudChanWhileRec()
 */
RTestVclntSetGetValidAudChanWhileRec::RTestVclntSetGetValidAudChanWhileRec(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError)
    : RTestVclntRecordAviFile(aTestName, aSectName, aKeyName, aExpectedError,EFalse)
    {
    iAudioChannel = 2;
    iNewAudioChannel = 0;
    iTestStepResult = EFail;
    }

/**
 * RTestVclntSetGetValidAudChanWhileRec::NewL()
 */
RTestVclntSetGetValidAudChanWhileRec* RTestVclntSetGetValidAudChanWhileRec::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError)
    {
    RTestVclntSetGetValidAudChanWhileRec* self = new (ELeave) RTestVclntSetGetValidAudChanWhileRec(aTestName, aSectName, aKeyName, aExpectedError);
    return self;
    }

/**
 * RTestVclntSetGetValidAudChanWhileRec::DoTestStepL
 */
TVerdict RTestVclntSetGetValidAudChanWhileRec::DoTestStepL()
    {
    // Call the state handler from IDLE state
    FsmL(EVRIdle);

    // Start the scheduler - Done only once !
    CActiveScheduler::Start();

    return iTestStepResult;
    }

/**
 * RTestVclntSetGetValidAudChanWhileRec::FsmL
 */
void RTestVclntSetGetValidAudChanWhileRec::FsmL(TVclntTestRecordEvents aEventCode)
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
				{
				//Set Video Frame size and Video Codec
				TSize frameSize;
				frameSize.iWidth = 176;
				frameSize.iHeight = 144;
				TRAPD(err, iVideoRecorder->SetVideoFrameSizeL(frameSize));
				if (err != KErrNone)
					{
					INFO_PRINTF2(_L("SetVideoFrameSizeL() returns with error :  %d"),iError);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				INFO_PRINTF2(_L("SetVideoFrameSizeL() returned with err : %d"), err);
				_LIT8(KAviVideoCodec,"XVID");
				TRAP(err, iVideoRecorder->SetVideoTypeL(KAviVideoCodec));
				if (err != KErrNone)
					{
					INFO_PRINTF2(_L("SetVideoTypeL() returns with error :  %d"),iError);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				INFO_PRINTF2(_L("SetVideoTypeL() returned with err : %d"), err);					
				// SetAudioEnabledL
				TBool enabled = ETrue;
				TRAP(err, iVideoRecorder->SetAudioEnabledL(enabled));
				if (err != KErrNone)
					{
					INFO_PRINTF2(_L("SetAudioEnabledL() returns with error :  %d"),err);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				INFO_PRINTF2(_L("iVideoRecorder->SetAudioEnabledL() : Error = %d"), err);

                // Prepare iVideoRecorder
                INFO_PRINTF1(_L("iVideoRecorder->Prepare()"));
                iVideoRecorder->Prepare();
                PrepareState(EVRPrepareComplete, KErrNone);
                break;
				}
            case EVRPrepareComplete:
                // Record iVideoRecorder
                INFO_PRINTF1(_L("iVideoRecorder->Record()"));
                iVideoRecorder->Record();
                TRAPD(err, iRecordTimer = CTestVclntAviRecordTimer::NewL(this, KFiveSeconds));
                if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CTestVclntAviRecordTimer::NewL left with error = %d"), err);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				iRecordTimer->Start();

				//Set Audio Channel no. to 2.
				TRAP(iError,iVideoRecorder->SetAudioChannelsL(iAudioChannel));
				if (iError != iTestExpectedError)
					{
					INFO_PRINTF3(_L("SetAudioChannelsL(%d) returns with error : %d"),iAudioChannel, iError);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				INFO_PRINTF3(_L("SetAudioChannelsL(%d) returns with error : %d"),iAudioChannel, iError);
				//Get the Audio Channel no as 2.
				TRAP(iError,iNewAudioChannel=iVideoRecorder->AudioChannelsL());
				if (iError != KErrNone)
					{
					INFO_PRINTF3(_L("AudioChannelsL() returned iNewAudioChannel : %d with error : %d"),iNewAudioChannel, iError);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				if(iNewAudioChannel == iAudioChannel)
					{
					INFO_PRINTF3(_L("AudioChannelsL() returned iNewAudioChannel : %d with error : %d"),iNewAudioChannel, iError);
					}
				else
					{
					INFO_PRINTF3(_L("AudioChannelsL() returned iNewAudioChannel : %d with error : %d"),iNewAudioChannel, iError);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
                PrepareState(EVRRecordComplete, KErrNone);
                break;
           	case EVRRecordComplete:
           	    // Cancel the timer
                iRecordTimer->Cancel();
                delete iRecordTimer;
                iRecordTimer = NULL;
                
                CActiveScheduler::Stop();
                iTestStepResult = EPass;
                break;
            }
        }
    }

/**
* set finished on callback from a timer
*
**/
void RTestVclntSetGetValidAudChanWhileRec::FinishedL()
	{
    // Stop and close iVideoRecorder
	iVideoRecorder->Stop();
    INFO_PRINTF1(_L("iVideoRecorder->Stop()"));
	iVideoRecorder->Close();
	INFO_PRINTF1(_L("iVideoRecorder->Close()"));
	TRAPD(err, FsmL(EVRRecordComplete));
    if (err != KErrNone)
		{
		iError = err;
		CActiveScheduler::Stop();
		iTestStepResult = EFail;
		}
	}

//
// RTestVclntGetSuppSRSetGetSRBefOpenThenOpenRec
//
//MM-MMF-VCLNTAVI-I-0322
//

/**
 * RTestVclntGetSuppSRSetGetSRBefOpenThenOpenRec::RTestVclntGetSuppSRSetGetSRBefOpenThenOpenRec()
 */
RTestVclntGetSuppSRSetGetSRBefOpenThenOpenRec::RTestVclntGetSuppSRSetGetSRBefOpenThenOpenRec(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError)
    : RTestVclntRecordAviFile(aTestName, aSectName, aKeyName, aExpectedError,EFalse)
    {
    iAudioSampleRate = 22050;
    iNewAudioSampleRate = 0;
    iTestStepResult = EFail;
    }

/**
 * RTestVclntGetSuppSRSetGetSRBefOpenThenOpenRec::NewL()
 */
RTestVclntGetSuppSRSetGetSRBefOpenThenOpenRec* RTestVclntGetSuppSRSetGetSRBefOpenThenOpenRec::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError)
    {
    RTestVclntGetSuppSRSetGetSRBefOpenThenOpenRec* self = new (ELeave) RTestVclntGetSuppSRSetGetSRBefOpenThenOpenRec(aTestName, aSectName, aKeyName, aExpectedError);
    return self;
    }

/**
 * RTestVclntGetSuppSRSetGetSRBefOpenThenOpenRec::DoTestStepL
 */
TVerdict RTestVclntGetSuppSRSetGetSRBefOpenThenOpenRec::DoTestStepL()
    {
    // Call the state handler from IDLE state
    FsmL(EVRIdle);

    // Start the scheduler - Done only once !
    CActiveScheduler::Start();

    return iTestStepResult;
    }

/**
 * RTestVclntGetSuppSRSetGetSRBefOpenThenOpenRec::FsmL
 */
void RTestVclntGetSuppSRSetGetSRBefOpenThenOpenRec::FsmL(TVclntTestRecordEvents aEventCode)
    {
    if (FsmCheck(aEventCode))
        {
        switch (aEventCode)
            {
            case EVRIdle:
				//Set audio sample rate to 22050Hz.
				TRAP(iError,iVideoRecorder->SetAudioSampleRateL(iAudioSampleRate));
				if (iError != iTestExpectedError)
					{
					INFO_PRINTF3(_L("SetAudioSampleRateL(%d) returns with error :  %d"),iAudioSampleRate, iError);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				INFO_PRINTF3(_L("SetAudioSampleRateL(%d) returns with error :  %d"),iAudioSampleRate, iError);
				//Get the audio sample rate as 22050Hz.
				TRAP(iError,iNewAudioSampleRate=iVideoRecorder->AudioSampleRateL());
				if (iError != iTestExpectedError)
					{
					INFO_PRINTF3(_L("AudioSampleRateL() returned iNewAudioSampleRate : %d with error :  %d"), iNewAudioSampleRate, iError);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
					INFO_PRINTF3(_L("AudioSampleRateL() returned iNewAudioSampleRate : %d with error :  %d"), iNewAudioSampleRate, iError);
				//Get the supported Audio Sample Rate.
				TRAP(iError,iVideoRecorder->GetSupportedAudioSampleRatesL(iAudioSampleRates));
				if (iError != iTestExpectedError)
					{
					INFO_PRINTF2(_L("GetSupportedAudioSampleRatesL() returns with error :  %d"),iError);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				INFO_PRINTF2(_L("GetSupportedAudioSampleRatesL() returns with error :  %d"),iError);
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
                TRAPD(err, iRecordTimer = CTestVclntAviRecordTimer::NewL(this, KFiveSeconds));
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
           		// Cancel the timer
                iRecordTimer->Cancel();
                delete iRecordTimer;
                iRecordTimer = NULL;
                
                CActiveScheduler::Stop();
                iTestStepResult = EPass;
                break;
            }
        }
    }

/**
* set finished on callback from a timer
*
**/
void RTestVclntGetSuppSRSetGetSRBefOpenThenOpenRec::FinishedL()
	{
    // Stop and close iVideoRecorder
	iVideoRecorder->Stop();
    INFO_PRINTF1(_L("iVideoRecorder->Stop()"));
	iVideoRecorder->Close();
	INFO_PRINTF1(_L("iVideoRecorder->Close()"));
	TRAPD(err, FsmL(EVRRecordComplete));
    if (err != KErrNone)
		{
		iError = err;
		CActiveScheduler::Stop();
		iTestStepResult = EFail;
		}
	}

//
// RTestVclntSetGetValidSRWhileRec
//
//MM-MMF-VCLNTAVI-I-0323
//

/**
 * RTestVclntSetGetValidSRWhileRec::RTestVclntSetGetValidSRWhileRec()
 */
RTestVclntSetGetValidSRWhileRec::RTestVclntSetGetValidSRWhileRec(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError)
    : RTestVclntRecordAviFile(aTestName, aSectName, aKeyName, aExpectedError,EFalse)
    {
    iAudioSampleRate = 22050;
    iNewAudioSampleRate = 0;
    iTestStepResult = EFail;
    }

/**
 * RTestVclntSetGetValidSRWhileRec::NewL()
 */
RTestVclntSetGetValidSRWhileRec* RTestVclntSetGetValidSRWhileRec::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError)
    {
    RTestVclntSetGetValidSRWhileRec* self = new (ELeave) RTestVclntSetGetValidSRWhileRec(aTestName, aSectName, aKeyName, aExpectedError);
    return self;
    }

/**
 * RTestVclntSetGetValidSRWhileRec::DoTestStepL
 */
TVerdict RTestVclntSetGetValidSRWhileRec::DoTestStepL()
    {
    // Call the state handler from IDLE state
    FsmL(EVRIdle);

    // Start the scheduler - Done only once !
    CActiveScheduler::Start();

    return iTestStepResult;
    }

/**
 * RTestVclntSetGetValidSRWhileRec::FsmL
 */
void RTestVclntSetGetValidSRWhileRec::FsmL(TVclntTestRecordEvents aEventCode)
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
				{
				//Set Video Frame size and Video Codec
				TSize frameSize;
				frameSize.iWidth = 176;
				frameSize.iHeight = 144;
				TRAPD(err, iVideoRecorder->SetVideoFrameSizeL(frameSize));
				if (err != KErrNone)
					{
					INFO_PRINTF2(_L("SetVideoFrameSizeL() returns with error :  %d"),iError);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				INFO_PRINTF2(_L("SetVideoFrameSizeL() returned with err : %d"), err);
				_LIT8(KAviVideoCodec,"XVID");
				TRAP(err, iVideoRecorder->SetVideoTypeL(KAviVideoCodec));
				if (err != KErrNone)
					{
					INFO_PRINTF2(_L("SetVideoTypeL() returns with error :  %d"),iError);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				INFO_PRINTF2(_L("SetVideoTypeL() returned with err : %d"), err);
				// SetAudioEnabledL
				TBool enabled = ETrue;
				TRAP(err, iVideoRecorder->SetAudioEnabledL(enabled));
				if (err != KErrNone)
					{
					INFO_PRINTF2(_L("SetAudioEnabledL() returns with error :  %d"),err);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				INFO_PRINTF2(_L("iVideoRecorder->SetAudioEnabledL() : Error = %d"), err);

                // Prepare iVideoRecorder
                INFO_PRINTF1(_L("iVideoRecorder->Prepare()"));
                iVideoRecorder->Prepare();
                PrepareState(EVRPrepareComplete, KErrNone);
                break;
				}
            case EVRPrepareComplete:
                // Record iVideoRecorder
                INFO_PRINTF1(_L("iVideoRecorder->Record()"));
                iVideoRecorder->Record();
                TRAPD(err, iRecordTimer = CTestVclntAviRecordTimer::NewL(this, KFiveSeconds));
                if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CTestVclntAviRecordTimer::NewL left with error = %d"), err);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				iRecordTimer->Start();

				//Set audio sample rate to 22050Hz.
				TInt iSetError; //used for invalid iAudioSampleRate
				TRAP(iError,iVideoRecorder->SetAudioSampleRateL(iAudioSampleRate));
				iSetError = iError;
				if (iError != iTestExpectedError)
					{
					INFO_PRINTF3(_L("SetAudioSampleRateL(%d) returns with error :  %d"),iAudioSampleRate, iError);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				INFO_PRINTF3(_L("SetAudioSampleRateL(%d) returns with error :  %d"),iAudioSampleRate, iError);
				//Get the audio sample rate as 22050Hz.
				TRAP(iError,iNewAudioSampleRate=iVideoRecorder->AudioSampleRateL());
				if (iError != KErrNone)
					{
					INFO_PRINTF3(_L("AudioSampleRateL() returned iNewAudioSampleRate : %d with error :  %d"), iNewAudioSampleRate, iError);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				if(iNewAudioSampleRate == iAudioSampleRate)
					{
					INFO_PRINTF3(_L("AudioSampleRateL() returned iNewAudioSampleRate : %d with error :  %d"), iNewAudioSampleRate, iError);
					}
				else
					{
					if(iSetError != KErrNotReady)
						{
						INFO_PRINTF3(_L("AudioSampleRateL() returned iNewAudioSampleRate : %d with error :  %d"), iNewAudioSampleRate, iError);
						iTestStepResult = EFail;
						CActiveScheduler::Stop();
						break;
						}
					else
						{
						INFO_PRINTF3(_L("AudioSampleRateL() returned iNewAudioSampleRate : %d with error :  %d"), iNewAudioSampleRate, iError);
						}
					}
                PrepareState(EVRRecordComplete, KErrNone);
                break;
           	case EVRRecordComplete:
           		// Cancel the timer
                iRecordTimer->Cancel();
                delete iRecordTimer;
                iRecordTimer = NULL;

                CActiveScheduler::Stop();
                iTestStepResult = EPass;
                break;
            }
        }
    }

/**
* set finished on callback from a timer
*
**/
void RTestVclntSetGetValidSRWhileRec::FinishedL()
	{
    // Stop and close iVideoRecorder
	iVideoRecorder->Stop();
    INFO_PRINTF1(_L("iVideoRecorder->Stop()"));
	iVideoRecorder->Close();
	INFO_PRINTF1(_L("iVideoRecorder->Close()"));
	TRAPD(err, FsmL(EVRRecordComplete));
    if (err != KErrNone)
		{
		iError = err;
		CActiveScheduler::Stop();
		iTestStepResult = EFail;
		}
	}

