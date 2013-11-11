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

#include "TSI_MMF_SDEVSOUND_0010_STEP.h"
#include "TSI_MMF_SDEVSOUND_SUITE.h"

#include <mmf/plugin/mmfhwdeviceimplementationuids.hrh>

CTestStepSDevSoundRecordDataCap* CTestStepSDevSoundRecordDataCap::NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName)
	{
	CTestStepSDevSoundRecordDataCap* self = new (ELeave) CTestStepSDevSoundRecordDataCap(aTestName, aSectName, aKeyName);
	return self;
	}

CTestStepSDevSoundRecordDataCap::CTestStepSDevSoundRecordDataCap(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName)
:	CSDSMPTSBase(aTestName, aSectName, aKeyName)
	{
	}

TVerdict CTestStepSDevSoundRecordDataCap::DoTestStepL( void )
	{
	INFO_PRINTF1( _L("CTestStepSDevSoundRecordDataCap : DevSoundMP"));
	iVerdict = EPass;

	iServerMonitor = CServerMonitor::NewL(this);

	// Start Server to play back from another process
	RTestServ server;
	User::LeaveIfError(server.Connect(_L("SDSCapTestServer")));
	CleanupClosePushL(server);
	
	RTestSession session1;
	User::LeaveIfError(session1.Open(server,_L("SecDevSndTS0010")));
	TRequestStatus* status = &iServerMonitor->ActiveStatus();
	// Start Server playback. The RunL of the CServerMonitor class will be called by the server when
	// playback has started
	session1.StartProcessing(*status);
	// Begin activescheduler loop. This will only exit when the whole test is complete 
	CActiveScheduler::Start();

	iVerdict = DoRecordData();

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

TVerdict CTestStepSDevSoundRecordDataCap::DoRecordData()
	{
	INFO_PRINTF1(_L("Testing Simple Tone Playback"));

	//Initialize
	TVerdict initOK = TestInitialize(EMMFStateRecording);
	if (initOK != EPass)
		{
		return EInconclusive;
		}

	TestSetPriority(KDevSoundPriorityLow);

	initOK = TestRecordInit();
	if (initOK != EPass)
		{
		return EInconclusive;
		}

	TestSetVolume(iMMFDevSound->MaxVolume() / 2);

	TFileName	aFilename = _L("C:\\rectest2.wav");

	return RecordDataFile(aFilename);
	}

/**
 *
 * TestInitialize
 * @param aDataType
 * @param aMode
 * @result TVerdict
 *
 */
TVerdict CTestStepSDevSoundRecordDataCap::TestInitialize(TMMFState aMode)
	{
	TFourCC pcm16(KMMFFourCCCodePCM16); //default to pcm16 data type

	iCallbackError = KErrNone;
	iExpectedValue = KErrNone;
	
	ResetCallbacks();
	iAL->InitialiseActiveListener();
	
	INFO_PRINTF1(_L("Initializing DevSound"));
	// Initialize
	TRAPD(err, iMMFDevSound->InitializeL(*this, pcm16, aMode));
	if (err)
		{
		WARN_PRINTF2 (_L("DevSound InitializeL left with error = %d"), err);
		return EInconclusive;
		}
	else
		{
		CActiveScheduler::Start();
		if (iCallbackError != iExpectedValue)
			{
			ERR_PRINTF3 (_L("DevSound InitializeComplete returned %d, expected %d"), iCallbackError, iExpectedValue);
			return EFail;
			}
		if (iCallbackArray[EInitComplete] != 1)
			{
			ERR_PRINTF2 (_L("DevSound InitializeComplete was called %d times, expected 1"), iCallbackArray[EInitComplete]);
			return EFail;
			}
		}
	return EPass;
	}

/**
 *
 * TestRecordInit
 * @result TVerdict
 *
 */
TVerdict CTestStepSDevSoundRecordDataCap::TestRecordInit()
	{
	ResetCallbacks();

	iAL->InitialiseActiveListener();
	INFO_PRINTF1(_L("Initializing Digital Audio Recording"));
	//get buffer from devsound
	TRAPD(err, iMMFDevSound->RecordInitL());
	// Start the active scheduler and catch the callback
 	CActiveScheduler::Start();
	if (err)
		{
		WARN_PRINTF2 (_L("DevSound RecordInitL left with error = %d"), err);
		return EFail;
		}
	else
		{
		if (iCallbackArray[EBuffToEmpty] != 1)
			{
			ERR_PRINTF2 (_L("DevSound BufferToBeEmpty was called %d times, expected 1"), iCallbackArray[EBuffToEmpty]);
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

/**
 *
 * TestRecordData
 * @result TVerdict
 *
 */
TVerdict CTestStepSDevSoundRecordDataCap::TestRecordData()
	{
	ResetCallbacks();

	iAL->InitialiseActiveListener();
	INFO_PRINTF1(_L("Recording Data to a Buffer"));
	iMMFDevSound->RecordData();
	// Start the active scheduler and catch the callback
 	CActiveScheduler::Start();

	if (iCallbackArray[EBuffToEmpty] != 1)
		{
		if (iCallbackArray[EBuffToEmpty] == 0 && iCallbackArray[ERecError] == 1)
			{
			INFO_PRINTF1(_L("DevSound RecordError was called 1 time.  Must be EOF."));
			}
		else
			{
			ERR_PRINTF2 (_L("DevSound BufferToBeEmptied was called %d times, expected 1"), iCallbackArray[EBuffToEmpty]);
			return EFail;
			}
		}

	TInt tot = GetCallbackTotal();
	if (tot > 1)
		{
		ERR_PRINTF2 (_L("DevSound called %d callbacks, expected 1"), tot);
		return EFail;
		}
	return EPass;
	}

/**
 *
 * TestDigitalPlayback
 * @param aNumSamples
 * @param aFilename
 * @param aDataType
 * @result TVerdict
 *
 */
TVerdict CTestStepSDevSoundRecordDataCap::RecordDataFile(TDesC& aFilename)
	{
	TInt	initOK = KErrNone;
	TInt	aNumSamples = 1;
	//open file
	RFs fs;
	RFile file;
	TInt err = fs.Connect();
	if (err != KErrNone)
		{
		ERR_PRINTF2 (_L("Could not connect to Filesystem. Error is %d"), err);
		return EInconclusive;
		}

	//Create output directory if it doesn't already exist
	fs.MkDir(_L("C:\\TSU_MMF_DEVSOUND_SUITE\\Output\\"));

	err = file.Replace(fs, aFilename, EFileWrite);
	if (err != KErrNone)
		{
		ERR_PRINTF2 (_L("Could not create output file. Error is %d"), err);
		return EInconclusive;
		}

	//SetGain
	TInt vol = iMMFDevSound->MaxVolume();	// Hacked... Fix this...
	TestSetGain(vol);

	TInt bufferCount = 0;
	while (bufferCount < aNumSamples && initOK == KErrNone && iCallbackError == KErrNone)
		{
		//DevSound Record
		initOK = TestRecordData();

		//Write buffer to file
		CMMFDataBuffer* buffer = STATIC_CAST (CMMFDataBuffer*, iBuffer);
		file.Write(buffer->Data());

		bufferCount ++;
		}

	TRequestStatus* stat = &iAL->iStatus;
	User::RequestComplete(stat, KErrNone);
	INFO_PRINTF3(_L("Total buffers recorded = %d, Expected %d"), bufferCount, aNumSamples);

	file.Close();
	fs.Close();

	if (initOK != KErrNone)
		{
		return EFail;
		}

	if (aNumSamples >= 0 && bufferCount != aNumSamples)
		{
		return EFail;
		}

	return EPass;
	}

void CTestStepSDevSoundRecordDataCap::DoProcess(TInt aError)
	{
	if (aError == KErrNone)
		{
		CActiveScheduler::Stop();
		InternalState nextState = iState;
		switch (iState)
			{
		case EWaitingForServer:
			nextState = EInitPlayer;
			break;
		case EInitPlayer : 
			nextState = EPlaying;
			break;
		case EPlaying :
			iVerdict = EPass;
			CActiveScheduler::Stop();
			break;
			}
		iState = nextState;
		}
	else 
		{
		iVerdict = EFail;
		INFO_PRINTF2(_L("Unexpected failure in test, error code %d"), aError);
		CActiveScheduler::Stop();
		}
	}

CTestStepSDevSoundRecordDataCap::~CTestStepSDevSoundRecordDataCap()
	{
	}
