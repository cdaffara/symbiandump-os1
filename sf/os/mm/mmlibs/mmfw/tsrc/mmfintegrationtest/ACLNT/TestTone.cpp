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
// This program is designed the test of the MMF_ACLNT.
// 
//

/**
 @file TestTone.cpp
*/


#include "TestTone.h"

const TInt KHeapSizeToneTestEKA2 = 128000; // Heapsize for tone tests on EKA2

/**
 * Constructor
 */
CTestMmfAclntTone::CTestMmfAclntTone(const TDesC& aTestName, const TInt aExpectedResult)
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = aTestName;
	
// need a bigger heap size on EKA2 HW
#if !defined __WINS__
	iHeapSize = KHeapSizeToneTestEKA2;
#endif // EKA2

	iFrequency = KToneFrequency;
	iDuration = TTimeIntervalMicroSeconds(KOneSecond);
	iExpectedResult = aExpectedResult;
	iStop = ETrue;
	}

CTestMmfAclntTone* CTestMmfAclntTone::NewL(const TDesC& aTestName, const TInt aExpectedResult)
	{
	CTestMmfAclntTone* self = new (ELeave) CTestMmfAclntTone(aTestName,aExpectedResult);
	return self;
	}

void CTestMmfAclntTone::MatoPrepareComplete(TInt aError)
	{
	iError = aError;
	INFO_PRINTF1( _L("CTestMmfAclntTone::MatoPrepareComplete MMdaAudioToneObserver Callback for CMdaAudioToneUtility complete"));
	INFO_PRINTF2( _L("iError %d"), iError);
	if(iStop)
		CActiveScheduler::Stop();
	}

void CTestMmfAclntTone::MatoPlayComplete(TInt aError)
	{
	iError = aError;
	INFO_PRINTF1( _L("CTestMmfAclntTone::MatoPlayComplete MMdaAudioToneObserver Callback for CMdaAudioToneUtility complete"));
	INFO_PRINTF2( _L("iError %d"), iError);
	if(iStop)
		CActiveScheduler::Stop();
	}

/**
 * Play a tone
 */
TVerdict CTestMmfAclntTone::DoTestStepL()
	{
	INFO_PRINTF1( _L("TestTone : Play"));

	iStop = ETrue;
	TVerdict ret = EFail;
	iError = KErrTimedOut;

	CMdaAudioToneUtility* toneUtil = CMdaAudioToneUtility::NewL(*this);
	CleanupStack::PushL(toneUtil);
	toneUtil->PrepareToPlayTone(iFrequency,iDuration);
	// Wait for prepare
	INFO_PRINTF1( _L("Initialise CMdaAudioToneUtility"));
	CActiveScheduler::Start();

	if(iError == KErrNone)
		ret = DoTestL(toneUtil);

	//produce another tone to confirm that the configuration is retained over multiple plays
	if(iError == KErrNone)
		ret = DoTestL(toneUtil);
	
	if(ret == EFail)
		ERR_PRINTF2( _L("CMdaAudioToneUtility failed with error %d"),iError );

	CleanupStack::PopAndDestroy(toneUtil);
	return	ret;
	}

TVerdict CTestMmfAclntTone::DoTestL(CMdaAudioToneUtility* aToneUtil)
	{
	TVerdict ret = EFail;
	iError = KErrTimedOut;
		
	aToneUtil->Play();
	// wait for play.
	INFO_PRINTF1( _L("Play CMdaAudioToneUtility"));
	CActiveScheduler::Start();

	if(iError == iExpectedResult)
		ret = EPass;
	
	return ret;
	}

//------------------------------------------------------------------

//Play DualTone Test for Sirocco CR
CTestMmfAclntDualTone::CTestMmfAclntDualTone(const TDesC& aTestName) 
	: CTestMmfAclntTone(aTestName) 
	{
	iFreqOne = KToneFrequency;
	iFreqTwo = KToneFrequencyTwo;
	}

CTestMmfAclntDualTone* CTestMmfAclntDualTone::NewL(const TDesC& aTestName)
	{
	CTestMmfAclntDualTone* self = new (ELeave) CTestMmfAclntDualTone(aTestName);
	return self;
	}


/**
 * Play a Dual tone
 */

TVerdict CTestMmfAclntDualTone::DoTestStepL()
	{
	INFO_PRINTF1( _L("TestTone : Play Dual Tone"));

	TVerdict ret = EFail;
	iError = KErrTimedOut;

	CMdaAudioToneUtility* toneUtil = CMdaAudioToneUtility::NewL(*this);
	CleanupStack::PushL(toneUtil);
	toneUtil->PrepareToPlayDualTone(iFreqOne,iFreqTwo,iDuration);
	
	// Wait for prepare
	INFO_PRINTF1( _L("Initialise CMdaAudioToneUtility"));
	CActiveScheduler::Start();

	if(iError == KErrNone)
		ret = DoTestL(toneUtil);

	//produce another tone to confirm that the configuration is retained over multiple plays
	if(iError == KErrNone)
		ret = DoTestL(toneUtil);
	
	if(ret == EFail)
		ERR_PRINTF2( _L("CMdaAudioToneUtility failed with error %d"),iError );

	CleanupStack::PopAndDestroy(toneUtil);
	return	ret;
	}

//------------------------------------------------------------------


CTestMmfAclntToneDtmf::CTestMmfAclntToneDtmf(const TDesC& aTestName,const TDesC& aDTMF,const TInt aExpectedResult)
	: CTestMmfAclntTone(aTestName,aExpectedResult), iDTMF(aDTMF)
	{}

CTestMmfAclntToneDtmf* CTestMmfAclntToneDtmf::NewL(const TDesC& aTestName,const TDesC& aDTMF,const TInt aExpectedResult)
	{
	CTestMmfAclntToneDtmf* self = new (ELeave) CTestMmfAclntToneDtmf(aTestName,aDTMF,aExpectedResult);
	return self;
	}

/**
 * Play a DTMF string
 */
TVerdict CTestMmfAclntToneDtmf::DoTestStepL()
	{
	INFO_PRINTF1( _L("TestTone : Play DTMF"));

	TVerdict ret = EFail;
	iError = KErrTimedOut;

	CMdaAudioToneUtility* toneUtil = CMdaAudioToneUtility::NewL(*this);
	CleanupStack::PushL(toneUtil);
	toneUtil->PrepareToPlayDTMFString(iDTMF);
	// Wait for prepare to complete
	INFO_PRINTF1( _L("Initialise CMdaAudioToneUtility"));
	CActiveScheduler::Start();

	toneUtil->PrepareToPlayDTMFString(iDTMF);
	// Wait for prepare to complete
	INFO_PRINTF1( _L("Reinitialise CMdaAudioToneUtility"));
	CActiveScheduler::Start();

	if(iError == KErrNone)
		ret = DoTestL(toneUtil);
	else if (iError == iExpectedResult)
		{
		INFO_PRINTF2( _L("Initialisation failed as expected with code %d"), iError);
		ret = EPass;
		iError = KErrNone;
		}
		
	if(ret == EFail)
		ERR_PRINTF2( _L("CMdaAudioToneUtility failed with error %d"),iError );

	CleanupStack::PopAndDestroy(toneUtil);
	return ret;
	}

//------------------------------------------------------------------

/**
 * Constructor
 */
CTestMmfAclntToneFile::CTestMmfAclntToneFile(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName)
	: CTestMmfAclntTone(aTestName)
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iSectName = aSectName;
	iKeyName= aKeyName;
	}

CTestMmfAclntToneFile* CTestMmfAclntToneFile::NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName)
	{
	CTestMmfAclntToneFile* self = new (ELeave) CTestMmfAclntToneFile(aTestName,aSectName,aKeyName);
	return self;
	}

TVerdict CTestMmfAclntToneFile::DoTestStepPreambleL()
	{
	TPtrC filename;
	if(!GetStringFromConfig(iSectName, iKeyName, filename))
		return EInconclusive;

	// Create a sequence file
	TInt length;
	RFs fs;

	fs.Connect();
	CleanupClosePushL(fs);
#ifdef __IPC_V2_PRESENT__
	User::LeaveIfError(fs.ShareAuto());
#else
	User::LeaveIfError(fs.Share(RSessionBase::EExplicitAttach));
#endif
	RFile file;
	User::LeaveIfError(file.Replace(fs,filename,EFileWrite));
	CleanupClosePushL(file);
	User::LeaveIfError(file.Write(KFixedSequenceData()));
	User::LeaveIfError(file.Size(length));
	CleanupStack::PopAndDestroy(2, &fs);

	return CTestMmfAclntStep::DoTestStepPreambleL();
	}

/**
 * Play a tone file
 */
TVerdict CTestMmfAclntToneFile::DoTestStepL( void )
	{
	INFO_PRINTF1( _L("TestTone : Play File"));

	TVerdict ret = EFail;


	TPtrC filename;
	if(!GetStringFromConfig(iSectName, iKeyName, filename))
		return EInconclusive;

	iError = KErrTimedOut;
	// perform test using this file
	CMdaAudioToneUtility* toneUtil = CMdaAudioToneUtility::NewL(*this);
	CleanupStack::PushL(toneUtil);
	toneUtil->PrepareToPlayFileSequence(filename);
	// Wait for prepare
	INFO_PRINTF1( _L("Initialise CMdaAudioToneUtility"));
	CActiveScheduler::Start();

	toneUtil->PrepareToPlayFileSequence(filename);
	// Wait for prepare
	INFO_PRINTF1( _L("re-initialise CMdaAudioToneUtility"));
	CActiveScheduler::Start();

	if(iError == KErrNone)
		ret = DoTestL(toneUtil);

	if(ret == EFail)
		ERR_PRINTF2( _L("CMdaAudioToneUtility failed with error %d"),iError );

	CleanupStack::PopAndDestroy(toneUtil);
	return ret;
	}

//------------------------------------------------------------------

/**
 * Constructor
 */
CTestMmfAclntToneDes::CTestMmfAclntToneDes(const TDesC& aTestName, const TDesC8& aDes, const TInt aExpectedResult)
	: CTestMmfAclntTone(aTestName, aExpectedResult), iDes(aDes)
	{}

CTestMmfAclntToneDes* CTestMmfAclntToneDes::NewL(const TDesC& aTestName, const TDesC8& aDes, const TInt aExpectedResult)
	{
	CTestMmfAclntToneDes* self = new (ELeave) CTestMmfAclntToneDes(aTestName,aDes, aExpectedResult);
	return self;
	}

/**
 * Play a tone from a descriptor.
 */
TVerdict CTestMmfAclntToneDes::DoTestStepL()
	{
	INFO_PRINTF1( _L("TestTone : Play Des"));

	TVerdict ret = EFail;
	iError = KErrTimedOut;

	CMdaAudioToneUtility* toneUtil = CMdaAudioToneUtility::NewL(*this);
	CleanupStack::PushL(toneUtil);
	toneUtil->PrepareToPlayDesSequence(iDes);
	// Wait for prepare
	INFO_PRINTF1( _L("Initialise CMdaAudioToneUtility"));
	CActiveScheduler::Start();

	toneUtil->PrepareToPlayDesSequence(iDes);
	// Wait for prepare
	INFO_PRINTF1( _L("re-initialise CMdaAudioToneUtility"));
	CActiveScheduler::Start();

	if(iError == KErrNone)
		ret = DoTestL(toneUtil);

	if(ret == EFail)
		ERR_PRINTF2( _L("CMdaAudioToneUtility failed with error %d"),iError );

	CleanupStack::PopAndDestroy(toneUtil);
	return ret;
	}

//------------------------------------------------------------------

/**
 * Constructor
 */
CTestMmfAclntToneFixed::CTestMmfAclntToneFixed(const TDesC& aTestName,const TInt aTone)
	:CTestMmfAclntTone(aTestName), iTone(aTone)
	{}

CTestMmfAclntToneFixed* CTestMmfAclntToneFixed::NewL(const TDesC& aTestName,const TInt aTone)
	{
	CTestMmfAclntToneFixed* self = new (ELeave) CTestMmfAclntToneFixed(aTestName,aTone);
	return self;
	}

/**
 * Play a predefined/fixed tone
 */
TVerdict CTestMmfAclntToneFixed::DoTestStepL()
	{
	#ifdef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
		INFO_PRINTF1( _L("TestTone : Play Fixed - no longer supported"));

		TVerdict ret = EFail;
		iError = KErrTimedOut;

		CMdaAudioToneUtility* toneUtil = CMdaAudioToneUtility::NewL(*this);
		CleanupStack::PushL(toneUtil);
		toneUtil->PrepareToPlayFixedSequence(iTone);
		// Wait for prepare
		INFO_PRINTF1( _L("Initialise CMdaAudioToneUtility"));
		CActiveScheduler::Start();

		toneUtil->PrepareToPlayFixedSequence(iTone);
		// Wait for prepare
		INFO_PRINTF1( _L("re-initialise CMdaAudioToneUtility"));
		CActiveScheduler::Start();

		// Has Audio Utility intialisied ?
		TInt sequenceCount = toneUtil->FixedSequenceCount();
		INFO_PRINTF2( _L("FixedSequenceCount() returned %d"),sequenceCount);
		if(sequenceCount <= 0)
			{
			INFO_PRINTF1( _L("Play Fixed Sequence is no longer supported"));
			ret = EPass;
			}

		if(ret == EFail)
			ERR_PRINTF2( _L("CMdaAudioToneUtility failed with error %d"),iError );

		CleanupStack::PopAndDestroy(toneUtil);
		return ret;
		
	#else
		INFO_PRINTF1( _L("TestTone : Play Fixed"));

		TVerdict ret = EFail;
		iError = KErrTimedOut;

		CMdaAudioToneUtility* toneUtil = CMdaAudioToneUtility::NewL(*this);
		CleanupStack::PushL(toneUtil);
		toneUtil->PrepareToPlayFixedSequence(iTone);
		// Wait for prepare
		INFO_PRINTF1( _L("Initialise CMdaAudioToneUtility"));
		CActiveScheduler::Start();

		toneUtil->PrepareToPlayFixedSequence(iTone);
		// Wait for prepare
		INFO_PRINTF1( _L("re-initialise CMdaAudioToneUtility"));
		CActiveScheduler::Start();

		// Has Audio Utility intialisied ?
		TInt sequenceCount = toneUtil->FixedSequenceCount() ;
		if((iError == KErrNone) && (sequenceCount > 0))
			{
			INFO_PRINTF2( _L("FixedSequenceName is %S"), &toneUtil->FixedSequenceName(sequenceCount - 1)) ;
			ret = DoTestL(toneUtil);
			}

		if(ret == EFail)
			ERR_PRINTF2( _L("CMdaAudioToneUtility failed with error %d"),iError );

		CleanupStack::PopAndDestroy(toneUtil);
		return ret;
	#endif
	}
//------------------------------------------------------------------

/**
 * Constructor
 */
CTestMmfAclntToneAudio::CTestMmfAclntToneAudio()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACLNT-I-0156-LP");
	
// this test does not inherit from CTestMmfAclntTone
// so we need to make the heap bigger here	
// need a bigger heap size on EKA2 HW
#if !defined __WINS__
	iHeapSize = KHeapSizeToneTestEKA2;
#endif // EKA2
	}

void CTestMmfAclntToneAudio::MatoPrepareComplete(TInt aError)
	{
	iToneError = aError;
	INFO_PRINTF1( _L("CTestMmfAclntToneAudio::MatoPrepareComplete MMdaAudioToneObserver Callback for CMdaAudioToneUtility complete"));
	INFO_PRINTF2( _L("iToneError %d"), iToneError);
	CActiveScheduler::Stop();
	}

void CTestMmfAclntToneAudio::MatoPlayComplete(TInt aError)
	{
	INFO_PRINTF1( _L("CTestMmfAclntToneAudio::MatoPlayComplete MMdaAudioToneObserver Callback for CMdaAudioToneUtility called"));
	iToneError = aError;
	INFO_PRINTF2( _L("iToneError %d"), iToneError);

	if((--iCallbackCount) == 0)
		CActiveScheduler::Stop();
	}

void CTestMmfAclntToneAudio::MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& /*aDuration*/)
	{
	iPlayerError = aError;
	CActiveScheduler::Stop();
	}	

void CTestMmfAclntToneAudio::MapcPlayComplete(TInt aError)
	{
	INFO_PRINTF1( _L("CTestMmfAclntToneAudio::MatcPlayComplete MMdaAudioPlayerCallback for CMdaAudioPlayerUtility called"));
	iPlayerError = aError;
	INFO_PRINTF2( _L("iPlayerError %d"), iPlayerError);

	if((--iCallbackCount) == 0)
		CActiveScheduler::Stop();
	}

//------------------------------------------------------------------

/**
 * Playing a tone and playing an audio file.
 */
TVerdict CTestMmfAclntToneAudio::DoTestStepL( void )
	{
	INFO_PRINTF1( _L("TestTone : Tone/File"));

	TBuf<KSizeBuf>	filename;
	TPtrC			filename1; 
	if(!GetStringFromConfig(_L("SectionOne"), _L("playerAudioFile"), filename1))
		return EInconclusive;

	GetDriveName(filename);
	filename.Append(filename1);
	
	iPlayerError = KErrTimedOut;
	CMdaAudioPlayerUtility* playerUtility = 
		//CMdaAudioPlayerUtility::NewFilePlayerL(filename,*this);//, EPriorityHigh);
		CMdaAudioPlayerUtility::NewFilePlayerL(filename,*this, EPriorityNormal);

	CleanupStack::PushL(playerUtility);
	
	// Wait for prepare
	INFO_PRINTF1( _L("Initialise CMdaAudioPlayerUtility"));
	CActiveScheduler::Start();

	if(iPlayerError == KErrNone)
		{
		iToneError = KErrTimedOut;
		INFO_PRINTF1( _L("Create audio tone utility..."));
		CMdaAudioToneUtility* toneUtil = CMdaAudioToneUtility::NewL(*this);
		CleanupStack::PushL(toneUtil);
		
		INFO_PRINTF1( _L("Prepare to play tone..."));
		toneUtil->PrepareToPlayTone(KToneFrequency,TTimeIntervalMicroSeconds(KOneSecond));
		// Wait for prepare
		INFO_PRINTF1( _L("Initialise CMdaAudioToneUtility"));
		CActiveScheduler::Start();

		INFO_PRINTF1( _L("Re-prepare to play tone..."));
		toneUtil->PrepareToPlayTone(KToneFrequency,TTimeIntervalMicroSeconds(KOneSecond));
		// Wait for prepare
		INFO_PRINTF1( _L("Initialise CMdaAudioToneUtility"));
		CActiveScheduler::Start();

		toneUtil->SetPriority(EPriorityHigh, EMdaPriorityPreferenceNone);
		if(iToneError == KErrNone)
			{
			iPlayerError = KErrTimedOut;
			iToneError = KErrTimedOut;

			// play files together
			INFO_PRINTF1( _L("Play CMdaAudioPlayerUtility"));
			playerUtility->Play();
			INFO_PRINTF1( _L("Play CMdaAudioToneUtility"));
			toneUtil->Play();

			iCallbackCount = 2;
			CActiveScheduler::Start();
			// Wait for two callbacks

			//Tone utility is at priority high - should complete normally
			//Player is at priority normal should complete with KErrInUse
			//KErrAccessDenied is OK here, at least until we can do Mixing
			if(iToneError==KErrNone && (iPlayerError==KErrInUse || iPlayerError==KErrNone))
				{
				CleanupStack::PopAndDestroy(2); // playerUtility, toneUtil
				return EPass;
				}
			}
		CleanupStack::PopAndDestroy(toneUtil);
		}
	else
		INFO_PRINTF1( _L("Cannot initialise CMdaAudioPlayerUtility"));

	ERR_PRINTF2( _L("CMdaAudioToneUtility completed with player error %d"),iPlayerError );
	ERR_PRINTF2( _L("CMdaAudioToneUtility completed with tone error %d"),iToneError );
	CleanupStack::PopAndDestroy(playerUtility);
	return EFail;
	}

//------------------------------------------------------------------

/** Constructor
 */
CTestMmfAclntToneCancelP::CTestMmfAclntToneCancelP(const TDesC& aTestName)
	:CTestMmfAclntTone(aTestName)
	{}

CTestMmfAclntToneCancelP* CTestMmfAclntToneCancelP::NewL(const TDesC& aTestName)
	{
	CTestMmfAclntToneCancelP* self = new (ELeave) CTestMmfAclntToneCancelP(aTestName);
	return self;
	}

/**
 * Cancel tone play
 */
TVerdict CTestMmfAclntToneCancelP::DoTestL(CMdaAudioToneUtility* aToneUtil)
	{
	INFO_PRINTF1( _L("TestTone : Cancel Play"));

	iStop = EFalse;
	TVerdict ret = EFail;

	aToneUtil->Play();
	INFO_PRINTF1( _L("Play CMdaAudioToneUtility"));
	if(aToneUtil->State() == EMdaAudioToneUtilityPlaying)
		{
		// cancel play.
		INFO_PRINTF1( _L("Cancel Play CMdaAudioToneUtility"));
		aToneUtil->CancelPlay();
		// make sure tone is no longer playing
		if(aToneUtil->State() != EMdaAudioToneUtilityPlaying)
			ret = EPass;
		}
	return ret;
	}

//------------------------------------------------------------------
/**
 * Constructor
 */
CTestMmfAclntToneCancelIni::CTestMmfAclntToneCancelIni(const TDesC& aTestName)
	:CTestMmfAclntTone(aTestName)
	{}

CTestMmfAclntToneCancelIni* CTestMmfAclntToneCancelIni::NewL(const TDesC& aTestName)
	{
	CTestMmfAclntToneCancelIni* self = new (ELeave) CTestMmfAclntToneCancelIni(aTestName);
	return self;
	}

/** 
 * Cancel tone prepare.
 * 
 * This fucntion cannot leave
 */
TVerdict CTestMmfAclntToneCancelIni::DoTestStepL()
	{
	INFO_PRINTF1( _L("TestTone : Cancel Prepare"));

	TVerdict ret = EFail;
	iError = KErrTimedOut;

	CMdaAudioToneUtility* toneUtil = CMdaAudioToneUtility::NewL(*this);
	CleanupStack::PushL(toneUtil);
	INFO_PRINTF1( _L("Initialise CMdaAudioToneUtility"));
	toneUtil->PrepareToPlayFixedSequence(0);
	INFO_PRINTF1( _L("Cancel Prep CMdaAudioToneUtility"));
	toneUtil->CancelPrepare();
	// make sure init callback did not complete
	if((iError == KErrTimedOut) && (toneUtil->State() != EMdaAudioToneUtilityPrepared))
		ret = EPass;

	if(ret == EFail)
		ERR_PRINTF2( _L("CMdaAudioToneUtility failed with error %d"),iError );

	CleanupStack::PopAndDestroy(toneUtil);
	return ret;
	}

//------------------------------------------------------------------

/**
 * Constructor
 */
CTestMmfAclntToneNames::CTestMmfAclntToneNames(const TDesC& aTestName)
	:CTestMmfAclntToneFixed(aTestName,1)
	{}

CTestMmfAclntToneNames* CTestMmfAclntToneNames::NewL(const TDesC& aTestName)
	{
	CTestMmfAclntToneNames* self = new (ELeave) CTestMmfAclntToneNames(aTestName);
	return self;
	}

/**
 * Enquire sequence name
 *
 * This function cannot leave
 */
TVerdict CTestMmfAclntToneNames::DoTestL(CMdaAudioToneUtility* aToneUtil)
	{
	INFO_PRINTF1( _L("TestTone : Seq Name"));

	TVerdict ret = EPass;
	TBuf<32> seqName;
	TInt count = aToneUtil->FixedSequenceCount();

	for(TInt i = 0; i < count; i++)
		{
		seqName = aToneUtil->FixedSequenceName(i);
		if(seqName.Length() < 1) 
			{
			ret = EFail;
			break;
			}
		}
	return ret;
	}

//------------------------------------------------------------------

/**
 * Constructor
 */
CTestMmfAclntToneCount::CTestMmfAclntToneCount(const TDesC& aTestName)
	:CTestMmfAclntToneFixed(aTestName,1)
	{}

CTestMmfAclntToneCount* CTestMmfAclntToneCount::NewL(const TDesC& aTestName)
	{
	CTestMmfAclntToneCount* self = new (ELeave) CTestMmfAclntToneCount(aTestName);
	return self;
	}

/**
 * Enquire sequence count
 */
TVerdict CTestMmfAclntToneCount::DoTestL(CMdaAudioToneUtility* aToneUtil)
	{
	INFO_PRINTF1( _L("TestTone : Seq Count"));

	iError = KErrTimedOut;

	if(aToneUtil->FixedSequenceCount() == KFixedSequenceCount)
		return EPass;

	return	EFail ;
	}

//------------------------------------------------------------------

/**
 * Constructor
 */
CTestMmfAclntToneVolume::CTestMmfAclntToneVolume(const TDesC& aTestName,const TInt aVolume)
	:CTestMmfAclntTone(aTestName), iVolume(aVolume)
	{}

CTestMmfAclntToneVolume* CTestMmfAclntToneVolume::NewL(const TDesC& aTestName,const TInt aVolume)
	{
	CTestMmfAclntToneVolume* self = new (ELeave) CTestMmfAclntToneVolume(aTestName,aVolume);
	return self;
	}

/**
 * Set volume to max and enquire volume
 */
TVerdict CTestMmfAclntToneVolume::DoTestL(CMdaAudioToneUtility* aToneUtil)
	{
	INFO_PRINTF1( _L("TestTone : Volume"));

	TVerdict ret = EFail;

	// added from CTestMmfAclntTone : we need the device to be open before
	// SetVolume() will function correctly

	aToneUtil->Play();
	// wait for play.
	INFO_PRINTF1( _L("Play CMdaAudioToneUtility"));

	// don't wait for it to finish - run this test while playing
	// so we know device is open

	// Check maxvolume function
	if(iVolume == -1)
		{
		iVolume = aToneUtil->MaxVolume();
		aToneUtil->SetVolume(iVolume);
		INFO_PRINTF3(_L("volume = %d iVolume = %d"), aToneUtil->Volume(), iVolume); 
		if(aToneUtil->Volume() == iVolume)
			ret = EPass;
		}
	// Volume is truncated to maxvolume
	if(iVolume > aToneUtil->MaxVolume())
		{
		aToneUtil->SetVolume(iVolume);
		INFO_PRINTF3(_L("volume = %d maxVolume = %d"), aToneUtil->Volume(), aToneUtil->MaxVolume()); 
		if(aToneUtil->Volume() == aToneUtil->MaxVolume())
			ret = EPass;
		}
	// Volume is truncated to 0
	else if(iVolume < 0)
		{
		aToneUtil->SetVolume(iVolume);
		INFO_PRINTF2(_L("volume = %d, expecting 0"), aToneUtil->Volume()); 
		if(aToneUtil->Volume() == 0)
			ret = EPass;
		}
	// Set volume and check
	else
		{
		aToneUtil->SetVolume(iVolume);
		INFO_PRINTF3(_L("volume = %d iVolume = %d"), aToneUtil->Volume(), iVolume); 
		if(aToneUtil->Volume() == iVolume)
			ret = EPass;
		}

	// let it finish playing
	CActiveScheduler::Start();

	return ret;
	}


//------------------------------------------------------------------

/**
 * Constructor
 */
CTestMmfAclntVolumeRamp::CTestMmfAclntVolumeRamp(const TDesC& aTestName, const TInt aRamp)
	:CTestMmfAclntTone(aTestName), iRamp(aRamp)
	{}

CTestMmfAclntVolumeRamp* CTestMmfAclntVolumeRamp::NewL(const TDesC& aTestName, const TInt aRamp)
	{
	CTestMmfAclntVolumeRamp* self = new (ELeave) CTestMmfAclntVolumeRamp(aTestName,aRamp);
	return self;
	}

/**
 * Set volume ramp
 * This function cannot leave
 */
TVerdict CTestMmfAclntVolumeRamp::DoTestL(CMdaAudioToneUtility* aToneUtil)
	{

	INFO_PRINTF1( _L("TestTone : Ramp"));

	TTimeIntervalMicroSeconds ramp(iRamp);
	aToneUtil->SetVolumeRamp(ramp);

	// aToneUtil->[Get]VolumeRamp() doesn't exist.
	// For now, if SetVolumeRamp() doesn't panic, we have to return EPass.
	// In future, maybe we can play the clip and get the volume at intervals?
	INFO_PRINTF1(_L("Warning : no API function to get volume ramp"));

	return EPass;
	}

//------------------------------------------------------------------

/**
 * Constructor
 */
CTestMmfAclntToneRepeat::CTestMmfAclntToneRepeat(const TDesC& aTestName)
	:CTestMmfAclntTone(aTestName)
	{}

CTestMmfAclntToneRepeat* CTestMmfAclntToneRepeat::NewL(const TDesC& aTestName)
	{
	CTestMmfAclntToneRepeat* self = new (ELeave) CTestMmfAclntToneRepeat(aTestName);
	return self;
	}

/**
 * Set repeats
 */
TVerdict CTestMmfAclntToneRepeat::DoTestL(CMdaAudioToneUtility* aToneUtil)
	{
	INFO_PRINTF1( _L("TestTone : Repeats"));

	iError = KErrTimedOut;

	TTimeIntervalMicroSeconds silence(0);
	aToneUtil->SetRepeats(NUMBER_OF_REPEATS,silence);

	TInt duration = I64INT(iDuration.Int64());

	iError = KErrTimedOut;
	INFO_PRINTF1( _L("Play CMdaAudioToneUtility"));
	aToneUtil->Play();

	TTime start;
	start.HomeTime();
	CActiveScheduler::Start();
	TTime stop;
	stop.HomeTime();

	TUint actualDuration = I64INT(stop.MicroSecondsFrom(start).Int64());

	INFO_PRINTF2(_L("Repeats : %d"), NUMBER_OF_REPEATS);
	INFO_PRINTF6(_L("Error : %d Start = %d Stop = %d Duration = %d ActualDuration = %d"),
		iError, I64INT(start.Int64()), I64INT(stop.Int64()), duration, actualDuration);

	if((iError == KErrNone) && (TimeComparison(actualDuration, duration * (NUMBER_OF_REPEATS + 1),
											   KExpectedDeviation * NUMBER_OF_REPEATS)))
		return EPass;

	return	EFail ;
	}

//------------------------------------------------------------------

/**
 * Constructor
 */
CTestMmfAclntToneLength::CTestMmfAclntToneLength(const TDesC& aTestName)
	:CTestMmfAclntToneDtmf(aTestName,KShortDTMFString)
	{}

CTestMmfAclntToneLength* CTestMmfAclntToneLength::NewL(const TDesC& aTestName)
	{
	CTestMmfAclntToneLength* self = new (ELeave) CTestMmfAclntToneLength(aTestName);
	return self;
	}

/**
 * Configure tone on length, tone off length, pause length of DTMF Tones
 */
TVerdict CTestMmfAclntToneLength::DoTestL(CMdaAudioToneUtility* aToneUtil)
	{
	INFO_PRINTF1( _L("TestTone : Length"));

	iError = KErrTimedOut;
	TTimeIntervalMicroSeconds32 on(KOneSecond), off(KOneSecond), pause(0);

	aToneUtil->SetDTMFLengths(on, off, pause);

	iError = KErrTimedOut;
	INFO_PRINTF1( _L("Play CMdaAudioToneUtility"));
	aToneUtil->Play();

	TTime start;
	start.HomeTime();
	CActiveScheduler::Start();
	TTime stop;
	stop.HomeTime();

	TUint actualDuration = I64INT(stop.MicroSecondsFrom(start).Int64());

	INFO_PRINTF6(_L("Error : %d Start = %d Stop = %d Duration = %d ActualDuration = %d"),
		iError, I64INT(start.Int64()), I64INT(stop.Int64()), KTwoSeconds, actualDuration);

	if((iError == KErrNone) && (TimeComparison(actualDuration, KTwoSeconds, KExpectedDeviation)))
		return EPass;

	return EFail ;
	}

//------------------------------------------------------------------

/**
 * Constructor
 */
CTestMmfAclntPriorityTones::CTestMmfAclntPriorityTones()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACLNT-I-0165-HP");
	}

void CTestMmfAclntPriorityTones::MchoComplete(TInt aID, TInt aError)
	{
	INFO_PRINTF1( _L("CTestMmfAclntPriorityTones : MMdaAudioToneObserver Callback for CMdaAudioToneUtility called"));
	iError = aError;
	INFO_PRINTF3( _L("iError %d ID %d"), iError, aID);

	if(iFirstCallback == -1)
		iFirstCallback = aID;

	if((--iCallbackCount) == 0)
		CActiveScheduler::Stop();
	}

/**
 * Set priorities of tones
 */
TVerdict CTestMmfAclntPriorityTones::DoTestStepL()
	{
	INFO_PRINTF1( _L("TestTone : Priorities"));

	iError = KErrTimedOut;

    CMdaAudioToneUtility* toneUtil[2];
	CToneCallbackHandler* callback[2];

	for(TInt i=0; i<2; i++)
		{
		callback[i] = new (ELeave) CToneCallbackHandler(i,this);
		CleanupStack::PushL(callback[i]);
		}
	
	toneUtil[0] = CMdaAudioToneUtility::NewL(*callback[0],NULL, -10, EMdaPriorityPreferenceTimeAndQuality);
	CleanupStack::PushL(toneUtil[0]);
	toneUtil[1] = CMdaAudioToneUtility::NewL(*callback[1],NULL,  0, EMdaPriorityPreferenceTimeAndQuality);
	CleanupStack::PushL(toneUtil[1]);

	toneUtil[0]->PrepareToPlayDTMFString(KDTMFString);
	toneUtil[1]->PrepareToPlayDTMFString(KDTMFString);
	
	// wait for initilisation callback
	iCallbackCount = 2;
	INFO_PRINTF1( _L("Initialise CMdaAudioToneUtility"));
	INFO_PRINTF1( _L("Initialise CMdaAudioToneUtility"));
	CActiveScheduler::Start();

	if(iError == KErrNone)
		{
		iError = KErrTimedOut;

		toneUtil[0]->Play();
		toneUtil[1]->Play();

		INFO_PRINTF1( _L("Play CMdaAudioToneUtility"));
		INFO_PRINTF1( _L("Play CMdaAudioToneUtility"));

		// wait for play to complete
		iCallbackCount = 2;
		iFirstCallback = -1;
		CActiveScheduler::Start();

		if((iError == KErrNone) && (iFirstCallback == 0))
			{
			CleanupStack::PopAndDestroy(4);
			return	EPass;
			}
		}

	ERR_PRINTF2( _L("CMdaAudioToneUtility failed with error %d"),iError );

	CleanupStack::PopAndDestroy(4);
	return	EFail ;
	}

//------------------------------------------------------------------

/**
 * Constructor
 */
CTestMmfAclntToneBalance::CTestMmfAclntToneBalance(const TDesC& aTestName,const TInt aBalance)
	:CTestMmfAclntTone(aTestName) ,iBalance(aBalance)
	{}

CTestMmfAclntToneBalance* CTestMmfAclntToneBalance::NewL(const TDesC& aTestName,const TInt aBalance)
	{
	CTestMmfAclntToneBalance* self = new (ELeave) CTestMmfAclntToneBalance(aTestName,aBalance);
	return self;
	}

/**
 * Set balance and enquire balance
 */
TVerdict CTestMmfAclntToneBalance::DoTestL(CMdaAudioToneUtility* aToneUtil)
	{
	INFO_PRINTF1( _L("TestTone : Balance"));

	if (iBalance < KMinBalance)
		{
		TRAPD(err, aToneUtil->SetBalanceL(iBalance));

		if (err != KErrNone)
		{
			INFO_PRINTF2(_L("SetBalanceL() returned unexpected error %d"), err);
			return EFail;
		}

		if(aToneUtil->GetBalanceL() == KMinBalance)
			return EPass;
		}
	else if (iBalance > KMaxBalance)
		{
		TRAPD(err, aToneUtil->SetBalanceL(iBalance));

		if (err != KErrNone)
		{
			INFO_PRINTF2(_L("SetBalanceL() returned unexpected error %d"), err);
			return EFail;
		}

		if(aToneUtil->GetBalanceL() == KMaxBalance)
			return EPass;
		}
	else 
		{
		TRAPD(err, aToneUtil->SetBalanceL(iBalance));

		if (err != KErrNone)
		{
			INFO_PRINTF2(_L("SetBalanceL() returned unexpected error %d"), err);
			return EFail;
		}

		if(aToneUtil->GetBalanceL() == iBalance)
			return EPass;
		}

	return EFail;
	}

//------------------------------------------------------------------

// NEGATIVE TESTING

//------------------------------------------------------------------



/**
 * Constructor
 */
CTestMmfAclntOnOffPause::CTestMmfAclntOnOffPause()
	{
	// store the name of this test case
	// this is the name that is used by the script file
// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACLNT-I-1155-HP");

// this test does not inherit from CTestMmfAclntTone
// so we need to make the heap bigger here	
// need a bigger heap size on EKA2 HW
#if !defined __WINS__	
	iHeapSize = KHeapSizeToneTestEKA2;
#endif // EKA2
	}

void CTestMmfAclntOnOffPause::MatoPrepareComplete(TInt aError)
	{
	iError = aError;
	INFO_PRINTF1( _L("CTestMmfAclntOnOffPause::MatoPrepareComplete MMdaAudioToneObserver Callback for CMdaAudioToneUtility complete"));
	INFO_PRINTF2( _L("iError %d"), iError);
	CActiveScheduler::Stop();
	}

void CTestMmfAclntOnOffPause::MatoPlayComplete(TInt aError)
	{
	iError = aError;
	INFO_PRINTF1( _L("CTestMmfAclntOnOffPause::MatoPlayComplete MMdaAudioToneObserver Callback for CMdaAudioToneUtility complete"));
	INFO_PRINTF2( _L("iError %d"), iError);
	CActiveScheduler::Stop();
	}

/**
 * Set up tone on, off and pause length to illegal values.
 */
TVerdict CTestMmfAclntOnOffPause::DoTestStepL( void )
	{
	INFO_PRINTF1( _L("TestTone : On/Off/Pause"));
	TVerdict res = EFail;

	iError = KErrTimedOut;

	CMdaAudioToneUtility* toneUtil = CMdaAudioToneUtility::NewL(*this);
	CleanupStack::PushL(toneUtil);
	toneUtil->PrepareToPlayDTMFString(KDTMFString);
	// Wait for prepare
	INFO_PRINTF1( _L("Initialise CMdaAudioToneUtility"));
	CActiveScheduler::Start();

	if(iError == KErrNone)
		{
		iError = KErrTimedOut;

		TTimeIntervalMicroSeconds32 on(-4), off(-3), pause(-5);
		
		toneUtil->SetDTMFLengths(on, off, pause);

		toneUtil->Play();
		// wait for play to complete
		INFO_PRINTF1( _L("Play CMdaAudioToneUtility"));
		CActiveScheduler::Start();

		// check this worked
		if(iError == KErrNone)
			res = EPass;
		}

	CleanupStack::Pop(); // toneUtil
	ERR_PRINTF2( _L("CMdaAudioToneUtility completed with error %d"),iError );
	delete toneUtil;
	return	res;
	}

