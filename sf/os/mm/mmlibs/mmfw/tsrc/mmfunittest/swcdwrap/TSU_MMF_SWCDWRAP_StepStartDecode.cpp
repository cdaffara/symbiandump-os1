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
#include "TSU_MMF_SWCDWRAP_StepStartDecode.h"
#include "TSU_MMF_SWCDWRAP_TestDeviceImplementationUID.hrh"
#include "TSU_MMF_SWCDWRAP_PlayHwDevObserver.h"
#include "TSU_MMF_SWCDWRAP_AsyncTimer.h"
#include "TSU_MMF_SWCDWRAP_TestDevice.h"

#ifdef SYMBIAN_MDF_SHAREDCHUNK_SOUNDDRIVER
	#include "mdasoundadapter.h"
#else
	#include <mdasound.h>
#endif

/**
 * sets up test step hw device and observer
 */
TVerdict CTestStep_MMF_SWCDWRAP_StartTestDecode::InitializeTestStepUsingHwDeviceL(TUid aHwDeviceUid)
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
			verdict = EInconclusive;//cannot run test proper if no HwDevice plugin available
			//therefore if no hw dev plugin not possible to test the API under test
			}
		}
	
	if (verdict == EPass)
		{
		iActiveScheduler = CActiveScheduler::Current();
		if (!iActiveScheduler) iActiveScheduler = new(ELeave)CActiveScheduler;
		CActiveScheduler::Install(iActiveScheduler);

		iHwDeviceObserver = new(ELeave)CTestPlayMMFHwDeviceObserver(iHwDevice);

		THwDeviceInitParams	devInfo;
		devInfo.iHwDeviceObserver = iHwDeviceObserver;
		TInt error = iHwDevice->Init(devInfo);
		
		if (error == KErrNone)
			{
			TUid playCustomInterfaceUid;
			playCustomInterfaceUid.iUid = KMmfPlaySettingsCustomInterface;
			iPlayCustomInterface = 
			(MPlayCustomInterface*)iHwDevice->CustomInterface(playCustomInterfaceUid);
			iPlayCustomInterface->SetVolume(50000);
			
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
TVerdict CTestStep_MMF_SWCDWRAP_StartTestDecode::DoTestStepPreambleL()
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
TVerdict CTestStep_MMF_SWCDWRAP_StartTest2To1Decode::DoTestStepPreambleL()
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
TVerdict CTestStep_MMF_SWCDWRAP_StartTestDecode::DoTestStepPostambleL()
	{
	delete iHwDevice;
	delete iHwDeviceObserver;
	delete iActiveScheduler;

	return CTestStep_MMF_SWCDWRAP::DoTestStepPostambleL();
	}
 


CTestStep_MMF_SWCDWRAP_U_0010::CTestStep_MMF_SWCDWRAP_U_0010()
/** Constructor
 */
	{
	iTestStepName = _L("MM-MMF-SWCDWRAP-U-0010-HP");
	}
 

TVerdict CTestStep_MMF_SWCDWRAP_U_0010::DoTestStepL( void )
/**
 * Use case: N/A
 * Use Start() to play one not completed buffer synchronously
 * buffer size < codec buffer size
 * @test Req. under test REQ1931.2
 */
	{
	 
	TVerdict verdict = EPass;

	TFileName filename = _L("\\mm\\mmf\\testfiles\\swcdwrap\\TSU_MMF_SWCDWRAPpcm16_8m.raw");
	TInt error = iHwDeviceObserver->FillBufferFromFileSync(filename, KTestBufferSize/2);
 
	if (error == KErrNone)
		{
		TInt error = iHwDevice->Start(EDevDecode, EDevOutFlow);
		if (error == KErrNone)
			{
			iActiveScheduler->Start();
			error = iHwDeviceObserver->ErrorStatus();
			if ((error != KErrNone)&&(error != KErrUnderflow))
				{
				INFO_PRINTF2(_L("error during audio playback %d" ), error );
				verdict = EFail;
				}
			if (iHwDeviceObserver->NumberOfBuffersObserved() != 1)
				{
				INFO_PRINTF1(_L("incorrect number of buffers"));
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
		INFO_PRINTF2(_L("problem creating source buffer from file error %d" ), error );
		verdict = EInconclusive;
		}
	
	return verdict ;
	} 


CTestStep_MMF_SWCDWRAP_U_0011::CTestStep_MMF_SWCDWRAP_U_0011()
/** Constructor
 */
	{
	iTestStepName = _L("MM-MMF-SWCDWRAP-U-0011-HP");
	}
 

TVerdict CTestStep_MMF_SWCDWRAP_U_0011::DoTestStepL( void )
/**
 * Use case: N/A
 * Use Start() to play exactly one complete buffer synchronously
 * buffer size < codec buffer size
 * @test Req. under test REQ1931.2
 */
	{
	 
	TVerdict verdict = EPass;

	TFileName filename = _L("\\mm\\mmf\\testfiles\\swcdwrap\\TSU_MMF_SWCDWRAPpcm16_8m.raw");
	TInt error = iHwDeviceObserver->FillBufferFromFileSync(filename, KTestBufferSize);//exactly 1 buffer
 
	if (error == KErrNone)
		{
		TInt error = iHwDevice->Start(EDevDecode, EDevOutFlow);
		if (error == KErrNone)
			{
			iActiveScheduler->Start();
			error = iHwDeviceObserver->ErrorStatus();
			if ((error != KErrNone)&&(error != KErrUnderflow))
				{
				INFO_PRINTF2(_L("error during audio playback %d" ), error );
				verdict = EFail;
				}
			if (iHwDeviceObserver->NumberOfBuffersObserved() != 1)
				{
				INFO_PRINTF1(_L("incorrect number of buffers"));
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
		INFO_PRINTF2(_L("problem creating source buffer from file error %d" ), error );
		verdict = EInconclusive;
		}
	
	return verdict ;
	} 


CTestStep_MMF_SWCDWRAP_U_0012::CTestStep_MMF_SWCDWRAP_U_0012()
/** Constructor
 */
	{
	iTestStepName = _L("MM-MMF-SWCDWRAP-U-0012-HP");
	}
 

TVerdict CTestStep_MMF_SWCDWRAP_U_0012::DoTestStepL( void )
/**
 * Use case: N/A
 * Use Start() to play five complete and 1 not completed buffers synchronously
 * @test Req. under test REQ1931.2
 */
	{
	 
	TVerdict verdict = EPass;

	TFileName filename = _L("\\mm\\mmf\\testfiles\\swcdwrap\\TSU_MMF_SWCDWRAPpcm16_8m.raw");
	TInt error = iHwDeviceObserver->FillBufferFromFileSync(filename, 5*KTestBufferSize + KTestBufferSize/2);
 
	if (error == KErrNone)
		{
		TInt error = iHwDevice->Start(EDevDecode, EDevOutFlow);
		if (error == KErrNone)
			{
			iActiveScheduler->Start();
			error = iHwDeviceObserver->ErrorStatus();
			if ((error != KErrNone)&&(error != KErrUnderflow))
				{
				INFO_PRINTF2(_L("error during audio playback %d" ), error );
				verdict = EFail;
				}
			if (iHwDeviceObserver->NumberOfBuffersObserved() != 6)
				{//expected 6 buffers 5 complete + 1 not completed buffer
				INFO_PRINTF1(_L("incorrect number of buffers"));
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
		INFO_PRINTF2(_L("problem creating source buffer from file error %d" ), error );
		verdict = EInconclusive;
		}
	
	return verdict ;
	} 


CTestStep_MMF_SWCDWRAP_U_0013::CTestStep_MMF_SWCDWRAP_U_0013()
/** Constructor
 */
	{
	iTestStepName = _L("MM-MMF-SWCDWRAP-U-0013-HP");
	}
 

TVerdict CTestStep_MMF_SWCDWRAP_U_0013::DoTestStepL( void )
/**
 * Use case: N/A
 * Use Start() to play 1 buffer from a file using async read operation
 * @test Req. under test REQ1931.2
 */
	{
	TVerdict verdict = EPass;

	TFileName filename = _L("\\mm\\mmf\\testfiles\\swcdwrap\\TSU_MMF_SWCDWRAPpcm16_8m.raw");

	TInt error = iHwDeviceObserver->ReadBufferDirectFromFileAsync(filename,1); //1 buffer
 
	if (error == KErrNone)
		{
		error = iHwDevice->Start(EDevDecode, EDevOutFlow);
		if (error == KErrNone)
			{
			iActiveScheduler->Start();
			error = iHwDeviceObserver->ErrorStatus();
			if ((error != KErrNone)&&(error != KErrUnderflow))
				{
				INFO_PRINTF2(_L("error during audio playback %d" ), error );
				verdict = EFail;
				}
			if (iHwDeviceObserver->NumberOfBuffersObserved() != 1)
				{
				INFO_PRINTF1(_L("incorrect number of buffers"));
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
		INFO_PRINTF2(_L("problem opening source file error %d" ), error );
		verdict = EInconclusive;
		}

	return verdict ;
	} 


CTestStep_MMF_SWCDWRAP_U_0014::CTestStep_MMF_SWCDWRAP_U_0014()
/** Constructor
 */
	{
	iTestStepName = _L("MM-MMF-SWCDWRAP-U-0014-HP");
	}
 

TVerdict CTestStep_MMF_SWCDWRAP_U_0014::DoTestStepL( void )
/**
 * Use case: N/A
 * Use Start() to play 5 buffers from a file using async read operation
 * @test Req. under test REQ1931.2
 */
	{
	TVerdict verdict = EPass;

	TFileName filename = _L("\\mm\\mmf\\testfiles\\swcdwrap\\TSU_MMF_SWCDWRAPpcm16_8m.raw");

	TInt error = iHwDeviceObserver->ReadBufferDirectFromFileAsync(filename,5); //5 buffers
 
	if (error == KErrNone)
		{
		error = iHwDevice->Start(EDevDecode, EDevOutFlow);
		if (error == KErrNone)
			{
			iActiveScheduler->Start();
			error = iHwDeviceObserver->ErrorStatus();
			if ((error != KErrNone)&&(error != KErrUnderflow))
				{
				INFO_PRINTF2(_L("error during audio playback %d" ), error );
				verdict = EFail;
				}
			if (iHwDeviceObserver->NumberOfBuffersObserved() != 5)
				{
				INFO_PRINTF1(_L("incorrect number of buffers"));
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
		INFO_PRINTF2(_L("problem opening source file error %d" ), error );
		verdict = EInconclusive;
		}

	return verdict ;
	} 


CTestStep_MMF_SWCDWRAP_U_0015::CTestStep_MMF_SWCDWRAP_U_0015()
/** Constructor
 */
	{
	iTestStepName = _L("MM-MMF-SWCDWRAP-U-0015-HP");
	}
 

TVerdict CTestStep_MMF_SWCDWRAP_U_0015::DoTestStepL( void )
/**
 * Use case: N/A
 * Use Start() to play a file to completion
 * @test Req. under test REQ1931.2
 */
	{
	TVerdict verdict = EPass;

	TFileName filename = _L("\\mm\\mmf\\testfiles\\swcdwrap\\TSU_MMF_SWCDWRAPpcm16_8m.raw");

	TInt error = iHwDeviceObserver->ReadBufferDirectFromFileAsync(filename,0xFFFF); //EOF

	//calculate the expected duration of the file
	//for some reason the calculation needs to broken up into these steps
	//otherwise the compiler gets it wrong !!
	TInt sourceFileSize = 0;
	iHwDeviceObserver->ReadFile().Size(sourceFileSize);	
	TUint expectedNumberOfBuffers = sourceFileSize/KTestBufferSize;
	TInt64 expectedDurationInt64 = sourceFileSize;
	expectedDurationInt64 *= 1000000; //uS
	expectedDurationInt64 /= (8000*2*1);//(samplerate*2BytesPerSample*mono)
	TTimeIntervalMicroSeconds expectedDuration(expectedDurationInt64);
 
	if (error == KErrNone)
		{
		error = iHwDevice->Start(EDevDecode, EDevOutFlow);
		INFO_PRINTF1(_L("***** PLAYING *****"));
		if (error == KErrNone)
			{
			TTime startTime(0);
			TTime endTime(0);
			startTime.HomeTime();
			iActiveScheduler->Start();		
			endTime.HomeTime();
			INFO_PRINTF1(_L("***** PLAYING COMPLETE EOF *****"));
			
			//check file played for the expected time - allow margin of error +/- 0.55S
			TTimeIntervalMicroSeconds duration = endTime.MicroSecondsFrom(startTime);
#ifdef __WINS__
			if ( (duration.Int64() < (expectedDuration.Int64()-850000)) ||
			   (duration.Int64() > (expectedDuration.Int64()+550000)) )
#else		
			if ( (duration.Int64() < (expectedDuration.Int64()-550000)) ||
			   (duration.Int64() > (expectedDuration.Int64()+550000)) )
#endif // __WINS__		   
				{
				INFO_PRINTF1(_L("file didn't play for the expected duration"));
				INFO_PRINTF2(_L("duration observed : %ld"), duration.Int64());
				INFO_PRINTF2(_L("duration expected : %ld"), expectedDuration.Int64());
				verdict = EFail;
				}
			error = iHwDeviceObserver->ErrorStatus();
			if ((error != KErrNone)&&(error != KErrUnderflow))
				{
				INFO_PRINTF2(_L("error during audio playback %d" ), error );
				verdict = EFail;
				}
			if (iHwDeviceObserver->NumberOfBuffersObserved() < expectedNumberOfBuffers)
				{
				INFO_PRINTF1(_L("incorrect number of buffers"));
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
		INFO_PRINTF2(_L("problem opening source file error %d" ), error );
		verdict = EInconclusive;
		}

	return verdict ;
	} 


CTestStep_MMF_SWCDWRAP_U_0016::CTestStep_MMF_SWCDWRAP_U_0016()
/** Constructor
 */
	{
	iTestStepName = _L("MM-MMF-SWCDWRAP-U-0016-HP");
	}
 

TVerdict CTestStep_MMF_SWCDWRAP_U_0016::DoTestStepL( void )
/**
 * Use case: N/A
 * Use Start() to play from a file, play for 4 seconds
 * and then call Stop()
 * @test Req. under test REQ1931.2
 */
	{
	TVerdict verdict = EPass;

	TFileName filename = _L("\\mm\\mmf\\testfiles\\swcdwrap\\TSU_MMF_SWCDWRAPpcm16_8m.raw");

	TInt error = iHwDeviceObserver->ReadBufferDirectFromFileAsync(filename,0xFFFF); //EOF
 
	if (error == KErrNone)
		{
		error = iHwDevice->Start(EDevDecode, EDevOutFlow);
		INFO_PRINTF1(_L("***** PLAYING *****"));
		if (error == KErrNone)
			{
			CMMF_TSU_SWCDWRAP_MakeAsyncHwDeviceCall* asyncTimer = NULL;
			TRAP(error,asyncTimer = CMMF_TSU_SWCDWRAP_MakeAsyncHwDeviceCall::NewL();)
			if (!error)
				{
				asyncTimer->CallStopAfter(*iHwDevice, TTimeIntervalMicroSeconds32(4000000));//4S
				iActiveScheduler->Start();
				INFO_PRINTF1(_L("***** STOPPED *****"));
				}
			else
				{
				return EInconclusive;
				}
			error = iHwDeviceObserver->ErrorStatus();
			if ((error != KErrNone)&&(error != KErrUnderflow))
				{
				INFO_PRINTF2(_L("error during audio playback %d" ), error );
				verdict = EFail;
				}
			error = asyncTimer->StopError();
			if (error != KErrNone)
				{
				INFO_PRINTF2(_L("error during on Stop %d" ), error );
				verdict = EFail;
				}
			//check device drivers are not already opened
			RMdaDevSound device;
			error = device.Open();
			device.Close();
			if (error != KErrNone)
				{
				INFO_PRINTF2(_L("sound driver was not closed %d" ), error );
				verdict = EFail;
				}

			//(4seconds * sampleRate * pcm16_2BytesperSample)
			TUint samplesPlayed = 4*8000*2;
			TUint expectedNumberOfCompleteBuffersPlayed = samplesPlayed/KTestBufferSize;
			if (iHwDeviceObserver->NumberOfBuffersObserved() <= expectedNumberOfCompleteBuffersPlayed)
				{
				INFO_PRINTF1(_L("insufficient number of buffers"));
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
		INFO_PRINTF2(_L("problem opening source file error %d" ), error );
		verdict = EInconclusive;
		}

	return verdict ;
	} 


CTestStep_MMF_SWCDWRAP_U_0017::CTestStep_MMF_SWCDWRAP_U_0017()
/** Constructor
 */
	{
	iTestStepName = _L("MM-MMF-SWCDWRAP-U-0017-HP");
	}
 

TVerdict CTestStep_MMF_SWCDWRAP_U_0017::DoTestStepL( void )
/**
 * Use case: N/A
 * Use Start() to play from a file and then play for 4 seconds
 * and then call Pause() - tests basic pause functionality
 * after Pause() resume playing again
 * @test Req. under test REQ1931.2
 */
	{
	TVerdict verdict = EPass;

	TFileName filename = _L("\\mm\\mmf\\testfiles\\swcdwrap\\TSU_MMF_SWCDWRAPpcm16_8m.raw");

	TInt error = iHwDeviceObserver->ReadBufferDirectFromFileAsync(filename,0xFFFF); //EOF
 
	if (error == KErrNone)
		{
		error = iHwDevice->Start(EDevDecode, EDevOutFlow);
		INFO_PRINTF1(_L("***** PLAYING *****"));
		if (error == KErrNone)
			{
			CMMF_TSU_SWCDWRAP_MakeAsyncHwDeviceCall* asyncTimer = NULL;
			TRAP(error,asyncTimer = CMMF_TSU_SWCDWRAP_MakeAsyncHwDeviceCall::NewL());
			if (!error)
				{
				asyncTimer->CallPauseAfter(*iHwDevice, TTimeIntervalMicroSeconds32(4000000));
				iActiveScheduler->Start();
				INFO_PRINTF1(_L("***** PAUSED *****"));
				}
			else
				{
				return EInconclusive;
				}
			error = iHwDeviceObserver->ErrorStatus();
			if ((error != KErrNone)&&(error != KErrUnderflow))
				{
				INFO_PRINTF2(_L("error during audio playback %d" ), error );
				verdict = EFail;
				}
			error = asyncTimer->PauseError();
			if (error != KErrNone)
				{
				INFO_PRINTF2(_L("error on Pause %d" ), error );
				verdict = EFail;
				}
			//(4seconds * sampleRate * pcm16_2BytesperSample)
			TUint samplesPlayed = 4*8000*2;
			TUint expectedNumberOfCompleteBuffersPlayed = samplesPlayed/KTestBufferSize;
			if (iHwDeviceObserver->NumberOfBuffersObserved() <= expectedNumberOfCompleteBuffersPlayed)
				{//not sure how many buffers played to EOF but should be at least 5
				INFO_PRINTF1(_L("insufficient number of buffers"));
				verdict = EFail;
				}
			//check the number of bytes played by the device corresponds
			//to the time that the pause was at ie 4Seconds
			//The pause will have occurred on buffer number expectedNumberOfCompleteBuffersPlayed+2;
			//allow an extra margin of +2 buffers.
			TUint minAllowedBytesPlayed = KTestBufferSize*expectedNumberOfCompleteBuffersPlayed;
			TUint maxAllowedBytesPlayed = KTestBufferSize*(expectedNumberOfCompleteBuffersPlayed+2);
			TUint bytesPlayed = iPlayCustomInterface->BytesPlayed();
			if ((bytesPlayed < minAllowedBytesPlayed)||(bytesPlayed > maxAllowedBytesPlayed))
				{//then the number of bytes played was outside the expected value
				INFO_PRINTF2(_L("Num of bytes played not as expected at least %d" ), minAllowedBytesPlayed);
				INFO_PRINTF2(_L("actual number of bytes played %d"),bytesPlayed);
				verdict = EFail;
				//note even if this test fails we shall carry on as this
				//failure may not be fatal
				}
			User::After(TTimeIntervalMicroSeconds32(1000000));//1S pause
			INFO_PRINTF1(_L("***** RESUME PLAYING *****"));
			error = iHwDevice->Start(EDevDecode, EDevOutFlow);//resume play
			if (error == KErrNone)
				{
				iActiveScheduler->Start();
				INFO_PRINTF1(_L("***** PLAYING COMPLETE EOF *****"));
				error = iHwDeviceObserver->ErrorStatus();
				if ((error != KErrNone)&&(error != KErrUnderflow))
					{
					INFO_PRINTF2(_L("error during audio playback resumed from pause%d" ), error );
					verdict = EFail;
					}
				if (iHwDeviceObserver->NumberOfBuffersObserved() <= expectedNumberOfCompleteBuffersPlayed+1)
					{//not sure how many buffers played to EOF but should be more than at pause
					INFO_PRINTF1(_L("insufficient number of buffers following resumed playing"));
					verdict = EFail;
					}
				}
			else
				{
				INFO_PRINTF2(_L("problem resuming playback after pause error %d" ), error );
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
		INFO_PRINTF2(_L("problem opening source file error %d" ), error );
		verdict = EInconclusive;
		}

	return verdict ;
	} 


CTestStep_MMF_SWCDWRAP_U_0018::CTestStep_MMF_SWCDWRAP_U_0018()
/** Constructor
 */
	{
	iTestStepName = _L("MM-MMF-SWCDWRAP-U-0018-HP");
	}
 

TVerdict CTestStep_MMF_SWCDWRAP_U_0018::DoTestStepL( void )
/**
 * Use case: N/A
 * Use Start() to play from a file and
 * and then call Pause() repeating every 1S till EOF
 * @test Req. under test REQ1931.2
 */
	{
	TVerdict verdict = EPass;

	TFileName filename = _L("\\mm\\mmf\\testfiles\\swcdwrap\\TSU_MMF_SWCDWRAPpcm16_8m.raw");

	TInt error = iHwDeviceObserver->ReadBufferDirectFromFileAsync(filename,0xFFFF); //EOF

	//get the size of the source file in bytes
	TInt sourceFileSize = 0;
	iHwDeviceObserver->ReadFile().Size(sourceFileSize);	
 
	if (error == KErrNone)
		{
		TUint calculatedBytesPlayed = 0;
		TUint expectedNumberOfCompleteBuffersPlayed = 0;
		TUint minAllowedBytesPlayed = 0;
		TUint maxAllowedBytesPlayed = 0;
		TUint bytesPlayed = 0;
		CMMF_TSU_SWCDWRAP_MakeAsyncHwDeviceCall* asyncTimer = NULL;
		TRAP(error,asyncTimer = CMMF_TSU_SWCDWRAP_MakeAsyncHwDeviceCall::NewL();)
		//only have timer stopping active scheduler prevents confusion
		iHwDeviceObserver->StopActiveSchedulerOnStop(EFalse);
		//Keep Pausing every second and resume playing till hw device observer
		//has been informed the CMMFHwDevice plugin has stopped or until
		//an error occurs
		while ( !(error||iHwDeviceObserver->IsStopped()) )
			{
			error = iHwDevice->Start(EDevDecode, EDevOutFlow);
			INFO_PRINTF1(_L("***** PLAYING *****"));
			if (error == KErrNone)
				{
				asyncTimer->CallPauseAfter(*iHwDevice, TTimeIntervalMicroSeconds32(1000000));
				TTime startTime(0);
				TTime endTime(0);
				startTime.HomeTime();//need to time the 1S as it might not be exactly 1S!
				iActiveScheduler->Start();
				bytesPlayed = iPlayCustomInterface->BytesPlayed();
				endTime.HomeTime();		
				INFO_PRINTF1(_L("***** PAUSED *****"));
				error = iHwDeviceObserver->ErrorStatus();
				if ((error != KErrNone)&&(error != KErrUnderflow))
					{
					INFO_PRINTF2(_L("error during audio playback %d" ), error );
					verdict = EFail;
					}
				error = asyncTimer->PauseError();
				if (error != KErrNone)
					{
					INFO_PRINTF2(_L("error on Pause %d" ), error );
					verdict = EFail;
					}
				
				//1S might not be exactly 1S due to active object
				TTimeIntervalMicroSeconds duration = endTime.MicroSecondsFrom(startTime);
				TInt64 durationInt = duration.Int64();
				//(sampleRate * pcm16_2BytesperSample)
				TInt64 calculatedBytesPlayedSincePause = 8000*2;
				//adjust samples played for error in 1S timer
				calculatedBytesPlayedSincePause *= durationInt;
				calculatedBytesPlayedSincePause /= 1000000;
				calculatedBytesPlayed += I64LOW(calculatedBytesPlayedSincePause);

				if (calculatedBytesPlayed > (bytesPlayed*2))
					{// provides a sanity check if the calculated
					//number of bytes is more than double the file size
					//then something has gone wrong ie EOF has not been 
					//successfully detected
					error = KErrAbort;
					INFO_PRINTF1(_L("Aborting should have EOFed by now"));
					}

				expectedNumberOfCompleteBuffersPlayed = calculatedBytesPlayed/KTestBufferSize;
				if (iHwDeviceObserver->NumberOfBuffersObserved() < expectedNumberOfCompleteBuffersPlayed)
					{
					INFO_PRINTF1(_L("insufficient number of buffers"));
					
					//Return EFail when it is target test
					//Return EKnownFailure when it is emulator test
#if defined(__WINS__) || defined(__WINSCW__)  

					//CHANGE VERDICT TO EKnownFailure!
					//A known failure due to paused the bytes played does not 
					//reflect the actual number of bytes played.
					//MMF code doesn't use pause on the hwDevice to pause. 
					//These tests are windows sound driver and hardware dependent as well 
					INFO_PRINTF1(_L("This is a known failure due to a sound driver problem"));
					verdict = EKnownFailure; //return EKnownFailure when it is emulator test
#else
					verdict = EFail; //return EFail when it is target test
#endif
					}

				//check the number of bytes played by the device corresponds
				//to the time that the pause was at
				//allow a margin of error of 4% +/- 1 buffer
				//not the actual margin depends on the sound driver
				//windows NT will fail this badly.
				//other drivers seem to fail too cause is that the BytesPlayed
				//doesn't reflect the actual bytes palyed on pause as the driver
				//adds then entire buffer to bytes played including the bytes it
				//hasn't played !
				minAllowedBytesPlayed = calculatedBytesPlayed-KTestBufferSize;
				minAllowedBytesPlayed *= 96;
				minAllowedBytesPlayed /= 100;
				maxAllowedBytesPlayed = calculatedBytesPlayed+KTestBufferSize;
				maxAllowedBytesPlayed *= 104;
				maxAllowedBytesPlayed /= 100;
				
				INFO_PRINTF2(_L("sounddriver says %d" ), bytesPlayed);
				INFO_PRINTF2(_L("calculated bytes played %d" ), calculatedBytesPlayed);
			
				if ((bytesPlayed < minAllowedBytesPlayed)||(bytesPlayed > maxAllowedBytesPlayed))
					{//then the number of bytes played was outside the expected value
					INFO_PRINTF2(_L("Num of bytes played not as expected at least %d" ), minAllowedBytesPlayed);
					INFO_PRINTF2(_L("actual number of bytes played %d"),bytesPlayed);
					
					//Return EKnownFailure when it is target test
					//Return EFail when it is emulator test
#if defined(__WINS__) || defined(__WINSCW__)  
					verdict = EFail; //return EFail when it is emulator test
					//note even if this test fails we shall carry on as this
					//failure may not be fatal

#else
					//CHANGE VERDICT TO EKnownFailure!
					//A known failure due to paused the bytes played does not 
					//reflect the actual number of bytes played.
					//MMF code doesn't use pause on the hwDevice to pause. 
					//These tests are windows sound driver and hardware dependent as well 
					INFO_PRINTF1(_L("This is a known failure due to a sound driver problem"));
					verdict = EKnownFailure; //return EKnownFailure when it is target test
#endif
					}
				}
			else
				{
				INFO_PRINTF2(_L("error on hw device start %d" ), error );
				verdict = EFail;
				}
			}//while (!error) && (!iHwDeviceObserver->IsStopped())
		if (error)
			{
			INFO_PRINTF2(_L("error occured %d" ), error );
			verdict = EFail;
			}
		delete asyncTimer;
		}
	else
		{
		INFO_PRINTF2(_L("problem opening source file error %d" ), error );
		verdict = EInconclusive;
		}

	return verdict ;
	} 


CTestStep_MMF_SWCDWRAP_U_0019::CTestStep_MMF_SWCDWRAP_U_0019()
/** Constructor
 */
	{
	iTestStepName = _L("MM-MMF-SWCDWRAP-U-0019-HP");
	}
 

TVerdict CTestStep_MMF_SWCDWRAP_U_0019::DoTestStepL( void )
/**
 * Use case: N/A
 * Use Start() to play from a file and then play for 4 seconds
 * and then call Stop() and then call Start() again.
 * @test Req. under test REQ1931.2
 */
	{
	TVerdict verdict = EPass;

	TFileName filename = _L("\\mm\\mmf\\testfiles\\swcdwrap\\TSU_MMF_SWCDWRAPpcm16_8m.raw");

	TInt error = iHwDeviceObserver->ReadBufferDirectFromFileAsync(filename,0xFFFF); //EOF
 
	if (error == KErrNone)
		{
		error = iHwDevice->Start(EDevDecode, EDevOutFlow);
		INFO_PRINTF1(_L("***** PLAYING *****"));
		if (error == KErrNone)
			{
			CMMF_TSU_SWCDWRAP_MakeAsyncHwDeviceCall* asyncTimer = NULL;
			TRAP(error, asyncTimer = CMMF_TSU_SWCDWRAP_MakeAsyncHwDeviceCall::NewL();)
			if (error)
				{
				return EInconclusive;
				}
			asyncTimer->CallStopAfter(*iHwDevice, TTimeIntervalMicroSeconds32(4000000));
			iActiveScheduler->Start();
			INFO_PRINTF1(_L("***** STOPPED *****"));
			error = iHwDeviceObserver->ErrorStatus();
			if ((error != KErrNone)&&(error != KErrUnderflow))
				{
				INFO_PRINTF2(_L("error during audio playback %d" ), error );
				verdict = EFail;
				}
			error = asyncTimer->StopError();
			if (error != KErrNone)
				{
				INFO_PRINTF2(_L("error during on Stop %d" ), error );
				verdict = EFail;
				}
			//check device drivers are not already opened
			RMdaDevSound device;
			error = device.Open();
			device.Close();
			if (error != KErrNone)
				{
				INFO_PRINTF2(_L("sound driver was not closed %d" ), error );
				verdict = EFail;
				}
		
			//(4seconds * sampleRate * pcm16_2BytesperSample)
			TUint samplesPlayed = 4*8000*2;
			TUint expectedNumberOfCompleteBuffersPlayed = samplesPlayed/KTestBufferSize;
			if (iHwDeviceObserver->NumberOfBuffersObserved() <= expectedNumberOfCompleteBuffersPlayed)
				{//not sure how many buffers played to EOF but should be at least 5
				INFO_PRINTF1(_L("insufficient number of buffers"));
				verdict = EFail;
				}
			User::After(TTimeIntervalMicroSeconds32(100000));//.1S pause
			INFO_PRINTF1(_L("***** PLAYING RESTARTED *****"));
			error = iHwDevice->Start(EDevDecode, EDevOutFlow);
			if (error == KErrNone)
				{
				iActiveScheduler->Start();
				INFO_PRINTF1(_L("***** PLAYING COMPLETE EOF *****"));
				error = iHwDeviceObserver->ErrorStatus();
				if ((error != KErrNone)&&(error != KErrUnderflow))
					{
					INFO_PRINTF2(_L("error during audio playback resumed from pause%d" ), error );
					verdict = EFail;
					}
				if (iHwDeviceObserver->NumberOfBuffersObserved() <= expectedNumberOfCompleteBuffersPlayed+1)
					{//not sure how many buffers played to EOF but should be more than at pause
					INFO_PRINTF1(_L("insufficient number of buffers following resumed playing"));
					verdict = EFail;
					}
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
		INFO_PRINTF2(_L("problem opening source file error %d" ), error );
		verdict = EInconclusive;
		}

	return verdict ;
	} 


CTestStep_MMF_SWCDWRAP_U_0020::CTestStep_MMF_SWCDWRAP_U_0020()
/** Constructor
 */
	{
	iTestStepName = _L("MM-MMF-SWCDWRAP-U-0020-HP");
	}
 

TVerdict CTestStep_MMF_SWCDWRAP_U_0020::DoTestStepL( void )
/**
 * Use case: N/A
 * Use Start() to play a file to completion
 * Using custom play interface to set to stereo and different sample rate
 * @test Req. under test REQ1931.2
 */
	{
	TVerdict verdict = EPass;

	iPlayCustomInterface->SetVolume(20000);
	
	TTaskConfig taskConfig;
	taskConfig.iUid = KUidRefDevSoundTaskConfig;
	taskConfig.iRate = 16000;
	taskConfig.iStereoMode = ETaskInterleaved;
	
	if (iHwDevice->SetConfig(taskConfig) != KErrNone)
		{
		return EFail;
		}

	TFileName filename = _L("\\mm\\mmf\\testfiles\\swcdwrap\\TSU_MMF_SWCDWRAPpcm16_16s.raw");

	TInt error = iHwDeviceObserver->ReadBufferDirectFromFileAsync(filename,0xFFFF); //EOF

	//calculate the expected duration of the file
	//for some reason the calculation needs to broken up into these steps
	//otherwise the compiler gets it wrong !!
	TInt sourceFileSize = 0;
	iHwDeviceObserver->ReadFile().Size(sourceFileSize);	
	TInt64 expectedDurationInt64 = sourceFileSize;
	expectedDurationInt64 *= 1000000; //uS
	expectedDurationInt64 /= (16000*2*2);//(samplerate*2BytesPerSample*stereo)
	TTimeIntervalMicroSeconds expectedDuration(expectedDurationInt64);
 
	if (error == KErrNone)
		{
		error = iHwDevice->Start(EDevDecode, EDevOutFlow);
		INFO_PRINTF1(_L("***** PLAYING *****"));
		if (error == KErrNone)
			{
			TTime startTime(0);
			TTime endTime(0);
			startTime.HomeTime();
			iActiveScheduler->Start();
			endTime.HomeTime();
			INFO_PRINTF1(_L("***** PLAYING COMPLETE EOF *****"));
			//check file played for the expected time - allow margin of error +/- 0.5S
			TTimeIntervalMicroSeconds duration = endTime.MicroSecondsFrom(startTime);
			if ( (duration.Int64() < (expectedDuration.Int64()-500000)) ||
			   (duration.Int64() > (expectedDuration.Int64()+500000)) )
				{
				INFO_PRINTF1(_L("file didn't play for the expected duration"));
				verdict = EFail;
				}
			error = iHwDeviceObserver->ErrorStatus();
			if ((error != KErrNone)&&(error != KErrUnderflow))
				{
				INFO_PRINTF2(_L("error during audio playback %d" ), error );
				verdict = EFail;
				}
			if (iHwDeviceObserver->NumberOfBuffersObserved() <= 5)
				{//not sure how many buffers played to EOF but should be at least 5
				INFO_PRINTF1(_L("incorrect number of buffers"));
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
		INFO_PRINTF2(_L("problem opening source file error %d" ), error );
		verdict = EInconclusive;
		}

	return verdict ;
	} 

CTestStep_MMF_SWCDWRAP_U_0021::CTestStep_MMF_SWCDWRAP_U_0021()
/** Constructor
 */
	{
	iTestStepName = _L("MM-MMF-SWCDWRAP-U-0021-HP");
	}
 

TVerdict CTestStep_MMF_SWCDWRAP_U_0021::DoTestStepL( void )
/**
 * Use case: N/A
 * Call Pause() after sending last buffer
 * @test Req. under test REQ1931.2
 */
	{
	TVerdict verdict = EPass;

	TFileName filename = _L("\\mm\\mmf\\testfiles\\swcdwrap\\TSU_MMF_SWCDWRAPpcm16_8m.raw");

	TInt error = iHwDeviceObserver->ReadBufferDirectFromFileAsync(filename,0xFFFF); //EOF
 
	if (error == KErrNone)
		{
		iHwDeviceObserver->CallPauseAndResumeAfterSendingLastBuffer();
		error = iHwDevice->Start(EDevDecode, EDevOutFlow);
		INFO_PRINTF1(_L("***** PLAYING *****"));
		if (error == KErrNone)
			{
			iActiveScheduler->Start();		
			INFO_PRINTF1(_L("***** PLAYING COMPLETE EOF *****"));
			
			error = iHwDeviceObserver->ErrorStatus();
			if ((error != KErrNone)&&(error != KErrUnderflow))
				{
				INFO_PRINTF2(_L("error during audio playback %d" ), error );
				verdict = EFail;
				}
			if (iHwDeviceObserver->NumberOfBuffersObserved() <= 5)
				{//not sure how many buffers played to EOF but should be at least 5
				INFO_PRINTF1(_L("incorrect number of buffers"));
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
		INFO_PRINTF2(_L("problem opening source file error %d" ), error );
		verdict = EInconclusive;
		}

	return verdict ;
	} 


CTestStep_MMF_SWCDWRAP_U_0040::CTestStep_MMF_SWCDWRAP_U_0040()
/** Constructor
 */
	{
	iTestStepName = _L("MM-MMF-SWCDWRAP-U-0040-HP");
	}
 

TVerdict CTestStep_MMF_SWCDWRAP_U_0040::DoTestStepL( void )
/**
 * Use case: N/A
 * Use Start() to play one not completed buffer synchronously using 2:1 codec
 * buffer size < codec buffer size
 * @test Req. under test REQ1931.2
 */
	{
	 
	TVerdict verdict = EPass;

	TFileName filename = _L("\\mm\\mmf\\testfiles\\swcdwrap\\TSU_MMF_SWCDWRAPpcm16_8m.raw");
	TInt error = iHwDeviceObserver->FillBufferFromFileSync(filename, KTest2To1SourceBufferSize/2);
 
	if (error == KErrNone)
		{
		TInt error = iHwDevice->Start(EDevDecode, EDevOutFlow);
		if (error == KErrNone)
			{
			iActiveScheduler->Start();
			error = iHwDeviceObserver->ErrorStatus();
			if ((error != KErrNone)&&(error != KErrUnderflow))
				{
				INFO_PRINTF2(_L("error during audio playback %d" ), error );
				verdict = EFail;
				}
			if (iHwDeviceObserver->NumberOfBuffersObserved() != 1)
				{
				INFO_PRINTF1(_L("incorrect number of buffers"));
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
		INFO_PRINTF2(_L("problem creating source buffer from file error %d" ), error );
		verdict = EInconclusive;
		}
	
	return verdict ;
	} 


CTestStep_MMF_SWCDWRAP_U_0041::CTestStep_MMF_SWCDWRAP_U_0041()
/** Constructor
 */
	{
	iTestStepName = _L("MM-MMF-SWCDWRAP-U-0041-HP");
	}
 

TVerdict CTestStep_MMF_SWCDWRAP_U_0041::DoTestStepL( void )
/**
 * Use case: N/A
 * Use Start() to play exactly one complete buffer synchronously using 2:1 codec
 * buffer size < codec buffer size
 * @test Req. under test REQ1931.2
 */
	{
	 
	TVerdict verdict = EPass;

	TFileName filename = _L("\\mm\\mmf\\testfiles\\swcdwrap\\TSU_MMF_SWCDWRAPpcm16_8m.raw");
	TInt error = iHwDeviceObserver->FillBufferFromFileSync(filename, KTest2To1SourceBufferSize);//exactly 1 buffer
 
	if (error == KErrNone)
		{
		TInt error = iHwDevice->Start(EDevDecode, EDevOutFlow);
		if (error == KErrNone)
			{
			iActiveScheduler->Start();
			error = iHwDeviceObserver->ErrorStatus();
			if ((error != KErrNone)&&(error != KErrUnderflow))
				{
				INFO_PRINTF2(_L("error during audio playback %d" ), error );
				verdict = EFail;
				}
			if (iHwDeviceObserver->NumberOfBuffersObserved() != 1)
				{
				INFO_PRINTF1(_L("incorrect number of buffers"));
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
		INFO_PRINTF2(_L("problem creating source buffer from file error %d" ), error );
		verdict = EInconclusive;
		}
	
	return verdict ;
	} 


CTestStep_MMF_SWCDWRAP_U_0042::CTestStep_MMF_SWCDWRAP_U_0042()
/** Constructor
 */
	{
	iTestStepName = _L("MM-MMF-SWCDWRAP-U-0042-HP");
	}
 

TVerdict CTestStep_MMF_SWCDWRAP_U_0042::DoTestStepL( void )
/**
 * Use case: N/A
 * Use Start() to play five complete and 1 not completed buffers synchronously using 2:1 codec
 * buffer size < codec buffer size
 * @test Req. under test REQ1931.2
 */
	{
	 
	TVerdict verdict = EPass;

	TFileName filename = _L("\\mm\\mmf\\testfiles\\swcdwrap\\TSU_MMF_SWCDWRAPpcm16_8m.raw");
	TInt error = iHwDeviceObserver->FillBufferFromFileSync(filename, 5*KTest2To1SourceBufferSize + KTest2To1SourceBufferSize/2);
 
	if (error == KErrNone)
		{
		TInt error = iHwDevice->Start(EDevDecode, EDevOutFlow);
		if (error == KErrNone)
			{
			iActiveScheduler->Start();
			error = iHwDeviceObserver->ErrorStatus();
			if ((error != KErrNone)&&(error != KErrUnderflow))
				{
				INFO_PRINTF2(_L("error during audio playback %d" ), error );
				verdict = EFail;
				}
			if (iHwDeviceObserver->NumberOfBuffersObserved() != 6)
				{//expected 6 buffers 5 complete + 1 not completed buffer
				INFO_PRINTF1(_L("incorrect number of buffers"));
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
		INFO_PRINTF2(_L("problem creating source buffer from file error %d" ), error );
		verdict = EInconclusive;
		}
	
	return verdict ;
	} 


CTestStep_MMF_SWCDWRAP_U_0043::CTestStep_MMF_SWCDWRAP_U_0043()
/** Constructor
 */
	{
	iTestStepName = _L("MM-MMF-SWCDWRAP-U-0043-HP");
	}
 

TVerdict CTestStep_MMF_SWCDWRAP_U_0043::DoTestStepL( void )
/**
 * Use case: N/A
 * Use Start() to play 1 buffer from a file using async read operation using 2:1 codec
 * @test Req. under test REQ1931.2
 */
	{
	TVerdict verdict = EPass;

	TFileName filename = _L("\\mm\\mmf\\testfiles\\swcdwrap\\TSU_MMF_SWCDWRAPpcm16_8m.raw");

	TInt error = iHwDeviceObserver->ReadBufferDirectFromFileAsync(filename,1); //1 buffer
 
	if (error == KErrNone)
		{
		error = iHwDevice->Start(EDevDecode, EDevOutFlow);
		if (error == KErrNone)
			{
			iActiveScheduler->Start();
			error = iHwDeviceObserver->ErrorStatus();
			if ((error != KErrNone)&&(error != KErrUnderflow))
				{
				INFO_PRINTF2(_L("error during audio playback %d" ), error );
				verdict = EFail;
				}
			if (iHwDeviceObserver->NumberOfBuffersObserved() != 1)
				{
				INFO_PRINTF1(_L("incorrect number of buffers"));
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
		INFO_PRINTF2(_L("problem opening source file error %d" ), error );
		verdict = EInconclusive;
		}

	return verdict ;
	} 


CTestStep_MMF_SWCDWRAP_U_0044::CTestStep_MMF_SWCDWRAP_U_0044()
/** Constructor
 */
	{
	iTestStepName = _L("MM-MMF-SWCDWRAP-U-0044-HP");
	}
 

TVerdict CTestStep_MMF_SWCDWRAP_U_0044::DoTestStepL( void )
/**
 * Use case: N/A
 * Use Start() to play 5 buffers from a file using async read operation using 2:1 codec
 * @test Req. under test REQ1931.2
 */
	{
	TVerdict verdict = EPass;

	TFileName filename = _L("\\mm\\mmf\\testfiles\\swcdwrap\\TSU_MMF_SWCDWRAPpcm16_8m.raw");

	TInt error = iHwDeviceObserver->ReadBufferDirectFromFileAsync(filename,5); //5 buffers
 
	if (error == KErrNone)
		{
		error = iHwDevice->Start(EDevDecode, EDevOutFlow);
		if (error == KErrNone)
			{
			iActiveScheduler->Start();
			error = iHwDeviceObserver->ErrorStatus();
			if ((error != KErrNone)&&(error != KErrUnderflow))
				{
				INFO_PRINTF2(_L("error during audio playback %d" ), error );
				verdict = EFail;
				}
			if (iHwDeviceObserver->NumberOfBuffersObserved() != 5)
				{
				INFO_PRINTF1(_L("incorrect number of buffers"));
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
		INFO_PRINTF2(_L("problem opening source file error %d" ), error );
		verdict = EInconclusive;
		}

	return verdict ;
	} 


CTestStep_MMF_SWCDWRAP_U_0045::CTestStep_MMF_SWCDWRAP_U_0045()
/** Constructor
 */
	{
	iTestStepName = _L("MM-MMF-SWCDWRAP-U-0045-HP");
	}
 

TVerdict CTestStep_MMF_SWCDWRAP_U_0045::DoTestStepL( void )
/**
 * Use case: N/A
 * Use Start() to play a file to completion using 2:1 codec
 * @test Req. under test REQ1931.2
 */
	{
	TVerdict verdict = EPass;

	TFileName filename = _L("\\mm\\mmf\\testfiles\\swcdwrap\\TSU_MMF_SWCDWRAPpcm16_8m.raw");

	TInt error = iHwDeviceObserver->ReadBufferDirectFromFileAsync(filename,0xFFFF); //EOF

	//calculate the expected duration of the file
	//for some reason the calculation needs to broken up into these steps
	//otherwise the compiler gets it wrong !!
	TInt sourceFileSize = 0;
	iHwDeviceObserver->ReadFile().Size(sourceFileSize);	
	TInt64 expectedDurationInt64 = sourceFileSize;
	expectedDurationInt64 *= 1000000; //uS
	expectedDurationInt64 /= (8000*2*1*2);//(samplerate*2BytesPerSample*mono*2:1 codec)
	TTimeIntervalMicroSeconds expectedDuration(expectedDurationInt64);
 
	if (error == KErrNone)
		{
		error = iHwDevice->Start(EDevDecode, EDevOutFlow);
		INFO_PRINTF1(_L("***** PLAYING SPEED 2:1 *****"));
		if (error == KErrNone)
			{
			TTime startTime(0);
			TTime endTime(0);
			startTime.HomeTime();
			iActiveScheduler->Start();
			endTime.HomeTime();
			INFO_PRINTF1(_L("***** PLAYING COMPLETE EOF *****"));
			//check file played for the expected time - allow margin of error +/- 0.5S
			TTimeIntervalMicroSeconds duration = endTime.MicroSecondsFrom(startTime);
			if ( (duration.Int64() < (expectedDuration.Int64()-500000)) ||
			   (duration.Int64() > (expectedDuration.Int64()+500000)) )
				{
				INFO_PRINTF1(_L("file didn't play for the expected duration"));
				verdict = EFail;
				}
			error = iHwDeviceObserver->ErrorStatus();
			if ((error != KErrNone)&&(error != KErrUnderflow))
				{
				INFO_PRINTF2(_L("error during audio playback %d" ), error );
				verdict = EFail;
				}
			if (iHwDeviceObserver->NumberOfBuffersObserved() <= 5)
				{//not sure how many buffers played to EOF but should be at least 5
				INFO_PRINTF1(_L("incorrect number of buffers"));
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
		INFO_PRINTF2(_L("problem opening source file error %d" ), error );
		verdict = EInconclusive;
		}

	return verdict ;
	} 

CTestStep_MMF_SWCDWRAP_U_0046::CTestStep_MMF_SWCDWRAP_U_0046()
/** Constructor
 */
	{
	iTestStepName = _L("MM-MMF-SWCDWRAP-U-0046-HP");
	}
 

TVerdict CTestStep_MMF_SWCDWRAP_U_0046::DoTestStepL( void )
/**
 * Use case: N/A
 * Use Start() to play from a file, play for 2 seconds using 2:1 codec
 * and then call Stop()
 * @test Req. under test REQ1931.2
 */
	{
	TVerdict verdict = EPass;

	TFileName filename = _L("\\mm\\mmf\\testfiles\\swcdwrap\\TSU_MMF_SWCDWRAPpcm16_8m.raw");

	TInt error = iHwDeviceObserver->ReadBufferDirectFromFileAsync(filename,0xFFFF); //EOF
 
	if (error == KErrNone)
		{
		error = iHwDevice->Start(EDevDecode, EDevOutFlow);
		INFO_PRINTF1(_L("***** PLAYING *****"));
		if (error == KErrNone)
			{
			CMMF_TSU_SWCDWRAP_MakeAsyncHwDeviceCall* asyncTimer = NULL;
			TRAP(error, asyncTimer = CMMF_TSU_SWCDWRAP_MakeAsyncHwDeviceCall::NewL();)
			if (error)
				{
				return EInconclusive;
				}
			asyncTimer->CallStopAfter(*iHwDevice, TTimeIntervalMicroSeconds32(4000000));
			iActiveScheduler->Start();
			INFO_PRINTF1(_L("***** STOPPED *****"));
			error = iHwDeviceObserver->ErrorStatus();
			if ((error != KErrNone)&&(error != KErrUnderflow))
				{
				INFO_PRINTF2(_L("error during audio playback %d" ), error );
				verdict = EFail;
				}
			error = asyncTimer->StopError();
			if (error != KErrNone)
				{
				INFO_PRINTF2(_L("error during on Stop %d" ), error );
				verdict = EFail;
				}
			//check device drivers are not already opened
			RMdaDevSound device;
			error = device.Open();
			device.Close();
			
			//(4seconds * sampleRate * pcm16_2BytesperSample)
			TUint samplesPlayed = 4*8000*2;
			TUint expectedNumberOfCompleteBuffersPlayed = samplesPlayed/KTest2To1SinkBufferSize;
			if (iHwDeviceObserver->NumberOfBuffersObserved() <= expectedNumberOfCompleteBuffersPlayed)
				{//not sure how many buffers played to EOF but should be at least 5
				INFO_PRINTF1(_L("insufficient number of buffers"));
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
		INFO_PRINTF2(_L("problem opening source file error %d" ), error );
		verdict = EInconclusive;
		}

	return verdict ;
	} 


CTestStep_MMF_SWCDWRAP_U_0047::CTestStep_MMF_SWCDWRAP_U_0047()
/** Constructor
 */
	{
	iTestStepName = _L("MM-MMF-SWCDWRAP-U-0047-HP");
	}
 

TVerdict CTestStep_MMF_SWCDWRAP_U_0047::DoTestStepL( void )
/**
 * Use case: N/A
 * Use Start() to play from a file, play for 2 seconds using 2:1 codec
 * and then call Pause() - tests basic pause functionality
 * after Pause() resume playing again
 * @test Req. under test REQ1931.2
 */
	{
	TVerdict verdict = EPass;

	TFileName filename = _L("\\mm\\mmf\\testfiles\\swcdwrap\\TSU_MMF_SWCDWRAPpcm16_8m.raw");

	TInt error = iHwDeviceObserver->ReadBufferDirectFromFileAsync(filename,0xFFFF); //EOF
 
	if (error == KErrNone)
		{
		error = iHwDevice->Start(EDevDecode, EDevOutFlow);
		INFO_PRINTF1(_L("***** PLAYING *****"));
		if (error == KErrNone)
			{
			CMMF_TSU_SWCDWRAP_MakeAsyncHwDeviceCall* asyncTimer = NULL;
			TRAP(error, asyncTimer = CMMF_TSU_SWCDWRAP_MakeAsyncHwDeviceCall::NewL();)
			if (error)
				{
				return EInconclusive;
				}
			asyncTimer->CallPauseAfter(*iHwDevice, TTimeIntervalMicroSeconds32(4000000));
			iActiveScheduler->Start();
			INFO_PRINTF1(_L("***** PAUSED *****"));
			error = iHwDeviceObserver->ErrorStatus();
			if ((error != KErrNone)&&(error != KErrUnderflow))
				{
				INFO_PRINTF2(_L("error during audio playback %d" ), error );
				verdict = EFail;
				}
			error = asyncTimer->PauseError();
			if (error != KErrNone)
				{
				INFO_PRINTF2(_L("error on Pause %d" ), error );
				verdict = EFail;
				}
		
			//(4seconds * sampleRate * pcm16_2BytesperSample)
			TUint samplesPlayed = 4*8000*2;
			TUint expectedNumberOfCompleteBuffersPlayed = samplesPlayed/KTest2To1SinkBufferSize;
			if (iHwDeviceObserver->NumberOfBuffersObserved() <= expectedNumberOfCompleteBuffersPlayed)
				{//not sure how many buffers played to EOF but should be at least 5
				INFO_PRINTF1(_L("insufficient number of buffers"));
				verdict = EFail;
				}
			//check the number of bytes played by the device corresponds
			//to the time that the pause was at ie 4Seconds
			//The pause will have occurred on buffer number expectedNumberOfCompleteBuffersPlayed+2;
			TUint minAllowedBytesPlayed = KTest2To1SinkBufferSize*expectedNumberOfCompleteBuffersPlayed;
			TUint maxAllowedBytesPlayed = KTest2To1SinkBufferSize*(expectedNumberOfCompleteBuffersPlayed+2);
			TUint bytesPlayed = iPlayCustomInterface->BytesPlayed();
			if ((bytesPlayed < minAllowedBytesPlayed)||(bytesPlayed > maxAllowedBytesPlayed))
				{//then the number of bytes played was outside the expected value
				INFO_PRINTF2(_L("Num of bytes played not as expected at least %d" ), minAllowedBytesPlayed);
				INFO_PRINTF2(_L("actual number of bytes played %d"),bytesPlayed);
				verdict = EFail;
				//note even if this test fails we shall carry on as this
				//failure may not be fatal
				}
			User::After(TTimeIntervalMicroSeconds32(1000000));//1S pause
			INFO_PRINTF1(_L("***** RESUME PLAYING *****"));
			error = iHwDevice->Start(EDevDecode, EDevOutFlow);//resume play
			if (error == KErrNone)
				{
				iActiveScheduler->Start();
				INFO_PRINTF1(_L("***** PLAYING COMPLETE EOF *****"));
				error = iHwDeviceObserver->ErrorStatus();
				if ((error != KErrNone)&&(error != KErrUnderflow))
					{
					INFO_PRINTF2(_L("error during audio playback resumed from pause%d" ), error );
					verdict = EFail;
					}
				if (iHwDeviceObserver->NumberOfBuffersObserved() <= expectedNumberOfCompleteBuffersPlayed+1)
					{//not sure how many buffers played to EOF but should be more than at pause
					INFO_PRINTF1(_L("insufficient number of buffers following resumed playing"));
					verdict = EFail;
					}
				}
			else
				{
				INFO_PRINTF2(_L("problem resuming playback after pause error %d" ), error );
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
		INFO_PRINTF2(_L("problem opening source file error %d" ), error );
		verdict = EInconclusive;
		}

	return verdict ;
	} 



CTestStep_MMF_SWCDWRAP_U_0048::CTestStep_MMF_SWCDWRAP_U_0048()
/** Constructor
 */
	{
	iTestStepName = _L("MM-MMF-SWCDWRAP-U-0048-HP");
	}
 

TVerdict CTestStep_MMF_SWCDWRAP_U_0048::DoTestStepL( void )
/**
 * Use case: N/A
 * Use Start() to play from a file using 2:1 codec
 * and then call Pause() repeatidly every second till EOF
 * @test Req. under test REQ1931.2
 */
	{
//this test has been seen to cause the EKA2 lubbock sounddriver
//to occasionaly die, however putting debug info in to get the
//call sequence to the sound driver results in the test running to completion
	TVerdict verdict = EPass;
#ifndef __WINS__
	verdict = EKnownFailure;
#else

	TFileName filename = _L("\\mm\\mmf\\testfiles\\swcdwrap\\TSU_MMF_SWCDWRAPpcm16_8m.raw");

	TInt error = iHwDeviceObserver->ReadBufferDirectFromFileAsync(filename,0xFFFF); //EOF

	//get the size of the source file in bytes
	TInt sourceFileSize = 0;
	iHwDeviceObserver->ReadFile().Size(sourceFileSize);	
 
	if (error == KErrNone)
		{
		TUint calculatedBytesPlayed = 0;
		TUint expectedNumberOfCompleteBuffersPlayed = 0;
		TUint minAllowedBytesPlayed = 0;
		TUint maxAllowedBytesPlayed = 0;
		TUint bytesPlayed = 0;
		CMMF_TSU_SWCDWRAP_MakeAsyncHwDeviceCall* asyncTimer = NULL;
		TRAP(error,asyncTimer = CMMF_TSU_SWCDWRAP_MakeAsyncHwDeviceCall::NewL();)
		//only have timer stopping active scheduler prevents confusion
		iHwDeviceObserver->StopActiveSchedulerOnStop(EFalse);
		//Keep Pausing every second and resume playing till hw device observer
		//has been informed the CMMFHwDevice plugin has stopped or until
		//an error occurs
		while ( !(error||iHwDeviceObserver->IsStopped()) )
			{
			error = iHwDevice->Start(EDevDecode, EDevOutFlow);
			INFO_PRINTF1(_L("***** PLAYING *****"));
			if (error == KErrNone)
				{
				asyncTimer->CallPauseAfter(*iHwDevice, TTimeIntervalMicroSeconds32(1000000));
				TTime startTime(0);
				TTime endTime(0);
				startTime.HomeTime();
				iActiveScheduler->Start();
				bytesPlayed = iPlayCustomInterface->BytesPlayed();
				endTime.HomeTime();	
				INFO_PRINTF1(_L("***** PAUSED *****"));
				error = iHwDeviceObserver->ErrorStatus();
				if ((error != KErrNone)&&(error != KErrUnderflow))
					{
					INFO_PRINTF2(_L("error during audio playback %d" ), error );
					verdict = EFail;
					}
				error = asyncTimer->PauseError();
				if (error != KErrNone)
					{
					INFO_PRINTF2(_L("error on Pause %d" ), error );
					verdict = EFail;
					}
			
				//(sampleRate * pcm16_2BytesperSample)
				TTimeIntervalMicroSeconds duration = endTime.MicroSecondsFrom(startTime);
				TInt64 durationInt = duration.Int64();
				TInt64 calculatedBytesPlayedSincePause = 8000*2;
				calculatedBytesPlayedSincePause *= durationInt;
				calculatedBytesPlayedSincePause /= 1000000;
				calculatedBytesPlayed += I64LOW(calculatedBytesPlayedSincePause);

				if (calculatedBytesPlayed > (bytesPlayed*2))
					{// provides a sanity check if the calculated
					//number of bytes is more than double the expected value
					//(would expect at EOF calcultedBytesPlayed to be half sourceFileSize
					//with 2:1 codec
					//then something has gone wrong ie EOF has not been 
					//successfully detected
					error = KErrAbort;
					INFO_PRINTF1(_L("Aborting should have EOFed by now"));
					}

				expectedNumberOfCompleteBuffersPlayed = calculatedBytesPlayed/KTest2To1SinkBufferSize;
				if (iHwDeviceObserver->NumberOfBuffersObserved() < expectedNumberOfCompleteBuffersPlayed)
					{
					INFO_PRINTF1(_L("insufficient number of buffers"));

					//Return EFail when it is target test
					//Return EKnownFailure when it is emulator test
#if defined(__WINS__) || defined(__WINSCW__)  

					//CHANGE VERDICT TO EKnownFailure!
					//A known failure due to paused the bytes played does not 
					//reflect the actual number of bytes played.
					//MMF code doesn't use pause on the hwDevice to pause. 
					//These tests are windows sound driver and hardware dependent as well 
					INFO_PRINTF1(_L("This is a known failure due to a sound driver problem"));
					verdict = EKnownFailure; //return EKnownFailure when it is emulator test
#else
					verdict = EFail; //return EFail when it is target test
#endif
					}

				//check the number of bytes played by the device corresponds
				//to the time that the pause was at
				//allow a margin of error of 4% +/- 1 buffer
				//not the actual margin depends on the sound driver
				//windows NT will fail this.
				//also timer RunL is AO and may be blocked ie timing may not be exact.
				minAllowedBytesPlayed = calculatedBytesPlayed-KTestBufferSize;
				minAllowedBytesPlayed *= 96;
				minAllowedBytesPlayed /= 100;
				maxAllowedBytesPlayed = calculatedBytesPlayed+KTestBufferSize;
				maxAllowedBytesPlayed *= 104;
				maxAllowedBytesPlayed /= 100;
				
				INFO_PRINTF2(_L("sounddriver says %d" ), bytesPlayed);
				INFO_PRINTF2(_L("calculated bytes played %d" ), calculatedBytesPlayed);
			
				if ((bytesPlayed < minAllowedBytesPlayed)||(bytesPlayed > maxAllowedBytesPlayed))
					{//then the number of bytes played was outside the expected value
					INFO_PRINTF2(_L("Num of bytes played not as expected at least %d" ), minAllowedBytesPlayed);
					INFO_PRINTF2(_L("actual number of bytes played %d"),bytesPlayed);

					//Return EKnownFailure when it is target test
					//Return EFail when it is emulator test
#if defined(__WINS__) || defined(__WINSCW__)  
					verdict = EFail; //return EFail when it is emulator test
					//note even if this test fails we shall carry on as this
					//failure may not be fatal

#else
					//CHANGE VERDICT TO EKnownFailure!
					//A known failure due to paused the bytes played does not 
					//reflect the actual number of bytes played.
					//MMF code doesn't use pause on the hwDevice to pause. 
					//These tests are windows sound driver and hardware dependent as well 
					INFO_PRINTF1(_L("This is a known failure due to a sound driver problem"));
					verdict = EKnownFailure; //return EKnownFailure when it is target test
#endif
					}
				}
			else
				{
				INFO_PRINTF2(_L("error on hw device start %d" ), error );
				verdict = EFail;
				}
			}//while (!error) && (!iHwDeviceObserver->IsStopped())
		if (error)
			{
			INFO_PRINTF2(_L("error occured %d" ), error );
			verdict = EFail;
			}
		delete asyncTimer;
		}
	else
		{
		INFO_PRINTF2(_L("problem opening source file error %d" ), error );
		verdict = EInconclusive;
		}
#endif //__WINS__
	return verdict ;

	} 


CTestStep_MMF_SWCDWRAP_U_0049::CTestStep_MMF_SWCDWRAP_U_0049()
/** Constructor
 */
	{
	iTestStepName = _L("MM-MMF-SWCDWRAP-U-0049-HP");
	}
 

TVerdict CTestStep_MMF_SWCDWRAP_U_0049::DoTestStepL( void )
/**
 * Use case: N/A
 * Use Start() to play from a file, play for 2 seconds using 2:1 codec
 * and then call Stop() and then call Start() again.
 * @test Req. under test REQ1931.2
 */
	{
	TVerdict verdict = EPass;

	TFileName filename = _L("\\mm\\mmf\\testfiles\\swcdwrap\\TSU_MMF_SWCDWRAPpcm16_8m.raw");

	TInt error = iHwDeviceObserver->ReadBufferDirectFromFileAsync(filename,0xFFFF); //EOF
 
	if (error == KErrNone)
		{
		error = iHwDevice->Start(EDevDecode, EDevOutFlow);
		INFO_PRINTF1(_L("***** PLAYING *****"));
		if (error == KErrNone)
			{
			CMMF_TSU_SWCDWRAP_MakeAsyncHwDeviceCall* asyncTimer = NULL;
			TRAP(error, asyncTimer = CMMF_TSU_SWCDWRAP_MakeAsyncHwDeviceCall::NewL();)
			if (error)
				{
				return EInconclusive;
				}
			asyncTimer->CallStopAfter(*iHwDevice, TTimeIntervalMicroSeconds32(4000000));
			iActiveScheduler->Start();
			INFO_PRINTF1(_L("***** STOPPED *****"));
			error = iHwDeviceObserver->ErrorStatus();
			if ((error != KErrNone)&&(error != KErrUnderflow))
				{
				INFO_PRINTF2(_L("error during audio playback %d" ), error );
				verdict = EFail;
				}
			error = asyncTimer->StopError();
			if (error != KErrNone)
				{
				INFO_PRINTF2(_L("error during on Stop %d" ), error );
				verdict = EFail;
				}
			//check device drivers are not already opened
			RMdaDevSound device;
			error = device.Open();
			device.Close();
			if (error != KErrNone)
				{
				INFO_PRINTF2(_L("sound driver was not closed %d" ), error );
				verdict = EFail;
				}
			
			//(4seconds * sampleRate * pcm16_2BytesperSample)
			TUint samplesPlayed = 4*8000*2;
			TUint expectedNumberOfCompleteBuffersPlayed = samplesPlayed/KTest2To1SinkBufferSize;
			if (iHwDeviceObserver->NumberOfBuffersObserved() <= expectedNumberOfCompleteBuffersPlayed)
				{//not sure how many buffers played to EOF but should be at least 5
				INFO_PRINTF1(_L("insufficient number of buffers"));
				verdict = EFail;
				}
			User::After(TTimeIntervalMicroSeconds32(100000));//.1S pause
			INFO_PRINTF1(_L("***** PLAYING RESTARTED *****"));
			error = iHwDevice->Start(EDevDecode, EDevOutFlow);
			if (error == KErrNone)
				{
				iActiveScheduler->Start();
				INFO_PRINTF1(_L("***** PLAYING COMPLETE EOF *****"));
				error = iHwDeviceObserver->ErrorStatus();
				if ((error != KErrNone)&&(error != KErrUnderflow))
					{
					INFO_PRINTF2(_L("error during audio playback resumed from pause%d" ), error );
					verdict = EFail;
					}
				if (iHwDeviceObserver->NumberOfBuffersObserved() <= expectedNumberOfCompleteBuffersPlayed+1)
					{//not sure how many buffers played to EOF but should be more than at pause
					INFO_PRINTF1(_L("insufficient number of buffers following resumed playing"));
					verdict = EFail;
					}
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
		INFO_PRINTF2(_L("problem opening source file error %d" ), error );
		verdict = EInconclusive;
		}

	return verdict ;
	} 


CTestStep_MMF_SWCDWRAP_U_0050::CTestStep_MMF_SWCDWRAP_U_0050()
/** Constructor
 */
	{
	iTestStepName = _L("MM-MMF-SWCDWRAP-U-0050-LP");
	}
 

TVerdict CTestStep_MMF_SWCDWRAP_U_0050::DoTestStepL( void )
/**
 * Use case: N/A
 * Use Start() to play from a file, play for 2 seconds using 2:1 codec
 * and then call StopAndDeleteCodec() and then call Start() again.
 */
	{
	TVerdict verdict = EPass;

	TFileName filename = _L("\\mm\\mmf\\testfiles\\swcdwrap\\TSU_MMF_SWCDWRAPpcm16_8m.raw");

	TInt error = iHwDeviceObserver->ReadBufferDirectFromFileAsync(filename,0xFFFF); //EOF
 
	if (error == KErrNone)
		{
		error = iHwDevice->Start(EDevDecode, EDevOutFlow);
		INFO_PRINTF1(_L("***** PLAYING *****"));
		if (error == KErrNone)
			{
			CMMF_TSU_SWCDWRAP_MakeAsyncHwDeviceCall* asyncTimer = NULL;
			TRAP(error, asyncTimer = CMMF_TSU_SWCDWRAP_MakeAsyncHwDeviceCall::NewL();)
			if (error)
				{
				return EInconclusive;
				}
			asyncTimer->CallStopAndDeleteCodecAfter(*iHwDevice, TTimeIntervalMicroSeconds32(4000000));
			iActiveScheduler->Start();
			INFO_PRINTF1(_L("***** STOPPED *****"));
			error = iHwDeviceObserver->ErrorStatus();
			if ((error != KErrNone)&&(error != KErrUnderflow))
				{
				INFO_PRINTF2(_L("error during audio playback %d" ), error );
				verdict = EFail;
				}
			error = asyncTimer->StopAndDeleteCodecError();
			if (error != KErrNone)
				{
				INFO_PRINTF2(_L("error during on StopAndDeleteCodec %d" ), error );
				verdict = EFail;
				}
			//check device drivers are not already opened
			RMdaDevSound device;
        #ifdef SYMBIAN_MDF_SHAREDCHUNK_SOUNDDRIVER
			//Try querying version before opening the device. Added to improve the code coverage
			TVersion version = device.VersionRequired();
			
			error = device.Open();
			if (error != KErrNone)
				{
				INFO_PRINTF2(_L("sound driver was not opened %d" ), error );
				verdict = EFail;
				}
			version = device.VersionRequired();
			
			TBool isMdaDevSound = device.IsMdaSound();
			if(!isMdaDevSound)
			    {
			    INFO_PRINTF1(_L("IsMdaDevSound() returned EFalse: Unexpected"));
				verdict = EFail;
			    }
			//Try queryinig play volume after opening the device
			TInt volume = device.PlayVolume();
			//Try querying gain
			volume = device.RecordLevel();
			device.Close();
	    #else		
			error = device.Open();
			device.Close();
		#endif //SYMBIAN_MDF_SHAREDCHUNK_SOUNDDRIVER	
			if (error != KErrNone)
				{
				INFO_PRINTF2(_L("sound driver was not closed %d" ), error );
				verdict = EFail;
				}

			//(4seconds * sampleRate * pcm16_2BytesperSample)
			TUint samplesPlayed = 4*8000*2;
			TUint expectedNumberOfCompleteBuffersPlayed = samplesPlayed/KTest2To1SinkBufferSize;
			if (iHwDeviceObserver->NumberOfBuffersObserved() <= expectedNumberOfCompleteBuffersPlayed)
				{//not sure how many buffers played to EOF but should be at least 5
				INFO_PRINTF1(_L("insufficient number of buffers"));
				verdict = EFail;
				}
			User::After(TTimeIntervalMicroSeconds32(100000));//.1S pause
			INFO_PRINTF1(_L("***** PLAYING RESTARTED *****"));
			error = iHwDevice->Start(EDevDecode, EDevOutFlow);
			if (error == KErrNone)
				{
				iActiveScheduler->Start();
				INFO_PRINTF1(_L("***** PLAYING COMPLETE EOF *****"));
				error = iHwDeviceObserver->ErrorStatus();
				if ((error != KErrNone)&&(error != KErrUnderflow))
					{
					INFO_PRINTF2(_L("error during audio playback resumed from pause%d" ), error );
					verdict = EFail;
					}
				if (iHwDeviceObserver->NumberOfBuffersObserved() <= expectedNumberOfCompleteBuffersPlayed+1)
					{//not sure how many buffers played to EOF but should be more than at pause
					INFO_PRINTF1(_L("insufficient number of buffers following resumed playing"));
					verdict = EFail;
					}
				// now delete the codec
				INFO_PRINTF1(_L("***** DELETING CODEC *****" ));
				error = iHwDevice->DeleteCodec();
				if (error != KErrNone)
					{
					INFO_PRINTF2(_L("error deleting codec, error = %d" ), error );
					verdict = EFail;
					}
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
		INFO_PRINTF2(_L("problem opening source file error %d" ), error );
		verdict = EInconclusive;
		}

	return verdict ;
	} 


