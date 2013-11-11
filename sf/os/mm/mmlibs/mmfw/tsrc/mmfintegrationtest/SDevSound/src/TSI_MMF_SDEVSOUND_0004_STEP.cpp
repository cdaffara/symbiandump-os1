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
// TSI_MMF_SDEVSOUND_STEP.CPP
// 
//

#include <e32base.h>

#include <simulprocclient.h>

#include "TSI_MMF_SDEVSOUND_0004_STEP.h"
#include "TSI_MMF_SDEVSOUND_SUITE.h"

#include <mmf/plugin/mmfhwdeviceimplementationuids.hrh>

CTestStepSDevSoundPlayDTMFToneCap* CTestStepSDevSoundPlayDTMFToneCap::NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName)
	{
	CTestStepSDevSoundPlayDTMFToneCap* self = new (ELeave) CTestStepSDevSoundPlayDTMFToneCap(aTestName, aSectName, aKeyName);
	return self;
	}

CTestStepSDevSoundPlayDTMFToneCap::CTestStepSDevSoundPlayDTMFToneCap(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName)
:	CSDSMPTSBase(aTestName, aSectName, aKeyName)
	{
	}

TVerdict CTestStepSDevSoundPlayDTMFToneCap::DoTestStepL( void )
	{
	INFO_PRINTF1( _L("CTestStepSDevSoundPlayDTMFToneCap : DevSoundMP"));
	iVerdict = EFail;

	iServerMonitor = CServerMonitor::NewL(this);

	// Start Server to play back from another process
	RTestServ server;
	User::LeaveIfError(server.Connect(_L("SDSCapTestServer")));
	CleanupClosePushL(server);
	
	RTestSession session1;
	User::LeaveIfError(session1.Open(server,_L("SecDevSndTS0004")));
	TRequestStatus* status = &iServerMonitor->ActiveStatus();
	// Start Server playback. The RunL of the CServerMonitor class will be called by the server when
	// playback has started
	session1.StartProcessing(*status);
	// Begin activescheduler loop. This will only exit when the whole test is complete 
	CActiveScheduler::Start();

	User::After(2000000);	// wait for 2 secs - allow server to get started
	iVerdict = DoPlayDTMFTone();

	// The test is complete. Now shut down the server and get any errors /messages from the server
	TBuf8<256> message;
	TVerdict verdict = session1.EndProcessingAndReturnResult(message);
	if (verdict != EPass)
		iVerdict = verdict;
	TBuf16<256> copymess;
	copymess.Copy(message);
	INFO_PRINTF2(_L("end processing and return result: %S"),&copymess);

	CleanupStack::PopAndDestroy(&server);
	return iVerdict;
	}

/**
 *
 * DoTestStepL
 * @result TVerdict
 *
 */

TVerdict CTestStepSDevSoundPlayDTMFToneCap::DoPlayDTMFTone()
	{
	INFO_PRINTF1(_L("Testing Simple Tone Playback"));

	//Initialize
	TVerdict initOK = TestInitialize(EMMFStateTonePlaying);
	if (initOK != EPass)
		{
		return EInconclusive;
		}

	TestSetPriority(KDevSoundPriorityLow);
	TestSetVolume(iMMFDevSound->MaxVolume());

	_LIT(KDTMFString,"01234, 56789, ABCDEF#*");

	iExpectedValue = KErrUnderflow;
	return TestPlayDTMFString(KDTMFString);
	}

/**
 *
 * TestPlayTone
 * @param aFreq
 * @param aDur
 * @result TVerdict
 *
 */
TVerdict CTestStepSDevSoundPlayDTMFToneCap::TestPlayDTMFString(const TDesC &aDTMFString)
	{
	iCallbackError = KErrNone;

	ResetCallbacks();

	INFO_PRINTF1(_L("Playing DTMF Tone"));
	TRAPD(err, iMMFDevSound->PlayDTMFStringL(aDTMFString));
	if (err)
		{
		WARN_PRINTF2 (_L("DevSound PlayToneL left with error = %d"), err);
		iCallbackError = err;
		return EFail;
		}
	else
		{
		iAL->InitialiseActiveListener();
		// Start the active scheduler and catch the callback
 		CActiveScheduler::Start();
		if (iCallbackError != iExpectedValue)
			{
			ERR_PRINTF3 (_L("DevSound ToneFinished returned %d, expected %d"), iCallbackError, iExpectedValue);
			return EFail;
			}
		if (iCallbackArray[EToneFinished] != 1)
			{
			ERR_PRINTF2 (_L("DevSound ToneFinished was called %d times, expected 1"), iCallbackArray[EToneFinished]);
			return EFail;
			}
		TInt tot = GetCallbackTotal();
		if (tot > 1)
			{
			ERR_PRINTF2 (_L("DevSound called %d callbacks, expected 1"), tot);
			return EFail;
			}
		}
	return EPass;
	}

CTestStepSDevSoundPlayDTMFToneCap::~CTestStepSDevSoundPlayDTMFToneCap()
	{
	}
