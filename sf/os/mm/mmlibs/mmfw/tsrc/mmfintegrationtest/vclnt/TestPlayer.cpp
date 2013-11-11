// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "TestPlayer.h"

#include <e32math.h>

/**
 * Constructor
 */
CTestVclntVideoFile::CTestVclntVideoFile(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName,const TBool aPlay)
	:iPlay (aPlay)
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = aTestName;
	iSectName = aSectName;
	iKeyName = aKeyName;

	// expand heap, so we can load 80k video
	iHeapSize = 150000;
	}

CTestVclntVideoFile* CTestVclntVideoFile::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName,const TBool aPlay)
	{
	CTestVclntVideoFile* self = new (ELeave) CTestVclntVideoFile(aTestName,aSectName,aKeyName,aPlay);
	return self;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         
	}

CTestVclntVideoFile* CTestVclntVideoFile::NewLC(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName,const TBool aPlay)
	{
	CTestVclntVideoFile* self = CTestVclntVideoFile::NewLC(aTestName,aSectName,aKeyName,aPlay);
	CleanupStack::PushL(self);
	return self;
	}

void CTestVclntVideoFile::MvpuoOpenComplete(TInt aError)
	{
	iError = aError;
	INFO_PRINTF1(_L("Open Complete callback"));
	CActiveScheduler::Stop();
	}

void CTestVclntVideoFile::MvpuoPrepareComplete(TInt aError)
	{
	iError = aError;
	INFO_PRINTF1(_L("Open Complete callback"));
	CActiveScheduler::Stop();
	}

void CTestVclntVideoFile::MvpuoFrameReady(CFbsBitmap& /*aFrame*/, TInt aError)
	{
	iError = aError;
	INFO_PRINTF1(_L("Frame Ready callback"));
	}

void CTestVclntVideoFile::MvpuoPlayComplete(TInt aError)
	{
	iError = aError;
	INFO_PRINTF1(_L("Play Complete callback"));
	CActiveScheduler::Stop();
	}
	
void CTestVclntVideoFile::MvpuoEvent(const TMMFEvent& /*aEvent*/)
	{
	}


/**
 * Load and initialise an audio file.
 */
TVerdict CTestVclntVideoFile::DoTestStepL()
	{
	TVerdict ret = EFail;
	iError = KErrTimedOut;

	INFO_PRINTF1(_L("Test : Video Player - File"));

	TPtrC filename;
	if(!GetStringFromConfig(iSectName,iKeyName,filename))
		return EInconclusive;

	InitWservL();

	TRect rect, clipRect;
	CVideoPlayerUtility* player = CVideoPlayerUtility::NewL(*this, EMdaPriorityNormal,
															EMdaPriorityPreferenceTimeAndQuality,
															iWs, *iScreen, *iWindow, rect, clipRect);
	CleanupStack::PushL(player);
	TRAP(iError,player->OpenFileL(filename));
	
	if(iError == KErrNone)
		{
		// Wait for initialisation callback
		INFO_PRINTF1(_L("CVideoPlayerUtility: Opening file"));
		CActiveScheduler::Start();
		}

	// Until JetStream
#ifndef JETSTREAM_TESTS
	if(iError == KErrNotSupported)
		ret = EPass;
#endif

	// Check for errors.
	if (iError == KErrNone && player != NULL)
		{
		player->Prepare();
		CActiveScheduler::Start();
		}

	// Check for errors.
	if (iError == KErrNone && player != NULL)
		ret = DoTestL(player);
		
	INFO_PRINTF1(_L("CVideoPlayerUtility: Destroying"));
	CleanupStack::PopAndDestroy(player);
	if(iError != KErrNone)
		ERR_PRINTF2( _L("CVideoPlayerUtility failed with error %d"),iError );
	return	ret;
	}

TVerdict CTestVclntVideoFile::DoTestL(CVideoPlayerUtility* aPlayer)
	{
	TVerdict ret = EFail;

// #ifdef JETSTREAM_TESTS
	if(iPlay)
		{
		iError = KErrTimedOut;
		aPlayer->Play();
		INFO_PRINTF1(_L("CVideoPlayerUtility: Playing file"));
		// Wait for init callback
		CActiveScheduler::Start();
		if(iError == KErrNone)
			ret = EPass;
		}
	else
		ret = EPass;
// #else
//	aPlayer->Stop();
// #endif

	return ret;
	}

//------------------------------------------------------------------

/**
 * Constructor
 */
CTestVclntVideoDes::CTestVclntVideoDes(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName, const TBool aPlay)
	:iPlay (aPlay)

	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = aTestName;
	iSectName = aSectName;
	iKeyName = aKeyName;

	// *** Jim : expand heap, so we can load 80k video
	iHeapSize = 150000;
	}

CTestVclntVideoDes* CTestVclntVideoDes::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName,const TBool aPlay)
	{
	CTestVclntVideoDes* self = new (ELeave) CTestVclntVideoDes(aTestName,aSectName,aKeyName,aPlay);
	return self;
	}

CTestVclntVideoDes* CTestVclntVideoDes::NewLC(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName,const TBool aPlay)
	{
	CTestVclntVideoDes* self = CTestVclntVideoDes::NewLC(aTestName,aSectName,aKeyName,aPlay);
	CleanupStack::PushL(self);
	return self;
	}

void CTestVclntVideoDes::MvpuoOpenComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop();
	}

void CTestVclntVideoDes::MvpuoPrepareComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop();
	}

void CTestVclntVideoDes::MvpuoFrameReady(CFbsBitmap& /*aFrame*/,TInt aError)
	{
	iError = aError;
	// Will this function require a Active Scheduler Stop?
	}

void CTestVclntVideoDes::MvpuoPlayComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop();
	}
	
void CTestVclntVideoDes::MvpuoEvent(const TMMFEvent& /*aEvent*/)
	{
	}


TVerdict  CTestVclntVideoDes::DoTestStepPreambleL()
	{
	TPtrC filename; 
	if(!GetStringFromConfig(iSectName, iKeyName, filename))
		return EInconclusive;

	RFs fs;
	RFile file;
	TInt size = 0;

	// connect to file system and open file
	User::LeaveIfError(fs.Connect());
	User::LeaveIfError(file.Open(fs,filename,EFileRead));
	CleanupClosePushL(file);

	// Set HBuf size
	User::LeaveIfError(file.Size(size));
	INFO_PRINTF2(_L("size of file = %d\n"),size);//Statement Changed under DEF105143

	iVideo = HBufC8::NewMaxL(size);

	// read data into Hbuf
	TPtr8 bufferDes(iVideo->Des());
	User::LeaveIfError(file.Read(bufferDes));
	
	CleanupStack::PopAndDestroy(); //file
	return CTestMmfVclntStep::DoTestStepPreambleL();
	}

TVerdict CTestVclntVideoDes::DoTestStepPostambleL()
	{
	delete iVideo;
	iVideo = NULL;
	return CTestMmfVclntStep::DoTestStepPostambleL();
	}

/**
 * Load and initialise an audio descriptor.
 */
TVerdict CTestVclntVideoDes::DoTestStepL()
	{
	TVerdict ret = EFail;

	INFO_PRINTF1(_L("Test : Video Player - Descriptor"));

	iError = KErrTimedOut;

	InitWservL();

	TRect rect, clipRect;
	CVideoPlayerUtility* player = CVideoPlayerUtility::NewL(*this, EMdaPriorityNormal,
															EMdaPriorityPreferenceTimeAndQuality,
															iWs, *iScreen, *iWindow, rect, clipRect);
	CleanupStack::PushL(player);
	

	TRAP(iError, player->OpenDesL(iVideo->Des()));
	
	if(iError == KErrNone)
		{
		INFO_PRINTF1(_L("CVideoPlayerUtility: Opening descriptor"));
		// Wait for init callback
		CActiveScheduler::Start();
		}

	// Until JetStream
#ifndef JETSTREAM_TESTS
	if(iError == KErrNotSupported)
		ret = EPass;
#endif

	if (iError == KErrNone && player != NULL)
		{
		player->Prepare();
		CActiveScheduler::Start();
		}


	if ((iError == KErrNone) && (player != NULL))
		ret = DoTestL(player);
	
	INFO_PRINTF1(_L("CVideoPlayerUtility: Destroying"));
	CleanupStack::PopAndDestroy(player);
	if(iError != KErrNone)
		ERR_PRINTF2( _L("CVideoPlayerUtility failed with error %d"),iError );
	return	ret;
	}

TVerdict CTestVclntVideoDes::DoTestL(CVideoPlayerUtility* aPlayer)
	{
	TVerdict ret = EFail;
// #ifdef JETSTREAM_TESTS

	if(iPlay)
		{
		iError = KErrTimedOut;
		aPlayer->Play();
		INFO_PRINTF1(_L("CVideoPlayerUtility: Playing descriptor"));
		// Wait for init callback
		CActiveScheduler::Start();
		if(iError == KErrNone)
			ret = EPass;
		}
	else
		ret = EPass;	
//#else
//	aPlayer->Stop();
//#endif
	return ret;		
	}


//------------------------------------------------------------------

/**
 * Constructor
 */
CTestVclntVideoUrl::CTestVclntVideoUrl(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName,const TBool aPlay)
	:iPlay (aPlay)
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = aTestName;
	iSectName = aSectName;
	iKeyName = aKeyName;
	}

CTestVclntVideoUrl* CTestVclntVideoUrl::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName,const TBool aPlay)
	{
	CTestVclntVideoUrl* self = CTestVclntVideoUrl::NewLC(aTestName,aSectName,aKeyName,aPlay);
	CleanupStack::Pop();
	return self;
	}

CTestVclntVideoUrl* CTestVclntVideoUrl::NewLC(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName,const TBool aPlay)
	{
	CTestVclntVideoUrl* self = new (ELeave) CTestVclntVideoUrl(aTestName,aSectName,aKeyName,aPlay);
	CleanupStack::PushL(self);
	return self;
	}

void CTestVclntVideoUrl::MvpuoOpenComplete(TInt aError)
	{
	iError = aError;
	INFO_PRINTF1(_L("Open complete callback"));
	CActiveScheduler::Stop();
	}

void CTestVclntVideoUrl::MvpuoPrepareComplete(TInt aError)
	{
	iError = aError;
	INFO_PRINTF1(_L("Open complete callback"));
	CActiveScheduler::Stop();
	}

void CTestVclntVideoUrl::MvpuoFrameReady(CFbsBitmap& /*aFrame*/, TInt aError)
	{
	iError = aError;
	// ???
	INFO_PRINTF1(_L("Frame Ready callback"));
	}

void CTestVclntVideoUrl::MvpuoPlayComplete(TInt aError)
	{
	iError = aError;
	INFO_PRINTF1(_L("Play complete callback"));
	CActiveScheduler::Stop();
	}
	
void CTestVclntVideoUrl::MvpuoEvent(const TMMFEvent& /*aEvent*/)
	{
	}


/**
 * Open audio from a URL and play.
 */
TVerdict CTestVclntVideoUrl::DoTestStepL()
	{
	TVerdict ret = EFail;

	INFO_PRINTF1(_L("Test : Video Player - URL"));

	iError = KErrTimedOut;	

	TPtrC urlname;
	if(!GetStringFromConfig(iSectName,iKeyName,urlname))
		return EInconclusive;

	InitWservL();

	TRect rect, clipRect;
	CVideoPlayerUtility* player = CVideoPlayerUtility::NewL(*this, EMdaPriorityNormal,
															EMdaPriorityPreferenceTimeAndQuality,
															iWs, *iScreen, *iWindow, rect, clipRect);
	// wait for init to complete
	CleanupStack::PushL(player);
	player->OpenUrlL(urlname);
	INFO_PRINTF1(_L("CVideoPlayerUtility: Opening URL"));
	CActiveScheduler::Start();

	// Until JetStream
#ifndef JETSTREAM_TESTS
	if(iError == KErrNotSupported)
		ret = EPass;
#endif

	if (iError == KErrNone)
		{
		player->Prepare();
		CActiveScheduler::Start();
		}

	if (iError == KErrNone)
		ret = DoTestL(player);

	INFO_PRINTF1(_L("CVideoPlayerUtility: Destroying"));
	CleanupStack::PopAndDestroy(player);
	if(iError != KErrNone)
		ERR_PRINTF2( _L("CVideoPlayerUtility failed with error %d"),iError );
	return	ret;
	}

TVerdict CTestVclntVideoUrl::DoTestL(CVideoPlayerUtility* aPlayer)
	{
	TVerdict ret = EFail;

//#ifdef JETSTREAM_TESTS

	iError = KErrTimedOut;	

	aPlayer->Play();
	INFO_PRINTF1(_L("CVideoPlayerUtility: Playing URL"));
	// Wait for play to complete
	CActiveScheduler::Start();

	if(iError == KErrNone)
		ret = EPass;
//#else
//	aPlayer->Stop();
//#endif

	return ret;
	}


//------------------------------------------------------------------


CTestVclntEnqFrameRate::CTestVclntEnqFrameRate(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, const TInt aFrameRate)
	: CTestVclntVideoFile(aTestName,aSectName,aKeyName,EFalse)
	{
	iFrameRate = aFrameRate;
	}

CTestVclntEnqFrameRate* CTestVclntEnqFrameRate::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, const TInt aFrameRate)
	{
	CTestVclntEnqFrameRate* self = new (ELeave) CTestVclntEnqFrameRate(aTestName,aSectName,aKeyName,aFrameRate);
	return self;
	}

TVerdict CTestVclntEnqFrameRate::DoTestL(CVideoPlayerUtility* aPlayer)
	{
	INFO_PRINTF1(_L("Test : Video Player - Enquire Frame Rate"));

	TVerdict ret = EFail;

	TInt32 theFrameRate;
	Math::Int(theFrameRate, aPlayer->VideoFrameRateL());
	INFO_PRINTF2(_L("(Frame rate : %d)"), theFrameRate); 

	// if we get here, we pass. The player does not have a SetVideoFrameRate()
	ret = EPass;

	return ret;
	}


//------------------------------------------------------------------

CTestVclntPosition::CTestVclntPosition(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, const TTimeIntervalMicroSeconds aPosition)
	: CTestVclntVideoFile(aTestName,aSectName,aKeyName,EFalse)
	{
	iPosition = aPosition;
	}

CTestVclntPosition* CTestVclntPosition::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, const TTimeIntervalMicroSeconds aPosition)
	{
	CTestVclntPosition* self = new (ELeave) CTestVclntPosition(aTestName,aSectName,aKeyName,aPosition);
	return self;
	}

TVerdict CTestVclntPosition::DoTestL(CVideoPlayerUtility* aPlayer)
	{
	// NB test video controller is stubbing certain functions. May have to rewrite this
	// for Jet Stream

	TVerdict ret = EFail;

	INFO_PRINTF1(_L("Test : Video Player - Position"));

	TBool validPosition = EFalse;

	INFO_PRINTF3(_L("Set Position %d Duration = %d"), 
		I64INT(iPosition.Int64()), I64INT(aPlayer->DurationL().Int64()));

	//  Set position: middle of clip.
	if (I64INT(iPosition.Int64()) == 0)
		iPosition = I64INT(aPlayer->DurationL().Int64()) / 2;
	// Set position: end of clip.
	if (I64INT(iPosition.Int64()) == -1)
		iPosition = aPlayer->DurationL();

	// Position is beyond the end of the clips duration
	// so check that the value is clipped.
	if(aPlayer->DurationL() < iPosition)
		{
		INFO_PRINTF2(_L("Longer than duration : Setting position to %d"), I64INT(iPosition.Int64()));
		aPlayer->SetPositionL(iPosition);
		INFO_PRINTF3(_L("Set : position = %d (if clipped : %d)"), 
			I64INT(aPlayer->PositionL().Int64()), I64INT(aPlayer->DurationL().Int64()));
		if(aPlayer->PositionL() == aPlayer->DurationL())
			validPosition = ETrue;
		}
	// Position is negative
	// so check that the value is clipped.
	else if (I64INT(iPosition.Int64()) < 0)
		{
		INFO_PRINTF2(_L("Negative value : Setting position to %d"), I64INT(iPosition.Int64()));
		aPlayer->SetPositionL(iPosition);
		INFO_PRINTF2(_L("Set : position = %d (if clipped : 0)"), I64INT(aPlayer->PositionL().Int64()));
		if (I64INT(aPlayer->PositionL().Int64()) == 0)
			validPosition = ETrue;
		}
	else
		{
		INFO_PRINTF2(_L("Normal : Setting position to %d"), I64INT(iPosition.Int64()));
		aPlayer->SetPositionL(iPosition);
		INFO_PRINTF3(_L("Set : position = %d (if clipped : %d)"), 
			I64INT(aPlayer->PositionL().Int64()), I64INT(iPosition.Int64()));
		if(aPlayer->PositionL() == iPosition)
			validPosition = ETrue;
		}

	// Position was set to a valid value.
	if(validPosition)
		{
		aPlayer->Play();
		INFO_PRINTF1(_L("CVideoPlayerUtility: Playing"));
		// wait for play to complete
		CActiveScheduler::Start();

		INFO_PRINTF2(_L("Error : %d"), iError);
		if (iError == KErrNone)
			ret = EPass;
		}
	else
		{
		// We've got an invalid position, attempt to play anyway...
		// the test controller should handle it
		aPlayer->Play();
		INFO_PRINTF1(_L("CVideoPlayerUtility: Playing from invalid position"));
		// wait for play to complete
		CActiveScheduler::Start();

		INFO_PRINTF2(_L("Error : %d"), iError);
		if (iError == KErrNone)
			ret = EPass;

		}

	return ret;
	}


//------------------------------------------------------------------

CTestVclntPriority::CTestVclntPriority(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, const TInt aPriority)
	: CTestVclntVideoFile(aTestName,aSectName,aKeyName,EFalse)
	{
	iPriority = aPriority;
	}

CTestVclntPriority* CTestVclntPriority::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, const TInt aPriority)
	{
	CTestVclntPriority* self = new (ELeave) CTestVclntPriority(aTestName,aSectName,aKeyName,aPriority);
	return self;
	}

TVerdict CTestVclntPriority::DoTestL(CVideoPlayerUtility* aPlayer)
	{
	TVerdict ret = EFail;

	INFO_PRINTF1(_L("Test : Video Player - Priority"));

	aPlayer->SetPriorityL(iPriority, EMdaPriorityPreferenceNone);	
	TInt thePriority;
	TMdaPriorityPreference thePref;
	aPlayer->PriorityL(thePriority, thePref);
	INFO_PRINTF3(_L("Priority = %d (expecting %d)"), thePriority, iPriority);
	INFO_PRINTF3(_L("Pref = %d (expecting %d)"), thePref, EMdaPriorityPreferenceNone);
	if( (thePriority == iPriority) && (thePref == EMdaPriorityPreferenceNone) )
		return EPass;
	return ret;
	}


//------------------------------------------------------------------

CTestVclntDuration::CTestVclntDuration(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, const TTimeIntervalMicroSeconds aDuration)
	: CTestVclntVideoFile(aTestName,aSectName,aKeyName,EFalse)
	{
	iDuration = aDuration;
	}

CTestVclntDuration* CTestVclntDuration::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, const TTimeIntervalMicroSeconds aDuration)
	{
	CTestVclntDuration* self = new (ELeave) CTestVclntDuration(aTestName,aSectName,aKeyName,aDuration);
	return self;
	}

TVerdict CTestVclntDuration::DoTestL(CVideoPlayerUtility* aPlayer)
	{
	TVerdict ret = EFail;

	INFO_PRINTF1(_L("Test : Video Player - Duration"));

	if (I64INT(iDuration.Int64()) == 0)
		{
		TInt duration = I64INT(aPlayer->DurationL().Int64());
		aPlayer->Play();
		INFO_PRINTF1(_L("CVideoPlayerUtility: Playing"));
		TTime start;
		start.HomeTime();
		CActiveScheduler::Start();
		TTime stop;
		stop.HomeTime();

		TUint actualDuration = I64INT(stop.MicroSecondsFrom(start).Int64());

		INFO_PRINTF6(_L("Error : %d Start = %d Stop = %d Duration = %d ActualDuration = %d"),
			iError, I64INT(start.Int64()), I64INT(stop.Int64()), duration, actualDuration);
		if((iError == KErrNone) && (TimeComparison(actualDuration, duration, KExpectedDeviation)))
			ret = EPass;
		}
	else
		{
		if(aPlayer->DurationL() == iDuration)
			ret = EPass;
		}
	return ret;
	}

//------------------------------------------------------------------

CTestVclntVolume::CTestVclntVolume(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, const TInt aVolume)
	: CTestVclntVideoFile(aTestName,aSectName,aKeyName,EFalse)
	{
	iVolume = aVolume;
	}

CTestVclntVolume* CTestVclntVolume::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, const TInt aVolume)
	{
	CTestVclntVolume* self = new (ELeave) CTestVclntVolume(aTestName,aSectName,aKeyName,aVolume);
	return self;
	}

TVerdict CTestVclntVolume::DoTestL(CVideoPlayerUtility* aPlayer)
	{
	TVerdict ret = EFail;

	// NB : test video controller does not adjust invalid volumes
	// comment the checks back in for JS

	INFO_PRINTF1(_L("Test : Video Player - Volume"));

	INFO_PRINTF1(_L("Warning : test video controller does not adjust invalid volumes"));

	// Check maxvolume function
	if(iVolume == -1)
		{
		iVolume = aPlayer->MaxVolume();
		INFO_PRINTF3(_L("Volume = %d MaxVolume = %d"), iVolume, KMaxVolume);
		//if(iVolume == KMaxVolume) 
			ret = EPass;
		}
	// Volume is truncated to maxvolume
	else if(iVolume > aPlayer->MaxVolume())
		{
		aPlayer->SetVolumeL(iVolume);
		INFO_PRINTF3(_L("Volume = %d MaxVolume = %d"), aPlayer->Volume(), aPlayer->MaxVolume());
		//if(aPlayer->Volume() == aPlayer->MaxVolume())
			ret = EPass;
		}
	// Volume is truncated to 0
	else if(iVolume < 0)
		{
		aPlayer->SetVolumeL(iVolume);
		INFO_PRINTF2(_L("Volume = %d (expecting 0)"), aPlayer->Volume());
		//if(aPlayer->Volume() == 0)
			ret = EPass;
		}
	// Set volume and check
	else
		{
		aPlayer->SetVolumeL(iVolume);
		INFO_PRINTF3(_L("Volume = %d (expecting %d)"), aPlayer->Volume(), iVolume);
		if(aPlayer->Volume() == iVolume)
			ret = EPass;
		}

	return ret;
	}

//------------------------------------------------------------------

CTestVclntCloseOpen::CTestVclntCloseOpen(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName)
	: CTestVclntVideoFile(aTestName,aSectName,aKeyName,EFalse)
	{}

CTestVclntCloseOpen* CTestVclntCloseOpen::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName)
	{
	CTestVclntCloseOpen* self = new (ELeave) CTestVclntCloseOpen(aTestName,aSectName,aKeyName);
	return self;
	}

TVerdict CTestVclntCloseOpen::DoTestL(CVideoPlayerUtility* aPlayer)
	{
	TVerdict ret = EFail;

	INFO_PRINTF1(_L("Test : Video Player - Close/Open"));

//#ifdef JETSTREAM_TESTS

	TPtrC filename;
	if(!GetStringFromConfig(iSectName,iKeyName,filename))
		return EInconclusive;

	aPlayer->Close();
	User::After(KOneSecond);

	TRAP(iError,aPlayer->OpenFileL(filename));
	if(iError == KErrNone)
		{
		// Wait for initialisation callback
		INFO_PRINTF1(_L("CVideoPlayerUtility: Opening file"));
		ret = EPass;
		}

//#else
//	aPlayer->Stop();
//#endif

	return ret;
	}

//------------------------------------------------------------------

CTestVclntPause::CTestVclntPause(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName)
	: CTestVclntVideoFile(aTestName,aSectName,aKeyName,EFalse)
	{}

CTestVclntPause* CTestVclntPause::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName)
	{
	CTestVclntPause* self = new (ELeave) CTestVclntPause(aTestName,aSectName,aKeyName);
	return self;
	}

TVerdict CTestVclntPause::DoTestL(CVideoPlayerUtility* aPlayer)
	{
	TVerdict ret = EFail;

	INFO_PRINTF1(_L("Test : Video Player - Pause"));

	// Tests pause functionality
	// NB : not implemented in test controller
	INFO_PRINTF1(_L("CVideoPlayerUtility: Playing"));
	aPlayer->Play();
	CActiveScheduler::Start();
	aPlayer->PauseL();

#ifdef JETSTREAM_TESTS
	TInt duration = I64INT(aPlayer->DurationL().Int64());

	iError = KErrTimedOut;
	aPlayer->Play();
	INFO_PRINTF1(_L("CVideoPlayerUtility: Playing"));
	TTime start;
	start.HomeTime();
	CActiveScheduler::Start();
	TTime stop;
	stop.HomeTime();

	TUint actualDuration = I64INT(stop.MicroSecondsFrom(start).Int64());

	INFO_PRINTF6(_L("Error : %d Start = %d Stop = %d Duration = %d ActualDuration = %d"),
		iError, I64INT(start.Int64()), I64INT(stop.Int64()), duration, actualDuration);
	if((iError == KErrNone) && (TimeComparison(actualDuration, duration, KExpectedDeviation)))
		ret = EPass;
#else
	INFO_PRINTF1(_L("Warning : PauseL() functionality not supported in test controller"));
	aPlayer->Stop();
	ret = EPass;
#endif

	return ret;
	}

//------------------------------------------------------------------

CTestVclntBalance::CTestVclntBalance(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, const TInt aBalance)
	: CTestVclntVideoFile(aTestName,aSectName,aKeyName,EFalse)
	{
	iBalance = aBalance;
	}

CTestVclntBalance* CTestVclntBalance::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, const TInt aBalance)
	{
	CTestVclntBalance* self = new (ELeave) CTestVclntBalance(aTestName,aSectName,aKeyName,aBalance);
	return self;
	}

TVerdict CTestVclntBalance::DoTestL(CVideoPlayerUtility* aPlayer)
	{
	INFO_PRINTF1(_L("Test : Video Player - Balance"));

#ifdef JETSTREAM_TESTS
	if (iBalance < KMinBalance)
		{
		aPlayer->SetBalanceL(iBalance);		
		INFO_PRINTF3(_L("Balance = %d (expecting %d)"), aPlayer->Balance(), KMinBalance);
		if(aPlayer->Balance() == KMinBalance)
			return EPass;
		}
	else if (iBalance > KMaxBalance)
		{
		aPlayer->SetBalanceL(iBalance);		
		INFO_PRINTF3(_L("Balance = %d (expecting %d)"), aPlayer->Balance(), KMaxBalance);
		if(aPlayer->Balance() == KMaxBalance)
			return EPass;
		}
	else
		{
		aPlayer->SetBalanceL(iBalance);		
		INFO_PRINTF3(_L("Balance = %d (expecting %d)"), aPlayer->Balance(), iBalance);
		if(aPlayer->Balance() == iBalance)
			return EPass;
		}
#else
	// the test video controller returns 1 for all calls of Balance()
	// call SetBalanceL() to show it doesn't leave
	aPlayer->SetBalanceL(iBalance);		
	INFO_PRINTF1(_L("Test controller should return 1 for all calls of Balance()"));
	INFO_PRINTF2(_L("Balance = %d (expecting 1)"), aPlayer->Balance());
	if(aPlayer->Balance() == 1)
		return EPass;
#endif

	return EFail;
	}


//------------------------------------------------------------------

CTestVclntPlayWindow::CTestVclntPlayWindow(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, const TTimeIntervalMicroSeconds aStart, const TTimeIntervalMicroSeconds aEnd)
	: CTestVclntVideoFile(aTestName,aSectName,aKeyName,EFalse)
	{
	iStart = aStart;
	iEnd = aEnd;
	}

CTestVclntPlayWindow* CTestVclntPlayWindow::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, const TTimeIntervalMicroSeconds aStart, const TTimeIntervalMicroSeconds aEnd)
	{
	CTestVclntPlayWindow* self = new (ELeave) CTestVclntPlayWindow(aTestName,aSectName,aKeyName,aStart,aEnd);
	return self;
	}

TVerdict CTestVclntPlayWindow::DoTestL(CVideoPlayerUtility* aPlayer)
	{
	TVerdict ret = EFail;

	INFO_PRINTF1(_L("Test : Video Player - Window"));

//	InitWservL();

	TRect rect, clipRect;
	TRAPD(err, aPlayer->SetDisplayWindowL(iWs, *iScreen, *iWindow, rect, clipRect));
	if(err)
		{
		ERR_PRINTF2(_L("SetDisplayWindowL() failed, error %d"), err);
		ret = EFail;
		}
	else
		{
		ret = EPass;
		}

	// removed an attempt at opening a window server manually - it seemed to work, but a RWsSession
	// couldn't Connect(), connectL() claimed no asynchronous operation...
	
	// we have no window server, so the test can't run - 
	// so call it with dummy parameters and add explanatory note to the test

	// NB ws.Connect() will panic on the target, hence don't run it
/*
#if defined (__WINS__)
	TBool testInvalid = EFalse;
 	RWsSession ws;
	if(ws.Connect() != KErrNone)
		testInvalid = ETrue;

	// we can't call it at all - SetDisplayWindowL() will cause an access violation if ws is null
	if(testInvalid)
		{
		INFO_PRINTF1(_L("Warning : SetDisplayWindowL() cannot be tested, no window server available"));
		ret = EPass;
		}
	else
		{
		// TBD - arguments to be correctly set
		CWsScreenDevice sd;
		RBlankWindow window;
		TRect rect, clipRect;

		TRAPD(err, aPlayer->SetDisplayWindowL(ws, sd, window, rect, clipRect));
		if(err)
			{
				ERR_PRINTF2(_L("SetDisplayWindowL() failed, error %d"), err);
				ret = EFail;
			}
		else
			ret = EPass;
		
		}
#else
	INFO_PRINTF1(_L("Warning : SetDisplayWindowL() cannot be tested on target"));
	ret = EPass;
#endif
*/
	aPlayer->Stop();
	return ret;
	}


//------------------------------------------------------------------

CTestVclntMeta::CTestVclntMeta(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName)
	: CTestVclntVideoFile(aTestName,aSectName,aKeyName,EFalse)
	{}

CTestVclntMeta* CTestVclntMeta::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName)
	{
	CTestVclntMeta* self = new (ELeave) CTestVclntMeta(aTestName,aSectName,aKeyName);
	return self;
	}

TVerdict CTestVclntMeta::DoTestL(CVideoPlayerUtility* aPlayer)
	{
	INFO_PRINTF1(_L("Test : Video Player - Metadata"));

	TInt numOfMeta = -1;
	TRAPD(err, numOfMeta = aPlayer->NumberOfMetaDataEntriesL() );
	INFO_PRINTF3(_L("Error : %d Entries = %d"), err, numOfMeta);
	if (err != KErrNone || numOfMeta != 0)
		return EFail;
	// call that goes to the dummy controller
	// NB we expect zero metadata entries
	CMMFMetaDataEntry* theEntry=NULL;
	TRAP(err, theEntry = aPlayer->MetaDataEntryL(1));
	INFO_PRINTF2(_L("Error : %d"), err);
	if (err != KErrNone)
		return EFail;
	else 
		{
		delete theEntry;
		return EPass;
		}
	}


//------------------------------------------------------------------

CTestVclntFrameSize::CTestVclntFrameSize(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName,const TInt aFrameSize)
	: CTestVclntVideoFile(aTestName,aSectName,aKeyName,EFalse)
	{
	iFrameSize = aFrameSize;
	}

CTestVclntFrameSize* CTestVclntFrameSize::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName,const TInt aFrameSize)
	{
	CTestVclntFrameSize* self = new (ELeave) CTestVclntFrameSize(aTestName,aSectName,aKeyName,aFrameSize);
	return self;
	}

TVerdict CTestVclntFrameSize::DoTestL(CVideoPlayerUtility* aPlayer)
	{
	TVerdict ret = EFail;

	INFO_PRINTF1(_L("Test : Video Player - Frame Size"));

	TSize	frameSize;
	TRAPD(err, aPlayer->VideoFrameSizeL(frameSize) );

	INFO_PRINTF4(_L("Error : %d, Frame size : (%d, %d)"), err, frameSize.iWidth, frameSize.iHeight);
	if (err == KErrNone &&
		frameSize.iWidth >= 0 && frameSize.iHeight >= 0 )  
		return EPass;

	return ret;
	}

//------------------------------------------------------------------

CTestVclntMimeType::CTestVclntMimeType(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName)
	: CTestVclntVideoFile(aTestName,aSectName,aKeyName,EFalse)
	{
	}

CTestVclntMimeType* CTestVclntMimeType::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName)
	{
	CTestVclntMimeType* self = new (ELeave) CTestVclntMimeType(aTestName,aSectName,aKeyName);
	return self;
	}

TVerdict CTestVclntMimeType::DoTestL(CVideoPlayerUtility* aPlayer)
	{
	INFO_PRINTF1(_L("Test : Video Player - MimeType"));

	// NB we cannot check whether the mime type returned is valid, or even exists.
	TPtrC8 mimeType = aPlayer->VideoFormatMimeType();

	// take it to a 16-bit string
	TBuf<64> mimeType16;
	mimeType16.Copy(mimeType);
	INFO_PRINTF2(_L("Mime type : \'%S\'"), &mimeType16);
	return EPass;
	}

//------------------------------------------------------------------

CTestVclntRebuffering::CTestVclntRebuffering(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName)
	: CTestVclntVideoFile(aTestName,aSectName,aKeyName,EFalse)
	{
	}

CTestVclntRebuffering* CTestVclntRebuffering::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName)
	{
	CTestVclntRebuffering* self = new (ELeave) CTestVclntRebuffering(aTestName,aSectName,aKeyName);
	return self;
	}

TVerdict CTestVclntRebuffering::DoTestL(CVideoPlayerUtility* aPlayer)
	{
	INFO_PRINTF1(_L("Test : Video Player - Rebuffering"));

	aPlayer->RegisterForVideoLoadingNotification(*this);

	INFO_PRINTF1(_L("Rebuffering request completed"));
	return EPass;
	}

// from MRebufferCallback
void CTestVclntRebuffering::MvloLoadingStarted()
	{
	INFO_PRINTF1(_L("CTestVclntRebuffering::MvloLoadingStarted"));
	}

void CTestVclntRebuffering::MvloLoadingComplete()
	{
	INFO_PRINTF1(_L("CTestVclntRebuffering::MvloLoadingComplete"));
	}

//------------------------------------------------------------------

CTestVclntRepeat::CTestVclntRepeat(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName,const TInt aRepeat)
	: CTestVclntVideoFile(aTestName,aSectName,aKeyName,EFalse)
	{
	iRepeat = aRepeat;
	}

CTestVclntRepeat* CTestVclntRepeat::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName,const TInt aRepeat)
	{
	CTestVclntRepeat* self = new (ELeave) CTestVclntRepeat(aTestName,aSectName,aKeyName,aRepeat);
	return self;
	}

TVerdict CTestVclntRepeat::DoTestL(CVideoPlayerUtility* aPlayer)
	{
	TVerdict ret = EFail;

	INFO_PRINTF1(_L("Test : Video Player - Repeats"));

	TInt duration = I64INT(aPlayer->DurationL().Int64());

	// SetRepeats() doesn't exist in the video controller, so set iRepeat to 1
	iRepeat = 1;
	INFO_PRINTF1(_L("Warning : SetRepeats() does not exist in player API. Repeat count set to 1"));
	
	aPlayer->Play();
	INFO_PRINTF1(_L("CVideoPlayerUtility: Playing"));
	TTime start;
	start.HomeTime();
	CActiveScheduler::Start();
	TTime stop;
	stop.HomeTime();

	TUint actualDuration = I64INT(stop.MicroSecondsFrom(start).Int64());

	INFO_PRINTF6(_L("Error : %d Start = %d Stop = %d Duration = %d ActualDuration = %d"),
		iError, I64INT(start.Int64()), I64INT(stop.Int64()), duration, actualDuration);
	if((iError == KErrNone) && (TimeComparison(actualDuration, duration * (iRepeat + 1), 
											   KExpectedDeviation * iRepeat)))
		ret = EPass;

	return ret;
	}

//------------------------------------------------------------------

CTestVclntDelete::CTestVclntDelete(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName)
	: CTestVclntVideoFile(aTestName,aSectName,aKeyName,EFalse)
	{}

CTestVclntDelete* CTestVclntDelete::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName)
	{
	CTestVclntDelete* self = new (ELeave) CTestVclntDelete(aTestName,aSectName,aKeyName);
	return self;
	}

TVerdict CTestVclntDelete::DoTestL(CVideoPlayerUtility* /* aPlayer */)
	{
	// this is never called
	return EFail;
	}

TVerdict CTestVclntDelete::DoTestStepL()
	{
	TVerdict ret = EFail;

	INFO_PRINTF1(_L("Test : Video Player - Delete"));

	TPtrC filename;
	if(!GetStringFromConfig(iSectName,iKeyName,filename))
		return EInconclusive;

	InitWservL();

	TRect rect, clipRect;
	CVideoPlayerUtility* player = CVideoPlayerUtility::NewL(*this, EMdaPriorityNormal,
															EMdaPriorityPreferenceTimeAndQuality,
															iWs, *iScreen, *iWindow, rect, clipRect);
	CleanupStack::PushL(player);

	iError = KErrTimedOut;
	INFO_PRINTF2(_L("Attempting to open file %S"), &filename);
	TRAP(iError,player->OpenFileL(filename));
	
	if(iError == KErrNone)
		{
		// Wait for initialisation callback
		INFO_PRINTF1(_L("CVideoPlayerUtility: Opening file"));
		CActiveScheduler::Start();
		}

	if (iError == KErrNone)
		{
		player->Prepare();
		CActiveScheduler::Start();
		}

	if(iError == KErrNone)
		{
		player->Play();
		INFO_PRINTF1(_L("CVideoPlayerUtility: Playing"));
		INFO_PRINTF1(_L("CVideoPlayerUtility: Destroying before play complete"));
		CleanupStack::Pop(player); // destroy player before play has completed

		User::Heap().Check();
		return EPass;
		}

	CleanupStack::Pop(player);
	ERR_PRINTF2( _L("CVideoPlayerUtility failed with error %d"),iError );
	return ret;
	}

