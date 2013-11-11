// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

// Test system includes
#include "TSU_MMF_SWCDWRAP_StepStartEncode.h"
#include "TSU_MMF_SWCDWRAP_TestDeviceImplementationUID.hrh"
#include "TSU_MMF_SWCDWRAP_RecordHwDevObserver.h"
#include "TSU_MMF_SWCDWRAP_AsyncTimer.h"
#include "TSU_MMF_SWCDWRAP_TestDevice.h"

/**
 * sets up test step hw device and observer
 */
TVerdict CTestStep_MMF_SWCDWRAP_StartTestEncode::InitializeTestStepUsingHwDeviceL(TUid aHwDeviceUid)
	{
	TVerdict verdict = EPass;

	verdict = CTestStep_MMF_SWCDWRAP::DoTestStepPreambleL();

	if (verdict == EPass)
		{

		TRAPD(err, iHwDevice = CMMFHwDevice::NewL(aHwDeviceUid));

		//check hw device has been created
		if (err != KErrNone)
			{
			INFO_PRINTF2(_L("could not create hw device plugin error %d" ), err );
			verdict = EInconclusive;
			}
		}
	
	if (verdict == EPass)
		{
		iActiveScheduler = CActiveScheduler::Current();
		if (!iActiveScheduler) iActiveScheduler = new(ELeave)CActiveScheduler;
		CActiveScheduler::Install(iActiveScheduler);

		iHwDeviceObserver = new(ELeave)CTestRecordMMFHwDeviceObserver(iHwDevice);

		THwDeviceInitParams	devInfo;
		devInfo.iHwDeviceObserver = iHwDeviceObserver;
		TInt error = iHwDevice->Init(devInfo);
		
		if (error == KErrNone)
			{
			TUid recordCustomInterfaceUid;
			recordCustomInterfaceUid.iUid = KMmfRecordSettingsCustomInterface;
			iRecordCustomInterface = 
			(MRecordCustomInterface*)iHwDevice->CustomInterface(recordCustomInterfaceUid);
			iRecordCustomInterface->SetGain(250);
			
			TTaskConfig taskConfig;
			taskConfig.iUid = KUidRefDevSoundTaskConfig;
			taskConfig.iRate = 8000;
			taskConfig.iStereoMode = ETaskMono;
			
			if (iHwDevice->SetConfig(taskConfig) != KErrNone)
				{
				return EFail;
				}
			}
		else
			{
			verdict = EInconclusive;
			}
		}

	return verdict;
	}

/**
 * Pre-amble for sw codec test step using 'null' codec hw device
 */
TVerdict CTestStep_MMF_SWCDWRAP_StartTestEncode::DoTestStepPreambleL()
	{
	TVerdict verdict = EPass;

	TUid hwDeviceUid;
	hwDeviceUid.iUid = KMmfUidTSUSWCDWRAPTestDevice;

	verdict = InitializeTestStepUsingHwDeviceL(hwDeviceUid);

	return verdict;
	}


/**
 * Pre-amble for sw codec test step using 2:1 buffer ratio codec
 */
TVerdict CTestStep_MMF_SWCDWRAP_StartTest2To1Encode::DoTestStepPreambleL()
	{
	TVerdict verdict = EPass;

	TUid hwDeviceUid;
	hwDeviceUid.iUid = KMmfUidTSUSWCDWRAPTest2To1Device;

	verdict = InitializeTestStepUsingHwDeviceL(hwDeviceUid);

	return verdict;
	}

/**
 * Post-amble for sw codec test step
 */
TVerdict CTestStep_MMF_SWCDWRAP_StartTestEncode::DoTestStepPostambleL()
	{
	delete iHwDevice;
	delete iHwDeviceObserver;
	delete iActiveScheduler;

	return CTestStep_MMF_SWCDWRAP::DoTestStepPostambleL();
	}
 


CTestStep_MMF_SWCDWRAP_U_0100::CTestStep_MMF_SWCDWRAP_U_0100()
/** Constructor
 */
	{
	iTestStepName = _L("MM-MMF-SWCDWRAP-U-0100-HP");
	}
 

TVerdict CTestStep_MMF_SWCDWRAP_U_0100::DoTestStepL( void )
/**
 * Use case: N/A
 * Use Start() to record one buffer
 * @test Req. under test REQ1931.2
 */
	{
	 
	TVerdict verdict = EPass;
	TInt recordedFileSize = 0;

	TFileName filename = _L("\\mm\\mmf\\testfiles\\swcdwrap\\TSU_MMF_SWCDWRAP_record.raw");
	TInt error = iHwDeviceObserver->WriteBufferDirectToFileAsync(filename, 1);
 
	if (error == KErrNone)
		{
		TInt error = iHwDevice->Start(EDevEncode, EDevInFlow);
		if (error == KErrNone)
			{
			INFO_PRINTF1(_L("***** RECORDING *****"));
			iActiveScheduler->Start();
			error = iHwDeviceObserver->ErrorStatus();
			if (error != KErrNone)
				{
				INFO_PRINTF2(_L("error during audio record %d" ), error );
				verdict = EFail;
				}
			if (iHwDeviceObserver->NumberOfBuffersObserved() != 1)
				{
				INFO_PRINTF1(_L("incorrect number of buffers"));
				verdict = EFail;
				}
			error = iHwDeviceObserver->WriteFile().Size(recordedFileSize);
			if (error != KErrNone)
				{
				INFO_PRINTF2(_L("problem with recorded file error %d"), error);
				verdict = EFail;
				}
			if (recordedFileSize != KTestBufferSize)
				{
				INFO_PRINTF1(_L("recorded file was not the expected size"));
				verdict = EFail;
				}
			}
		else
			{
			INFO_PRINTF2(_L("error on hw device start %d" ), error );
			verdict = EFail;
			}
		} 
	else
		{
		INFO_PRINTF2(_L("problem opening file to record to error %d" ), error );
		verdict = EInconclusive;
		}

	return verdict ;
	} 


CTestStep_MMF_SWCDWRAP_U_0101::CTestStep_MMF_SWCDWRAP_U_0101()
/** Constructor
 */
	{
	iTestStepName = _L("MM-MMF-SWCDWRAP-U-0101-HP");
	}
 

TVerdict CTestStep_MMF_SWCDWRAP_U_0101::DoTestStepL( void )
/**
 * Use case: N/A
 * Use Start() to record five buffers
 * @test Req. under test REQ1931.2
 */
	{
	 
	TVerdict verdict = EPass;
	TInt recordedFileSize = 0;

	TFileName filename = _L("\\mm\\mmf\\testfiles\\swcdwrap\\TSU_MMF_SWCDWRAP_record.raw");
	TInt error = iHwDeviceObserver->WriteBufferDirectToFileAsync(filename, 5);
 
	if (error == KErrNone)
		{
		TInt error = iHwDevice->Start(EDevEncode, EDevInFlow);
		if (error == KErrNone)
			{
			INFO_PRINTF1(_L("***** RECORDING *****"));
			iActiveScheduler->Start();
			error = iHwDeviceObserver->ErrorStatus();
			if (error != KErrNone)
				{
				INFO_PRINTF2(_L("error during audio record %d" ), error );
				verdict = EFail;
				}
			if (iHwDeviceObserver->NumberOfBuffersObserved() != 5)
				{
				INFO_PRINTF1(_L("incorrect number of buffers"));
				verdict = EFail;
				}
			error = iHwDeviceObserver->WriteFile().Size(recordedFileSize);
			if (error != KErrNone)
				{
				INFO_PRINTF2(_L("problem with recorded file error %d"), error);
				verdict = EFail;
				}
			if (recordedFileSize != KTestBufferSize*5)
				{
				INFO_PRINTF1(_L("recorded file was not the expected size"));
				verdict = EFail;
				}
			}
		else
			{
			INFO_PRINTF2(_L("error on hw device start %d" ), error );
			verdict = EFail;
			}
		} 
	else
		{
		INFO_PRINTF2(_L("problem opening file to record to error %d" ), error );
		verdict = EInconclusive;
		}

	return verdict ;
	} 


CTestStep_MMF_SWCDWRAP_U_0102::CTestStep_MMF_SWCDWRAP_U_0102()
/** Constructor
 */
	{
	iTestStepName = _L("MM-MMF-SWCDWRAP-U-0102-HP");
	}
 

TVerdict CTestStep_MMF_SWCDWRAP_U_0102::DoTestStepL( void )
/**
 * Use case: N/A
 * Use Start() to record and then pause after 4S of recording
 * Note record Pause() does not really 'pause' as such but completes
 * recording
 * @test Req. under test REQ1931.2
 */
	{
	 
	TVerdict verdict = EPass;
	TInt recordedFileSize = 0;

	TFileName filename = _L("\\mm\\mmf\\testfiles\\swcdwrap\\TSU_MMF_SWCDWRAP_record.raw");
	TInt error = iHwDeviceObserver->WriteBufferDirectToFileAsync(filename, 100);//4S should be less than 100 buffers
 
	if (error == KErrNone)
		{
		TInt error = iHwDevice->Start(EDevEncode, EDevInFlow);
		if (error == KErrNone)
			{
			INFO_PRINTF1(_L("***** RECORDING *****"));
			CMMF_TSU_SWCDWRAP_MakeAsyncHwDeviceCall* asyncTimer = NULL;
			TRAP(error, asyncTimer = CMMF_TSU_SWCDWRAP_MakeAsyncHwDeviceCall::NewL();)
			if (error)
				{
				return EInconclusive;
				}
			asyncTimer->StopActiveSchedulerOnCall(EFalse);
			asyncTimer->CallPauseAfter(*iHwDevice, TTimeIntervalMicroSeconds32(4000000));//4S
			iActiveScheduler->Start();
			error = iHwDeviceObserver->ErrorStatus();
			if (error != KErrNone)
				{
				INFO_PRINTF2(_L("error during audio record %d" ), error );
				verdict = EFail;
				}
			error = asyncTimer->PauseError();
			if (error != KErrNone)
				{
				INFO_PRINTF2(_L("error on Pause %d" ), error );
				verdict = EFail;
				}

			//calculate expected number of buffers
			//4S * samplerate * 2 bytes per sample
			TUint bytesPlayed = 4*8000*2;
			TUint expectedNumberOfCompleteBuffersPlayed = (bytesPlayed/KTestBufferSize)+1;//+1 for flushed buf

			//allow a margin of error of 1 buffer either side of the expected number
			if ((iHwDeviceObserver->NumberOfBuffersObserved() < expectedNumberOfCompleteBuffersPlayed-1)||
				(iHwDeviceObserver->NumberOfBuffersObserved() > expectedNumberOfCompleteBuffersPlayed+1))
				{
				INFO_PRINTF1(_L("incorrect number of buffers"));
				verdict = EFail;
				}
			error = iHwDeviceObserver->WriteFile().Size(recordedFileSize);
			if (error != KErrNone)
				{
				INFO_PRINTF2(_L("problem with recorded file error %d"), error);
				verdict = EFail;
				}

			//allow a margin of error of +/- 1 buffer
			if ((static_cast<TUint>(recordedFileSize) < KTestBufferSize*(expectedNumberOfCompleteBuffersPlayed-1))||
				(static_cast<TUint>(recordedFileSize) > KTestBufferSize*(expectedNumberOfCompleteBuffersPlayed+1)))
				{
				INFO_PRINTF1(_L("recorded file was not the expected size"));
				INFO_PRINTF2(_L("Expected %d" ), KTestBufferSize*expectedNumberOfCompleteBuffersPlayed );
				INFO_PRINTF2(_L("Actual %d" ), recordedFileSize);
				verdict = EFail;
				}
			delete asyncTimer;
			}
		else
			{
			INFO_PRINTF2(_L("error on hw device start %d" ), error );
			verdict = EFail;
			}
		} 
	else
		{
		INFO_PRINTF2(_L("problem opening file to record to error %d" ), error );
		verdict = EInconclusive;
		}

	return verdict ;
	} 


CTestStep_MMF_SWCDWRAP_U_0103::CTestStep_MMF_SWCDWRAP_U_0103()
/** Constructor
 */
	{
	iTestStepName = _L("MM-MMF-SWCDWRAP-U-0103-HP");
	}
 

TVerdict CTestStep_MMF_SWCDWRAP_U_0103::DoTestStepL( void )
/**
 * Use case: N/A
 * Use Start() to record and then Stop after 4S of recording
 * @test Req. under test REQ1931.2
 */
	{
	 
	TVerdict verdict = EPass;
	TInt recordedFileSize = 0;

	TFileName filename = _L("\\mm\\mmf\\testfiles\\swcdwrap\\TSU_MMF_SWCDWRAP_record.raw");
	TInt error = iHwDeviceObserver->WriteBufferDirectToFileAsync(filename, 100);//4S should be less than 100 buffers
 
	if (error == KErrNone)
		{
		TInt error = iHwDevice->Start(EDevEncode, EDevInFlow);
		if (error == KErrNone)
			{
			INFO_PRINTF1(_L("***** RECORDING *****"));
			CMMF_TSU_SWCDWRAP_MakeAsyncHwDeviceCall* asyncTimer = NULL;
			TRAP(error, asyncTimer = CMMF_TSU_SWCDWRAP_MakeAsyncHwDeviceCall::NewL();)
			if (error)
				{
				return EInconclusive;
				}
			asyncTimer->CallStopAfter(*iHwDevice, TTimeIntervalMicroSeconds32(4000000));//4S
			iActiveScheduler->Start();
			error = iHwDeviceObserver->ErrorStatus();
			if (error != KErrNone)
				{
				INFO_PRINTF2(_L("error during audio record %d" ), error );
				verdict = EFail;
				}
			error = asyncTimer->StopError();
			if (error != KErrNone)
				{
				INFO_PRINTF2(_L("error on Pause %d" ), error );
				verdict = EFail;
				}

			//calculate expected number of buffers
			//4S * samplerate * 2 bytes per sample
			TUint bytesPlayed = 4*8000*2;
			TUint expectedNumberOfCompleteBuffersPlayed = bytesPlayed/KTestBufferSize;

			//allow a margin of error of 2 buffer either side of the expected number
			//note we need a margin of 2 because Stop does not flush
			//the currently recorded buffer.
			if ((iHwDeviceObserver->NumberOfBuffersObserved() < expectedNumberOfCompleteBuffersPlayed-2)||
				(iHwDeviceObserver->NumberOfBuffersObserved() > expectedNumberOfCompleteBuffersPlayed+2))
				{
				INFO_PRINTF2(_L("incorrect number of buffers %d"), iHwDeviceObserver->NumberOfBuffersObserved());
				INFO_PRINTF2(_L("expected %d" ), expectedNumberOfCompleteBuffersPlayed );
				verdict = EFail;
				}
			error = iHwDeviceObserver->WriteFile().Size(recordedFileSize);
			if (error != KErrNone)
				{
				INFO_PRINTF2(_L("problem with recorded file error %d"), error);
				verdict = EFail;
				}

			//allow a margin of error of 1 buffer
			if ((static_cast<TUint>(recordedFileSize) < KTestBufferSize*(expectedNumberOfCompleteBuffersPlayed-2))||
				(static_cast<TUint>(recordedFileSize) > KTestBufferSize*(expectedNumberOfCompleteBuffersPlayed+2)))
				{
				INFO_PRINTF1(_L("recorded file was not the expected size"));
				verdict = EFail;
				}
			delete asyncTimer;
			}
		else
			{
			INFO_PRINTF2(_L("error on hw device start %d" ), error );
			verdict = EFail;
			}
		} 
	else
		{
		INFO_PRINTF2(_L("problem opening file to record to error %d" ), error );
		verdict = EInconclusive;
		}

	return verdict ;
	} 


CTestStep_MMF_SWCDWRAP_U_0104::CTestStep_MMF_SWCDWRAP_U_0104()
/** Constructor
 */
	{
	iTestStepName = _L("MM-MMF-SWCDWRAP-U-0104-HP");
	}
 

TVerdict CTestStep_MMF_SWCDWRAP_U_0104::DoTestStepL( void )
/**
 * Use case: N/A
 * Use Start() to record and then Stop after 2S of recording
 * Then resume recording again and Stop a further 2 times
 * @test Req. under test REQ1931.2
 */
	{
	 
	TVerdict verdict = EPass;
	TInt recordedFileSize = 0;

	TFileName filename = _L("\\mm\\mmf\\testfiles\\swcdwrap\\TSU_MMF_SWCDWRAP_record.raw");
	TInt error = iHwDeviceObserver->WriteBufferDirectToFileAsync(filename, 100);//4S should be less than 100 buffers

	// Stop the recording after 4.5 buffers have played
	// We stop half way through a buffer to try to ensure we don't stop 
	// on a buffer boundary which can make buffer counting unpredictable
	const TInt KIterations = 3;
	const TInt KExpectedBuffers = 5;
	const TInt KOneMillion = 1000000;
	const TInt KSampleRate = 8000;
	const TInt KBytesPerSample = 2;
	const TInt KTimeToRecordMicroSeconds = 
		I64LOW(
			TInt64(KTestBufferSize) * TInt64(KOneMillion) * 
			TInt64(9) / TInt64(2) /			// 4.5
			TInt64(KBytesPerSample) / TInt64(KSampleRate)
		);
 
	if (error == KErrNone)
		{
		TUint expectedNumberOfCompleteBuffersPlayed = 0;
		CMMF_TSU_SWCDWRAP_MakeAsyncHwDeviceCall* asyncTimer = NULL;
		TRAP(error, asyncTimer = CMMF_TSU_SWCDWRAP_MakeAsyncHwDeviceCall::NewL();)
		if (error)
			{
			return EInconclusive;
			}
		for (TInt i=0; i<KIterations; i++)
			{//do 3 iterations
			TInt error = iHwDevice->Start(EDevEncode, EDevInFlow);
			if (error == KErrNone)
				{
				INFO_PRINTF1(_L("***** RECORDING *****"));
				asyncTimer->StopActiveSchedulerOnCall(EFalse);
				asyncTimer->Cancel(); //just in case
				asyncTimer->CallPauseAfter(*iHwDevice, TTimeIntervalMicroSeconds32(KTimeToRecordMicroSeconds));//2.5 secs

				iActiveScheduler->Start();
				error = iHwDevice->Stop();
				if (error != KErrNone)
					{
					INFO_PRINTF2(_L("error stopping audio record %d" ), error );
					verdict = EFail;
					}
				error = iHwDeviceObserver->ErrorStatus();
				if (error != KErrNone)
					{
					INFO_PRINTF2(_L("error during audio record %d" ), error );
					verdict = EFail;
					}
				error = asyncTimer->PauseError();
				if (error != KErrNone)
					{
					INFO_PRINTF2(_L("error on Pause %d" ), error );
					verdict = EFail;
					}				
				}//if (error == KErrNone)
			else
				{
				INFO_PRINTF2(_L("error on hw device start %d" ), error );
				verdict = EFail;
				}
			}//for (i=0, i++, i<=3)
		//calculate expected number of buffers
		expectedNumberOfCompleteBuffersPlayed = KExpectedBuffers * KIterations;

		//allow a margin of error of +/- 2 buffer either side of the expected number
		if ((iHwDeviceObserver->NumberOfBuffersObserved() < expectedNumberOfCompleteBuffersPlayed-2)||
			(iHwDeviceObserver->NumberOfBuffersObserved() > expectedNumberOfCompleteBuffersPlayed+2))
			{
			INFO_PRINTF2(_L("unexpected number of buffers %d"), iHwDeviceObserver->NumberOfBuffersObserved());
			INFO_PRINTF2(_L("expected %d" ), expectedNumberOfCompleteBuffersPlayed );
			}
		error = iHwDeviceObserver->WriteFile().Size(recordedFileSize);
		if (error != KErrNone)
			{
			INFO_PRINTF2(_L("problem with recorded file error %d"), error);
			verdict = EFail;
			}

		//allow a margin of error of 1 buffer
		if ((static_cast<TUint>(recordedFileSize) < KTestBufferSize*(expectedNumberOfCompleteBuffersPlayed-2))||
			(static_cast<TUint>(recordedFileSize) > KTestBufferSize*(expectedNumberOfCompleteBuffersPlayed+2)))
			{
			INFO_PRINTF2(_L("recorded file was not the expected size %d"), recordedFileSize);
			INFO_PRINTF2(_L("expected %d"), KTest2To1SinkBufferSize*expectedNumberOfCompleteBuffersPlayed );
			verdict = EFail;
			}
		delete asyncTimer;
		}//if (error == KErrNone)
	else
		{
		INFO_PRINTF2(_L("problem opening file to record to error %d" ), error );
		verdict = EInconclusive;
		}

	return verdict ;
	} 


CTestStep_MMF_SWCDWRAP_U_0105::CTestStep_MMF_SWCDWRAP_U_0105()
/** Constructor
 */
	{
	iTestStepName = _L("MM-MMF-SWCDWRAP-U-0105-HP");
	}
 

TVerdict CTestStep_MMF_SWCDWRAP_U_0105::DoTestStepL( void )
/**
 * Use case: N/A
 * Use Start() to record five buffers
 * Using the record custom interface to set sample rate 16000 and stereo record
 * and change gain
 */
	{
	 
	TVerdict verdict = EPass;
	TInt recordedFileSize = 0;

	iRecordCustomInterface->SetGain(200);
	
	TTaskConfig taskConfig;
	taskConfig.iUid = KUidRefDevSoundTaskConfig;
	taskConfig.iRate = 16000;
	taskConfig.iStereoMode = ETaskInterleaved;
	
	if (iHwDevice->SetConfig(taskConfig) != KErrNone)
		{
		return EFail;
		}

	TFileName filename = _L("\\mm\\mmf\\testfiles\\swcdwrap\\TSU_MMF_SWCDWRAP_record.raw");
	TInt error = iHwDeviceObserver->WriteBufferDirectToFileAsync(filename, 5);
 
	if (error == KErrNone)
		{
		TInt error = iHwDevice->Start(EDevEncode, EDevInFlow);
		if (error == KErrNone)
			{
			INFO_PRINTF1(_L("***** RECORDING *****"));
			iActiveScheduler->Start();
			error = iHwDeviceObserver->ErrorStatus();
			if (error != KErrNone)
				{
				INFO_PRINTF2(_L("error during audio record %d" ), error );
				verdict = EFail;
				}
			if (iHwDeviceObserver->NumberOfBuffersObserved() != 5)
				{
				INFO_PRINTF1(_L("incorrect number of buffers"));
				verdict = EFail;
				}
			error = iHwDeviceObserver->WriteFile().Size(recordedFileSize);
			if (error != KErrNone)
				{
				INFO_PRINTF2(_L("problem with recorded file error %d"), error);
				verdict = EFail;
				}
			if (recordedFileSize != KTestBufferSize*5)
				{
				INFO_PRINTF1(_L("recorded file was not the expected size"));
				verdict = EFail;
				}
			}
		else
			{
			INFO_PRINTF2(_L("error on hw device start %d" ), error );
			verdict = EFail;
			}
		} 
	else
		{
		INFO_PRINTF2(_L("problem opening file to record to error %d" ), error );
		verdict = EInconclusive;
		}

	return verdict ;
	} 



CTestStep_MMF_SWCDWRAP_U_0106::CTestStep_MMF_SWCDWRAP_U_0106()
/** Constructor
 */
	{
	iTestStepName = _L("MM-MMF-SWCDWRAP-U-0106-HP");
	}


TVerdict CTestStep_MMF_SWCDWRAP_U_0106::DoTestStepL()
/**
 * DEF075721 : Sound recoding's pause operation causes panic after its stop
 * Issuing a Stop/Pause sequence causes panic
 */
	{
	const TInt KIterations = 3;
	TVerdict verdict = EPass;
	
	TFileName filename = _L("\\mm\\mmf\\testfiles\\swcdwrap\\TSU_MMF_SWCDWRAP_record.raw");
	TInt error = iHwDeviceObserver->WriteBufferDirectToFileAsync(filename, 100);//4S should be less than 100 buffers
	if (error != KErrNone)
		{
		INFO_PRINTF2(_L("problem opening file to record to error %d" ), error );
		return EInconclusive;
		}
		
	const TInt KOneMillion = 1000000;
	const TInt KSampleRate = 8000;
	const TInt KBytesPerSample = 2;
	const TInt KTimeToRecordMicroSeconds = 
		I64LOW(
			TInt64(KTestBufferSize) * TInt64(KOneMillion) * 
			TInt64(9) / TInt64(2) /			// 4.5
			TInt64(KBytesPerSample) / TInt64(KSampleRate)
		);
 
	CMMF_TSU_SWCDWRAP_MakeAsyncHwDeviceCall* asyncTimer = NULL;
		
	TRAP(error, asyncTimer = CMMF_TSU_SWCDWRAP_MakeAsyncHwDeviceCall::NewL());
	if (error != KErrNone)
		{
		return EInconclusive;
		}
		
	CleanupStack::PushL(asyncTimer);
			
	for (TInt i = 0; i < KIterations; i++)
		{//do 3 iterations
		TInt error = iHwDevice->Start(EDevEncode, EDevInFlow);
		if (error != KErrNone)
			{
			INFO_PRINTF2(_L("error on hw device start %d" ), error );
			CleanupStack::PopAndDestroy(asyncTimer);
			return EFail;
			}
			
		INFO_PRINTF1(_L("***** RECORDING *****"));
		asyncTimer->StopActiveSchedulerOnCall(ETrue);
		asyncTimer->Cancel(); //just in case
		asyncTimer->CallStopPauseAfter(*iHwDevice, TTimeIntervalMicroSeconds32(KTimeToRecordMicroSeconds));//2.5 secs

		iActiveScheduler->Start();
		error = iHwDeviceObserver->ErrorStatus();
		if (error != KErrNone)
			{
			INFO_PRINTF2(_L("error during audio record %d" ), error );
			CleanupStack::PopAndDestroy(asyncTimer);
			return EFail;
			}
		error = asyncTimer->PauseError();
		if (error != KErrNone)
			{
			INFO_PRINTF2(_L("error on Pause %d" ), error );
			CleanupStack::PopAndDestroy(asyncTimer);
			return EFail;
			}
		error = asyncTimer->StopError();
		if (error != KErrNone)
			{
			INFO_PRINTF2(_L("error on Stop %d"), error);
			CleanupStack::PopAndDestroy(asyncTimer);
			return EFail;
			}
		}
	CleanupStack::PopAndDestroy(asyncTimer);
	asyncTimer = NULL;

	return verdict;
	}

CTestStep_MMF_SWCDWRAP_U_0140::CTestStep_MMF_SWCDWRAP_U_0140()
/** Constructor
 */
	{
	iTestStepName = _L("MM-MMF-SWCDWRAP-U-0140-HP");
	}
 

TVerdict CTestStep_MMF_SWCDWRAP_U_0140::DoTestStepL( void )
/**
 * Use case: N/A
 * Use Start() to record one buffer using 2:1 codec
 * @test Req. under test REQ1931.2
 */
	{
	 
	TVerdict verdict = EPass;
	TInt recordedFileSize = 0;

	TFileName filename = _L("\\mm\\mmf\\testfiles\\swcdwrap\\TSU_MMF_SWCDWRAP_record2To1.raw");
	TInt error = iHwDeviceObserver->WriteBufferDirectToFileAsync(filename, 1);
 
	if (error == KErrNone)
		{
		TInt error = iHwDevice->Start(EDevEncode, EDevInFlow);
		if (error == KErrNone)
			{
			INFO_PRINTF1(_L("***** RECORDING *****"));
			iActiveScheduler->Start();
			error = iHwDeviceObserver->ErrorStatus();
			if (error != KErrNone)
				{
				INFO_PRINTF2(_L("error during audio record %d" ), error );
				verdict = EFail;
				}
			if (iHwDeviceObserver->NumberOfBuffersObserved() != 1)
				{
				INFO_PRINTF1(_L("incorrect number of buffers"));
				verdict = EFail;
				}
			error = iHwDeviceObserver->WriteFile().Size(recordedFileSize);
			if (error != KErrNone)
				{
				INFO_PRINTF2(_L("problem with recorded file error %d"), error);
				verdict = EFail;
				}
			if (recordedFileSize != KTest2To1SinkBufferSize)
				{
				INFO_PRINTF1(_L("recorded file was not the expected size"));
				verdict = EFail;
				}
			}
		else
			{
			INFO_PRINTF2(_L("error on hw device start %d" ), error );
			verdict = EFail;
			}
		} 
	else
		{
		INFO_PRINTF2(_L("problem opening file to record to error %d" ), error );
		verdict = EInconclusive;
		}

	return verdict ;
	} 


CTestStep_MMF_SWCDWRAP_U_0141::CTestStep_MMF_SWCDWRAP_U_0141()
/** Constructor
 */
	{
	iTestStepName = _L("MM-MMF-SWCDWRAP-U-0141-HP");
	}
 

TVerdict CTestStep_MMF_SWCDWRAP_U_0141::DoTestStepL( void )
/**
 * Use case: N/A
 * Use Start() to record five buffers using 2:1 codec
 * @test Req. under test REQ1931.2
 */
	{
	 
	TVerdict verdict = EPass;
	TInt recordedFileSize = 0;

	TFileName filename = _L("\\mm\\mmf\\testfiles\\swcdwrap\\TSU_MMF_SWCDWRAP_record2To1.raw");
	TInt error = iHwDeviceObserver->WriteBufferDirectToFileAsync(filename, 5);
 
	if (error == KErrNone)
		{
		TInt error = iHwDevice->Start(EDevEncode, EDevInFlow);
		if (error == KErrNone)
			{
			INFO_PRINTF1(_L("***** RECORDING *****"));
			iActiveScheduler->Start();
			error = iHwDeviceObserver->ErrorStatus();
			if (error != KErrNone)
				{
				INFO_PRINTF2(_L("error during audio record %d" ), error );
				verdict = EFail;
				}
			if (iHwDeviceObserver->NumberOfBuffersObserved() != 5)
				{
				INFO_PRINTF1(_L("incorrect number of buffers"));
				verdict = EFail;
				}
			error = iHwDeviceObserver->WriteFile().Size(recordedFileSize);
			if (error != KErrNone)
				{
				INFO_PRINTF2(_L("problem with recorded file error %d"), error);
				verdict = EFail;
				}
			if (recordedFileSize != KTest2To1SinkBufferSize*5)
				{
				INFO_PRINTF1(_L("recorded file was not the expected size"));
				verdict = EFail;
				}
			}
		else
			{
			INFO_PRINTF2(_L("error on hw device start %d" ), error );
			verdict = EFail;
			}
		} 
	else
		{
		INFO_PRINTF2(_L("problem opening file to record to error %d" ), error );
		verdict = EInconclusive;
		}

	return verdict ;
	} 


CTestStep_MMF_SWCDWRAP_U_0142::CTestStep_MMF_SWCDWRAP_U_0142()
/** Constructor
 */
	{
	iTestStepName = _L("MM-MMF-SWCDWRAP-U-0142-HP");
	}
 

TVerdict CTestStep_MMF_SWCDWRAP_U_0142::DoTestStepL( void )
/**
 * Use case: N/A
 * Use Start() to record using 2:1 codec,pause after 4S of recording
 * Note record Pause() does not really 'pause' as such but completes
 * recording
 * @test Req. under test REQ1931.2
 */
	{
	 
	TVerdict verdict = EPass;
	TInt recordedFileSize = 0;

	TFileName filename = _L("\\mm\\mmf\\testfiles\\swcdwrap\\TSU_MMF_SWCDWRAP_record2To1.raw");
	TInt error = iHwDeviceObserver->WriteBufferDirectToFileAsync(filename, 100);//4S should be less than 100 buffers
 
	if (error == KErrNone)
		{
		TInt error = iHwDevice->Start(EDevEncode, EDevInFlow);
		if (error == KErrNone)
			{
			INFO_PRINTF1(_L("***** RECORDING *****"));
			CMMF_TSU_SWCDWRAP_MakeAsyncHwDeviceCall* asyncTimer = NULL;
			TRAP(error, asyncTimer = CMMF_TSU_SWCDWRAP_MakeAsyncHwDeviceCall::NewL();)
			if (error)
				{
				return EInconclusive;
				}
			asyncTimer->StopActiveSchedulerOnCall(EFalse);
			asyncTimer->CallPauseAfter(*iHwDevice, TTimeIntervalMicroSeconds32(4000000));//4S
			iActiveScheduler->Start();
			error = iHwDeviceObserver->ErrorStatus();
			if (error != KErrNone)
				{
				INFO_PRINTF2(_L("error during audio record %d" ), error );
				verdict = EFail;
				}
			error = asyncTimer->PauseError();
			if (error != KErrNone)
				{
				INFO_PRINTF2(_L("error on Pause %d" ), error );
				verdict = EFail;
				}

			//calculate expected number of buffers
			//4S * samplerate * 2 bytes per sample
			TUint bytesPlayed = 4*8000*2;
			TUint expectedNumberOfCompleteBuffersPlayed = bytesPlayed/KTest2To1SourceBufferSize+1; // +1 for the last buffer

			//allow a margin of error of +/- 1 buffer either side of the expected number
			if ((iHwDeviceObserver->NumberOfBuffersObserved() < expectedNumberOfCompleteBuffersPlayed-1)||
				(iHwDeviceObserver->NumberOfBuffersObserved() > expectedNumberOfCompleteBuffersPlayed+1))
				{
				INFO_PRINTF3(_L("number of buffers not that expected (found %d, expected %d)"),
				        iHwDeviceObserver->NumberOfBuffersObserved(), expectedNumberOfCompleteBuffersPlayed);
				}
			
			error = iHwDeviceObserver->WriteFile().Size(recordedFileSize);
			if (error != KErrNone)
				{
				INFO_PRINTF2(_L("problem with recorded file error %d"), error);
				verdict = EFail;
				}

			//allow a margin of error of +/- 1 buffer
			TInt expectedRecordSize = 4*4000*2; // 4s of 8K samples per sec, but then we divide by two
			if ((static_cast<TUint>(recordedFileSize) < expectedRecordSize-KTest2To1SinkBufferSize)||
				(static_cast<TUint>(recordedFileSize) > expectedRecordSize+KTest2To1SinkBufferSize))
				{
				INFO_PRINTF1(_L("recorded file was not the expected size"));
				INFO_PRINTF2(_L("Expected %d" ), expectedRecordSize );
				INFO_PRINTF2(_L("Actual %d" ), recordedFileSize);
				verdict = EFail;
				}
			delete asyncTimer;
			}
		else
			{
			INFO_PRINTF2(_L("error on hw device start %d" ), error );
			verdict = EFail;
			}
		} 
	else
		{
		INFO_PRINTF2(_L("problem opening file to record to error %d" ), error );
		verdict = EInconclusive;
		}

	return verdict ;
	} 


CTestStep_MMF_SWCDWRAP_U_0143::CTestStep_MMF_SWCDWRAP_U_0143()
/** Constructor
 */
	{
	iTestStepName = _L("MM-MMF-SWCDWRAP-U-0143-HP");
	}
 

TVerdict CTestStep_MMF_SWCDWRAP_U_0143::DoTestStepL( void )
/**
 * Use case: N/A
 * Use Start() to record using 2:1 codec, Stop after 4S of recording
 * @test Req. under test REQ1931.2
 */
	{
	 
	TVerdict verdict = EPass;
	TInt recordedFileSize = 0;

	TFileName filename = _L("\\mm\\mmf\\testfiles\\swcdwrap\\TSU_MMF_SWCDWRAP_record2To1.raw");
	TInt error = iHwDeviceObserver->WriteBufferDirectToFileAsync(filename, 100);//4S should be less than 100 buffers
 
	if (error == KErrNone)
		{
		TInt error = iHwDevice->Start(EDevEncode, EDevInFlow);
		if (error == KErrNone)
			{
			INFO_PRINTF1(_L("***** RECORDING *****"));
			CMMF_TSU_SWCDWRAP_MakeAsyncHwDeviceCall* asyncTimer = NULL;
			TRAP(error, asyncTimer = CMMF_TSU_SWCDWRAP_MakeAsyncHwDeviceCall::NewL();)
			if (error)
				{
				return EInconclusive;
				}
			asyncTimer->CallStopAfter(*iHwDevice, TTimeIntervalMicroSeconds32(4000000));//4S
			iActiveScheduler->Start();
			error = iHwDeviceObserver->ErrorStatus();
			if (error != KErrNone)
				{
				INFO_PRINTF2(_L("error during audio record %d" ), error );
				verdict = EFail;
				}
			error = asyncTimer->StopError();
			if (error != KErrNone)
				{
				INFO_PRINTF2(_L("error on Pause %d" ), error );
				verdict = EFail;
				}

			//calculate expected number of buffers
			//4S * samplerate * 2 bytes per sample
			TUint bytesPlayed = 4*8000*2;
			TUint expectedNumberOfCompleteBuffersPlayed = bytesPlayed/KTest2To1SourceBufferSize;

			//allow a margin of error of 2 buffers either side of the expected number
			if ((iHwDeviceObserver->NumberOfBuffersObserved() < expectedNumberOfCompleteBuffersPlayed-2)||
				(iHwDeviceObserver->NumberOfBuffersObserved() > expectedNumberOfCompleteBuffersPlayed+2))
				{
				INFO_PRINTF1(_L("incorrect number of buffers"));
				verdict = EFail;
				}
			error = iHwDeviceObserver->WriteFile().Size(recordedFileSize);
			if (error != KErrNone)
				{
				INFO_PRINTF2(_L("problem with recorded file error %d"), error);
				verdict = EFail;
				}

			//allow a margin of error of 1 buffer
			if ((static_cast<TUint>(recordedFileSize) < KTest2To1SinkBufferSize*(expectedNumberOfCompleteBuffersPlayed-2))||
				(static_cast<TUint>(recordedFileSize) > KTest2To1SinkBufferSize*(expectedNumberOfCompleteBuffersPlayed+2)))
				{
				INFO_PRINTF1(_L("recorded file was not the expected size"));
				verdict = EFail;
				}
			delete asyncTimer;
			}
		else
			{
			INFO_PRINTF2(_L("error on hw device start %d" ), error );
			verdict = EFail;
			}
		} 
	else
		{
		INFO_PRINTF2(_L("problem opening file to record to error %d" ), error );
		verdict = EInconclusive;
		}

	return verdict ;
	} 


CTestStep_MMF_SWCDWRAP_U_0144::CTestStep_MMF_SWCDWRAP_U_0144()
/** Constructor
 */
	{
	iTestStepName = _L("MM-MMF-SWCDWRAP-U-0144-HP");
	}
 

TVerdict CTestStep_MMF_SWCDWRAP_U_0144::DoTestStepL( void )
/**
 * Use case: N/A
 * Use Start() to record using 2:1 codec, Stop after 2S of recording
 * Then resume recording again and Stop a further 2 times
 * @test Req. under test REQ1931.2
 */
	{
	 
	TVerdict verdict = EPass;
	TInt recordedFileSize = 0;

	TFileName filename = _L("\\mm\\mmf\\testfiles\\swcdwrap\\TSU_MMF_SWCDWRAP_record2To1.raw");
	TInt error = iHwDeviceObserver->WriteBufferDirectToFileAsync(filename, 100);//4S should be less than 100 buffers
 
	if (error == KErrNone)
		{
		TUint bytesPlayed = 0;
		TUint expectedNumberOfCompleteBuffersPlayed = 0;
		CMMF_TSU_SWCDWRAP_MakeAsyncHwDeviceCall* asyncTimer = NULL;
		TRAP(error,asyncTimer = CMMF_TSU_SWCDWRAP_MakeAsyncHwDeviceCall::NewL();)
		if (error)
			{
			return EInconclusive;
			}
		for (TUint i=0; i<3; i++)
			{//do 3 iterations
			TInt error = iHwDevice->Start(EDevEncode, EDevInFlow);
			if (error == KErrNone)
				{
				INFO_PRINTF1(_L("***** RECORDING *****"));
				asyncTimer->StopActiveSchedulerOnCall(EFalse);
				asyncTimer->Cancel(); //just in case
				asyncTimer->CallPauseAfter(*iHwDevice, TTimeIntervalMicroSeconds32(2000000));//2S
				iActiveScheduler->Start();
				error = iHwDevice->Stop();
				if (error != KErrNone)
					{
					INFO_PRINTF2(_L("error stopping audio record %d" ), error );
					verdict = EFail;
					}
				error = iHwDeviceObserver->ErrorStatus();
				if (error != KErrNone)
					{
					INFO_PRINTF2(_L("error during audio record %d" ), error );
					verdict = EFail;
					}
				error = asyncTimer->PauseError();
				if (error != KErrNone)
					{
					INFO_PRINTF2(_L("error on Pause %d" ), error );
					verdict = EFail;
					}			
				}//if (error == KErrNone)
			else
				{
				INFO_PRINTF2(_L("error on hw device start %d" ), error );
				verdict = EFail;
				}
			}//for (i=0, i++, i<=3)
		//calculate expected number of buffers
		//because of this we give a margin of +/- 2 less than the expected number
		bytesPlayed = 3*2*8000;//only  half bytes played with 2:1 codec
		expectedNumberOfCompleteBuffersPlayed = bytesPlayed/KTest2To1SourceBufferSize + 6; // add 6 for partial and last buffers
		//allow a margin of error of +/- 2 buffer either side of the expected number
		if ((iHwDeviceObserver->NumberOfBuffersObserved() < expectedNumberOfCompleteBuffersPlayed-2)||
			(iHwDeviceObserver->NumberOfBuffersObserved() > expectedNumberOfCompleteBuffersPlayed+2))
			{
			INFO_PRINTF2(_L("unexpected number of buffers %d"), iHwDeviceObserver->NumberOfBuffersObserved());
			INFO_PRINTF2(_L("expected %d" ), expectedNumberOfCompleteBuffersPlayed );
			}
		error = iHwDeviceObserver->WriteFile().Size(recordedFileSize);
		if (error != KErrNone)
			{
			INFO_PRINTF2(_L("problem with recorded file error %d"), error);
			verdict = EFail;
			}

		//allow a margin of error of 2 buffers
		TInt numberBytesExpected = 3*2*8000*2/2; // 3 times 2s of 8000 samples/sec PCM16 /2 by codec
		if ((static_cast<TUint>(recordedFileSize) < numberBytesExpected-KTest2To1SinkBufferSize)||
			(static_cast<TUint>(recordedFileSize) > numberBytesExpected+KTest2To1SinkBufferSize))
			{
			INFO_PRINTF2(_L("recorded file was not the expected size %d"), recordedFileSize);
			INFO_PRINTF2(_L("expected %d"), numberBytesExpected );
			verdict = EFail;
			}
		delete asyncTimer;
		}//if (error == KErrNone)
	else
		{
		INFO_PRINTF2(_L("problem opening file to record to error %d" ), error );
		verdict = EInconclusive;
		}

	return verdict ;
	} 

