// Copyright (c) 2005-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "testvideoplayer.h"

#include <e32math.h>
#include <mmf/common/mmfvideo.h>
#include "u32hal.h"

#ifdef SYMBIAN_BUILD_GCE
#include <gceavailable.h>
#endif

const TInt KFrameWidth = 100;
const TInt KFrameHeight = 50;

const TInt KCropRegionRectTop = 1;
const TInt KCropRegionRectLeft = 3;
const TInt KCropRegionRectRight = 5;
const TInt KCropRegionRectBottom = 7;
const TUint KCacheSize = 1000000; //1MB
const TUint KMaxCacheSize = 30000000; //30MB

//
// RTestVclntPlayAviFile
//

/**
 * RTestVclntPlayAviFile::Constructor
 */
RTestVclntPlayAviFile::RTestVclntPlayAviFile(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError, const TBool aPlay)
    : RTestVclntAviPlayerStep(aTestName, aSectName, aKeyName, aExpectedError), iPlay(aPlay)
    {
    iHeapSize = 2000000; //-2MB
    }

/**
 * RTestVclntPlayAviFile::NewL
 */
RTestVclntPlayAviFile* RTestVclntPlayAviFile::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError,const TBool aPlay)
    {
    RTestVclntPlayAviFile* self = new (ELeave) RTestVclntPlayAviFile(aTestName, aSectName, aKeyName, aExpectedError, aPlay);
    return self;
    }

/**
 * RTestVclntPlayAviFile::NewLC
 */
RTestVclntPlayAviFile* RTestVclntPlayAviFile::NewLC(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError,const TBool aPlay)
    {
    RTestVclntPlayAviFile* self = new (ELeave) RTestVclntPlayAviFile(aTestName, aSectName, aKeyName, aExpectedError, aPlay);
    CleanupStack::PushL(self);
    return self;
    }

/**
 * RTestVclntPlayAviFile::DoTestStepL
 */
TVerdict RTestVclntPlayAviFile::DoTestStepL()
    {
	 // WDP:We are going to start the test now
	 // Ensure we set paging memory to appropriate cache size for tests which need it
	 //ignore other tests
    TVerdict verdict=SetCacheSize();
    if(verdict!=EPass)
    	{
    	return verdict;
    	}
     
    // Call the state handler from IDLE state
    FsmL(EVPIdle);
    User::LeaveIfError(iError);
    // Start the scheduler - Done only once !
    CActiveScheduler::Start();
    
    return iTestStepResult;
    }

//Default SetCache size
TVerdict RTestVclntPlayAviFile::SetCacheSize()
	{
	//Do not try to increase cache size for tests which dont need it
	return EPass;
		    
	}

/**
 * RTestVclntPlayAviFile::FsmL
 */
void RTestVclntPlayAviFile::FsmL(TVclntTestPlayEvents aEventCode)
    {
    if (FsmCheck(aEventCode))
        {
        //TInt err = KErrNone;
        switch (aEventCode)
            {
            case EVPIdle:
                // Open iVideoPlayer
                INFO_PRINTF2(_L("iVideoPlayer->OpenFileL() %S"), &iFilename);
                TRAP(iError, iVideoPlayer->OpenFileL(iFilename, ControllerUid()));
                PrepareState(EVPOpenComplete, KErrNone);
                break;
            case EVPOpenComplete:
                // Prepare iVideoPlayer
                INFO_PRINTF1(_L("iVideoPlayer->Prepare()"));
                iVideoPlayer->Prepare();
                PrepareState(EVPPrepareComplete, KErrNone);
                break;
            case EVPPrepareComplete:
                iTestStepResult = DoTestL(iVideoPlayer);
                break;
            case EVPPlayComplete:
                iTestStepResult = EPass;
                CActiveScheduler::Stop();
                break;
            }
        }
    }

/**
 * RTestVclntPlayAviFile::DoTestL
 */
TVerdict RTestVclntPlayAviFile::DoTestL(CVideoPlayerUtility* /*aPlayer*/)
    {
    TVerdict ret = EFail;

    if(iPlay)
        {
        iError = KErrTimedOut;
        INFO_PRINTF1(_L("iVideoPlayer->Play()"));
        PrepareState(EVPPlayComplete, KErrNone);
        iVideoPlayer->Play();
        }
    else
        {
        CActiveScheduler::Stop();
        ret = EPass;
        }
        
    return ret;
    }

//
// RTestVclntPlayAviDes
//

/**
 * RTestVclntPlayAviDes::Constructor
 */
RTestVclntPlayAviDes::RTestVclntPlayAviDes(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError, const TBool aPlay)
    : RTestVclntAviPlayerStep(aTestName, aSectName, aKeyName, aExpectedError), iPlay(aPlay)
    {
    iHeapSize = 500000;
    }

/**
 * RTestVclntPlayAviDes::NewL
 */
RTestVclntPlayAviDes* RTestVclntPlayAviDes::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError,const TBool aPlay)
    {
    RTestVclntPlayAviDes* self = new (ELeave) RTestVclntPlayAviDes(aTestName, aSectName, aKeyName, aExpectedError, aPlay);
    return self;
    }

/**
 * RTestVclntPlayAviDes::NewLC
 */
RTestVclntPlayAviDes* RTestVclntPlayAviDes::NewLC(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError,const TBool aPlay)
    {
    RTestVclntPlayAviDes* self = new (ELeave) RTestVclntPlayAviDes(aTestName, aSectName, aKeyName, aExpectedError, aPlay);
    CleanupStack::PushL(self);
    return self;
    }

/**
 * RTestVclntPlayAviDes::DoTestStepPreambleL
 */
TVerdict  RTestVclntPlayAviDes::DoTestStepPreambleL()
    {
    TVerdict preamRes;
    preamRes = EPass;
    
    // Base DoTestStepPreambleL
    preamRes = RTestVclntAviPlayerStep::DoTestStepPreambleL();
    if (preamRes != EPass)
        {
        return preamRes;
        }
    
    RFs fs;
    RFile file;
    TInt size = 0;

    // connect to file system and open file
    User::LeaveIfError(fs.Connect());
    CleanupClosePushL(fs);
    User::LeaveIfError(file.Open(fs,iFilename,EFileRead));
    CleanupClosePushL(file);

    // Set HBuf size
    User::LeaveIfError(file.Size(size));
    INFO_PRINTF2(_L("size of file = %d\n"),size);//Statement Changed under DEF105143

	iVideo = HBufC8::NewMaxL(size);

    // read data into Hbuf
    TPtr8 bufferDes(iVideo->Des());
    User::LeaveIfError(file.Read(bufferDes));
    
    CleanupStack::PopAndDestroy(2); //fs, file
    
    return preamRes;
    }

/**
 * RTestVclntPlayAviDes::DoTestStepPostambleL
 */
TVerdict RTestVclntPlayAviDes::DoTestStepPostambleL()
    {
    delete iVideo;
    iVideo = NULL;
    return RTestVclntAviPlayerStep::DoTestStepPostambleL();
    }

/**
 * Load and initialise an video descriptor.
 */
TVerdict RTestVclntPlayAviDes::DoTestStepL()
    {
    iTestStepResult = EFail;
    // Call the state handler from IDLE state
    TRAPD(err, FsmL(EVPIdle));
    if (err == KErrNone)
        {
        CActiveScheduler::Start();    
        }
    // Start the scheduler - Done only once !
    return iTestStepResult;
    }

/**
 * RTestVclntPlayAviDes::FsmL
 */
void RTestVclntPlayAviDes::FsmL(TVclntTestPlayEvents aEventCode)
    {
    if (FsmCheck(aEventCode))
        {
        //TInt err = KErrNone;
        switch (aEventCode)
            {
            case EVPIdle:
                // Open iVideoPlayer
                INFO_PRINTF2(_L("iVideoPlayer->OpenDesL() %S"), &iFilename);
                TRAP(iError, iVideoPlayer->OpenDesL(iVideo->Des(), ControllerUid()));
                PrepareState(EVPOpenComplete, KErrNone);
                break;
            case EVPOpenComplete:
                // Prepare iVideoPlayer
                INFO_PRINTF1(_L("iVideoPlayer->Prepare()"));
                iVideoPlayer->Prepare();
                PrepareState(EVPPrepareComplete, KErrNone);
                break;
            case EVPPrepareComplete:
                iTestStepResult = DoTestL(iVideoPlayer);
                break;
            case EVPPlayComplete:
                iTestStepResult = EPass;
                CActiveScheduler::Stop();
                break;
            }
        }
    }

/**
 * RTestVclntPlayAviDes::DoTestL
 */
TVerdict RTestVclntPlayAviDes::DoTestL(CVideoPlayerUtility* aPlayer)
    {
    TVerdict ret = EFail;

    if(iPlay)
        {
        iError = KErrTimedOut;
        INFO_PRINTF1(_L("iVideoPlayer->Play()"));
        PrepareState(EVPPlayComplete, KErrNone);
        aPlayer->Play();
        }
    else
        {
        CActiveScheduler::Stop();
        ret = EPass;
        }
        
    return ret;
    }


//
// RTestVclntPlayAviUrl
//

/**
 * RTestVclntPlayAviUrl::Constructor
 */
 
RTestVclntPlayAviUrl::RTestVclntPlayAviUrl(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError,const TBool aPlay)
    : RTestVclntAviPlayerStep(aTestName, aSectName, aKeyName, aExpectedError), iPlay(aPlay)
    {
    }

/**
 * RTestVclntPlayAviUrl::NewL
 */

RTestVclntPlayAviUrl* RTestVclntPlayAviUrl::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError,const TBool aPlay)
    {
    RTestVclntPlayAviUrl* self = RTestVclntPlayAviUrl::NewLC(aTestName, aSectName, aKeyName, aExpectedError, aPlay);
    CleanupStack::Pop();
    return self;
    }

/**
 * RTestVclntPlayAviUrl::NewLC
 */

RTestVclntPlayAviUrl* RTestVclntPlayAviUrl::NewLC(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError,const TBool aPlay)
    {
    RTestVclntPlayAviUrl* self = new (ELeave) RTestVclntPlayAviUrl(aTestName, aSectName, aKeyName, aExpectedError, aPlay);
    CleanupStack::PushL(self);
    return self;
    }

/**
 * RTestVclntPlayAviUrl::Constructor
 */

TVerdict RTestVclntPlayAviUrl::DoTestStepL()
    {
    // Call the state handler from IDLE state
    FsmL(EVPIdle);
    
    // Start the scheduler - Done only once !
    CActiveScheduler::Start();
    
    return iTestStepResult;
    }

/**
 * RTestVclntPlayAviUrl::FsmL
 */
 
void RTestVclntPlayAviUrl::FsmL(TVclntTestPlayEvents aEventCode)
    {
    if (FsmCheck(aEventCode))
        {
        switch (aEventCode)
            {
            case EVPIdle:
                // Open iVideoPlayer
                
                INFO_PRINTF2(_L("iVideoPlayer->OpenUrlL() %S"), &iFilename);
                TRAP(iError,iVideoPlayer->OpenUrlL(iFilename, KUseDefaultIap, KNullDesC8, ControllerUid()));
                PrepareState(EVPOpenComplete, KErrNone);
                break;
            case EVPOpenComplete:
                // Prepare iVideoPlayer
                INFO_PRINTF1(_L("iVideoPlayer->Prepare()"));
                iVideoPlayer->Prepare();
                PrepareState(EVPPrepareComplete, KErrNone);
                break;
            case EVPPrepareComplete:
                iTestStepResult = DoTestL(iVideoPlayer);
                break;
            case EVPPlayComplete:
                CActiveScheduler::Stop();
                iTestStepResult = EPass;
                break;
            }
        }
    }

/**
 * RTestVclntPlayAviUrl::Constructor
 */
 
TVerdict RTestVclntPlayAviUrl::DoTestL(CVideoPlayerUtility* aPlayer)
    {
    TVerdict ret = EFail;

    if(iPlay)
        {
        iError = KErrTimedOut;
        INFO_PRINTF1(_L("iVideoPlayer->Play()"));
        PrepareState(EVPPlayComplete, KErrNone);
        aPlayer->Play();
        }
    else
        {
        CActiveScheduler::Stop();
        ret = EPass;
        }
        
    return ret;
    }


//
// RTestVclntEnqFrameRate
//

/**
 * RTestVclntEnqFrameRate::Constructor
 */
RTestVclntEnqFrameRate::RTestVclntEnqFrameRate(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError, const TInt aFrameRate)
    : RTestVclntPlayAviFile(aTestName, aSectName, aKeyName, aExpectedError, EFalse)
    {
    iFrameRate = aFrameRate;
    }

/**
 * RTestVclntEnqFrameRate::Constructor
 */
RTestVclntEnqFrameRate* RTestVclntEnqFrameRate::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError, const TInt aFrameRate)
    {
    RTestVclntEnqFrameRate* self = new (ELeave) RTestVclntEnqFrameRate(aTestName, aSectName, aKeyName, aExpectedError, aFrameRate);
    return self;
    }

/**
 * RTestVclntEnqFrameRate::Constructor
 */
TVerdict RTestVclntEnqFrameRate::DoTestL(CVideoPlayerUtility* aPlayer)
    {
    INFO_PRINTF1(_L("Test : Video Player - Enquire Frame Rate"));

    TVerdict ret = EFail;

    TReal32 theFrameRate = 0.0;
    TRAPD(err, theFrameRate = aPlayer->VideoFrameRateL());
    INFO_PRINTF3(_L("iVideoPlayer->VideoFrameRateL() = %d...error =%d"), TInt(theFrameRate), err);

    // if we get here, we pass. The iVideoPlayer does not have a SetVideoFrameRate()
    INFO_PRINTF1(_L("iVideoPlayer->Play()"));
    PrepareState(EVPPlayComplete, KErrNone);
    aPlayer->Play();

    return ret;
    }


//
// RTestVclntPosition
//

/**
 * RTestVclntPosition::Constructor
 */
RTestVclntPosition::RTestVclntPosition(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError, const TTimeIntervalMicroSeconds aPosition)
    : RTestVclntPlayAviFile(aTestName, aSectName, aKeyName, aExpectedError, EFalse)
    {
    iPosition = aPosition;
    }

/**
 * RTestVclntPosition::NewL
 */
RTestVclntPosition* RTestVclntPosition::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError, const TTimeIntervalMicroSeconds aPosition)
    {
    RTestVclntPosition* self = new (ELeave) RTestVclntPosition(aTestName, aSectName, aKeyName, aExpectedError, aPosition);
    return self;
    }

/**
 * RTestVclntPosition::DoTestL
 */
TVerdict RTestVclntPosition::DoTestL(CVideoPlayerUtility* aPlayer)
    {
    // NB test video controller is stubbing certain functions. May have to rewrite this
    // for AVI Controller

    TVerdict ret = EFail;
    TInt err = KErrNone;
    TTimeIntervalMicroSeconds pos;
    
    INFO_PRINTF1(_L("Test : Video Player - Position"));
    INFO_PRINTF3(_L("Set Position = %d, Duration = %d"), I64INT(iPosition.Int64()), I64INT(aPlayer->DurationL().Int64()));

    //  Set position: middle of clip.
    if (I64INT(iPosition.Int64()) == 0)
        {
        iPosition = I64INT(aPlayer->DurationL().Int64()) / 2;
        }
        
    // Set position: end of clip.
    if (I64INT(iPosition.Int64()) == -1)
        {
        iPosition = aPlayer->DurationL();
        }
        
    // Position is beyond the end of the clips duration, so check that the value is clipped.
    if(aPlayer->DurationL() < iPosition)
        {
        INFO_PRINTF2(_L("Longer than duration : Setting position to %d"), I64INT(iPosition.Int64()));
        TRAP(err, aPlayer->SetPositionL(iPosition));
        if (err == KErrNotSupported)
            {
            INFO_PRINTF1(_L("SetPositionL() left with Error - KErrNotSupported"));
            }
        TRAP(err, pos = aPlayer->PositionL());
        if (err == KErrNotReady)
            {
            INFO_PRINTF1(_L("PositionL() left with Error - KErrNotReady"));
            }
        INFO_PRINTF3(_L("Set Position = %d (if clipped : %d)"), I64INT(pos.Int64()), I64INT(aPlayer->DurationL().Int64()));
        }
    
    // Position is negative, so check that the value is clipped.
    else if (I64INT(iPosition.Int64()) < 0)
        {
        INFO_PRINTF2(_L("Negative value : Setting position to %d"), I64INT(iPosition.Int64()));
        TRAP(err, aPlayer->SetPositionL(iPosition));
        if (err == KErrNotSupported)
            {
            INFO_PRINTF1(_L("SetPositionL() left with Error - KErrNotSupported"));
            }
        TRAP(err, pos = aPlayer->PositionL());
        if (err == KErrNotReady)
            {
            INFO_PRINTF1(_L("PositionL() left with Error - KErrNotReady"));
            }
        INFO_PRINTF2(_L("Set : position = %d (if clipped : 0)"), I64INT(pos.Int64()));
        }
    else
        {
        INFO_PRINTF2(_L("Normal : Setting position to %d"), I64INT(iPosition.Int64()));
        TRAP(err, aPlayer->SetPositionL(iPosition));
        if (err == KErrNotSupported)
            {
            INFO_PRINTF1(_L("SetPositionL() left with Error - KErrNotSupported"));
            }
        TRAP(err, pos = aPlayer->PositionL());
        if (err == KErrNotReady)
            {
            INFO_PRINTF1(_L("PositionL() left with Error - KErrNotReady"));
            }
        INFO_PRINTF3(_L("Set : position = %d (if clipped : %d)"), I64INT(pos.Int64()), I64INT(iPosition.Int64()));
        }

    INFO_PRINTF1(_L("iVideoPlayer->Play()"));
    PrepareState(EVPPlayComplete, KErrNone);
    aPlayer->Play();
    
    
    TRAP(err, pos = aPlayer->PositionL());
    if (err != KErrNone)
        {
        INFO_PRINTF2(_L("PositionL() left with Error - %d"), err);
        ret = EFail;
        }
    
    return ret;
    }


//
// RTestVclntPriority
//

/**
 * RTestVclntPriority::Constructor
 */
RTestVclntPriority::RTestVclntPriority(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError, const TInt aPriority)
    : RTestVclntPlayAviFile(aTestName, aSectName, aKeyName, aExpectedError, EFalse)
    {
    iPriority = aPriority;
    }

/**
 * RTestVclntRecordAviFile::Constructor
 */
RTestVclntPriority* RTestVclntPriority::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError, const TInt aPriority)
    {
    RTestVclntPriority* self = new (ELeave) RTestVclntPriority(aTestName, aSectName, aKeyName, aExpectedError, aPriority);
    return self;
    }

/**
 * RTestVclntRecordAviFile::Constructor
 */
TVerdict RTestVclntPriority::DoTestL(CVideoPlayerUtility* aPlayer)
    {
    TVerdict ret = EFail;

    INFO_PRINTF1(_L("Test : Video Player - Priority"));

    aPlayer->SetPriorityL(iPriority, EMdaPriorityPreferenceNone);    
    TInt thePriority = 0;
    TMdaPriorityPreference thePref;
    aPlayer->PriorityL(thePriority, thePref);
    INFO_PRINTF3(_L("Priority = %d (expecting %d)"), thePriority, iPriority);
    INFO_PRINTF3(_L("Pref = %d (expecting %d)"), thePref, EMdaPriorityPreferenceNone);
    
    if( (thePriority == iPriority) && (thePref == EMdaPriorityPreferenceNone) )
        {
        ret = EPass;
        }
        
    CActiveScheduler::Stop();
    
    return ret;
    }


//
// RTestVclntDuration
//

/**
 * RTestVclntDuration::Constructor
 */
RTestVclntDuration::RTestVclntDuration(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError, const TTimeIntervalMicroSeconds aDuration)
    : RTestVclntPlayAviFile(aTestName, aSectName, aKeyName, aExpectedError, EFalse)
    {
    iDuration = aDuration;
    }

/**
 * RTestVclntDuration::Constructor
 */
RTestVclntDuration* RTestVclntDuration::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError, const TTimeIntervalMicroSeconds aDuration)
    {
    RTestVclntDuration* self = new (ELeave) RTestVclntDuration(aTestName, aSectName, aKeyName, aExpectedError, aDuration);
    return self;
    }

/**
 * RTestVclntDuration::Constructor
 */
TVerdict RTestVclntDuration::DoTestL(CVideoPlayerUtility* aPlayer)
    {
    TVerdict ret = EFail;
    
    INFO_PRINTF1(_L("Test : Video Player - Duration"));

    if (I64INT(iDuration.Int64()) == 0)
        {
        TInt duration = I64INT(aPlayer->DurationL().Int64());
        INFO_PRINTF1(_L("iVideoPlayer->Play()"));
        PrepareState(EVPPlayComplete, KErrNone);
        aPlayer->Play();
        TTime start;
        start.HomeTime();
        CActiveScheduler::Start();
        TTime stop;
        stop.HomeTime();

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
        if(aPlayer->DurationL() == iDuration)
            {
            ret = EPass;
            }
        }
    
    CActiveScheduler::Stop();
    TInt error=UserSvr::HalFunction(EHalGroupVM,EVMHalSetCacheSize,(TAny*)iCurrentCacheSize.iMinSize,(TAny*)iCurrentCacheSize.iMaxSize);
    if(CheckCacheError(error)==EInconclusive)
    	{
    	return EInconclusive;
    	}
    INFO_PRINTF3(_L("Setting Cache Min Size to %d,Setting Cache Max Size is %d"),iCurrentCacheSize.iMinSize,iCurrentCacheSize.iMaxSize);
    return ret;
    }

TVerdict RTestVclntDuration::SetCacheSize()
	{
	TVerdict ret=EPass;
	//save the default cache sizes, set the cache size back these values after the test
	TInt error=UserSvr::HalFunction(EHalGroupVM,EVMHalGetCacheSize,&iCurrentCacheSize,0);
	ret=CheckCacheError(error);
	if(ret==EPass)
		{
		error=UserSvr::HalFunction(EHalGroupVM,EVMHalSetCacheSize,(TAny*)KCacheSize,(TAny*)KMaxCacheSize);
		ret=CheckCacheError(error);
		}
	return ret;
	}

TVerdict RTestVclntDuration::CheckCacheError(TInt aError)
	{
	TVerdict verdict=EPass;
	#ifdef __WINSCW__
	//Winscw does not support getting cache size. Ignoring -5 error
	if(aError!=KErrNone)
		{
		if(aError!=KErrNotSupported)
			{
			INFO_PRINTF2(_L("Could not get the cache size  %d"),aError);
			return EInconclusive;
			}
		}
	#else
	if(aError!=KErrNone)
		{//For ARMV5 we stop for all errors
		INFO_PRINTF2(_L("Could not get the cache size  %d"),aError);
		return EInconclusive;
		}
	#endif
	return verdict;
	}

//
// RTestVclntVolume
//

/**
 * RTestVclntVolume::Constructor
 */
RTestVclntVolume::RTestVclntVolume(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError, const TInt aVolume)
    : RTestVclntPlayAviFile(aTestName, aSectName, aKeyName, aExpectedError, EFalse)
    {
    iVolume = aVolume;
    }

/**
 * RTestVclntVolume::Constructor
 */
RTestVclntVolume* RTestVclntVolume::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError, const TInt aVolume)
    {
    RTestVclntVolume* self = new (ELeave) RTestVclntVolume(aTestName, aSectName, aKeyName, aExpectedError, aVolume);
    return self;
    }

/**
 * RTestVclntVolume::Constructor
 */
TVerdict RTestVclntVolume::DoTestL(CVideoPlayerUtility* aPlayer)
    {
    TVerdict ret = EFail;
    TInt err;
    
    INFO_PRINTF1(_L("Test : Video Player - Volume"));
    
    // see if audio is enabled
    if (aPlayer->AudioEnabledL())
        {
        INFO_PRINTF1(_L("AudioEnabledL() returned True"));
        }
    else
        {
        INFO_PRINTF1(_L("AudioEnabledL() returned False"));   
        }
    
    // Check maxvolume function
    if(iVolume == -1)
        {
        iVolume = aPlayer->MaxVolume();
        TRAP(err, aPlayer->SetVolumeL(iVolume));
        INFO_PRINTF2(_L("Volume should be set to Max Volume. Returned with %d"), err);
        INFO_PRINTF3(_L("Volume = %d (expecting %d)"), aPlayer->Volume(), aPlayer->MaxVolume());
        if(aPlayer->Volume() == aPlayer->MaxVolume())
            {
            ret = EPass;
            }
        }
    // Volume should truncated to maxvolume
    else if(iVolume > aPlayer->MaxVolume())
        {
        TRAP(err, aPlayer->SetVolumeL(iVolume));
        INFO_PRINTF2(_L("Volume should be set to Max Volume. Returned with %d"), err);
        INFO_PRINTF3(_L("Volume = %d, MaxVolume = %d"), aPlayer->Volume(), aPlayer->MaxVolume());
        if(aPlayer->Volume() == aPlayer->MaxVolume())
            {
            ret = EPass;
            }
        }
    // Volume is truncated to 0
    else if(iVolume < 0)
        {
        TRAP(err, aPlayer->SetVolumeL(iVolume));
        INFO_PRINTF2(_L("Volume should be set to Min Volume.Returned with %d"), err);
        INFO_PRINTF2(_L("Volume = %d (expecting 0)"), aPlayer->Volume());
        if(aPlayer->Volume() == 0)
            {
            ret = EPass;
            }
        }
    // Set volume and check
    else
        {
        TRAP(err, aPlayer->SetVolumeL(iVolume));
        INFO_PRINTF2(_L("Volume should be set to the desired value. Retuned with %d"), err);
        INFO_PRINTF3(_L("Volume = %d (expecting %d)"), aPlayer->Volume(), iVolume);
        if(aPlayer->Volume() == iVolume)
            {
            ret = EPass;
            }
        }
    
    CActiveScheduler::Stop();
    
    return ret;
    }

//
// RTestVclntCloseOpen
//

/**
 * RTestVclntCloseOpen::Constructor
 */
RTestVclntCloseOpen::RTestVclntCloseOpen(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError)
    : RTestVclntPlayAviFile(aTestName, aSectName, aKeyName, aExpectedError, EFalse)
    {
    iOpenCount = 1;
    }

/**
 * RTestVclntCloseOpen::Constructor
 */
RTestVclntCloseOpen* RTestVclntCloseOpen::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError)
    {
    RTestVclntCloseOpen* self = new (ELeave) RTestVclntCloseOpen(aTestName, aSectName, aKeyName, aExpectedError);
    return self;
    }

/**
 * RTestVclntCloseOpen::DoTestStepL
 */
TVerdict RTestVclntCloseOpen::DoTestStepL()
    {
    // Call the state handler from IDLE state
    FsmL(EVPIdle);
    
    // Start the scheduler - Done only once !
    CActiveScheduler::Start();
    
    return iTestStepResult;
    }

/**
 * RTestVclntCloseOpen::FsmL
 */
void RTestVclntCloseOpen::FsmL(TVclntTestPlayEvents aEventCode)
    {
    if (FsmCheck(aEventCode))
        {
        switch (aEventCode)
            {
            case EVPIdle:
                // Open iVideoPlayer First time
                iOpenCount = 1;
                INFO_PRINTF2(_L("iVideoPlayer->OpenFileL(%S)"), &iFilename);
                TRAP(iError,iVideoPlayer->OpenFileL(iFilename, ControllerUid()));
                PrepareState(EVPOpenComplete, KErrNone);
                break;
            case EVPOpenComplete:
                if (iOpenCount == 1)
                    {
                    // Prepare iVideoPlayer
                    INFO_PRINTF1(_L("iVideoPlayer->Prepare()"));
                    iVideoPlayer->Prepare();
                    PrepareState(EVPPrepareComplete, KErrNone);    
                    }
                else
                    {
                    CActiveScheduler::Stop();
                    iTestStepResult = EPass;
                    break;
                    }
                break;
            case EVPPrepareComplete:
                // Close iVideoPlayer
                CActiveScheduler::Stop();
                INFO_PRINTF1(_L("iVideoPlayer->Close()"));
                iVideoPlayer->Close();
                User::After(KOneSecond);
                // Open iVideoPlayer Second time
                iOpenCount = 2;
                INFO_PRINTF2(_L("iVideoPlayer->OpenFileL(%S)"), &iFilename);
                iVideoPlayer->OpenFileL(iFilename, ControllerUid());
                PrepareState(EVPOpenComplete, KErrNone);
                CActiveScheduler::Start();
                break;
            }
        }
    }

//
// RTestVclntPause
//

/**
 * RTestVclntPause::Constructor
 */
RTestVclntPause::RTestVclntPause(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError)
    : RTestVclntPlayAviFile(aTestName, aSectName, aKeyName, aExpectedError,EFalse)
    {}

/**
 * RTestVclntPause::NewL
 */
RTestVclntPause* RTestVclntPause::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError)
    {
    RTestVclntPause* self = new (ELeave) RTestVclntPause(aTestName, aSectName, aKeyName, aExpectedError);
    return self;
    }

/**
 * RTestVclntPause::DoTestStepL
 */
TVerdict RTestVclntPause::DoTestStepL()
    {
    // Call the state handler from IDLE state
    FsmL(EVPIdle);
    
    // Start the scheduler - Done only once !
    CActiveScheduler::Start();
    
    return iTestStepResult;
    }

/**
 * RTestVclntPause::FsmL
 */
void RTestVclntPause::FsmL(TVclntTestPlayEvents aEventCode)
    {
    TTime stop;
    TTime start = NULL;
    
    if (FsmCheck(aEventCode))
        {
        switch (aEventCode)
            {
            case EVPIdle:
                // Open iVideoPlayer
                INFO_PRINTF2(_L("iVideoPlayer->OpenFileL() %S"), &iFilename);
                TRAP(iError, iVideoPlayer->OpenFileL(iFilename, ControllerUid()));
                PrepareState(EVPOpenComplete, KErrNone);
                break;
            case EVPOpenComplete:
                // Prepare iVideoPlayer
                INFO_PRINTF1(_L("iVideoPlayer->Prepare()"));
                iVideoPlayer->Prepare();
                PrepareState(EVPPrepareComplete, KErrNone);
                break;
            case EVPPrepareComplete:
                // Play iVideoPlayer
                INFO_PRINTF1(_L("iVideoPlayer->Play()"));
                PrepareState(EVPPlayComplete, KErrNone);
                iVideoPlayer->Play();
                // Pause iVideoPlayer
                INFO_PRINTF1(_L("iVideoPlayer->Pause()"));
                TRAPD(err,iVideoPlayer->PauseL());
                if (err == KErrNotSupported)
                    {
                    INFO_PRINTF1(_L("Pause not supported presently"));
                    }
                else
                    {
                    INFO_PRINTF2(_L("Pause() returns with error :  %d"),err);
                    iTestStepResult = EFail;
                    CActiveScheduler::Stop();
                    break;
                    }
                // Stop iVideoPlayer
                INFO_PRINTF1(_L("iVideoPlayer->Stop()"));
                iVideoPlayer->Stop();
                // Get the duration of the file.
                iPlayerDuration = I64INT(iVideoPlayer->DurationL().Int64());
                // Resume play iVideoPlayer
                INFO_PRINTF1(_L("iVideoPlayer->Play()"));
                iVideoPlayer->Play();
                // Note the start time
                start.HomeTime();    
                break;
            case EVPPlayComplete:
                // Note the stop time
                stop.HomeTime();
                // Get the actual duration
                iActualDuration = I64INT(stop.MicroSecondsFrom(start).Int64());
                INFO_PRINTF6(_L("Error : %d Start = %d Stop = %d PlayerDuration = %d ActualDuration = %d"),
                    iError, I64INT(start.Int64()), I64INT(stop.Int64()), iPlayerDuration, iActualDuration);
                // Check the results
                if((iError == KErrNone) && (TimeComparison(iActualDuration, iPlayerDuration, KExpectedDeviation)))
                    {
                    iTestStepResult = EPass;
                    }
                CActiveScheduler::Stop();
                break;
            }
        }
    }

//
// RTestVclntBalance
//

/**
 * RTestVclntBalance::Constructor
 */
RTestVclntBalance::RTestVclntBalance(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError, const TInt aBalance)
    : RTestVclntPlayAviFile(aTestName, aSectName, aKeyName, aExpectedError, EFalse)
    {
    iBalance = aBalance;
    }

/**
 * RTestVclntBalance::NewL
 */
RTestVclntBalance* RTestVclntBalance::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError, const TInt aBalance)
    {
    RTestVclntBalance* self = new (ELeave) RTestVclntBalance(aTestName, aSectName, aKeyName, aExpectedError, aBalance);
    return self;
    }

/**
 * RTestVclntBalance::DoTestL
 */
TVerdict RTestVclntBalance::DoTestL(CVideoPlayerUtility* aPlayer)
    {
    TVerdict ret = EFail;
    TInt err = KErrNone;
    
    INFO_PRINTF1(_L("Test : Video Player - Balance"));
    
    // see if audio is enabled
    if (aPlayer->AudioEnabledL())
        {
        INFO_PRINTF1(_L("AudioEnabledL() returned True"));
        }
    else
        {
        INFO_PRINTF1(_L("AudioEnabledL() returned False"));   
        }
    
    // set the balance
    TRAP(err, aPlayer->SetBalanceL(iBalance));
    INFO_PRINTF2(_L("SetBalanceL() left with Error - %d"), err);
    
    // check for the valid balance    
    if (iBalance < KMinBalance)
        {
        INFO_PRINTF3(_L("Balance = %d (expecting %d)"), aPlayer->Balance(), KMinBalance);
        if(aPlayer->Balance() == KMinBalance)
            {
            ret = EPass;
            }
        }
    else if (iBalance > KMaxBalance)
        {
        INFO_PRINTF3(_L("Balance = %d (expecting %d)"), aPlayer->Balance(), KMaxBalance);
        if(aPlayer->Balance() == KMaxBalance)
            {
            ret = EPass;
            }
        }
    else
        {
        INFO_PRINTF3(_L("Balance = %d (expecting %d)"), aPlayer->Balance(), iBalance);
        if(aPlayer->Balance() == iBalance)
            {
            ret = EPass;
            }
        }
    
    CActiveScheduler::Stop();
    
    return ret;
    }


//
// RTestVclntPlayWindow
//

/**
 * RTestVclntPlayWindow::Constructor
 */
RTestVclntPlayWindow::RTestVclntPlayWindow(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError, const TTimeIntervalMicroSeconds aStart, const TTimeIntervalMicroSeconds aEnd)
    : RTestVclntPlayAviFile(aTestName, aSectName, aKeyName, aExpectedError, EFalse)
    {
    iStart = aStart;
    iEnd = aEnd;
    }

/**
 * RTestVclntPlayWindow::Constructor
 */
RTestVclntPlayWindow* RTestVclntPlayWindow::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError, const TTimeIntervalMicroSeconds aStart, const TTimeIntervalMicroSeconds aEnd)
    {
    RTestVclntPlayWindow* self = new (ELeave) RTestVclntPlayWindow(aTestName, aSectName, aKeyName, aExpectedError, aStart, aEnd);
    return self;
    }

/**
 * RTestVclntPlayWindow::Constructor
 */
TVerdict RTestVclntPlayWindow::DoTestL(CVideoPlayerUtility* aPlayer)
    {
    TVerdict ret = EFail;

    INFO_PRINTF1(_L("Test : Video Player - Window"));

    TPoint rectOrigin(10, 10);
    TSize rectSize(176, 144);
    TRect rect(rectOrigin, rectSize);
    
    TPoint clipOrigin(10, 10);
    TSize clipSize(176, 144);
    TRect clipRect(clipOrigin, clipSize);
    
    // first call tests creation of display instance
    TRAPD(err, aPlayer->SetDisplayWindowL(iWs, *iScreen, *iWindow, rect, clipRect));
    INFO_PRINTF1(_L("Test : Made first call to SetDisplayWindowL()"));
    if(err)
        {
        ERR_PRINTF2(_L("First call to SetDisplayWindowL() failed, error %d"), err);
        CActiveScheduler::Stop();
        return EFail;
        }

    // second call tests update of display instance
    TRAP(err, aPlayer->SetDisplayWindowL(iWs, *iScreen, *iWindow, rect, clipRect));
    INFO_PRINTF1(_L("Test : Made second call to SetDisplayWindowL()"));
    if(err)
        {
        ERR_PRINTF2(_L("Second call to SetDisplayWindowL() failed, error %d"), err);
        ret = EFail;
        }
    else
        {
        ret = EPass;
        }

    CActiveScheduler::Stop();
    return ret;
    }


//
// RTestVclntMeta
//

/**
 * RTestVclntMeta::Constructor
 */
RTestVclntMeta::RTestVclntMeta(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError)
    : RTestVclntPlayAviFile(aTestName, aSectName, aKeyName, aExpectedError, EFalse)
    {}

/**
 * RTestVclntMeta::Constructor
 */
RTestVclntMeta* RTestVclntMeta::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError)
    {
    RTestVclntMeta* self = new (ELeave) RTestVclntMeta(aTestName, aSectName, aKeyName, aExpectedError);
    return self;
    }

/**
 * RTestVclntMeta::Constructor
 */
TVerdict RTestVclntMeta::DoTestL(CVideoPlayerUtility* aPlayer)
    {
    INFO_PRINTF1(_L("Test : Video Player - Metadata"));
    
    CActiveScheduler::Stop();
    
    // there are zero meta entries now.
    TInt numOfMeta = -1;
    TRAPD(err, numOfMeta = aPlayer->NumberOfMetaDataEntriesL() );
    INFO_PRINTF3(_L("Error : %d Entries = %d"), err, numOfMeta);
    if (err != KErrNotSupported || numOfMeta != -1)
        {
        return EFail;
        }
    
    // attempt to get an entry when no entry exists.
    CMMFMetaDataEntry* theEntry=NULL;
    TRAP(err, theEntry = aPlayer->MetaDataEntryL(1));
    INFO_PRINTF2(_L("Error : %d"), err);
    if (err != KErrNotSupported)
        {
        return EFail;
        }
    else 
        {
        delete theEntry;
        return EPass;
        }
    }


//
// RTestVclntFrameSize
//

/**
 * RTestVclntFrameSize::Constructor
 */
RTestVclntFrameSize::RTestVclntFrameSize(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError)
    : RTestVclntPlayAviFile(aTestName, aSectName, aKeyName, aExpectedError, EFalse)
    {
    iFrameSize.iWidth = 0;
    iFrameSize.iHeight = 0;
    }

/**
 * RTestVclntFrameSize::NewL
 */
RTestVclntFrameSize* RTestVclntFrameSize::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError)
    {
    RTestVclntFrameSize* self = new (ELeave) RTestVclntFrameSize(aTestName, aSectName, aKeyName, aExpectedError);
    return self;
    }

/**
 * RTestVclntFrameSize::DoTestL
 */
TVerdict RTestVclntFrameSize::DoTestL(CVideoPlayerUtility* aPlayer)
    {
    TVerdict ret = EFail;

    INFO_PRINTF1(_L("Test : Video Player - Get Frame Size"));
    TInt err;
    TRAP(err, aPlayer->VideoFrameSizeL(iFrameSize));

    INFO_PRINTF4(_L("Error : %d, Frame size : (%d, %d)"), err, iFrameSize.iWidth, iFrameSize.iHeight);
    if (err == KErrNone && iFrameSize.iWidth >= 0 && iFrameSize.iHeight >= 0 )
        {
        ret = EPass;
        }
    
    CActiveScheduler::Stop();
    
    return ret;
    }

//
// RTestVclntMimeType
//

/**
 * RTestVclntMimeType::Constructor
 */
RTestVclntMimeType::RTestVclntMimeType(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError)
    : RTestVclntPlayAviFile(aTestName, aSectName, aKeyName, aExpectedError,EFalse)
    {
    }

/**
 * RTestVclntMimeType::Constructor
 */
RTestVclntMimeType* RTestVclntMimeType::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError)
    {
    RTestVclntMimeType* self = new (ELeave) RTestVclntMimeType(aTestName, aSectName, aKeyName, aExpectedError);
    return self;
    }

/**
 * RTestVclntMimeType::Constructor
 */
TVerdict RTestVclntMimeType::DoTestL(CVideoPlayerUtility* aPlayer)
    {
        
    TVerdict ret = EFail;
    
    INFO_PRINTF1(_L("Test : Video Player - MimeType"));

    // Get the Video Mime type
    TPtrC8 mimeType = aPlayer->VideoFormatMimeType();

    // take it to a 16-bit string
    TBuf<64> mimeType16;
    mimeType16.Copy(mimeType);
    INFO_PRINTF2(_L("Mime type : \'%S\'"), &mimeType16);
    
    // Check if its valid
    if (mimeType16.Compare(_L("XVID")) == 0)
        {
        INFO_PRINTF1(_L("MIME Types match"));
        ret = EPass;
        }
    
    CActiveScheduler::Stop();
    
    return ret;
    }

//
// RTestVclntScale
//

/**
 * RTestVclntScale::Constructor
 */
RTestVclntScale::RTestVclntScale(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError)
    : RTestVclntPlayAviFile(aTestName, aSectName, aKeyName, aExpectedError,EFalse)
    {
    }

/**
 * RTestVclntScale::Constructor
 */
RTestVclntScale* RTestVclntScale::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError)
    {
    RTestVclntScale* self = new (ELeave) RTestVclntScale(aTestName, aSectName, aKeyName, aExpectedError);
    return self;
    }

/**
 * RTestVclntScale::Constructor
 */
TVerdict RTestVclntScale::DoTestL(CVideoPlayerUtility* aPlayer)
    {
    INFO_PRINTF1(_L("Test : Video Player - Scale Values test"));
    
    CActiveScheduler::Stop();
    
    // Set the scale factor using values set #1
    INFO_PRINTF1(_L("iVideoPlayer->SetScaleFactorL() #1"));
    TRAPD(err, aPlayer->SetScaleFactorL(100, 100, ETrue));

#ifdef SYMBIAN_BUILD_GCE
	TInt expected = iBinaryCompatibility || !GCEAvailable() ? KErrNotSupported : KErrNone;
#else
	TInt expected = KErrNotSupported;
#endif

    if (err != expected)
        {       
        ERR_PRINTF2(_L("iVideoPlayer->SetScaleFactorL() encountered error : %d"), err);
        return EFail;
        }

    return EPass;
    }

//
// RTestVclntCrop
//

/**
 * RTestVclntCrop::Constructor
 */
RTestVclntCrop::RTestVclntCrop(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError)
    : RTestVclntPlayAviFile(aTestName, aSectName, aKeyName, aExpectedError,EFalse)
    {
    }

/**
 * RTestVclntCrop::Constructor
 */
RTestVclntCrop* RTestVclntCrop::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError)
    {
    RTestVclntCrop* self = new (ELeave) RTestVclntCrop(aTestName, aSectName, aKeyName, aExpectedError);
    return self;
    }

/**
 * RTestVclntCrop::Constructor
 */
TVerdict RTestVclntCrop::DoTestL(CVideoPlayerUtility* aPlayer)
    {
    TRect cropRegion(KCropRegionRectLeft, KCropRegionRectTop, KCropRegionRectRight, KCropRegionRectBottom);
    TRect region;
    
    INFO_PRINTF1(_L("Test : Video Player - Crop Values"));
    
    CActiveScheduler::Stop();

    // Set the Crop Region
    INFO_PRINTF1(_L("iVideoPlayer->SetCropRegionL()"));
    TRAPD(err, aPlayer->SetCropRegionL(cropRegion));
    
#ifdef SYMBIAN_BUILD_GCE
	TInt expected = iBinaryCompatibility || !GCEAvailable() ? KErrNotSupported : KErrNone;
#else
	TInt expected = KErrNotSupported;
#endif

    if (err != expected)
        {       
        ERR_PRINTF2(_L("iVideoPlayer->SetCropRegionL() encountered error : %d"), err);
        return EFail;
        }

    return EPass;
    }
    
#ifdef SYMBIAN_BUILD_GCE

RTestVclntAutoScale::RTestVclntAutoScale(const TDesC& aTestName)
    : RTestVclntPlayAviFile(aTestName, KNullDesC, KNullDesC, KErrNone, EFalse)
    {
    }

/**
 * RTestVclntCrop::Constructor
 */
RTestVclntAutoScale* RTestVclntAutoScale::NewL(const TDesC& aTestName)
    {
    RTestVclntAutoScale* self = new (ELeave) RTestVclntAutoScale(aTestName);
    return self;
    }

/**
 * RTestVclntCrop::Constructor
 */
TVerdict RTestVclntAutoScale::DoTestL(CVideoPlayerUtility* aPlayer)
    {
    INFO_PRINTF1(_L("Test : Video Player - AutoScale Values"));
    
    CActiveScheduler::Stop();

    // Set auto scale
    INFO_PRINTF1(_L("iVideoPlayer->SetAutoScaleL()"));
    TRAPD(err, aPlayer->SetAutoScaleL(EAutoScaleBestFit));

    if (err != KErrNone)
        {
        ERR_PRINTF2(_L("iVideoPlayer->SetAutoScaleL() encountered error : %d"), err);
        return EFail;
        }

    TRAP(err, aPlayer->SetAutoScaleL(EAutoScaleNone));

    if (err != KErrNone)
        {
        ERR_PRINTF2(_L("iVideoPlayer->SetAutoScaleL() encountered error : %d"), err);
        return EFail;
        }

    TRAP(err, aPlayer->SetAutoScaleL(EAutoScaleClip));

    if (err != KErrNone)
        {
        ERR_PRINTF2(_L("iVideoPlayer->SetAutoScaleL() encountered error : %d"), err);
        return EFail;
        }

    TRAP(err, aPlayer->SetAutoScaleL(EAutoScaleStretch, EHorizontalAlignLeft, EVerticalAlignBottom));

    if (err != KErrNone)
        {
        ERR_PRINTF2(_L("iVideoPlayer->SetAutoScaleL() encountered error : %d"), err);
        return EFail;
        }

    return EPass;
    }

#endif // SYMBIAN_BUILD_GCE

//
// RTestVclntGetFrame
//

/**
 * RTestVclntGetFrame::Constructor
 */
RTestVclntGetFrame::RTestVclntGetFrame(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError, TBool aUseIntentAPI)
    : RTestVclntPlayAviFile(aTestName, aSectName, aKeyName, aExpectedError, EFalse), iUseIntentAPI(aUseIntentAPI)
    {
    }

/**
 * RTestVclntGetFrame::Constructor
 */
RTestVclntGetFrame* RTestVclntGetFrame::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError, TBool aUseIntentAPI)
    {
    RTestVclntGetFrame* self = new (ELeave) RTestVclntGetFrame(aTestName, aSectName, aKeyName, aExpectedError, aUseIntentAPI);
    return self;
    }

/**
 * RTestVclntGetFrame::DoTestStepL
 */
TVerdict RTestVclntGetFrame::DoTestStepL()
    {
    // Call the state handler from IDLE state
    FsmL(EVPIdle);
    
    // Start the scheduler - Done only once !
    CActiveScheduler::Start();
    
    return iTestStepResult;
    }

/**
 * RTestVclntGetFrame::FsmL
 */
void RTestVclntGetFrame::FsmL(TVclntTestPlayEvents aEventCode)
    {
    if (FsmCheck(aEventCode))
        {
        switch (aEventCode)
            {
            case EVPIdle:
                // Open iVideoPlayer
                INFO_PRINTF2(_L("iVideoPlayer->OpenFileL(%S)"), &iFilename);
                TRAP(iError,iVideoPlayer->OpenFileL(iFilename, ControllerUid()));
                PrepareState(EVPOpenComplete, KErrNone);
                break;
            case EVPOpenComplete:
                // Prepare iVideoPlayer
                INFO_PRINTF1(_L("iVideoPlayer->Prepare()"));
                iVideoPlayer->Prepare();
                PrepareState(EVPPrepareComplete, KErrNone);
                break;
            case EVPPrepareComplete:
                // Play iVideoPlayer
                INFO_PRINTF1(_L("iVideoPlayer->Play()"));
                iVideoPlayer->Play();
                // Call GetFrameL
                if (iUseIntentAPI)
                    {
                    INFO_PRINTF1(_L("iVideoPlayer->GetFrameL() with EPeek Intent"));
                    TRAP(iError, iVideoPlayer->GetFrameL(EColor16M, ContentAccess::EPeek));
                    INFO_PRINTF2(_L("iVideoPlayer->GetFrameL() left with error - %d"), iError);
                    }
                else 
                    {
                    INFO_PRINTF1(_L("iVideoPlayer->GetFrameL()"));
                    TRAP(iError, iVideoPlayer->GetFrameL(EColor16M));
                    INFO_PRINTF2(_L("iVideoPlayer->GetFrameL() left with error - %d"), iError);
                    }
                PrepareState(EVPFrameReady, KErrNone);
            case EVPFrameReady:
                // Stop iVideoPlayer
                iVideoPlayer->Stop();
                CActiveScheduler::Stop();
                iTestStepResult = EPass;
                break;
            }
        }
    }

/**
 * RTestVclntGetFrame::MvpuoFrameReady
 * Overriding the callback method here, to check the bitmap..
 */
void RTestVclntGetFrame::MvpuoFrameReady(CFbsBitmap& aFrame, TInt aError)
    {
    if (aError!=KErrNone)
        {
        iError = aError;
        }
    else if (aFrame.Handle() == NULL || aFrame.SizeInPixels() != TSize(KFrameWidth, KFrameHeight) )
        {
        iError = KErrArgument;
        ERR_PRINTF1(_L("The Frame returned is either NULL or there is a mismatch in the size.."));
        }
    TRAP(iError, FsmL(EVPFrameReady));
    INFO_PRINTF2(_L("Error code : %d"),iError);
    }

    
//
// RTestVclntRebuffering
//

/**
 * RTestVclntRebuffering::Constructor
 */
RTestVclntRebuffering::RTestVclntRebuffering(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError)
    : RTestVclntPlayAviFile(aTestName, aSectName, aKeyName, aExpectedError,EFalse)
    {
    }

/**
 * RTestVclntRebuffering::NewL
 */
RTestVclntRebuffering* RTestVclntRebuffering::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError)
    {
    RTestVclntRebuffering* self = new (ELeave) RTestVclntRebuffering(aTestName, aSectName, aKeyName, aExpectedError);
    return self;
    }

/**
 * RTestVclntRebuffering::DoTestL
 */
TVerdict RTestVclntRebuffering::DoTestL(CVideoPlayerUtility* aPlayer)
    {
    INFO_PRINTF1(_L("Test : Video Player - Rebuffering"));

    aPlayer->RegisterForVideoLoadingNotification(*this);

    INFO_PRINTF1(_L("Rebuffering request completed"));
    
    CActiveScheduler::Stop();
    
    return EPass;
    }

/**
 * RTestVclntRebuffering::MvloLoadingStarted
 */
void RTestVclntRebuffering::MvloLoadingStarted()
    {
    INFO_PRINTF1(_L("RTestVclntRebuffering::MvloLoadingStarted"));
    }

/**
 * RTestVclntRebuffering::MvloLoadingComplete
 */
void RTestVclntRebuffering::MvloLoadingComplete()
    {
    INFO_PRINTF1(_L("RTestVclntRebuffering::MvloLoadingComplete"));
    }

//
// RTestVclntRepeat
//

/**
 * RTestVclntRepeat::Constructor
 */
RTestVclntRepeat::RTestVclntRepeat(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError,const TInt aRepeat)
    : RTestVclntPlayAviFile(aTestName, aSectName, aKeyName, aExpectedError,EFalse)
    {
    iRepeat = aRepeat;
    }

/**
 * RTestVclntRepeat::NewL
 */
RTestVclntRepeat* RTestVclntRepeat::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError,const TInt aRepeat)
    {
    RTestVclntRepeat* self = new (ELeave) RTestVclntRepeat(aTestName, aSectName, aKeyName, aExpectedError, aRepeat);
    return self;
    }

/**
 * RTestVclntRepeat::DoTestL
  */
  //The commented lines are kept for future implementation
  
TVerdict RTestVclntRepeat::DoTestL(CVideoPlayerUtility* aPlayer)
    {
    TVerdict ret = EFail;

    INFO_PRINTF1(_L("Test : Video Player - Repeats"));
    iRepeat = 1;
    INFO_PRINTF1(_L("Warning : SetRepeats() does not exist in iVideoPlayer API. Repeat count set to 1"));
    
    INFO_PRINTF1(_L("iVideoPlayer->Play()"));
    PrepareState(EVPPlayComplete, KErrNone);
    aPlayer->Play();
    
    CActiveScheduler::Start();
    
    if(iError == KErrNotSupported || iRepeat == 1) 
        {
        ret = EPass;    
        INFO_PRINTF2(_L("Repeat not supported currently : err : %d"),iError);   
        }
    else
        {
        ret = EFail;
        INFO_PRINTF2(_L("Returned with : err : %d"),iError);   
        }    
        
    CActiveScheduler::Stop();
    
    return ret;
    }

//
// RTestVclntDelete
//

/**
 * RTestVclntDelete::Constructor
 */
RTestVclntDelete::RTestVclntDelete(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError)
    : RTestVclntPlayAviFile(aTestName, aSectName, aKeyName, aExpectedError,EFalse)
    {}

/**
 * RTestVclntDelete::NewL
 */
RTestVclntDelete* RTestVclntDelete::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError)
    {
    RTestVclntDelete* self = new (ELeave) RTestVclntDelete(aTestName, aSectName, aKeyName, aExpectedError);
    return self;
    }

/**
 * RTestVclntDelete::DoTestStepL
 */
TVerdict RTestVclntDelete::DoTestStepL()
    {
    // Call the state handler from IDLE state
    FsmL(EVPIdle);
    
    // Start the scheduler - Done only once !
    CActiveScheduler::Start();
    
    return iTestStepResult;
    }

/**
 * RTestVclntDelete::FsmL
 */
void RTestVclntDelete::FsmL(TVclntTestPlayEvents aEventCode)
    {
    if (FsmCheck(aEventCode))
        {
        switch (aEventCode)
            {
            case EVPIdle:
                // Open iVideoPlayer
                INFO_PRINTF2(_L("iVideoPlayer->OpenFileL(%S)"), &iFilename);
                TRAP(iError,iVideoPlayer->OpenFileL(iFilename, ControllerUid()));
                PrepareState(EVPOpenComplete, KErrNone);
                break;
            case EVPOpenComplete:
                // Prepare iVideoPlayer
                INFO_PRINTF1(_L("iVideoPlayer->Prepare()"));
                iVideoPlayer->Prepare();
                PrepareState(EVPPrepareComplete, KErrNone);    
                break;
            case EVPPrepareComplete:
                // Play iVideoPlayer
                INFO_PRINTF1(_L("iVideoPlayer->Play()"));
                iVideoPlayer->Play();
                PrepareState(EVPPlayComplete, KErrNone);
                INFO_PRINTF1(_L("delete iVideoPlayer before Play completes"));
                
                iVideoPlayer = NULL;
                delete iVideoPlayer; // destroy iVideoPlayer before play has completed
                
                CActiveScheduler::Stop();
                iTestStepResult = EPass;
                break;
            }
        }
    }

//
// RTestVclntPlayAviFileHandle
//

/**
 * RTestVclntPlayAviFileHandle::Constructor
 */
RTestVclntPlayAviFileHandle::RTestVclntPlayAviFileHandle(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError, const TBool aAlloc)
    : RTestVclntAviPlayerStep(aTestName, aSectName, aKeyName, aExpectedError), iAlloc(aAlloc)
    {
    iIsOpen = EFalse;
    }

/**
 * RTestVclntPlayAviFileHandle::NewL
 */
RTestVclntPlayAviFileHandle* RTestVclntPlayAviFileHandle::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError, const TBool aAlloc)
    {
    RTestVclntPlayAviFileHandle* self = new (ELeave) RTestVclntPlayAviFileHandle(aTestName, aSectName, aKeyName, aExpectedError, aAlloc);
    return self;
    }

/**
 * RTestVclntPlayAviFileHandle::NewLC
 */
RTestVclntPlayAviFileHandle* RTestVclntPlayAviFileHandle::NewLC(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError, const TBool aAlloc)
    {
    RTestVclntPlayAviFileHandle* self = new (ELeave) RTestVclntPlayAviFileHandle(aTestName, aSectName, aKeyName, aExpectedError, aAlloc);
    CleanupStack::PushL(self);
    return self;
    }

/**
 * RTestVclntPlayAviFileHandle::DoTestStepL()
 */
TVerdict RTestVclntPlayAviFileHandle::DoTestStepL()
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
        if( PerformTestStepL() != EPass )
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
            
             //INFO_PRINTF2(_L("CVideoPlayerUtility: Alloc Test Loop: %d"), failCount);
            //>>>>>>>>>>>>>>>>>>>>>>>>Test Method Call<<<<<<<<<<<<<<<<<<<<<<<<<<
            TVerdict verdict = EFail;
            TRAP(err, verdict = PerformTestStepL());
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
 * RTestVclntPlayAviFileHandle::PerformTestStepL()
 */
TVerdict RTestVclntPlayAviFileHandle::PerformTestStepL()
    {
    INFO_PRINTF1(_L("Test : Video Player - OpenFileL(RFile&)"));
    
    // Connect to RFs
    User::LeaveIfError(iFs.Connect());
    User::LeaveIfError(iFs.ShareProtected());
    CleanupClosePushL(iFs);
    
    // Open RFile
    User::LeaveIfError(iFile.Open(iFs, iFilename, EFileRead));
    CleanupClosePushL(iFile);
    
    // Call the state handler from IDLE state
    iError = KErrNone;
    PrepareState(EVPIdle, KErrNone);
    FsmL(EVPIdle);
    
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
 * RTestVclntPlayAviFileHandle::FsmL()
 */
void RTestVclntPlayAviFileHandle::FsmL(TVclntTestPlayEvents aEventCode)
    {
    if (FsmCheck(aEventCode))
        {
        switch (aEventCode)
            {
            case EVPIdle:
                // Open iVideoPlayer
                if ((iTestStepName.Compare(_L("MM-MMF-VCLNTAVI-I-1013-HP")) == 0) ||
                	(iTestStepName.Compare(_L("MM-MMF-VCLNTAVI-I-9113-HP")) == 0))
                    {
                    INFO_PRINTF2(_L("iVideoPlayer->OpenFileL() %S with Invalid Controller UID"), &iFilename);
                    TRAP(iError, iVideoPlayer->OpenFileL(iFile, KVideoInvalidControllerUid));
                    INFO_PRINTF2(_L("iVideoPlayer->OpenFileL() returned error - %d"), iError);
                    PrepareState(EVPOpenComplete, KErrNone);
                    }
                else
                    {
                    INFO_PRINTF2(_L("iVideoPlayer->OpenFileL() %S"), &iFilename);
                	iVideoPlayer->OpenFileL(iFile, ControllerUid());
                    PrepareState(EVPOpenComplete, KErrNone);
                    }
                break;
            case EVPOpenComplete:
                iIsOpen = ETrue; //-set iIsOpen flag (for closing Player, in case of Underflow)...
                // Prepare iVideoPlayer
                INFO_PRINTF1(_L("iVideoPlayer->Prepare()"));
                PrepareState(EVPPrepareComplete, KErrNone);
                iVideoPlayer->Prepare();
                break;
            case EVPPrepareComplete:
                // Play iVideoPlayer
                INFO_PRINTF1(_L("iVideoPlayer->Play()"));
                iVideoPlayer->Play();
                PrepareState(EVPPlayComplete, KErrNone);
                break;
            case EVPPlayComplete:
                iVideoPlayer->Stop();
                iVideoPlayer->Close();
                CActiveScheduler::Stop();
                iTestStepResult = EPass;
                break;
            }
        }
    else if(iIsOpen)
        {
        iVideoPlayer->Close();
        }
    }

TInt RTestVclntPlayAviFileHandle::SetCacheSize()
	{
	TInt error=UserSvr::HalFunction(EHalGroupVM,EVMHalSetCacheSize,(TAny*)KCacheSize,(TAny*)KMaxCacheSize);
	TInt ret=CheckCacheError(error);
		
	return ret;
	}

TInt RTestVclntPlayAviFileHandle::CheckCacheError(TInt aError)
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
// RTestVideoPlayCapabilityVelocity
//

/**
RTestVideoPlayCapabilityVelocity::Constructor
*/
RTestVideoPlayCapabilityVelocity::RTestVideoPlayCapabilityVelocity(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError)
    : RTestVclntAviPlayerStep(aTestName, aSectName, aKeyName, aExpectedError)
    { ;
    }

/**
RTestVideoPlayCapabilityVelocity::NewL
*/
RTestVideoPlayCapabilityVelocity* RTestVideoPlayCapabilityVelocity::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError)
	{
    RTestVideoPlayCapabilityVelocity* self = new (ELeave) RTestVideoPlayCapabilityVelocity(aTestName, aSectName, aKeyName, aExpectedError);
    return self;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         
    }

/**
RTestVideoPlayCapabilityVelocity::DoTestStepL()
*/
TVerdict RTestVideoPlayCapabilityVelocity::DoTestStepL()
    {
    // Call the state handler from IDLE state
    TRAPD(err, FsmL(EVPIdle));
    if (err == KErrNone)
        {
        // Start the scheduler - Done only once !
        CActiveScheduler::Start();    
        }
    return iTestStepResult;  
    }
/**
RTestVideoPlayCapabilityVelocity::FsmL()
*/
void RTestVideoPlayCapabilityVelocity::FsmL(TVclntTestPlayEvents aEventCode)
    {
    const TUint KVelocityDefault = 100;
    const TUint KVelocityMinPositive = 1;
    const TInt KVelocityMinNegative = -1;
    const TUint KVelocityPositive = 200;
    const TUint KVelocityBigPositive = 500;
    const TInt KVelocityNegative = -200;
    const TInt KVelocityBigNegative= -500;
    //Set default value for PlayRateCapability
    iVideoPlayRateCapabilities.iStepForward = 1;
    iVideoPlayRateCapabilities.iPlayBackward = 0;
    if (FsmCheck(aEventCode))
        {
        TInt err;
        switch (aEventCode)
            {
            case EVPIdle:
	        	//Calling GetPlayRateCapabilitiesL() before opening should return KErrNotReady.
	        	INFO_PRINTF1(_L("iVideoPlayer->GetPlayRateCapabilitiesL()"));            	
	            TRAP(err,iVideoPlayer->GetPlayRateCapabilitiesL(iVideoPlayRateCapabilities));
	            if( err != KErrNotReady)
	            	{
	            	INFO_PRINTF2(_L("GetPlayRateCapabilitiesL returned wrong err= %d"), err);
	                iTestStepResult = EFail;
	                CActiveScheduler::Stop();
	                return;                	
	            	}
	            iPlayVelocity = KVelocityDefault;
	        	//Calling SetPlayVelocityL() before opening should return KErrNotReady.
	        	INFO_PRINTF2(_L("iVideoPlayer->SetPlayVelocityL(%d)"),iPlayVelocity);
                TRAP(err, iVideoPlayer->SetPlayVelocityL(iPlayVelocity));
                if(KErrNotReady != err)
                	{
                	INFO_PRINTF2(_L("SetPlayVelocityL returned wrong err= %d"), err);
                    iTestStepResult = EFail;
                    CActiveScheduler::Stop();
                    return;
                	}

                //calling PlayVelocityL before opening should result in error KErrNotReady
            	INFO_PRINTF1(_L("iVideoPlayer->PlayVelocityL()"));            	
                TRAP(err, iPlayVelocity = iVideoPlayer->PlayVelocityL());
                if( err != KErrNotReady)
                	{
                	INFO_PRINTF2(_L("PlayVelocityL returned wrong err= %d"), err);
                    iTestStepResult = EFail;
                    CActiveScheduler::Stop();
                    return;
                	}
                
                // Open iVideoPlayer
                INFO_PRINTF2(_L("iVideoPlayer->OpenFileL() %S"), &iFilename);
                iVideoPlayer->OpenFileL(iFilename);
                PrepareState(EVPOpenComplete, KErrNone);
                break;
            case EVPOpenComplete:
            	//Get playrate capabilities
            	INFO_PRINTF1(_L("iVideoPlayer->GetPlayRateCapabilitiesL()"));            	
                TRAP(err,iVideoPlayer->GetPlayRateCapabilitiesL(iVideoPlayRateCapabilities));
                if( err != KErrNone)
                	{
                	INFO_PRINTF2(_L("GetPlayRateCapabilitiesL returned err= %d"), err);
                	INFO_PRINTF1(_L("invalid TPlayRateCapabilities"));
                    iTestStepResult = EFail;
                    CActiveScheduler::Stop();
                    return;                	
                	}
            	INFO_PRINTF2(_L("iVideoPlayRateCapabilities.iPlayForward= %d"), iVideoPlayRateCapabilities.iPlayForward);
                INFO_PRINTF2(_L("iVideoPlayRateCapabilities.iPlayBackward= %d"), iVideoPlayRateCapabilities.iPlayBackward);
                INFO_PRINTF2(_L("iVideoPlayRateCapabilities.iStepForward= %d"), iVideoPlayRateCapabilities.iStepForward);
                INFO_PRINTF2(_L("iVideoPlayRateCapabilities.iStepBackward= %d"), iVideoPlayRateCapabilities.iStepBackward);                
                if ((iVideoPlayRateCapabilities.iPlayForward != 0) && (iVideoPlayRateCapabilities.iPlayForward != 1)||
                	(iVideoPlayRateCapabilities.iPlayBackward != 0) && (iVideoPlayRateCapabilities.iPlayBackward != 1)||
                	(iVideoPlayRateCapabilities.iStepForward != 0) && (iVideoPlayRateCapabilities.iStepForward != 1)||
                	(iVideoPlayRateCapabilities.iStepBackward != 0) && (iVideoPlayRateCapabilities.iStepBackward != 1) )
                	{
                	INFO_PRINTF1(_L("invalid TPlayRateCapabilities"));
                    iTestStepResult = EFail;
                    CActiveScheduler::Stop();
                    return;
                	}
                
                //Get default play velocity, it shall be 100.
            	INFO_PRINTF1(_L("iVideoPlayer->PlayVelocityL()"));            	
                TRAP(err, iPlayVelocity = iVideoPlayer->PlayVelocityL());
                if( err != KErrNone)
                	{
                	INFO_PRINTF2(_L("PlayVelocityL returned err= %d"), err);
                	INFO_PRINTF1(_L("invalid TPlayRateCapabilities"));
                    iTestStepResult = EFail;
                    CActiveScheduler::Stop();
                    return;                	
                	}
            	
            	if( iPlayVelocity != KVelocityDefault )
            		{
                	INFO_PRINTF2(_L("invalid default play velocity =%d"), iPlayVelocity);
                    iTestStepResult = EFail;
                    CActiveScheduler::Stop();
                    return;
            		}
            	
                // Prepare iVideoPlayer
                INFO_PRINTF1(_L("iVideoPlayer->Prepare()"));
                PrepareState(EVPPrepareComplete, KErrNone);
                iVideoPlayer->Prepare();
                break;
            case EVPPrepareComplete:
            	//Try to set different play velocities and play.
	        	INFO_PRINTF2(_L("iVideoPlayer->SetPlayVelocityL(%d)"),iPlayVelocity);
                TRAP(err, iVideoPlayer->SetPlayVelocityL(iPlayVelocity));
                if(KErrNotSupported == err)
                	{
                	if(iVideoPlayRateCapabilities.iPlayForward || iVideoPlayRateCapabilities.iPlayBackward)
                		{
	                	INFO_PRINTF2(_L("SetPlayVelocityL returned err= %d"), err);
	                    iTestStepResult = EFail;
	                    CActiveScheduler::Stop();
	                    return;                	
                		}
                	INFO_PRINTF1(_L("SetPlayVelocityL returned err KErrNotSupported"));
                	}
                else if( err != KErrNone)
                	{
                	INFO_PRINTF2(_L("SetPlayVelocityL returned err= %d"), err);
                    iTestStepResult = EFail;
                    CActiveScheduler::Stop();
                    return;                	
                	}	        	
                // Play iVideoPlayer
                INFO_PRINTF1(_L("iVideoPlayer->Play()"));
                iVideoPlayer->Play();
                PrepareState(EVPPlayComplete, KErrNone);
                break;
            case EVPPlayComplete:
                //Try to set different play velocities and play.
                if (iVideoPlayRateCapabilities.iPlayForward )
                	{
	                if (iPlayVelocity == KVelocityDefault)
	                	{
	                	iPlayVelocity = KVelocityMinPositive;
	                	}
	                else if(iPlayVelocity == KVelocityMinPositive)
	                	{
	                	iPlayVelocity = KVelocityPositive;
	                	}
	                else if(iPlayVelocity == KVelocityPositive)
	                	{
	                	iPlayVelocity = KVelocityBigPositive;
	                	}
                	}
                if (iVideoPlayRateCapabilities.iPlayBackward )
                	{
	                if (iPlayVelocity == KVelocityBigPositive)
	                	{
	                	iPlayVelocity = KVelocityMinNegative;
	                	}
	                else if(iPlayVelocity == KVelocityMinNegative)
	                	{
	                	iPlayVelocity = KVelocityNegative;
	                	}
	                else if(iPlayVelocity == KVelocityNegative)
	                	{
	                	iPlayVelocity = KVelocityBigNegative;
	                	}
	                else if(iPlayVelocity == KVelocityBigNegative)
	                	{
		                iVideoPlayer->Close();
		                CActiveScheduler::Stop();
		                iTestStepResult = EPass;
		                return;
	                	}
                	}

                //Set next velocity
                INFO_PRINTF2(_L("iVideoPlayer->SetPlayVelocityL(%d)"),iPlayVelocity);
                TRAP(err,iVideoPlayer->SetPlayVelocityL(iPlayVelocity));
                if ( err == KErrNotSupported )
                	{
                		INFO_PRINTF1(_L("iVideoPlayer->SetPlayVelocityL returned KErrNotSupported"));
                		if( (iPlayVelocity >0) && iVideoPlayRateCapabilities.iPlayForward)
                			{
                    		INFO_PRINTF1(_L("PlayVelocity and capabilities are valid, still returned KErrNotSupported!!"));
                        	User::Leave(err);
                			}
                		else if( (iPlayVelocity <0) && iVideoPlayRateCapabilities.iPlayBackward)
                			{
                    		INFO_PRINTF1(_L("PlayVelocity and capabilities are valid still returned KErrNotSupported!!"));                			
                        	User::Leave(err);                			
                			}
                		else
                			{
                            // Play The file once again to test with different play velocity.
                            INFO_PRINTF1(_L("iVideoPlayer->Play()"));
                            iVideoPlayer->Play();
                            PrepareState(EVPPlayComplete, KErrNone);             			
                			}
                	}
                else if (err != KErrNone)
                	{
                    INFO_PRINTF2(_L("iVideoPlayer->SetPlayVelocityL returned (%d)"),err);                	
                	User::Leave(err);
                	}
                else //Success case
                	{
                    // Play The file once again to test with different play velocity.
                    INFO_PRINTF1(_L("iVideoPlayer->Play()"));
                    iVideoPlayer->Play();
                    PrepareState(EVPPlayComplete, KErrNone);                	
                	}
                
                if ((!iVideoPlayRateCapabilities.iPlayForward ) && (!iVideoPlayRateCapabilities.iPlayBackward ))
                	{
                    INFO_PRINTF1(_L("Both forward and backward is not supported"));
                    INFO_PRINTF1(_L("Pass the test, error checking for API SetPlayVelocity is already done."));
	                iVideoPlayer->Close();
	                CActiveScheduler::Stop();
	                iTestStepResult = EPass;
	                return;                	
                	}
                break;
            }
        }
    }


//
// RTestVideoPlayStepFrame
//

/**
RTestVideoPlayStepFrame::Constructor
*/
RTestVideoPlayStepFrame::RTestVideoPlayStepFrame(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError)
    : RTestVclntAviPlayerStep(aTestName, aSectName, aKeyName, aExpectedError), iTimer(NULL)
    {
    }

/**
RTestVideoPlayStepFrame::NewL
*/
RTestVideoPlayStepFrame* RTestVideoPlayStepFrame::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError)
	{
    RTestVideoPlayStepFrame* self = new (ELeave) RTestVideoPlayStepFrame(aTestName, aSectName, aKeyName, aExpectedError);
    return self;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         
    }

/**
RTestVideoPlayStepFrame::DoTestStepL()
*/
TVerdict RTestVideoPlayStepFrame::DoTestStepL()
    {
	iTimer = CPeriodic::NewL(CActive::EPriorityHigh);
    // Call the state handler from IDLE state
    TRAPD(err, FsmL(EVPIdle));
    if (err == KErrNone)
        {
        // Start the scheduler - Done only once !
        CActiveScheduler::Start();    
        }
	delete iTimer;    
    return iTestStepResult;
    }

/**
RTestVideoPlayStepFrame::FsmL()
*/
void RTestVideoPlayStepFrame::FsmL(TVclntTestPlayEvents aEventCode)
    {
    if (FsmCheck(aEventCode))
        {
        TInt err = KErrNone;
    	TInt frame = 0xff;
    	TBool pauseSupported = ETrue;    	
        const TUint KStepForwardZeroFrame = 0;            	
        const TUint KStepForwardOneFrame = 1;
        const TInt KStepBackwardOneFrame = -1;
        const TUint KStepForwardFourFrames = 4;
        const TInt KStepBackwardFourFrames = -4;  
        
        switch (aEventCode)
            {
            case EVPIdle:
	            // Calling StepFrameL() before Opeing file should result KErrNotReady.
	            INFO_PRINTF2(_L("iVideoPlayer->StepFrameL(%d)"),1);
	            TRAP(err,iVideoPlayer->StepFrameL(1));
	            if(err != KErrNotReady)
	            	{
	            	INFO_PRINTF2(_L("StepFrameL returned wrong err= %d"), KErrNotReady);
	                iTestStepResult = EFail;
	                CActiveScheduler::Stop();
	                return;	            	
	            	}
            
                // Open iVideoPlayer
                INFO_PRINTF2(_L("iVideoPlayer->OpenFileL() %S"), &iFilename);
                iVideoPlayer->OpenFileL(iFilename);
                PrepareState(EVPOpenComplete, KErrNone);
                break;
            case EVPOpenComplete:
            	//Get playrate capabilities
            	INFO_PRINTF1(_L("iVideoPlayer->GetPlayRateCapabilitiesL()"));            	
                TRAP(err,iVideoPlayer->GetPlayRateCapabilitiesL(iVideoPlayRateCapabilities));
                if( err != KErrNone)
                	{
                	INFO_PRINTF2(_L("GetPlayRateCapabilitiesL returned err= %d"), err);
                    iTestStepResult = EFail;
                    CActiveScheduler::Stop();
                    return;
                	}
                // Calling StepFrameL() in non-pause state should result KErrNotReady.
                INFO_PRINTF2(_L("iVideoPlayer->StepFrameL(%d)"),1);
	            TRAP(err,iVideoPlayer->StepFrameL(1));
	            if(err != KErrNotReady)
	            	{
                	INFO_PRINTF2(_L("StepFrameL returned wrong err= %d"), KErrNotReady);	            	
                    iTestStepResult = EFail;
                    CActiveScheduler::Stop();
                    return;	            	
	            	}
	            	
                // Prepare iVideoPlayer
                INFO_PRINTF1(_L("iVideoPlayer->Prepare()"));
                PrepareState(EVPPrepareComplete, KErrNone);
                iVideoPlayer->Prepare();
                break;
            case EVPPrepareComplete:
	            // Play iVideoPlayer
	            INFO_PRINTF1(_L("iVideoPlayer->Play()"));
	            iVideoPlayer->Play();

                //wait for 1Second to pause the playback.
                INFO_PRINTF1(_L("calling User::After(KOneSecond);"));
                User::After(KOneSecond);
                
            	/* 
				Pause the playback, if pause is not supported wait for play to complete.
				In case of any other err fail the test with appropriate error code.
				*/
			    TRAP(err, iVideoPlayer->PauseL());

			    if( err == KErrNotSupported)
			    	{
			    	pauseSupported = EFalse;			    	
			    	PrepareState(EVPPlayComplete, KErrNone);
			    	}
			    else if( err != KErrNone)
			    	{
				    INFO_PRINTF2(_L("PauseL has returned error =%d"), err);
		            iVideoPlayer->Stop();
		            iVideoPlayer->Close();
		            CActiveScheduler::Stop();
		            iTestStepResult = EFail;			    	
		            return;
			    	}                
			    
			    INFO_PRINTF1(_L("Playback paused"));
         	
            	//Call StepFrameL with different step values.
            	while(1)
            		{
            		if( frame == 0xff)
            			frame = KStepForwardZeroFrame;
            		else if (frame == KStepForwardZeroFrame)
            			frame = KStepForwardOneFrame;
            		else if (frame == KStepForwardOneFrame)
            			frame = KStepForwardFourFrames;
            		else if (frame == KStepForwardFourFrames)
            			frame = KStepBackwardOneFrame;
            		else if (frame == KStepBackwardOneFrame)
            			frame = KStepBackwardFourFrames;
            		else if (frame == KStepBackwardFourFrames)
            			{
                        INFO_PRINTF1(_L("iVideoPlayer->Stop"));
                        iVideoPlayer->Stop();
                        iVideoPlayer->Close();
                        CActiveScheduler::Stop();
                        iTestStepResult = EPass;
            			return; //Break the while loop
            			}
	            	INFO_PRINTF2(_L("iVideoPlayer->StepFrameL(%d)"),frame);
		            TRAP(err,iVideoPlayer->StepFrameL(frame));
		            if(err == KErrNotSupported)
		            	{
		            	if( ((iVideoPlayRateCapabilities.iStepForward) && (frame>0)) ||
	            			((iVideoPlayRateCapabilities.iStepBackward) && (frame<0))   )
		            		{
			            	INFO_PRINTF2(_L("StepFrameL returned wrong err= %d"), err);
			                iTestStepResult = EFail;
			                CActiveScheduler::Stop();
			                return;
		            		}
		            	INFO_PRINTF1(_L("StepFrameL returned KErrNotSupported"));
		            	continue;
		            	}
		            else if( (err == KErrNotReady) && (!pauseSupported) )
		            	{
		            	INFO_PRINTF1(_L("pause is not supported in controller plugin. Returned correct err(KErrNotReady)"));
		            	continue;
		            	}
		            else if( err != KErrNone)
                    	{
                    	INFO_PRINTF2(_L("StepFrameL returned err= %d"), err);
                        iTestStepResult = EFail;
                        CActiveScheduler::Stop();
                        break;
                    	}            		
            		}
                break;
            case EVPPlayComplete:
            	INFO_PRINTF1(_L("Plyback completed before pausing!!"));
            	INFO_PRINTF1(_L("Video clip is too small to run this test step"));
	            INFO_PRINTF1(_L("iVideoPlayer->Stop"));
	            iVideoPlayer->Stop();
	            iVideoPlayer->Close();
	            CActiveScheduler::Stop();
	            iTestStepResult = EInconclusive;
	            return;
            }
        }
    }

//
// RTestVideoPlayAudVidEnable.
//

/**
 * RTestVideoPlayAudVidEnable::Constructor
 */
RTestVideoPlayAudVidEnable::RTestVideoPlayAudVidEnable(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError)
    : RTestVclntAviPlayerStep(aTestName, aSectName, aKeyName, aExpectedError)
    {
    }

/**
 * RTestVideoPlayAudVidEnable::NewL
 */
RTestVideoPlayAudVidEnable* RTestVideoPlayAudVidEnable::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError)
	{
    RTestVideoPlayAudVidEnable* self = new (ELeave) RTestVideoPlayAudVidEnable(aTestName, aSectName, aKeyName, aExpectedError);
    return self;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         
    }

/**
 * RTestVideoPlayAudVidEnable::DoTestStepL()
 */
TVerdict RTestVideoPlayAudVidEnable::DoTestStepL()
    {
    // Call the state handler from IDLE state
    TRAPD(err, FsmL(EVPIdle));
    if (err == KErrNone)
        {
        // Start the scheduler - Done only once !
        CActiveScheduler::Start();    
        }
    return iTestStepResult;  
    }
/**
 * RTestVideoPlayAudVidEnable::FsmL()
 */
void RTestVideoPlayAudVidEnable::FsmL(TVclntTestPlayEvents aEventCode)
    {
    if (FsmCheck(aEventCode))
        {
        TInt err = KErrNone;
    	TBool enableFlag = EFalse;
    	
        switch (aEventCode)
            {
            case EVPIdle:
	            //Calling SetAudioEnabledL() before opening file should return KErrNotReady
	        	INFO_PRINTF1(_L("iVideoPlayer->SetAudioEnabledL()"));            	
	            TRAP(err, iVideoPlayer->SetAudioEnabledL(ETrue));
	            if (err != KErrNotReady)
	            	{
	            	INFO_PRINTF2(_L("SetAudioEnabledL returned wrong error code= %d"), err);
	                iTestStepResult = EFail;
	                CActiveScheduler::Stop();
	                return;
	            	}
	            //Calling SetVideoEnabledL() before opening file should return KErrNotReady
	        	INFO_PRINTF1(_L("iVideoPlayer->SetVideoEnabledL()"));            	
	            TRAP(err, iVideoPlayer->SetVideoEnabledL(ETrue));
	            if (err != KErrNotReady)
	            	{
	            	INFO_PRINTF2(_L("SetVideoEnabledL returned wrong error code= %d"), err);
	                iTestStepResult = EFail;
	                CActiveScheduler::Stop();
	                return;
	            	}
	            //Calling VideoEnabledL() before opening file should return KErrNotReady
	        	INFO_PRINTF1(_L("iVideoPlayer->VideoEnabledL()"));            	
	            TRAP(err, iVideoPlayer->VideoEnabledL());
	            if(err != KErrNotReady)
	            	{
	            	INFO_PRINTF2(_L("VideoEnabledL returned wrong error code= %d"), err);
	                iTestStepResult = EFail;
	                CActiveScheduler::Stop();
	                return;
	            	}
                // Open iVideoPlayer
                INFO_PRINTF2(_L("iVideoPlayer->OpenFileL() %S"), &iFilename);
                iVideoPlayer->OpenFileL(iFilename);
                PrepareState(EVPOpenComplete, KErrNone);
                break;
            case EVPOpenComplete:
            	//Check that by default audio is enabled.
	        	INFO_PRINTF1(_L("iVideoPlayer->AudioEnabledL()"));            	
	            TRAP(err,enableFlag = iVideoPlayer->AudioEnabledL());
	            if( (err != KErrNone) || (enableFlag != (TBool)ETrue) )
	            	{
                	INFO_PRINTF3(_L("AudioEnabledL Leave err= %d, returned:%d"), err,enableFlag);
                    iTestStepResult = EFail;
                    CActiveScheduler::Stop();
                    return;
	            	}

            	//Check that by default video is enabled.
	        	INFO_PRINTF1(_L("iVideoPlayer->VideoEnabledL()"));            	
	            TRAP(err,enableFlag = iVideoPlayer->VideoEnabledL());
	            if( (err != KErrNone) || (enableFlag != (TBool)ETrue) )
	            	{
                	INFO_PRINTF3(_L("VideoEnabledL Leave err= %d, returned:%d"), err,enableFlag);
                    iTestStepResult = EFail;
                    CActiveScheduler::Stop();
                    return;
	            	}
            	
	            //Try to disable audio, It shall leave with notsupported error.
	        	INFO_PRINTF1(_L("iVideoPlayer->SetAudioEnabledL()"));            	
	            TRAP(err, iVideoPlayer->SetAudioEnabledL(ETrue));
	            if (err != KErrNotSupported)
	            	{
                	INFO_PRINTF2(_L("SetAudioEnabledL returned wrong error code= %d"), err);
                    iTestStepResult = EFail;
                    CActiveScheduler::Stop();
                    return;
	            	}

	            //Try to disable video, It shall leave with notsupported error.
	        	INFO_PRINTF1(_L("iVideoPlayer->SetVideoEnabledL()"));            	
	            TRAP(err, iVideoPlayer->SetVideoEnabledL(ETrue));
	            if (err != KErrNotSupported)
	            	{
                	INFO_PRINTF2(_L("SetVideoEnabledL returned wrong error code= %d"), err);
                    iTestStepResult = EFail;
                    CActiveScheduler::Stop();
                    return;
	            	}
	            
                // Prepare iVideoPlayer
                INFO_PRINTF1(_L("iVideoPlayer->Prepare()"));
                PrepareState(EVPPrepareComplete, KErrNone);
                iVideoPlayer->Prepare();
                break;
            case EVPPrepareComplete:
	            //Enabling/disabling audio after prepare() shall return KErrNotSupported error.
	        	INFO_PRINTF1(_L("iVideoPlayer->SetAudioEnabledL()"));            	
	            TRAP(err, iVideoPlayer->SetAudioEnabledL(ETrue));
	            if(err != KErrNotSupported)
	            	{
	            	INFO_PRINTF2(_L("SetAudioEnabledL returned wrong error code= %d"), err);
	                iTestStepResult = EFail;
	                CActiveScheduler::Stop();
	                return;
	            	}

	            //Enabling/disabling video after prepare() shll return KErrNotSupported error.
	        	INFO_PRINTF1(_L("iVideoPlayer->SetVideoEnabledL()"));            	
	            TRAP(err, iVideoPlayer->SetVideoEnabledL(ETrue));
	            if(err != KErrNotSupported)
	            	{
	            	INFO_PRINTF2(_L("SetVideoEnabledL returned wrong error code= %d"), err);
	                iTestStepResult = EFail;
	                CActiveScheduler::Stop();
	                return;
	            	}

                // Play iVideoPlayer
                INFO_PRINTF1(_L("iVideoPlayer->Play()"));
                iVideoPlayer->Play();
                PrepareState(EVPPlayComplete, KErrNone);

                //calling SetAudioEnabledL while playing should return KErrNotReady
	        	INFO_PRINTF1(_L("iVideoPlayer->SetAudioEnabledL()"));            	
	            TRAP(err, iVideoPlayer->SetAudioEnabledL(ETrue));
	            if (err != KErrNotReady)
	            	{
                	INFO_PRINTF2(_L("SetAudioEnabledL returned wrong error code= %d"), err);
                    iTestStepResult = EFail;
                    iVideoPlayer->Stop();
                    iVideoPlayer->Close();                    
                    CActiveScheduler::Stop();
                    return;
	            	}

	            //calling SetVideoEnabledL while playing should return KErrNotReady
	        	INFO_PRINTF1(_L("iVideoPlayer->SetVideoEnabledL()"));            	
	            TRAP(err, iVideoPlayer->SetVideoEnabledL(ETrue));
	            if (err != KErrNotReady)
	            	{
                	INFO_PRINTF2(_L("SetVideoEnabledL returned wrong error code= %d"), err);
                    iTestStepResult = EFail;
                    iVideoPlayer->Stop();
                    iVideoPlayer->Close();                    
                    CActiveScheduler::Stop();
                    return;
	            	}

	            break;
            case EVPPlayComplete:
                INFO_PRINTF1(_L("iVideoPlayer->Stop"));
                iVideoPlayer->Stop();
                iVideoPlayer->Close();
                CActiveScheduler::Stop();
                iTestStepResult = EPass;
                break;
            }
        }
    }


//
// RTestVideoPlayAutoScale.
//

/**
 * RTestVideoPlayAutoScale::Constructor
 */
RTestVideoPlayAutoScale::RTestVideoPlayAutoScale(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError)
    : RTestVclntAviPlayerStep(aTestName, aSectName, aKeyName, aExpectedError)
    {
    }

/**
 * RTestVideoPlayAutoScale::NewL
 */
RTestVideoPlayAutoScale* RTestVideoPlayAutoScale::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError)
	{
    RTestVideoPlayAutoScale* self = new (ELeave) RTestVideoPlayAutoScale(aTestName, aSectName, aKeyName, aExpectedError);
    return self;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         
    }

/**
 * RTestVideoPlayAutoScale::DoTestStepL()
 */
TVerdict RTestVideoPlayAutoScale::DoTestStepL()
    {
    // Call the state handler from IDLE state
    TRAPD(err, FsmL(EVPIdle));
    if (err == KErrNone)
        {
        // Start the scheduler - Done only once !
        CActiveScheduler::Start();    
        }
    return iTestStepResult;  
    }
/**
 * RTestVideoPlayAutoScale::FsmL()
 */
void RTestVideoPlayAutoScale::FsmL(TVclntTestPlayEvents aEventCode)
    {
    if (FsmCheck(aEventCode))
        {
        TInt err = KErrNone;
        TAutoScaleType scaleType = EAutoScaleNone;
        THorizontalAlign horizontalAlign = EHorizontalAlignCenter;
        TVerticalAlign verticalAlign = EVerticalAlignCenter;
        
        switch (aEventCode)
            {
            case EVPIdle:
            	//Calling SetAutoScaleL() before opening file should return KErrNotReady error.
	            INFO_PRINTF1(_L("Calling SetAutoScaleL(EAutoScaleNone);"));
	            TRAP(err, iVideoPlayer->SetAutoScaleL(EAutoScaleNone));
	            if( err != KErrNotReady)
	            	{
	                INFO_PRINTF2(_L("SetAutoScaleL() returned wrong error=%d"),err);
                    CActiveScheduler::Stop();
                    iTestStepResult = EFail;	    	                        
                    return;	                
	            	}
	            
                // Open iVideoPlayer
                INFO_PRINTF2(_L("iVideoPlayer->OpenFileL() %S"), &iFilename);
                iVideoPlayer->OpenFileL(iFilename);
                PrepareState(EVPOpenComplete, KErrNone);
                break;
            case EVPOpenComplete:
                // Prepare iVideoPlayer
                INFO_PRINTF1(_L("iVideoPlayer->Prepare()"));
                PrepareState(EVPPrepareComplete, KErrNone);
                iVideoPlayer->Prepare();
                break;
            case EVPPrepareComplete:
                // Play iVideoPlayer
                INFO_PRINTF1(_L("iVideoPlayer->Play()"));
                iVideoPlayer->Play();
                PrepareState(EVPPlayComplete, KErrNone);

                /*
                Wait for 1/4th of a second each time and call SetAutoScaleL multiple times
                with different scale parameters.
                This loop tests only "void SetAutoScaleL(TAutoScaleType aScaleType);"
                */
               
                while(1)
                	{
    	            switch(scaleType)
		            	{
		            	case EAutoScaleNone:
		            		scaleType = EAutoScaleBestFit;
		            		break;
		            	case EAutoScaleBestFit:
		            		scaleType = EAutoScaleClip;
		            		break;
		            	case EAutoScaleClip:
		            		scaleType = EAutoScaleStretch;
		            		break;
		            	case EAutoScaleStretch:
		            		scaleType = EAutoScaleNone;		            		
		            		break;
		            	}
                    INFO_PRINTF1(_L("calling User::After(KOneSecond/4);"));
                    User::After(KOneSecond/4);
                    
                    INFO_PRINTF1(_L("Calling SetAutoScaleL();"));
    	            TRAP(err, iVideoPlayer->SetAutoScaleL(scaleType));
    	            
#ifdef SYMBIAN_BUILD_GCE
    	            const TInt KExpected = GCEAvailable() ? KErrNone : KErrNotSupported;
#else
					const TInt KExpected = KErrNotSupported;
#endif
    	            
    	            if( err == KExpected)
    	            	{
                        INFO_PRINTF3(_L("SetAutoScaleL(%d) successfully returned %d"), scaleType, err);
    	            	}
    	            else
    	            	{
                        INFO_PRINTF4(_L("SetAutoScaleL(%d) returned error= %d.  Expected %d"),scaleType, err, KExpected);
                        INFO_PRINTF1(_L("iVideoPlayer->Stop"));
                        iVideoPlayer->Stop();
                        iVideoPlayer->Close();
                        CActiveScheduler::Stop();
                        iTestStepResult = EFail;	    	                        
                        return;
    	            	}
    	            if( scaleType == EAutoScaleNone )
    	            	{
    	            	break;//Break the forever while loop;
    	            	}
                	}

                /*
                Wait for half second each time and call SetAutoScaleL multiple times
                with all combinations of TAutoScaleType, THorizontalAlign, and TVerticalAlign.
                */
                
                while(1)
                	{
    	            switch(scaleType)
		            	{
		            	case EAutoScaleNone:
		            		scaleType = EAutoScaleBestFit;
		            		break;
		            	case EAutoScaleBestFit:
		            		scaleType = EAutoScaleClip;
		            		break;
		            	case EAutoScaleClip:
		            		scaleType = EAutoScaleStretch;
		            		break;
		            	case EAutoScaleStretch:
		            		scaleType = EAutoScaleNone;		            		
		            		break;
		            	}
                    while(1)
                    	{
		            	if( horizontalAlign == EHorizontalAlignCenter)
		            		{
		            		horizontalAlign = EHorizontalAlignLeft;
		            		}
		            	else if( horizontalAlign == EHorizontalAlignLeft)
		            		{
		            		horizontalAlign = EHorizontalAlignRight;
		            		}
		            	else if( horizontalAlign == EHorizontalAlignRight)
		            		{
		            		horizontalAlign = EHorizontalAlignCenter;
		            		}
		                while(1)
	                    	{
    		            	if( verticalAlign == EVerticalAlignCenter)
    		            		{
    		            		verticalAlign = EVerticalAlignTop;
    		            		}
    		            	else if( verticalAlign == EVerticalAlignTop)
    		            		{
    		            		verticalAlign = EVerticalAlignBottom;
    		            		}
    		            	else if( verticalAlign == EVerticalAlignBottom)
    		            		{
    		            		verticalAlign = EVerticalAlignCenter;
    		            		}
    		            	
	    		            	INFO_PRINTF1(_L("calling User::After(KOneSecond/4);"));
	    	                    User::After(KOneSecond/4);
								err = KErrNone;
	    	                    
	    	                    INFO_PRINTF1(_L("Calling SetAutoScaleL();"));
	    	    	            TRAP(err, iVideoPlayer->SetAutoScaleL(scaleType,horizontalAlign,verticalAlign));
	    	    	            if( err == KErrNotSupported)
	    	    	            	{
	    	                        INFO_PRINTF4(_L("SetAutoScaleL(%d,%d, %d) returned KErrNotSupported"),scaleType, horizontalAlign,verticalAlign);
	    	    	            	}
    							else if( err == KErrNone)
    	            				{
									INFO_PRINTF1(_L("Success calling SetAutoScaleL()"));
	    	    	            	}
	    	    	            else
	    	    	            	{
	    	                        INFO_PRINTF3(_L("SetAutoScaleL(%d) returned error= %d"),scaleType, err);
	    	                        INFO_PRINTF1(_L("iVideoPlayer->Stop"));
	    	                        iVideoPlayer->Stop();
	    	                        iVideoPlayer->Close();
	    	                        CActiveScheduler::Stop();
	    	                        iTestStepResult = EFail;	    	                        
	    	                        return;	    	                        
	    	    	            	}
    	    	            
    		            	if( verticalAlign == EVerticalAlignCenter )
    	    	            	{
    	    	            	break;//Break the forever while loop;
    	    	            	}
	    		            }    		            	
		            	if( horizontalAlign == EHorizontalAlignCenter )
	    	            	{
	    	            	break;//Break the forever while loop;
	    	            	}    		            	
    		            }
      	            if( scaleType == EAutoScaleNone )
    	            	{
    	            	break;//Break the forever while loop;
    	            	}
                	}
                
                INFO_PRINTF1(_L("iVideoPlayer->Stop"));
                iVideoPlayer->Stop();
                iVideoPlayer->Close();
                CActiveScheduler::Stop();
                iTestStepResult = EPass;
                break;
            case EVPPlayComplete:
            	INFO_PRINTF1(_L("Clip is too small to run the test"));
                INFO_PRINTF1(_L("iVideoPlayer->Stop"));
                iVideoPlayer->Stop();
                iVideoPlayer->Close();
                CActiveScheduler::Stop();
                iTestStepResult = EInconclusive;
                break;
            }
        }
    }
