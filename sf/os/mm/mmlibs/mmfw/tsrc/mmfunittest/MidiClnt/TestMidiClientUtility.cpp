
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
//

#include "TestMidiClientUtility.h"
#include <e32math.h>


CTestMidiClntOpenFile::CTestMidiClntOpenFile(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName,const TBool aPlay)
	:CTestMmfMidiClntStep(aTestName, ETestValid),
	 iPlay(aPlay)
	{
	iSectName = aSectName;
	iKeyName = aKeyName;
	}

CTestMidiClntOpenFile* CTestMidiClntOpenFile::NewL(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName,const TBool aPlay)
	{
	CTestMidiClntOpenFile* self = new(ELeave) CTestMidiClntOpenFile(aTestName, aSectName, aKeyName, aPlay);
	return self;
	}

TVerdict CTestMidiClntOpenFile::DoTestStepL()
	{
	TPtrC filename;
	if(!GetStringFromConfig(iSectName,iKeyName,filename))
		return EInconclusive;

	CMidiClientUtility* player = CMidiClientUtility::NewL(*this, EMdaPriorityNormal, EMdaPriorityPreferenceTimeAndQuality);
	if (!player)
		{
		ERR_PRINTF1(_L("Could not create a CMidiClientUtility"));
		return EInconclusive;
		}
	CleanupStack::PushL(player);

	TMMFMessageDestinationPckg dummyPckg;
	TInt dummyFunc = 0; //EDevMidiOff;
	TBuf8<8> dummyBuff;
	player->CustomCommandSyncL(dummyPckg, dummyFunc, dummyBuff, dummyBuff, dummyBuff);

	player->OpenFile(filename);
	
	// Wait for initialisation callback
	INFO_PRINTF1(_L("CMidiClientUtility: Opening file"));
	CActiveScheduler::Start();

	TVerdict ret = EFail;

	// Check for errors.
	if (iError == KErrNone)
		ret = DoTestL(player);
		
	INFO_PRINTF1(_L("CMidiClientUtility: Destroying"));
	CleanupStack::PopAndDestroy(player);

	if(iError != KErrNone)
		ERR_PRINTF2( _L("CMidiClientUtility failed with error %d"),iError );
	
	return	ret;
	}

TVerdict CTestMidiClntOpenFile::DoTestL(CMidiClientUtility* /*aMidi*/)
	{
	return EPass;
	}

//------------------------------------------------------------------

CTestMidiClntOpenDes::CTestMidiClntOpenDes(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName,const TBool aPlay)
	:CTestMmfMidiClntStep(aTestName, ETestValid),
	 iPlay(aPlay)
	{
	iSectName = aSectName;
	iKeyName = aKeyName;
	}

CTestMidiClntOpenDes* CTestMidiClntOpenDes::NewL(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName,const TBool aPlay)
	{
	CTestMidiClntOpenDes* self = new(ELeave) CTestMidiClntOpenDes(aTestName, aSectName, aKeyName, aPlay);
	return self;
	}

TVerdict  CTestMidiClntOpenDes::DoTestStepPreambleL()
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
	INFO_PRINTF2(_L("size of file = %d\n"),size);

	iAudio = HBufC8::NewMaxL(size);

	// read data into Hbuf
	TPtr8 bufferDes(iAudio->Des());
	User::LeaveIfError(file.Read(bufferDes));
	
	CleanupStack::PopAndDestroy(); //file
	return CTestMmfMidiClntStep::DoTestStepPreambleL();
	}

TVerdict CTestMidiClntOpenDes::DoTestStepPostambleL()
	{
	delete iAudio;
	iAudio = NULL;
	return CTestMmfMidiClntStep::DoTestStepPostambleL();
	}

TVerdict CTestMidiClntOpenDes::DoTestStepL()
	{
	CMidiClientUtility* player = CMidiClientUtility::NewL(*this, EMdaPriorityNormal, EMdaPriorityPreferenceTimeAndQuality);
	if (!player)
		{
		ERR_PRINTF1(_L("Could not create a CMidiClientUtility"));
		return EInconclusive;
		}
	
	CleanupStack::PushL(player);

	TMMFMessageDestinationPckg dummyPckg;
	TInt dummyFunc = EDevMidiOff;
	TBuf8<8> dummyBuff;
	player->CustomCommandSyncL(dummyPckg, dummyFunc, dummyBuff, dummyBuff, dummyBuff);

	player->OpenDes(iAudio->Des());
	
	// Wait for initialisation callback
	INFO_PRINTF1(_L("CMidiClientUtility: Opening file"));
	CActiveScheduler::Start();

	TVerdict ret = EFail;

	// Check for errors.
	if (iError == KErrNone)
		ret = DoTestL(player);
		
	INFO_PRINTF1(_L("CMidiClientUtility: Destroying"));
	CleanupStack::PopAndDestroy(player);

	if(iError != KErrNone)
		ERR_PRINTF2( _L("CMidiClientUtility failed with error %d"),iError );
	
	return	ret;
	}

TVerdict CTestMidiClntOpenDes::DoTestL(CMidiClientUtility* /*aMidi*/)
	{
	return EPass;
	}

//------------------------------------------------------------------

CTestMidiClntOpenUrl::CTestMidiClntOpenUrl(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName,const TBool aPlay)
	:CTestMmfMidiClntStep(aTestName, ETestValid),
	 iPlay(aPlay)
	{
	iSectName = aSectName;
	iKeyName = aKeyName;
	}

CTestMidiClntOpenUrl* CTestMidiClntOpenUrl::NewL(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName,const TBool aPlay)
	{
	CTestMidiClntOpenUrl* self = new(ELeave) CTestMidiClntOpenUrl(aTestName, aSectName, aKeyName, aPlay);
	return self;
	}

TVerdict CTestMidiClntOpenUrl::DoTestStepL()
	{
	TPtrC urlname;
	if(!GetStringFromConfig(iSectName,iKeyName,urlname))
		return EInconclusive;

	CMidiClientUtility* player = CMidiClientUtility::NewL(*this, EMdaPriorityNormal, EMdaPriorityPreferenceTimeAndQuality);
	if (!player)
		{
		ERR_PRINTF1(_L("Could not create a CMidiClientUtility"));
		return EInconclusive;
		}
	
	CleanupStack::PushL(player);

	TMMFMessageDestinationPckg dummyPckg;
	TInt dummyFunc = EDevMidiOff;
	TBuf8<8> dummyBuff;
	player->CustomCommandSyncL(dummyPckg, dummyFunc, dummyBuff, dummyBuff, dummyBuff);

	player->OpenUrl(urlname);
	
	// Wait for initialisation callback
	INFO_PRINTF1(_L("CMidiClientUtility: Opening url"));
	CActiveScheduler::Start();

	TVerdict ret = EFail;

	// Check for errors.
	if (iError == KErrNone)
		ret = DoTestL(player);
		
	INFO_PRINTF1(_L("CMidiClientUtility: Destroying"));
	CleanupStack::PopAndDestroy(player);

	if(iError != KErrNone)
		ERR_PRINTF2( _L("CMidiClientUtility failed with error %d"),iError );
	
	return	ret;
	}

TVerdict CTestMidiClntOpenUrl::DoTestL(CMidiClientUtility* /*aMidi*/)
	{
	return EPass;
	}

//------------------------------------------------------------------

CTestMidiClntClose::CTestMidiClntClose(const TDesC& aTestName)
	:CTestMmfMidiClntStep(aTestName, ETestValid)
	{
	}

CTestMidiClntClose* CTestMidiClntClose::NewL(const TDesC& aTestName)
	{
	CTestMidiClntClose* self = new(ELeave) CTestMidiClntClose(aTestName);
	return self;
	}

TVerdict CTestMidiClntClose::DoTestL(CMidiClientUtility* aMidi)
	{
	INFO_PRINTF1(_L("CMidiClientUtility: Closing file"));
	aMidi->Close();
	return EPass;
	}

//------------------------------------------------------------------

CTestMidiClntPlay::CTestMidiClntPlay(const TDesC& aTestName, const TTestStepType aTestType)
	:CTestMmfMidiClntStep(aTestName, aTestType)
	{
	}

CTestMidiClntPlay* CTestMidiClntPlay::NewL(const TDesC& aTestName, const TTestStepType aTestType)
	{
	CTestMidiClntPlay* self = new(ELeave) CTestMidiClntPlay(aTestName, aTestType);
	return self;
	}

TVerdict CTestMidiClntPlay::DoTestL(CMidiClientUtility* aMidi)
	{
	TInt expErr = KErrNone;
	//TTimeIntervalMicroSeconds fadeOutDuration(0);

	INFO_PRINTF1(_L("CMidiClientUtility: Play midi file"));
	// expected results
	switch(iTestType)
		{
		case ETestValid:
			expErr = KErrNone;
			break;
		case ETestNoPlugin:
			expErr = KErrNotSupported;
			break;
		case ETestInvalidState:
			expErr = KErrUnknown;
			break;
		default:
			ERR_PRINTF1(_L("Error - invalid test step type"));
			return EInconclusive;
		}
	
	iCurrentState = EMidiStateClosedDisengaged;
	aMidi->OpenFile(_L("c:\\DoesntExist.mid"));
	INFO_PRINTF1(_L("Waiting for EMidiStateOpenDisengaged state..."));
	CActiveScheduler::Start();	// EMidiStateClosedDisengaged -> EMidiStateOpenDisengaged
	if (iCurrentState != EMidiStateOpenDisengaged)
		{
		ERR_PRINTF2(_L("Unexpected state (expected = EMidiStateOpenDisengaged, received = %d)"), iCurrentState);
		return EFail;
		}

	aMidi->Play();
	if (expErr != iError)
		{
		ERR_PRINTF3(_L("Play gave error %d (expected %d)"),iError, expErr);
		return EFail;
		}
	else
		INFO_PRINTF3(_L("Play, %d = %d"), iError, expErr);
	
	return EPass;	
	}

//------------------------------------------------------------------


CTestMidiClntStop::CTestMidiClntStop(const TDesC& aTestName, const TTestStepType aTestType)
	:CTestMmfMidiClntStep(aTestName, aTestType)
	{
	}

CTestMidiClntStop* CTestMidiClntStop::NewL(const TDesC& aTestName, const TTestStepType aTestType)
	{
	CTestMidiClntStop* self = new(ELeave) CTestMidiClntStop(aTestName, aTestType);
	return self;
	}

TVerdict CTestMidiClntStop::DoTestL(CMidiClientUtility* aMidi)
	{
	TVerdict ret = EPass;
	TInt expErr = KErrNone;
	TTimeIntervalMicroSeconds fadeOutDuration(0);

	INFO_PRINTF1(_L("CMidiClientUtility: Stop midi file"));
	// expected results
	switch(iTestType)
		{
		case ETestValid:
			expErr = KErrNone;
			fadeOutDuration = 20;
			break;
		case ETestNoPlugin:
			expErr = KErrNotSupported;
			break;
		case ETestNoResource:
			expErr = KErrNotReady;
			break;
		default:
			ERR_PRINTF1(_L("Error - invalid test step type"));
			return EInconclusive;
		}

	aMidi->Stop(fadeOutDuration);
	if (expErr != KErrNone)
		INFO_PRINTF2(_L("Stop gave error %d"),expErr);
	else
		INFO_PRINTF1(_L("Stopping midi file"));

	return ret;
	}

//------------------------------------------------------------------


CTestMidiClntGetState::CTestMidiClntGetState(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, const TTestStepType aTestType, const TBool aPlay)
	:CTestMidiClntOpenFile(aTestName, aSectName, aKeyName, aPlay)
	{
	// NB this inherits from CTestMidiClntOpenFile, NOT from CTestMmfMidiClntStep
	// so we have to set this manually.
	iTestType = aTestType;
	}

CTestMidiClntGetState* CTestMidiClntGetState::NewL(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName, const TTestStepType aTestType, const TBool aPlay)
	{
	CTestMidiClntGetState* self = new(ELeave) CTestMidiClntGetState(aTestName, aSectName, aKeyName, aTestType, aPlay);
	return self;
	}

TVerdict CTestMidiClntGetState::DoTestL(CMidiClientUtility* aMidi)
	{
	TVerdict ret = EPass;
	TMidiState expErr;
	//TTimeIntervalMicroSeconds fadeOutDuration(10);

	INFO_PRINTF1(_L("CMidiClientUtility: Get current state of midi file"));
	// expected results
	switch(iTestType)
		{
		case ETestValid:
			expErr = EMidiStateOpenDisengaged;
			break;
		case ETestNegative:
			expErr = EMidiStateClosedDisengaged;
			// Close player so we can perform negative test, get state before any initialisation has been done
			aMidi->Close();
			break;
		default:
			ERR_PRINTF1(_L("Error - invalid test step type"));
			return EInconclusive;
		}

	TMidiState state = aMidi->State();
	if (expErr != state)
		{
		ERR_PRINTF3(_L("State gave error %d (expected %d)"),state, expErr);
		ret = EFail;
		}
	else
		INFO_PRINTF3(_L("State %d = %d"),state, expErr);

	return ret;
	}

//------------------------------------------------------------------

CTestMidiClntPlayNote::CTestMidiClntPlayNote(const TDesC& aTestName, const TTestStepType aTestType)
	:CTestMmfMidiClntStep(aTestName, aTestType)
	{
	}

CTestMidiClntPlayNote* CTestMidiClntPlayNote::NewL(const TDesC& aTestName, const TTestStepType aTestType)
	{
	CTestMidiClntPlayNote* self = new(ELeave) CTestMidiClntPlayNote(aTestName, aTestType);
	return self;
	}

TVerdict CTestMidiClntPlayNote::DoTestL(CMidiClientUtility* aMidi)
	{
	TVerdict ret = EPass;
	TInt expErr = KErrNone;
	TInt channel = 0;
	TInt note = 0;
	TTimeIntervalMicroSeconds duration(10);
	TInt noteOnVelocity = 0;
	TInt noteOffVelocity = 0;

	INFO_PRINTF1(_L("CMidiClientUtility: Play midi note"));
	// expected results
	switch(iTestType)
		{
		case ETestValid:
			expErr = KErrNone;
			break;
		case ETestZeroDurationOutOfRange:
			expErr = KErrNotSupported;
			duration = 0;
			break;
		case ETestInvalidChannelOutOfRange:
			expErr = KErrArgument;
			channel = 16;
			break;
		case ETestNoteHighOutOfRange:
			expErr = KErrArgument;
			note = 128;
			break;
		case ETestNoteLowOutOfRange:
			expErr = KErrArgument;
			note = -1;
			break;
		case ETestAttackHighOutOfRange:
			expErr = KErrArgument;
			noteOnVelocity = 128;
			break;
		case ETestAttackLowOutOfRange:
			expErr = KErrArgument;
			noteOnVelocity = -1;
			break;
		case ETestReleaseHighOutOfRange:
			expErr = KErrArgument;
			noteOffVelocity = 128;
			break;
		case ETestReleaseLowOutOfRange:
			expErr = KErrArgument;
			noteOffVelocity = -1;
			break;
		default:
			ERR_PRINTF1(_L("Error - invalid test step type"));
			return EInconclusive;
		}
	TRAPD(err, aMidi->PlayNoteL(channel, note, duration, noteOnVelocity, noteOffVelocity));

	if (expErr != err)
		{
		ERR_PRINTF3(_L("State gave error %d (expected %d)"),err, expErr);
		ret = EFail;
		}
	else
		INFO_PRINTF3(_L("PlayNoteL %d = %d"),err ,expErr);

	return ret;
	}

//------------------------------------------------------------------

CTestMidiClntStopNotes::CTestMidiClntStopNotes(const TDesC& aTestName, const TTestStepType aTestType)
	:CTestMmfMidiClntStep(aTestName, aTestType)
	{
	}

CTestMidiClntStopNotes* CTestMidiClntStopNotes::NewL(const TDesC& aTestName, const TTestStepType aTestType)
	{
	CTestMidiClntStopNotes* self = new(ELeave) CTestMidiClntStopNotes(aTestName, aTestType);
	return self;
	}

TVerdict CTestMidiClntStopNotes::DoTestL(CMidiClientUtility* aMidi)
	{
	// The framework will catch the invalid channel and no note available
	TVerdict ret = EPass;
	TInt channel = 0;
	//TInt expErr = KErrNone;

	switch(iTestType)
		{
		case ETestValid:
			break;
		case ETestInvalidChannelOutOfRange:
			channel = 16;
			break;
		case ETestNoNoteAvailable:
			break;
		default:
			ERR_PRINTF1(_L("Error - invalid test step type"));
			return EInconclusive;
		}

	INFO_PRINTF1(_L("CMidiClientUtility: Stop midi note"));
	aMidi->StopNotes(channel);
	return ret;
	}

//------------------------------------------------------------------


CTestMidiClntNoteOn::CTestMidiClntNoteOn(const TDesC& aTestName, const TTestStepType aTestType)
	:CTestMmfMidiClntStep(aTestName, aTestType)
	{
	}

CTestMidiClntNoteOn* CTestMidiClntNoteOn::NewL(const TDesC& aTestName, const TTestStepType aTestType)
	{
	CTestMidiClntNoteOn* self = new(ELeave) CTestMidiClntNoteOn(aTestName, aTestType);
	return self;
	}

TVerdict CTestMidiClntNoteOn::DoTestL(CMidiClientUtility* aMidi)
	{
	TVerdict ret = EPass;
	TInt expErr = KErrNone;
	TInt channel = 0;
	TInt note = 0;
	TInt velocity = 0;

	INFO_PRINTF1(_L("CMidiClientUtility: Midi note on"));
	// expected results
	switch(iTestType)
		{
		case ETestValid:
			expErr = KErrNone;
			channel = 1;
			note = 2;
			velocity = 10;
			break;
		case ETestNoNoteAvailable:
			expErr = KErrArgument;
			break;
		case ETestInvalidChannelOutOfRange:
			expErr = KErrArgument;
			channel = 16;
			break;
		case ETestNoteHighOutOfRange:
			expErr = KErrArgument;
			note = 128;
			break;
		case ETestNoteLowOutOfRange:
			expErr = KErrArgument;
			note = -1;
			break;
		case ETestVelocityHighOutOfRange:
			expErr = KErrArgument;
			velocity = 128;
			break;
		case ETestVelocityLowOutOfRange:
			expErr = KErrArgument;
			velocity = -1;
			break;
		default:
			ERR_PRINTF1(_L("Error - invalid test step type"));
			return EInconclusive;
		}

	TRAPD(err, aMidi->NoteOnL(channel,note,velocity));
	if (expErr != err)
		{
		ERR_PRINTF3(_L("NoteOnL gave error %d (expected %d)"),err, expErr);
		ret = EFail;
		}
	else
		INFO_PRINTF3(_L("NoteOnL %d = %d"),err ,expErr);

	return ret;
	}

//------------------------------------------------------------------


CTestMidiClntNoteOff::CTestMidiClntNoteOff(const TDesC& aTestName, const TTestStepType aTestType)
	:CTestMmfMidiClntStep(aTestName, aTestType)
	{
	}

CTestMidiClntNoteOff* CTestMidiClntNoteOff::NewL(const TDesC& aTestName, const TTestStepType aTestType)
	{
	CTestMidiClntNoteOff* self = new(ELeave) CTestMidiClntNoteOff(aTestName, aTestType);
	return self;
	}

TVerdict CTestMidiClntNoteOff::DoTestL(CMidiClientUtility* aMidi)
	{
	TVerdict ret = EPass;
	TInt expErr = KErrNone;
	TInt channel = 0;
	TInt note = 0;
	TInt velocity = 0;
	
	// Play Midi Note on channel=0, note=1, velocity=50
	TRAPD(err, aMidi->NoteOnL(0,1,50));
	if (expErr != err)
		{
		ERR_PRINTF3(_L("NoteOnL gave error %d (expected %d)"),err, expErr);
		ret = EFail;
		}
	else
		INFO_PRINTF3(_L("NoteOnL %d = %d"),err ,expErr);

	INFO_PRINTF1(_L("CMidiClientUtility: Midi note off"));
	// expected results
	switch(iTestType)
		{
		case ETestValid:
			expErr = KErrNone;
			channel = 1;
			note = 2;
			velocity = 10;
			break;
		case ETestNoNoteAvailable:
			expErr = KErrNone;      // No error is raised, as per specification
			break;
		case ETestInvalidChannelOutOfRange:
			expErr = KErrArgument;
			channel = 16;
			break;
		case ETestDifferentNoteVelocity:
			expErr = KErrArgument;
			note = 1;
			velocity = 127;
			break;
		default:
			ERR_PRINTF1(_L("Error - invalid test step type"));
			return EInconclusive;
		}

	TRAPD(err2, aMidi->NoteOffL(channel,note,velocity));
	if (expErr != err2)
		{
		ERR_PRINTF3(_L("NoteOffL gave error %d (expected %d)"),err2, expErr);
		ret = EFail;
		}
	else
		INFO_PRINTF3(_L("NoteOffL %d = %d"),err2 ,expErr);

	return ret;
	}

//------------------------------------------------------------------


CTestMidiClntReturnPlaybackRate::CTestMidiClntReturnPlaybackRate(const TDesC& aTestName, const TTestStepType aTestType)
	:CTestMmfMidiClntStep(aTestName, aTestType)
	{
	}

CTestMidiClntReturnPlaybackRate* CTestMidiClntReturnPlaybackRate::NewL(const TDesC& aTestName, const TTestStepType aTestType)
	{
	CTestMidiClntReturnPlaybackRate* self = new(ELeave) CTestMidiClntReturnPlaybackRate(aTestName, aTestType);
	return self;
	}

TVerdict CTestMidiClntReturnPlaybackRate::DoTestL(CMidiClientUtility* aMidi)
	{
	TVerdict ret = EPass;
	TInt expErr = KErrNone;

	INFO_PRINTF1(_L("CMidiClientUtility: Check playback rate"));
	
	// expected results
	switch (iTestType)
		{
		case ETestValid:
			expErr = KErrNone;
			break;
		case ETestNoResource:
			expErr = KErrNotReady;
			break;
		default:
			ERR_PRINTF1(_L("Error - invalid test step type"));
			return EInconclusive;
		}
//	TInt rate = 0;	// fixes warning

//	TRAPD(err, rate = aMidi->PlaybackRateL());
	TRAPD(err, aMidi->PlaybackRateL());	// EABI warning removal
	if(expErr != err)
		{
		ERR_PRINTF3( _L("PlaybackRateL gave error %d (expected %d)"),err, expErr);
		ret = EFail;
		}
	else	
		INFO_PRINTF3(_L("PlaybackRateL %d = %d"), err, expErr);

	return ret;
	}

//------------------------------------------------------------------


CTestMidiClntSetPlaybackRate::CTestMidiClntSetPlaybackRate(const TDesC& aTestName, const TTestStepType aTestType)
	:CTestMmfMidiClntStep(aTestName, aTestType)
	{
	}

CTestMidiClntSetPlaybackRate* CTestMidiClntSetPlaybackRate::NewL(const TDesC& aTestName, const TTestStepType aTestType)
	{
	CTestMidiClntSetPlaybackRate* self = new(ELeave) CTestMidiClntSetPlaybackRate(aTestName, aTestType);
	return self;
	}

TVerdict CTestMidiClntSetPlaybackRate::DoTestL(CMidiClientUtility* aMidi)
	{
	TVerdict ret = EPass;
	TInt expErr = KErrNone;
	TInt playbackRate = 0;

	INFO_PRINTF1(_L("CMidiClientUtility: Set playback rate"));

	switch (iTestType)
		{
		case ETestValid:
			expErr = KErrNone;
			break;
		case ETestNoResource:
			expErr = KErrNotReady;
			break;
		case ETestOutOfRangeHigh:
			// high limit on playback rate is dependant on controller
			// when have a proper controller the value below should be changed
			// to a value higher than the upper limit
			playbackRate = -1;	
			expErr = KErrNotSupported;
			break;
		case ETestOutOfRangeLow:
			playbackRate = -1;		// negative values not supported
			expErr = KErrNotSupported;
			break;
		default:
			ERR_PRINTF1(_L("Error - invalid test step type"));
			return EInconclusive;
		}
	
	TRAPD(err, aMidi->SetPlaybackRateL(playbackRate));
	if(expErr != err)
		{
		ERR_PRINTF3( _L("SetPlaybackRateL gave error %d (expected %d)"),err, expErr);
		ret = EFail;
		}
	else
		INFO_PRINTF3(_L("SetPlaybackRateL %d = %d"), err, expErr);

	return ret;
	}

//------------------------------------------------------------------


CTestMidiClntReturnMaxPlayRate::CTestMidiClntReturnMaxPlayRate(const TDesC& aTestName, const TTestStepType aTestType)
	:CTestMmfMidiClntStep(aTestName, aTestType)
	{
	}

CTestMidiClntReturnMaxPlayRate* CTestMidiClntReturnMaxPlayRate::NewL(const TDesC& aTestName, const TTestStepType aTestType)
	{
	CTestMidiClntReturnMaxPlayRate* self = new(ELeave) CTestMidiClntReturnMaxPlayRate(aTestName, aTestType);
	return self;
	}

TVerdict CTestMidiClntReturnMaxPlayRate::DoTestL(CMidiClientUtility* aMidi)
	{
	TVerdict ret = EPass;
	TInt expErr = KErrNone;

	INFO_PRINTF1(_L("CMidiClientUtility: Check the max play rate"));
	
	// expected results
	switch (iTestType)
		{
		case ETestValid:
			expErr = KErrNone;
			break;
		case ETestNoResource:
			expErr = KErrNotReady;
			break;
		default:
			ERR_PRINTF1(_L("Error - invalid test step type"));
			return EInconclusive;
		}
//	TInt rate = 0;	// fixes warning
//	TRAPD(err, rate = aMidi->MaxPlaybackRateL());
	TRAPD(err, aMidi->MaxPlaybackRateL());		// EABI warning removal
	if(expErr != err)
		{
		ERR_PRINTF3( _L("MaxPlayRateL gave error %d (expected %d)"),err, expErr);
		ret = EFail;
		}
	else
		INFO_PRINTF3(_L("MaxPlayRateL %d = %d"), err, expErr);

	return ret;
	}

//------------------------------------------------------------------


CTestMidiClntReturnMinPlayRate::CTestMidiClntReturnMinPlayRate(const TDesC& aTestName, const TTestStepType aTestType)
	:CTestMmfMidiClntStep(aTestName, aTestType)
	{
	}

CTestMidiClntReturnMinPlayRate* CTestMidiClntReturnMinPlayRate::NewL(const TDesC& aTestName, const TTestStepType aTestType)
	{
	CTestMidiClntReturnMinPlayRate* self = new(ELeave) CTestMidiClntReturnMinPlayRate(aTestName, aTestType);
	return self;
	}

TVerdict CTestMidiClntReturnMinPlayRate::DoTestL(CMidiClientUtility* aMidi)
	{
	TVerdict ret = EPass;
	TInt expErr = KErrNone;

	INFO_PRINTF1(_L("CMidiClientUtility: Check the min play rate"));

	// expected results
	switch (iTestType)
		{
		case ETestValid:
			expErr = KErrNone;
			break;
		case ETestNoResource:
			expErr = KErrNotReady;
			break;
		default:
			ERR_PRINTF1(_L("Error - invalid test step type"));
			return EInconclusive;
		}

//	TInt rate = 0;	// fixes warning
//	TRAPD(err, rate = aMidi->MinPlaybackRateL());
	TRAPD(err, aMidi->MinPlaybackRateL());	// EABI warning removal
	if(expErr != err)
		{
		ERR_PRINTF3( _L("MinPlayRateL gave error %d (expected %d)"),err, expErr);
		ret = EFail;
		}
	else
		INFO_PRINTF3(_L("MinPlayRateL %d = %d"), err, expErr);

	return ret;	
	}

//------------------------------------------------------------------


CTestMidiClntTempoMicroBeatsPerMinute::CTestMidiClntTempoMicroBeatsPerMinute(const TDesC& aTestName, const TTestStepType aTestType)
	:CTestMmfMidiClntStep(aTestName, aTestType)
	{
	}

CTestMidiClntTempoMicroBeatsPerMinute* CTestMidiClntTempoMicroBeatsPerMinute::NewL(const TDesC& aTestName, const TTestStepType aTestType)
	{
	CTestMidiClntTempoMicroBeatsPerMinute* self = new(ELeave) CTestMidiClntTempoMicroBeatsPerMinute(aTestName, aTestType);
	return self;
	}

TVerdict CTestMidiClntTempoMicroBeatsPerMinute::DoTestL(CMidiClientUtility* aMidi)
	{
	TVerdict ret = EPass;
	TInt expErr = KErrNone;
//	TInt microBeatsPerMinute = 0;	// EABI warning removal

	INFO_PRINTF1(_L("CMidiClientUtility: Return the tempo micro beats per minute"));
	// expected results
	switch(iTestType)
		{
		case ETestValid:
			expErr = KErrNone;
			break;
		case ETestNoResource:
			expErr = KErrNotReady;
			break;
		default:
			ERR_PRINTF1(_L("Error - invalid test step type"));
			return EInconclusive;
		}

//	TRAPD(err, microBeatsPerMinute = aMidi->TempoMicroBeatsPerMinuteL());
	TRAPD(err, aMidi->TempoMicroBeatsPerMinuteL());	// EABI warning removal
	if (expErr != err)
		{
		ERR_PRINTF3(_L("TempoMicroBeatsPerMinuteL gave error %d (expected %d)"),err, expErr);
		ret = EFail;
		}
	else
		INFO_PRINTF3(_L("TempoMicroBeatsPerMinuteL %d = %d"),err ,expErr);

	return ret;
	}

//------------------------------------------------------------------


CTestMidiClntSetTempo::CTestMidiClntSetTempo(const TDesC& aTestName, const TTestStepType aTestType)
	:CTestMmfMidiClntStep(aTestName, aTestType)
	{
	}

CTestMidiClntSetTempo* CTestMidiClntSetTempo::NewL(const TDesC& aTestName, const TTestStepType aTestType)
	{
	CTestMidiClntSetTempo* self = new(ELeave) CTestMidiClntSetTempo(aTestName, aTestType);
	return self;
	}

TVerdict CTestMidiClntSetTempo::DoTestL(CMidiClientUtility* aMidi)
	{
	TVerdict ret = EPass;
	TInt expErr = KErrNone;
	TInt microBeatsPerMinute = 0;

	INFO_PRINTF1(_L("CMidiClientUtility: Set the tempo micro beats per minute"));
	// expected results
	switch(iTestType)
		{
		case ETestValid:
			expErr = KErrNone;
			microBeatsPerMinute = 60*1000000;
			break;
		case ETestNoResource:
			expErr = KErrNotReady;
			microBeatsPerMinute = 80*1000000;
			break;
		case ETestBeatsLowOutOfRange:
			expErr = KErrArgument;
			microBeatsPerMinute = -1*1000000;
			break;
		case ETestBeatsHighOutOfRange:
			expErr = KErrArgument;
			//microBeatsPerMinute = 1000001*1000000; XXX overflow warning
			microBeatsPerMinute = 1000001;
			break;
		default:
			ERR_PRINTF1(_L("Error - invalid test step type"));
			return EInconclusive;
		}

	TRAPD(err, aMidi->SetTempoL(microBeatsPerMinute));
	if (expErr != err)
		{
		ERR_PRINTF3(_L("SetTempoL gave error %d (expected %d)"),err, expErr);
		ret = EFail;
		}
	else
		INFO_PRINTF3(_L("SetTempoL %d = %d"),err ,expErr);

	return ret;
	}

//------------------------------------------------------------------


CTestMidiClntGetPitchTranspositionCents::CTestMidiClntGetPitchTranspositionCents(const TDesC& aTestName, const TTestStepType aTestType)
	:CTestMmfMidiClntStep(aTestName, aTestType)
	{
	}

CTestMidiClntGetPitchTranspositionCents* CTestMidiClntGetPitchTranspositionCents::NewL(const TDesC& aTestName, const TTestStepType aTestType)
	{
	CTestMidiClntGetPitchTranspositionCents* self = new(ELeave) CTestMidiClntGetPitchTranspositionCents(aTestName, aTestType);
	return self;
	}

TVerdict CTestMidiClntGetPitchTranspositionCents::DoTestL(CMidiClientUtility* aMidi)
	{
	TVerdict ret = EPass;
	TInt expErr = KErrNone;

	INFO_PRINTF1(_L("CMidiClientUtility: Set the pitch transposition"));
	// expected results

	switch (iTestType)
		{
		case ETestValid:
			expErr = KErrNone;
			break;
		case ETestNoResource:
			expErr = KErrNotReady;
			break;
		default:
			ERR_PRINTF1(_L("Error - invalid test step type"));
			return EInconclusive;
		}

//	TInt pitch = 0;	// fixes warning
//	TRAPD(err, pitch = aMidi->PitchTranspositionCentsL());
	TRAPD(err, aMidi->PitchTranspositionCentsL());	// EABI warning removal - "I see trouble ahead..."
	if(expErr != err)
		{
		ERR_PRINTF3( _L("PitchTranspositionCentsL gave error %d (expected %d)"),err, expErr);
		ret = EFail;
		}
	else
		INFO_PRINTF3(_L("PitchTranspositionCentsL %d = %d"), err, expErr);

	return ret;
	}

//------------------------------------------------------------------


CTestMidiClntSetPitchTransposition::CTestMidiClntSetPitchTransposition(const TDesC& aTestName, const TTestStepType aTestType)
	:CTestMmfMidiClntStep(aTestName, aTestType)
	{
	}

CTestMidiClntSetPitchTransposition* CTestMidiClntSetPitchTransposition::NewL(const TDesC& aTestName, const TTestStepType aTestType)
	{
	CTestMidiClntSetPitchTransposition* self = new(ELeave) CTestMidiClntSetPitchTransposition(aTestName, aTestType);
	return self;
	}

TVerdict CTestMidiClntSetPitchTransposition::DoTestL(CMidiClientUtility* aMidi)
	{
	TVerdict ret = EPass;
	TInt expErr = KErrNone;
	TInt pitchTrans = 1200;	// one octave

	INFO_PRINTF1(_L("CMidiClientUtility: Set the pitch transposition"));
	// expected results

	switch (iTestType)
		{
		case ETestValid:
			expErr = KErrNone;
			break;
		case ETestNoResource:
			expErr = KErrNotReady;
			break;
//According to GM2 spec, the minimum value can be -6499 cents, or at least -1299 cents.
		case ETestOutOfRangeHigh:
			// high limit on pitch transposition is dependant on controller
			// when have a proper controller the value below should be changed
			// to a value higher than the upper limit
			pitchTrans = -6499;	//KErrArgument expect by a real controller implementation if out-of-range
			expErr = KErrNone;	
			break;
/*This test has been removed by AD as it was not valid - negative values ARE allowed by MIDI spec
According to GM2 spec, the minimum value can be -6499 cents, or at least -1299 cents.
		case ETestOutOfRangeLow:
			pitchTrans = -1;		// negative values not supported
			expErr = KErrNotSupported;
			break;
*/
		default:
			ERR_PRINTF1(_L("Error - invalid test step type"));
			return EInconclusive;
		}
	
	TRAPD(err, aMidi->SetPitchTranspositionL(pitchTrans));
	if(expErr != err)
		{
		ERR_PRINTF3( _L("SetPitchTranspositionL gave error %d (expected %d)"),err, expErr);
		ret = EFail;
		}
	else
		INFO_PRINTF3(_L("SetPitchTranspositionL %d = %d"), err, expErr);

	return ret;
	}

//------------------------------------------------------------------


CTestMidiClntDurationMicroSeconds::CTestMidiClntDurationMicroSeconds(const TDesC& aTestName, const TTestStepType aTestType)
	:CTestMmfMidiClntStep(aTestName, aTestType)
	{
	}

CTestMidiClntDurationMicroSeconds* CTestMidiClntDurationMicroSeconds::NewL(const TDesC& aTestName, const TTestStepType aTestType)
	{
	CTestMidiClntDurationMicroSeconds* self = new(ELeave) CTestMidiClntDurationMicroSeconds(aTestName, aTestType);
	return self;
	}

TVerdict CTestMidiClntDurationMicroSeconds::DoTestL(CMidiClientUtility* aMidi)
	{
	TVerdict ret = EPass;
	TInt expErr = KErrNone;
	TTimeIntervalMicroSeconds durationMicroSeconds(0);

	INFO_PRINTF1(_L("CMidiClientUtility: Get duration in micro seconds"));
	// expected results
	switch(iTestType)
		{
		case ETestValid:
			expErr = KErrNone;
			break;
		case ETestNoResource:
			expErr = KErrNotReady;
			break;
		default:
			ERR_PRINTF1(_L("Error - invalid test step type"));
			return EInconclusive;
		}

	TRAPD(err, 	durationMicroSeconds = aMidi->DurationMicroSecondsL());
	if (expErr != err)
		{
		ERR_PRINTF3(_L("DurationMicroSecondsL gave error %d (expected %d)"),err, expErr);
		ret = EFail;
		}
	else
		INFO_PRINTF3(_L("DurationMicroSecondsL %d = %d"),err ,expErr);

	return ret;
	}

//------------------------------------------------------------------


CTestMidiClntDurationMicroBeats::CTestMidiClntDurationMicroBeats(const TDesC& aTestName, const TTestStepType aTestType)
	:CTestMmfMidiClntStep(aTestName, aTestType)
	{
	}

CTestMidiClntDurationMicroBeats* CTestMidiClntDurationMicroBeats::NewL(const TDesC& aTestName, const TTestStepType aTestType)
	{
	CTestMidiClntDurationMicroBeats* self = new(ELeave) CTestMidiClntDurationMicroBeats(aTestName, aTestType);
	return self;
	}

TVerdict CTestMidiClntDurationMicroBeats::DoTestL(CMidiClientUtility* aMidi)
	{
	TVerdict ret = EPass;
	TInt expErr = KErrNone;
	//TTimeIntervalMicroSeconds durationMicroSeconds(0);
	
	INFO_PRINTF1(_L("CMidiClientUtility: Get duration in micro beats"));
	// expected results
	switch(iTestType)
		{
		case ETestValid:
			expErr = KErrNone;
			break;
		case ETestNoResource:
			expErr = KErrNotReady;
			break;
		default:
			ERR_PRINTF1(_L("Error - invalid test step type"));
			return EInconclusive;
		}
	TRAPD(err, aMidi->DurationMicroBeatsL());
	if (expErr != err)
		{
		ERR_PRINTF3(_L("DurationMicroBeatsL gave error %d (expected %d)"),err, expErr);
		ret = EFail;
		}
	else
		INFO_PRINTF3(_L("DurationMicroBeatsL %d = %d"),err ,expErr);

	return ret;
	}

//------------------------------------------------------------------


CTestMidiClntNumTracks::CTestMidiClntNumTracks(const TDesC& aTestName, const TTestStepType aTestType)
	:CTestMmfMidiClntStep(aTestName, aTestType)
	{
	}

CTestMidiClntNumTracks* CTestMidiClntNumTracks::NewL(const TDesC& aTestName, const TTestStepType aTestType)
	{
	CTestMidiClntNumTracks* self = new(ELeave) CTestMidiClntNumTracks(aTestName, aTestType);
	return self;
	}

TVerdict CTestMidiClntNumTracks::DoTestL(CMidiClientUtility* aMidi)
	{
	TVerdict ret = EPass;
	TInt expErr = KErrNone;
	
	INFO_PRINTF1(_L("CMidiClientUtility: Get current number of tracks"));
	// expected results
	switch(iTestType)
		{
		case ETestValid:
			expErr = KErrNone;
			break;
		default:
			ERR_PRINTF1(_L("Error - invalid test step type"));
			return EInconclusive;
		}
//	TInt numOfTracks = 0;
//	TRAPD(err, numOfTracks = aMidi->NumTracksL());
	TRAPD(err, aMidi->NumTracksL());	// EABI warning removal
	if (expErr != err)
		{
		ERR_PRINTF3(_L("NumTracksL gave error %d (expected %d)"),err, expErr);
		ret = EFail;
		}
	else
		INFO_PRINTF3(_L("NumTracksL %d = %d"),err ,expErr);

	return ret;
	}

//------------------------------------------------------------------


CTestMidiClntSetTrackMute::CTestMidiClntSetTrackMute(const TDesC& aTestName, const TTestStepType aTestType)
	:CTestMmfMidiClntStep(aTestName, aTestType)
	{
	}

CTestMidiClntSetTrackMute* CTestMidiClntSetTrackMute::NewL(const TDesC& aTestName, const TTestStepType aTestType)
	{
	CTestMidiClntSetTrackMute* self = new(ELeave) CTestMidiClntSetTrackMute(aTestName, aTestType);
	return self;
	}

TVerdict CTestMidiClntSetTrackMute::DoTestL(CMidiClientUtility* aMidi)
	{
	TVerdict ret = EPass;
	TInt expErr = KErrNone;
	TInt track = 0;
	TBool muted = ETrue;
	
	INFO_PRINTF1(_L("CMidiClientUtility: Set track mute"));
	// expected results
	switch(iTestType)
		{
		case ETestValid:
			expErr = KErrNone;
			break;
		case ETestNoResource:
			expErr = KErrNotReady;
			TRAP_IGNORE(track = aMidi->NumTracksL()+1);
			break;
		case ETestInvalidTrack:
			expErr = KErrArgument;
			TRAP_IGNORE(track = aMidi->NumTracksL()+1);
			break;
		default:
			ERR_PRINTF1(_L("Error - invalid test step type"));
			return EInconclusive;
		}

	TRAPD(err, 	aMidi->SetTrackMuteL(track, muted));
	
	// check correct result
	if (iTestType == ETestValid)
		{
		TBool currMuted = EFalse;

		// until real MIDI controller IsTrackMuteL is always TRUE
		TRAPD(err2, currMuted = aMidi->IsTrackMuteL(track));
		
		if(err2 != KErrNone)
			{
			ERR_PRINTF2(_L("IsTrackMuteL() returned %d"), err2);
			ret = EInconclusive;
			return ret;
			}

		if(currMuted != muted)
			{
			ERR_PRINTF2(_L("Error : expected %d"), muted);
			ret = EFail;
			return ret;
			}
		}
		
	if (expErr != err)
		{
		ERR_PRINTF3(_L("SetTrackMuteL gave error %d (expected %d)"),err, expErr);
		ret = EFail;
		}
	else
		INFO_PRINTF3(_L("SetTrackMuteL %d = %d"),err ,expErr);

	return ret;
	}
	
//------------------------------------------------------------------


CTestMidiClntMimeType::CTestMidiClntMimeType(const TDesC& aTestName, const TTestStepType aTestType)
	:CTestMmfMidiClntStep(aTestName, aTestType)
	{
	}

CTestMidiClntMimeType* CTestMidiClntMimeType::NewL(const TDesC& aTestName, const TTestStepType aTestType)
	{
	CTestMidiClntMimeType* self = new(ELeave) CTestMidiClntMimeType(aTestName, aTestType);
	return self;
	}

TVerdict CTestMidiClntMimeType::DoTestL(CMidiClientUtility* aMidi)
	{
	TVerdict ret = EPass;
	TInt expErr = KErrNone;
	
	INFO_PRINTF1(_L("CMidiClientUtility: Get mime type"));
	// expected results
	switch(iTestType)
		{
		case ETestValid:
			expErr = KErrNone;
			break;
		case ETestNoResource:
			expErr = KErrNotReady;
			break;
		default:
			ERR_PRINTF1(_L("Error - invalid test step type"));
			return EInconclusive;
		}

	//TRAPD(err, 	TDesC8 mimeType = aMidi->MimeTypeL());//XXX: TDesC8 mimeType has to be define above
	TRAPD(err, 	aMidi->MimeTypeL());//XXX: TDesC8 mimeType has to be define above
	if (expErr != err)
		{
		ERR_PRINTF3(_L("MimeTypeL gave error %d (expected %d)"),err, expErr);
		ret = EFail;
		}
	else
		INFO_PRINTF3(_L("MimeTypeL %d = %d"),err ,expErr);

	return ret;
	}

//------------------------------------------------------------------


CTestMidiClntReturnPositionMicroSeconds::CTestMidiClntReturnPositionMicroSeconds(const TDesC& aTestName, const TTestStepType aTestType)
	:CTestMmfMidiClntStep(aTestName, aTestType)
	{
	}

CTestMidiClntReturnPositionMicroSeconds* CTestMidiClntReturnPositionMicroSeconds::NewL(const TDesC& aTestName, const TTestStepType aTestType)
	{
	CTestMidiClntReturnPositionMicroSeconds* self = new(ELeave) CTestMidiClntReturnPositionMicroSeconds(aTestName, aTestType);
	return self;
	}

TVerdict CTestMidiClntReturnPositionMicroSeconds::DoTestL(CMidiClientUtility* aMidi)
	{
	TVerdict ret = EPass;
	TInt expErr = KErrNone;
	
	INFO_PRINTF1(_L("CMidiClientUtility: Get position in micro seconds"));
	// expected results
	switch(iTestType)
		{
		case ETestValid:
			expErr = KErrNone;
			break;
		case ETestNoResource:
			expErr = KErrNotReady;
			break;
		case ETestNoResourcePlaying:
			expErr = KErrNotReady;
			break;
		default:
			ERR_PRINTF1(_L("Error - invalid test step type"));
			return EInconclusive;
		}
	TTimeIntervalMicroSeconds positionMicroSeconds(0);
	TRAPD(err, positionMicroSeconds = aMidi->PositionMicroSecondsL());
	if (expErr != err) 
		{
		if (KErrNotReady != err)
			{
			ERR_PRINTF3(_L("Get PositionMicroSecondsL gave error %d (expected %d)"),err, expErr);
		    ret = EFail;
			}
		else
			{
		    ret = EPass;	
			}
		}
	else
		INFO_PRINTF3(_L("Get PositionMicroSecondsL %d = %d"),err ,expErr);
	
	return ret;
	}

//------------------------------------------------------------------


CTestMidiClntSetPositionMicroSeconds::CTestMidiClntSetPositionMicroSeconds(const TDesC& aTestName, const TTestStepType aTestType)
	:CTestMmfMidiClntStep(aTestName, aTestType)
	{
	}

CTestMidiClntSetPositionMicroSeconds* CTestMidiClntSetPositionMicroSeconds::NewL(const TDesC& aTestName, const TTestStepType aTestType)
	{
	CTestMidiClntSetPositionMicroSeconds* self = new(ELeave) CTestMidiClntSetPositionMicroSeconds(aTestName, aTestType);
	return self;
	}

TVerdict CTestMidiClntSetPositionMicroSeconds::DoTestL(CMidiClientUtility* aMidi)
	{
	TVerdict ret = EPass;
	TInt expErr = KErrNone;
	TTimeIntervalMicroSeconds position(0);;
	
	INFO_PRINTF1(_L("CMidiClientUtility: Set position in micro seconds"));
	// expected results
	switch(iTestType)
		{
		case ETestValid:
			expErr = KErrNone;
			break;
		case ETestNoResource:
			expErr = KErrNotReady;
			break;
		case ETestMicrosecondsHighOutOfRange:
			expErr = KErrNotSupported;
			position = 1008;
			break;
		case ETestMicrosecondsLowOutOfRange:
			expErr = KErrNotSupported;
			position = -1;
			break;
		default:
			ERR_PRINTF1(_L("Error - invalid test step type"));
			return EInconclusive;
		}

	TRAPD(err, aMidi->SetPositionMicroSecondsL(position));
	if (expErr != err)
		{
		ERR_PRINTF3(_L("Set PositionMicroSecondsL gave error %d (expected %d)"),err, expErr);
		ret = EFail;
		}
	else
		INFO_PRINTF3(_L("Set PositionMicroSecondsL %d = %d"),err ,expErr);

	return ret;
	}

//------------------------------------------------------------------


CTestMidiClntReturnsPositionMicroBeats::CTestMidiClntReturnsPositionMicroBeats(const TDesC& aTestName, const TTestStepType aTestType)
	:CTestMmfMidiClntStep(aTestName, aTestType)
	{
	}

CTestMidiClntReturnsPositionMicroBeats* CTestMidiClntReturnsPositionMicroBeats::NewL(const TDesC& aTestName, const TTestStepType aTestType)
	{
	CTestMidiClntReturnsPositionMicroBeats* self = new(ELeave) CTestMidiClntReturnsPositionMicroBeats(aTestName, aTestType);
	return self;
	}

TVerdict CTestMidiClntReturnsPositionMicroBeats::DoTestL(CMidiClientUtility* aMidi)
	{
	TVerdict ret = EPass;
	TInt expErr = KErrNone;
	
	INFO_PRINTF1(_L("CMidiClientUtility: Return position in micro beats"));
	// expected results
	switch(iTestType)
		{
		case ETestValid:
			expErr = KErrNone;
			break;
		case ETestNoResource:
			expErr = KErrNotReady;
			break;
		case ETestNoResourcePlaying:
			expErr = KErrNotReady;
			break;
		default:
			ERR_PRINTF1(_L("Error - invalid test step type"));
			return EInconclusive;
		}
	TRAPD(err, aMidi->PositionMicroBeatsL());
	if (expErr != err)
		{
		ERR_PRINTF3(_L("PositionMicroBeatsL gave error %d (expected %d)"),err, expErr);
		ret = EFail;
		}
	else
		INFO_PRINTF3(_L("PositionMicroBeatsL %d = %d"),err ,expErr);

	return ret;
	}

//------------------------------------------------------------------


CTestMidiClntSetPositionMicroBeats::CTestMidiClntSetPositionMicroBeats(const TDesC& aTestName, const TTestStepType aTestType)
	:CTestMmfMidiClntStep(aTestName, aTestType)
	{
	}

CTestMidiClntSetPositionMicroBeats* CTestMidiClntSetPositionMicroBeats::NewL(const TDesC& aTestName, const TTestStepType aTestType)
	{
	CTestMidiClntSetPositionMicroBeats* self = new(ELeave) CTestMidiClntSetPositionMicroBeats(aTestName, aTestType);
	return self;
	}

TVerdict CTestMidiClntSetPositionMicroBeats::DoTestL(CMidiClientUtility* aMidi)
	{
	TVerdict ret = EPass;
	TInt expErr = KErrNone;
	TInt64 microBeats = 0;
	
	INFO_PRINTF1(_L("CMidiClientUtility: Set position in micro beats"));
	// expected results
	switch(iTestType)
		{
		case ETestValid:
			expErr = KErrNone;
			break;
		case ETestNoResource:
			expErr = KErrNotReady;
			break;
		case ETestMicrobeatsHighOutOfRange:
			expErr = KErrNotSupported;
			microBeats = 1000000;
			break;
		case ETestMicrobeatsLowOutOfRange:
			expErr = KErrNotSupported;
			microBeats = -1;
			break;
		default:
			ERR_PRINTF1(_L("Error - invalid test step type"));
			return EInconclusive;
		}

	TRAPD(err, aMidi->SetPositionMicroBeatsL(microBeats));
	if (expErr != err)
		{
		ERR_PRINTF3(_L("SetPositionMicroBeatsL gave error %d (expected %d)"),err, expErr);
		ret = EFail;
		}
	else
		INFO_PRINTF3(_L("SetPositionMicroBeatsL %d = %d"),err ,expErr);

	return ret;
	}

//------------------------------------------------------------------


CTestSetSyncUpdateCallbackInterval::CTestSetSyncUpdateCallbackInterval(const TDesC& aTestName, const TTestStepType aTestType)
	:CTestMmfMidiClntStep(aTestName, aTestType)
	{
	}

CTestSetSyncUpdateCallbackInterval* CTestSetSyncUpdateCallbackInterval::NewL(const TDesC& aTestName, const TTestStepType aTestType)
	{
	CTestSetSyncUpdateCallbackInterval* self = new(ELeave) CTestSetSyncUpdateCallbackInterval(aTestName, aTestType);
	return self;
	}

TVerdict CTestSetSyncUpdateCallbackInterval::DoTestL(CMidiClientUtility* aMidi)
	{
	TVerdict ret = EPass;
	TInt expErr = KErrNone;
	TTimeIntervalMicroSeconds microSeconds(0);
	TInt64 microBeats = 0;
	
	INFO_PRINTF1(_L("CMidiClientUtility: Set update call back interval"));
	// expected results
	switch(iTestType)
		{
		case ETestValid:
			expErr = KErrNone;
			break;
		case ETestMicrosecondsMicrobeatsZero:
			microSeconds = 0;
			microBeats = 0;					
			expErr = KErrNotReady;
			break;
		case ETestMicrosecondsLowOutOfRange:
			expErr = KErrArgument;
			microSeconds = -1;
			break;
		case ETestMicrobeatsHighOutOfRange:
			expErr = KErrArgument;
			microBeats = 1000000;
			break;
		case ETestMicrobeatsLowOutOfRange:
			expErr = KErrArgument;
			microBeats = -1;
			break;
		default:
			ERR_PRINTF1(_L("Error - invalid test step type"));
			return EInconclusive;
		}

	TRAPD(err, aMidi->SetSyncUpdateCallbackIntervalL(microSeconds, microBeats));
	if (expErr != err)
		{
		ERR_PRINTF3(_L("SetSyncUpdateCallbackIntervalL gave error %d (expected %d)"),err, expErr);
		ret = EFail;
		}
	else
		INFO_PRINTF3(_L("SetSyncUpdateCallbackIntervalL %d = %d"),err ,expErr);

	return ret;
	}

CTestSendMessage::CTestSendMessage(const TDesC& aTestName,
									 const TTestStepType aTestType, const TDesC8& aMessage)
	:CTestMmfMidiClntStep(aTestName, aTestType)
	{
	iMessage = aMessage;
	}

CTestSendMessage* CTestSendMessage::NewL(const TDesC& aTestName,
									     const TTestStepType aTestType, const TDesC8& aMessage)
	{
	CTestSendMessage* self = new (ELeave) CTestSendMessage(aTestName,
														   aTestType, aMessage);
	return self;
	}

TVerdict CTestSendMessage::DoTestL(CMidiClientUtility* aMidi)
	{
	INFO_PRINTF1(_L("CMidiClientUtility: Send Message"));

	TVerdict ret = EPass;
	TInt expErr = KErrNone;

	// expected results
	switch(iTestType)
		{
		case ETestValid:
			expErr = KErrNone;
			break;
		case ETestNullMessage:
			expErr = KErrArgument;
			break;
		case ETestUnsupported:
			expErr = KErrNotSupported;
			break;
		case ETestCorrupt:
			expErr = KErrCorrupt;
			break;
		default:
			ERR_PRINTF1(_L("Error - invalid test step type"));
			return EInconclusive;
		}

//	TInt byteProcessed;
//	TRAPD(err1, byteProcessed = aMidi->SendMessageL(iMessage));
	TRAPD(err1, aMidi->SendMessageL(iMessage));	// EABI warning removal
	if(err1 != expErr)
		{
		ERR_PRINTF3(_L("SendMessageL(iMessage) gave error %d (expected %d)"), err1, expErr);
		ret = EFail;
		}

	TTimeIntervalMicroSeconds startTime(105);
//	TRAPD(err2, byteProcessed = aMidi->SendMessageL(iMessage, startTime));
	TRAPD(err2, aMidi->SendMessageL(iMessage, startTime));	// EABI warning removal
	if(err2 != expErr)
		{
		ERR_PRINTF3(_L("SendMessageL(iMessage, startTime) gave error %d (expected %d)"), err1, expErr);
		ret = EFail;
		}

	return ret;
	}

//------------------------------------------------------------------


// CTestSendMessageTime incorporated into CTestSendMessage

//------------------------------------------------------------------

// aTestType as follows :
// 0115 : valid
// 0116 : invalid device ID - REMOVED, API has changed
// 0117 : null
// 0118 : unsupported
// 0119 : unsupported - REMOVED, duplicate
// 0120 : corrupt

CTestSendMipMessage::CTestSendMipMessage(const TDesC& aTestName,
									 const TTestStepType aTestType)
	:CTestMmfMidiClntStep(aTestName, aTestType)
	{
	}

CTestSendMipMessage* CTestSendMipMessage::NewL(const TDesC& aTestName,
									     const TTestStepType aTestType)
	{
	CTestSendMipMessage* self = new (ELeave) CTestSendMipMessage(aTestName,
														   aTestType);
	return self;
	}

TVerdict CTestSendMipMessage::DoTestL(CMidiClientUtility* aMidi)
	{
	INFO_PRINTF1(_L("CMidiClientUtility: Send Mip Message"));

	TVerdict ret = EPass;
	TInt expErr = KErrNone;

	RArray<TMipMessageEntry> mipArray;
	TMipMessageEntry mipEntry1;
	TMipMessageEntry mipEntry2;

	// expected results
	switch(iTestType)
		{
		case ETestValid:
			expErr = KErrNone;
			break;
		case ETestNullMessage:
			expErr = KErrArgument;
			break;
		case ETestUnsupported:
			expErr = KErrNotSupported;
			break;
		case ETestCorrupt:
			expErr = KErrCorrupt;
			break;
		default:
			ERR_PRINTF1(_L("Error - invalid test step type"));
			return EInconclusive;
		}

	switch(iTestType)
		{
		case ETestValid:
			mipEntry1.iChannel = 10;
			mipEntry1.iMIPValue = 20;
			mipEntry2.iChannel = 11;
			mipEntry2.iMIPValue = 21;

			mipArray.Append(mipEntry1);
			mipArray.Append(mipEntry2);
			break;

		case ETestNullMessage:
			// leave array empty
			// *** NB this will cause a problem in debug mode - causes a server panic!
			break;

		case ETestUnsupported:
			// XXX : values defined as unsupported?
			mipEntry1.iChannel = 0;
			mipEntry1.iMIPValue = 0;
			mipEntry2.iChannel = 0;
			mipEntry2.iMIPValue = 0;

			mipArray.Append(mipEntry1);
			mipArray.Append(mipEntry2);
			break;

		case ETestCorrupt:
			// XXX : values defined as corrupt?
			mipEntry1.iChannel = -1;
			mipEntry1.iMIPValue = -1;
			mipEntry2.iChannel = -1;
			mipEntry2.iMIPValue = -1;

			mipArray.Append(mipEntry1);
			mipArray.Append(mipEntry2);
			break;
		}

	TRAPD(err1, aMidi->SendMipMessageL(mipArray));
	if(err1 != expErr)
		{
		ERR_PRINTF3(_L("SendMipMessageL(iMessage) gave error %d (expected %d)"), err1, expErr);
		ret = EFail;
		}

	mipArray.Close();
	return ret;
	}

//------------------------------------------------------------------


CTestNumberOfBanks::CTestNumberOfBanks(const TDesC& aTestName,
									 const TTestStepType aTestType)
	:CTestMmfMidiClntStep(aTestName, aTestType)
	{
	}

CTestNumberOfBanks* CTestNumberOfBanks::NewL(const TDesC& aTestName,
									 const TTestStepType aTestType)
	{
	CTestNumberOfBanks* self = new (ELeave) CTestNumberOfBanks(aTestName,
														   aTestType);
	return self;
	}

TVerdict CTestNumberOfBanks::DoTestL(CMidiClientUtility* aMidi)
	{
	// to do : test for standard & custom banks

	// currently using ETestOverflow to simulate no banks

	TVerdict ret = EPass;
	TInt expErr = KErrNone;

	// expected results
	switch(iTestType)
		{
		case ETestValid:
			expErr = KErrNone;
			break;
		case ETestNegative:
			expErr = KErrArgument;
			break;
		default:
			ERR_PRINTF1(_L("Error - invalid test step type"));
			return EInconclusive;
		}

	TInt numBanks = 0;
	TRAPD(err1, numBanks = aMidi->NumberOfBanksL(EFalse));
	if(err1 != expErr)
		{
		ERR_PRINTF3(_L("NumberOfBanksL(EFalse) gave error %d (expected %d)"), err1, expErr);
		ret = EFail;
		}
	else
		INFO_PRINTF2(_L("Number of Banks (standard) = %d"), numBanks);


	TRAPD(err2, numBanks = aMidi->NumberOfBanksL(ETrue));
	if(err1 != expErr)
		{
		ERR_PRINTF3(_L("NumberOfBanksL(ETrue) gave error %d (expected %d)"), err2, expErr);
		ret = EFail;
		}
	else
		INFO_PRINTF2(_L("Number of Banks (custom) = %d"), numBanks);

	return ret;
	}

//------------------------------------------------------------------


CTestGetBankId::CTestGetBankId(const TDesC& aTestName,
									 const TTestStepType aTestType)
	:CTestMmfMidiClntStep(aTestName, aTestType)
	{
	}

CTestGetBankId* CTestGetBankId::NewL(const TDesC& aTestName,
									   const TTestStepType aTestType)
	{
	CTestGetBankId* self = new (ELeave) CTestGetBankId(aTestName,
														   aTestType);
	return self;
	}

TVerdict CTestGetBankId::DoTestL(CMidiClientUtility* aMidi)
	{
	// currently using ETestOverflow to simulate no banks
	TVerdict ret = EPass;
	TInt expErr;
	TInt bankNum;

	// expected results
	switch(iTestType)
		{
		case ETestValid:
			expErr = KErrNone;
			bankNum = 54;
			break;
		case ETestNegative:
			expErr = KErrArgument;
			bankNum = -1;
			break;
		case ETestOverflow:
			expErr = KErrArgument;
			bankNum = 12000;
			break;
		default:
			ERR_PRINTF1(_L("Error - invalid test step type"));
			return EInconclusive;
		}

	TInt bankId = 0;
	TRAPD(err1, bankId = aMidi->GetBankIdL(EFalse, bankNum));
	if(err1 != expErr)
		{
		ERR_PRINTF4(_L("GetBankIdL(EFalse, %d) gave error %d (expected %d)"), bankNum, err1, expErr);
		ret = EFail;
		}
	else
		INFO_PRINTF3(_L("GetBankIdL(EFalse, %d) = %d"), bankNum, bankId);

	TRAPD(err2, bankId = aMidi->GetBankIdL(ETrue, bankNum));
	if(err2 != expErr)
		{
		ERR_PRINTF4(_L("GetBankIdL(ETrue, %d) gave error %d (expected %d)"), bankNum, err2, expErr);
		ret = EFail;
		}
	else
		INFO_PRINTF3(_L("GetBankIdL(ETrue, %d) = %d"), bankNum, bankId);

	return ret;
	}

//------------------------------------------------------------------


CTestLoadCustomBank::CTestLoadCustomBank(const TDesC& aTestName,
									 const TTestStepType aTestType)
	:CTestMmfMidiClntStep(aTestName, aTestType)
	{
	}

CTestLoadCustomBank* CTestLoadCustomBank::NewL(const TDesC& aTestName,
									 const TTestStepType aTestType)
	{
	CTestLoadCustomBank* self = new (ELeave) CTestLoadCustomBank(aTestName,
														   aTestType);
	return self;
	}

TVerdict CTestLoadCustomBank::DoTestL(CMidiClientUtility* aMidi)
	{
	TVerdict ret = EPass;
	TInt expErr;
	TFileName bankFileName;

	_LIT8(KBankFileName, "Bank file name");
	_LIT8(KBankFileNameInvalid, "Invalid file name");
	_LIT8(KBankFileNameUnsupported, "Unsupported file name");

	// expected results
	switch(iTestType)
		{
		case ETestValid:
			expErr = KErrNone;
			bankFileName.Copy(KBankFileName);
			break;
		case ETestNullMessage:
			expErr = KErrArgument;
			bankFileName.Copy(KNullDesC16);
			break;
		case ETestInvalidMessage:
			expErr = KErrArgument;
			bankFileName.Copy(KBankFileNameInvalid);
			break;
		case ETestUnsupported:
			expErr = KErrNotSupported;
			bankFileName.Copy(KBankFileNameUnsupported);
			break;
		default:
			ERR_PRINTF1(_L("Error - invalid test step type"));
			return EInconclusive;
		}

	TInt bankId = 0;

	TRAPD(err1, aMidi->LoadCustomBankL(bankFileName, bankId));
	INFO_PRINTF3(_L("LoadCustomBankL gave error %d (expected %d)"), err1, expErr);
	if(err1 != expErr)
		{
		ret = EFail;
		return ret;
		}
	// see if it's loaded
	TBool loaded = EFalse;
	TRAP(err1, loaded = aMidi->CustomBankLoadedL(bankId));
	if(err1 != KErrNone)
		{
		ERR_PRINTF2(_L("CustomBankLoadedL left with error %d"), err1);
		ret = EFail;
		return ret;
		}
	if(iTestType == ETestValid)
		{
		if(!loaded)
			{
			ERR_PRINTF1(_L("CustomBankLoadedL returned false"));
			ret = EFail;
			}
		}
	else
		{
		if(loaded)
			{
			ERR_PRINTF1(_L("CustomBankLoadedL returned true"));
			ret = EFail;
			}
		}

	return ret;
	}

//------------------------------------------------------------------


CTestUnloadCustomBank::CTestUnloadCustomBank(const TDesC& aTestName,
									 const TTestStepType aTestType)
	:CTestMmfMidiClntStep(aTestName, aTestType)
	{
	}

CTestUnloadCustomBank* CTestUnloadCustomBank::NewL(const TDesC& aTestName,
									 const TTestStepType aTestType)
	{
	CTestUnloadCustomBank* self = new (ELeave) CTestUnloadCustomBank(aTestName,
														   aTestType);
	return self;
	}

TVerdict CTestUnloadCustomBank::DoTestL(CMidiClientUtility* aMidi)
	{
	TVerdict ret = EPass;
	TInt expErr;
	TFileName bankFileName;

	_LIT8(KBankFileName, "Bank file name");
	bankFileName.Copy(KBankFileName);

	// expected results
	switch(iTestType)
		{
		case ETestValid:
			expErr = KErrNone;
			break;
		case ETestNegative:
			expErr = KErrNotFound;
			break;
		default:
			ERR_PRINTF1(_L("Error - invalid test step type"));
			return EInconclusive;
		}

	TInt bankId = 0;

	// load a known bank
	TRAPD(err, aMidi->LoadCustomBankL(bankFileName, bankId));
	if(err != KErrNone)
		{
		ERR_PRINTF2(_L("LoadCustomBankL gave error %d"), err);
		ret = EInconclusive;
		return ret;
		}

	// ensure it's loaded
	TBool loaded = EFalse;
	TRAP(err, loaded = aMidi->CustomBankLoadedL(bankId));
	if(err != KErrNone)
		{
		ERR_PRINTF2(_L("CustomBankLoadedL left with error %d"), err);
		ret = EInconclusive;
		return ret;
		}
	if(!loaded)
		{
		ERR_PRINTF1(_L("CustomBankLoadedL returned false"));
		ret = EInconclusive;
		return ret;
		}

	// unload the bank we know is loaded
	TRAP(err, aMidi->UnloadCustomBankL(bankId));
	if(err != KErrNone)
		{
		ERR_PRINTF2(_L("UnloadCustomBankL gave error %d"), err);
		ret = EFail;
		return ret;
		}

	// ensure it's not loaded
	TRAP(err, loaded = aMidi->CustomBankLoadedL(bankId));
	if(err != KErrNone)
		{
		ERR_PRINTF2(_L("CustomBankLoadedL left with error %d"), err);
		ret = EInconclusive;
		return ret;
		}
	if(loaded)
		{
		ERR_PRINTF1(_L("CustomBankLoadedL returned true -> bank not unloaded"));
		ret = EFail;
		return ret;
		}

	// if negative test, try and unload the same bank again
	if(iTestType == ETestValid)
		return ret;
	else
		{
		TRAP(err, aMidi->UnloadCustomBankL(bankId));
		INFO_PRINTF3(_L("UnloadCustomBankL gave error %d (expected %d)"), err, expErr);	
		if(err != expErr)
			ret = EFail;
		}

	return ret;
	}

//------------------------------------------------------------------


CTestUnloadAllCustomBanks::CTestUnloadAllCustomBanks(const TDesC& aTestName,
									 const TTestStepType aTestType)	
	:CTestMmfMidiClntStep(aTestName, aTestType)
	{
	}

CTestUnloadAllCustomBanks* CTestUnloadAllCustomBanks::NewL(const TDesC& aTestName,
									 const TTestStepType aTestType)	
	{
	CTestUnloadAllCustomBanks* self = new (ELeave) CTestUnloadAllCustomBanks(aTestName,
														   aTestType);
	return self;
	}

TVerdict CTestUnloadAllCustomBanks::DoTestL(CMidiClientUtility* aMidi)
	{
	TVerdict ret = EPass;
	TInt expErr;
	TFileName bankFileName;

	_LIT8(KBankFileName, "Bank file name");
	bankFileName.Copy(KBankFileName);

	// expected results
	switch(iTestType)
		{
		case ETestValid:
			expErr = KErrNone;
			break;
		case ETestNegative:
			expErr = KErrNotFound;
			break;
		default:
			ERR_PRINTF1(_L("Error - invalid test step type"));
			return EInconclusive;
		}

	TInt bankId = 0;

	// load a known bank
	TRAPD(err, aMidi->LoadCustomBankL(bankFileName, bankId));
	if(err != KErrNone)
		{
		ERR_PRINTF2(_L("LoadCustomBankL gave error %d"), err);
		ret = EInconclusive;
		return ret;
		}

	// ensure it's loaded
	TBool loaded = EFalse;
	TRAP(err, loaded = aMidi->CustomBankLoadedL(bankId));
	if(err != KErrNone)
		{
		ERR_PRINTF2(_L("CustomBankLoadedL left with error %d"), err);
		ret = EInconclusive;
		return ret;
		}
	if(!loaded)
		{
		ERR_PRINTF1(_L("CustomBankLoadedL returned false"));
		ret = EInconclusive;
		return ret;
		}

	// unload all
	TRAP(err, aMidi->UnloadAllCustomBanksL());
	if(err != KErrNone)
		{
		ERR_PRINTF2(_L("UnloadAllCustomBanksL gave error %d"), err);
		ret = EFail;
		return ret;
		}

	loaded = EFalse;
	// ensure our bank is not loaded
	TRAP(err, loaded = aMidi->CustomBankLoadedL(bankId));
	if(err != KErrNone)
		{
		ERR_PRINTF2(_L("CustomBankLoadedL left with error %d"), err);
		ret = EInconclusive;
		return ret;
		}
	if(loaded)
		{
		ERR_PRINTF1(_L("CustomBankLoadedL returned true -> bank not unloaded"));
		ret = EFail;
		return ret;
		}

	// if negative test, try and unload again
	if(iTestType == ETestValid)
		return ret;
	else
		{
		TRAP(err, aMidi->UnloadAllCustomBanksL());
		INFO_PRINTF3(_L("UnloadAllCustomBanksL gave error %d (expected %d)"), err, expErr);	
		if(err != expErr)
			ret = EFail;
		}

	return ret;
	}

//------------------------------------------------------------------


CTestNumberOfInstruments::CTestNumberOfInstruments(const TDesC& aTestName,
									 const TTestStepType aTestType)	
	:CTestMmfMidiClntStep(aTestName, aTestType)
	{
	}

CTestNumberOfInstruments* CTestNumberOfInstruments::NewL(const TDesC& aTestName,
									 const TTestStepType aTestType)	
	{
	CTestNumberOfInstruments* self = new (ELeave) CTestNumberOfInstruments(aTestName,
														   aTestType);
	return self;
	}

TVerdict CTestNumberOfInstruments::DoTestL(CMidiClientUtility* aMidi)
	{
	// TO DO : situation where no standard banks exist

	TVerdict ret = EPass;
	TInt expErr;
	TFileName bankFileName;

	_LIT8(KBankFileName, "Bank file name");
	bankFileName.Copy(KBankFileName);
	TInt bankId = 0;
	TInt numInstruments = 0;

	// expected results
	switch(iTestType)
		{
		case ETestValid:
			expErr = KErrNone;
			bankId = 0;
			break;
		case ETestNegative:
			expErr = KErrNotFound;
			bankId = 0;
			break;
		case ETestInvalidId:
			expErr = KErrArgument;
			bankId = -2;
			break;
		default:
			ERR_PRINTF1(_L("Error - invalid test step type"));
			return EInconclusive;
		}

	// check a known bank - standard
	// *** TO DO : this will cause a failure with ETestNegative. To be re-done
	// for no standard banks present
	TRAPD(err, numInstruments = aMidi->NumberOfInstrumentsL(bankId, EFalse));
	if(err != expErr)
		{
		ERR_PRINTF3(_L("NumberOfInstrumentsL(bankId, EFalse) gave error %d (expected %d)"), err, expErr);
		ret = EFail;
		return ret;
		}
	INFO_PRINTF2(_L("NumberOfInstrumentsL(bankId, EFalse) = %d"), numInstruments);

	if(iTestType != ETestInvalidMessage)
		{	
		// load a known bank - custom
		TRAPD(err, aMidi->LoadCustomBankL(bankFileName, bankId));
		if(err != KErrNone)
			{
			ERR_PRINTF2(_L("LoadCustomBankL left with error %d"), err);
			ret = EInconclusive;
			return ret;
			}
		// ensure it's loaded
		TBool loaded = EFalse;
		TRAP(err, loaded = aMidi->CustomBankLoadedL(bankId));
		if(err != KErrNone)
			{
			ERR_PRINTF2(_L("CustomBankLoadedL left with error %d"), err);
			ret = EInconclusive;
			return ret;
			}
		if(!loaded)
			{
			ERR_PRINTF1(_L("CustomBankLoadedL returned false -> bank not loaded"));
			ret = EInconclusive;
			return ret;
			}
		}
	
	// check the custom bank
	// NB if test type is InvalidId then we haven't bothered to load anything
	TRAP(err, aMidi->NumberOfInstrumentsL(bankId, ETrue));
	if(err != expErr)
		{
		ERR_PRINTF2(_L("NumberOfInstrumentsL(bankId, ETrue) gave error %d"), err);
		ret = EFail;
		return ret;
		}
	INFO_PRINTF2(_L("NumberOfInstrumentsL(bankId, ETrue) = %d"), numInstruments);

	if(iTestType == ETestNegative)
		{
		// negative test :- unload all, then check it again
		TRAP(err, aMidi->UnloadAllCustomBanksL());
		if(err != KErrNone)
			{
			ERR_PRINTF2(_L("UnloadAllCustomBanksL gave error %d"), err);
			ret = EInconclusive;
			return ret;
			}
		// check it. it should leave
		TRAP(err, aMidi->NumberOfInstrumentsL(bankId, ETrue));
		if(err != expErr)
			{
			ERR_PRINTF3(_L("NumberOfInstrumentsL gave error %d (expected %d)"), err, expErr);	
			ret = EFail;
			return ret;
			}
		}

	return ret;	
	}

//------------------------------------------------------------------


CTestGetInstrumentId::CTestGetInstrumentId(const TDesC& aTestName,
									 const TTestStepType aTestType)	
	:CTestMmfMidiClntStep(aTestName, aTestType)
	{
	}

CTestGetInstrumentId* CTestGetInstrumentId::NewL(const TDesC& aTestName,
									 const TTestStepType aTestType)	
	{
	CTestGetInstrumentId* self = new (ELeave) CTestGetInstrumentId(aTestName,
														   aTestType);
	return self;
	}

TVerdict CTestGetInstrumentId::DoTestL(CMidiClientUtility* aMidi)
	{
	// TO DO : situation where no standard banks exist

	TVerdict ret = EPass;
	TInt expErr;
	TFileName bankFileName;

	_LIT8(KBankFileName, "Bank file name");
	bankFileName.Copy(KBankFileName);
	TInt bankId = 0;
	TInt instrIndex = 0;
	TInt instrId = 0;

	// expected results
	switch(iTestType)
		{
		case ETestValid:
			expErr = KErrNone;
			bankId = 0;
			instrIndex = 0;
			break;
		case ETestNegative:
			expErr = KErrNotFound;
			bankId = 0;
			instrIndex = 0;
			break;
		case ETestInvalidId:
			expErr = KErrArgument;
			bankId = -2;
			instrIndex = 0;
			break;
		case ETestInvalidIndex:
			expErr = KErrArgument;
			bankId = 0;
			instrIndex = -2;
			break;
		default:
			ERR_PRINTF1(_L("Error - invalid test step type"));
			return EInconclusive;
		}

	// check a known bank - standard
	// *** TO DO : this will cause a failure with ETestNegative. To be re-done
	// for no standard banks present
	TRAPD(err, instrId = aMidi->GetInstrumentIdL(bankId, EFalse, instrIndex));
	if(err != expErr)
		{
		ERR_PRINTF3(_L("GetInstrumentIdL(bankId, EFalse, instrIndex) gave error %d (expected %d)"), err, expErr);
		ret = EFail;
		return ret;
		}
	INFO_PRINTF2(_L("GetInstrumentIdL(bankId, EFalse, instrIndex) = %d"), instrId);

	if(iTestType != ETestInvalidId && iTestType != ETestInvalidIndex)
		{	
		// load a known bank - custom
		TRAPD(err, aMidi->LoadCustomBankL(bankFileName, bankId));
		if(err != KErrNone)
			{
			ERR_PRINTF2(_L("LoadCustomBankL left with error %d"), err);
			ret = EInconclusive;
			return ret;
			}
		// ensure it's loaded
		TBool loaded = EFalse;
		TRAP(err, loaded = aMidi->CustomBankLoadedL(bankId));
		if(err != KErrNone)
			{
			ERR_PRINTF2(_L("CustomBankLoadedL left with error %d"), err);
			ret = EInconclusive;
			return ret;
			}
		if(!loaded)
			{
			ERR_PRINTF1(_L("CustomBankLoadedL returned false -> bank not loaded"));
			ret = EInconclusive;
			return ret;
			}
		}
	
	// check the custom bank
	// NB if test type is InvalidId/Index then we haven't bothered to load anything
	TRAP(err, instrId = aMidi->GetInstrumentIdL(bankId, ETrue, instrIndex));
	if(err != expErr)
		{
		ERR_PRINTF2(_L("GetInstrumentIdL(bankId, ETrue, instrIndex) gave error %d"), err);
		ret = EFail;
		return ret;
		}
	INFO_PRINTF2(_L("GetInstrumentIdL(bankId, ETrue, instrIndex) = %d"), instrId);

	if(iTestType == ETestNegative)
		{
		// negative test :- unload all, then check it again
		TRAP(err, aMidi->UnloadAllCustomBanksL());
		if(err != KErrNone)
			{
			ERR_PRINTF2(_L("UnloadAllCustomBanksL gave error %d"), err);
			ret = EInconclusive;
			return ret;
			}
		// check it. it should leave
		TRAP(err, instrId = aMidi->GetInstrumentIdL(bankId, ETrue, instrIndex));
		if(err != expErr)
			{
			ERR_PRINTF3(_L("GetInstrumentIdL gave error %d (expected %d)"), err, expErr);	
			ret = EFail;
			return ret;
			}
		}

	return ret;	
	}

//Check this
//Change it to return EFail for failing negative test
//------------------------------------------------------------------
CTestReturnsInstrumentName::CTestReturnsInstrumentName(const TDesC& aTestName,
									 const TTestStepType aTestType)	
	:CTestMmfMidiClntStep(aTestName, aTestType)
	{
	}

CTestReturnsInstrumentName* CTestReturnsInstrumentName::NewL(const TDesC& aTestName,
									 const TTestStepType aTestType)	
	{
	CTestReturnsInstrumentName* self = new (ELeave) CTestReturnsInstrumentName(aTestName,
														   aTestType);
	return self;
	}

TVerdict CTestReturnsInstrumentName::DoTestL(CMidiClientUtility* aMidi)
	{
	TVerdict ret = EPass;
	TInt expErr;
	//TFileName bankFileName;

	TInt bankId = 0;
	TInt instrId = 0;
	HBufC* instrName = NULL;

	// expected results
	switch(iTestType)
		{
		case ETestValid:
			expErr = KErrNone;
			bankId = 0;
			instrId = 89;
			break;
		case ETestNegative:
			expErr = KErrArgument;
			bankId = 0;
			instrId = -241;
			break;
		default:
			ERR_PRINTF1(_L("Error - invalid test step type"));
			return EInconclusive;
		}

	// check a known bank - standard
	// *** TO DO : custom?

	TRAPD(err, instrName = aMidi->InstrumentNameL(bankId, EFalse, instrId));
	INFO_PRINTF3(_L("InstrumentNameL(bankId, EFalse, instrId) gave error %d (expected %d)"), err, expErr);
	if(err != expErr)
		{
		//ret = EInconclusive;
		ret = EFail;
		return ret;
		}
	// don't try and do this for negative test! it'll panic
	if(iTestType == ETestValid)
		INFO_PRINTF2(_L("InstrumentNameL(bankId, EFalse, instrId) = \'%S\'"), instrName);

	delete instrName;
	return ret;
	}



//------------------------------------------------------------------
// Tests that the GetInstrumentL() doesn't return KErrArgument when the
// channel value is between 0 and 15, but it will return KErrArgument 
// when the channel value is out of range

CTestGetInstrument::CTestGetInstrument(const TDesC& aTestName,
									 const TTestStepType aTestType)	
	:CTestMmfMidiClntStep(aTestName, aTestType)
	{
	}

CTestGetInstrument* CTestGetInstrument::NewL(const TDesC& aTestName,
									 const TTestStepType aTestType)	
	{
	CTestGetInstrument* self = new (ELeave) CTestGetInstrument(aTestName,
														   aTestType);
	return self;
	}

TVerdict CTestGetInstrument::DoTestL(CMidiClientUtility* aMidi)
	{
	TVerdict ret = EPass;
	TInt instrumentId = 0;
	TInt bankId = 0;
	TInt validChannelId = 0;
	TInt invalidChannelId = 16;

	TRAPD(err, aMidi->GetInstrumentL(validChannelId, instrumentId, bankId));

	if(err == KErrArgument)
		{
		ERR_PRINTF2(_L("GetInstrumentL(validChannelId, ...) gave an error = %d"), err);
		ret = EFail;
		return ret;
		}
		
	TRAPD(err1, aMidi->GetInstrumentL(invalidChannelId, instrumentId, bankId));

	if(err1 != KErrArgument)
		{
		ERR_PRINTF2(_L("GetInstrumentL(invalidChannelId, ...) gave an error = %d"), err);
		ret = EFail;
		return ret;
		}	

	return ret;	
	}

//------------------------------------------------------------------

CTestSetInstrument::CTestSetInstrument(const TDesC& aTestName,
									 const TTestStepType aTestType)	
	:CTestMmfMidiClntStep(aTestName, aTestType)
	{
	}

CTestSetInstrument* CTestSetInstrument::NewL(const TDesC& aTestName,
									 const TTestStepType aTestType)	
	{
	CTestSetInstrument* self = new (ELeave) CTestSetInstrument(aTestName,
														   aTestType);
	return self;
	}

TVerdict CTestSetInstrument::DoTestL(CMidiClientUtility* aMidi)
	{
	TVerdict ret = EPass;
	TInt expErr;
	//TFileName bankFileName;

	TInt bankId = 0;
	TInt channelId = 0;
	TInt instrId = 0;

	// expected results
	switch(iTestType)
		{
		case ETestValid:
			expErr = KErrNone;
			bankId = 0;
			channelId = 0;
			instrId = 89;
			break;
		case ETestInvalidChannel:
			expErr = KErrNotFound;
			bankId = 0;
			channelId = -2;
			instrId = 89;
			break;
		case ETestInvalidId:
			expErr = KErrNotFound;
			bankId = 0;
			channelId = 0;
			instrId = -241;
			break;
		default:
			ERR_PRINTF1(_L("Error - invalid test step type"));
			return EInconclusive;
		}

	// *** TO DO : custom?

	TRAPD(err, aMidi->SetInstrumentL(channelId, bankId, instrId));
	INFO_PRINTF3(_L("SetInstrumentL(channelId, bankId, instrId) gave error %d (expected %d)"), err, expErr);
	if(err != expErr)
		{
		ret = EFail;
		return ret;
		}

	return ret;
	}

//------------------------------------------------------------------


CTestLoadCustomInstrument::CTestLoadCustomInstrument(const TDesC& aTestName,
									 const TTestStepType aTestType)	
	:CTestMmfMidiClntStep(aTestName, aTestType)
	{
	}

CTestLoadCustomInstrument* CTestLoadCustomInstrument::NewL(const TDesC& aTestName,
									 const TTestStepType aTestType)	
	{
	CTestLoadCustomInstrument* self = new (ELeave) CTestLoadCustomInstrument(aTestName,
														   aTestType);
	return self;
	}

TVerdict CTestLoadCustomInstrument::DoTestL(CMidiClientUtility* aMidi)
	{
	// TO DO : need instrument file which we can load, with known bank and instrument IDs
	// Negative tests to be done properly.

	TVerdict ret = EPass;
	TInt expErr;

	_LIT(KInstrumentFile, "Instrument File Name");	// *** to change
	_LIT(KBadInstrumentFile, "Bad Instrument File Name");	// *** to change
	TFileName instFileName;

	TInt fileBankId = 90;
	TInt fileInstrId = 91;
	TInt bankId = 92;
	TInt instrId = 93;

	// expected results
	switch(iTestType)
		{
		case ETestValid:
			expErr = KErrNone;
			instFileName.Copy(KInstrumentFile);
			break;
		case ETestInvalidId:
			expErr = KErrArgument;
			instFileName.Copy(KInstrumentFile);
			fileBankId = -2;
			break;
		case ETestAlreadyLoaded:
			// ** to do : load once and attempt to load again.
			// when instrument data files are available, this will work
			instFileName.Copy(KInstrumentFile);
			expErr = KErrInUse;
			break;
		case ETestUnsupported:	// ** to define test data for this
			instFileName.Copy(KBadInstrumentFile);
			expErr = KErrNotSupported;
			break;
		default:
			ERR_PRINTF1(_L("Error - invalid test step type"));
			return EInconclusive;
		}

	// if testing already loaded, preload the instrument here
	if(iTestType == ETestAlreadyLoaded)
		{
		TRAPD(err, aMidi->LoadCustomInstrumentL(instFileName, fileBankId, fileInstrId, bankId, instrId));
		INFO_PRINTF2(_L("Preload : LoadCustomInstrumentL gave error %d"), err);
		if(err != KErrNone)
			{
			ret = EInconclusive;
			return ret;
			}
		}

	// load the instrument for real
	TRAPD(err, aMidi->LoadCustomInstrumentL(instFileName, fileBankId, fileInstrId, bankId, instrId));
	INFO_PRINTF3(_L("LoadCustomInstrumentL gave error %d (expected %d)"), err, expErr);
	if(err != expErr)
		{
		ret = EFail;
		return ret;
		}

	return ret;
	}

	
//------------------------------------------------------------------

CTestUnloadCustomInstrument::CTestUnloadCustomInstrument(const TDesC& aTestName,
									 const TTestStepType aTestType)	
	:CTestMmfMidiClntStep(aTestName, aTestType)
	{
	}

CTestUnloadCustomInstrument* CTestUnloadCustomInstrument::NewL(const TDesC& aTestName,
									 const TTestStepType aTestType)	
	{
	CTestUnloadCustomInstrument* self = new (ELeave) CTestUnloadCustomInstrument(aTestName,
														   aTestType);
	return self;
	}

TVerdict CTestUnloadCustomInstrument::DoTestL(CMidiClientUtility* aMidi)
	{
	TVerdict ret = EPass;
	TInt expErr;

	_LIT(KInstrumentFile, "Instrument File Name");	// *** to change
	TFileName instFileName;

	TInt fileBankId = 90;
	TInt fileInstrId = 91;
	TInt bankId = 92;
	TInt instrId = 93;

	// expected results
	switch(iTestType)
		{
		case ETestValid:
			expErr = KErrNone;
			instFileName.Copy(KInstrumentFile);
			break;
		case ETestInvalidId:
			expErr = KErrNotFound;
			instFileName.Copy(KInstrumentFile);
			bankId = -2;
			break;
		case ETestNotLoaded:
			expErr = KErrNotFound;
			instFileName.Copy(KInstrumentFile);
			break;
		case ETestNotUnloadable:	// ** TO DO : define test data for this
			expErr = KErrNotSupported;
			break;
		default:
			ERR_PRINTF1(_L("Error - invalid test step type"));
			return EInconclusive;
		}

	// preload the instrument here
	if(iTestType == ETestValid)
		{
		TRAPD(err, aMidi->LoadCustomInstrumentL(instFileName, fileBankId, fileInstrId, bankId, instrId));
		INFO_PRINTF2(_L("Preload : LoadCustomInstrumentL gave error %d"), err);
		if(err != KErrNone)
			{
			ret = EInconclusive;
			return ret;
			}
		}

	// now unload it
	TRAPD(err, aMidi->UnloadCustomInstrumentL(bankId, instrId));
	INFO_PRINTF3(_L("UnloadCustomInstrumentL gave error %d (expected %d)"), err, expErr);
	if(err != expErr)
		{
		ret = EFail;
		return ret;
		}

	return ret;
	}

//------------------------------------------------------------------


CTestPercussionKeyName::CTestPercussionKeyName(const TDesC& aTestName,
									 const TTestStepType aTestType)	
	:CTestMmfMidiClntStep(aTestName, aTestType)
	{
	}

CTestPercussionKeyName* CTestPercussionKeyName::NewL(const TDesC& aTestName,
									 const TTestStepType aTestType)	
	{
	CTestPercussionKeyName* self = new (ELeave) CTestPercussionKeyName(aTestName,
														   aTestType);
	return self;
	}

TVerdict CTestPercussionKeyName::DoTestL(CMidiClientUtility* aMidi)
	{
	// *** NB the actual arguments do not match those in the current API or test specs
	TVerdict ret = EPass;
	TInt expErr;
	//TFileName bankFileName;

	TInt bankId = 0;
	TInt instrId = 89;
	TInt keyId = 0;
	HBufC* keyName = NULL;

	// expected results
	switch(iTestType)
		{
		case ETestValid:
			expErr = KErrNone;
			break;
		case ETestInvalidId:
			expErr = KErrNotFound;
			keyId = -2;
			break;
		default:
			ERR_PRINTF1(_L("Error - invalid test step type"));
			return EInconclusive;
		}

	// check a known bank - standard
	// *** TO DO : custom?

	TRAPD(err, keyName = aMidi->PercussionKeyNameL(keyId, bankId, EFalse, instrId));
	INFO_PRINTF3(_L("PercussionKeyNameL gave error %d (expected %d)"), err, expErr);
	if(err != expErr)
		{
		ret = EFail;
		return ret;
		}
	// don't try and do this for negative test! it'll panic
	if(iTestType == ETestValid)
		INFO_PRINTF2(_L("PercussionKeyNameL(keyId, bankId, EFalse, instrId) = \'%S\'"), keyName);

	delete keyName;
	return ret;
	}

//Check this//
//Not sure how to implement 'no opened resource'
//------------------------------------------------------------------
CTestStopTime::CTestStopTime(const TDesC& aTestName,
									 const TTestStepType aTestType)	
	:CTestMmfMidiClntStep(aTestName, aTestType)
	{
	}

CTestStopTime* CTestStopTime::NewL(const TDesC& aTestName,
									 const TTestStepType aTestType)	
	{
	CTestStopTime* self = new (ELeave) CTestStopTime(aTestName,
														   aTestType);
	return self;
	}

TVerdict CTestStopTime::DoTestL(CMidiClientUtility* aMidi)
	{
	TVerdict ret = EPass;
	TInt expErr;
	TTimeIntervalMicroSeconds stopTime(0);

	// expected results
	switch(iTestType)
		{
		case ETestValid:
			expErr = KErrNone;
			break;

		// NB no negative test specified
		//Not sure how to implement 'no opened resource'
		case ETestNoResource:
			expErr = KErrNotReady;
			break;

		default:
			ERR_PRINTF1(_L("Error - invalid test step type"));
			return EInconclusive;
		}

	TRAPD(err, aMidi->StopTimeL(stopTime));
	INFO_PRINTF3(_L("StopTimeL gave error %d (expected %d)"), err, expErr);
	if(err != expErr)
		{
		ret = EFail;
		return ret;
		}
	INFO_PRINTF2(_L("StopTime = %ld"), I64INT(stopTime.Int64()));
	return ret;
	}


//------------------------------------------------------------------


CTestSetStopTime::CTestSetStopTime(const TDesC& aTestName,
									 const TTestStepType aTestType)	
	:CTestMmfMidiClntStep(aTestName, aTestType)
	{
	}

CTestSetStopTime* CTestSetStopTime::NewL(const TDesC& aTestName,
									 const TTestStepType aTestType)	
	{
	CTestSetStopTime* self = new (ELeave) CTestSetStopTime(aTestName,
														   aTestType);
	return self;
	}

TVerdict CTestSetStopTime::DoTestL(CMidiClientUtility* aMidi)
	{
	TVerdict ret = EPass;
	TInt expErr;
	TTimeIntervalMicroSeconds stopTime(999);

	// expected results
	switch(iTestType)
		{
		case ETestValid:
			expErr = KErrNone;
			break;
		case ETestOutOfRangeHigh:
			stopTime = 999999999;
			expErr = KErrArgument;
			break;
		case ETestOutOfRangeLow:
			stopTime = -1;
			expErr = KErrArgument;
			break;
		default:
			ERR_PRINTF1(_L("Error - invalid test step type"));
			return EInconclusive;
		}

	TRAPD(err, aMidi->SetStopTimeL(stopTime));
	INFO_PRINTF3(_L("SetStopTimeL gave error %d (expected %d)"), err, expErr);
	if(err != expErr)
		{
		ret = EFail;
		return ret;
		}

	// check set correctly if a valid test
	if (iTestType == ETestValid)
		{
		TTimeIntervalMicroSeconds newStopTime(999);
		TRAP(err, aMidi->StopTimeL(newStopTime));
		INFO_PRINTF3(_L("StopTimeL gave error %d (expected %d)"), err, expErr);
		
		if(err != expErr)
			{
			ret = EInconclusive;
			return ret;
			}
		INFO_PRINTF2(_L("StopTime = %ld"), I64INT(newStopTime.Int64()));
		
		if(newStopTime != stopTime)
			{
			ERR_PRINTF2(_L("Error : expected %ld"), I64INT(stopTime.Int64()));
			ret = EFail;
			return ret;
			}
		}

	return ret;
	}

//------------------------------------------------------------------


CTestSetRepeats::CTestSetRepeats(const TDesC& aTestName,
									 const TTestStepType aTestType)	
	:CTestMmfMidiClntStep(aTestName, aTestType)
	{
	}

CTestSetRepeats* CTestSetRepeats::NewL(const TDesC& aTestName,
									 const TTestStepType aTestType)	
	{
	CTestSetRepeats* self = new (ELeave) CTestSetRepeats(aTestName,
														   aTestType);
	return self;
	}

TVerdict CTestSetRepeats::DoTestL(CMidiClientUtility* aMidi)
	{
	TVerdict ret = EPass;
	TInt expErr;
	TInt repeats = 0;
	TTimeIntervalMicroSeconds silentTime(1000000);

	// expected results
	switch(iTestType)
		{
		case ETestValid:
			expErr = KErrNone;
			break;
			// ** NB these are testing ONE out-of-range argument at a time. Suggest
			// we modify the test spec to go with this?
		case ETestOutOfRange:
			expErr = KErrArgument;
			//repeats = 9999;			// ** XXX : max. range of repeats?
			repeats = -1;
			silentTime = 1000000;
			break;
		default:
			ERR_PRINTF1(_L("Error - invalid test step type"));
			return EInconclusive;
		}

	TRAPD(err, aMidi->SetRepeatsL(repeats, silentTime));

	// check set correctly if a valid test
	if (iTestType == ETestValid)
		{
		// check the repeats value
		TInt currRepeats = -1;
		currRepeats = aMidi->GetRepeats();
		
		INFO_PRINTF2(_L("Repeats = %d"), currRepeats);
		
		if(currRepeats != repeats)
			{
			ERR_PRINTF2(_L("Error : expected %d"), currRepeats);
			ret = EFail;
			return ret;
			}
		}	

	INFO_PRINTF3(_L("SetRepeatsL gave error %d (expected %d)"), err, expErr);
	if(err != expErr)
		{
		ret = EFail;
		return ret;
		}

	return ret;
	}

//------------------------------------------------------------------
// This test case has two steps:
// 1. High Polyphony Song vs. Low Polyphony Midi Utility
// - a test file has exactly 57 voices from beginning to end
// - during the instantiation of utility, the Maximum Polyphony is default to 48 implicitly.
// For this case:
//		MaxPolyphonyL() return value is 48 
//		PolyphonyL()    return value is 48 (number of active voice the engine is playing now, 
//											this is being limited by the MaxPolyphonyL() number)
// 
//
// 2. Low Polyphony Song vs. High Polyphony Midi Utility 
// - a test file has exactly 57 voices from beginning to end
// - during the instantiation of utility, the Maximum Polyphony is default to 60 implicitly
// For this case:
//		MaxPolyphonyL() return value is 60 
//		PolyphonyL()    return value is 57 

// constants for the test case
const TUint8 KMaxPolyphonyValue1 = 48;
const TUint8 KMaxPolyphonyValue2 = 60;
const TUint8 KPolyphonyValue = 57;

CTestPolyphony::CTestPolyphony(const TDesC& aTestName,
									 const TTestStepType aTestType)	
	:CTestMmfMidiClntStep(aTestName, aTestType)
	{
	}

CTestPolyphony* CTestPolyphony::NewL(const TDesC& aTestName,
									 const TTestStepType aTestType)	
	{
	CTestPolyphony* self = new (ELeave) CTestPolyphony(aTestName,
														   aTestType);
	return self;
	}

TVerdict CTestPolyphony::DoTestL(CMidiClientUtility* aMidi)
	{
	TVerdict ret = EPass;
	TInt poly = 0; 
	TInt maxPoly = 0;
	TInt expErr = 0;

	// expected results
	switch(iTestType)
		{
		case ETestValid:
			expErr = KErrNone;
			break;
			// NB no negative test specified
		default:
			ERR_PRINTF1(_L("Error - invalid test step type"));
			return EInconclusive;
		}

	// set the maxPolyphony to KMaxPolyphponyValue1
	TRAPD(err, aMidi->SetMaxPolyphonyL(KMaxPolyphonyValue1));
	if (err != expErr)
		{
		INFO_PRINTF2(_L("SetMaxPolyphonyL failed (error = %d)"), err);
		ret = EFail;
		return ret;
		}		
	
	// verify that the maxPolyphony value was correctly set
	TRAPD(err1, maxPoly = aMidi->MaxPolyphonyL());
	if (err1 != expErr)
		{
		INFO_PRINTF2(_L("MaxPolyphonyL failed (error = %d)"), err);
		ret = EFail;
		return ret;
		}
	INFO_PRINTF3(_L("The maximum number of simultaneous voices the engine can handle : %d (expected %d)"), maxPoly, KMaxPolyphonyValue1);

	// get the number of the currently active voices. This value is set as 
	// KPolyphonyValue, but because the max number of voices that can be handled by the 
	// engine is smaller than the currently active voce, PolyphonyL() will
	// return the MaxPolyphony() value
	TRAPD(err2, poly = aMidi->PolyphonyL());

	if (err2 != expErr)
		{
		INFO_PRINTF2(_L("PolyphonyL failed (error = %d)"), err);
		ret = EFail;
		return ret;
		}
	
	INFO_PRINTF3(_L("The number of currently active voices is: %d (expected %d)"), poly, KMaxPolyphonyValue1);
	INFO_PRINTF1(_L("----------------------------------------------------------"));
	
	// set the maxPolyphony to KMaxPolyphonyValue2
	TRAPD(err3, aMidi->SetMaxPolyphonyL(KMaxPolyphonyValue2));
	if (err3 != expErr)
		{
		INFO_PRINTF2(_L("SetMaxPolyphonyL failed (error = %d)"), err);
		ret = EFail;
		return ret;
		}		
	
	// verify that the maxPolyphony value was correctly set
	TRAPD(err4, maxPoly = aMidi->MaxPolyphonyL());
	if (err4 != expErr)
		{
		INFO_PRINTF2(_L("MaxPolyphonyL failed (error = %d)"), err);
		ret = EFail;
		return ret;
		}
	INFO_PRINTF3(_L("The maximum number of simultaneous voices the engine can handle : %d (expected %d)"), maxPoly, KMaxPolyphonyValue2);

	// get the number of the currently active voices. This value is set as 
	// KPolyphonyValue, but because the max number of voices that can be handled by the 
	// engine is smaller than the currently active voce, PolyphonyL() will
	// return the MaxPolyphony() value
	TRAPD(err5, poly = aMidi->PolyphonyL());
		if (err5 != expErr)
		{
		INFO_PRINTF2(_L("PolyphonyL failed (error = %d)"), err);
		ret = EFail;
		return ret;
		}
		INFO_PRINTF3(_L("The number of currently active voices is: %d (expected %d)"), poly, KPolyphonyValue);
	
	return ret;
	}

//Check this (0086) //
//Not sure how to implement 'no opened resource'
//------------------------------------------------------------------


CTestChannelsSupported::CTestChannelsSupported(const TDesC& aTestName,
									 const TTestStepType aTestType)	
	:CTestMmfMidiClntStep(aTestName, aTestType)
	{
	}

CTestChannelsSupported* CTestChannelsSupported::NewL(const TDesC& aTestName,
									 const TTestStepType aTestType)	
	{
	CTestChannelsSupported* self = new (ELeave) CTestChannelsSupported(aTestName,
														   aTestType);
	return self;
	}

TVerdict CTestChannelsSupported::DoTestL(CMidiClientUtility* aMidi)
	{
	TVerdict ret = EPass;
	TInt expErr;
	TInt numChannels = 0;

	// expected results
	switch(iTestType)
		{
		case ETestValid:
			expErr = KErrNone;
			break;
		
		// NB no negative test specified
		
		//Not sure how to implement 'no opened resource'
		case ETestNoResource:
			expErr = KErrNotReady;
			break;

		default:
			ERR_PRINTF1(_L("Error - invalid test step type"));
			return EInconclusive;
		}

	TRAPD(err, numChannels = aMidi->ChannelsSupportedL());
	INFO_PRINTF3(_L("ChannelsSupportedL gave error %d (expected %d)"), err, expErr);
	if(err != expErr)
		{
		ret = EFail;
		return ret;
		}
	INFO_PRINTF2(_L("Channels supported = %d"), numChannels);
	return ret;
	}

//------------------------------------------------------------------
// Checks the ChannelVolumeL(...) method behaivior
// 		- if the argument passed in is a valid channel, the test should 
//		not return any error code, but should return the expected volume
//		value
//		- if the argument passed in is an invalid channel, the test should 
//		return KErrArgument

CTestReturnChannelVolume::CTestReturnChannelVolume(const TDesC& aTestName, const TTestStepType aTestType)
	:CTestMmfMidiClntStep(aTestName, aTestType)
	{
	}

CTestReturnChannelVolume* CTestReturnChannelVolume::NewL(const TDesC& aTestName, const TTestStepType aTestType)
	{
	CTestReturnChannelVolume* self = new(ELeave) CTestReturnChannelVolume(aTestName, aTestType);
	return self;
	}

TVerdict CTestReturnChannelVolume::DoTestL(CMidiClientUtility* aMidi)
	{
	TInt channel = 2;
	TInt volume = 0;
	
	INFO_PRINTF1(_L("CMidiClientUtility: Return channel volume"));
	
	// positive test
	if (iTestType == ETestValid)
		{
		// if the test is valid check that the value returned for the
		// volume is correct
		TInt chanVolume = aMidi->MaxChannelVolumeL() - 10;
		TRAPD(err1, aMidi->SetChannelVolumeL(channel, chanVolume));
		if (err1 != KErrNone)
			{
			ERR_PRINTF2(_L("SetChannelVolumeL gave error %d"),err1);
			return EFail;
			}	
					
		TRAPD(err, volume = aMidi->ChannelVolumeL(channel));	
		if (err != KErrNone)
			{
			ERR_PRINTF2(_L("ChannelVolumeL gave error %d (expected KErrNone)"),err);
			return EFail;
			}
		
		if (volume != chanVolume)
			{
			ERR_PRINTF3(_L("ChannelVolumeL retrieve volume value %d (expected %d)"),volume, chanVolume);
			return EFail;
			}
		return EPass;
		}

	// negative test
	if (iTestType == ETestInvalidChannel)
		{
			channel = -1;
			TRAPD(err, aMidi->ChannelVolumeL(channel));
			if (err != KErrArgument)	
				{
					ERR_PRINTF2(_L("ChannelVolumeL gave error %d (expected KErrArgument)"),err);
					return EFail;
				}
			return EPass;
		}
	
	ERR_PRINTF1(_L("Error - invalid test step type"));
	return EInconclusive;
	}


//Check This//
//------------------------------------------------------------------
CTestMaxChannelVolume::CTestMaxChannelVolume(const TDesC& aTestName, const TTestStepType aTestType)
	:CTestMmfMidiClntStep(aTestName, aTestType)
	{
	}

CTestMaxChannelVolume* CTestMaxChannelVolume::NewL(const TDesC& aTestName, const TTestStepType aTestType)
	{
	CTestMaxChannelVolume* self = new(ELeave) CTestMaxChannelVolume(aTestName, aTestType);
	return self;
	}

TVerdict CTestMaxChannelVolume::DoTestL(CMidiClientUtility* aMidi)
	{
	TVerdict ret = EPass;
	TInt expErr = KErrNone;
	
	INFO_PRINTF1(_L("CMidiClientUtility: Return Maximum channel volume"));
	// expected results
	switch(iTestType)
		{
		case ETestValid:
			expErr = KErrNone;
			break;

		default:
			ERR_PRINTF1(_L("Error - invalid test step type"));
			return EInconclusive;
		}
//	TReal32 volume = 0;
//	TRAPD(err, volume = aMidi->MaxChannelVolumeL() );
	TRAPD(err, aMidi->MaxChannelVolumeL() );	// EABI warning removal

	if (expErr != err)
		{
		ERR_PRINTF3(_L("MaxChannelVolumeL gave error %d (expected %d)"),err, expErr);
		ret = EFail;
		}
	else
		INFO_PRINTF3(_L("MaxChannelVolumeL %d = %d "),err ,expErr);

	return ret;
	}

//------------------------------------------------------------------

CTestSetChannelVolume::CTestSetChannelVolume(const TDesC& aTestName, const TTestStepType aTestType)
	:CTestMmfMidiClntStep(aTestName, aTestType)
	{
	}

CTestSetChannelVolume* CTestSetChannelVolume::NewL(const TDesC& aTestName, const TTestStepType aTestType)
	{
	CTestSetChannelVolume* self = new(ELeave) CTestSetChannelVolume(aTestName, aTestType);
	return self;
	}

TVerdict CTestSetChannelVolume::DoTestL(CMidiClientUtility* aMidi)
	{
	TVerdict ret = EPass;
	TInt expErr = KErrNone;
	TInt channel = 0;
	TReal32 volume = 0;
	
	INFO_PRINTF1(_L("CMidiClientUtility: Set channel volume"));
	// expected results
	switch(iTestType)
		{
		case ETestValid:
			expErr = KErrNone;
			break;
		case ETestInvalidChannelOutOfRange:
			expErr = KErrArgument;
			channel = 16;
			volume = 10;
			break;
		case ETestVolumeHighOutOfRange:
			expErr = KErrArgument;
			channel = 0;
			volume = aMidi->MaxChannelVolumeL()+1;
			break;
		case ETestVolumeLowOutOfRange:
			expErr = KErrNone;	//real controller may return KErrArgument if out of range
			channel = 0;
			volume = -1;
			break;
		default:
			ERR_PRINTF1(_L("Error - invalid test step type"));
			return EInconclusive;
		}

	TRAPD(err, aMidi->SetChannelVolumeL(channel, volume));
	if (expErr != err)
		{
		ERR_PRINTF3(_L("SetChannelVolumeL gave error %d (expected %d)"),err, expErr);
		ret = EFail;
		}
	else
		INFO_PRINTF3(_L("SetChannelVolumeL %d = %d"),err ,expErr);

	return ret;
	}

//Check this
//------------------------------------------------------------------
CTestSetChannelMute::CTestSetChannelMute(const TDesC& aTestName, const TTestStepType aTestType)
	:CTestMmfMidiClntStep(aTestName, aTestType)
	{
	}

CTestSetChannelMute* CTestSetChannelMute::NewL(const TDesC& aTestName, const TTestStepType aTestType)
	{
	CTestSetChannelMute* self = new(ELeave) CTestSetChannelMute(aTestName, aTestType);
	return self;
	}

TVerdict CTestSetChannelMute::DoTestL(CMidiClientUtility* aMidi)
	{
	TVerdict ret = EPass;
	TInt expErr = KErrNone;
	TInt channel = 0;
	TBool muted = ETrue;
	
	INFO_PRINTF1(_L("CMidiClientUtility: Set channel mute"));
	// expected results
	switch(iTestType)
		{
		case ETestValid:
			expErr = KErrNone;
			break;
		case ETestInvalidChannelOutOfRange:
			expErr = KErrArgument;
			channel = 16;
			break;

		default:
			ERR_PRINTF1(_L("Error - invalid test step type"));
			return EInconclusive;
		}

	TRAPD(err, aMidi->SetChannelMuteL(channel, muted));
	
	// check correct result
	if (iTestType == ETestValid)
		{
		TBool currMuted = EFalse;

		// until real MIDI controller IsChannelMuteL is always TRUE
		TRAPD(err2, currMuted = aMidi->IsChannelMuteL(channel));
		
		if(err2 != KErrNone)
			{
			ERR_PRINTF2(_L("IsChannelMuteL() returned %d"), err2);
			ret = EInconclusive;
			return ret;
			}

		if(currMuted != muted)
			{
			ERR_PRINTF2(_L("Error : expected %d"), muted);
			ret = EFail;
			return ret;
			}
		}

	
	if (expErr != err)
		{
		ERR_PRINTF3(_L("SetChannelMuteL gave error %d (expected %d)"),err, expErr);
		ret = EFail;
		}
	else
		INFO_PRINTF3(_L("SetChannelMuteL %d = %d"),err ,expErr);

	return ret;
	}

//------------------------------------------------------------------

CTestReturnVolume::CTestReturnVolume(const TDesC& aTestName, const TTestStepType aTestType)
	:CTestMmfMidiClntStep(aTestName, aTestType)
	{
	}

CTestReturnVolume* CTestReturnVolume::NewL(const TDesC& aTestName, const TTestStepType aTestType)
	{

	CTestReturnVolume* self = new(ELeave) CTestReturnVolume(aTestName, aTestType);
	return self;
	}

TVerdict CTestReturnVolume::DoTestL(CMidiClientUtility* aMidi)
	{
	TVerdict ret = EPass;
	TInt expErr = KErrNone;
	
	INFO_PRINTF1(_L("CMidiClientUtility: Return channel volume"));
	// expected results
	switch(iTestType)
		{
		case ETestValid:
			expErr = KErrNone;
			break;
		default:
			ERR_PRINTF1(_L("Error - invalid test step type"));
			return EInconclusive;
		}
//	TInt volume = 0;
//	TRAPD(err, volume = aMidi->VolumeL());
	TRAPD(err, aMidi->VolumeL());	// EABI warning removal
	if (expErr != err)
		{
		ERR_PRINTF3(_L("VolumeL gave error %d (expected %d)"),err, expErr);
		ret = EFail;
		}
	else
		INFO_PRINTF3(_L("VolumeL %d = %d"),err ,expErr);

	return ret;
	}

//------------------------------------------------------------------

CTestReturnMaxVolume::CTestReturnMaxVolume(const TDesC& aTestName, const TTestStepType aTestType)
	:CTestMmfMidiClntStep(aTestName, aTestType)
	{
	}

CTestReturnMaxVolume* CTestReturnMaxVolume::NewL(const TDesC& aTestName, const TTestStepType aTestType)
	{
		CTestReturnMaxVolume* self = new(ELeave) CTestReturnMaxVolume(aTestName, aTestType);
	return self;
	}

TVerdict CTestReturnMaxVolume::DoTestL(CMidiClientUtility* aMidi)
	{
	TVerdict ret = EPass;
	TInt expErr = KErrNone;
	
	INFO_PRINTF1(_L("CMidiClientUtility: Return max channel volume"));
	// expected results
	switch(iTestType)
		{
		case ETestValid:
			expErr = KErrNone;
			break;
		default:
			ERR_PRINTF1(_L("Error - invalid test step type"));
			return EInconclusive;
		}
//	TInt volume = 0;
//	TRAPD(err, volume = aMidi->MaxVolumeL());
	TRAPD(err, aMidi->MaxVolumeL());	// EABI warning removal
	if (expErr != err)
		{
		ERR_PRINTF3(_L("MaxVolumeL gave error %d (expected %d)"),err, expErr);
		ret = EFail;
		}
	else
		INFO_PRINTF3(_L("MaxVolumeL %d = %d"),err ,expErr);

	return ret;
	}

//------------------------------------------------------------------

CTestSetVolume::CTestSetVolume(const TDesC& aTestName, const TTestStepType aTestType)
	:CTestMmfMidiClntStep(aTestName, aTestType)
	{
	}

CTestSetVolume* CTestSetVolume::NewL(const TDesC& aTestName, const TTestStepType aTestType)
	{
	CTestSetVolume* self = new(ELeave) CTestSetVolume(aTestName, aTestType);
	return self;
	}

TVerdict CTestSetVolume::DoTestL(CMidiClientUtility* aMidi)
	{
	TVerdict ret = EPass;
	TInt expErr = KErrNone;
	TInt volume = 0;
	
	INFO_PRINTF1(_L("CMidiClientUtility: Set channel volume"));
	// expected results
	switch(iTestType)
		{
		case ETestValid:
			expErr = KErrNone;
			volume = 10;
			break;
		case ETestVolumeHighOutOfRange:
			expErr = KErrNotSupported;
			volume = 9999999;
			break;
		case ETestVolumeLowOutOfRange:
			expErr = KErrNotSupported;
			volume = -9999999;
			break;
		default:
			ERR_PRINTF1(_L("Error - invalid test step type"));
			return EInconclusive;
		}

	TRAPD(err, aMidi->SetVolumeL(volume));
	if (expErr != err)
		{
		ERR_PRINTF3(_L("SetVolumeL gave error %d (expected %d)"),err, expErr);
		ret = EFail;
		}
	else
		INFO_PRINTF3(_L("SetVolumeL %d = %d"),err ,expErr);

	return ret;
	}

//------------------------------------------------------------------

CTestSetVolumeRamp::CTestSetVolumeRamp(const TDesC& aTestName, const TTestStepType aTestType)
	:CTestMmfMidiClntStep(aTestName, aTestType)
	{
	}

CTestSetVolumeRamp* CTestSetVolumeRamp::NewL(const TDesC& aTestName, const TTestStepType aTestType)
	{
	CTestSetVolumeRamp* self = new(ELeave) CTestSetVolumeRamp(aTestName, aTestType);
	return self;
	}

TVerdict CTestSetVolumeRamp::DoTestL(CMidiClientUtility* aMidi)
	{
	TVerdict ret = EPass;
	TInt expErr = KErrNone;
	TTimeIntervalMicroSeconds rampDuration;
	
	INFO_PRINTF1(_L("CMidiClientUtility: Set channel volume ramp"));
	// expected results
	switch(iTestType)
		{
		case ETestValid:
			expErr = KErrNone;
			break;
		case ETestRampDurationHighOutOfRange:
			expErr = KErrNotSupported;
			rampDuration = 9999999;
			break;
		case ETestRampDurationLowOutOfRange:
			expErr = KErrNotSupported;
			rampDuration = -9999999;
			break;
		default:
			ERR_PRINTF1(_L("Error - invalid test step type"));
			return EInconclusive;
		}

	TRAPD(err, aMidi->SetVolumeRampL(rampDuration));
	if (expErr != err)
		{
		ERR_PRINTF3(_L("SetVolumeRampL gave error %d (expected %d)"),err, expErr);
		ret = EFail;
		}
	else
		INFO_PRINTF3(_L("SetVolumeL %d = %d"),err ,expErr);

	return ret;
	}

//------------------------------------------------------------------

CTestGetBalance::CTestGetBalance(const TDesC& aTestName, const TTestStepType aTestType)
	:CTestMmfMidiClntStep(aTestName, aTestType)
	{
	}

CTestGetBalance* CTestGetBalance::NewL(const TDesC& aTestName, const TTestStepType aTestType)
	{
	CTestGetBalance* self = new(ELeave) CTestGetBalance(aTestName, aTestType);
	return self;
	}

TVerdict CTestGetBalance::DoTestL(CMidiClientUtility* aMidi)
	{
	TVerdict ret = EPass;
	TInt expErr = KErrNone;
	//TTimeIntervalMicroSeconds rampDuration;
	
	INFO_PRINTF1(_L("CMidiClientUtility: Get balance"));
	// expected results
	switch(iTestType)
		{
		case ETestValid:
			expErr = KErrNone;
			break;
		case ETestUnsupported:
			expErr = KErrNotSupported;
			break;
		default:
			ERR_PRINTF1(_L("Error - invalid test step type"));
			return EInconclusive;
		}
//	TInt getBalance = 0;
//	TRAPD(err, getBalance = aMidi->GetBalanceL());
	TRAPD(err, aMidi->GetBalanceL());	// EABI warning removal

	if (expErr != err)
		{
		ERR_PRINTF3(_L("GetBalanceL gave error %d (expected %d)"),err, expErr);
		ret = EFail;
		}
	else
		INFO_PRINTF3(_L("GetBalanceL %d = %d"),err ,expErr);

	return ret;
	}

//------------------------------------------------------------------

CTestSetBalance::CTestSetBalance(const TDesC& aTestName, const TTestStepType aTestType)
	:CTestMmfMidiClntStep(aTestName, aTestType)
	{
	}

CTestSetBalance* CTestSetBalance::NewL(const TDesC& aTestName, const TTestStepType aTestType)
	{
	CTestSetBalance* self = new(ELeave) CTestSetBalance(aTestName, aTestType);
	return self;
	}

TVerdict CTestSetBalance::DoTestL(CMidiClientUtility* aMidi)
	{
	TVerdict ret = EPass;
	TInt expErr = KErrNone;
//	TInt balance = 0;		// EABI warning removal
	
	INFO_PRINTF1(_L("CMidiClientUtility: Set balance"));
	// expected results
	switch(iTestType)
		{
		case ETestValid:
			expErr = KErrNone;
			break;
		case ETestUnsupported:
			expErr = KErrNotSupported;
			break;
		case ETestBalanceHighOutOfRange:
			expErr = KErrNotSupported;
//			balance = 101;		// EABI warning removal
			break;
		case ETestBalanceLowOutOfRange:
			expErr = KErrNotSupported;
//			balance = -101;		// EABI warning removal
			break;
		default:
			ERR_PRINTF1(_L("Error - invalid test step type"));
			return EInconclusive;
		}

	TRAPD(err, aMidi->SetBalanceL());
	if (expErr != err)
		{
		ERR_PRINTF3(_L("SetBalanceL gave error %d (expected %d)"),err, expErr);
		ret = EFail;
		}
	else
		INFO_PRINTF3(_L("SetBalanceL %d = %d"),err ,expErr);

	return ret;
	}

//------------------------------------------------------------------

CTestSetPriority::CTestSetPriority(const TDesC& aTestName, const TTestStepType aTestType)
	:CTestMmfMidiClntStep(aTestName, aTestType)
	{
	}

CTestSetPriority* CTestSetPriority::NewL(const TDesC& aTestName, const TTestStepType aTestType)
	{
	CTestSetPriority* self = new(ELeave) CTestSetPriority(aTestName, aTestType);
	return self;
	}

TVerdict CTestSetPriority::DoTestL(CMidiClientUtility* aMidi)
	{
	TVerdict ret = EPass;
	TInt expErr = KErrNone;
	TInt priority = 0;
	TMdaPriorityPreference pref = EMdaPriorityPreferenceNone;
	
	INFO_PRINTF1(_L("CMidiClientUtility: Set priority"));
	// expected results
	switch(iTestType)
		{
		case ETestValid:
			expErr = KErrNone;
			break;
		case ETestPreferenceConflictsCannotBeResolved:
			expErr = KErrNotSupported;
			break;
		default:
			ERR_PRINTF1(_L("Error - invalid test step type"));
			return EInconclusive;
		}
//  TMdaPriorityPreference aPref Priority preference to use if there should be contention with another client.
//  We need this to confict and cause PreferenceConflictsCannotBeResolved
	TRAPD(err, aMidi->SetPriorityL(priority, pref));
	if (expErr != err)
		{
		ERR_PRINTF3(_L("SetPriorityL gave error %d (expected %d)"),err, expErr);
		ret = EFail;
		}
	else
		INFO_PRINTF3(_L("SetPriorityL %d = %d"),err ,expErr);

	return ret;
	}

//------------------------------------------------------------------

CTestNumberOfXmfMetaDataEntries::CTestNumberOfXmfMetaDataEntries(const TDesC& aTestName, const TTestStepType aTestType)
	:CTestMmfMidiClntStep(aTestName, aTestType)
	{
	}

CTestNumberOfXmfMetaDataEntries* CTestNumberOfXmfMetaDataEntries::NewL(const TDesC& aTestName, const TTestStepType aTestType)
	{
	CTestNumberOfXmfMetaDataEntries* self = new(ELeave) CTestNumberOfXmfMetaDataEntries(aTestName, aTestType);
	return self;
	}

TVerdict CTestNumberOfXmfMetaDataEntries::DoTestL(CMidiClientUtility* aMidi)
	{
	TVerdict ret = EPass;
	TInt expErr = KErrNone;
	
	INFO_PRINTF1(_L("CMidiClientUtility: Number Of Xmf Meta Data Entries"));
	// expected results
	switch(iTestType)
		{
		case ETestValid:
			expErr = KErrNone;
			break;
		case ETestUnsupported:
			expErr = KErrNotSupported;
			break;
		default:
			ERR_PRINTF1(_L("Error - invalid test step type"));
			return EInconclusive;
		}
//	TInt numberOfXmfMetaDataEntriesL = 0;
//	TRAPD(err, numberOfXmfMetaDataEntriesL = aMidi->NumberOfMetaDataEntriesL());
	TRAPD(err, aMidi->NumberOfMetaDataEntriesL());	// EABI warning removal
	if (expErr != err)
		{
		ERR_PRINTF3(_L("NumberOfXmfMetaDataEntriesL gave error %d (expected %d)"),err, expErr);
		ret = EFail;
		}
	else
		INFO_PRINTF3(_L("NumberOfXmfMetaDataEntriesL %d = %d"),err ,expErr);

	return ret;
	}

//------------------------------------------------------------------

CTestGetXmfMetaDataEntry::CTestGetXmfMetaDataEntry(const TDesC& aTestName, const TTestStepType aTestType)
	:CTestMmfMidiClntStep(aTestName, aTestType)
	{
	}

CTestGetXmfMetaDataEntry* CTestGetXmfMetaDataEntry::NewL(const TDesC& aTestName, const TTestStepType aTestType)
	{
	CTestGetXmfMetaDataEntry* self = new(ELeave) CTestGetXmfMetaDataEntry(aTestName, aTestType);
	return self;
	}

TVerdict CTestGetXmfMetaDataEntry::DoTestL(CMidiClientUtility* /*aMidi*/)
	{
	TVerdict ret = EPass;
	TInt err = KErrNone;

	TInt expErr = KErrNone;
//	TInt metaDataIndex = 1;		// EABI warning removal
	
	INFO_PRINTF1(_L("CMidiClientUtility: Get Xmf Meta Data Entry"));
	// expected results
	switch(iTestType)
		{
		case ETestValid:
			expErr = KErrNone;
			break;
		case ETestUnsupported:
			expErr = KErrNotSupported;
			break;
		case ETestMetaDataIndexInvalid:
//			metaDataIndex = -1;		// EABI warning removal
			break;
		default:
			ERR_PRINTF1(_L("Error - invalid test step type"));
			return EInconclusive;
		}

	// Default constructor. Returns a blank meta data object. To be used when internalizing data into the object.
//	CMMFMetaDataEntry* getXmfMetaDataEntry = CMMFMetaDataEntry::NewL();

// Causes Access Violation
//	TRAPD(err, CMMFMetaDataEntry* getXmfMetaDataEntry = aMidi->GetMetaDataEntryL(metaDataIndex));
	if (expErr != err)
		{
		ERR_PRINTF3(_L("GetXmfMetaDataEntry gave error %d (expected %d)"), err, expErr);
		ret = EFail;
		}
	else
		INFO_PRINTF3(_L("GetXmfMetaDataEntry %d = %d"),err ,expErr);

	return ret;
	}

//------------------------------------------------------------------


//------------------------------------------------------------------

CTestLoadCustomInstrumentData::CTestLoadCustomInstrumentData(const TDesC& aTestName,
									 const TTestStepType aTestType)	
	:CTestMmfMidiClntStep(aTestName, aTestType)
	{
	}

CTestLoadCustomInstrumentData* CTestLoadCustomInstrumentData::NewL(const TDesC& aTestName,
									 const TTestStepType aTestType)	
	{
	CTestLoadCustomInstrumentData* self = new (ELeave) CTestLoadCustomInstrumentData(aTestName,
														   aTestType);
	return self;
	}


TVerdict CTestLoadCustomInstrumentData::DoTestL(CMidiClientUtility* aMidi)
	{
	// N.B.  Until there is a MIDI controller there is no sense in having any
	// invalid tests since at present LoadCustomInstrumentDataL always returns
	// ETrue [it doesn't actually attempt a load].  When we actually have a 
	// proper MIDI instrument this test can be modified to load this instead of
	// the NULL descriptor below.  NJ
	
	TVerdict ret = EPass;
	TInt expErr;

	TInt memBankId = 90;
	TInt memInstrId = 91;
	TInt bankId = 92;
	TInt instrId = 93;

	// change these to real descriptors when we have a MIDI instruments
	const TDesC8* ptrInstrument = &KNullDesC8;
	const TDesC8* ptrBadInstrument = &KNullDesC8;

	// XXX::When we have a MIDI instrument / controller load the instrument
	// into the descriptor here.

	// expected results
	switch(iTestType)
		{
		case ETestValid:
			expErr = KErrNone;
			break;
		case ETestInvalidId:
			expErr = KErrArgument;
			bankId = -2;
			break;
		case ETestAlreadyLoaded:
			// ** to do : load once and attempt to load again.
			// when instrument data files are available, this will work
			expErr = KErrInUse;
			break;
		case ETestUnsupported:	// ** to define test data for this
			expErr = KErrNotSupported;
			break;

		default:
			ERR_PRINTF1(_L("Error - invalid test step type"));
			return EInconclusive;
		}
	TInt err = KErrNone;

	// bad descriptor test
	if (iTestType == ETestUnsupported)
		{
		// load the bad / unsupported instrument
		TRAP(err, aMidi->LoadCustomInstrumentDataL(*ptrBadInstrument, bankId, instrId, memBankId, memInstrId));	
		}
	else
		{
		// if testing already loaded, preload the instrument here
		if(iTestType == ETestAlreadyLoaded)
			{
			TRAP(err, aMidi->LoadCustomInstrumentDataL(*ptrInstrument, bankId, instrId, memBankId, memInstrId));
			INFO_PRINTF2(_L("Preload : LoadCustomInstrumentDataL gave error %d"), err);
			if(err != KErrNone)
				{
				ret = EInconclusive;
				return ret;
				}
			}

		// load the instrument
		TRAP(err, aMidi->LoadCustomInstrumentDataL(*ptrInstrument, bankId, instrId, memBankId, memInstrId));
		}
	
	INFO_PRINTF3(_L("LoadCustomInstrumentDataL gave error %d (expected %d)"), err, expErr);
	if(err != expErr)
		{
		ret = EFail;
		return ret;
		}

	return ret;	
	}


//------------------------------------------------------------------


//------------------------------------------------------------------

_LIT(KMidiClntChunk, "MidiClntChunk");
_LIT(KMidiClntSemaphore, "MidiClntSemaphore");

CTestCheckInterface::CTestCheckInterface(const TDesC& aTestName, const TTestStepType aTestType)	
	:CTestMmfMidiClntStep(aTestName,aTestType)
	{
	}

CTestCheckInterface* CTestCheckInterface::NewL(const TDesC& aTestName, const TTestStepType aTestType)	
	{
	CTestCheckInterface* self = new (ELeave) CTestCheckInterface(aTestName, aTestType);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CTestCheckInterface::ConstructL()
	{
	User::LeaveIfError(iChunk.CreateGlobal(KMidiClntChunk,20,20));
	User::LeaveIfError(iSemaphore.CreateGlobal(KMidiClntSemaphore, 0));

	}

CTestCheckInterface::~CTestCheckInterface()
	{
	}

void CTestCheckInterface::Close()
	{
	iChunk.Close();
	iSemaphore.Close();
	}


TVerdict CTestCheckInterface::DoTestL(CMidiClientUtility* aMidi)
	{
	
	TVerdict ret = EPass;
	TInt expErr = KErrNone;
	TInt err = KErrNone;
	
	//
	//PlayNoteL//
	//
	TRAP_IGNORE(aMidi->PlayNoteL(0,0,TTimeIntervalMicroSeconds(0),0,0));
	err = CompareInterface(	_L8("PlayNoteL"));
	INFO_PRINTF3(_L("Check interface PlayNoteL gave error %d (expected %d)"), err, expErr);
	if(err != expErr)
		{
		ret = EFail;
		return ret;
		}

	//
	//PlayNoteL(WithStartTime)//
	//
	TRAP_IGNORE(aMidi->PlayNoteL(0,0,TTimeIntervalMicroSeconds(0),TTimeIntervalMicroSeconds(0),0,0));
	err = CompareInterface(_L8("PlayNoteWithStartTime"));
	INFO_PRINTF3(_L("Check interface PlayNoteL (WithStartTime) gave error %d (expected %d)"), err, expErr);
	if(err != expErr)
		{
		ret = EFail;
		return ret;
		}

	//
	//StopNotes//
	//
	aMidi->StopNotes(0);
	err = CompareInterface(_L8("StopNotes"));
	INFO_PRINTF3(_L("Check interface StopNotes gave error %d (expected %d)"), err, expErr);
	if(err != expErr)
		{
		ret = EFail;
		return ret;
		}

	//
	//NoteOnL//
	//
	TRAP_IGNORE(aMidi->NoteOnL(0,0,0));
	err = CompareInterface(_L8("NoteOnL"));
	INFO_PRINTF3(_L("Check interface NoteOnL gave error %d (expected %d)"), err, expErr);
	if(err != expErr)
		{
		ret = EFail;
		return ret;
		}

	//
	//NoteOffL//
	//
	TRAP_IGNORE(aMidi->NoteOffL(0,0,0));
	err = CompareInterface(_L8("NoteOffL"));
	INFO_PRINTF3(_L("Check interface NoteOffL gave error %d (expected %d)"), err, expErr);
	if(err != expErr)
		{
		ret = EFail;
		return ret;
		}

	//
	//PlaybackRateL//
	//
	TRAP_IGNORE(aMidi->PlaybackRateL());
	err = CompareInterface(_L8("PlaybackRateL"));
	INFO_PRINTF3(_L("Check interface PlaybackRateL gave error %d (expected %d)"), err, expErr);
	if(err != expErr)
		{
		ret = EFail;
		return ret;
		}

	//
	//SetPlaybackRateL//
	//
	TRAP_IGNORE(aMidi->SetPlaybackRateL(0));
	err = CompareInterface(_L8("SetPlaybackRateL"));
	INFO_PRINTF3(_L("Check interface SetPlaybackRateL gave error %d (expected %d)"), err, expErr);
	if(err != expErr)
		{
		ret = EFail;
		return ret;
		}

	//
	//MaxPlaybackRateL//
	//
	TRAP_IGNORE(aMidi->MaxPlaybackRateL());
	err = CompareInterface(_L8("MaxPlaybackRateL"));
	INFO_PRINTF3(_L("Check interface MaxPlaybackRateL gave error %d (expected %d)"), err, expErr);
	if(err != expErr)
		{
		ret = EFail;
		return ret;
		}

	//
	//MinPlaybackRateL//
	//
	TRAP_IGNORE(aMidi->MinPlaybackRateL());
	err = CompareInterface(_L8("MinPlaybackRateL"));
	INFO_PRINTF3(_L("Check interface MinPlaybackRateL gave error %d (expected %d)"), err, expErr);
	if(err != expErr)
		{
		ret = EFail;
		return ret;
		}

	//
	//TempoMicroBeatsPerMinuteL//
	//
	TRAP_IGNORE(aMidi->TempoMicroBeatsPerMinuteL());
	err = CompareInterface(_L8("TempoMicroBeatsPerMinuteL"));
	INFO_PRINTF3(_L("Check interface TempoMicroBeatsPerMinuteL gave error %d (expected %d)"), err, expErr);
	if(err != expErr)
		{
		ret = EFail;
		return ret;
		}

	//
	//SetTempoL//
	//
	TRAP_IGNORE(aMidi->SetTempoL(1));
	err = CompareInterface(_L8("SetTempoL"));
	INFO_PRINTF3(_L("Check interface SetTempoL gave error %d (expected %d)"), err, expErr);
	if(err != expErr)
		{
		ret = EFail;
		return ret;
		}

	//
	//PitchTranspositionCentsL//
	//
	TRAP_IGNORE(aMidi->PitchTranspositionCentsL());
	err = CompareInterface(_L8("PitchTranspositionCentsL"));
	INFO_PRINTF3(_L("Check interface PitchTranspositionCentsL gave error %d (expected %d)"), err, expErr);
	if(err != expErr)
		{
		ret = EFail;
		return ret;
		}

	//
	//SetPitchTranspositionL//
	//
	TRAP_IGNORE(aMidi->SetPitchTranspositionL(0));
	err = CompareInterface(_L8("SetPitchTranspositionL"));
	INFO_PRINTF3(_L("Check interface SetPitchTranspositionL gave error %d (expected %d)"), err, expErr);
	if(err != expErr)
		{
		ret = EFail;
		return ret;
		}

	//
	//DurationMicroBeatsL//
	//
	TRAP_IGNORE(aMidi->DurationMicroBeatsL());
	err = CompareInterface(_L8("DurationMicroBeatsL"));
	INFO_PRINTF3(_L("Check interface DurationMicroBeatsL gave error %d (expected %d)"), err, expErr);
	if(err != expErr)
		{
		ret = EFail;
		return ret;
		}

	//
	//NumTracksL//
	//
	TRAP_IGNORE(aMidi->NumTracksL());
	err = CompareInterface(_L8("NumTracksL"));
	INFO_PRINTF3(_L("Check interface NumTracksL gave error %d (expected %d)"), err, expErr);
	if(err != expErr)
		{
		ret = EFail;
		return ret;
		}

	//
	//SetTrackMuteL//
	//
	TRAP_IGNORE(aMidi->SetTrackMuteL(0,ETrue));
	err = CompareInterface(_L8("SetTrackMuteL"));
	INFO_PRINTF3(_L("Check interface SetTrackMuteL gave error %d (expected %d)"), err, expErr);
	if(err != expErr)
		{
		ret = EFail;
		return ret;
		}

	//
	//MimeTypeL//
	//
	TRAP_IGNORE(aMidi->MimeTypeL());
	err = CompareInterface(_L8("MimeTypeL"));
	INFO_PRINTF3(_L("Check interface MimeTypeL gave error %d (expected %d)"), err, expErr);
	if(err != expErr)
		{
		ret = EFail;
		return ret;
		}

	//
	//PositionMicroBeatsL//
	//
	TRAP_IGNORE(aMidi->PositionMicroBeatsL());
	err = CompareInterface(_L8("PositionMicroBeatsL"));
	INFO_PRINTF3(_L("Check interface PositionMicroBeatsL gave error %d (expected %d)"), err, expErr);
	if(err != expErr)
		{
		ret = EFail;
		return ret;
		}

	//
	//SetPositionMicroBeatsL//
	//
	TRAP_IGNORE(aMidi->SetPositionMicroBeatsL(0));
	err = CompareInterface(_L8("SetPositionMicroBeatsL"));
	INFO_PRINTF3(_L("Check interface SetPositionMicroBeatsL gave error %d (expected %d)"), err, expErr);
	if(err != expErr)
		{
		ret = EFail;
		return ret;
		}

	//
	//SetSyncUpdateCallbackIntervalL//
	//
	TRAP_IGNORE(aMidi->SetSyncUpdateCallbackIntervalL(TTimeIntervalMicroSeconds(1)));
	err = CompareInterface(_L8("SetSyncUpdateCallbackIntervalL"));
	INFO_PRINTF3(_L("Check interface SetSyncUpdateCallbackIntervalL gave error %d (expected %d)"), err, expErr);
	if(err != expErr)
		{
		ret = EFail;
		return ret;
		}

	//
	//SendMessageL//
	//
	TRAP_IGNORE(aMidi->SendMessageL(_L8("")));
	err = CompareInterface(_L8("SendMessageL"));
	INFO_PRINTF3(_L("Check interface SendMessageL gave error %d (expected %d)"), err, expErr);
	if(err != expErr)
		{
		ret = EFail;
		return ret;
		}

	//
	//SendMessageL (WithTimeStamp)//
	//
	TRAP_IGNORE(aMidi->SendMessageL(_L8(""), TTimeIntervalMicroSeconds(0)));
	err = CompareInterface(_L8("SendMessageWithTimeStamp"));
	INFO_PRINTF3(_L("Check interface SendMessageL (WithTimeStamp) gave error %d (expected %d)"), err, expErr);
	if(err != expErr)
		{
		ret = EFail;
		return ret;
		}

	//
	//SendMipMessageL//
	//
	{
	TMipMessageEntry mipEntry;
	mipEntry.iChannel = 10;
	mipEntry.iMIPValue = 20;
	RArray<TMipMessageEntry> mipArray;
	mipArray.Append(mipEntry);

	TRAP_IGNORE(aMidi->SendMipMessageL(mipArray));
	mipArray.Close();
	}
	err = CompareInterface(_L8("SendMipMessageL"));
	INFO_PRINTF3(_L("Check interface SendMipMessageL gave error %d (expected %d)"), err, expErr);
	if(err != expErr)
		{
		ret = EFail;
		return ret;
		}

	//
	//NumberOfBanksL//
	//
	TRAP_IGNORE(aMidi->NumberOfBanksL(ETrue));
	err = CompareInterface(_L8("NumberOfBanksL"));
	INFO_PRINTF3(_L("Check interface NumberOfBanksL gave error %d (expected %d)"), err, expErr);
	if(err != expErr)
		{
		ret = EFail;
		return ret;
		}

	//
	//GetBankIdL//
	//
	TRAP_IGNORE(aMidi->GetBankIdL(ETrue,0));
	err = CompareInterface(_L8("GetBankIdL"));
	INFO_PRINTF3(_L("Check interface GetBankIdL gave error %d (expected %d)"), err, expErr);
	if(err != expErr)
		{
		ret = EFail;
		return ret;
		}

	//
	//LoadCustomBankL//
	//
	{
	TInt  bankId = 0;
	TRAP_IGNORE(aMidi->LoadCustomBankL(_L(""),bankId));
	}
	err = CompareInterface(_L8("LoadCustomBankL"));
	INFO_PRINTF3(_L("Check interface LoadCustomBankL gave error %d (expected %d)"), err, expErr);
	if(err != expErr)
		{
		ret = EFail;
		return ret;
		}

	//
	//LoadCustomBankDataL//
	//
	{
	TInt  bankId = 0;
	TRAP_IGNORE(aMidi->LoadCustomBankDataL(_L8(""),bankId));
	}
	err = CompareInterface(_L8("LoadCustomBankDataL"));
	INFO_PRINTF3(_L("Check interface LoadCustomBankDataL gave error %d (expected %d)"), err, expErr);
	if(err != expErr)
		{
		ret = EFail;
		return ret;
		}

	//
	//UnloadCustomBankL//
	//
	TRAP_IGNORE(aMidi->UnloadCustomBankL(0));
	err = CompareInterface(_L8("UnloadCustomBankL"));
	INFO_PRINTF3(_L("Check interface UnloadCustomBankL gave error %d (expected %d)"), err, expErr);
	if(err != expErr)
		{
		ret = EFail;
		return ret;
		}

	//
	//CustomBankLoadedL//
	//
	TRAP_IGNORE(aMidi->CustomBankLoadedL(0));
	err = CompareInterface(_L8("CustomBankLoadedL"));
	INFO_PRINTF3(_L("Check interface CustomBankLoadedL gave error %d (expected %d)"), err, expErr);
	if(err != expErr)
		{
		ret = EFail;
		return ret;
		}

	//
	//UnloadAllCustomBanksL//
	//
	TRAP_IGNORE(aMidi->UnloadAllCustomBanksL());
	err = CompareInterface(_L8("UnloadAllCustomBanksL"));
	INFO_PRINTF3(_L("Check interface UnloadAllCustomBanksL gave error %d (expected %d)"), err, expErr);
	if(err != expErr)
		{
		ret = EFail;
		return ret;
		}

	//
	//NumberOfInstrumentsL//
	//
	TRAP_IGNORE(aMidi->NumberOfInstrumentsL(0,ETrue));
	err = CompareInterface(_L8("NumberOfInstrumentsL"));
	INFO_PRINTF3(_L("Check interface NumberOfInstrumentsL gave error %d (expected %d)"), err, expErr);
	if(err != expErr)
		{
		ret = EFail;
		return ret;
		}

	//
	//GetInstrumentIdL//
	//
	TRAP_IGNORE(aMidi->GetInstrumentIdL(0, ETrue, 0));
	err = CompareInterface(_L8("GetInstrumentIdL"));
	INFO_PRINTF3(_L("Check interface GetInstrumentIdL gave error %d (expected %d)"), err, expErr);
	if(err != expErr)
		{
		ret = EFail;
		return ret;
		}

	//
	//InstrumentNameL//
	//
	{
	HBufC* instrumentName = NULL;
	TRAP_IGNORE(instrumentName = aMidi->InstrumentNameL(0, ETrue, 0));
	delete instrumentName;
	}
	err = CompareInterface(_L8("InstrumentNameL"));
	INFO_PRINTF3(_L("Check interface InstrumentNameL gave error %d (expected %d)"), err, expErr);
	if(err != expErr)
		{
		ret = EFail;
		return ret;
		}

	//
	//GetInstrumentL//
	//
	{
	TInt instrumentId = 0;
	TInt bankId = 0;
	TRAP_IGNORE(aMidi->GetInstrumentL(0, instrumentId, bankId));
	}
	err = CompareInterface(_L8("GetInstrumentL"));
	INFO_PRINTF3(_L("Check interface GetInstrumentL gave error %d (expected %d)"), err, expErr);
	if(err != expErr)
		{
		ret = EFail;
		return ret;
		}

	//
	//SetInstrumentL//
	//
	TRAP_IGNORE(aMidi->SetInstrumentL(0, 0, 0));
	err = CompareInterface(_L8("SetInstrumentL"));
	INFO_PRINTF3(_L("Check interface SetInstrumentL gave error %d (expected %d)"), err, expErr);
	if(err != expErr)
		{
		ret = EFail;
		return ret;
		}

	//
	//LoadCustomInstrumentL//
	//
	TRAP_IGNORE(aMidi->LoadCustomInstrumentL(_L(""),0,0,0,0));
	err = CompareInterface(_L8("LoadCustomInstrumentL"));
	INFO_PRINTF3(_L("Check interface LoadCustomInstrumentL gave error %d (expected %d)"), err, expErr);
	if(err != expErr)
		{
		ret = EFail;
		return ret;
		}

	//
	//LoadCustomInstrumentDataL//
	//
	TRAP_IGNORE(aMidi->LoadCustomInstrumentDataL(_L8(""),0,0,0,0));
	err = CompareInterface(_L8("LoadCustomInstrumentDataL"));
	INFO_PRINTF3(_L("Check interface LoadCustomInstrumentDataL gave error %d (expected %d)"), err, expErr);
	if(err != expErr)
		{
		ret = EFail;
		return ret;
		}

	//
	//UnloadCustomInstrumentL//
	//
	TRAP_IGNORE(aMidi->UnloadCustomInstrumentL(0,0));
	err = CompareInterface(_L8("UnloadCustomInstrumentL"));
	INFO_PRINTF3(_L("Check interface UnloadCustomInstrumentL gave error %d (expected %d)"), err, expErr);
	if(err != expErr)
		{
		ret = EFail;
		return ret;
		}

	//
	//PercussionKeyNameL//
	//
	{
	HBufC* percussionKeyName = NULL;
	TRAP_IGNORE(percussionKeyName = aMidi->PercussionKeyNameL(0,0,0,0));
	delete percussionKeyName;
	}
	err = CompareInterface(_L8("PercussionKeyNameL"));
	INFO_PRINTF3(_L("Check interface PercussionKeyNameL gave error %d (expected %d)"), err, expErr);
	if(err != expErr)
		{
		ret = EFail;
		return ret;
		}

	//
	//StopTimeL//
	//
	{
	TTimeIntervalMicroSeconds stopTime(0);
	TRAP_IGNORE(aMidi->StopTimeL(stopTime));
	}
	err = CompareInterface(_L8("StopTimeL"));
	INFO_PRINTF3(_L("Check interface StopTimeL gave error %d (expected %d)"), err, expErr);
	if(err != expErr)
		{
		ret = EFail;
		return ret;
		}

	//
	//SetStopTimeL//
	//
	{
	TTimeIntervalMicroSeconds stopTime(0);
	TRAP_IGNORE(aMidi->SetStopTimeL(stopTime));
	}
	err = CompareInterface(_L8("SetStopTimeL"));
	INFO_PRINTF3(_L("Check interface SetStopTimeL gave error %d (expected %d)"), err, expErr);
	if(err != expErr)
		{
		ret = EFail;
		return ret;
		}

	//
	//SetRepeatsL//
	//
	{
	TInt repeatNumberOfTimes = 0;
	TTimeIntervalMicroSeconds trailingSilence(0);
	TRAP_IGNORE(aMidi->SetRepeatsL(repeatNumberOfTimes, trailingSilence));
	}
	err = CompareInterface(_L8("SetRepeatsL"));
	INFO_PRINTF3(_L("Check interface SetRepeatsL gave error %d (expected %d)"), err, expErr);
	if(err != expErr)
		{
		ret = EFail;
		return ret;
		}
	//
	//GetRepeats//
	//
	TRAP_IGNORE(aMidi->GetRepeats());
	err = CompareInterface(_L8("GetRepeats"));
	INFO_PRINTF3(_L("Check interface GetRepeats gave error %d (expected %d)"), err, expErr);
	if(err != expErr)
		{
		ret = EFail;
		return ret;
		}


	//
	//PolyphonyL//
	//
	TRAP_IGNORE(aMidi->PolyphonyL());
	err = CompareInterface(_L8("PolyphonyL"));
	INFO_PRINTF3(_L("Check interface PolyphonyL gave error %d (expected %d)"), err, expErr);
	if(err != expErr)
		{
		ret = EFail;
		return ret;
		}

	//
	//SetMaxPolyphonyL//
	//
	TRAP_IGNORE(aMidi->SetMaxPolyphonyL(1));
	err = CompareInterface(_L8("SetMaxPolyphonyL"));
	INFO_PRINTF3(_L("Check interface SetMaxPolyphonyL gave error %d (expected %d)"), err, expErr);
	if(err != expErr)
		{
		ret = EFail;
		return ret;
		}

	//
	//ChannelsSupportedL//
	//
	TRAP_IGNORE(aMidi->ChannelsSupportedL());
	err = CompareInterface(_L8("ChannelsSupportedL"));
	INFO_PRINTF3(_L("Check interface ChannelsSupportedL gave error %d (expected %d)"), err, expErr);
	if(err != expErr)
		{
		ret = EFail;
		return ret;
		}

	//
	//ChannelVolumeL//
	//
	TRAP_IGNORE(aMidi->ChannelVolumeL(0));
	err = CompareInterface(_L8("ChannelVolumeL"));
	INFO_PRINTF3(_L("Check interface ChannelVolumeL gave error %d (expected %d)"), err, expErr);
	if(err != expErr)
		{
		ret = EFail;
		return ret;
		}

	//
	//MaxChannelVolumeL//
	//
	TRAP_IGNORE(aMidi->MaxChannelVolumeL());
	err = CompareInterface(_L8("MaxChannelVolumeL"));
	INFO_PRINTF3(_L("Check interface MaxChannelVolumeL gave error %d (expected %d)"), err, expErr);
	if(err != expErr)
		{
		ret = EFail;
		return ret;
		}

	//
	//SetChannelVolumeL//
	//
	TRAP_IGNORE(aMidi->SetChannelVolumeL(0,0));
	err = CompareInterface(_L8("SetChannelVolumeL"));
	INFO_PRINTF3(_L("Check interface SetChannelVolumeL gave error %d (expected %d)"), err, expErr);
	if(err != expErr)
		{
		ret = EFail;
		return ret;
		}

	//
	//SetChannelMuteL//
	//
	TRAP_IGNORE(aMidi->SetChannelMuteL(0,0));
	err = CompareInterface(_L8("SetChannelMuteL"));
	INFO_PRINTF3(_L("Check interface SetChannelMuteL gave error %d (expected %d)"), err, expErr);
	if(err != expErr)
		{
		ret = EFail;
		return ret;
		}

	//
	//VolumeL//
	//
	TRAP_IGNORE(aMidi->VolumeL());
	err = CompareInterface(_L8("VolumeL"));
	INFO_PRINTF3(_L("Check interface VolumeL gave error %d (expected %d)"), err, expErr);
	if(err != expErr)
		{
		ret = EFail;
		return ret;
		}

	//
	//MaxVolumeL//
	//
	TRAP_IGNORE(aMidi->MaxVolumeL());
	err = CompareInterface(_L8("MaxVolumeL"));
	INFO_PRINTF3(_L("Check interface MaxVolumeL gave error %d (expected %d)"), err, expErr);
	if(err != expErr)
		{
		ret = EFail;
		return ret;
		}

	//
	//SetVolumeL//
	//
	TRAP_IGNORE(aMidi->SetVolumeL(0));
	err = CompareInterface(_L8("SetVolumeL"));
	INFO_PRINTF3(_L("Check interface SetVolumeL gave error %d (expected %d)"), err, expErr);
	if(err != expErr)
		{
		ret = EFail;
		return ret;
		}

	//
	//SetVolumeRampL//
	//
	TRAP_IGNORE(aMidi->SetVolumeRampL(TTimeIntervalMicroSeconds(0)));
	err = CompareInterface(_L8("SetVolumeRampL"));
	INFO_PRINTF3(_L("Check interface SetVolumeRampL gave error %d (expected %d)"), err, expErr);
	if(err != expErr)
		{
		ret = EFail;
		return ret;
		}

	//
	//GetBalanceL//
	//
	TRAP_IGNORE(aMidi->GetBalanceL());
	err = CompareInterface(_L8("GetBalanceL"));
	INFO_PRINTF3(_L("Check interface GetBalanceL gave error %d (expected %d)"), err, expErr);
	if(err != expErr)
		{
		ret = EFail;
		return ret;
		}

	//
	//SetBalanceL//
	//
	TRAP_IGNORE(aMidi->SetBalanceL());
	err = CompareInterface(_L8("SetBalanceL"));
	INFO_PRINTF3(_L("Check interface SetBalanceL gave error %d (expected %d)"), err, expErr);
	if(err != expErr)
		{
		ret = EFail;
		return ret;
		}

	//
	//SetBankL//
	//
	TRAP_IGNORE(aMidi->SetBankL(ETrue));
	err = CompareInterface(_L8("SetBankL"));
	INFO_PRINTF3(_L("Check interface SetBankL gave error %d (expected %d)"), err, expErr);
	if(err != expErr)
		{
		ret = EFail;
		return ret;
		}

	//
	//IsTrackMuteL//
	//
	TRAP_IGNORE(aMidi->IsTrackMuteL(0));
	err = CompareInterface(_L8("IsTrackMuteL"));
	INFO_PRINTF3(_L("Check interface IsTrackMuteL gave error %d (expected %d)"), err, expErr);
	if(err != expErr)
		{
		ret = EFail;
		return ret;
		}

	//
	//IsChannelMuteL//
	//
	TRAP_IGNORE(aMidi->IsChannelMuteL(0));
	err = CompareInterface(_L8("IsChannelMuteL"));
	INFO_PRINTF3(_L("Check interface IsChannelMuteL gave error %d (expected %d)"), err, expErr);
	if(err != expErr)
		{
		ret = EFail;
		return ret;
		}

	return ret;	
	}

TInt CTestCheckInterface::CompareInterface(const TDesC8& aInterfaceName)
	{
	TBuf8<40> clientBuf;
	TBuf8<40> controllerBuf;
	clientBuf.FillZ(40);
	controllerBuf.FillZ(40);

	clientBuf.Copy(aInterfaceName);
	iSemaphore.Wait();
	controllerBuf = iChunk.Base();
	controllerBuf.SetLength(aInterfaceName.Length());

	TInt err = clientBuf.Compare(controllerBuf);

	return err;
	}

//--------------------------------------------------------------------------------
//MS 3.4 File Handles API
//--------------------------------------------------------------------------------
CTestMidiClntOpenFileByHandle::CTestMidiClntOpenFileByHandle(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName,const TBool aPlay, const TTestStepType aTestType)
	:CTestMmfMidiClntStep(aTestName, aTestType),
	 iPlay(aPlay)
	{
	iSectName = aSectName;
	iKeyName = aKeyName;
	}

CTestMidiClntOpenFileByHandle* CTestMidiClntOpenFileByHandle::NewL(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName,const TBool aPlay, const TTestStepType aTestType)
	{
	CTestMidiClntOpenFileByHandle* self = new(ELeave) CTestMidiClntOpenFileByHandle(aTestName, aSectName, aKeyName, aPlay, aTestType);
	return self;
	}

TVerdict CTestMidiClntOpenFileByHandle::DoTestStepL()
	{
	//[ Local variables ]
	TPtrC filename;
	TInt expErr = KErrNone;
	TVerdict ret = EFail;

	//[ Get the File name from ini file ]
	if(!GetStringFromConfig(iSectName,iKeyName,filename))
		{
		return EInconclusive;
		}
	
	//[ Expected results ]
	switch(iTestType)
		{
		case ETestValid:
			expErr = KErrNone;
			break;
		case ETestNegative:
			expErr = KErrNotSupported;
			break;
		default:
			ERR_PRINTF1(_L("Error - invalid test step type"));
			return EInconclusive;
		}
		
	//[ Create CMidiClientUtility - player ]
	CMidiClientUtility* player=NULL;
	TRAPD(err1, player = CMidiClientUtility::NewL(*this, EMdaPriorityNormal, EMdaPriorityPreferenceTimeAndQuality));
	if (err1 != KErrNone)
		{
		ERR_PRINTF2(_L("Error while creating a CMidiClientUtility : %d"),err1);
		return EInconclusive;
		}
	
	if (!player)
		{
		ERR_PRINTF1(_L("Could not create a CMidiClientUtility"));
		return EInconclusive;
		}
	CleanupStack::PushL(player);

	TMMFMessageDestinationPckg dummyPckg;
	TInt dummyFunc = EDevMidiOff;
	TBuf8<8> dummyBuff;
	player->CustomCommandSyncL(dummyPckg, dummyFunc, dummyBuff, dummyBuff, dummyBuff);

	//[ RFs and RFile ]
	RFs rFs;
	RFile rFile;
	
	//[ Connect to File System using RFs ]
	User::LeaveIfError(rFs.Connect());
	CleanupClosePushL(rFs);
	User::LeaveIfError(rFs.ShareProtected());

	//[ Open the file using RFile ]
	TFileName theDestinationFileName;
	theDestinationFileName.Append(filename);
	
	INFO_PRINTF2(_L("Start : Opening the file : %S by RFile"), &filename);
	TInt theRes = rFile.Open(rFs, theDestinationFileName, EFileRead);
	User::LeaveIfError(theRes);
	CleanupClosePushL(rFile);
	INFO_PRINTF2(_L("End : Opening the file : %S by RFile"), &filename);

	//[ API Call ]
	player->OpenFile(rFile);
	
	//[ Wait for Initialisation Callback ]
	INFO_PRINTF1(_L("Start : CMidiClientUtility->OpenFile()"));
	CActiveScheduler::Start();
	INFO_PRINTF1(_L("End : CMidiClientUtility->OpenFile()"));

	//[ Check for errors ]
	if (iError == expErr)
		{
		INFO_PRINTF3(_L("Expected Error : %d, Actual Error : %d"), expErr, iError);
		ret = EPass;
		}
	else
		{
		ERR_PRINTF3(_L("Expected Error : %d, Actual Error : %d"), expErr, iError);
		ret = EFail;
		}
		
	//[ Clean-up RFs, RFile and CMidiClientUtility ]
	CleanupStack::PopAndDestroy(3);

	return	ret;
	}


//------------------------------------------------------------------------

CTestMidiClntPlayFileByHandle::CTestMidiClntPlayFileByHandle(const TDesC& aTestName)
	:CTestMmfMidiClntStep(aTestName, ETestValid)
	{
	}

CTestMidiClntPlayFileByHandle* CTestMidiClntPlayFileByHandle::NewL(const TDesC& aTestName)
	{
	CTestMidiClntPlayFileByHandle* self = new(ELeave) CTestMidiClntPlayFileByHandle(aTestName);
	return self;
	}

TVerdict CTestMidiClntPlayFileByHandle::DoTestL(CMidiClientUtility* aMidi)
	{
	//[ Local variables ]
	TPtrC filename;
	
	//[ Get the filename ]
	if(!GetStringFromConfig(_L("SectionOne"),_L("filename"),filename))
		{
		return EInconclusive;
		}

	INFO_PRINTF1(_L("CMidiClientUtility : Play MIDI File"));
	
	iCurrentState = EMidiStateClosedDisengaged;

	//[ RFs and RFile ]
	RFs rFs;
	RFile rFile;
	
	//[ Connect to File System using RFs ]
	User::LeaveIfError(rFs.Connect());
	CleanupClosePushL(rFs);
	User::LeaveIfError(rFs.ShareProtected());

	INFO_PRINTF1(_L("Opening the file : c:\\DoesntExist.mid"));
	
	//[ Open the file using RFile ]
	TInt theRes = rFile.Open(rFs, filename, EFileRead);
	if(theRes != KErrNone)
		{
		INFO_PRINTF2(_L("Cannot open file : %S"), &filename);
		}  
	CleanupClosePushL(rFile);

	//[ Call the OpenFile Method using the File Handle ]
	aMidi->OpenFile(rFile);
	
	INFO_PRINTF1(_L("Waiting for EMidiStateOpenDisengaged state..."));
	CActiveScheduler::Start();	// EMidiStateClosedDisengaged -> EMidiStateOpenDisengaged
	
	//[ Clean up RFs and RFile ]
	CleanupStack::PopAndDestroy(2);

	if (iCurrentState != EMidiStateOpenDisengaged)
		{
		ERR_PRINTF2(_L("Unexpected state (expected = EMidiStateOpenDisengaged, received = %d)"), iCurrentState);
		return EFail;
		}
	aMidi->Play();
	if (iError != KErrNone)
		{
		ERR_PRINTF2(_L("Play gave error %d"),iError);
		return EFail;
		}
	
	INFO_PRINTF1(_L("Waiting for EMidiStateOpenPlaying state..."));
	CActiveScheduler::Start();	// EMidiStateOpenDisengaged ->EMidiStateOpenPlaying
	if (iCurrentState != EMidiStateOpenPlaying)
		{
		ERR_PRINTF2(_L("Unexpected state (expected = EMidiStateOpenPlaying, received = %d)"), iCurrentState);
		return EFail;
		}

	INFO_PRINTF1(_L("Waiting for EMidiStateOpenDisengaged state..."));
	CActiveScheduler::Start();	// EMidiStateOpenPlaying ->EMidiStateOpenEngaged
	if (iCurrentState != EMidiStateOpenEngaged)
		{
		ERR_PRINTF2(_L("Unexpected state (expected = EMidiStateOpenDisengaged, received = %d)"), iCurrentState);
		return EFail;
		}
	
	return EPass;
	}
	
	
//------------------------------------------------------------------------
// The purpose of this test is that we can hold more than 10 events in the
// MIDI event queue and none of them gets lost

CTestMidiClntThirteenMidiEvents::CTestMidiClntThirteenMidiEvents(const TDesC& aTestName, const TTestStepType aTestType)
	:CTestMmfMidiClntStep(aTestName, aTestType)
	{
	// set the current transion to 0, the start point
	iCurrentTransion = 0;
	}


CTestMidiClntThirteenMidiEvents* CTestMidiClntThirteenMidiEvents::NewL(const TDesC& aTestName, const TTestStepType aTestType)
	{
	CTestMidiClntThirteenMidiEvents* self = new(ELeave)CTestMidiClntThirteenMidiEvents(aTestName, aTestType);	
	return self;
	}

void CTestMidiClntThirteenMidiEvents::InitialiseExpectedTransArrayL()
{
	User::LeaveIfError(expectedTransitions.Append(EMmcuoTempoChanged));
	User::LeaveIfError(expectedTransitions.Append(EMmcuoVolumeChanged));
	User::LeaveIfError(expectedTransitions.Append(EMmcuoMuteChanged));	
	User::LeaveIfError(expectedTransitions.Append(EMmcuoPolyphonyChanged));
	User::LeaveIfError(expectedTransitions.Append(EMmcuoInstrumentChanged));
	User::LeaveIfError(expectedTransitions.Append(EMmcuoMetaDataEntryFound));
	User::LeaveIfError(expectedTransitions.Append(EMmcuoVolumeChanged));
	User::LeaveIfError(expectedTransitions.Append(EMmcuoMuteChanged));
	User::LeaveIfError(expectedTransitions.Append(EMmcuoPolyphonyChanged));
	User::LeaveIfError(expectedTransitions.Append(EMmcuoInstrumentChanged));
	User::LeaveIfError(expectedTransitions.Append(EMmcuoMetaDataEntryFound));
	User::LeaveIfError(expectedTransitions.Append(EMmcuoMipMessageReceived));
	User::LeaveIfError(expectedTransitions.Append(EMmcuoMipMessageReceived));

}

void CTestMidiClntThirteenMidiEvents::MmcuoTempoChanged(TInt /*aMicroBeatsPerMinute*/)
{
	if (expectedTransitions[iCurrentTransion] == EMmcuoTempoChanged)
		{
		INFO_PRINTF1(_L("CTestMidiClntThirteenMidiEvents::MmcuoTempoChanged callback"));	
		iCurrentTransion++;
		}
	else
		{
		INFO_PRINTF1(_L("CTestMidiClntThirteenMidiEvents::MmcuoTempoChanged callback not expected"));	
		expectedTransitions[iCurrentTransion] = EMmcuoInvalid;
		}
}

void CTestMidiClntThirteenMidiEvents::MmcuoVolumeChanged(TInt /*aChannel*/,TReal32 /*aVolumeInDecibels*/)
{
	if (expectedTransitions[iCurrentTransion] == EMmcuoVolumeChanged)
		{
		INFO_PRINTF1(_L("CTestMidiClntThirteenMidiEvents::MmcuoVolumeChanged callback"));	
		iCurrentTransion++;
		}
	else
		{
		INFO_PRINTF1(_L("CTestMidiClntThirteenMidiEvents::MmcuoVolumeChanged callback not expected"));
		expectedTransitions[iCurrentTransion] = EMmcuoInvalid;
		}	
}

void CTestMidiClntThirteenMidiEvents::MmcuoMuteChanged(TInt /*aChannel*/,TBool /*aMuted*/)
{
	if (expectedTransitions[iCurrentTransion] == EMmcuoMuteChanged)
		{
		INFO_PRINTF1(_L("CTestMidiClntThirteenMidiEvents::MmcuoMuteChanged callback"));	
		iCurrentTransion++;
		}
	else
		{
		INFO_PRINTF1(_L("CTestMidiClntThirteenMidiEvents::MmcuoMuteChanged callback not expected"));
		expectedTransitions[iCurrentTransion] = EMmcuoInvalid;
		}	
}


void CTestMidiClntThirteenMidiEvents::MmcuoMetaDataEntryFound(const TInt /*aMetaDataEntryId*/,const TTimeIntervalMicroSeconds& /*aPosition*/)
{
	if (expectedTransitions[iCurrentTransion] == EMmcuoMetaDataEntryFound)
		{
		INFO_PRINTF1(_L("CTestMidiClntThirteenMidiEvents::MmcuoMetaDataEntryFound callback"));	
		iCurrentTransion++;
		}
	else
		{
		INFO_PRINTF1(_L("CTestMidiClntThirteenMidiEvents::MmcuoMetaDataEntryFound callback not expected"));	
		expectedTransitions[iCurrentTransion] = EMmcuoInvalid;
		}	
}

void CTestMidiClntThirteenMidiEvents::MmcuoMipMessageReceived(const RArray<TMipMessageEntry>& /*aEntry*/)
{
	if (expectedTransitions[iCurrentTransion] == EMmcuoMipMessageReceived)
		{
		INFO_PRINTF1(_L("CTestMidiClntThirteenMidiEvents::MmcuoMipMessageReceived callback"));	
		iCurrentTransion++;
		}
	else
		{
		INFO_PRINTF1(_L("CTestMidiClntThirteenMidiEvents::MmcuoMipMessageReceived callback not expected"));
		expectedTransitions[iCurrentTransion] = EMmcuoInvalid;
		}	
}

void CTestMidiClntThirteenMidiEvents::MmcuoPolyphonyChanged(TInt /*aNewPolyphony*/)
{
	if (expectedTransitions[iCurrentTransion] == EMmcuoPolyphonyChanged)
		{
		INFO_PRINTF1(_L("CTestMidiClntThirteenMidiEvents::MmcuoPolyphonyChanged callback"));	
		iCurrentTransion++;
		}
	else
		{
		INFO_PRINTF1(_L("CTestMidiClntThirteenMidiEvents::MmcuoPolyphonyChanged callback not expected"));
		expectedTransitions[iCurrentTransion] = EMmcuoInvalid;
		}
}

void CTestMidiClntThirteenMidiEvents::MmcuoInstrumentChanged(TInt /*aChannel*/,TInt /*aBankId*/,TInt /*aInstrumentId*/)
{
	if (expectedTransitions[iCurrentTransion] == EMmcuoInstrumentChanged)
		{
		INFO_PRINTF1(_L("CTestMidiClntThirteenMidiEvents::MmcuoInstrumentChanged callback"));	
		iCurrentTransion++;
		}
	else
		{
		INFO_PRINTF1(_L("CTestMidiClntThirteenMidiEvents::MmcuoInstrumentChanged callback not expected"));
		expectedTransitions[iCurrentTransion] = EMmcuoInvalid;
		}
}


TVerdict CTestMidiClntThirteenMidiEvents::DoTestL(CMidiClientUtility* aMidi)
	{
	TVerdict ret = EPass;
	TInt expErr = KErrNone;
	
	InitialiseExpectedTransArrayL();
	
	TPtrC fileName;
	if(!GetStringFromConfig(_L("SectionOne"),_L("filename"),fileName))
		{
		return EInconclusive;
		}			

	INFO_PRINTF1(_L("CMidiClientUtility: Play midi file"));
	// expected results
	switch(iTestType)
		{
		case ETestValid:
			expErr = KErrNone;
			break;
		case ETestNoPlugin:
			expErr = KErrNotSupported;
			break;
		case ETestInvalidState:
			expErr = KErrUnknown;
			break;
		default:
			ERR_PRINTF1(_L("Error - invalid test step type"));
			return EInconclusive;
		}
	
	iCurrentState = EMidiStateClosedDisengaged;
	aMidi->OpenFile(fileName);
	INFO_PRINTF1(_L("Waiting for EMidiStateOpenDisengaged state..."));
	CActiveScheduler::Start();	// EMidiStateClosedDisengaged -> EMidiStateOpenDisengaged
	if (iCurrentState != EMidiStateOpenDisengaged)
		{
		ERR_PRINTF2(_L("Unexpected state (expected = EMidiStateOpenDisengaged, received = %d)"), iCurrentState);
		expectedTransitions.Close();
		return EFail;
		}

	aMidi->Play();
	if (expErr != iError)
		{
		ERR_PRINTF3(_L("Play gave error %d (expected %d)"),iError, expErr);
		expectedTransitions.Close();
		return EFail;
		}
	else
		{
		INFO_PRINTF3(_L("Play, %d = %d"), iError, expErr);
		}
	
	
	INFO_PRINTF1(_L("Waiting for EMidiStateOpenPlaying state..."));
	CActiveScheduler::Start();	// EMidiStateOpenDisengaged ->EMidiStateOpenPlaying
	if (iCurrentState != EMidiStateOpenPlaying)
		{
		ERR_PRINTF2(_L("Unexpected state (expected = EMidiStateOpenPlaying, received = %d)"), iCurrentState);
		expectedTransitions.Close();
		return EFail;
		}
	INFO_PRINTF1(_L("Waiting for EMidiStateOpenDisengaged state..."));
	CActiveScheduler::Start();	// EMidiStateOpenPlaying ->EMidiStateOpenEngaged
	if (iCurrentState != EMidiStateOpenEngaged)
		{
		ERR_PRINTF2(_L("Unexpected state (expected = EMidiStateOpenEngaged, received = %d)"), iCurrentState);
		expectedTransitions.Close();
		return EFail;
		}
	
	if (expectedTransitions.Find(EMmcuoInvalid) != KErrNotFound)
		{
		ERR_PRINTF1(_L("One of the transactions was lost"));
		expectedTransitions.Close();
		return EFail;
		}	
			
	expectedTransitions.Close();

	return ret;
	}

//------------------------------------------------------------------
// TEST 1: Test the transtion from EMidiStateClosedDisengaged to EMidiStateClosedEngaged.

CTestTrasitionFromEClosedToEClosedEngaged::CTestTrasitionFromEClosedToEClosedEngaged(const TDesC& aTestName, const TTestStepType aTestType)
	:CTestMmfMidiClntStep(aTestName, aTestType)
	{
	}

CTestTrasitionFromEClosedToEClosedEngaged* CTestTrasitionFromEClosedToEClosedEngaged::NewL(const TDesC& aTestName, const TTestStepType aTestType)
	{
	CTestTrasitionFromEClosedToEClosedEngaged* self = new(ELeave) CTestTrasitionFromEClosedToEClosedEngaged(aTestName, aTestType);
	return self;
	}

TVerdict CTestTrasitionFromEClosedToEClosedEngaged::DoTestL(CMidiClientUtility* aMidi)
	{
	//[ Local variables ]
	TVerdict ret = EPass;
	TInt expErr = KErrNone;
 
	INFO_PRINTF1(_L("CMidiClientUtility: Transition from EMidiStateClosedDisengaged to EMidiStateClosedEngaged"));
	//[ Expected results ]
	switch(iTestType)
		{
		case ETestValid:
			expErr = KErrNone;
			break;
		case ETestNoPlugin:
			expErr = KErrNotSupported;
			break;
		case ETestInvalidState:
			expErr = KErrUnknown;
			break;
		default:
			ERR_PRINTF1(_L("Error - invalid test step type"));
			return EInconclusive;
		}
	
	//[ Set the current state to EMidiStateClosedDisengaged ]
	iCurrentState = EMidiStateClosedDisengaged;
	
	//[ Retrieve the MIDI client utility state and check it is EMidiStateClosedDisengaged ]
	TMidiState MIDIClientState = aMidi->State();

	if (MIDIClientState == EMidiStateClosedDisengaged)
		{
		ERR_PRINTF1(_L("MIDI Client Utility ----> EMidiStateClosedDisengaged"));
		}
	else
		{
		ERR_PRINTF2(_L("MIDI Client Utility unexpected state %d (expected ----> EMidiStateClosedDisengaged)"), MIDIClientState);
		return EFail;
		}
		
	// [ Call Play() ]
	aMidi->Play();
	if (expErr != iError)
		{
		ERR_PRINTF3(_L("Play() gave error %d (expected %d)"),iError, expErr);
		return EFail;
		}

	CActiveScheduler::Start();	// EMidiStateClosedDisengaged -> EMidiStateClosedEngaged
	
	if (iCurrentState != EMidiStateClosedEngaged)
		{
		ERR_PRINTF2(_L("Unexpected state (expected = EMidiStateClosedEngaged, received = %d)"), iCurrentState);
		return EFail;
		}

	MIDIClientState = aMidi->State();
	
	if (MIDIClientState == EMidiStateClosedEngaged)
		{
		ERR_PRINTF1(_L("MIDI Client Utility ----> EMidiStateClosedEngaged"));
		}
	else
		{
		ERR_PRINTF2(_L("MIDI Client Utility unexpected state %d (expected ----> EMidiStateClosedEngaged)"), MIDIClientState);
		return EFail;
		}
	
	return ret;
	}


//------------------------------------------------------------------
// TEST 2: Test the transtion from EMidiStateClosedDisengaged to EMidiStateOpenEngaged.

CTestTrasitionFromEClosedToEOpenEngaged::CTestTrasitionFromEClosedToEOpenEngaged(const TDesC& aTestName, const TTestStepType aTestType)
	:CTestMmfMidiClntStep(aTestName, aTestType)
	{
	}

CTestTrasitionFromEClosedToEOpenEngaged* CTestTrasitionFromEClosedToEOpenEngaged::NewL(const TDesC& aTestName, const TTestStepType aTestType)
	{
	CTestTrasitionFromEClosedToEOpenEngaged* self = new(ELeave) CTestTrasitionFromEClosedToEOpenEngaged(aTestName, aTestType);
	return self;
	}

TVerdict CTestTrasitionFromEClosedToEOpenEngaged::DoTestL(CMidiClientUtility* aMidi)
	{
	//[ Local variables ]
	TVerdict ret = EPass;
	TInt expErr = KErrNone;

	INFO_PRINTF1(_L("CMidiClientUtility: Transition from EMidiStateClosedDisengaged to EMidiStateOpenEngaged"));
	//[ Expected results ]
	switch(iTestType)
		{
		case ETestValid:
			expErr = KErrNone;
			break;
		case ETestNoPlugin:
			expErr = KErrNotSupported;
			break;
		case ETestInvalidState:
			expErr = KErrUnknown;
			break;
		default:
			ERR_PRINTF1(_L("Error - invalid test step type"));
			return EInconclusive;
		}
	
	//[ Set the current state to EMidiStateClosedDisengaged ]
	iCurrentState = EMidiStateClosedDisengaged;
	
	//[ Retrieve the MIDI client utility state and check it is EMidiStateClosedDisengaged ]
	TMidiState MIDIClientState = aMidi->State();

	if (MIDIClientState == EMidiStateClosedDisengaged)
		{
		ERR_PRINTF1(_L("MIDI Client Utility ----> EMidiStateClosedDisengaged"));
		}
	else
		{
		ERR_PRINTF2(_L("MIDI Client Utility unexpected state %d (expected ----> EMidiStateClosedDisengaged)"), MIDIClientState);
		return EFail;
		}
		
	// [ Call OpenFile() ]
	TPtrC fileName;
	if(!GetStringFromConfig(_L("SectionOne"),_L("filename"),fileName))
		{
		return EInconclusive;
		}			
	aMidi->OpenFile(fileName);
	
	CActiveScheduler::Start();	// EMidiStateClosedDisengaged -> EMidiStateOpenDisengaged(EMidiStateOpenDisengaged)
	if (iCurrentState != EMidiStateOpenDisengaged)
		{
		ERR_PRINTF2(_L("Unexpected state (expected = EMidiStateOpenDisengaged, received = %d)"), iCurrentState);
		return EFail;
		}
		
	MIDIClientState = aMidi->State();
	if (MIDIClientState == EMidiStateOpenDisengaged)
		{
		ERR_PRINTF1(_L("MIDI Client Utility ----> EMidiStateOpenDisengaged"));
		}
	else
		{
		ERR_PRINTF2(_L("MIDI Client Utility unexpected state %d (expected ----> EMidiStateOpenDisengaged)"), MIDIClientState);
		return EFail;
		}
	
	// [ Call Play() ]
	aMidi->Play();
	if (expErr != iError)
		{
		ERR_PRINTF3(_L("Play() gave error %d (expected %d)"),iError, expErr);
		return EFail;
		}

	CActiveScheduler::Start();	// EMidiStateOpenDisengaged -> EMidiStateOpenPlaying
	
	if (iCurrentState != EMidiStateOpenPlaying)
		{
		ERR_PRINTF2(_L("Unexpected state (expected = EMidiStateOpenPlaying, received = %d)"), iCurrentState);
		return EFail;
		}

	MIDIClientState = aMidi->State();
	if (MIDIClientState == EMidiStateOpenPlaying)
		{
		ERR_PRINTF1(_L("MIDI Client Utility ----> EMidiStateOpenPlaying"));
		}
	else
		{
		ERR_PRINTF2(_L("MIDI Client Utility unexpected state %d (expected ----> EMidiStateOpenPlaying)"), MIDIClientState);
		return EFail;
		}
	
	CActiveScheduler::Start();	// EMidiStateOpenPlaying -> EMidiStateOpenEngaged
	
	if (iCurrentState != EMidiStateOpenEngaged)
		{
		ERR_PRINTF2(_L("Unexpected state (expected = EMidiStateOpenEngaged, received = %d)"), iCurrentState);
		return EFail;
		}
		
	MIDIClientState = aMidi->State();
	
	if (MIDIClientState == EMidiStateOpenEngaged)
		{
		ERR_PRINTF1(_L("MIDI Client Utility ----> EMidiStateOpenEngaged"));
		}
	else
		{
		ERR_PRINTF2(_L("MIDI Client Utility unexpected state %d (expected ----> EMidiStateOpenEngaged)"), MIDIClientState);
		return EFail;
		}
	
	return ret;
	}


//------------------------------------------------------------------
// TEST 3: Test the transtion from EMidiStateOpenEngaged to EMidiStateClosedEngaged.

CTestTrasitionFromEOpenEngagedToEClosedEngaged::CTestTrasitionFromEOpenEngagedToEClosedEngaged(const TDesC& aTestName, const TTestStepType aTestType)
	:CTestMmfMidiClntStep(aTestName, aTestType)
	{
	}

CTestTrasitionFromEOpenEngagedToEClosedEngaged* CTestTrasitionFromEOpenEngagedToEClosedEngaged::NewL(const TDesC& aTestName, const TTestStepType aTestType)
	{
	CTestTrasitionFromEOpenEngagedToEClosedEngaged* self = new(ELeave) CTestTrasitionFromEOpenEngagedToEClosedEngaged(aTestName, aTestType);
	return self;
	}

TVerdict CTestTrasitionFromEOpenEngagedToEClosedEngaged::DoTestL(CMidiClientUtility* aMidi)
	{
	//[ Local variables ]
	TVerdict ret = EPass;
	TInt expErr = KErrNone;

	INFO_PRINTF1(_L("CMidiClientUtility: Transition from EMidiStateOpenEngaged to EMidiStateClosedEngaged"));
	//[ Expected results ]
	switch(iTestType)
		{
		case ETestValid:
			expErr = KErrNone;
			break;
		case ETestNoPlugin:
			expErr = KErrNotSupported;
			break;
		case ETestInvalidState:
			expErr = KErrUnknown;
			break;
		default:
			ERR_PRINTF1(_L("Error - invalid test step type"));
			return EInconclusive;
		}
	
	//[ Set the current state to EMidiStateClosedDisengaged ]
	iCurrentState = EMidiStateClosedDisengaged;
	
	//[ Retrieve the MIDI client utility state and check it is EMidiStateClosedDisengaged ]
	TMidiState MIDIClientState = aMidi->State();

	if (MIDIClientState == EMidiStateClosedDisengaged)
		{
		ERR_PRINTF1(_L("MIDI Client Utility ----> EMidiStateClosedDisengaged"));
		}
	else
		{
		ERR_PRINTF2(_L("MIDI Client Utility unexpected state %d (expected ----> EMidiStateClosedDisengaged)"), MIDIClientState);
		return EFail;
		}
		
	// [ Call OpenFile() ]
	TPtrC fileName;
	if(!GetStringFromConfig(_L("SectionOne"),_L("filename"),fileName))
		{
		return EInconclusive;
		}			
	
	aMidi->OpenFile(fileName);
	INFO_PRINTF1(_L("Waiting for EMidiStateOpenDisengaged state..."));
	CActiveScheduler::Start();	// EMidiStateClosedDisengaged -> EMidiStateOpenDisengaged
	if (iCurrentState != EMidiStateOpenDisengaged)
		{
		ERR_PRINTF2(_L("Unexpected state (expected = EMidiStateOpenDisengaged, received = %d)"), iCurrentState);
		return EFail;
		}
	MIDIClientState = aMidi->State();
	
	if (MIDIClientState == EMidiStateOpenDisengaged)
		{
		ERR_PRINTF1(_L("MIDI Client Utility ----> EMidiStateOpenDisengaged"));
		}
	else
		{
		ERR_PRINTF2(_L("MIDI Client Utility unexpected state %d (expected ----> EMidiStateOpenDisengaged)"), MIDIClientState);
		return EFail;
		}
	
	// [ Call Play() ]
	aMidi->Play();
	if (expErr != iError)
		{
		ERR_PRINTF3(_L("Play() gave error %d (expected %d)"),iError, expErr);
		return EFail;
		}

	CActiveScheduler::Start();	// EMidiStateOpenDisengaged -> EMidiStateOpenPlaying
	
	if (iCurrentState != EMidiStateOpenPlaying)
		{
		ERR_PRINTF2(_L("Unexpected state (expected = EMidiStateClosedEngaged, received = %d)"), iCurrentState);
		return EFail;
		}

	MIDIClientState = aMidi->State();	
	if (MIDIClientState == EMidiStateOpenPlaying)
		{
		ERR_PRINTF1(_L("MIDI Client Utility ----> EMidiStateOpenPlaying"));
		}
	else
		{
		ERR_PRINTF2(_L("MIDI Client Utility unexpected state %d (expected ----> EMidiStateOpenPlaying)"), MIDIClientState);
		return EFail;
		}
	
	CActiveScheduler::Start();	// EMidiStateOpenPlaying -> EMidiStateOpenEngaged
	
	if (iCurrentState != EMidiStateOpenEngaged)
		{
		ERR_PRINTF2(_L("Unexpected state (expected = EMidiStateOpenEngaged, received = %d)"), iCurrentState);
		return EFail;
		}
		
	MIDIClientState = aMidi->State();
	if (MIDIClientState == EMidiStateOpenEngaged)
		{
		ERR_PRINTF1(_L("MIDI Client Utility ----> EMidiStateOpenEngaged"));
		}
	else
		{
		ERR_PRINTF2(_L("MIDI Client Utility unexpected state %d (expected ----> EMidiStateOpenEngaged)"), MIDIClientState);
		return EFail;
		}
		
		
	// [ Call Close() ]
	aMidi->Close();
	if (expErr != iError)
		{
		ERR_PRINTF3(_L("Play() gave error %d (expected %d)"),iError, expErr);
		return EFail;
		}

	CActiveScheduler::Start();	// EMidiStateOpenEngaged -> EMidiStateClosedEngaged
	
	if (iCurrentState != EMidiStateClosedEngaged)
		{
		ERR_PRINTF2(_L("Unexpected state (expected = EMidiStateClosedEngaged, received = %d)"), iCurrentState);
		return EFail;
		}

	MIDIClientState = aMidi->State();
	if (MIDIClientState == EMidiStateClosedEngaged)
		{
		ERR_PRINTF1(_L("MIDI Client Utility ----> EMidiStateClosedEngaged"));
		}
	else
		{
		ERR_PRINTF2(_L("MIDI Client Utility unexpected state %d (expected ----> EMidiStateClosedEngaged)"), MIDIClientState);
		return EFail;
		}
	
	return ret;
	}



//---------------------------------------------------------------------
// TEST 4: Test the transtion from EMidiStateClosedEngaged to EMidiStateClosedDisengaged.

CTestTrasitionFromEClosedEngagedToEClosed::CTestTrasitionFromEClosedEngagedToEClosed(const TDesC& aTestName, const TTestStepType aTestType)
	:CTestMmfMidiClntStep(aTestName, aTestType)
	{
	}

CTestTrasitionFromEClosedEngagedToEClosed* CTestTrasitionFromEClosedEngagedToEClosed::NewL(const TDesC& aTestName, const TTestStepType aTestType)
	{
	CTestTrasitionFromEClosedEngagedToEClosed* self = new(ELeave) CTestTrasitionFromEClosedEngagedToEClosed(aTestName, aTestType);
	return self;
	}

TVerdict CTestTrasitionFromEClosedEngagedToEClosed::DoTestL(CMidiClientUtility* aMidi)
	{
	//[ Local variables ]
	TVerdict ret = EPass;
	TInt expErr = KErrNone;

	INFO_PRINTF1(_L("CMidiClientUtility: Transition from EMidiStateClosedEngaged to EMidiStateClosedDisengaged"));
	//[ Expected results ]
	switch(iTestType)
		{
		case ETestValid:
			expErr = KErrNone;
			break;
		case ETestNoPlugin:
			expErr = KErrNotSupported;
			break;
		case ETestInvalidState:
			expErr = KErrUnknown;
			break;
		default:
			ERR_PRINTF1(_L("Error - invalid test step type"));
			return EInconclusive;
		}
	
	//[ Set the current state to EMidiStateClosedDisengaged ]
	iCurrentState = EMidiStateClosedDisengaged;
	
	//[ Retrieve the MIDI client utility state and check it is EMidiStateClosedDisengaged ]
	TMidiState MIDIClientState = aMidi->State();

	if (MIDIClientState == EMidiStateClosedDisengaged)
		{
		ERR_PRINTF1(_L("MIDI Client Utility ----> EMidiStateClosedDisengaged"));
		}
	else
		{
		ERR_PRINTF2(_L("MIDI Client Utility unexpected state %d (expected ----> EMidiStateClosedDisengaged)"), MIDIClientState);
		return EFail;
		}
		
	// [ Call OpenFile() ]
	TPtrC fileName;
	if(!GetStringFromConfig(_L("SectionOne"),_L("filename"),fileName))
		{
		return EInconclusive;
		}			
	
	aMidi->OpenFile(fileName);
	INFO_PRINTF1(_L("Waiting for EMidiStateOpenDisengaged state..."));
	CActiveScheduler::Start();	// EMidiStateClosedDisengaged -> EMidiStateOpenDisengaged
	if (iCurrentState != EMidiStateOpenDisengaged)
		{
		ERR_PRINTF2(_L("Unexpected state (expected = EMidiStateOpenDisengaged, received = %d)"), iCurrentState);
		return EFail;
		}
		
	MIDIClientState = aMidi->State();
	if (MIDIClientState == EMidiStateOpenDisengaged)
		{
		ERR_PRINTF1(_L("MIDI Client Utility ----> EMidiStateOpenDisengaged"));
		}
	else
		{
		ERR_PRINTF2(_L("MIDI Client Utility unexpected state %d (expected ----> EMidiStateOpenDisengaged)"), MIDIClientState);
		return EFail;
		}
	
	// [ Call Play() ]
	aMidi->Play();
	if (expErr != iError)
		{
		ERR_PRINTF3(_L("Play() gave error %d (expected %d)"),iError, expErr);
		return EFail;
		}

	CActiveScheduler::Start();	// EMidiStateOpenDisengaged -> EMidiStateOpenPlaying
	
	if (iCurrentState != EMidiStateOpenPlaying)
		{
		ERR_PRINTF2(_L("Unexpected state (expected = EMidiStateOpenPlaying, received = %d)"), iCurrentState);
		return EFail;
		}

	MIDIClientState = aMidi->State();
	if (MIDIClientState == EMidiStateOpenPlaying)
		{
		ERR_PRINTF1(_L("MIDI Client Utility ----> EMidiStateOpenPlaying"));
		}
	else
		{
		ERR_PRINTF2(_L("MIDI Client Utility unexpected state %d (expected ----> EMidiStateOpenPlaying)"), MIDIClientState);
		return EFail;
		}
	
	CActiveScheduler::Start();	// EMidiStateOpenPlaying -> EMidiStateOpenEngaged
	
	if (iCurrentState != EMidiStateOpenEngaged)
		{
		ERR_PRINTF2(_L("Unexpected state (expected = EMidiStateOpenEngaged, received = %d)"), iCurrentState);
		return EFail;
		}
		
	MIDIClientState = aMidi->State();
	
	if (MIDIClientState == EMidiStateOpenEngaged)
		{
		ERR_PRINTF1(_L("MIDI Client Utility ----> EMidiStateOpenEngaged"));
		}
	else
		{
		ERR_PRINTF2(_L("MIDI Client Utility unexpected state %d (expected ----> EMidiStateOpenEngaged)"), MIDIClientState);
		return EFail;
		}
		
		
	// [ Call Close() ]
	aMidi->Close();
	if (expErr != iError)
		{
		ERR_PRINTF3(_L("Play() gave error %d (expected %d)"),iError, expErr);
		return EFail;
		}

	CActiveScheduler::Start();	// EMidiStateOpenEngaged -> EMidiStateClosedEngaged
	
	if (iCurrentState != EMidiStateClosedEngaged)
		{
		ERR_PRINTF2(_L("Unexpected state (expected = EMidiStateClosedEngaged, received = %d)"), iCurrentState);
		return EFail;
		}

	MIDIClientState = aMidi->State();
	if (MIDIClientState == EMidiStateClosedEngaged)
		{
		ERR_PRINTF1(_L("MIDI Client Utility ----> EMidiStateClosedEngaged"));
		}
	else
		{
		ERR_PRINTF2(_L("MIDI Client Utility unexpected state %d (expected ----> EMidiStateClosedEngaged)"), MIDIClientState);
		return EFail;
		}
	
	// [ Call Stop() ]
	TTimeIntervalMicroSeconds fadeOutDuration(0);
	aMidi->Stop(fadeOutDuration);
	if (expErr != iError)
		{
		ERR_PRINTF3(_L("Play() gave error %d (expected %d)"),iError, expErr);
		return EFail;
		}

	CActiveScheduler::Start();	// EMidiStateClosedEngaged -> EMidiStateClosedDisengaged
	
	if (iCurrentState != EMidiStateClosedDisengaged)
		{
		ERR_PRINTF2(_L("Unexpected state (expected = EMidiStateClosedDisengaged, received = %d)"), iCurrentState);
		return EFail;
		}

	MIDIClientState = aMidi->State();
	
	if (MIDIClientState == EMidiStateClosedDisengaged)
		{
		ERR_PRINTF1(_L("MIDI Client Utility ----> EMidiStateClosedDisengaged"));
		}
	else
		{
		ERR_PRINTF2(_L("MIDI Client Utility unexpected state %d (expected ----> EMidiStateClosedDisengaged)"), MIDIClientState);
		return EFail;
		}
	return ret;
	}
	
//------------------------------------------------------------------------
// This test checks for opening the file using a FileHandle more than
// once.
	
	
CTestMidiOpenFileHandleRepeat::CTestMidiOpenFileHandleRepeat(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName,const TBool aPlay)
	:CTestMmfMidiClntStep(aTestName, ETestValid),
	 iPlay(aPlay)
	{
	iSectName = aSectName;
	iKeyName = aKeyName;
	}

CTestMidiOpenFileHandleRepeat* CTestMidiOpenFileHandleRepeat::NewL(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName,const TBool aPlay)
	{
	CTestMidiOpenFileHandleRepeat* self = new(ELeave) CTestMidiOpenFileHandleRepeat(aTestName, aSectName, aKeyName, aPlay);
	return self;
	}

TVerdict CTestMidiOpenFileHandleRepeat::DoTestStepL()
	{
	TPtrC filename;
	if(!GetStringFromConfig(iSectName,iKeyName,filename))
		return EInconclusive;
	TVerdict ret = EFail;
	INFO_PRINTF1(_L("Test to check whether OpenFile(const RFile& aFile) works fine when opened more than once using the same filehandle "));
	
	CMidiClientUtility *midiClient = CMidiClientUtility::NewL(*this);
	if (!midiClient)
		{
		ERR_PRINTF1(_L("Could not create a CMidiClientUtility"));
		return EInconclusive;
		}
	CleanupStack::PushL(midiClient);
	RFs fs;
	RFile file;
	fs.Connect();
	CleanupClosePushL(fs);
	
	TMMFMessageDestinationPckg dummyPckg;
	TInt dummyFunc = 0; //EDevMidiOff;
	TBuf8<8> dummyBuff;
	midiClient->CustomCommandSyncL(dummyPckg, dummyFunc, dummyBuff, dummyBuff, dummyBuff);
	TInt err = fs.ShareProtected();
	if ((err = file.Open(fs, filename, EFileRead )) == KErrNone)
		{
		INFO_PRINTF1(_L("Inside the first call"));
		midiClient->OpenFile(file);//opening the file for the first time using the handle
		CActiveScheduler::Start();
		midiClient->Close();
		CActiveScheduler::Start();
		file.Close();
		}
	err = file.Open(fs, filename, EFileRead);//Opening the file for the second time 
	if(err==KErrNone)						 //shouldn't give inuse error at this point	
		{
		INFO_PRINTF1(_L("Inside the second call"));
		midiClient->OpenFile(file);
		CActiveScheduler::Start();
		midiClient->Close();
		CActiveScheduler::Start();
		file.Close();
		ret=EPass;
		}
	else
		INFO_PRINTF2(_L("Failed to open the file the second time returned with err = %d"),err);	
	
	//To check whether OpenFile(TDesC& aFileName) works fine when opened more than once.
	_LIT (KFilename,"c:\\MidiClntITestData\\midi.mid");
	midiClient->OpenFile(KFilename);//First Open(TDesC& aFileName)
	// Wait for initialisation callback
	INFO_PRINTF1(_L("CMidiClientUtility: Opening file"));
	CActiveScheduler::Start();
    if(iError != KErrNone)
    	{
    	INFO_PRINTF2(_L("Failed to open the file.Failed with error %d"),err);
    	}
    midiClient->Close();
    CActiveScheduler::Start();
    midiClient->OpenFile(KFilename);//Second Open(TDesC& aFileName)
    // Wait for initialisation callback
    INFO_PRINTF1(_L("CMidiClientUtility: Opening file for the second time"));
	CActiveScheduler::Start();
    if(iError != KErrNone)
    	{
    	INFO_PRINTF2(_L("Failed to open the file second time.Failed with error %d"),err);
    	}
    //Opening the file again without calling the close for the previous open
    midiClient->OpenFile(KFilename);
    INFO_PRINTF1(_L("CMidiClientUtility: Opening file for the second time without close"));
    CActiveScheduler::Start();
    if(iError != KErrNone)
    	{
    	INFO_PRINTF2(_L("Failed with error %d"),err);
    	}
    midiClient->Close();
    CActiveScheduler::Start();
    
	// To check that calling Open twice without closing the file inbetween does return an inuse error
	file.Open(fs, filename, EFileRead);//Opening for the first time
	User::After(1000000);
	err = file.Open(fs, filename, EFileRead);//Opening for the secondtime without closing the previous open, should return KErrInUse
	if(err != KErrInUse)
		{
		ret=EFail;
		INFO_PRINTF2(_L("Expected to return error -14, but returned error %d"),err);
		}
	file.Close();
	CleanupStack::PopAndDestroy(2); // fileServer, midiClient
	return	ret;
	}

TVerdict CTestMidiOpenFileHandleRepeat::DoTestL(CMidiClientUtility* /*aMidi*/)
	{
	return EPass;
	}

/*
Playing a midi file without opening a file and checking for its status during call back.
*/

CTestMidiClntPlayWithoutFile::CTestMidiClntPlayWithoutFile(const TDesC& aTestName)
	:CTestMmfMidiClntStep(aTestName, ETestValid)	 
	{}
	
CTestMidiClntPlayWithoutFile* CTestMidiClntPlayWithoutFile::NewL(const TDesC& aTestName)
	{
	CTestMidiClntPlayWithoutFile* self = new(ELeave) CTestMidiClntPlayWithoutFile(aTestName);
	return self;
	}


TVerdict CTestMidiClntPlayWithoutFile::DoTestStepL()
	{
	CMidiClientUtility* player = CMidiClientUtility::NewL(*this, EMdaPriorityNormal, EMdaPriorityPreferenceTimeAndQuality);
	TVerdict ret = EPass;
	iError = KErrNone;
	
	if (!player)
		{
		ERR_PRINTF1(_L("Could not create a CMidiClientUtility"));
		return EInconclusive;
		}
	CleanupStack::PushL(player);
          
	player->Play();
	CActiveScheduler::Start();
	
    if (iError != KErrNone)
	    {
	    ERR_PRINTF1(_L("Error while playing"));
		return EInconclusive;
		}
    
    TTimeIntervalMicroSeconds fadeOutDuration(0);
	player->Stop(fadeOutDuration);
	CActiveScheduler::Start();
	
	// Check for errors.
	if (iError != KErrNone)
		{
		ret = EFail;
		ERR_PRINTF2( _L("CMidiClientUtility failed with error %d"),iError );
		}
		
	INFO_PRINTF1(_L("CMidiClientUtility: Destroying"));
	CleanupStack::PopAndDestroy(player);
	
	return	ret;
	}

void CTestMidiClntPlayWithoutFile::MmcuoStateChanged(TMidiState aOldState,TMidiState aNewState,const TTimeIntervalMicroSeconds& /*aTime*/,TInt aError)
	{
	iError=aError;
	ERR_PRINTF2(_L("Old State: %d "),aOldState);	
	ERR_PRINTF2(_L("New State : %d "),aNewState);	
	ERR_PRINTF2(_L("Error Code : %d "),aError);
	
	if(aError == KErrNone)
		{
	     if((aOldState == EMidiStateClosedDisengaged) && (aNewState == EMidiStateClosedEngaged))	
	     	{
	     	INFO_PRINTF1(_L("State changed call back occurs,change from EMidiStateClosedDisengaged to EMidiStateClosedEngaged, with no error"));	
	     	}
	     else if((aOldState == EMidiStateClosedEngaged) && (aNewState == EMidiStateClosedDisengaged))	
	     	{
	      	INFO_PRINTF1(_L("State changed call back occurs,change from EMidiStateClosedEngaged to EMidiStateClosedDisengaged, with no error"));		
	     	}
	     else if((aOldState == EMidiStateClosedDisengaged) && (aNewState == EMidiStateOpenDisengaged))	
	     	{
	      	INFO_PRINTF1(_L("State changed call back occurs,change from EMidiStateClosedDisengaged to EMidiStateOpenDisengaged, with no error"));		
	     	}	
	     else if((aOldState == EMidiStateOpenDisengaged) && (aNewState == EMidiStateOpenPlaying))	
	     	{
	      	INFO_PRINTF1(_L("State changed call back occurs,change from EMidiStateOpenDisengaged to EMidiStateOpenPlaying, with no error"));		
	     	}		
	     else if((aOldState == EMidiStateOpenPlaying) && (aNewState == EMidiStateOpenDisengaged))	
	     	{
	      	INFO_PRINTF1(_L("State changed call back occurs,change from EMidiStateOpenPlaying to EMidiStateOpenDisengaged, with no error"));		
	     	}			
	     else
	        {
	        INFO_PRINTF1(_L("State changed call back occurs,the states remain the same with no error"));			
	        }
	    }
	 else
	 	{
	 	INFO_PRINTF1(_L("Error during calback"));			
	 	}       	
	 	
	CActiveScheduler::Stop();
	}

//------------------------------------------------------------------

/*
Opening a midi file and playing it and checking for its status during call back.
*/

CTestMidiClntOpenAndPlayFile::CTestMidiClntOpenAndPlayFile(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName)
	:CTestMmfMidiClntStep(aTestName, ETestValid)
	{
	iSectName = aSectName;
	iKeyName = aKeyName;
	}

CTestMidiClntOpenAndPlayFile* CTestMidiClntOpenAndPlayFile::NewL(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName)
	{
	CTestMidiClntOpenAndPlayFile* self = new(ELeave) CTestMidiClntOpenAndPlayFile(aTestName, aSectName, aKeyName);
	return self;
	}

TVerdict CTestMidiClntOpenAndPlayFile::DoTestStepL()
	{
	TVerdict ret = EPass;
	iError = KErrNone;
	TPtrC filename;
	if(!GetStringFromConfig(iSectName,iKeyName,filename))
		return EInconclusive;

	CMidiClientUtility* player = CMidiClientUtility::NewL(*this, EMdaPriorityNormal, EMdaPriorityPreferenceTimeAndQuality);
	if (!player)
		{
		ERR_PRINTF1(_L("Could not create a CMidiClientUtility"));
		return EInconclusive;
		}
	CleanupStack::PushL(player);
	
	player->OpenFile(filename);
	
	// Wait for initialisation callback
	INFO_PRINTF1(_L("CMidiClientUtility: Opening file"));
	CActiveScheduler::Start();

	player->Play();
	CActiveScheduler::Start();
	         
    if (iError != KErrNone)
	    {
	    ERR_PRINTF1(_L("Error while playing"));
		return EInconclusive;
		}
    
    TTimeIntervalMicroSeconds fadeOutDuration(0);
	player->Stop(fadeOutDuration);
	CActiveScheduler::Start();
		
	// Check for errors.
	if (iError != KErrNone)
		{
		ret = EFail;
		ERR_PRINTF2( _L("CMidiClientUtility failed with error %d"),iError );
		}
		
	INFO_PRINTF1(_L("CMidiClientUtility: Destroying"));
	CleanupStack::PopAndDestroy(player);
	
	return	ret;
	}

void CTestMidiClntOpenAndPlayFile::MmcuoStateChanged(TMidiState aOldState,TMidiState aNewState,const TTimeIntervalMicroSeconds& /*aTime*/,TInt aError)
	{
	iError=aError;
	ERR_PRINTF2(_L("Old State: %d "),aOldState);	
	ERR_PRINTF2(_L("New State : %d "),aNewState);	
	ERR_PRINTF2(_L("Error Code : %d "),aError);

   	if(aError == KErrNone)
		{
	     if((aOldState == EMidiStateClosedDisengaged) && (aNewState == EMidiStateClosedEngaged))	
	     	{
	     	INFO_PRINTF1(_L("State changed call back occurs,change from EMidiStateClosedDisengaged to EMidiStateClosedEngaged, with no error"));	
	     	}
	     else if((aOldState == EMidiStateClosedEngaged) && (aNewState == EMidiStateClosedDisengaged))	
	     	{
	      	INFO_PRINTF1(_L("State changed call back occurs,change from EMidiStateClosedEngaged to EMidiStateClosedDisengaged, with no error"));		
	     	}
	     else if((aOldState == EMidiStateClosedDisengaged) && (aNewState == EMidiStateOpenDisengaged))	
	     	{
	      	INFO_PRINTF1(_L("State changed call back occurs,change from EMidiStateClosedDisengaged to EMidiStateOpenDisengaged, with no error"));		
	     	}	
	     else if((aOldState == EMidiStateOpenDisengaged) && (aNewState == EMidiStateOpenPlaying))	
	     	{
	      	INFO_PRINTF1(_L("State changed call back occurs,change from EMidiStateOpenDisengaged to EMidiStateOpenPlaying, with no error"));		
	     	}		
	     else if((aOldState == EMidiStateOpenPlaying) && (aNewState == EMidiStateOpenDisengaged))	
	     	{
	      	INFO_PRINTF1(_L("State changed call back occurs,change from EMidiStateOpenPlaying to EMidiStateOpenDisengaged, with no error"));		
	     	}			
	     else
	        {
	        INFO_PRINTF1(_L("State changed call back occurs,the states remain the same with no error"));			
	        }
	    }
	 else
	 	{
	 	INFO_PRINTF1(_L("Error during calback"));			
	 	}       	
       
	CActiveScheduler::Stop();
	}

