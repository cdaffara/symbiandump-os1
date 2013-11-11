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

#include "TSI_MMF_SDEVSOUND_0008_STEP.h"
#include "TSI_MMF_SDEVSOUND_SUITE.h"

#include <mmf/plugin/mmfhwdeviceimplementationuids.hrh>

CTestStepSDevSoundPlayDataCap* CTestStepSDevSoundPlayDataCap::NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName)
	{
	CTestStepSDevSoundPlayDataCap* self = new (ELeave) CTestStepSDevSoundPlayDataCap(aTestName, aSectName, aKeyName);
	return self;
	}

CTestStepSDevSoundPlayDataCap::CTestStepSDevSoundPlayDataCap(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName)
:	CSDSMPTSBase(aTestName, aSectName, aKeyName)
	{
	}

TVerdict CTestStepSDevSoundPlayDataCap::DoTestStepL( void )
	{
	INFO_PRINTF1( _L("CTestStepSDevSoundPlayDataCap : DevSoundMP"));
	iVerdict = EFail;

	iServerMonitor = CServerMonitor::NewL(this);

	// Start Server to play back from another process
	RTestServ server;
	User::LeaveIfError(server.Connect(_L("SDSCapTestServer")));
	CleanupClosePushL(server);
	
	RTestSession session1;
	User::LeaveIfError(session1.Open(server,_L("SecDevSndTS0008")));
	TRequestStatus* status = &iServerMonitor->ActiveStatus();
	// Start Server playback. The RunL of the CServerMonitor class will be called by the server when
	// playback has started
	session1.StartProcessing(*status);
	// Begin activescheduler loop. This will only exit when the whole test is complete 
	CActiveScheduler::Start();

	User::After(2000000);
	iVerdict = DoPlayData();

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

TVerdict CTestStepSDevSoundPlayDataCap::DoPlayData()
	{
	INFO_PRINTF1(_L("Testing Simple Tone Playback"));

	//Initialize
	TVerdict initOK = TestInitialize(EMMFStatePlaying);
	if (initOK != EPass)
		{
		return EInconclusive;
		}

	TestSetPriority(KDevSoundPriorityLow);

	initOK = TestPlayInit();
	if (initOK != EPass)
		{
		return EInconclusive;
		}

	TestSetVolume(iMMFDevSound->MaxVolume() / 2);

	_LIT(KTestFileName, "C:\\sdevsoundinttestdata\\mainTst.wav");
	TFileName filename(KTestFileName);

	return PlayDataFile(filename);
	}

/**
 *
 * TestInitialize
 * @param aDataType
 * @param aMode
 * @result TVerdict
 *
 */
TVerdict CTestStepSDevSoundPlayDataCap::TestInitialize(TMMFState aMode)
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
 * TestPlayInit
 * @result TVerdict
 *
 */
TVerdict CTestStepSDevSoundPlayDataCap::TestPlayInit()
	{
	ResetCallbacks();

	iAL->InitialiseActiveListener();

	//get buffer from devsound
	TRAPD(err, iMMFDevSound->PlayInitL());
	// Start the active scheduler and catch the callback
 	CActiveScheduler::Start();
	if (err)
		{
		WARN_PRINTF2 (_L("DevSound PlayInitL left with error = %d"), err);
		return EFail;
		}
	else
		{
		if (iCallbackArray[EBuffToFill] != 1)
			{
			ERR_PRINTF2 (_L("DevSound BufferToBeFilled was called %d times, expected 1"), iCallbackArray[EBuffToFill]);
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
 * TestPlayData
 * @result TVerdict
 *
 */
TVerdict CTestStepSDevSoundPlayDataCap::TestPlayData()
	{
	ResetCallbacks();

	iAL->InitialiseActiveListener();

	iMMFDevSound->PlayData();
	// Start the active scheduler and catch the callback
 	CActiveScheduler::Start();

	if (iCallbackArray[EBuffToFill] != 1)
		{
		if (iCallbackArray[EBuffToFill] == 0 && iCallbackArray[EPlayError] == 1)
			{
			INFO_PRINTF1(_L("DevSound PlayError was called 1 time.  Must be EOF."));
			}
		else
			{
			ERR_PRINTF2 (_L("DevSound BufferToBeFilled was called %d times, expected 1"), iCallbackArray[EBuffToFill]);
			return EFail;
			}
		}

	TInt tot = GetCallbackTotal();
	if (tot > 2)
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
TVerdict CTestStepSDevSoundPlayDataCap::PlayDataFile(TDesC& aFilename)
	{
	TInt		initOK = KErrNone;
	TInt		aNumSamples = -1;

	//open file
	RFs fs;
	RFile file;
	TInt err = fs.Connect();
	if (err != KErrNone)
		{
		ERR_PRINTF2 (_L("Could not connect to Filesystem. Error is %d"), err);
		return EInconclusive;
		}

	err = file.Open(fs, aFilename, EFileRead);
	if (err != KErrNone)
		{
		ERR_PRINTF2 (_L("Could not open input file. Error is %d"), err);
		fs.Close();
		return EInconclusive;
		}

	TInt bufferCount = 0;
	if (aNumSamples < 0)
		{// Play to EOF
		while (initOK == KErrNone && iCallbackError == KErrNone)
			{
			//read sizeof buffer from file
			CMMFDataBuffer*  buffer = STATIC_CAST (CMMFDataBuffer*, iBuffer);
			file.Read(buffer->Data());
			if (buffer->Data().Length()!= buffer->RequestSize())
				{
				INFO_PRINTF3(_L("Data length copied from file = %d. Expected %d. Must be EOF"), buffer->Data().Length(), buffer->RequestSize());
				iBuffer->SetLastBuffer(ETrue);
				}

			//DevSound Play
			initOK = TestPlayData();
			bufferCount ++;
			}
		}
	else
		{
		while (bufferCount < aNumSamples && initOK == KErrNone && iCallbackError == KErrNone)
			{
			//read sizeof buffer from file
			CMMFDataBuffer* buffer = STATIC_CAST (CMMFDataBuffer*, iBuffer);
			file.Read(buffer->Data());
			if (buffer->Data().Length()!= buffer->RequestSize())
				{
				INFO_PRINTF3(_L("Data length copied from file = %d. Expected %d. Must be EOF"), buffer->Data().Length(), buffer->RequestSize());
				iBuffer->SetLastBuffer(ETrue);
				}

			//DevSound Play
			initOK = TestPlayData();
			bufferCount ++;
			}
		TRequestStatus* stat = &iAL->iStatus;
		User::WaitForRequest(*stat);
		User::RequestComplete(stat, KErrNone);
		}

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

void CTestStepSDevSoundPlayDataCap::DoProcess(TInt aError)
	{
	if (aError == KErrNone)
		{
		CActiveScheduler::Stop();
		InternalState nextState = iState;
		switch (iState)
			{
		case EWaitingForServer:
//			CreatePlayer();
			nextState = EInitPlayer;
			break;
		case EInitPlayer : 
//			BeginPlayback();
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

CTestStepSDevSoundPlayDataCap::~CTestStepSDevSoundPlayDataCap()
	{
	}
