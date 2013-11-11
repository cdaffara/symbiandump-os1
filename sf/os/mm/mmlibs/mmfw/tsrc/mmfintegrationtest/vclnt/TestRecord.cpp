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


#include "TestRecord.h"

const TInt KBufMimeTypeGranularity = 4;

/**
 * Constructor
 */
CTestVclntRecordVideoFile::CTestVclntRecordVideoFile(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName,const TBool aRec)
	:iRec (aRec)
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = aTestName;
	iSectName = aSectName;
	iKeyName = aKeyName;
	}

CTestVclntRecordVideoFile* CTestVclntRecordVideoFile::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName,const TBool aRec)
	{
	CTestVclntRecordVideoFile* self = new (ELeave) CTestVclntRecordVideoFile(aTestName,aSectName,aKeyName,aRec);
	return self;
	}

CTestVclntRecordVideoFile* CTestVclntRecordVideoFile::NewLC(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName,const TBool aRec)
	{
	CTestVclntRecordVideoFile* self = CTestVclntRecordVideoFile::NewL(aTestName,aSectName,aKeyName,aRec);
	CleanupStack::PushL(self);
	return self;
	}

void CTestVclntRecordVideoFile::MvruoOpenComplete(TInt aError)
	{
	iError = aError;
	INFO_PRINTF1(_L("Open complete callback"));
	CActiveScheduler::Stop();
	}

void CTestVclntRecordVideoFile::MvruoRecordComplete(TInt aError)
	{
	iError = aError;
	INFO_PRINTF1(_L("Record complete callback"));
	CActiveScheduler::Stop();
	}

void CTestVclntRecordVideoFile::MvruoPrepareComplete(TInt aError)
	{
	iError = aError;
	INFO_PRINTF1(_L("Prepare complete callback"));
	CActiveScheduler::Stop();
	}
	
void CTestVclntRecordVideoFile::MvruoEvent(const TMMFEvent& /*aEvent*/)
	{
	}

/**
 * Load and initialise an audio file.
 */
TVerdict CTestVclntRecordVideoFile::DoTestStepL()
	{
	TVerdict ret = EFail;

	INFO_PRINTF1(_L("Test : Video Recorder - File"));

	iError = KErrTimedOut;

	TPtrC filename;
	if(!GetStringFromConfig(iSectName,iKeyName,filename))
		return EInconclusive;

	CVideoRecorderUtility* rec = CVideoRecorderUtility::NewL(*this);
	CleanupStack::PushL(rec);
	// *** Jim
	const TUid KVidTstControllerUid = {KMmfVideoTestControllerUid};
	TRAP(iError,rec->OpenFileL(filename, NULL, KVidTstControllerUid, KUidMdaBmpClipFormat));

	if(iError == KErrNone)
		{
		INFO_PRINTF1(_L("CVideoRecorderUtility: Open file"));
		// Wait for initialisation callback
		CActiveScheduler::Start();
		}

	// Until JETSTREAM_TESTS
#ifndef JETSTREAM_TESTS
	if(iError == KErrNotSupported)
		ret = EPass;
#endif

	// Check for errors.
	if ((iError == KErrNone) && (rec != NULL))
		ret = DoTestL(rec);
	
	INFO_PRINTF1(_L("CVideoRecorderUtility: Destroy"));
	CleanupStack::PopAndDestroy(rec);
	User::After(KOneSecond); // wait for deletion to shut down devsound
	if(iError != KErrNone)
		ERR_PRINTF2( _L("CVideoRecorderUtility failed with error %d"),iError );
	return	ret;
	}

TVerdict CTestVclntRecordVideoFile::DoTestL(CVideoRecorderUtility* aRec)
	{
	TVerdict ret = EFail;

	aRec->Prepare();
	CActiveScheduler::Start();
	if(iError != KErrNone)
		{
		INFO_PRINTF2(_L("Prepare callback : error %d"), iError);
		return EInconclusive;
		}

	if(iRec)
		{
		iError = KErrTimedOut;
		aRec->Record();
		INFO_PRINTF1(_L("CVideoRecorderUtility: Record"));
		// Wait for init callback
		CActiveScheduler::Start();
		if(iError == KErrNone)
			ret = EPass;
		}
	else
		ret = EPass;			

	return ret;
	}

//------------------------------------------------------------------

/**
 * Constructor
 */
CTestVclntRecorderVideoDes::CTestVclntRecorderVideoDes(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName, const TBool aRec)
	:iRec (aRec)

	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = aTestName;
	iSectName = aSectName;
	iKeyName = aKeyName;
	iHeapSize = 0xFFFFF;
	}

CTestVclntRecorderVideoDes* CTestVclntRecorderVideoDes::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName,const TBool aRec)
	{
	CTestVclntRecorderVideoDes* self = new (ELeave) CTestVclntRecorderVideoDes(aTestName,aSectName,aKeyName,aRec);
	return self;
	}

CTestVclntRecorderVideoDes* CTestVclntRecorderVideoDes::NewLC(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName,const TBool aRec)
	{
	CTestVclntRecorderVideoDes* self = CTestVclntRecorderVideoDes::NewL(aTestName,aSectName,aKeyName,aRec);
	CleanupStack::PushL(self);
	return self;
	}

void CTestVclntRecorderVideoDes::MvruoOpenComplete(TInt aError)
	{
	iError = aError;
	INFO_PRINTF1(_L("Open complete"));
	CActiveScheduler::Stop();
	}

void CTestVclntRecorderVideoDes::MvruoRecordComplete(TInt aError)
	{
	iError = aError;
	INFO_PRINTF1(_L("Record complete"));
	CActiveScheduler::Stop();
	}
	
void CTestVclntRecorderVideoDes::MvruoEvent(const TMMFEvent& /*aEvent*/)
	{
	}

void CTestVclntRecorderVideoDes::MvruoPrepareComplete(TInt aError)
	{
	iError = aError;
	INFO_PRINTF1(_L("Prepare complete callback"));
	CActiveScheduler::Stop();
	}

TVerdict  CTestVclntRecorderVideoDes::DoTestStepPreambleL()
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

TVerdict CTestVclntRecorderVideoDes::DoTestStepPostambleL()
	{
	delete iVideo;
	iVideo = NULL;
	return CTestMmfVclntStep::DoTestStepPostambleL();
	}

/**
 * Load and initialise an audio descriptor.
 */
TVerdict CTestVclntRecorderVideoDes::DoTestStepL()
	{
	TVerdict ret = EFail;

	INFO_PRINTF1(_L("Test : Video Recorder - Descriptor"));

	iError = KErrTimedOut;

	CVideoRecorderUtility * rec = NULL;
	rec = CVideoRecorderUtility::NewL(*this);
	CleanupStack::PushL(rec);
	TPtr8 bufferDes(iVideo->Des());
	// *** Jim
	const TUid KVidTstControllerUid = {KMmfVideoTestControllerUid};
	TRAP(iError,rec->OpenDesL(bufferDes, NULL, KVidTstControllerUid, KUidMdaBmpClipFormat));
		
	if(iError == KErrNone)
		{
		// Wait for init callback
		INFO_PRINTF1(_L("CVideoRecorderUtility: Open descriptor"));
		CActiveScheduler::Start();
		}

	// Until JETSTREAM_TESTS
#ifndef JETSTREAM_TESTS
	if(iError == KErrNotSupported)
	{
		INFO_PRINTF2( _L("CVideoRecorderUtility failed with expected error %d"),iError );
		ret = EPass;
	}
#endif

	if (iError == KErrNone && rec != NULL)
		ret = DoTestL(rec);
		
	INFO_PRINTF1(_L("CVideoRecorderUtility: Destroy"));
	CleanupStack::PopAndDestroy(rec);
	User::After(KOneSecond);
	if(iError != KErrNone)
		ERR_PRINTF2( _L("CMdaVideoRecorderUtility failed with error %d"),iError );
	return	ret;
	}

TVerdict CTestVclntRecorderVideoDes::DoTestL(CVideoRecorderUtility* aRec)
	{
	TVerdict ret = EFail;

	aRec->Prepare();
	CActiveScheduler::Start();
	if(iError != KErrNone)
		{
		INFO_PRINTF2(_L("Prepare callback : error %d"), iError);
		return EInconclusive;
		}

	if(iRec)
		{
		iError = KErrTimedOut;
		aRec->Record();
		// Wait for init callback
		INFO_PRINTF1(_L("CVideoRecorderUtility: Record"));
		CActiveScheduler::Start();
		if(iError == KErrNone)
			ret = EPass;
		}
	else
		ret = EPass;	

	return ret;		
	}

//------------------------------------------------------------------

/**
 * Constructor
 */
CTestVclntRecorderVideoUrl::CTestVclntRecorderVideoUrl(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName, const TBool aRec)
	:iRec (aRec)
	{
	iTestStepName = aTestName;
	iSectName = aSectName;
	iKeyName = aKeyName;
	}

CTestVclntRecorderVideoUrl* CTestVclntRecorderVideoUrl::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName,const TBool aRec)
	{
	CTestVclntRecorderVideoUrl* self = new (ELeave) CTestVclntRecorderVideoUrl(aTestName,aSectName,aKeyName,aRec);
	return self;
	}

CTestVclntRecorderVideoUrl* CTestVclntRecorderVideoUrl::NewLC(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName,const TBool aRec)
	{
	CTestVclntRecorderVideoUrl* self = CTestVclntRecorderVideoUrl::NewL(aTestName,aSectName,aKeyName,aRec);
	CleanupStack::PushL(self);
	return self;
	}

void CTestVclntRecorderVideoUrl::MvruoOpenComplete(TInt aError)
	{
	iError = aError;
	INFO_PRINTF1(_L("Open complete"));
	CActiveScheduler::Stop();
	}

void CTestVclntRecorderVideoUrl::MvruoRecordComplete(TInt aError)
	{
	iError = aError;
	INFO_PRINTF1(_L("Record complete"));
	CActiveScheduler::Stop();
	}

void CTestVclntRecorderVideoUrl::MvruoPrepareComplete(TInt aError)
	{
	iError = aError;
	INFO_PRINTF1(_L("Prepare complete callback"));
	CActiveScheduler::Stop();
	}
	
void CTestVclntRecorderVideoUrl::MvruoEvent(const TMMFEvent& /*aEvent*/)
	{
	}


/**
 * Load and initialise an audio descriptor.
 */
TVerdict CTestVclntRecorderVideoUrl::DoTestStepL()
	{
	TVerdict ret = EFail;

	INFO_PRINTF1(_L("Test : Video Recorder - URL"));

	iError = KErrTimedOut;

	TPtrC filename;
	if(!GetStringFromConfig(iSectName,iKeyName,filename))
		return EInconclusive;

	CVideoRecorderUtility* rec = CVideoRecorderUtility::NewL(*this);
	CleanupStack::PushL(rec);
	
	const TUid KVidTstControllerUid = {KMmfVideoTestControllerUid};
	TRAP(iError,rec->OpenUrlL(filename, KUseDefaultIap, NULL, KVidTstControllerUid, KUidMdaBmpClipFormat));

	if(iError == KErrNone)
		{
		INFO_PRINTF1(_L("CVideoRecorderUtility: Open URL"));
		// Wait for initialisation callback
		CActiveScheduler::Start();
		}

	// Until JETSTREAM_TESTS
#ifndef JETSTREAM_TESTS
	if(iError == KErrNotSupported)
	{
		INFO_PRINTF2( _L("CVideoRecorderUtility failed with expected error %d"),iError );
		ret = EPass;
	}
#endif

	// Check for errors.
	if ((iError == KErrNone) && (rec != NULL))
		ret = DoTestL(rec);
	
	INFO_PRINTF1(_L("CVideoRecorderUtility: Destroy"));
	CleanupStack::PopAndDestroy(rec);
	User::After(KOneSecond); // wait for deletion to shut down devsound
	if(iError != KErrNone)
		ERR_PRINTF2( _L("CVideoRecorderUtility failed with error %d"),iError );
	return	ret;
	}

TVerdict CTestVclntRecorderVideoUrl::DoTestL(CVideoRecorderUtility* aRec)
	{
	TVerdict ret = EFail;

	aRec->Prepare();
	CActiveScheduler::Start();
	if(iError != KErrNone)
		{
		INFO_PRINTF2(_L("Prepare callback : error %d"), iError);
		return EInconclusive;
		}

	if(iRec)
		{
		iError = KErrTimedOut;
		aRec->Record();
		// Wait for init callback
		INFO_PRINTF1(_L("CVideoRecorderUtility: Record"));
		CActiveScheduler::Start();
		if(iError == KErrNone)
			ret = EPass;
		}
	else
		ret = EPass;	

	return ret;		
	}

//------------------------------------------------------------------

/**
 * Constructor
 */
CTestVclntRecordConfig::CTestVclntRecordConfig(const TDesC& aTestName)
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = aTestName;
	}

CTestVclntRecordConfig* CTestVclntRecordConfig::NewL(const TDesC& aTestName)
	{
	CTestVclntRecordConfig* self = new (ELeave) CTestVclntRecordConfig(aTestName);
	return self;
	}

CTestVclntRecordConfig* CTestVclntRecordConfig::NewLC(const TDesC& aTestName)
	{
	CTestVclntRecordConfig* self = CTestVclntRecordConfig::NewLC(aTestName);
	CleanupStack::PushL(self);
	return self;
	}

void CTestVclntRecordConfig::MvruoOpenComplete(TInt aError)
	{
	iError = aError;
	INFO_PRINTF1(_L("Open complete callback"));
	CActiveScheduler::Stop();
	}

void CTestVclntRecordConfig::MvruoRecordComplete(TInt aError)
	{
	iError = aError;
	INFO_PRINTF1(_L("Record complete callback"));
	CActiveScheduler::Stop();
	}

void CTestVclntRecordConfig::MvruoPrepareComplete(TInt aError)
	{
	iError = aError;
	INFO_PRINTF1(_L("Prepare complete callback"));
	CActiveScheduler::Stop();
	}
	
void CTestVclntRecordConfig::MvruoEvent(const TMMFEvent& /*aEvent*/)
	{
	}


/**
 * Load and initialise an audio file.
 */
TVerdict CTestVclntRecordConfig::DoTestStepL()
	{
	TVerdict ret = EFail;

	INFO_PRINTF1(_L("Test : Video Recorder - Config"));

	iError = KErrTimedOut;

	// Functions to test - now tested in CTestVclntRecordTypes where appropriate
/*	virtual void SetAudioCodecL(TFourCC aCodec)=0;
	virtual void SetVideoCodecL(TFourCC aCodec)=0;
	virtual void GetSupportedAudioCodecsL(RArray<TFourCC>& aAudioCodecs) const=0;
	virtual void GetSupportedVideoCodecsL(RArray<TFourCC>& aVideoCodecs) const=0;
	virtual void SetVideoFrameRateL(TReal32 aFrameRate)=0;
	virtual TReal32 VideoFrameRateL() const=0;
	virtual void SetVideoFrameSizeL(const TSize& aSize)=0;
	virtual void GetVideoFrameSizeL(TSize& aSize) const=0;
	virtual void SetVideoBitRateL(TInt aBitRate)=0;
	virtual TInt VideoBitRateL()=0;
	virtual void SetAudioBitRateL(TInt aBitRate)=0;
	virtual TInt AudioBitRateL() const=0;
	virtual void SetAudioEnabledL(TBool aEnabled)=0;
*/

	TPtrC filename;
	if(!GetStringFromConfig(iSectName,iKeyName,filename))
		return EInconclusive;

	CVideoRecorderUtility* rec = CVideoRecorderUtility::NewL(*this);
	CleanupStack::PushL(rec);
	const TUid KVidTstControllerUid = {KMmfVideoTestControllerUid};
	rec->OpenFileL(filename,  NULL, KVidTstControllerUid, KUidMdaBmpClipFormat);
	// Wait for initialisation callback
	CActiveScheduler::Start();

	if(iError != KErrNone)
		{
		ERR_PRINTF2( _L("CVideoRecorderUtility::OpenFileL failed with error %d"),iError );
		return EFail;
		}

	ret = DoTestL(rec);
	
	INFO_PRINTF1(_L("CVideoRecorderUtility: Destroy"));
	CleanupStack::PopAndDestroy(rec);
	User::After(KOneSecond); // wait for deletion to shut down devsound
	if(iError != KErrNone)
		ERR_PRINTF2( _L("CVideoRecorderUtility::Destroy failed with error %d"),iError );

	return ret;
	}

TVerdict CTestVclntRecordConfig::DoTestL(CVideoRecorderUtility* aRec)
	{
	TVerdict ret = EFail;

	aRec->Prepare();
	CActiveScheduler::Start();
	if(iError != KErrNone)
		{
		INFO_PRINTF2(_L("Prepare callback : error %d"), iError);
		return EInconclusive;
		}

	iError = KErrTimedOut;
	aRec->Record();
	// Wait for init callback
	CActiveScheduler::Start();
	if(iError == KErrNone)
		ret = EPass;

	return ret;
	}


//------------------------------------------------------------------

CTestVclntRecordCloseOpen::CTestVclntRecordCloseOpen(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName)
	: CTestVclntRecordVideoFile(aTestName,aSectName,aKeyName,EFalse)
	{}

CTestVclntRecordCloseOpen* CTestVclntRecordCloseOpen::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName)
	{
	CTestVclntRecordCloseOpen* self = new (ELeave) CTestVclntRecordCloseOpen(aTestName,aSectName,aKeyName);
	return self;
	}

TVerdict CTestVclntRecordCloseOpen::DoTestL(CVideoRecorderUtility* aRec)
	{
	TVerdict ret = EFail;

	INFO_PRINTF1(_L("Test : Video Recorder - Close/Open"));

	TPtrC filename;
	if(!GetStringFromConfig(iSectName,iKeyName,filename))
		return EInconclusive;

	aRec->Close();
	INFO_PRINTF1(_L("CVideoRecorderUtility: Close file"));
	User::After(KOneSecond);

	const TUid KVidTstControllerUid = {KMmfVideoTestControllerUid};
	aRec->OpenFileL(filename,  NULL, KVidTstControllerUid, KUidMdaBmpClipFormat);
	INFO_PRINTF1(_L("CVideoRecorderUtility: Open file"));
	CActiveScheduler::Start();
	if(iError == KErrNone)
		ret = EPass;
//#else
//	aRec->Stop();
//#endif

	return ret;
	}

//------------------------------------------------------------------

CTestVclntRecordPause::CTestVclntRecordPause(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName)
	: CTestVclntRecordVideoFile(aTestName,aSectName,aKeyName,EFalse)
	{}

CTestVclntRecordPause* CTestVclntRecordPause::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName)
	{
	CTestVclntRecordPause* self = new (ELeave) CTestVclntRecordPause(aTestName,aSectName,aKeyName);
	return self;
	}

TVerdict CTestVclntRecordPause::DoTestL(CVideoRecorderUtility* aRec)
	{
	TVerdict ret = EFail;

	INFO_PRINTF1(_L("Test : Video Recorder - Pause"));

	TPtrC filename;
	if(!GetStringFromConfig(iSectName,iKeyName,filename))
		return EInconclusive;
	
	aRec->Prepare();
	CActiveScheduler::Start();
	if(iError != KErrNone)
		{
		INFO_PRINTF2(_L("Prepare callback : error %d"), iError);
		return EInconclusive;
		}

	// Tests pause functionality
	// NB : not implemented in test controller
	INFO_PRINTF1(_L("CVideoRecorderUtility: Record"));
	aRec->Record();
	CActiveScheduler::Start();
	aRec->PauseL();

#ifdef JETSTREAM_TESTS
	TInt duration = I64INT(aRec->DurationL().Int64());

	iError = KErrTimedOut;
	aRec->Record();
	INFO_PRINTF1(_L("CVideoRecorderUtility: Record"));
	TTime start;
	start.HomeTime();
	CActiveScheduler::Start();
	TTime stop;
	stop.HomeTime();

	TUint actualDuration = I64INT(stop.MicroSecondsFrom(start).Int64());

	INFO_PRINTF6(_L("Error : %d Start = %d Stop = %d Duration = %d ActualDuration = %d"),
		iError, I64INT(start.Int64()), I64INT(stop.Int64()), duration, actualDuration);
	INFO_PRINTF5(_L("Error : %d Start = %d Stop = %d ActualDuration = %d"),
		iError, start, stop, actualDuration);
	if((iError == KErrNone) && (TimeComparison(actualDuration, duration, KExpectedDeviation)))
		ret = EPass;
#else
	INFO_PRINTF1(_L("Warning : PauseL() functionality not supported in test controller"));
	aRec->Stop();
	ret = EPass;
#endif

	return ret;
	}

//------------------------------------------------------------------

CTestVclntRecordDuration::CTestVclntRecordDuration(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, const TTimeIntervalMicroSeconds aDuration)
	: CTestVclntRecordVideoFile(aTestName,aSectName,aKeyName,EFalse)
	{
	iDuration = aDuration;
	}

CTestVclntRecordDuration* CTestVclntRecordDuration::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, const TTimeIntervalMicroSeconds aDuration)
	{
	CTestVclntRecordDuration* self = new (ELeave) CTestVclntRecordDuration(aTestName,aSectName,aKeyName,aDuration);
	return self;
	}

TVerdict CTestVclntRecordDuration::DoTestL(CVideoRecorderUtility* aRec)
	{
	TVerdict ret = EFail;

	INFO_PRINTF1(_L("Test : Video Recorder - Duration"));

	aRec->Prepare();
	CActiveScheduler::Start();
	if(iError != KErrNone)
		{
		INFO_PRINTF2(_L("Prepare callback : error %d"), iError);
		return EInconclusive;
		}

	if (I64INT(iDuration.Int64()) == 0)
		{
		TInt duration = I64INT(aRec->DurationL().Int64());
		aRec->Record();
		INFO_PRINTF1(_L("CVideoRecorderUtility: Record"));
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
		if(aRec->DurationL() == iDuration)
			ret = EPass;
		}

	return ret;
	}

//------------------------------------------------------------------

CTestVclntRecordTime::CTestVclntRecordTime(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, const TInt aTime)
	: CTestVclntRecordVideoFile(aTestName,aSectName,aKeyName,EFalse)
	{
	iTime = aTime;	
	}

CTestVclntRecordTime* CTestVclntRecordTime::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, const TInt aTime)
	{
	CTestVclntRecordTime* self = new (ELeave) CTestVclntRecordTime(aTestName,aSectName,aKeyName, aTime);
	return self;
	}

TVerdict CTestVclntRecordTime::DoTestL(CVideoRecorderUtility* aRec)
	{
	TVerdict ret = EPass;
	TTimeIntervalMicroSeconds recTime;

	INFO_PRINTF1(_L("Test : Video Recorder - RecordTimeAvailable()"));

	recTime = aRec->RecordTimeAvailable();

	// if the test controller is working correctly then the value KMMFVideoRecordTimeAvailable
	// will be returned

	TTimeIntervalMicroSeconds refTime (KMMFVideoRecordTimeAvailable);
	if (recTime != refTime)
		ret = EFail;
		
	return ret;
	}

//------------------------------------------------------------------

CTestVclntRecordCrop::CTestVclntRecordCrop(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName,const TBool aToEnd)
	: CTestVclntRecordVideoFile(aTestName,aSectName,aKeyName,EFalse)
	{
	iToEnd = aToEnd;
	}

CTestVclntRecordCrop* CTestVclntRecordCrop::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, const TBool aToEnd)
	{
	CTestVclntRecordCrop* self = new (ELeave) CTestVclntRecordCrop(aTestName,aSectName,aKeyName, aToEnd);
	return self;
	}

TVerdict CTestVclntRecordCrop::DoTestL(CVideoRecorderUtility* /* aRec */)
	{
	INFO_PRINTF1(_L("Test : Video Recorder - Crop"));

	// should be descoped...
	INFO_PRINTF1(_L("Note : CropL() does not exist in recorder API"));
	return EPass;
	}

//------------------------------------------------------------------

CTestVclntRecordGain::CTestVclntRecordGain(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, const TInt aGain)
	: CTestVclntRecordVideoFile(aTestName,aSectName,aKeyName,EFalse)
	{
	iGain = aGain;
	}

CTestVclntRecordGain* CTestVclntRecordGain::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, const TInt aGain)
	{
	CTestVclntRecordGain* self = new (ELeave) CTestVclntRecordGain(aTestName,aSectName,aKeyName, aGain);
	return self;
	}

TVerdict CTestVclntRecordGain::DoTestL(CVideoRecorderUtility* aRec)
	{
	INFO_PRINTF1(_L("Test : Video Recorder - Gain"));

	TVerdict ret = EFail;

	if(iGain == 0)
		iGain = aRec->MaxGainL();
	aRec->SetGainL(iGain);
	if(iGain == aRec->GainL())
		return EPass;
	return ret;
	}

//------------------------------------------------------------------


CTestVclntRecordMeta::CTestVclntRecordMeta(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName)
	: CTestVclntRecordVideoFile(aTestName,aSectName,aKeyName,EFalse)
	{}

CTestVclntRecordMeta* CTestVclntRecordMeta::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName)
	{
	CTestVclntRecordMeta* self = new (ELeave) CTestVclntRecordMeta(aTestName,aSectName,aKeyName);
	return self;
	}

TVerdict CTestVclntRecordMeta::DoTestL(CVideoRecorderUtility* aRec)
	{
	INFO_PRINTF1(_L("Test : Video Recorder - Metadata"));

	TInt numEntries = 0;

	TRAPD(err, numEntries = aRec->NumberOfMetaDataEntriesL() );
	INFO_PRINTF3(_L("Num entries : Error : %d Entries = %d"), err, numEntries);
	if (err != KErrNone || numEntries != 0)
		return EFail;

	// call that goes to the dummy controller
	// NB we expect zero metadata entries
	CMMFMetaDataEntry* theEntry=NULL;
	TRAP(err, theEntry = aRec->MetaDataEntryL(1));

	INFO_PRINTF2(_L("Get entry : Error : %d"), err);
	if (err != KErrNone)
		return EFail;
	else
		{
		delete theEntry;
		}

	_LIT(name,"<Copyright>");
	_LIT(data,"<Symbian (c) 2002>");

	// make entries
	CMMFMetaDataEntry* metaData = NULL;
	metaData = CMMFMetaDataEntry::NewL(name,data);
	CleanupStack::PushL(metaData);

	TRAP(err, aRec->AddMetaDataEntryL(*metaData));	// not supported in test controller
	if(err != KErrNotSupported)
		return EFail;

	TRAP(err, aRec->RemoveMetaDataEntryL(0));	// not supported in test controller
	if(err != KErrNotSupported)
		return EFail;

	TRAP(err, aRec->ReplaceMetaDataEntryL(0, *metaData));	// not supported in test controller
	if(err != KErrNotSupported)
		return EFail;

	CleanupStack::PopAndDestroy(metaData);
	
	return EPass;
	}


//------------------------------------------------------------------

CTestVclntRecordPriority::CTestVclntRecordPriority(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, const TInt aPriority)
	: CTestVclntRecordVideoFile(aTestName,aSectName,aKeyName,EFalse)
	{
	iPriority = aPriority;
	}

CTestVclntRecordPriority* CTestVclntRecordPriority::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, const TInt aPriority)
	{
	CTestVclntRecordPriority* self = new (ELeave) CTestVclntRecordPriority(aTestName,aSectName,aKeyName,aPriority);
	return self;
	}

TVerdict CTestVclntRecordPriority::DoTestL(CVideoRecorderUtility* aRec)
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
		return EPass;
	return ret;
	}


//------------------------------------------------------------------

CTestVclntRecordBalance::CTestVclntRecordBalance(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, const TInt aBalance)
	: CTestVclntRecordVideoFile(aTestName,aSectName,aKeyName,EFalse)
	{
	iBalance = aBalance;
	}

CTestVclntRecordBalance* CTestVclntRecordBalance::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, const TInt aBalance)
	{
	CTestVclntRecordBalance* self = new (ELeave) CTestVclntRecordBalance(aTestName,aSectName,aKeyName, aBalance);
	return self;
	}

TVerdict CTestVclntRecordBalance::DoTestL(CVideoRecorderUtility* /* aRec */)
	{
	INFO_PRINTF1(_L("Test : Video Recorder - Balance"));

	// should be descoped...
	INFO_PRINTF1(_L("Note : Set/GetBalance does not exist in recorder API"));
	return EPass;
	};

//------------------------------------------------------------------


CTestVclntRecordSize::CTestVclntRecordSize(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName)
	: CTestVclntRecordVideoFile(aTestName,aSectName,aKeyName,EFalse)
	{}

CTestVclntRecordSize* CTestVclntRecordSize::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName)
	{
	CTestVclntRecordSize* self = new (ELeave) CTestVclntRecordSize(aTestName,aSectName,aKeyName);
	return self;
	}

TVerdict CTestVclntRecordSize::DoTestL(CVideoRecorderUtility* aRec)
	{
	INFO_PRINTF1(_L("Test : Video Recorder - SetMaxClipSize"));

	// call max clip size with 'no max clip size' value
	aRec->SetMaxClipSizeL( KMMFNoMaxClipSize );

	// if this call doesn't leave then test has passed

	return EPass;
	}

//------------------------------------------------------------------


CTestVclntRecordTypes::CTestVclntRecordTypes(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName)
	: CTestVclntRecordVideoFile(aTestName,aSectName,aKeyName,EFalse)
	{}

CTestVclntRecordTypes* CTestVclntRecordTypes::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName)
	{
	CTestVclntRecordTypes* self = new (ELeave) CTestVclntRecordTypes(aTestName,aSectName,aKeyName);
	return self;
	}

TVerdict CTestVclntRecordTypes::DoTestL(CVideoRecorderUtility* aRec)
	{
	TInt cumulErr = 0;
	TInt i;
	INFO_PRINTF1(_L("Test : Video Recorder - Audio/Video Types"));

	RArray<TFourCC> theAudioTypes;
	TRAPD(err, aRec->GetSupportedAudioTypesL(theAudioTypes));
	INFO_PRINTF2(_L("GetSupportedAudioTypesL returned %d"), err);
	if(err != KErrNone && err != KErrNotSupported)
		cumulErr++;

	CDesC8ArrayFlat* theVideoTypes = new (ELeave) CDesC8ArrayFlat(KBufMimeTypeGranularity);
	CleanupStack::PushL(theVideoTypes);
	TRAP(err, aRec->GetSupportedVideoTypesL(*theVideoTypes));
	INFO_PRINTF2(_L("GetSupportedVideoTypesL returned %d"), err);
	if(err != KErrNone && err != KErrNotSupported)
		cumulErr++;
	else
		{
		for (i = 0; i < theVideoTypes->Count(); i++)
			{
			TBuf<64> temp;
			temp.Copy((*theVideoTypes)[i]);
			INFO_PRINTF2(_L("Mime Type:  %S"), &temp);	
			}
		}
	CleanupStack::PopAndDestroy(theVideoTypes);

	TFourCC audType;
	TRAP(err, aRec->SetAudioTypeL(audType));
	INFO_PRINTF2(_L("SetAudioTypeL returned %d"), err);
	if(err != KErrNone && err != KErrNotSupported)
		cumulErr++;

	_LIT8(KVidType, "x-video");
	TRAP(err, aRec->SetVideoTypeL(KVidType));
	INFO_PRINTF2(_L("SetVideoTypeL returned %d"), err);
	if(err != KErrNone && err != KErrNotSupported)
		cumulErr++;

	TBool enabled = ETrue;
	TRAP(err, aRec->SetAudioEnabledL(enabled));
	INFO_PRINTF2(_L("SetAudioEnabledL returned %d"), err);
	if(err != KErrNone && err != KErrNotSupported)
		cumulErr++;

	TBool checkEnabled = EFalse;  // Ensure AudioEnabled is Set Correctly (INC022977)
	TRAP(err, checkEnabled = aRec->AudioEnabledL());
	if (!checkEnabled)
		cumulErr++;
    
    // Added to print the displayname
    TBuf<80> theControllerImplName;
    
	// enhanced - to use CMMFControllerPluginSelectionParameters
	// this will satisfy REQ172.16.1.1, REQ172.2.1.1

	INFO_PRINTF1(_L("Querying record formats..."));

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
			theControllerImplName = controllers[i]->DisplayName();
			INFO_PRINTF3(_L("controllers[%d] -> DisplayName is %S"), i, &theControllerImplName);
			RMMFFormatImplInfoArray cFormats = controllers[i]->RecordFormats();
			// NB : a controller may have no formats
			if(cFormats.Count() == 0)
			    {
			    INFO_PRINTF2(_L("- Controller: %d supports no record formats"), i);
			    }
			else
				{
				const CDesC8Array* fileExtensions = &cFormats[0]->SupportedMimeTypes();
				INFO_PRINTF3(_L("- Controller: %d Extensions : %d"), i, fileExtensions->Count());
				for(int j = 0; j < fileExtensions->Count(); j++)
					{
					TBuf<24> ext16;
					ext16.Copy(fileExtensions->MdcaPoint(j));
					INFO_PRINTF2(_L("-- %S"), &ext16);
					}
				}
			}
		}

	CleanupStack::PopAndDestroy(3);//controllers, fSelect, cSelect

	INFO_PRINTF1(_L("Querying play formats..."));

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
			theControllerImplName = controllers[i]->DisplayName();
			INFO_PRINTF3(_L("controllers[%d] -> DisplayName is %S"), i, &theControllerImplName);
			RMMFFormatImplInfoArray cFormats = controllers[i]->PlayFormats();
			if (cFormats.Count() == 0)
			    {
			    INFO_PRINTF2(_L("- Controller: %d supports no play formats"), i);
			    }
			else
			    {
			    const CDesC8Array* fileExtensions = &cFormats[0]->SupportedMimeTypes();
    			INFO_PRINTF3(_L("- Controller: %d Extensions : %d"), i, fileExtensions->Count());
    			for(int j = 0; j < fileExtensions->Count(); j++)
    				{
    				TBuf<24> ext16;
    				ext16.Copy(fileExtensions->MdcaPoint(j));
    				INFO_PRINTF2(_L("-- %S"), &ext16);
    				}
			    }
			}
		}

	CleanupStack::PopAndDestroy(3);//controllers, fSelect, cSelect

	return (cumulErr ? EFail : EPass);
	}

//------------------------------------------------------------------

CTestVclntRecordDelete::CTestVclntRecordDelete(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName)
	: CTestVclntRecordVideoFile(aTestName,aSectName,aKeyName,EFalse)
	{}

CTestVclntRecordDelete* CTestVclntRecordDelete::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName)
	{
	CTestVclntRecordDelete* self = new (ELeave) CTestVclntRecordDelete(aTestName,aSectName,aKeyName);
	return self;
	}

TVerdict CTestVclntRecordDelete::DoTestL(CVideoRecorderUtility* /* aRec */)
	{
	// this is never called
	return EFail;
	}

TVerdict CTestVclntRecordDelete::DoTestStepL()
	{
	INFO_PRINTF1(_L("Test : Video Recorder - Delete"));

	iError = KErrTimedOut;

	TPtrC filename;
	if(!GetStringFromConfig(iSectName,iKeyName,filename))
		return EInconclusive;

	CVideoRecorderUtility* rec = CVideoRecorderUtility::NewL(*this);
	CleanupStack::PushL(rec);
	const TUid KVidTstControllerUid = {KMmfVideoTestControllerUid};
	rec->OpenFileL(filename, NULL, KVidTstControllerUid, KUidMdaBmpClipFormat);
	INFO_PRINTF1(_L("CVideoRecorderUtility: Open file"));
	// wait
	CActiveScheduler::Start();

	rec->Prepare();
	CActiveScheduler::Start();
	if(iError != KErrNone)
		{
		INFO_PRINTF2(_L("Prepare callback : error %d"), iError);
		return EInconclusive;
		}

	INFO_PRINTF1(_L("CVideoRecorderUtility: Record"));
	rec->Record();
	INFO_PRINTF1(_L("CVideoRecorderUtility: Destroy"));
	CleanupStack::PopAndDestroy(rec);
	User::Heap().Check();

	return EPass;
	}
