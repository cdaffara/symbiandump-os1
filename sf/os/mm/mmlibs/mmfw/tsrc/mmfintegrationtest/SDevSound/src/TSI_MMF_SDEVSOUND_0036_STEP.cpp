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

#include "TSI_MMF_SDEVSOUND_0036_STEP.h"
#include "TSI_MMF_SDEVSOUND_SUITE.h"
#include "captestinfosession.h"

#include <mmf/plugin/mmfhwdeviceimplementationuids.hrh>
#include <mmf/server/devsoundstandardcustominterfaces.h>

CTestStepSDevSoundClientThreadNoCap* CTestStepSDevSoundClientThreadNoCap::NewL(const TDesC& aTestName, 
																			   const TDesC& aProcessTestName, 
																			   const TDesC& aSectName, 
																			   const TDesC& aKeyName,
																			   TBool aAttemptInitialCI)
	{
	CTestStepSDevSoundClientThreadNoCap* self = new (ELeave) CTestStepSDevSoundClientThreadNoCap(aTestName, aProcessTestName, aSectName, aKeyName, aAttemptInitialCI);
	return self;
	}

CTestStepSDevSoundClientThreadNoCap::CTestStepSDevSoundClientThreadNoCap(const TDesC& aTestName, 
																		 const TDesC& aProcessTestName, 
																		 const TDesC& aSectName, 
																		 const TDesC& aKeyName,
																		 TBool aAttemptInitialCI)
:	CSDSMPTSBase(aTestName, aSectName, aKeyName), iAttemptInitialCI(aAttemptInitialCI)
	{
	iProcessTestName = aProcessTestName;
	}

CTestStepSDevSoundClientThreadNoCap::~CTestStepSDevSoundClientThreadNoCap()
	{
	}

TVerdict CTestStepSDevSoundClientThreadNoCap::DoTestStepL( void )
	{
	INFO_PRINTF1( _L("CTestStepSDevSoundClientThreadNoCap : DevSoundMP"));
	iVerdict = EFail;

	iServerMonitor = CServerMonitor::NewL(this);
	
	RTestServ server;
	User::LeaveIfError(server.Connect(_L("SDSCapTestServer")));
	CleanupClosePushL(server);
	
	RTestSession session2;
	User::LeaveIfError(session2.Open(server,_L("SecDevSndTS0036")));
	CleanupClosePushL(session2);
	TRequestStatus status2 = KRequestPending;
	session2.StartProcessing(status2);
	User::WaitForRequest(status2);
	TPckgBuf<TThreadId> serverTid;
	session2.EndProcessingAndReturnResult(serverTid);
	iServerTid = serverTid();
	
	RTestServ mmddServer;
	User::LeaveIfError(mmddServer.Connect(_L("SDSMMDDTestServer")));
	CleanupClosePushL(mmddServer);
	RTestSession session1;
	User::LeaveIfError(session1.Open(mmddServer,_L("SecDevSndTS0036")));
	CleanupClosePushL(session1);
	
	TInt err = DoPlaySimpleTone();
	if (err == KErrNone)
		{
		if (iAttemptInitialCI)
			{
			// assume DoPlaySimpleTone() terminated early prior to InitializeL()
			iVerdict = EPass;
			}
		else
			{
			// Start playing simple tone in this thread
			TRequestStatus status = KRequestPending;
			session1.StartProcessing(status);
			iAL->InitialiseActiveListener();
			CActiveScheduler::Start();
			// Simple tone in this thread finished, wait until the 
			// simple tone playing in the other thread to finish
			User::WaitForRequest(status);
			iServerResult = status.Int();
			
			// Verify the results from both threads
			iVerdict = DoVerifyResult();
			TBuf8<256> message;
			session1.EndProcessingAndReturnResult(message);		
			}
		}
	else
		{
		iVerdict = EInconclusive;
		}
	CleanupStack::PopAndDestroy(4, &server);	// session1, mmddServer, session2, server
	return iVerdict;
	}

TInt CTestStepSDevSoundClientThreadNoCap::DoPlaySimpleTone()
	{
	INFO_PRINTF1(_L("Testing Simple Tone Playback"));
	TInt freq = 1000;
	TTimeIntervalMicroSeconds dur(10000000);	// play for some period of time, eg 10secs
	
	TUid KInvalidCIUid = KUidCustomInterfaceDevSoundFileBlockLength; // use block length CI
	TAny* tempPtr = NULL;
	if (iAttemptInitialCI)
		{
		// if requested we get a CustomInferface() prior to SetClientThreadInfo. The latter
		// should then fail since it is called prior to InitializeL
		tempPtr = iMMFDevSound->CustomInterface(KInvalidCIUid); // request unknown i/f
		if (tempPtr)
			{
			INFO_PRINTF1(_L("CustomInterface() expected to return NULL but did not!"));
			return KErrGeneral;
			}
		}

	TInt err = iMMFDevSound->SetClientThreadInfo(iServerTid);
	
	TInt expectedErr = KErrNone;
#ifdef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
	if (iAttemptInitialCI)
		{
		// characterisation says that SetClientThreadInfo() called 2nd returns an error
		// but the old DevSound works differently
		expectedErr = KErrNotReady;
		}
#endif // SYMBIAN_MULTIMEDIA_A3FDEVSOUND

	if (err != expectedErr)
		{
		// with iAttemptInitialCI we expect KErrNotReady otherwise KErrNone
		INFO_PRINTF3(_L("Unexpected return from SetClientThreadInfo(%d, not %d)"), err, expectedErr);
		return err==KErrNone ? KErrGeneral : err; // always return a real error
		}
		
	// always ask for CI after SetClientThreadInfo(). This should be a NOP, and do no harm
	// call just to make sure no Kern-exec-3 or similar occurs
	tempPtr = iMMFDevSound->CustomInterface(KInvalidCIUid); // request unknown i/f
	if (tempPtr)
		{
		INFO_PRINTF1(_L("CustomInterface() expected to return NULL but did not!"));
		return KErrGeneral;
		}
		
	// if iAttemptInitialCI, the SetClientThreadInfo() would have failed with KErrNotReady
	// return now
	if (iAttemptInitialCI)
		{
		return KErrNone;
		}

	//Initialize
	TVerdict initOK = TestInitialize(EMMFStateTonePlaying);
	if (initOK != EPass)
		{
		return KErrGeneral;
		}

	TestSetPriority(KDevSoundPriorityHigh);
	TestSetVolume(iMMFDevSound->MaxVolume());
	iExpectedValue = KErrInUse;
	return TestPlayTone(freq, dur);
	}
	
TInt CTestStepSDevSoundClientThreadNoCap::TestPlayTone(TInt aFreq, TTimeIntervalMicroSeconds aDur)
	{
	iCallbackError = KErrNone;
	ResetCallbacks();

	INFO_PRINTF1(_L("Playing Simple Tone"));
	TRAPD(err, iMMFDevSound->PlayToneL(aFreq, aDur));
	if (err)
		{
		WARN_PRINTF2 (_L("DevSound PlayToneL left with error = %d"), err);
		iCallbackError = err;
		}
	return err;
	}

TVerdict CTestStepSDevSoundClientThreadNoCap::DoVerifyResult()
	{
	if (iCallbackError != iExpectedValue)
		{
		ERR_PRINTF3 (_L("DevSound ToneFinished returned %d, expected %d"), iCallbackError, iExpectedValue);
		return EFail;
		}
	if (iServerResult != KErrUnderflow)
		{
		ERR_PRINTF2 (_L("DevSound(server-side) ToneFinished returned %d, expected 0"), iServerResult);
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
	return EPass;
	}
